obj-m+=led_module.o

ifeq ($(BUILD_DIR),)
	BUILD_DIR=/lib/modules/$(shell uname -r)/build
endif

EXTRA_CFLAGS:=-I$(src)

all:
	make -C $(BUILD_DIR) M=$(PWD) modules
	make user_space.out
clean:
	make -C $(BUILD_DIR) M=$(PWD) clean
	rm user_space.out

CC=$(CROSS_COMPILE)gcc
user_space.out: user_space.c
	 $(CC) -g -Og -I./ user_space.c -o user_space.out
