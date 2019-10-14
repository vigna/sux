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

#include "RankSelect.hpp"

namespace sux::ranking {

/**
 * Word - Linear search on a single word.
 * @bitvector: A bitvector of 64-bit words.
 * @size: The length (in words) of the bitvector.
 * @T: Underlining Fenwick tree with an ungiven <size_t> bound.
 *
 */
template <template <size_t> class T> class Word : public RankSelect {
  private:
	static constexpr size_t BOUNDSIZE = 64;
  size_t Size;
	T<BOUNDSIZE> Fenwick;
	DArray<uint64_t> Vector;

  public:
	Word(uint64_t bitvector[], size_t size) : Size(size), Fenwick(buildFenwick(bitvector, divRoundup(size, BOUNDSIZE))), Vector(DArray<uint64_t>(divRoundup(size, BOUNDSIZE))) {
		std::copy_n(bitvector, divRoundup(size, BOUNDSIZE), Vector.get());
	}

	Word(DArray<uint64_t> bitvector, size_t size) : Size(size), Fenwick(buildFenwick(bitvector.get(), divRoundup(size, BOUNDSIZE))), Vector(std::move(bitvector)) {}

	virtual const uint64_t *bitvector() const { return Vector.get(); }

	using Rank::rank;
	using Rank::rankZero;
	virtual uint64_t rank(size_t pos) { return Fenwick.prefix(pos / 64) + nu(Vector[pos / 64] & ((1ULL << (pos % 64)) - 1)); }

	virtual size_t select(uint64_t rank) {
		size_t idx = Fenwick.find(&rank);

		if (idx >= Vector.size()) return SIZE_MAX;

		uint64_t rank_chunk = nu(Vector[idx]);
		if (rank < rank_chunk) return idx * 64 + select64(Vector[idx], rank);

		return SIZE_MAX;
	}

	virtual size_t selectZero(uint64_t rank) {
		const size_t idx = Fenwick.compFind(&rank);

		if (idx >= Vector.size()) return SIZE_MAX;

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

	virtual size_t bitCount() const { return sizeof(Word<T>) + Vector.bitCount() - sizeof(Vector) + Fenwick.bitCount() - sizeof(Fenwick); }

  private:
  static size_t divRoundup(size_t x, size_t y) {
    if (y > x) return 1;
    return (x / y) + ((x % y != 0) ? 1 : 0);
  }

	T<BOUNDSIZE> buildFenwick(const uint64_t bitvector[], size_t size) {
		uint64_t *sequence = new uint64_t[size];
		for (size_t i = 0; i < size; i++) sequence[i] = nu(bitvector[i]);

		T<BOUNDSIZE> tree(sequence, size);
		delete[] sequence;
		return tree;
	}

	friend std::ostream &operator<<(std::ostream &os, const Word<T> &bv) {
		const uint64_t nsize = htol((uint64_t)bv.Size);
		os.write((char *)&nsize, sizeof(uint64_t));
    
    return os << bv.Fenwick << bv.Vector;
  }

	friend std::istream &operator>>(std::istream &is, Word<T> &bv) {
		uint64_t nsize;
		is.read((char *)(&nsize), sizeof(uint64_t));
		bv.Size = ltoh(nsize);

    return is >> bv.Fenwick >> bv.Vector;
  }
};

} // namespace sux::ranking
