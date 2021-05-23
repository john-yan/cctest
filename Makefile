
CFLAGS=-g -O0 -std=c99

all: testdriver

Linklist.o: Linklist.c Linklist.h
	gcc $(CFLAGS) -c Linklist.c -o Linklist.o

db.o: db.c db.h Linklist.o
	gcc $(CFLAGS) -c db.c -o db.o

testdriver.o: testdriver.c
	gcc $(CFLAGS) -c testdriver.c -o testdriver.o

testdriver: testdriver.o db.o Linklist.o
	gcc $(CFLAGS) db.o testdriver.o Linklist.o -o testdriver

run_test:
	./testdriver

