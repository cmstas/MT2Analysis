#!/bin/bash

make -j 12 || return $?

LOGDIR=logs/splitpt
mkdir -p ${LOGDIR}

doD16=1
doM16=1
doD17=1
doM17=1
doD18=1
doM18=0
doS17=1

tag=centralMT2

# 2016 MC
if [ "$doM16" -eq "1" ]; then
    OUTDIR=output_unmerged/2016_${tag}
    mkdir -p ${OUTDIR}
    CONFIG=mc_94x_Summer16
    INDIR=/nfs-6/userdata/dpgilber/mt2babies/mc_2016_loose
    declare -a Samples=(ttsl ttdl singletop qcd ttX dy wjets zinv ww)
    
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}_2016.txt &"
	echo $command
	eval $command
    done
fi

# 2016 data
if [ "$doD16" -eq "1" ]; then
    OUTDIR=output_unmerged/2016_${tag}/data
    mkdir -p ${OUTDIR}
    CONFIG=data_2016_94x
#    INDIR=/nfs-6/userdata/dpgilber/mt2babies/data_2016_loose
#    INDIR=/nfs-6/userdata/dpgilber/mt2babies/data_2016_leploose
    INDIR=/nfs-6/userdata/mt2/V00-10-07_2016fullYear_17Jul2018_skim_ST/
    declare -a Samples=(data_Run2016B data_Run2016C data_Run2016D data_Run2016E data_Run2016F data_Run2016G data_Run2016H)
    
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}.txt &"
	echo $command
	eval $command
    done
fi


#2017 MC
if [ "$doM17" -eq "1" ]; then
    OUTDIR=output_unmerged/2017_${tag}
    mkdir -p ${OUTDIR}
    CONFIG=mc_94x_Fall17
    INDIR=/nfs-6/userdata/dpgilber/mt2babies/mc_2017_loose
    declare -a Samples=(ttsl ttdl singletop qcd ttX dy gjets wjets zinv multiboson)
    
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}_2017.txt &"
	echo $command
	eval $command
    done
fi

# 2017 data
if [ "$doD17" -eq "1" ]; then
    OUTDIR=output_unmerged/2017_${tag}/data
    mkdir -p ${OUTDIR}
    CONFIG=data_2017_31Mar2018
    INDIR=/nfs-6/userdata/mt2/V00-10-07_2017fullYear_31Mar2018_ST_skim
#    INDIR=/nfs-6/userdata/dpgilber/mt2babies/data_2017_loose
#    INDIR=/nfs-6/userdata/dpgilber/mt2babies/data_2017_leploose
    declare -a Samples=(data_Run2017B data_Run2017C data_Run2017D data_Run2017E data_Run2017F)
    
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}.txt &"
	echo $command
	eval $command
    done
fi

#2018 MC
if [ "$doM18" -eq "1" ]; then
    OUTDIR=output_unmerged/2018_${tag}
    mkdir -p ${OUTDIR}
    CONFIG=mc_18
    INDIR=/nfs-6/userdata/dpgilber/mt2babies/mc_2017_loose
    declare -a Samples=(ttsl ttdl singletop qcd ttX dy gjets wjets zinv multiboson)
    
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}_2018.txt &"
	echo $command
	eval $command
    done
fi

# 2018 data
if [ "$doD18" -eq "1" ]; then
    OUTDIR=output_unmerged/2018_${tag}/data
    mkdir -p ${OUTDIR}
    CONFIG=data_2018_Prompt
    INDIR=/nfs-6/userdata/mt2/V00-10-07_2018fullYear_17Sep2018_skim_ST/
    declare -a Samples=(data_Run2018A data_Run2018B data_Run2018C data_Run2018D)
#    declare -a Samples=(data_Run2018A data_Run2018B data_Run2018C)
    
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}.txt &"
	echo $command
	eval $command
    done
fi

#2017 Signal
if [ "$doS17" -eq "1" ]; then
    OUTDIR=output_unmerged/2017_${tag}/signal
    mkdir -p ${OUTDIR}
    CONFIG=mc_94x_Fall17
    INDIR=/nfs-6/userdata/dpgilber/ShortTrackSignals
#    declare -a Samples=(fullsim_10cm_mt2 fullsim_90cm_mt2)
    declare -a Samples=(10cm_1800-1700 10cm_1800-1600 10cm_1800-1400 10cm_1700-1600 90cm_1800-1700 90cm_1800-1600 90cm_1800-1400 90cm_1700-1600)
    
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/fastsim_${SAMPLE} ${INDIR}/fastsim_${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}_2017.txt &"
	echo $command
	eval $command
    done
fi
