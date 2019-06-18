#!/bin/bash

tag=$1

declare -a Samples=(T1qqqq T2qq T2bt)
#declare -a Samples=(T2bt)
#declare -a Samples=(T2qq)
#declare -a Samples=(T1qqqq)
declare -a ctaus=(1 5 10 30 50 100 150 200 300 500 1000)
for sample in ${Samples[@]}; do
    for ctau in ${ctaus[@]}; do
	eval "nohup python make_plot.py ${sample}_${ctau} ${tag} &> logs/${sample}_${ctau}.log &"
    done
done
