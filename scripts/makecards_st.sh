#!/bin/bash

tag=$1

declare -a Samples=(10cm_1800-1700 10cm_1800-1600 10cm_1800-1400 90cm_1800-1700 90cm_1800-1600 90cm_1800-1400)

for sample in ${Samples[@]}; do
    command="nohup nice -n 10 python cardMakerShortTrack.py fastsim_${sample} ${tag} cards_${tag}_${sample}_Data_SR 0 0 1 &> /dev/null &"
    eval $command
#    command="nohup nice -n 10 python cardMakerShortTrack.py fastsim_${sample} ${tag} cards_${tag}_${sample}_Data_VR 0 0 0 &> /dev/null &"
#    eval $command
#    command="nohup nice -n 10 python cardMakerShortTrack.py fastsim_${sample} ${tag} cards_${tag}_${sample}_MC_VR 0 1 0 &> /dev/null &"
#    eval $command
#    command="nohup nice -n 10 python cardMakerShortTrack.py fastsim_${sample} ${tag} cards_${tag}_${sample}_MC_SR 0 1 1 &> /dev/null &"
#    eval $command
done