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
int makeCR2Lpred( TFile* fData_SR , TFile* fMC_SR , TFile* fOut ,  std::string dir_name ) {

  fOut->cd();

  //get SR suffix
  TString srName = dir_name;
  srName.ReplaceAll("srLep","");

  //get other HT region
  TString srName2 = srName;
  if (srName.Contains("L")) srName2.ReplaceAll("L","M");
  else if (srName.Contains("M")) srName2.ReplaceAll("M","L");
  else srName2 = "";
  bool doHTsum = srName2 != "";
  if (doHTsum)  cout << "Summing over additional HT regions: Adding " << srName2 << " to " << srName << "..." << endl;

  TDirectory * dirData = fData_SR ->GetDirectory("cr2L"+srName);
  TDirectory * dirMC   = fMC_SR   ->GetDirectory("cr2L"+srName);

  if (!dirData || !dirMC ) {
    cerr << "ERROR: Directory doesn't exist! Skipping..." << endl;
    return 1;
  }
    
  //get relevant histograms
  TH1D* h_crMC             = (TH1D*) fMC_SR->Get("cr2L"+srName+"/h_mtbins");              h_crMC->SetName("h_crMC");
  TH1D* h_crMCfake         = (TH1D*) fMC_SR->Get("cr2L"+srName+"/h_mtbinsFake");          h_crMCfake->SetName("h_crMCfake");
  TH1D* h_crMCDilepton     = (TH1D*) fMC_SR->Get("cr2L"+srName+"/h_mtbinsDilepton");      h_crMCDilepton->SetName("h_crMCDilepton");
  TH1D* h_srMCDilepton     = (TH1D*) fMC_SR->Get("srLep"+srName+"/h_mtbinsDilepton");     h_srMCDilepton->SetName("h_srMCDilepton"); 
  TH1D* h_crData           = (TH1D*) fData_SR->Get("cr2L"+srName+"/h_mtbins");            h_crData->SetName("h_crData");
  //integrated
  TH1D* h_crMCInt          = (TH1D*) fMC_SR->Get("cr2L"+srName+"/h_Events_w");            h_crMCInt->SetName("h_crMCInt");
  TH1D* h_crMCfakeInt      = (TH1D*) fMC_SR->Get("cr2L"+srName+"/h_Events_wFake");        h_crMCfakeInt->SetName("h_crMCfakeInt");
  TH1D* h_crMCDileptonInt  = (TH1D*) fMC_SR->Get("cr2L"+srName+"/h_Events_wDilepton");    h_crMCDileptonInt->SetName("h_crMCDileptonInt");
  TH1D* h_srMCDileptonInt  = (TH1D*) fMC_SR->Get("srLep"+srName+"/h_Events_wDilepton");   h_srMCDileptonInt->SetName("h_srMCDileptonInt"); 
  TH1D* h_crDataInt        = (TH1D*) fData_SR->Get("cr2L"+srName+"/h_Events_w");          h_crDataInt->SetName("h_crDataInt");

  //if second HT region exists, add hists
  TH1D* h_crMC2 = 0;
  TH1D* h_crMCfake2 = 0;
  TH1D* h_crMCDilepton2 = 0;
  TH1D* h_srMCDilepton2 = 0;
  TH1D* h_crData2 = 0;
  //integrated
  TH1D* h_crMCInt2 = 0;
  TH1D* h_crMCfakeInt2 = 0;
  TH1D* h_crMCDileptonInt2 = 0;
  TH1D* h_srMCDileptonInt2 = 0;
  TH1D* h_crDataInt2 = 0;

  //safety checks to make sure other directories exist
  TDirectory * dirData2 = fData_SR ->GetDirectory("cr2L"+srName2);
  TDirectory * dirMC2   = fMC_SR   ->GetDirectory("cr2L"+srName2);
  
  if (doHTsum && dirData2 && dirMC2) {
    h_crMC2          = (TH1D*) fMC_SR->Get("cr2L"+srName2+"/h_mtbins");            h_crMC2->SetName("h_crMC2");
    h_crMCfake2      = (TH1D*) fMC_SR->Get("cr2L"+srName2+"/h_mtbinsFake");        h_crMCfake2->SetName("h_crMCfake2");
    h_crMCDilepton2  = (TH1D*) fMC_SR->Get("cr2L"+srName2+"/h_mtbinsDilepton");    h_crMCDilepton2->SetName("h_crMCDilepton2");
    h_srMCDilepton2  = (TH1D*) fMC_SR->Get("srLep"+srName2+"/h_mtbinsDilepton");   h_srMCDilepton2->SetName("h_srMCDilepton2"); 
    h_crData2        = (TH1D*) fData_SR->Get("cr2L"+srName2+"/h_mtbins");          h_crData2->SetName("h_crData2");
    //integrated
    h_crMCInt2          = (TH1D*) fMC_SR->Get("cr2L"+srName2+"/h_Events_w");            h_crMCInt2->SetName("h_crMCInt2");
    h_crMCfakeInt2      = (TH1D*) fMC_SR->Get("cr2L"+srName2+"/h_Events_wFake");        h_crMCfakeInt2->SetName("h_crMCfakeInt2");
    h_crMCDileptonInt2  = (TH1D*) fMC_SR->Get("cr2L"+srName2+"/h_Events_wDilepton");    h_crMCDileptonInt2->SetName("h_crMCDileptonInt2");
    h_srMCDileptonInt2  = (TH1D*) fMC_SR->Get("srLep"+srName2+"/h_Events_wDilepton");   h_srMCDileptonInt2->SetName("h_srMCDileptonInt2"); 
    h_crDataInt2        = (TH1D*) fData_SR->Get("cr2L"+srName2+"/h_Events_w");          h_crDataInt2->SetName("h_crDataInt2");

    h_crMC->Add(h_crMC2);
    h_crMCfake->Add(h_crMCfake2);
    h_crMCDilepton->Add(h_crMCDilepton2);
    h_srMCDilepton->Add(h_srMCDilepton2);
    h_crData->Add(h_crData2);
    h_crMCInt->Add(h_crMCInt2);
    h_crMCfakeInt->Add(h_crMCfakeInt2);
    h_crMCDileptonInt->Add(h_crMCDileptonInt2);
    h_srMCDileptonInt->Add(h_srMCDileptonInt2);
    h_crDataInt->Add(h_crDataInt2);
  }
  else if (doHTsum && (!dirData2 || !dirMC2)) {
    cerr << "WARNING: Additional HT directory does not exist! Make sure this is expected..." << endl;
    doHTsum = false;
  }
  
  //calculate the ratio histogram, N(SR/CR) in MC
  TH1D* h_ratio = sameBin(h_crMC, "h_ratio");
  h_ratio->Divide(h_srMCDilepton, h_crMCDilepton);
  TH1D* h_ratioInt = sameBin(h_crMCInt, "h_ratioInt");
  h_ratioInt->Divide(h_srMCDileptonInt, h_crMCDileptonInt);

  //calculate the purity histogram, N(Fake/Total) in CR, directly from MC
  TH1D* h_purity = sameBin(h_crMC, "h_purity");
  h_purity->Divide(h_crMCDilepton,h_crMC);
  TH1D* h_purityInt = sameBin(h_crMCInt, "h_purityInt");
  h_purityInt->Divide(h_crMCDileptonInt,h_crMCInt);   
  // TEfficiency* h_purity = new TEfficiency( *h_crMCDilepton,  *h_crMC);
  // h_purity->SetName("h_purity");

  //normalized histogram to extrapolate in HT
  TH1D* h_kFactor_ht = sameBin(h_crMCInt, "h_kFactor_ht");
  if (doHTsum) {
    h_kFactor_ht->Add(h_srMCDileptonInt,h_srMCDileptonInt2,1,-1);
    h_kFactor_ht->Divide(h_srMCDileptonInt);
  }
  
  //normalized histogram to extrapolate in MT, given HT bin
  TH1D* h_kFactor_mt = sameBin(h_crMC, "h_kFactor_mt");
  if (doHTsum) h_kFactor_mt->Add(h_srMCDilepton,h_srMCDilepton2,1,-1);
  else h_kFactor_mt->Add(h_srMCDilepton);
  h_kFactor_mt->Scale(1/h_kFactor_mt->Integral(0,-1));
  
  //initialize pred histogram
  TH1D* h_pred = sameBin(h_crMC, "h_pred");

  //calculate the prediction in each bin
  for (int ibin = 0; ibin <= h_crData->GetSize(); ibin++) {

    //get bin contents
    double binDataCR         = h_crDataInt  ->GetBinContent(1);
    double binRatio          = h_ratioInt   ->GetBinContent(1);
    double binPurity         = h_purityInt  ->GetBinContent(1);
    //double binPurity         = h_purity  ->GetEfficiency(ibin);
    double binkFactor_ht     = h_kFactor_ht  ->GetBinContent(1);
    double binkFactor_mt     = h_kFactor_mt  ->GetBinContent(ibin);

    //get bin errors
    double binDataCR_err     = h_crDataInt  ->GetBinError(1);
    double binRatio_err      = h_ratioInt   ->GetBinError(1);
    double binPurity_err     = h_purityInt  ->GetBinError(1);
    // double binPurity_errL    = h_purity  ->GetEfficiencyErrorLow(ibin); binPurity_errL = (binPurity_errL > 0) ? binPurity_errL : 0;
    // double binPurity_errU    = h_purity  ->GetEfficiencyErrorUp(ibin); binPurity_errU = (binPurity_errU > 0) ? binPurity_errU : 0;
    // double binPurity_err     = max(binPurity_errL, binPurity_errU);  //for now, just take larger of asymmetric errors
    double binkFactor_ht_err = h_kFactor_ht ->GetBinError(1);
    double binkFactor_mt_err = h_kFactor_mt ->GetBinError(ibin);
    
    //calculate pred = N(CR, data) * Ratio(SR/CR, mc) * Purity * k_ht * k_mt
    double binPred = -1;
    if (doHTsum) binPred = binDataCR * binRatio * binPurity * binkFactor_ht * binkFactor_mt;
    else binPred = binDataCR * binRatio * binPurity * binkFactor_mt;
    
    //calculate total error in quadrature
    double binErrSq = binDataCR_err * binDataCR_err + binRatio_err * binRatio_err + binPurity_err * binPurity_err + binkFactor_ht_err * binkFactor_ht_err + binkFactor_mt_err * binkFactor_mt_err;

    //set bin content/error
    h_pred->SetBinContent(ibin, binPred);
    h_pred->SetBinError(ibin, sqrt(binErrSq));     

  }//loop over bins

  //write hists to output file
  cout << "Saving hists for " << dir_name << "..." << endl;
  fOut->cd();
  TDirectory* dir = fOut->mkdir(dir_name.c_str());
  dir->cd();
  h_crMC            ->Write();
  h_crMCDilepton    ->Write();
  h_srMCDilepton    ->Write();
  h_crData          ->Write();
  h_ratio           ->Write();
  h_purity          ->Write();
  h_crMCInt         ->Write();
  h_crMCDileptonInt ->Write();
  h_srMCDileptonInt ->Write();
  h_crDataInt       ->Write();
  h_ratioInt        ->Write();
  h_purityInt       ->Write();
  h_kFactor_ht      ->Write();
  h_kFactor_mt      ->Write();
  h_pred            ->Write();

  return 0;
  
}

