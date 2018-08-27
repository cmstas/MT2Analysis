#! /bin/bash


INPATH=/nfs-6/userdata/dpgilber/trial2018
OUTPATH=/nfs-6/userdata/dpgilber/trial2018_skim

mkdir -p $OUTPATH
mkdir -p skimLogs_$USER

declare -a Samples=(
dyjetsll_ht100to200
dyjetsll_ht200to400
dyjetsll_ht400to600
dyjetsll_ht600to800
dyjetsll_ht800to1200
dyjetsll_ht1200to2500
dyjetsll_ht2500toInf
qcd_ht
singletop
ttsl
ttw
ttz
wjets_ht100to200
wjets_ht200to400
wjets_ht400to600
wjets_ht600to800
wjets_ht800to1200
wjets_ht1200to2500
wjets_ht2500toInf
ww
zinv_ht100to200
zinv_ht200to400
zinv_ht400to600
zinv_ht600to800
zinv_ht800to1200
zinv_ht1200to2500
zinv_ht2500toInf
)

for SAMPLE in ${Samples[@]}
  do nohup root -b -q -l skim_base_mt2_Zinv_v6_JECs.C\(\"$INPATH\",\"$OUTPATH\",\"$SAMPLE\"\) >& skimLogs_$USER/log_${SAMPLE}.txt &
done

