#pragma once

#include <sux/util/FenwickBitF.hpp>
#include <sux/util/FenwickBitL.hpp>
#include <sux/util/FenwickByteF.hpp>
#include <sux/util/FenwickByteL.hpp>
#include <sux/util/FenwickFixedF.hpp>
#include <sux/util/FenwickFixedL.hpp>

#include <sux/bits/StrideDynRankSel.hpp>
#include <sux/bits/WordDynRankSel.hpp>

TEST(dynranksel, all_ones) {
	using namespace sux;
	constexpr size_t ELEMS = 16;
	constexpr size_t BITELEMS = ELEMS * 64;
	uint64_t bitvect[ELEMS] = {UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX,
							   UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX};

	bits::WordDynRankSel<util::FenwickFixedF> fixedf(bitvect, BITELEMS);
	bits::WordDynRankSel<util::FenwickFixedL> fixedl(bitvect, BITELEMS);
	bits::WordDynRankSel<util::FenwickByteF> bytef(bitvect, BITELEMS);
	bits::WordDynRankSel<util::FenwickByteL> bytel(bitvect, BITELEMS);
	bits::WordDynRankSel<util::FenwickBitF> bitf(bitvect, BITELEMS);
	bits::WordDynRankSel<util::FenwickBitL> bitl(bitvect, BITELEMS);

	// rank
	for (size_t i = 0; i <= BITELEMS; i++) {
		EXPECT_EQ(i, fixedf.rank(i)) << "at index " << i;
		EXPECT_EQ(i, fixedl.rank(i)) << "at index " << i;
		EXPECT_EQ(i, bytef.rank(i)) << "at index " << i;
		EXPECT_EQ(i, bytel.rank(i)) << "at index " << i;
		EXPECT_EQ(i, bitf.rank(i)) << "at index " << i;
		EXPECT_EQ(i, bitl.rank(i)) << "at index " << i;
	}

	// rankZero
	for (size_t i = 0; i <= BITELEMS; i++) {
		EXPECT_EQ(0, fixedf.rankZero(i)) << "at index " << i;
		EXPECT_EQ(0, fixedl.rankZero(i)) << "at index " << i;
		EXPECT_EQ(0, bytef.rankZero(i)) << "at index " << i;
		EXPECT_EQ(0, bytel.rankZero(i)) << "at index " << i;
		EXPECT_EQ(0, bitf.rankZero(i)) << "at index " << i;
		EXPECT_EQ(0, bitl.rankZero(i)) << "at index " << i;
	}

	// select
	for (size_t i = 0; i < BITELEMS; i++) {
		EXPECT_EQ(i, fixedf.select(i)) << "at index " << i;
		EXPECT_EQ(i, fixedl.select(i)) << "at index " << i;
		EXPECT_EQ(i, bytef.select(i)) << "at index " << i;
		EXPECT_EQ(i, bytel.select(i)) << "at index " << i;
		EXPECT_EQ(i, bitf.select(i)) << "at index " << i;
		EXPECT_EQ(i, bitl.select(i)) << "at index " << i;
	}

	// selectZero
	for (size_t i = 0; i < BITELEMS; i++) {
		EXPECT_EQ(SIZE_MAX, fixedf.selectZero(i)) << "at index " << i;
		EXPECT_EQ(SIZE_MAX, fixedl.selectZero(i)) << "at index " << i;
		EXPECT_EQ(SIZE_MAX, bytef.selectZero(i)) << "at index " << i;
		EXPECT_EQ(SIZE_MAX, bytel.selectZero(i)) << "at index " << i;
		EXPECT_EQ(SIZE_MAX, bitf.selectZero(i)) << "at index " << i;
		EXPECT_EQ(SIZE_MAX, bitl.selectZero(i)) << "at index " << i;
	}

	// update
	for (size_t i = 0; i < ELEMS; i++) {
		fixedf.update(i, 1);
		fixedl.update(i, 1);
		bytef.update(i, 1);
		bytel.update(i, 1);
		bitf.update(i, 1);
		bitl.update(i, 1);
	}

	// select (check update correctness)
	for (size_t i = 0; i < ELEMS; i++) {
		EXPECT_EQ(i * 64, fixedf.select(i)) << "at index " << i;
		EXPECT_EQ(i * 64, fixedl.select(i)) << "at index " << i;
		EXPECT_EQ(i * 64, bytef.select(i)) << "at index " << i;
		EXPECT_EQ(i * 64, bytel.select(i)) << "at index " << i;
		EXPECT_EQ(i * 64, bitf.select(i)) << "at index " << i;
		EXPECT_EQ(i * 64, bitl.select(i)) << "at index " << i;
	}

	// update
	for (size_t i = 0; i < ELEMS; i++) {
		fixedf.update(i, UINT64_MAX);
		fixedl.update(i, UINT64_MAX);
		bytef.update(i, UINT64_MAX);
		bytel.update(i, UINT64_MAX);
		bitf.update(i, UINT64_MAX);
		bitl.update(i, UINT64_MAX);
	}

	// rank (check update correctness)
	for (size_t i = 0; i <= BITELEMS; i++) {
		EXPECT_EQ(i, fixedf.rank(i)) << "at index " << i;
		EXPECT_EQ(i, fixedl.rank(i)) << "at index " << i;
		EXPECT_EQ(i, bytef.rank(i)) << "at index " << i;
		EXPECT_EQ(i, bytel.rank(i)) << "at index " << i;
		EXPECT_EQ(i, bitf.rank(i)) << "at index " << i;
		EXPECT_EQ(i, bitl.rank(i)) << "at index " << i;
	}
}

