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

#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <algorithm>
#include "elias_fano.h"

elias_fano::elias_fano( const uint64_t * const bits, const uint64_t num_bits ) {
	const uint64_t num_words = ( num_bits + 63 ) / 64;
	uint64_t m = 0;
	for( uint64_t i = num_words; i-- != 0; ) m += __builtin_popcountll( bits[ i ] );
	num_ones = m;
	this->num_bits = num_bits;
	l = num_ones == 0 ? 0 : max( 0, msb( num_bits / num_ones ) );

	printf( "Number of ones: %lld l: %d\n", num_ones, l );
	printf( "Upper bits: %lld\n", num_ones + ( num_bits >> l ) + 1 );
	printf( "Lower bits: %lld\n", num_ones * l );

	const uint64_t lower_bits_mask = ( 1ULL << l ) - 1;

	lower_bits = new uint64_t[ ( num_ones * l + 63  ) / 64 + 2 * ( l == 0 ) ];
	upper_bits = new uint64_t[ ( ( num_ones + ( num_bits >> l ) + 1 ) + 63 ) / 64 ]();

	uint64_t pos = 0;
	for( uint64_t i = 0; i < num_bits; i++ ) {
		if ( bits[ i / 64 ] & ( 1ULL << i % 64 ) ) {
			if ( l != 0 ) set_bits( lower_bits, pos * l, l, i & lower_bits_mask );
			set( upper_bits, ( i >> l ) + pos );
			pos++;
		}
	}


#ifdef DEBUG
	printf("First lower: %016llx %016llx %016llx %016llx\n", lower_bits[ 0 ], lower_bits[ 1 ], lower_bits[ 2 ], lower_bits[ 3 ] );
	printf("First upper: %016llx %016llx %016llx %016llx\n", upper_bits[ 0 ], upper_bits[ 1 ], upper_bits[ 2 ], upper_bits[ 3 ] );
#endif

	select_upper = new simple_select_half( upper_bits, num_ones + ( num_bits >> l ) );
	selectz_upper = new simple_select_zero_half( upper_bits, num_ones + ( num_bits >> l ) );

	block_size = 0;
	while( ++block_size * l + block_size <= 64 && block_size <= l );
	block_size--;

	printf( "Block size: %d\n", block_size );
	block_size_mask = ( 1ULL << block_size ) - 1;
	block_length = block_size * l;
	block_length_mask = block_length - 1;

	ones_step_l = 0;
	for( int i = 0; i < block_size; i++) ones_step_l |= 1ULL << i * l;
	msbs_step_l = ones_step_l << l - 1;

	compressor = 0;
	for( int i = 0; i < block_size; i++) compressor |= 1ULL << ( l - 1 ) * i + block_size;
	
	lower_l_bits_mask = ( 1ULL << l ) - 1;

#ifndef NDEBUG
	uint64_t r, t;
	for( uint64_t i = 0; i < num_ones; i++ ) {
		t = select( i );
		r = rank( t );
		if ( r != i ) {
			printf( "i: %lld s: %lld r: %lld\n", i, t, r );
			assert( r == i );
		}
	}

	for( uint64_t i = 0; i < num_bits; i++ ) {
		r = rank( i );
		if ( r < num_ones ) {
			t = select( r );
			if ( t < i ) {
				printf( "i: %lld r: %lld s: %lld\n", i, r, t );
				assert( t >= i );
			}
		}
	}
#endif
}

