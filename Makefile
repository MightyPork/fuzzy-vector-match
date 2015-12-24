all: main.out

main.out: main.c src/vec_match.c src/vec_match.h
	gcc -o main.out main.c src/vec_match.c -Isrc

run: main.out
	./main.out

clean:
	rm -f *.out *.elf *.lst
	cd src && rm -f *.out *.elf *.lst
