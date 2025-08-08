run: builds/out | builds
	@ ./builds/out $(ss) $(d)

memchk: memvar = -g -O0
memchk: builds/out
	@ valgrind --leak-check=yes ./builds/out

builds/out: ./src/main.cpp | builds
	@ g++ -Wall -Wextra -ggdb -Wl,-rpath=./lib -o ./builds/out ./src/main.cpp -Llib -l:libctring.so

clean: | builds
	rm -rdf ./builds/*

builds:
	mkdir -p $@
