
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
    c.pu_weights_file = "";
    c.triggers = std::map<std::string, std::vector<std::string> > ();
    c.filters["eeBadScFilter"] = false;
    c.filters["globalTightHalo2016Filter"] = false;
    c.filters["globalSuperTightHalo2016Filter"] = false;
    c.filters["goodVertices"] = false;
    c.filters["HBHENoiseFilter"] = false;
    c.filters["HBHENoiseIsoFilter"] = false;
    c.filters["EcalDeadCellTriggerPrimitiveFilter"] = false;
    c.filters["ecalBadCalibFilter"] = false;
    c.filters["badMuonFilter"] = false;
    c.filters["badChargedCandidateFilter"] = false;
    c.filters["badMuonFilterV2"] = false;
    c.filters["badChargedHadronFilterV2"] = false;

    if(tag == "data_2016_94x"){

        c.json               = "Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON_snt.txt";
        c.lumi               = 35.92;
        c.btagcalib_csv      = "CSVv2_Moriond17_B_H.csv";
        c.btageff_file       = "btageff__ttbar_powheg_pythia8_25ns_Moriond17.root";
        c.btag_med_threshold = 0.8484;
        c.ea_version         = 1;
        c.jet_id             = "50nsV1";
        c.JECs.push_back(std::pair<std::string, std::string> ("2016B", "Summer16_23Sep2016BCDV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016C", "Summer16_23Sep2016BCDV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016D", "Summer16_23Sep2016BCDV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016E", "Summer16_23Sep2016EFV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016F", "Summer16_23Sep2016EFV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016G", "Summer16_23Sep2016GV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016H", "Summer16_23Sep2016HV4_DATA"));
        c.filters["eeBadScFilter"] = true;
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;
        c.triggers["SR"] = std::vector<std::string> ();
        c.triggers["SR"].push_back("PFHT900");
        c.triggers["SR"].push_back("PFJet450");
        c.triggers["SR"].push_back("PFHT300_PFMET110");
        c.triggers["SR"].push_back("PFMET120_PFMHT120");
        c.triggers["SR"].push_back("PFMETNoMu120_PFMHTNoMu120");
        c.triggers["Photon"] = std::vector<std::string> ();
        c.triggers["Photon"].push_back("Photon165_HE10");
        c.triggers["DilepSF"] = std::vector<std::string> ();
        c.triggers["DilepSF"].push_back("DoubleEl");
        c.triggers["DilepSF"].push_back("DoubleMu");
        c.triggers["DilepSF"].push_back("Photon165_HE10");
        c.triggers["DilepSF"].push_back("DoubleMu_NonIso");
        c.triggers["DilepSF"].push_back("SingleMu_NonIso");
        c.triggers["DilepSF"].push_back("DoubleEl33");
        c.triggers["SingleMu"] = std::vector<std::string> ();
        c.triggers["SingleMu"].push_back("SingleMu");
        c.triggers["SingleMu"].push_back("SingleMu_NonIso");
        c.triggers["SingleEl"] = std::vector<std::string> ();
        c.triggers["SingleEl"].push_back("SingleEl");
        c.triggers["SingleEl"].push_back("SingleEl_NonIso");
        c.triggers["prescaledHT"] = std::vector<std::string> ();
        c.triggers["prescaledHT"].push_back("PFHT125_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT200_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT300_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT350_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT475_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT600_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT900");
        c.triggers["DilepOF"] = std::vector<std::string> ();
        c.triggers["DilepOF"].push_back("MuX_Ele12");
        c.triggers["DilepOF"].push_back("Mu8_EleX");
        c.triggers["DilepOF"].push_back("Mu30_Ele30_NonIso");
        c.triggers["DilepOF"].push_back("Mu33_Ele33_NonIso");
        c.triggers["DilepOF"].push_back("Photon165_HE10");
        c.triggers["DilepOF"].push_back("SingleMu_NonIso");

    }else if(tag == "data_2016_Moriond17"){

        c.json               = "Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON_snt.txt";
        c.lumi               = 35.92;
        c.btagcalib_csv      = "CSVv2_Moriond17_B_H.csv";
        c.btageff_file       = "btageff__ttbar_powheg_pythia8_25ns_Moriond17.root";
        c.btag_med_threshold = 0.8484;
        c.ea_version         = 1;
        c.jet_id             = "50nsV1";
        c.JECs.push_back(std::pair<std::string, std::string> ("2016B", "Summer16_23Sep2016BCDV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016C", "Summer16_23Sep2016BCDV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016D", "Summer16_23Sep2016BCDV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016E", "Summer16_23Sep2016EFV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016F", "Summer16_23Sep2016EFV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016G", "Summer16_23Sep2016GV4_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016H", "Summer16_23Sep2016HV4_DATA"));
        c.filters["eeBadScFilter"] = true;
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["badMuonFilterV2"] = true;
        c.filters["badChargedHadronFilterV2"] = true;
        c.triggers["SR"] = std::vector<std::string> ();
        c.triggers["SR"].push_back("PFHT900");
        c.triggers["SR"].push_back("PFJet450");
        c.triggers["SR"].push_back("PFHT300_PFMET110");
        c.triggers["SR"].push_back("PFMET120_PFMHT120");
        c.triggers["SR"].push_back("PFMETNoMu120_PFMHTNoMu120");
        c.triggers["Photon"] = std::vector<std::string> ();
        c.triggers["Photon"].push_back("Photon165_HE10");
        c.triggers["DilepSF"] = std::vector<std::string> ();
        c.triggers["DilepSF"].push_back("DoubleEl");
        c.triggers["DilepSF"].push_back("DoubleMu");
        c.triggers["DilepSF"].push_back("Photon165_HE10");
        c.triggers["DilepSF"].push_back("DoubleMu_NonIso");
        c.triggers["DilepSF"].push_back("SingleMu_NonIso");
        c.triggers["DilepSF"].push_back("DoubleEl33");
        c.triggers["SingleMu"] = std::vector<std::string> ();
        c.triggers["SingleMu"].push_back("SingleMu");
        c.triggers["SingleMu"].push_back("SingleMu_NonIso");
        c.triggers["SingleEl"] = std::vector<std::string> ();
        c.triggers["SingleEl"].push_back("SingleEl");
        c.triggers["SingleEl"].push_back("SingleEl_NonIso");
        c.triggers["prescaledHT"] = std::vector<std::string> ();
        c.triggers["prescaledHT"].push_back("PFHT125_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT200_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT300_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT350_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT475_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT600_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT900");
        c.triggers["DilepOF"] = std::vector<std::string> ();
        c.triggers["DilepOF"].push_back("MuX_Ele12");
        c.triggers["DilepOF"].push_back("Mu8_EleX");
        c.triggers["DilepOF"].push_back("Mu30_Ele30_NonIso");
        c.triggers["DilepOF"].push_back("Mu33_Ele33_NonIso");
        c.triggers["DilepOF"].push_back("Photon165_HE10");
        c.triggers["DilepOF"].push_back("SingleMu_NonIso");

    }else if(tag == "data_2017_31Mar2018"){

        c.json               = "Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_v1_snt.txt";
        c.lumi               = 41.37;
        c.btagcalib_csv      = "CSVv2_94XSF_V2_B_F.csv";
        c.btageff_file       = "btageff__ttbar_powheg_pythia8_25ns_Fall17.root";
        c.btag_med_threshold = 0.8838;
        c.ea_version         = 3;
        c.jet_id             = "2017_v1";
        c.JECs.push_back(std::pair<std::string, std::string> ("2017B", "Fall17_17Nov2017B_V6_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2017C", "Fall17_17Nov2017C_V6_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2017D", "Fall17_17Nov2017D_V6_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2017E", "Fall17_17Nov2017E_V6_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2017F", "Fall17_17Nov2017F_V6_DATA"));
        c.filters["eeBadScFilter"] = true;
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["ecalBadCalibFilter"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;
        c.triggers["SR"] = std::vector<std::string> ();
        c.triggers["SR"].push_back("PFHT1050");
        c.triggers["SR"].push_back("PFHT800_PFMET75_PFMHT75");
        c.triggers["SR"].push_back("PFHT500_PFMET100_PFMHT100");
        c.triggers["SR"].push_back("PFMET120_PFMHT120");
        c.triggers["SR"].push_back("PFMET120_PFMHT120_PFHT60");
        c.triggers["SR"].push_back("PFMETNoMu120_PFMHTNoMu120");
        c.triggers["SR"].push_back("PFMETNoMu120_PFMHTNoMu120_PFHT60");
        c.triggers["Photon"] = std::vector<std::string> ();
        c.triggers["Photon"].push_back("Photon200");
        c.triggers["DilepSF"] = std::vector<std::string> ();
        c.triggers["DilepSF"].push_back("DoubleEl");
        c.triggers["DilepSF"].push_back("DoubleMu");
        c.triggers["DilepSF"].push_back("Photon200");
        c.triggers["DilepSF"].push_back("DoubleMu_NonIso");
        c.triggers["DilepSF"].push_back("SingleMu_NonIso");
        c.triggers["DilepSF"].push_back("DoubleEl33");
        c.triggers["SingleMu"] = std::vector<std::string> ();
        c.triggers["SingleMu"].push_back("SingleMu");
        c.triggers["SingleMu"].push_back("SingleMu_NonIso");
        c.triggers["SingleEl"] = std::vector<std::string> ();
        c.triggers["SingleEl"].push_back("SingleEl");
        c.triggers["SingleEl"].push_back("SingleEl_NonIso");
        c.triggers["prescaledHT"] = std::vector<std::string> ();
        c.triggers["prescaledHT"].push_back("PFHT180_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT250_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT370_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT430_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT510_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT590_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT680_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT780_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT890_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT1050");
        c.triggers["DilepOF"] = std::vector<std::string> ();
        c.triggers["DilepOF"].push_back("MuX_Ele12");
        c.triggers["DilepOF"].push_back("Mu8_EleX");
        c.triggers["DilepOF"].push_back("Mu12_EleX");
        c.triggers["DilepOF"].push_back("Mu37_Ele27_NonIso");
        c.triggers["DilepOF"].push_back("Mu27_Ele37_NonIso");
        c.triggers["DilepOF"].push_back("Photon200");
        c.triggers["DilepOF"].push_back("SingleMu_NonIso");

    }else if(tag == "data_2017_Prompt"){

        c.json               = "Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON_snt.txt";
        c.lumi               = 41.97;
        c.btagcalib_csv      = "CSVv2_94XSF_V2_B_F.csv";
        c.btageff_file       = "btageff__ttbar_powheg_pythia8_25ns_Fall17.root";
        c.btag_med_threshold = 0.8838;
        c.ea_version         = 3;
        c.jet_id             = "2017_v1";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Summer16_23Sep2016HV4_DATA"));
        c.filters["eeBadScFilter"] = true;
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.triggers["SR"] = std::vector<std::string> ();
        c.triggers["SR"].push_back("PFHT1050");
        c.triggers["SR"].push_back("PFHT800_PFMET75_PFMHT75");
        c.triggers["SR"].push_back("PFHT500_PFMET100_PFMHT100");
        c.triggers["SR"].push_back("PFMET120_PFMHT120");
        c.triggers["SR"].push_back("PFMET120_PFMHT120_PFHT60");
        c.triggers["SR"].push_back("PFMETNoMu120_PFMHTNoMu120");
        c.triggers["SR"].push_back("PFMETNoMu120_PFMHTNoMu120_PFHT60");
        c.triggers["Photon"] = std::vector<std::string> ();
        c.triggers["Photon"].push_back("Photon200");
        c.triggers["DilepSF"] = std::vector<std::string> ();
        c.triggers["DilepSF"].push_back("DoubleEl");
        c.triggers["DilepSF"].push_back("DoubleMu");
        c.triggers["DilepSF"].push_back("Photon200");
        c.triggers["DilepSF"].push_back("DoubleMu_NonIso");
        c.triggers["DilepSF"].push_back("SingleMu_NonIso");
        c.triggers["DilepSF"].push_back("DoubleEl33");
        c.triggers["SingleMu"] = std::vector<std::string> ();
        c.triggers["SingleMu"].push_back("SingleMu");
        c.triggers["SingleMu"].push_back("SingleMu_NonIso");
        c.triggers["SingleEl"] = std::vector<std::string> ();
        c.triggers["SingleEl"].push_back("SingleEl");
        c.triggers["SingleEl"].push_back("SingleEl_NonIso");
        c.triggers["prescaledHT"] = std::vector<std::string> ();
        c.triggers["prescaledHT"].push_back("PFHT180_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT250_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT370_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT430_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT510_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT590_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT680_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT780_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT890_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT1050");
        c.triggers["DilepOF"] = std::vector<std::string> ();
        c.triggers["DilepOF"].push_back("MuX_Ele12");
        c.triggers["DilepOF"].push_back("Mu8_EleX");
        c.triggers["DilepOF"].push_back("Mu12_EleX");
        c.triggers["DilepOF"].push_back("Mu37_Ele27_NonIso");
        c.triggers["DilepOF"].push_back("Mu27_Ele37_NonIso");
        c.triggers["DilepOF"].push_back("Photon200");
        c.triggers["DilepOF"].push_back("SingleMu_NonIso");

    }else if(tag == "data_2018_Prompt"){

        c.json               = "Cert_314472-321221_13TeV_PromptReco_Collisions18_JSON_snt.txt";
        c.lumi               = 19.26;
        c.btagcalib_csv      = "CSVv2_94XSF_V2_B_F.csv";
        c.btageff_file       = "btageff__ttbar_powheg_pythia8_25ns_Fall17.root";
        c.btag_med_threshold = 0.8838;
        c.ea_version         = 3;
        c.jet_id             = "2017_v1";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Fall17_17Nov2017C_V6_DATA"));
        c.filters["eeBadScFilter"] = true;
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["ecalBadCalibFilter"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;
        c.triggers["SR"] = std::vector<std::string> ();
        c.triggers["SR"].push_back("PFHT1050");
        c.triggers["SR"].push_back("PFHT800_PFMET75_PFMHT75");
        c.triggers["SR"].push_back("PFHT500_PFMET100_PFMHT100");
        c.triggers["SR"].push_back("PFMET120_PFMHT120");
        c.triggers["SR"].push_back("PFMET120_PFMHT120_PFHT60");
        c.triggers["SR"].push_back("PFMETNoMu120_PFMHTNoMu120");
        c.triggers["SR"].push_back("PFMETNoMu120_PFMHTNoMu120_PFHT60");
        c.triggers["Photon"] = std::vector<std::string> ();
        c.triggers["Photon"].push_back("Photon200");
        c.triggers["DilepSF"] = std::vector<std::string> ();
        c.triggers["DilepSF"].push_back("DoubleEl");
        c.triggers["DilepSF"].push_back("DoubleMu");
        c.triggers["DilepSF"].push_back("Photon200");
        c.triggers["DilepSF"].push_back("DoubleMu_NonIso");
        c.triggers["DilepSF"].push_back("SingleMu_NonIso");
        c.triggers["DilepSF"].push_back("DoubleEl33");
        c.triggers["SingleMu"] = std::vector<std::string> ();
        c.triggers["SingleMu"].push_back("SingleMu");
        c.triggers["SingleMu"].push_back("SingleMu_NonIso");
        c.triggers["SingleEl"] = std::vector<std::string> ();
        c.triggers["SingleEl"].push_back("SingleEl");
        c.triggers["SingleEl"].push_back("SingleEl_NonIso");
        c.triggers["prescaledHT"] = std::vector<std::string> ();
        c.triggers["prescaledHT"].push_back("PFHT180_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT250_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT370_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT430_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT510_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT590_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT680_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT780_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT890_Prescale");
        c.triggers["prescaledHT"].push_back("PFHT1050");
        c.triggers["DilepOF"] = std::vector<std::string> ();
        c.triggers["DilepOF"].push_back("MuX_Ele12");
        c.triggers["DilepOF"].push_back("Mu8_EleX");
        c.triggers["DilepOF"].push_back("Mu12_EleX");
        c.triggers["DilepOF"].push_back("Mu37_Ele27_NonIso");
        c.triggers["DilepOF"].push_back("Mu27_Ele37_NonIso");
        c.triggers["DilepOF"].push_back("Photon200");
        c.triggers["DilepOF"].push_back("SingleMu_NonIso");

    }else if(tag == "mc_101x_Spring18"){

        c.lumi               = 41.37;
        c.btagcalib_csv      = "CSVv2_94XSF_V2_B_F.csv";
        c.btageff_file       = "btageff__ttbar_powheg_pythia8_25ns_Fall17.root";
        c.btag_med_threshold = 0.8838;
        c.pu_weights_file    = "";
        c.ea_version         = 3;
        c.jet_id             = "2017_v1";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Fall17_17Nov2017_V4_MC"));
        c.filters["globalSuperTightHalo2016Filter"] = true;
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
        c.btageff_file       = "btageff__ttbar_powheg_pythia8_25ns_Moriond17.root";
        c.btag_med_threshold = 0.8484;
        c.pu_weights_file    = "puWeight2016.root";
        c.ea_version         = 1;
        c.jet_id             = "50nsV1";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Summer16_23Sep2016V4_MC"));
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["badMuonFilterV2"] = true;
        c.filters["badChargedHadronFilterV2"] = true;

    }else if(tag == "mc_80x_fastsim_Moriond17"){

        c.lumi               = 35.92;
        c.btagcalib_csv      = "CSVv2_Moriond17_B_H.csv";
        c.btageff_file       = "btageff__ttbar_powheg_pythia8_25ns_Moriond17.root";
        c.btag_med_threshold = 0.8484;
        c.pu_weights_file    = "puWeight2016.root";
        c.ea_version         = 1;
        c.jet_id             = "50nsV1";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Spring16_FastSimV1_MC"));
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["badMuonFilterV2"] = true;
        c.filters["badChargedHadronFilterV2"] = true;

    }else if(tag == "mc_94x_Fall17"){

        c.lumi               = 41.37;
        c.btagcalib_csv      = "CSVv2_94XSF_V2_B_F.csv";
        c.btageff_file       = "btageff__ttbar_powheg_pythia8_25ns_Fall17.root";
        c.btag_med_threshold = 0.8838;
        c.pu_weights_file    = "puWeight2017.root";
        c.ea_version         = 3;
        c.jet_id             = "2017_v1";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Fall17_17Nov2017_V4_MC"));
        c.filters["globalSuperTightHalo2016Filter"] = true;
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
