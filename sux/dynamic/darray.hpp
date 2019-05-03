#ifndef __DARRAY_HPP__
#define __DARRAY_HPP__

#include "common.hpp"
#include <assert.h>
#include <iostream>
#include <sys/mman.h>

namespace hft {

/**
 * class DArray - Dinamically-allocated fixed-sized array with hugepages support
 *
 * This class is a wrapper of mmap. It supports hugepages [1], transparent hugepags [2]. It
 * guarantees to return page-aligned pointers. It has four behaviors you can choose by defining
 * their name:
 * - HFT_FORCEHUGE: every request allocs hugepages;
 * - HFT_FORCENOHUGE: every request allocs standard 4k pages (non-transparent)
 * - HFT_HUGE: every request of at least 2MB allocs hugepages while smaller use 4k pages;
 *
 * By default (without defining any of the above) every request of at least 2MB allocs 4k pages, but
 * they are transparently defragmented into hugepages by khugepaged thanks to a call to madvice.
 *
 * [1] https://www.kernel.org/doc/html/latest/admin-guide/mm/hugetlbpage.html
 * [2] https://www.kernel.org/doc/html/latest/admin-guide/mm/transhuge.html
 *
 */
template <typename T> class DArray {

public:
  static constexpr int PROT = PROT_READ | PROT_WRITE;
  static constexpr int FLAGS = MAP_PRIVATE | MAP_ANONYMOUS;

private:
  size_t Size = 0;
  T *Buffer = nullptr;

public:
  DArray<T>() = default;

  explicit DArray<T>(size_t length) : Size(length) {
    const size_t space = page_aligned(Size);
    if (space) {
#ifdef HFT_FORCE_HUGETLBPAGE
      void *mem = mmap(nullptr, space, PROT, FLAGS | MAP_HUGETLB, -1, 0);
      assert(mem != MAP_FAILED && "mmap failed");
#elif HFT_DISABLE_TRANSHUGE
      void *mem = mmap(nullptr, space, PROT, FLAGS, -1, 0);
      assert(mem != MAP_FAILED && "mmap failed");
#else
      void *mem = mmap(nullptr, space, PROT, FLAGS, -1, 0);
      assert(mem != MAP_FAILED && "mmap failed");

      int adv = madvise(mem, space, MADV_HUGEPAGE);
      assert(adv == 0 && "madvise failed");
#endif

      Buffer = static_cast<T *>(mem);
    }
  }

  DArray(DArray<T> &&oth)
      : Size(std::exchange(oth.Size, 0)), Buffer(std::exchange(oth.Buffer, nullptr)) {}

  DArray<T> &operator=(DArray<T> &&oth) {
    swap(*this, oth);
    return *this;
  }

  ~DArray<T>() {
    if (Buffer != nullptr) {
      int result = munmap(Buffer, page_aligned(Size));
      assert(result == 0 && "mmunmap failed");
    }
  }

  friend void swap(DArray<T> &first, DArray<T> &second) noexcept {
    std::swap(first.Size, second.Size);
    std::swap(first.Buffer, second.Buffer);
  }

  inline T *get() const { return Buffer; }

  inline T &operator[](size_t i) const { return Buffer[i]; };

  inline size_t size() const { return Size; }

  size_t bitCount() const { return sizeof(DArray<T>) * 8 + page_aligned(Size) * 8; }

private:
  static size_t page_aligned(size_t size) {
#ifdef HFT_FORCE_HUGETLBPAGE
    return ((2 * 1024 * 1024 - 1) | (size * sizeof(T) - 1)) + 1;
#else
    return ((4 * 1024 - 1) | (size * sizeof(T) - 1)) + 1;
#endif
  }

  friend std::ostream &operator<<(std::ostream &os, const DArray<T> &darray) {
    const uint64_t nsize = hton(static_cast<uint64_t>(darray.Size));
    os.write((char *)&nsize, sizeof(uint64_t));

    for (size_t i = 0; i < darray.Size; ++i) {
      const T value = hton(darray[i]);
      os.write((char *)&value, sizeof(T));
    }

    return os;
  }

  friend std::istream &operator>>(std::istream &is, DArray<T> &darray) {
    uint64_t nsize;
    is.read((char *)&nsize, sizeof(uint64_t));

    darray = DArray<T>(ntoh(nsize));

    for (size_t i = 0; i < darray.size(); ++i) {
      is.read((char *)&darray[i], sizeof(T));
      darray[i] = ntoh(darray[i]);
    }

    return is;
  }
}; // namespace hft

} // namespace hft

#endif // __DARRAY_HPP__
