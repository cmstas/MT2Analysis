#!/bin/bash

make -j 8

INDIR=/nfs-6/userdata/mt2/softLep-v3_V00-01-10_25ns_miniaodv2_skim/
OUTDIR=output/test/

declare -a Samples=(data_Run2015C_SinglePhoton data_Run2015D_SinglePhoton gjet_ht)
#declare -a Samples=(gjet_ht)

mkdir -p ${OUTDIR}
mkdir -p logs

for SAMPLE in ${Samples[@]};
  do echo ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
  nohup ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& logs/${SAMPLE}.txt &
done
