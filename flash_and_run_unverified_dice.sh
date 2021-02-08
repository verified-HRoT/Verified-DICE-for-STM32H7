#! /bin/bash

read -p "[Experiment] Press Enter to flash the unverified dice image binary to the device."

cd src/jtag_scripts/
./msrc-flash.sh

echo "Flash done!"
