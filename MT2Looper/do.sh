#!/bin/bash

make -j 12 || return $?

# OUTDIR=output/test/
# OUTDIR=output/V00-10-06_2017fullYear_newBinning
# OUTDIR=output/V00-10-04_80x_MC_v2
# OUTDIR=output/V00-10-10_unblinded1718
# OUTDIR=output/V00-10-15_2016fullYear
OUTDIR=output/V00-10-15_2017fullYear
# OUTDIR=output/V00-10-15_2018fullYear
# OUTDIR=output/V00-10-11_2018fullYear_with2018MC
# OUTDIR=output/V00-10-10_withExtraB_2018_with2017MC
# OUTDIR=output/zlllowmet/2017_withZNJetWeights
# OUTDIR=output/V00-10-10_80x_2016_fastsim
LOGDIR=logs/
mkdir -p ${OUTDIR}
mkdir -p ${LOGDIR}

#most recent MC
# CONFIG=mc_80x_Summer16
# INDIR=/nfs-6/userdata/mt2/2016_moriond_results/
CONFIG=mc_94x_Summer16
INDIR=/nfs-7/userdata/mt2/V00-10-13_2016fullYear_skim_base/extmerge
# CONFIG=mc_94x_Fall17
# INDIR=/nfs-7/userdata/mt2/V00-10-12_2017fullYear_skim_base/
# CONFIG=mc_102x_Autumn18
# INDIR=/nfs-7/userdata/mt2/V00-10-14_2018fullYear_skim_base/
declare -a Samples=(ttsl ttdl tttt singletop qcd_ht ttw ttz ttg tth dyjetsll_ht gjets_dr0p05_ht wjets_ht100to200 wjets_ht1200to2500 wjets_ht200to400 wjets_ht2500toInf wjets_ht400to600 wjets_ht600to800 wjets_ht800to1200 zinv_ht ww wz)
# declare -a Samples=(dyjetsll_ht ttsl ttdl ttz ttw ttg singletop)
# declare -a Samples=(singletop wjets_ht1200to2500 wjets_ht200to400 wjets_ht2500toInf wjets_ht400to600 wjets_ht600to800 wjets_ht800to1200 ww wz)
# declare -a Samples=(dyjetsll_ht)
# declare -a Samples=(wjets_ht100to200 wjets_ht1200to2500 wjets_ht200to400 wjets_ht2500toInf wjets_ht400to600 wjets_ht600to800 wjets_ht800to1200)
# declare -a Samples=(dyjetsll_ht100to200 dyjetsll_ht1200to2500 dyjetsll_ht200to400 dyjetsll_ht2500toInf dyjetsll_ht400to600 dyjetsll_ht600to800 dyjetsll_ht800to1200)
# declare -a Samples=(zinv_ht100to200 zinv_ht1200to2500 zinv_ht200to400 zinv_ht2500toInf zinv_ht400to600 zinv_ht600to800 zinv_ht800to1200)

# for SAMPLE in ${Samples[@]}; do
#     echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
#     eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
# done

# #2018
CONFIG=data_2018_17Sep2018
INDIR=/nfs-7/userdata/mt2/V00-10-14_2018fullYear_skim_base/
# INDIR=/nfs-6/userdata/mt2/V00-10-07_2018B_specialHEM_26Sep2018_skim_base/
# INDIR=/nfs-6/userdata/mt2/V00-10-02_testHcalOfflineFix_skim/
declare -a Samples=(data_Run2018A data_Run2018B data_Run2018C data_Run2018D)
# declare -a Samples=(data_Run2018B_JetHT_26Sep2018_HEmiss data_Run2018B_JetHT_26Sep2018_noHEmiss)

# for SAMPLE in ${Samples[@]}; do
#     echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
#     eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
# done

# #2017 re-reco
CONFIG=data_2017_31Mar2018
INDIR=/nfs-7/userdata/mt2/V00-10-12_2017fullYear_skim_base/
# INDIR=/nfs-6/userdata/mt2/V00-10-06_2017fullYear_31Mar2018_skim/
declare -a Samples=(data_Run2017B data_Run2017C data_Run2017D data_Run2017E data_Run2017F)
# declare -a Samples=(data_Run2017D)

