#!/bin/bash

mkdir -p logs

tag=$1

#declare -a Samples=(T1qqqq T2qq T2bt)
#declare -a Samples=(T1qqqq T2qq)
declare -a Samples=(T2bt)
declare -a ctaus=(1 5 10 30 50 100 150 200 300 500 1000)
#declare -a ctaus=(5)

for sample in ${Samples[@]}; do
    for ctau in ${ctaus[@]}; do
	./writeConfig.sh $tag $sample $ctau
    done
done