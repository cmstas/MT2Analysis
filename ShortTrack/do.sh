#!/bin/bash

make -j 12 || return $?

doD16=0
doM16=0
doD17=0
doM17=0
doD18=0
doM18=0
doS17=1

tag=FullMC
outtag=FullMCv2
LOGDIR=logs/${outtag}
mkdir -p ${LOGDIR}

skim_string=_skim_ST

# 2016 MC
if [ "$doM16" -eq "1" ]; then
    OUTDIR=output_unmerged/2016_${outtag}
    mkdir -p ${OUTDIR}
    CONFIG=mc_94x_Summer16
    INDIR=/nfs-7/userdata/mt2/V00-10-10_2016fullYear${skim_string}/extmerge
    declare -a Samples=(ttsl ttdl singletop qcd ttw ttz ttg tttt dy wjets_ht zinv ww wz)    
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}_2016.txt &"
	echo $command
	eval $command
    done
fi

# 2016 data
if [ "$doD16" -eq "1" ]; then
    OUTDIR=output_unmerged/2016_${outtag}/data
    mkdir -p ${OUTDIR}
    CONFIG=data_2016_94x
    INDIR=/nfs-7/userdata/mt2/V00-10-10_2016fullYear${skim_string}/extmerge
    declare -a Samples=(data_Run2016B data_Run2016C data_Run2016D data_Run2016E data_Run2016F data_Run2016G data_Run2016H)    
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}.txt &"
	echo $command
	eval $command
    done
fi


#2017 MC
if [ "$doM17" -eq "1" ]; then
    OUTDIR=output_unmerged/2017_${outtag}
    mkdir -p ${OUTDIR}
    CONFIG=mc_94x_Fall17
    INDIR=/nfs-7/userdata/mt2/V00-10-10_2017fullYear${skim_string}
    declare -a Samples=(ttsl ttdl singletop qcd ttw ttz tttt ttg dy gjets wjets_ht zinv ww wz)        
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}_2017.txt &"
	echo $command
	eval $command
    done
fi

# 2017 data
if [ "$doD17" -eq "1" ]; then
    OUTDIR=output_unmerged/2017_${outtag}/data
    mkdir -p ${OUTDIR}
    CONFIG=data_2017_31Mar2018
    INDIR=/nfs-7/userdata/mt2/V00-10-10_2017fullYear${skim_string}
    declare -a Samples=(data_Run2017B data_Run2017C data_Run2017D data_Run2017E data_Run2017F)
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}.txt &"
	echo $command
	eval $command
    done
fi

#2018 MC
if [ "$doM18" -eq "1" ]; then
    OUTDIR=output_unmerged/2018_${outtag}
    mkdir -p ${OUTDIR}
    CONFIG=mc_102x_Autumn18
    INDIR=/nfs-7/userdata/mt2/V00-10-10_2018fullYear${skim_string}
    declare -a Samples=(ttsl ttdl qcd ttw ttz tttt dy wjets_ht zinv ww wz) # excluding singletop due to missing/2017 duplicated samples and gjets because no events pass skim    
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}_2018.txt &"
	echo $command
	eval $command
    done
fi

# 2018 data
if [ "$doD18" -eq "1" ]; then
    OUTDIR=output_unmerged/2018_${outtag}/data
    mkdir -p ${OUTDIR}
    CONFIG=data_2018_Prompt
    INDIR=/nfs-7/userdata/mt2/V00-10-10_2018fullYear${skim_string}
    declare -a Samples=(data_Run2018A data_Run2018B data_Run2018C data_Run2018D)
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/${SAMPLE} ${INDIR}/${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}.txt &"
	echo $command
	eval $command
    done
fi

#2017 Signal
if [ "$doS17" -eq "1" ]; then
    OUTDIR=output_unmerged/2017_${outtag}/signal
    mkdir -p ${OUTDIR}
    CONFIG=mc_94x_Fall17
    INDIR=/nfs-6/userdata/dpgilber/ShortTrackSignals
    declare -a Samples=(10cm_1800-1700 10cm_1800-1600 10cm_1800-1400 90cm_1800-1700 90cm_1800-1600 90cm_1800-1400)
    
    for SAMPLE in ${Samples[@]}; do
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/fastsim_${SAMPLE} ${INDIR}/fastsim_${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}_2017.txt &"
	echo $command
	eval $command
	command="nohup nice -n 10 ./ShortTrackLooper.exe ${OUTDIR}/fastsim_${SAMPLE}_GENMET ${INDIR}/fastsim_${SAMPLE} ${CONFIG} ${tag} >& ${LOGDIR}/log_${SAMPLE}_2017_GENMET.txt &"
	echo $command
	eval $command
    done
fi
