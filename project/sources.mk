
# Set up some directories for later use
APP_SRC_DIR=app/src
APP_INC_DIR=app/inc
APP_OUT=app/out

# Build the output names for the application
X86_APP_OUT=$(APP_OUT)/$(X86)
ARM_APP_OUT=$(APP_OUT)/$(ARM)

APP_SRC_C += \
	$(APP_SRC_DIR)/log.c \
	$(APP_SRC_DIR)/main.c \
	$(APP_SRC_DIR)/profiler.c \
	$(APP_SRC_DIR)/circbuf.c

# Make a src list without any directories to feed into the allasm/alli targets
SRC_LIST = $(subst $(APP_SRC_DIR)/,,$(APP_SRC_C))
SRC_LIST = $(subst $(APP_SRC_DIR)/,,$(APP_SRC_CPP))

# Build a list objects for each platform
X86_OBJS = $(subst src,out/$(X86),$(patsubst %.c,%.o,$(APP_SRC_C)))
ARM_OBJS = $(subst src,out/$(ARM),$(patsubst %.c,%.o,$(APP_SRC_C)))

# Build a list of .d files to clean
APP_DEPS += $(patsubst %.o,%.d, $(OBJS) $(25Z_OBJS) $(ARM_OBJS))

# Add to List of items that need to be cleaned
CLEAN+=$(ARM_OBJS) \
       $(X86_OBJS) \
       $(CLIENT_X86_OBJS) \
       $(CLIENT_ARM_OBJS) \
       $(SERVER_X86_OBJS) \
       $(SERVER_ARM_OBJS) \
       $(APP_DEPS) \
       $(TEST_OBJS) \
       $(APP_OUT)
