CC=gcc
CFLAGS=-Wall -Wextra -g
LDFLAGS=-lpthread
SOURCES=main.c manager.c logica.c network.c thread.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=p.exe

all: $(SOURCES) $(EXECUTABLE)
	rm -f *.o *~

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@  $(LDFLAGS)

.o:
	$(CC) $(CFLAGS) $< -o $@
