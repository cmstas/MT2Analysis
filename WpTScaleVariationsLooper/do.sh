#!/bin/bash

make -j 8

INDIR=/nfs-6/userdata/mt2/softLep-v3_V00-01-10_25ns_miniaodv2/
OUTDIR=output/test/

#declare -a Samples=(data_Run2015C_SinglePhoton data_Run2015D_SinglePhoton gjet_ht)
declare -a Samples=(wjets_ht100 wjets_ht200 wjets_ht400 wjets_ht600 ttsl )
#declare -a Samples=(data_Run2015C data_Run2015D ttsl_mg_lo ttdl_mg_lo wjets_ht zinv_ht singletop qcd_ht ttw ttz ttg tth dyjetsll_ht gjet_ht)
#declare -a Samples=(data_Run2015D_MuonEG data_Run2015C_MuonEG data_Run2015D_DoubleMuon data_Run2015D_DoubleEG data_Run2015C_DoubleMuon data_Run2015C_DoubleEG ttdl_mg_lo  ttw ttz ttg tth dyjetsll_ht singletop ww wz zz wjets_ht)

mkdir -p ${OUTDIR}
mkdir -p logs

for SAMPLE in ${Samples[@]};
  do echo ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
  nohup ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& logs/${SAMPLE}.txt &
done
