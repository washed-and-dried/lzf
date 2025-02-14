main: run | builds
	@ ./builds/out

run: ./src/main.c | builds
	@ gcc -Wall -o ./builds/out ./src/main.c

clean: | builds
	rm -rdf ./builds/*

builds:
	mkdir -p $@
