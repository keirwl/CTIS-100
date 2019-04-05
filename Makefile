P=ctis
CFLAGS=-Wall -O3 -flto
OBJECTS=ctis.o scanner.o node.o assembler.o

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(P)

debug: CFLAGS=-Wall -Werror -O0 -g
debug: clean
debug: all

clean:
	rm -f $(P) *.o
