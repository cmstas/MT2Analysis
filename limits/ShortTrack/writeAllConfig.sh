#!/bin/bash

tag=$1
outname=$2

#declare -a Samples=(T1qqqq T2qq T2bt)
#declare -a Samples=(T1qqqq)
declare -a Samples=(T2bt)
#declare -a Samples=(T2qq)
#declare -a Samples=(T1qqqq T2qq)
#declare -a ctaus=(500 1000)
declare -a ctaus=(1000)
#declare -a ctaus=(1 5 10 10val 30 50 50val 70 100 150 200 300 500 1000)
#declare -a ctaus=(1 5 10 50 100 150 200 300 500 1000) # no 30 for T1qqqq yet
#declare -a ctaus=(30)
#declare -a ctaus=(1 5 10 30 100 150 200 300 500) # no 50 or 1000 for T2qq yet
#declare -a ctaus=(1 5 10) # rerun of T2qq from 10cm including 975 GeV LSP
#declare -a ctaus=(50 1000)
#declare -a ctaus=(1 10 30 50 100 150 200 300 500 1000) # no 5 yet for T2bt
#declare -a ctaus=(5)

for sample in ${Samples[@]}; do
    for ctau in ${ctaus[@]}; do       
	carddir=tarred_cards_${tag}_${sample}_${ctau}
	./writeConfig.sh $carddir ${sample}_${ctau} $tag $outname
    done
done
