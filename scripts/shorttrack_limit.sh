#!/bin/bash

tag=$1

shopt -s nullglob

declare -a Samples=(10cm_1800-1700 10cm_1800-1600 10cm_1800-1400 90cm_1800-1700 90cm_1800-1600 90cm_1800-1400)

limitdir=limits_shorttrack_${tag}
mkdir -p $limitdir
pushd $limitdir

for sample in ${Samples[@]}; do
    echo "combining full set"
    combineCards.py -S ../cards_${tag}_${sample}_Data_SR/datacard*.txt > combined_${sample}.txt
#    combine -M AsymptoticLimits --noFitAsimov -n _All_${sample} combined_${sample}.txt &>> combine_${sample}.log &
    combine -M AsymptoticLimits -n _All_${sample} combined_${sample}.txt &> combine_${sample}.log &
done

popd
