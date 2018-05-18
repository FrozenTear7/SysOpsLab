CC: gcc -Wall

all:

	$(CC) -o main main.c -pthread -lm
	rm -rf

test:
	./main 5 ./mona_lisa.ascii.pgm ./filter.txt ./result.ascii.pgm

test2:
	./main 5 ./test.ascii.pgm ./filter.txt ./result.ascii.pgm

test3:
	./main 5 ./feep.ascii.pgm ./filter.txt ./result.ascii.pgm