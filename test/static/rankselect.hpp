#pragma once

#include <sux/static/rank9sel.hpp>
#include <sux/static/simple_select_zero.hpp>

TEST(rankselect, all_ones) {
  using namespace sux;
  constexpr size_t ELEMS = 16;
  constexpr size_t BITELEMS = ELEMS * 64;
  uint64_t bitvect[ELEMS] = {UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
                             UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
                             UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX};

  Rank9Sel rank9sel(bitvect, ELEMS);

  // rank
  for (size_t i = 0; i <= BITELEMS; i++) {
    EXPECT_EQ(i, rank9sel.rank(i)) << "at index " << i;
  }

  // rankZero
  for (size_t i = 0; i <= BITELEMS; i++) {
    EXPECT_EQ(0, rank9sel.rankZero(i)) << "at index " << i;
  }

  // select
  for (size_t i = 0; i < BITELEMS; i++) {
    EXPECT_EQ(i, rank9sel.select(i)) << "at index " << i;
  }

  // selectZero
  for (size_t i = 0; i < BITELEMS; i++) {
  }
}

TEST(rankselect, all_zeroes) {
  using namespace sux;
  constexpr size_t ELEMS = 16;
  constexpr size_t BITELEMS = ELEMS * 64;
  uint64_t bitvect[ELEMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  Rank9Sel rank9sel(bitvect, ELEMS);

  // rank
  for (size_t i = 0; i <= BITELEMS; i++) {
    EXPECT_EQ(0, rank9sel.rank(i)) << "at index " << i;
  }

  // rankZero
  for (size_t i = 0; i <= BITELEMS; i++) {
    EXPECT_EQ(1, rank9sel.rankZero(i)) << "at index " << i;
  }

  // select
  for (size_t i = 0; i < BITELEMS; i++) {
    EXPECT_EQ(-1, rank9sel.select(i)) << "at index " << i;
  }

  // selectZero
  for (size_t i = 0; i < BITELEMS; i++) {
  }
}

static void run_rankselect(std::size_t size) {
  using namespace sux;

  std::uint64_t *bitvect = new std::uint64_t[size];

  uint64_t ones = 0, zeros = 0;
  for (std::size_t i = 0; i < size; i++) {
    bitvect[i] = next();
    ones += __builtin_popcountll(bitvect[i]);
    zeros += 64 - ones;
  }

  Rank9Sel rank9sel(bitvect, size);
  simple_select_zero simple_select_zero(bitvect, size, 3); // TODO: try different LONGWORDS_PER_SUBINVENTORY

  // rank
  for (size_t i = 0; i < ones; i++) {
    auto pos = rank9sel.select(i);
    EXPECT_EQ(i, rank9sel.rank(pos));
  }

  // select
  for (size_t pos = 0; pos < size * sizeof(uint64_t); pos++) {
    auto res = rank9sel.rank(pos);
    if (bitvect[pos / 64] & UINT64_C(1) << pos % 64) EXPECT_EQ(pos, rank9sel.select(res));
    else EXPECT_LT(pos, rank9sel.select(res));
  }

  // rankZero
  for (size_t i = 0; i < zeros; i++) {
    auto pos = simple_select_zero.select_zero(i);
    EXPECT_EQ(i, rank9sel.rankZero(pos));
  }

  // select
  for (size_t pos = 0; pos < size * sizeof(uint64_t); pos++) {
    auto res = rank9sel.rankZero(pos);
    if (bitvect[pos / 64] & UINT64_C(1) << pos % 64) EXPECT_LT(pos, simple_select_zero.select_zero(res));
    else EXPECT_EQ(pos, simple_select_zero.select_zero(res));
  }

  delete[] bitvect;
}

TEST(rankselect, small_large) {
  run_rankselect(1);
  run_rankselect(1000);
  run_rankselect(512 * 1024);
}
