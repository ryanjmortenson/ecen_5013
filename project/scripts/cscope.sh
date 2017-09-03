#/usr/bin/env bash

# @file cscope.sh 
#
# @brief Builds cscope database 
# @author Ryan Mortenson
# @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
#

find . -name "*.py" -o -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.mk" -o -name "makefile" -o -name "*.S"> files.txt
cscope -i files.txt -bqU
rm files.txt
