# Set up some directories for later use
BBB_PROP_INC_DIR=prop/bbb/inc
BBB_PROP_SRC_DIR=prop/bbb/src
BBB_PROP_OUT=prop/bbb/out

# Build a list of source
BBB_PROP_SRCS+= \
	$(BBB_PROP_SRC_DIR)/i2c.c \
	$(BBB_PROP_SRC_DIR)/led.c

# Build a list of objects and dependencies
BBB_PROP_OBJS=$(subst src,out,$(patsubst %.S,%.o,$(patsubst %.c,%.o,$(BBB_PROP_SRCS))))
BBB_PROP_DEPS=$(patsubst %.o,%.d,$(BBB_PROP_OBJS))

# Build a list of things to clean
CLEAN+=$(BBB_PROP_OBJS) \
       $(BBB_PROP_DEPS) \
       $(BBB_PROP_OUT)
