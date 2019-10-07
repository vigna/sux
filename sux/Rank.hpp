/*
 * Sux: Succinct data structures
 *
 * Copyright (C) 2019 Stefano Marchini and Sebastiano Vigna
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

#include "common.hpp"

namespace sux {

class Rank {
public:
  virtual ~Rank() = default;

  /**
   * rank() - Numbers of 1-bits preceding a specified position.
   * @pos: An index of the bit vector.
   *
   */
  virtual uint64_t rank(size_t pos) const = 0;

  /**
   * rank() - Numbers of 1-bits between a given range.
   * @from: Starting index of the bit vector.
   * @to: Ending index of the bit vector.
   *
   */
  uint64_t rank(size_t from, size_t to) const { return rank(to) - rank(from); }

  /**
   * rankZero() - Numbers of 0-bits preceding a specified position.
   * @pos: An index of the bit vector.
   *
   */
  virtual uint64_t rankZero(size_t pos) const { return pos - rank(pos); }

  /**
   * rankZero() - Number of 0-bits between a given range.
   * @from: Starting index of the bit vector.
   * @to: Ending index of the bit vector.
   *
   */

  uint64_t rankZero(size_t from, size_t to) const { return rankZero(to) - rankZero(from); }

  /**
   * size() - length (in bits) of the bitvector.
   *
   */
  virtual size_t size() const = 0;
};

} // namespace sux
