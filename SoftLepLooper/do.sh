#!/bin/bash

make -j12

OUTDIR=output/oldSRs_test_Mar29/



##old MC
#INDIR=/nfs-6/userdata/mt2/softLep-v3_V00-01-10_25ns_miniaodv2_skim/
#declare -a Samples=(wjets_ht zinv_ht singletop ttdl ttsl ttg tth ttw ttz dyjetsll qcd_ht ww zz wz)
#
#for SAMPLE in ${Samples[@]};
#  do echo nice -n 19 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
#  nohup nice -n 19 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& logs/${SAMPLE}.log &
#done


#new signals
INDIR=/nfs-6/userdata/mt2/softLep-new2017/
OUTDIR=output/newSRs_test_Mar29/
#declare -a Samples=(T6qqWW_dM05 T6qqWW_dM10 T6qqWW_dM15 T6qqWW_dM20)
declare -a Samples=(T6qqWW_dM20 )

#individual points
#INDIR=/nfs-6/userdata/mt2/softGio/
#OUTDIR=output/oldSRs_test_Mar29/
#declare -a Samples=(T6qqWW_dM20_1300_300)
# T6qqWW_dM20)

mkdir -p ${OUTDIR}
mkdir -p logs

for SAMPLE in ${Samples[@]};
  do echo nice -n 19 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
  nohup nice -n 19 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& logs/${SAMPLE}.log &
done
