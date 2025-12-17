#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

# gt dowloader
# ------------

# 1. clean cache
if [ ! -d "./gt" ]; then
    mkdir ./gt
else
    rm -rf ./gt
    mkdir ./gt
fi

# 2. clone
git clone https://github.com/gael12334/gt.git

# 3. done
echo "Downloaded gt. Ready to install."