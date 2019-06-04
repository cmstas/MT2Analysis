#!/bin/bash

carddir=$1

namestring=""
for i in $carddir/*.txt; do
    shortname=${i##**/}
    shortname=${shortname#datacard_}
    if [[ $shortname == *"_1_"* ]]; then
	continue
    fi
    shortname=${shortname%%_fastsim*}
    namestring+="bin_${shortname%.txt}=${i} "
done
combineCards.py -S $namestring > combined.txt 
