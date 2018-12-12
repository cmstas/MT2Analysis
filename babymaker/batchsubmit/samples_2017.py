import sys
sys.path.append("ProjectMetis/scripts")
import dis_client

# format is samples_2017[type][ds_name] = [xsec, kfactor, filt_eff]

samples_2017 = {
    "data" : {
        "/JetHT/Run2017B-31Mar2018-v1/MINIAOD"          : [None, None, None, "data_Run2017B_JetHT_31Mar2018"],
        "/MET/Run2017B-31Mar2018-v1/MINIAOD"            : [None, None, None, "data_Run2017B_MET_31Mar2018"],
        "/HTMHT/Run2017B-31Mar2018-v1/MINIAOD"          : [None, None, None, "data_Run2017B_HTMHT_31Mar2018"],
        "/SingleMuon/Run2017B-31Mar2018-v1/MINIAOD"     : [None, None, None, "data_Run2017B_SingleMuon_31Mar2018"],
        "/SingleElectron/Run2017B-31Mar2018-v1/MINIAOD" : [None, None, None, "data_Run2017B_SingleElectron_31Mar2018"],
        "/SinglePhoton/Run2017B-31Mar2018-v1/MINIAOD"   : [None, None, None, "data_Run2017B_SinglePhoton_31Mar2018"],
        "/DoubleMuon/Run2017B-31Mar2018-v1/MINIAOD"     : [None, None, None, "data_Run2017B_DoubleMuon_31Mar2018"],
        "/DoubleEG/Run2017B-31Mar2018-v1/MINIAOD"       : [None, None, None, "data_Run2017B_DoubleEG_31Mar2018"],
        "/MuonEG/Run2017B-31Mar2018-v1/MINIAOD"         : [None, None, None, "data_Run2017B_MuonEG_31Mar2018"],

        "/JetHT/Run2017C-31Mar2018-v1/MINIAOD"          : [None, None, None, "data_Run2017C_JetHT_31Mar2018"],
        "/MET/Run2017C-31Mar2018-v1/MINIAOD"            : [None, None, None, "data_Run2017C_MET_31Mar2018"],
        "/HTMHT/Run2017C-31Mar2018-v1/MINIAOD"          : [None, None, None, "data_Run2017C_HTMHT_31Mar2018"],
        "/SingleMuon/Run2017C-31Mar2018-v1/MINIAOD"     : [None, None, None, "data_Run2017C_SingleMuon_31Mar2018"],
        "/SingleElectron/Run2017C-31Mar2018-v1/MINIAOD" : [None, None, None, "data_Run2017C_SingleElectron_31Mar2018"],
        "/SinglePhoton/Run2017C-31Mar2018-v1/MINIAOD"   : [None, None, None, "data_Run2017C_SinglePhoton_31Mar2018"],
        "/DoubleMuon/Run2017C-31Mar2018-v1/MINIAOD"     : [None, None, None, "data_Run2017C_DoubleMuon_31Mar2018"],
        "/DoubleEG/Run2017C-31Mar2018-v1/MINIAOD"       : [None, None, None, "data_Run2017C_DoubleEG_31Mar2018"],
        "/MuonEG/Run2017C-31Mar2018-v1/MINIAOD"         : [None, None, None, "data_Run2017C_MuonEG_31Mar2018"],

        "/JetHT/Run2017D-31Mar2018-v1/MINIAOD"          : [None, None, None, "data_Run2017D_JetHT_31Mar2018"],
        "/MET/Run2017D-31Mar2018-v1/MINIAOD"            : [None, None, None, "data_Run2017D_MET_31Mar2018"],
        "/HTMHT/Run2017D-31Mar2018-v1/MINIAOD"          : [None, None, None, "data_Run2017D_HTMHT_31Mar2018"],
        "/SingleMuon/Run2017D-31Mar2018-v1/MINIAOD"     : [None, None, None, "data_Run2017D_SingleMuon_31Mar2018"],
        "/SingleElectron/Run2017D-31Mar2018-v1/MINIAOD" : [None, None, None, "data_Run2017D_SingleElectron_31Mar2018"],
        "/SinglePhoton/Run2017D-31Mar2018-v1/MINIAOD"   : [None, None, None, "data_Run2017D_SinglePhoton_31Mar2018"],
        "/DoubleMuon/Run2017D-31Mar2018-v1/MINIAOD"     : [None, None, None, "data_Run2017D_DoubleMuon_31Mar2018"],
        "/DoubleEG/Run2017D-31Mar2018-v1/MINIAOD"       : [None, None, None, "data_Run2017D_DoubleEG_31Mar2018"],
        "/MuonEG/Run2017D-31Mar2018-v1/MINIAOD"         : [None, None, None, "data_Run2017D_MuonEG_31Mar2018"],

        "/JetHT/Run2017E-31Mar2018-v1/MINIAOD"          : [None, None, None, "data_Run2017E_JetHT_31Mar2018"],
        "/MET/Run2017E-31Mar2018-v1/MINIAOD"            : [None, None, None, "data_Run2017E_MET_31Mar2018"],
        "/HTMHT/Run2017E-31Mar2018-v1/MINIAOD"          : [None, None, None, "data_Run2017E_HTMHT_31Mar2018"],
        "/SingleMuon/Run2017E-31Mar2018-v1/MINIAOD"     : [None, None, None, "data_Run2017E_SingleMuon_31Mar2018"],
        "/SingleElectron/Run2017E-31Mar2018-v1/MINIAOD" : [None, None, None, "data_Run2017E_SingleElectron_31Mar2018"],
        "/SinglePhoton/Run2017E-31Mar2018-v1/MINIAOD"   : [None, None, None, "data_Run2017E_SinglePhoton_31Mar2018"],
        "/DoubleMuon/Run2017E-31Mar2018-v1/MINIAOD"     : [None, None, None, "data_Run2017E_DoubleMuon_31Mar2018"],
        "/DoubleEG/Run2017E-31Mar2018-v1/MINIAOD"       : [None, None, None, "data_Run2017E_DoubleEG_31Mar2018"],
        "/MuonEG/Run2017E-31Mar2018-v1/MINIAOD"         : [None, None, None, "data_Run2017E_MuonEG_31Mar2018"],

        "/JetHT/Run2017F-31Mar2018-v1/MINIAOD"          : [None, None, None, "data_Run2017F_JetHT_31Mar2018"],
        "/MET/Run2017F-31Mar2018-v1/MINIAOD"            : [None, None, None, "data_Run2017F_MET_31Mar2018"],
        "/HTMHT/Run2017F-31Mar2018-v1/MINIAOD"          : [None, None, None, "data_Run2017F_HTMHT_31Mar2018"],
        "/SingleMuon/Run2017F-31Mar2018-v1/MINIAOD"     : [None, None, None, "data_Run2017F_SingleMuon_31Mar2018"],
        "/SingleElectron/Run2017F-31Mar2018-v1/MINIAOD" : [None, None, None, "data_Run2017F_SingleElectron_31Mar2018"],
        "/SinglePhoton/Run2017F-31Mar2018-v1/MINIAOD"   : [None, None, None, "data_Run2017F_SinglePhoton_31Mar2018"],
        "/DoubleMuon/Run2017F-31Mar2018-v1/MINIAOD"     : [None, None, None, "data_Run2017F_DoubleMuon_31Mar2018"],
        "/DoubleEG/Run2017F-31Mar2018-v1/MINIAOD"       : [None, None, None, "data_Run2017F_DoubleEG_31Mar2018"],
        "/MuonEG/Run2017F-31Mar2018-v1/MINIAOD"         : [None, None, None, "data_Run2017F_MuonEG_31Mar2018"],
        },
    "ttbar" : {
        "/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM"         : [365.34, 1.0, 1.0, "ttsl_powheg", "DELETE"],
        "/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [365.34, 1.0, 1.0, "ttsl_powheg", "TOADD"],
        "/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"                : [88.29,  1.0, 1.0, "ttdl_powheg", "DELETE"],
        "/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM"        : [88.29,  1.0, 1.0, "ttdl_powheg", "TOADD"], 

        # madgraph samples, consistent with 2016
        "/TTJets_DiLept_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"             : [54.23, 1.5225, 1.0, "ttdl_mg", "TOADD"],
        "/TTJets_SingleLeptFromT_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"    : [109.6, 1.594,  1.0, "ttsl_top_mg", "TOADD"],
        "/TTJets_SingleLeptFromTbar_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [109.6, 1.594,  1.0, "ttsl_tbar_mg", "TOADD"],
        },
    "wjets" : {
         "/WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM"   : [1379,   1.21, 1.0, "wjets_ht100to200"],
         "/WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [359.7,  1.21, 1.0, "wjets_ht200to400"],
         "/WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [48.91,  1.21, 1.0, "wjets_ht400to600"],
         "/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [12.05,  1.21, 1.0, "wjets_ht600to800"],
         "/WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"  : [5.501,  1.21, 1.0, "wjets_ht800to1200"],
         "/WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [1.329,  1.21, 1.0, "wjets_ht1200to2500"],
         "/WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v3/MINIAODSIM"  : [.03216, 1.21, 1.0, "wjets_ht2500toInf"],
        },
    "zinv" : {
        "/ZJetsToNuNu_HT-400To600_13TeV-madgraph/RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1/MINIAODSIM" : [10.73, 1.23, 1.0, "zinv_ht400to600", "DELETE"],

        "/ZJetsToNuNu_HT-100To200_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [280.35,  1.23, 1.0, "zinv_ht100to200"],
        "/ZJetsToNuNu_HT-200To400_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [77.67,   1.23, 1.0, "zinv_ht200to400"],
        "/ZJetsToNuNu_HT-600To800_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [3.221,   1.0,  1.0, "zinv_ht600to800", "DELETE"],
        "/ZJetsToNuNu_HT-800To1200_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"  : [1.474,   1.0,  1.0, "zinv_ht800to1200"],
        "/ZJetsToNuNu_HT-1200To2500_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [.3586,   1.0,  1.0, "zinv_ht1200to2500", "DELETE"],
        "/ZJetsToNuNu_HT-2500ToInf_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"  : [.008203, 1.0,  1.0, "zinv_ht2500toInf"],

        "/ZJetsToNuNu_HT-400To600_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [10.73, 1.23, 1.0, "zinv_ht400to600", "TOADD"],
        "/ZJetsToNuNu_HT-600To800_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [3.221, 1.0,  1.0, "zinv_ht600to800", "TOADD"],
        "/ZJetsToNuNu_HT-1200To2500_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [.3586, 1.0,  1.0, "zinv_ht1200to2500", "TOADD"],
        },
    "dyjetsll" : {
        "/DYJetsToLL_M-50_HT-100to200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [164.4,   1.0, 1.0, "dyjetsll_ht100to200", "DELETE"],
        "/DYJetsToLL_M-50_HT-200to400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [49.77,   1.0, 1.0, "dyjetsll_ht200to400", "DELETE"],
        "/DYJetsToLL_M-50_HT-200to400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM"   : [49.77,   1.0, 1.0, "dyjetsll_ht200to400", "TOADD"],
        "/DYJetsToLL_M-50_HT-400to600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [6.805,   1.0, 1.0, "dyjetsll_ht400to600", "DELETE"],
        "/DYJetsToLL_M-50_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [1.778,   1.0, 1.0, "dyjetsll_ht600to800", "DELETE"],
        "/DYJetsToLL_M-50_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"  : [.8526,   1.0, 1.0, "dyjetsll_ht800to1200", "DELETE"],
        "/DYJetsToLL_M-50_HT-1200to2500_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [.1862,   1.0, 1.0, "dyjetsll_ht1200to2500"],
        "/DYJetsToLL_M-50_HT-2500toInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"  : [.003652, 1.0, 1.0, "dyjetsll_ht2500toInf", "DELETE"],

        "/DYJetsToLL_M-50_HT-100to200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v2/MINIAODSIM"  : [164.4,   1.0, 1.0, "dyjetsll_ht100to200", "TOADD"],
        "/DYJetsToLL_M-50_HT-400to600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v2/MINIAODSIM"  : [6.805,   1.0, 1.0, "dyjetsll_ht400to600", "TOADD"],
        "/DYJetsToLL_M-50_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v2/MINIAODSIM"  : [1.778,   1.0, 1.0, "dyjetsll_ht600to800", "TOADD"],
        "/DYJetsToLL_M-50_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v2/MINIAODSIM" : [.8526,   1.0, 1.0, "dyjetsll_ht800to1200", "TOADD"],
        "/DYJetsToLL_M-50_HT-2500toInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v2/MINIAODSIM" : [.003652, 1.0, 1.0, "dyjetsll_ht2500toInf", "TOADD"],
        },
    "singletop" : {
        "/ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"                    : [3.74,   1.0, 1.0,  "singletop_schan", "DELETE"],
        "/ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"                : [34.91,  1.0, 0.58, "singletop_tW_top", "DELETE"],
        "/ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"            : [34.91,  1.0, 0.53, "singletop_tW_tbar", "DELETE"],
        "/ST_t-channel_top_4f_inclusiveDecays_TuneCP5_13TeV-powhegV2-madspin-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"     : [136.02, 1.0, 1.0,  "singletop_tchan_top", "DELETE"],
        "/ST_t-channel_antitop_4f_inclusiveDecays_TuneCP5_13TeV-powhegV2-madspin-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [80.95,  1.0, 1.0,  "singletop_tchan_tbar", "DELETE"],

        "/ST_s-channel_4f_leptonDecays_TuneCP5_PSweights_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM"      : [3.74,   1.0, 1.0,  "singletop_schan", "TOADD"],
        "/ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM"            : [34.91,  1.0, 0.58, "singletop_tW_top", "TOADD"],
        "/ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM"        : [34.91,  1.0, 0.53, "singletop_tW_tbar", "TOADD"],
        "/ST_t-channel_top_4f_inclusiveDecays_TuneCP5_13TeV-powhegV2-madspin-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [136.02, 1.0, 1.0,  "singletop_tchan_top", "TOADD"],
        "/ST_t-channel_antitop_4f_inclusiveDecays_TuneCP5_13TeV-powhegV2-madspin-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM"     : [80.95,  1.0, 1.0,  "singletop_tchan_tbar", "TOADD"],
        },
    "gjets" : {
        "/GJets_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v2/MINIAODSIM" : [274.4, 1.0, 1.0, "gjets_dr0p05_ht400to600", "DELETE"],

        "/GJets_HT-40To100_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_1core_94X_mc2017_realistic_v14_ext1-v1/MINIAODSIM" : [20790, 1.0, 1.0, "gjets_dr0p05_ht40to100", "DELETE"],
        "/GJets_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"           : [9238,  1.0, 1.0, "gjets_dr0p05_ht100to200"],
        "/GJets_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"           : [2305,  1.0, 1.0, "gjets_dr0p05_ht200to400", "DELETE"],
        "/GJets_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"           : [274.4, 1.0, 1.0, "gjets_dr0p05_ht400to600", "TOADD"],
        "/GJets_HT-600ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"           : [93.46, 1.0, 1.0, "gjets_dr0p05_ht600toInf"],

        "/GJets_HT-40To100_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14_ext1-v2/MINIAODSIM" : [20790, 1.0, 1.0, "gjets_dr0p05_ht40to100", "TOADD"],
        "/GJets_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v2/MINIAODSIM"     : [2305,  1.0, 1.0, "gjets_dr0p05_ht200to400", "TOADD"],
        },
    "ttv" : {
        "/ttWJets_TuneCP5_13TeV_madgraphMLM_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [0.6105, 1.0, 1.0, "ttw_mg"],
        "/ttZJets_TuneCP5_13TeV_madgraphMLM_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [0.7826, 1.0, 1.0, "ttz_mg", "TOADD"],
        "/TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [3.697, 1.0, 1.0, "ttg_amcatnlo", "TOADD"]
        },
    "diboson" : {
        "/WW_TuneCP5_13TeV-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [63.21, 1.878, 1.0, "ww"],
        "/WZ_TuneCP5_13TeV-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [22.82, 1.0, 1.0, "wz"],
        },
    "qcd_ht" : {
        "/QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"         : [311900, 1.0, 1.0, "qcd_ht300to500"],
        "/QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_old_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [29070,  1.0, 1.0, "qcd_ht500to700", "DELETE"],
        "/QCD_HT700to1000_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"        : [5962,   1.0, 1.0, "qcd_ht700to1000"],
        "/QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"       : [1005,   1.0, 1.0, "qcd_ht1000to1500", "DELETE"],
        "/QCD_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM"       : [101.8,  1.0, 1.0, "qcd_ht1500to2000"],
        "/QCD_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM"        : [20.54,  1.0, 1.0, "qcd_ht2000toInf"],

        "/QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM"           : [29070, 1.0, 1.0, "qcd_ht500to700", "TOADD"],
        "/QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [1005,  1.0, 1.0, "qcd_ht1000to1500", "TOADD"],
        },
    

    }

if __name__ == "__main__":

    todelete = []
    toadd = []
    badpu = []
    for type in samples_2017:
        for ds in samples_2017[type]:
            vals = samples_2017[type][ds]
            if len(vals) > 4 and vals[4]=="DELETE":
                todelete.append(ds)
            if len(vals) > 4 and vals[4]=="TOADD":
                toadd.append(ds)
                
            if "Run2017" not in ds and (len(vals) <= 4 or vals[4] != "DELETE"):
                out = dis_client.query(ds, "parents")["response"]["payload"]["parents"]
                if len(out) < 1 or "PU2017" not in out[0]:
                    badpu.append(ds)

    print "Datasets to replace:"
    for ds in todelete:
        print ds

    print "\nDatasets to add:"
    for ds in toadd:
        print ds

    print "\nDatasets with old PU and no replacement:"
    for ds in badpu:
        print ds




