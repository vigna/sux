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

#include "../../common.hpp"
#include "../DArray.hpp"
#include "../Vector.hpp"

namespace sux::fenwick {

/** Fenwick Tree data structure interface.
 *
 * @param sequence An integer vector.
 * @param size The length of the sequence.
 * @param BOUND maximum value that @sequence can store.
 *
 * This data structure indices starts from 1 and ends in @see size.
 */
class FenwickTree {
  public:
	virtual ~FenwickTree() = default;

	/** Compute the prefix sum.
   *
	 * @param idx Length of the prefix sum.
	 * @return The sum of the elements in the range (0 .. @see idx] or zero when @see idx
	 * is zero.
	 */
	virtual uint64_t prefix(size_t idx) const = 0;

	/** Increment an element of the sequence (not the tree).
   *
	 * @param idx: Index (starting from 1) of the element.
	 * @param inc: Value to sum.
	 *
	 * You are allowed to use negative values for the increment, but keep in mind you should respect
	 * the structure boundaries.
	 *
	 */
	virtual void add(size_t idx, int64_t inc) = 0;

	/** Search the index of the closest (less or equal than) prefix.
   *
	 * @param val Prefix to search.
	 *
	 * If @see val is an l-value reference its value will be changed with the distance between the found
	 * and the searched prefix (i.e. the difference between the prefix and @see val).
	 *
	 * This method returns zero if such an element doesn't exists (i.e. there are no prefixes that are
	 * greater or equal to @see val).
	 *
	 */
	virtual size_t find(uint64_t *val) const = 0;
	size_t find(uint64_t val) const { return find(&val); }

	/**
	 * compFind() - Complement find.
	 * @param val Prefix to search.
	 *
	 * This method search the index whose its prefix its the closest to MAXVAL-@see val. MAXVAL is the
	 * maximum possibile value for such a prefix (@sequence is therefore bounded).
	 *
	 * The same considerations made for FenwickTree::find() holds.
	 *
	 */
	virtual size_t compFind(uint64_t *val) const = 0;
	size_t compFind(uint64_t val) const { return compFind(&val); }

	/**
	 * push() - Append a value to the sequence
	 * @param val: Value to append
	 *
	 * Append a new value to the sequence and update the Tree respectively. You are allowed to use
	 * negative values for the increment, but keep in mind you should respect the structure
	 * boundaries.
	 *
	 */
	virtual void push(int64_t val) = 0;

	/**
	 * pop() - Remove the last value of the sequence
	 *
	 * This method does not release the allocated space.
	 *
	 */
	virtual void pop() = 0;

	/**
	 * reserve() - Reserve enough space to contain @see space elements
	 * @param space how much space to reserve
	 *
	 * Nothing happens if the requested space is already reserved. Opposite of FenwickTree::shrink().
	 *
	 */
	virtual void reserve(size_t space) = 0;

	/**
	 * shrink() - Free part of the reserved memory
	 * @param space how much space to reserve
	 *
	 * Nothing happens if @see space is more than the reserved spcae. This method behaves as
	 * FenwickTree::shrinkToFit() for values less or equal than the required space for the tree.
	 * Nothing happens if the requested space more than the space reserved. Opposite of
	 * FenwickTree::reserve().
	 *
	 */
	virtual void shrink(size_t space) = 0;

	/**
	 * shrinkToFit() - Reserve enough space to contain @see space elements
	 * @param space how much space to reserve
	 *
	 * Nothing happens if @see space is more than the reserved spcae. This method behaves as
	 * FenwickTree::shrinkToFit() for values less or equal than the required space for the tree.
	 * Nothing happens if the requested space more than the space reserved. Opposite of
	 * FenwickTree::reserve().
	 *
	 */
	void shrinkToFit() { shrink(0); };

	/**
	 * size() - Returns the length of the sequence.
	 *
	 */
	virtual size_t size() const = 0;

	/**
	 * bitCount() - Estimation of the size (in bits) of this structure.
	 *
	 */
	virtual size_t bitCount() const = 0;

	/**
	 * Each FenwickTree is serializable and deserializable with:
	 * - friend std::ostream &operator<<(std::ostream &os, const FenwickTree &ft);
	 * - friend std::istream &operator>>(std::istream &is, FenwickTree &ft);
	 *
	 * The data is stored and loaded with the network (big-endian) byte order to guarantee
	 * compatibility on different architectures.
	 *
	 * The serialized data follows the compression and node ordering of the specific Fenwick tree
	 * without any compatibility layer (e.g. if you serialize a FixedF, you cannot deserialize the
	 * very same data with a ByteL).
	 *
	 */
};

} // namespace sux::fenwick
