all: main set tests

main: obj/list.o obj/s7.o
	g++ -std=c++20 -g $< obj/s7.o -o $@

set: obj/set.o obj/s7.o
	g++ -std=c++20 -g $< obj/s7.o -o $@

tests: obj/tests.o obj/s7.o
	g++ -std=c++20 -g $< obj/s7.o -o $@

obj/list.o: list.cpp s7.hpp
	g++ -std=c++20 -g -c $< -o $@

obj/s7.o: s7/s7.c
	g++ -std=c++20 -g -c $< -o $@

obj/set.o: set.cpp s7.hpp
	g++ -std=c++20 -g -c $< -o $@

obj/tests.o: tests.cpp s7.hpp
	g++ -std=c++20 -g -c $< -o $@

