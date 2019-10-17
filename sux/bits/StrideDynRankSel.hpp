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

#include "../util/Vector.hpp"
#include "DynamicBitVector.hpp"
#include "Rank.hpp"
#include "Select.hpp"
#include "SelectZero.hpp"

namespace sux::bits {

/** Ranking and selection in a dynamic bit vector by means
 *  of a searchable prefix-sum data structure and linear
 *  searches on a sequence of words of given length.
 *
 * @tparam T: Underlying sux::util::SearchablePrefixSums implementation.
 * @tparam WORDS length (in words) of the linear search stride.
 * @tparam AT a type of memory allocation out of util::AllocType.
 */
template <template <size_t, util::AllocType AT> class T, size_t WORDS, util::AllocType AT = util::MALLOC>
class StrideDynRankSel : public DynamicBitVector, public Rank, public Select, public SelectZero {
  private:
	static constexpr size_t BOUND = 64 * WORDS;
	size_t Size;
	T<BOUND, AT> Fenwick;
	util::Vector<uint64_t, AT> Vector;

  public:
	/** Creates a new instance using a given bit vector.
	 *
	 * Note that the bit vector will be copied.
	 * 
	 * @param bitvector a bit vector of 64-bit words.
	 * @param size the length (in bits) of the bit vector.
	 */
	StrideDynRankSel(uint64_t bitvector[], size_t size) : Size(size), Fenwick(buildFenwick(bitvector, divRoundup(size, 64))), Vector(util::Vector<uint64_t, AT>(divRoundup(size, 64))) {
		std::copy_n(bitvector, divRoundup(size, 64), Vector.p());
	}

	/** Creates a new instance using a given bit vector.
	 *
	 * Note that thte argument Vector will not be copied.
	 * 
	 * @param bitvector a bit vector of 64-bit words.
	 * @param size the length (in bits) of the bit vector.
	 */
	StrideDynRankSel(util::Vector<uint64_t, AT> bitvector, size_t size) : Size(size), Fenwick(buildFenwick(bitvector.p(), divRoundup(size, 64))), Vector(std::move(bitvector)) {}

	const uint64_t *bitvector() const { return Vector.p(); }

	virtual uint64_t rank(size_t pos) {
		size_t idx = pos / (64 * WORDS);
		uint64_t value = Fenwick.prefix(idx);

		for (size_t i = idx * WORDS; i < pos / 64; i++) value += nu(Vector[i]);

		return value + nu(Vector[pos / 64] & ((1ULL << (pos % 64)) - 1));
	}

	using Rank::rank;
	using Rank::rankZero;
	virtual uint64_t rank(size_t from, size_t to) { return rank(to) - rank(from); }

	virtual size_t select(uint64_t rank) {
		size_t idx = Fenwick.find(&rank);

		for (size_t i = idx * WORDS; i < idx * WORDS + WORDS; i++) {
			if (i >= Vector.size()) return SIZE_MAX;

			uint64_t rank_chunk = nu(Vector[i]);
			if (rank < rank_chunk)
				return i * 64 + select64(Vector[i], rank);
			else
				rank -= rank_chunk;
		}

		return SIZE_MAX;
	}

	virtual size_t selectZero(uint64_t rank) {
		size_t idx = Fenwick.compFind(&rank);

		for (size_t i = idx * WORDS; i < idx * WORDS + WORDS; i++) {
			if (i >= Vector.size()) return SIZE_MAX;

			uint64_t rank_chunk = nu(~Vector[i]);
			if (rank < rank_chunk)
				return i * 64 + select64(~Vector[i], rank);
			else
				rank -= rank_chunk;
		}

		return SIZE_MAX;
	}

	virtual uint64_t update(size_t index, uint64_t word) {
		uint64_t old = Vector[index];
		Vector[index] = word;
		Fenwick.add(index / WORDS + 1, nu(word) - nu(old));

		return old;
	}

	virtual bool set(size_t index) {
		uint64_t old = Vector[index / 64];
		Vector[index / 64] |= uint64_t(1) << (index % 64);

		if (Vector[index / 64] != old) {
			Fenwick.add(index / (WORDS * 64) + 1, 1);
			return false;
		}

		return true;
	}

	virtual bool clear(size_t index) {
		uint64_t old = Vector[index / 64];
		Vector[index / 64] &= ~(uint64_t(1) << (index % 64));

		if (Vector[index / 64] != old) {
			Fenwick.add(index / (WORDS * 64) + 1, -1);
			return true;
		}

		return false;
	}

	virtual bool toggle(size_t index) {
		uint64_t old = Vector[index / 64];
		Vector[index / 64] ^= uint64_t(1) << (index % 64);
		bool was_set = Vector[index / 64] < old;
		Fenwick.add(index / (WORDS * 64) + 1, was_set ? -1 : 1);

		return was_set;
	}

	virtual size_t size() const { return Size; }

	virtual size_t bitCount() const { return sizeof(StrideDynRankSel<T, WORDS, AT>) + Vector.bitCount() - sizeof(Vector) + Fenwick.bitCount() - sizeof(Fenwick); }

  private:
	static size_t divRoundup(size_t x, size_t y) {
		if (y > x) return 1;
		return (x / y) + ((x % y != 0) ? 1 : 0);
	}

	static T<BOUND, AT> buildFenwick(const uint64_t bitvector[], size_t size) {
		uint64_t *sequence = new uint64_t[divRoundup(size, WORDS)]();
		for (size_t i = 0; i < size; i++) sequence[i / WORDS] += nu(bitvector[i]);

		T<BOUND, AT> tree(sequence, divRoundup(size, WORDS));
		delete[] sequence;
		return tree;
	}

	friend std::ostream &operator<<(std::ostream &os, const StrideDynRankSel<T, WORDS, AT> &bv) {
		const uint64_t nsize = htol((uint64_t)bv.Size);
		os.write((char *)&nsize, sizeof(uint64_t));

		return os << bv.Fenwick << bv.Vector;
	}

	friend std::istream &operator>>(std::istream &is, StrideDynRankSel<T, WORDS, AT> &bv) {
		uint64_t nsize;
		is.read((char *)(&nsize), sizeof(uint64_t));
		bv.Size = ltoh(nsize);

		return is >> bv.Fenwick >> bv.Vector;
	}
};

} // namespace sux::bits
