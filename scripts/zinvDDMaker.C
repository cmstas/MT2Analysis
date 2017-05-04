// -*- C++ -*-
// The code is stealed from the cardMaker.C with only the zinv data driven part

#include <iostream>
#include <utility>
#include <vector>
#include <map>

#include "TROOT.h"
#include "TLatex.h"
#include "TString.h"
#include "TH2.h"
#include "THStack.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TPaveText.h"
#include "TKey.h"

using namespace std;


//_______________________________________________________________________________
void combineZinvDataDriven(TFile* f_zinv, TFile* f_purity, TFile* f_zgratio, TFile* f_zinvMC, vector<string> dirs, string output_name, bool integratedZinvEstimate = true, float kFactorGJetForRatio = 1.0 ) {

  // Generate histogram file with Zinv prediction based on GJetsData * R(Zinv/GJ)

  TFile * outfile = new TFile(output_name.c_str(),"RECREATE") ;
  outfile->cd();
  const unsigned int ndirs = dirs.size();

  double last_zinv_ratio(.5);

  for ( unsigned int idir = 0; idir < ndirs; ++idir ) {

    TString directory = "sr" + dirs.at(idir);
    // TString directoryGJ = "crhgj"+dirs.at(idir);

    cout<<"Looking at directory "<<directory<<endl;

    TString fullhistname = directory + "/h_mt2bins";
    TString fullhistnameCRyield  = fullhistname+"CRyield";
    TString fullhistnameAlpha  = fullhistname+"Alpha";
    TString fullhistnameRatio  = fullhistname+"Ratio";
    TString fullhistnameRatioInt  = fullhistname+"RatioInt";
    TString fullhistnamePurity = directory + "/h_mt2binspurityFailSieieData";
    TString fullhistnamePurityInt = directory + "/h_mt2binspurityIntFailSieieData";

    TH1D* h_zinv = (TH1D*) f_zinv->Get(fullhistname);
    int n_mt2bins = 1;
    if (h_zinv != 0) n_mt2bins = h_zinv->GetNbinsX();
    else cout << "Cannot find histogram: " << fullhistname << endl;

    TH1D* h_zinv_gjetyield = (TH1D*) f_zinv->Get(fullhistnameCRyield);
    TH1D* h_zinv_cryield = (TH1D*) f_purity->Get(fullhistname);
    TH1D* h_zinv_zgratio = (TH1D*) f_zinv->Get(fullhistnameRatio);
    TH1D* h_zinv_purity = (TH1D*) f_purity->Get(fullhistnamePurity);
    // When using the integrated estimate (over MT2), should use the integrated purity.
    if (integratedZinvEstimate) {
      h_zinv_zgratio = (TH1D*) f_zinv->Get(fullhistnameRatioInt);
      h_zinv_purity = (TH1D*) f_purity->Get(fullhistnamePurityInt);
    }

    // Make directory and plot(s) in the output file
    TDirectory* dir = 0;
    dir = (TDirectory*)outfile->Get(directory.Data());
    if (dir == 0) {
      dir = outfile->mkdir(directory.Data());
    }
    dir->cd();

    std::map<string, TH1D*> kineHistMap;
    kineHistMap["mt2"]       = (TH1D*) f_zinvMC->Get(directory + "/h_mt2");
    kineHistMap["met"]       = (TH1D*) f_zinvMC->Get(directory + "/h_met");
    kineHistMap["ht"]        = (TH1D*) f_zinvMC->Get(directory + "/h_ht");
    kineHistMap["nJet30"]    = (TH1D*) f_zinvMC->Get(directory + "/h_nJet30");
    kineHistMap["nBJet20"]   = (TH1D*) f_zinvMC->Get(directory + "/h_nBJet20");
    kineHistMap["minMTbmet"] = (TH1D*) f_zinvMC->Get(directory + "/h_minMTbmet");
    kineHistMap["MbbMax"]    = (TH1D*) f_zinvMC->Get(directory + "/h_MbbMax");

    if (!h_zinv_zgratio) cout << "Cannot find histogram: " << fullhistnameRatio << endl;
    if (!h_zinv_purity) cout << "Cannot find histogram: " << fullhistnamePurity << endl;
    if (!h_zinv_cryield) cout << "Cannot find CR yields histogram in purity.root: " << fullhistname << endl;
    if (!h_zinv_cryield && !h_zinv_purity && !h_zinv_zgratio) continue;

    TH1D* h_n_mt2bins = new TH1D("h_n_mt2bins", "n_mt2bins", 1, 0, 2);
    h_n_mt2bins->SetBinContent(1, n_mt2bins);
    TH1D* pred = (TH1D*) h_zinv->Clone("h_mt2bins");
    TH1D* h_mcyield = (TH1D*) h_zinv->Clone("h_mt2binsMCyield");
    TH1D* h_cryield;
    if (h_zinv_cryield) h_cryield = (TH1D*) h_zinv_cryield->Clone("h_mt2binsCRyield");
    else h_cryield = new TH1D("h_mt2binsCRyield", "", n_mt2bins, 0, 1500);
    TH1D* h_gjyield;
    if (h_zinv_gjetyield) h_gjyield = (TH1D*) h_zinv_gjetyield->Clone("h_mt2binsGJyield");
    else h_gjyield = new TH1D("h_mt2binsGJyield", "", n_mt2bins, 0, 1500);

    TH1D* h_ratio;
    if (h_zinv_zgratio) {
      h_ratio = (TH1D*) h_zinv_zgratio->Clone();
      // h_ratio->SetBinError(1, h_ratio->GetBinError(1)/h_ratio->GetBinContent(1));
    } else {
      h_ratio = new TH1D(fullhistnameRatioInt, "Ratio From Last bin", 1, 0, 1);
      h_ratio->SetBinContent(1, last_zinv_ratio);
      h_ratio->SetBinError(1, 1.);
    }
    TH1D* h_purity;
    if (h_zinv_purity) {
      h_purity = (TH1D*) h_zinv_purity->Clone();
      // h_purity->SetBinError(1, h_purity->GetBinError(1)/h_purity->GetBinContent(1));
    } else {
      h_purity = new TH1D(fullhistnamePurityInt, "Purity 1", 1, 0, 1);
      h_purity->SetBinContent(1, 1.);
      h_purity->SetBinError(1, 1.);
    }

    // TH1D* h_cryield;
    // if (h_zinv_cryield) h_cryield = (TH1D*) h_zinv_cryield->Clone("h_mt2binsCRyield");
    // else h_cryield = new TH1D("h_mt2binsCRyield", "Empty hist", 1, 0, 1);

    for (int mt2bin = 1; mt2bin <= n_mt2bins; ++mt2bin) {
      double n_zinv(0.);
      double n_zinv_cr(0.);
      double err_zinv_crstat(0.);
      double err_zinv_ratio_zg(0.);
      double zinv_ratio_zg(0.);
      double zinv_purity(1.);
      double err_zinv_purity(0.);

      n_zinv = h_zinv->GetBinContent(mt2bin);
      if (h_zinv_zgratio && h_zinv_zgratio->GetBinContent(mt2bin) != 0) {
        err_zinv_ratio_zg = h_zinv_zgratio->GetBinError(mt2bin)/h_zinv_zgratio->GetBinContent(mt2bin);
        zinv_ratio_zg = h_zinv_zgratio->GetBinContent(mt2bin);
        last_zinv_ratio = zinv_ratio_zg;
      } else { // catch zeroes (shouldn't be any)
        err_zinv_ratio_zg = 1.;
        zinv_ratio_zg = last_zinv_ratio;
      }
      if (h_zinv_cryield) {
        n_zinv_cr = round(h_zinv_cryield->GetBinContent(mt2bin));
        err_zinv_crstat = h_zinv_cryield->GetBinError(mt2bin)/n_zinv_cr;
      } else {
        n_zinv_cr = 0;
        err_zinv_crstat = 1.;   // not sure about this
      }
      if (h_zinv_cryield && integratedZinvEstimate) {
        n_zinv_cr = round(h_zinv_cryield->IntegralAndError(0, -1, err_zinv_crstat));
        if (n_zinv_cr) err_zinv_crstat /= n_zinv_cr;
      }

      int mt2bin_tmp = (integratedZinvEstimate)? 1 : mt2bin;
      if (h_zinv_purity && h_zinv_purity->GetBinContent(mt2bin_tmp) != 0) {
        zinv_purity = h_zinv_purity->GetBinContent(mt2bin_tmp);
        err_zinv_purity = h_zinv_purity->GetBinError(mt2bin_tmp)/h_zinv_purity->GetBinContent(mt2bin_tmp);
      }

      double zinv_alpha = 1.;
      double err_shape = 0.;

      if (integratedZinvEstimate /* && nbjets >= 2 */) {
        // err_zinv_ratio_zg, zinv_ratio_zg, n_zinv_cr
        zinv_alpha = zinv_ratio_zg * zinv_purity * 0.92; // 0.92 is a fixed factor for "f = GJetPrompt / (GJetPrompt+QCDPrompt)"
        zinv_alpha *= 0.93; // data-driven correction, based on the double-ratio R(Zll/Gamma)
        if (zinv_alpha > 0.5) zinv_alpha = 0.5; // Hard bound to avoid statistical fluctuations
        n_zinv = n_zinv_cr * zinv_alpha; // don't use Zinv MC as central value any more!

        if (n_mt2bins > 1) {
          if (mt2bin == 1 && n_zinv > 0) {
            // first bin needs to compensate normalization from the rest
            float increment = 0.;
            for (int ibin=2; ibin<=h_zinv->GetNbinsX(); ibin++) 
              increment += 0.4 / (n_mt2bins - 1) * (ibin - 1) * h_zinv->GetBinContent(ibin);
            err_shape = - increment/n_zinv;
          } else {
            err_shape = 0.4 / (n_mt2bins - 1) * (mt2bin - 1);
          }
        }
      }

      // 1: zinv_alphaerr (stat on ratio).
      // 2: 20% syst. gamma function, no Nuisances for now 
      // 3: xx% purity stat unc.
      // 4: 10% purity Syst.
      // 5: 11% doubleRatioOffset
      // 6: xx% CR Stat, use simple Gaussian error for now

      // filling the histogram
      if (n_zinv > 0) {
        pred->SetBinContent(mt2bin, n_zinv);
        double err_zinv_squares = err_zinv_ratio_zg*err_zinv_ratio_zg + err_zinv_purity*err_zinv_purity + 0.20*0.20 + 0.10*0.10 + 0.11*0.11 + err_zinv_crstat*err_zinv_crstat;
	pred->SetBinError(mt2bin, n_zinv*sqrt(err_zinv_squares));
      } else {
        pred->SetBinContent(mt2bin, 0.);
        pred->SetBinError(mt2bin, h_zinv->GetBinContent(mt2bin));
      }
    }

    float zinv_purity = h_zinv_purity->GetBinContent(1); // assumes integratedZinvEstimate
    float zinv_ratio_zg = h_zinv_zgratio->Integral(0, -1);
    float norm = zinv_ratio_zg * zinv_purity * 0.92 * 0.93;
    for (auto it = kineHistMap.begin(); it != kineHistMap.end(); ++it) {
      if (it->second != nullptr) {
        it->second = (TH1D*) it->second->Clone(Form("h_%s", it->first.c_str()));
        it->second->Scale(norm);
      } else {
        cout << __LINE__ << ": Can't find hist " << it->first << endl;
      }
    }

    h_n_mt2bins->Write();
    pred->Write();
    h_mcyield->Write();
    h_cryield->Write();
    h_gjyield->Write();
    h_purity->Write();
    h_ratio->Write();
    for (auto it = kineHistMap.begin(); it != kineHistMap.end(); ++it) {
      if (it->second != nullptr) it->second->Write();
    }
  } // loop over signal regions

  return;
}


