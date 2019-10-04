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

using namespace std;

#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include "select.h"
#include "simple_select_zero.h"
#include "rank9.h"

#define MAX_ONES_PER_INVENTORY (8192)

simple_select_zero::simple_select_zero() {}

simple_select_zero::simple_select_zero( const uint64_t * const bits, const uint64_t num_bits, const int max_log2_longwords_per_subinventory ) {
	this->bits = bits;
	num_words = ( num_bits + 63 ) / 64;
	
	// Init rank/select structure
	uint64_t c = 0;
	for( uint64_t i = 0; i < num_words; i++ ) c += __builtin_popcountll( ~bits[ i ] );
	num_ones = c;

if ( num_bits % 64 != 0 ) c -= 64 - num_bits % 64;
	assert( c <= num_bits );

	ones_per_inventory = num_bits == 0 ? 0 : ( c * MAX_ONES_PER_INVENTORY + num_bits - 1 ) / num_bits;
	// Make ones_per_inventory into a power of 2
	log2_ones_per_inventory = max( 0, msb( ones_per_inventory ) );
	ones_per_inventory = 1ULL << log2_ones_per_inventory;
	ones_per_inventory_mask = ones_per_inventory - 1;
	inventory_size = ( c + ones_per_inventory - 1 ) / ones_per_inventory;

	printf("Number of ones: %lld Number of ones per inventory item: %d\n", c, ones_per_inventory );	

	log2_longwords_per_subinventory = min( max_log2_longwords_per_subinventory, max( 0, log2_ones_per_inventory - 2 ) );
	longwords_per_subinventory = 1 << log2_longwords_per_subinventory;
	longwords_per_inventory = longwords_per_subinventory + 1;
	log2_ones_per_sub64 = max( 0, log2_ones_per_inventory - log2_longwords_per_subinventory );
	log2_ones_per_sub16 = max( 0, log2_ones_per_sub64 - 2 );
	ones_per_sub64 = 1ULL << log2_ones_per_sub64;
	ones_per_sub16 = 1ULL << log2_ones_per_sub16;
	ones_per_sub64_mask = ones_per_sub64 - 1;
	ones_per_sub16_mask = ones_per_sub16 - 1;

	printf("Longwords per subinventory: %d Ones per sub 64: %d sub 16: %d\n", longwords_per_subinventory, ones_per_sub64, ones_per_sub16 );

	inventory = new int64_t[ inventory_size * longwords_per_inventory + 1 ];
	const int64_t *end_of_inventory = inventory + inventory_size * longwords_per_inventory + 1;

	uint64_t d = 0;

	// First phase: we build an inventory for each one out of ones_per_inventory.
	for( uint64_t i = 0; i < num_words; i++ )
		for( int j = 0; j < 64; j++ ) {
			if ( i * 64 + j >= num_bits ) break;
			if ( ~bits[ i ] & 1ULL << j ) {
				if ( ( d & ones_per_inventory_mask ) == 0 ) inventory[ ( d >> log2_ones_per_inventory ) * longwords_per_inventory ] = i * 64 + j;
				d++;
			}
		}

	assert( c == d );
	inventory[ inventory_size * longwords_per_inventory ] = num_bits;

	printf("Inventory entries filled: %lld\n", inventory_size + 1 );

	if ( ones_per_inventory > 1 ) {
		d = 0;
		int ones;
		uint64_t spilled = 0, exact = 0, start, span, inventory_index;

		for( uint64_t i = 0; i < num_words; i++ )
			// We estimate the subinventory and exact spill size
			for( int j = 0; j < 64; j++ ) {
				if ( i * 64 + j >= num_bits ) break;
				if ( ~bits[ i ] & 1ULL << j ) {
					if ( ( d & ones_per_inventory_mask ) == 0 ) {
						inventory_index = d >> log2_ones_per_inventory;
						start = inventory[ inventory_index * longwords_per_inventory ];
						span = inventory[ ( inventory_index + 1 ) * longwords_per_inventory ] - start;
						ones = min( c - d, (uint64_t)ones_per_inventory );
						
						assert( start + span == num_bits || ones == ones_per_inventory );

						// We accumulate space for exact pointers ONLY if necessary.
						if ( span >= (1<<16) ) {
							exact += ones;
							if ( ones_per_sub64 > 1 ) spilled += ones;
						}

					}
					d++;
				}
			}

		printf("Spilled entries: %lld exact: %lld\n", spilled, exact );

		exact_spill_size = spilled;
		exact_spill = new uint64_t[ exact_spill_size ];

		uint16_t *p16;
		int64_t *p64;
		int offset;
		spilled = 0;
		d = 0;

		for( uint64_t i = 0; i < num_words; i++ )
			for( int j = 0; j < 64; j++ ) {
				if ( i * 64 + j >= num_bits ) break;
				if ( ~bits[ i ] & 1ULL << j ) {
					if ( ( d & ones_per_inventory_mask ) == 0 ) {
						inventory_index = d >> log2_ones_per_inventory;
						start = inventory[ inventory_index * longwords_per_inventory ];
						span = inventory[ ( inventory_index + 1 ) * longwords_per_inventory ] - start;
						p64 = &inventory[ inventory_index * longwords_per_inventory + 1 ];
						p16 = (uint16_t *)p64;
						offset = 0;
					}

					if ( span < (1<<16) ) {
						assert( i * 64 + j - start <= (1<<16) );
						if ( ( d & ones_per_sub16_mask ) == 0 ) {
							assert( offset < longwords_per_subinventory * 4 );
							assert( p16 + offset < (uint16_t *)end_of_inventory );
							p16[ offset++ ] = i * 64 + j - start;
						}
					}
					else {
						if ( ones_per_sub64 == 1 ) {
							assert( p64 + offset < end_of_inventory );
							p64[ offset++ ] = i * 64 + j;
						}
						else {
							assert( p64 < end_of_inventory );
							if ( ( d & ones_per_inventory_mask ) == 0 ) {
								inventory[ inventory_index * longwords_per_inventory ] |= 1ULL << 63;
								p64[ 0 ] = spilled;
							}
							assert( spilled < exact_spill_size );
							exact_spill[ spilled++ ] = i * 64 + j;
						}
					}

					d++;
				}
			}
	}
	else {
		exact_spill = NULL;
		exact_spill_size = 0;
	}

#ifdef DEBUG
	printf("First inventories: %lld %lld %lld %lld\n", inventory[ 0 ], inventory[ 1 ], inventory[ 2 ], inventory[ 3 ] );
	//if ( subinventory_size > 0 ) printf("First subinventories: %016llx %016llx %016llx %016llx\n", subinventory[ 0 ], subinventory[ 1 ], subinventory[ 2 ], subinventory[ 3 ] );
	if ( exact_spill_size > 0 ) printf("First spilled entries: %016llx %016llx %016llx %016llx\n", exact_spill[ 0 ], exact_spill[ 1 ], exact_spill[ 2 ], exact_spill[ 3 ] );
#endif

#ifndef NDEBUG
	uint64_t r, t;
	rank9 rank9( bits, num_bits );
	for( uint64_t i = 0; i < c; i++ ) {
		t = select_zero( i );
		assert( t < num_bits );
		r = t - rank9.rank( t );
		if ( r != i ) {
			printf( "i: %lld s: %lld r: %lld\n", i, t, r );
			assert( r == i );
		}
	}

	for( uint64_t i = 0; i < num_bits; i++ ) {
		r = i - rank9.rank( i );
		if ( r < c ) {
			t = select_zero( r );
			if ( t < i ) {
				printf( "i: %lld r: %lld s: %lld\n", i, r, t );
				assert( t >= i );
			}
		}
	}

#endif

}

