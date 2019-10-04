/*
 * Sux: Succinct data structures
 *
 * Copyright (C) 2007-2013 Sebastiano Vigna
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

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include "jacobson.hpp"
#include "../common.hpp"

namespace sux {

jacobson::jacobson() {}

jacobson::jacobson(const uint64_t *const bits, const uint64_t num_bits) {
  this->bits = bits;
  num_words = (num_bits + 63) / 64;
  num_counts = ((num_bits + 64 * 8 - 1) / (64 * 8)) * 2;
  block_size = (uint64_t)floor(log(num_bits) / (2 * log(2)));
  superblock_size = lambda_safe(num_bits) * block_size;

  counter_bits_per_superblock = ceil_log2(num_bits);
  num_bits_for_superblocks =
      ((num_bits + superblock_size - 1) / superblock_size) * counter_bits_per_superblock;
  counter_bits_per_block = ceil_log2(superblock_size);
  num_bits_for_blocks = ((num_bits + block_size - 1) / block_size) * counter_bits_per_block;
  counter_bits_per_precomp = ceil_log2(block_size);
  num_bits_for_precomp = (1ULL << block_size) * block_size * counter_bits_per_precomp;

  printf("Block size: %lld Superblock size: %lld Bits per block counter: %lld Bits per superblock "
         "counter: %lld Bits per table entry: %lld\n",
         block_size, superblock_size, counter_bits_per_block, counter_bits_per_superblock,
         counter_bits_per_precomp);

  // Init rank structure
  counts = new uint64_t[(num_bits_for_blocks + 63) / 64];
  supercounts = new uint64_t[(num_bits_for_superblocks + 63) / 64];
  precomp = new uint64_t[(num_bits_for_precomp + 63) / 64];

  uint64_t c = 0;
  uint64_t pos = 0, superpos = 0, start;
  for (uint64_t i = 0; i < num_words; i++) {
    for (int j = 0; j < 64; j++) {
      if (i * 64 + j >= num_bits)
        break;
      if ((i * 64 + j) % superblock_size == 0) {
        assert(c < (1ULL << counter_bits_per_superblock));
        set_bits(supercounts, ((i * 64 + j) / superblock_size) * counter_bits_per_superblock,
                 counter_bits_per_superblock, c);
        start = c;
      }
      if ((i * 64 + j) % block_size == 0) {
        assert(c - start < (1ULL << counter_bits_per_block));
        set_bits(counts, ((i * 64 + j) / block_size) * counter_bits_per_block,
                 counter_bits_per_block, c - start);
      }
      if (bits[i] & 1ULL << j)
        c++;
    }
  }

  num_patterns = 1ULL << block_size;
  for (uint64_t i = 0; i < num_patterns; i++)
    for (int j = 0; j < block_size; j++)
      set_bits(precomp, (i * block_size + j) * counter_bits_per_precomp, counter_bits_per_precomp,
               __builtin_popcountll(i & (1ULL << j) - 1));

  assert(c <= num_bits);

#ifndef NDEBUG
  c = 0;
  for (uint64_t i = 0; i < num_words; i++)
    for (int j = 0; j < 64; j++) {
      if (i * 64 + j >= num_bits)
        break;
      if (bits[i] & 1ULL << j) {
        // printf( "pos: %lld rank: %lld result: %lld\n", i * 64 + j, c, rank( i * 64 + j ) );
        assert(rank(i * 64 + j) == c);
        c++;
      }
    }
#endif
}

jacobson::~jacobson() {
  delete[] counts;
  delete[] supercounts;
  delete[] precomp;
}

uint64_t jacobson::rank(const uint64_t k) {
  const uint64_t superblock = k / superblock_size;
  const uint64_t block = k / block_size;
  const uint64_t residual = k % block_size;
  return get_bits(supercounts, superblock * counter_bits_per_superblock,
                  counter_bits_per_superblock) +
         get_bits(counts, block * counter_bits_per_block, counter_bits_per_block) +
         +get_bits(precomp,
                   (get_bits(bits, k - residual, block_size) * block_size + residual) *
                       counter_bits_per_precomp,
                   counter_bits_per_precomp);
}

uint64_t jacobson::bit_count() {
  return num_bits_for_blocks + num_bits_for_superblocks + num_bits_for_precomp;
}

void jacobson::print_counts() {}

}
