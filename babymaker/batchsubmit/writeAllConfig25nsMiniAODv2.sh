#!/bin/bash

#
# All MT2 related datasets available on hadoop
#

TAG="V00-08-00_MiniAODv2_json_Cert_271036-274240"

#
# DATA
#

./writeConfig.sh /hadoop/cms/store/group/snt/run2_data/Run2016B_JetHT_MINIAOD_PromptReco-v2/merged/V08-00-04 ${TAG}_data_Run2016B_JetHT_PromptReco
./writeConfig.sh /hadoop/cms/store/group/snt/run2_data/Run2016B_MET_MINIAOD_PromptReco-v2/merged/V08-00-04 ${TAG}_data_Run2016B_MET_PromptReco
./writeConfig.sh /hadoop/cms/store/group/snt/run2_data/Run2016B_HTMHT_MINIAOD_PromptReco-v2/merged/V08-00-04 ${TAG}_data_Run2016B_HTMHT_PromptReco
./writeConfig.sh /hadoop/cms/store/group/snt/run2_data/Run2016B_SingleElectron_MINIAOD_PromptReco-v2/merged/V08-00-04 ${TAG}_data_Run2016B_SingleElectron_PromptReco
./writeConfig.sh /hadoop/cms/store/group/snt/run2_data/Run2016B_SingleMuon_MINIAOD_PromptReco-v2/merged/V08-00-04 ${TAG}_data_Run2016B_SingleMuon_PromptReco
./writeConfig.sh /hadoop/cms/store/group/snt/run2_data/Run2016B_SinglePhoton_MINIAOD_PromptReco-v2/merged/V08-00-04 ${TAG}_data_Run2016B_SinglePhoton_PromptReco
./writeConfig.sh /hadoop/cms/store/group/snt/run2_data/Run2016B_DoubleEG_MINIAOD_PromptReco-v2/merged/V08-00-04 ${TAG}_data_Run2016B_DoubleEG_PromptReco
./writeConfig.sh /hadoop/cms/store/group/snt/run2_data/Run2016B_DoubleMuon_MINIAOD_PromptReco-v2/merged/V08-00-04 ${TAG}_data_Run2016B_DoubleMuon_PromptReco
./writeConfig.sh /hadoop/cms/store/group/snt/run2_data/Run2016B_MuonEG_MINIAOD_PromptReco-v2/merged/V08-00-04 ${TAG}_data_Run2016B_MuonEG_PromptReco

#
# TTBAR
#

./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/TTJets_SingleLeptFromT_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_ttsl_mg_lo_top
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/TTJets_SingleLeptFromTbar_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_ttsl_mg_lo_tbar
#./writeConfig.sh  ${TAG}_ttdl_mg_lo

#
# HIGH STATS TTBAR EXTENSION
#

#./writeConfig.sh  ${TAG}_ttsl_mg_lo_top_ext1
#./writeConfig.sh  ${TAG}_ttsl_mg_lo_tbar_ext1
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/TTJets_DiLept_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1/V08-00-05/  ${TAG}_ttdl_mg_lo_ext1

#
# W+JETS
#

#./writeConfig.sh  ${TAG}_wjets_ht100to200
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_wjets_ht200to400
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_wjets_ht400to600
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/WJetsToLNu_HT-600To800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_wjets_ht600to800
#./writeConfig.sh  ${TAG}_wjets_ht800to1200
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/WJetsToLNu_HT-1200To2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_wjets_ht1200to2500
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/WJetsToLNu_HT-2500ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_wjets_ht2500toInf

#
# W+JETS extensions
#

./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1/V08-00-05/ ${TAG}_wjets_ht100to200_ext1
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1/V08-00-05/ ${TAG}_wjets_ht200to400_ext1
#./writeConfig.sh  ${TAG}_wjets_ht400to600_ext1
#./writeConfig.sh  ${TAG}_wjets_ht600to800_ext1
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1/V08-00-05/ ${TAG}_wjets_ht800to1200_ext1
#./writeConfig.sh  ${TAG}_wjets_ht1200to2500_ext1
#./writeConfig.sh  ${TAG}_wjets_ht2500toInf_ext1

#
# SINGLE TOP
#

./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/ST_s-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_singletop_amcatnlo_4f_schan
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/ST_tW_top_5f_NoFullyHadronicDecays_13TeV-powheg_TuneCUETP8M1_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_singletop_powheg_5f_tWchan_l
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/ST_tW_antitop_5f_NoFullyHadronicDecays_13TeV-powheg_TuneCUETP8M1_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_singletop_powheg_5f_tbarWchan_l
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/ST_t-channel_antitop_4f_leptonDecays_13TeV-powheg-pythia8_TuneCUETP8M1_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_singletop_powheg_4f_tbartchan_l

