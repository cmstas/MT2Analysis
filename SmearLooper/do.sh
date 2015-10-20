#!/bin/bash

make -j12

#INDIR=/nfs-6/userdata/mt2/Spring15_QCD_MG_RebalanceAndSmear_25nsV2_MC_JEC
#OUTDIR=output/MC_controlRegions_579pb
#declare -a Samples=(qcd_ht2000toInf qcd_ht1500to2000 qcd_ht1000to1500 qcd_ht300to500 qcd_ht700to1000 qcd_ht500to700)

#INDIR=/nfs-6/userdata/mt2/25ns_golden_246908-258159_25nsV5JEC_RebalanceAndSmear
#OUTDIR=output/25ns_golden_246908-258159_25nsV5JEC_RebalanceAndSmear
#declare -a Samples=(data)

INDIR=/nfs-6/userdata/mt2/V00-01-05_25ns_json_246908-256869_Summer15_25nsV2_skim_trig_nj2_ht450_met30_mt2gt50_Zinv
declare -a Samples=(ttall_mg_lo wjets_ht zinv_ht)
OUTDIR=output/MC_controlRegions_579pb

mkdir -p ${OUTDIR}

for SAMPLE in ${Samples[@]};
  do echo ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
  nohup ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& log_${SAMPLE}.txt &
done
