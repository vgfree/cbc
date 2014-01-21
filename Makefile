# ##############################################################################
# Codeblock compiler "cbc" makefile
# ##############################################################################

TARGET	:= cbc
SRC_YAC	:= cbc.y
SRC_LEX	:= cbc.l
TLEX	:= $(TARGET)_lex.c
TYACC	:= $(TARGET)_parse.c
HYACC	:= $(TYACC:%.c=%.h)
SRC		:=	$(TYACC) $(TLEX) errors.c symbol.c symtab.c function.c value.c \
			strlist.c codeblock.c syntree.c symref.c funccall.c funcdecl.c
OBJ		:= $(SRC:%.c=%.o)
CFLAGS	:= 
LDFLAGS	:= 

TEST_DIR	:= test
TEST_MAKE	:= Makefile
TEST		:= $(TEST_DIR)/$(TARGET)_test

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
runtest: $(TEST)
	@echo "Running unit-test:"
ifeq ($(OS), Windows_NT)
	$<.exe
else
	./$<
endif

# build test
test: $(TEST_DIR)/$(TEST_MAKE)
	cd $(TEST_DIR); make -f $(TEST_MAKE)

clean:
ifeq ($(OS), Windows_NT)
	rm $(TARGET).exe $(TYACC) $(HYACC) $(TLEX) $(OBJ)
else
	rm $(TARGET) $(TYACC) $(HYACC) $(TLEX) $(OBJ)
endif

clean-test: $(TEST_DIR)/$(TEST_MAKE)
	cd $(TEST_DIR); make -f $(TEST_MAKE) clean

.PHONY: clean clean-test test
