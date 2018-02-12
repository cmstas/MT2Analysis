#!/bin/bash

#
# All MT2 related datasets available on hadoop
#

# TAG="RebalanceAndSmear_V00-09-02_29p02fb"
# TAG="V00-09-04_json_294927-306462_PromptReco_41p96fb"
TAG="V00-09-04_json_294927-306462_17Nov2017"
# TAG="V00-09-05_Fall17MC"

#
# DATA
#

# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/JetHT_Run2017B-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_JetHT_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MET_Run2017B-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_MET_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/HTMHT_Run2017B-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_HTMHT_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleMuon_Run2017B-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_SingleMuon_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleElectron_Run2017B-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_SingleElectron_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SinglePhoton_Run2017B-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_SinglePhoton_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleMuon_Run2017B-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_DoubleMuon_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleEG_Run2017B-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_DoubleEG_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MuonEG_Run2017B-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_MuonEG_PromptReco-v1

# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/JetHT_Run2017B-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_JetHT_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MET_Run2017B-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_MET_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/HTMHT_Run2017B-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_HTMHT_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleMuon_Run2017B-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_SingleMuon_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleElectron_Run2017B-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_SingleElectron_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SinglePhoton_Run2017B-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_SinglePhoton_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleMuon_Run2017B-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_DoubleMuon_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleEG_Run2017B-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_DoubleEG_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MuonEG_Run2017B-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_MuonEG_PromptReco-v2

# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/JetHT_Run2017C-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_JetHT_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MET_Run2017C-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_MET_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/HTMHT_Run2017C-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_HTMHT_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleMuon_Run2017C-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_SingleMuon_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleElectron_Run2017C-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_SingleElectron_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SinglePhoton_Run2017C-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_SinglePhoton_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleMuon_Run2017C-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_DoubleMuon_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleEG_Run2017C-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_DoubleEG_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MuonEG_Run2017C-PromptReco-v1_MINIAOD_CMS4_V00-00-06> ${TAG}_data_Run2017C_MuonEG_PromptReco-v1

# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/JetHT_Run2017C-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_JetHT_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MET_Run2017C-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_MET_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/HTMHT_Run2017C-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_HTMHT_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleMuon_Run2017C-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_SingleMuon_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleElectron_Run2017C-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_SingleElectron_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SinglePhoton_Run2017C-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_SinglePhoton_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleMuon_Run2017C-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_DoubleMuon_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleEG_Run2017C-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_DoubleEG_PromptReco-v2
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MuonEG_Run2017C-PromptReco-v2_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_MuonEG_PromptReco-v2

# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/JetHT_Run2017C-PromptReco-v3_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_JetHT_PromptReco-v3
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MET_Run2017C-PromptReco-v3_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_MET_PromptReco-v3
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/HTMHT_Run2017C-PromptReco-v3_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_HTMHT_PromptReco-v3
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleMuon_Run2017C-PromptReco-v3_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_SingleMuon_PromptReco-v3
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleElectron_Run2017C-PromptReco-v3_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_SingleElectron_PromptReco-v3
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SinglePhoton_Run2017C-PromptReco-v3_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_SinglePhoton_PromptReco-v3
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleMuon_Run2017C-PromptReco-v3_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_DoubleMuon_PromptReco-v3
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleEG_Run2017C-PromptReco-v3_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_DoubleEG_PromptReco-v3
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MuonEG_Run2017C-PromptReco-v3_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_MuonEG_PromptReco-v3

# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/JetHT_Run2017D-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017D_JetHT_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MET_Run2017D-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017D_MET_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/HTMHT_Run2017D-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017D_HTMHT_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleMuon_Run2017D-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017D_SingleMuon_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleElectron_Run2017D-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017D_SingleElectron_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SinglePhoton_Run2017D-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017D_SinglePhoton_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleMuon_Run2017D-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017D_DoubleMuon_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleEG_Run2017D-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017D_DoubleEG_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MuonEG_Run2017D-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017D_MuonEG_PromptReco-v1

# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/JetHT_Run2017E-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017E_JetHT_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MET_Run2017E-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017E_MET_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/HTMHT_Run2017E-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017E_HTMHT_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleMuon_Run2017E-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017E_SingleMuon_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleElectron_Run2017E-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017E_SingleElectron_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SinglePhoton_Run2017E-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017E_SinglePhoton_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleMuon_Run2017E-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017E_DoubleMuon_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleEG_Run2017E-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017E_DoubleEG_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MuonEG_Run2017E-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017E_MuonEG_PromptReco-v1

# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/JetHT_Run2017F-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017F_JetHT_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MET_Run2017F-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017F_MET_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/HTMHT_Run2017F-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017F_HTMHT_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleMuon_Run2017F-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017F_SingleMuon_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SingleElectron_Run2017F-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017F_SingleElectron_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/SinglePhoton_Run2017F-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017F_SinglePhoton_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleMuon_Run2017F-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017F_DoubleMuon_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/DoubleEG_Run2017F-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017F_DoubleEG_PromptReco-v1
# ./writeConfig.sh /hadoop/cms/store/user/namin/ProjectMetis/MuonEG_Run2017F-PromptReco-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017F_MuonEG_PromptReco-v1


./writeConfig.sh /hadoop/cms/store/user/smay/ProjectMetis/DoubleMuon_Run2017B-17Nov2017-v1_MINIAOD_CMS4_V00-00-06_allPfCands ${TAG}_data_Run2017B_DoubleMuon_17Nov2017-v1
./writeConfig.sh /hadoop/cms/store/user/smay/ProjectMetis/DoubleEG_Run2017B-17Nov2017-v1_MINIAOD_CMS4_V00-00-06_allPfCands ${TAG}_data_Run2017B_DoubleEG_17Nov2017-v1
./writeConfig.sh /hadoop/cms/store/user/bemarsh/ProjectMetis/SinglePhoton_Run2017B-17Nov2017-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017B_SinglePhoton_17Nov2017-v1

./writeConfig.sh /hadoop/cms/store/user/smay/ProjectMetis/DoubleMuon_Run2017C-17Nov2017-v1_MINIAOD_CMS4_V00-00-06_allPfCands ${TAG}_data_Run2017C_DoubleMuon_17Nov2017-v1
./writeConfig.sh /hadoop/cms/store/user/smay/ProjectMetis/DoubleEG_Run2017C-17Nov2017-v1_MINIAOD_CMS4_V00-00-06_allPfCands ${TAG}_data_Run2017C_DoubleEG_17Nov2017-v1
./writeConfig.sh /hadoop/cms/store/user/bemarsh/ProjectMetis/SinglePhoton_Run2017C-17Nov2017-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017C_SinglePhoton_17Nov2017-v1

./writeConfig.sh /hadoop/cms/store/user/smay/ProjectMetis/DoubleMuon_Run2017D-17Nov2017-v1_MINIAOD_CMS4_V00-00-06_allPfCands ${TAG}_data_Run2017D_DoubleMuon_17Nov2017-v1
./writeConfig.sh /hadoop/cms/store/user/smay/ProjectMetis/DoubleEG_Run2017D-17Nov2017-v1_MINIAOD_CMS4_V00-00-06_allPfCands ${TAG}_data_Run2017D_DoubleEG_17Nov2017-v1
./writeConfig.sh /hadoop/cms/store/user/bemarsh/ProjectMetis/SinglePhoton_Run2017D-17Nov2017-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017D_SinglePhoton_17Nov2017-v1

