/*
 * Sux: Succinct data structures
 *
 * Copyright (C) 2009-2019 Sebastiano Vigna
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

#include "../common.hpp"
#include "EliasFano.hpp"
#include "Rank9.hpp"
#include "tables.hpp"
#include <cstdint>

namespace sux {

class BalParen {
private:
  const uint64_t *bits;
  uint64_t *opening_pioneers, *opening_pioneers_bits, *opening_pioneers_matches;
  Rank9 *opening_pioneers_rank;
  uint64_t num_words;

  __inline static void set(uint64_t *const bits, const uint64_t pos) {
    bits[pos / 64] |= 1ULL << pos % 64;
  }

  __inline static int count_far_close(uint64_t word, int l) {
    int c = 0, e = 0;
    for (int i = 0; i < l; i++) {
      if ((word & 1ULL << i) != 0) {
        if (e > 0)
          e = -1;
        else
          --e;
      } else {
        if (++e > 0)
          c++;
      }
    }

    return c;
  }

  __inline static int count_far_open(uint64_t word, int l) {
    int c = 0, e = 0;
    while (l-- != 0) {
      if ((word & 1ULL << l) != 0) {
        if (++e > 0)
          c++;
      } else {
        if (e > 0)
          e = -1;
        else
          --e;
      }
    }

    return c;
  }

#if !defined(SLOW_NO_TABS) && !defined(SLOW_TABS)
  /** Finds the k-th far close parenthesis. */
  __inline static int find_far_close(const uint64_t word, int k) {
    const uint64_t b1 = (word & (0xA * ONES_STEP_4)) >> 1;
    const uint64_t b0 = word & (0x5 * ONES_STEP_4);
    const uint64_t lsb = (b1 ^ b0) & b1;

    const uint64_t open2 = (b1 & b0) << 1 | lsb;
    const uint64_t closed2 = ((b1 | b0) ^ (0x5 * ONES_STEP_4)) << 1 | lsb;

    const uint64_t open4eccess = (open2 & (0x3 * ONES_STEP_4));
    const uint64_t closed4eccess = (closed2 & (0xC * ONES_STEP_4)) >> 2;

    uint64_t open4 = ((open4eccess | MSBS_STEP_4) - closed4eccess) ^ MSBS_STEP_4;
    const uint64_t open4mask =
        ((((open4 & MSBS_STEP_4) >> 3) | MSBS_STEP_4) - ONES_STEP_4) ^ MSBS_STEP_4;

    uint64_t closed4 = ((closed4eccess | MSBS_STEP_4) - open4eccess) ^ MSBS_STEP_4;
    const uint64_t closed4mask =
        ((((closed4 & MSBS_STEP_4) >> 3) | MSBS_STEP_4) - ONES_STEP_4) ^ MSBS_STEP_4;

    open4 = ((open2 & (0xC * ONES_STEP_4)) >> 2) + (open4mask & open4);
    closed4 = (closed2 & (0x3 * ONES_STEP_4)) + (closed4mask & closed4);

    const uint64_t open8eccess = (open4 & (0xF * ONES_STEP_8));
    const uint64_t closed8eccess = (closed4 & (0xF0 * ONES_STEP_8)) >> 4;

    uint64_t open8 = ((open8eccess | MSBS_STEP_8) - closed8eccess) ^ MSBS_STEP_8;
    const uint64_t open8mask =
        ((((open8 & MSBS_STEP_8) >> 7) | MSBS_STEP_8) - ONES_STEP_8) ^ MSBS_STEP_8;

    uint64_t closed8 = ((closed8eccess | MSBS_STEP_8) - open8eccess) ^ MSBS_STEP_8;
    const uint64_t closed8mask =
        ((((closed8 & MSBS_STEP_8) >> 7) | MSBS_STEP_8) - ONES_STEP_8) ^ MSBS_STEP_8;

    open8 = ((open4 & (0xF0 * ONES_STEP_8)) >> 4) + (open8mask & open8);
    closed8 = (closed4 & (0xF * ONES_STEP_8)) + (closed8mask & closed8);

    const uint64_t open16eccess = (open8 & (0xFF * ONES_STEP_16));
    const uint64_t closed16eccess = (closed8 & (0xFF00 * ONES_STEP_16)) >> 8;

    uint64_t open16 = ((open16eccess | MSBS_STEP_16) - closed16eccess) ^ MSBS_STEP_16;
    const uint64_t open16mask =
        ((((open16 & MSBS_STEP_16) >> 15) | MSBS_STEP_16) - ONES_STEP_16) ^ MSBS_STEP_16;

    uint64_t closed16 = ((closed16eccess | MSBS_STEP_16) - open16eccess) ^ MSBS_STEP_16;
    const uint64_t closed16mask =
        ((((closed16 & MSBS_STEP_16) >> 15) | MSBS_STEP_16) - ONES_STEP_16) ^ MSBS_STEP_16;

    open16 = ((open8 & (0xFF00 * ONES_STEP_16)) >> 8) + (open16mask & open16);
    closed16 = (closed8 & (0xFF * ONES_STEP_16)) + (closed16mask & closed16);

    const uint64_t open32eccess = (open16 & 0xFFFF * ONES_STEP_32);
    const uint64_t closed32eccess = (closed16 & (0xFFFF0000ULL * ONES_STEP_32)) >> 16;

    uint64_t open32 = ((open32eccess | MSBS_STEP_32) - closed32eccess) ^ MSBS_STEP_32;
    const uint64_t open32mask =
        ((((open32 & MSBS_STEP_32) >> 31) | MSBS_STEP_32) - ONES_STEP_32) ^ MSBS_STEP_32;

    uint64_t closed32 = ((closed32eccess | MSBS_STEP_32) - open32eccess) ^ MSBS_STEP_32;
    const uint64_t closed32mask =
        ((((closed32 & MSBS_STEP_32) >> 31) | MSBS_STEP_32) - ONES_STEP_32) ^ MSBS_STEP_32;

    open32 = ((open16 & (0xFFFF0000ULL * ONES_STEP_32)) >> 16) + (open32mask & open32);
    closed32 = (closed16 & (0xFFFF * ONES_STEP_32)) + (closed32mask & closed32);

    const uint64_t check32 = ((k - (closed32 & 0xFFFFFFFFULL)) >> 63) - 1;
    uint64_t mask = check32 & 0xFFFFFFFFULL;
    k -= closed32 & mask;
    k += open32 & mask;
    int shift = (int)(32 & check32);

    const uint64_t check16 = ((k - (closed16 >> shift & 0xFFFF)) >> 63) - 1;
    mask = check16 & 0xFFFF;
    k -= closed16 >> shift & mask;
    k += open16 >> shift & mask;
    shift += 16 & check16;

    const uint64_t check8 = ((k - (closed8 >> shift & 0xFF)) >> 63) - 1;
    mask = check8 & 0xFF;
    k -= closed8 >> shift & mask;
    k += open8 >> shift & mask;
    shift += 8 & check8;

    const uint64_t check4 = ((k - (closed4 >> shift & 0xF)) >> 63) - 1;
    mask = check4 & 0xF;
    k -= closed4 >> shift & mask;
    k += open4 >> shift & mask;
    shift += 4 & check4;

    const uint64_t check2 = ((k - (closed2 >> shift & 0x3)) >> 63) - 1;
    mask = check2 & 0x3;
    k -= closed2 >> shift & mask;
    k += open2 >> shift & mask;
    shift += 2 & check2;

    return (int)(shift + k + ((word >> shift & ((k << 1) | 1)) << 1));
  }

  /*

  Slower...

  #define L (0x3830282018100800ULL+0x0202020202020202ULL)
          __inline static int find_near_close( uint64_t word ) {
                  uint64_t byte_sums = ( word << 6 ) - ( ( ( word << 6 ) & 0xa * ONES_STEP_4 ) >> 1
  ); uint64_t zeroes, update; byte_sums = ( byte_sums & 3 * ONES_STEP_4 ) + ( ( byte_sums >> 2 ) & 3
  * ONES_STEP_4 ); byte_sums = ( ( byte_sums + ( byte_sums >> 4 ) ) & 0x0f * ONES_STEP_8 ) * (
  ONES_STEP_8 << 1 ); // Twice the number of open parentheses (cumulative by byte)

                  byte_sums = ( ( MSBS_STEP_8 | byte_sums ) - L ) ^ MSBS_STEP_8; // Closed excess
  per byte

                  update = ( ~( ( ( byte_sums - ONES_STEP_8 ) & MSBS_STEP_8 ) >> 7 ) & ONES_STEP_8 )
  - ONES_STEP_8; zeroes = ( MSBS_STEP_8 | ONES_STEP_8 * 1 ) & update;

                  word >>= 2;
                  byte_sums -= ONES_STEP_8 * 2 - ( ( word & ( ONES_STEP_8 << 1 ) ) + ( word << 1 & (
  ONES_STEP_8 << 1 ) ) ); update = ( ~( ( ( ( ( byte_sums | zeroes ) - ONES_STEP_8 ) ^( byte_sums |
  zeroes ) ) ) >> 7 ) & ONES_STEP_8 ) - ONES_STEP_8; zeroes |= ( MSBS_STEP_8 | ONES_STEP_8 * 3 ) &
  update;

                  word >>= 2;
                  byte_sums -= ONES_STEP_8 * 2 - ( ( word & ( ONES_STEP_8 << 1 ) ) + ( word << 1 & (
  ONES_STEP_8 << 1 ) ) ); update = ( ~( ( ( ( ( byte_sums | zeroes  )- ONES_STEP_8 )  ^( byte_sums |
  zeroes ) ) ) >> 7 ) & ONES_STEP_8  ) - ONES_STEP_8; zeroes |= ( MSBS_STEP_8 | ONES_STEP_8 * 5 ) &
  update;

                  word >>= 2;
                  byte_sums -= ONES_STEP_8 * 2 - ( ( word & ( ONES_STEP_8 << 1 ) ) + ( word << 1 & (
  ONES_STEP_8 << 1 ) ) ); update = ( ~( ( ( ( ( byte_sums | zeroes  )- ONES_STEP_8 )  ^( byte_sums |
  zeroes ) ) ) >> 7 ) & ONES_STEP_8 ) - ONES_STEP_8; zeroes |= ( MSBS_STEP_8 | ONES_STEP_8 * 7 ) &
  update;

                  const uint64_t t = zeroes >> 7 & ONES_STEP_8;
                  int block;
                  for( block = 0; block < 64; block += 8 ) if ( zeroes & 1ULL << block ) break;
                  //const int block = msb( t & -t );
                  // A simple trick to return 127 if block < 0 (i.e., no match)
                  return ( (int)( block + ( zeroes >> block & 0x3F ) ) | ( block >> 8 ) ) & 0x7F;
          }
  */

