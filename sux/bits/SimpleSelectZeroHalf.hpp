/*
 * Sux: Succinct data structures
 *
 * Copyright (C) 2007-2019 Sebastiano Vigna
 *
 *  This library is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License as published by the Free
 *  Software Foundation; either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  This library is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "../support/common.hpp"
#include "../util/Vector.hpp"
#include "SelectZero.hpp"
#include <cstdint>

#define LOG2_ZEROS_PER_INVENTORY (10)
#define ZEROS_PER_INVENTORY (1 << LOG2_ZEROS_PER_INVENTORY)
#define ZEROS_PER_INVENTORY_MASK (ZEROS_PER_INVENTORY - 1)
#define LOG2_LONGWORDS_PER_SUBINVENTORY (2)
#define LONGWORDS_PER_SUBINVENTORY (1 << LOG2_LONGWORDS_PER_SUBINVENTORY)
#define LOG2_ZEROS_PER_SUB64 (LOG2_ZEROS_PER_INVENTORY - LOG2_LONGWORDS_PER_SUBINVENTORY)
#define ZEROS_PER_SUB64 (1 << LOG2_ZEROS_PER_SUB64)
#define ZEROS_PER_SUB64_MASK (ZEROS_PER_SUB64 - 1)
#define LOG2_ZEROS_PER_SUB16 (LOG2_ZEROS_PER_SUB64 - 2)
#define ZEROS_PER_SUB16 (1 << LOG2_ZEROS_PER_SUB16)
#define ZEROS_PER_SUB16_MASK (ZEROS_PER_SUB16 - 1)

namespace sux::bits {

using namespace std;
using namespace sux;

/** A simple SelectZero implementation based on a two-level inventory,
 *  and wired for approximately the same number of zeros and ones.
 *
 * This implementation has been specifically developed to be used
 * with EliasFano.
 *
 * @tparam AT a type of memory allocation out of sux::util::AllocType.
 */

