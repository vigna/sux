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

#include "../support/common.hpp"
#include "Select.hpp"
#include <cstdint>

using namespace std;
namespace sux::bits {

class SimpleSelect : public Select {
  private:
	const uint64_t *bits;
	int64_t *inventory;
	uint64_t *exact_spill;
	int log2_ones_per_inventory, log2_ones_per_sub16, log2_ones_per_sub64, log2_longwords_per_subinventory, ones_per_inventory, ones_per_sub16, ones_per_sub64, longwords_per_subinventory,
		longwords_per_inventory, ones_per_inventory_mask, ones_per_sub16_mask, ones_per_sub64_mask;

	uint64_t num_words, inventory_size, exact_spill_size, num_ones;

  public:
	SimpleSelect();
	SimpleSelect(const uint64_t *const bits, const uint64_t num_bits, const int max_log2_longwords_per_subinventory);
	~SimpleSelect();
	size_t select(const uint64_t rank);
	// Just for analysis purposes
	void printCounts();
	uint64_t bitCount();
};

} // namespace sux::bits
