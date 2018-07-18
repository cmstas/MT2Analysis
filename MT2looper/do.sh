#!/bin/bash

make -j 12 || return $?

OUTDIR=output/test
# OUTDIR=output/V00-10-01_2017_31Mar2018_correctJECs
# OUTDIR=output/V00-10-01_2018_16p59fb_2017C_JECs
# OUTDIR=output/V00-09-04_41p96fb
# OUTDIR=output/full2016_data
LOGDIR=logs/
mkdir -p ${OUTDIR}
mkdir -p ${LOGDIR}

#most recent MC
CONFIG=mc_80x_Moriond17
INDIR=/nfs-6/userdata/mt2/2016_moriond_results
declare -a Samples=(ttsl ttdl singletop qcd_ht ttw_mg_lo ttz_mg_lo ttg tth dyjetsll_ht gjets_dr0p05_ht wjets_ht100to200 wjets_ht1200to2500 wjets_ht200to400 wjets_ht2500toInf wjets_ht400to600 wjets_ht600to800 wjets_ht800to1200 zinv_ht)

# for SAMPLE in ${Samples[@]}; do
#     echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
#     eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
# done

# #2018
CONFIG=data_2018_Prompt
INDIR=/nfs-6/userdata/mt2/V00-10-01_json_314472-317696_2017C_JECs_skim
declare -a Samples=(data_Run2018A data_Run2018B)

for SAMPLE in ${Samples[@]}; do
    echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
    eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
done

# #2017 re-reco
CONFIG=data_2017_31Mar2018
INDIR=/nfs-6/userdata/mt2/V00-10-01_json_294927-306462_31Mar2018_correctJECs_skim/
declare -a Samples=(data_Run2017B data_Run2017C data_Run2017D data_Run2017E data_Run2017F)

# for SAMPLE in ${Samples[@]}; do
#     echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
#     eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
# done

# #most recent run2016H and re-reco
CONFIG=data_2016_Moriond17
INDIR=/nfs-6/userdata/mt2/2016_moriond_results
declare -a Samples=(data_Run2016B data_Run2016C data_Run2016D data_Run2016E data_Run2016F data_Run2016G data_Run2016H)

# for SAMPLE in ${Samples[@]}; do
#     echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
#     eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
# done
