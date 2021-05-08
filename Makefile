
CFLAGS=-g -O0 --std=c99

all: testdriver

db.o: db.c
	gcc $(CFLAGS) -c db.c -o db.o

testdriver.o: testdriver.c
	gcc $(CFLAGS) -c testdriver.c -o testdriver.o

testdriver: testdriver.o db.o
	gcc $(CFLAGS) db.o testdriver.o -o testdriver

run_test:
	./testdriver

