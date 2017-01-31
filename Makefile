INCLUDES=-I include/ -I libs/hoedown/src/ -I libs/libnucommon/ -I libs/goatee/include
INCLUDES2=$(INCLUDES) -Ilibs/goatee/libs/lua/src -Ilibs/goatee/libs/libstring/include 
CFLAGS=$(INCLUDES2) -Wall -Werror -pedantic
LIBFLAGS=-Llibs -lhoedown -lnucommon -lgoatee -llua -lstring
OBJ=objs/util.o objs/pageList.o objs/post.o objs/unvo.o objs/kg.o objs/post.o objs/nu.o objs/cmds.o objs/strlist.o
OUTPUT=nu

default: nu

debug: CFLAGS += -g -O0 -D__DEBUG
debug: nu

objs/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(EXTRA)

nu: $(OBJ)
	$(CC) -o $(OUTPUT) $^ $(LIBFLAGS) $(CFLAGS)

clean:
	-rm -f $(OBJ)
	-rm -f $(OUTPUT)
	
################################################################################
#                    LIB STUFF                                                 #
################################################################################

libs: libs/libhoedown.a libs/libnucommon.a libs/libgoatee.a

libs/libhoedown.a: libs/hoedown
	@cd libs/hoedown; \
	make; \
	cp libhoedown.a ..;

libs/hoedown:
	@mkdir -p libs
	@cd libs/; \
	git clone https://github.com/hoedown/hoedown.git

libs/libnucommon.a: libs/libnucommon
	@cd libs/libnucommon; \
	make; \
	cp libnucommon.a ..;

libs/libnucommon:
	@mkdir -p libs
	@cd libs/; \
	git clone https://github.com/nu-dev/libnucommon.git

libs/libgoatee.a: libs/goatee
	@cd libs/goatee; \
	make; \
	cp libgoatee.a ..; \
	cp libs/liblua.a ..; \
	cp libs/libstring.a ..; \

libs/goatee:
	@mkdir -p libs
	@cd libs/; \
	git clone https://github.com/ohnx/goatee.git
