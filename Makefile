
CFLAGS=-g -O0 -std=c99

all: testdriver

db_internal.o:db_internal.c Linklist.c
	gcc $(CFLAG) -c db_internal.c -o db_internal.o

db_IO.o:db_IO.c  db_IO.h db_internal.c
	gcc $(CFLAGS) -c db_IO.c -o db_IO.o

Linklist.o: Linklist.c Linklist.h
	gcc $(CFLAGS) -c Linklist.c -o Linklist.o

db.o: db.c db.h Linklist.o db_IO.o db_internal.o
	gcc $(CFLAGS) -c db.c -o db.o

testdriver.o: testdriver.c
	gcc $(CFLAGS) -c testdriver.c -o testdriver.o

testdriver: testdriver.o db.o Linklist.o db_IO.o db_internal.o db_internal.o
	gcc $(CFLAGS) db.o testdriver.o Linklist.o db_IO.o db_internal.o -o testdriver

run_test:
	./testdriver

