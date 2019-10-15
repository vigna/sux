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

#include "../support/common.hpp"
#include <assert.h>
#include <iostream>
#include <string>
#include <sys/mman.h>

namespace sux::util {

/** Possible types of memory paging. */
enum PageType {
	/** Standard allocation with `malloc()` (the default). */
	MALLOC,
	/** Allocation with `mmap()`. Usually in this case allocations
	  * are aligned on a memory page (typically, 4KiB). */
	SMALLPAGE,
	/** Transparent huge pages support through `mmap()` and `madvise()` 
	  * on Linux. Note that the pages in this case are usually 2MiB each. */
	TRANSHUGEPAGE,
	/** Direct huge page support through `mmap()` on Linux. 
	  * Also in this case pages are usually 2MiB each. */
	FORCEHUGEPAGE };

/** An expandable vector with settable type of memory paging.
 *
 *  @tparam T the data type.
 *  @tparam PT a memory-paging type out of ::PageType.
 *
 * [1] https://www.kernel.org/doc/html/latest/admin-guide/mm/hugetlbpage.html
 * [2] https://www.kernel.org/doc/html/latest/admin-guide/mm/transhuge.html
 */

template <typename T, PageType PT = MALLOC> class Vector {

  public:
	static constexpr int PROT = PROT_READ | PROT_WRITE;
	static constexpr int FLAGS = MAP_PRIVATE | MAP_ANONYMOUS | (PT == FORCEHUGEPAGE ? MAP_HUGETLB : 0);

  private:
	size_t Size = 0, Capacity = 0;
	T *Data = nullptr;

  public:
	Vector<T, PT>() = default;

	explicit Vector<T, PT>(size_t size) { resize(size); }

	~Vector<T, PT>() {
		if (Data) {
			if (PT == MALLOC) {
				free(Data);
			} else {
				int result = munmap(Data, Capacity);
				assert(result == 0 && "mmunmap failed");
			}
		}
	}

	Vector(Vector<T, PT> &&oth) : Size(std::exchange(oth.Size, 0)), Capacity(std::exchange(oth.Capacity, 0)), Data(std::exchange(oth.Data, nullptr)) {}

	Vector<T, PT> &operator=(Vector<T, PT> &&oth) {
		swap(*this, oth);
		return *this;
	}

	void trim(size_t size) {
		if (size * sizeof(T) < Capacity) remap(size);
	}

	void reserve(size_t size) {
		if (size * sizeof(T) > Capacity) remap(size);
	}

	void resize(size_t size) {
		if (size * sizeof(T) > Capacity) reserve(1ULL << (lambda(size) + 1));
		Size = size;
	}

	void pushBack(T elem) {
		resize(Size + 1);
		Data[Size - 1] = elem;
	}

	T popBack() { return Data[--Size]; }

	friend void swap(Vector<T, PT> &first, Vector<T, PT> &second) noexcept {
		std::swap(first.Size, second.Size);
		std::swap(first.Capacity, second.Capacity);
		std::swap(first.Data, second.Data);
	}

	inline T *get() const { return Data; }

	inline T &operator[](size_t i) const { return Data[i]; };

	inline size_t size() const { return Size; }

	inline size_t capacity() const { return Capacity; }

	size_t bitCount() const { return sizeof(Vector<T, PT>) * 8 + Capacity * 8; }

  private:
	static size_t page_aligned(size_t size) {
		if (PT == FORCEHUGEPAGE)
			return ((2 * 1024 * 1024 - 1) | (size * sizeof(T) - 1)) + 1;
		else
			return ((4 * 1024 - 1) | (size * sizeof(T) - 1)) + 1;
	}

	void remap(size_t size) {
		if (size == 0) return;

		void *mem;
		size_t space;

		if (PT == MALLOC) {
			space = size * sizeof(T);
			mem = Capacity == 0 ? malloc(space) : realloc(Data, space);
			assert(mem != NULL && "malloc failed");
		} else {
			space = page_aligned(size);
			mem = Capacity == 0 ? mmap(nullptr, space, PROT, FLAGS, -1, 0) : mremap(Data, Capacity, space, MREMAP_MAYMOVE, -1, 0);
			assert(mem != MAP_FAILED && "mmap failed");

			if (PT == TRANSHUGEPAGE) {
				int adv = madvise(mem, space, MADV_HUGEPAGE);
				assert(adv == 0 && "madvise failed");
			}
		}

		Capacity = space;
		Data = static_cast<T *>(mem);
	}

	friend std::ostream &operator<<(std::ostream &os, const Vector<T, PT> &vector) {
		const uint64_t nsize = static_cast<uint64_t>(vector.Size);
		os.write((char *)&nsize, sizeof(uint64_t));

		for (size_t i = 0; i < vector.Size; ++i) {
			const T value = vector[i];
			os.write((char *)&value, sizeof(T));
		}

		return os;
	}

	friend std::istream &operator>>(std::istream &is, Vector<T, PT> &vector) {
		uint64_t nsize;
		is.read((char *)&nsize, sizeof(uint64_t));

		vector = Vector<T, PT>(nsize);

		for (size_t i = 0; i < vector.size(); ++i) is.read((char *)&vector[i], sizeof(T));

		return is;
	}
};

} // namespace sux::util