template <util::AllocType AT = util::AllocType::MALLOC> class SimpleSelectZeroHalf {
  private:
	const uint64_t *bits;
	util::Vector<int64_t, AT> inventory;

	uint64_t num_words, inventory_size, num_zeros;

  public:
	SimpleSelectZeroHalf() {}

	/** Creates a new instance using a given bit vector.
	 *
	 * @param bits a bit vector of 64-bit words.
	 * @param num_bits the length (in bits) of the bit vector.
	 */

	SimpleSelectZeroHalf(const uint64_t *const bits, const uint64_t num_bits) : bits(bits) {
		num_words = (num_bits + 63) / 64;

		// Init rank/select structure
		uint64_t c = 0;
		for (uint64_t i = 0; i < num_words; i++) c += __builtin_popcountll(~bits[i]);
		num_zeros = c;

		if (num_bits % 64 != 0) c -= 64 - num_bits % 64;
		assert(c <= num_bits);

		inventory_size = (c + ZEROS_PER_INVENTORY - 1) / ZEROS_PER_INVENTORY;

#ifdef DEBUG
		printf("Number of bits: %" PRId64 " Number of zeros: %" PRId64 " (%.2f%%)\n", num_bits, c, (c * 100.0) / num_bits);

		printf("Ones per inventory: %d Ones per sub 64: %d sub 16: %d\n", ZEROS_PER_INVENTORY, ZEROS_PER_SUB64, ZEROS_PER_SUB16);
#endif

		inventory.size(inventory_size * (LONGWORDS_PER_SUBINVENTORY + 1) + 1);

		uint64_t d = 0;
		const uint64_t mask = ZEROS_PER_INVENTORY - 1;

		// First phase: we build an inventory for each one out of ZEROS_PER_INVENTORY.
		for (uint64_t i = 0; i < num_words; i++)
			for (int j = 0; j < 64; j++) {
				if (i * 64 + j >= num_bits) break;
				if (~bits[i] & 1ULL << j) {
					if ((d & mask) == 0) inventory[(d >> LOG2_ZEROS_PER_INVENTORY) * (LONGWORDS_PER_SUBINVENTORY + 1)] = i * 64 + j;
					d++;
				}
			}

		assert(c == d);
		inventory[inventory_size * (LONGWORDS_PER_SUBINVENTORY + 1)] = num_bits;

#ifdef DEBUG
		printf("Inventory entries filled: %" PRId64 "\n", inventory_size + 1);
#endif

		uint16_t *p16;
		int64_t *p64;

		d = 0;
		uint64_t exact = 0, start, span, inventory_index;
		int offset;

		for (uint64_t i = 0; i < num_words; i++)
			for (int j = 0; j < 64; j++) {
				if (i * 64 + j >= num_bits) break;
				if (~bits[i] & 1ULL << j) {
					if ((d & mask) == 0) {
						inventory_index = (d >> LOG2_ZEROS_PER_INVENTORY) * (LONGWORDS_PER_SUBINVENTORY + 1);
						start = inventory[inventory_index];
						span = inventory[inventory_index + LONGWORDS_PER_SUBINVENTORY + 1] - start;
						if (span > (1 << 16)) inventory[inventory_index] = -inventory[inventory_index] - 1;
						offset = 0;
						p64 = &inventory[inventory_index + 1];
						p16 = (uint16_t *)p64;
					}

					if (span < (1 << 16)) {
						assert(i * 64 + j - start <= (1 << 16));
						if ((d & ZEROS_PER_SUB16_MASK) == 0) {
							assert(offset < LONGWORDS_PER_SUBINVENTORY * 4);
							p16[offset++] = i * 64 + j - start;
						}
					} else {
						if ((d & ZEROS_PER_SUB64_MASK) == 0) {
							assert(offset < LONGWORDS_PER_SUBINVENTORY);
							p64[offset++] = i * 64 + j - start;
							exact++;
						}
					}

					d++;
				}
			}

#ifdef DEBUG
			// printf("Exact entries: %" PRId64 "\n", exact);
			// printf("First inventories: %" PRId64 " %" PRId64 " %" PRId64 " %" PRId64 "\n", inventory[0], inventory[1], inventory[2],
			//       inventory[3]);
#endif
	}

	uint64_t selectZero(const uint64_t rank) {
#ifdef DEBUG
		printf("Selecting %" PRId64 "\n...", rank);
#endif
		assert(rank < num_zeros);

		const uint64_t inventory_index = rank >> LOG2_ZEROS_PER_INVENTORY;
		assert(inventory_index <= inventory_size);
		const int64_t *inventory_start = inventory.p() + (inventory_index << LOG2_LONGWORDS_PER_SUBINVENTORY) + inventory_index;

		const int64_t inventory_rank = *inventory_start;
		const int subrank = rank & ZEROS_PER_INVENTORY_MASK;
#ifdef DEBUG
		printf("Rank: %" PRId64 " inventory index: %" PRId64 " inventory rank: %" PRId64 " subrank: %d\n", rank, inventory_index, inventory_rank, subrank);
#endif

		uint64_t start;
		int residual;

		if (inventory_rank >= 0) {
			start = inventory_rank + ((uint16_t *)(inventory_start + 1))[subrank >> LOG2_ZEROS_PER_SUB16];
			residual = subrank & ZEROS_PER_SUB16_MASK;
		} else {
			assert((subrank >> LOG2_ZEROS_PER_SUB64) < LONGWORDS_PER_SUBINVENTORY);
			start = -inventory_rank - 1 + *(inventory_start + 1 + (subrank >> LOG2_ZEROS_PER_SUB64));
			residual = subrank & ZEROS_PER_SUB64_MASK;
		}

#ifdef DEBUG
		printf("Differential; start: %" PRId64 " residual: %d\n", start, residual);
		if (residual == 0) puts("No residual; returning start");
#endif

		if (residual == 0) return start;

		uint64_t word_index = start / 64;
		uint64_t word = ~bits[word_index] & -1ULL << start % 64;

		for (;;) {
			const int bit_count = __builtin_popcountll(word);
			if (residual < bit_count) break;
			word = ~bits[++word_index];
			residual -= bit_count;
		}

		return word_index * 64 + select64(word, residual);
	}

	uint64_t selectZero(const uint64_t rank, uint64_t *const next) {
		const uint64_t s = selectZero(rank);
		int curr = s / 64;

		uint64_t window = ~bits[curr] & -1ULL << s;
		window &= window - 1;

		while (window == 0) window = ~bits[++curr];
		*next = curr * 64 + __builtin_ctzll(window);

		return s;
	}

	/** Returns an estimate of the size (in bits) of this structure. */
	uint64_t bitCount() { return inventory.bitCount() + sizeof(SimpleSelectZeroHalf) * 8; };
};

} // namespace sux::bits
