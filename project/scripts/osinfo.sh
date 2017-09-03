#/usr/bin/env bash

# @file osinfo.sh 
#
# @brief Gathers os info placing it in a file and catting that file 
# @author Ryan Mortenson
# @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
#

echo "Operating System Type `uname -o`" > osinfo.txt
echo "Operating System Distribution:\n`lsb_release -a`" >> osinfo.txt
echo "Kernel Version: `uname -r`" >> osinfo.txt
echo "Kernel Build: `uname -v`" >> osinfo.txt
echo "Kernel GCC Version: `cat /proc/version | grep -Po "gcc version [\d\.]+ [\d]{8}"`" >> osinfo.txt
echo "Kernel Date: `uname -a | grep -Po "[a-zA-Z]+ [a-zA-Z]+ [\d]{2} [\d]{2}:[\d]{2}:[\d]{2} [a-zA-Z]* [\d]{4}"`" >> osinfo.txt
echo "Kernel Architecture: `uname -m`" >> osinfo.txt

cat osinfo.txt