simple_select_zero::~simple_select_zero() {
	delete [] inventory;
	delete [] exact_spill;
}

uint64_t simple_select_zero::select_zero( const uint64_t rank ) {
#ifdef DEBUG
	printf( "Selecting %lld\n...", rank );
#endif

	const uint64_t inventory_index = rank >> log2_ones_per_inventory;
	const int64_t *inventory_start = inventory + ( inventory_index << log2_longwords_per_subinventory ) + inventory_index;
	assert( inventory_index < inventory_size );

	const int64_t inventory_rank = *inventory_start;
	const int subrank = rank & ones_per_inventory_mask;
#ifdef DEBUG
	printf( "Rank: %lld inventory index: %lld inventory rank: %lld subrank: %d\n", rank, inventory_index, inventory_rank, subrank );
#endif

#ifdef DEBUG
	if ( subrank == 0 ) puts( "Exact hit (no subrank); returning inventory" );
#endif
	if ( subrank == 0 ) return inventory_rank & ~(1ULL<<63);

	uint64_t start;
	int residual;

	if ( inventory_rank >= 0 ) {
		start = inventory_rank + ((uint16_t *)( inventory_start + 1 ) )[ subrank >> log2_ones_per_sub16 ];
		residual = subrank & ones_per_sub16_mask;
	}
	else {
		if ( ones_per_sub64 == 1 ) return *(inventory_start + 1 + subrank);
		assert( *(inventory_start + 1) + subrank < exact_spill_size );
		return exact_spill[ *(inventory_start + 1) + subrank ];
	}

#ifdef DEBUG
	printf( "Differential; start: %lld residual: %d\n", start, residual );
	if ( residual == 0 ) puts( "No residual; returning start" );
#endif

	if ( residual == 0 ) return start;

	uint64_t word_index = start / 64;
	uint64_t word = ~bits[ word_index ] & -1ULL << start;

	for(;;) {
		const int bit_count = __builtin_popcountll( word );
		if ( residual < bit_count ) break;
		word = ~bits[ ++word_index ];
		residual -= bit_count;
	} 

	return word_index * 64 + select_in_word( word, residual );
}

uint64_t simple_select_zero::bit_count() {
	return ( inventory_size * longwords_per_inventory + 1 + exact_spill_size ) * 64;
}

void simple_select_zero::print_counts() {}
