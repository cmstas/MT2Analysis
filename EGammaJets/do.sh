#!/bin/bash

make -j 12 || return $?

MAXEVT=-1
OUTDIR=output/
LOGDIR=logs/
mkdir -p ${OUTDIR}
mkdir -p ${LOGDIR}

#most recent run2017
INDIR=../EGammaJetsSkim/output/
#INDIR=/nfs-6/userdata/mt2/2017_test_v4_skim/
declare -a Samples=(data_Run2017B data_Run2017C)

for SAMPLE in ${Samples[@]}; do
    eval "nohup nice -n 10 ./EGammaJets.exe ${INDIR} ${SAMPLE} ${OUTDIR} ${MAXEVT} >& ${LOGDIR}/log_${SAMPLE}.txt &"
done