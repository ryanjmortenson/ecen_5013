# Include proprieatry bbb sources
-include prop/bbb/sources.mk

# Set up rules for proprietary kl25z targets
$(BBB_PROP_OUT)/%.o: CFLAGS+=-MD -MP
$(BBB_PROP_OUT)/%.o: $(BBB_PROP_SRC_DIR)/%.c
	$(BUILD_TARGET)
	$(BUILD_WITH)
	$(shell $(MK_DIR) $(BBB_PROP_OUT))
	$(CC) $(CFLAGS) -c -o "$@" "$<"

# Builds an asm file
%.asm : $(BBB_PROP_SRC_DIR)/%.c
	$(BUILD_TARGET)
	$(BUILD_WITH)
	$(CC) -S $(CFLAGS) -c -o "$@" "$<"

# Build an i file
%.i : $(BBB_PROP_SRC_DIR)/%.c
	$(BUILD_TARGET)
	$(BUILD_WITH)
	$(CC) -E $(CFLAGS) -c -o "$@" "$<"
