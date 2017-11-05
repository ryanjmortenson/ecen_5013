
# Set up some directories for later use
APP_SRC_DIR=app/src
APP_INC_DIR=app/inc
APP_OUT=app/out

# Build the output names for the application
X86_APP_OUT=$(APP_OUT)/$(X86)
BBB_APP_OUT=$(APP_OUT)/$(BBB)

APP_SRC_C += \
	$(APP_SRC_DIR)/apds9301.c \
	$(APP_SRC_DIR)/circbuf.c \
	$(APP_SRC_DIR)/light.c \
	$(APP_SRC_DIR)/linkedlist.c \
	$(APP_SRC_DIR)/log.c \
	$(APP_SRC_DIR)/log_msg.c \
	$(APP_SRC_DIR)/main_task.c \
	$(APP_SRC_DIR)/workers.c \
	$(APP_SRC_DIR)/temp.c \
	$(APP_SRC_DIR)/tmp102.c

# Add mock i2c if platform isn't
ifeq ($(PLATFORM),)
APP_SRC_C += \
	$(APP_SRC_DIR)/mock_i2c.c
endif

TEST_SRC+= \
	$(NON_MAIN_SRC) \
	$(APP_SRC_DIR)/unit_circbuf.c \
	$(APP_SRC_DIR)/unit_main_task.c \
	$(APP_SRC_DIR)/unit_light.c \
	$(APP_SRC_DIR)/unit_linkedlist.c

# Make a src list without any directories to feed into the allasm/alli targets
SRC_LIST = $(subst $(APP_SRC_DIR)/,,$(APP_SRC_C))
SRC_LIST = $(subst $(APP_SRC_DIR)/,,$(APP_SRC_CPP))

# Build a list objects for each platform
X86_OBJS = $(subst src,out/$(X86),$(patsubst %.c,%.o,$(APP_SRC_C)))
BBB_OBJS = $(subst src,out/$(BBB),$(patsubst %.c,%.o,$(APP_SRC_C)))
X86_TEST_OBJS = $(subst src,out/$(X86),$(patsubst %.c,%.o,$(TEST_SRC)))
BBB_TEST_OBJS = $(subst src,out/$(BBB),$(patsubst %.c,%.o,$(TEST_SRC)))

# Build a list of .d files to clean
APP_DEPS += $(patsubst %.o,%.d, $(OBJS) $(25Z_OBJS) $(BBB_OBJS))

# Add to List of items that need to be cleaned
CLEAN+=$(BBB_OBJS) \
       $(X86_OBJS) \
       $(X86_TEST_OBJS) \
       $(BBB_TEST_OBJS) \
       $(APP_DEPS) \
       $(APP_OUT)
