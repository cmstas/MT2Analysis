#!/bin/bash

tag=$1

shopt -s nullglob

declare -a Samples=(10cm_1800-1700 10cm_1800-1600 10cm_1800-1400 90cm_1800-1700 90cm_1800-1600 90cm_1800-1400)

for sample in ${Samples[@]}; do
    limitdir=fits_${tag}_${sample}_Data_SR
    mkdir -p $limitdir
    pushd $limitdir
    echo "combining full set of ${sample}"
    carddir=../cards_${tag}_${sample}_Data_SR
    eval "../combineDir.sh $carddir $(pwd)"
    combine -M FitDiagnostics -d combined.txt -n _fit_CRonly_result --toysFrequentist --saveShapes --saveWithUncertainties --setParameters mask_ch1=1 &> combine_BG.log &
    combine -M FitDiagnostics -d combined.txt -n _fit_result --toysFrequentist --saveShapes --saveWithUncertainties &> combine_SIG.log &
    combine -M FitDiagnostics -t -1 --expectSignal 0 --rMin -1 --toysFrequentist combined.txt -n _covariance --forceRecreateNLL  --saveWithUncertainties --saveOverallShapes --numToysForShapes 200 &> combined_cov.log &
    popd
done
