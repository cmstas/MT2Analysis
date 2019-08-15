#!/bin/bash

make -j12

# OUTDIR=output/V00-09-06_noRS
# OUTDIR=output/V00-10-04_94x_2017_noRS_fullBinning/
# OUTDIR=output/V00-10-16_94x_2016_noRS
# OUTDIR=output/V00-10-16_94x_2017_noRS
OUTDIR=output/V00-10-16_94x_2018_noRS
# OUTDIR=output/test
LOGDIR=logs

CONFIG=data_2018_17Sep2018
INDIR=/nfs-7/userdata/mt2/V00-10-16_2018fullYear_skim_RS/
declare -a Samples=(data_Run2018A data_Run2018B data_Run2018C data_Run2018D)

# CONFIG=data_2017_31Mar2018
# INDIR=/nfs-7/userdata/mt2/V00-10-16_2017fullYear_skim_RS/
# declare -a Samples=(data_Run2017B data_Run2017C data_Run2017D data_Run2017E data_Run2017F)

# CONFIG=data_2016_94x
# INDIR=/nfs-7/userdata/mt2/V00-10-16_2016fullYear_skim_RS/
# declare -a Samples=(data_Run2016B data_Run2016C data_Run2016D data_Run2016E data_Run2016F data_Run2016G data_Run2016H)

# CONFIG=mc_102x_Autumn18
# INDIR=/nfs-7/userdata/mt2/V00-10-16_2018fullYear_skim_RS/
# # CONFIG=mc_94x_Fall17
# # INDIR=/nfs-7/userdata/mt2/V00-10-16_2017fullYear_skim_RS/
# # CONFIG=mc_94x_Summer16
# # INDIR=/nfs-7/userdata/mt2/V00-10-16_2016fullYear_skim_RS/extmerge
# declare -a Samples=(ttsl ttdl ttw ttz tth ttg singletop dyjetsll_ht wjets_ht100to200 wjets_ht200to400 wjets_ht400to600 wjets_ht600to800 wjets_ht800to1200 wjets_ht1200to2500 wjets_ht2500toInf zinv_ht100to200 zinv_ht200to400 zinv_ht400to600 zinv_ht600to800 zinv_ht800to1200 zinv_ht1200to2500 zinv_ht2500toInf)
# # declare -a Samples=(wjets_ht100to200 wjets_ht200to400 wjets_ht400to600 wjets_ht600to800 wjets_ht800to1200 wjets_ht1200to2500 wjets_ht2500toInf )
# # declare -a Samples=(qcd_ht300to500 qcd_ht500to700 qcd_ht700to1000 qcd_ht1000to1500 qcd_ht1500to2000 qcd_ht2000toInf )
# # declare -a Samples=(dyjetsll_ht)

# CONFIG=mc_94x_Fall17
# INDIR=/nfs-7/userdata/mt2/V00-10-09_2017fullYear_skim_qcd/
# # CONFIG=mc_94x_Summer16
# # INDIR=??
# declare -a Samples=(qcd_ht300to500 qcd_ht500to700 qcd_ht700to1000 qcd_ht1000to1500 qcd_ht1500to2000 qcd_ht2000toInf )

mkdir -p ${OUTDIR}
mkdir -p ${LOGDIR}

for SAMPLE in ${Samples[@]}; do 
    if [[ $SAMPLE == *data* ]]; then
        ARGS="-l 2 -n -1 -d -g $CONFIG"
    else
        ARGS="-l 2 -n -1 -w -g $CONFIG"
    fi
  echo ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} $ARGS 
  nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} ${ARGS} >& ${LOGDIR}/log_${SAMPLE}.txt&
done
