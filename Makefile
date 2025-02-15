main: run | builds
	@ ./builds/out

run: ./src/main.c | builds
	@ gcc -Wall -o ./builds/out ./src/main.c -Llib -l:libctring.a
	@ LD_LIBRARY_PATH=./lib

clean: | builds
	rm -rdf ./builds/*

builds:
	mkdir -p $@
