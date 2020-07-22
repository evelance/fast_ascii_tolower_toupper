all: asciitolower_upper.o

test: test-sse2 test-no-sse2
	./test-sse2
	./test-no-sse2
	@echo Test successful

clean:
	rm -rf asciitolower_upper.o test-sse2 test-no-sse2

asciitolower_upper.o: asciitolower_upper.c Makefile
	$(CC) -std=c89 -Wall -Wextra -pedantic -O2 -c $<

test-sse2: test.cpp asciitolower_upper.o Makefile
	$(CXX) -std=c++11 -Wall -Wextra -pedantic -O2 -msse2 -o $@ $< -lstdc++

test-no-sse2: test.cpp asciitolower_upper.o Makefile
	$(CXX) -std=c++11 -Wall -Wextra -pedantic -O2 -mno-sse2 -o $@ $< -lstdc++
