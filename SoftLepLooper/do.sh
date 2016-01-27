#!/bin/bash

make -j12

INDIR=/nfs-6/userdata/mt2/softLep_V00-01-10_25ns_miniaodv2_skim/
OUTDIR=output/softLep/

#INDIR=/nfs-6/userdata/mt2/V00-01-09_25ns_softLepSR/
#OUTDIR=output/softLepSR/

# INDIR=/nfs-6/userdata/mt2/V00-01-09_25ns_softLepCR/
# OUTDIR=output/softLepCR/

#declare -a Samples=(data_Run2015C data_Run2015D ttsl_mg_lo ttdl_mg_lo wjets_ht zinv_ht singletop qcd_ht ttw ttz ttg tth dyjetsll_ht gjet_ht)
#declare -a Samples=(wjets_ht zinv_ht singletop ttdl ttsl ttg tth ttw ttz dyjetsll qcd_ht ww zz wz data_Run2015D data_Run2015C)
declare -a Samples=(wjets_ht zinv_ht singletop ttdl ttsl ttg tth ttw ttz dyjetsll qcd_ht ww T2-4bd_375 T2-4bd_275)
#declare -a Samples=(T2-4bd_375)

mkdir -p ${OUTDIR}
mkdir -p logs

for SAMPLE in ${Samples[@]};
  do echo nice -n 19 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
  nohup nice -n 19 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& logs/${SAMPLE}.log &
done

