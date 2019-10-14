#include <gtest/gtest.h>

#include "../xoroshiro128pp.hpp"
#include "ricebitvector.hpp"

#define LEAF 4
#define NKEYS_TEST 1000000
#include "recsplit.hpp"

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
