# Script for building the kernel

make -j4
make modules -j4
sudo make modules_install
