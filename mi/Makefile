C = cc                                                                                                                          
CFLAGS = -Wall -g
LDFLAGS =
 
RIPREQ_C = ripreq.c
 
all: ripreq 

ripreq: $(RIPREQ_C)
	$(CC) $(RIPREQ_C) -o $@
 
clean:
	rm -f ripreq *.o *.core

