#CC = cc
CFLAGS = -Wall -g
LDFLAGS =

TFTPSERVER_C = tftpserver.c

all: tftpserver

tftpserver: $(TFTPSERVER_C)
	$(CC) $(TFTPSERVER_C) -o $@

clean:
	rm -f tftpserver *.o *.core