# for SAMPLE in ${Samples[@]}; do
#     echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
#     eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
# done

# #most recent run2016H and re-reco
CONFIG=data_2016_94x
INDIR=/nfs-7/userdata/mt2/V00-10-12_2016fullYear_skim_base/extmerge
declare -a Samples=(data_Run2016B data_Run2016C data_Run2016D data_Run2016E data_Run2016F data_Run2016G data_Run2016H)
# declare -a Samples=(data_Run2016G)

# for SAMPLE in ${Samples[@]}; do
#     echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
#     eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
# done



# FastSim 80x
CONFIG=mc_80x_fastsim_Moriond17
INDIR=/nfs-7/userdata/mt2/V00-10-10_2016fullYear_skim_base/80x_signal/
declare -a Samples=(T1tttt T1bbbb T1qqqq T2tt T2bb T2qq)

# for SAMPLE in ${Samples[@]}; do
#     echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
#     eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
# done

# FastSim 102x Autumn18
CONFIG=mc_102x_fastsim_Autumn18
# INDIR=/nfs-7/userdata/mt2/V00-10-15_2017fullYear_skim_base/
INDIR=/nfs-6/userdata/dpgilber/ShortTrackScanBaseSkims
# declare -a Samples=(T1tttt T1bbbb T1qqqq T2tt T2bb T2qq)
# declare -a Samples=(T1tttt T1bbbb T1qqqq T2bb T2qq T2tt_mStop150to250 T2tt_mStop250to350 T2tt_mStop350to400 T2tt_mStop400to1200 T2tt_mStop1200to2000) 
# declare -a Samples=(T1tttt T1bbbb T1qqqq T2bb T2qq) 
# declare -a Samples=(T2tt_mStop150to250 T2tt_mStop250to350 T2tt_mStop350to400 T2tt_mStop400to1200 T2tt_mStop1200to2000)
declare -a Samples=(T1qqqq_10 T1qqqq_50 T1qqqq_200 T2qq_10 T2qq_50 T2qq_200 T2bt_10 T2bt_50 T2bt_200) 

# for SAMPLE in ${Samples[@]}; do
#     echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
#     eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
# done

# fastsim 94x Fall17
CONFIG=mc_94x_fastsim_Fall17
# INDIR=/nfs-7/userdata/mt2/V00-10-15_2017fullYear_skim_base/
INDIR=/nfs-6/userdata/dpgilber/ShortTrackScanBaseSkims
# declare -a Samples=(T1tttt T1bbbb T1qqqq T2bb T2qq T2tt_mStop150to250 T2tt_mStop250to350 T2tt_mStop350to400 T2tt_mStop400to1200 T2tt_mStop1200to2000) 
# declare -a Samples=(T1tttt T1bbbb T1qqqq T2bb T2qq) 
# declare -a Samples=(T2tt_mStop150to250 T2tt_mStop250to350 T2tt_mStop350to400 T2tt_mStop400to1200 T2tt_mStop1200to2000)
declare -a Samples=(T1qqqq_10 T1qqqq_50 T1qqqq_200 T2qq_10 T2qq_50 T2qq_200 T2bt_10 T2bt_50 T2bt_200) 

for SAMPLE in ${Samples[@]}; do
    echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
    eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
done


# fastsim 94x Summer16
CONFIG=mc_94x_fastsim_Summer16
INDIR=/nfs-7/userdata/mt2/V00-10-15_2016fullYear_skim_base/
# declare -a Samples=(T1tttt T1bbbb T1qqqq T2tt T2bb T2qq)
# declare -a Samples=(T1tttt T1bbbb T1qqqq T2bb T2qq)
declare -a Samples=(T2tt_mStop150to250 T2tt_mStop250to350 T2tt_mStop350to400 T2tt_mStop400to1200 T2tt_mStop1200to2000)

# for SAMPLE in ${Samples[@]}; do
#     echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR}
#     eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${CONFIG} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
# done

