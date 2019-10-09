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

#include "../Rank.hpp"
#include <cstdint>

namespace sux {

class Rank9 : public Rank {
private:
  const size_t num_bits;
  const uint64_t *bits;
  uint64_t *counts, *inventory;
  uint64_t num_words, num_counts, inventory_size, ones_per_inventory, log2_ones_per_inventory,
      num_ones;

public:
  Rank9(const uint64_t *const bits, const uint64_t num_bits);
  ~Rank9();

  using Rank::rank;
  uint64_t rank(const size_t pos) const;
  size_t size() const;

  // Just for analysis purposes
  void printCounts();
  uint64_t bitCount();
};

} // namespace sux
