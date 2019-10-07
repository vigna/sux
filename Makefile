CC = g++ -g -std=c++17 -Wall -Wextra -O0 -march=native -lgtest -I./

bin/dynamic: test/dynamic/* 
	@mkdir -p bin
	$(CC) test/dynamic/test.cpp -o bin/dynamic

bin/static: test/static/* 
	@mkdir -p bin
	$(CC) test/static/test.cpp sux/static/rank9.cpp sux/static/rank9sel.cpp sux/static/simple_select_zero.cpp -o bin/static

test: bin/static bin/dynamic
	./bin/static --gtest_color=yes
	./bin/dynamic --gtest_color=yes

.PHONY: clean

clean:
	@rm -rf ./bin

