#include <gtest/gtest.h>

#include "../xoroshiro128pp.hpp"
#include "rankselect.hpp"
#include "balparen.hpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
