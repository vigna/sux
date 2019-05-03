#ifndef __RANKSELECT_WORD_HPP__
#define __RANKSELECT_WORD_HPP__

#include "rank_select.hpp"

namespace hft::ranking {

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
  T<BOUNDSIZE> Fenwick;
  DArray<uint64_t> Vector;

public:
  Word(uint64_t bitvector[], size_t size)
      : Fenwick(buildFenwick(bitvector, size)), Vector(DArray<uint64_t>(size)) {
    std::copy_n(bitvector, size, Vector.get());
  }

  Word(DArray<uint64_t> bitvector, size_t size)
      : Fenwick(buildFenwick(bitvector.get(), size)),
        Vector(std::move(bitvector)) {}

  virtual const uint64_t *bitvector() const { return Vector.get(); }

  virtual size_t size() const { return Vector.size() * sizeof(uint64_t); }

  virtual uint64_t rank(size_t pos) const {
    return Fenwick.prefix(pos / 64) +
           popcount(Vector[pos / 64] & ((1ULL << (pos % 64)) - 1));
  }

  virtual uint64_t rank(size_t from, size_t to) const {
    return rank(to) - rank(from);
  }

  virtual uint64_t rankZero(size_t pos) const { return pos - rank(pos); }

  virtual uint64_t rankZero(size_t from, size_t to) const {
    return (to - from) - rank(from, to);
  }

  virtual size_t select(uint64_t rank) const {
    size_t idx = Fenwick.find(&rank);

    if (idx >= Vector.size())
      return SIZE_MAX;

    uint64_t rank_chunk = popcount(Vector[idx]);
    if (rank < rank_chunk)
      return idx * 64 + select64(Vector[idx], rank);

    return SIZE_MAX;
  }

  virtual size_t selectZero(uint64_t rank) const {
    const size_t idx = Fenwick.compFind(&rank);

    if (idx >= Vector.size())
      return SIZE_MAX;

    uint64_t rank_chunk = popcount(~Vector[idx]);
    if (rank < rank_chunk)
      return idx * 64 + select64(~Vector[idx], rank);

    return SIZE_MAX;
  }

  virtual uint64_t update(size_t index, uint64_t word) {
    uint64_t old = Vector[index];
    Vector[index] = word;
    Fenwick.add(index + 1, popcount(word) - popcount(old));

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

  virtual size_t bitCount() const {
    return sizeof(Word<T>) +
           Vector.bitCount() - sizeof(Vector) +
           Fenwick.bitCount() - sizeof(Fenwick);
  }

private:
  T<BOUNDSIZE> buildFenwick(const uint64_t bitvector[], size_t size) {
    uint64_t *sequence = new uint64_t[size];
    for (size_t i = 0; i < size; i++)
      sequence[i] = popcount(bitvector[i]);

    T<BOUNDSIZE> tree(sequence, size);
    delete[] sequence;
    return tree;
  }

  friend std::ostream &operator<<(std::ostream &os, const Word<T> &bv) {
    return os << bv.Fenwick << bv.Vector;
  }

  friend std::istream &operator>>(std::istream &is, Word<T> &bv) {
    return is >> bv.Fenwick >> bv.Vector;
  }
};

} // namespace hft::ranking

#endif // __RANKSELECT_WORD_HPP__
