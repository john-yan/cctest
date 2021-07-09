
CFLAGS=-g -Og -std=c99

all: db-server db_data_handler

db_internal.o:db_internal.c db_internal.h
	gcc $(CFLAGS) -c db_internal.c -o db_internal.o

db_IO.o:db_IO.c  db_IO.h db_internal.o 
	gcc $(CFLAGS) -c db_IO.c -o db_IO.o

Linklist.o: Linklist.c Linklist.h
	gcc $(CFLAGS) -c Linklist.c -o Linklist.o

db.o: db.c db.h Linklist.o db_IO.o 
	gcc $(CFLAGS) -c db.c -o db.o

db_server.o: db_server.c 
	gcc $(CFLAGS) -c db_server.c -o db_server.o

db-server: db_server.o db.o Linklist.o db_IO.o db_internal.o db_descriptor.o db_call_table.o 
	gcc $(CFLAGS) -pthread db.o db_server.o db_descriptor.o db_call_table.o Linklist.o db_internal.o db_IO.o -o db-server

db_descriptor.o: db_descriptor.c Linklist.o db.o db_IO.o db_internal.o
	gcc $(CFLAGS) -c db_descriptor.c -o db_descriptor.o

db_call_table.o:db_descriptor.o db_call_table.c
	gcc $(CFLAGS) -c db_call_table.c -o db_call_table.o

db_data_handler: db_data_handler.c db_call_table.o db_descriptor.o Linklist.o db_IO.o db.o db_internal.o
	gcc -g -O0 -pthread db_data_handler.c db_call_table.o db_descriptor.o Linklist.o db_IO.o db.o db_internal.o -o db_data_handler

run_test:
	./testdriver

