#! /usr/bin/env python
#
# Here we store all of the configurations used in the babymaker/looper
# (e.g. json, lumi, btag scale factors, JECs, filters, etc)
#
# This is more convenient than editing the babymaker/looper every time
# you change what you're running over
#
# To use within a C++ program, run "python configurations.py" to generate
# a .cc file, and then use the configurations.[h,cc] files (see MT2Looper for example)
#
# To use within a python script, simply do "from configurations import MT2Config_defs"
#

import copy

MT2Config_defs = {}

MT2Config_defs["data_2017_31Mar2018"] = {
    "year" : 2017,
    "cmssw_ver" : 94,
    "json" : "Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_v1_snt.txt",
    "lumi" : 41.37,
    "btagcalib_csv" : "DeepCSV_94XSF_V3_B_F.csv",
    "btageff_file" : "btageff__DeepCSV_ttbar_powheg_pythia8_25ns_Fall17.root",
    "btag_med_threshold_DeepCSV" : 0.4941,
    "btag_med_threshold_CSVv2" : 0.8838,
    "ea_version" : 3,
    "jet_id" : "2017_v1",
    "JRT_file" : "JetResponseTemplates_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma.root",
    "rphi_tag" : "V00-10-09_2017fullYear",
    "JECs" : [ ["2017B", "Fall17_17Nov2017B_V32_DATA"],
               ["2017C", "Fall17_17Nov2017C_V32_DATA"],
               ["2017D", "Fall17_17Nov2017DE_V32_DATA"],
               ["2017E", "Fall17_17Nov2017DE_V32_DATA"],
               ["2017F", "Fall17_17Nov2017F_V32_DATA"]
               ],
    "filters" : [ "eeBadScFilter",
                  "globalSuperTightHalo2016Filter",
                  "goodVertices",
                  "HBHENoiseFilter",
                  "HBHENoiseIsoFilter",
                  "EcalDeadCellTriggerPrimitiveFilter",
                  "ecalBadCalibFilterUpdate",
                  "badMuonFilter",
                  # "badChargedCandidateFilter"
                  ],
    "triggers" : {
        "SR" : [ 
            "PFHT1050",
            "PFJet500",
            "PFHT800_PFMET75_PFMHT75",
            "PFHT500_PFMET100_PFMHT100",
            "PFMET120_PFMHT120",
            "PFMET120_PFMHT120_PFHT60",
            "PFMETNoMu120_PFMHTNoMu120",
            "PFMETNoMu120_PFMHTNoMu120_PFHT60"
            ],
        "Photon" : [ 
            "Photon200" 
            ],
        "SingleEl" : [
            "SingleEl",
            "SingleEl_NonIso"
            ],
        "SingleMu" : [
            "SingleMu",
            "SingleMu_NonIso"
            ],
        "DilepSF" : [
            "DoubleEl",
            "DoubleMu",
            "Photon200",
            "DoubleMu_NonIso",
            "SingleMu_NonIso",
            "DoubleEl33"
            ],
        "DilepOF" : [
            "MuX_Ele12",
            "Mu8_EleX",
            "Mu12_EleX",
            "Mu37_Ele27_NonIso",
            "Mu27_Ele37_NonIso",
            "Photon200",
            "SingleMu_NonIso"
            ],
        "prescaledHT" : [
            "PFHT180_Prescale",
            "PFHT250_Prescale",
            "PFHT370_Prescale",
            "PFHT430_Prescale",
            "PFHT510_Prescale",
            "PFHT590_Prescale",
            "PFHT680_Prescale",
            "PFHT780_Prescale",
            "PFHT890_Prescale",
            "PFHT1050",
            ]
        }                              
    }


MT2Config_defs["data_2017_Prompt"] = {
    "year" : 2017,
    "cmssw_ver" : 94,
    "json" : "Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON_snt.txt",
    "lumi" : 41.97,
    "btagcalib_csv" : "DeepCSV_94XSF_V3_B_F.csv",
    "btageff_file" : "btageff__DeepCSV_ttbar_powheg_pythia8_25ns_Fall17.root",
    "btag_med_threshold_DeepCSV" : 0.4941,
    "btag_med_threshold_CSVv2" : 0.8838,
    "ea_version" : 3,
    "jet_id" : "2017_v1",
    "JRT_file" : "JetResponseTemplates_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma.root",
    "rphi_tag" : "V00-10-09_2017fullYear",
    "JECs" : [ ["", "Summer16_23Sep2016HV4_DATA"] 
               ],
    "filters" : [ "eeBadScFilter",
                  "globalSuperTightHalo2016Filter",
                  "goodVertices",
                  "HBHENoiseFilter",
                  "HBHENoiseIsoFilter",
                  "EcalDeadCellTriggerPrimitiveFilter"
                  ],
    "triggers" : MT2Config_defs["data_2017_31Mar2018"]["triggers"]
    }

MT2Config_defs["data_2018_Prompt"] = {
    "year" : 2018,
    "cmssw_ver" : 102,
    "json" : "Cert_314472-325175_13TeV_17SeptEarlyReReco2018ABC_PromptEraD_Collisions18_JSON_snt.txt",
    "lumi" : 59.74,
    "btagcalib_csv" : "DeepCSV_102XSF_V1.csv",
    "btageff_file" : "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Autumn18.root",
    "btag_med_threshold_DeepCSV" : 0.4184,
    "btag_med_threshold_CSVv2" : 0.8838,
    "ea_version" : 3,
    "jet_id" : "2018_v1",
    "JRT_file" : "JetResponseTemplates_ptBinned_102x_JetID_PUID_BTagSFs_core2sigma.root",
    "rphi_tag" : "V00-10-09_2018fullYear",
    "JECs" : [ ["2018A", "Autumn18_RunA_V8_DATA"],
               ["2018B", "Autumn18_RunB_V8_DATA"],
               ["2018C", "Autumn18_RunC_V8_DATA"],
               ["2018D", "Autumn18_RunD_V8_DATA"],
               ],
    "filters"  : MT2Config_defs["data_2017_31Mar2018"]["filters"],
    "triggers" : MT2Config_defs["data_2017_31Mar2018"]["triggers"]
    }

