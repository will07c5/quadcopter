#!/bin/sh

scp -r $HOME/work/quadcopter william@192.168.7.135:
ssh william@192.168.7.135 quadcopter/software/ducati/build_linux.sh
scp william@192.168.7.135:quadcopter/software/ducati/quadcopter.xem3 .
