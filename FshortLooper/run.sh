#!/bin/bash

INDIR=/nfs-6/userdata/dpgilber/ShortTrackSkim94x/2016
CONFIG=data_2016_94x

declare -a samples=(B C D E F G H)

for i in ${samples[@]}; do
    command="nohup ./FshortLooper.exe unmerged_output/$i ${INDIR}/data_Run2016${i}.root ${CONFIG} &> logs/data_Run2016${i}.txt &"
    echo $command
    eval $command
done