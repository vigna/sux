#ifndef __TEST_RANKSELECT_HPP__
#define __TEST_RANKSELECT_HPP__

#include <sux/dynamic/rankselect.hpp>

TEST(rankselect, all_ones) {
  using namespace sux;
  constexpr size_t ELEMS = 16;
  constexpr size_t BITELEMS = ELEMS * 64;
  uint64_t bitvect[ELEMS] = {UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
                             UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
                             UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX};

  ranking::Word<fenwick::FixedF> fixedf(bitvect, ELEMS);
  ranking::Word<fenwick::FixedL> fixedl(bitvect, ELEMS);
  ranking::Word<fenwick::ByteF> bytef(bitvect, ELEMS);
  ranking::Word<fenwick::ByteL> bytel(bitvect, ELEMS);
  ranking::Word<fenwick::BitF> bitf(bitvect, ELEMS);
  ranking::Word<fenwick::BitL> bitl(bitvect, ELEMS);

  // rank
  for (size_t i = 0; i <= BITELEMS; i++) {
    EXPECT_EQ(i, fixedf.rank(i)) << "at index " << i;
    EXPECT_EQ(i, fixedl.rank(i)) << "at index " << i;
    EXPECT_EQ(i, bytef.rank(i)) << "at index " << i;
    EXPECT_EQ(i, bytel.rank(i)) << "at index " << i;
    EXPECT_EQ(i, bitf.rank(i)) << "at index " << i;
    EXPECT_EQ(i, bitl.rank(i)) << "at index " << i;
  }

  // rankZero
  for (size_t i = 0; i <= BITELEMS; i++) {
    EXPECT_EQ(0, fixedf.rankZero(i)) << "at index " << i;
    EXPECT_EQ(0, fixedl.rankZero(i)) << "at index " << i;
    EXPECT_EQ(0, bytef.rankZero(i)) << "at index " << i;
    EXPECT_EQ(0, bytel.rankZero(i)) << "at index " << i;
    EXPECT_EQ(0, bitf.rankZero(i)) << "at index " << i;
    EXPECT_EQ(0, bitl.rankZero(i)) << "at index " << i;
  }

  // select
  for (size_t i = 0; i < BITELEMS; i++) {
    EXPECT_EQ(i, fixedf.select(i)) << "at index " << i;
    EXPECT_EQ(i, fixedl.select(i)) << "at index " << i;
    EXPECT_EQ(i, bytef.select(i)) << "at index " << i;
    EXPECT_EQ(i, bytel.select(i)) << "at index " << i;
    EXPECT_EQ(i, bitf.select(i)) << "at index " << i;
    EXPECT_EQ(i, bitl.select(i)) << "at index " << i;
  }

  // selectZero
  for (size_t i = 0; i < BITELEMS; i++) {
    EXPECT_EQ(SIZE_MAX, fixedf.selectZero(i)) << "at index " << i;
    EXPECT_EQ(SIZE_MAX, fixedl.selectZero(i)) << "at index " << i;
    EXPECT_EQ(SIZE_MAX, bytef.selectZero(i)) << "at index " << i;
    EXPECT_EQ(SIZE_MAX, bytel.selectZero(i)) << "at index " << i;
    EXPECT_EQ(SIZE_MAX, bitf.selectZero(i)) << "at index " << i;
    EXPECT_EQ(SIZE_MAX, bitl.selectZero(i)) << "at index " << i;
  }

  // update
  for (size_t i = 0; i < ELEMS; i++) {
    fixedf.update(i, 1);
    fixedl.update(i, 1);
    bytef.update(i, 1);
    bytel.update(i, 1);
    bitf.update(i, 1);
    bitl.update(i, 1);
  }

  // select (check update correctness)
  for (size_t i = 0; i < ELEMS; i++) {
    EXPECT_EQ(i * 64, fixedf.select(i)) << "at index " << i;
    EXPECT_EQ(i * 64, fixedl.select(i)) << "at index " << i;
    EXPECT_EQ(i * 64, bytef.select(i)) << "at index " << i;
    EXPECT_EQ(i * 64, bytel.select(i)) << "at index " << i;
    EXPECT_EQ(i * 64, bitf.select(i)) << "at index " << i;
    EXPECT_EQ(i * 64, bitl.select(i)) << "at index " << i;
  }

  // update
  for (size_t i = 0; i < ELEMS; i++) {
    fixedf.update(i, UINT64_MAX);
    fixedl.update(i, UINT64_MAX);
    bytef.update(i, UINT64_MAX);
    bytel.update(i, UINT64_MAX);
    bitf.update(i, UINT64_MAX);
    bitl.update(i, UINT64_MAX);
  }

  // rank (check update correctness)
  for (size_t i = 0; i <= BITELEMS; i++) {
    EXPECT_EQ(i, fixedf.rank(i)) << "at index " << i;
    EXPECT_EQ(i, fixedl.rank(i)) << "at index " << i;
    EXPECT_EQ(i, bytef.rank(i)) << "at index " << i;
    EXPECT_EQ(i, bytel.rank(i)) << "at index " << i;
    EXPECT_EQ(i, bitf.rank(i)) << "at index " << i;
    EXPECT_EQ(i, bitl.rank(i)) << "at index " << i;
  }
}

