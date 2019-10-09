#pragma once

#include <cstdint>

static inline uint64_t rotl(const uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }

static uint64_t s[2] = {0x333e2c3815b27604, 0x47ed6e7691d8f09f};

static uint64_t next(void) {
  const uint64_t s0 = s[0];
  uint64_t s1 = s[1];
  const uint64_t result = rotl(s0 + s1, 17) + s0;

  s1 ^= s0;
  s[0] = rotl(s0, 49) ^ s1 ^ (s1 << 21); // a, b
  s[1] = rotl(s1, 28);                   // c

  return result;
}
