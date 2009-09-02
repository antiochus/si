#!/bin/sh
#make
#./siemu_new invaders > log_newcpu.txt
#./siemu_old invaders > log_oldcpu.txt
dd if=log_newcpu.txt of=cmp0.txt bs=512k count=1 skip=8090
dd if=log_oldcpu.txt of=cmp1.txt bs=512k count=1 skip=890