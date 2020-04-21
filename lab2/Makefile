#CC = cc
CFLAGS = -Wall -g
LDFLAGS =

BOT_C = bot.c

SERVER_C = server.c

all: bot server

bot: $(BOT_C)
	$(CC) $(BOT_C) -o $@

server: $(SERVER_C)
	$(CC) $(SERVER_C) -o $@
	
clean:
	rm -f bot server *.o *.core
