# Include proprieatry tiva sources
-include prop/tiva/sources.mk

# Set tiva flags
TIVA_CFLAGS=-mthumb \
            -mcpu=cortex-m4 \
            -mfpu=fpv4-sp-d16 \
            -mfloat-abi=hard \
            -ffunction-sections \
            -fdata-sections \
            -T linker_script.ld \
            -L ./prop/tiva/linker \
            -DTIVA \
            -Dgcc \
            -I$(TIVA_PROP_INC_DIR) \
            -I./freertos/FreeRTOS/Source/include \
            -I./freertos/FreeRTOS/ \
            -I./freertos/FreeRTOS/Source/portable/GCC/ARM_CM4F \
            -I$(TIVA_WARE_LOCATION) \
            -DPART_TM4C1294NCPDT \
            -DTARGET_IS_TM4C129_RA1

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
