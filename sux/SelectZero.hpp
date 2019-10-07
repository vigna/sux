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

class SelectZero {
public:
  virtual ~SelectZero() = default;

  /**
   * selectZero() - Index of the 0-bit with a given rank.
   * @rank: Number of 0-bits before the returned index.
   *
   * This method returns SIZE_MAX if no sucn an index exists.
   *
   */
  virtual size_t selectZero(uint64_t rank) const = 0;
};

} // namespace sux
