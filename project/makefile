###############################################################################
# See README.md for build information                                         #
###############################################################################

# Some commands used for building
RM:=rm -rf
MK_DIR:=mkdir -p
CD:=cd
MAKE_CMD=make
CMAKE_CMD=cmake

# Directory names used for building targets
OUT=out
ARM=linux-arm
X86=linux-x86

# Export variables to use in submake
export $(X86)
export $(ARM)
export $(MK_DIR)
export $(CFLAGS)

# Unit test output filename
UNIT_TEST_OUT=test.out

# CMOCKA unit test variables
CMOCKA_BUILD_DIR=cmocka-out
CMOCKA_DIR=cmocka
CMOCKA_LIB_OUT_DIR=$(CMOCKA_BUILD_DIR)/lib
CMOCKA_INC_OUT_DIR=$(CMOCKA_BUILD_DIR)/include
CMOCKA_LIB=$(CMOCKA_LIB_OUT_DIR)/libcmocka.a

# Target out directories when building
X86_OUT_DIR=$(OUT)/$(X86)
ARM_OUT_DIR=$(OUT)/$(ARM)

# Target names for 2 built types
ARM_OUT=$(ARM_OUT_DIR)/$(OUT_FILE)
X86_OUT=$(X86_OUT_DIR)/$(OUT_FILE)

# List of map files used for cleaning
MAPS=$(X86_OUT_DIR)/$(MAP_FILE) \
     $(ARM_OUT_DIR)/$(MAP_FILE)

# Call sub make files to build dependencies for each target
-include sources.mk

# Project output file names
EXERCISE_OUT_FILE=exercise3problem5.out
LIB_OUT_FILE=libexercise3problem5.a

# Set CFLAGS used by every build type
CFLAGS=-I$(APP_INC_DIR) \
       -Wall \
       -Werror \
       -std=c99 \
			 -pthread \
       -D_POSIX_C_SOURCE=199309L \
       -D_DEFAULT_SOURCE

# Set colored logs if specified
ifneq ($(COLOR_LOGS),)
	CFLAGS+=-D COLOR_LOGS
endif

# Set colored logs if specified
ifneq ($(PROBLEM),)
	CFLAGS+=-D PROBLEM=$(PROBLEM)
endif

# System log turned on
ifneq ($(SYS_LOG),)
	CFLAGS+=-D SYS_LOG
endif

# Set log level if specified otherwise set to make level
ifeq ($(LOG_LEVEL),)
	CFLAGS+=-D LOG_LEVEL=4
else
	CFLAGS+=-D LOG_LEVEL=$(LOG_LEVEL)
endif

# Set a map flag to be added CFLAGS for certain targets
MAP_FLAG=-Wl,-Map,"$@.map"

# Set of 2 compilers use
ARM_CC=arm-linux-gnueabihf-gcc
X86_CC=gcc

# Set compiler, flags, and objects based on platform type
ifneq ($(or $(findstring bbb,$(PLATFORM)),$(findstring armv7,$(shell uname -a))),)
ifeq ($(PLATFORM),bbb)
	CC=$(ARM_CC)
else ifneq ($(findstring armv7,$(shell uname -a)),)
	CC=gcc
endif
	CFLAGS+=-lrt \
          -D BBB \
          -I$(ARM_PROP_INC_DIR)
	OBJS=$(ARM_PROP_OBJS) \
       $(ARM_OBJS)
	OUT_DIR=$(ARM_APP_OUT)
else
	CC=gcc
	OBJS=$(X86_OBJS)
	TEST_OBJS=$(X86_TEST_OBJS)
	OUT_DIR=$(X86_APP_OUT)
endif

OBJS+=$(OUT_DIR)/$(patsubst %.out,%.o,$(MAKECMDGOALS))

# Create names for size, ar, and objdump binaries
SIZE=$(subst gcc,size,$(CC))
AR=$(subst gcc,ar,$(CC))
OBJDUMP=$(subst gcc,objdump,$(CC))

# If release isn't specfied build with debug symbols
ifneq ($(TYPE),release)
	CFLAGS+=-g3 -Og
else
	CFLAGS+=-O3
endif

# Setup some build strings
BUILD_TARGET=@echo "Building Target $@"
BUILD_WITH=@echo "Building with $<"

# Set PHONY for all targets that don't have outputs for tracking
.PHONY: build build-lib compile-all debug allasm alli allobjdump clean test

