# ##############################################################################
# Codeblock compiler/interpreter "cbc" makefile
# ##############################################################################

TARGET			:= cbc

SRC_LEXER		:= cbc.l
SRC_PARSER		:= cbc.y
LEXER			:= $(TARGET)_lex.c
INC_LEXER		:= $(TARGET)_lex.h
PARSER			:= $(TARGET)_parse.c
INC_PARSER		:= $(PARSER:%.c=%.h)

SRC				:= 	$(LEXER) $(PARSER) \
					codeblock.c symbol.c symtab.c function.c value.c strlist.c \
					syntree.c symref.c funccall.c funcdecl.c scope.c stack.c \
					builtin.c cblib.c
OBJ				:= $(SRC:%.c=%.o)

SRC_CBC			:= main.c $(SRC)
OBJ_CBC			:= $(SRC_CBC:%.c=%.o)

CFLAGS_COMMON	:=	-D _CBC_NOLOG \
					-D _CBC_DEFAULT_FUNC_RESULT_SYMBOL
ifeq ($(OS), Windows_NT)
CFLAGS_COMMON	:= $(CFLAGS_COMMON) -D _CBC_PLAT_WNDS
endif

CFLAGS			:=	-g -D _CBC_DEBUG $(CFLAGS_COMMON)
CFLAGS_RELEASE	:=	-D _CBC_TRACK_EXECUTION_TIME $(CFLAGS_COMMON)
LDFLAGS			:= 

LEX				:= flex
YACC			:= bison

TEST_DIR		:= test
TEST_DEP_FILE	:= $(TEST_DIR)/test.dep


# ------------------------------------------------------------------------------
# codeblock-compiler/interpreter
# ------------------------------------------------------------------------------

# default target is debug
default: debug

# source-dependencies
$(LEXER): $(SRC_LEXER)
$(PARSER): $(SRC_PARSER)
$(OBJ_CBC): $(SRC_CBC)
$(OBJ): $(SRC)

# debug-build
debug: build

# release-build
release: CFLAGS := $(CFLAGS_RELEASE)
release: build

# build codeblock-compiler executable
build: $(OBJ_CBC)
	$(CC) -o $(TARGET) $^ $(LDFLAGS)

# build object-files
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# build yacc/bison output
$(PARSER): $(SRC_PARSER)
	$(YACC) -y -d $^ -o $@

# build lex/flex output
$(LEXER): $(SRC_LEXER)
	$(LEX) --header-file=$(INC_LEXER) -o $@ $^


# ------------------------------------------------------------------------------
# codeblock-test
# ------------------------------------------------------------------------------

# build test
test: $(OBJ) make-depfile
	$(MAKE) --directory $(TEST_DIR)

# run test
runtest:
	$(MAKE) --directory $(TEST_DIR) run

# create dependencies-file for test build
make-depfile:
	@echo $(addprefix ../, $(OBJ)) > $(TEST_DEP_FILE)

# clean test
clean-test:
	$(MAKE) --directory $(TEST_DIR) clean
	$(RM) $(TEST_DEP_FILE)


# ------------------------------------------------------------------------------
# cleanup targets
# ------------------------------------------------------------------------------

# clean codeblock-compiler/interpreter
ifeq ($(OS), Windows_NT)
clean-cbc: TARGET := $(TARGET).exe
endif
clean-cbc:
	$(RM) $(TARGET) $(PARSER) $(INC_PARSER) $(LEXER) $(INC_LEXER) $(OBJ_CBC)

# default clean-target
clean: clean-cbc

# invoke all clean targets
clean-all: clean-cbc clean-test


.PHONY:	clean clean-all clean-cbc clean-test test runtest build debug release \
		default make-depfile
