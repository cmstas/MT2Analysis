// -*- C++ -*-
// The code is stealed from the cardMaker.C with only the zinv data driven part

#include <iostream>
#include <utility>
#include <vector>

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
void combineZinvDataDriven(TFile* f_zinv , TFile* f_purity , TFile* f_zgratio , vector<string> dirs, string output_name, bool integratedZinvEstimate = true, float kFactorGJetForRatio = 1.0 ) {

  // Generate histogram file with Zinv prediction based on GJetsData * R(Zinv/GJ)

  TFile * outfile = new TFile(output_name.c_str(),"RECREATE") ;
  outfile->cd();
  const unsigned int ndirs = dirs.size();

  for ( unsigned int idir = 0; idir < ndirs; ++idir ) {

    TString directory = "srh" + dirs.at(idir);
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

    TH1D* h_gjetyield = (TH1D*) f_zinv->Get(fullhistnameCRyield);
    TH1D* h_zinv_cryield = (TH1D*) f_purity->Get(fullhistname);
    TH1D* h_zinv_mcstat = (TH1D*) f_zinv->Get(fullhistnameRatio);
    TH1D* h_zinv_purity = (TH1D*) f_purity->Get(fullhistnamePurity);
    // When using the integrated estimate (over MT2), should use the integrated purity.
    if (integratedZinvEstimate) {
      h_zinv_mcstat = (TH1D*) f_zinv->Get(fullhistnameRatioInt);
      h_zinv_purity = (TH1D*) f_purity->Get(fullhistnamePurityInt);
    }

    if (h_zinv_mcstat == 0) cout << "Cannot find histogram: " << fullhistnameRatio << endl;
    if (h_zinv_purity == 0) cout << "Cannot find histogram: " << fullhistnamePurity << endl;
    if (h_zinv_cryield == 0) {
      cout << "Cannot find CR histogram in purity.root: " << fullhistname << endl;
      continue;
      // h_zinv_cryield = new TH1D("h_empty", "Empty hist", 1, 0, 1);
    }

    // Make directory and plot(s) in the output file
    TDirectory* dir = 0;
    dir = (TDirectory*)outfile->Get(directory.Data());
    if (dir == 0) {
      dir = outfile->mkdir(directory.Data());
    }
    dir->cd();

    TH1D* pred = (TH1D*) h_zinv->Clone("h_mt2bins");
    TH1D* h_mcyield = (TH1D*) h_zinv->Clone("h_mt2binsMCyield");
    TH1D* h_cryield = (TH1D*) h_zinv_cryield->Clone("h_mt2binsCRyield");
    TH1D* h_gjyield = (TH1D*) h_gjetyield->Clone("h_mt2binsGJyield");
    // TH1D* h_cryield;
    // if (h_zinv_cryield) h_cryield = (TH1D*) h_zinv_cryield->Clone("h_mt2binsCRyield");
    // else h_cryield = new TH1D("h_mt2binsCRyield", "Empty hist", 1, 0, 1);

    for (int mt2bin = 1; mt2bin <= n_mt2bins; ++mt2bin) {
      double n_zinv(0.);
      double n_zinv_cr(0.);
      double err_zinv_mcstat(0.);
      double zinv_ratio_zg(0.);
      double zinv_purity(1.);
      double err_zinv_purity(0.);
      double last_zinv_ratio(.5);

      n_zinv = h_zinv->GetBinContent(mt2bin);
      if (h_zinv_mcstat->GetBinContent(mt2bin) != 0) {
        err_zinv_mcstat = h_zinv_mcstat->GetBinError(mt2bin)/h_zinv_mcstat->GetBinContent(mt2bin);
        zinv_ratio_zg = h_zinv_mcstat->GetBinContent(mt2bin);
        last_zinv_ratio = zinv_ratio_zg;
      } else { // catch zeroes (there shouldn't be any)
        err_zinv_mcstat = 1.;
        zinv_ratio_zg = last_zinv_ratio;
      }
      if (h_zinv_cryield) n_zinv_cr = round(h_zinv_cryield->GetBinContent(mt2bin));
      else n_zinv_cr = 0;
      if (integratedZinvEstimate) n_zinv_cr = round(h_zinv_cryield->Integral(0,-1));

      int mt2bin_tmp = (integratedZinvEstimate)? 1 : mt2bin;
      if (h_zinv_purity->GetBinContent(mt2bin_tmp) != 0) {
        zinv_purity = h_zinv_purity->GetBinContent(mt2bin_tmp);
        err_zinv_purity = h_zinv_purity->GetBinError(mt2bin_tmp)/h_zinv_purity->GetBinContent(mt2bin_tmp);
      }

      double zinv_alpha = 1.;
      double zinv_alphaerr = 1. + err_zinv_mcstat;
      double zinv_purityerr = 1. + err_zinv_purity;

      if (integratedZinvEstimate /* && nbjets >= 2 */) {
        // err_zinv_mcstat, zinv_ratio_zg, n_zinv_cr
        zinv_alpha = zinv_ratio_zg * zinv_purity * 0.92; // 0.92 is a fixed factor for "f = GJetPrompt / (GJetPrompt+QCDPrompt)"
        zinv_alpha *= 0.93; // data-driven correction, based on the double-ratio R(Zll/Gamma)
        if (zinv_alpha > 0.5) zinv_alpha = 0.5; // Hard bound to avoid statistical fluctuations

        n_zinv = n_zinv_cr * zinv_alpha; // don't use Zinv MC as central value any more!
      }

      // filling the histogram
      if (n_zinv > 0) {
        pred->SetBinContent(mt2bin, n_zinv);
        double err_zinv_squares = err_zinv_mcstat*err_zinv_mcstat + err_zinv_purity*err_zinv_purity;
	pred->SetBinError(mt2bin, n_zinv*sqrt(err_zinv_squares));
      } else {
        pred->SetBinContent(mt2bin, 0.);
        pred->SetBinError(mt2bin, h_zinv->GetBinContent(mt2bin));
      }
    }

    pred->Write();
    h_mcyield->Write();
    h_cryield->Write();
    h_gjyield->Write();
  } // loop over signal regions

  return;
}