MT2Config_defs["data_2018_17Sep2018"] = {
    "year" : 2018,
    "cmssw_ver" : 102,
    "json" : "Cert_314472-325175_13TeV_17SeptEarlyReReco2018ABC_PromptEraD_Collisions18_JSON_snt.txt",
    "lumi" : 59.74,
    "btagcalib_csv" : "DeepCSV_102XSF_V1.csv",
    "btageff_file" : "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Autumn18.root",
    "btag_med_threshold_DeepCSV" : 0.4184,
    "btag_med_threshold_CSVv2" : 0.8838,
    "ea_version" : 3,
    "jet_id" : "2018_v1",
    "JRT_file" : "JetResponseTemplates_ptBinned_102x_JetID_PUID_BTagSFs_core2sigma.root",
    "rphi_tag" : "V00-10-09_2018fullYear",
    "JECs" : [ ["2018A", "Autumn18_RunA_V8_DATA"],
               ["2018B", "Autumn18_RunB_V8_DATA"],
               ["2018C", "Autumn18_RunC_V8_DATA"],
               ["2018D", "Autumn18_RunD_V8_DATA"],
               ],
    "filters"  : MT2Config_defs["data_2017_31Mar2018"]["filters"],
    "triggers" : MT2Config_defs["data_2017_31Mar2018"]["triggers"]
    }

MT2Config_defs["data_2016_Moriond17"] = {
    "year" : 2016,
    "cmssw_ver" : 80,
    "json" : "Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON_snt.txt",
    "lumi" : 35.922,
    "btagcalib_csv" : "DeepCSV_Moriond17_B_H.csv",
    "btageff_file" : "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Summer16_94x.root",
    "btag_med_threshold_DeepCSV" : 0.6324,
    "btag_med_threshold_CSVv2" : 0.8484,
    "ea_version" : 1,
    "jet_id" : "50nsV1",
    "JRT_file" : "JetResponseTemplates_ptBinned_80x_JetID_PUID_BTagSFs_core2sigma.root",
    "rphi_tag" : "V00-10-10_2016fullYear",
    "JECs" : [ ["2016B", "Summer16_23Sep2016BCDV4_DATA"],
               ["2016C", "Summer16_23Sep2016BCDV4_DATA"],
               ["2016D", "Summer16_23Sep2016BCDV4_DATA"],
               ["2016E", "Summer16_23Sep2016EFV4_DATA"],
               ["2016F", "Summer16_23Sep2016EFV4_DATA"],
               ["2016G", "Summer16_23Sep2016GV4_DATA"],
               ["2016H", "Summer16_23Sep2016HV4_DATA"]
               ],
    "filters" : [ "eeBadScFilter",
                  "globalSuperTightHalo2016Filter",
                  "goodVertices",
                  "HBHENoiseFilter",
                  "HBHENoiseIsoFilter",
                  "EcalDeadCellTriggerPrimitiveFilter",
                  "badMuonFilterV2",
                  "badChargedHadronFilterV2"
                  ],
    "triggers" : {
        "SR" : [ 
            "PFHT900",
            "PFJet450",
            "PFHT300_PFMET110",
            "PFMET120_PFMHT120",
            "PFMETNoMu120_PFMHTNoMu120",
            ],
        "Photon" : [ 
            "Photon165_HE10" 
            ],
        "SingleEl" : [
            "SingleEl",
            "SingleEl_NonIso"
            ],
        "SingleMu" : [
            "SingleMu",
            "SingleMu_NonIso"
            ],
        "DilepSF" : [
            "DoubleEl",
            "DoubleMu",
            "Photon165_HE10",
            "DoubleMu_NonIso",
            "SingleMu_NonIso",
            "DoubleEl33"
            ],
        "DilepOF" : [
            "MuX_Ele12",
            "Mu8_EleX",
            "Mu30_Ele30_NonIso",
            "Mu33_Ele33_NonIso",
            "Photon165_HE10",
            "SingleMu_NonIso"
            ],
        "prescaledHT" : [
            "PFHT125_Prescale",
            "PFHT200_Prescale",
            "PFHT300_Prescale",
            "PFHT350_Prescale",
            "PFHT475_Prescale",
            "PFHT600_Prescale",
            "PFHT900",
            ]
        }
    }

MT2Config_defs["data_2016_94x"] = copy.deepcopy(MT2Config_defs["data_2016_Moriond17"])
MT2Config_defs["data_2016_94x"]["cmssw_ver"] = 94
MT2Config_defs["data_2016_94x"]["btagcalib_csv"] = "DeepCSV_2016LegacySF_V1.csv"
MT2Config_defs["data_2016_94x"]["btag_med_threshold_DeepCSV"] = 0.6321
MT2Config_defs["data_2016_94x"]["JECs"] = [
    ["2016B", "Summer16_07Aug2017BCD_V11_DATA"],
    ["2016C", "Summer16_07Aug2017BCD_V11_DATA"],
    ["2016D", "Summer16_07Aug2017BCD_V11_DATA"],
    ["2016E", "Summer16_07Aug2017EF_V11_DATA"],
    ["2016F", "Summer16_07Aug2017EF_V11_DATA"],
    ["2016G", "Summer16_07Aug2017GH_V11_DATA"],
    ["2016H", "Summer16_07Aug2017GH_V11_DATA"]
    ]
