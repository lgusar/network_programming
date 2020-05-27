#CC = cc
CFLAGS = -Wall -g
LDFLAGS =

HTTPGET_C = httpget.c

all: httpget

httpget: $(HTTPGET_C)
	$(CC) $(HTTPGET_C) -o $@

clean:
	rm -f httpget *.o *.core
