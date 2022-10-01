all: p1

List.o: List.c List.h
	@gcc -c List.c

p1.o: p1.c List.h
	@gcc -c p1.c

p1: p1.o List.o
	@gcc -std=c99 -g -Wall -pedantic -Werror -o p1 p1.o List.c

clean:
	@rm p1 *.o