MT2Config_defs["data_2016_94x"]["filters"] = [
    "eeBadScFilter",
    "globalSuperTightHalo2016Filter",
    "goodVertices",
    "HBHENoiseFilter",
    "HBHENoiseIsoFilter",
    "EcalDeadCellTriggerPrimitiveFilter",
    "badMuonFilter",
    # "badChargedCandidateFilter"
    ]

MT2Config_defs["mc_94x_Fall17"] = {
    "year" : 2017,
    "cmssw_ver" : 94,
    "lumi" : 41.529,
    "btagcalib_csv" : "DeepCSV_94XSF_V3_B_F.csv",
    "btageff_file" : "btageff__DeepCSV_ttbar_powheg_pythia8_25ns_Fall17.root",
    "btag_med_threshold_DeepCSV" : 0.4941,
    "btag_med_threshold_CSVv2" : 0.8838,
    "pu_weights_file" : "puWeight2017.root",
    "elSF_IDISOfile" : "lepsf/Fall17/ElectronScaleFactors_Run2017.root",
    "elSF_TRKfile" : "lepsf/Fall17/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root",
    "elSF_TRKfileLowPt" : "lepsf/Fall17/egammaEffi.txt_EGM2D_runBCDEF_passingRECO_lowEt.root",
    "elSF_IDhistName" : "Run2017_CutBasedVetoNoIso94XV2",
    "elSF_ISOhistName" : "Run2017_MVAVLooseTightIP2DMini",
    "muSF_IDfile" : "lepsf/Fall17/MuonPOG_SF_ID_2017BCDEF.root",
    "muSF_ISOfile" : "lepsf/Fall17/SUS_MuSF_MiniIso2_LooseID.root",
    "muSF_IPfile" : "",
    "muSF_TRKfile" : "",
    "muSF_IDhistName": "NUM_LooseID_DEN_genTracks_pt_abseta",
    "muSF_ISOhistName" : "TnP_MC_NUM_MiniIso02Cut_DEN_LooseID_PAR_pt_eta",
    "muSF_IPhistName" : "",
    "muSF_TRKLT10histName" : "",
    "muSF_TRKGT10histName" : "",
    "dilep_trigeff_file" : "lepsf/trigeff/trigeff_dilep_2017fullYear.root",
    "ea_version" : 3,
    "jet_id" : "2017_v1",
    "JRT_file" : "JetResponseTemplates_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma.root",
    "rphi_tag" : "V00-10-09_2017fullYear",
    "JECs" : [ ["", "Fall17_17Nov2017_V32_MC"]
               ],
    "filters" : [ "globalSuperTightHalo2016Filter",
                  "goodVertices",
                  "HBHENoiseFilter",
                  "HBHENoiseIsoFilter",
                  "EcalDeadCellTriggerPrimitiveFilter",
                  "ecalBadCalibFilterUpdate",
                  "badMuonFilter",
                  # "badChargedCandidateFilter"
                  ],
}

MT2Config_defs["mc_102x_Autumn18"] = {
    "year" : 2018,
    "cmssw_ver" : 102,
    "lumi" : 59.74,
    "btagcalib_csv" : "DeepCSV_102XSF_V1.csv",
    "btageff_file" : "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Autumn18.root",
    "btag_med_threshold_DeepCSV" : 0.4184,
    "btag_med_threshold_CSVv2" : 0.8838,
    "pu_weights_file" : "puWeight2018.root",

    "elSF_IDISOfile" : "lepsf/Autumn18/ElectronScaleFactors_Run2018.root",
    "elSF_TRKfile" : "lepsf/Autumn18/egammaEffi.txt_EGM2D.root",
    # "elSF_TRKfileLowPt" : "lepsf/Autumn18/egammaEffi.txt_EGM2D_low.root",  # DO NOT TRUST, getting scale factors >1.9
    "elSF_TRKfileLowPt" : "lepsf/Autumn18/egammaEffi.txt_EGM2D.root",        # use above bin instead
    "elSF_IDhistName" : "Run2018_CutBasedVetoNoIso94XV2",
    "elSF_ISOhistName" : "Run2018_Mini",
    "muSF_IDfile" : "lepsf/Autumn18/MuonPOG_RunABCD_SF_ID.root",
    "muSF_ISOfile" : "lepsf/Fall17/SUS_MuSF_MiniIso2_LooseID.root",
    "muSF_IPfile" : "",
    "muSF_TRKfile" : "",
    "muSF_IDhistName": "NUM_LooseID_DEN_genTracks_pt_abseta",
    "muSF_ISOhistName" : "TnP_MC_NUM_MiniIso02Cut_DEN_LooseID_PAR_pt_eta",
    "muSF_IPhistName" : "",
    "muSF_TRKLT10histName" : "",
    "muSF_TRKGT10histName" : "",

    # "elSF_IDISOfile" : "lepsf/Fall17/ElectronScaleFactors_Run2017.root",
    # "elSF_TRKfile" : "lepsf/Fall17/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root",
    # "elSF_TRKfileLowPt" : "lepsf/Fall17/egammaEffi.txt_EGM2D_runBCDEF_passingRECO_lowEt.root",
    # "elSF_IDhistName" : "Run2017_CutBasedVetoNoIso94XV2",
    # "elSF_ISOhistName" : "Run2017_MVAVLooseTightIP2DMini",
    # "muSF_IDfile" : "lepsf/Fall17/MuonPOG_SF_ID_2017BCDEF.root",
    # "muSF_ISOfile" : "lepsf/Fall17/SUS_MuSF_MiniIso2_LooseID.root",
    # "muSF_IPfile" : "",
    # "muSF_TRKfile" : "",
    # "muSF_IDhistName": "NUM_LooseID_DEN_genTracks_pt_abseta",
    # "muSF_ISOhistName" : "TnP_MC_NUM_MiniIso02Cut_DEN_LooseID_PAR_pt_eta",
    # "muSF_IPhistName" : "",
    # "muSF_TRKLT10histName" : "",
    # "muSF_TRKGT10histName" : "",

    "dilep_trigeff_file" : "lepsf/trigeff/trigeff_dilep_2018fullYear.root",
    "ea_version" : 3,
    "jet_id" : "2018_v1",
    "JRT_file" : "JetResponseTemplates_ptBinned_102x_JetID_PUID_BTagSFs_core2sigma.root",
    "rphi_tag" : "V00-10-09_2018fullYear",
    "JECs" : [ ["", "Autumn18_V8_MC"]
               ],
    "filters" : [ "globalSuperTightHalo2016Filter",
                  "goodVertices",
                  "HBHENoiseFilter",
                  "HBHENoiseIsoFilter",
                  "EcalDeadCellTriggerPrimitiveFilter",
                  "ecalBadCalibFilterUpdate",
                  "badMuonFilter",
                  # "badChargedCandidateFilter"
                  ],
}


