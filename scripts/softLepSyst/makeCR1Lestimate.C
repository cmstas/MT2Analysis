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
int makeCR1Lpred( TFile* fData , TFile* fMC , TFile* fOut ,  std::string dir_name ) {

  fOut->cd();

  //get SR suffix
  TString srName = dir_name;
  srName.ReplaceAll("srLep","");

  TDirectory * dirData = fData ->GetDirectory("cr1L"+srName);
  TDirectory * dirMC   = fMC   ->GetDirectory("cr1L"+srName);

  if (!dirData || !dirMC ) {
    cerr << "ERROR: Directory doesn't exist! Skipping..." << endl;
    return 1;
  }

  //initialize map of histograms
  std::map<string, TH1D*> histMap;
  histMap["h_crMC"]           = (TH1D*) fMC->Get("cr1L"+srName+"/h_mtbins");           
  histMap["h_crMCfake"]       = (TH1D*) fMC->Get("cr1L"+srName+"/h_mtbinsFake");     
  histMap["h_crMConelep"]     = (TH1D*) fMC->Get("cr1L"+srName+"/h_mtbinsOnelep");    
  histMap["h_srMConelep"]     = (TH1D*) fMC->Get("srLep"+srName+"/h_mtbinsOnelep");  
  histMap["h_crData"]         = (TH1D*) fData->Get("cr1L"+srName+"/h_mtbins");      
  //integrated
  histMap["h_crMCInt"]        = (TH1D*) fMC->Get("cr1L"+srName+"/h_Events_w");         
  histMap["h_crMCfakeInt"]    = (TH1D*) fMC->Get("cr1L"+srName+"/h_Events_wFake");    
  histMap["h_crMConelepInt"]  = (TH1D*) fMC->Get("cr1L"+srName+"/h_Events_wOnelep");   
  histMap["h_srMConelepInt"]  = (TH1D*) fMC->Get("srLep"+srName+"/h_Events_wOnelep"); 
  histMap["h_crDataInt"]      = (TH1D*) fData->Get("cr1L"+srName+"/h_Events_w");  

  //check for empty hists, set names
  for ( std::map<string, TH1D*>::iterator iter = histMap.begin(); iter != histMap.end(); ++iter ) {
    if (!(iter->second)){
      cerr << "WARNING: hist " << iter->first << " does not exist in region " << srName << "! Setting to 0..." << endl;
      iter->second = sameBin(histMap["h_crMC"], "h");
    }
    iter->second->SetName(iter->first.c_str());
  }
		  
  //calculate the ratio histogram, N(SR/CR) in MC
  histMap["h_ratio"] = sameBin(histMap["h_crMC"], "h_ratio");
  histMap["h_ratio"]->Divide(histMap["h_srMConelep"], histMap["h_crMConelep"]);
  histMap["h_ratioInt"] = sameBin(histMap["h_crMCInt"], "h_ratioInt");
  histMap["h_ratioInt"]->Divide(histMap["h_srMConelepInt"], histMap["h_crMConelepInt"]);

  //calculate the purity histogram, N(Fake/Total) in CR, directly from MC
  histMap["h_purity"] = sameBin(histMap["h_crMC"], "h_purity");
  histMap["h_purity"]->Divide(histMap["h_crMConelep"],histMap["h_crMC"]);
  histMap["h_purityInt"] = sameBin(histMap["h_crMCInt"], "h_purityInt");
  histMap["h_purityInt"]->Divide(histMap["h_crMConelepInt"],histMap["h_crMCInt"]);   
  // TEfficiency* h_purity = new TEfficiency( *h_crMConelep,  *h_crMC);
  // h_purity->SetName("h_purity");

  //normalized histogram to extrapolate in MT
  histMap["h_kMT"] = sameBin(histMap["h_crMC"], "h_kMT");
  histMap["h_kMT"]->Add(histMap["h_srMConelep"]);
  histMap["h_kMT"]->Scale(1/histMap["h_kMT"]->Integral(0,-1));
  
  //initialize pred histogram
  histMap["h_pred"] = sameBin(histMap["h_crMC"], "h_mtbins");

  //calculate the prediction in each bin
  for (int ibin = 0; ibin <= histMap["h_crData"]->GetSize(); ibin++) {

    //get bin contents
    double binDataCR         = histMap["h_crDataInt"]  ->GetBinContent(1);
    double binRatio          = histMap["h_ratioInt"]   ->GetBinContent(1);
    double binPurity         = histMap["h_purityInt"]  ->GetBinContent(1);
    //double binPurity         = h_purity  ->GetEfficiency(ibin);
    double binkMT            = histMap["h_kMT"]    ->GetBinContent(ibin);

    //get bin errors
    double binDataCR_err     = (binDataCR != 0) ? histMap["h_crDataInt"]  ->GetBinError(1) / binDataCR : 0;
    double binRatio_err      = (binRatio != 0)  ? histMap["h_ratioInt"]   ->GetBinError(1) / binRatio : 0;
    double binPurity_err     = (binPurity != 0) ? histMap["h_purityInt"]  ->GetBinError(1) / binPurity : 0;
    // double binPurity_errL    = h_purity  ->GetEfficiencyErrorLow(ibin); binPurity_errL = (binPurity_errL > 0) ? binPurity_errL : 0;
    // double binPurity_errU    = h_purity  ->GetEfficiencyErrorUp(ibin); binPurity_errU = (binPurity_errU > 0) ? binPurity_errU : 0;
    // double binPurity_err     = max(binPurity_errL, binPurity_errU);  //for now, just take larger of asymmetric errors
    double binkMT_err        = (binkMT != 0)    ? histMap["h_kMT"]        ->GetBinError(ibin) / binkMT : 0;
    
    //calculate pred = N(CR, data) * Ratio(SR/CR, mc) * Purity * k(MT)
    double binPred = binDataCR * binRatio * binPurity * binkMT;
      
    //calculate total error in quadrature
    double binErrSq = binDataCR_err * binDataCR_err + binRatio_err * binRatio_err + binPurity_err * binPurity_err + binkMT_err * binkMT_err;

    //set bin content/error
    histMap["h_pred"]->SetBinContent(ibin, binPred);
    histMap["h_pred"]->SetBinError(ibin, binPred*sqrt(binErrSq));     

  }//loop over bins

  //write hists to output file
  cout << "Saving hists for " << dir_name << "..." << endl;
  fOut->cd();
  TDirectory* dir = fOut->mkdir(dir_name.c_str());
  dir->cd();
  for ( std::map<string, TH1D*>::iterator iter = histMap.begin(); iter != histMap.end(); ++iter ) {
    if (iter->second) iter->second->Write();
    delete iter->second;
  }

  
  return 0;
  
}

//_______________________________________________________________________________
void makeCR1Lestimate(string input_dir = "../../SoftLepLooper/output/softLep/", string dataname = "data"){


  string output_name = input_dir+"/pred_CR1L.root";
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
    cout << "Calculating prediction for " << dir_name << "..." << endl;
    makeCR1Lpred( f_data , f_mc , f_out , dir_name );
  }

  return;
  
}

