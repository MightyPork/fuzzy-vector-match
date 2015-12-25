CFLAGS      += -Os -ggdb -std=gnu99 -Wfatal-errors
CFLAGS      += -Wall -Wextra -Wshadow
CFLAGS      += -Wwrite-strings -Wold-style-definition -Winline -Wmissing-noreturn -Wstrict-prototypes
CFLAGS      += -Wredundant-decls -Wfloat-equal -Wsign-compare
CFLAGS      += -Wunused-function
CFLAGS    += -MD -lm

all: main.out

main.out: main.c src/vec_match.c src/vec_match.h
	gcc $(CFLAGS) -o main.out main.c src/vec_match.c -Isrc

run: main.out
	./main.out

clean:
	rm -f *.out *.elf *.lst
	cd src && rm -f *.out *.elf *.lst
