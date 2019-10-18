#pragma once

#include <sux/bits/EliasFano.hpp>
#include <sux/bits/Rank9Sel.hpp>
#include <sux/bits/SimpleSelect.hpp>
#include <sux/bits/SimpleSelectHalf.hpp>
#include <sux/bits/SimpleSelectZero.hpp>
#include <sux/bits/SimpleSelectZeroHalf.hpp>

TEST(rankselect, all_ones) {
	using namespace sux::bits;

	for (size_t size = 0; size <= 2049; size++) {
		uint64_t *bitvect = new uint64_t[size / 64 + 1]();
		for (size_t i = 0; i < size; i++) bitvect[i / 64] |= UINT64_C(1) << i % 64;

		Rank9Sel Rank9Sel(bitvect, size);
		EliasFano EliasFano(bitvect, size);
		SimpleSelect SimpleSelect(bitvect, size, 3);
		SimpleSelectHalf SimpleSelectHalf(bitvect, size);

		// rank
		for (size_t i = 0; i <= size; i++) {
			EXPECT_EQ(i, Rank9Sel.rank(i)) << "at index " << i;
			EXPECT_EQ(i, EliasFano.rank(i)) << "at index " << i;
		}

		// rankZero
		for (size_t i = 0; i < size; i++) {
			EXPECT_EQ(0, Rank9Sel.rankZero(i)) << "at index " << i;
			EXPECT_EQ(0, EliasFano.rankZero(i)) << "at index " << i;
		}

		// select
		for (size_t i = 0; i < size; i++) {
			EXPECT_EQ(i, Rank9Sel.select(i)) << "at index " << i;
			EXPECT_EQ(i, EliasFano.select(i)) << "at index " << i;
			EXPECT_EQ(i, SimpleSelect.select(i)) << "at index " << i;
			EXPECT_EQ(i, SimpleSelectHalf.select(i)) << "at index " << i;
		}

		delete[] bitvect;
	}
}

TEST(rankselect, all_zeroes) {
	using namespace sux::bits;

	for (size_t size = 0; size <= 2049; size++) {
		uint64_t *bitvect = new uint64_t[size / 64 + 1]();

		Rank9Sel Rank9Sel(bitvect, size);
		EliasFano EliasFano(bitvect, size);
		SimpleSelectZero SimpleSelectZero(bitvect, size, 3);
		SimpleSelectZeroHalf SimpleSelectZeroHalf(bitvect, size);

		// rank
		for (size_t i = 0; i <= size; i++) {
			EXPECT_EQ(0, Rank9Sel.rank(i)) << "at index " << i;
			EXPECT_EQ(0, EliasFano.rank(i)) << "at index " << i;
		}

		// rankZero
		for (size_t i = 0; i < size; i++) {
			EXPECT_EQ(i, Rank9Sel.rankZero(i)) << "at index " << i;
			EXPECT_EQ(i, EliasFano.rankZero(i)) << "at index " << i;
		}

		// selectZero
		for (size_t i = 0; i < size; i++) {
			EXPECT_EQ(i, SimpleSelectZero.selectZero(i)) << "at index " << i;
			EXPECT_EQ(i, SimpleSelectZeroHalf.selectZero(i)) << "at index " << i;
		}

		delete[] bitvect;
	}
}

static void run_rankselect(std::size_t size) {
	using namespace sux::bits;
	const size_t words = size / 64 + 1;

	uint64_t *bitvect = new uint64_t[words];

	uint64_t ones = 0;
	for (size_t i = 0; i < (size + 63) / 64; i++) {
		bitvect[i] = next();
		if (i == (size + 63) / 64 - 1 && size % 64 != 0) bitvect[i] &= (UINT64_C(1) << size % 64) - 1;
		ones += __builtin_popcountll(bitvect[i]);
	}

	const size_t zeros = size - ones;

	Rank9Sel Rank9Sel(bitvect, size);
	EliasFano EliasFano(bitvect, size);
	SimpleSelect SimpleSelect(bitvect, size, 3); // TODO: try different LONGWORDS_PER_SUBINVENTORY
	SimpleSelectHalf SimpleSelectHalf(bitvect, size);
	SimpleSelectZero SimpleSelectZero(bitvect, size,
									  3); // TODO: try different LONGWORDS_PER_SUBINVENTORY
	SimpleSelectZeroHalf SimpleSelectZeroHalf(bitvect, size);

	// rank
	for (size_t i = 0; i < ones; i++) {
		auto pos = Rank9Sel.select(i);
		EXPECT_EQ(pos, EliasFano.select(i));
		EXPECT_EQ(i, Rank9Sel.rank(pos));
		EXPECT_EQ(i, EliasFano.rank(pos));
		pos = SimpleSelect.select(i);
		EXPECT_EQ(i, Rank9Sel.rank(pos));
		EXPECT_EQ(i, EliasFano.rank(pos));
		pos = SimpleSelectHalf.select(i);
		EXPECT_EQ(i, Rank9Sel.rank(pos));
		EXPECT_EQ(i, EliasFano.rank(pos));
	}

	// select
	auto poslim = std::min(size, Rank9Sel.select(Rank9Sel.rank(size) - 1));
	for (size_t pos = 0; pos <= poslim; pos++) {
		auto res = Rank9Sel.rank(pos);
		EXPECT_EQ(res, EliasFano.rank(pos));
		if (bitvect[pos / 64] & UINT64_C(1) << pos % 64)
			EXPECT_EQ(pos, Rank9Sel.select(res));
		else
			EXPECT_LT(pos, Rank9Sel.select(res));
		EXPECT_EQ(Rank9Sel.select(res), EliasFano.select(res));
		EXPECT_EQ(Rank9Sel.select(res), SimpleSelect.select(res));
		EXPECT_EQ(Rank9Sel.select(res), SimpleSelectHalf.select(res));
	}

	// rankZero
	for (size_t i = 0; i < zeros; i++) {
		auto pos = SimpleSelectZero.selectZero(i);
		EXPECT_EQ(pos, SimpleSelectZeroHalf.selectZero(i));
		EXPECT_EQ(i, Rank9Sel.rankZero(pos));
		EXPECT_EQ(i, EliasFano.rankZero(pos));
	}

	// selectZero
	poslim = std::min(size - 1, (size_t)SimpleSelectZero.selectZero(Rank9Sel.rankZero(size) - 1));
	for (size_t pos = 0; pos < poslim; pos++) {
		auto res = Rank9Sel.rankZero(pos);
		EXPECT_EQ(res, EliasFano.rankZero(pos));
		if (bitvect[pos / 64] & UINT64_C(1) << pos % 64)
			EXPECT_LT(pos, SimpleSelectZero.selectZero(res));
		else
			EXPECT_EQ(pos, SimpleSelectZero.selectZero(res));
		EXPECT_EQ(SimpleSelectZero.selectZero(res), SimpleSelectZeroHalf.selectZero(res));
	}

	delete[] bitvect;
}

TEST(rankselect, small_large) {
	run_rankselect(10);
	run_rankselect(64);
	run_rankselect(1000);
	run_rankselect(1024);
	run_rankselect(512 * 1024);
}
