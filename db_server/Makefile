
CFLAGS=-g -O0 -std=c99

all: db_server

db_internal.o:db_internal.c db_internal.h
	gcc $(CFLAGS) -c db_internal.c -o db_internal.o

db_IO.o:db_IO.c  db_IO.h db_internal.c
	gcc $(CFLAGS) -c db_IO.c -o db_IO.o

Linklist.o: Linklist.c Linklist.h
	gcc $(CFLAGS) -c Linklist.c -o Linklist.o

db.o: db.c db.h Linklist.o db_IO.o 
	gcc $(CFLAGS) -c db.c -o db.o

db_server.o: db_server.c
	gcc $(CFLAGS) -c db_server.c -o db_server.o

db_server: db_server.o db.o Linklist.o db_IO.o db_internal.o
	gcc $(CFLAGS) db.o db_server.o Linklist.o db_internal.o db_IO.o -o db_server

run_test:
	./testdriver

