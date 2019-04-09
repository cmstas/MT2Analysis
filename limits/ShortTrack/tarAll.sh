#!/bin/bash

tag=$1

#declare -a Samples=(T1qqqq_10 T1qqqq_50 T1qqqq_200 T2qq_10 T2qq_50 T2qq_200 T2bt_10 T2bt_50 T2bt_200)
#declare -a Samples=(T1qqqq_10 T1qqqq_50 T1qqqq_200 T2qq_10 T2qq_50 T2qq_200)
#
declare -a Samples=(T2qq_10 T2qq_50 T2qq_200)
for sample in ${Samples[@]}; do
    outdir=tarred_cards_${tag}_${sample}
    mkdir -p $outdir
    carddir=cards_${tag}_${sample}_Data_SR
    pushd $carddir
    for masspoint in *; do
	if [ $masspoint == *points* ]; then
	    continue
	fi
	if [ $masspoint == *templates* ]; then
	    continue
	fi
	tar cfz ${masspoint}.tar $masspoint
	mv ${masspoint}.tar ../$outdir
    done
    popd
done
