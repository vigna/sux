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

namespace sux::bits {

/** An interface for all classes implementating dynamic bit vectors.
 *
 * We suggest to make every implementation of this interface
 * serializable and deserializable with:
 * - friend std::ostream &operator<<(std::ostream &os, const FenwickTree &ft);
 * - friend std::istream &operator>>(std::istream &is, FenwickTree &ft);
 *
 * The data should be stored and loaded in little-endian byte order to guarantee
 * compatibility on different architectures.
 */
class DynamicBitVector {
  public:
	virtual ~DynamicBitVector() = default;

	/** Replace a given word in the bitvector.
	 *
	 * @param index index (in words) in the bitvector.
	 * @param word new value for `bitvector[index]`.
	 * @return the replaced word.
	 *
	 */
	virtual uint64_t update(size_t index, uint64_t word) = 0;

	/** Set (set to 1) a given bit in the bitvector.
	 *
	 * @param index index (in bits) in the bitvector.
	 * @return the previous value of such a bit.
	 */
	virtual bool set(size_t index) = 0;

	/** Clear (set to 0) a given bit in the bitvector.
	 *
	 * @param index index (in bits) in the bitvector.
	 * @return: the previous value of such a bit.
	 *
	 */
	virtual bool clear(size_t index) = 0;

	/** Change the value of a given bit in the bitvector.
	 *
	 * @param index index (in bits) in the bitvector.
	 * @return: the previous value of such a bit.
	 *
	 */
	virtual bool toggle(size_t index) = 0;

	/** Returns an estimate of the size (in bits) of this structure. */
	virtual size_t bitCount() const = 0;
};

} // namespace sux::ranking
