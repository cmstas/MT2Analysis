#!/bin/bash

make -j 12 || return $?

OUTDIR=output/RegionTestPJ
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
declare -a Samples=(T1bbbb T1qqqq T1tttt T2qq T2bb T2tt)
#declare -a Samples=(T1qqqq)

for SAMPLE in ${Samples[@]}; do
    echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
    eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
done
