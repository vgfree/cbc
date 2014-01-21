# ##############################################################################
# Codeblock compiler unit-test makefile
# ##############################################################################

TARGET		:= cbc_test
SRC_TEST	:= cbc_test.c
SRC_CBC		:=	codeblock.c symtab.c symbol.c function.c syntree.c symref.c \
				value.c strlist.c
SRC_CBC		:= $(addprefix ../, $(SRC_CBC))
OBJ_TEST	:= $(SRC_TEST:%.c=%.o)
OBJ_ALL		:= $(OBJ_TEST) $(SRC_CBC:%.c=%.o)
CFLAGS		:= -g -D _CBC_DEBUG -I ..
LDFLAGS		:= -lcheck

# default-target
build: $(OBJ_ALL)
	$(CC) -o $(TARGET) $^ $(LDFLAGS)

# build object-files
%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

clean:
	rm $(TARGET) $(OBJ_TEST)

.PHONY: clean
