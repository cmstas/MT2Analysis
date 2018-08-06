#!/bin/bash

make -j12

# OUTDIR=output/V00-09-06_noRS
OUTDIR=output/V00-10-01_noRS_mt2100
LOGDIR=logs

# INDIR=/nfs-6/userdata/mt2/V00-10-01_json_294927-306462_31Mar2018_correctJECs_RS_skim/
# declare -a Samples=(data_Run2017B data_Run2017C data_Run2017D data_Run2017E data_Run2017F)

# INDIR=/nfs-6/userdata/mt2/RebalanceAndSmear_V00-09-06_qcd_skim
INDIR=/nfs-6/userdata/mt2/V00-10-01_94X_mc_qcd_skim/
declare -a Samples=(qcd_ht300to500 qcd_ht500to700 qcd_ht700to1000 qcd_ht1000to1500 qcd_ht1500to2000 qcd_ht2000toInf )

mkdir -p ${OUTDIR}
mkdir -p ${LOGDIR}

for SAMPLE in ${Samples[@]}; do 
    if [[ $SAMPLE == *data* ]]; then
        ARGS="-l 2 -n -1 -d"
    else
        ARGS="-l 2 -n -1"
    fi
  echo ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} $ARGS 
  nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} ${ARGS} >& ${LOGDIR}/log_${SAMPLE}.txt&
done
