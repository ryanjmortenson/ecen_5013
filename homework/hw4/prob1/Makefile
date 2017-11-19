obj-m+=led_module.o
#led_module-objs:=src/led_fops.o

BUILD_DIR=/home/mortzula/Workspace/bbblinux/lib/modules/4.13.9-bone-rt-r2/build
EXTRA_CFLAGS:=-I$(src)/inc

all:
	make -C $(BUILD_DIR) M=$(PWD) modules
	make user_space.out
clean:
	make -C $(BUILD_DIR) M=$(PWD) clean
	rm user_space.out

CC=$(CROSS_COMPILE)gcc
user_space.out: user_space.c
	 $(CC) -g -Og user_space.c -o user_space.out
