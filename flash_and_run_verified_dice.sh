#! /bin/bash

read -p "[Experiment] Press Enter to flash the unverified dice image binary to the device."

docker cp experiment:/home/dicestar/dicestar_artifact/experiment .
cd experiment/scripts
./flash.sh

echo "[Experiment] Flash done!"