MT2Config_defs["mc_80x_Summer16"] = {
    "year" : 2016,
    "cmssw_ver" : 80,
    "lumi" : 35.922,
    "btagcalib_csv" : "DeepCSV_Moriond17_B_H.csv",
    "btageff_file" : "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Summer16_94x.root",
    "btag_med_threshold_DeepCSV" : 0.6324,
    "btag_med_threshold_CSVv2" : 0.8484,
    "pu_weights_file" : "puWeight2016.root",
    "elSF_IDISOfile" : "lepsf/moriond17/scaleFactors_el_moriond_2017.root",
    "elSF_TRKfile" : "lepsf/moriond17/egammaEffi.txt_EGM2D.root",
    "elSF_IDhistName" : "GsfElectronToCutBasedSpring15V",
    "elSF_ISOhistName" : "MVAVLooseElectronToMini",
    "muSF_IDfile" : "lepsf/moriond17/TnP_NUM_LooseID_DENOM_generalTracks_VAR_map_pt_eta.root",
    "muSF_ISOfile" : "lepsf/moriond17/TnP_NUM_MiniIsoTight_DENOM_LooseID_VAR_map_pt_eta.root",
    "muSF_IPfile" : "lepsf/moriond17/TnP_NUM_MediumIP2D_DENOM_LooseID_VAR_map_pt_eta.root",
    "muSF_TRKfile" : "lepsf/moriond17/Tracking_EfficienciesAndSF_BCDEFGH_hists.root",
    "muSF_IDhistName": "SF",
    "muSF_ISOhistName" : "SF",
    "muSF_IPhistName" : "SF",
    "muSF_TRKLT10histName" : "ratio_eff_eta3_tk0_dr030e030_corr",
    "muSF_TRKGT10histName" : "ratio_eff_eta3_dr030e030_corr",
    "dilep_trigeff_file" : "lepsf/trigeff/trigeff_dilep_2016fullYear.root",
    "ea_version" : 1,
    "jet_id" : "50nsV1",
    "JRT_file" : "JetResponseTemplates_ptBinned_80x_JetID_PUID_BTagSFs_core2sigma.root",
    "rphi_tag" : "V00-10-10_2016fullYear",
    "JECs" : [ ["", "Summer16_23Sep2016V4_MC"]
               ],
    "filters" : [ "globalSuperTightHalo2016Filter",
                  "goodVertices",
                  "HBHENoiseFilter",
                  "HBHENoiseIsoFilter",
                  "EcalDeadCellTriggerPrimitiveFilter",
                  "badMuonFilterV2",
                  "badChargedHadronFilterV2"
                  ],
}

MT2Config_defs["mc_94x_Summer16"] = {
    "year" : 2016,
    "cmssw_ver" : 94,
    "lumi" : 35.922,
    "btagcalib_csv" : "DeepCSV_2016LegacySF_V1.csv",
    "btageff_file" : "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Summer16_94x.root",
    "btag_med_threshold_DeepCSV" : 0.6321,
    "btag_med_threshold_CSVv2" : 0.8484,
    "pu_weights_file" : "puWeight2016.root",
    "elSF_IDISOfile" : "lepsf/Summer16_94x/ElectronScaleFactors_Run2016.root",
    "elSF_TRKfile" : "lepsf/moriond17/egammaEffi.txt_EGM2D.root",
    "elSF_IDhistName" : "Run2016_CutBasedVetoNoIso94XV2",
    "elSF_ISOhistName" : "Run2016_Mini",
    "muSF_IDfile" : "lepsf/moriond17/TnP_NUM_LooseID_DENOM_generalTracks_VAR_map_pt_eta.root",
    "muSF_ISOfile" : "lepsf/moriond17/TnP_NUM_MiniIsoTight_DENOM_LooseID_VAR_map_pt_eta.root",
    "muSF_IPfile" : "lepsf/moriond17/TnP_NUM_MediumIP2D_DENOM_LooseID_VAR_map_pt_eta.root",
    "muSF_TRKfile" : "lepsf/moriond17/Tracking_EfficienciesAndSF_BCDEFGH_hists.root",
    "muSF_IDhistName": "SF",
    "muSF_ISOhistName" : "SF",
    "muSF_IPhistName" : "SF",
    "muSF_TRKLT10histName" : "ratio_eff_eta3_tk0_dr030e030_corr",
    "muSF_TRKGT10histName" : "ratio_eff_eta3_dr030e030_corr",
    "dilep_trigeff_file" : "lepsf/trigeff/trigeff_dilep_2016fullYear.root",
    "ea_version" : 1,
    "jet_id" : "50nsV1",
    "JRT_file" : "JetResponseTemplates_ptBinned_80x_JetID_PUID_BTagSFs_core2sigma.root",
    "rphi_tag" : "V00-10-10_2016fullYear",
    "JECs" : [ ["", "Summer16_07Aug2017_V11_MC"]
               ],
    "filters" : [ "eeBadScFilter",
                  "globalSuperTightHalo2016Filter",
                  "goodVertices",
                  "HBHENoiseFilter",
                  "HBHENoiseIsoFilter",
                  "EcalDeadCellTriggerPrimitiveFilter",
                  "badMuonFilter",
                  # "badChargedCandidateFilter"
                  ],
}

