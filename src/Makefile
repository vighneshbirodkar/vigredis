CC=gcc
CFLAGS=-c -Wall -O3 -g
SOURCES=vigredis.c dict.c hash.c list.c util.c vr_string.c  skip_list.c handler.c set.c dump.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=vigredis.out

all: $(EXECUTABLE) $(HEADERS)


$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
