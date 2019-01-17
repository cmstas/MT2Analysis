#!/bin/bash

if [ "$#" -eq 0 ]; then
    echo "Need a tag"
    return
fi

tag=$1

#hadd -f output_merged/data_2018_${tag}.root output_unmerged/2018_${tag}/data/*.root
#hadd -f output_merged/data_2017_${tag}.root output_unmerged/2017_${tag}/data/*.root
hadd -f output_merged/data_2017and2018_${tag}.root output_unmerged/2017_${tag}/data/*.root output_unmerged/2018_${tag}/data/*.root
hadd -f output_merged/mc_2017_${tag}.root output_unmerged/2017_${tag}/*.root
hadd -f output_merged/data_2016_${tag}.root output_unmerged/2016_${tag}/data/*.root
hadd -f output_merged/mc_2016_${tag}.root output_unmerged/2016_${tag}/*.root