elias_fano::elias_fano( const std::vector<uint64_t> ones, const uint64_t num_bits ) {
	const uint64_t num_words = ( num_bits + 63 ) / 64;
	num_ones = ones.size();
	this->num_bits = num_bits;
	l = num_ones == 0 ? 0 : max( 0, msb( num_bits / num_ones ) );

	printf( "Number of ones: %lld l: %d\n", num_ones, l );
	printf( "Upper bits: %lld\n", num_ones + ( num_bits >> l ) + 1 );
	printf( "Lower bits: %lld\n", num_ones * l );

	const uint64_t lower_bits_mask = ( 1ULL << l ) - 1;

	lower_bits = new uint64_t[ ( num_ones * l + 63  ) / 64 + 2 * ( l == 0 ) ];
	upper_bits = new uint64_t[ ( ( num_ones + ( num_bits >> l ) + 1 ) + 63 ) / 64 ]();

	for( int64_t i = 0; i < num_ones; i++ ) {
		if ( l != 0 ) set_bits( lower_bits, i * l, l, ones[i] & lower_bits_mask );
		set( upper_bits, ( ones[i] >> l ) + i );
	}


#ifdef DEBUG
	printf("First lower: %016llx %016llx %016llx %016llx\n", lower_bits[ 0 ], lower_bits[ 1 ], lower_bits[ 2 ], lower_bits[ 3 ] );
	printf("First upper: %016llx %016llx %016llx %016llx\n", upper_bits[ 0 ], upper_bits[ 1 ], upper_bits[ 2 ], upper_bits[ 3 ] );
#endif

	select_upper = new simple_select_half( upper_bits, num_ones + ( num_bits >> l ) );
	selectz_upper = new simple_select_zero_half( upper_bits, num_ones + ( num_bits >> l ) );

	block_size = 0;
	while( ++block_size * l + block_size <= 64 && block_size <= l );
	block_size--;

	printf( "Block size: %d\n", block_size );
	block_size_mask = ( 1ULL << block_size ) - 1;
	block_length = block_size * l;
	block_length_mask = block_length - 1;

	ones_step_l = 0;
	for( int i = 0; i < block_size; i++) ones_step_l |= 1ULL << i * l;
	msbs_step_l = ones_step_l << l - 1;

	compressor = 0;
	for( int i = 0; i < block_size; i++) compressor |= 1ULL << ( l - 1 ) * i + block_size;
	
	lower_l_bits_mask = ( 1ULL << l ) - 1;

#ifndef NDEBUG
	for( uint64_t i = 0; i < num_ones; i++ ) assert( select( i ) == ones[i] );
	for( uint64_t i = 0; i < num_ones; i++ ) assert( rank( ones[i] ) == i );

	uint64_t r, t;
	for( uint64_t i = 0; i < num_ones; i++ ) {
		t = select( i );
		r = rank( t );
		if ( r != i ) {
			printf( "i: %lld s: %lld r: %lld\n", i, t, r );
			assert( r == i );
		}
	}

	for( uint64_t i = 0; i < num_bits; i++ ) {
		r = rank( i );
		if ( r < num_ones ) {
			t = select( r );
			if ( t < i ) {
				printf( "i: %lld r: %lld s: %lld\n", i, r, t );
				assert( t >= i );
			}
		}
	}
#endif
}



elias_fano::~elias_fano() {
	delete [] upper_bits;
	delete [] lower_bits;
	delete select_upper;
	delete selectz_upper;
}

