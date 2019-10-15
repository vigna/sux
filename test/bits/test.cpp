#include <gtest/gtest.h>

#include "../xoroshiro128pp.hpp"
#include "dynranksel.hpp"
#include "rankselect.hpp"
#include <sux/util/BitF.hpp>
#include <sux/util/BitL.hpp>
#include <sux/util/ByteF.hpp>
#include <sux/util/ByteL.hpp>
#include <sux/util/FixedF.hpp>
#include <sux/util/FixedL.hpp>

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
