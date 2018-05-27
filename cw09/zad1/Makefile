CC: gcc -Wall

all:
	$(CC) -o main main.c -pthread
	$(CC) -o main2 main2.c -pthread -lrt
	rm -rf

test:
	./main ./setup.txt

test2:
	./main2 ./setup.txt