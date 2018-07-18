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
    float btag_med_threshold;
    std::vector<std::pair<std::string, std::string> > JECs;
    std::map<std::string, bool> filters;
};

MT2Configuration GetMT2Config(std::string tag);

#endif
