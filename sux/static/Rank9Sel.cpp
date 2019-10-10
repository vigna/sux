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

#include "Rank9Sel.hpp"
#include <cassert>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/resource.h>
#include <sys/time.h>

#define ONES_PER_INVENTORY (512)
#define LOG2_ONES_PER_INVENTORY (9)
#define INVENTORY_MASK (ONES_PER_INVENTORY - 1)

using namespace sux;

Rank9Sel::Rank9Sel(const uint64_t *const bits, const uint64_t num_bits)
    : num_bits(num_bits), bits(bits) {

  num_words = (num_bits + 63) / 64;
  num_counts = ((num_bits + 64 * 8 - 1) / (64 * 8)) * 2;

  // Init rank/select structure
  counts = new uint64_t[num_counts + 2]();

  uint64_t c = 0;
  uint64_t pos = 0;
  for (uint64_t i = 0; i < num_words; i += 8, pos += 2) {
    counts[pos] = c;
    c += __builtin_popcountll(bits[i]);
    for (int j = 1; j < 8; j++) {
      counts[pos + 1] |= (c - counts[pos]) << 9 * (j - 1);
      if (i + j < num_words)
        c += __builtin_popcountll(bits[i + j]);
    }
  }

  counts[num_counts] = c;
#ifdef DEBUG
  printf("Number of ones: %" PRId64 "\n", c);
#endif

  assert(c <= num_bits);

  inventory_size = (c + ONES_PER_INVENTORY - 1) / ONES_PER_INVENTORY;

#ifdef DEBUG
  printf("Number of ones per inventory item: %d\n", ONES_PER_INVENTORY);
#endif
  assert(ONES_PER_INVENTORY <= 8 * 64);

  inventory = new uint64_t[inventory_size + 1]();
  subinventory = new uint64_t[(num_words + 3) / 4]();

  uint64_t d = 0;
  for (uint64_t i = 0; i < num_words; i++)
    for (int j = 0; j < 64; j++)
      if (bits[i] & 1ULL << j) {
        if ((d & INVENTORY_MASK) == 0) {
          inventory[d >> LOG2_ONES_PER_INVENTORY] = i * 64 + j;
          assert(counts[(i / 8) * 2] <= d);
          assert(counts[(i / 8) * 2 + 2] > d);
        }

        d++;
      }

  assert(c == d);
  inventory[inventory_size] = ((num_words + 3) & ~3ULL) * 64;

#ifdef DEBUG
  printf("Inventory size: %" PRId64 "\n", inventory_size);
  printf("Inventory entries filled: %" PRId64 "\n", d / ONES_PER_INVENTORY + 1);
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
        if ((d & INVENTORY_MASK) == 0) {
          first_bit = i * 64 + j;
          index = d >> LOG2_ONES_PER_INVENTORY;
          assert(inventory[index] == first_bit);
          s = &subinventory[(inventory[index] / 64) / 4];
          span = (inventory[index + 1] / 64) / 4 - (inventory[index] / 64) / 4;
          state = -1;
          counts_at_start = counts[((inventory[index] / 64) / 8) * 2];
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
              ((uint16_t *)s)[k + 8] = counts[(block_left + k + 1) * 2] - counts_at_start;
            }

            for (; k < ((block_span + 8) & -8LL); k++) {
              assert(((uint16_t *)s)[k + 8] == 0);
              ((uint16_t *)s)[k + 8] = 0xFFFFU;
            }

            assert(block_span / 8 <= 8);

            for (k = 0; k < block_span / 8; k++) {
              assert(((uint16_t *)s)[k] == 0);
              ((uint16_t *)s)[k] = counts[(block_left + (k + 1) * 8) * 2] - counts_at_start;
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
              ((uint16_t *)s)[k] = counts[(block_left + k + 1) * 2] - counts_at_start;
            }

            for (; k < ((block_span + 8) & -8LL); k++) {
              assert(((uint16_t *)s)[k] == 0);
              ((uint16_t *)s)[k] = 0xFFFFU;
            }
          }
        }

        switch (state) {
        case 0:
          assert(s[d & INVENTORY_MASK] == 0);
          s[d & INVENTORY_MASK] = i * 64 + j;
          break;
        case 1:
          assert(((uint32_t *)s)[d & INVENTORY_MASK] == 0);
          assert(i * 64 + j - first_bit < (1ULL << 32));
          ((uint32_t *)s)[d & INVENTORY_MASK] = i * 64 + j - first_bit;
          break;
        case 2:
          assert(((uint16_t *)s)[d & INVENTORY_MASK] == 0);
          assert(i * 64 + j - first_bit < (1 << 16));
          ((uint16_t *)s)[d & INVENTORY_MASK] = i * 64 + j - first_bit;
          break;
        }

        d++;
      }
}

Rank9Sel::~Rank9Sel() {
  delete[] counts;
  delete[] inventory;
  delete[] subinventory;
}

uint64_t Rank9Sel::rank(const size_t k) const {
  const uint64_t word = k / 64;
  const uint64_t block = word / 4 & ~1;
  const int offset = word % 8 - 1;
  return counts[block] +
         (counts[block + 1] >> (offset + (offset >> (sizeof offset * 8 - 4) & 0x8)) * 9 & 0x1FF) +
         __builtin_popcountll(bits[word] & ((1ULL << k % 64) - 1));
}