//_______________________________________________________________________________
void zinvDDMaker(string input_dir = "/home/users/sicheng/MT2Analysis/MT2looper/output/temp"){

  // input_dir = "/home/users/sicheng/MT2Analysis/MT2looper/output/minMTbmet";
  string output_name = input_dir+"/zinvDataDriven.root";
  // ----------------------------------------
  //  samples definition
  // ----------------------------------------

  std::cout << "Writing to file: " << output_name << std::endl;

  // get input files
  TFile* f_zinv = new TFile(Form("%s/zinvFromGJ.root", input_dir.c_str()));
  TFile* f_purity = new TFile(Form("%s/purity.root", input_dir.c_str()));
  TFile* f_zgratio = new TFile(Form("%s/doubleRatio.root", input_dir.c_str()));

  if(f_zinv->IsZombie() || f_zgratio->IsZombie() || f_purity->IsZombie()) {
    std::cerr << "Input file does not exist" << std::endl;
    return;
  }

  vector<string> dirs;

  // Loop through list of every directory in the signal file.
  // if directory begins with "sr", excluding "srbase", add it to vector signal regions.
  TIter it(f_zinv->GetListOfKeys());
  TKey* k;
  std::string keep = "srh";
  std::string skip = "srhbase";
  while ((k = (TKey *)it())) {
    if (strncmp (k->GetTitle(), skip.c_str(), skip.length()) == 0) continue;
    if (strncmp (k->GetTitle(), keep.c_str(), keep.length()) == 0) { // it is a signal region
      std::string sr_string = k->GetTitle();
      sr_string.erase(0, 3);    // remove "srh" from front of string
      dirs.push_back(sr_string);
    }
  }

  combineZinvDataDriven(f_zinv, f_purity, f_zgratio, dirs, output_name);
}
