/*
 * Sux: Succinct data structures
 *
 * Copyright (C) 2019 Stefano Marchini
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

#include "../../Rank.hpp"
#include "../../Select.hpp"
#include "../../SelectZero.hpp"
#include "../../common.hpp"
#include "../DArray.hpp"

namespace sux::ranking {

/**
 * RankSelect - Dynamic rank & select data structure interface.
 * @bitvector: A bit vector of 64-bit words.
 * @length: The length (in words) of the bitvector.
 * @T: Underlining Fenwick tree of an ungiven <size_t> bound.
 *
 * This data structure indices starts from 0 and ends in @length-1.
 *
 */
class RankSelect : public Rank, public Select, public SelectZero {
  public:
	virtual ~RankSelect() = default;

	/**
	 * bitvector() - Returns the bit vector.
	 *
	 * It's a constant, so you can't change the junk inside unless you
	 * explicitly cast away the constness (at your own risk).
	 *
	 */
	virtual const uint64_t *bitvector() const = 0;
	/**
	 * update() - Replace a given word in the bitvector.
	 * @index: index (in words) in the bitvector.
	 * @word: new value for @bitvector[@index]
	 *
	 * This method returns the replaced word.
	 *
	 */
	virtual uint64_t update(size_t index, uint64_t word) = 0;

	/**
	 * set() - Set (set to 1) a given bit in the bitvector.
	 * @index: Index (in bits) in the bitvector.
	 *
	 * This method returns the previous value of such a bit.
	 *
	 */
	virtual bool set(size_t index) = 0;

	/**
	 * clear() - Clear (set to 0) a given bit in the bitvector.
	 * @index: Index (in bits) in the bitvector.
	 *
	 * This method returns the previous value of such a bit.
	 *
	 */
	virtual bool clear(size_t index) = 0;

	/**
	 * toggle() - Change the value of a given bit in the bitvector.
	 * @index: Index (in bits) in the bitvector.
	 *
	 * This method returns the previous value of such a bit.
	 *
	 */
	virtual bool toggle(size_t index) = 0;

	/**
	 * bitCount() - Estimation of the size (in bits) of this structure.
	 *
	 */
	virtual size_t bitCount() const = 0;

	/**
	 * Each RankSelect is serializable and deserializable with:
	 * - friend std::ostream &operator<<(std::ostream &os, const RankSelect &bv);
	 * - friend std::istream &operator>>(std::istream &is, RankSelect &bv);
	 *
	 * The data is stored and loaded with the network (big-endian) byte order to guarantee
	 * compatibility on different architectures.
	 *
	 * The serialized data follows the compression and node ordering of the specific underlying
	 * data structures without any compatibility layer.
	 *
	 */
};

} // namespace sux::ranking