TEST(dynranksel, all_zeroes) {
	using namespace sux;
	constexpr size_t ELEMS = 16;
	constexpr size_t BITELEMS = ELEMS * 64;
	uint64_t bitvect[ELEMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	bits::WordDynRankSel<util::FenwickFixedF> fixedf(bitvect, BITELEMS);
	bits::WordDynRankSel<util::FenwickFixedL> fixedl(bitvect, BITELEMS);
	bits::WordDynRankSel<util::FenwickByteF> bytef(bitvect, BITELEMS);
	bits::WordDynRankSel<util::FenwickByteL> bytel(bitvect, BITELEMS);
	bits::WordDynRankSel<util::FenwickBitF> bitf(bitvect, BITELEMS);
	bits::WordDynRankSel<util::FenwickBitL> bitl(bitvect, BITELEMS);

	// rank
	for (size_t i = 0; i <= BITELEMS; i++) {
		EXPECT_EQ(0, fixedf.rank(i)) << "at index " << i;
		EXPECT_EQ(0, fixedl.rank(i)) << "at index " << i;
		EXPECT_EQ(0, bytef.rank(i)) << "at index " << i;
		EXPECT_EQ(0, bytel.rank(i)) << "at index " << i;
		EXPECT_EQ(0, bitf.rank(i)) << "at index " << i;
		EXPECT_EQ(0, bitl.rank(i)) << "at index " << i;
	}

	// rankZero
	for (size_t i = 0; i <= BITELEMS; i++) {
		EXPECT_EQ(i, fixedf.rankZero(i)) << "at index " << i;
		EXPECT_EQ(i, fixedl.rankZero(i)) << "at index " << i;
		EXPECT_EQ(i, bytef.rankZero(i)) << "at index " << i;
		EXPECT_EQ(i, bytel.rankZero(i)) << "at index " << i;
		EXPECT_EQ(i, bitf.rankZero(i)) << "at index " << i;
		EXPECT_EQ(i, bitl.rankZero(i)) << "at index " << i;
	}

	// select
	for (size_t i = 0; i < BITELEMS; i++) {
		EXPECT_EQ(SIZE_MAX, fixedf.select(i)) << "at index " << i;
		EXPECT_EQ(SIZE_MAX, fixedl.select(i)) << "at index " << i;
		EXPECT_EQ(SIZE_MAX, bytef.select(i)) << "at index " << i;
		EXPECT_EQ(SIZE_MAX, bytel.select(i)) << "at index " << i;
		EXPECT_EQ(SIZE_MAX, bitf.select(i)) << "at index " << i;
		EXPECT_EQ(SIZE_MAX, bitl.select(i)) << "at index " << i;
	}

	// selectZero
	for (size_t i = 0; i < BITELEMS; i++) {
		EXPECT_EQ(i, fixedf.selectZero(i)) << "at index " << i;
		EXPECT_EQ(i, fixedl.selectZero(i)) << "at index " << i;
		EXPECT_EQ(i, bytef.selectZero(i)) << "at index " << i;
		EXPECT_EQ(i, bytel.selectZero(i)) << "at index " << i;
		EXPECT_EQ(i, bitf.selectZero(i)) << "at index " << i;
		EXPECT_EQ(i, bitl.selectZero(i)) << "at index " << i;
	}

	// update
	for (size_t i = 0; i < ELEMS; i++) {
		fixedf.update(i, 0b10);
		fixedl.update(i, 0b10);
		bytef.update(i, 0b10);
		bytel.update(i, 0b10);
		bitf.update(i, 0b10);
		bitl.update(i, 0b10);
	}

	// select (check update correctness)
	for (size_t i = 0; i < ELEMS; i++) {
		EXPECT_EQ(i * 64 + 1, fixedf.select(i)) << "at index " << i;
		EXPECT_EQ(i * 64 + 1, fixedl.select(i)) << "at index " << i;
		EXPECT_EQ(i * 64 + 1, bytef.select(i)) << "at index " << i;
		EXPECT_EQ(i * 64 + 1, bytel.select(i)) << "at index " << i;
		EXPECT_EQ(i * 64 + 1, bitf.select(i)) << "at index " << i;
		EXPECT_EQ(i * 64 + 1, bitl.select(i)) << "at index " << i;
	}

	// update
	for (size_t i = 0; i < ELEMS; i++) {
		fixedf.update(i, 0);
		fixedl.update(i, 0);
		bytef.update(i, 0);
		bytel.update(i, 0);
		bitf.update(i, 0);
		bitl.update(i, 0);
	}

	// rank (check update correctness)
	for (size_t i = 0; i <= BITELEMS; i++) {
		EXPECT_EQ(0, fixedf.rank(i)) << "at index " << i;
		EXPECT_EQ(0, fixedl.rank(i)) << "at index " << i;
		EXPECT_EQ(0, bytef.rank(i)) << "at index " << i;
		EXPECT_EQ(0, bytel.rank(i)) << "at index " << i;
		EXPECT_EQ(0, bitf.rank(i)) << "at index " << i;
		EXPECT_EQ(0, bitl.rank(i)) << "at index " << i;
	}
}

template <std::size_t S> void run_dynranksel(std::size_t size) {
	using namespace sux;
	const size_t words = size / 64 + ((size % 64 != 0) ? 1 : 0);

	uint64_t *bitvect = new uint64_t[words];
	uint64_t *updates = new uint64_t[words];

	uint64_t ones = 0;
	for (size_t i = 0; i < (size + 63) / 64; i++) {
		bitvect[i] = next();
		if (i == (size + 63) / 64 - 1 && size % 64 != 0) bitvect[i] &= (UINT64_C(1) << size % 64) - 1;
		ones += __builtin_popcountll(bitvect[i]);
	}

	const size_t zeros = size - ones;

	// word
	bits::WordDynRankSel<util::FenwickFixedF> fixedf(bitvect, size);
	bits::WordDynRankSel<util::FenwickFixedL> fixedl(bitvect, size);
	bits::WordDynRankSel<util::FenwickBitF> bit(bitvect, size);
	bits::WordDynRankSel<util::FenwickBitL> bitl(bitvect, size);
	bits::WordDynRankSel<util::FenwickByteF> byte(bitvect, size);
	bits::WordDynRankSel<util::FenwickByteL> bytel(bitvect, size);

	// line
	bits::StrideDynRankSel<util::FenwickFixedF, S> fixedfS(bitvect, size);
	bits::StrideDynRankSel<util::FenwickFixedL, S> fixedlS(bitvect, size);
	bits::StrideDynRankSel<util::FenwickBitF, S> bitS(bitvect, size);
	bits::StrideDynRankSel<util::FenwickBitL, S> bitlS(bitvect, size);
	bits::StrideDynRankSel<util::FenwickByteF, S> byteS(bitvect, size);
	bits::StrideDynRankSel<util::FenwickByteL, S> bytelS(bitvect, size);

	// rank
	for (size_t i = 0; i < ones; i++) {
		auto res = fixedf.rank(i);

		auto pos = fixedf.select(i);
		EXPECT_EQ(i, fixedf.rank(pos));

		EXPECT_EQ(res, fixedl.rank(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bit.rank(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bitl.rank(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, byte.rank(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bytel.rank(i)) << "at index " << i << ", stride " << S;

		EXPECT_EQ(res, fixedfS.rank(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, fixedlS.rank(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bitS.rank(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bitlS.rank(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, byteS.rank(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bytelS.rank(i)) << "at index " << i << ", stride " << S;
	}

	// rankZero
	for (size_t i = 0; i < zeros; i++) {
		auto res = fixedf.rankZero(i);

		auto pos = fixedf.selectZero(i);
		EXPECT_EQ(i, fixedf.rankZero(pos));

		EXPECT_EQ(res, fixedl.rankZero(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bit.rankZero(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bitl.rankZero(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, byte.rankZero(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bytel.rankZero(i)) << "at index " << i << ", stride " << S;

		EXPECT_EQ(res, fixedl.rankZero(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, fixedfS.rankZero(i)) << "at index " << i << "  stride " << S;
		EXPECT_EQ(res, bitS.rankZero(i)) << "at index " << i << "  stride " << S;
		EXPECT_EQ(res, bitlS.rankZero(i)) << "at index " << i << "  stride " << S;
		EXPECT_EQ(res, byteS.rankZero(i)) << "at index " << i << "  stride " << S;
		EXPECT_EQ(res, bytelS.rankZero(i)) << "at index " << i << "  stride " << S;
	}

	// select
	auto poslim = std::min(size, fixedf.select(fixedf.rank(size) - 1));
	for (size_t pos = 0; pos <= poslim; pos++) {
		auto res = fixedf.rank(pos);

		if (bitvect[pos / 64] & UINT64_C(1) << pos % 64) {
			EXPECT_EQ(pos, fixedf.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, fixedl.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, bit.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, bitl.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, byte.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, bytel.select(res)) << "at index " << pos << ", stride " << S;

			EXPECT_EQ(pos, fixedfS.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, fixedlS.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, bitS.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, bitlS.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, byteS.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, bytelS.select(res)) << "at index " << pos << ", stride " << S;
		} else {
			EXPECT_LT(pos, fixedf.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, fixedl.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, bit.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, bitl.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, byte.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, bytel.select(res)) << "at index " << pos << ", stride " << S;

			EXPECT_LT(pos, fixedfS.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, fixedlS.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, bitS.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, bitlS.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, byteS.select(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, bytelS.select(res)) << "at index " << pos << ", stride " << S;
		}
	}

	// selectZero
	poslim = std::min(size, fixedf.selectZero(fixedf.rankZero(size) - 1));
	for (size_t pos = 0; pos <= poslim; pos++) {

		auto res = fixedf.rankZero(pos);

		if (bitvect[pos / 64] & UINT64_C(1) << pos % 64) {
			EXPECT_LT(pos, fixedf.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, fixedl.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, bit.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, bitl.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, byte.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, bytel.selectZero(res)) << "at index " << pos << ", stride " << S;

			EXPECT_LT(pos, fixedfS.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, fixedlS.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, bitS.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, bitlS.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, byteS.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_LT(pos, bytelS.selectZero(res)) << "at index " << pos << ", stride " << S;
		} else {
			EXPECT_EQ(pos, fixedf.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, fixedl.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, bit.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, bitl.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, byte.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, bytel.selectZero(res)) << "at index " << pos << ", stride " << S;

			EXPECT_EQ(pos, fixedfS.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, fixedlS.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, bitS.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, bitlS.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, byteS.selectZero(res)) << "at index " << pos << ", stride " << S;
			EXPECT_EQ(pos, bytelS.selectZero(res)) << "at index " << pos << ", stride " << S;
		}
	}

	// update
	for (size_t i = 0; i < words; i++) {
		fixedf.update(i, updates[i]);
		fixedl.update(i, updates[i]);
		bit.update(i, updates[i]);
		bitl.update(i, updates[i]);
		byte.update(i, updates[i]);
		bytel.update(i, updates[i]);

		fixedfS.update(i, updates[i]);
		fixedlS.update(i, updates[i]);
		bitS.update(i, updates[i]);
		bitlS.update(i, updates[i]);
		byteS.update(i, updates[i]);
		bytelS.update(i, updates[i]);
	}

	// select
	for (size_t i = 0; i < size; i++) {
		auto res = fixedf.select(i);

		EXPECT_EQ(res, fixedl.select(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bit.select(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bitl.select(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, byte.select(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bytel.select(i)) << "at index " << i << ", stride " << S;

		EXPECT_EQ(res, fixedfS.select(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, fixedlS.select(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bitS.select(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bitlS.select(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, byteS.select(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bytelS.select(i)) << "at index " << i << ", stride " << S;
	}

	// selectZero
	for (size_t i = 0; i < size; i++) {
		auto res = fixedf.selectZero(i);

		EXPECT_EQ(res, fixedl.selectZero(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bit.selectZero(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bitl.selectZero(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, byte.selectZero(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bytel.selectZero(i)) << "at index " << i << ", stride " << S;

		EXPECT_EQ(res, fixedfS.selectZero(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, fixedlS.selectZero(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bitS.selectZero(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bitlS.selectZero(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, byteS.selectZero(i)) << "at index " << i << ", stride " << S;
		EXPECT_EQ(res, bytelS.selectZero(i)) << "at index " << i << ", stride " << S;
	}

	delete[] updates;
	delete[] bitvect;
}

TEST(dynranksel, small_large) {
	run_dynranksel<8>(1);
	run_dynranksel<16>(512 * 1024);
}

TEST(dynranksel, stride) {
	for (size_t i = 1000; i < 2000; i += 100) {
		run_dynranksel<1>(i);
		run_dynranksel<2>(i);
		run_dynranksel<3>(i);
		run_dynranksel<4>(i);
		run_dynranksel<5>(i);

		run_dynranksel<8>(i);
		run_dynranksel<16>(i);
		run_dynranksel<32>(i);
		run_dynranksel<64>(i);
		run_dynranksel<128>(i);
		run_dynranksel<256>(i);
		run_dynranksel<512>(i);
		run_dynranksel<1024>(i);
	}
}
