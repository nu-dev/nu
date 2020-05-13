INCLUDES=-I include/ -I libs/include
CFLAGS=$(INCLUDES) -Wall -Werror -ansi -pedantic -Wextra
LIBFLAGS=-Llibs/ -lhoedown -lnucommon -lgoatee -llua -lstring -lm

OBJ=objs/build.o objs/cmdline.o objs/cmds.o objs/kg.o objs/pageList.o objs/post.o objs/strlist.o objs/unvo.o objs/util.o
OUTPUT=nu

default: nu

debug: CFLAGS += -g -O0 -D__DEBUG
debug: nu

objs/%.o: src/%.c
	@mkdir -p objs/
	$(CC) -c -o $@ $< $(CFLAGS) $(EXTRA)

nu: libs $(OBJ)
	$(CC) -o $(OUTPUT) $(OBJ) $(LIBFLAGS) $(CFLAGS)

.PHONY: clean
clean:
	-rm -f $(OBJ)
	-rm -f $(OUTPUT)

################################################################################
#                               LIB STUFF                                      #
################################################################################

libs: libs/libhoedown.a libs/libnucommon.a libs/libgoatee.a libs/include

.PHONY: cleanlibs
cleanlibs:
	rm -rf libs/libhoedown.a libs/libnucommon.a libs/libgoatee.a libs/include

# build hoedown
libs/libhoedown.a: libs/hoedown
	@cd libs/hoedown; \
	make; \
	cp libhoedown.a ..;

libs/hoedown:
	@mkdir -p libs
	@cd libs/; \
	git clone https://github.com/hoedown/hoedown.git

# build libnucommon
libs/libnucommon.a: libs/libnucommon
	@cd libs/libnucommon; \
	make; \
	cp libnucommon.a ..;

libs/libnucommon:
	@mkdir -p libs
	@cd libs/; \
	git clone https://github.com/nu-dev/libnucommon.git

# build goatee + libstring + liblua
libs/libgoatee.a: libs/goatee
	@cd libs/goatee; \
	make; \
	cp libgoatee.a ..; \
	cp libs/liblua.a ..; \
	cp libs/libstring.a ..

libs/goatee:
	@mkdir -p libs
	@cd libs/; \
	git clone https://github.com/ohnx/goatee.git

# copy all headers into one place, for easier access
libs/include: libs/hoedown libs/libnucommon libs/goatee
	@mkdir -p libs
	@cd libs/; \
	mkdir -p include; \
	cp hoedown/src/*.h include/; \
	cp libnucommon/*.h include/; \
	cp goatee/include/*.h include/; \
	cp goatee/libs/libstring/include/*.h include/; \
	cp goatee/libs/lua/src/*.h include/
