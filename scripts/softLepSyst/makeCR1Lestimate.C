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
#include "TDirectory.h"
#include "TH1.h"
#include "TEfficiency.h"
#include "TPaveText.h"
#include "TKey.h"

using namespace std;

bool isData = false;

//returns an empty hist with same binning
inline TH1D* sameBin(TH1D* h_in, std::string name)
{
  TH1D* h_out = (TH1D*) h_in->Clone();
  h_out->Reset(); //clear hist
  h_out->SetName(name.c_str()); //set name
  h_out->GetSumw2()->Set(0); h_out->Sumw2(); // reset sumw2
  return h_out;
}

//_______________________________________________________________________________
int makeCR1Lpred( TFile* fData_CR , TFile* fMC_CR , TFile* fMC_SR , TFile* fOut ,  std::string dir_name ) {

  fOut->cd();

  //get SR suffix
  TString srName = dir_name;
  srName.ReplaceAll("srLep","");

  TDirectory * dirData = fData_CR ->GetDirectory("cr1L"+srName);
  TDirectory * dirMC   = fMC_CR   ->GetDirectory("cr1L"+srName);
  TDirectory * dirMC2  = fMC_SR   ->GetDirectory("cr1L"+srName);

  if (!dirData || !dirMC || !dirMC2) {
    cerr << "ERROR: Directory doesn't exist! Skipping..." << endl;
    return 1;
  }
  
  //get relevant histograms
  TH1D* h_crMC           = (TH1D*) fMC_CR->Get("cr1L"+srName+"/h_mtbins");            h_crMC->SetName("h_crMC");
  TH1D* h_crMCfake       = (TH1D*) fMC_CR->Get("cr1L"+srName+"/h_mtbinsFake");        h_crMCfake->SetName("h_crMCfake");
  TH1D* h_crMConelep     = (TH1D*) fMC_CR->Get("cr1L"+srName+"/h_mtbinsOnelep");      h_crMConelep->SetName("h_crMConelep");
  TH1D* h_srMConelep     = (TH1D*) fMC_SR->Get("srLep"+srName+"/h_mtbinsOnelep");     h_srMConelep->SetName("h_srMConelep"); 
  TH1D* h_crData         = (TH1D*) fData_CR->Get("cr1L"+srName+"/h_mtbins");          h_crData->SetName("h_crData");
  //integrated
  TH1D* h_crMCInt        = (TH1D*) fMC_CR->Get("cr1L"+srName+"/h_Events_w");          h_crMCInt->SetName("h_crMCInt");
  TH1D* h_crMCfakeInt    = (TH1D*) fMC_CR->Get("cr1L"+srName+"/h_Events_wFake");      h_crMCfakeInt->SetName("h_crMCfakeInt");
  TH1D* h_crMConelepInt  = (TH1D*) fMC_CR->Get("cr1L"+srName+"/h_Events_wOnelep");    h_crMConelepInt->SetName("h_crMConelepInt");
  TH1D* h_srMConelepInt  = (TH1D*) fMC_SR->Get("srLep"+srName+"/h_Events_wOnelep");   h_srMConelepInt->SetName("h_srMConelepInt"); 
  TH1D* h_crDataInt      = (TH1D*) fData_CR->Get("cr1L"+srName+"/h_Events_w");        h_crDataInt->SetName("h_crDataInt");
  
  //calculate the ratio histogram, N(SR/CR) in MC
  TH1D* h_ratio = sameBin(h_crMC, "h_ratio");
  h_ratio->Divide(h_srMConelep, h_crMConelep);
  TH1D* h_ratioInt = sameBin(h_crMCInt, "h_ratioInt");
  h_ratioInt->Divide(h_srMConelepInt, h_crMConelepInt);

  //calculate the purity histogram, N(Fake/Total) in CR, directly from MC
  TH1D* h_purity = sameBin(h_crMC, "h_purity");
  h_purity->Divide(h_crMConelep,h_crMC);
  TH1D* h_purityInt = sameBin(h_crMCInt, "h_purityInt");
  h_purityInt->Divide(h_crMConelepInt,h_crMCInt);   
  // TEfficiency* h_purity = new TEfficiency( *h_crMConelep,  *h_crMC);
  // h_purity->SetName("h_purity");

  //normalized histogram to extrapolate in MT
  TH1D* h_kFactor = sameBin(h_crMC, "h_kFactor");
  h_kFactor->Add(h_srMConelep);
  h_kFactor->Scale(1/h_kFactor->Integral(0,-1));
  
  //initialize pred histogram
  TH1D* h_pred = sameBin(h_crMC, "h_pred");

  //calculate the prediction in each bin
  for (int ibin = 0; ibin <= h_crData->GetSize(); ibin++) {

    //get bin contents
    double binDataCR         = h_crDataInt  ->GetBinContent(1);
    double binRatio          = h_ratioInt   ->GetBinContent(1);
    double binPurity         = h_purityInt  ->GetBinContent(1);
    //double binPurity         = h_purity  ->GetEfficiency(ibin);
    double binkFactor        = h_kFactor    ->GetBinContent(ibin);

    //get bin errors
    double binDataCR_err     = h_crDataInt  ->GetBinError(1);
    double binRatio_err      = h_ratioInt   ->GetBinError(1);
    double binPurity_err     = h_purityInt  ->GetBinError(1);
    // double binPurity_errL    = h_purity  ->GetEfficiencyErrorLow(ibin); binPurity_errL = (binPurity_errL > 0) ? binPurity_errL : 0;
    // double binPurity_errU    = h_purity  ->GetEfficiencyErrorUp(ibin); binPurity_errU = (binPurity_errU > 0) ? binPurity_errU : 0;
    // double binPurity_err     = max(binPurity_errL, binPurity_errU);  //for now, just take larger of asymmetric errors
    double binkFactor_err    = h_kFactor    ->GetBinError(ibin);
    
    //calculate pred = N(CR, data) * Ratio(SR/CR, mc) * Purity * k-Factor (MT)
    double binPred = binDataCR * binRatio * binPurity * binkFactor;
      
    //calculate total error in quadrature
    double binErrSq = binDataCR_err * binDataCR_err + binRatio_err * binRatio_err + binPurity_err * binPurity_err + binkFactor_err * binkFactor_err;

    //set bin content/error
    h_pred->SetBinContent(ibin, binPred);
    h_pred->SetBinError(ibin, sqrt(binErrSq));     

  }//loop over bins

  //write hists to output file
  cout << "Saving hists for " << dir_name << "..." << endl;
  fOut->cd();
  TDirectory* dir = fOut->mkdir(dir_name.c_str());
  dir->cd();
  h_crMC          ->Write();
  h_crMConelep    ->Write();
  h_srMConelep    ->Write();
  h_crData        ->Write();
  h_ratio         ->Write();
  h_purity        ->Write();
  h_crMCInt       ->Write();
  h_crMConelepInt ->Write();
  h_srMConelepInt ->Write();
  h_crDataInt     ->Write();
  h_ratioInt      ->Write();
  h_purityInt     ->Write();
  h_kFactor       ->Write();
  h_pred          ->Write();

  return 0;
  
}

