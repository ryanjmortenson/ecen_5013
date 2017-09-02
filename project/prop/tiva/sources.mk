# Set up some directories for later use
TIVA_PROP_INC_DIR=prop/tiva/inc
TIVA_PROP_SRC_DIR=prop/tiva/src
TIVA_PROP_OUT=prop/tiva/out

# Build a list of source
TIVA_PROP_SRCS+= \
	$(TIVA_PROP_SRC_DIR)/tm4c1294nczad_startup_ccs.c

# Build a list of objects and dependencies
TIVA_PROP_OBJS=$(subst src,out,$(patsubst %.S,%.o,$(patsubst %.c,%.o,$(TIVA_PROP_SRCS))))
TIVA_PROP_DEPS=$(patsubst %.o,%.d,$(TIVA_PROP_OBJS))

# Build a list of things to clean
CLEAN+=$(TIVA_PROP_OBJS) \
       $(TIVA_PROP_DEPS) \
       $(TIVA_PROP_OUT)
