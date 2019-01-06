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
        },
    "wjets" : {
        },
    "zinv" : {
        "/ZJetsToNuNu_HT-100To200_13TeV-madgraph/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"   : [280.35,  1.23, 1.0, "zinv_ht100to200"],
        "/ZJetsToNuNu_HT-200To400_13TeV-madgraph/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"   : [77.67,   1.23, 1.0, "zinv_ht200to400"],
        "/ZJetsToNuNu_HT-800To1200_13TeV-madgraph/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"  : [1.474,   1.0,  1.0, "zinv_ht800to1200"],
        },
    "dyjetsll" : {
        },
    "singletop" : {
        "/ST_tW_antitop_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext1-v3/MINIAODSIM" : [34.91,  1.0, 0.53, "singletop_tW_tbar"],
        "/ST_tW_top_5f_NoFullyHadronicDecays_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15_ext1-v3/MINIAODSIM"     : [34.91,  1.0, 0.58, "singletop_tW_tbar"],
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
        "/QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"  : [29070,  1.0, 1.0, "qcd_ht500to700"],
        "/QCD_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM" : [20.54,  1.0, 1.0, "qcd_ht2000toInf"],
        },
    

    }

if __name__ == "__main__":

    for type in samples_2018:
        if type == "data":
            continue
        for ds in samples_2018[type]:
            vals = samples_2018[type][ds]

            print '"{0}|{1}|{2}|{3}",'.format(ds, *samples_2018[type][ds])



