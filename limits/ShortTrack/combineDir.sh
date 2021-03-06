#!/bin/bash

carddir=$1

namestring=""
for i in $carddir/*.txt; do
    echo $i
    shortname=${i##**/}
    shortname=${shortname#datacard_}
    echo $shortname
    if [[ $shortname == *"SR_1_"* ]]; then
	continue
    fi
    shortname=${shortname%%_fastsim*}
    echo "bin_${shortname%.txt}=${i} "
    namestring+="bin_${shortname%.txt}=${i} "
done
echo $namestring
combineCards.py -S $namestring > combined.txt 
