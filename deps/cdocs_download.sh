#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

# cdocs dowloader
# ------------

# 1. clean cache
if [ ! -d "./cdocs" ]; then
    mkdir ./cdocs
else
    rm -rf ./cdocs
    mkdir ./cdocs
fi

# 2. clone
git clone https://github.com/gael12334/cdocs.git

# 3. done
echo "Downloaded cdocs. Ready to install."
