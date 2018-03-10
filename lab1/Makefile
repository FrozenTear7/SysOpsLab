LIB = libblocks

makeall: static compile

compile: main.c libblocks.a
	gcc $< -L. -libblocks -o $@

static:
	gcc -c $(LIB).c -o $(LIB).o
	ar rcs $(LIB).a $(LIB).o

shared: 
	gcc -fPIC -c $(LIB).c -o $(LIB).o
	gcc -fPIC -shared -o lib$(LIB).so $(LIB).o