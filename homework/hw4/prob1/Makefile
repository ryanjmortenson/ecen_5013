obj-m+=led_module.o
#led_module-objs:=src/led_fops.o

BUILD_DIR=/home/mortzula/Workspace/bbblinux/lib/modules/4.13.9-bone-rt-r2/build
EXTRA_CFLAGS:=-I$(src)/inc

all:
	make -C $(BUILD_DIR) M=$(PWD) modules
clean:
	make -C $(BUILD_DIR) M=$(PWD) clean
