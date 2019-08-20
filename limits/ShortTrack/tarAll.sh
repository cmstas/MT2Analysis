#!/bin/bash

tag=$1

#declare -a Samples=(T2qq T2bt T1qqqq)
#declare -a Samples=(T2qq T1qqqq)
#declare -a Samples=(T1qqqq)
declare -a Samples=(T2bt)
#declare -a Samples=(T2qq)
#declare -a ctaus=(500 1000)
declare -a ctaus=(1000)
#declare -a ctaus=(1 5 10 50 100 150 200 300 500 1000) # no 30 yet, for T1qqqq
#declare -a ctaus=(30)
#declare -a ctaus=(1 5 10 30 100 150 200 300 500) # no 50 or 1000 yet, for T2qq
#declare -a ctaus=(1 5 10) # rerun of T2qq from 10cm with 975 GeV
#declare -a ctaus=(50 1000)
#declare -a ctaus=(1 10 30 50 100 150 200 300 500 1000) # no 5 yet, for T2bt
#declare -a ctaus=(5)

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
