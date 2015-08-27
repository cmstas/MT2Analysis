#!/bin/bash

make -j12

INDIR=/nfs-6/userdata/mt2/V00-01-02_25ns_softmus/
OUTDIR=output/test_root6/

declare -a Samples=(wjets_ht100to200)
#declare -a Samples=(wjets_ht100to200 wjets_ht200to400 wjets_ht400to600 wjets_ht600toInf wjets_ht600to800 wjets_ht800to1200 wjets_ht1200to2500 wjets_ht2500toInf)

mkdir -p ${OUTDIR}

for SAMPLE in ${Samples[@]};
  do echo ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
  nohup ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& log_${SAMPLE}.txt &
done
