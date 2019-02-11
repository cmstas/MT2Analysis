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
        # "/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [365.34, 1.0, 1.0, "ttsl_powheg"],
        # "/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM"        : [88.29,  1.0, 1.0, "ttdl_powheg"], 

        # madgraph samples, consistent with 2016
        "/TTJets_DiLept_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"             : [54.29, 1.677,  1.0, "ttdl_mg"],
        "/TTJets_SingleLeptFromT_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"    : [109.1, 1.677,  1.0, "ttsl_top_mg"],
        "/TTJets_SingleLeptFromTbar_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [109.1, 1.677,  1.0, "ttsl_tbar_mg"],
        },
    "wjets" : {
         "/WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM"   : [1379,   1.21, 1.005, "wjets_ht100to200"],
         "/WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [359.7,  1.21, 1.136, "wjets_ht200to400"],
         "/WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [48.91,  1.21, 1.186, "wjets_ht400to600"],
         "/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [12.05,  1.21, 1.196, "wjets_ht600to800"],
         "/WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"  : [5.501,  1.21, 1.173, "wjets_ht800to1200"],
         "/WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [1.329,  1.21, 1.077, "wjets_ht1200to2500"],
         "/WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v3/MINIAODSIM"  : [.03216, 1.21, 1.044, "wjets_ht2500toInf"],

         "/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14_ext1-v2/MINIAODSIM" : [52940, 1.21, 1.0, "wjets_incl"]
        },
    "zinv" : {

        "/ZJetsToNuNu_HT-100To200_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [280.35,  1.23, 1.073, "zinv_ht100to200"],
        "/ZJetsToNuNu_HT-200To400_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [77.67,   1.23, 1.169, "zinv_ht200to400"],
        "/ZJetsToNuNu_HT-800To1200_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"  : [1.198,   1.23, 1.139, "zinv_ht800to1200"],
        "/ZJetsToNuNu_HT-2500ToInf_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"  : [.006669, 1.23, 0.985, "zinv_ht2500toInf"],

        "/ZJetsToNuNu_HT-400To600_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [10.73, 1.23, 1.200, "zinv_ht400to600"],
        "/ZJetsToNuNu_HT-600To800_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM"   : [2.619, 1.23, 1.212, "zinv_ht600to800"],
        "/ZJetsToNuNu_HT-1200To2500_13TeV-madgraph/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [.2915, 1.23, 1.032, "zinv_ht1200to2500"],
        },
    "dyjetsll" : {
        "/DYJetsToLL_M-50_HT-200to400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM"   : [42.91,   1.16, 1.133, "dyjetsll_ht200to400"],
        "/DYJetsToLL_M-50_HT-1200to2500_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [.1574,   1.16, 1.023, "dyjetsll_ht1200to2500"],

        "/DYJetsToLL_M-50_HT-100to200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v2/MINIAODSIM"  : [141.7,   1.16, 1.137, "dyjetsll_ht100to200"],
        "/DYJetsToLL_M-50_HT-400to600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v2/MINIAODSIM"  : [5.867,   1.16, 1.175, "dyjetsll_ht400to600"],
        "/DYJetsToLL_M-50_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v2/MINIAODSIM"  : [1.533,   1.16, 1.109, "dyjetsll_ht600to800"],
        "/DYJetsToLL_M-50_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v2/MINIAODSIM" : [.7350,   1.16, 0.994, "dyjetsll_ht800to1200"],
        "/DYJetsToLL_M-50_HT-2500toInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v2/MINIAODSIM" : [.003148, 1.16, 1.117, "dyjetsll_ht2500toInf"],

        "/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017RECOSIMstep_12Apr2018_94X_mc2017_realistic_v14_ext1-v1/MINIAODSIM" : [5343, 1.16, 1.0, "dyjetsll_m50_incl"],
        },
    "singletop" : {
        "/ST_s-channel_4f_leptonDecays_TuneCP5_PSweights_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM"      : [3.74,   1.0, 1.0,  "singletop_schan"],
        "/ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM"            : [34.91,  1.0, 0.58, "singletop_tW_top"],
        "/ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM"        : [34.91,  1.0, 0.53, "singletop_tW_tbar"],
        "/ST_t-channel_top_4f_inclusiveDecays_TuneCP5_13TeV-powhegV2-madspin-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [136.02, 1.0, 1.0,  "singletop_tchan_top"],
        "/ST_t-channel_antitop_4f_inclusiveDecays_TuneCP5_13TeV-powhegV2-madspin-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM"     : [80.95,  1.0, 1.0,  "singletop_tchan_tbar"],
        },
    "gjets" : {
        "/GJets_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"           : [9238,  1.0, 1.0, "gjets_dr0p05_ht100to200"],
        "/GJets_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"           : [274.4, 1.0, 1.0, "gjets_dr0p05_ht400to600"],
        "/GJets_HT-600ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"           : [93.46, 1.0, 1.0, "gjets_dr0p05_ht600toInf"],

        "/GJets_HT-40To100_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14_ext1-v2/MINIAODSIM" : [20790, 1.0, 1.0, "gjets_dr0p05_ht40to100"],
        "/GJets_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v2/MINIAODSIM"     : [2305,  1.0, 1.0, "gjets_dr0p05_ht200to400"],
        },
    "ttv" : {
        "/ttWJets_TuneCP5_13TeV_madgraphMLM_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [0.6105, 1.0, 1.0, "ttw_mg"],
        "/ttZJets_TuneCP5_13TeV_madgraphMLM_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [0.7826, 1.0, 1.0, "ttz_mg"],
        "/TTGJets_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [3.697, 1.0, 1.0, "ttg_amcatnlo"]
        },
    "tttt": {
        "/TTTT_TuneCP5_PSweights_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [0.008213, 1.0, 1.0, "tttt"],
        },
    "diboson" : {
        "/WW_TuneCP5_13TeV-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [75.80, 1.566, 1.0, "ww"],
        "/WZ_TuneCP5_13TeV-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [27.57, 1.0, 1.0, "wz"],
        },
    "qcd_ht" : {
        "/QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"         : [323300, 1.0, 1.0, "qcd_ht300to500"],
        "/QCD_HT700to1000_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM"        : [6330,   1.0, 1.0, "qcd_ht700to1000"],
        "/QCD_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM"       : [98.80,  1.0, 1.0, "qcd_ht1500to2000"],
        "/QCD_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM"        : [20.26,  1.0, 1.0, "qcd_ht2000toInf"],

        "/QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/MINIAODSIM"           : [30000, 1.0, 1.0, "qcd_ht500to700"],
        "/QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM" : [1098,   1.0, 1.0, "qcd_ht1000to1500"],
        },

    "signal" : {
        "/SMS-T1tttt_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_pilot_94X_mc2017_realistic_v15-v3/MINIAODSIM"          : [None, None, None, "T1tttt"],
        "/SMS-T1tttt_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_pilot_94X_mc2017_realistic_v15-v3/MINIAODSIM"          : [None, None, None, "T1tttt_ext1"],
        "/SMS-T1bbbb_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_94X_mc2017_realistic_v15-v1/MINIAODSIM"                : [None, None, None, "T1bbbb"],
        "/SMS-T1qqqq_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_94X_mc2017_realistic_v15-v1/MINIAODSIM"                : [None, None, None, "T1qqqq"],
        "/SMS-T1qqqq_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_94X_mc2017_realistic_v15_ext1-v1/MINIAODSIM"           : [None, None, None, "T1qqqq_ext1"],
        "/SMS-T2tt_mStop-150to250_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_94X_mc2017_realistic_v15-v1/MINIAODSIM"   : [None, None, None, "T2tt_mStop150to250"],
        "/SMS-T2tt_mStop-250to350_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_94X_mc2017_realistic_v15-v1/MINIAODSIM"   : [None, None, None, "T2tt_mStop250to350"],
        "/SMS-T2tt_mStop-350to400_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_94X_mc2017_realistic_v15-v1/MINIAODSIM"   : [None, None, None, "T2tt_mStop350to400"],
        "/SMS-T2tt_mStop-400to1200_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_94X_mc2017_realistic_v15-v1/MINIAODSIM"  : [None, None, None, "T2tt_mStop400to1200"],
        "/SMS-T2tt_mStop-1200to2000_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_94X_mc2017_realistic_v15-v1/MINIAODSIM" : [None, None, None, "T2tt_mStop1200to2000"],
        "/SMS-T2bb_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_94X_mc2017_realistic_v15-v1/MINIAODSIM"                                 : [None, None, None, "T2bb"],
        "/SMS-T2bb_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_94X_mc2017_realistic_v15_ext1-v1/MINIAODSIM"                            : [None, None, None, "T2bb_ext1"],
        "/SMS-T2bb_mSbot-1650to2600_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_pileupfromucsd_94X_mc2017_realistic_v15-v2/MINIAODSIM" : [None, None, None, "T2bb_mSbot1650to2600"],
        "/SMS-T2bb_mSbot-1650to2600_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_94X_mc2017_realistic_v15_ext1-v1/MINIAODSIM"           : [None, None, None, "T2bb_mSbot1650to2600_ext1"],
        "/SMS-T2qq_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_94X_mc2017_realistic_v15-v1/MINIAODSIM"                                 : [None, None, None, "T2qq"],
        "/SMS-T2qq_mSq-1850to2600_TuneCP2_13TeV-madgraphMLM-pythia8/RunIIFall17MiniAODv2-PUFall17Fast_94X_mc2017_realistic_v15-v1/MINIAODSIM"                  : [None, None, None, "T2qq_mSq_1850to2600"],
        }

    }

