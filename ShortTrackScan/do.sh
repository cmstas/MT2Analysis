#!/bin/bash

make -j 12 || return $?

doS17=1

tag=preapproval
outtag=preapproval
LOGDIR=logs/${outtag}
mkdir -p ${LOGDIR}

skim_string=_skim_ST


#2017 Signal
if [ "$doS17" -eq "1" ]; then
    OUTDIR=output_unmerged/2017_${outtag}/signal
    mkdir -p ${OUTDIR}
    CONFIG=mc_94x_fastsim_Fall17
#    INDIR=/nfs-6/userdata/dpgilber/ShortTrackSignals
    INDIR=../babymaker
    declare -a Samples=(T2qq_test)
    
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}_2017.txt &"
	echo $command
	eval $command
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE}_GENMET ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}_2017_GENMET.txt &"
	echo $command
	eval $command
    done
fi
