#!/bin/bash

make -j12

#INDIR=/nfs-6/userdata/mt2/Spring15_QCD_MG_RebalanceAndSmear_25nsV2_MC_JEC
#OUTDIR=output/MC_controlRegions_579pb
#declare -a Samples=(qcd_ht2000toInf qcd_ht1500to2000 qcd_ht1000to1500 qcd_ht300to500 qcd_ht700to1000 qcd_ht500to700)

# INDIR=/nfs-6/userdata/mt2/RebalanceAndSmear_V00-08-14_data_rereco_skim
# INDIR=/hadoop/cms/store/user/bemarsh/mt2babies/merged/RebalanceAndSmear_V00-08-12
INDIR=/hadoop/cms/store/user/bemarsh/mt2babies//RebalanceAndSmear_V00-08-12_data_rereco_data_Run2016B_JetHT_23Sep2016 
# REBALDIR=../RebalanceLooper/output/mt2babies_withPUID_withBTAG_forEWK_allEvents
# OUTDIR=output/RebalanceAndSmear_V00-08-12_noRS

# INDIR=..
# REBALDIR=../RebalanceLooper/output/test
OUTDIR=output/test

LOGDIR=logs

# declare -a Samples=(data_Run2016B data_Run2016C data_Run2016D data_Run2016E data_Run2016F data_Run2016G data_Run2016H)
declare -a Samples=(data_Run2016B_23Sep2016_merged_ntuple_786)
# declare -a Samples=(data_Run2016B_23Sep2016_merged_ntuple_397 data_Run2016B_23Sep2016_merged_ntuple_398 data_Run2016B_23Sep2016_merged_ntuple_399 data_Run2016B_23Sep2016_merged_ntuple_4 data_Run2016B_23Sep2016_merged_ntuple_40 data_Run2016B_23Sep2016_merged_ntuple_400 data_Run2016B_23Sep2016_merged_ntuple_401 data_Run2016B_23Sep2016_merged_ntuple_402 data_Run2016B_23Sep2016_merged_ntuple_403 data_Run2016B_23Sep2016_merged_ntuple_404)

# declare -a Samples=(ttsl ttdl wjets_ht100to200 wjets_ht200to400 wjets_ht400to600 wjets_ht600to800 wjets_ht800to1200 wjets_ht1200to2500 wjets_ht2500toInf )
# declare -a Samples=(zinv_ht100to200 zinv_ht200to400 zinv_ht400to600 zinv_ht600to800 zinv_ht800to1200 zinv_ht1200to2500 zinv_ht2500toInf )
# declare -a Samples=(qcd_pt15to30 qcd_pt30to50 qcd_pt50to80 qcd_pt80to120 qcd_pt120to170 qcd_pt170to300)
# declare -a Samples=(qcd_pt300to470 qcd_pt470to600 qcd_pt600to800 qcd_pt800to1000 qcd_pt1000to1400 qcd_pt1400to1800 qcd_pt1800to2400 qcd_pt2400to3200 qcd_pt3200toInf)

mkdir -p ${OUTDIR}
mkdir -p ${LOGDIR}

for SAMPLE in ${Samples[@]};
  do echo ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
  nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}  -l 1 -r -n 100 -d >& ${LOGDIR}/log_${SAMPLE}.txt&
done
