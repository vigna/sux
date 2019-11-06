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

	for (size_t size = 0; size <= 2049; size++) {
		uint64_t *bvfixedf = new uint64_t[size / 64 + 1]();
		uint64_t *bvfixedl = new uint64_t[size / 64 + 1]();
		uint64_t *bvbytef = new uint64_t[size / 64 + 1]();
		uint64_t *bvbytel = new uint64_t[size / 64 + 1]();
		uint64_t *bvbitf = new uint64_t[size / 64 + 1]();
		uint64_t *bvbitl = new uint64_t[size / 64 + 1]();

		for (size_t i = 0; i < size; i++) bvfixedf[i / 64] |= UINT64_C(1) << i % 64;
		memcpy(bvfixedl, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));
		memcpy(bvbytef, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));
		memcpy(bvbytel, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));
		memcpy(bvbitf, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));
		memcpy(bvbitl, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));

		bits::WordDynRankSel<util::FenwickFixedF> fixedf(bvfixedf, size);
		bits::WordDynRankSel<util::FenwickFixedL> fixedl(bvfixedl, size);
		bits::WordDynRankSel<util::FenwickByteF> bytef(bvbytef, size);
		bits::WordDynRankSel<util::FenwickByteL> bytel(bvbytel, size);
		bits::WordDynRankSel<util::FenwickBitF> bitf(bvbitf, size);
		bits::WordDynRankSel<util::FenwickBitL> bitl(bvbitl, size);

		// rank
		for (size_t i = 0; i <= size; i++) {
			EXPECT_EQ(i, fixedf.rank(i)) << "at index " << i;
			EXPECT_EQ(i, fixedl.rank(i)) << "at index " << i;
			EXPECT_EQ(i, bytef.rank(i)) << "at index " << i;
			EXPECT_EQ(i, bytel.rank(i)) << "at index " << i;
			EXPECT_EQ(i, bitf.rank(i)) << "at index " << i;
			EXPECT_EQ(i, bitl.rank(i)) << "at index " << i;
		}

		// rankZero
		for (size_t i = 0; i <= size; i++) {
			EXPECT_EQ(0, fixedf.rankZero(i)) << "at index " << i;
			EXPECT_EQ(0, fixedl.rankZero(i)) << "at index " << i;
			EXPECT_EQ(0, bytef.rankZero(i)) << "at index " << i;
			EXPECT_EQ(0, bytel.rankZero(i)) << "at index " << i;
			EXPECT_EQ(0, bitf.rankZero(i)) << "at index " << i;
			EXPECT_EQ(0, bitl.rankZero(i)) << "at index " << i;
		}

		// select
		for (size_t i = 0; i < size; i++) {
			EXPECT_EQ(i, fixedf.select(i)) << "at index " << i;
			EXPECT_EQ(i, fixedl.select(i)) << "at index " << i;
			EXPECT_EQ(i, bytef.select(i)) << "at index " << i;
			EXPECT_EQ(i, bytel.select(i)) << "at index " << i;
			EXPECT_EQ(i, bitf.select(i)) << "at index " << i;
			EXPECT_EQ(i, bitl.select(i)) << "at index " << i;
		}

		// update
		for (size_t i = 0; i < size; i++) {
			fixedf.clear(i);
			fixedl.clear(i);
			bytef.clear(i);
			bytel.clear(i);
			bitf.clear(i);
			bitl.clear(i);
		}

		// rankZero (check update correctness)
		for (size_t i = 0; i < size; i++) {
			EXPECT_EQ(i, fixedf.rankZero(i)) << "at index " << i;
			EXPECT_EQ(i, fixedl.rankZero(i)) << "at index " << i;
			EXPECT_EQ(i, bytef.rankZero(i)) << "at index " << i;
			EXPECT_EQ(i, bytel.rankZero(i)) << "at index " << i;
			EXPECT_EQ(i, bitf.rankZero(i)) << "at index " << i;
			EXPECT_EQ(i, bitl.rankZero(i)) << "at index " << i;
		}

		delete[] bvbitl;
		delete[] bvbitf;
		delete[] bvbytel;
		delete[] bvbytef;
		delete[] bvfixedl;
		delete[] bvfixedf;
	}
}

