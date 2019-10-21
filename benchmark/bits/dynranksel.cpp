#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <sux/util/Vector.hpp>

#include <sux/util/FenwickBitF.hpp>
#include <sux/util/FenwickBitL.hpp>
#include <sux/util/FenwickByteF.hpp>
#include <sux/util/FenwickByteL.hpp>
#include <sux/util/FenwickFixedF.hpp>
#include <sux/util/FenwickFixedL.hpp>

#include <sux/bits/StrideDynRankSel.hpp>
#include <sux/bits/WordDynRankSel.hpp>

#include "../../test/xoroshiro128pp.hpp"

using namespace std;
using namespace sux;
using namespace sux::util;
using namespace sux::bits;

template <class RANKSEL, AllocType AT> void runall(std::string name, size_t size, size_t queries) {
	uint64_t u = 0;
	const double c = 1. / queries;

	Vector<uint64_t, AT> bitvect((size + 63) / 64);

	uint64_t ones = 0;
	for (size_t i = 0; i < (size + 63) / 64; i++) {
		bitvect[i] = next();
		if (i == (size + 63) / 64 - 1 && size % 64 != 0) bitvect[i] &= (UINT64_C(1) << size % 64) - 1;
		ones += __builtin_popcountll(bitvect[i]);
	}

	size_t zeros = size - ones;

	cout << name << endl;
	RANKSEL bv(&bitvect, size);

	// fenwick.reserve(size); // push becomes much faster

	cout << "rank: " << flush;
	auto begin = chrono::high_resolution_clock::now();
	for (size_t i = 0; i < queries; i++) u ^= bv.rank((next() % (size + 1)) ^ (u & 1));
	auto end = chrono::high_resolution_clock::now();
	cout << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() * c << " ns/item" << endl;

	cout << "select: " << flush;
	begin = chrono::high_resolution_clock::now();
	for (uint64_t i = 0; i < queries; ++i) u ^= bv.select((next() ^ (u & 1)) % ones);
	end = chrono::high_resolution_clock::now();
	cout << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() * c << " ns/item" << endl;

	cout << "toggle: " << flush;
	begin = chrono::high_resolution_clock::now();
	for (size_t i = 0; i < queries; i++) u ^= bv.toggle((next() ^ (u & 1)) % size);
	end = chrono::high_resolution_clock::now();
	cout << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() * c << " ns/item" << endl;

	cout << "space: " << bv.bitCount() / (double)size << " b/item\n";

	const volatile uint64_t __attribute__((unused)) unused = u;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		cerr << "Not enough parameters: <size> <queries>\n";
		return -1;
	}

#ifdef SET_ALLOC
	static constexpr AllocType AT = SET_ALLOC;
#else
	static constexpr AllocType = MALLOC;
#endif

	size_t size = stoul(argv[1]);
	size_t queries = stoul(argv[2]);

	cout << "Performing " << queries << " queries over " << size << " bits\n";

#ifdef SET_STRIDE
	runall<StrideDynRankSel<FenwickFixedF, SET_STRIDE, AT>, AT>(std::string("\nStrideDynRankSel of ") + STRINGIFY(SET_STRIDE) + "through FenwickFixedF", size, queries);
	runall<StrideDynRankSel<FenwickFixedL, SET_STRIDE, AT>, AT>(std::string("\nStrideDynRankSel of ") + STRINGIFY(SET_STRIDE) + "through FenwickFixedL", size, queries);
	runall<StrideDynRankSel<FenwickByteF, SET_STRIDE, AT>, AT>(std::string("\nStrideDynRankSel of ") + STRINGIFY(SET_STRIDE) + "through FenwickByteF", size, queries);
	runall<StrideDynRankSel<FenwickByteL, SET_STRIDE, AT>, AT>(std::string("\nStrideDynRankSel of ") + STRINGIFY(SET_STRIDE) + "through FenwickByteL", size, queries);
	runall<StrideDynRankSel<FenwickBitF, SET_STRIDE, AT>, AT>(std::string("\nStrideDynRankSel of ") + STRINGIFY(SET_STRIDE) + "through FenwickBitF", size, queries);
	runall<StrideDynRankSel<FenwickBitL, SET_STRIDE, AT>, AT>(std::string("\nStrideDynRankSel of ") + STRINGIFY(SET_STRIDE) + "through FenwickBitL", size, queries);
#else
	runall<WordDynRankSel<FenwickFixedF, AT>, AT>(std::string("\nWordDynRankSel through FenwickFixedF"), size, queries);
	runall<WordDynRankSel<FenwickFixedL, AT>, AT>(std::string("\nWordDynRankSel through FenwickFixedL"), size, queries);
	runall<WordDynRankSel<FenwickByteF, AT>, AT>(std::string("\nWordDynRankSel through FenwickByteF"), size, queries);
	runall<WordDynRankSel<FenwickByteL, AT>, AT>(std::string("\nWordDynRankSel through FenwickByteL"), size, queries);
	runall<WordDynRankSel<FenwickBitF, AT>, AT>(std::string("\nWordDynRankSel through FenwickBitF"), size, queries);
	runall<WordDynRankSel<FenwickBitL, AT>, AT>(std::string("\nWordDynRankSel through FenwickBitL"), size, queries);
#endif

	return 0;
}
