#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

# cdocs installer
# ------------

# 1. check if downloaded
if [ ! -d "./cdocs" ]; then
    echo "cdocs is not downloaded."
    exit
fi

# 3. create folder
if [ ! -d "../src/cdocs" ]; then
    mkdir ../src/cdocs
else
    rm -rf ../src/cdocs
    mkdir ../src/cdocs
fi

# 4. copy files
cp ./cdocs/src/cdocs.c ../src/cdocs/cdocs.c
cp ./cdocs/src/cdocs.h ../src/cdocs/cdocs.h

# 5. done
echo "Installed cdocs."
