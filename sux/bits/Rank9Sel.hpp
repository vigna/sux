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
#include "Rank.hpp"
#include "Select.hpp"
#include <cstdint>

namespace sux::bits {

/**
 *  A class implementing a combination of Rank9 for ranking and Select9 for selection.
 *  Select9 uses 25%-37.5% additional space (beside the 25% due to Rank9), depending on density,
 *  and provides constant-time selection.
 */

class Rank9Sel : public Rank, public Select {
  private:
	const size_t num_bits;
	const uint64_t *bits;
	uint64_t *counts, *inventory, *subinventory;
	uint64_t num_words, num_counts, inventory_size, ones_per_inventory, log2_ones_per_inventory, num_ones;

  public:
	Rank9Sel(const uint64_t *const bits, const uint64_t num_bits);
	~Rank9Sel();

	uint64_t rank(const size_t pos);
	size_t select(const uint64_t rank);
	size_t size() const;

	uint64_t bitCount();
};

} // namespace sux::bits
