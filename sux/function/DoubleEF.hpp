/*
 * Sux: Succinct data structures
 *
 * Copyright (C) 2019 Emmanuel Esposito and Sebastiano Vigna
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
#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

#ifndef LOG2Q
#define LOG2Q 8
#endif

namespace sux::function {

class DoubleEF {
  private:
	static constexpr uint64_t log2q = LOG2Q;
	static constexpr uint64_t q = 1 << log2q;
	static constexpr uint64_t q_mask = q - 1;
	static constexpr uint64_t super_q = 1 << 14;
	static constexpr uint64_t super_q_mask = super_q - 1;
	static constexpr uint64_t q_per_super_q = super_q / q;
	static constexpr uint64_t super_q_size = 1 + q_per_super_q / 4;
	uint64_t *lower_bits, *upper_bits_position, *upper_bits_cum_keys, *jump;
	uint64_t lower_bits_mask_cum_keys, lower_bits_mask_position;

	uint64_t num_buckets, u_cum_keys, u_position;
	uint64_t l_position, l_cum_keys;
	int64_t cum_keys_min_delta, min_diff;
	uint64_t bits_per_key_fixed_point;

	__inline static void set(uint64_t *const bits, const uint64_t pos) { bits[pos / 64] |= 1ULL << pos % 64; }

	__inline static void set_bits(uint64_t *const bits, const uint64_t start, const int width, const uint64_t value) {
		const uint64_t mask = ((UINT64_C(1) << width) - 1) << start % 8;
		uint64_t t;
		memcpy(&t, (uint8_t *)bits + start / 8, 8);
		t = (t & ~mask) | value << start % 8;
		memcpy((uint8_t *)bits + start / 8, &t, 8);
	}

	__inline size_t lower_bits_size_words() const { return ((num_buckets + 1) * (l_cum_keys + l_position) + 63) / 64 + 1; }

	__inline size_t cum_keys_size_words() const { return (num_buckets + 1 + (u_cum_keys >> l_cum_keys) + 63) / 64; }

	__inline size_t position_size_words() const { return (num_buckets + 1 + (u_position >> l_position) + 63) / 64; }

	__inline size_t jump_size_words() const {
		size_t size = (num_buckets / super_q) * super_q_size * 2;                                        // Whole blocks
		if (num_buckets % super_q != 0) size += (1 + ((num_buckets % super_q + q - 1) / q + 3) / 4) * 2; // Partial block
		return size;
	}

  public:
	DoubleEF() {}
	DoubleEF(const std::vector<uint64_t> &cum_keys, const std::vector<uint64_t> &position);
	~DoubleEF();
	void get(const uint64_t i, uint64_t &cum_keys, uint64_t &position);
	void get(const uint64_t i, uint64_t &cum_keys, uint64_t &cum_keys_next, uint64_t &position);
	uint64_t bit_count_cum_keys();
	uint64_t bit_count_position();
	int dump(FILE *fp) const;
	void load(FILE *fp);
};

} // namespace sux
