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
                  "badChargedCandidateFilter"
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
    "json" : "Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON_snt.txt",
    "lumi" : 58.83,
    "btagcalib_csv" : "DeepCSV_94XSF_V3_B_F.csv",
    "btageff_file" : "btageff__DeepCSV_ttbar_powheg_pythia8_25ns_Fall17.root",
    "btag_med_threshold_DeepCSV" : 0.4184,
    "btag_med_threshold_CSVv2" : 0.8838,
    "ea_version" : 3,
    "jet_id" : "2017_v1",
    "JRT_file" : "JetResponseTemplates_ptBinned_102x_JetID_PUID_BTagSFs_core2sigma.root",
    "JECs" : [ ["", "Fall17_17Nov2017C_V32_DATA"]
               ],
    "filters"  : MT2Config_defs["data_2017_31Mar2018"]["filters"],
    "triggers" : MT2Config_defs["data_2017_31Mar2018"]["triggers"]
    }

MT2Config_defs["data_2018_17Sep2018"] = {
    "year" : 2018,
    "cmssw_ver" : 102,
    "json" : "Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON_snt.txt",
    "lumi" : 58.83,
    "btagcalib_csv" : "DeepCSV_94XSF_V3_B_F.csv",
    "btageff_file" : "btageff__DeepCSV_ttbar_powheg_pythia8_25ns_Fall17.root",
    "btag_med_threshold_DeepCSV" : 0.4184,
    "btag_med_threshold_CSVv2" : 0.8838,
    "ea_version" : 3,
    "jet_id" : "2017_v1",
    "JRT_file" : "JetResponseTemplates_ptBinned_102x_JetID_PUID_BTagSFs_core2sigma.root",
    "JECs" : [ ["", "Fall17_17Nov2017C_V32_DATA"]
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
    "badChargedCandidateFilter"
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
    "elSF_file" : "lepsf/FullRunII/ElectronScaleFactors_Run2017.root",
    "elSFtrk_file" : "lepsf/FullRunII/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root",
    "elSF_histName" : "Run2017_CutBasedVetoNoIso94XV2",
    "elSF_isoHistName" : "Run2017_MVAVLooseTightIP2DMini",
    "ea_version" : 3,
    "jet_id" : "2017_v1",
    "JRT_file" : "JetResponseTemplates_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma.root",
    "JECs" : [ ["", "Fall17_17Nov2017_V32_MC"]
               ],
    "filters" : [ "globalSuperTightHalo2016Filter",
                  "goodVertices",
                  "HBHENoiseFilter",
                  "HBHENoiseIsoFilter",
                  "EcalDeadCellTriggerPrimitiveFilter",
                  "ecalBadCalibFilterUpdate",
                  "badMuonFilter",
                  "badChargedCandidateFilter"
                  ],
}

MT2Config_defs["mc_102x_Autumn18"] = copy.deepcopy(MT2Config_defs["mc_94x_Fall17"])
MT2Config_defs["mc_102x_Autumn18"]["year"] = 2018
MT2Config_defs["mc_102x_Autumn18"]["cmssw_ver"] = 102
MT2Config_defs["mc_102x_Autumn18"]["lumi"] = 58.83
MT2Config_defs["mc_102x_Autumn18"]["btag_med_threshold_DeepCSV"] = 0.4184
MT2Config_defs["mc_102x_Autumn18"]["pu_weights_file"] = "puWeight2018.root"
MT2Config_defs["mc_102x_Autumn18"]["JRT_file"] = "JetResponseTemplates_ptBinned_102x_JetID_PUID_BTagSFs_core2sigma.root"


MT2Config_defs["mc_80x_Summer16"] = {
    "year" : 2016,
    "cmssw_ver" : 80,
    "lumi" : 35.922,
    "btagcalib_csv" : "DeepCSV_Moriond17_B_H.csv",
    "btageff_file" : "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Summer16_94x.root",
    "btag_med_threshold_DeepCSV" : 0.6324,
    "btag_med_threshold_CSVv2" : 0.8484,
    "pu_weights_file" : "puWeight2016.root",
    "elSF_file" : "lepsf/moriond17/scaleFactors_el_moriond_2017.root",
    "elSFtrk_file" : "lepsf/moriond17/egammaEffi.txt_EGM2D.root",
    "elSF_histName" : "GsfElectronToCutBasedSpring15V",
    "elSF_isoHistName" : "MVAVLooseElectronToMini",
    "ea_version" : 1,
    "jet_id" : "50nsV1",
    "JRT_file" : "JetResponseTemplates_ptBinned_80x_JetID_PUID_BTagSFs_core2sigma.root",
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
    "btagcalib_csv" : "DeepCSV_Moriond17_B_H.csv",
    "btageff_file" : "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Summer16_94x.root",
    "btag_med_threshold_DeepCSV" : 0.6321,
    "btag_med_threshold_CSVv2" : 0.8484,
    "pu_weights_file" : "puWeight2016.root",
    "elSF_file" : "lepsf/moriond17/scaleFactors_el_moriond_2017.root",
    "elSFtrk_file" : "lepsf/moriond17/egammaEffi.txt_EGM2D.root",
    "elSF_histName" : "GsfElectronToCutBasedSpring15V",
    "elSF_isoHistName" : "MVAVLooseElectronToMini",
    "ea_version" : 1,
    "jet_id" : "50nsV1",
    "JRT_file" : "JetResponseTemplates_ptBinned_80x_JetID_PUID_BTagSFs_core2sigma.root",
    "JECs" : [ ["", "Summer16_07Aug2017_V11_MC"]
               ],
    "filters" : [ "eeBadScFilter",
                  "globalSuperTightHalo2016Filter",
                  "goodVertices",
                  "HBHENoiseFilter",
                  "HBHENoiseIsoFilter",
                  "EcalDeadCellTriggerPrimitiveFilter",
                  "badMuonFilter",
                  "badChargedCandidateFilter"
                  ],
}

MT2Config_defs["mc_80x_fastsim_Moriond17"] = copy.deepcopy(MT2Config_defs["mc_80x_Summer16"])
MT2Config_defs["mc_80x_fastsim_Moriond17"]["JECs"] = [ ["", "Spring16_FastSimV1_MC"] ]
MT2Config_defs["mc_80x_fastsim_Moriond17"]["filters"] = [
    "goodVertices",
    "HBHENoiseFilter",
    "HBHENoiseIsoFilter",
    "EcalDeadCellTriggerPrimitiveFilter",
    "badMuonFilterV2",
    "badChargedHadronFilterV2"
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
    c.elSF_file = "";
    c.elSFtrk_file = "";
    c.elSF_histName = "";
    c.elSF_isoHistName = "";
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
        fout.write("        c.btag_med_threshold_DeepCSV = {0};\n".format(c[tag]["btag_med_threshold_DeepCSV"]))
        fout.write("        c.btag_med_threshold_CSVv2   = {0};\n".format(c[tag]["btag_med_threshold_CSVv2"]))
        if "pu_weights_file" in c[tag]:
            fout.write("        c.pu_weights_file    = \"{0}\";\n".format(c[tag]["pu_weights_file"])) 
        if "elSF_file" in c[tag]:
            fout.write("        c.elSF_file          = \"{0}\";\n".format(c[tag]["elSF_file"]))
            fout.write("        c.elSFtrk_file       = \"{0}\";\n".format(c[tag]["elSFtrk_file"]))
            fout.write("        c.elSF_histName      = \"{0}\";\n".format(c[tag]["elSF_histName"]))
            fout.write("        c.elSF_isoHistName   = \"{0}\";\n".format(c[tag]["elSF_isoHistName"]))
        fout.write("        c.ea_version         = {0};\n".format(c[tag]["ea_version"]))
        fout.write("        c.jet_id             = \"{0}\";\n".format(c[tag]["jet_id"]))
        fout.write("        c.JRT_file           = \"{0}\";\n".format(c[tag]["JRT_file"]))
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
