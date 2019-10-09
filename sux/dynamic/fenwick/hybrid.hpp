#ifndef __FENWICK_HYBRID_HPP__
#define __FENWICK_HYBRID_HPP__

#include "fenwick_tree.hpp"
#include <vector>

namespace hft::fenwick {

template <template <size_t> class TOP, template <size_t> class BOTTOM, size_t BOUND, size_t CUT>
class Hybrid : public FenwickTree {
private:
  size_t BottomSize = (1ULL << CUT) - 1;
  static constexpr size_t TOPBOUND = BOUND * (1ULL << CUT);

protected:
  size_t Size;
  TOP<TOPBOUND> TopFenwick;
  std::vector<BOTTOM<BOUND>> BottomForest;

public:
  Hybrid(uint64_t sequence[], size_t size) : Size(size), TopFenwick(buildTop(sequence, size)) {
    for (size_t i = 0; i <= TopFenwick.size(); i++) {
      const size_t next = (BottomSize + 1) * i;
      const size_t bsize = BottomSize + next <= size ? BottomSize : size & BottomSize;

      BottomForest.push_back(BOTTOM<BOUND>(sequence + next, bsize));
    }

    for (size_t i = 0; i < TopFenwick.size(); i++)
      TopFenwick.add(i + 1, BottomForest[i].prefix(BottomForest[i].size()));
  }

  virtual uint64_t prefix(size_t idx) const {
    const size_t top = idx >> CUT;
    const size_t bottom = idx & BottomSize;

    return TopFenwick.prefix(top) + BottomForest[top].prefix(bottom);
  }

  virtual void add(size_t idx, int64_t inc) {
    const size_t top = idx >> CUT;
    const size_t bottom = idx & BottomSize;

    if (bottom == 0) {
      TopFenwick.add(top, inc);
    } else {
      TopFenwick.add(top + 1, inc);
      BottomForest[top].add(bottom, inc);
    }
  }

  using FenwickTree::find;
  virtual size_t find(uint64_t *val) const {
    const size_t top = TopFenwick.size() != 0 ? TopFenwick.find(val) : 0;
    const size_t bottom = top < BottomForest.size() ? BottomForest[top].find(val) : 0;

    return (top << CUT) + bottom;
  }

  using FenwickTree::compFind;
  virtual size_t compFind(uint64_t *val) const {
    const size_t top = TopFenwick.size() != 0 ? TopFenwick.compFind(val) : 0;
    const size_t bottom = top < BottomForest.size() ? BottomForest[top].compFind(val) : 0;

    return (top << CUT) + bottom;
  }

  virtual size_t size() const { return Size; }

  virtual size_t bitCount() const {
    size_t bottomSize = 0;
    for (auto &t : BottomForest)
      bottomSize += t.bitCount();

    return bottomSize + sizeof(Hybrid<BOTTOM, TOP, BOUND, CUT>) + TopFenwick.bitCount();
  }

private:
  TOP<TOPBOUND> buildTop(const uint64_t sequence[], size_t size) const {
    const size_t topsize = size >> CUT;
    const std::unique_ptr<uint64_t[]> subseq = std::make_unique<uint64_t[]>(topsize);

    for (size_t i = 1; i <= topsize; i++)
      subseq[i - 1] = sequence[i * (BottomSize + 1) - 1];

    return TOP<TOPBOUND>(subseq.get(), topsize);
  }

  friend std::ostream &operator<<(std::ostream &os, const Hybrid<TOP, BOTTOM, BOUND, CUT> &ft) {
    const uint64_t nsize = hton(ft.Size);
    os.write((char *)&nsize, sizeof(uint64_t));

    const uint64_t nbottom = hton(ft.BottomForest.size());
    os.write((char *)&nbottom, sizeof(uint64_t));

    os << ft.TopFenwick;
    for (size_t i = 0; i < nbottom; ++i)
      os << ft.BottomForest[i];

    return os;
  }

  friend std::istream &operator>>(std::istream &is, Hybrid<TOP, BOTTOM, BOUND, CUT> &ft) {
    uint64_t nsize;
    is.read((char *)(&nsize), sizeof(uint64_t));

    uint64_t nbottom;
    is.read((char *)(&nbottom), sizeof(uint64_t));

    is >> ft.TopFenwick;

    auto readNextBottom = [&]() -> BOTTOM<BOUND> {
      BOTTOM<BOUND> bottom(nullptr, 0);
      is >> bottom;
      return bottom;
    };

    ft.BottomForest.reserve(nbottom);
    for (size_t i = 0; i < nbottom; ++i)
      ft.BottomForest.push_back(readNextBottom());

    return is;
  }
};

} // namespace hft::fenwick

#endif // __FENWICK_HYBRID_HPP__
