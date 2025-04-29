main: run | builds
	@ ./builds/out

nc: ./src/nc.c | builds
	@ gcc -Wall -o ./builds/nc ./src/nc.c -lncurses
	@ ./builds/nc

run: ./src/main.c | builds
	@ gcc -Wall -o ./builds/out ./src/main.c -Llib -l:libctring.a
	@ LD_LIBRARY_PATH=./lib

clean: | builds
	rm -rdf ./builds/*

builds:
	mkdir -p $@
