CC=gcc
CFLAGS=-I include/ -I hoedown/src/ -I libnucommon/ -Lhoedown/ -lhoedown -Llibnucommon -lnucommon -Wall
#-Werror
OBJ=nu.o cmds.o util.o unvo.o kg.o post.o
OUTPUT=nu

default: nu

%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

nu: $(OBJ)
	gcc -o $(OUTPUT) $^ $(CFLAGS)
	-rm -f $(OBJ)

clean:
	-rm -f $(OBJ)
	-rm -f $(OUTPUT)