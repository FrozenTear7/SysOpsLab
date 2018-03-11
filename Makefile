LIB = libblocks

static:
	gcc -c $(LIB).c -o $(LIB).o
	ar rcs lib$(LIB).a $(LIB).o
	gcc -static main.c -L. -l$(LIB) -o mainStatic

shared:
	gcc -fPIC -c $(LIB).c -o $(LIB).o
	gcc -fPIC -shared -o lib$(LIB).so $(LIB).o
	gcc -Wl,-rpath=. -L. -o mainShared main.c -l$(LIB)

dynamic:
	gcc -fPIC -c $(LIB).c -o $(LIB).o
	gcc -Wl,-rpath=. -fPIC -shared -o lib$(LIB).so $(LIB).o
	gcc -L. -o mainDynamic main.c -ldl -D DLL
