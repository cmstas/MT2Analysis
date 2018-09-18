#ifndef MT2CONFIG_H
#define MT2CONFIG_H

#include <string>
#include <utility>
#include <vector>
#include <map>

struct MT2Configuration {
    std::string json;
    float lumi;
    std::string btagcalib_csv;
    std::string btageff_file;
    float btag_med_threshold;
    std::string pu_weights_file;
    int ea_version;
    std::string jet_id;
    std::vector<std::pair<std::string, std::string> > JECs;
    std::map<std::string, bool> filters;
    std::map<std::string, std::vector<std::string> > triggers;
};

MT2Configuration GetMT2Config(std::string tag);

#endif
