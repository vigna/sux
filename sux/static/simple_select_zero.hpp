/*
 * Sux: Succinct data structures
 *
 * Copyright (C) 2007-2019 Sebastiano Vigna
 *
 *  This library is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License as published by the Free
 *  Software Foundation; either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  This library is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "../common.hpp"
#include <cstdint>

namespace sux {
using namespace std;

class simple_select_zero {
private:
  const uint64_t *bits;
  int64_t *inventory;
  uint64_t *exact_spill;
  int log2_ones_per_inventory, log2_ones_per_sub16, log2_ones_per_sub64,
      log2_longwords_per_subinventory, ones_per_inventory, ones_per_sub16, ones_per_sub64,
      longwords_per_subinventory, longwords_per_inventory, ones_per_inventory_mask,
      ones_per_sub16_mask, ones_per_sub64_mask;

  uint64_t num_words, inventory_size, exact_spill_size, num_ones;

public:
  simple_select_zero();
  simple_select_zero(const uint64_t *const bits, const uint64_t num_bits,
                     const int max_log2_longwords_per_subinventory);
  ~simple_select_zero();
  uint64_t select_zero(const uint64_t rank);
  // Just for analysis purposes
  void printCounts();
  uint64_t bitCount();
};

} // namespace sux
