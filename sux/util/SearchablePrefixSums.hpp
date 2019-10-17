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

namespace sux::util {

/** An interface for classes implementing _searchable prefix sums_.
 *
 * A searchable prefix-sums data structure maintains a list of
 * nonnegative values and makes it possible to compute sum of
 * prefixes and find the length of the longest prefix sum smaller
 * than or equal to a given bound.
 *
 * Indices into the list start (oddly) from 1 and end at size(),
 * included, because the typical implementation is by a Fenwick
 * tree, for which indexing from 1 is natural.
 *
 * Each SearchablePrefixSums is serializable and deserializable with:
 * - friend std::ostream &operator<<(std::ostream &os, const SearchablePrefixSums &ft);
 * - friend std::istream &operator>>(std::istream &is, SearchablePrefixSums &ft);
 *
 * The data is stored and loaded in little-endian byte order to guarantee
 * compatibility on different architectures.
 *
 * The serialized data follows the compression and node ordering of the specific Fenwick tree
 * without any compatibility layer (e.g., if you serialize a FenwickFixedF, you cannot deserialize the
 * very same data with a FenwickByteL).
 */
class SearchablePrefixSums {
  public:
	virtual ~SearchablePrefixSums() = default;

	/** Compute the prefix sum.
	 *
	 * @param length length of the prefix sum (from 0 to size(), included).
	 * @return the sum of the elements in the range `(0 .. length]`.
	 */
	virtual uint64_t prefix(size_t length) = 0;

	/** Increment an element of the sequence (not the tree).
	 *
	 * @param idx: index of the element.
	 * @param c: value to sum.
	 *
	 * You are allowed to use negative values for the increment, but elements of
	 * of the sequence must remain all nonnegative.
	 */
	virtual void add(size_t idx, int64_t c) = 0;

	/** Search the length of the longest prefix whose sum is less than or equal to a given bound.
	 *
	 * @param val bound for the prefix sum.
	 *
	 * If `val` is an l-value reference its value will be replaced with the excess, that is,
	 * the difference between `val` and the longest prefix sum described above.
	 *
	 * This method returns zero if there are no prefixes whose sum is
	 * greater or equal to `val`.
	 *
	 */
	virtual size_t find(uint64_t *val) = 0;
	size_t find(uint64_t val) { return find(&val); }

	/** Search the length of the longest prefix whose complemented sum is less than or equal to a given bound.
	 *
	 * @param val bound for the complemented prefix sum.
	 *
	 * If `val` is an l-value reference its value will be replaced with the excess, that is,
	 * the difference between `val` and the complemented longest prefix sum described above.
	 *
	 * This method returns zero if there are no prefixes whose complemented sum is
	 * greater or equal to `val`.
	 *
	 */
	virtual size_t compFind(uint64_t *val) = 0;
	size_t compFind(uint64_t val) { return compFind(&val); }

	/** Append a value to the sequence
	 *
	 * @param val: value to append.
	 *
	 * Append a new value to the sequence and update the tree.
	 */
	virtual void push(uint64_t val) = 0;

	/** Remove the last value of the sequence.
	 *
	 * This method does not release the allocated space.
	 */
	virtual void pop() = 0;

	/** Returns the length of the sequence (i.e., the size of the tree). */
	virtual size_t size() const = 0;

	/** Returns an estimate of the size (in bits) of this structure. */
	virtual size_t bitCount() const = 0;
};

} // namespace sux::util
