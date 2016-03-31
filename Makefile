CC=gcc
CFLAGS=-I include/
OBJ=nu.o cmds.o util.o
OUTPUT=nu

default: nu

%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

nu: $(OBJ)
	gcc -o $(OUTPUT) $^ $(CFLAGS)

clean:
	-rm -f $(OBJ)
	-rm -f $(OUTPUT)