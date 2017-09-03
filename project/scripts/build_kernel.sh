#/usr/bin/env bash

# @file build_kernel.sh 
#
# @brief Builds the kernel 
# @author Ryan Mortenson
# @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
#

make -j4
make modules -j4
sudo make modules_install
