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

namespace sux {

/** An interface specifying selection primitives on ones. */

class Select {
  public:
	virtual ~Select() = default;

	/** Returns the position of the one with given rank.
	 *
	 * @param rank The desired rank (index) of a one in the bit vector.
	 *
	 * @return The return value is undefined if no one of the given rank exists.
	 */
	virtual size_t select(uint64_t rank) = 0;
};

} // namespace sux
