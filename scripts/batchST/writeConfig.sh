#!/bin/bash

while  ! voms-proxy-info -exist
do echo "No Proxy found issuing \"voms-proxy-init -voms cms\""
   voms-proxy-init -hours 168 -voms cms
done

TAG=$1
SAMPLE=$2
CTAU=$3

# UNIVERSE="grid"
UNIVERSE="vanilla"
EXE="wrapper.sh"
INPUT="wrapper.sh, cardMakerShortTrack.py"
# can add other US sites here if desired
SITE="T2_US_UCSD"
SUBMITLOGDIR="${PWD}/submit_logs"
JOBLOGDIR="${PWD}/job_logs"
PROXY=$(voms-proxy-info -path)
USERNAME=$(whoami)

LOGDIR="/data/tmp/$USER/condor_submit_logs/CardMaking/${TAG}_${SAMPLE}_${CTAU}"
OUTDIR="/data/tmp/$USER/condor_job_logs/CardMaking/${TAG}_${SAMPLE}_${CTAU}"
LOG="${LOGDIR}/condor_`date "+%m_%d_%Y"`.log"
OUT="${OUTDIR}/1e.\$(Cluster).\$(Process).out"
ERR="${OUTDIR}/1e.\$(Cluster).\$(Process).err"

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

COPYDIR=/hadoop/cms/store/user/${USERNAME}/SigScanCards/${TAG}
echo "[writeConfig] running on input ${TAG}"
echo "[writeConfig] copying output to ${COPYDIR}"

if [ ! -d "${COPYDIR}" ]; then
    echo "[writeConfig] creating job output directory " ${COPYDIR}
    mkdir -p ${COPYDIR}
fi

#
# write configuration
#
   
#Grid_Resource=gt2 osg-gw-6.t2.ucsd.edu:2119/jobmanager-condor
Grid_Resource="condor cmssubmit-r1.t2.ucsd.edu glidein-collector.t2.ucsd.edu"
echo "
universe=${UNIVERSE}
when_to_transfer_output = ON_EXIT
transfer_input_files=${INPUT}
#the actual executable to run is not transfered by its name.
#In fact, some sites may do weird things like renaming it and such.
+DESIRED_Sites=\"${SITE}\"
+remote_DESIRED_Sites=\"T2_US_UCSD\"
+Owner = undefined
log=${LOG}
output=${OUT}
error =${ERR}
notification=Never
x509userproxy=${PROXY}
executable=${EXE}
transfer_executable=True
arguments=${TAG} ${SAMPLE} ${CTAU} ${COPYDIR}
queue
" > condor/${TAG}_${SAMPLE}_${CTAU}.cmd

echo "[writeConfig] wrote condor/${TAG}_${SAMPLE}_${CTAU}.cmd" 
