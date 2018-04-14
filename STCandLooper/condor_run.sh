#!/bin/bash
# HTCondor sort submission script

if [ "$#" -ne 2 ]; then
    echo "Usage: ./condor_run.sh INPUT_DIR OUTPUT_DIR_RELATIVE_TO_YOUR_HADOOP_HOME"
    exit 1
fi

while  ! voms-proxy-info -exist
do echo "No Proxy found issuing \"voms-proxy-init -voms cms\""
   voms-proxy-init -hours 168 -voms cms
done

UNIVERSE="vanilla"
EXE="wrapper.sh"
INPUT="wrapper.sh, job_input/input.tar.gz"
SITE="T2_US_UCSD"
PROXY=$(voms-proxy-info -path)
USERNAME=$(whoami)

INPUT_DIR=$1
COPYDIRBASE=$2
COPYDIR=/hadoop/cms/store/user/${USERNAME}/${COPYDIRBASE}

LOGDIR="/data/tmp/${USERNAME}/condor_submit_logs/$COPYDIRBASE"
OUTDIR="/data/tmp/${USERNAME}/condor_job_logs/$COPYDIRBASE"
LOG="${LOGDIR}/condor_`date "+%m_%d_%Y"`.log"
OUT="${OUTDIR}/1e.\$(Cluster).\$(Process).out"
ERR="${OUTDIR}/1e.\$(Cluster).\$(Process).err"

if [ ! -d "${LOGDIR}" ]; then
    echo "[sort_condor] creating log directory " ${LOGDIR}
    mkdir -p ${LOGDIR}
fi

if [ ! -d "${OUTDIR}" ]; then
    echo "[sort_condor] creating job output log directory " ${OUT}
    mkdir -p ${OUT}
fi

DIR=$PWD
rm ${DIR}/job_input/input.*
tar -hcf ${DIR}/job_input/input.tar job_input/*
gzip ${DIR}/job_input/input.tar
cd ${DIR}

echo "[sort_condor] running on files in ${INPUT_DIR}"
echo "[sort_condor] copying output to ${COPYDIR}"

if [ ! -d "${COPYDIR}" ]; then
    echo "[sort_condor] ${COPYDIR} does not exist, making it..."
    mkdir -p ${COPYDIR}
fi

Grid_Resource="condor cmssubmit-r1.t2.ucsd.edu glidein-collector.t2.ucsd.edu"
echo "
universe=${UNIVERSE}
when_to_transfer_output = ON_EXIT
#the actual executable to run is not transfered by its name.
#In fact, some sites may do weird things like renaming it and such.
transfer_input_files=${INPUT}
+DESIRED_Sites=\"${SITE}\"
+remote_DESIRED_Sites=\"T2_US_UCSD\"
+Owner = undefined
log=${LOG}
output=${OUT}
error =${ERR}
notification=Never
x509userproxy=${PROXY}
" > condor_${COPYDIRBASE##*/}.cmd
for FILE in `ls ${INPUT_DIR}/*_sorted_friend.root`; do
echo "
executable=${EXE}
transfer_executable=True
arguments=`echo ${FILE##*/} | sed 's/\.root//g'` ${FILE} ${COPYDIR}
queue
" >> condor_${COPYDIRBASE##*/}.cmd
done

echo "[sort_condor] wrote condor_${COPYDIRBASE##*/}.cmd" 