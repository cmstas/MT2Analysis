import sys
sys.path.append("ProjectMetis/scripts")
import dis_client

# format is samples_2016[type][ds_name] = [xsec, kfactor, filt_eff]

samples_2016 = {
    "data" : { 
        "/JetHT/Run2016B-17Jul2018_ver1-v1/MINIAOD" : [None, None, None, "data_Run2016B_JetHT_17Jul2018_ver1-v1"],
        "/JetHT/Run2016B-17Jul2018_ver2-v2/MINIAOD" : [None, None, None, "data_Run2016B_JetHT_17Jul2018_ver2-v2"],
        "/JetHT/Run2016C-17Jul2018-v1/MINIAOD" :     [None, None, None, "data_Run2016C_JetHT_17Jul2018-v1"],
        "/JetHT/Run2016D-17Jul2018-v1/MINIAOD" :     [None, None, None, "data_Run2016D_JetHT_17Jul2018-v1"],
        "/JetHT/Run2016E-17Jul2018-v1/MINIAOD" :     [None, None, None, "data_Run2016E_JetHT_17Jul2018-v1"],
        "/JetHT/Run2016F-17Jul2018-v1/MINIAOD" :     [None, None, None, "data_Run2016F_JetHT_17Jul2018-v1"],
        "/JetHT/Run2016G-17Jul2018-v1/MINIAOD" :     [None, None, None, "data_Run2016G_JetHT_17Jul2018-v1"],
        "/JetHT/Run2016H-17Jul2018-v1/MINIAOD" :     [None, None, None, "data_Run2016H_JetHT_17Jul2018-v1"],
        
        "/MET/Run2016B-17Jul2018_ver1-v1/MINIAOD" : [None, None, None, "data_Run2016B_MET_17Jul2018_ver1-v1"],
        "/MET/Run2016B-17Jul2018_ver2-v1/MINIAOD" : [None, None, None, "data_Run2016B_MET_17Jul2018_ver2-v1"],
        "/MET/Run2016C-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016C_MET_17Jul2018-v1"],
        "/MET/Run2016D-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016D_MET_17Jul2018-v1"],
        "/MET/Run2016E-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016E_MET_17Jul2018-v1"],
        "/MET/Run2016F-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016F_MET_17Jul2018-v1"],
        "/MET/Run2016G-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016G_MET_17Jul2018-v1"],
        "/MET/Run2016H-17Jul2018-v2/MINIAOD" :      [None, None, None, "data_Run2016H_MET_17Jul2018-v2"],

        "/HTMHT/Run2016B-17Jul2018_ver1-v1/MINIAOD" : [None, None, None, "data_Run2016B_HTMHT_17Jul2018_ver1-v1"],
        "/HTMHT/Run2016B-17Jul2018_ver2-v1/MINIAOD" : [None, None, None, "data_Run2016B_HTMHT_17Jul2018_ver2-v1"],
        "/HTMHT/Run2016C-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016C_HTMHT_17Jul2018-v1"],
        "/HTMHT/Run2016D-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016D_HTMHT_17Jul2018-v1"],
        "/HTMHT/Run2016E-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016E_HTMHT_17Jul2018-v1"],
        "/HTMHT/Run2016F-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016F_HTMHT_17Jul2018-v1"],
        "/HTMHT/Run2016G-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016G_HTMHT_17Jul2018-v1"],
        "/HTMHT/Run2016H-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016H_HTMHT_17Jul2018-v1"],

        "/SingleMuon/Run2016B-17Jul2018_ver1-v1/MINIAOD" : [None, None, None, "data_Run2016B_SingleMuon_17Jul2018_ver1-v1"],
        "/SingleMuon/Run2016B-17Jul2018_ver2-v1/MINIAOD" : [None, None, None, "data_Run2016B_SingleMuon_17Jul2018_ver2-v1"],
        "/SingleMuon/Run2016C-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016C_SingleMuon_17Jul2018-v1"],
        "/SingleMuon/Run2016D-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016D_SingleMuon_17Jul2018-v1"],
        "/SingleMuon/Run2016E-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016E_SingleMuon_17Jul2018-v1"],
        "/SingleMuon/Run2016F-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016F_SingleMuon_17Jul2018-v1"],
        "/SingleMuon/Run2016G-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016G_SingleMuon_17Jul2018-v1"],
        "/SingleMuon/Run2016H-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016H_SingleMuon_17Jul2018-v1"],
        
        "/SingleElectron/Run2016B-17Jul2018_ver1-v1/MINIAOD" : [None, None, None, "data_Run2016B_SingleElectron_17Jul2018_ver1-v1"],
        "/SingleElectron/Run2016B-17Jul2018_ver2-v1/MINIAOD" : [None, None, None, "data_Run2016B_SingleElectron_17Jul2018_ver2-v1"],
        "/SingleElectron/Run2016C-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016C_SingleElectron_17Jul2018-v1"],
        "/SingleElectron/Run2016D-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016D_SingleElectron_17Jul2018-v1"],
        "/SingleElectron/Run2016E-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016E_SingleElectron_17Jul2018-v1"],
        "/SingleElectron/Run2016F-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016F_SingleElectron_17Jul2018-v1"],
        "/SingleElectron/Run2016G-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016G_SingleElectron_17Jul2018-v1"],
        "/SingleElectron/Run2016H-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016H_SingleElectron_17Jul2018-v1"],
        
        "/SinglePhoton/Run2016B-17Jul2018_ver1-v1/MINIAOD" : [None, None, None, "data_Run2016B_SinglePhoton_17Jul2018_ver1-v1"],
        "/SinglePhoton/Run2016B-17Jul2018_ver2-v1/MINIAOD" : [None, None, None, "data_Run2016B_SinglePhoton_17Jul2018_ver2-v1"],
        "/SinglePhoton/Run2016C-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016C_SinglePhoton_17Jul2018-v1"],
        "/SinglePhoton/Run2016D-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016D_SinglePhoton_17Jul2018-v1"],
        "/SinglePhoton/Run2016E-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016E_SinglePhoton_17Jul2018-v1"],
        "/SinglePhoton/Run2016F-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016F_SinglePhoton_17Jul2018-v1"],
        "/SinglePhoton/Run2016G-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016G_SinglePhoton_17Jul2018-v1"],
        "/SinglePhoton/Run2016H-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016H_SinglePhoton_17Jul2018-v1"],
        
        "/DoubleMuon/Run2016B-17Jul2018_ver1-v1/MINIAOD" : [None, None, None, "data_Run2016B_DoubleMuon_17Jul2018_ver1-v1"],
        "/DoubleMuon/Run2016B-17Jul2018_ver2-v1/MINIAOD" : [None, None, None, "data_Run2016B_DoubleMuon_17Jul2018_ver2-v1"],
        "/DoubleMuon/Run2016C-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016C_DoubleMuon_17Jul2018-v1"],
        "/DoubleMuon/Run2016D-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016D_DoubleMuon_17Jul2018-v1"],
        "/DoubleMuon/Run2016E-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016E_DoubleMuon_17Jul2018-v1"],
        "/DoubleMuon/Run2016F-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016F_DoubleMuon_17Jul2018-v1"],
        "/DoubleMuon/Run2016G-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016G_DoubleMuon_17Jul2018-v1"],
        "/DoubleMuon/Run2016H-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016H_DoubleMuon_17Jul2018-v1"],
        
        "/DoubleEG/Run2016B-17Jul2018_ver1-v1/MINIAOD" : [None, None, None, "data_Run2016B_DoubleEG_17Jul2018_ver1-v1"],
        "/DoubleEG/Run2016B-17Jul2018_ver2-v1/MINIAOD" : [None, None, None, "data_Run2016B_DoubleEG_17Jul2018_ver2-v1"],
        "/DoubleEG/Run2016C-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016C_DoubleEG_17Jul2018-v1"],
        "/DoubleEG/Run2016D-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016D_DoubleEG_17Jul2018-v1"],
        "/DoubleEG/Run2016E-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016E_DoubleEG_17Jul2018-v1"],
        "/DoubleEG/Run2016F-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016F_DoubleEG_17Jul2018-v1"],
        "/DoubleEG/Run2016G-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016G_DoubleEG_17Jul2018-v1"],
        "/DoubleEG/Run2016H-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016H_DoubleEG_17Jul2018-v1"],
        
        "/MuonEG/Run2016B-17Jul2018_ver1-v1/MINIAOD" : [None, None, None, "data_Run2016B_MuonEG_17Jul2018_ver1-v1"],
        "/MuonEG/Run2016B-17Jul2018_ver2-v1/MINIAOD" : [None, None, None, "data_Run2016B_MuonEG_17Jul2018_ver2-v1"],
        "/MuonEG/Run2016C-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016C_MuonEG_17Jul2018-v1"],
        "/MuonEG/Run2016D-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016D_MuonEG_17Jul2018-v1"],
        "/MuonEG/Run2016E-17Jul2018-v2/MINIAOD" :      [None, None, None, "data_Run2016E_MuonEG_17Jul2018-v2"],
        "/MuonEG/Run2016F-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016F_MuonEG_17Jul2018-v1"],
        "/MuonEG/Run2016G-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016G_MuonEG_17Jul2018-v1"],
        "/MuonEG/Run2016H-17Jul2018-v1/MINIAOD" :      [None, None, None, "data_Run2016H_MuonEG_17Jul2018-v1"],

        },
    "ttbar" : {
        "/TTJets_SingleLeptFromT_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"    : [114.6, 1.594,  1.0, "ttsl_top"],
        "/TTJets_SingleLeptFromTbar_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM" : [114.6, 1.594,  1.0, "ttsl_tbar"],
        "/TTJets_DiLept_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"             : [57.35, 1.5225, 1.0, "ttdl"],

        "/TTJets_SingleLeptFromT_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM"    : [114.6, 1.594,  1.0, "ttsl_top_ext1"],
        "/TTJets_SingleLeptFromTbar_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [114.6, 1.594,  1.0, "ttsl_tbar_ext1"],
        "/TTJets_DiLept_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM"             : [57.35, 1.5225, 1.0, "ttdl_ext1"],
        },
    "wjets" : {
        "/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"   : [1345.0,  1.21, 1.0, "wjets_ht100to200"],
        "/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"   : [359.7,   1.21, 1.0, "wjets_ht200to400"],
        "/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"   : [48.91,   1.21, 1.0, "wjets_ht400to600"],
        "/WJetsToLNu_HT-600To800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"   : [12.05,   1.21, 1.0, "wjets_ht600to800"],
        "/WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"  : [5.501,   1.21, 1.0, "wjets_ht800to1200"],
        "/WJetsToLNu_HT-1200To2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM" : [1.329,   1.21, 1.0, "wjets_ht1200to2500"],
        "/WJetsToLNu_HT-2500ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"  : [0.03216, 1.21, 1.0, "wjets_ht2500toInf"],

        "/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM"   : [359.7,   1.21, 1.0, "wjets_ht200to400_ext1"],
        "/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext2-v2/MINIAODSIM"   : [359.7,   1.21, 1.0, "wjets_ht200to400_ext2"],
        "/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM"   : [48.91,   1.21, 1.0, "wjets_ht400to600_ext1"],
        "/WJetsToLNu_HT-600To800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM"   : [12.05,   1.21, 1.0, "wjets_ht600to800_ext1"],
        "/WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM"  : [5.501,   1.21, 1.0, "wjets_ht800to1200_ext1"],
        "/WJetsToLNu_HT-1200To2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [1.329,   1.21, 1.0, "wjets_ht1200to2500_ext1"],
        "/WJetsToLNu_HT-2500ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM"  : [0.03216, 1.21, 1.0, "wjets_ht2500toInf_ext1"],
        },
    "zinv" : {
        "/ZJetsToNuNu_HT-100To200_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"   : [280.35, 1.23, 1.0, "zinv_ht100to200"],
        "/ZJetsToNuNu_HT-200To400_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"   : [77.67,  1.23, 1.0, "zinv_ht200to400"],
        "/ZJetsToNuNu_HT-400To600_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"   : [10.73,  1.23, 1.0, "zinv_ht400to600"],
        "/ZJetsToNuNu_HT-800To1200_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM"  : [1.474,  1.0,  1.0, "zinv_ht800to1200"],
        "/ZJetsToNuNu_HT-1200To2500_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM" : [.3586,  1.0,  1.0, "zinv_ht1200to2500"],

        "/ZJetsToNuNu_HT-100To200_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [280.35, 1.23, 1.0, "zinv_ht100to200_ext1"],
        "/ZJetsToNuNu_HT-200To400_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [77.67,  1.23, 1.0, "zinv_ht200to400_ext1"],
        "/ZJetsToNuNu_HT-400To600_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [10.73,  1.23, 1.0, "zinv_ht400to600_ext1"],
        },
    "dyjetsll" : {
        "/DYJetsToLL_M-50_HT-100to200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"   : [147.4,    1.23, 1.0, "dyjetsll_ht100to200"],
        "/DYJetsToLL_M-50_HT-200to400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"   : [40.99,    1.23, 1.0, "dyjetsll_ht200to400"],
        "/DYJetsToLL_M-50_HT-400to600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"   : [5.678,    1.23, 1.0, "dyjetsll_ht400to600"],
        "/DYJetsToLL_M-50_HT-600to800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"   : [1.367,    1.23, 1.0, "dyjetsll_ht600to800"],
        "/DYJetsToLL_M-50_HT-800to1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM"  : [0.6304,   1.23, 1.0, "dyjetsll_ht800to1200"],
        "/DYJetsToLL_M-50_HT-1200to2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM" : [0.1514,   1.23, 1.0, "dyjetsll_ht1200to2500"],
        "/DYJetsToLL_M-50_HT-2500toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM"  : [0.003565, 1.23, 1.0, "dyjetsll_ht2500toInf"],

        "/DYJetsToLL_M-50_HT-100to200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [147.4, 1.23, 1.0, "dyjetsll_ht100to200_ext1"],
        "/DYJetsToLL_M-50_HT-200to400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [40.99, 1.23, 1.0, "dyjetsll_ht200to400_ext1"],
        "/DYJetsToLL_M-50_HT-400to600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [5.678, 1.23, 1.0, "dyjetsll_ht400to600_ext1"],
        },
    "singletop" : {
        "/ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM"     : [136.02, 1.0,    1.0, "singletop_tchan_top"],
        "/ST_t-channel_antitop_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM" : [80.95,  1.0,    1.0, "singletop_tchan_tbar"],
        "/ST_tW_top_5f_NoFullyHadronicDecays_13TeV-powheg_TuneCUETP8M1/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM"                   : [38.09,  0.5135, 1.0, "singletop_tW_top"],
        "/ST_tW_antitop_5f_NoFullyHadronicDecays_13TeV-powheg_TuneCUETP8M1/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM"               : [38.09,  0.5135, 1.0, "singletop_tW_tbar"],
        },
    "gjets" : {
        "/GJets_HT-40To100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"  : [20790.0, 1.0, 1.0, "gjets_dr0p05_ht40to100"],
        "/GJets_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM" : [9238.0,  1.0, 1.0, "gjets_dr0p05_ht100to200"],
        "/GJets_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM" : [2305.0,  1.0, 1.0, "gjets_dr0p05_ht200to400"],
        "/GJets_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM" : [274.4,   1.0, 1.0, "gjets_dr0p05_ht400to600"],
        "/GJets_HT-600ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM" : [93.46,   1.0, 1.0, "gjets_dr0p05_ht600toInf"],

        "/GJets_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [2305.0, 1.0, 1.0, "gjets_dr0p05_ht200to400_ext1"],
        "/GJets_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [274.4,  1.0, 1.0, "gjets_dr0p05_ht400to600_ext1"],
        "/GJets_HT-600ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [93.46,  1.0, 1.0, "gjets_dr0p05_ht600toInf_ext1"],
        },
    "ttv" : {
        "/ttZJets_13TeV_madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM"                                 : [0.7826, 1.0, 1.0, "ttz_mg_lo"],
        "/TTWJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext2-v1/MINIAODSIM" : [0.2043, 1.0, 1.0, "ttw_lnu_amcatnlo"],
        "/TTWJetsToQQ_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"       : [0.4062, 1.0, 1.0, "ttw_qq_amcatnlo"],
        "/TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM"      : [3.697,  1.0, 1.0, "ttg_amcatnlo"],
        },
    "diboson" : {
        "/WW_TuneCUETP8M1_13TeV-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [63.21, 1.878, 1.0, "ww"],
        "/WZ_TuneCUETP8M1_13TeV-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [22.82, 1.0,   1.0, "wz"],
        },
    "qcd_ht" : {
        "/QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"   : [347700.0, 1.0, 1.0, "qcd_ht300to500"],
        "/QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"   : [32100.0,  1.0, 1.0, "qcd_ht500to700"],
        "/QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"  : [6831.0,   1.0, 1.0, "qcd_ht700to1000"],
        "/QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM" : [1207.0,   1.0, 1.0, "qcd_ht1000to1500"],
        "/QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM" : [119.9,    1.0, 1.0, "qcd_ht1500to2000"],
        "/QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM"  : [25.24,    1.0, 1.0, "qcd_ht2000toInf"],

        "/QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM"   : [347700.0, 1.0, 1.0, "qcd_ht300to500_ext1"],
        "/QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM"   : [32100.0,  1.0, 1.0, "qcd_ht500to700_ext1"],
        "/QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [1207.0,   1.0, 1.0, "qcd_ht1000to1500_ext1"],
        "/QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM" : [119.9,    1.0, 1.0, "qcd_ht1500to2000_ext1"],
        "/QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM"  : [25.24,    1.0, 1.0, "qcd_ht2000toInf_ext1"],
        },

    }

if __name__ == "__main__":

    lines = samples_2016.split("\n")
    for line in lines:
        if "MINIAODSIM" not in line:
            print line
            continue
        ds = line.split('"')[1]
        info = dis_client.query(ds, "snt")["response"]["payload"][0]
        line = line.replace("1.0, 1.0, 1.0", "{0}, {1}, {2}".format(info["xsec"], info["kfactor"], info["filter_eff"]))
        print line
