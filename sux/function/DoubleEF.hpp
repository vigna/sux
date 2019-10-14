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
#include <iostream>
#include <limits>
#include <vector>

#ifndef LOG2Q
#define LOG2Q 8
#endif

namespace sux::function {

/** A double Elias-Fano list.
 *
 *  This class exists solely to implement RecSplit.
 */

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
	uint64_t bitCountCumKeys();
	uint64_t bitCountPosition();

  private:
	friend std::ostream &operator<<(std::ostream &os, DoubleEF &ef) {
		os.write((char *)&ef.num_buckets, sizeof(ef.num_buckets));
		os.write((char *)&ef.u_cum_keys, sizeof(ef.u_cum_keys));
		os.write((char *)&ef.u_position, sizeof(ef.u_position));
		os.write((char *)&ef.cum_keys_min_delta, sizeof(ef.cum_keys_min_delta));
		os.write((char *)&ef.min_diff, sizeof(ef.min_diff));
		os.write((char *)&ef.bits_per_key_fixed_point, sizeof(ef.bits_per_key_fixed_point));

		const uint64_t words_lower_bits = ef.lower_bits_size_words();
		os.write((char *)ef.lower_bits, sizeof(uint64_t) * (size_t)words_lower_bits);
		const uint64_t words_cum_keys = ef.cum_keys_size_words();
		os.write((char *)ef.upper_bits_cum_keys, sizeof(uint64_t) * (size_t)words_cum_keys);
		const uint64_t words_position = ef.position_size_words();
		os.write((char *)ef.upper_bits_position, sizeof(uint64_t) * (size_t)words_position);

		const uint64_t jump_words = ef.jump_size_words();
		os.write((char *)ef.jump, sizeof(uint64_t) * (size_t)jump_words);
		return os;
	}

	friend std::istream &operator>>(std::istream &is, DoubleEF &ef) {
		is.read((char *)&ef.num_buckets, sizeof(ef.num_buckets));
		is.read((char *)&ef.u_cum_keys, sizeof(ef.u_cum_keys));
		is.read((char *)&ef.u_position, sizeof(ef.u_position));
		is.read((char *)&ef.cum_keys_min_delta, sizeof(ef.cum_keys_min_delta));
		is.read((char *)&ef.min_diff, sizeof(ef.min_diff));
		is.read((char *)&ef.bits_per_key_fixed_point, sizeof(ef.bits_per_key_fixed_point));

		ef.l_position = ef.u_position / (ef.num_buckets + 1) == 0 ? 0 : lambda(ef.u_position / (ef.num_buckets + 1));
		ef.l_cum_keys = ef.u_cum_keys / (ef.num_buckets + 1) == 0 ? 0 : lambda(ef.u_cum_keys / (ef.num_buckets + 1));
		assert(ef.l_cum_keys * 2 + ef.l_position <= 56);

		ef.lower_bits_mask_cum_keys = (UINT64_C(1) << ef.l_cum_keys) - 1;
		ef.lower_bits_mask_position = (UINT64_C(1) << ef.l_position) - 1;

		const uint64_t words_lower_bits = ef.lower_bits_size_words();
		ef.lower_bits = new uint64_t[words_lower_bits];
		is.read((char *)ef.lower_bits, sizeof(uint64_t) * (size_t)words_lower_bits);
		const uint64_t words_cum_keys = ef.cum_keys_size_words();
		ef.upper_bits_cum_keys = new uint64_t[words_cum_keys];
		is.read((char *)ef.upper_bits_cum_keys, sizeof(uint64_t) * (size_t)words_cum_keys);
		const uint64_t words_position = ef.position_size_words();
		ef.upper_bits_position = new uint64_t[words_position];
		is.read((char *)ef.upper_bits_position, sizeof(uint64_t) * (size_t)words_position);

		const uint64_t jump_words = ef.jump_size_words();
		ef.jump = new uint64_t[jump_words];
		is.read((char *)ef.jump, sizeof(uint64_t) * (size_t)jump_words);
		return is;
	}
};

} // namespace sux::function
