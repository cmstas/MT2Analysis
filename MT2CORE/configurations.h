#ifndef MT2CONFIG_H
#define MT2CONFIG_H

#include <string>
#include <utility>
#include <vector>
#include <map>

#include "mt2tree.h"

struct MT2Configuration {
    std::string json;
    int year = -1;
    int cmssw_ver = -1;
    float lumi = -1;
    std::string btagcalib_csv;
    std::string btageff_file;
    float btag_med_threshold_DeepCSV;
    float btag_med_threshold_CSVv2;
    std::string pu_weights_file;
    std::string elSF_IDISOfile;
    std::string elSF_TRKfile;
    std::string elSF_IDhistName;
    std::string elSF_ISOhistName;
    std::string muSF_IDfile;
    std::string muSF_ISOfile;
    std::string muSF_IPfile;
    std::string muSF_TRKfile;
    std::string muSF_IDhistName;
    std::string muSF_ISOhistName;
    std::string muSF_IPhistName;
    std::string muSF_TRKLT10histName;
    std::string muSF_TRKGT10histName;
    std::string dilep_trigeff_file;
    int ea_version;
    std::string JRT_file;
    std::string jet_id;
    std::vector<std::pair<std::string, std::string> > JECs;
    std::map<std::string, bool> filters;
    std::map<std::string, std::vector<std::string> > triggers;
};

MT2Configuration GetMT2Config(std::string tag);

void fillTriggerVector(const mt2tree& t, std::vector<const Int_t*>& trigs, std::vector<std::string>& trig_names);

bool passTrigger(const mt2tree& t, std::vector<const Int_t*>& trigs, bool debug=false);

#endif
