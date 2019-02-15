
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
    c.dilep_trigeff_file = "";
    c.triggers = std::map<std::string, std::vector<std::string> > ();
    c.filters["eeBadScFilter"] = false;
    c.filters["globalTightHalo2016Filter"] = false;
    c.filters["globalSuperTightHalo2016Filter"] = false;
    c.filters["goodVertices"] = false;
    c.filters["HBHENoiseFilter"] = false;
    c.filters["HBHENoiseIsoFilter"] = false;
    c.filters["EcalDeadCellTriggerPrimitiveFilter"] = false;
    c.filters["ecalBadCalibFilter"] = false;
    c.filters["ecalBadCalibFilterUpdate"] = false;
    c.filters["badMuonFilter"] = false;
    c.filters["badChargedCandidateFilter"] = false;
    c.filters["badMuonFilterV2"] = false;
    c.filters["badChargedHadronFilterV2"] = false;

    if(tag == "data_2016_94x"){

        c.year               = 2016;
        c.cmssw_ver          = 94;
        c.json               = "Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON_snt.txt";
        c.lumi               = 35.922;
        c.btagcalib_csv      = "DeepCSV_Moriond17_B_H.csv";
        c.btageff_file       = "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Summer16_94x.root";
        c.btag_med_threshold_DeepCSV = 0.6321;
        c.btag_med_threshold_CSVv2   = 0.8484;
        c.ea_version         = 1;
        c.jet_id             = "50nsV1";
        c.JRT_file           = "JetResponseTemplates_ptBinned_80x_JetID_PUID_BTagSFs_core2sigma.root";
        c.rphi_tag           = "V00-10-10_2016fullYear";
        c.JECs.push_back(std::pair<std::string, std::string> ("2016B", "Summer16_07Aug2017BCD_V11_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016C", "Summer16_07Aug2017BCD_V11_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016D", "Summer16_07Aug2017BCD_V11_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016E", "Summer16_07Aug2017EF_V11_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016F", "Summer16_07Aug2017EF_V11_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016G", "Summer16_07Aug2017GH_V11_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2016H", "Summer16_07Aug2017GH_V11_DATA"));
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

        c.year               = 2016;
        c.cmssw_ver          = 80;
        c.json               = "Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON_snt.txt";
        c.lumi               = 35.922;
        c.btagcalib_csv      = "DeepCSV_Moriond17_B_H.csv";
        c.btageff_file       = "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Summer16_94x.root";
        c.btag_med_threshold_DeepCSV = 0.6324;
        c.btag_med_threshold_CSVv2   = 0.8484;
        c.ea_version         = 1;
        c.jet_id             = "50nsV1";
        c.JRT_file           = "JetResponseTemplates_ptBinned_80x_JetID_PUID_BTagSFs_core2sigma.root";
        c.rphi_tag           = "V00-10-10_2016fullYear";
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

        c.year               = 2017;
        c.cmssw_ver          = 94;
        c.json               = "Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_v1_snt.txt";
        c.lumi               = 41.37;
        c.btagcalib_csv      = "DeepCSV_94XSF_V3_B_F.csv";
        c.btageff_file       = "btageff__DeepCSV_ttbar_powheg_pythia8_25ns_Fall17.root";
        c.btag_med_threshold_DeepCSV = 0.4941;
        c.btag_med_threshold_CSVv2   = 0.8838;
        c.ea_version         = 3;
        c.jet_id             = "2017_v1";
        c.JRT_file           = "JetResponseTemplates_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma.root";
        c.rphi_tag           = "V00-10-09_2017fullYear";
        c.JECs.push_back(std::pair<std::string, std::string> ("2017B", "Fall17_17Nov2017B_V32_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2017C", "Fall17_17Nov2017C_V32_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2017D", "Fall17_17Nov2017DE_V32_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2017E", "Fall17_17Nov2017DE_V32_DATA"));
        c.JECs.push_back(std::pair<std::string, std::string> ("2017F", "Fall17_17Nov2017F_V32_DATA"));
        c.filters["eeBadScFilter"] = true;
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["ecalBadCalibFilterUpdate"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;
        c.triggers["SR"] = std::vector<std::string> ();
        c.triggers["SR"].push_back("PFHT1050");
        c.triggers["SR"].push_back("PFJet500");
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

        c.year               = 2017;
        c.cmssw_ver          = 94;
        c.json               = "Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON_snt.txt";
        c.lumi               = 41.97;
        c.btagcalib_csv      = "DeepCSV_94XSF_V3_B_F.csv";
        c.btageff_file       = "btageff__DeepCSV_ttbar_powheg_pythia8_25ns_Fall17.root";
        c.btag_med_threshold_DeepCSV = 0.4941;
        c.btag_med_threshold_CSVv2   = 0.8838;
        c.ea_version         = 3;
        c.jet_id             = "2017_v1";
        c.JRT_file           = "JetResponseTemplates_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma.root";
        c.rphi_tag           = "V00-10-09_2017fullYear";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Summer16_23Sep2016HV4_DATA"));
        c.filters["eeBadScFilter"] = true;
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.triggers["SR"] = std::vector<std::string> ();
        c.triggers["SR"].push_back("PFHT1050");
        c.triggers["SR"].push_back("PFJet500");
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

    }else if(tag == "data_2018_17Sep2018"){

        c.year               = 2018;
        c.cmssw_ver          = 102;
        c.json               = "Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON_snt.txt";
        c.lumi               = 59.97;
        c.btagcalib_csv      = "DeepCSV_94XSF_V3_B_F.csv";
        c.btageff_file       = "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Autumn18.root";
        c.btag_med_threshold_DeepCSV = 0.4184;
        c.btag_med_threshold_CSVv2   = 0.8838;
        c.ea_version         = 3;
        c.jet_id             = "2018_v1";
        c.JRT_file           = "JetResponseTemplates_ptBinned_102x_JetID_PUID_BTagSFs_core2sigma.root";
        c.rphi_tag           = "V00-10-09_2018fullYear";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Autumn18_V3_DATAdummy"));
        c.filters["eeBadScFilter"] = true;
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["ecalBadCalibFilterUpdate"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;
        c.triggers["SR"] = std::vector<std::string> ();
        c.triggers["SR"].push_back("PFHT1050");
        c.triggers["SR"].push_back("PFJet500");
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

        c.year               = 2018;
        c.cmssw_ver          = 102;
        c.json               = "Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON_snt.txt";
        c.lumi               = 59.97;
        c.btagcalib_csv      = "DeepCSV_94XSF_V3_B_F.csv";
        c.btageff_file       = "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Autumn18.root";
        c.btag_med_threshold_DeepCSV = 0.4184;
        c.btag_med_threshold_CSVv2   = 0.8838;
        c.ea_version         = 3;
        c.jet_id             = "2017_v1";
        c.JRT_file           = "JetResponseTemplates_ptBinned_102x_JetID_PUID_BTagSFs_core2sigma.root";
        c.rphi_tag           = "V00-10-09_2018fullYear";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Fall17_17Nov2017C_V32_DATA"));
        c.filters["eeBadScFilter"] = true;
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["ecalBadCalibFilterUpdate"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;
        c.triggers["SR"] = std::vector<std::string> ();
        c.triggers["SR"].push_back("PFHT1050");
        c.triggers["SR"].push_back("PFJet500");
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

    }else if(tag == "mc_102x_Autumn18"){

        c.year               = 2018;
        c.cmssw_ver          = 102;
        c.lumi               = 59.97;
        c.btagcalib_csv      = "DeepCSV_94XSF_V3_B_F.csv";
        c.btageff_file       = "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Autumn18.root";
        c.btag_med_threshold_DeepCSV = 0.4184;
        c.btag_med_threshold_CSVv2   = 0.8838;
        c.pu_weights_file    = "puWeight2018.root";
        c.elSF_IDISOfile     = "lepsf/Autumn18/ElectronScaleFactors_Run2018.root";
        c.elSF_TRKfile       = "lepsf/Autumn18/egammaEffi.txt_EGM2D.root";
        c.elSF_TRKfileLowPt  = "lepsf/Autumn18/egammaEffi.txt_EGM2D.root";
        c.elSF_IDhistName    = "Run2018_CutBasedVetoNoIso94XV2";
        c.elSF_ISOhistName   = "Run2018_Mini";
        c.muSF_IDfile        = "lepsf/Fall17/MuonPOG_SF_ID_2017BCDEF.root";
        c.muSF_ISOfile       = "lepsf/Fall17/SUS_MuSF_MiniIso2_LooseID.root";
        c.muSF_IPfile        = "";
        c.muSF_TRKfile       = "";
        c.muSF_IDhistName    = "NUM_LooseID_DEN_genTracks_pt_abseta";
        c.muSF_ISOhistName   = "TnP_MC_NUM_MiniIso02Cut_DEN_LooseID_PAR_pt_eta";
        c.muSF_IPhistName    = "";
        c.muSF_TRKLT10histName = "";
        c.muSF_TRKGT10histName = "";
        c.dilep_trigeff_file = "lepsf/trigeff/trigeff_dilep_2018fullYear.root";
        c.ea_version         = 3;
        c.jet_id             = "2018_v1";
        c.JRT_file           = "JetResponseTemplates_ptBinned_102x_JetID_PUID_BTagSFs_core2sigma.root";
        c.rphi_tag           = "V00-10-09_2018fullYear";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Autumn18_V3_MC"));
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["ecalBadCalibFilterUpdate"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;

    }else if(tag == "mc_102x_fastsim_Autumn18"){

        c.year               = 2018;
        c.cmssw_ver          = 94;
        c.lumi               = 59.97;
        c.btagcalib_csv      = "DeepCSV_94XSF_V3_B_F.csv";
        c.btageff_file       = "btageff__DeepCSV_ttbar_powheg_pythia8_25ns_Fall17.root";
        c.btag_med_threshold_DeepCSV = 0.4941;
        c.btag_med_threshold_CSVv2   = 0.8838;
        c.pu_weights_file    = "puWeight2017.root";
        c.elSF_IDISOfile     = "lepsf/Fall17/ElectronScaleFactors_Run2017.root";
        c.elSF_TRKfile       = "lepsf/Fall17/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root";
        c.elSF_TRKfileLowPt  = "lepsf/Fall17/egammaEffi.txt_EGM2D_runBCDEF_passingRECO_lowEt.root";
        c.elSF_IDhistName    = "Run2017_CutBasedVetoNoIso94XV2";
        c.elSF_ISOhistName   = "Run2017_MVAVLooseTightIP2DMini";
        c.muSF_IDfile        = "lepsf/Fall17/MuonPOG_SF_ID_2017BCDEF.root";
        c.muSF_ISOfile       = "lepsf/Fall17/SUS_MuSF_MiniIso2_LooseID.root";
        c.muSF_IPfile        = "";
        c.muSF_TRKfile       = "";
        c.muSF_IDhistName    = "NUM_LooseID_DEN_genTracks_pt_abseta";
        c.muSF_ISOhistName   = "TnP_MC_NUM_MiniIso02Cut_DEN_LooseID_PAR_pt_eta";
        c.muSF_IPhistName    = "";
        c.muSF_TRKLT10histName = "";
        c.muSF_TRKGT10histName = "";
        c.dilep_trigeff_file = "lepsf/trigeff/trigeff_dilep_2017fullYear.root";
        c.ea_version         = 3;
        c.jet_id             = "2017_v1";
        c.JRT_file           = "JetResponseTemplates_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma.root";
        c.rphi_tag           = "V00-10-09_2017fullYear";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Fall17_17Nov2017_V32_MC"));
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["ecalBadCalibFilterUpdate"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;

    }else if(tag == "mc_80x_Summer16"){

        c.year               = 2016;
        c.cmssw_ver          = 80;
        c.lumi               = 35.922;
        c.btagcalib_csv      = "DeepCSV_Moriond17_B_H.csv";
        c.btageff_file       = "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Summer16_94x.root";
        c.btag_med_threshold_DeepCSV = 0.6324;
        c.btag_med_threshold_CSVv2   = 0.8484;
        c.pu_weights_file    = "puWeight2016.root";
        c.elSF_IDISOfile     = "lepsf/moriond17/scaleFactors_el_moriond_2017.root";
        c.elSF_TRKfile       = "lepsf/moriond17/egammaEffi.txt_EGM2D.root";
        c.elSF_TRKfileLowPt  = "lepsf/moriond17/egammaEffi.txt_EGM2D.root";
        c.elSF_IDhistName    = "GsfElectronToCutBasedSpring15V";
        c.elSF_ISOhistName   = "MVAVLooseElectronToMini";
        c.muSF_IDfile        = "lepsf/moriond17/TnP_NUM_LooseID_DENOM_generalTracks_VAR_map_pt_eta.root";
        c.muSF_ISOfile       = "lepsf/moriond17/TnP_NUM_MiniIsoTight_DENOM_LooseID_VAR_map_pt_eta.root";
        c.muSF_IPfile        = "lepsf/moriond17/TnP_NUM_MediumIP2D_DENOM_LooseID_VAR_map_pt_eta.root";
        c.muSF_TRKfile       = "lepsf/moriond17/Tracking_EfficienciesAndSF_BCDEFGH_hists.root";
        c.muSF_IDhistName    = "SF";
        c.muSF_ISOhistName   = "SF";
        c.muSF_IPhistName    = "SF";
        c.muSF_TRKLT10histName = "ratio_eff_eta3_tk0_dr030e030_corr";
        c.muSF_TRKGT10histName = "ratio_eff_eta3_dr030e030_corr";
        c.dilep_trigeff_file = "lepsf/trigeff/trigeff_dilep_2016fullYear.root";
        c.ea_version         = 1;
        c.jet_id             = "50nsV1";
        c.JRT_file           = "JetResponseTemplates_ptBinned_80x_JetID_PUID_BTagSFs_core2sigma.root";
        c.rphi_tag           = "V00-10-10_2016fullYear";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Summer16_23Sep2016V4_MC"));
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["badMuonFilterV2"] = true;
        c.filters["badChargedHadronFilterV2"] = true;

    }else if(tag == "mc_80x_fastsim_Moriond17"){

        c.year               = 2016;
        c.cmssw_ver          = 80;
        c.lumi               = 35.922;
        c.btagcalib_csv      = "DeepCSV_Moriond17_B_H.csv";
        c.btageff_file       = "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Summer16_94x.root";
        c.btag_med_threshold_DeepCSV = 0.6324;
        c.btag_med_threshold_CSVv2   = 0.8484;
        c.pu_weights_file    = "puWeight2016.root";
        c.elSF_IDISOfile     = "lepsf/moriond17/scaleFactors_el_moriond_2017.root";
        c.elSF_TRKfile       = "lepsf/moriond17/egammaEffi.txt_EGM2D.root";
        c.elSF_TRKfileLowPt  = "lepsf/moriond17/egammaEffi.txt_EGM2D.root";
        c.elSF_IDhistName    = "GsfElectronToCutBasedSpring15V";
        c.elSF_ISOhistName   = "MVAVLooseElectronToMini";
        c.muSF_IDfile        = "lepsf/moriond17/TnP_NUM_LooseID_DENOM_generalTracks_VAR_map_pt_eta.root";
        c.muSF_ISOfile       = "lepsf/moriond17/TnP_NUM_MiniIsoTight_DENOM_LooseID_VAR_map_pt_eta.root";
        c.muSF_IPfile        = "lepsf/moriond17/TnP_NUM_MediumIP2D_DENOM_LooseID_VAR_map_pt_eta.root";
        c.muSF_TRKfile       = "lepsf/moriond17/Tracking_EfficienciesAndSF_BCDEFGH_hists.root";
        c.muSF_IDhistName    = "SF";
        c.muSF_ISOhistName   = "SF";
        c.muSF_IPhistName    = "SF";
        c.muSF_TRKLT10histName = "ratio_eff_eta3_tk0_dr030e030_corr";
        c.muSF_TRKGT10histName = "ratio_eff_eta3_dr030e030_corr";
        c.dilep_trigeff_file = "lepsf/trigeff/trigeff_dilep_2016fullYear.root";
        c.ea_version         = 1;
        c.jet_id             = "50nsV1";
        c.JRT_file           = "JetResponseTemplates_ptBinned_80x_JetID_PUID_BTagSFs_core2sigma.root";
        c.rphi_tag           = "V00-10-10_2016fullYear";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Spring16_FastSimV1_MC"));
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["badMuonFilterV2"] = true;
        c.filters["badChargedHadronFilterV2"] = true;

    }else if(tag == "mc_94x_Fall17"){

        c.year               = 2017;
        c.cmssw_ver          = 94;
        c.lumi               = 41.529;
        c.btagcalib_csv      = "DeepCSV_94XSF_V3_B_F.csv";
        c.btageff_file       = "btageff__DeepCSV_ttbar_powheg_pythia8_25ns_Fall17.root";
        c.btag_med_threshold_DeepCSV = 0.4941;
        c.btag_med_threshold_CSVv2   = 0.8838;
        c.pu_weights_file    = "puWeight2017.root";
        c.elSF_IDISOfile     = "lepsf/Fall17/ElectronScaleFactors_Run2017.root";
        c.elSF_TRKfile       = "lepsf/Fall17/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root";
        c.elSF_TRKfileLowPt  = "lepsf/Fall17/egammaEffi.txt_EGM2D_runBCDEF_passingRECO_lowEt.root";
        c.elSF_IDhistName    = "Run2017_CutBasedVetoNoIso94XV2";
        c.elSF_ISOhistName   = "Run2017_MVAVLooseTightIP2DMini";
        c.muSF_IDfile        = "lepsf/Fall17/MuonPOG_SF_ID_2017BCDEF.root";
        c.muSF_ISOfile       = "lepsf/Fall17/SUS_MuSF_MiniIso2_LooseID.root";
        c.muSF_IPfile        = "";
        c.muSF_TRKfile       = "";
        c.muSF_IDhistName    = "NUM_LooseID_DEN_genTracks_pt_abseta";
        c.muSF_ISOhistName   = "TnP_MC_NUM_MiniIso02Cut_DEN_LooseID_PAR_pt_eta";
        c.muSF_IPhistName    = "";
        c.muSF_TRKLT10histName = "";
        c.muSF_TRKGT10histName = "";
        c.dilep_trigeff_file = "lepsf/trigeff/trigeff_dilep_2017fullYear.root";
        c.ea_version         = 3;
        c.jet_id             = "2017_v1";
        c.JRT_file           = "JetResponseTemplates_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma.root";
        c.rphi_tag           = "V00-10-09_2017fullYear";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Fall17_17Nov2017_V32_MC"));
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["ecalBadCalibFilterUpdate"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;

    }else if(tag == "mc_94x_Summer16"){

        c.year               = 2016;
        c.cmssw_ver          = 94;
        c.lumi               = 35.922;
        c.btagcalib_csv      = "DeepCSV_Moriond17_B_H.csv";
        c.btageff_file       = "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Summer16_94x.root";
        c.btag_med_threshold_DeepCSV = 0.6321;
        c.btag_med_threshold_CSVv2   = 0.8484;
        c.pu_weights_file    = "puWeight2016.root";
        c.elSF_IDISOfile     = "lepsf/Summer16_94x/ElectronScaleFactors_Run2016.root";
        c.elSF_TRKfile       = "lepsf/moriond17/egammaEffi.txt_EGM2D.root";
        c.elSF_TRKfileLowPt  = "lepsf/moriond17/egammaEffi.txt_EGM2D.root";
        c.elSF_IDhistName    = "Run2016_CutBasedVetoNoIso94XV2";
        c.elSF_ISOhistName   = "Run2016_Mini";
        c.muSF_IDfile        = "lepsf/moriond17/TnP_NUM_LooseID_DENOM_generalTracks_VAR_map_pt_eta.root";
        c.muSF_ISOfile       = "lepsf/moriond17/TnP_NUM_MiniIsoTight_DENOM_LooseID_VAR_map_pt_eta.root";
        c.muSF_IPfile        = "lepsf/moriond17/TnP_NUM_MediumIP2D_DENOM_LooseID_VAR_map_pt_eta.root";
        c.muSF_TRKfile       = "lepsf/moriond17/Tracking_EfficienciesAndSF_BCDEFGH_hists.root";
        c.muSF_IDhistName    = "SF";
        c.muSF_ISOhistName   = "SF";
        c.muSF_IPhistName    = "SF";
        c.muSF_TRKLT10histName = "ratio_eff_eta3_tk0_dr030e030_corr";
        c.muSF_TRKGT10histName = "ratio_eff_eta3_dr030e030_corr";
        c.dilep_trigeff_file = "lepsf/trigeff/trigeff_dilep_2016fullYear.root";
        c.ea_version         = 1;
        c.jet_id             = "50nsV1";
        c.JRT_file           = "JetResponseTemplates_ptBinned_80x_JetID_PUID_BTagSFs_core2sigma.root";
        c.rphi_tag           = "V00-10-10_2016fullYear";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Summer16_07Aug2017_V11_MC"));
        c.filters["eeBadScFilter"] = true;
        c.filters["globalSuperTightHalo2016Filter"] = true;
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;

    }else if(tag == "mc_94x_fastsim_Fall17"){

        c.year               = 2017;
        c.cmssw_ver          = 94;
        c.lumi               = 41.529;
        c.btagcalib_csv      = "DeepCSV_94XSF_V3_B_F.csv";
        c.btageff_file       = "btageff__DeepCSV_ttbar_powheg_pythia8_25ns_Fall17.root";
        c.btag_med_threshold_DeepCSV = 0.4941;
        c.btag_med_threshold_CSVv2   = 0.8838;
        c.pu_weights_file    = "puWeight2017.root";
        c.elSF_IDISOfile     = "lepsf/Fall17/ElectronScaleFactors_Run2017.root";
        c.elSF_TRKfile       = "lepsf/Fall17/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root";
        c.elSF_TRKfileLowPt  = "lepsf/Fall17/egammaEffi.txt_EGM2D_runBCDEF_passingRECO_lowEt.root";
        c.elSF_IDhistName    = "Run2017_CutBasedVetoNoIso94XV2";
        c.elSF_ISOhistName   = "Run2017_MVAVLooseTightIP2DMini";
        c.muSF_IDfile        = "lepsf/Fall17/MuonPOG_SF_ID_2017BCDEF.root";
        c.muSF_ISOfile       = "lepsf/Fall17/SUS_MuSF_MiniIso2_LooseID.root";
        c.muSF_IPfile        = "";
        c.muSF_TRKfile       = "";
        c.muSF_IDhistName    = "NUM_LooseID_DEN_genTracks_pt_abseta";
        c.muSF_ISOhistName   = "TnP_MC_NUM_MiniIso02Cut_DEN_LooseID_PAR_pt_eta";
        c.muSF_IPhistName    = "";
        c.muSF_TRKLT10histName = "";
        c.muSF_TRKGT10histName = "";
        c.dilep_trigeff_file = "lepsf/trigeff/trigeff_dilep_2017fullYear.root";
        c.ea_version         = 3;
        c.jet_id             = "2017_v1";
        c.JRT_file           = "JetResponseTemplates_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma.root";
        c.rphi_tag           = "V00-10-09_2017fullYear";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Spring16_FastSimV1_MC"));
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;
        c.filters["ecalBadCalibFilterUpdate"] = true;

    }else if(tag == "mc_94x_fastsim_Summer16"){

        c.year               = 2016;
        c.cmssw_ver          = 94;
        c.lumi               = 35.922;
        c.btagcalib_csv      = "DeepCSV_Moriond17_B_H.csv";
        c.btageff_file       = "btageff__DeepCSV_ttbar_MG_pythia8_25ns_Summer16_94x.root";
        c.btag_med_threshold_DeepCSV = 0.6321;
        c.btag_med_threshold_CSVv2   = 0.8484;
        c.pu_weights_file    = "puWeight2016.root";
        c.elSF_IDISOfile     = "lepsf/Summer16_94x/ElectronScaleFactors_Run2016.root";
        c.elSF_TRKfile       = "lepsf/moriond17/egammaEffi.txt_EGM2D.root";
        c.elSF_TRKfileLowPt  = "lepsf/moriond17/egammaEffi.txt_EGM2D.root";
        c.elSF_IDhistName    = "Run2016_CutBasedVetoNoIso94XV2";
        c.elSF_ISOhistName   = "Run2016_Mini";
        c.muSF_IDfile        = "lepsf/moriond17/TnP_NUM_LooseID_DENOM_generalTracks_VAR_map_pt_eta.root";
        c.muSF_ISOfile       = "lepsf/moriond17/TnP_NUM_MiniIsoTight_DENOM_LooseID_VAR_map_pt_eta.root";
        c.muSF_IPfile        = "lepsf/moriond17/TnP_NUM_MediumIP2D_DENOM_LooseID_VAR_map_pt_eta.root";
        c.muSF_TRKfile       = "lepsf/moriond17/Tracking_EfficienciesAndSF_BCDEFGH_hists.root";
        c.muSF_IDhistName    = "SF";
        c.muSF_ISOhistName   = "SF";
        c.muSF_IPhistName    = "SF";
        c.muSF_TRKLT10histName = "ratio_eff_eta3_tk0_dr030e030_corr";
        c.muSF_TRKGT10histName = "ratio_eff_eta3_dr030e030_corr";
        c.dilep_trigeff_file = "lepsf/trigeff/trigeff_dilep_2016fullYear.root";
        c.ea_version         = 1;
        c.jet_id             = "50nsV1";
        c.JRT_file           = "JetResponseTemplates_ptBinned_80x_JetID_PUID_BTagSFs_core2sigma.root";
        c.rphi_tag           = "V00-10-10_2016fullYear";
        c.JECs.push_back(std::pair<std::string, std::string> ("", "Spring16_FastSimV1_MC"));
        c.filters["goodVertices"] = true;
        c.filters["HBHENoiseFilter"] = true;
        c.filters["HBHENoiseIsoFilter"] = true;
        c.filters["EcalDeadCellTriggerPrimitiveFilter"] = true;
        c.filters["badMuonFilter"] = true;
        c.filters["badChargedCandidateFilter"] = true;

    }else{
        std::cout << "[MT2Configuration] ERROR! invalid tag \""+tag+"\"" << std::endl;
        throw std::exception();
    }

    return c;

}

void fillTriggerVector(const mt2tree& t, std::vector<const Int_t*>& trigs, std::vector<std::string>& trig_names) {

    trigs.clear();

    for(uint i=0; i<trig_names.size(); i++){
        std::string s = trig_names.at(i);
        if     (s=="DoubleEl")                         trigs.push_back(&t.HLT_DoubleEl);
        else if(s=="DoubleEl33")                       trigs.push_back(&t.HLT_DoubleEl33);
        else if(s=="DoubleMu")                         trigs.push_back(&t.HLT_DoubleMu);
        else if(s=="DoubleMu_NonIso")                  trigs.push_back(&t.HLT_DoubleMu_NonIso);
        else if(s=="Mu12_EleX")                        trigs.push_back(&t.HLT_Mu12_EleX);
        else if(s=="Mu27_Ele37_NonIso")                trigs.push_back(&t.HLT_Mu27_Ele37_NonIso);
        else if(s=="Mu30_Ele30_NonIso")                trigs.push_back(&t.HLT_Mu30_Ele30_NonIso);
        else if(s=="Mu33_Ele33_NonIso")                trigs.push_back(&t.HLT_Mu33_Ele33_NonIso);
        else if(s=="Mu37_Ele27_NonIso")                trigs.push_back(&t.HLT_Mu37_Ele27_NonIso);
        else if(s=="Mu8_EleX")                         trigs.push_back(&t.HLT_Mu8_EleX);
        else if(s=="MuX_Ele12")                        trigs.push_back(&t.HLT_MuX_Ele12);
        else if(s=="PFHT1050")                         trigs.push_back(&t.HLT_PFHT1050);
        else if(s=="PFHT125_Prescale")                 trigs.push_back(&t.HLT_PFHT125_Prescale);
        else if(s=="PFHT180_Prescale")                 trigs.push_back(&t.HLT_PFHT180_Prescale);
        else if(s=="PFHT200_Prescale")                 trigs.push_back(&t.HLT_PFHT200_Prescale);
        else if(s=="PFHT250_Prescale")                 trigs.push_back(&t.HLT_PFHT250_Prescale);
        else if(s=="PFHT300_PFMET110")                 trigs.push_back(&t.HLT_PFHT300_PFMET110);
        else if(s=="PFHT300_Prescale")                 trigs.push_back(&t.HLT_PFHT300_Prescale);
        else if(s=="PFHT350_Prescale")                 trigs.push_back(&t.HLT_PFHT350_Prescale);
        else if(s=="PFHT370_Prescale")                 trigs.push_back(&t.HLT_PFHT370_Prescale);
        else if(s=="PFHT430_Prescale")                 trigs.push_back(&t.HLT_PFHT430_Prescale);
        else if(s=="PFHT475_Prescale")                 trigs.push_back(&t.HLT_PFHT475_Prescale);
        else if(s=="PFHT500_PFMET100_PFMHT100")        trigs.push_back(&t.HLT_PFHT500_PFMET100_PFMHT100);
        else if(s=="PFHT510_Prescale")                 trigs.push_back(&t.HLT_PFHT510_Prescale);
        else if(s=="PFHT590_Prescale")                 trigs.push_back(&t.HLT_PFHT590_Prescale);
        else if(s=="PFHT600_Prescale")                 trigs.push_back(&t.HLT_PFHT600_Prescale);
        else if(s=="PFHT680_Prescale")                 trigs.push_back(&t.HLT_PFHT680_Prescale);
        else if(s=="PFHT780_Prescale")                 trigs.push_back(&t.HLT_PFHT780_Prescale);
        else if(s=="PFHT800_PFMET75_PFMHT75")          trigs.push_back(&t.HLT_PFHT800_PFMET75_PFMHT75);
        else if(s=="PFHT890_Prescale")                 trigs.push_back(&t.HLT_PFHT890_Prescale);
        else if(s=="PFHT900")                          trigs.push_back(&t.HLT_PFHT900);
        else if(s=="PFJet450")                         trigs.push_back(&t.HLT_PFJet450);
        else if(s=="PFJet500")                         trigs.push_back(&t.HLT_PFJet500);
        else if(s=="PFMET120_PFMHT120")                trigs.push_back(&t.HLT_PFMET120_PFMHT120);
        else if(s=="PFMET120_PFMHT120_PFHT60")         trigs.push_back(&t.HLT_PFMET120_PFMHT120_PFHT60);
        else if(s=="PFMETNoMu120_PFMHTNoMu120")        trigs.push_back(&t.HLT_PFMETNoMu120_PFMHTNoMu120);
        else if(s=="PFMETNoMu120_PFMHTNoMu120_PFHT60") trigs.push_back(&t.HLT_PFMETNoMu120_PFMHTNoMu120_PFHT60);
        else if(s=="Photon165_HE10")                   trigs.push_back(&t.HLT_Photon165_HE10);
        else if(s=="Photon200")                        trigs.push_back(&t.HLT_Photon200);
        else if(s=="SingleEl")                         trigs.push_back(&t.HLT_SingleEl);
        else if(s=="SingleEl_NonIso")                  trigs.push_back(&t.HLT_SingleEl_NonIso);
        else if(s=="SingleMu")                         trigs.push_back(&t.HLT_SingleMu);
        else if(s=="SingleMu_NonIso")                  trigs.push_back(&t.HLT_SingleMu_NonIso);
        else
            std::cout << "[MT2Looper::fillTriggerVectors] WARNING: unknown trigger " << s << "! Not applying." << std::endl;

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
