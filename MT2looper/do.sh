#!/bin/bash

make -j12

INDIR=/nfs-6/userdata/mt2/V00-01-04_25ns_softleps_skim_trig_njets30_ht_met_mt2_Zinv/
OUTDIR=output/test_softleps/

declare -a Samples=(wjets_amcatnlo)
#declare -a Samples=(wjets_ht100to200 wjets_ht200to400 wjets_ht400to600 wjets_ht600toInf wjets_ht600to800 wjets_ht800to1200 wjets_ht1200to2500 wjets_ht2500toInf)

mkdir -p ${OUTDIR}

for SAMPLE in ${Samples[@]};
  do echo ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
  nohup ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& log_${SAMPLE}.txt &
done
