#!/bin/bash

make -j 12 || return $?

LOGDIR=logs/
mkdir -p ${LOGDIR}

#tag=isoFloorSTCsWith1L_veto
tag=NoisoFloorSTCsWith1L_veto
#tag=NoisoFloorSTCsWith1L
#tag=calveto

# 2016 data
OUTDIR=output_unmerged/2016_${tag}/data
mkdir -p ${OUTDIR}
CONFIG=data_2016_94x
INDIR=/nfs-6/userdata/dpgilber/mt2babies/data_2016_leploose
declare -a Samples=(data_Run2016B data_Run2016C data_Run2016D data_Run2016E data_Run2016F data_Run2016G data_Run2016H)

#for SAMPLE in ${Samples[@]}; do
#     command="nohup nice -n 10 ./FshortLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} >& ${LOGDIR}/log_${SAMPLE}.txt &"
#     echo $command
#     eval $command
#done

#2017 MC
OUTDIR=output_unmerged/2017like16_${tag}
mkdir -p ${OUTDIR}
CONFIG=mc_94x_Fall17
INDIR=/nfs-6/userdata/dpgilber/mt2babies/mc_2017_leploose
declare -a Samples=(ttsl ttdl singletop qcd ttX dy gjets wjets zinv multiboson)

for SAMPLE in ${Samples[@]}; do
     command="nohup nice -n 10 ./FshortLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} >& ${LOGDIR}/log_${SAMPLE}.txt &"
     echo $command
     eval $command
done

return

# 2017 data
OUTDIR=output_unmerged/2017like16_${tag}/data
mkdir -p ${OUTDIR}
CONFIG=data_2017_31Mar2018
INDIR=/nfs-6/userdata/dpgilber/mt2babies/data_2017_leploose
declare -a Samples=(data_Run2017B data_Run2017C data_Run2017D data_Run2017E data_Run2017F)

for SAMPLE in ${Samples[@]}; do
     command="nohup nice -n 10 ./FshortLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} >& ${LOGDIR}/log_${SAMPLE}.txt &"
     echo $command
     eval $command
done