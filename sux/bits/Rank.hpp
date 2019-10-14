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

/** An interface specifying ranking primitives.
 *
 * This inferface specifies primitives both on ones and on zeroes because ranking on
 * ones automatically makes it possible ranking on zeros, and viceversa. This interface
 * provides template methods for rank(size_t, size_t), rankZero(size_t) and rankZero(size_t,size_t)
 * using just rank(size_t).
 */

class Rank {
  public:
	virtual ~Rank() = default;

	/** Returns the number of ones before the given posistion.
	 *
	 * @param pos A position from 0 to size() (included).
	 * @return the number of ones from the start of the bit vector up to the given position (excluded).
	 */
	virtual uint64_t rank(size_t pos) = 0;

	/** Returns the number of ones between two given positions.
	 *
	 * @param from A starting position from 0 to size() (included).
	 * @param to An ending position from `from` to size() (included).
	 * @return the number of ones from the starting position (included) to the ending position (excluded).
	 */
	uint64_t rank(size_t from, size_t to) { return rank(to) - rank(from); }

	/** Returns the number of zeros before the given posistion.
	 *
	 * @param pos A position from 0 to size() (included).
	 * @return the number of zeros from the start of the bit vector up to the given position (excluded).
	 */
	virtual uint64_t rankZero(size_t pos) { return pos - rank(pos); }

	/** Returns the number of zeros between two given positions.
	 *
	 * @param from A starting position from 0 to size() (included).
	 * @param to An ending position from `from` to size() (included).
	 * @return the number of zeros from the starting position (included) to the ending position (excluded).
	 */
	uint64_t rankZero(size_t from, size_t to) { return rankZero(to) - rankZero(from); }

	/** Returns the length (in bits) of the underlying bit vector. */
	virtual size_t size() const = 0;
};

} // namespace sux
