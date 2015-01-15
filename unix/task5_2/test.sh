#!/usr/bin/env sh
gcc -Wall -fPIC -shared -o freezer.so freezer.c -ldl
gcc test.c -o test
TRUSTED_RANDOM="4" LD_PRELOAD=./freezer.so ./test 
