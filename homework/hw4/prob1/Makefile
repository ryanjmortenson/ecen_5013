obj-m+=led_module.o

ifeq ($(BUILD_DIR),)
	BUILD_DIR=/lib/modules/$(shell uname -r)/build
endif

EXTRA_CFLAGS:=-I$(src)

all:
	make -C $(BUILD_DIR) M=$(PWD) modules
	make user_space.out
	make user_space.o
	make client_socket.out
	make server_socket.out
	make server_socket.x86.out

clean:
	make -C $(BUILD_DIR) M=$(PWD) clean
	rm -rf user_space.out
	rm -rf user_space.x86.o
	rm -rf user_space.arm.o
	rm -rf client_socket.out
	rm -rf server_socket.out
	rm -rf server_socket.x86.out

CC=$(CROSS_COMPILE)gcc
user_space.out: user_space.c
	 $(CC) -g -Og -I./ user_space.c -o user_space.out

user_space.o: user_space.c
	 gcc -g -c -Og -DNO_MAIN -I./ user_space.c -o user_space.x86.o
	 $(CC) -g -c -Og -DNO_MAIN -I./ user_space.c -o user_space.arm.o

client_socket.out: client_socket.c
	 gcc -g -Og -I./ client_socket.c -o client_socket.out user_space.x86.o

server_socket.out: server_socket.c
	 $(CC) -g -Og -I./ server_socket.c -o server_socket.out  user_space.arm.o

server_socket.x86.out: server_socket.c
	 gcc -g -Og -I./ server_socket.c -o server_socket.x86.out  user_space.x86.o
