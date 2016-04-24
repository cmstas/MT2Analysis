#!/bin/bash

make -j 8

#OUTDIR=output/test/

INDIR=/nfs-6/userdata/mt2/softLep-v3_V00-01-10_25ns_miniaodv2_skim/
OUTDIR=output/skim_apr22/

# INDIR=/nfs-6/userdata/mt2/softLep-v3_V00-01-10_25ns_miniaodv2/
# OUTDIR=output/noskim_reweight_apr19/

 # INDIR=/nfs-6/userdata/mt2/softLepJECdown-v3_V00-01-10_25ns_miniaodv2/
 # OUTDIR=output/JECdown_apr19/

#declare -a Samples=(wjets_ht zinv_ht singletop ttdl ttsl ttg tth ttw ttz dyjetsll qcd_ht ww zz wz data_Run2015D data_Run2015C)
declare -a Samples=(wjets_ht dyjetsll ttsl)

mkdir -p ${OUTDIR}
mkdir -p logs

for SAMPLE in ${Samples[@]};
  do echo ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
  nohup ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& logs/${SAMPLE}.txt &
done
