#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

# gt installer
# ------------

# 1. check if downloaded
if [ ! -d "./gt" ]; then
    echo "gt is not downloaded."
    exit
fi

# 3. create folder
if [ ! -d "../src/gt" ]; then
    mkdir ./src/gt
else
    rm -rf ../src/gt
    mkdir ../src/gt
fi

# 4. copy files
cp ./gt/src/test.c ../src/gt/test.c
cp ./gt/src/test.h ../src/gt/test.h

# 5. done
echo "Installed gt."