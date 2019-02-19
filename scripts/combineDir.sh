#!/bin/bash

carddir=$1
outdir=$2

mkdir -p $outdir

namestring=""
for i in $carddir/*.txt; do
    shortname=${i##**/}
    shortname=${shortname#datacard_}
    shortname=${shortname%%_fastsim*}
    namestring+="${shortname%.txt}=${i} "
done
combineCards.py -S $namestring > $outdir/combined.txt 
