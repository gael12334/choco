#!/bin/bash

if [ ! -d "./out" ]; then
    mkdir ./out
fi
gcc -o ./out/choco_test -Werror -Wreturn-type -ggdb $(find . -name '*.c' -print) -I/usr/include -L/usr/lib -static-libgcc
chmod +x ./out/choco_test
