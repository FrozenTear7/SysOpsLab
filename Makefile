static:
	gcc -c libblocks.c -o libblocks.o
	ar rcs liblibblocks.a libblocks.o
	gcc -static main.c -L. -llibblocks -o mainStatic

shared:
	gcc -fPIC -c libblocks.c -o libblocks.o
	gcc -fPIC -shared -o liblibblocks.so libblocks.o
	gcc -Wl,-rpath=. -L. -o mainShared main.c -llibblocks

dynamic:
	gcc -fPIC -c libblocks.c -o libblocks.o
	gcc -Wl,-rpath=. -fPIC -shared -o liblibblocks.so libblocks.o
	gcc -L. -o mainDynamic main.c -ldl -D DLL
