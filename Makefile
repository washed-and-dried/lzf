main: run | builds
	@ ./builds/out

run: ./src/main.c ./src/ctring.c | builds
	@ gcc -Wall -o ./builds/out ./main.c

clean: | builds
	rm -rdf ./builds/*

builds:
	mkdir -p $@
