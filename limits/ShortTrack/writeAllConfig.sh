#!/bin/bash

tag=$1

#declare -a Samples=(T1qqqq_10 T1qqqq_50 T1qqqq_200 T2qq_10 T2qq_50 T2qq_200 T2bt_10 T2bt_50 T2bt_200)
#declare -a Samples=(T1qqqq_10 T1qqqq_50 T1qqqq_200 T2qq_10 T2qq_50 T2qq_200)
declare -a Samples=(T2qq_10 T2qq_50 T2qq_200)
for sample in ${Samples[@]}; do
    carddir=tarred_cards_${tag}_${sample}
    ./writeConfig.sh $carddir $sample $tag
done