# ##############################################################################
# Codeblock Unit-Test makefile
# ##############################################################################

TARGET		:= cbc_test

DEP_FILE	:= test.dep
OBJ_DEP		:= 

SRC			:= cbc_test.c
OBJ			:= $(SRC:%.c=%.o)

CFLAGS		:= -g -I sput
LDFLAGS		:= 


# ------------------------------------------------------------------------------
# Unit-Test
# ------------------------------------------------------------------------------

# default target is test
default: test

# build Unit-Test executable
test: OBJ_DEP := $(shell cat ${DEP_FILE})
test: $(OBJ) $(OBJ_DEP)
	$(CC) -o $(TARGET) $(OBJ) $(OBJ_DEP) $(LDFLAGS)

# build object-files
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# run test
run:
	@echo "####################################################################"
ifeq ($(OS), Windows_NT)
	$(TARGET).exe
else
	./$(TARGET)
endif
	@echo "####################################################################"


# ------------------------------------------------------------------------------
# cleanup target
# ------------------------------------------------------------------------------

ifeq ($(OS), Windows_NT)
clean: TARGET := $(TARGET).exe
endif
clean:
	$(RM) $(TARGET) $(OBJ)


.PHONY:	clean
