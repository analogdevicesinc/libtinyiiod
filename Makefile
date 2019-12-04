# Extern variables
export IIO_DEFS

# Shared variables
ROOT = $(CURDIR)

# Include
include $(ROOT)/src.mk

# Local variables
TINYIIOD_VERSION_MAJOR = 0
TINYIIOD_VERSION_MINOR = 1
TINYIIOD_VERSION = $(TINYIIOD_VERSION_MAJOR).$(TINYIIOD_VERSION_MINOR)

LIB_NAME = libtinyiiod

BUILD = $(ROOT)/build
LIBRARY = $(BUILD)/$(LIB_NAME).a
SRC_DIR = $(ROOT)
INC_DIR = $(ROOT)
LIB_DIR = $(BUILD)
OBJ_DIR = $(BUILD)/obj
TST_DIR = $(BUILD)/unit_tests

BUFF_SIZE ?= 0x1000
STD_TYPES ?= true


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
IIO_DEFS = -D TINYIIOD_VERSION_MAJOR=$(TINYIIOD_VERSION_MAJOR)		\
	   -D TINYIIOD_VERSION_MINOR=$(TINYIIOD_VERSION_MINOR)		\
	   -D TINYIIOD_VERSION_GIT=0x$(shell git rev-parse --short HEAD)\
	   -D IIOD_BUFFER_SIZE=$(BUFF_SIZE)

ifeq (true,$(strip $(STD_TYPES)))
IIO_DEFS += -D _USE_STD_INT_TYPES
endif

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
