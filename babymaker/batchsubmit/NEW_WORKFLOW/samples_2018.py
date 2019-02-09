import sys
sys.path.append("ProjectMetis/scripts")
import dis_client

# format is samples_2018[type][ds_name] = [xsec, kfactor, filt_eff]

samples_2018 = {
    "data" : {
        "/JetHT/Run2018A-17Sep2018-v1/MINIAOD"      : [None, None, None, "data_Run2018A_JetHT_17Sep2018"],
        "/MET/Run2018A-17Sep2018-v1/MINIAOD"        : [None, None, None, "data_Run2018A_MET_17Sep2018"],
        "/SingleMuon/Run2018A-17Sep2018-v2/MINIAOD" : [None, None, None, "data_Run2018A_SingleMuon_17Sep2018"],
        "/EGamma/Run2018A-17Sep2018-v2/MINIAOD"     : [None, None, None, "data_Run2018A_EGamma_17Sep2018"],
        "/DoubleMuon/Run2018A-17Sep2018-v2/MINIAOD" : [None, None, None, "data_Run2018A_DoubleMuon_17Sep2018"],
        "/MuonEG/Run2018A-17Sep2018-v1/MINIAOD"     : [None, None, None, "data_Run2018A_MuonEG_17Sep2018"],
        
        "/JetHT/Run2018B-17Sep2018-v1/MINIAOD"      : [None, None, None, "data_Run2018B_JetHT_17Sep2018"],
        "/MET/Run2018B-17Sep2018-v1/MINIAOD"        : [None, None, None, "data_Run2018B_MET_17Sep2018"],
        "/SingleMuon/Run2018B-17Sep2018-v1/MINIAOD" : [None, None, None, "data_Run2018B_SingleMuon_17Sep2018"],
        "/EGamma/Run2018B-17Sep2018-v1/MINIAOD"     : [None, None, None, "data_Run2018B_EGamma_17Sep2018"],
        "/DoubleMuon/Run2018B-17Sep2018-v1/MINIAOD" : [None, None, None, "data_Run2018B_DoubleMuon_17Sep2018"],
        "/MuonEG/Run2018B-17Sep2018-v1/MINIAOD"     : [None, None, None, "data_Run2018B_MuonEG_17Sep2018"],

        "/JetHT/Run2018C-17Sep2018-v1/MINIAOD"      : [None, None, None, "data_Run2018C_JetHT_17Sep2018"],
        "/MET/Run2018C-17Sep2018-v1/MINIAOD"        : [None, None, None, "data_Run2018C_MET_17Sep2018"],
        "/SingleMuon/Run2018C-17Sep2018-v1/MINIAOD" : [None, None, None, "data_Run2018C_SingleMuon_17Sep2018"],
        "/EGamma/Run2018C-17Sep2018-v1/MINIAOD"     : [None, None, None, "data_Run2018C_EGamma_17Sep2018"],
        "/DoubleMuon/Run2018C-17Sep2018-v1/MINIAOD" : [None, None, None, "data_Run2018C_DoubleMuon_17Sep2018"],
        "/MuonEG/Run2018C-17Sep2018-v1/MINIAOD"     : [None, None, None, "data_Run2018C_MuonEG_17Sep2018"],

        # "/JetHT/Run2018D-PromptReco-v1/MINIAOD"      : [None, None, None, "data_Run2018D_JetHT_PromptReco-v1"],
        # "/MET/Run2018D-PromptReco-v1/MINIAOD"        : [None, None, None, "data_Run2018D_MET_PromptReco-v1"],
        # "/SingleMuon/Run2018D-PromptReco-v1/MINIAOD" : [None, None, None, "data_Run2018D_SingleMuon_PromptReco-v1"],
        # "/EGamma/Run2018D-PromptReco-v1/MINIAOD"     : [None, None, None, "data_Run2018D_EGamma_PromptReco-v1"],
        # "/DoubleMuon/Run2018D-PromptReco-v1/MINIAOD" : [None, None, None, "data_Run2018D_DoubleMuon_PromptReco-v1"],
        # "/MuonEG/Run2018D-PromptReco-v1/MINIAOD"     : [None, None, None, "data_Run2018D_MuonEG_PromptReco-v1"],
        
        "/JetHT/Run2018D-PromptReco-v2/MINIAOD"      : [None, None, None, "data_Run2018D_JetHT_PromptReco-v2"],
        "/MET/Run2018D-PromptReco-v2/MINIAOD"        : [None, None, None, "data_Run2018D_MET_PromptReco-v2"],
        "/SingleMuon/Run2018D-PromptReco-v2/MINIAOD" : [None, None, None, "data_Run2018D_SingleMuon_PromptReco-v2"],
        "/EGamma/Run2018D-PromptReco-v2/MINIAOD"     : [None, None, None, "data_Run2018D_EGamma_PromptReco-v2"],
        "/DoubleMuon/Run2018D-PromptReco-v2/MINIAOD" : [None, None, None, "data_Run2018D_DoubleMuon_PromptReco-v2"],
        "/MuonEG/Run2018D-PromptReco-v2/MINIAOD"     : [None, None, None, "data_Run2018D_MuonEG_PromptReco-v2"],
        },
    "ttbar" : {
        "/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [88.29, 1.0, 1.0, "ttdl_powheg"],
        # "/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [365.34, 1.0, 1.0, "ttsl_powheg"],

        "/TTJets_SingleLeptFromT_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"    : [109.1, 1.677,  1.0, "ttsl_top_mg"],
        "/TTJets_SingleLeptFromTbar_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [109.1, 1.677,  1.0, "ttsl_tbar_mg"],
        },
    "wjets" : {
        "/WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"   : [1379,   1.21, 1.005, "wjets_ht100to200"],
        "/WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"   : [359.7,  1.21, 1.136, "wjets_ht200to400"],
        "/WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"   : [48.91,  1.21, 1.186, "wjets_ht400to600"],
        "/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"   : [12.05,  1.21, 1.196, "wjets_ht600to800"],        
        "/WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"  : [5.501,  1.21, 1.173, "wjets_ht800to1200"],
        "/WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [1.329,  1.21, 1.077, "wjets_ht1200to2500"],
        "/WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"  : [.03216, 1.21, 1.044, "wjets_ht2500toInf"],

        "/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM" : [52940, 1.21, 1.0, "wjets_incl"],
        },
    "zinv" : {
        "/ZJetsToNuNu_HT-100To200_13TeV-madgraph/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"   : [280.35,  1.23, 1.073, "zinv_ht100to200"],
        "/ZJetsToNuNu_HT-200To400_13TeV-madgraph/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"   : [77.67,   1.23, 1.169, "zinv_ht200to400"],
        "/ZJetsToNuNu_HT-400To600_13TeV-madgraph/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"   : [10.73,   1.23, 1.200, "zinv_ht400to600"],
        "/ZJetsToNuNu_HT-600To800_13TeV-madgraph/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"   : [2.619,   1.23, 1.212, "zinv_ht600to800"],
        "/ZJetsToNuNu_HT-800To1200_13TeV-madgraph/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"  : [1.198,   1.23, 1.139, "zinv_ht800to1200"],
        "/ZJetsToNuNu_HT-1200To2500_13TeV-madgraph/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [.2915,   1.23, 1.032, "zinv_ht1200to2500"],
        "/ZJetsToNuNu_HT-2500ToInf_13TeV-madgraph/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"  : [.006669, 1.23, 0.985, "zinv_ht2500toInf"],
        },
    "dyjetsll" : {
        "/DYJetsToLL_M-50_HT-100to200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM"   : [141.7,   1.16, 1.137, "dyjetsll_ht100to200"],
        "/DYJetsToLL_M-50_HT-200to400_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM"   : [42.91,   1.16, 1.133, "dyjetsll_ht200to400"],
        "/DYJetsToLL_M-50_HT-400to600_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v3/MINIAODSIM"   : [5.867,   1.16, 1.175, "dyjetsll_ht400to600"],
        "/DYJetsToLL_M-50_HT-600to800_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM"   : [1.533,   1.16, 1.109, "dyjetsll_ht600to800"],
        "/DYJetsToLL_M-50_HT-800to1200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM"  : [.7350,   1.16, 0.994, "dyjetsll_ht800to1200"],
        "/DYJetsToLL_M-50_HT-1200to2500_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM" : [.1574,   1.16, 1.023, "dyjetsll_ht1200to2500"],
        "/DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM"  : [.003148, 1.16, 1.117, "dyjetsll_ht2500toInf"],

        "/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [5343, 1.16, 1.0, "dyjetsll_m50_incl"],
        },
    "singletop" : {
        "/ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext1-v3/MINIAODSIM"     : [34.91,  1.0, 0.53, "singletop_tW_tbar"],
        "/ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext1-v3/MINIAODSIM"         : [34.91,  1.0, 0.58, "singletop_tW_top"],
        "/ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext1-v4/MINIAODSIM"             : [3.74,   1.0, 1.0,  "singletop_schan"],
        "/ST_t-channel_antitop_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [80.95,  1.0, 1.0,  "singletop_tchan_tbar"],
        "/ST_t-channel_top_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"     : [136.02, 1.0, 1.0,  "singletop_tchan_top"],
        },
    "gjets" : {
        "/GJets_HT-40To100_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"  : [20790, 1.0, 1.0, "gjets_dr0p05_ht40to100"],         
        "/GJets_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [2305,  1.0, 1.0, "gjets_dr0p05_ht200to400"],
        "/GJets_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [274.4, 1.0, 1.0, "gjets_dr0p05_ht400to600"],

        "/GJets_HT-600ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext1-v2/MINIAODSIM" : [93.46, 1.0, 1.0, "gjets_dr0p05_ht600toInf_ext1"],
        },
    "ttv" : {
        "/ttWJets_TuneCP5_13TeV_madgraphMLM_pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext1-v2/MINIAODSIM" : [0.6105, 1.0, 1.0, "ttw_mg"],
        "/ttZJets_TuneCP5_13TeV_madgraphMLM_pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext1-v2/MINIAODSIM" : [0.7826, 1.0, 1.0, "ttz_mg"],
        },
    "tttt": {
        "/TTTT_TuneCP5_13TeV-amcatnlo-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext1-v2/MINIAODSIM" : [0.008213, 1.0, 1.0, "tttt"],
        },
    "diboson" : {
        "/WW_TuneCP5_13TeV-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM" : [75.80, 1.878, 1.0, "ww"],
        "/WZ_TuneCP5_13TeV-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v3/MINIAODSIM" : [27.57, 1.0, 1.0, "wz"],
        },
    "qcd_ht" : {
        "/QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"   : [323300, 1.0, 1.0, "qcd_ht300to500"],
        "/QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"   : [30000,  1.0, 1.0, "qcd_ht500to700"],
        "/QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"  : [6330,   1.0, 1.0, "qcd_ht700to1000"], 
        "/QCD_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [1098,   1.0, 1.0, "qcd_ht1000to1500"],
        "/QCD_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [98.90,  1.0, 1.0, "qcd_ht1500to2000"],
        "/QCD_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"  : [20.26,  1.0, 1.0, "qcd_ht2000toInf"],
        },
    

    }

if __name__ == "__main__":

    for type in samples_2018:
        if type == "data":
            continue
        for ds in samples_2018[type]:
            vals = samples_2018[type][ds]

            print '"{0}|{1}|{2}|{3}",'.format(ds, *samples_2018[type][ds])



