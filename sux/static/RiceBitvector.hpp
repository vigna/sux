#pragma once

#include <cstdint>
#include <cstdio>
#include <vector>

using namespace std;

namespace sux {

#define DEFAULT_VECTSIZE (1 << 2)

class RiceBitvector {
  private:
	uint64_t *data;
	std::size_t data_bytes;
	std::size_t bit_count;

	size_t curr_fixed_offset;
	uint64_t *curr_ptr_unary;
	uint64_t curr_window_unary;
	int valid_lower_bits_unary;

  public:
	RiceBitvector(const size_t alloc_words = DEFAULT_VECTSIZE);
	~RiceBitvector();
	uint64_t read_next(const int log2golomb);
	void skip_subtree(const size_t nodes, const size_t fixed_len);
	void read_reset(const size_t bit_pos = 0, const size_t unary_offset = 0);
	void append_fixed(const uint64_t v, const int log2golomb);
	void append_unary_all(const vector<uint32_t> unary);
	size_t get_bits() const;
	void fit_data();
	void print_bits() const;
	int dump(FILE *fp) const;
	void load(FILE *fp);
};

} // namespace sux