//_______________________________________________________________________________
void makeCR1Lestimate(string input_dir = "../../SoftLepLooper/output", string dataname = "data"){


  string output_name = input_dir+"/pred_CR1L.root";
  // ----------------------------------------
  //  samples definition
  // ----------------------------------------

  std::cout << "Writing to file: " << output_name << std::endl;

  TFile * f_out = new TFile(output_name.c_str(),"RECREATE") ; 
  
  // get input files
  TString datanamestring(dataname);

  if (datanamestring.Contains("Data") || datanamestring.Contains("data")) isData = true;
  TFile* f_dataCR = new TFile(Form("%s/softLepCR/%s.root",input_dir.c_str(),dataname.c_str())); //data or dummy-data file
  TFile* f_mcCR   = new TFile(Form("%s/softLepCR/allBkg.root",input_dir.c_str()));
  TFile* f_mcSR   = new TFile(Form("%s/softLepSR/allBkg.root",input_dir.c_str()));


  if(f_dataCR->IsZombie() || f_mcCR->IsZombie() || f_mcSR->IsZombie()) {
    std::cerr << "Input file does not exist" << std::endl;
    return;
  }

  TIter it(f_dataCR->GetListOfKeys());
  TKey* k;
  while ((k = (TKey *)it())) {
    std::string dir_name = k->GetTitle();
    if(dir_name.find("srLep")==std::string::npos) continue; //to do only signal regions
    cout << "Calculating prediction for " << dir_name << "..." << endl;
    makeCR1Lpred( f_dataCR , f_mcCR , f_mcSR , f_out , dir_name );
  }

  return;
  
}

