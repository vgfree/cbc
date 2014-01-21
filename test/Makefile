# ##############################################################################
# Codeblock compiler unit-test makefile
# ##############################################################################

TARGET		:= cbc_test
SRC			:=	codeblock.c symtab.c symbol.c function.c syntree.c symref.c \
				value.c strlist.c
SRC			:= $(addprefix ../, $(SRC))
SRC_TEST	:= cbc_test.c
OBJ			:= $(SRC:%.c=%.o) $(SRC_TEST:%.c=%.o)
CFLAGS		:= -g -D _CBC_DEBUG -I ..
LDFLAGS		:= -lcheck

# default-target
build: $(OBJ)
	$(CC) -o $(TARGET) $^ $(LDFLAGS)

# build object-files
%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm $(TARGET) $(OBJ)

.PHONY: clean
