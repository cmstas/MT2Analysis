#!/bin/bash

tag=$1

declare -a inputs=(data_2017_${tag}.root mc_2017_${tag}.root data_2016_${tag}.root)

for input in ${inputs[@]}; do
    python calculate_TFs.py output_merged/${input} &> /dev/null &
    python ewk_dists.py output_merged/${input} &> /dev/null &
    python ewk_contam.py output_merged/${input} &> /dev/null &
done