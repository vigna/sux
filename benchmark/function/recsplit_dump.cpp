#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sux/function/RecSplit.hpp>

using namespace std;
using namespace sux::function;

int main(int argc, char **argv) {
	if (argc < 4) {
		fprintf(stderr, "Usage: %s <keys> <bucket size> <mpfh>\n", argv[0]);
		return 1;
	}

	FILE *keys_fp = fopen(argv[1], "r");
	if (keys_fp == NULL) {
		fprintf(stderr, "Keys file %s not found\n", argv[1]);
		return 1;
	}
	const size_t bucket_size = strtoll(argv[2], NULL, 0);

	printf("Building...\n");
	auto begin = chrono::high_resolution_clock::now();
	RecSplit<LEAF, ALLOC_TYPE> rs(keys_fp, bucket_size);
	auto elapsed = chrono::duration_cast<std::chrono::nanoseconds>(chrono::high_resolution_clock::now() - begin).count();
	printf("Construction time: %.3f s, %.0f ns/key\n", elapsed * 1E-9, elapsed / (double)rs.size());

	fstream fs;
	fs.exceptions(fstream::failbit | fstream::badbit);
	fs.open(argv[3], fstream::out | fstream::binary | fstream::trunc);
	fs << rs;
	fs.close();

	return 0;
}
