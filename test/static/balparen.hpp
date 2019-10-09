#pragma once

#include <sux/static/BalParen.hpp>

static void fill_paren(uint64_t *bits, uint64_t num_bits, double twist) {
  bits[0] = 1; // First open parenthesis
  for (int64_t i = 1, r = 0; (uint64_t)i < num_bits - 1; i++) {
    const double coeff = r * (num_bits - 1 - i + r + 2) / (2. * (num_bits - 1 - i) * (r + 1));
    assert(coeff >= 0);
    assert(coeff <= 1);

    if (next() >= UINT64_MAX * (coeff != 1 ? twist * coeff : 1)) {
      bits[i / 64] |= 1ULL << i % 64;
      r++;
    } else
      r--;
    assert(r >= 0);
  }
}

TEST(bal_paren, random) {
  using namespace sux;

  for (size_t size = 2; size <= 2048; size += 2) {
    uint64_t *bitvect = new uint64_t[(size + 63) / 64]();
    fill_paren(bitvect, size, 1);

    BalParen BalParen(bitvect, size);

    for (uint64_t i = 0; i < size; i++) {
      if (bitvect[i / 64] & 1ULL << (i & 63)) {
        int64_t c = 1;
        uint64_t j;
        for (j = i + 1; j < size; j++) {
          if (bitvect[j / 64] & 1ULL << (j & 63))
            c++;
          else
            c--;

          if (c == 0)
            break;
        }

        EXPECT_EQ(0, c);
        EXPECT_EQ(j, BalParen.findClose(i));
      }
    }

    delete[] bitvect;
  }
}
