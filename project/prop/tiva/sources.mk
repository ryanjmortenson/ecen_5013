# Set up some directories for later use
TIVA_PROP_INC_DIR=prop/tiva/inc
TIVA_PROP_SRC_DIR=prop/tiva/src
TIVA_PROP_OUT=prop/tiva/out

# Build a list of source
TIVA_PROP_SRCS+= \
	$(TIVA_PROP_SRC_DIR)/mqueue_wrapper.c \
	$(TIVA_PROP_SRC_DIR)/lwip_missing.c \
	$(TIVA_PROP_SRC_DIR)/i2c.c \
	$(TIVA_PROP_SRC_DIR)/pthread_wrapper.c \
	$(TIVA_PROP_SRC_DIR)/sbrk.c \
	$(TIVA_PROP_SRC_DIR)/startup_gcc.c

# Build a list of objects and dependencies
TIVA_PROP_OBJS=$(subst src,out,$(patsubst %.S,%.o,$(patsubst %.c,%.o,$(TIVA_PROP_SRCS))))
TIVA_PROP_OBJS += $(TIVA_PROP_OUT)/queue.o \
                  $(TIVA_PROP_OUT)/lwiplib.o \
                  $(TIVA_PROP_OUT)/tasks.o \
                  $(TIVA_PROP_OUT)/list.o \
                  $(TIVA_PROP_OUT)/port.o \
                  $(TIVA_PROP_OUT)/heap_5.o
TIVA_PROP_DEPS=$(patsubst %.o,%.d,$(TIVA_PROP_OBJS))

# Build a list of things to clean
CLEAN+=$(TIVA_PROP_OBJS) \
       $(TIVA_PROP_DEPS) \
       $(TIVA_PROP_OUT)
