CC: gcc -Wall

runStaticRaport:
	./mainStatic 1000 1000 d 1 5 5000 3 500 >> raport2.txt
	./mainStatic 1000 1000 d 1 4 500 >> raport2.txt

runStaticConsole:
	./mainStatic 1000 100 d 1 2 1 3 2
	./mainStatic 1000 1000 d 1 4 500
	
static:
	$(CC) -c libblocks.c -o libblocks.o
	ar rcs liblibblocks.a libblocks.o
	$(CC) -Wl,-rpath=. -L. -o mainStatic main.c -llibblocks

shared:
	$(CC) -fPIC -c libblocks.c -o libblocks.o
	$(CC) -fPIC -shared -o liblibblocks.so libblocks.o
	$(CC) -Wl,-rpath=. -L. -o mainShared main.c -llibblocks

dynamic:
	$(CC) -fPIC -c libblocks.c -o libblocks.o
	$(CC) -Wl,-rpath=. -fPIC -shared -o libblocks.so libblocks.o
	$(CC) -L. -o mainDynamic main.c -ldl -D DLL
