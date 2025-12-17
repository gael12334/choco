#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

if [ ! -d "./out" ]; then
    mkdir ./out
fi
gcc -o ./out/choco_test -Werror -Wreturn-type -ggdb $(find ./src -name '*.c' -print) $(find ./tests -name '*.c' -print) -I/usr/include -L/usr/lib -static-libgcc
chmod +x ./out/choco_test