size_t Rank9Sel::select(const uint64_t rank) const {
  const uint64_t inventory_index_left = rank >> LOG2_ONES_PER_INVENTORY;
  assert(inventory_index_left <= inventory_size);

  const uint64_t inventory_left = inventory[inventory_index_left];
  const uint64_t block_right = inventory[inventory_index_left + 1] / 64;
  uint64_t block_left = inventory_left / 64;
  const uint64_t span = block_right / 4 - block_left / 4;
  const uint64_t *const s = &subinventory[block_left / 4];
  uint64_t count_left, rank_in_block;

#ifdef DEBUG
  printf("Initially, rank: %" PRId64 " block_left: %" PRId64 " block_right: %" PRId64
         " span: %" PRId64 "\n",
         rank, block_left, block_right, span);
#endif

  if (span < 2) {
    block_left &= ~7;
    count_left = block_left / 4 & ~1;
    assert(rank < counts[count_left + 2]);
    rank_in_block = rank - counts[count_left];
#ifdef DEBUG
    printf("Single span; rank_in_block: %" PRId64 " block_left: %" PRId64 "\n", rank_in_block,
           block_left);
#endif
  } else if (span < 16) {
    block_left &= ~7;
    count_left = block_left / 4 & ~1;
    const uint64_t rank_in_superblock = rank - counts[count_left];
    const uint64_t rank_in_superblock_step_16 = rank_in_superblock * ONES_STEP_16;

    const uint64_t first = s[0], second = s[1];
    const int where = (ULEQ_STEP_16(first, rank_in_superblock_step_16) +
                       ULEQ_STEP_16(second, rank_in_superblock_step_16)) *
                          ONES_STEP_16 >>
                      47;
    assert(where >= 0);
    assert(where <= 16);
#ifdef DEBUG
    printf("rank_in_superblock: %" PRId64 " (%" PRIx64 ") %" PRIx64 " %" PRIx64 "\n",
           rank_in_superblock, rank_in_superblock, s[0], s[1]);
#endif

    block_left += where * 4;
    count_left += where;
    rank_in_block = rank - counts[count_left];
    assert(rank_in_block < 512);
#ifdef DEBUG
    printf("Found where (1): %d rank_in_block: %" PRId64 " block_left: %" PRId64 "\n", where,
           rank_in_block, block_left);
    printf("supercounts: %016" PRIx64 " %016" PRIx64 "\n", s[0], s[1]);
#endif
  } else if (span < 128) {
    block_left &= ~7;
    count_left = block_left / 4 & ~1;
    const uint64_t rank_in_superblock = rank - counts[count_left];
    const uint64_t rank_in_superblock_step_16 = rank_in_superblock * ONES_STEP_16;

    const uint64_t first = s[0], second = s[1];
    const int where0 = (ULEQ_STEP_16(first, rank_in_superblock_step_16) +
                        ULEQ_STEP_16(second, rank_in_superblock_step_16)) *
                           ONES_STEP_16 >>
                       47;
    assert(where0 <= 16);
    const uint64_t first_bis = s[where0 + 2], second_bis = s[where0 + 2 + 1];
    const int where1 = where0 * 8 + ((ULEQ_STEP_16(first_bis, rank_in_superblock_step_16) +
                                      ULEQ_STEP_16(second_bis, rank_in_superblock_step_16)) *
                                         ONES_STEP_16 >>
                                     47);

    block_left += where1 * 4;
    count_left += where1;
    rank_in_block = rank - counts[count_left];
    assert(rank_in_block < 512);

#ifdef DEBUG
    printf("Found where (2): %d rank_in_block: %" PRId64 " block_left: %" PRId64 "\n", where1,
           rank_in_block, block_left);
#endif
  } else if (span < 256) {
    return ((uint16_t *)s)[rank % ONES_PER_INVENTORY] + inventory_left;
  } else if (span < 512) {
    return ((uint32_t *)s)[rank % ONES_PER_INVENTORY] + inventory_left;
  } else {
    return s[rank % ONES_PER_INVENTORY];
  }

  const uint64_t rank_in_block_step_9 = rank_in_block * ONES_STEP_9;
  const uint64_t subcounts = counts[count_left + 1];
  const uint64_t offset_in_block =
      (ULEQ_STEP_9(subcounts, rank_in_block_step_9) * ONES_STEP_9 >> 54 & 0x7);

  const uint64_t word = block_left + offset_in_block;
  const uint64_t rank_in_word =
      rank_in_block - (subcounts >> ((offset_in_block - 1) & 7) * 9 & 0x1FF);
  assert(offset_in_block <= 7);

#ifdef DEBUG
  printf("rank_in_block: %" PRId64 " offset_in_block: %" PRId64 " rank_in_word: %" PRId64 "\n",
         rank_in_block, offset_in_block, rank_in_word);
  printf("subcounts: ");
  for (int i = 0; i < 7; i++)
    printf("%" PRId64 " ", subcounts >> i * 9 & 0x1FF);
  printf("\n");
  fflush(stdout);
#endif

  assert(rank_in_word < 64);

#ifdef DEBUG
  printf("Returning %" PRId64 "\n", word * UINT64_C(64) + select64(bits[word], rank_in_word));
#endif
  return word * UINT64_C(64) + select64(bits[word], rank_in_word);
}

size_t Rank9Sel::size() const { return num_bits; }

uint64_t Rank9Sel::bitCount() {
  return (num_counts + inventory_size + num_words / 4) * 64 + sizeof(Rank9Sel);
}
