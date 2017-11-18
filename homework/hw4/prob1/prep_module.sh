#! /bin/bash
set -x

sudo dmesg -C
module=led_module
file_name=/dev/led_module0
REMOVE_RES=`sudo rmmod $module`
echo "Remove res ${REMOVE_RES}"
sudo insmod led_module.ko
dmesg
major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)
if [ ! -e $file_name ]; then 
  sudo mknod /dev/led_module0 c $major 0
fi
