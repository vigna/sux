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
		fprintf(stderr, "Usage: %s <keys> <bucket size> <mpfh>\n", argv[0]);
		return 1;
	}

	ifstream ifs;
	ifs.open(argv[1]);
	if (!ifs) {
		cerr << "Can't open file \"" << argv[1] << "\"" << endl;
		return 1;
	}
	const size_t bucket_size = strtoll(argv[2], NULL, 0);

	printf("Building...\n");
	auto begin = chrono::high_resolution_clock::now();
	RecSplit<LEAF, ALLOC_TYPE> rs(ifs, bucket_size);
	ifs.close();

	auto elapsed = chrono::duration_cast<std::chrono::nanoseconds>(chrono::high_resolution_clock::now() - begin).count();
	printf("Construction time: %.3f s, %.0f ns/key\n", elapsed * 1E-9, elapsed / (double)rs.size());

	ofstream ofs;
	ofs.exceptions(fstream::failbit | fstream::badbit);
	ofs.open(argv[3], fstream::out | fstream::binary | fstream::trunc);
	ofs << rs;
	ofs.close();

	return 0;
}
