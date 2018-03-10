CC = gcc
LIB = library

makeall: static compile

compile: main.c
	$(CC) -std=c99 -c main.c

static:
	$(CC) -c $(LIB).c -o $(LIB).o
	ar rcs $(LIB).a $(LIB).o