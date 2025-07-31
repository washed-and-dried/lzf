main: run | builds
	@ ./builds/out $(ss) $(d)

nc: ./src/nc.c | builds
	@ gcc -Wall -o ./builds/nc ./src/nc.c -lncurses
	@ ./builds/nc

run: ./src/main.cpp | builds
	@ g++ -Wall -Wextra -Wl,-rpath=./lib -o ./builds/out ./src/main.cpp -Llib -l:libctring.so

clean: | builds
	rm -rdf ./builds/*

builds:
	mkdir -p $@
