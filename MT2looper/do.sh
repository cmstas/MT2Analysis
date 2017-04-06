#!/bin/bash

make -j 32 || return $?

# INDIR=/nfs-6/userdata/mt2/V00-08-08_nojson_skim_base_mt2gt200_ZinvV6
# INDIR2=/nfs-6/userdata/mt2/V00-08-07_nojson_skim_base_mt2gt200_ZinvV6
# INDIR3=/nfs-6/userdata/mt2/2015results_eta2p4_v2_skim_base_mt2gt200_ZinvV4
# INDIR4=/nfs-6/userdata/mt2/V00-08-08_mt2Higgs

OUTDIR=output/temp
LOGDIR=logs
# LOGDIR=$OUTDIR/logs/
mkdir -p ${OUTDIR}
mkdir -p ${LOGDIR}

# declare -a Samples=(ttsl ttdl wjets_ht zinv_ht gjets_dr0p05_ht singletop ttw ttz ttg qcd_ht dyjetsll_ht)
# declare -a Samples4=(T5qqqqWH_1400_700 T5qqqqWH_1100_950 T5qqqqWH_1400_200 T2ttZH_800_400 T2ttZH_800_200)

#most recent MC
INDIR=/nfs-6/userdata/mt2/V00-08-16_skim_base_mt2gt200_ZinvV6_JECs/
declare -a Samples=(ttsl ttdl singletop qcd_ht ttw_mg_lo ttz_mg_lo ttg tth dyjetsll_ht gjets_dr0p05_ht wjets_ht100to200 wjets_ht1200to2500 wjets_ht200to400 wjets_ht2500toInf wjets_ht400to600 wjets_ht600to800 wjets_ht800to1200 zinv_ht)
# #declare -a Samples=(wz_1l3n wz_3lnu wz_lnqq ww_2l2nu ww_lnuqq www_incl_amcnlo wwz_incl_amcnlo wzz_incl_amcnlo zzz_incl_amcnlo zz_2l2n zz_2l2q zz_2q2n zz_4l) #diboson and triboson samples
# declare -a Samples=(ttsl ttdl zinv_ht) # Testing

for SAMPLE in ${Samples[@]}; do
    echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
    eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
done

#most recent run2016H and re-reco
INDIR=/nfs-6/userdata/mt2/V00-08-16_skim_base_mt2gt200_ZinvV6_JECs/
declare -a Samples=(data_Run2016B data_Run2016C data_Run2016D data_Run2016E data_Run2016F data_Run2016G data_Run2016H)
# declare -a Samples=(data_Run2016B) # Testing

for SAMPLE in ${Samples[@]}; do
    echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
    eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
done

# #signal
# INDIR=/nfs-6/userdata/mt2/V00-08-16_skim_base_mt2gt200_ZinvV6_JECs/
# declare -a Samples=(T1bbbb T1qqqq T1tttt T2bb T2qq T2tt)

# for SAMPLE in ${Samples[@]}; do
#     echo nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
#     eval "nohup nice -n 10 ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
# done

# for SAMPLE in ${Samples2[@]}; do
#     echo ./runLooper ${INDIR2} ${SAMPLE} ${OUTDIR}
#     eval "nohup ./runLooper ${INDIR2} ${SAMPLE} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
# done

# for SAMPLE in ${Samples3[@]}; do
#     echo ./runLooper ${INDIR3} ${SAMPLE} ${OUTDIR}
#     eval "nohup ./runLooper ${INDIR3} ${SAMPLE} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
# done

# for SAMPLE in ${Samples4[@]}; do
#     echo ./runLooper ${INDIR4} ${SAMPLE} ${OUTDIR}
#     eval "nohup ./runLooper ${INDIR4} ${SAMPLE} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &"
# done