#define L (0x4038302820181008ULL)
  __inline static int find_near_close(const uint64_t word) {
    uint64_t byte_sums = word - ((word & 0xa * ONES_STEP_4) >> 1);
    uint64_t zeroes, update;
    byte_sums = (byte_sums & 3 * ONES_STEP_4) + ((byte_sums >> 2) & 3 * ONES_STEP_4);
    byte_sums = ((byte_sums + (byte_sums >> 4)) & 0x0f * ONES_STEP_8) *
                (ONES_STEP_8 << 1); // Twice the number of open parentheses (cumulative by byte)
    byte_sums = (L | MSBS_STEP_8) - byte_sums; // Closed excess per byte

    // Set up flags for excess values that are already zero
    update = ((((byte_sums | MSBS_STEP_8) - ONES_STEP_8) >> 7 & ONES_STEP_8) | MSBS_STEP_8) -
             ONES_STEP_8;
    zeroes = (MSBS_STEP_8 >> 1 | ONES_STEP_8 * 7) & update;

    byte_sums -=
        ONES_STEP_8 * 2 - ((word >> 6 & (ONES_STEP_8 << 1)) + (word >> 5 & (ONES_STEP_8 << 1)));
    update = ((((byte_sums | MSBS_STEP_8) - ONES_STEP_8) >> 7 & ONES_STEP_8) | MSBS_STEP_8) -
             ONES_STEP_8;
    zeroes = (zeroes & ~update) | ((MSBS_STEP_8 >> 1 | ONES_STEP_8 * 5) & update);

    byte_sums -=
        ONES_STEP_8 * 2 - ((word >> 4 & (ONES_STEP_8 << 1)) + (word >> 3 & (ONES_STEP_8 << 1)));
    update = ((((byte_sums | MSBS_STEP_8) - ONES_STEP_8) >> 7 & ONES_STEP_8) | MSBS_STEP_8) -
             ONES_STEP_8;
    zeroes = (zeroes & ~update) | ((MSBS_STEP_8 >> 1 | ONES_STEP_8 * 3) & update);

    byte_sums -=
        ONES_STEP_8 * 2 - ((word >> 2 & (ONES_STEP_8 << 1)) + (word >> 1 & (ONES_STEP_8 << 1)));
    update = ((((byte_sums | MSBS_STEP_8) - ONES_STEP_8) >> 7 & ONES_STEP_8) | MSBS_STEP_8) -
             ONES_STEP_8;
    zeroes = (zeroes & ~update) | ((MSBS_STEP_8 >> 1 | ONES_STEP_8 * 1) & update);

    int block;
    for (block = 0; block < 64; block += 8)
      if (zeroes & 1ULL << block)
        break;
    // The expression block & 0x3F avoids undefined behaviour when block == 64
    return block + (zeroes >> (block & 0x3F) & 0x3F);
  }

