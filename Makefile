
CFLAGS=-g -O0

all: testdriver

Linklist.o: Linklist.c
	gcc $(CFLAGS) -c Linklist.c -o Linklist.o

db.o: db.c
	gcc $(CFLAGS) -c db.c -o db.o

testdriver.o: testdriver.c
	gcc $(CFLAGS) -c testdriver.c -o testdriver.o

testdriver: testdriver.o db.o Linklist.o
	gcc $(CFLAGS) db.o testdriver.o Linklist.o -o testdriver

run_test:
	./testdriver

