CC = g++ -g -std=c++17 -Wall -Wextra -O0 -march=native -lgtest -I./ -fsanitize=address -fsanitize=undefined

bin/dynamic: test/dynamic/* 
	@mkdir -p bin
	$(CC) test/dynamic/test.cpp -o bin/dynamic

bin/static: test/static/* 
	@mkdir -p bin
	$(CC) test/static/test.cpp sux/static/Rank9.cpp sux/static/Rank9Sel.cpp sux/static/SimpleSelect.cpp sux/static/SimpleSelectZero.cpp sux/static/SimpleSelectHalf.cpp sux/static/SimpleSelectZeroHalf.cpp sux/static/EliasFano.cpp sux/static/BalParen.cpp -o bin/static

test: bin/static bin/dynamic
	./bin/static --gtest_color=yes
	./bin/dynamic --gtest_color=yes

.PHONY: clean

clean:
	@rm -rf ./bin