# Build will build project
build: $(OBJS)

# PHONY target so you don't have to type .out
test:
	$(MAKE) $(UNIT_TEST_OUT)

# Build the unit test binary
$(UNIT_TEST_OUT): CFLAGS+=-I$(CMOCKA_INC_OUT_DIR) -D UNITTEST
$(UNIT_TEST_OUT): $(CMOCKA_LIB) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o "$@" $(TEST_OBJS) $(CMOCKA_LIB)

# Build the cmocka library
$(CMOCKA_LIB):
	@echo "Installing and making CMOCKA static library"
	$(MK_DIR) $(CMOCKA_BUILD_DIR)
	$(CD) $(CMOCKA_BUILD_DIR) && \
	$(CMAKE_CMD) \
	-DCMAKE_INSTALL_PREFIX=. \
	-DCMAKE_BUILD_TYPE=debug \
	-DCMAKE_C_COMPILER=$(CC) \
	-DWITH_STATIC_LIB=ON \
	../cmocka/ && \
	$(MAKE_CMD) install

# Build will build project library
build-lib: $(OBJS)
	$(MAKE) $(LIB_OUT_FILE)

# Debug target is just used for debugging the make file
debug:
	@echo "Debug output"
	@echo $(X86_OBJS)
	@echo $(SERVER_X86_OBJS)


# Compiles the project into project binary which can be executed with ./project*.out
%.out: CFLAGS+=$(MAP_FLAG) -lpthread -lrt -lm
%.out:
%.out: $(OBJS)
	$(BUILD_TARGET)
	$(CC) $(CFLAGS) -o "$@" $(OBJS)
	$(SIZE) $@

# Build the library file for static linking
$(LIB_OUT_FILE): $(OBJS)
	$(BUILD_TARGET)
	$(AR) cr $@ $(OBJS)

# Builds all objects files but doesn't link
compile-all: $(OBJS)

# Builds an asm file
%.asm : $(APP_SRC_DIR)/%.c
	$(BUILD_TARGET)
	$(BUILD_WITH)
	$(CC) -S $(CFLAGS) -c -o "$@" "$<"

# Build all asm files
allasm :
	@echo "Building all asm files"
	$(MAKE) $(patsubst %.c,%.asm,$(SRC_LIST))

# Build an i file
%.i : $(APP_SRC_DIR)/%.c
	$(BUILD_TARGET)
	$(BUILD_WITH)
	$(CC) -E $(CFLAGS) -c -o "$@" "$<"

# Build all i files
alli :
	@echo "Building all i files"
	$(MAKE) $(patsubst %.c,%.i,$(SRC_LIST))

# Builds .o file when a .c file is specified by command line
%.o: $(APP_SRC_DIR)/%.c
	$(BUILD_TARGET)
	$(BUILD_WITH)
	$(CC) $(CFLAGS) -c -o "$@" "$<"

# Covert all object file to objdump files
allobjdump: $(OBJS) $(EXERCISE_OUT_FILE)
	@echo "Build all objdump files"
	$(MAKE) $(patsubst %.o,%.objdump,$(OBJS))
	$(MAKE) $(EXERCISE_OUT_FILE).objdump

# Do an objdump on a file
%.objdump: %.o
	$(BUILD_TARGET)
	$(BUILD_WITH)
	$(OBJDUMP) -D "$<" > "$@"

# Do an objdump on the project output file
%.out.objdump: %.out
	$(BUILD_TARGET)
	$(BUILD_WITH)
	$(OBJDUMP) -D "$<" > "$@"

# Builds .o files from .c files
$(OUT_DIR)/%.o: CFLAGS+=-MD -MP
$(OUT_DIR)/%.o: $(APP_SRC_DIR)/%.c
	$(BUILD_TARGET)
	$(BUILD_WITH)
	$(shell $(MK_DIR) $(OUT_DIR))
	$(CC) $(CFLAGS) -c -o "$@" "$<"

# List of items that need to be cleaned
CLEAN+=$(OUT) \
       $(MAPS) \
       *.i \
       *.asm \
       *.o \
       exercise*.out \
       *.map \
       *.objdump \
			 *.out \
       $(UNIT_TEST_OUT) \
       $(LIB_OUT_FILE) \
       $(CMOCKA_LIB)

# Clean should clean up all .elf, .map, .o, .d, and .S files created during
# build
clean:
	$(RM) $(CLEAN)
