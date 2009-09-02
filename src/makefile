CFLAGS=-Wall -Werror -I.
LDFLAGS=-lc `allegro-config --libs`
RELEASE=-O3
DEBUG=-g
SOURCE=$(wildcard *.c)
OBJECTS:= $(patsubst %.c,%.o,$(SOURCE))
OUTPUT=siemu_new siemu_old

all : $(OBJECTS)
#	$(CC) $(LDFLAGS) $(OBJECTS) -o $(OUTPUT)
	$(CC) $(LDFLAGS) cpu_new.o si.o -o siemu_new
	$(CC) $(LDFLAGS) cpu_old.o si.o -o siemu_old

release : override CFLAGS+=-O3
release : clean all;

debug : override CFLAGS+=-g -DDEBUG
debug : clean all;

clean :
	-rm $(OUTPUT) $(OBJECTS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@
