CC: gcc -Wall

all:
	$(CC) -o main main.c -pthread
	$(CC) -o mainTmp mainTmp.c -pthread
	rm -rf

test:
	./main ./setup.txt

test2:
	./mainTmp ./setup.txt