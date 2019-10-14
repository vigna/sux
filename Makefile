CC = g++ -g -std=c++17 -Wall -Wextra -O0 -march=native -l gtest -I./ -fsanitize=address -fsanitize=undefined

bin/dynamic: test/dynamic/* 
	@mkdir -p bin
	$(CC) test/dynamic/test.cpp -o bin/dynamic

bin/static: test/static/* 
	@mkdir -p bin
	$(CC) test/static/test.cpp sux/bits/Rank9.cpp sux/bits/Rank9Sel.cpp sux/bits/SimpleSelect.cpp sux/bits/SimpleSelectZero.cpp sux/bits/SimpleSelectHalf.cpp sux/bits/SimpleSelectZeroHalf.cpp sux/bits/EliasFano.cpp sux/function/DoubleEF.cpp sux/function/RiceBitvector.cpp sux/support/SpookyV2.cpp -o bin/static

test: bin/static bin/dynamic
	./bin/static --gtest_color=yes
	./bin/dynamic --gtest_color=yes

LEAF?=8

recsplit: test/static/recsplit_*
	@mkdir -p bin
	g++ -std=c++17 -I./ -O3 -DSTATS -march=native -DLEAF=$(LEAF) test/static/recsplit_dump.cpp sux/function/DoubleEF.cpp sux/function/RiceBitvector.cpp sux/support/SpookyV2.cpp -o bin/recsplit_dump_$(LEAF)
	g++ -std=c++17 -I./ -O3 -DSTATS -march=native -DLEAF=$(LEAF) test/static/recsplit_dump128.cpp sux/function/DoubleEF.cpp sux/function/RiceBitvector.cpp sux/support/SpookyV2.cpp -o bin/recsplit_dump128_$(LEAF)
	g++ -std=c++17 -I./ -O3 -DSTATS -march=native -DLEAF=$(LEAF) test/static/recsplit_load.cpp sux/function/DoubleEF.cpp sux/function/RiceBitvector.cpp sux/support/SpookyV2.cpp -o bin/recsplit_load_$(LEAF)
	g++ -std=c++17 -I./ -O3 -DSTATS -march=native -DLEAF=$(LEAF) test/static/recsplit_load128.cpp sux/function/DoubleEF.cpp sux/function/RiceBitvector.cpp sux/support/SpookyV2.cpp -o bin/recsplit_load128_$(LEAF)

ranksel: test/static/ranksel_*
	@mkdir -p bin
	g++ -std=c++17 -I./ -O3 -march=native -DCLASS=SimpleSelect -DNORANKTEST -DMAX_LOG2_LONGWORDS_PER_SUBINVENTORY=0 sux/bits/Rank9.cpp sux/bits/SimpleSelect.cpp test/static/ranksel_test.cpp -o bin/testsimplesel0
	g++ -std=c++17 -I./ -O3 -march=native -DCLASS=SimpleSelect -DNORANKTEST -DMAX_LOG2_LONGWORDS_PER_SUBINVENTORY=1 sux/bits/Rank9.cpp sux/bits/SimpleSelect.cpp test/static/ranksel_test.cpp -o bin/testsimplesel1
	g++ -std=c++17 -I./ -O3 -march=native -DCLASS=SimpleSelect -DNORANKTEST -DMAX_LOG2_LONGWORDS_PER_SUBINVENTORY=2 sux/bits/Rank9.cpp sux/bits/SimpleSelect.cpp test/static/ranksel_test.cpp -o bin/testsimplesel2
	g++ -std=c++17 -I./ -O3 -march=native -DCLASS=SimpleSelect -DNORANKTEST -DMAX_LOG2_LONGWORDS_PER_SUBINVENTORY=3 sux/bits/Rank9.cpp sux/bits/SimpleSelect.cpp test/static/ranksel_test.cpp -o bin/testsimplesel3
	g++ -std=c++17 -I./ -O3 -march=native -DCLASS=SimpleSelectHalf -DNORANKTEST sux/bits/Rank9.cpp sux/bits/SimpleSelectHalf.cpp test/static/ranksel_test.cpp -o bin/testsimplehalf
	g++ -std=c++17 -I./ -O3 -march=native -DCLASS=EliasFano sux/bits/Rank9.cpp sux/bits/SimpleSelectHalf.cpp sux/bits/SimpleSelectZeroHalf.cpp sux/bits/EliasFano.cpp test/static/ranksel_test.cpp -o bin/testeliasfano
	g++ -std=c++17 -I./ -O3 -march=native -DCLASS=Rank9Sel sux/bits/Rank9Sel.cpp test/static/ranksel_test.cpp -o bin/testrank9sel

.PHONY: clean

clean:
	@rm -rf ./bin

