#pragma once

#include <sux/function/RiceBitvector.hpp>
#include <random>
#include <vector>

using namespace std;
using namespace sux::function;

static const size_t rice_test_nkeys = 256;
static const size_t rice_test_ntrees = 100000;

static vector<uint64_t> gen_keys() {
	static random_device rd;
	static mt19937_64 rng(rd());
	static uniform_int_distribution<uint64_t> gen(32, 64);

	vector<uint64_t> keys;
	for (size_t i = 0; i < rice_test_nkeys; ++i) {
		keys.push_back(gen(rng));
	}

	return keys;
}

static RiceBitvector *build_bitvector(const vector<uint64_t> &keys, const int golomb_param) {
	RiceBitvector *r = new RiceBitvector();
	for (size_t t = 0; t < rice_test_ntrees; ++t) {
		vector<uint32_t> unary;
		for (uint64_t k : keys) {
			r->append_fixed(k, golomb_param);
			unary.push_back(k >> golomb_param);
		}
		r->append_unary_all(unary);
	}
	return r;
}

static void test_rice_trees(RiceBitvector *const r, const vector<uint64_t> &keys, const int golomb_param, const size_t tree_offset) {
	for (size_t t = 0; t < rice_test_ntrees; ++t) {
		r->read_reset(t * tree_offset, golomb_param * keys.size());
		for (size_t i = 0; i < keys.size(); ++i) {
			auto k = r->read_next(golomb_param);
			ASSERT_EQ(k, keys[i]) << "ERROR: " << k << " != " << keys[i] << endl;
		}
	}
}

TEST(RiceBitvector_test, trees_golomb_0) {
	const size_t golomb_param = 0;
	vector<uint64_t> keys = gen_keys();

	RiceBitvector *r = build_bitvector(keys, golomb_param);
	size_t tree_offset = 0;
	for (uint64_t k : keys) {
		tree_offset += 1 + (k >> golomb_param) + golomb_param;
	}

	test_rice_trees(r, keys, golomb_param, tree_offset);
	delete r;
}

TEST(RiceBitvector_test, trees_golomb_1) {
	const size_t golomb_param = 1;
	vector<uint64_t> keys = gen_keys();

	RiceBitvector *r = build_bitvector(keys, golomb_param);
	size_t tree_offset = 0;
	for (uint64_t k : keys) {
		tree_offset += 1 + (k >> golomb_param) + golomb_param;
	}

	test_rice_trees(r, keys, golomb_param, tree_offset);
	delete r;
}

TEST(RiceBitvector_test, trees_golomb_2) {
	const size_t golomb_param = 2;
	vector<uint64_t> keys = gen_keys();

	RiceBitvector *r = build_bitvector(keys, golomb_param);
	size_t tree_offset = 0;
	for (uint64_t k : keys) {
		tree_offset += 1 + (k >> golomb_param) + golomb_param;
	}

	test_rice_trees(r, keys, golomb_param, tree_offset);
	delete r;
}

TEST(RiceBitvector_test, trees_golomb_3) {
	const size_t golomb_param = 3;
	vector<uint64_t> keys = gen_keys();

	RiceBitvector *r = build_bitvector(keys, golomb_param);
	size_t tree_offset = 0;
	for (uint64_t k : keys) {
		tree_offset += 1 + (k >> golomb_param) + golomb_param;
	}

	test_rice_trees(r, keys, golomb_param, tree_offset);
	delete r;
}

TEST(RiceBitvector_test, trees_golomb_4) {
	const size_t golomb_param = 4;
	vector<uint64_t> keys = gen_keys();

	RiceBitvector *r = build_bitvector(keys, golomb_param);
	size_t tree_offset = 0;
	for (uint64_t k : keys) {
		tree_offset += 1 + (k >> golomb_param) + golomb_param;
	}

	test_rice_trees(r, keys, golomb_param, tree_offset);
	delete r;
}

TEST(RiceBitvector_test, trees_golomb_5) {
	const size_t golomb_param = 5;
	vector<uint64_t> keys = gen_keys();

	RiceBitvector *r = build_bitvector(keys, golomb_param);
	size_t tree_offset = 0;
	for (uint64_t k : keys) {
		tree_offset += 1 + (k >> golomb_param) + golomb_param;
	}

	test_rice_trees(r, keys, golomb_param, tree_offset);
	delete r;
}

TEST(RiceBitvector_test, trees_golomb_6) {
	const size_t golomb_param = 6;
	vector<uint64_t> keys = gen_keys();

	RiceBitvector *r = build_bitvector(keys, golomb_param);
	size_t tree_offset = 0;
	for (uint64_t k : keys) {
		tree_offset += 1 + (k >> golomb_param) + golomb_param;
	}

	test_rice_trees(r, keys, golomb_param, tree_offset);
	delete r;
}
