// -*- C++ -*-
// The code is stealed from the cardMaker.C with only the zinv data driven part

#include <iostream>
#include <utility>
#include <vector>

#include "TROOT.h"
#include "TLatex.h"
#include "TString.h"
#include "TFile.h"
#include "TH1.h"
#include "TKey.h"

using namespace std;


void combineSRkinematics(TFile* f_in, TFile* f_out, string sr = "") {

  if (f_in->IsZombie()) {
    std::cerr << "Input file does not exist" << std::endl;
    return;
  }

  vector<string> dirs;
  std::map<string, TH1D*> kineHistMap;
  kineHistMap["mt2"]       = nullptr;
  kineHistMap["met"]       = nullptr;
  kineHistMap["ht"]        = nullptr;
  kineHistMap["nJet30"]    = nullptr;
  kineHistMap["nBJet20"]   = nullptr;
  kineHistMap["minMTbmet"] = nullptr;
  kineHistMap["MbbMax"]    = nullptr;

  // Make directory and plot(s) in the output file
  string outname = sr + "allIncl";
  TDirectory* dir = (TDirectory*) f_out->mkdir(outname.c_str());
  dir->cd();

  // Loop through list of every directory in the signal file.
  // if directory begins with "sr", excluding "srbase", add it to vector signal regions.
  TIter it(f_in->GetListOfKeys());
  TKey* k;
  std::string keep = "sr";
  std::string skip = "srbase";
  while ((k = (TKey *)it())) {
    if (strncmp (k->GetTitle(), skip.c_str(), skip.length()) == 0) continue;
    if (strncmp (k->GetTitle(), keep.c_str(), keep.length()) == 0) { // it is a signal region
      std::string sr_string = k->GetTitle();
      // sr_string.erase(0, 2);    // remove "sr" from front of string
      if (sr_string.find(sr) == string::npos) continue;
      // dirs.push_back(sr_string);
      for (auto it = kineHistMap.begin(); it != kineHistMap.end(); ++it) {
        TH1D* temphist = (TH1D*) f_in->Get(Form("%s/h_%s", sr_string.c_str(), it->first.c_str()));
        if (temphist == nullptr) { cout << "The hist " << Form("%s/h_%s", sr_string.c_str(), it->first.c_str()) <<  " is not found!\n"; continue; }
        if (it->second == nullptr) {
          it->second = (TH1D*) temphist->Clone(Form("h_%s", it->first.c_str()));
        } else {
          it->second->Add(temphist);
        }
      }
    }
  }

  for (auto it = kineHistMap.begin(); it != kineHistMap.end(); ++it)
    if (it->second) it->second->Write();

}

void combineDDkinematics(string input_dir = "/home/users/sicheng/working/MT2Analysis/MT2looper/output/temp", string sr = "srCR") {

  // get input files

  TFile* f_in = new TFile(Form("%s/data_Run2016.root", input_dir.c_str()), "UPDATE");
  // TFile* f_out = new TFile(Form("%s/data_mbbcrDDkin.root", input_dir.c_str()), "RECREATE");
  combineSRkinematics(f_in, f_in, sr);

  f_in = new TFile(Form("%s/zinvDataDriven.root", input_dir.c_str()), "UPDATE");
  // f_out = new TFile(Form("%s/zinv_mbbcrDDkin.root", input_dir.c_str()), "RECREATE");
  combineSRkinematics(f_in, f_in, sr);

  f_in = new TFile(Form("%s/lostlepFromCRs.root", input_dir.c_str()), "UPDATE");
  // f_out = new TFile(Form("%s/lostlep_mbbcrDDkin.root", input_dir.c_str()), "RECREATE");
  combineSRkinematics(f_in, f_in, sr);
}
