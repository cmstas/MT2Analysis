#!/bin/bash

function run () {
    echo root -b -q mergeHadoopFiles.C\(\"${HADOOPDIR}/${TAG}_$1/\",\"${OUTPUTDIR}/$1.root\"\)
    nohup nice -n 19 root -b -q mergeHadoopFiles.C\(\"${HADOOPDIR}/${TAG}_$1/\",\"${OUTPUTDIR}/$1.root\"\) >& ${LOGDIR}/log_merge_${TAG}_$1.txt &
}

TAG=V00-10-03_94x_2017_mc

HADOOPDIR=/hadoop/cms/store/user/${USER}/mt2babies/
OUTPUTDIR=/nfs-6/userdata/dpgilber/trial2018
LOGDIR=mergeLogs

mkdir -p $OUTPUTDIR
mkdir -p $LOGDIR
chmod -R a+wrx $OUTPUTDIR

declare -a samples=(dyjetsll_ht100to200 qcd_ht500to700 wjets_ht1200to2500 dyjetsll_ht1200to2500 qcd_ht700to1000 wjets_ht200to400 dyjetsll_ht200to400   singletop_s_amcatnlo  wjets_ht2500toInf dyjetsll_ht2500toInf  singletop_tW_antitop  wjets_ht400to600 dyjetsll_ht400to600   singletop_tW_top      wjets_ht600to800 dyjetsll_ht600to800   singletop_t_antitop   wjets_ht800to1200 dyjetsll_ht800to1200  singletop_t_top       ww gjets_dr0p4_ht100to200ttdl                  zinv_ht100to200 gjets_dr0p4_ht200to400ttg_lfromt            zinv_ht1200to2500 gjets_dr0p4_ht400to600ttg_lfromtbar         zinv_ht200to400 gjets_dr0p4_ht40to100 ttsl                  zinv_ht2500toInf gjets_dr0p4_ht600toInfttw_lnu               zinv_ht400to600 qcd_ht1000to1500      ttw_qq                zinv_ht600to800 qcd_ht1500to2000      ttz_llnunu            zinv_ht800to1200 qcd_ht2000toInf       ttz_qq               qcd_ht300to500        wjets_ht100to200)

for sample in ${samples[@]}; do
    run $sample
done