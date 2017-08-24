#!/bin/bash

make -j 8

OUTDIR=output/V00-09-01_v2_8p32fb

LOGDIR=logs

mkdir -p ${OUTDIR}
mkdir -p ${LOGDIR}


# #most recent MC
INDIR=/nfs-6/userdata/mt2/rphi_skim_V00-08-10_JECandRenorm

declare -a Samples=(qcd_ht ttsl ttdl singletop ttw_mg_lo ttz_mg_lo ttg dyjetsll_ht wjets_ht gjets_dr0p05_ht zinv_ht)

for SAMPLE in ${Samples[@]};
  do echo ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
  nohup ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &
done

#except most recent run2016H
INDIR=/nfs-6/userdata/mt2/V00-09-01_v2_rphi_skim

declare -a Samples=(data_Run2017B data_Run2017C)

# for SAMPLE in ${Samples[@]};
#   do echo ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
#   nohup ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &
# done
