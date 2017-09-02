# Include proprieatry tiva sources
-include prop/tiva/sources.mk

# Set tiva flags
TIVA_CFLAGS=-mv7M4 \
						--code_state=16 \
						--float_support=FPv4SPD16 \
						-me \
						--include_path="/home/mortzula/workspace_v7/tivatest" \
						--include_path="/media/mortzula/Linux_Partition/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" \
						--define=ccs="ccs" \
						--define=PART_TM4C1294NCZAD \
						--gcc \
						--diag_warning=225 \
						--diag_wrap=off \
						--display_error_number \
						--abi=eabi \
						--preproc_with_compile

# Set up rules for proprietary tiva targets
$(TIVA_PROP_OUT)/%.o: CFLAGS+=-MD -MP
$(TIVA_PROP_OUT)/%.o: $(TIVA_PROP_SRC_DIR)/%.c
	$(BUILD_TARGET)
	$(BUILD_WITH)
	$(shell $(MK_DIR) $(TIVA_PROP_OUT))
	$(CC) $(CFLAGS) -c --output_file="$@" "$<"

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
