#!/bin/bash

make -j 24 || return $?

# INDIR=/nfs-6/userdata/mt2/V00-08-02_nojson_skim_base_mt2gt200_ZinvV4/
# INDIR=/nfs-6/userdata/mt2/V00-08-03_MiniAODv2_nojson_skim_base_mt2gt200_ZinvV4
# INDIR=/nfs-6/userdata/mt2/V00-08-06_nojson_skim_base_mt2gt200_ZinvV4
# INDIR3=/nfs-6/userdata/mt2/V00-08-04_MiniAODv2_nojson_skim_base_mt2gt200_ZinvV4
INDIR3=/nfs-6/userdata/mt2/2015results_eta2p4_v2_skim_base_mt2gt200_ZinvV4
INDIR2=/nfs-6/userdata/mt2/V00-08-07_nojson_skim_base_mt2gt200_ZinvV6
INDIR=/nfs-6/userdata/mt2/V00-08-08_nojson_skim_base_mt2gt200_ZinvV6
INDIR4=/nfs-6/userdata/mt2/V00-08-08_mt2Higgs
# OUTDIR=output/V00-08-01_json_Cert_271036-274421_skim_base_mt2gt200_ZinvV4
# OUTDIR=output/original
# OUTDIR=output/isHcand
# OUTDIR=output/minMTbmet
# OUTDIR=output/mMTnHcand
# OUTDIR=output/MbbMax200
# OUTDIR=output/mMTnMbb200
# OUTDIR=output/MbbMax300
# OUTDIR=output/mMTnMbb300
OUTDIR=output/temp
# OUTDIR=output/test
LOGDIR=logs/
# LOGDIR=$OUTDIR/logs/

#declare -a Samples=(data_Run2015C data_Run2015D)
#declare -a Samples=(ttsl_mg_lo ttdl_mg_lo wjets_ht zinv_ht singletop qcd_ht ttw ttz ttg tth dyjetsll_ht gjet_ht)
#declare -a Samples=(data_Run2015C data_Run2015D ttsl_mg_lo ttdl_mg_lo wjets_ht zinv_ht singletop qcd_ht ttw ttz ttg tth dyjetsll_ht gjet_ht)
#declare -a Samples=(gjet_ht ttsl ttdl ttz ttw wjets_ht zinv_ht singletop qcd_ht ttg data_Run2016B dyjetsll_ht)
# declare -a Samples=(ttsl ttdl ttz ttw wjets_ht 2015zinv_ht gjets_dr0p05_ht singletop ttg data_Run2016B)
# declare -a Samples3=(2015zinv_ht 2015gjets_ht 2015qcd_ht 2015dyjetsll_ht)
declare -a Samples3=(zinv_ht gjet_ht qcd_ht dyjetsll_ht)
declare -a Samples2=(data_Run2016B data_Run2016C data_Run2016D)
# declare -a Samples=(ttsl ttdl wjets_ht zinv_ht gjets_dr0p05_ht singletop ttw ttz ttg qcd_ht dyjetsll_ht)
declare -a Samples=(ttsl ttdl wjets_ht singletop ttw ttz ttg)
declare -a Samples4=(T5qqqqWH_1400_700 T5qqqqWH_1100_950 T5qqqqWH_1400_200 T2ttZH_800_400 T2ttZH_800_200)
# declare -a Samples=(T1tttt_1500_100)
#declare -a Samples=(T1tttt_1500_100 T1tttt_1200_800 T1bbbb_1500_100 T1bbbb_1000_900 T1qqqq_1400_100 T1qqqq_1000_800 T2tt_850_100 T2tt_650_325 T2tt_500_325 T2tt_425_325 T2bb_900_100 T2bb_600_580 T2qq_1200_100 T2qq_600_550)
#declare -a Samples=(T1tttt_1500_100 T1tttt_1200_800 T1bbbb_1500_100 T1bbbb_1000_900 T1qqqq_1400_100 T1qqqq_1000_800)
# declare -a Samples2=(data_Run2016C)

# 2015 samples for Zinvisible estimate
#INDIR=/nfs-6/userdata/mt2/V00-08-02_nojson_skim_base_mt2gt200_ZinvV4/
#declare -a Samples=(2015qcd_ht 2015gjets_ht 2015dyjetsll_ht 2015zinv_ht)

mkdir -p ${OUTDIR}
mkdir -p ${LOGDIR}

for SAMPLE in ${Samples[@]};
  do echo ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
     nohup ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &
done

for SAMPLE in ${Samples2[@]};
  do echo ./runLooper ${INDIR2} ${SAMPLE} ${OUTDIR}
     nohup ./runLooper ${INDIR2} ${SAMPLE} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &
done

for SAMPLE in ${Samples3[@]};
  do echo ./runLooper ${INDIR3} ${SAMPLE} ${OUTDIR}
     nohup ./runLooper ${INDIR3} ${SAMPLE} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &
done

# for SAMPLE in ${Samples4[@]};
#   do echo ./runLooper ${INDIR4} ${SAMPLE} ${OUTDIR}
#      nohup ./runLooper ${INDIR4} ${SAMPLE} ${OUTDIR} >& ${LOGDIR}/log_${SAMPLE}.txt &
# done
