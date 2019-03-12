#!/bin/bash

mkdir -p logs

tag=$1

declare -a Samples=(T1qqqq_10 T1qqqq_50 T1qqqq_200 T2qq_10 T2qq_50 T2qq_200 T2bt_10 T2bt_50 T2bt_200)

for sample in ${Samples[@]}; do
    command="nohup nice -n 10 python cardMakerShortTrack.py ${sample} ${tag} cards_${tag}_${sample}_Data_SR 1 0 1 &> logs/cards_${sample}.log &"
#    command="nice -n 10 python cardMakerShortTrack.py ${sample} ${tag} cards_${tag}_${sample}_Data_SR 1 0 1"
    eval $command
done