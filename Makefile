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
OBJ_DIR = $(BUILD)/obj

CC ?= gcc
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

IIO_DEFS = -D TINYIIOD_VERSION_MAJOR=$(TINYIIOD_VERSION_MAJOR)		\
	   -D TINYIIOD_VERSION_MINOR=$(TINYIIOD_VERSION_MINOR)		\
	   -D TINYIIOD_VERSION_GIT=$(shell git rev-parse --short HEAD)	\
	   -D IIOD_BUFFER_SIZE=0x1000					\
	   -D _USE_STD_INT_TYPES

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

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(BUILD)

re: fclean all
