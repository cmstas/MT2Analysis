#!/bin/bash

#!/bin/bash

while  ! voms-proxy-info -exist
do echo "No Proxy found issuing \"voms-proxy-init -voms cms\""
   voms-proxy-init -hours 168 -voms cms
done

DATADIR=$1
COPYDIRBASE=$2

#for i in {1..1144}; do
#    FILE="/hadoop/cms/store/group/snt/run2_data/Run2016H_DoubleEG_MINIAOD_03Feb2017_ver2-v1/merged/V08-00-18/merged_ntuple_${i}.root"
#for i in {1..1180}; do
#    FILE="/hadoop/cms/store/group/snt/run2_data/Run2016B_DoubleEG_MINIAOD_03Feb2017_ver2-v2/merged/V08-00-18/merged_ntuple_${i}.root"
# for i in {1..272}; do
#    FILE="/hadoop/cms/store/group/snt/run2_data/Run2016B_MuonEG_MINIAOD_03Feb2017_ver2-v2/merged/V08-00-18/merged_ntuple_${i}.root"
#for i in {1..972}; do
#    FILE="/hadoop/cms/store/group/snt/run2_data/Run2016B_SingleMuon_MINIAOD_03Feb2017_ver2-v2/merged/V08-00-18/merged_ntuple_${i}.root"


#UNIVERSE="grid"
UNIVERSE="vanilla"
EXE="wrapper_neb.sh"
INPUT="wrapper_neb.sh, job_input/input.tar.gz"
# can add other US sites here if desired
#SITE="T2_US_UCSD"
SITE="T2_US_Nebraska"
SUBMITLOGDIR="${PWD}/submit_logs"
JOBLOGDIR="${PWD}/job_logs"
PROXY=$(voms-proxy-info -path)
USERNAME=$(whoami)

LOGDIR="/data/tmp/$USER/condor_submit_logs/$COPYDIRBASE"
OUTDIR="/data/tmp/$USER/condor_job_logs/$COPYDIRBASE"
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

DIR=$PWD
rm ${DIR}/job_input/input.*
#printenv | grep -v PS1 | grep -v printenv | awk '{print "export "$1}' > ${DIR}/job_input/setupenv.sh
tar -hcf ${DIR}/job_input/input.tar job_input/*
gzip ${DIR}/job_input/input.tar
cd ${DIR}

COPYDIR=/hadoop/cms/store/user/${USERNAME}/mt2babies/${COPYDIRBASE}
#echo "[writeConfig] running on dataset ${DATADIR}"
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
#Grid_Resource=${Grid_Resource}
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
" > condor_${COPYDIRBASE##*/}.cmd

    #
    # now set the rest of the arguments 
    # for each job
    # 

#    for FILE in `ls ${DATADIR}/*.root`; do

# check if a list contains a given element
containsElement () {
  local e match="$1"
  shift
  for e; do [[ "$e" == "$match" ]] && return 1; done
  return 0
}

echo "[writeConfig] seeking list of files to run on..."

xrdfsout=`xrdfs xrootd-local.unl.edu ls ${DATADIR}`

# xrdfs ... ls returns the same filenames multiple times, see: https://github.com/xrootd/xrootd/issues/541
# once the second set of identical files begins, bug out
echo "[writeConfig] assembling list of unique file names..."

filearray=()
for FILE in ${xrdfsout}; do
    containsElement ${FILE} ${filearray[@]}
    if [ $? -eq 0 ]; then
	filearray+=(${FILE})
    else break
    fi
done

echo "[writeConfig] writing condor_${COPYDIRBASE##*/}.cmd" 

for FILE in ${filearray[@]}; do
    echo "Running on $FILE"
    echo "
executable=${EXE}
transfer_executable=True
arguments=`echo ${FILE##*/} | sed 's/\.root//g'` ${FILE} ${COPYDIR}
queue
" >> condor_${COPYDIRBASE##*/}.cmd
done

echo "[writeConfig] wrote condor_${COPYDIRBASE##*/}.cmd" 
