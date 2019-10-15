#include <gtest/gtest.h>

#include "../xoroshiro128pp.hpp"
#include "dynranksel.hpp"
#include "rankselect.hpp"
#include <sux/util/FenwickBitF.hpp>
#include <sux/util/FenwickBitL.hpp>
#include <sux/util/FenwickByteF.hpp>
#include <sux/util/FenwickByteL.hpp>
#include <sux/util/FenwickFixedF.hpp>
#include <sux/util/FenwickFixedL.hpp>

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
