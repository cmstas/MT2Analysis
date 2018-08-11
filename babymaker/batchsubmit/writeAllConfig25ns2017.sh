#!/bin/bash

#
# All MT2 related datasets available on hadoop
#

# TAG="RebalanceAndSmear_V00-09-06"
# TAG="V00-09-04_json_294927-306462_PromptReco_41p96fb"
# TAG="RebalanceAndSmear_V00-10-01_json_294927-306462_31Mar2018_correctJECs"
# TAG="RebalanceAndSmear_V00-10-01_94X_mc"
TAG="V00-10-03_94x_2017_mc"

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


# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/JetHT_Run2017B-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017B_JetHT_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/MET_Run2017B-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017B_MET_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/HTMHT_Run2017B-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017B_HTMHT_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SingleMuon_Run2017B-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017B_SingleMuon_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SingleElectron_Run2017B-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017B_SingleElectron_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SinglePhoton_Run2017B-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017B_SinglePhoton_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/DoubleMuon_Run2017B-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017B_DoubleMuon_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/DoubleEG_Run2017B-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017B_DoubleEG_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/MuonEG_Run2017B-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017B_MuonEG_31Mar2018

# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/JetHT_Run2017C-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017C_JetHT_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/MET_Run2017C-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017C_MET_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/HTMHT_Run2017C-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017C_HTMHT_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SingleMuon_Run2017C-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017C_SingleMuon_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SingleElectron_Run2017C-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017C_SingleElectron_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SinglePhoton_Run2017C-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017C_SinglePhoton_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/DoubleMuon_Run2017C-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017C_DoubleMuon_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/DoubleEG_Run2017C-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017C_DoubleEG_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/MuonEG_Run2017C-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017C_MuonEG_31Mar2018

# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/JetHT_Run2017D-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017D_JetHT_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/MET_Run2017D-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017D_MET_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/HTMHT_Run2017D-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017D_HTMHT_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SingleMuon_Run2017D-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017D_SingleMuon_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SingleElectron_Run2017D-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017D_SingleElectron_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SinglePhoton_Run2017D-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017D_SinglePhoton_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/DoubleMuon_Run2017D-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017D_DoubleMuon_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/DoubleEG_Run2017D-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017D_DoubleEG_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/MuonEG_Run2017D-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017D_MuonEG_31Mar2018

# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/JetHT_Run2017E-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017E_JetHT_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/MET_Run2017E-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017E_MET_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/HTMHT_Run2017E-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017E_HTMHT_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SingleMuon_Run2017E-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017E_SingleMuon_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SingleElectron_Run2017E-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017E_SingleElectron_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SinglePhoton_Run2017E-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017E_SinglePhoton_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/DoubleMuon_Run2017E-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017E_DoubleMuon_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/DoubleEG_Run2017E-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017E_DoubleEG_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/MuonEG_Run2017E-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017E_MuonEG_31Mar2018

# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/JetHT_Run2017F-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017F_JetHT_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/MET_Run2017F-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017F_MET_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/HTMHT_Run2017F-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017F_HTMHT_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SingleMuon_Run2017F-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017F_SingleMuon_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SingleElectron_Run2017F-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017F_SingleElectron_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/SinglePhoton_Run2017F-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017F_SinglePhoton_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/DoubleMuon_Run2017F-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017F_DoubleMuon_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/DoubleEG_Run2017F-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017F_DoubleEG_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/MuonEG_Run2017F-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017F_MuonEG_31Mar2018

# # #
# # # JetHT only for Rebalance and Smear
# # #
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/JetHT_Run2017B-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017B_JetHT_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/JetHT_Run2017C-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017C_JetHT_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/JetHT_Run2017D-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017D_JetHT_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/JetHT_Run2017E-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017E_JetHT_31Mar2018
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_data2017/JetHT_Run2017F-31Mar2018-v1_MINIAOD_CMS4_V09-04-12/ ${TAG}_data_Run2017F_JetHT_31Mar2018


# #
# # TTBAR
# #
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_ttsl
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_ttdl

# #
# # HIGH STATS TTBAR EXTENSION
# #

# #
# # W+JETS
# #
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_wjets_ht100to200
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_wjets_ht200to400
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_wjets_ht400to600
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_wjets_ht600to800
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_wjets_ht800to1200
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_wjets_ht1200to2500
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v3_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_wjets_ht2500toInf

# #
# # W+JETS extensions
# #

