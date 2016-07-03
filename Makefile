CC=gcc
CFLAGS=-g -I include/ -I hoedown/src/ -I libnucommon/ -Wall -pedantic -static -Werror
LIBFLAGS= -Lhoedown/ -l:libhoedown.a -Llibnucommon -lnucommon
OBJ=util.o pageList.o post.o unvo.o kg.o post.o nu.o cmds.o strlist.o
OUTPUT=nu

default: nu

%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

nu: $(OBJ)
	$(CC) -o $(OUTPUT) $^ $(CFLAGS) $(LIBFLAGS)
	-rm -f $(OBJ)

clean:
	-rm -f $(OBJ)
	-rm -f $(OUTPUT)