# ##############################################################################
# Codeblock compiler "cbc" makefile
# ##############################################################################

TARGET	:= cbc
SRC_YAC	:= cbc.y
SRC_LEX	:= cbc.l
TLEX	:= $(TARGET)_lex.c
TYACC	:= $(TARGET)_parse.c
HYACC	:= $(TYACC:%.c=%.h)
SRC		:= $(TYACC) $(TLEX) syntree.c
OBJ		:= $(SRC:%.c=%.o)
CFLAGS	:= 
LDFLAGS	:= 
UNITTEST:= test.sh

# default target is debug
default: debug

# debug
debug: CFLAGS := -g -D _CBC_DEBUG -D _CBC_NOLOG
debug: build

# release
release: build

# build executable
build: $(TLEX) $(TYACC) $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)

# build objects
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# build yacc/bison output
$(TYACC): $(SRC_YAC)
	bison -y -d $^ -o $@

# build lex/flex output
$(TLEX): $(SRC_LEX)
	flex -o $@ $^

# run unit-test
test: $(UNITTEST)
	./$<

clean:
	rm $(TARGET) $(TYACC) $(HYACC) $(TLEX) $(OBJ)

.PHONY: clean
