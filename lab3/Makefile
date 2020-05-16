#CC = cc
CFLAGS = -Wall -g
LDFLAGS =

CANDC_C = CandC.c

all: CandC

CandC: $(CANDC_C)
	$(CC) $(CANDC_C) -o $@

clean:
	rm -f CandC *.o *.core
