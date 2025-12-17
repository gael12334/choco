#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

# cdocs uninstaller
# --------------

# 1. clean cache
if [ -d "./cdocs" ]; then
    rm -rf ./cdocs
fi

# 2. remove code
if [ -d "../src/cdocs" ]; then
    rm -rf ../src/cdocs
fi

# 3. done 
echo "Uninstalled cdocs."
