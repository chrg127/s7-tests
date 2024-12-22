all: main set

main: obj/list.o obj/s7.o
	g++ -std=c++20 -g obj/list.o obj/s7.o -o $@

set: obj/set2.o obj/s7.o
	g++ -std=c++20 -g obj/set2.o obj/s7.o -o $@

obj/list.o: list.cpp s7.hpp
	g++ -std=c++20 -g -c $< -o $@

obj/s7.o: s7/s7.c
	g++ -std=c++20 -g -c $< -o $@

obj/set2.o: set2.cpp s7.hpp
	g++ -std=c++20 -g -c $< -o $@
