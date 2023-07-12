FLAGS_G++ = -g -O2 -pedantic -std=c++17 -Wall -Wextra -Werror

all: suffix_tree main
	g++ $(FLAGS_G++) suffix_tree.o main.o -o solution

suffix_tree: suffix_tree.hpp suffix_tree.cpp
	g++ $(FLAGS_G++) -c suffix_tree.cpp -o suffix_tree.o

main: main.cpp
	g++ $(FLAGS_G++) -c main.cpp -o main.o

clean:
	rm -rf suffix_tree.o main.o solution
