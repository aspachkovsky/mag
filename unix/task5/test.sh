#!/usr/bin/env sh
gcc -Wall -fPIC -shared -o mahook.so mahook.c -ldl
gcc test.c -o test
LD_PRELOAD=./mahook.so ./test 128 2048 50 66 588 600 1024 128 4000 128
