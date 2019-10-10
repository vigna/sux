#include <cstdio>
#include <chrono>
#include <sux/static/RecSplit.hpp>

using namespace std;

int main(int argc, char** argv) {
    if(argc < 4) {
        fprintf(stderr, "Usage: %s <keys> <bucket size> <mpfh>\n", argv[0]);
        exit(1);
    }

    FILE* keys_fp = fopen(argv[1], "r");
    const size_t bucket_size = strtoll(argv[2], NULL, 0);

    printf("Building...\n");
    auto begin = chrono::high_resolution_clock::now();
    sux::RecSplit<LEAF> rs(keys_fp, bucket_size);
    auto elapsed = chrono::duration_cast<std::chrono::nanoseconds>(chrono::high_resolution_clock::now()-begin).count();
    printf("Construction time: %.3f s, %.0f ns/key\n", elapsed * 1E-9, elapsed / (double)rs.get_keycount());

    FILE* fp = fopen(argv[3], "w");
    rs.dump(fp);
    fclose(fp);

    return 0;
}
