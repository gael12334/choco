#
# Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
#

# Downloads and installs dependencies
# -----------------------------------
cd deps
./cdocs_download.sh
./cdocs_install.sh
./gt_download.sh
./gt_install.sh
cd ..