if __name__ == "__main__":

    # todelete = []
    # toadd = []
    # badpu = []
    # for type in samples_2017:
    #     for ds in samples_2017[type]:
    #         vals = samples_2017[type][ds]
    #         if len(vals) > 4 and vals[4]=="DELETE":
    #             todelete.append(ds)
    #         if len(vals) > 4 and vals[4]=="TOADD":
    #             toadd.append('"{0}|{1}|{2}|{3}",'.format(ds, *samples_2017[type][ds]))
                
    #         if "Run2017" not in ds and (len(vals) <= 4 or vals[4] != "DELETE"):
    #             out = dis_client.query(ds, "parents")["response"]["payload"]["parents"]
    #             if len(out) < 1 or "PU2017" not in out[0]:
    #                 badpu.append(ds)

    # print "Datasets to replace:"
    # for ds in todelete:
    #     print ds

    # print "\nDatasets to add:"
    # for ds in toadd:
    #     print ds

    # print "\nDatasets with old PU and no replacement:"
    # for ds in badpu:
    #     print ds


    for type in samples_2017:
        if type == "data":
            continue
        for ds in samples_2017[type]:
            vals = samples_2017[type][ds]

            print '"{0}|{1}|{2}|{3}",'.format(ds, *samples_2017[type][ds])



    
