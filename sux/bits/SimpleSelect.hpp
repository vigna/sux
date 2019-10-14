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

/** A simple Select implementation based on a two-level inventory, a spill list and broadword bit search.
 *
 * This implementation uses around 13.75% additional space on evenly distributed bit arrays, and,
 * under the same conditions, provide very fast selects. For very unevenly distributed arrays
 * the space occupancy will grow significantly, and access time might vary wildly.
 */

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
	/** Creates a new instance using a given bit vector.
	 *
	 * @param bits a bit vector of 64-bit words.
	 * @param num_bits the length (in bits) of the bit vector.
	 * @param max_log2_longwords_per_subinventory the number of words per subinventory:
	 * a larger value yields a faster map that uses more space; typical values are between 0 and 3.
	 */
	SimpleSelect(const uint64_t *const bits, const uint64_t num_bits, const int max_log2_longwords_per_subinventory);
	~SimpleSelect();
	size_t select(const uint64_t rank);
	// Just for analysis purposes
	void printCounts();
	uint64_t bitCount();
};

} // namespace sux::bits
