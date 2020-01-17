/*
 * Sux: Succinct data structures
 *
 * Copyright (C) 2019-2020 Stefano Marchini
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

#include "../util/Vector.hpp"
#include "DynamicBitVector.hpp"
#include "Rank.hpp"
#include "Select.hpp"
#include "SelectZero.hpp"

namespace sux::bits {

/** Ranking and selection in a dynamic bit vector by means
 * of a searchable prefix-sum data structure and linear
 * searches on a sequence of words of given length.
 *
 * **Warning**: if you plan an calling rank(size_t) with
 * argument size(), you must have at least one additional
 * free bit at the end of the provided bit vector.
 *
 * @tparam SPS underlying sux::util::SearchablePrefixSums implementation.
 * @tparam WORDS length (in words) of the linear search stride.
 * @tparam AT a type of memory allocation for the underlying structure.
 */
template <template <size_t, util::AllocType AT> class SPS, size_t WORDS, util::AllocType AT = util::AllocType::MALLOC>
class StrideDynRankSel : public DynamicBitVector, public Rank, public Select, public SelectZero {
  private:
	static constexpr size_t BOUND = 64 * WORDS;
	uint64_t *const Vector;
	size_t Size;
	SPS<BOUND, AT> SrcPrefSum;

  public:
	/** Creates a new instance using a given bit vector.
	 *
	 * Thus constructor only store a reference
	 * to the provided bit vector. The content of the bit vector
	 * should be changed only through the mutation methods of
	 * this class, or the results will be unpredictable.
	 *
	 * **Warning**: if you plan an calling rank(size_t) with
	 * argument size(), you must have at least one additional
	 * free bit at the end of the provided bit vector.
	 *
	 * @param bitvector a bit vector of 64-bit words.
	 * @param size the length (in bits) of the bit vector.
	 */
	StrideDynRankSel(uint64_t bitvector[], size_t size) : Vector(bitvector), Size(size), SrcPrefSum(buildSrcPrefSum(bitvector, divRoundup(size, 64))) {}

	uint64_t *bitvector() const { return Vector; }

	virtual uint64_t rank(size_t pos) {
		size_t idx = pos / (64 * WORDS);
		uint64_t value = SrcPrefSum.prefix(idx);

		for (size_t i = idx * WORDS; i < pos / 64; i++) value += nu(Vector[i]);

		return value + nu(Vector[pos / 64] & ((1ULL << (pos % 64)) - 1));
	}

	using Rank::rank;
	using Rank::rankZero;
	virtual uint64_t rank(size_t from, size_t to) { return rank(to) - rank(from); }

	virtual size_t select(uint64_t rank) {
		size_t idx = SrcPrefSum.find(&rank);

		for (size_t i = idx * WORDS; i < idx * WORDS + WORDS; i++) {
			uint64_t rank_chunk = nu(Vector[i]);
			if (rank < rank_chunk)
				return i * 64 + select64(Vector[i], rank);
			else
				rank -= rank_chunk;
		}

		return SIZE_MAX;
	}

	virtual size_t selectZero(uint64_t rank) {
		size_t idx = SrcPrefSum.compFind(&rank);

		for (size_t i = idx * WORDS; i < idx * WORDS + WORDS; i++) {
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
		SrcPrefSum.add(index / WORDS + 1, nu(word) - nu(old));

		return old;
	}

	virtual bool set(size_t index) {
		uint64_t old = Vector[index / 64];
		Vector[index / 64] |= uint64_t(1) << (index % 64);

		if (Vector[index / 64] != old) {
			SrcPrefSum.add(index / (WORDS * 64) + 1, 1);
			return false;
		}

		return true;
	}

	virtual bool clear(size_t index) {
		uint64_t old = Vector[index / 64];
		Vector[index / 64] &= ~(uint64_t(1) << (index % 64));

		if (Vector[index / 64] != old) {
			SrcPrefSum.add(index / (WORDS * 64) + 1, -1);
			return true;
		}

		return false;
	}

	virtual bool toggle(size_t index) {
		uint64_t old = Vector[index / 64];
		Vector[index / 64] ^= uint64_t(1) << (index % 64);
		bool was_set = Vector[index / 64] < old;
		SrcPrefSum.add(index / (WORDS * 64) + 1, was_set ? -1 : 1);

		return was_set;
	}

	virtual size_t size() const { return Size; }

	virtual size_t bitCount() const { return SrcPrefSum.bitCount() - sizeof(SrcPrefSum) * 8 + sizeof(*this) * 8 + ((Size + 63) & ~63); }

  private:
	static size_t divRoundup(size_t x, size_t y) {
		if (y > x) return 1;
		return (x / y) + ((x % y != 0) ? 1 : 0);
	}

	static SPS<BOUND, AT> buildSrcPrefSum(const uint64_t bitvector[], size_t size) {
		unique_ptr<uint64_t[]> sequence = make_unique<uint64_t[]>(divRoundup(size, WORDS));
		for (size_t i = 0; i < size; i++) sequence[i / WORDS] += nu(bitvector[i]);
		return SPS<BOUND, AT>(sequence.get(), divRoundup(size, WORDS));
	}

	friend std::ostream &operator<<(std::ostream &os, const StrideDynRankSel<SPS, WORDS, AT> &bv) {
		os.write((char *)&bv.Size, sizeof(uint64_t));
		return os << bv.SrcPrefSum << bv.Vector;
	}

	friend std::istream &operator>>(std::istream &is, StrideDynRankSel<SPS, WORDS, AT> &bv) {
		is.read((char *)&bv.Size, sizeof(uint64_t));
		return is >> bv.SrcPrefSum >> bv.Vector;
	}
};

} // namespace sux::bits
