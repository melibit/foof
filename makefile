all: comp 
	./foof test.ff

comp:
	clang++ main.cpp -std=c++20 -o foof
