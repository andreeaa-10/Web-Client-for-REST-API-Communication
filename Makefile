CC=gcc
CFLAGS=-I.

client: client.c requests.c helper.c buffer.c parson.c
	$(CC) -o client client.c requests.c helper.c buffer.c parson.c -Wall

run: client
	./client

clean:
	rm -f *.o client