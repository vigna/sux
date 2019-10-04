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

#ifndef elias_fano_h
#define elias_fano_h
#include <stdint.h>
#include <vector>
#include "simple_select_half.h"
#include "simple_select_zero_half.h"

class elias_fano {
private:
	uint64_t *lower_bits, *upper_bits;

	simple_select_half *select_upper;
	simple_select_zero_half *selectz_upper;
	uint64_t num_bits, num_ones;
	int l;
	int block_size;
	int block_length;
	uint64_t block_size_mask, block_length_mask;
	uint64_t lower_l_bits_mask;
	uint64_t ones_step_l;
	uint64_t msbs_step_l;
	uint64_t compressor;

	__inline static void set( uint64_t * const bits, const uint64_t pos ) {
		bits[ pos / 64 ] |= 1ULL << pos % 64;
	}

	__inline static uint64_t get_bits( const uint64_t * const bits, const uint64_t start, const int width ) {
		const int start_word = start / 64;
		const int start_bit = start % 64;
		const int total_offset = start_bit + width;
		const uint64_t result = bits[ start_word ] >> start_bit;
		return ( total_offset <= 64 ? result : result | bits[ start_word + 1 ] << 64 - start_bit ) & ( 1ULL << width ) - 1;
	}

	__inline static void set_bits( uint64_t * const bits, const uint64_t start, const int width, const uint64_t value ) { 
			const uint64_t start_word = start / 64;
			const uint64_t end_word = ( start + width - 1 ) / 64;
			const uint64_t start_bit = start % 64;

			if ( start_word == end_word ) {
				bits[ start_word ] &= ~ ( ( ( 1ULL << width ) - 1 ) << start_bit );
				bits[ start_word ] |= value << start_bit;
			}
			else {
				// Here start_bit > 0.
				bits[ start_word ] &= ( 1ULL << start_bit ) - 1;
				bits[ start_word ] |= value << start_bit;
				bits[ end_word ] &=  - ( 1ULL << width - 64 + start_bit );
				bits[ end_word ] |= value >> 64 - start_bit;
			}
		}

public:
	elias_fano( const uint64_t * const bits, const uint64_t num_bits );
	elias_fano( const std::vector<uint64_t> pos, const uint64_t num_bits );
	~elias_fano();
	uint64_t rank( const uint64_t pos );
	uint64_t select( const uint64_t rank );
	uint64_t select( const uint64_t rank, uint64_t * const next );
	// Just for analysis purposes
	void print_counts();
	uint64_t bit_count();
};

#endif
