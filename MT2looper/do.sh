#!/bin/bash

make -j12

#INDIR=/nfs-6/userdata/mt2/V00-00-12/
#INDIR=/nfs-6/userdata/mt2/SRSM/
#INDIR=/nfs-6/userdata/mt2/scsm_skim_trig_nj2_ht450_met30_Zinv
#INDIR=/nfs-6/userdata/mt2/SRSM_skim_trig_nj2_ht450_met30_Zinv
#INDIR=/nfs-6/userdata/mt2/SRSM_skim_trig_nj2_ht450_met30_mt2g200_Zinv
#INDIR=/nfs-6/userdata/mt2/V00-00-12_skim_trig_nj2_ht450_met30_mt2gt200_Zinv
#OUTDIR=output/rawBabyOld/
#OUTDIR=output/noisomt2_200/
#OUTDIR=output/test/

#declare -a Samples=(ttall_msdecays wjets_ht zinv_ht dyjetsll_ht qcd_ht gjet_ht)
#declare -a Samples=(T1tttt_1500_100 T1tttt_1200_800 T1bbbb_1500_100 T1bbbb_1000_900 T1qqqq_1400_100 T1qqqq_1000_800 T2tt_850_100 T2tt_650_325 T2tt_500_325 T2tt_425_325 T2bb_900_100 T2bb_600_580 T2qq_1200_100 T2qq_600_550)
#declare -a Samples=(qcd_pt30to50 qcd_pt50to80 qcd_pt80to120 qcd_pt120to170 qcd_pt170to300 qcd_pt300to470 qcd_pt470to600 qcd_pt600to800 qcd_pt800to1000 qcd_pt1000to1400 qcd_pt1400to1800 qcd_pt1800to2400 qcd_pt2400to3200 qcd_pt3200)
#declare -a Samples=( ttall_msdecays wjets_ht)
#declare -a Samples=(ttall_msdecays wjets_ht zinv_ht qcd_pt T5qqqqWWDeg_mGo1000_mCh310_mChi300 T5qqqqWWDeg_mGo1000_mCh315_mChi300 T5qqqqWWDeg_mGo1000_mCh325_mChi300 T5qqqqWWDeg_mGo800_mCh305_mChi300)
#declare -a Samples=(ttall_msdecays wjets_ht zinv_ht qcd_pt T5qqqqWW_315_300 T5qqqqWW_325_300 T5qqqqWW_310_300 T5qqqqWW_305_300)
#declare -a Samples=(T5qqqqWW_315_300 T5qqqqWW_325_300 T5qqqqWW_310_300 T5qqqqWW_305_300)

#INDIR=/nfs-6/userdata/mt2/V00-00-12_root6
#INDIR=/nfs-6/userdata/mt2/V00-01-04_25ns_softleps
INDIR=/nfs-6/userdata/mt2/V00-01-04_25ns_softleps_skim_trig_njets30_ht_met_mt2_Zinv
OUTDIR=output/elec5_absiso/

declare -a Samples=(ttall_mg_lo wjets_ht zinv_ht qcd_pt T5qqqqWW_1000_775_750)
#declare -a Samples=(wjets_ht100to200 wjets_ht200to400 wjets_ht400to600 wjets_ht600toInf wjets_ht600to800 wjets_ht800to1200 wjets_ht1200to2500 wjets_ht2500toInf)

mkdir -p ${OUTDIR}

for SAMPLE in ${Samples[@]};
  do echo ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR}
  nohup ./runLooper ${INDIR} ${SAMPLE} ${OUTDIR} >& log_${SAMPLE}.txt &
done

# INDIR2=/hadoop/cms/store/user/sicheng/mt2babies
# # INDIR2=/nfs-6/userdata/mt2/SRSM_skim_trig_nj2_ht450_met30_Zinv
# declare -a Samples2=(T5qqqqWW_1000_775_750) 
# #declare -a Samples2=(T5qqqqWWDeg_mGo1000_mCh310_mChi300)

# for SAMPLE2 in ${Samples2[@]};
#   do echo ./runLooper ${INDIR2} ${SAMPLE2} ${OUTDIR}
#   nohup ./runLooper ${INDIR2} ${SAMPLE2} ${OUTDIR} >& log_${SAMPLE2}.txt &
# done

# INDIR3=/nfs-6/userdata/mt2/sctest_skim_trig_nj2_ht450_met30_Zinv/
# declare -a Samples3=(qcd_pt)

# for SAMPLE3 in ${Samples3[@]};
#   do echo root -b -q -l doAll.C\(\"${INDIR3}\",\"${SAMPLE3}\",\"${OUTDIR}\"\)
#   nohup root -b -q -l doAll.C\(\"${INDIR3}\",\"${SAMPLE3}\",\"${OUTDIR}\"\) >& log_${SAMPLE3}.txt &
# done

