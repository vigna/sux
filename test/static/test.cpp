#include <gtest/gtest.h>

#include "../xoroshiro128pp.hpp"
#include "rankselect.hpp"
#include "ricebitvector.hpp"
#include "recsplit.hpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
