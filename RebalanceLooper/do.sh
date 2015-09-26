#!/bin/bash

make -j12

#INDIR=/nfs-6/userdata/mt2/V00-00-12_root6
INDIR=/nfs-6/userdata/mt2/RebalanceAndSmear_V00-00-12_jetpt10_genjets_noJEC_V2/
OUTDIR=output/test/

declare -a Samples=(qcd_pt300to470)

mkdir -p ${OUTDIR}

for SAMPLE in ${Samples[@]};
  do echo ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
  nohup ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& log_${SAMPLE}.txt &
done
