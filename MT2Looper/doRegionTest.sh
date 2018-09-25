#!/bin/bash

make -j 12 || return $?

OUTDIR=output/RegionTest5
LOGDIR=logs/
mkdir -p ${OUTDIR}
mkdir -p ${LOGDIR}

CONFIG=mc_94x_Fall17
INDIR=/nfs-6/userdata/mt2/V00-10-04_2017fullYear_31Mar2018_skim/
declare -a Samples=(ttsl ttdl singletop qcd_ht ww ttw ttz ttg dyjetsll_ht gjets_dr0p4_ht wjets_ht100to200 wjets_ht1200to2500 wjets_ht200to400 wjets_ht2500toInf wjets_ht400to600 wjets_ht600to800 wjets_ht800to1200 zinv_ht)

for SAMPLE in ${Samples[@]}; do
    echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
    eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
done

CONFIG=mc_80x_Moriond17
INDIR=/home/users/dpgilber/2017/SkimSMS
declare -a Samples=(T1bbbb_1400_1200 T1qqqq_1200_1000 T1tttt_1200_800 T2bb_1000_600 T2qq_1400_700 T2tt_1000_500 T1bbbb_1900_1400 T1qqqq_1800_1000 T1tttt_1600_1000 T2bb_1200_100 T2qq_1600_100 T2tt_1100_100 T1bbbb_2100_200 T1qqqq_1900_200 T1tttt_1900_200 T2bb_700_600 T2qq_900_800 T2tt_700_400)

for SAMPLE in ${Samples[@]}; do
    echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
    eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
done
