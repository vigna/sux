#include "../../test/xoroshiro128pp.hpp"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <utility>
#include <sux/function/RecSplit.hpp>

#define LEAF 4
#define ALLOC_TYPE sux::util::AllocType::MALLOC
#define SAMPLES (11)

using namespace std;
using namespace sux::function;

void benchmark(RecSplit<LEAF, ALLOC_TYPE> &rs, const uint64_t n) {
	printf("Benchmarking...\n");

	uint64_t sample[SAMPLES];
	uint64_t h = 0;

	for (int k = SAMPLES; k-- != 0;) {
		s[0] = 0x5603141978c51071;
		s[1] = 0x3bbddc01ebdf4b72;
		auto begin = chrono::high_resolution_clock::now();
		for (uint64_t i = 0; i < n; i++) h ^= rs(hash128_t(next(), next() ^ h));
		auto end = chrono::high_resolution_clock::now();
		const uint64_t elapsed = chrono::duration_cast<chrono::nanoseconds>(end - begin).count();
		sample[k] = elapsed;
		printf("Elapsed: %.3fs; %.3f ns/key\n", elapsed * 1E-9, elapsed / (double)n);
	}

	const volatile uint64_t unused = h;
	sort(sample, sample + SAMPLES);
	printf("\nMedian: %.3fs; %.3f ns/key\n", sample[SAMPLES / 2] * 1E-9, sample[SAMPLES / 2] / (double)n);
}

int main(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <n> <mphf>\n", argv[0]);
		return 1;
	}

	const uint64_t n = strtoll(argv[1], NULL, 0);

	fstream fs;
	RecSplit<LEAF, ALLOC_TYPE> rs;

	fs.exceptions(fstream::failbit | fstream::badbit);
	fs.open(argv[2], fstream::in | fstream::binary);
	fs >> rs;
	fs.close();

	benchmark(rs, n);

	return 0;
}
