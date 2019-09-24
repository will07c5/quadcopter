#!/bin/sh

cd $HOME/quadcopter/software/ducati
export XDCPATH="/home/william/ducati/bios_6_31_00_18/packages;/home/william/ducati/syslink/packages"

/home/william/ducati/xdctools_3_20_02_59/xdc -PD .
