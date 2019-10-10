#include "RiceBitvector.hpp"
#include "../common.hpp"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace sux {

RiceBitvector::RiceBitvector(const std::size_t alloc_words) {
	data = (uint64_t *)calloc(alloc_words, sizeof(*data));
	data_bytes = alloc_words * sizeof(*data);
	bit_count = 0;
	curr_ptr_unary = data;
	curr_fixed_offset = 0;
	curr_window_unary = 0;
	valid_lower_bits_unary = 0;
}

RiceBitvector::~RiceBitvector() {
	if (data) {
		free(data);
		data = NULL;
	}
}

uint64_t RiceBitvector::read_next(const int log2golomb) {
	uint64_t result = 0;

	if (curr_window_unary == 0) {
		result += valid_lower_bits_unary;
		curr_window_unary = *(curr_ptr_unary++);
		valid_lower_bits_unary = 64;
		while (__builtin_expect(curr_window_unary == 0, 0)) {
			result += 64;
			curr_window_unary = *(curr_ptr_unary++);
		}
	}

	const size_t pos = rho(curr_window_unary);

	curr_window_unary >>= pos;
	curr_window_unary >>= 1;
	valid_lower_bits_unary -= pos + 1;

	result += pos;
	result <<= log2golomb;

	uint64_t fixed;
	memcpy(&fixed, (uint8_t *)data + curr_fixed_offset / 8, 8);
	result |= (fixed >> curr_fixed_offset % 8) & ((uint64_t(1) << log2golomb) - 1);
	curr_fixed_offset += log2golomb;
	return result;
}

void RiceBitvector::skip_subtree(const size_t nodes, const size_t fixed_len) {
	assert(nodes > 0);
	size_t missing = nodes, cnt;
	while ((cnt = nu(curr_window_unary)) < missing) {
		curr_window_unary = *(curr_ptr_unary++);
		missing -= cnt;
		valid_lower_bits_unary = 64;
	}
	cnt = select64(curr_window_unary, missing - 1);
	curr_window_unary >>= cnt;
	curr_window_unary >>= 1;
	valid_lower_bits_unary -= cnt + 1;

	curr_fixed_offset += fixed_len;
}

void RiceBitvector::read_reset(const size_t bit_pos, const size_t unary_offset) {
	// assert(bit_pos < bit_count);
	curr_fixed_offset = bit_pos;
	size_t unary_pos = bit_pos + unary_offset;
	curr_ptr_unary = data + unary_pos / 64;
	curr_window_unary = *(curr_ptr_unary++) >> (unary_pos & 63);
	valid_lower_bits_unary = 64 - (unary_pos & 63);
}

void RiceBitvector::append_fixed(const uint64_t v, const int log2golomb) {
	const uint64_t lower_bits = v & ((uint64_t(1) << log2golomb) - 1);
	int used_bits = bit_count & 63;

	while (((bit_count + log2golomb + 63) / 64) * 8 + 7 > data_bytes) {
		auto offset_unary = curr_ptr_unary - data;
		data = (uint64_t *)realloc(data, data_bytes * 2);
		memset(data + data_bytes / sizeof(*data), 0, data_bytes);
		data_bytes *= 2;
		curr_ptr_unary = data + offset_unary;
	}

	uint64_t *append_ptr = data + bit_count / 64;
	uint64_t cur_word = *append_ptr;

	cur_word |= lower_bits << used_bits;
	if (used_bits + log2golomb > 64) {
		*(append_ptr++) = cur_word;
		cur_word = lower_bits >> (64 - used_bits);
		used_bits += log2golomb - 64;
	}
	*append_ptr = cur_word;
	bit_count += log2golomb;
}

void RiceBitvector::append_unary_all(const std::vector<uint32_t> unary) {
	size_t bit_inc = 0;
	for (const auto &u : unary) {
		bit_inc += u + 1;
	}

	while (((bit_count + bit_inc + 63) / 64) * 8 + 7 > data_bytes) {
		auto offset_unary = curr_ptr_unary - data;
		data = (uint64_t *)realloc(data, data_bytes * 2);
		memset(data + data_bytes / sizeof(*data), 0, data_bytes);
		data_bytes *= 2;
		curr_ptr_unary = data + offset_unary;
	}

	for (const auto &u : unary) {
		bit_count += u;
		uint64_t *append_ptr = data + bit_count / 64;
		*append_ptr |= uint64_t(1) << (bit_count & 63);
		++bit_count;
	}
}

size_t RiceBitvector::get_bits() const { return bit_count; }

void RiceBitvector::fit_data() {
	data_bytes = (bit_count + 63) / 64 * sizeof(uint64_t) + 7;
	data = (uint64_t *)realloc(data, data_bytes);
	curr_ptr_unary = data;
}

void RiceBitvector::print_bits() const {
	std::size_t size = bit_count;
	for (uint64_t *p = data; p <= data + bit_count / 64; ++p) {
		for (std::size_t i = 0; i < std::min(size, size_t(64)); ++i) {
			printf("%lu", ((*p) >> i) & 1);
		}
		size -= 64;
		printf("\n");
	}
}

int RiceBitvector::dump(FILE *fp) const {
	fwrite(&bit_count, sizeof(bit_count), (size_t)1, fp);
	fwrite(&data_bytes, sizeof(data_bytes), (size_t)1, fp);
	fwrite(data, data_bytes, (size_t)1, fp);

	return 1;
}

void RiceBitvector::load(FILE *fp) {
	curr_window_unary = 0;
	valid_lower_bits_unary = 0;

	fread(&bit_count, sizeof(bit_count), (size_t)1, fp);
	fread(&data_bytes, sizeof(data_bytes), (size_t)1, fp);

	if (data) {
		free(data);
		data = NULL;
	}
	data = (uint64_t *)malloc(data_bytes);
	fread(data, data_bytes, (size_t)1, fp);
	curr_ptr_unary = data;
}

} // namespace sux