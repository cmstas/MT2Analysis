#!/bin/bash

make -j 12 || return $?

doS17=1

tag=preapproval
outtag=preapproval
LOGDIR=logs/${outtag}
mkdir -p ${LOGDIR}

#2017 Signal
OUTDIR=output_unmerged/2017_${outtag}/signal
mkdir -p ${OUTDIR}
CONFIG=mc_94x_fastsim_Fall17
INDIR=/nfs-6/userdata/dpgilber/ShortTrackScanSkims
declare -a Samples=(T1qqqq_10 T1qqqq_50 T1qqqq_200 T2qq_10 T2qq_50 T2qq_200 T2bt_10 T2bt_50 T2bt_200)

for SAMPLE in ${Samples[@]}; do
    command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}_2017.txt &"
    echo $command
    eval $command
    command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE}_GENMET ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}_2017_GENMET.txt &"
    echo $command
    eval $command
    command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE}_ISR ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}_2017_GENMET.txt &"
    echo $command
    eval $command
done