TEST(dynranksel, all_zeroes) {
	using namespace sux;

	for (size_t size = 0; size <= 2049; size++) {
		uint64_t *bvfixedf = new uint64_t[size / 64 + 1]();
		uint64_t *bvfixedl = new uint64_t[size / 64 + 1]();
		uint64_t *bvbytef = new uint64_t[size / 64 + 1]();
		uint64_t *bvbytel = new uint64_t[size / 64 + 1]();
		uint64_t *bvbitf = new uint64_t[size / 64 + 1]();
		uint64_t *bvbitl = new uint64_t[size / 64 + 1]();

		bits::WordDynRankSel<util::FenwickFixedF> fixedf(bvfixedf, size);
		bits::WordDynRankSel<util::FenwickFixedL> fixedl(bvfixedl, size);
		bits::WordDynRankSel<util::FenwickByteF> bytef(bvbytef, size);
		bits::WordDynRankSel<util::FenwickByteL> bytel(bvbytel, size);
		bits::WordDynRankSel<util::FenwickBitF> bitf(bvbitf, size);
		bits::WordDynRankSel<util::FenwickBitL> bitl(bvbitl, size);

		// rank
		for (size_t i = 0; i <= size; i++) {
			EXPECT_EQ(0, fixedf.rank(i)) << "at index " << i;
			EXPECT_EQ(0, fixedl.rank(i)) << "at index " << i;
			EXPECT_EQ(0, bytef.rank(i)) << "at index " << i;
			EXPECT_EQ(0, bytel.rank(i)) << "at index " << i;
			EXPECT_EQ(0, bitf.rank(i)) << "at index " << i;
			EXPECT_EQ(0, bitl.rank(i)) << "at index " << i;
		}

		// rankZero
		for (size_t i = 0; i <= size; i++) {
			EXPECT_EQ(i, fixedf.rankZero(i)) << "at index " << i;
			EXPECT_EQ(i, fixedl.rankZero(i)) << "at index " << i;
			EXPECT_EQ(i, bytef.rankZero(i)) << "at index " << i;
			EXPECT_EQ(i, bytel.rankZero(i)) << "at index " << i;
			EXPECT_EQ(i, bitf.rankZero(i)) << "at index " << i;
			EXPECT_EQ(i, bitl.rankZero(i)) << "at index " << i;
		}

		// selectZero
		for (size_t i = 0; i < size; i++) {
			EXPECT_EQ(i, fixedf.selectZero(i)) << "at index " << i;
			EXPECT_EQ(i, fixedl.selectZero(i)) << "at index " << i;
			EXPECT_EQ(i, bytef.selectZero(i)) << "at index " << i;
			EXPECT_EQ(i, bytel.selectZero(i)) << "at index " << i;
			EXPECT_EQ(i, bitf.selectZero(i)) << "at index " << i;
			EXPECT_EQ(i, bitl.selectZero(i)) << "at index " << i;
		}

		// update
		for (size_t i = 0; i < size; i++) {
			fixedf.set(i);
			fixedl.set(i);
			bytef.set(i);
			bytel.set(i);
			bitf.set(i);
			bitl.set(i);
		}

		// rank (check update correctness)
		for (size_t i = 0; i < size; i++) {
			EXPECT_EQ(i, fixedf.rank(i)) << "at index " << i;
			EXPECT_EQ(i, fixedl.rank(i)) << "at index " << i;
			EXPECT_EQ(i, bytef.rank(i)) << "at index " << i;
			EXPECT_EQ(i, bytel.rank(i)) << "at index " << i;
			EXPECT_EQ(i, bitf.rank(i)) << "at index " << i;
			EXPECT_EQ(i, bitl.rank(i)) << "at index " << i;
		}

		delete[] bvbitl;
		delete[] bvbitf;
		delete[] bvbytel;
		delete[] bvbytef;
		delete[] bvfixedl;
		delete[] bvfixedf;
	}
}

