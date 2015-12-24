all: main.out

main.out: main.c vec_match.c vec_match.h
	gcc -o main.out main.c vec_match.c -I.
