run: build | builds
	@ ./builds/out $(ss) $(d)

memchk: memvar = -g -O0
memchk: build
	@ valgrind --leak-check=yes ./builds/out

nc: ./src/nc.c | builds
	@ gcc -Wall -o ./builds/nc ./src/nc.c -lncurses
	@ ./builds/nc

build: ./src/main.cpp | builds
	@ g++ -Wall -Wextra -ggdb -Wl,-rpath=./lib -o ./builds/out ./src/main.cpp -Llib -l:libctring.so

clean: | builds
	rm -rdf ./builds/*

builds:
	mkdir -p $@
