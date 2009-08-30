CFLAGS=-ansi -Wall -Werror --pedantic -I.
LDFLAGS=-lc
RELEASE=-O3
DEBUG=-g
SOURCE=$(wildcard *.c)
OBJECTS:= $(patsubst %.c,%.o,$(SOURCE))
OUTPUT=siemu

all : $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(OUTPUT)

release : override CFLAGS+=-O3
release : clean all;

debug : override CFLAGS+=-g -DDEBUG
debug : clean all;

clean :
	rm $(OUTPUT) $(OBJECTS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@
