/*
 * Sux: Succinct data structures
 *
 * Copyright (C) 2007-2020 Sebastiano Vigna
 *
 *  This library is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License as published by the Free
 *  Software Foundation; either version 3 of the License, or (at your option)
 *  any later version.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * Under Section 7 of GPL version 3, you are granted additional permissions
 * described in the GCC Runtime Library Exception, version 3.1, as published by
 * the Free Software Foundation.
 *
 * You should have received a copy of the GNU General Public License and a copy of
 * the GCC Runtime Library Exception along with this program; see the files
 * COPYING3 and COPYING.RUNTIME respectively.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "../support/common.hpp"
#include "../util/Vector.hpp"
#include "Select.hpp"
#include <cstdint>

namespace sux::bits {

using namespace std;

/** A simple Select implementation based on a two-level inventory, a spill list and broadword bit search.
 *
 * This implementation uses around 13.75% additional space on evenly distributed bit arrays, and,
 * under the same conditions, provide very fast selects. For very unevenly distributed arrays
 * the space occupancy will grow significantly, and access time might vary wildly.
 *
 * The constructors of this class only store a reference
 * to a provided bit vector. Should the content of the
 * bit vector change, the results will be unpredictable.
 *
 * @tparam AT a type of memory allocation out of sux::util::AllocType.
 */