uint64_t elias_fano::rank( const uint64_t k ) {
	if ( num_ones == 0 ) return 0;
	if ( k >= num_bits ) return num_ones;
#ifdef DEBUG
	printf( "Ranking %lld...\n", k );
#endif
	const uint64_t k_shiftr_l = k >> l;

#ifndef PARSEARCH
	int64_t pos = selectz_upper->select_zero( k_shiftr_l );
	uint64_t rank = pos - ( k_shiftr_l );

#ifdef DEBUG
	printf( "Position: %lld rank: %lld\n", pos, rank );
#endif
	uint64_t rank_times_l = rank * l;
	const uint64_t k_lower_bits = k & lower_l_bits_mask;

	do {
		rank--; 
		rank_times_l -= l;
		pos--; 
	} while( pos >= 0 && ( upper_bits[ pos / 64 ] & 1ULL << pos % 64 ) && get_bits( lower_bits, rank_times_l, l ) >= k_lower_bits );

	return ++rank;
#else

	const uint64_t k_lower_bits = k & lower_l_bits_mask;

#ifdef DEBUG
	printf( "k: %llx lower %d : %llx\n", k, l, k_lower_bits );
#endif

	const uint64_t k_lower_bits_step_l = k_lower_bits * ones_step_l;

	uint64_t pos = selectz_upper->select_zero( k_shiftr_l );
	uint64_t rank = pos - ( k_shiftr_l );
	uint64_t rank_times_l = rank * l;

#ifdef DEBUG
	printf( "pos: %lld rank: %lld\n", pos, rank );
#endif

	uint64_t block_upper_bits, block_lower_bits;
	
	while ( rank > block_size ) {
		rank -= block_size;
		rank_times_l -= block_length;
		pos -= block_size;
		block_upper_bits = get_bits( upper_bits, pos, block_size );
		block_lower_bits = get_bits( lower_bits, rank_times_l, block_length );

		//printf( "block upper bits: %llx block lower bits: %llx\n", block_upper_bits, block_lower_bits );

		const uint64_t cmp =	( ( ( ( block_lower_bits | msbs_step_l ) - ( k_lower_bits_step_l & ~msbs_step_l ) ) | ( k_lower_bits_step_l ^ block_lower_bits ) ) ^ ( k_lower_bits_step_l & ~block_lower_bits ) ) & msbs_step_l;

		//printf( "Compare: %016llx compressed: %016llx shifted: %016llx\n", cmp, cmp * compressor, cmp * compressor >> block_size * l );

		const uint64_t cmp_compr = ~( cmp * compressor >> block_length & block_upper_bits ) & block_size_mask;

		//printf( "Combined compare: %llx\n", ~t );

		if ( cmp_compr ) return rank + 1 + msb( cmp_compr );
	}

	block_upper_bits = get_bits( upper_bits, pos - rank, rank );
	block_lower_bits = get_bits( lower_bits, 0, rank_times_l );

	//printf( "\nTail (%lld bits)...\n", rank );

	//printf( "block upper bits: %llx block lower bits: %llx\n", block_upper_bits, block_lower_bits );

	const uint64_t cmp =	( ( ( ( block_lower_bits | msbs_step_l ) - ( k_lower_bits_step_l & ~msbs_step_l ) ) | ( k_lower_bits_step_l ^ block_lower_bits ) ) ^ ( k_lower_bits_step_l & ~block_lower_bits ) ) & msbs_step_l;

	//printf( "Compressor: %llx\n", compressor );
	//printf( "Compare: %016llx compressed: %016llx shifted: %016llx\n", cmp, cmp * compressor, cmp * compressor >> block_size * l );

	const uint64_t cmp_compr = ~( cmp * compressor >> block_length & block_upper_bits ) & ( 1ULL << rank ) - 1;

	//printf( "Combined compare: %llx\n", ~t );

	return 1 + msb( cmp_compr );

#endif
}

uint64_t elias_fano::select( const uint64_t rank ) {
#ifdef DEBUG
	printf( "Selecting %lld...\n", rank );
#endif
#ifdef DEBUG
	printf( "Returning %lld = %llx << %d | %llx\n", ( select_upper->select( rank ) - rank ) << l | get_bits( lower_bits, rank * l, l ), select_upper->select( rank ) - rank , l, get_bits( lower_bits, rank * l, l ) );
#endif
	return ( select_upper->select( rank ) - rank ) << l | get_bits( lower_bits, rank * l, l );
}

uint64_t elias_fano::select( const uint64_t rank, uint64_t * const next ) {
	uint64_t s, t;
	s = select_upper->select( rank, &t ) - rank;
	t -= rank + 1;

	const uint64_t position = rank * l;
	*next = t << l | get_bits( lower_bits, position + l, l );
	return s << l | get_bits( lower_bits, position, l );
}

uint64_t elias_fano::bit_count() {
	return num_ones * l + num_ones + ( num_bits >> l ) + select_upper->bit_count() + selectz_upper->bit_count();
}

void elias_fano::print_counts() {}
