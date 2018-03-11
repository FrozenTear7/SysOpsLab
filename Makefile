static:
	gcc -c libblocks.c -o libblocks.o
	ar rcs liblibblocks.a libblocks.o
	gcc -Wl,-rpath=. -L. -o mainStatic main.c -llibblocks

shared:
	gcc -fPIC -c libblocks.c -o libblocks.o
	gcc -fPIC -shared -o liblibblocks.so libblocks.o
	gcc -Wl,-rpath=. -L. -o mainShared main.c -llibblocks

dynamic:
	gcc -fPIC -c libblocks.c -o libblocks.o
	gcc -Wl,-rpath=. -fPIC -shared -o libblocks.so libblocks.o
	gcc -L. -o mainDynamic main.c -ldl -D DLL
