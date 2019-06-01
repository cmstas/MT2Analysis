#!/bin/bash

tag=$1

#declare -a Samples=(T2qq T2bt T1qqqq)
#declare -a Samples=(T2qq T1qqqq)
#declare -a Samples=(T1qqqq)
declare -a Samples=(T2bt)
#declare -a Samples=(T2qq)
#declare -a ctaus=(1 5 10 10val 30 50 50val 70 100 150 200 300 500 1000)
#declare -a ctaus=(1 10 10val 30 50val 70 100 150 300 500 1000) # no 5, 50 or 200, for T2bt
#declare -a ctaus=(1 5 10 10val 30 50 70 100 150 200 300 500 1000) # No 50val, for T2qq
#declare -a ctaus=(1 5 30 50 50val 70 100 150 200 300 500)
#declare -a ctaus=(10 50 200)
declare -a ctaus=(50 200)

for sample in ${Samples[@]}; do
    for ctau in ${ctaus[@]}; do
	outdir=tarred_cards_${tag}_${sample}_${ctau}
	mkdir -p $outdir
	carddir=cards_${tag}_${sample}_${ctau}_Data_SR
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
done
