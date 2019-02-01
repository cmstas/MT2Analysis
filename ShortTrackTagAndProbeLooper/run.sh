#!/bin/bash

INDIR=/hadoop/cms/store/user/dpgilber/LostLepBabies
LOGDIR=logs
OUTDIR=outputSTs

mkdir -p $LOGDIR
mkdir -p $OUTDIR

declare -a samples=(DY)
CONFIG=mc_94x_Fall17
for sample in ${samples[@]}; do
    command="nohup nice -n 10 ./MuonHoleLooper.exe ${OUTDIR}/${sample} ${INDIR} ${sample} ${CONFIG} &> ${LOGDIR}/${sample}.txt & "
    echo $command
    eval $command
done

return

declare -a samples=(2016B 2016C 2016D 2016E 2016F 2016G 2016H)
CONFIG=data_2016_94x
for sample in ${samples[@]}; do
    command="nohup nice -n 10 ./MuonHoleLooper.exe ${OUTDIR}/${sample} ${INDIR} ${sample} ${CONFIG} &> ${LOGDIR}/${sample}.txt & "
    echo $command
    eval $command
done

declare -a samples=(2017B 2017C 2017D 2017E 2017F)
CONFIG=data_2017_31Mar2018
for sample in ${samples[@]}; do
    command="nohup nice -n 10 ./MuonHoleLooper.exe ${OUTDIR}/${sample} ${INDIR} ${sample} ${CONFIG} &> ${LOGDIR}/${sample}.txt & "
    echo $command
    eval $command
done

declare -a samples=(2018A 2018B 2018C 2018D)
CONFIG=data_2018_Prompt
for sample in ${samples[@]}; do
    command="nohup nice -n 10 ./MuonHoleLooper.exe ${OUTDIR}/${sample} ${INDIR} ${sample} ${CONFIG} &> ${LOGDIR}/${sample}.txt & "
    echo $command
    eval $command
done