//_______________________________________________________________________________
void zinvDDMaker(string input_dir = "/home/users/sicheng/working/MT2Analysis/MT2looper/output/temp"){

  string output_name = input_dir+"/zinvDataDriven.root";
  // ----------------------------------------
  //  samples definition
  // ----------------------------------------

  std::cout << "Writing to file: " << output_name << std::endl;

  // get input files
  TFile* f_zinv = new TFile(Form("%s/zinvFromGJ.root", input_dir.c_str()));
  TFile* f_purity = new TFile(Form("%s/purity.root", input_dir.c_str()));
  TFile* f_zgratio = new TFile(Form("%s/doubleRatio.root", input_dir.c_str()));
  TFile* f_zinvMC = new TFile(Form("%s/zinv_ht.root", input_dir.c_str()));

  if(f_zinv->IsZombie() || f_zgratio->IsZombie() || f_purity->IsZombie()) {
    std::cerr << "Input file does not exist" << std::endl;
    return;
  }

  vector<string> dirs;

  // Loop through list of every directory in the signal file.
  // if directory begins with "sr", excluding "srbase", add it to vector signal regions.
  TIter it(f_purity->GetListOfKeys());
  TKey* k;
  std::string keep = "sr";
  std::string skip = "srbase";
  while ((k = (TKey *)it())) {
    if (strncmp (k->GetTitle(), skip.c_str(), skip.length()) == 0) continue;
    if (strncmp (k->GetTitle(), keep.c_str(), keep.length()) == 0) { // it is a signal region
      std::string sr_string = k->GetTitle();
      sr_string.erase(0, 2);    // remove "sr" from front of string
      if (sr_string[0] < 'A' || sr_string[0] == 'b') continue; // don't want the standard mt2 regions
      dirs.push_back(sr_string);
    }
  }

  combineZinvDataDriven(f_zinv, f_purity, f_zgratio, f_zinvMC, dirs, output_name);
}
