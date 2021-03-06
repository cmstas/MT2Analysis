#! /bin/bash

# INPATH=/nfs-6/userdata/mt2/2016_moriond_results
# OUTPATH=/nfs-6/userdata/mt2/2016_moriond_results_SR_only

# INPATH=/nfs-6/userdata/mt2/V00-10-01_json_314472-317696_2017C_JECs/
# OUTPATH=/nfs-6/userdata/mt2/V00-10-01_json_314472-317696_2017C_JECs_RS_skim

# INPATH=/nfs-6/userdata/mt2/V00-10-01_json_294927-306462_31Mar2018_correctJECs
# OUTPATH=/nfs-6/userdata/mt2/V00-10-01_json_294927-306462_31Mar2018_correctJECs_RS_skim_v2

# INPATH=/nfs-6/userdata/mt2/V00-09-04_json_294927-306462_PromptReco_41p96fb_skim_noMT2/
# OUTPATH=/nfs-6/userdata/mt2/V00-09-04_json_294927-306462_PromptReco_41p96fb_trigger_skim

INPATH=/nfs-6/userdata/mt2/V00-10-06_2017fullYear_31Mar2018/
OUTPATH=/nfs-6/userdata/mt2/V00-10-06_2017fullYear_31Mar2018_trigger_skim/

# INPATH=/nfs-6/userdata/mt2/V00-10-04_2016fullYear_17Jul2018/
# OUTPATH=/nfs-6/userdata/mt2/V00-10-04_2016fullYear_17Jul2018_trigger_skim/

# INPATH=/nfs-6/userdata/mt2/V00-10-06_2018fullYear_17Sep2018/
# OUTPATH=/nfs-6/userdata/mt2/V00-10-06_2018fullYear_17Sep2018_trigger_skim

# INPATH=/nfs-6/userdata/mt2/V00-10-02_testHcalOfflineFix/
# OUTPATH=/nfs-6/userdata/mt2/V00-10-02_testHcalOfflineFix_RS_skim

mkdir -p $OUTPATH

declare -a Samples=()

# Samples+=(data_Run2018A_MET data_Run2018B_MET data_Run2018C_MET data_Run2018D_MET)
# Samples+=(data_Run2018A_EGamma data_Run2018B_EGamma data_Run2018C_EGamma data_Run2018D_EGamma)
# Samples+=(data_Run2018A_SingleMuon data_Run2018B_SingleMuon data_Run2018C_SingleMuon data_Run2018D_SingleMuon)

# Samples+=(data_Run2017B_JetHT data_Run2017C_JetHT data_Run2017D_JetHT data_Run2017E_JetHT data_Run2017F_JetHT)
Samples+=(data_Run2017B_MET data_Run2017C_MET data_Run2017D_MET data_Run2017E_MET data_Run2017F_MET)
# Samples+=(data_Run2017B_HTMHT data_Run2017C_HTMHT data_Run2017D_HTMHT data_Run2017E_HTMHT data_Run2017F_HTMHT)
# Samples+=(data_Run2017B_SingleMuon data_Run2017C_SingleMuon data_Run2017D_SingleMuon data_Run2017E_SingleMuon data_Run2017F_SingleMuon)
Samples+=(data_Run2017B_SingleElectron data_Run2017C_SingleElectron data_Run2017D_SingleElectron data_Run2017E_SingleElectron data_Run2017F_SingleElectron)
# Samples+=(data_Run2017B_SinglePhoton data_Run2017C_SinglePhoton data_Run2017D_SinglePhoton data_Run2017E_SinglePhoton data_Run2017F_SinglePhoton)
# Samples+=(data_Run2017B_DoubleMuon data_Run2017C_DoubleMuon data_Run2017D_DoubleMuon data_Run2017E_DoubleMuon data_Run2017F_DoubleMuon)
# Samples+=(data_Run2017B_DoubleEG data_Run2017C_DoubleEG data_Run2017D_DoubleEG data_Run2017E_DoubleEG data_Run2017F_DoubleEG)
# Samples+=(data_Run2017B_MuonEG data_Run2017C_MuonEG data_Run2017D_MuonEG data_Run2017E_MuonEG data_Run2017F_MuonEG)

# Samples+=(data_Run2016B_SingleElectron data_Run2016C_SingleElectron data_Run2016D_SingleElectron data_Run2016E_SingleElectron data_Run2016F_SingleElectron data_Run2016G_SingleElectron data_Run2016H_SingleElectron)
# Samples+=(data_Run2016B_MET data_Run2016C_MET data_Run2016D_MET data_Run2016E_MET data_Run2016F_MET data_Run2016G_MET data_Run2016H_MET)
# Samples+=(data_Run2016B_JetHT data_Run2016C_JetHT data_Run2016D_JetHT data_Run2016E_JetHT data_Run2016F_JetHT data_Run2016G_JetHT data_Run2016H_JetHT)
# Samples+=(data_Run2016B_SingleMuon data_Run2016C_SingleMuon data_Run2016D_SingleMuon data_Run2016E_SingleMuon data_Run2016F_SingleMuon data_Run2016G_SingleMuon data_Run2016H_SingleMuon)

# Samples+=(data_Run2018A_JetHT data_Run2018A_MET data_Run2018A_SingleMuon data_Run2018A_EGamma data_Run2018A_MuonEG)
# Samples+=(data_Run2018B_JetHT data_Run2018B_MET data_Run2018B_SingleMuon data_Run2018B_EGamma data_Run2018B_MuonEG)
# Samples+=(data_Run2018C_JetHT data_Run2018C_MET)
# Samples+=(data_Run2018A_DoubleMuon data_Run2018B_DoubleMuon)
# Samples+=(data_Run2018A_JetHT data_Run2018B_JetHT)
# Samples+=(data_Run2018A_MET data_Run2018B_MET)
# Samples+=(data_Run2016B data_Run2016C data_Run2016D)
# Samples+=(wjets_ht gjets_dr0p05_ht)
# Samples+=(qcd_ht dyjetsll_ht)
# Samples+=(ttdl ttsl singletop ttw ttz ttg)
# Samples+=(singletop_tWchan_top)
# samples+=(qcd_ht100to200 qcd_ht200to300 qcd_ht300to500 qcd_ht500to700 qcd_ht700to1000 qcd_ht1000to1500 qcd_ht1500to2000 qcd_ht2000toInf)
# Samples+=(data_Run2018A_JetHT data_Run2018A_MET data_Run2018A_SingleMuon data_Run2018A_EGamma)
# Samples+=(zinv_ht ttsl ttdl singletop tt_z tt_w tth ttg qcd_ht dy gjets_ht ww wz)
# Samples+=(wjets_ht100to200 wjets_ht200to400 wjets_ht400to600 wjets_ht600to800 wjets_ht800to1200 wjets_ht1200to2500 wjets_ht2500toInf)
# Samples+=(wjets_ht zinv_ht ttsl ttdl singletop ttz ttw tth ttg gjets_dr0p4_ht dyjetsll_ht qcd_ht ww_2l2nu)
# Samples+=(T1tttt T1bbbb T1qqqq T2tt T2bb T2qq)

for SAMPLE in ${Samples[@]}
  do nohup nice -n10 root -b -l -q skim_trigger.C\(\"$INPATH\",\"$OUTPATH\",\"$SAMPLE\"\) &> log_${SAMPLE}.txt &
done
