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
#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;

namespace sux::function {

#define DEFAULT_VECTSIZE (1 << 2)

/** Storage for Golomb-Rice code of a RecSplit bucket.
 *
 *  This class exists solely to implement RecSplit.
 */

class RiceBitVector {
  private:
	uint64_t *data;
	std::size_t data_bytes;
	std::size_t bit_count;

	size_t curr_fixed_offset;
	uint64_t *curr_ptr_unary;
	uint64_t curr_window_unary;
	int valid_lower_bits_unary;

  public:
	RiceBitVector(const size_t alloc_words = DEFAULT_VECTSIZE);
	~RiceBitVector();
	uint64_t readNext(const int log2golomb);
	void skipSubtree(const size_t nodes, const size_t fixed_len);
	void readReset(const size_t bit_pos = 0, const size_t unary_offset = 0);
	void appendFixed(const uint64_t v, const int log2golomb);
	void appendUnaryAll(const vector<uint32_t> unary);
	size_t getBits() const;
	void fitData();
	void printBits() const;

  private:
	friend std::ostream &operator<<(std::ostream &os, const RiceBitVector &rbv) {
		os.write((char *)&rbv.bit_count, sizeof(rbv.bit_count));
		os.write((char *)&rbv.data_bytes, sizeof(rbv.data_bytes));
		os.write((char *)rbv.data, rbv.data_bytes);
		return os;
	}

	friend std::istream &operator>>(std::istream &is, RiceBitVector &rbv) {
		rbv.curr_window_unary = 0;
		rbv.valid_lower_bits_unary = 0;

		is.read((char *)&rbv.bit_count, sizeof(rbv.bit_count));
		is.read((char *)&rbv.data_bytes, sizeof(rbv.data_bytes));

		if (rbv.data) free(rbv.data);
		rbv.data = (uint64_t *)malloc(rbv.data_bytes);
		is.read((char *)rbv.data, rbv.data_bytes);
		rbv.curr_ptr_unary = rbv.data;
		return is;
	}
};

} // namespace sux::function
