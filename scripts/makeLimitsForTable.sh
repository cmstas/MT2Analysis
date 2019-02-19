#!/bin/bash

tag=$1

shopt -s nullglob

declare -a Samples=(10cm_1800-1700 10cm_1800-1600 10cm_1800-1400 90cm_1800-1700 90cm_1800-1600 90cm_1800-1400)

for sample in ${Samples[@]}; do
    limitdir=limits_${tag}_${sample}_Data_SR
    mkdir -p $limitdir
    pushd $limitdir
    for card in ../cards_${tag}_${sample}_Data_SR/datacard*.txt; do	
	region=${card##**datacard_}
	region=${region%.txt}
	command="combine -M AsymptoticLimits -n $region $card &>> combine.log &"
	echo $command
	eval $command
    done
    hiset=(../cards_${tag}_${sample}_Data_SR/datacard*hi*.txt)
    if [ ${#hiset[@]} -gt 0 ]; then
	echo "combining hi pt"
	combineCards.py -S ../cards_${tag}_${sample}_Data_SR/datacard*hi*.txt  > combined_hi.txt
	combine -M AsymptoticLimits -n hi combined_hi.txt &>> combine.log &
    else 
	echo "skipping hi pt"
    fi
    loset=(../cards_${tag}_${sample}_Data_SR/datacard*lo*.txt)
    if [ ${#loset[@]} -gt 0 ]; then
	echo "combining lo pt"
	command="combineCards.py -S ../cards_${tag}_${sample}_Data_SR/datacard*lo*.txt > combined_lo.txt"
	eval $command
	combine -M AsymptoticLimits -n lo combined_lo.txt &>> combine.log &
    else
	echo "skipping lo pt"
    fi
    longset=(../cards_${tag}_${sample}_Data_SR/datacard*_L_*.txt)
    if [ ${#longset[@]} -gt 0 ]; then
	echo "combining Long"
	combineCards.py -S ../cards_${tag}_${sample}_Data_SR/datacard*_L_*.txt  > combined_Long.txt
	combine -M AsymptoticLimits -n Long combined_Long.txt &>> combine.log &
    else 
	echo "skipping Long"
    fi
    echo "combining full set"
    ../combineDir.sh ../cards_${tag}_${sample}_Data_SR `pwd`
    combine -M AsymptoticLimits -n All combined.txt &>> combine.log
    popd
done
