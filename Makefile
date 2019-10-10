CC = g++ -g -std=c++17 -Wall -Wextra -O0 -march=native -lgtest -I./ -fsanitize=address -fsanitize=undefined

bin/dynamic: test/dynamic/* 
	@mkdir -p bin
	$(CC) test/dynamic/test.cpp -o bin/dynamic

bin/static: test/static/* 
	@mkdir -p bin
	$(CC) test/static/test.cpp sux/static/Rank9.cpp sux/static/Rank9Sel.cpp sux/static/SimpleSelect.cpp sux/static/SimpleSelectZero.cpp sux/static/SimpleSelectHalf.cpp sux/static/SimpleSelectZeroHalf.cpp sux/static/EliasFano.cpp sux/static/DoubleEF.cpp sux/static/RiceBitvector.cpp sux/support/SpookyV2.cpp -o bin/static

test: bin/static bin/dynamic
	./bin/static --gtest_color=yes
	./bin/dynamic --gtest_color=yes

LEAF?=8

rs: test/static/recsplit_*
	g++ -std=c++17 -I./ -O3 -DSTATS -march=native -DLEAF=$(LEAF) test/static/recsplit_dump.cpp sux/static/DoubleEF.cpp sux/static/RiceBitvector.cpp sux/support/SpookyV2.cpp -o bin/recsplit_dump_$(LEAF)
	g++ -std=c++17 -I./ -O3 -DSTATS -march=native -DLEAF=$(LEAF) test/static/recsplit_dump128.cpp sux/static/DoubleEF.cpp sux/static/RiceBitvector.cpp sux/support/SpookyV2.cpp -o bin/recsplit_dump128_$(LEAF)
	g++ -std=c++17 -I./ -O3 -DSTATS -march=native -DLEAF=$(LEAF) test/static/recsplit_load.cpp sux/static/DoubleEF.cpp sux/static/RiceBitvector.cpp sux/support/SpookyV2.cpp -o bin/recsplit_load_$(LEAF)
	g++ -std=c++17 -I./ -O3 -DSTATS -march=native -DLEAF=$(LEAF) test/static/recsplit_load128.cpp sux/static/DoubleEF.cpp sux/static/RiceBitvector.cpp sux/support/SpookyV2.cpp -o bin/recsplit_load128_$(LEAF)

.PHONY: clean

clean:
	@rm -rf ./bin

