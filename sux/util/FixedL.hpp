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

namespace sux::util {

/** A standard (fixed-size) Fenwick tree in level-order layout.
 *
 * @tparam BOUND maximum representable value (at most the maximum value of a `uint64_t`).
 */

template <size_t BOUND> class FixedL : public SearchablePrefixSums {
  public:
	static constexpr size_t BOUNDSIZE = ceil_log2_plus1(BOUND);
	static_assert(BOUNDSIZE >= 1 && BOUNDSIZE <= 64, "Leaves can't be stored in a 64-bit word");

  protected:
	Vector<uint64_t> Tree[64];
	size_t Levels, Size;

  public:
	/** Creates a new instance with no values (empty tree). */
	FixedL() : Levels(0), Size(0) {}

	/** Creates a new instance with given vector of values.
	 *
	 * @param sequence a sequence of nonnegative integers smaller than or equal to the template parameter `BOUND`.
	 * @param size the number of elements in the sequence.
	 */
	FixedL(uint64_t sequence[], size_t size) : Levels(size != 0 ? lambda(size) + 1 : 1), Size(size) {
		for (size_t i = 1; i <= Levels; i++) {
			size_t space = (size + (1ULL << (i - 1))) / (1ULL << i);
			Tree[i - 1].reserve(space);
			Tree[i - 1].resize(space);
		}

		for (size_t l = 0; l < Levels; l++) {
			for (size_t node = 1ULL << l; node <= size; node += 1ULL << (l + 1)) {
				size_t sequence_idx = node - 1;
				uint64_t value = sequence[sequence_idx];
				for (size_t j = 0; j < l; j++) {
					sequence_idx >>= 1;
					value += Tree[j][sequence_idx];
				}

				Tree[l][node >> (l + 1)] = value;
			}
		}
	}

	virtual uint64_t prefix(size_t idx) {
		uint64_t sum = 0;

		while (idx != 0) {
			const int height = rho(idx);
			size_t level_idx = idx >> (1 + height);
			sum += Tree[height][level_idx];

			idx = clear_rho(idx);
		}

		return sum;
	}

	virtual void add(size_t idx, int64_t inc) {
		while (idx <= Size) {
			const int height = rho(idx);
			size_t level_idx = idx >> (1 + height);
			Tree[height][level_idx] += inc;

			idx += mask_rho(idx);
		}
	}

	using SearchablePrefixSums::find;
	virtual size_t find(uint64_t *val) {
		size_t node = 0, idx = 0;

		for (size_t height = Levels - 1; height != SIZE_MAX; height--) {
			size_t pos = idx;

			idx <<= 1;

			if (pos >= Tree[height].size()) continue;

			uint64_t value = Tree[height][pos];
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
			size_t pos = idx;

			idx <<= 1;

			if (pos >= Tree[height].size()) continue;

			uint64_t value = (BOUND << height) - Tree[height][pos];
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
		size_t level_idx = Size >> (1 + height);
		Tree[height].resize(level_idx + 1);

		Tree[height][level_idx] = val;

		size_t idx = level_idx << 1;
		for (size_t h = height - 1; h != SIZE_MAX; h--) {
			Tree[height][level_idx] += Tree[h][idx];
			idx = (idx << 1) + 1;
		}
	}

	virtual void pop() {
		int height = rho(Size--);
		Tree[height].popBack();
	}

	virtual void reserve(size_t space) {
		size_t levels = lambda(space) + 1;
		for (size_t i = 1; i <= levels; i++) Tree[i - 1].reserve((space + (1ULL << (i - 1))) / (1ULL << i));
	}

	using SearchablePrefixSums::trimToFit;
	virtual void trim(size_t space) {
		size_t levels = lambda(space) + 1;
		for (size_t i = 1; i <= levels; i++) Tree[i - 1].trim((space + (1ULL << (i - 1))) / (1ULL << i));
	};

	virtual size_t size() const { return Size; }

	virtual size_t bitCount() const {
		size_t ret = sizeof(FixedL<BOUNDSIZE>) * 8;

		for (size_t i = 0; i < 64; i++) ret += Tree[i].bitCount() - sizeof(Tree[i]);

		return ret;
	}

  private:
	friend std::ostream &operator<<(std::ostream &os, const FixedL<BOUND> &ft) {
		const uint64_t nsize = htol((uint64_t)ft.Size);
		os.write((char *)&nsize, sizeof(uint64_t));

		const uint64_t nlevels = htol((uint64_t)ft.Levels);
		os.write((char *)&nlevels, sizeof(uint64_t));

		for (size_t i = 0; i < ft.Levels; ++i) {
			const uint64_t nlevel = htol((uint64_t)ft.Level[i]);
			os.write((char *)&nlevel, sizeof(uint64_t));
		}

		for (size_t i = 0; i < 64; i++) os << ft.Tree[i];

		return os;
	}

	friend std::istream &operator>>(std::istream &is, FixedL<BOUND> &ft) {
		uint64_t nsize;
		is.read((char *)(&nsize), sizeof(uint64_t));
		ft.Size = ltoh(nsize);

		uint64_t nlevels;
		is.read((char *)&nlevels, sizeof(uint64_t));
		ft.Levels = ltoh(nlevels);

		ft.Level = make_unique<size_t[]>(ft.Levels);
		for (size_t i = 0; i < ft.Levels; ++i) {
			uint64_t nlevel;
			is.read((char *)&nlevel, sizeof(uint64_t));
			ft.Level[i] = ltoh(nlevel);
		}

		for (size_t i = 0; i < 64; i++) is >> ft.Tree[i];

		return is;
	}
};

} // namespace sux::util