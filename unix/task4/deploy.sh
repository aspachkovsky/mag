#!/usr/bin/env sh

SC=5
SS=64
SEP=","
ITER=3

rmmod lexsorter.ko
make
insmod lexsorter.ko strings_count=$SC string_size=$SS separator=$SEP
gcc test.c -o test
./test -i $ITER -c $SC -s $SS ax 12 aa 44 z a bx b1 b 01 0az 1bz 1ab 1abz zz 12 aa 14 z
