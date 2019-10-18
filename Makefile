CXXFLAGS = -g -std=c++17 -Wall -Wextra -O0 -march=native -l gtest -I./ -fsanitize=address -fsanitize=undefined

bin/bits: test/bits/* sux/bits/* sux/util/Vector.hpp sux/support/*
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) test/bits/test.cpp -o bin/bits

bin/util: test/util/* sux/util/* sux/support/*
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) test/util/test.cpp -o bin/util

bin/function: test/function/* sux/function/* sux/util/Vector.hpp sux/support/*
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) test/function/test.cpp -o bin/function

test: bin/bits bin/util bin/function
	./bin/bits --gtest_color=yes
	./bin/util --gtest_color=yes
	./bin/function --gtest_color=yes

LEAF?=8
ALLOC_TYPE?=MALLOC

recsplit: benchmark/function/recsplit_*
	@mkdir -p bin
	$(CXX) -std=c++17 -I./ -O3 -DSTATS -march=native -DLEAF=$(LEAF) -DALLOC_TYPE=$(ALLOC_TYPE) benchmark/function/recsplit_dump.cpp -o bin/recsplit_dump_$(LEAF)
	$(CXX) -std=c++17 -I./ -O3 -DSTATS -march=native -DLEAF=$(LEAF) -DALLOC_TYPE=$(ALLOC_TYPE) benchmark/function/recsplit_dump128.cpp -o bin/recsplit_dump128_$(LEAF)
	$(CXX) -std=c++17 -I./ -O3 -DSTATS -march=native -DLEAF=$(LEAF) -DALLOC_TYPE=$(ALLOC_TYPE) benchmark/function/recsplit_load.cpp -o bin/recsplit_load_$(LEAF)
	$(CXX) -std=c++17 -I./ -O3 -DSTATS -march=native -DLEAF=$(LEAF) -DALLOC_TYPE=$(ALLOC_TYPE) benchmark/function/recsplit_load128.cpp -o bin/recsplit_load128_$(LEAF)

ranksel: benchmark/bits/ranksel.cpp
	@mkdir -p bin
	$(CXX) -std=c++17 -I./ -O3 -march=native -DCLASS=SimpleSelect -DNORANKTEST -DMAX_LOG2_LONGWORDS_PER_SUBINVENTORY=0 benchmark/bits/ranksel.cpp -o bin/testsimplesel0
	$(CXX) -std=c++17 -I./ -O3 -march=native -DCLASS=SimpleSelect -DNORANKTEST -DMAX_LOG2_LONGWORDS_PER_SUBINVENTORY=1 benchmark/bits/ranksel.cpp -o bin/testsimplesel1
	$(CXX) -std=c++17 -I./ -O3 -march=native -DCLASS=SimpleSelect -DNORANKTEST -DMAX_LOG2_LONGWORDS_PER_SUBINVENTORY=2 benchmark/bits/ranksel.cpp -o bin/testsimplesel2
	$(CXX) -std=c++17 -I./ -O3 -march=native -DCLASS=SimpleSelect -DNORANKTEST -DMAX_LOG2_LONGWORDS_PER_SUBINVENTORY=3 benchmark/bits/ranksel.cpp -o bin/testsimplesel3
	$(CXX) -std=c++17 -I./ -O3 -march=native -DCLASS=SimpleSelectHalf -DNORANKTEST benchmark/bits/ranksel.cpp -o bin/testsimplehalf
	$(CXX) -std=c++17 -I./ -O3 -march=native -DCLASS=EliasFano benchmark/bits/ranksel.cpp -o bin/testeliasfano
	$(CXX) -std=c++17 -I./ -O3 -march=native -DCLASS=Rank9Sel benchmark/bits/ranksel.cpp -o bin/testrank9sel

fenwick: benchmark/util/fenwick.cpp
	@mkdir -p bin/fenwick
	$(CXX) -std=c++17 -I./ -O3 -march=native -DSET_BOUND=64 -DSET_ALLOC=MALLOC benchmark/util/fenwick.cpp -o bin/fenwick/malloc_64
	$(CXX) -std=c++17 -I./ -O3 -march=native -DSET_BOUND=64 -DSET_ALLOC=SMALLPAGE benchmark/util/fenwick.cpp -o bin/fenwick/smallpage_64
	$(CXX) -std=c++17 -I./ -O3 -march=native -DSET_BOUND=64 -DSET_ALLOC=TRANSHUGEPAGE benchmark/util/fenwick.cpp -o bin/fenwick/transhugepage_64
	$(CXX) -std=c++17 -I./ -O3 -march=native -DSET_BOUND=64 -DSET_ALLOC=FORCEHUGEPAGE benchmark/util/fenwick.cpp -o bin/fenwick/forcehugepage_64

dynranksel: benchmark/bits/dynranksel.cpp
	@mkdir -p bin/dynranksel
	g++ -std=c++17 -I./ -O3 -march=native -DSET_ALLOC=MALLOC benchmark/bits/dynranksel.cpp -o bin/dynranksel/malloc_1
	g++ -std=c++17 -I./ -O3 -march=native -DSET_ALLOC=SMALLPAGE benchmark/bits/dynranksel.cpp -o bin/dynranksel/smallpage_1
	g++ -std=c++17 -I./ -O3 -march=native -DSET_ALLOC=TRANSHUGEPAGE benchmark/bits/dynranksel.cpp -o bin/dynranksel/transhugepage_1
	g++ -std=c++17 -I./ -O3 -march=native -DSET_ALLOC=FORCEHUGEPAGE benchmark/bits/dynranksel.cpp -o bin/dynranksel/forcehugepage_1
	g++ -std=c++17 -I./ -O3 -march=native -DSET_STRIDE=16 -DSET_ALLOC=MALLOC benchmark/bits/dynranksel.cpp -o bin/dynranksel/malloc_16
	g++ -std=c++17 -I./ -O3 -march=native -DSET_STRIDE=16 -DSET_ALLOC=SMALLPAGE benchmark/bits/dynranksel.cpp -o bin/dynranksel/smallpage_16
	g++ -std=c++17 -I./ -O3 -march=native -DSET_STRIDE=16 -DSET_ALLOC=TRANSHUGEPAGE benchmark/bits/dynranksel.cpp -o bin/dynranksel/transhugepage_16
	g++ -std=c++17 -I./ -O3 -march=native -DSET_STRIDE=16 -DSET_ALLOC=FORCEHUGEPAGE benchmark/bits/dynranksel.cpp -o bin/dynranksel/forcehugepage_16

.PHONY: clean

clean:
	@rm -rf ./bin