./writeConfig.sh /hadoop/cms/store/user/smay/ProjectMetis/DoubleMuon_Run2017E-17Nov2017-v1_MINIAOD_CMS4_V00-00-06_allPfCands ${TAG}_data_Run2017E_DoubleMuon_17Nov2017-v1
./writeConfig.sh /hadoop/cms/store/user/smay/ProjectMetis/DoubleEG_Run2017E-17Nov2017-v1_MINIAOD_CMS4_V00-00-06_allPfCands ${TAG}_data_Run2017E_DoubleEG_17Nov2017-v1
./writeConfig.sh /hadoop/cms/store/user/bemarsh/ProjectMetis/SinglePhoton_Run2017E-17Nov2017-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017E_SinglePhoton_17Nov2017-v1

./writeConfig.sh /hadoop/cms/store/user/smay/ProjectMetis/DoubleMuon_Run2017F-17Nov2017-v1_MINIAOD_CMS4_V00-00-06_allPfCands ${TAG}_data_Run2017F_DoubleMuon_17Nov2017-v1
./writeConfig.sh /hadoop/cms/store/user/smay/ProjectMetis/DoubleEG_Run2017F-17Nov2017-v1_MINIAOD_CMS4_V00-00-06_allPfCands ${TAG}_data_Run2017F_DoubleEG_17Nov2017-v1
./writeConfig.sh /hadoop/cms/store/user/bemarsh/ProjectMetis/SinglePhoton_Run2017F-17Nov2017-v1_MINIAOD_CMS4_V00-00-06 ${TAG}_data_Run2017F_SinglePhoton_17Nov2017-v1


# #
# # TTBAR
# #


# #
# # HIGH STATS TTBAR EXTENSION
# #

# #
# # W+JETS
# #

# #
# # W+JETS extensions
# #

# #
# # SINGLE TOP
# #

# #
# # DY+JETS
# #

# ./writeConfig.sh /hadoop/cms/store/user/bemarsh/ProjectMetis/DYJetsToLL_M-50_HT-200to400_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V00-00-10/ ${TAG}_dyjetsll_ht200to400
# ./writeConfig.sh /hadoop/cms/store/user/bemarsh/ProjectMetis/DYJetsToLL_M-50_HT-400to600_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V00-00-10/ ${TAG}_dyjetsll_ht400to600
# ./writeConfig.sh /hadoop/cms/store/user/bemarsh/ProjectMetis/DYJetsToLL_M-50_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V00-00-10/ ${TAG}_dyjetsll_ht600to800
# ./writeConfig.sh /hadoop/cms/store/user/bemarsh/ProjectMetis/DYJetsToLL_M-50_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V00-00-10/ ${TAG}_dyjetsll_ht800to1200

# #
# # GAMMA + JETS
# #

# ./writeConfig.sh /hadoop/cms/store/user/bemarsh/ProjectMetis/GJets_HT-40To100_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V00-00-10/ ${TAG}_gjets_dr0p4_ht40to100
# ./writeConfig.sh /hadoop/cms/store/user/bemarsh/ProjectMetis/GJets_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v2_MINIAODSIM_CMS4_V00-00-10/ ${TAG}_gjets_dr0p4_ht100to200
# ./writeConfig.sh /hadoop/cms/store/user/bemarsh/ProjectMetis/GJets_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v2_MINIAODSIM_CMS4_V00-00-10/ ${TAG}_gjets_dr0p4_ht200to400
# ./writeConfig.sh /hadoop/cms/store/user/bemarsh/ProjectMetis/GJets_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v2_MINIAODSIM_CMS4_V00-00-10/ ${TAG}_gjets_dr0p4_ht400to600
# ./writeConfig.sh /hadoop/cms/store/user/bemarsh/ProjectMetis/GJets_HT-600ToInf_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V00-00-10/ ${TAG}_gjets_dr0p4_ht600toInf

# #
# # Z INVISIBLE
# #

# #
# # Z INVISIBLE EXTENSIONS
# #

# # #
# # # DIBOSON
# # #


# # #
# # # TRIBOSON
# # #

# # #
# # # TTV, tt+X
# # #

# #
# # QCD HT BINS
# #

# #
# # QCD HT BINS Extension
# #

#
# QCD PT bins
#


# #
# # SIGNAL
# #

# # SIGNAL SCANS
# #


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
