LIBS = -lm
CC = gcc
CFLAGS = -g -Wall

.PHONY: default all clean tests

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	echo $(OBJECTS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

tests: $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@
	./$@ -v

clean:
	-rm -f *.o
	-rm -f tests
