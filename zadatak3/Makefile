C = cc
CFLAGS = -Wall -g
LDFLAGS =

TCPKLIJENT_C = tcpklijent.c

TCPSERVER_C = tcpserver.c

all: tcpklijent tcpserver

tcpklijent: $(TCPKLIJENT_C)
	$(CC) $(TCPKLIJENT_C) -o $@

UDP_server: $(TCPSERVER_C)
	$(CC) $(TCPERVER_C) -o $@
            
clean:
	rm -f tcpklijent tcpserver *.o *.core