MT2Config_defs["mc_80x_fastsim_Moriond17"] = copy.deepcopy(MT2Config_defs["mc_80x_Summer16"])
MT2Config_defs["mc_80x_fastsim_Moriond17"]["btagcalib_csv_fastsim"] = "DeepCSV_94X_2016_Fastsim.csv"
MT2Config_defs["mc_80x_fastsim_Moriond17"]["btageff_file_fastsim"] = "btageff__SMS-T1tttt_2016_80X_deepCSV.root"
MT2Config_defs["mc_80x_fastsim_Moriond17"]["elSF_IDISOfile_fastsim"] = "lepsf/moriond17/sf_el_vetoCB_mini01.root"
MT2Config_defs["mc_80x_fastsim_Moriond17"]["elSF_IDhistName_fastsim"] = "histo2D"
MT2Config_defs["mc_80x_fastsim_Moriond17"]["elSF_ISOhistName_fastsim"] = ""
MT2Config_defs["mc_80x_fastsim_Moriond17"]["muSF_IDfile_fastsim"] = "lepsf/moriond17/sf_mu_looseID.root"
MT2Config_defs["mc_80x_fastsim_Moriond17"]["muSF_ISOfile_fastsim"] = "lepsf/moriond17/sf_mu_looseID_mini02.root"
MT2Config_defs["mc_80x_fastsim_Moriond17"]["muSF_IPfile_fastsim"] = "lepsf/moriond17/sf_mu_mediumID_looseIP2D.root"
MT2Config_defs["mc_80x_fastsim_Moriond17"]["muSF_IDhistName_fastsim"] = "histo2D"
MT2Config_defs["mc_80x_fastsim_Moriond17"]["muSF_ISOhistName_fastsim"] = "histo2D"
MT2Config_defs["mc_80x_fastsim_Moriond17"]["muSF_IPhistName_fastsim"] = "histo2D"
MT2Config_defs["mc_80x_fastsim_Moriond17"]["JECs"] = [ ["", "Spring16_FastSimV1_MC"] ]
MT2Config_defs["mc_80x_fastsim_Moriond17"]["filters"] = [
    "goodVertices",
    "HBHENoiseFilter",
    "HBHENoiseIsoFilter",
    "EcalDeadCellTriggerPrimitiveFilter",
    "badMuonFilterV2",
    "badChargedHadronFilterV2"
    ]

MT2Config_defs["mc_94x_fastsim_Summer16"] = copy.deepcopy(MT2Config_defs["mc_94x_Summer16"])
MT2Config_defs["mc_94x_fastsim_Summer16"]["btagcalib_csv_fastsim"] = "DeepCSV_94X_2016_Fastsim.csv"
MT2Config_defs["mc_94x_fastsim_Summer16"]["btageff_file_fastsim"] = "btageff__SMS-T1tttt_2016_80X_deepCSV.root"
MT2Config_defs["mc_94x_fastsim_Summer16"]["elSF_IDISOfile_fastsim"] = "lepsf/moriond17/sf_el_vetoCB_mini01.root"
MT2Config_defs["mc_94x_fastsim_Summer16"]["elSF_IDhistName_fastsim"] = "histo2D"
MT2Config_defs["mc_94x_fastsim_Summer16"]["elSF_ISOhistName_fastsim"] = ""
MT2Config_defs["mc_94x_fastsim_Summer16"]["muSF_IDfile_fastsim"] = "lepsf/moriond17/sf_mu_looseID.root"
MT2Config_defs["mc_94x_fastsim_Summer16"]["muSF_ISOfile_fastsim"] = "lepsf/moriond17/sf_mu_looseID_mini02.root"
MT2Config_defs["mc_94x_fastsim_Summer16"]["muSF_IPfile_fastsim"] = "lepsf/moriond17/sf_mu_mediumID_looseIP2D.root"
MT2Config_defs["mc_94x_fastsim_Summer16"]["muSF_IDhistName_fastsim"] = "histo2D"
MT2Config_defs["mc_94x_fastsim_Summer16"]["muSF_ISOhistName_fastsim"] = "histo2D"
MT2Config_defs["mc_94x_fastsim_Summer16"]["muSF_IPhistName_fastsim"] = "histo2D"
MT2Config_defs["mc_94x_fastsim_Summer16"]["JECs"] = [ ["", "Spring16_FastSimV1_MC"] ]
MT2Config_defs["mc_94x_fastsim_Summer16"]["filters"] = [
    "goodVertices",
    "HBHENoiseFilter",
    "HBHENoiseIsoFilter",
    "EcalDeadCellTriggerPrimitiveFilter",
    "badMuonFilter",
    # "badChargedCandidateFilter"
    ]

