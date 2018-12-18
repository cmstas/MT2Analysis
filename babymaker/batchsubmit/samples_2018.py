import sys
sys.path.append("ProjectMetis/scripts")
import dis_client

# format is samples_2018[type][ds_name] = [xsec, kfactor, filt_eff]

samples_2018 = {
    "data" : {
        "/JetHT/Run2018A-17Sep2018-v1/MINIAOD"      : [None, None, None, "data_Run2018A_JetHT_17Sep2018"],
        "/MET/Run2018A-17Sep2018-v1/MINIAOD"        : [None, None, None, "data_Run2018A_MET_17Sep2018"],
        "/SingleMuon/Run2018A-17Sep2018-v2/MINIAOD" : [None, None, None, "data_Run2018A_SingleMuon_17Sep2018"],
        "/EGamma/Run2018A-17Sep2018-v1/MINIAOD"     : [None, None, None, "data_Run2018A_EGamma_17Sep2018"],
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

        "/JetHT/Run2018D-PromptReco-v1/MINIAOD"      : [None, None, None, "data_Run2018D_JetHT_PromptReco-v1"],
        "/MET/Run2018D-PromptReco-v1/MINIAOD"        : [None, None, None, "data_Run2018D_MET_PromptReco-v1"],
        # "/SingleMuon/Run2018D-PromptReco-v1/MINIAOD" : [None, None, None, "data_Run2018D_SingleMuon_PromptReco-v1"],
        "/EGamma/Run2018D-PromptReco-v1/MINIAOD"     : [None, None, None, "data_Run2018D_EGamma_PromptReco-v1"],
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
        },
    "wjets" : {
        },
    "zinv" : {
        "/ZJetsToNuNu_HT-800To1200_13TeV-madgraph/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [1.474,   1.0,  1.0, "zinv_ht800to1200"],
        },
    "dyjetsll" : {
        },
    "singletop" : {
        },
    "gjets" : {
        "/GJets_HT-40To100_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"  : [20790, 1.0, 1.0, "gjets_dr0p05_ht40to100"], 
        "/GJets_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [2305,  1.0, 1.0, "gjets_dr0p05_ht200to400"],
        "/GJets_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [274.4, 1.0, 1.0, "gjets_dr0p05_ht400to600"],

        "/GJets_HT-600ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext1-v2/MINIAODSIM" : [93.46, 1.0, 1.0, "gjets_dr0p05_ht600toInf_ext1"],
        },
    "ttv" : {
        },
    "diboson" : {
        "/WW_TuneCP5_13TeV-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM" : [63.21, 1.878, 1.0, "ww"],
        },
    "qcd_ht" : {
        "/QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [29070, 1.0, 1.0, "qcd_ht500to700"],
        },
    

    }

if __name__ == "__main__":

    pass



