#!/bin/bash

make -j 8 || return $?

# ./processBaby test /hadoop/cms/store/group/snt/run2_data2016_94x/JetHT_Run2016B-17Jul2018_ver1-v1_MINIAOD_CMS4_V09-04-17/merged_ntuple_1.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2016_94x/JetHT_Run2016C-17Jul2018-v1_MINIAOD_CMS4_V10-02-04/merged_ntuple_42.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2016_94x/JetHT_Run2016D-17Jul2018-v1_MINIAOD_CMS4_V10-02-04/merged_ntuple_42.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2016_94x/JetHT_Run2016E-17Jul2018-v1_MINIAOD_CMS4_V10-02-04/merged_ntuple_42.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2016_94x/JetHT_Run2016F-17Jul2018-v1_MINIAOD_CMS4_V10-02-04/merged_ntuple_42.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2016_94x/JetHT_Run2016G-17Jul2018-v1_MINIAOD_CMS4_V10-02-04/merged_ntuple_42.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2016_94x/JetHT_Run2016H-17Jul2018-v1_MINIAOD_CMS4_V10-02-04/merged_ntuple_42.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2017/JetHT_Run2017E-31Mar2018-v1_MINIAOD_CMS4_V10-02-05/merged_ntuple_42.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2018/JetHT_Run2018D-PromptReco-v2_MINIAOD_CMS4_V10-02-04/merged_ntuple_42.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_mc2016_94x/TTJets_SingleLeptFromTbar_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2_MINIAODSIM_CMS4_V09-04-17/merged_ntuple_42.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_mc2017/TTJets_SingleLeptFromT_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V10-02-05/merged_ntuple_42.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_mc2018//DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8_RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2_MINIAODSIM_CMS4_V10-02-04/merged_ntuple_1.root 1000
# ./processBaby test /hadoop/cms/store/user/bemarsh/ProjectMetis/JetHT_Run2018B-26Sep2018-v1_MINIAOD_CMS4_V10-02-01/merged_ntuple_11.root 1000
# ./processBaby test /hadoop/cms/store/user/namin/run2_moriond17_cms4/ProjectMetis/SMS-T1tttt_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISpring16MiniAODv2-PUSpring16Fast_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1_MINIAODSIM_CMS4_V00-00-02_2017Sep27/merged_ntuple_3.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_mc2017/SMS-T1tttt_TuneCP2_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PUFall17Fast_pilot_94X_mc2017_realistic_v15-v3_MINIAODSIM_CMS4_V10-02-05/merged_ntuple_42.root 1000
./processBaby test /hadoop/cms/store/group/snt/run2_mc2016_94x/SMS-T1tttt_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISummer16MiniAODv3-PUSummer16v3Fast_94X_mcRun2_asymptotic_v3-v1_MINIAODSIM_CMS4_V10-02-05/merged_ntuple_1.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_mc2018/ZJetsToNuNu_HT-400To600_13TeV-madgraph_RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1_MINIAODSIM_CMS4_V10-02-04/merged_ntuple_20.root -1

# ./processBaby test /hadoop/cms/store/group/snt/run2_mc2017/ST_t-channel_antitop_4f_inclusiveDecays_TuneCP5_13TeV-powhegV2-madspin-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2_MINIAODSIM_CMS4_V09-04-19/merged_ntuple_12.root 1000

# ./processBaby test /hadoop/cms/store/group/snt/run2_mc2016_94x/TTJets_SingleLeptFromT_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2_MINIAODSIM_CMS4_V09-04-17/merged_ntuple_42.root 10000
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2018_prompt/MuonEG_Run2018D-PromptReco-v2_MINIAOD_CMS4_V10-01-00/merged_ntuple_87.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_mc2017//TTJets_SingleLeptFromT_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-19/merged_ntuple_42.root 10000
# ./processBaby test /home/users/bemarsh/NtupleMaker/CMSSW_9_4_9/src/CMS3/NtupleMaker/test/TTJets_SingleLeptFromT_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-19_ntuple.root 10000

# ./processBaby test /hadoop/cms/store/group/snt/run2_mc2017/QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-19/merged_ntuple_42.root 100 1
# ./processBaby test /hadoop/cms/store/group/snt/run2_mc2016_94x/QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2_MINIAODSIM_CMS4_V09-04-17/merged_ntuple_42.root 100 1
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2016_94x/JetHT_Run2016D-17Jul2018-v1_MINIAOD_CMS4_V09-04-17/merged_ntuple_42.root 100 1
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2017/JetHT_Run2017D-31Mar2018-v1_MINIAOD_CMS4_V09-04-19/merged_ntuple_42.root 100 1
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2018/JetHT_Run2018B-17Sep2018-v1_MINIAOD_CMS4_V10-02-01/merged_ntuple_42.root 100 1