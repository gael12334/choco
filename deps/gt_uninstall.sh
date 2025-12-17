#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

# gt uninstaller
# --------------

# 1. clean cache
if [ -d "./gt" ]; then
    rm -rf ./gt
fi

# 2. remove code
if [ -d "../src/gt" ]; then
    rm -rf ../src/gt
fi

# 3. done 
echo "Uninstalled gt."
