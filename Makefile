all: build link

build:
		gcc *.c -c

link:
		gcc -o thready.exe *.o

debug:
		gcc -g *.c -o debug.exe

test:
		bash results.sh