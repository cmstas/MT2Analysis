#!/bin/bash

make -j 8 || return $?

# ./processBaby test /hadoop/cms/store/group/snt/run2_data2016_94x/JetHT_Run2016E-17Jul2018-v1_MINIAOD_CMS4_V09-04-17/merged_ntuple_42.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2017/JetHT_Run2017E-31Mar2018-v1_MINIAOD_CMS4_V09-04-19/merged_ntuple_42.root 100000
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2018_prompt/JetHT_Run2018B-PromptReco-v1_MINIAOD_CMS4_V10-01-00/merged_ntuple_42.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_mc2017/WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v3_MINIAODSIM_CMS4_V09-04-17/merged_ntuple_42.root 1000

# ./processBaby test /hadoop/cms/store/group/snt/run2_mc2016_cms4/TTJets_SingleLeptFromT_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1_MINIAODSIM_CMS4_V00-00-03_2017Sep27/merged_ntuple_3.root 1000
# ./processBaby test /hadoop/cms/store/group/snt/run2_data2018_prompt/MuonEG_Run2018D-PromptReco-v2_MINIAOD_CMS4_V10-01-00/merged_ntuple_87.root 1000
./processBaby test /hadoop/cms/store/group/snt/run2_mc2017//TTJets_SingleLeptFromT_TuneCP5_13TeV-madgraphMLM-pythia8_RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1_MINIAODSIM_CMS4_V09-04-19/merged_ntuple_42.root 1000