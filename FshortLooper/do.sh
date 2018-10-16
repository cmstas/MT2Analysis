#!/bin/bash

make -j 12 || return $?

LOGDIR=logs/
mkdir -p ${LOGDIR}

# 2016 data
OUTDIR=output_unmerged/2016_noveto/data
mkdir -p ${OUTDIR}
CONFIG=data_2016_94x
INDIR=/nfs-6/userdata/dpgilber/ShortTrackSkim94x/2016/Nlep0
declare -a Samples=(data_Run2016B data_Run2016C data_Run2016D data_Run2016E data_Run2016F data_Run2016G data_Run2016H)

for SAMPLE in ${Samples[@]}; do
     command="nohup nice -n 10 ./FshortLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} >& ${LOGDIR}/log_${SAMPLE}.txt &"
     echo $command
     eval $command
done

#2017 MC
OUTDIR=output_unmerged/2017like16_noveto
mkdir -p ${OUTDIR}
CONFIG=mc_94x_Fall17
INDIR=/nfs-6/userdata/mt2/V00-10-05_2017fullYear_31Mar2018_bugfix_shorttrack
declare -a Samples=(ttsl ttdl singletop qcd tt_w tt_zinv ttg dy gjets wjets zinv ww)
#declare -a Samples=(wjets)

for SAMPLE in ${Samples[@]}; do
     command="nohup nice -n 10 ./FshortLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} >& ${LOGDIR}/log_${SAMPLE}.txt &"
     echo $command
     eval $command
done

# 2017 data
OUTDIR=output_unmerged/2017like16_noveto/data
mkdir -p ${OUTDIR}
CONFIG=data_2017_31Mar2018
INDIR=/nfs-6/userdata/mt2/V00-10-05_2017fullYear_31Mar2018_bugfix_shorttrack
declare -a Samples=(data_Run2017B data_Run2017C data_Run2017D data_Run2017E data_Run2017F)
#declare -a Samples=(data_Run2017B data_Run2017D)

for SAMPLE in ${Samples[@]}; do
     command="nohup nice -n 10 ./FshortLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} >& ${LOGDIR}/log_${SAMPLE}.txt &"
     echo $command
     eval $command
done