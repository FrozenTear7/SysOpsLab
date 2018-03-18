CC: gcc -Wall

runStaticRaport:
	./mainStatic 1000 1000 d 1 5 5000 3 500 >> raport3a.txt
	./mainStatic 1000 1000 d 1 4 500 >> raport3a.txt
	
runSharedRaport:
	./mainShared 1000 1000 d 1 5 5000 3 500 >> raport3a.txt
	./mainShared 1000 1000 d 1 4 500 >> raport3a.txt

runDynamicRaport:
	./mainDynamic 1000 1000 d 1 5 5000 3 500 >> raport3a.txt
	./mainDynamic 1000 1000 d 1 4 500 >> raport3a.txt

runRaports:
	./mainStatic 1000 1000 d 1 5 5000 3 500 >> raport3a.txt
	./mainStatic 1000 1000 d 1 4 500 >> raport3a.txt
	./mainShared 1000 1000 d 1 5 5000 3 500 >> raport3a.txt
	./mainShared 1000 1000 d 1 4 500 >> raport3a.txt
	./mainDynamic 1000 1000 d 1 5 5000 3 500 >> raport3a.txt
	./mainDynamic 1000 1000 d 1 4 500 >> raport3a.txt

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
	$(CC) -L. -o mainDynamic mainDynamic.c -ldl -D DLL

staticO1:
	$(CC) -c libblocks.c -o libblocks.o
	ar rcs liblibblocks.a libblocks.o
	$(CC) -O1 -Wl,-rpath=. -L. -o mainStatic main.c -llibblocks
	./mainStatic 1000 1000 d 1 5 5000 3 500 >> raport3b.txt
	./mainStatic 1000 1000 d 1 4 500 >> raport3b.txt

staticO3:
	$(CC) -c libblocks.c -o libblocks.o
	ar rcs liblibblocks.a libblocks.o
	$(CC) -O3 -Wl,-rpath=. -L. -o mainStatic main.c -llibblocks
	./mainStatic 1000 1000 d 1 5 5000 3 500 >> raport3b.txt
	./mainStatic 1000 1000 d 1 4 500 >> raport3b.txt

staticOs:
	$(CC) -c libblocks.c -o libblocks.o
	ar rcs liblibblocks.a libblocks.o
	$(CC) -Os -Wl,-rpath=. -L. -o mainStatic main.c -llibblocks
	./mainStatic 1000 1000 d 1 5 5000 3 500 >> raport3b.txt
	./mainStatic 1000 1000 d 1 4 500 >> raport3b.txt

sharedO1:
	$(CC) -fPIC -c libblocks.c -o libblocks.o
	$(CC) -Wl,-rpath=. -fPIC -shared -o libblocks.so libblocks.o
	$(CC) -O1 -L. -o mainDynamic mainDynamic.c -ldl -D DLL
	./mainShared 1000 1000 d 1 5 5000 3 500 >> raport3b.txt
	./mainShared 1000 1000 d 1 4 500 >> raport3b.txt

sharedO3:
	$(CC) -fPIC -c libblocks.c -o libblocks.o
	$(CC) -Wl,-rpath=. -fPIC -shared -o libblocks.so libblocks.o
	$(CC) -O3 -L. -o mainDynamic mainDynamic.c -ldl -D DLL
	./mainShared 1000 1000 d 1 5 5000 3 500 >> raport3b.txt
	./mainShared 1000 1000 d 1 4 500 >> raport3b.txt

sharedOs:
	$(CC) -fPIC -c libblocks.c -o libblocks.o
	$(CC) -Wl,-rpath=. -fPIC -shared -o libblocks.so libblocks.o
	$(CC) -Os -L. -o mainDynamic mainDynamic.c -ldl -D DLL
	./mainShared 1000 1000 d 1 5 5000 3 500 >> raport3b.txt
	./mainShared 1000 1000 d 1 4 500 >> raport3b.txt

dynamicO1:
	$(CC) -fPIC -c libblocks.c -o libblocks.o
	$(CC) -Wl,-rpath=. -fPIC -shared -o libblocks.so libblocks.o
	$(CC) -O1 -L. -o mainDynamic mainDynamic.c -ldl -D DLL
	./mainDynamic 1000 1000 d 1 5 5000 3 500 >> raport3b.txt
	./mainDynamic 1000 1000 d 1 4 500 >> raport3b.txt

dynamicO3:
	$(CC) -fPIC -c libblocks.c -o libblocks.o
	$(CC) -Wl,-rpath=. -fPIC -shared -o libblocks.so libblocks.o
	$(CC) -O3 -L. -o mainDynamic mainDynamic.c -ldl -D DLL
	./mainDynamic 1000 1000 d 1 5 5000 3 500 >> raport3b.txt
	./mainDynamic 1000 1000 d 1 4 500 >> raport3b.txt

dynamicOs:
	$(CC) -fPIC -c libblocks.c -o libblocks.o
	$(CC) -Wl,-rpath=. -fPIC -shared -o libblocks.so libblocks.o
	$(CC) -Os -L. -o mainDynamic mainDynamic.c -ldl -D DLL
	./mainDynamic 1000 1000 d 1 5 5000 3 500 >> raport3b.txt
	./mainDynamic 1000 1000 d 1 4 500 >> raport3b.txt