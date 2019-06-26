#!/bin/bash

function run () {
    echo root -b -q mergeHadoopFiles.C\(\"${HADOOPDIR}/${TAG}_$1/skim_${SKIM}\",\"${OUTPUTDIR}/$1.root\"\)
    # nohup nice -n 19 root -b -q mergeHadoopFiles.C\(\"${HADOOPDIR}/${TAG}_$1/\",\"${OUTPUTDIR}/$1.root\"\) >& ${LOGDIR}/log_merge_${TAG}_$1.txt &
    nohup nice -n 19 root -b -q mergeHadoopFiles.C\(\"${HADOOPDIR}/${TAG}_$1/skim_${SKIM}\",\"${OUTPUTDIR}/$1.root\"\) >& ${LOGDIR}/log_merge_${TAG}_$1.txt &
}

TAG=V00-10-16_2017fullYear
# TAG=V00-10-12_2017fullYear
SKIM=base

HADOOPDIR=/hadoop/cms/store/user/${USER}/mt2babies/
OUTPUTDIR=/nfs-7/userdata/mt2/${TAG}_skim_${SKIM}/
LOGDIR=mergeLogs

mkdir -p $OUTPUTDIR
mkdir -p $LOGDIR
chmod -R a+wrx $OUTPUTDIR

#
# DATA
#

# run data_Run2018A_JetHT_17Sep2018
# run data_Run2018A_MET_17Sep2018
# run data_Run2018A_SingleMuon_17Sep2018
# run data_Run2018A_DoubleMuon_17Sep2018
# run data_Run2018A_EGamma_17Sep2018
# run data_Run2018A_MuonEG_17Sep2018

# run data_Run2018B_JetHT_17Sep2018
# run data_Run2018B_MET_17Sep2018
# run data_Run2018B_SingleMuon_17Sep2018
# run data_Run2018B_DoubleMuon_17Sep2018
# run data_Run2018B_EGamma_17Sep2018
# run data_Run2018B_MuonEG_17Sep2018

# run data_Run2018C_JetHT_17Sep2018
# run data_Run2018C_MET_17Sep2018
# run data_Run2018C_SingleMuon_17Sep2018
# run data_Run2018C_DoubleMuon_17Sep2018
# run data_Run2018C_EGamma_17Sep2018
# run data_Run2018C_MuonEG_17Sep2018

# run data_Run2018D_JetHT_PromptReco-v2
# run data_Run2018D_MET_PromptReco-v2
# run data_Run2018D_SingleMuon_PromptReco-v2
# run data_Run2018D_DoubleMuon_PromptReco-v2
# run data_Run2018D_EGamma_PromptReco-v2
# run data_Run2018D_MuonEG_PromptReco-v2

# run data_Run2018B_JetHT_17Sep2018
# run data_Run2018B_EGamma_17Sep2018
# run data_Run2018B_SingleMuon_17Sep2018
# # run data_Run2018B_DoubleMuon_17Sep2018

# run data_Run2018C_EGamma_17Sep2018
# run data_Run2018C_MET_17Sep2018
# run data_Run2018C_SingleMuon_17Sep2018
# # run data_Run2018C_DoubleMuon_17Sep2018

# run data_Run2018D_JetHT_PromptReco-v2
# run data_Run2018D_EGamma_22Jan2019-v2
# run data_Run2018D_MET_PromptReco-v2
# run data_Run2018D_SingleMuon_PromptReco-v2
# # run data_Run2018D_DoubleMuon_PromptReco-v2

# run data_Run2017B_JetHT_31Mar2018
# run data_Run2017C_JetHT_31Mar2018
# run data_Run2017D_JetHT_31Mar2018
# run data_Run2017E_JetHT_31Mar2018
# run data_Run2017F_JetHT_31Mar2018

# run data_Run2017B_HTMHT_31Mar2018
# run data_Run2017C_HTMHT_31Mar2018
# run data_Run2017D_HTMHT_31Mar2018
# run data_Run2017E_HTMHT_31Mar2018
# run data_Run2017F_HTMHT_31Mar2018

# run data_Run2017B_MET_31Mar2018
# run data_Run2017C_MET_31Mar2018
# run data_Run2017D_MET_31Mar2018
# run data_Run2017E_MET_31Mar2018
# run data_Run2017F_MET_31Mar2018

# run data_Run2017B_SingleMuon_31Mar2018
# run data_Run2017C_SingleMuon_31Mar2018
# run data_Run2017D_SingleMuon_31Mar2018
# run data_Run2017E_SingleMuon_31Mar2018
# run data_Run2017F_SingleMuon_31Mar2018

# run data_Run2017B_SingleElectron_31Mar2018
# run data_Run2017C_SingleElectron_31Mar2018
# run data_Run2017D_SingleElectron_31Mar2018
# run data_Run2017E_SingleElectron_31Mar2018
# run data_Run2017F_SingleElectron_31Mar2018

# run data_Run2017B_SinglePhoton_31Mar2018
# run data_Run2017C_SinglePhoton_31Mar2018
# run data_Run2017D_SinglePhoton_31Mar2018
# run data_Run2017E_SinglePhoton_31Mar2018
# run data_Run2017F_SinglePhoton_31Mar2018

# run data_Run2017B_MuonEG_31Mar2018
# run data_Run2017C_MuonEG_31Mar2018
# run data_Run2017D_MuonEG_31Mar2018
# run data_Run2017E_MuonEG_31Mar2018
# run data_Run2017F_MuonEG_31Mar2018

# run data_Run2017B_DoubleEG_31Mar2018
# run data_Run2017C_DoubleEG_31Mar2018
# run data_Run2017D_DoubleEG_31Mar2018
# run data_Run2017E_DoubleEG_31Mar2018
# run data_Run2017F_DoubleEG_31Mar2018

# run data_Run2017B_DoubleMuon_31Mar2018
# run data_Run2017C_DoubleMuon_31Mar2018
# run data_Run2017D_DoubleMuon_31Mar2018
# run data_Run2017E_DoubleMuon_31Mar2018
# run data_Run2017F_DoubleMuon_31Mar2018

# run data_Run2016B_DoubleEG_17Jul2018_ver1-v1
# run data_Run2016B_DoubleEG_17Jul2018_ver2-v1
# run data_Run2016C_DoubleEG_17Jul2018-v1
# run data_Run2016D_DoubleEG_17Jul2018-v1
# run data_Run2016E_DoubleEG_17Jul2018-v1
# run data_Run2016G_DoubleEG_17Jul2018-v1
# run data_Run2016H_DoubleEG_17Jul2018-v1

# run data_Run2016B_DoubleMuon_17Jul2018_ver1-v1
# run data_Run2016B_DoubleMuon_17Jul2018_ver2-v1
# run data_Run2016C_DoubleMuon_17Jul2018-v1
# run data_Run2016D_DoubleMuon_17Jul2018-v1
# run data_Run2016E_DoubleMuon_17Jul2018-v1
# run data_Run2016G_DoubleMuon_17Jul2018-v1
# run data_Run2016H_DoubleMuon_17Jul2018-v1

#
# TTBAR
#

# # run ttdl_mg
# run ttdl_powheg
# run ttsl_top_mg
# run ttsl_tbar_mg
# run tttt

#
# HIGH STATS TTBAR EXTENSION
#

# run ttsl_mg_lo_top_ext1
# run ttsl_mg_lo_tbar_ext1
# run ttdl_mg_lo_ext1

#
# W+JETS
#

# run wjets_ht100to200
# run wjets_ht200to400
# run wjets_ht400to600
# run wjets_ht600to800
# run wjets_ht800to1200
# run wjets_ht1200to2500
# run wjets_ht2500toInf

# run wjets_ht100to200_ext1
# run wjets_ht200to400_ext1
# run wjets_ht400to600_ext1
# run wjets_ht600to800_ext1
# run wjets_ht800to1200_ext1
# run wjets_ht1200to2500_ext1
# run wjets_ht2500toInf_ext1

# # run wjets_incl

#
# SINGLE TOP
#

# run singletop_amcatnlo_4f_schan
# run singletop_powheg_5f_tWchan_l
# run singletop_powheg_5f_tbarWchan_l
# run singletop_powheg_4f_ttchan_l
# run singletop_powheg_4f_tbartchan_l

# run singletop_tW_tbar
# run singletop_schan
# run singletop_tchan_tbar
# run singletop_tchan_top

#
# DY+JETS
#

# run dyjetsll_ht100to200
# run dyjetsll_ht200to400
# run dyjetsll_ht400to600
# run dyjetsll_ht600to800
# run dyjetsll_ht800to1200
# run dyjetsll_ht1200to2500
# run dyjetsll_ht2500toInf
# # # #run dyjetsll_ht600toInf

# run dyjetsll_ht100to200_ext1
# run dyjetsll_ht200to400_ext1
# run dyjetsll_ht400to600_ext1
# #run dyjetsll_ht600toInf_ext1

# run dyjetsll_m50_incl


#
# GAMMA + JETS
#

# run gjets_dr0p4_ht40to100
# # run gjets_dr0p4_ht100to200
# run gjets_dr0p4_ht200to400
# run gjets_dr0p4_ht400to600
# run gjets_dr0p4_ht600toInf

# run gjets_dr0p05_ht40to100
# # run gjets_dr0p05_ht100to200
# run gjets_dr0p05_ht200to400
# run gjets_dr0p05_ht400to600
# run gjets_dr0p05_ht600toInf

#
# Z INVISIBLE
#

# run zinv_ht100to200
# run zinv_ht200to400
# run zinv_ht400to600
# run zinv_ht600to800
# run zinv_ht800to1200
# run zinv_ht1200to2500
# run zinv_ht2500toInf

# run zinv_ht100to200_ext1
# run zinv_ht200to400_ext1
# run zinv_ht400to600_ext1
# # run zinv_ht600to800_ext1
# # run zinv_ht800to1200_ext1
# run zinv_ht1200to2500_ext1
# # run zinv_ht2500toInf_ext1

#
# DIBOSON
#

# run ww
# run wz

#
# TRIBOSON
#

#
# TTV, TT+X
#

# run ttg_amcatnlo
# run ttw_lnu_amcatnlo
# run ttw_qq_amcatnlo
# run ttz_nunu_amcatnlo
# # run ttz_qq_amcatnlo

# run ttw_mg
# run ttz_mg

# run tttt
# run ttbb
# run ttbb_ext1


# run tth_bb
# run tth_nonbb

#
# QCD
#

# run qcd_pt80to120
# run qcd_pt120to170
# run qcd_pt170to300
# run qcd_pt300to470
# run qcd_pt470to600
# run qcd_pt600to800
# run qcd_pt800to1000
# run qcd_pt1000to1400
# run qcd_pt1400to1800
# run qcd_pt1800to2400
# run qcd_pt2400to3200
# run qcd_pt3200toInf

# run qcd_pt470to600_ext1
# run qcd_pt600to800_ext1

# run qcd_ht200to300
# run qcd_ht300to500
# run qcd_ht500to700
# run qcd_ht700to1000
# run qcd_ht1000to1500
# run qcd_ht1500to2000
# run qcd_ht2000toInf

# run qcd_ht200to300_ext1
# run qcd_ht300to500_ext1
# run qcd_ht500to700_ext1
# run qcd_ht700to1000_ext1
# run qcd_ht1000to1500_ext1
# run qcd_ht1500to2000_ext1
# run qcd_ht2000toInf_ext1

#
# SIGNAL
#

# run T1tttt_1500_100
# run T1tttt_1200_800
# run T1bbbb_1500_100
# run T1bbbb_1000_900
# # run T1qqqq_1400_100
# # run T1qqqq_1000_800

#
# SIGNAL SCANS
#

# run T1tttt
# run T1bbbb
# run T1bbbb_ext1
# run T1qqqq
# run T1qqqq_80x
# # run T5qqqqVV
# run T2bb_80x
# run T2tt_mStop1200to2000
# run T2tt_mStop400to1200
# run T2tt_mStop150to250
# run T2tt_mStop250to350
# run T2tt_mStop350to400
# run T2tt_mStop1200to2000_ext1
# run T2tt_mStop400to1200_ext1
# run T2tt_mStop150to250_ext1
# run T2tt_mStop250to350_ext1
# run T2tt_mStop350to400_ext1
# run T2qq_mSq0to1850
# run T2qq_mSq1850to2600
# run T2bb_mSbot0to1850
# run T2bb_mSbot1850to2600
# run T2cc
# run T2bW
# run T2bt
run T5qqqqVV_dM20_ext1
run T5qqqqVV_mChg0p5mGlumLSP_ext1
# run rpvMonoPhi
# run extraT2tt