/*
 * Sux: Succinct data structures
 *
 * Copyright (C) 2019 Stefano Marchini
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
#include <assert.h>
#include <iostream>
#include <string>
#include <sys/mman.h>

namespace sux::util {

class Expandable {
public:

	/** Trim the the memory allocated so that it holds at most the given number of elements.
	 * @param capacity the new desired capacity.
	 */
	virtual void trim(size_t space) = 0;

	/** Trim the the memory allocated so that it holds exactly size() elements. */
	virtual void trimToFit() = 0;

	/** Enlarges the backing array to that it can contain a given number of elements.
	 *
	 * If the current capacity is sufficient, nothing happens. Otherwise, the
	 * backing is enlarged to the provided capacity.
	 *
	 * @param capacity the desired new capacity.
	 */
	virtual void reserve(size_t space) = 0;

	/** Enlarges the backing array to that it can contain a given number of elements, plus possibly extra space.
	 *
	 * If the current capacity is sufficient, nothing happens. Otherwise, the
	 * backing is enlarged to the maximum between the provided capacity and
	 * 50% more than the current capacity.
	 *
	 * @param capacity the desired new capacity.
	 */
	virtual void grow(size_t space) = 0;

	/** Changes the vector size to the given value.
	  *
	  * If the argument is smaller than or equal to the current size,
	  * the backing array is unmodified. Otherwise, the backing array
	  * is enlarged to the given size using grow(). New elements are
	  * initialized to zero.
	  *
	  * @param size the desired new size.
	  */
	virtual void resize(size_t size) = 0;

	/** Returns the number of elements in this vector. */
	virtual inline size_t size() const = 0;

	/** Changes the vector size and capacity to the given value.
   *
   * Both size and capacity are set to the provided size.
   * If necessary, new elements are initialized to zero.
   *
   * @param size the desired new size.
   */
	virtual void size(size_t size) = 0;

	/** Returns the number of elements that this vector
	 * can hold currently without increasing its capacity.
	 *
	 * @return the number of elements that this vector
	 * can hold currently without increasing its capacity.
	 */
	virtual inline size_t capacity() const = 0;
};

} // namespace sux::util
