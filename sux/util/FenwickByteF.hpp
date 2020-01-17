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

#include "SearchablePrefixSums.hpp"
#include "Vector.hpp"

namespace sux::util {

/** A byte-compressed Fenwick tree in classical layout.
 *
 * @tparam BOUND maximum representable value (at most the maximum value of a `uint64_t`).
 * @tparam AT a type of memory allocation out of ::AllocType.
 */
template <size_t BOUND, AllocType AT = MALLOC> class FenwickByteF : public SearchablePrefixSums, public Expandable {
  public:
	static constexpr size_t BOUNDSIZE = ceil_log2_plus1(BOUND);
	static_assert(BOUNDSIZE >= 1 && BOUNDSIZE <= 64, "Leaves can't be stored in a 64-bit word");

  protected:
	Vector<uint8_t, AT> Tree;
	size_t Size;

  public:
	/** Creates a new instance with no values (empty tree). */
	FenwickByteF() : Tree(0), Size(0) {}

	/** Creates a new instance with given vector of values.
	 *
	 * Note that the provided sequence is read at construction time but
	 * it will not be referenced afterwards.
	 *
	 * @param sequence a sequence of nonnegative integers smaller than or equal to the template parameter `BOUND`.
	 * @param size the number of elements in the sequence.
	 */
	FenwickByteF(uint64_t sequence[], size_t size) : Tree(pos(size + 1) + 8), Size(size) {
		for (size_t i = 1; i <= Size; i++) bytewrite(&Tree[pos(i)], bytesize(i), sequence[i - 1]);

		for (size_t m = 2; m <= Size; m <<= 1) {
			for (size_t idx = m; idx <= Size; idx += m) {
				const uint64_t left = byteread(&Tree[pos(idx)], bytesize(idx));
				const uint64_t right = byteread(&Tree[pos(idx - m / 2)], bytesize(idx - m / 2));
				bytewrite(&Tree[pos(idx)], bytesize(idx), left + right);
			}
		}
	}

	virtual uint64_t prefix(size_t idx) {
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

	using SearchablePrefixSums::find;
	virtual size_t find(uint64_t *val) {
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

	using SearchablePrefixSums::compFind;
	virtual size_t compFind(uint64_t *val) {
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

	virtual void push(uint64_t val) {
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

	virtual void grow(size_t space) { Tree.grow(pos(space) + 8); }

	virtual void reserve(size_t space) { Tree.reserve(pos(space) + 8); }

	virtual void trim(size_t space) { Tree.trim(pos(space) + 8); };

	virtual void trimToFit() { trim(Size); };

	virtual void resize(size_t space) { Tree.resize(pos(space) + 8); }

	virtual void size(size_t space) { Tree.size(pos(space) + 8); };

	virtual size_t size() const { return Size; }

	virtual size_t bitCount() const { return Tree.bitCount() - sizeof(Tree) * 8 - sizeof(*this) * 8; }

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

	friend std::ostream &operator<<(std::ostream &os, const FenwickByteF<BOUND, AT> &ft) {
		os.write((char *)&ft.Size, sizeof(uint64_t));
		return os << ft.Tree;
	}

	friend std::istream &operator>>(std::istream &is, FenwickByteF<BOUND, AT> &ft) {
		is.read((char *)(&ft.Size), sizeof(uint64_t));
		return is >> ft.Tree;
	}
};

} // namespace sux::util