#endif

#ifdef SLOW_NO_TABS
  __inline static int find_far_close(const uint64_t word, int k) {
    int e = 0;
    for (int i = 0; i < 64; i++) {
      if ((word & 1ULL << i) != 0) {
        if (e > 0)
          e = -1;
        else
          --e;
      } else {
        if (++e > 0 && k-- == 0)
          return i;
      }
    }

    return -1;
  }

  __inline static int find_near_close(const uint64_t word) {
    int e = 1;
    for (int i = 1; i < 64; i++) {
      if ((word & 1ULL << i) != 0)
        e++;
      else
        e--;
      if (e == 0)
        return i;
    }

    return 64;
  }
#endif

#ifdef SLOW_TABS
  __inline static int find_far_close(uint64_t word, int k) {
    int f;
    for (int i = 0; i < 64; i += 8) {
      const int b = word & 0xFF;
      if (k < 8 && (f = find_far_close8[b][k]) != 8)
        return i + f;
      k += delta_far_close[b];
      word >>= 8;
    }

    return 64;
  }

  __inline static int find_near_close(uint64_t word) {
    int e = 0, f;
    for (int i = 0; i < 64; i += 8) {
      const int b = word & 0xFF;
      if (e <= 4 && (f = find_near_close8[b][e]) != 8)
        return i + f;
      e += half_open_excess_delta[b];
      word >>= 8;
    }

    return 64;
  }
#endif

public:
  BalParen();
  BalParen(const uint64_t *const bits, const uint64_t num_bits);
  ~BalParen();
  uint64_t findClose(const uint64_t pos);
  // Just for analysis purposes
  void printCounts();
  uint64_t bitCount();
};

} // namespace sux
