#!/bin/sh
export QUAD_ROOT=/home/william/quadcopter/software/panda

mount /dev/mmcblk0p1 /media/sdcard0
cp $QUAD_ROOT/a9/build/m3-boot.bin /media/sdcard0
cp $QUAD_ROOT/m3/build/m3-image.bin /media/sdcard0
mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n "PandaBoard boot script" -d $QUAD_ROOT/boot.cmd $QUAD_ROOT/boot.scr
cp $QUAD_ROOT/boot.scr /media/sdcard0
umount /dev/mmcblk0p1
