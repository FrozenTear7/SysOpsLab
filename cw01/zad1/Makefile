CC: gcc -Wall

static:
	$(CC) -c libblocks.c -o libblocks.o
	ar rcs liblibblocks.a libblocks.o

shared:
	$(CC) -fPIC -c libblocks.c -o libblocks.o
	$(CC) -fPIC -shared -o liblibblocks.so libblocks.o
