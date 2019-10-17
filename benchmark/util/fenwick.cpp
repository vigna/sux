#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <sux/util/FenwickBitF.hpp>
#include <sux/util/FenwickBitL.hpp>
#include <sux/util/FenwickByteF.hpp>
#include <sux/util/FenwickByteL.hpp>
#include <sux/util/FenwickFixedF.hpp>
#include <sux/util/FenwickFixedL.hpp>

#include "../../test/xoroshiro128pp.hpp"

using namespace std;
using namespace sux;
using namespace sux::util;

template <template <size_t, AllocType> class FENWICK, size_t BOUND, AllocType AT> void runall(const char *name, size_t size, size_t queries) {
	uint64_t u = 0;
	const double c = 1. / queries;

	cout << name << endl;
	FENWICK<BOUND, AT> fenwick;

	// fenwick.reserve(size); // pushing becomes inexpensive

	cout << "push: " << flush;
	auto begin = chrono::high_resolution_clock::now();
	for (size_t i = 0; i < size; i++) fenwick.push(next() % (BOUND + 1));
	auto end = chrono::high_resolution_clock::now();
	cout << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() / (double)size << " ns/item" << endl;

	cout << "prefix: " << flush;
	begin = chrono::high_resolution_clock::now();
	for (uint64_t i = 0; i < queries; ++i) u ^= fenwick.prefix(1 + (next() ^ (u & 1)) % size);
	end = chrono::high_resolution_clock::now();
	cout << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() * c << " ns/item" << endl;

	cout << "find: " << flush;
	begin = chrono::high_resolution_clock::now();
	for (size_t i = 0; i < queries; i++) u ^= fenwick.find((next() ^ (u & 1)) % ((BOUND + 1) * size));
	end = chrono::high_resolution_clock::now();
	cout << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() * c << " ns/item" << endl;

	cout << "add: " << flush;
	begin = chrono::high_resolution_clock::now();
	for (size_t i = 0; i < queries; i++) fenwick.add(1 + (next() ^ (u & 1)) % size, next() % (BOUND + 1));
	end = chrono::high_resolution_clock::now();
	cout << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() * c << " ns/item" << endl;

	cout << "space: " << fenwick.bitCount() / (double)size << " b/item\n";

	// The add cannot be erased by the compiler
	u ^= fenwick.prefix(1 + next() % size);
	const volatile uint64_t __attribute__((unused)) unused = u;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		cerr << "Not enough parameters: <size> <queries>\n";
		return -1;
	}

#ifdef SET_BOUND
	static constexpr size_t B = SET_BOUND;
#else
	static constexpr size_t B = 64;
#endif

#ifdef SET_ALLOC
	static constexpr AllocType AT = SET_ALLOC;
#else
	static constexpr AllocType = MALLOC;
#endif

	size_t size = stoul(argv[1]);
	size_t queries = stoul(argv[2]);

	cout << "Performing " << queries << " queries over " << size << " elements\n";
	cout << "The bound is " << SET_BOUND << " and allocations use " << STRINGIFY(SET_ALLOC) << "\n";

	runall<FenwickFixedF, B, AT>("\nFenwickFixedF", size, queries);
	runall<FenwickFixedL, B, AT>("\nFenwickFixedL", size, queries);
	runall<FenwickByteF, B, AT>("\nFenwickByteF", size, queries);
	runall<FenwickByteL, B, AT>("\nFenwickByteL", size, queries);
	runall<FenwickBitF, B, AT>("\nFenwickBitF", size, queries);
	runall<FenwickBitL, B, AT>("\nFenwickBitL", size, queries);

	return 0;
}
