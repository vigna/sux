#pragma once

#include "sux/static/RecSplit.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <random>

using namespace std;
using namespace sux;

using RecSplit2 = RecSplit<LEAF>;

static constexpr size_t BUCKET_SIZE_TEST = 1024;

template <class RS, typename T> static void recsplit_unit_test(RS &rs, const vector<T> &keys) {
	uint64_t *recsplit_check = (uint64_t *)calloc(keys.size(), sizeof(uint64_t));
	uint64_t i = 0;
	for (const auto &k : keys) {
		uint64_t h = rs.apply(k);
		ASSERT_EQ(0, recsplit_check[h]) << "ERROR: h(key#" << recsplit_check[h] - 1 << ") = h(key#" << i << ") = " << h << endl;
		recsplit_check[h] = ++i;
	}
	free(recsplit_check);
}

template <class RS> static void recsplit_unit_test(RS &rs, FILE *keys_fp) {
	uint64_t *recsplit_check = (uint64_t *)calloc(rs.get_keycount(), sizeof(uint64_t));
	uint64_t i = 0;
	char *key = NULL;
	size_t key_len, bsize = 0;
	while ((key_len = getline(&key, &bsize, keys_fp)) != size_t(-1)) {
		string str(key);
		uint64_t h = rs.apply(str);
		ASSERT_EQ(0, recsplit_check[h]) << "ERROR: h(key#" << recsplit_check[h] - 1 << ") = h(key#" << i << ") = " << h << endl;
		recsplit_check[h] = ++i;
	}
	if (key) free(key);
	free(recsplit_check);
}

TEST(recsplit_test, random_hash128) {
	vector<hash128_t> keys;
	for (size_t i = 0; i < NKEYS_TEST; ++i) {
		keys.push_back(hash128_t(next(), next()));
	}

	RecSplit2 rs(keys, BUCKET_SIZE_TEST);
	recsplit_unit_test(rs, keys);
}

/*TEST(recsplit_test, from_sample1) {
	FILE* keys_fp = fopen("samples/sample1.txt", "r");
	ASSERT_NE(keys_fp, nullptr) << "Sample file not found" << endl;
	RecSplit2 rs(keys_fp, BUCKET_SIZE_TEST);
	fseek(keys_fp, 0, SEEK_SET);
	recsplit_unit_test(rs, keys_fp);
	fclose(keys_fp);
}*/

TEST(recsplit_test, dump_and_load) {
	vector<hash128_t> keys;
	for (size_t i = 0; i < NKEYS_TEST; ++i) {
		keys.push_back(hash128_t(next(), next()));
	}

	RecSplit2 rs_dump(keys, BUCKET_SIZE_TEST);
	FILE *fp = fopen("test/test_dump", "w");
	rs_dump.dump(fp);
	fclose(fp);

	RecSplit2 rs_load;
	fp = fopen("test/test_dump", "r");
	rs_load.load(fp);
	fclose(fp);
	recsplit_unit_test(rs_load, keys);
	remove("test/test_dump");
}