MT2Config_defs["mc_94x_fastsim_Fall17"] = copy.deepcopy(MT2Config_defs["mc_94x_Fall17"])
MT2Config_defs["mc_94x_fastsim_Fall17"]["btagcalib_csv_fastsim"] = "DeepCSV_94X_2017_Fastsim.csv"
MT2Config_defs["mc_94x_fastsim_Fall17"]["btageff_file_fastsim"] = "btageff__SMS-T1tttt_2017_94X_deepCSV.root"
MT2Config_defs["mc_94x_fastsim_Fall17"]["elSF_IDISOfile_fastsim"] = "lepsf/Fall17/ElectronScaleFactors_Run2017_Fastsim.root"
MT2Config_defs["mc_94x_fastsim_Fall17"]["elSF_IDhistName_fastsim"] = "CutBasedVetoNoIso94XV2_sf"
MT2Config_defs["mc_94x_fastsim_Fall17"]["elSF_ISOhistName_fastsim"] = "MVAVLooseTightIP2DMini_sf"
MT2Config_defs["mc_94x_fastsim_Fall17"]["muSF_IDfile_fastsim"] = "lepsf/moriond17/sf_mu_looseID.root"
MT2Config_defs["mc_94x_fastsim_Fall17"]["muSF_ISOfile_fastsim"] = "lepsf/Fall17/detailed_mu_full_fast_sf_17.root"
MT2Config_defs["mc_94x_fastsim_Fall17"]["muSF_IPfile_fastsim"] = ""
MT2Config_defs["mc_94x_fastsim_Fall17"]["muSF_IDhistName_fastsim"] = "histo2D"
MT2Config_defs["mc_94x_fastsim_Fall17"]["muSF_ISOhistName_fastsim"] = "miniIso02_LooseId_sf"
MT2Config_defs["mc_94x_fastsim_Fall17"]["muSF_IPhistName_fastsim"] = ""
MT2Config_defs["mc_94x_fastsim_Fall17"]["JECs"] = [ ["", "Fall17_FastsimV1"] ]
MT2Config_defs["mc_94x_fastsim_Fall17"]["filters"] = [
    "goodVertices",
    "HBHENoiseFilter",
    "HBHENoiseIsoFilter",
    "EcalDeadCellTriggerPrimitiveFilter",
    "badMuonFilter",
    # "badChargedCandidateFilter",
    "ecalBadCalibFilterUpdate"
    ]

MT2Config_defs["mc_102x_fastsim_Autumn18"] = copy.deepcopy(MT2Config_defs["mc_102x_Autumn18"])
MT2Config_defs["mc_102x_fastsim_Autumn18"]["btagcalib_csv_fastsim"] = "DeepCSV_102X_2018_Fastsim.csv"
MT2Config_defs["mc_102x_fastsim_Autumn18"]["btageff_file_fastsim"] = "btageff__DeepCSV_SMS_T2tt_fastsim_Autumn18.root"
MT2Config_defs["mc_102x_fastsim_Autumn18"]["elSF_IDISOfile_fastsim"] = "lepsf/Autumn18/detailed_ele_full_fast_sf_18.root"
MT2Config_defs["mc_102x_fastsim_Autumn18"]["elSF_IDhistName_fastsim"] = "CutBasedVetoNoIso94XV2_sf"
MT2Config_defs["mc_102x_fastsim_Autumn18"]["elSF_ISOhistName_fastsim"] = "MVAVLooseTightIP2DMini_sf"
MT2Config_defs["mc_102x_fastsim_Autumn18"]["muSF_IDfile_fastsim"] = "lepsf/moriond17/sf_mu_looseID.root"
MT2Config_defs["mc_102x_fastsim_Autumn18"]["muSF_ISOfile_fastsim"] = "lepsf/Autumn18/detailed_mu_full_fast_sf_18.root"
MT2Config_defs["mc_102x_fastsim_Autumn18"]["muSF_IPfile_fastsim"] = ""
MT2Config_defs["mc_102x_fastsim_Autumn18"]["muSF_IDhistName_fastsim"] = "histo2D"
MT2Config_defs["mc_102x_fastsim_Autumn18"]["muSF_ISOhistName_fastsim"] = "miniIso02_LooseId_sf"
MT2Config_defs["mc_102x_fastsim_Autumn18"]["muSF_IPhistName_fastsim"] = ""
MT2Config_defs["mc_102x_fastsim_Autumn18"]["JECs"] = [ ["", "Autumn18_FastsimV1"] ]
MT2Config_defs["mc_102x_fastsim_Autumn18"]["filters"] = [
    "goodVertices",
    "HBHENoiseFilter",
    "HBHENoiseIsoFilter",
    "EcalDeadCellTriggerPrimitiveFilter",
    "badMuonFilter",
    # "badChargedCandidateFilter",
    "ecalBadCalibFilterUpdate"
    ]

def GetFilterString(config_tag):
    if config_tag not in MT2Config_defs:
        raise ValueError("'{0}' not a valid configuration!".format(config_tag))
    c = MT2Config_defs[config_tag]
    s = "( nVert>0 && nJet30FailId==0"
    for filt in c["filters"]:
        s += " && Flag_"+filt
    s += " )"
    return s

def GetTriggerString(config_tag, trig_reg):
    if config_tag not in MT2Config_defs:
        raise ValueError("'{0}' not a valid configuration!".format(config_tag))
    c = MT2Config_defs[config_tag]
    if "triggers" not in c:
        return "1"
    if trig_reg not in c["triggers"]:
        raise ValueError("'{0}' not a valid trigger set for configuration '{1}'".format(trig_reg, config_tag))
    s = "( "
    for i,trig in enumerate(c["triggers"][trig_reg]):
        if i==0: s += "HLT_"+trig
        else:    s += " || HLT_"+trig
    s += " )"
    return s

