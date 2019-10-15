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

#include "SearchablePrefixSums.hpp"
#include "Vector.hpp"
#include <cstring>

namespace sux::util {

/** A bit-compressed Fenwick tree in classical layout.
 *
 * @tparam BOUND maximum representable value (at most the maximum value of a `uint64_t`).
 * @tparam AT a type of memory allocation out of ::AllocType.
 */
template <size_t BOUND, PageType AT = MALLOC> class BitF : public SearchablePrefixSums {
  public:
	static constexpr size_t BOUNDSIZE = ceil_log2_plus1(BOUND);
	static constexpr size_t STARTING_OFFSET = 1;
	static constexpr size_t END_PADDING = 56;
	static_assert(BOUNDSIZE >= 1 && BOUNDSIZE <= 55, "Some nodes will span on multiple words");

  protected:
	Vector<uint8_t, AT> Tree;
	size_t Size;

  public:
	/** Creates a new instance with no values (empty tree). */
	BitF() : Size(0) {}

	/** Creates a new instance with given vector of values.
	 *
	 * @param sequence a sequence of nonnegative integers smaller than or equal to the template parameter `BOUND`.
	 * @param size the number of elements in the sequence.
	 */

	BitF(uint64_t sequence[], size_t size) : Tree((first_bit_after(size) + END_PADDING + 7) >> 3), Size(size) {
		for (size_t idx = 1; idx <= Size; idx++) addToPartialFrequency(idx, sequence[idx - 1]);

		for (size_t m = 2; m <= Size; m <<= 1)
			for (size_t idx = m; idx <= Size; idx += m) addToPartialFrequency(idx, getPartialFrequency(idx - m / 2));
	}

	virtual uint64_t prefix(size_t idx) {
		uint64_t sum = 0;

		while (idx != 0) {
			sum += getPartialFrequency(idx);
			idx = clear_rho(idx);
		}

		return sum;
	}

	virtual void add(size_t idx, int64_t inc) {
		while (idx <= Size) {
			addToPartialFrequency(idx, inc);
			idx += mask_rho(idx);
		}
	}

	using SearchablePrefixSums::find;
	virtual size_t find(uint64_t *val) {
		size_t node = 0;

		for (size_t m = mask_lambda(Size); m != 0; m >>= 1) {
			if (node + m > Size) continue;

			const uint64_t value = getPartialFrequency(node + m);

			if (*val >= value) {
				node += m;
				*val -= value;
			}
		}

		return node;
	}

	using SearchablePrefixSums::compFind;
	virtual size_t compFind(uint64_t *val) {
		size_t node = 0;

		for (size_t m = mask_lambda(Size); m != 0; m >>= 1) {
			if (node + m > Size) continue;

			const int height = rho(node + m);
			const uint64_t value = (BOUND << height) - getPartialFrequency(node + m);

			if (*val >= value) {
				node += m;
				*val -= value;
			}
		}

		return node;
	}

	virtual void push(uint64_t val) {
		Tree.resize((first_bit_after(++Size) + END_PADDING + 7) >> 3);
		addToPartialFrequency(Size, val);

		if ((Size & 1) == 0) {
			for (size_t idx = Size - 1; rho(idx) < rho(Size); idx = clear_rho(idx)) addToPartialFrequency(Size, getPartialFrequency(idx));
		}
	}

	virtual void pop() { Size--; }

	virtual void reserve(size_t space) { Tree.reserve((first_bit_after(space) + END_PADDING + 7) >> 3); }

	using SearchablePrefixSums::trimToFit;
	virtual void trim(size_t space) { Tree.reserve((first_bit_after(space) + END_PADDING + 7) >> 3); };

	virtual size_t size() const { return Size; }

	virtual size_t bitCount() const { return sizeof(BitF<BOUNDSIZE, AT>) * 8 + Tree.bitCount() - sizeof(Tree); }

  private:
	inline static size_t holes(size_t idx) { return STARTING_OFFSET + (idx >> 14) * 64; }

	inline static size_t first_bit_after(size_t idx) { return (BOUNDSIZE + 1) * idx - nu(idx) + holes(idx); }

	inline uint64_t getPartialFrequency(size_t idx) const {
		const uint64_t mask = (UINT64_C(1) << (BOUNDSIZE + rho(idx))) - 1;
		idx--;
		const uint64_t prod = (BOUNDSIZE + 1) * idx;
		const uint64_t pos = prod - nu(idx) + holes(idx);

		uint64_t t;
		if ((prod + (BOUNDSIZE + 1)) % 64 == 0) {
			memcpy(&t, (uint64_t *)&Tree[0] + pos / 64, 8);
			return t >> (pos % 64) & mask;
		} else {
			memcpy(&t, &Tree[0] + pos / 8, 8);
			return t >> (pos % 8) & mask;
		}
	}

	inline void addToPartialFrequency(size_t idx, uint64_t value) {
		idx--;
		const uint64_t prod = (BOUNDSIZE + 1) * idx;
		const uint64_t pos = prod - nu(idx) + holes(idx);

		uint64_t t;
		if ((prod + (BOUNDSIZE + 1)) % 64 == 0) {
			uint64_t *const p = (uint64_t *)&Tree[0] + pos / 64;
			memcpy(&t, p, 8);
			t += value << (pos % 64);
			memcpy(p, &t, 8);
		} else {
			uint8_t *const p = &Tree[0] + pos / 8;
			memcpy(&t, p, 8);
			t += value << (pos % 8);
			memcpy(p, &t, 8);
		}
	}

	friend std::ostream &operator<<(std::ostream &os, const BitF<BOUND, AT> &ft) {
		const uint64_t nsize = htol((uint64_t)ft.Size);
		os.write((char *)&nsize, sizeof(uint64_t));

		return os << ft.Tree;
	}

	friend std::istream &operator>>(std::istream &is, BitF<BOUND> &ft) {
		uint64_t nsize;
		is.read((char *)(&nsize), sizeof(uint64_t));
		ft.Size = ltoh(nsize);

		return is >> ft.Tree;
	}
};

} // namespace sux::util