template <util::AllocType AT = util::AllocType::MALLOC> class SimpleSelect : public Select {
  private:
	static const int max_ones_per_inventory = 8192;

	const uint64_t *bits;
	util::Vector<int64_t, AT> inventory;
	util::Vector<uint64_t, AT> exact_spill;
	int log2_ones_per_inventory, log2_ones_per_sub16, log2_ones_per_sub64, log2_longwords_per_subinventory, ones_per_inventory, ones_per_sub16, ones_per_sub64, longwords_per_subinventory,
		longwords_per_inventory, ones_per_inventory_mask, ones_per_sub16_mask, ones_per_sub64_mask;

	uint64_t num_words, inventory_size, exact_spill_size, num_ones;

  public:
	SimpleSelect() {}

	/** Creates a new instance using a given bit vector.
	 *
	 * @param bits a bit vector of 64-bit words.
	 * @param num_bits the length (in bits) of the bit vector.
	 * @param max_log2_longwords_per_subinventory the number of words per subinventory:
	 * a larger value yields a faster map that uses more space; typical values are between 0 and 3.
	 */
	SimpleSelect(const uint64_t *const bits, const uint64_t num_bits, const int max_log2_longwords_per_subinventory) : bits(bits) {
		num_words = (num_bits + 63) / 64;

		// Init rank/select structure
		uint64_t c = 0;
		for (uint64_t i = 0; i < num_words; i++) c += __builtin_popcountll(bits[i]);
		num_ones = c;

		assert(c <= num_bits);

		ones_per_inventory = num_bits == 0 ? 0 : (c * max_ones_per_inventory + num_bits - 1) / num_bits;
		// Make ones_per_inventory into a power of 2
		log2_ones_per_inventory = max(0, lambda_safe(ones_per_inventory));
		ones_per_inventory = 1ULL << log2_ones_per_inventory;
		ones_per_inventory_mask = ones_per_inventory - 1;
		inventory_size = (c + ones_per_inventory - 1) / ones_per_inventory;

#ifdef DEBUG
		printf("Number of ones: %" PRId64 " Number of ones per inventory item: %d\n", c, ones_per_inventory);
#endif

		log2_longwords_per_subinventory = min(max_log2_longwords_per_subinventory, max(0, log2_ones_per_inventory - 2));
		longwords_per_subinventory = 1 << log2_longwords_per_subinventory;
		longwords_per_inventory = longwords_per_subinventory + 1;
		log2_ones_per_sub64 = max(0, log2_ones_per_inventory - log2_longwords_per_subinventory);
		log2_ones_per_sub16 = max(0, log2_ones_per_sub64 - 2);
		ones_per_sub64 = 1ULL << log2_ones_per_sub64;
		ones_per_sub16 = 1ULL << log2_ones_per_sub16;
		ones_per_sub64_mask = ones_per_sub64 - 1;
		ones_per_sub16_mask = ones_per_sub16 - 1;

#ifdef DEBUG
		printf("Longwords per subinventory: %d Ones per sub 64: %d sub 16: %d\n", longwords_per_subinventory, ones_per_sub64, ones_per_sub16);
#endif

		inventory.size(inventory_size * longwords_per_inventory + 1);
		const int64_t *end_of_inventory = &inventory + inventory_size * longwords_per_inventory + 1;

		uint64_t d = 0;

		// First phase: we build an inventory for each one out of ones_per_inventory.
		for (uint64_t i = 0; i < num_words; i++)
			for (int j = 0; j < 64; j++) {
				if (i * 64 + j >= num_bits) break;
				if (bits[i] & 1ULL << j) {
					if ((d & ones_per_inventory_mask) == 0) inventory[(d >> log2_ones_per_inventory) * longwords_per_inventory] = i * 64 + j;
					d++;
				}
			}

		assert(c == d);
		inventory[inventory_size * longwords_per_inventory] = num_bits;

#ifdef DEBUG
		printf("Inventory entries filled: %" PRId64 "\n", inventory_size + 1);
#endif

		if (ones_per_inventory > 1) {
			d = 0;
			int ones;
			uint64_t spilled = 0, exact = 0, start, span, inventory_index;

			for (uint64_t i = 0; i < num_words; i++)
				// We estimate the subinventory and exact spill size
				for (int j = 0; j < 64; j++) {
					if (i * 64 + j >= num_bits) break;
					if (bits[i] & 1ULL << j) {
						if ((d & ones_per_inventory_mask) == 0) {
							inventory_index = d >> log2_ones_per_inventory;
							start = inventory[inventory_index * longwords_per_inventory];
							span = inventory[(inventory_index + 1) * longwords_per_inventory] - start;
							ones = min(c - d, (uint64_t)ones_per_inventory);

							assert(start + span == num_bits || ones == ones_per_inventory);

							// We accumulate space for exact pointers ONLY if necessary.
							if (span >= (1 << 16)) {
								exact += ones;
								if (ones_per_sub64 > 1) spilled += ones;
							}
						}
						d++;
					}
				}

#ifdef DEBUG
			printf("Spilled entries: %" PRId64 " exact: %" PRId64 "\n", spilled, exact);
#endif

			exact_spill_size = spilled;
			exact_spill.size(exact_spill_size);

			uint16_t *p16;
			int64_t *p64;
			int offset;
			spilled = 0;
			d = 0;

			for (uint64_t i = 0; i < num_words; i++)
				for (int j = 0; j < 64; j++) {
					if (i * 64 + j >= num_bits) break;
					if (bits[i] & 1ULL << j) {
						if ((d & ones_per_inventory_mask) == 0) {
							inventory_index = d >> log2_ones_per_inventory;
							start = inventory[inventory_index * longwords_per_inventory];
							span = inventory[(inventory_index + 1) * longwords_per_inventory] - start;
							p64 = &inventory[inventory_index * longwords_per_inventory + 1];
							p16 = (uint16_t *)p64;
							offset = 0;
						}

						if (span < (1 << 16)) {
							assert(i * 64 + j - start <= (1 << 16));
							if ((d & ones_per_sub16_mask) == 0) {
								assert(offset < longwords_per_subinventory * 4);
								assert(p16 + offset < (uint16_t *)end_of_inventory);
								p16[offset++] = i * 64 + j - start;
							}
						} else {
							if (ones_per_sub64 == 1) {
								assert(p64 + offset < end_of_inventory);
								p64[offset++] = i * 64 + j;
							} else {
								assert(p64 < end_of_inventory);
								if ((d & ones_per_inventory_mask) == 0) {
									inventory[inventory_index * longwords_per_inventory] |= 1ULL << 63;
									p64[0] = spilled;
								}
								assert(spilled < exact_spill_size);
								exact_spill[spilled++] = i * 64 + j;
							}
						}

						d++;
					}
				}
		}
#ifdef DEBUG
		// printf("First inventories: %" PRId64 " %" PRId64 " %" PRId64 " %" PRId64 "\n", inventory[0],
		// inventory[1], inventory[2],
		//       inventory[3]);
		// if ( subinventory_size > 0 ) printf("First subinventories: %016" PRIx64 " %016" PRIx64 " %016"
		// PRIx64 " %016" PRIx64 "\n", subinventory[ 0 ], subinventory[ 1 ], subinventory[ 2 ],
		// subinventory[ 3 ] );
		// if (exact_spill_size > 0)
		//  printf("First spilled entries: %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n",
		//  exact_spill[0],
		//         exact_spill[1], exact_spill[2], exact_spill[3]);
#endif
	}

	size_t select(const uint64_t rank) {
#ifdef DEBUG
		printf("Selecting %" PRId64 "\n...", rank);
#endif

		const uint64_t inventory_index = rank >> log2_ones_per_inventory;
		const int64_t *inventory_start = &inventory + (inventory_index << log2_longwords_per_subinventory) + inventory_index;
		assert(inventory_index <= inventory_size);

		const int64_t inventory_rank = *inventory_start;
		const int subrank = rank & ones_per_inventory_mask;
#ifdef DEBUG
		printf("Rank: %" PRId64 " inventory index: %" PRId64 " inventory rank: %" PRId64 " subrank: %d\n", rank, inventory_index, inventory_rank, subrank);
#endif

#ifdef DEBUG
		if (subrank == 0) puts("Exact hit (no subrank); returning inventory");
#endif
		if (subrank == 0) return inventory_rank & ~(1ULL << 63);

		uint64_t start;
		int residual;

		if (inventory_rank >= 0) {
			start = inventory_rank + ((uint16_t *)(inventory_start + 1))[subrank >> log2_ones_per_sub16];
			residual = subrank & ones_per_sub16_mask;
		} else {
			if (ones_per_sub64 == 1) return *(inventory_start + 1 + subrank);
			assert(*(inventory_start + 1) + subrank < (int64_t)exact_spill_size);
			return exact_spill[*(inventory_start + 1) + subrank];
		}

#ifdef DEBUG
		printf("Differential; start: %" PRId64 " residual: %d\n", start, residual);
		if (residual == 0) puts("No residual; returning start");
#endif

		if (residual == 0) return start;

		uint64_t word_index = start / 64;
		uint64_t word = bits[word_index] & -1ULL << start % 64;

		for (;;) {
			const int bit_count = __builtin_popcountll(word);
			if (residual < bit_count) break;
			word = bits[++word_index];
			residual -= bit_count;
		}

		return word_index * 64 + select64(word, residual);
	}

	/** Returns an estimate of the size (in bits) of this structure. */
	size_t bitCount() const { return inventory.bitCount() - sizeof(inventory) * 8 + exact_spill.bitCount() - sizeof(exact_spill) * 8 + sizeof(*this) * 8; }
};

} // namespace sux::bits
