#!/bin/bash

tag=$1
outname=$2

#declare -a Samples=(T1qqqq T2qq)
#declare -a Samples=(T1qqqq)
declare -a Samples=(T2bt)
#declare -a Samples=(T2qq)
#declare -a ctaus=(1 5 10 10val 30 50 50val 70 100 150 200 300 500 1000)
#declare -a ctaus=(1 10 10val 30 50val 70 100 150 300 500 1000) # no 5, 50, 200, for T2bt
#declare -a ctaus=(1 5 10 10val 30 50 70 100 150 200 300 500 1000) # no 50val, for T2qq
#declare -a ctaus=(1 5 10 10val 30 50 50val 70 100 150 200 300 500) # no 1000, for T1qqqq
#declare -a ctaus=(10 50 200)
#declare -a ctaus=(50 200)
declare -a ctaus=(5)
for sample in ${Samples[@]}; do
    for ctau in ${ctaus[@]}; do       
	carddir=tarred_cards_${tag}_${sample}_${ctau}
	./writeConfig.sh $carddir ${sample}_${ctau} $tag $outname
    done
done