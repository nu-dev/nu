INCLUDES=-I include/ -I hoedown/src/ -I libnucommon/ -I lua/
CFLAGS=$(INCLUDES) -Wall -Werror -pedantic
LIBFLAGS=libhoedown.a -Llibnucommon -lnucommon -Llua -llua -lm -ldl
OBJ=objs/util.o objs/pageList.o objs/post.o objs/luat.o objs/kg.o objs/nu.o objs/cmds.o objs/strlist.o objs/luat_file.o
OUTPUT=nu

default: nu

mkobjs:
	mkdir -p objs

debug: CFLAGS += -g -O0 -D__DEBUG
debug: nu

# requires packages liblua5.2-dev and lua5.2 (5.2 or any version)
luat_file.o: src/luat.lua
	mkdir -p objs
	lua/luac -o objs/luat_file.luac src/luat.lua
	xxd -i objs/luat_file.luac src/luat_file.c
	$(CC) -c -o luat_file.o src/luat_file.c

objs/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(EXTRA)

nu: mkobjs $(OBJ)
	$(CC) -o $(OUTPUT) $(OBJ) $(LIBFLAGS) $(CFLAGS)
	-rm -f $(OBJ) objs/luat_file.luac

clean:
	-rm -f $(OBJ) objs/luat_file.luac src/luat_file.c
	-rm -f $(OUTPUT)