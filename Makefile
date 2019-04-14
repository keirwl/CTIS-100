P=ctis
CFLAGS=--std=c11 -Wall -O3 -flto -g
OBJECTS=ctis.o scanner.o node.o assembler.o debugging.o

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(P)

debug: CFLAGS=--std=c11 -Wall -Werror -O0 -g
debug: clean
debug: all

clean:
	rm -f $(P) *.o
