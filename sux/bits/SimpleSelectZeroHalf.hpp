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
#include "SelectZero.hpp"
#include <cstdint>

namespace sux::bits {
using namespace std;

/** A simple SelectZero implementation based on a two-level inventory,
 *  and wired for approximately the same number of zeros and ones.
 *
 * This implementation has been specifically developed to be used
 * with EliasFano.
 */

class SimpleSelectZeroHalf {
  private:
	const uint64_t *bits;
	int64_t *inventory;

	uint64_t num_words, inventory_size, num_ones;

  public:
	SimpleSelectZeroHalf();
	/** Creates a new instance using a given bit vector.
	 *
	 * @param bits a bit vector of 64-bit words.
	 * @param num_bits the length (in bits) of the bit vector.
	 */
	SimpleSelectZeroHalf(const uint64_t *const bits, const uint64_t num_bits);
	~SimpleSelectZeroHalf();
	uint64_t selectZero(const uint64_t rank);
	uint64_t selectZero(const uint64_t rank, uint64_t *const next);
	// Just for analysis purposes
	void printCounts();
	/** Returns an estimate of the size (in bits) of this structure. */
	uint64_t bitCount();
};

} // namespace sux::bits
