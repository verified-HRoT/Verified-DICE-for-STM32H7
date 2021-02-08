#! /bin/bash

read -p "[Experiment] Press Enter to build the verified dice image binary."

docker build . -t dice-star:experiment
docker rm -f experiment
docker run --name 'experiment' -dit dice-star:experiment 
docker exec -it experiment /home/dicestar/dicestar_artifact/experiment/experiment_verified.sh