//_______________________________________________________________________________
void makeCR2Lestimate(string input_dir = "../../SoftLepLooper/output", string dataname = "data"){


  string output_name = input_dir+"/pred_CR2L.root";
  // ----------------------------------------
  //  samples definition
  // ----------------------------------------

  std::cout << "Writing to file: " << output_name << std::endl;

  TFile * f_out = new TFile(output_name.c_str(),"RECREATE") ; 
  
  // get input files
  TString datanamestring(dataname);

  if (datanamestring.Contains("Data") || datanamestring.Contains("data")) isData = true;
  TFile* f_dataSR = new TFile(Form("%s/softLepSR/%s.root",input_dir.c_str(),dataname.c_str())); //data or dummy-data file
  TFile* f_mcSR   = new TFile(Form("%s/softLepSR/allBkg.root",input_dir.c_str()));


  if(f_dataSR->IsZombie() || f_mcSR->IsZombie()) {
    std::cerr << "Input file does not exist" << std::endl;
    return;
  }

  TIter it(f_dataSR->GetListOfKeys());
  TKey* k;
  while ((k = (TKey *)it())) {
    std::string dir_name = k->GetTitle();
    if(dir_name.find("srLep")==std::string::npos) continue; //to do only signal regions
    cout << "----- Calculating prediction for " << dir_name << " -----" << endl;
    makeCR2Lpred( f_dataSR , f_mcSR , f_out , dir_name );
  }

  return;
  
}

