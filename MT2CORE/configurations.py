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
            "HLT_PFHT1050",
            "HLT_PFHT800_PFMET75_PFMHT75",
            "HLT_PFHT500_PFMET100_PFMHT100",
            "HLT_PFMET120_PFMHT120",
            "HLT_PFMET120_PFMHT120_PFHT60",
            "HLT_PFMETNoMu120_PFMHTNoMu120"
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
    }

MT2Config_defs["mc_94x_Fall17"] = {
    "lumi" : 41.37,
    "btagcalib_csv" : "CSVv2_94XSF_V2_B_F.csv",
    "btag_med_threshold" : 0.8838,
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
                  "badChargedCandidateFilter"
                  "badMuonFilterV2",
                  "badChargedHadronFilterV2"
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
        for jec in c[tag]["JECs"]:
            fout.write("        c.JECs.push_back(std::pair<std::string, std::string> (\"{0}\", \"{1}\"));\n".format(jec[0], jec[1]))
        for filter in c[tag]["filters"]:
            fout.write("        c.filters[\"{0}\"] = true;\n".format(filter))

    fout.write("""
    }else{
        std::cout << "[MT2Configuration] ERROR! invalid tag \\""+tag+"\\"" << std::endl;
        throw std::exception();
    }

    return c;

}
""")

