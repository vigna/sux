#include "../../test/xoroshiro128pp.hpp"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <utility>
#include <sux/function/RecSplit.hpp>

#define LEAF 4
#define ALLOC_TYPE sux::util::AllocType::MALLOC

using namespace std;
using namespace sux::function;

int main(int argc, char **argv) {
	if (argc < 4) {
		fprintf(stderr, "Usage: %s <n> <bucket size> <mphf>\n", argv[0]);
		return 1;
	}

	const uint64_t n = strtoll(argv[1], NULL, 0);
	const size_t bucket_size = strtoll(argv[2], NULL, 0);
	std::vector<hash128_t> keys;
	for (uint64_t i = 0; i < n; i++) keys.push_back(hash128_t(next(), next()));

	printf("Building...\n");
	auto begin = chrono::high_resolution_clock::now();
	RecSplit<LEAF, ALLOC_TYPE> rs(keys, bucket_size);
	auto elapsed = chrono::duration_cast<std::chrono::nanoseconds>(chrono::high_resolution_clock::now() - begin).count();
	printf("Construction time: %.3f s, %.0f ns/key\n", elapsed * 1E-9, elapsed / (double)n);

	fstream fs;
	fs.exceptions(fstream::failbit | fstream::badbit);
	fs.open(argv[3], fstream::out | fstream::binary | fstream::trunc);
	fs << rs;
	fs.close();

	return 0;
}
