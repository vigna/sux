#pragma once

#include <cmath>
#include <sux/dynamic/fenwick.hpp>

template <std::size_t S> void run_fenwick(std::size_t size) {
  using namespace sux::fenwick;

  std::uint64_t *increments = new std::uint64_t[size];
  std::int64_t *add_updates = new std::int64_t[size];

  for (std::size_t i = 0; i < size; i++) {
    increments[i] = next() % S;
    add_updates[i] = next() % (S - increments[i]);
  }

  FixedF<S> fixedf(increments, size);
  FixedL<S> fixedl(increments, size);
  ByteF<S> bytef(increments, size);
  ByteL<S> bytel(increments, size);
  BitF<S> bitf(increments, size);
  BitL<S> bitl(increments, size);

  // prefix
  for (size_t i = 0; i <= size; ++i) {
    std::uint64_t res = fixedf.prefix(i);

    EXPECT_EQ(res, fixedl.prefix(i)) << "at index " << i << ", size " << size << ", bound " << S;
    EXPECT_EQ(res, bytef.prefix(i)) << "at index " << i << ", size " << size << ", bound " << S;
    EXPECT_EQ(res, bytel.prefix(i)) << "at index " << i << ", size " << size << ", bound " << S;
    EXPECT_EQ(res, bitf.prefix(i)) << "at index " << i << ", size " << size << ", bound " << S;
    EXPECT_EQ(res, bitl.prefix(i)) << "at index " << i << ", size " << size << ", bound " << S;
  }

  // find
  for (std::uint64_t i = 0; i <= size; ++i) {
    std::uint64_t res = fixedf.find(i);

    EXPECT_EQ(res, fixedl.find(i)) << "at index " << i << ", size " << size << ", bound: " << S;
    EXPECT_EQ(res, bytef.find(i)) << "at index " << i << ", size " << size << ", bound: " << S;
    EXPECT_EQ(res, bytel.find(i)) << "at index " << i << ", size " << size << ", bound: " << S;
    EXPECT_EQ(res, bitf.find(i)) << "at index " << i << ", size " << size << ", bound: " << S;
    EXPECT_EQ(res, bitl.find(i)) << "at index " << i << ", size " << size << ", bound: " << S;
  }

  // add
  for (size_t i = 0; i < size; ++i) {
    fixedf.add(i + 1, add_updates[i]);
    fixedl.add(i + 1, add_updates[i]);
    bytef.add(i + 1, add_updates[i]);
    bytel.add(i + 1, add_updates[i]);
    bitf.add(i + 1, add_updates[i]);
    bitl.add(i + 1, add_updates[i]);
  }

  // post add prefix (check add correctness)
  for (size_t i = 0; i <= size; ++i) {
    std::uint64_t res = fixedf.prefix(i);

    EXPECT_EQ(res, fixedl.prefix(i)) << "at index " << i << ", size " << size << ", bound " << S;
    EXPECT_EQ(res, bytef.prefix(i)) << "at index " << i << ", size " << size << ", bound " << S;
    EXPECT_EQ(res, bytel.prefix(i)) << "at index " << i << ", size " << size << ", bound " << S;
    EXPECT_EQ(res, bitf.prefix(i)) << "at index " << i << ", size " << size << ", bound " << S;
    EXPECT_EQ(res, bitl.prefix(i)) << "at index " << i << ", size " << size << ", bound " << S;
  }

  // find complement
  for (std::uint64_t i = 0; i <= size; ++i) {
    std::uint64_t res = fixedf.compFind(i);

    EXPECT_EQ(res, fixedl.compFind(i)) << "atresx " << i << ", size " << size << ", bound " << S;
    EXPECT_EQ(res, bytef.compFind(i)) << "at index " << i << ", size " << size << ", bound " << S;
    EXPECT_EQ(res, bytel.compFind(i)) << "at index " << i << ", size " << size << ", bound " << S;
    EXPECT_EQ(res, bitf.compFind(i)) << "at index " << i << ", size " << size << ", bound " << S;
    EXPECT_EQ(res, bitl.compFind(i)) << "at index " << i << ", size " << size << ", bound " << S;
  }

  delete[] increments;
  delete[] add_updates;
}

TEST(fenwick, perfect) {
  for (std::size_t i = 1; i < 10; ++i)
    run_fenwick<64>(std::pow(2, i) - 1);
}

TEST(fenwick, partial) {
  for (std::size_t i = 1; i < 1000; ++i)
    run_fenwick<64>(i);
}

TEST(fenwick, bound) {
  run_fenwick<1>(1023);
  run_fenwick<2>(1023);
  run_fenwick<3>(1023);
  run_fenwick<4>(1023);
  run_fenwick<5>(1023);

  run_fenwick<256>(1023);             // 2^8
  run_fenwick<65536>(1023);           // 2^16
  run_fenwick<16777216>(1023);        // 2^24
  run_fenwick<4294967296>(1023);      // 2^32
  run_fenwick<1099511627776>(1023);   // 2^40
  run_fenwick<281474976710656>(1023); // 2^48

  run_fenwick<562949953421312>(1023);  // 2^49
  run_fenwick<1125899906842624>(1023); // 2^50
  run_fenwick<2251799813685248>(1023); // 2^51
  run_fenwick<4503599627370496>(1023); // 2^52
  run_fenwick<9007199254740992>(1023); // 2^53

  run_fenwick<18014398509481984>(511); // 2^54
  run_fenwick<36028797018963967>(511); // 2^55 - 1

  // Note: BOUND >= 2^55 is not supported in BitF
}