template <std::size_t S> void run_dynranksel(std::size_t size) {
	using namespace sux;
	const size_t words = size / 64 + ((size % 64 != 0) ? 1 : 0);

	uint64_t *bvfixedf = new uint64_t[size / 64 + 1]();
	uint64_t *bvfixedl = new uint64_t[size / 64 + 1]();
	uint64_t *bvbytef = new uint64_t[size / 64 + 1]();
	uint64_t *bvbytel = new uint64_t[size / 64 + 1]();
	uint64_t *bvbitf = new uint64_t[size / 64 + 1]();
	uint64_t *bvbitl = new uint64_t[size / 64 + 1]();
	uint64_t *bvfixedfS = new uint64_t[size / 64 + 1]();
	uint64_t *bvfixedlS = new uint64_t[size / 64 + 1]();
	uint64_t *bvbytefS = new uint64_t[size / 64 + 1]();
	uint64_t *bvbytelS = new uint64_t[size / 64 + 1]();
	uint64_t *bvbitfS = new uint64_t[size / 64 + 1]();
	uint64_t *bvbitlS = new uint64_t[size / 64 + 1]();

	uint64_t *updates = new uint64_t[words + 1]();

	uint64_t ones = 0;
	for (size_t i = 0; i < (size + 63) / 64; i++) {
		bvfixedf[i] = next();
		if (i == (size + 63) / 64 - 1 && size % 64 != 0) bvfixedf[i] &= (UINT64_C(1) << size % 64) - 1;
		ones += __builtin_popcountll(bvfixedf[i]);
	}

	memcpy(bvfixedl, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));
	memcpy(bvbytef, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));
	memcpy(bvbytel, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));
	memcpy(bvbitf, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));
	memcpy(bvbitl, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));
	memcpy(bvfixedfS, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));
	memcpy(bvfixedlS, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));
	memcpy(bvbytefS, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));
	memcpy(bvbytelS, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));
	memcpy(bvbitfS, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));
	memcpy(bvbitlS, bvfixedf, (size / 64 + 1) * sizeof(uint64_t));

	const size_t zeros = size - ones;

	// word
	bits::WordDynRankSel<util::FenwickFixedF> fixedf(bvfixedf, size);
	bits::WordDynRankSel<util::FenwickFixedL> fixedl(bvfixedl, size);
	bits::WordDynRankSel<util::FenwickByteF> byte(bvbytef, size);
	bits::WordDynRankSel<util::FenwickByteL> bytel(bvbytel, size);
	bits::WordDynRankSel<util::FenwickBitF> bit(bvbitf, size);
	bits::WordDynRankSel<util::FenwickBitL> bitl(bvbitl, size);

	// line
	bits::StrideDynRankSel<util::FenwickFixedF, S> fixedfS(bvfixedfS, size);
	bits::StrideDynRankSel<util::FenwickFixedL, S> fixedlS(bvfixedlS, size);
	bits::StrideDynRankSel<util::FenwickBitF, S> bitS(bvbitfS, size);
	bits::StrideDynRankSel<util::FenwickBitL, S> bitlS(bvbitlS, size);
	bits::StrideDynRankSel<util::FenwickByteF, S> byteS(bvbytefS, size);
	bits::StrideDynRankSel<util::FenwickByteL, S> bytelS(bvbytelS, size);

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
	for (size_t pos = 0; pos < fixedf.rank(size); pos++) {
		auto res = fixedf.rank(pos);

		if (bvfixedf[pos / 64] & UINT64_C(1) << pos % 64) {
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
	for (size_t pos = 0; pos < fixedf.rankZero(size); pos++) {

		auto res = fixedf.rankZero(pos);

		if (bvfixedf[pos / 64] & UINT64_C(1) << pos % 64) {
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
	for (size_t i = 0; i < fixedf.rank(size); i++) {
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
	for (size_t i = 0; i < fixedf.rankZero(size); i++) {
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

	delete[] bvbitlS;
	delete[] bvbitfS;
	delete[] bvbytelS;
	delete[] bvbytefS;
	delete[] bvfixedlS;
	delete[] bvfixedfS;
	delete[] bvbitl;
	delete[] bvbitf;
	delete[] bvbytel;
	delete[] bvbytef;
	delete[] bvfixedl;
	delete[] bvfixedf;
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
