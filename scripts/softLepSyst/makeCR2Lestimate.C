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
int makeCR2Lpred( TFile* fData , TFile* fMC , TFile* fOut ,  std::string dir_name ) {

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

  TDirectory * dirData = fData ->GetDirectory("cr2L"+srName);
  TDirectory * dirMC   = fMC   ->GetDirectory("cr2L"+srName);

  if (!dirData || !dirMC ) {
    cerr << "ERROR: Directory doesn't exist! Skipping..." << endl;
    return 1;
  }

  //initialize
  TH1D* h_crMC = 0;
  TH1D* h_crMCfake = 0;
  TH1D* h_crMCDilepton = 0;
  TH1D* h_srMCDilepton = 0;
  TH1D* h_crData = 0;
  TH1D* h_crMCInt = 0;
  TH1D* h_crMCfakeInt = 0;
  TH1D* h_crMCDileptonInt = 0;
  TH1D* h_srMCDileptonInt = 0;
  TH1D* h_crDataInt = 0;
  
  h_crMC             = (TH1D*) fMC->Get("cr2L"+srName+"/h_mtbins");              
  h_crMCfake         = (TH1D*) fMC->Get("cr2L"+srName+"/h_mtbinsFake");          
  h_crMCDilepton     = (TH1D*) fMC->Get("cr2L"+srName+"/h_mtbinsDilepton");      
  h_srMCDilepton     = (TH1D*) fMC->Get("srLep"+srName+"/h_mtbinsDilepton");     
  h_crData           = (TH1D*) fData->Get("cr2L"+srName+"/h_mtbins");            
  //integrated
  h_crMCInt          = (TH1D*) fMC->Get("cr2L"+srName+"/h_Events_w");           
  h_crMCfakeInt      = (TH1D*) fMC->Get("cr2L"+srName+"/h_Events_wFake");       
  h_crMCDileptonInt  = (TH1D*) fMC->Get("cr2L"+srName+"/h_Events_wDilepton");   
  h_srMCDileptonInt  = (TH1D*) fMC->Get("srLep"+srName+"/h_Events_wDilepton");   
  h_crDataInt        = (TH1D*) fData->Get("cr2L"+srName+"/h_Events_w");         

  //check for MC hist. should always exist
  if (!h_crMC) {
    cerr << "ERROR: MC hist does not exist in region " << srName << "! Skipping..." << endl;
    return 1;
  }
  
  //reset the data histogram if it doesn't exist (i.e. region is empty). 
  if (!h_crData) h_crData = sameBin(h_crMC, "h_crData");
  if (!h_crDataInt) h_crDataInt = sameBin(h_crMCInt, "h_crDataInt");

  //names
  h_crMC->SetName("h_crMC");
  h_crMCfake->SetName("h_crMCfake");
  h_crMCDilepton->SetName("h_crMCDilepton");
  h_srMCDilepton->SetName("h_srMCDilepton");
  h_crData->SetName("h_crData");
  h_crMCInt->SetName("h_crMCInt");
  h_crMCfakeInt->SetName("h_crMCfakeInt");
  h_crMCDileptonInt->SetName("h_crMCDileptonInt");
  h_srMCDileptonInt->SetName("h_srMCDileptonInt");
  h_crDataInt->SetName("h_crDataInt");
      
  //initialize
  TH1D* h_crMC2 = 0;
  TH1D* h_crMCfake2 = 0;
  TH1D* h_crMCDilepton2 = 0;
  TH1D* h_srMCDilepton2 = 0;
  TH1D* h_crData2 = 0;
  TH1D* h_crMCInt2 = 0;
  TH1D* h_crMCfakeInt2 = 0;
  TH1D* h_crMCDileptonInt2 = 0;
  TH1D* h_srMCDileptonInt2 = 0;
  TH1D* h_crDataInt2 = 0;

  //safety checks to make sure other directories exist
  TDirectory * dirData2 = fData ->GetDirectory("cr2L"+srName2);
  TDirectory * dirMC2   = fMC   ->GetDirectory("cr2L"+srName2);

  //if second HT region exists, add hists
  if (doHTsum && dirData2 && dirMC2) {
    h_crMC2          = (TH1D*) fMC->Get("cr2L"+srName2+"/h_mtbins");          
    h_crMCfake2      = (TH1D*) fMC->Get("cr2L"+srName2+"/h_mtbinsFake");
    h_crMCDilepton2  = (TH1D*) fMC->Get("cr2L"+srName2+"/h_mtbinsDilepton");   
    h_srMCDilepton2  = (TH1D*) fMC->Get("srLep"+srName2+"/h_mtbinsDilepton");
    h_crData2        = (TH1D*) fData->Get("cr2L"+srName2+"/h_mtbins");
    //integrated
    h_crMCInt2          = (TH1D*) fMC->Get("cr2L"+srName2+"/h_Events_w");          
    h_crMCfakeInt2      = (TH1D*) fMC->Get("cr2L"+srName2+"/h_Events_wFake");       
    h_crMCDileptonInt2  = (TH1D*) fMC->Get("cr2L"+srName2+"/h_Events_wDilepton");  
    h_srMCDileptonInt2  = (TH1D*) fMC->Get("srLep"+srName2+"/h_Events_wDilepton");  
    h_crDataInt2        = (TH1D*) fData->Get("cr2L"+srName2+"/h_Events_w");
  
    if (!h_crData2) h_crData = sameBin(h_crMC2, "h_crData2");
    if (!h_crDataInt2) h_crDataInt = sameBin(h_crMCInt2, "h_crDataInt2");
    
    if (h_crMC2) h_crMC->Add(h_crMC2);
    if (h_crMCfake2) h_crMCfake->Add(h_crMCfake2);
    if (h_crMCDilepton2) h_crMCDilepton->Add(h_crMCDilepton2);
    if (h_srMCDilepton2) h_srMCDilepton->Add(h_srMCDilepton2);
    if (h_crData2) h_crData->Add(h_crData2);
    if (h_crMCInt2) h_crMCInt->Add(h_crMCInt2);
    if (h_crMCfakeInt2) h_crMCfakeInt->Add(h_crMCfakeInt2);
    if (h_crMCDileptonInt2) h_crMCDileptonInt->Add(h_crMCDileptonInt2);
    if (h_srMCDileptonInt2) h_srMCDileptonInt->Add(h_srMCDileptonInt2);
    if (h_crDataInt2) h_crDataInt->Add(h_crDataInt2);
  }
  else if (doHTsum && (!dirData2 || !dirMC2)) {
    cerr << "WARNING: Additional HT directory " << srName2 << " does not exist! Make sure this is expected..." << endl;
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
  TH1D* h_k_ht = sameBin(h_crMCInt, "h_k_ht");
  if (doHTsum) {
    h_k_ht->Add(h_srMCDileptonInt,h_srMCDileptonInt2,1,-1);
    h_k_ht->Divide(h_srMCDileptonInt);
  }
  
  //normalized histogram to extrapolate in MT, given HT bin
  TH1D* h_k_mt = sameBin(h_crMC, "h_k_mt");
  if (doHTsum) h_k_mt->Add(h_srMCDilepton,h_srMCDilepton2,1,-1);
  else h_k_mt->Add(h_srMCDilepton);
  h_k_mt->Scale(1/h_k_mt->Integral(0,-1));
  
  //initialize pred histogram
  TH1D* h_pred = sameBin(h_crMC, "h_mtbins");

  //calculate the prediction in each bin
  for (int ibin = 0; ibin <= h_crData->GetSize(); ibin++) {

    //get bin contents
    double binDataCR         = h_crDataInt  ->GetBinContent(1);
    double binRatio          = h_ratioInt   ->GetBinContent(1);
    double binPurity         = h_purityInt  ->GetBinContent(1);
    //double binPurity         = h_purity  ->GetEfficiency(ibin);
    double bink_ht           = h_k_ht  ->GetBinContent(1);
    double bink_mt           = h_k_mt  ->GetBinContent(ibin);

    //get bin errors
    double binDataCR_err     = ( binDataCR != 0) ? h_crDataInt ->GetBinError(1) / binDataCR : 0;
    double binRatio_err      = ( binRatio != 0) ? h_ratioInt ->GetBinError(1) / binRatio : 0;
    double binPurity_err     = ( binPurity != 0) ? h_purityInt ->GetBinError(1) / binPurity : 0;
    // double binPurity_errL    = h_purity  ->GetEfficiencyErrorLow(ibin); binPurity_errL = (binPurity_errL > 0) ? binPurity_errL : 0;
    // double binPurity_errU    = h_purity  ->GetEfficiencyErrorUp(ibin); binPurity_errU = (binPurity_errU > 0) ? binPurity_errU : 0;
    // double binPurity_err     = max(binPurity_errL, binPurity_errU);  //for now, just take larger of asymmetric errors
    double bink_ht_err       = ( bink_ht != 0) ? h_k_ht ->GetBinError(1) / bink_ht : 0;
    double bink_mt_err       = ( bink_mt != 0) ? h_k_mt ->GetBinError(ibin) / bink_mt : 0;
    
    //calculate pred = N(CR, data) * Ratio(SR/CR, mc) * Purity * k_ht * k_mt
    double binPred = -1;
    if (doHTsum) binPred = binDataCR * binRatio * binPurity * bink_ht * bink_mt;
    else binPred = binDataCR * binRatio * binPurity * bink_mt;
    
    //calculate total error in quadrature
    double binErrSq = binDataCR_err * binDataCR_err + binRatio_err * binRatio_err + binPurity_err * binPurity_err + bink_ht_err * bink_ht_err + bink_mt_err * bink_mt_err;

    //set bin content/error
    h_pred->SetBinContent(ibin, binPred);
    h_pred->SetBinError(ibin, binPred*sqrt(binErrSq));     

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
  h_k_ht            ->Write();
  h_k_mt            ->Write();
  h_pred            ->Write();

  return 0;
  
}

//_______________________________________________________________________________
void makeCR2Lestimate(string input_dir = "../../SoftLepLooper/output/softLep", string dataname = "data"){


  string output_name = input_dir+"/pred_CR2L.root";
  // ----------------------------------------
  //  samples definition
  // ----------------------------------------

  std::cout << "Writing to file: " << output_name << std::endl;

  TFile * f_out = new TFile(output_name.c_str(),"RECREATE") ; 
  
  // get input files
  TString datanamestring(dataname);

  if (datanamestring.Contains("Data") || datanamestring.Contains("data")) isData = true;
  TFile* f_data = new TFile(Form("%s/%s.root",input_dir.c_str(),dataname.c_str())); //data or dummy-data file
  TFile* f_mc   = new TFile(Form("%s/allBkg.root",input_dir.c_str()));


  if(f_data->IsZombie() || f_mc->IsZombie()) {
    std::cerr << "Input file does not exist" << std::endl;
    return;
  }

  TIter it(f_data->GetListOfKeys());
  TKey* k;
  while ((k = (TKey *)it())) {
    std::string dir_name = k->GetTitle();
    if(dir_name.find("srLep")==std::string::npos) continue; //to do only signal regions
    cout << "----- Calculating prediction for " << dir_name << " -----" << endl;
    makeCR2Lpred( f_data , f_mc , f_out , dir_name );
  }

  return;
  
}

