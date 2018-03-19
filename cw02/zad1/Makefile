compileMain:
	gcc -Wall main.c -o main
	rm -rf

generate:
	./main generate dane 10000 10000

copyLib1:
	./main copy dane daneCopy 10000 10000 lib
	./main copy daneCopy daneCopy2 500000 8192 lib >> wyniki.txt

copyLib2:
	./main copy dane daneCopy 10000 10000 lib
	./main copy daneCopy daneCopy2 1000000 8192 lib >> wyniki.txt