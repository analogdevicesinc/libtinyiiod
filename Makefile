# Extern variables
export IIO_DEFS

# Shared variables
ROOT = $(CURDIR)

# Include
include $(ROOT)/src.mk
include Makefile_iio.variables

LIB_NAME = libtinyiiod

BUILD = $(ROOT)/build
LIBRARY = $(BUILD)/$(LIB_NAME).a
SRC_DIR = $(ROOT)
INC_DIR = $(ROOT)
LIB_DIR = $(BUILD)
OBJ_DIR = $(BUILD)/obj
TST_DIR = $(BUILD)/unit_tests

CC ?= gcc
LD = $(CC)
CFLAGS ?= -Wall 							\
	  -Wmissing-field-initializers					\
	  -Wclobbered 							\
	  -Wempty-body 							\
	  -Wignored-qualifiers 						\
	  -Wmissing-parameter-type					\
	  -Wno-format  							\
	  -Wold-style-declaration					\
	  -Woverride-init 						\
	  -Wsign-compare						\
	  -Wtype-limits							\
	  -Wuninitialized						\
	  -Wunused-but-set-parameter					\
	  -Wno-unused-parameter
LDFLAGS = -L $(LIB_DIR)							\
	  -ltinyiiod

OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(SRCS:.c=.o)))

$(OBJ_DIR)/%.o:%.c
	$(CC) $(CFLAGS) $(IIO_DEFS) -I $(INC_DIR) -o $@ -c $<

$(LIBRARY):$(OBJS)
	ar rc $(LIBRARY) $(OBJS)
	ranlib $(LIBRARY)

.DEFAULT_GOAL := all	

all: build-dir $(LIBRARY)

build-dir:
	mkdir -p $(BUILD)
	mkdir -p $(OBJ_DIR)

test-dir:
	mkdir -p $(TST_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(BUILD)

utests:	build-dir test-dir $(LIBRARY)
	for utest in $(UTESTS);						\
	do								\
		$(CC) $(CFLAGS) $(IIO_DEFS) -I $(INC_DIR)		\
			-c $(SRC_DIR)/$$utest.c				\
			-o $(OBJ_DIR)/$$utest.o;			\
		$(LD) -L $(LIB_DIR) $(OBJ_DIR)/$$utest.o -ltinyiiod	\
			-o $(TST_DIR)/$$utest;				\
	done

re: fclean all
