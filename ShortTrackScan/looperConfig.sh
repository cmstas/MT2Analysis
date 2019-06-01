#!/bin/bash

while  ! voms-proxy-info -exist
do echo "No Proxy found issuing \"voms-proxy-init -voms cms\""
   voms-proxy-init -hours 168 -voms cms
done

RUNTAG=$1
OUTTAG=$2
YEAR=$3

CONFIG=""

if [[ $YEAR == "2016" ]]; then
    CONFIG=mc_94x_Summer16
elif [[ $YEAR == "2017" ]]; then
    CONFIG=mc_94x_fastsim_Fall17
elif [[ $YEAR == "2018" ]]; then
    CONFIG=mc_102x_fastsim_Autumn18
else
    echo "$YEAR not a good year"
    exit
fi

INDIR=/hadoop/cms/store/user/dpgilber/ShortTrackScan/$YEAR
declare -a Samples=(T1qqqq T2qq T2bt)
declare -a Extras=('' _ISR _GENMET)

# UNIVERSE="grid"
UNIVERSE="vanilla"
EXE="looper.sh"
INPUT="looper.sh, looper_input/looper.tar.gz"
# can add other US sites here if desired
SITE="T2_US_UCSD"
SUBMITLOGDIR="${PWD}/submit_logs"
JOBLOGDIR="${PWD}/job_logs"
PROXY=$(voms-proxy-info -path)
USERNAME=$(whoami)

LOGDIR="/data/tmp/$USER/condor_submit_logs/SigScanLooper/$OUTTAG"
OUTDIR="/data/tmp/$USER/condor_job_logs/SigScanLooper/$OUTTAG"
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

COPYDIR=/hadoop/cms/store/user/${USERNAME}/SigScanLooper/${OUTTAG}/${YEAR}
echo "[writeConfig] running on input ${RUNTAG}"
echo "[writeConfig] copying output to ${COPYDIR}"

if [ ! -d "${COPYDIR}" ]; then
    echo "[writeConfig] creating job output directory " ${COPYDIR}
    mkdir -p ${COPYDIR}
fi

#
# write configuration
#

condorfile=condor/looper_${OUTTAG}_${YEAR}.cmd
   
#Grid_Resource=gt2 osg-gw-6.t2.ucsd.edu:2119/jobmanager-condor
Grid_Resource="condor cmssubmit-r1.t2.ucsd.edu glidein-collector.t2.ucsd.edu"
echo "
universe=${UNIVERSE}
when_to_transfer_output = ON_EXIT
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
" > $condorfile

    #
    # now set the rest of the arguments 
    # for each job
    # 

for sample in ${Samples[@]}; do
    for EXTRA in "${Extras[@]}"; do
	echo "
transfer_input_files=${INPUT}, ${FILE}
executable=${EXE}
transfer_executable=True
arguments=${COPYDIR} ${INDIR}  ${sample} 10 1 ${RUNTAG} ${CONFIG} ${EXTRA}
queue
" >> $condorfile
	echo "
transfer_input_files=${INPUT}, ${FILE}
executable=${EXE}
transfer_executable=True
arguments=${COPYDIR} ${INDIR}  ${sample} 10 5 ${RUNTAG} ${CONFIG} ${EXTRA}
queue
" >> $condorfile
	echo "
transfer_input_files=${INPUT}, ${FILE}
executable=${EXE}
transfer_executable=True
arguments=${COPYDIR} ${INDIR}  ${sample} 10 10 ${RUNTAG} ${CONFIG} ${EXTRA}
queue
" >> $condorfile
	echo "
transfer_input_files=${INPUT}, ${FILE}
executable=${EXE}
transfer_executable=True
arguments=${COPYDIR} ${INDIR}  ${sample} 50 30 ${RUNTAG} ${CONFIG} ${EXTRA}
queue
" >> $condorfile
	echo "
transfer_input_files=${INPUT}, ${FILE}
executable=${EXE}
transfer_executable=True
arguments=${COPYDIR} ${INDIR}  ${sample} 50 50 ${RUNTAG} ${CONFIG} ${EXTRA}
queue
" >> $condorfile
	echo "
transfer_input_files=${INPUT}, ${FILE}
executable=${EXE}
transfer_executable=True
arguments=${COPYDIR} ${INDIR}  ${sample} 200 50val ${RUNTAG} ${CONFIG} ${EXTRA}
queue
" >> $condorfile
	echo "
transfer_input_files=${INPUT}, ${FILE}
executable=${EXE}
transfer_executable=True
arguments=${COPYDIR} ${INDIR}  ${sample} 50 10val ${RUNTAG} ${CONFIG} ${EXTRA}
queue
" >> $condorfile
	echo "
transfer_input_files=${INPUT}, ${FILE}
executable=${EXE}
transfer_executable=True
arguments=${COPYDIR} ${INDIR}  ${sample} 10 200val ${RUNTAG} ${CONFIG} ${EXTRA}
queue
" >> $condorfile
#	echo "
#transfer_input_files=${INPUT}, ${FILE}
#executable=${EXE}
#transfer_executable=True
#arguments=${COPYDIR} ${INDIR}  ${sample} 200 70 ${RUNTAG} ${CONFIG} ${EXTRA}
#queue
#" >> $condorfile
	echo "
transfer_input_files=${INPUT}, ${FILE}
executable=${EXE}
transfer_executable=True
arguments=${COPYDIR} ${INDIR}  ${sample} 200 100 ${RUNTAG} ${CONFIG} ${EXTRA}
queue
" >> $condorfile
	echo "
transfer_input_files=${INPUT}, ${FILE}
executable=${EXE}
transfer_executable=True
arguments=${COPYDIR} ${INDIR}  ${sample} 200 150 ${RUNTAG} ${CONFIG} ${EXTRA}
queue
" >> $condorfile
	echo "
transfer_input_files=${INPUT}, ${FILE}
executable=${EXE}
transfer_executable=True
arguments=${COPYDIR} ${INDIR}  ${sample} 200 200 ${RUNTAG} ${CONFIG} ${EXTRA}
queue
" >> $condorfile
	echo "
transfer_input_files=${INPUT}, ${FILE}
executable=${EXE}
transfer_executable=True
arguments=${COPYDIR} ${INDIR}  ${sample} 200 300 ${RUNTAG} ${CONFIG} ${EXTRA}
queue
" >> $condorfile
	echo "
transfer_input_files=${INPUT}, ${FILE}
executable=${EXE}
transfer_executable=True
arguments=${COPYDIR} ${INDIR}  ${sample} 200 500 ${RUNTAG} ${CONFIG} ${EXTRA}
queue
" >> $condorfile
	echo "
transfer_input_files=${INPUT}, ${FILE}
executable=${EXE}
transfer_executable=True
arguments=${COPYDIR} ${INDIR}  ${sample} 200 1000 ${RUNTAG} ${CONFIG} ${EXTRA}
queue
" >> $condorfile
    done
done

echo "[writeConfig] wrote $condorfile" 
