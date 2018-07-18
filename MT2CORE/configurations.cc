
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
    c.filters["eeBadScFilter"] = false;
    c.filters["globalTightHalo2016Filter"] = false;
    c.filters["globalSuperTightHalo2016Filter"] = false;
    c.filters["goodVertices"] = false;
    c.filters["HBHENoiseFilter"] = false;
    c.filters["HBHENoiseIsoFilter"] = false;
    c.filters["EcalDeadCellTriggerPrimitiveFilter"] = false;
    c.filters["ecalBadCalibFilter"] = false;
    c.filters["badMuonFilter"] = false;
    c.filters["badChargedCandidateFilterbadMuonFilterV2"] = false;
    c.filters["badChargedHadronFilterV2"] = false;

    if(tag == "data_2016_Moriond17"){

        c.json               = "Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON_snt.txt";
        c.lumi               = 35.92;
        c.btagcalib_csv      = "CSVv2_Moriond17_B_H.csv";
        c.btag_med_threshold = 0.8484;
        c.JECs.push_back(std::pair<std::string, std::string> ("2016B", "Summer16_23Sep2016BCDV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016C", "Summer16_23Sep2016BCDV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016D", "Summer16_23Sep2016BCDV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016E", "Summer16_23Sep2016EFV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016F", "Summer16_23Sep2016EFV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016G", "Summer16_23Sep2016GV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016H", "Summer16_23Sep2016HV4_DATA"));
        c.filters["eeBadScFilter"] = true;
        c.filters["globalTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["badMuonFilterV2"] = true;
        c.filters["badChargedHadronFilterV2"] = true;

    }else if(tag == "data_2017_31Mar2018"){

        c.json               = "Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_v1_snt.txt";
        c.lumi               = 41.37;
        c.btagcalib_csv      = "CSVv2_94XSF_V2_B_F.csv";
        c.btag_med_threshold = 0.8838;
        c.JECs.push_back(std::pair<std::string, std::string> ("2017B", "Fall17_17Nov2017B_V6_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2017C", "Fall17_17Nov2017C_V6_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2017D", "Fall17_17Nov2017D_V6_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2017E", "Fall17_17Nov2017E_V6_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2017F", "Fall17_17Nov2017F_V6_DATA"));
        c.filters["eeBadScFilter"] = true;
        c.filters["globalTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["ecalBadCalibFilter"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;

    }else if(tag == "data_2017_Prompt"){

        c.json               = "Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON_snt.txt";
        c.lumi               = 41.97;
        c.btagcalib_csv      = "CSVv2_94XSF_V2_B_F.csv";
        c.btag_med_threshold = 0.8838;
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Summer16_23Sep2016HV4_DATA"));
        c.filters["eeBadScFilter"] = true;
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;

    }else if(tag == "data_2018_Prompt"){

        c.json               = "Cert_314472-317696_13TeV_PromptReco_Collisions18_JSON_snt.txt";
        c.lumi               = 19.26;
        c.btagcalib_csv      = "CSVv2_94XSF_V2_B_F.csv";
        c.btag_med_threshold = 0.8838;
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Fall17_17Nov2017C_V6_DATA"));
        c.filters["eeBadScFilter"] = true;
        c.filters["globalTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["ecalBadCalibFilter"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;

    }else if(tag == "mc_80x_Moriond17"){

        c.lumi               = 35.92;
        c.btagcalib_csv      = "CSVv2_Moriond17_B_H.csv";
        c.btag_med_threshold = 0.8484;
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Summer16_23Sep2016V4_MC"));
        c.filters["globalTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["badMuonFilterV2"] = true;
        c.filters["badChargedHadronFilterV2"] = true;

    }else if(tag == "mc_94x_Fall17"){

        c.lumi               = 41.37;
        c.btagcalib_csv      = "CSVv2_94XSF_V2_B_F.csv";
        c.btag_med_threshold = 0.8838;
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Fall17_17Nov2017_V4_MC"));
        c.filters["globalTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["ecalBadCalibFilter"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;

    }else{
        std::cout << "[MT2Configuration] ERROR! invalid tag \""+tag+"\"" << std::endl;
        throw std::exception();
    }

    return c;

}
