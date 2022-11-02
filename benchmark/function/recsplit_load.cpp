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

template <typename T> void benchmark(RecSplit<LEAF, ALLOC_TYPE> &rs, const vector<T> &keys) {
	printf("Benchmarking...\n");

	uint64_t sample[SAMPLES];
	uint64_t h = 0;

	for (int k = SAMPLES; k-- != 0;) {
		auto begin = chrono::high_resolution_clock::now();
		uint64_t h = 0;
		for (size_t i = 0; i < keys.size(); i += 2) {
			h ^= rs(keys[i ^ (h & 1)]);
		}
		for (size_t i = 1; i < keys.size(); i += 2) {
			h ^= rs(keys[i ^ (h & 1)]);
		}
		auto end = chrono::high_resolution_clock::now();
		const uint64_t elapsed = chrono::duration_cast<chrono::nanoseconds>(end - begin).count();
		sample[k] = elapsed;
		printf("Elapsed: %.3fs; %.3f ns/key\n", elapsed * 1E-9, elapsed / (double)keys.size());
	}

	const volatile uint64_t unused = h;
	auto end = chrono::high_resolution_clock::now();
	sort(sample, sample + SAMPLES);
	printf("\nMedian: %.3fs; %.3f ns/key\n", sample[SAMPLES / 2] * 1E-9, sample[SAMPLES / 2] / (double)keys.size());
}

int main(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <keys> <mphf>\n", argv[0]);
		return 1;
	}

	ifstream fin(argv[1]);
	string str;
	vector<string> keys;
	while (getline(fin, str)) keys.push_back(str);
	fin.close();

	fstream fs;
	RecSplit<LEAF, ALLOC_TYPE> rs;

	fs.exceptions(fstream::failbit | fstream::badbit);
	fs.open(argv[2], std::fstream::in | std::fstream::binary);
	fs >> rs;
	fs.close();

	benchmark(rs, keys);

	return 0;
}