# #
# # SINGLE TOP
# #
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_singletop_tW_antitop
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_singletop_tW_top
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/ST_t-channel_antitop_4f_inclusiveDecays_TuneCP5_13TeV-powhegV2-madspin-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_singletop_t_antitop
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/ST_t-channel_top_4f_inclusiveDecays_TuneCP5_13TeV-powhegV2-madspin-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_singletop_t_top
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-amcatnlo-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_singletop_s_amcatnlo

# #
# # DY+JETS
# #
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/DYJetsToLL_M-50_HT-100to200_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_dyjetsll_ht100to200
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/DYJetsToLL_M-50_HT-200to400_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_dyjetsll_ht200to400
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/DYJetsToLL_M-50_HT-400to600_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_dyjetsll_ht400to600
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/DYJetsToLL_M-50_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_dyjetsll_ht600to800
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/DYJetsToLL_M-50_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_dyjetsll_ht800to1200
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/DYJetsToLL_M-50_HT-1200to2500_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_dyjetsll_ht1200to2500
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/DYJetsToLL_M-50_HT-2500toInf_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_dyjetsll_ht2500toInf


#
# GAMMA + JETS
#
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/GJets_HT-40To100_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_1core_94X_mc2017_realistic_v14_ext1-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_gjets_dr0p4_ht40to100
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/GJets_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_gjets_dr0p4_ht100to200
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/GJets_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_gjets_dr0p4_ht200to400
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/GJets_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v2_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_gjets_dr0p4_ht400to600
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/GJets_HT-600ToInf_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_gjets_dr0p4_ht600toInf

# #
# # Z INVISIBLE
# #
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/ZJetsToNuNu_HT-100To200_13TeV-madgraph_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/   ${TAG}_zinv_ht100to200
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/ZJetsToNuNu_HT-200To400_13TeV-madgraph_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/   ${TAG}_zinv_ht200to400
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/ZJetsToNuNu_HT-400To600_13TeV-madgraph_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/                      ${TAG}_zinv_ht400to600
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/ZJetsToNuNu_HT-600To800_13TeV-madgraph_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/   ${TAG}_zinv_ht600to800
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/ZJetsToNuNu_HT-800To1200_13TeV-madgraph_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/  ${TAG}_zinv_ht800to1200
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/ZJetsToNuNu_HT-1200To2500_13TeV-madgraph_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_zinv_ht1200to2500
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/ZJetsToNuNu_HT-2500ToInf_13TeV-madgraph_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/  ${TAG}_zinv_ht2500toInf

# #
# # Z INVISIBLE EXTENSIONS
# #

# # #
# # # DIBOSON
# # #
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/WW_TuneCP5_13TeV-pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/  ${TAG}_ww

# # #
# # # TRIBOSON
# # #

# # #
# # # TTV, tt+X
# # #
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/TTWJetsToQQ_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_ttw_qq
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/TTZToQQ_TuneCP5_13TeV-amcatnlo-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_ttz_qq
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_ttw_lnu
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_ttz_llnunu
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017//TTGamma_SingleLeptFromT_TuneCP5_PSweights_13TeV_madgraph_pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_ttg_lfromt
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017//TTGamma_SingleLeptFromTbar_TuneCP5_PSweights_13TeV_madgraph_pythia8_RunIIFall17MiniAOD-PU2017_94X_mc2017_realistic_v11-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_ttg_lfromtbar

# #
# # QCD HT BINS
# #
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_ht300to500
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_old_pmx_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_ht500to700
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_HT700to1000_TuneCP5_13TeV-madgraph-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_ht700to1000
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_ht1000to1500
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_ht1500to2000
./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_ht2000toInf

# #
# # QCD HT BINS Extension
# #

#
# QCD PT bins
#

# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_15to30_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt15to30
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_30to50_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt30to50
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_50to80_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt50to80
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_80to120_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt80to120
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_120to170_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt120to170
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_170to300_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt170to300
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_300to470_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt300to470
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_470to600_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt470to600
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_600to800_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt600to800
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_800to1000_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v2_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt800to1000
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_1000to1400_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt1000to1400
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_1400to1800_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt1400to1800
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_1800to2400_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt1800to2400
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_2400to3200_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt2400to3200
# ./writeConfig.sh /hadoop/cms/store/group/snt/run2_mc2017/QCD_Pt_3200toInf_TuneCP5_13TeV_pythia8_RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1_MINIAODSIM_CMS4_V09-04-17/ ${TAG}_qcd_pt3200toInf

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
