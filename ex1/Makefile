static:
	gcc -c libblocks.c -o libblocks.o
	ar rcs liblibblocks.a libblocks.o

shared:
	gcc -fPIC -c libblocks.c -o libblocks.o
	gcc -fPIC -shared -o liblibblocks.so libblocks.o
