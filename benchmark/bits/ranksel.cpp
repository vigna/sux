#include "../../test/xoroshiro128pp.hpp"
#include <cassert>
#include <chrono>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <sux/bits/EliasFano.hpp>
#include <sux/bits/Rank9Sel.hpp>
#include <sux/bits/SimpleSelect.hpp>
#include <sux/bits/SimpleSelectHalf.hpp>

using namespace std;

#define REPEATS 10

using namespace sux;
using namespace sux::bits;

int main(int argc, char *argv[]) {
	if (argc < 4) {
		fprintf(stderr, "Usage: %s NUMBITS NUMPOS DENSITY0 [DENSITY1]\n", argv[0]);
		return 0;
	}

	assert(sizeof(int) == 4);

	const uint64_t num_bits = strtoll(argv[1], NULL, 0);
#ifndef __SIZEOF_INT128__
	if (num_bits > (uint64_t(1) << 32)) {
		printf("No 128-bit integers: num_bits must be less than 2^32\n");
		return 1;
	}
#endif
	const uint64_t num_pos = strtoll(argv[2], NULL, 0);
	printf("Number of bits: %lld\n", num_bits);
	printf("Number of positions: %lld\n", num_pos);
	printf("Number of words: %lld\n", num_bits / 64);
	printf("Number of blocks: %lld\n", (num_bits / 64) / 16);
	uint64_t *const bits = (uint64_t *)calloc(num_bits / 64 + 1, sizeof *bits);

	double density0 = atof(argv[3]), density1 = argc > 4 ? atof(argv[4]) : density0;
	assert(density0 >= 0);
	assert(density0 <= 1);
	assert(density1 >= 0);
	assert(density1 <= 1);

	// Init array with given density
	const uint64_t threshold0 = (uint64_t)((UINT64_MAX)*density0), threshold1 = (uint64_t)((UINT64_MAX)*density1);

	uint64_t num_ones_first_half = 0, num_ones_second_half = 0;

	for (int64_t i = 0; i < num_bits / 2; i++)
		if (next() < threshold0) {
			num_ones_first_half++;
			bits[i / 64] |= 1LL << i % 64;
		}
	for (int64_t i = num_bits / 2; i < num_bits; i++)
		if (next() < threshold1) {
			num_ones_second_half++;
			bits[i / 64] |= 1LL << i % 64;
		}

#ifdef MAX_LOG2_LONGWORDS_PER_SUBINVENTORY
	CLASS rs(bits, num_bits, MAX_LOG2_LONGWORDS_PER_SUBINVENTORY);
#else
	CLASS rs(bits, num_bits);
#endif

	printf("Bit cost: %lld (%.2f%%)\n", rs.bitCount(), (rs.bitCount() * 100.0) / num_bits);

	uint64_t u = 0;

#ifndef NORANKTEST

	auto begin = chrono::high_resolution_clock::now();

	for (int k = REPEATS; k-- != 0;) {
		s[0] = 0x333e2c3815b27604;
		s[1] = 0x47ed6e7691d8f09f;
		for (int i = 0; i < num_pos; i++) u ^= rs.rank(remap128(next() ^ u, num_bits));
	}

	auto end = chrono::high_resolution_clock::now();
	const uint64_t elapsed = chrono::duration_cast<chrono::nanoseconds>(end - begin).count();
	const double secs = elapsed / 1E9;
	printf("%f s, %f ranks/s, %f ns/rank\n", secs, (REPEATS * num_pos) / secs, 1E9 * secs / (REPEATS * num_pos));
#endif

#ifndef NOSELECTTEST

	if (num_ones_first_half && num_ones_second_half) {
		auto begin = chrono::high_resolution_clock::now();

		for (int k = REPEATS; k-- != 0;) {
			s[0] = 0x333e2c3815b27604;
			s[1] = 0x47ed6e7691d8f09f;
			for (int i = 0; i < num_pos; i++) u ^= rs.select((u & 1) ? remap128(next(), num_ones_first_half) : num_ones_first_half + remap128(next(), num_ones_second_half));
		}

		auto end = chrono::high_resolution_clock::now();
		const uint64_t elapsed = chrono::duration_cast<chrono::nanoseconds>(end - begin).count();
		const double secs = elapsed / 1E9;
		printf("%f s, %f selects/s, %f ns/select\n", secs, (REPEATS * num_pos) / secs, 1E9 * secs / (REPEATS * num_pos));
	} else
		printf("Too few ones to measure select speed\n");
#endif

	const auto volatile unused = u;
	return 0;
}
