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
#include "Rank.hpp"

#include <cstdint>

namespace sux::bits {

using namespace sux;

/** A class implementing Rank9, a ranking structure using
 * 25% additional space and providing very fast ranking.
 *
 * The constructors of this class only store a reference
 * to a provided bit vector. Should the content of the
 * bit vector change, the results will be unpredictable.
 *
 * **Warning**: if you plan an calling rank(size_t) with
 * argument size(), you must have at least one additional
 * free bit at the end of the provided bit vector.
 *
 * @tparam AT a type of memory allocation out of sux::util::AllocType.
 */

template <util::AllocType AT = util::AllocType::MALLOC> class Rank9 : public Rank {
  protected:
	const size_t num_bits;
	size_t num_ones;
	const uint64_t *bits;
	util::Vector<uint64_t, AT> counts;

  public:
	/** Creates a new instance using a given bit vector.
	 *
	 *  Note that this constructor only stores a reference
	 *  to the provided bit vector. Should the content of the
	 *  bit vector change, the results will be unpredictable.
	 *
	 *  **Warning**: if you plan an calling rank(size_t) with
	 *  argument size(), you must have at least one additional
	 *  free bit at the end of the provided bit vector.
	 *
	 * @param bits a bit vector of 64-bit words.
	 * @param num_bits the length (in bits) of the bit vector.
	 */

	Rank9(const uint64_t *const bits, const uint64_t num_bits) : num_bits(num_bits), bits(bits) {
		const uint64_t num_words = (num_bits + 63) / 64;
		const uint64_t num_counts = ((num_bits + 64 * 8 - 1) / (64 * 8)) * 2;

		// Init rank structure
		counts.size(num_counts + 2);

		num_ones = 0;
		uint64_t pos = 0;
		for (uint64_t i = 0; i < num_words; i += 8, pos += 2) {
			counts[pos] = num_ones;
			num_ones += __builtin_popcountll(bits[i]);
			for (int j = 1; j < 8; j++) {
				counts[pos + 1] |= (num_ones - counts[pos]) << 9 * (j - 1);
				if (i + j < num_words) num_ones += __builtin_popcountll(bits[i + j]);
			}
		}

		counts[num_counts] = num_ones;

		assert(num_ones <= num_bits);
	}

	uint64_t rank(const size_t k) {
		const uint64_t word = k / 64;
		const uint64_t block = word / 4 & ~1;
		const int offset = word % 8 - 1;
		return counts[block] + (counts[block + 1] >> (offset + (offset >> (sizeof offset * 8 - 4) & 0x8)) * 9 & 0x1FF) + __builtin_popcountll(bits[word] & ((1ULL << k % 64) - 1));
	}

	/** Returns an estimate of the size in bits of this structure. */
	size_t bitCount() const { return counts.bitCount() - sizeof(counts) * 8 + sizeof(*this) * 8; }

	/** Returns the size in bits of the underlying bit vector. */
	size_t size() const { return num_bits; }
};

} // namespace sux::bits
