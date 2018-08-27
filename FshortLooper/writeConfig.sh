#!/bin/bash

while  ! voms-proxy-info -exist
do echo "No Proxy found issuing \"voms-proxy-init -voms cms\""
   voms-proxy-init -hours 168 -voms cms
done

if [ "$#" -ne 3 ]; then
    echo "Usage: ./writeConfig.sh DATADIR TAG CONFIG"
    exit
fi

DATADIR=$1
TAG=$2
CONFIG=$3

set -e

# will exit if no files
filelist=`ls ${DATADIR}/*.root`

SUBDIR=${DATADIR##*/}

UNIVERSE="vanilla"
EXE="wrapper.sh"
INPUT="wrapper.sh, input.tar"
# can add other US sites here if desired
SITE="T2_US_UCSD,T2_US_Wisconsin,T2_US_Florida,T2_US_Nebraska,T2_US_Caltech,UCSB"
#SITE="T2_US_UCSD"
PROXY=$(voms-proxy-info -path)
USERNAME=$(whoami)

LOGDIR="/data/tmp/${USERNAME}/condor_submit_logs/FshortLooper/${TAG}/${SUBDIR}"
OUTDIR="/data/tmp/${USERNAME}/condor_job_logs/FshortLooper/${TAG}/${SUBDIR}"
LOG="${LOGDIR}/condor_`date "+%m_%d_%Y"`.log"
OUT="${OUTDIR}/1e.\$(Cluster).\$(Process).out"
ERR="${OUTDIR}/1e.\$(Cluster).\$(Process).err"

mkdir -p condor/${TAG}

if [ ! -d "${LOGDIR}" ]; then
    echo "[writeConfig] creating log directory " ${LOGDIR}
    mkdir -p ${LOGDIR}
fi

if [ ! -d "${OUTDIR}" ]; then
    echo "[writeConfig] creating job output log directory " ${OUT}
    mkdir -p ${OUT}
fi

#
# prepare input sandbox
#

DIR=$PWD

COPYDIR=/hadoop/cms/store/user/${USERNAME}/FshortLooper/${TAG}/${SUBDIR}
echo "[writeConfig] running on dataset ${DATADIR}"
echo "[writeConfig] copying output to ${COPYDIR}"

if [ ! -d "${COPYDIR}" ]; then
    echo "[writeConfig] creating job output directory " ${COPYDIR}
    mkdir -p ${COPYDIR}
fi

#
# write configuration
#

echo "
universe=${UNIVERSE}
when_to_transfer_output = ON_EXIT
#the actual executable to run is not transfered by its name.
#In fact, some sites may do weird things like renaming it and such.
transfer_input_files=${INPUT}
+DESIRED_Sites=\"${SITE}\"
+Owner = undefined
log=${LOG}
output=${OUT}
error =${ERR}
notification=Never
x509userproxy=${PROXY}
" > condor/${TAG}/condor_${SUBDIR}.cmd

    #
    # now set the rest of the arguments 
    # for each job
    # 


for FILE in $filelist; do
        echo "
executable=${EXE}
transfer_executable=True
arguments=`echo ${FILE##*/} | sed 's/\.root//g'` ${FILE} ${COPYDIR} ${TAG} ${CONFIG}
queue
" >> condor/${TAG}/condor_${SUBDIR}.cmd
    done

echo "[writeConfig] wrote condor/${TAG}/condor_${SUBDIR}.cmd" 
