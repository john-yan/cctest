
CFLAGS=-g -O0 -std=c99

all: testdriver

client_test.o: client_test.c
	gcc $(CFLAGS) -c client_test.c -o client_test.o

clietn_test: client_test.o
	gcc $(CFLAGS) client_test.o -o client_test

testdriver.o:testdriver.c
	gcc $(CFLAGS) -c testdriver.c -o testdriver.o

db_client.o:db_client.c
	gcc $(CFLAGS) -c db_client.c -o db_client.o

testdriver: testdriver.o db_client.o
	gcc $(CFLAGS) db_client.o testdriver.o -o testdriver

run_test:
	./testdriver

