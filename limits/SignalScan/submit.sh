#!/bin/bash

SAMPLE=$1
DATE=$2
MODEL=$3
COMBINED=$4

while  ! voms-proxy-info -exist
do echo "No Proxy found issuing \"voms-proxy-init -voms cms\""
   voms-proxy-init -hours 168 -voms cms 
done

#MODEL=$(echo "$SAMPLE"|awk -F- 'split($1,a,"_")&&$0=a[1]')
# MODEL=$(echo "$SAMPLE"|awk -F_ 'split($1,a,"_")&&$0=a[1]')

echo $MODEL $SAMPLE $DATE $MODEL

INPUT="job_input.tar.gz, card_tarballs/cards_${MODEL}_${DATE}/cards_$SAMPLE.tar.gz"
# SITE="T2_US_UCSD"
SITE="T2_US_UCSD,T2_US_Caltech,T3_US_UCR,T2_US_MIT,T2_US_Vanderbilt,T2_US_Wisconsin,T3_US_Baylor,T3_US_Colorado,T3_US_NotreDame,UCSB,T2_CH_CERN,T3_US_Rice,T3_US_Rutgers,T3_US_UMD,T3_US_Vanderbilt_EC2,T3_US_OSU"
PROXY=$(voms-proxy-info -path)
SUBMITLOGDIR="${PWD}/submit_logs_$MODEL"
JOBLOGDIR="/data/tmp/$USER/job_logs/mt2Limits_$MODEL"
JOBCFGDIR="${PWD}/job_cfg_$MODEL"
LOG="${SUBMITLOGDIR}/condor_submit.log"
OUT="${JOBLOGDIR}/1e.\$(Cluster).\$(Process).out"
ERR="${JOBLOGDIR}/1e.\$(Cluster).\$(Process).err"

OUTPUTDIR=/hadoop/cms/store/user/$USER/combine/limits/${MODEL}_${DATE}

if [ ! -d "${SUBMITLOGDIR}" ]; then
    mkdir -p ${SUBMITLOGDIR}
fi

if [ ! -d "${JOBLOGDIR}" ]; then
    mkdir -p ${JOBLOGDIR}
fi

if [ ! -d "${JOBCFGDIR}" ]; then
    mkdir -p ${JOBCFGDIR}
fi

if [ $COMBINED == 1 ]
then
    EXE=condorExecutableCombined.sh
else
    EXE=condorExecutable.sh
fi

echo "
universe=vanilla
when_to_transfer_output = ON_EXIT
transfer_input_files=${INPUT}
transfer_output_files = /dev/null
+DESIRED_Sites=\"${SITE}\"
+Owner = undefined
log=${LOG}
output=${OUT}
error =${ERR}
notification=Never
x509userproxy=${PROXY}
executable=${EXE}
transfer_executable=True
arguments=$SAMPLE $OUTPUTDIR 
queue
" > ${JOBCFGDIR}/condor_$SAMPLE.cmd

echo "condor_submit ${JOBCFGDIR}/condor_$SAMPLE.cmd"
condor_submit ${JOBCFGDIR}/condor_$SAMPLE.cmd