#
# DY+JETS
#

#./writeConfig.sh  ${TAG}_dyjetsll_ht100to200 
#./writeConfig.sh  ${TAG}_dyjetsll_ht200to400 
#./writeConfig.sh  ${TAG}_dyjetsll_ht400to600 
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/DYJetsToLL_M-50_HT-600toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_dyjetsll_ht600toInf 

#
# DY+JETS extensions
#

./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/DYJetsToLL_M-50_HT-100to200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1/V08-00-05/ ${TAG}_dyjetsll_ht100to200_ext1
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/DYJetsToLL_M-50_HT-200to400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1/V08-00-05/ ${TAG}_dyjetsll_ht200to400_ext1 
#./writeConfig.sh  ${TAG}_dyjetsll_ht400to600_ext1 
#./writeConfig.sh  ${TAG}_dyjetsll_ht600toInf_ext1 


#
# GAMMA + JETS
#

./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/GJets_DR-0p4_HT-40To100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_gjets_ht40to100 
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/GJets_DR-0p4_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_gjets_ht100to200 
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/GJets_DR-0p4_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_gjets_ht200to400 
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/GJets_DR-0p4_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_gjets_ht400to600  
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/GJets_DR-0p4_HT-600ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_gjets_ht600toInf  


#
# Z INVISIBLE
#

#./writeConfig.sh  ${TAG}_zinv_ht100to200
#./writeConfig.sh  ${TAG}_zinv_ht200to400
#./writeConfig.sh  ${TAG}_zinv_ht400to600
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/ZJetsToNuNu_HT-600To800_13TeV-madgraph_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_zinv_ht600to800
#./writeConfig.sh  ${TAG}_zinv_ht800to1200
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/ZJetsToNuNu_HT-1200To2500_13TeV-madgraph_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_zinv_ht1200to2500
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/ZJetsToNuNu_HT-2500ToInf_13TeV-madgraph_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_zinv_ht2500toInf

# #
# # DIBOSON
# #

# #
# # TRIBOSON
# #

# #
# # TTV
# #

./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/TTWJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_ttw_lnu_amcatnlo
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/TTWJetsToQQ_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_ttw_qq_amcatnlo
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/TTZToQQ_TuneCUETP8M1_13TeV-amcatnlo-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_ttz_qq_amcatnlo
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_ttz_nunu_amcatnlo
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_ttg_amcatnlo

# #
# # QCD
# #

#./writeConfig.sh  ${TAG}_qcd_ht200to300
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_qcd_ht300to500
#./writeConfig.sh  ${TAG}_qcd_ht500to700
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_qcd_ht700to1000
#./writeConfig.sh  ${TAG}_qcd_ht1000to1500
#./writeConfig.sh  ${TAG}_qcd_ht1500to2000
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_qcd_ht2000toInf

# # QCD Extension

#./writeConfig.sh  ${TAG}_qcd_ht200to300_ext1
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1/V08-00-05/ ${TAG}_qcd_ht300to500_ext1
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1/V08-00-05/ ${TAG}_qcd_ht500to700_ext1
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1/V08-00-05/ ${TAG}_qcd_ht700to1000_ext1
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1/V08-00-05/ ${TAG}_qcd_ht1000to1500_ext1
#./writeConfig.sh  ${TAG}_qcd_ht1500to2000_ext1
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1/V08-00-05/ ${TAG}_qcd_ht2000toInf_ext1


# #
# # SIGNAL
# #

./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/SMS-T1bbbb_mGluino-1500_mLSP-100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_T1bbbb_1500_100
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/SMS-T1bbbb_mGluino-1000_mLSP-900_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_T1bbbb_1000_900
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/SMS-T1tttt_mGluino-1500_mLSP-100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_T1tttt_1500_100
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/SMS-T1tttt_mGluino-1200_mLSP-800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_T1tttt_1200_800
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/SMS-T2tt_mStop-850_mLSP-100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_T2tt_850_100
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/SMS-T2tt_mStop-500_mLSP-325_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v2/V08-00-05/ ${TAG}_T2tt_500_325
./writeConfig.sh /hadoop/cms/store/group/snt/run2_25ns_80MiniAODv2/SMS-T2tt_mStop-425_mLSP-325_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/V08-00-05/ ${TAG}_T2tt_425_325


# --- write submit script ---
mkdir -p configs_${TAG}

mv condor_${TAG}*.cmd configs_${TAG}
echo "#!/bin/bash" > submitAll.sh
echo "voms-proxy-init -voms cms -valid 240:00" >> submitAll.sh
for file in configs_${TAG}/*.cmd
do 
    echo "condor_submit ${file}" >> submitAll.sh
done
chmod +x submitAll.sh
echo "[writeAllConfig] wrote submit script submitAll.sh"