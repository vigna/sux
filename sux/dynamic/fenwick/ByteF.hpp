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

#include "Tree.hpp"

namespace sux::fenwick {

/**
 * class ByteF - byte compression and classical node layout.
 * @sequence: sequence of integers.
 * @size: number of elements.
 * @BOUND: maximum value that @sequence can store.
 *
 */
template <size_t BOUND> class ByteF : public FenwickTree {
  public:
	static constexpr size_t BOUNDSIZE = ceil_log2_plus1(BOUND);
	static_assert(BOUNDSIZE >= 1 && BOUNDSIZE <= 64, "Leaves can't be stored in a 64-bit word");

  protected:
	Vector<uint8_t> Tree;
	size_t Size;

  public:
	ByteF() : Tree(0), Size(0) {}

	ByteF(uint64_t sequence[], size_t size) : Tree(pos(size + 1) + 8), Size(size) {
		for (size_t i = 1; i <= Size; i++) bytewrite(&Tree[pos(i)], bytesize(i), sequence[i - 1]);

		for (size_t m = 2; m <= Size; m <<= 1) {
			for (size_t idx = m; idx <= Size; idx += m) {
				const uint64_t left = byteread(&Tree[pos(idx)], bytesize(idx));
				const uint64_t right = byteread(&Tree[pos(idx - m / 2)], bytesize(idx - m / 2));
				bytewrite(&Tree[pos(idx)], bytesize(idx), left + right);
			}
		}
	}

	virtual uint64_t prefix(size_t idx) const {
		uint64_t sum = 0;

		while (idx != 0) {
			sum += byteread(&Tree[pos(idx)], bytesize(idx));
			idx = clear_rho(idx);
		}

		return sum;
	}

	virtual void add(size_t idx, int64_t inc) {
		while (idx <= Size) {
			bytewrite_inc(&Tree[pos(idx)], inc);
			idx += mask_rho(idx);
		}
	}

	using FenwickTree::find;
	virtual size_t find(uint64_t *val) const {
		size_t node = 0;

		for (size_t m = mask_lambda(Size); m != 0; m >>= 1) {
			if (node + m > Size) continue;

			const uint64_t value = byteread(&Tree[pos(node + m)], bytesize(node + m));

			if (*val >= value) {
				node += m;
				*val -= value;
			}
		}

		return node;
	}

	using FenwickTree::compFind;
	virtual size_t compFind(uint64_t *val) const {
		size_t node = 0;

		for (size_t m = mask_lambda(Size); m != 0; m >>= 1) {
			if (node + m > Size) continue;

			const uint64_t value = (BOUND << rho(node + m)) - byteread(&Tree[pos(node + m)], bytesize(node + m));

			if (*val >= value) {
				node += m;
				*val -= value;
			}
		}

		return node;
	}

	virtual void push(int64_t val) {
		size_t p = pos(++Size);
		Tree.resize(p + 8);
		bytewrite(&Tree[p], bytesize(Size), val);

		if ((Size & 1) == 0) {
			for (size_t idx = Size - 1; rho(idx) < rho(Size); idx = clear_rho(idx)) {
				uint64_t inc = byteread(&Tree[pos(idx)], bytesize(idx));
				bytewrite_inc(&Tree[p], inc);
			}
		}
	}

	virtual void pop() { Size--; }

	virtual void reserve(size_t space) { Tree.reserve(pos(space) + 8); }

	using FenwickTree::shrinkToFit;
	virtual void shrink(size_t space) { Tree.shrink(pos(space) + 8); };

	virtual size_t size() const { return Size; }

	virtual size_t bitCount() const { return sizeof(ByteF<BOUNDSIZE>) * 8 + Tree.bitCount() - sizeof(Tree); }

  private:
	static inline size_t bytesize(size_t idx) { return ((rho(idx) + BOUNDSIZE - 1) >> 3) + 1; }

	static inline size_t holes(size_t idx) {
		// Exhaustive benchmarking shows it is better to use no holes on (relatively) small trees,
		// but we expect holes to be handy again in (very) big trees
		if (BOUNDSIZE >= 32) return 0;

#ifdef HFT_DISABLE_TRANSHUGE
		return (idx >> (18 + (64 - BOUNDSIZE) % 8));
#else
		return (idx >> (28 + (64 - BOUNDSIZE) % 8));
#endif
	}

	static inline size_t pos(size_t idx) {
		idx--;
		constexpr size_t NEXTBYTE = ((BOUNDSIZE - 1) | (8 - 1)) + 1;

		constexpr size_t SMALL = ((BOUNDSIZE - 1) >> 3) + 1;
		constexpr size_t MEDIUM = NEXTBYTE - BOUNDSIZE + 1;
		constexpr size_t LARGE = MEDIUM + 8;

		constexpr size_t MULTIPLIER = 8 - SMALL - 1;

		return idx * SMALL + (idx >> MEDIUM) + (idx >> LARGE) * MULTIPLIER + holes(idx);
	}

	friend std::ostream &operator<<(std::ostream &os, const ByteF<BOUND> &ft) {
		uint64_t nsize = hton((uint64_t)ft.Size);
		os.write((char *)&nsize, sizeof(uint64_t));

		return os << ft.Tree;
	}

	friend std::istream &operator>>(std::istream &is, ByteF<BOUND> &ft) {
		uint64_t nsize;
		is.read((char *)(&nsize), sizeof(uint64_t));
		ft.Size = ntoh(nsize);

		return is >> ft.Tree;
	}
};

} // namespace sux::fenwick
