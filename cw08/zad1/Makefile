CC: gcc -Wall

all:

	$(CC) -o main main.c -pthread -lm
	rm -rf

test:
	./main 2 ./mona_lisa.ascii.pgm ./filter.txt ./result.ascii.pgm

test2:
	./main 2 ./test.ascii.pgm ./filter.txt ./result.ascii.pgm

test3:
	./main 2 ./feep.ascii.pgm ./filter.txt ./result.ascii.pgm