TEST(rankselect, all_zeroes) {
  using namespace sux;
  constexpr size_t ELEMS = 16;
  constexpr size_t BITELEMS = ELEMS * 64;
  uint64_t bitvect[ELEMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  ranking::Word<fenwick::FixedF> fixedf(bitvect, ELEMS);
  ranking::Word<fenwick::FixedL> fixedl(bitvect, ELEMS);
  ranking::Word<fenwick::ByteF> bytef(bitvect, ELEMS);
  ranking::Word<fenwick::ByteL> bytel(bitvect, ELEMS);
  ranking::Word<fenwick::BitF> bitf(bitvect, ELEMS);
  ranking::Word<fenwick::BitL> bitl(bitvect, ELEMS);

  // rank
  for (size_t i = 0; i <= BITELEMS; i++) {
    EXPECT_EQ(0, fixedf.rank(i)) << "at index " << i;
    EXPECT_EQ(0, fixedl.rank(i)) << "at index " << i;
    EXPECT_EQ(0, bytef.rank(i)) << "at index " << i;
    EXPECT_EQ(0, bytel.rank(i)) << "at index " << i;
    EXPECT_EQ(0, bitf.rank(i)) << "at index " << i;
    EXPECT_EQ(0, bitl.rank(i)) << "at index " << i;
  }

  // rankZero
  for (size_t i = 0; i <= BITELEMS; i++) {
    EXPECT_EQ(i, fixedf.rankZero(i)) << "at index " << i;
    EXPECT_EQ(i, fixedl.rankZero(i)) << "at index " << i;
    EXPECT_EQ(i, bytef.rankZero(i)) << "at index " << i;
    EXPECT_EQ(i, bytel.rankZero(i)) << "at index " << i;
    EXPECT_EQ(i, bitf.rankZero(i)) << "at index " << i;
    EXPECT_EQ(i, bitl.rankZero(i)) << "at index " << i;
  }

  // select
  for (size_t i = 0; i < BITELEMS; i++) {
    EXPECT_EQ(SIZE_MAX, fixedf.select(i)) << "at index " << i;
    EXPECT_EQ(SIZE_MAX, fixedl.select(i)) << "at index " << i;
    EXPECT_EQ(SIZE_MAX, bytef.select(i)) << "at index " << i;
    EXPECT_EQ(SIZE_MAX, bytel.select(i)) << "at index " << i;
    EXPECT_EQ(SIZE_MAX, bitf.select(i)) << "at index " << i;
    EXPECT_EQ(SIZE_MAX, bitl.select(i)) << "at index " << i;
  }

  // selectZero
  for (size_t i = 0; i < BITELEMS; i++) {
    EXPECT_EQ(i, fixedf.selectZero(i)) << "at index " << i;
    EXPECT_EQ(i, fixedl.selectZero(i)) << "at index " << i;
    EXPECT_EQ(i, bytef.selectZero(i)) << "at index " << i;
    EXPECT_EQ(i, bytel.selectZero(i)) << "at index " << i;
    EXPECT_EQ(i, bitf.selectZero(i)) << "at index " << i;
    EXPECT_EQ(i, bitl.selectZero(i)) << "at index " << i;
  }

  // update
  for (size_t i = 0; i < ELEMS; i++) {
    fixedf.update(i, 0b10);
    fixedl.update(i, 0b10);
    bytef.update(i, 0b10);
    bytel.update(i, 0b10);
    bitf.update(i, 0b10);
    bitl.update(i, 0b10);
  }

  // select (check update correctness)
  for (size_t i = 0; i < ELEMS; i++) {
    EXPECT_EQ(i * 64 + 1, fixedf.select(i)) << "at index " << i;
    EXPECT_EQ(i * 64 + 1, fixedl.select(i)) << "at index " << i;
    EXPECT_EQ(i * 64 + 1, bytef.select(i)) << "at index " << i;
    EXPECT_EQ(i * 64 + 1, bytel.select(i)) << "at index " << i;
    EXPECT_EQ(i * 64 + 1, bitf.select(i)) << "at index " << i;
    EXPECT_EQ(i * 64 + 1, bitl.select(i)) << "at index " << i;
  }

  // update
  for (size_t i = 0; i < ELEMS; i++) {
    fixedf.update(i, 0);
    fixedl.update(i, 0);
    bytef.update(i, 0);
    bytel.update(i, 0);
    bitf.update(i, 0);
    bitl.update(i, 0);
  }

  // rank (check update correctness)
  for (size_t i = 0; i <= BITELEMS; i++) {
    EXPECT_EQ(0, fixedf.rank(i)) << "at index " << i;
    EXPECT_EQ(0, fixedl.rank(i)) << "at index " << i;
    EXPECT_EQ(0, bytef.rank(i)) << "at index " << i;
    EXPECT_EQ(0, bytel.rank(i)) << "at index " << i;
    EXPECT_EQ(0, bitf.rank(i)) << "at index " << i;
    EXPECT_EQ(0, bitl.rank(i)) << "at index " << i;
  }
}

template <std::size_t S> void run_rankselect(std::size_t size) {
  using namespace sux;

  static std::mt19937 mte;
  std::uniform_int_distribution<std::uint64_t> dist(0, UINT64_MAX);

  std::uint64_t *bitvect = new std::uint64_t[size];
  std::uint64_t *updates = new std::uint64_t[size];

  for (std::size_t i = 0; i < size; i++) {
    bitvect[i] = dist(mte);
    updates[i] = dist(mte);
  }

  // word
  ranking::Word<fenwick::FixedF> fixedf(bitvect, size);
  ranking::Word<fenwick::FixedL> fixedl(bitvect, size);
  ranking::Word<fenwick::BitF> bit(bitvect, size);
  ranking::Word<fenwick::BitL> bitl(bitvect, size);
  ranking::Word<fenwick::ByteF> byte(bitvect, size);
  ranking::Word<fenwick::ByteL> bytel(bitvect, size);

  // line
  ranking::Stride<fenwick::FixedF, S> fixedfS(bitvect, size);
  ranking::Stride<fenwick::FixedL, S> fixedlS(bitvect, size);
  ranking::Stride<fenwick::BitF, S> bitS(bitvect, size);
  ranking::Stride<fenwick::BitL, S> bitlS(bitvect, size);
  ranking::Stride<fenwick::ByteF, S> byteS(bitvect, size);
  ranking::Stride<fenwick::ByteL, S> bytelS(bitvect, size);

  // rank
  for (size_t i = 0; i <= size; i++) {
    auto res = fixedf.rank(i);

    EXPECT_EQ(res, fixedl.rank(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bit.rank(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitl.rank(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, byte.rank(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bytel.rank(i)) << "at index " << i << ", stride " << S;

    EXPECT_EQ(res, fixedfS.rank(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, fixedlS.rank(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitS.rank(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitlS.rank(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, byteS.rank(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bytelS.rank(i)) << "at index " << i << ", stride " << S;
  }

  // rankZero
  for (size_t i = 0; i <= size; i++) {
    auto res = fixedf.rankZero(i);

    EXPECT_EQ(res, fixedl.rankZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bit.rankZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitl.rankZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, byte.rankZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bytel.rankZero(i)) << "at index " << i << ", stride " << S;

    EXPECT_EQ(res, fixedl.rankZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, fixedfS.rankZero(i)) << "at index " << i << "  stride " << S;
    EXPECT_EQ(res, bitS.rankZero(i)) << "at index " << i << "  stride " << S;
    EXPECT_EQ(res, bitlS.rankZero(i)) << "at index " << i << "  stride " << S;
    EXPECT_EQ(res, byteS.rankZero(i)) << "at index " << i << "  stride " << S;
    EXPECT_EQ(res, bytelS.rankZero(i)) << "at index " << i << "  stride " << S;
  }

  // select
  for (size_t i = 0; i < size; i++) {
    auto res = fixedf.select(i);

    EXPECT_EQ(res, fixedl.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bit.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitl.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, byte.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bytel.select(i)) << "at index " << i << ", stride " << S;

    EXPECT_EQ(res, fixedfS.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, fixedlS.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitS.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitlS.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, byteS.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bytelS.select(i)) << "at index " << i << ", stride " << S;
  }

  // selectZero
  for (size_t i = 0; i < size; i++) {
    auto res = fixedf.selectZero(i);

    EXPECT_EQ(res, fixedl.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bit.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitl.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, byte.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bytel.selectZero(i)) << "at index " << i << ", stride " << S;

    EXPECT_EQ(res, fixedfS.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, fixedlS.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitS.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitlS.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, byteS.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bytelS.selectZero(i)) << "at index " << i << ", stride " << S;
  }

  // update
  for (size_t i = 0; i < size; i++) {
    fixedf.update(i, updates[i]);
    fixedl.update(i, updates[i]);
    bit.update(i, updates[i]);
    bitl.update(i, updates[i]);
    byte.update(i, updates[i]);
    bytel.update(i, updates[i]);

    fixedfS.update(i, updates[i]);
    fixedlS.update(i, updates[i]);
    bitS.update(i, updates[i]);
    bitlS.update(i, updates[i]);
    byteS.update(i, updates[i]);
    bytelS.update(i, updates[i]);
  }

  // select
  for (size_t i = 0; i < size; i++) {
    auto res = fixedf.select(i);

    EXPECT_EQ(res, fixedl.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bit.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitl.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, byte.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bytel.select(i)) << "at index " << i << ", stride " << S;

    EXPECT_EQ(res, fixedfS.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, fixedlS.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitS.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitlS.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, byteS.select(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bytelS.select(i)) << "at index " << i << ", stride " << S;
  }

  // selectZero
  for (size_t i = 0; i < size; i++) {
    auto res = fixedf.selectZero(i);

    EXPECT_EQ(res, fixedl.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bit.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitl.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, byte.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bytel.selectZero(i)) << "at index " << i << ", stride " << S;

    EXPECT_EQ(res, fixedfS.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, fixedlS.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitS.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bitlS.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, byteS.selectZero(i)) << "at index " << i << ", stride " << S;
    EXPECT_EQ(res, bytelS.selectZero(i)) << "at index " << i << ", stride " << S;
  }

  delete[] updates;
  delete[] bitvect;
}

TEST(rankselect, small_large) {
  run_rankselect<8>(1);
  run_rankselect<16>(512 * 1024);
}

TEST(rankselect, stride) {
  for (size_t i = 1000; i < 2000; i += 100) {
    run_rankselect<1>(i);
    run_rankselect<2>(i);
    run_rankselect<3>(i);
    run_rankselect<4>(i);
    run_rankselect<5>(i);

    run_rankselect<8>(i);
    run_rankselect<16>(i);
    run_rankselect<32>(i);
    run_rankselect<64>(i);
    run_rankselect<128>(i);
    run_rankselect<256>(i);
    run_rankselect<512>(i);
    run_rankselect<1024>(i);
  }
}

#endif // __TEST_RANKSELECT_HPP__
