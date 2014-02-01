CC=gcc
CFLAGS=-c -Wall
SOURCES=vigredis.c hash.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=vigredis.out

all: $(EXECUTABLE)


$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
