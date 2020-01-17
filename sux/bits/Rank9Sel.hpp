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
#include "Rank9.hpp"
#include "Select.hpp"
#include <cstdint>

namespace sux::bits {

/** A class implementing a combination of Rank9 for ranking and Select9 for selection.
 * Select9 uses 25%-37.5% additional space (beside the 25% due to Rank9), depending on density,
 * and provides constant-time selection.
 *
 * The constructors of this class only store a reference
 * to a provided bit vector. Should the content of the
 * bit vector change, the results will be unpredictable.
 *
 * **Warning**: if you plan an calling rank(size_t) with
 * argument size(), you must have at least one additional
 * free bit at the end of the provided bit vector.
 */
template <util::AllocType AT = util::AllocType::MALLOC> class Rank9Sel : public Rank9<AT>, public Select {
  private:
	static const int log2_ones_per_inventory = 9;
	static const int ones_per_inventory = 1 << log2_ones_per_inventory;
	static const uint64_t inventory_mask = ones_per_inventory - 1;

	util::Vector<uint64_t, AT> inventory, subinventory;
	uint64_t inventory_size;

  public:
	/** Creates a new instance using a given bit vector.
	 *
	 * Note that this constructor only stores a reference
	 * to the provided bit vector. Should the content of the
	 * bit vector change, the results will be unpredictable.
	 *
	 * **Warning**: if you plan an calling rank(size_t) with
	 * argument size(), you must have at least one additional
	 * free bit at the end of the provided bit vector.
	 *
	 * @param bits a bit vector of 64-bit words.
	 * @param num_bits the length (in bits) of the bit vector.
	 */

	Rank9Sel(const uint64_t *const bits, const uint64_t num_bits) : Rank9<AT>(bits, num_bits) {
		const uint64_t num_words = (num_bits + 63) / 64;
		inventory_size = (this->num_ones + ones_per_inventory - 1) / ones_per_inventory;

#ifdef DEBUG
		printf("Number of ones per inventory item: %d\n", ones_per_inventory);
#endif
		assert(ones_per_inventory <= 8 * 64);

		inventory.size(inventory_size + 1);
		subinventory.size((num_words + 3) / 4);

		uint64_t d = 0;
		for (uint64_t i = 0; i < num_words; i++)
			for (int j = 0; j < 64; j++)
				if (bits[i] & 1ULL << j) {
					if ((d & inventory_mask) == 0) {
						inventory[d >> log2_ones_per_inventory] = i * 64 + j;
						assert(this->counts[(i / 8) * 2] <= d);
						assert(this->counts[(i / 8) * 2 + 2] > d);
					}

					d++;
				}

		assert(this->num_ones == d);
		inventory[inventory_size] = ((num_words + 3) & ~3ULL) * 64;

#ifdef DEBUG
		printf("Inventory size: %" PRId64 "\n", inventory_size);
		printf("Inventory entries filled: %" PRId64 "\n", d / ones_per_inventory + 1);
		// printf("First inventories: %" PRId64 " %" PRId64 " %" PRId64 " %" PRId64 "\n", inventory[0],
		// inventory[1], inventory[2],
		//       inventory[3]);
#endif

		d = 0;
		int state;
		uint64_t *s, first_bit, index, span, block_span, block_left, counts_at_start;

		for (uint64_t i = 0; i < num_words; i++)
			for (int j = 0; j < 64; j++)
				if (bits[i] & 1ULL << j) {
					if ((d & inventory_mask) == 0) {
						first_bit = i * 64 + j;
						index = d >> log2_ones_per_inventory;
						assert(inventory[index] == first_bit);
						s = &subinventory[(inventory[index] / 64) / 4];
						span = (inventory[index + 1] / 64) / 4 - (inventory[index] / 64) / 4;
						state = -1;
						counts_at_start = this->counts[((inventory[index] / 64) / 8) * 2];
						block_span = (inventory[index + 1] / 64) / 8 - (inventory[index] / 64) / 8;
						block_left = (inventory[index] / 64) / 8;

						if (span >= 512)
							state = 0;
						else if (span >= 256)
							state = 1;
						else if (span >= 128)
							state = 2;
						else if (span >= 16) {
							assert(((block_span + 8) & -8LL) + 8 <= span * 4);

							uint64_t k;
							for (k = 0; k < block_span; k++) {
								assert(((uint16_t *)s)[k + 8] == 0);
								((uint16_t *)s)[k + 8] = this->counts[(block_left + k + 1) * 2] - counts_at_start;
							}

							for (; k < ((block_span + 8) & -8LL); k++) {
								assert(((uint16_t *)s)[k + 8] == 0);
								((uint16_t *)s)[k + 8] = 0xFFFFU;
							}

							assert(block_span / 8 <= 8);

							for (k = 0; k < block_span / 8; k++) {
								assert(((uint16_t *)s)[k] == 0);
								((uint16_t *)s)[k] = this->counts[(block_left + (k + 1) * 8) * 2] - counts_at_start;
							}

							for (; k < 8; k++) {
								assert(((uint16_t *)s)[k] == 0);
								((uint16_t *)s)[k] = 0xFFFFU;
							}
						} else if (span >= 2) {
							assert(((block_span + 8) & -8LL) <= span * 4);

							uint64_t k;
							for (k = 0; k < block_span; k++) {
								assert(((uint16_t *)s)[k] == 0);
								((uint16_t *)s)[k] = this->counts[(block_left + k + 1) * 2] - counts_at_start;
							}

							for (; k < ((block_span + 8) & -8LL); k++) {
								assert(((uint16_t *)s)[k] == 0);
								((uint16_t *)s)[k] = 0xFFFFU;
							}
						}
					}

					switch (state) {
					case 0:
						assert(s[d & inventory_mask] == 0);
						s[d & inventory_mask] = i * 64 + j;
						break;
					case 1:
						assert(((uint32_t *)s)[d & inventory_mask] == 0);
						assert(i * 64 + j - first_bit < (1ULL << 32));
						((uint32_t *)s)[d & inventory_mask] = i * 64 + j - first_bit;
						break;
					case 2:
						assert(((uint16_t *)s)[d & inventory_mask] == 0);
						assert(i * 64 + j - first_bit < (1 << 16));
						((uint16_t *)s)[d & inventory_mask] = i * 64 + j - first_bit;
						break;
					}

					d++;
				}
	}

	size_t select(const uint64_t rank) {
		const uint64_t inventory_index_left = rank >> log2_ones_per_inventory;
		assert(inventory_index_left <= inventory_size);

		const uint64_t inventory_left = inventory[inventory_index_left];
		const uint64_t block_right = inventory[inventory_index_left + 1] / 64;
		uint64_t block_left = inventory_left / 64;
		const uint64_t span = block_right / 4 - block_left / 4;
		const uint64_t *const s = &subinventory[block_left / 4];
		uint64_t count_left, rank_in_block;

#ifdef DEBUG
		printf("Initially, rank: %" PRId64 " block_left: %" PRId64 " block_right: %" PRId64 " span: %" PRId64 "\n", rank, block_left, block_right, span);
#endif

		if (span < 2) {
			block_left &= ~7;
			count_left = block_left / 4 & ~1;
			assert(rank < this->counts[count_left + 2]);
			rank_in_block = rank - this->counts[count_left];
#ifdef DEBUG
			printf("Single span; rank_in_block: %" PRId64 " block_left: %" PRId64 "\n", rank_in_block, block_left);
#endif
		} else if (span < 16) {
			block_left &= ~7;
			count_left = block_left / 4 & ~1;
			const uint64_t rank_in_superblock = rank - this->counts[count_left];
			const uint64_t rank_in_superblock_step_16 = rank_in_superblock * ONES_STEP_16;

			const uint64_t first = s[0], second = s[1];
			const int where = (ULEQ_STEP_16(first, rank_in_superblock_step_16) + ULEQ_STEP_16(second, rank_in_superblock_step_16)) * ONES_STEP_16 >> 47;
			assert(where >= 0);
			assert(where <= 16);
#ifdef DEBUG
			printf("rank_in_superblock: %" PRId64 " (%" PRIx64 ") %" PRIx64 " %" PRIx64 "\n", rank_in_superblock, rank_in_superblock, s[0], s[1]);
#endif

			block_left += where * 4;
			count_left += where;
			rank_in_block = rank - this->counts[count_left];
			assert(rank_in_block < 512);
#ifdef DEBUG
			printf("Found where (1): %d rank_in_block: %" PRId64 " block_left: %" PRId64 "\n", where, rank_in_block, block_left);
			printf("superthis->counts: %016" PRIx64 " %016" PRIx64 "\n", s[0], s[1]);
#endif
		} else if (span < 128) {
			block_left &= ~7;
			count_left = block_left / 4 & ~1;
			const uint64_t rank_in_superblock = rank - this->counts[count_left];
			const uint64_t rank_in_superblock_step_16 = rank_in_superblock * ONES_STEP_16;

			const uint64_t first = s[0], second = s[1];
			const int where0 = (ULEQ_STEP_16(first, rank_in_superblock_step_16) + ULEQ_STEP_16(second, rank_in_superblock_step_16)) * ONES_STEP_16 >> 47;
			assert(where0 <= 16);
			const uint64_t first_bis = s[where0 + 2], second_bis = s[where0 + 2 + 1];
			const int where1 = where0 * 8 + ((ULEQ_STEP_16(first_bis, rank_in_superblock_step_16) + ULEQ_STEP_16(second_bis, rank_in_superblock_step_16)) * ONES_STEP_16 >> 47);

			block_left += where1 * 4;
			count_left += where1;
			rank_in_block = rank - this->counts[count_left];
			assert(rank_in_block < 512);

#ifdef DEBUG
			printf("Found where (2): %d rank_in_block: %" PRId64 " block_left: %" PRId64 "\n", where1, rank_in_block, block_left);
#endif
		} else if (span < 256) {
			return ((uint16_t *)s)[rank % ones_per_inventory] + inventory_left;
		} else if (span < 512) {
			return ((uint32_t *)s)[rank % ones_per_inventory] + inventory_left;
		} else {
			return s[rank % ones_per_inventory];
		}

		const uint64_t rank_in_block_step_9 = rank_in_block * ONES_STEP_9;
		const uint64_t subcounts = this->counts[count_left + 1];
		const uint64_t offset_in_block = (ULEQ_STEP_9(subcounts, rank_in_block_step_9) * ONES_STEP_9 >> 54 & 0x7);

		const uint64_t word = block_left + offset_in_block;
		const uint64_t rank_in_word = rank_in_block - (subcounts >> ((offset_in_block - 1) & 7) * 9 & 0x1FF);
		assert(offset_in_block <= 7);

#ifdef DEBUG
		printf("rank_in_block: %" PRId64 " offset_in_block: %" PRId64 " rank_in_word: %" PRId64 "\n", rank_in_block, offset_in_block, rank_in_word);
		printf("subcounts: ");
		for (int i = 0; i < 7; i++) printf("%" PRId64 " ", subcounts >> i * 9 & 0x1FF);
		printf("\n");
		fflush(stdout);
#endif

		assert(rank_in_word < 64);

#ifdef DEBUG
		printf("Returning %" PRId64 "\n", word * UINT64_C(64) + select64(this->bits[word], rank_in_word));
#endif
		return word * UINT64_C(64) + select64(this->bits[word], rank_in_word);
	}

	size_t bitCount() const {
		return this->counts.bitCount() - sizeof(this->counts) * 8 + inventory.bitCount() - sizeof(inventory) * 8 + subinventory.bitCount() - sizeof(subinventory) * 8 + sizeof(*this) * 8;
	}
};

} // namespace sux::bits
