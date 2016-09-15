CFLAGS=-I include/ -I hoedown/src/ -I libnucommon/ -Wall -Werror -pedantic
LIBFLAGS=libhoedown.a -Llibnucommon -lnucommon
OBJ=util.o pageList.o post.o unvo.o kg.o post.o nu.o cmds.o strlist.o
OUTPUT=nu

default: nu

debug: CFLAGS += -g -O0
debug: nu

%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(EXTRA)

nu: $(OBJ)
	$(CC) -o $(OUTPUT) $^ $(LIBFLAGS) $(CFLAGS)
	-rm -f $(OBJ)

clean:
	-rm -f $(OBJ)
	-rm -f $(OUTPUT)