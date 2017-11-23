# Include proprieatry tiva sources
-include prop/tiva/sources.mk

# Set tiva flags
TIVA_CFLAGS= -mthumb \
						 -mcpu=cortex-m4 \
						 -mfpu=fpv4-sp-d16 \
						 -mfloat-abi=hard \
						 -ffunction-sections \
						 -fdata-sections \
						 -T linker_script.ld \
						 -L ./prop/tiva/linker \
						 -I$(TIVA_PROP_INC_DIR)

# Set up rules for proprietary tiva targets
$(TIVA_PROP_OUT)/%.o: CFLAGS+=-MD -MP
$(TIVA_PROP_OUT)/%.o: $(TIVA_PROP_SRC_DIR)/%.c
	$(BUILD_TARGET)
	$(BUILD_WITH)
	$(shell $(MK_DIR) $(TIVA_PROP_OUT))
	$(CC) $(CFLAGS) -c -o "$@" "$<"

$(TIVA_PROP_OUT)/%.o: $(TIVA_PROP_SRC_DIR)/%.S
	$(BUILD_TARGET)
	$(BUILD_WITH)
	$(shell $(MK_DIR) $(TIVA_PROP_OUT))
	$(CC) $(CFLAGS) -c -o "$@" "$<"

# Builds an asm file
%.asm : $(TIVA_PROP_SRC_DIR)/%.c
	$(BUILD_TARGET)
	$(BUILD_WITH)
	$(CC) -S $(CFLAGS) -c -o "$@" "$<"

# Build an i file
%.i : $(TIVA_PROP_SRC_DIR)/%.c
	$(BUILD_TARGET)
	$(BUILD_WITH)
	$(CC) -E $(CFLAGS) -c -o "$@" "$<"
