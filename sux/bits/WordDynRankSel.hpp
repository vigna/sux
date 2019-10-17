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
 *  of a searchable prefix-sum data structure and broadword
 *  operations on a single word.
 *
 * @tparam T: Underlying sux::util::SearchablePrefixSums implementation.
 * @tparam AT a type of memory allocation out of util::AllocType.
 */

template <template <size_t, util::AllocType AT> class T, util::AllocType AT = util::MALLOC> class WordDynRankSel : public DynamicBitVector, public Rank, public Select, public SelectZero {
  private:
	static constexpr size_t BOUNDSIZE = 64;
	size_t Size;
	T<BOUNDSIZE, AT> Fenwick;
	util::Vector<uint64_t, AT> Vector;

  public:
	/** Creates a new instance using a given bit vector.
	 *
	 * Note that the bit vector will be copied.
	 *
	 * @param bitvector a bit vector of 64-bit words.
	 * @param size the length (in bits) of the bit vector.
	 */
	WordDynRankSel(uint64_t bitvector[], size_t size) : Size(size), Fenwick(buildFenwick(bitvector, divRoundup(size, BOUNDSIZE))), Vector(util::Vector<uint64_t, AT>(divRoundup(size + 1, 64))) {
		std::copy_n(bitvector, divRoundup(size, 64), Vector.p());
	}

	/** Creates a new instance using a given bit vector.
	 *
	 * Note that thte argument Vector will not be copied.
	 *
	 * @param bitvector a bit vector of 64-bit words.
	 * @param size the length (in bits) of the bit vector.
	 */
	WordDynRankSel(util::Vector<uint64_t, AT> bitvector, size_t size) : Size(size), Fenwick(buildFenwick(bitvector.p(), divRoundup(size, BOUNDSIZE))), Vector(std::move(bitvector)) {}

	const uint64_t *bitvector() const { return Vector.p(); }

	using Rank::rank;
	using Rank::rankZero;
	virtual uint64_t rank(size_t pos) { return Fenwick.prefix(pos / 64) + nu(Vector[pos / 64] & ((1ULL << (pos % 64)) - 1)); }

	virtual size_t select(uint64_t rank) {
		size_t idx = Fenwick.find(&rank);

		uint64_t rank_chunk = nu(Vector[idx]);
		if (rank < rank_chunk) return idx * 64 + select64(Vector[idx], rank);

		return SIZE_MAX;
	}

	virtual size_t selectZero(uint64_t rank) {
		const size_t idx = Fenwick.compFind(&rank);

		uint64_t rank_chunk = nu(~Vector[idx]);
		if (rank < rank_chunk) return idx * 64 + select64(~Vector[idx], rank);

		return SIZE_MAX;
	}

	virtual uint64_t update(size_t index, uint64_t word) {
		uint64_t old = Vector[index];
		Vector[index] = word;
		Fenwick.add(index + 1, nu(word) - nu(old));

		return old;
	}

	virtual bool set(size_t index) {
		uint64_t old = Vector[index / 64];
		Vector[index / 64] |= uint64_t(1) << (index % 64);

		if (Vector[index / 64] != old) {
			Fenwick.add(index / 64 + 1, 1);
			return false;
		}

		return true;
	}

	virtual bool clear(size_t index) {
		uint64_t old = Vector[index / 64];
		Vector[index / 64] &= ~(uint64_t(1) << (index % 64));

		if (Vector[index / 64] != old) {
			Fenwick.add(index / 64 + 1, -1);
			return true;
		}

		return false;
	}

	virtual bool toggle(size_t index) {
		uint64_t old = Vector[index / 64];
		Vector[index / 64] ^= uint64_t(1) << (index % 64);
		bool was_set = Vector[index / 64] < old;
		Fenwick.add(index / 64 + 1, was_set ? -1 : 1);

		return was_set;
	}

	virtual size_t size() const { return Size; }

	virtual size_t bitCount() const { return sizeof(WordDynRankSel<T, AT>) + Vector.bitCount() - sizeof(Vector) + Fenwick.bitCount() - sizeof(Fenwick); }

  private:
	static size_t divRoundup(size_t x, size_t y) {
		return (x + y - 1) / y;
	}

	T<BOUNDSIZE, AT> buildFenwick(const uint64_t bitvector[], size_t size) {
		uint64_t *sequence = new uint64_t[size];
		for (size_t i = 0; i < size; i++) sequence[i] = nu(bitvector[i]);

		T<BOUNDSIZE, AT> tree(sequence, size);
		delete[] sequence;
		return tree;
	}

	friend std::ostream &operator<<(std::ostream &os, const WordDynRankSel<T, AT> &bv) {
		const uint64_t nsize = htol((uint64_t)bv.Size);
		os.write((char *)&nsize, sizeof(uint64_t));

		return os << bv.Fenwick << bv.Vector;
	}

	friend std::istream &operator>>(std::istream &is, WordDynRankSel<T, AT> &bv) {
		uint64_t nsize;
		is.read((char *)(&nsize), sizeof(uint64_t));
		bv.Size = ltoh(nsize);

		return is >> bv.Fenwick >> bv.Vector;
	}
};

} // namespace sux::bits
