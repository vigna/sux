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

/** A byte-compressed Fenwick tree in level-order layout.
 *
 * @tparam BOUND maximum representable value (at most the maximum value of a `uint64_t`).
 * @tparam AT a type of memory allocation out of ::AllocType.
 */

template <size_t BOUND, AllocType AT = MALLOC> class FenwickByteL : public SearchablePrefixSums, public Expandable {
  public:
	static constexpr size_t BOUNDSIZE = ceil_log2_plus1(BOUND);
	static_assert(BOUNDSIZE >= 1 && BOUNDSIZE <= 64, "Leaves can't be stored in a 64-bit word");

  protected:
	Vector<uint8_t, AT> Tree[64];
	size_t Levels, Size;

  public:
	/** Creates a new instance with no values (empty tree). */
	FenwickByteL() : Levels(0), Size(0) {}

	/** Creates a new instance with given vector of values.
	 *
	 * Note that the provided sequence is read at construction time but
	 * it will not be referenced afterwards.
	 *
	 * @param sequence a sequence of nonnegative integers smaller than or equal to the template parameter `BOUND`.
	 * @param size the number of elements in the sequence.
	 */
	FenwickByteL(uint64_t sequence[], size_t size) : Levels(size != 0 ? lambda(size) + 1 : 1), Size(size) {
		this->size(size ? size : 1);

		for (size_t l = 0; l < Levels; l++) {
			for (size_t node = 1ULL << l; node <= Size; node += 1ULL << (l + 1)) {
				size_t sequence_idx = node - 1;
				uint64_t value = sequence[sequence_idx];

				for (size_t j = 0; j < l; j++) {
					sequence_idx >>= 1;
					size_t lowpos = heightsize(j) * sequence_idx;
					value += byteread(&Tree[j][lowpos], heightsize(j));
				}

				size_t highpos = heightsize(l) * (node >> (l + 1));
				bytewrite(&Tree[l][highpos], heightsize(l), value);
			}
		}
	}

	virtual uint64_t prefix(size_t idx) {
		uint64_t sum = 0;

		while (idx != 0) {
			const int height = rho(idx);
			const size_t isize = heightsize(height);
			const size_t pos = (idx >> (1 + height)) * isize;

			sum += byteread(&Tree[height][pos], isize);
			idx = clear_rho(idx);
		}

		return sum;
	}

	virtual void add(size_t idx, int64_t inc) {
		while (idx <= Size) {
			int height = rho(idx);
			size_t isize = heightsize(height);
			size_t pos = (idx >> (1 + height)) * isize;

			bytewrite_inc(&Tree[height][pos], inc);
			idx += mask_rho(idx);
		}
	}

	using SearchablePrefixSums::find;
	virtual size_t find(uint64_t *val) {
		size_t node = 0, idx = 0;

		for (size_t height = Levels - 1; height != SIZE_MAX; height--) {
			const size_t isize = heightsize(height);
			const size_t pos = idx * heightsize(height);

			idx <<= 1;

			if (pos >= Tree[height].size() - 8) continue;

			const uint64_t value = byteread(&Tree[height][pos], isize);

			if (*val >= value) {
				idx++;
				*val -= value;
				node += 1ULL << height;
			}
		}

		return min(node, Size);
	}

	using SearchablePrefixSums::compFind;
	virtual size_t compFind(uint64_t *val) {
		size_t node = 0, idx = 0;

		for (size_t height = Levels - 1; height != SIZE_MAX; height--) {
			const size_t isize = heightsize(height);
			const size_t pos = idx * heightsize(height);

			idx <<= 1;

			if (pos >= Tree[height].size() - 8) continue;

			const uint64_t value = (BOUND << height) - byteread(&Tree[height][pos], isize);

			if (*val >= value) {
				idx++;
				*val -= value;
				node += 1ULL << height;
			}
		}

		return min(node, Size);
	}

	virtual void push(uint64_t val) {
		Levels = lambda(++Size) + 1;

		int height = rho(Size);
		size_t idx = Size >> (1 + height);
		size_t hisize = heightsize(height);
		size_t highpos = idx * hisize;

		Tree[height].resize(highpos + 8);
		bytewrite(&Tree[height][highpos], hisize, val);

		idx <<= 1;
		for (size_t h = height - 1; h != SIZE_MAX; h--) {
			size_t losize = heightsize(h);
			size_t lowpos = idx * losize;

			int64_t inc = byteread(&Tree[h][lowpos], losize);

			bytewrite_inc(&Tree[height][highpos], inc);

			idx = (idx << 1) + 1;
		}
	}

	virtual void pop() {
		int height = rho(Size);
		Tree[height].resize((Size >> (1 + height)) * heightsize(height) + 7);
		Size--;
	}

	virtual void grow(size_t space) {
		size_t levels = lambda(space) + 1;
		for (size_t i = 0; i < levels; i++) Tree[i].grow(((space + (1ULL << i)) / (1ULL << (i + 1))) * heightsize(i) + 8);
	};

	virtual void reserve(size_t space) {
		size_t levels = lambda(space) + 1;
		for (size_t i = 0; i < levels; i++) Tree[i].reserve(((space + (1ULL << i)) / (1ULL << (i + 1))) * heightsize(i) + 8);
	}

	using Expandable::trimToFit;
	virtual void trim(size_t space) {
		size_t levels = lambda(space) + 1;
		for (size_t i = 0; i < levels; i++) Tree[i].trim(((space + (1ULL << i)) / (1ULL << (i + 1))) * heightsize(i) + 8);
	};

	virtual void resize(size_t space) {
		size_t levels = lambda(space) + 1;
		for (size_t i = 0; i < levels; i++) Tree[i].resize(((space + (1ULL << i)) / (1ULL << (i + 1))) * heightsize(i) + 8);
	}

	virtual void size(size_t space) {
		size_t levels = lambda(space) + 1;
		for (size_t i = 0; i < levels; i++) Tree[i].size(((space + (1ULL << i)) / (1ULL << (i + 1))) * heightsize(i) + 8);
	}

	virtual size_t size() const { return Size; }

	virtual size_t bitCount() const {
		size_t ret = sizeof(*this) * 8;
		for (size_t i = 0; i < 64; i++) ret += Tree[i].bitCount() - sizeof(Tree[i]) * 8;
		return ret;
	}

  private:
	static inline size_t heightsize(size_t height) { return ((height + BOUNDSIZE - 1) >> 3) + 1; }

	friend std::ostream &operator<<(std::ostream &os, const FenwickByteL<BOUND, AT> &ft) {
		os.write((char *)&ft.Size, sizeof(uint64_t));
		os.write((char *)&ft.Levels, sizeof(uint64_t));
		for (size_t i = 0; i < ft.Levels; i++) os << ft.Tree[i];
		return os;
	}

	friend std::istream &operator>>(std::istream &is, FenwickByteL<BOUND, AT> &ft) {
		is.read((char *)&ft.Size, sizeof(uint64_t));
		is.read((char *)&ft.Levels, sizeof(uint64_t));
		for (size_t i = 0; i < ft.Levels; i++) is >> ft.Tree[i];
		return is;
	}
};

} // namespace sux::util
