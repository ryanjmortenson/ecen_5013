# Include proprieatry tiva sources
-include prop/tiva/sources.mk

# Set tiva flags
TIVA_CFLAGS=-mthumb \
            -specs=nosys.specs \
            -mcpu=cortex-m4 \
            -mfpu=fpv4-sp-d16 \
            -mfloat-abi=hard \
            -ffunction-sections \
            -fdata-sections \
            -Tlinker_script.ld \
            -L./prop/tiva/linker \
            -DTIVA \
            -Dgcc \
            -I$(TIVA_PROP_INC_DIR) \
            -I./freertos/FreeRTOS/Source/include \
            -I./freertos/FreeRTOS/ \
            -I./freertos/FreeRTOS/Source/portable/GCC/ARM_CM4F \
            -I$(TIVA_WARE_LOCATION)/third_party/lwip-1.4.1/src/include \
            -I$(TIVA_WARE_LOCATION)/third_party/lwip-1.4.1/src/include/ipv4 \
            -I$(TIVA_WARE_LOCATION)/third_party/lwip-1.4.1/ports/tiva-tm4c129/include \
            -I$(TIVA_WARE_LOCATION)/inc \
            -I$(TIVA_WARE_LOCATION) \
            -DPART_TM4C1294NCPDT \
            -DTARGET_IS_TM4C129_RA1 \
            -DLWIP_DNS_API_DECLARE_STRUCTS \
            -DLWIP_PROVIDE_ERRNO

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
