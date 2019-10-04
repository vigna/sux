/*		 
 * Sux: Succinct data structures
 *
 * Copyright (C) 2007-2013 Sebastiano Vigna 
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

#ifndef simple_select_half_h
#define simple_select_half_h

using namespace std;

#include <stdint.h>
#include "macros.h"
#include "select.h"

class simple_select_half {
private:
	const uint64_t *bits;
	int64_t *inventory;

	uint64_t num_words, inventory_size, num_ones;

public:
	simple_select_half();
	simple_select_half( const uint64_t * const bits, const uint64_t num_bits );
	~simple_select_half();
	uint64_t select( const uint64_t rank );
	uint64_t select( const uint64_t rank, uint64_t * const next );
	// Just for analysis purposes
	void print_counts();
	uint64_t bit_count();
};

#endif