## This will be run if you call "python configurations.py"
## Generates a corresponding .cc file that can return
## a c++ object containing the configuration
if __name__=="__main__":
    
    allfilters =[ "eeBadScFilter",
                  "globalTightHalo2016Filter",
                  "globalSuperTightHalo2016Filter",
                  "goodVertices",
                  "HBHENoiseFilter",
                  "HBHENoiseIsoFilter",
                  "EcalDeadCellTriggerPrimitiveFilter",
                  "ecalBadCalibFilter",
                  "ecalBadCalibFilterUpdate",
                  "badMuonFilter",
                  "badChargedCandidateFilter",
                  "badMuonFilterV2",
                  "badChargedHadronFilterV2",
                  ]


    c = MT2Config_defs
    
    fout = open("configurations.cc",'w')
    
    fout.write("""
/*********************************************************************

[Auto-generated with configurations.py]

If you want to edit a configuration, update configurations.py and run:
python configurations.py

*********************************************************************/

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <exception>

#include "configurations.h"


MT2Configuration GetMT2Config(std::string tag){

    MT2Configuration c;

    c.year = -1;
    c.cmssw_ver = -1;
    c.json = "";
    c.lumi = 1.0;
    c.pu_weights_file = "";
    c.btagcalib_csv_fastsim = "";
    c.btageff_file_fastsim = "";
    c.elSF_IDISOfile = "";
    c.elSF_TRKfile = "";
    c.elSF_TRKfileLowPt = "";
    c.elSF_IDhistName = "";
    c.elSF_ISOhistName = "";
    c.muSF_IDfile = "";
    c.muSF_ISOfile = "";
    c.muSF_IPfile = "";
    c.muSF_TRKfile = "";
    c.muSF_IDhistName = "";
    c.muSF_ISOhistName = "";
    c.muSF_IPhistName = "";
    c.muSF_TRKLT10histName = "";
    c.muSF_TRKGT10histName = "";
    c.elSF_IDISOfile_fastsim = "";
    c.elSF_IDhistName_fastsim = "";
    c.elSF_ISOhistName_fastsim = "";
    c.muSF_IDfile_fastsim = "";
    c.muSF_ISOfile_fastsim = "";
    c.muSF_IPfile_fastsim = "";
    c.muSF_IDhistName_fastsim = "";
    c.muSF_ISOhistName_fastsim = "";
    c.muSF_IPhistName_fastsim = "";
    c.dilep_trigeff_file = "";
    c.triggers = std::map<std::string, std::vector<std::string> > ();
""")

    for filter in allfilters:
        fout.write("    c.filters[\"{0}\"] = false;\n".format(filter))

    for i,tag in enumerate(sorted(c.keys())):
        fout.write("\n    {0}(tag == \"{1}\"){{\n\n".format("if" if i==0 else "}else if", tag))
        if "year" in c[tag]:
            fout.write("        c.year               = {0};\n".format(c[tag]["year"]))
        if "cmssw_ver" in c[tag]:
            fout.write("        c.cmssw_ver          = {0};\n".format(c[tag]["cmssw_ver"]))
        if "json" in c[tag]:
            fout.write("        c.json               = \"{0}\";\n".format(c[tag]["json"]))
        if "lumi" in c[tag]:
            fout.write("        c.lumi               = {0};\n".format(c[tag]["lumi"]))
        fout.write("        c.btagcalib_csv      = \"{0}\";\n".format(c[tag]["btagcalib_csv"]))
        fout.write("        c.btageff_file       = \"{0}\";\n".format(c[tag]["btageff_file"]))
        if "btagcalib_csv_fastsim" in c[tag]:
            fout.write("        c.btagcalib_csv_fastsim      = \"{0}\";\n".format(c[tag]["btagcalib_csv_fastsim"]))
            fout.write("        c.btageff_file_fastsim       = \"{0}\";\n".format(c[tag]["btageff_file_fastsim"]))
        fout.write("        c.btag_med_threshold_DeepCSV = {0};\n".format(c[tag]["btag_med_threshold_DeepCSV"]))
        fout.write("        c.btag_med_threshold_CSVv2   = {0};\n".format(c[tag]["btag_med_threshold_CSVv2"]))
        if "pu_weights_file" in c[tag]:
            fout.write("        c.pu_weights_file    = \"{0}\";\n".format(c[tag]["pu_weights_file"])) 
        if "elSF_IDISOfile" in c[tag]:
            fout.write("        c.elSF_IDISOfile     = \"{0}\";\n".format(c[tag]["elSF_IDISOfile"]))
            fout.write("        c.elSF_TRKfile       = \"{0}\";\n".format(c[tag]["elSF_TRKfile"]))
            if "elSF_TRKfileLowPt" in c[tag]:
                fout.write("        c.elSF_TRKfileLowPt  = \"{0}\";\n".format(c[tag]["elSF_TRKfileLowPt"]))
            else:
                fout.write("        c.elSF_TRKfileLowPt  = \"{0}\";\n".format(c[tag]["elSF_TRKfile"]))                
            fout.write("        c.elSF_IDhistName    = \"{0}\";\n".format(c[tag]["elSF_IDhistName"]))
            fout.write("        c.elSF_ISOhistName   = \"{0}\";\n".format(c[tag]["elSF_ISOhistName"]))
        if "muSF_IDfile" in c[tag]:
            fout.write("        c.muSF_IDfile        = \"{0}\";\n".format(c[tag]["muSF_IDfile"]))
            fout.write("        c.muSF_ISOfile       = \"{0}\";\n".format(c[tag]["muSF_ISOfile"]))
            fout.write("        c.muSF_IPfile        = \"{0}\";\n".format(c[tag]["muSF_IPfile"]))
            fout.write("        c.muSF_TRKfile       = \"{0}\";\n".format(c[tag]["muSF_TRKfile"]))
            fout.write("        c.muSF_IDhistName    = \"{0}\";\n".format(c[tag]["muSF_IDhistName"]))
            fout.write("        c.muSF_ISOhistName   = \"{0}\";\n".format(c[tag]["muSF_ISOhistName"]))
            fout.write("        c.muSF_IPhistName    = \"{0}\";\n".format(c[tag]["muSF_IPhistName"]))
            fout.write("        c.muSF_TRKLT10histName = \"{0}\";\n".format(c[tag]["muSF_TRKLT10histName"]))
            fout.write("        c.muSF_TRKGT10histName = \"{0}\";\n".format(c[tag]["muSF_TRKGT10histName"]))
        if "elSF_IDISOfile_fastsim" in c[tag]:
            fout.write("        c.elSF_IDISOfile_fastsim    = \"{0}\";\n".format(c[tag]["elSF_IDISOfile_fastsim"]))
            fout.write("        c.elSF_IDhistName_fastsim   = \"{0}\";\n".format(c[tag]["elSF_IDhistName_fastsim"]))
            fout.write("        c.elSF_ISOhistName_fastsim  = \"{0}\";\n".format(c[tag]["elSF_ISOhistName_fastsim"]))
            fout.write("        c.muSF_IDfile_fastsim       = \"{0}\";\n".format(c[tag]["muSF_IDfile_fastsim"]))
            fout.write("        c.muSF_ISOfile_fastsim      = \"{0}\";\n".format(c[tag]["muSF_ISOfile_fastsim"]))
            fout.write("        c.muSF_IPfile_fastsim       = \"{0}\";\n".format(c[tag]["muSF_IPfile_fastsim"]))
            fout.write("        c.muSF_IDhistName_fastsim   = \"{0}\";\n".format(c[tag]["muSF_IDhistName_fastsim"]))
            fout.write("        c.muSF_ISOhistName_fastsim  = \"{0}\";\n".format(c[tag]["muSF_ISOhistName_fastsim"]))
            fout.write("        c.muSF_IPhistName_fastsim   = \"{0}\";\n".format(c[tag]["muSF_IPhistName_fastsim"]))
        if "dilep_trigeff_file" in c[tag]:
            fout.write("        c.dilep_trigeff_file = \"{0}\";\n".format(c[tag]["dilep_trigeff_file"]))
        fout.write("        c.ea_version         = {0};\n".format(c[tag]["ea_version"]))
        fout.write("        c.jet_id             = \"{0}\";\n".format(c[tag]["jet_id"]))
        fout.write("        c.JRT_file           = \"{0}\";\n".format(c[tag]["JRT_file"]))
        fout.write("        c.rphi_tag           = \"{0}\";\n".format(c[tag]["rphi_tag"]))
        for jec in c[tag]["JECs"]:
            fout.write("        c.JECs.push_back(std::pair<std::string, std::string> (\"{0}\", \"{1}\"));\n".format(jec[0], jec[1]))
        for filter in c[tag]["filters"]:            
            if filter not in allfilters:
                raise Exception("ERROR: filter {0} not in list of all filters!".format(filter))
            fout.write("        c.filters[\"{0}\"] = true;\n".format(filter))
        if "triggers" in c[tag]:
            for reg in c[tag]["triggers"]:
                fout.write("        c.triggers[\"{0}\"] = std::vector<std::string> ();\n".format(reg))
                for trig in c[tag]["triggers"][reg]:
                    fout.write("        c.triggers[\"{0}\"].push_back(\"{1}\");\n".format(reg, trig))

    fout.write("""
    }else{
        std::cout << "[MT2Configuration] ERROR! invalid tag \\""+tag+"\\"" << std::endl;
        throw std::exception();
    }

    return c;

}
""")

    fout.write("""
void fillTriggerVector(const mt2tree& t, std::vector<const Int_t*>& trigs, std::vector<std::string>& trig_names) {

    trigs.clear();

    for(uint i=0; i<trig_names.size(); i++){
        std::string s = trig_names.at(i);
""")

    unique_trigs = set()
    for conf in c:
        if "triggers" not in c[conf]:
            continue
        for cat in c[conf]["triggers"]:
            unique_trigs |= set(c[conf]["triggers"][cat])

    unique_trigs = sorted(unique_trigs)
    maxlen = 0
    for trig in unique_trigs:
        maxlen = max(maxlen, len(trig))
    fout.write("        if     (s==\"{0}\")".format(unique_trigs[0]))
    fout.write(" "*(maxlen-len(unique_trigs[0])+1))
    fout.write("trigs.push_back(&t.HLT_{0});\n".format(unique_trigs[0]))
    for trig in unique_trigs[1:]:
        fout.write("        else if(s==\"{0}\")".format(trig))
        fout.write(" "*(maxlen-len(trig)+1))
        fout.write("trigs.push_back(&t.HLT_{0});\n".format(trig))

    fout.write("        else\n")
    fout.write("            std::cout << \"[MT2Looper::fillTriggerVectors] WARNING: unknown trigger \" << s << \"! Not applying.\" << std::endl;\n")

    fout.write("""
    }
}

// perform an "OR" of all triggers stored in "trigs" vector 
// this vector is just a list of pointers to ints (t.HLT_*)
bool passTrigger(const mt2tree& t, std::vector<const Int_t*> &trigs, bool debug) {
 
    if(!t.isData) return true; 

    if(debug){
        for(uint i=0; i<trigs.size(); i++){
            std::cout << *trigs.at(i) << " ";
        }
        std::cout << std::endl;
    }
  
    for(uint i=0; i<trigs.size(); i++){
        if(*trigs.at(i))
            return true;
    }
  
    return false;
  
}
""")
