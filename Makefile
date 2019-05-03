CC = g++ -g -std=c++17 -Wall -Wextra -O0 -march=native -lgtest -I./

bin/dynamic: test/dynamic/* 
	@mkdir -p bin
	$(CC) test/dynamic/test.cpp -o bin/dynamic

test: bin/dynamic
	./bin/dynamic --gtest_color=yes

.PHONY: clean

clean:
	@rm -rf ./bin

