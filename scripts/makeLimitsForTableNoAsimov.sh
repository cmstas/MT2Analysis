#!/bin/bash

tag=$1

shopt -s nullglob

#declare -a Samples=(10cm_1800-1700 10cm_1800-1600 10cm_1800-1400 10cm_1700-1600 90cm_1800-1700 90cm_1800-1600 90cm_1800-1400 90cm_1700-1600)
declare -a Samples=(10cm_1800-1700 10cm_1800-1600 10cm_1800-1400 90cm_1800-1700 90cm_1800-1600 90cm_1800-1400)
#declare -a dirs=(MC_SR MC_VR Data_VR)
declare -a dirs=($2)

for dir in ${dirs[@]}; do
    for sample in ${Samples[@]}; do
	limitdir=limits_${tag}_${sample}_${dir}
	mkdir -p $limitdir
	pushd $limitdir
	for card in ../cards_${tag}_${sample}_${dir}/datacard*.txt; do	
	    region=${card##**datacard_}
	    region=${region%.txt}
	    command="combine -M AsymptoticLimits --noFitAsimov -n $region $card &>> combine.log &"
	    echo $command
	    eval $command
	done
	hiset=(../cards_${tag}_${sample}_${dir}/datacard*hi*.txt)
	if [ ${#hiset[@]} -gt 0 ]; then
	    echo "combining hi pt"
	    combineCards.py -S ../cards_${tag}_${sample}_${dir}/datacard*hi*.txt  > combined_hi.txt
	    combine -M AsymptoticLimits --noFitAsimov -n hi combined_hi.txt &>> combine.log &
	else 
	    echo "skipping hi pt"
	fi
	loset=(../cards_${tag}_${sample}_${dir}/datacard*lo*.txt)
	if [ ${#loset[@]} -gt 0 ]; then
	    echo "combining lo pt"
	    command="combineCards.py -S ../cards_${tag}_${sample}_${dir}/datacard*lo*.txt > combined_lo.txt"
	    eval $command
	    combine -M AsymptoticLimits --noFitAsimov -n lo combined_lo.txt &>> combine.log &
	else
	    echo "skipping lo pt"
	fi
	longset=(../cards_${tag}_${sample}_${dir}/datacard*_L_*.txt)
	if [ ${#longset[@]} -gt 0 ]; then
	    echo "combining Long"
	    combineCards.py -S ../cards_${tag}_${sample}_${dir}/datacard*_L_*.txt  > combined_Long.txt
	    combine -M AsymptoticLimits --noFitAsimov -n Long combined_Long.txt &>> combine.log &
	else 
	    echo "skipping Long"
	fi
	echo "combining full set"
	combineCards.py -S ../cards_${tag}_${sample}_${dir}/datacard*.txt > combined.txt &
	combine -M AsymptoticLimits --noFitAsimov -n All combined.txt &>> combine.log
	popd
    done
done
