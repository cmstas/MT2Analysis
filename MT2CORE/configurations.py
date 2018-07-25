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

MT2Config_defs = {}

MT2Config_defs["data_2017_31Mar2018"] = {
    "json" : "Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_v1_snt.txt",
    "lumi" : 41.37,
    "btagcalib_csv" : "CSVv2_94XSF_V2_B_F.csv",
    "btag_med_threshold" : 0.8838,
    "ea_version" : 3,
    "JECs" : [ ["2017B", "Fall17_17Nov2017B_V6_DATA"],
               ["2017C", "Fall17_17Nov2017C_V6_DATA"],
               ["2017D", "Fall17_17Nov2017D_V6_DATA"],
               ["2017E", "Fall17_17Nov2017E_V6_DATA"],
               ["2017F", "Fall17_17Nov2017F_V6_DATA"]
               ],
    "filters" : [ "eeBadScFilter",
                  "globalTightHalo2016Filter",
                  "goodVertices",
                  "HBHENoiseFilter",
                  "HBHENoiseIsoFilter",
                  "EcalDeadCellTriggerPrimitiveFilter",
                  "ecalBadCalibFilter",
                  "badMuonFilter",
                  "badChargedCandidateFilter"
                  ],
    "triggers" : {
        "SR" : [ 
            "PFHT1050",
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
            "Mu30_Ele30_NonIso",
            "Mu33_Ele33_NonIso",
            "Mu37_Ele27_NonIso",
            "Mu27_Ele37_NonIso",
            "Photon200",
            "SingleMu_NonIso"
            ]
        }                              
    }


MT2Config_defs["data_2017_Prompt"] = {
    "json" : "Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON_snt.txt",
    "lumi" : 41.97,
    "btagcalib_csv" : "CSVv2_94XSF_V2_B_F.csv",
    "btag_med_threshold" : 0.8838,
    "ea_version" : 3,
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
    "json" : "Cert_314472-317696_13TeV_PromptReco_Collisions18_JSON_snt.txt",
    "lumi" : 19.26,
    "btagcalib_csv" : "CSVv2_94XSF_V2_B_F.csv",
    "btag_med_threshold" : 0.8838,
    "ea_version" : 3,
    "JECs" : [ ["", "Fall17_17Nov2017C_V6_DATA"]
               ],
    "filters"  : MT2Config_defs["data_2017_31Mar2018"]["filters"],
    "triggers" : MT2Config_defs["data_2017_31Mar2018"]["triggers"]
    }

MT2Config_defs["data_2016_Moriond17"] = {
    "json" : "Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON_snt.txt",
    "lumi" : 35.92,
    "btagcalib_csv" : "CSVv2_Moriond17_B_H.csv",
    "btag_med_threshold" : 0.8484,
    "ea_version" : 1,
    "JECs" : [ ["2016B", "Summer16_23Sep2016BCDV4_DATA"],
               ["2016C", "Summer16_23Sep2016BCDV4_DATA"],
               ["2016D", "Summer16_23Sep2016BCDV4_DATA"],
               ["2016E", "Summer16_23Sep2016EFV4_DATA"],
               ["2016F", "Summer16_23Sep2016EFV4_DATA"],
               ["2016G", "Summer16_23Sep2016GV4_DATA"],
               ["2016H", "Summer16_23Sep2016HV4_DATA"]
               ],
    "filters" : [ "eeBadScFilter",
                  "globalTightHalo2016Filter",
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
            ]
        }
    }

MT2Config_defs["mc_94x_Fall17"] = {
    "lumi" : 41.37,
    "btagcalib_csv" : "CSVv2_94XSF_V2_B_F.csv",
    "btag_med_threshold" : 0.8838,
    "ea_version" : 3,
    "JECs" : [ ["", "Fall17_17Nov2017_V4_MC"]
               ],
    "filters" : [ "globalTightHalo2016Filter",
                  "goodVertices",
                  "HBHENoiseFilter",
                  "HBHENoiseIsoFilter",
                  "EcalDeadCellTriggerPrimitiveFilter",
                  "ecalBadCalibFilter",
                  "badMuonFilter",
                  "badChargedCandidateFilter"
                  ],
}

MT2Config_defs["mc_80x_Moriond17"] = {
    "lumi" : 35.92,
    "btagcalib_csv" : "CSVv2_Moriond17_B_H.csv",
    "btag_med_threshold" : 0.8484,
    "ea_version" : 1,
    "JECs" : [ ["", "Summer16_23Sep2016V4_MC"]
               ],
    "filters" : [ "globalTightHalo2016Filter",
                  "goodVertices",
                  "HBHENoiseFilter",
                  "HBHENoiseIsoFilter",
                  "EcalDeadCellTriggerPrimitiveFilter",
                  "badMuonFilterV2",
                  "badChargedHadronFilterV2"
                  ],
}




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

    c.json = "";
    c.lumi = 1.0;
    c.triggers = std::map<std::string, std::vector<std::string> > ();
""")

    for filter in allfilters:
        fout.write("    c.filters[\"{0}\"] = false;\n".format(filter))

    for i,tag in enumerate(sorted(c.keys())):
        fout.write("\n    {0}(tag == \"{1}\"){{\n\n".format("if" if i==0 else "}else if", tag))
        if "json" in c[tag]:
            fout.write("        c.json               = \"{0}\";\n".format(c[tag]["json"]))
        if "lumi" in c[tag]:
            fout.write("        c.lumi               = {0};\n".format(c[tag]["lumi"]))
        fout.write("        c.btagcalib_csv      = \"{0}\";\n".format(c[tag]["btagcalib_csv"]))
        fout.write("        c.btag_med_threshold = {0};\n".format(c[tag]["btag_med_threshold"]))
        fout.write("        c.ea_version         = {0};\n".format(c[tag]["ea_version"]))
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

