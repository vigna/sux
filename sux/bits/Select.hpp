/*
 * Sux: Succinct data structures
 *
 * Copyright (C) 2019-2020 Stefano Marchini and Sebastiano Vigna
 *
 *  This library is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License as published by the Free
 *  Software Foundation; either version 3 of the License, or (at your option)
 *  any later version.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * Under Section 7 of GPL version 3, you are granted additional permissions
 * described in the GCC Runtime Library Exception, version 3.1, as published by
 * the Free Software Foundation.
 *
 * You should have received a copy of the GNU General Public License and a copy of
 * the GCC Runtime Library Exception along with this program; see the files
 * COPYING3 and COPYING.RUNTIME respectively.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstddef>
#include <cstdint>

namespace sux {

/** An interface specifying selection primitives on ones. */

class Select {
  public:
	virtual ~Select() = default;

	/** Returns the position of the one with given rank.
	 *
	 * @param rank the desired rank (index) of a one in the bit vector.
	 *
	 * @return the position of the zero with given rank;
	 * the result is undefined if no zero of the given rank exists.
	 */
	virtual std::size_t select(uint64_t rank) = 0;
};

} // namespace sux
