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
int makeCR2Lpred( TFile* fData , TFile* fMC , TFile* fOut ,  std::string dir_name ) {

  fOut->cd();

  //get SR suffix
  TString srName = dir_name;
  srName.ReplaceAll("srLep","");

  TDirectory * dirData = fData ->GetDirectory("cr2L"+srName);
  TDirectory * dirMC   = fMC   ->GetDirectory("cr2L"+srName);

  if (!dirData || !dirMC ) {
    cerr << "ERROR: Directory doesn't exist! Skipping..." << endl;
    return 1;
  }

  //get other MET region
  TString srName2 = srName;
  
  if (srName.Contains("L")) srName2.ReplaceAll("L","M");
  else if (srName.Contains("M")) srName2.ReplaceAll("M","L");
  else srName2 = "";
  bool doMETsum = srName2 != "";
  if (doMETsum)  cout << "Summing over additional MET regions: Adding " << srName2 << " to " << srName << "..." << endl;
  
  //initialize map of histograms
  std::map<string, TH1D*> histMap;
  histMap["h_crMC"]             = (TH1D*) fMC->Get("cr2L"+srName+"/h_mtbinsAlt");           
  histMap["h_crMCfake"]         = (TH1D*) fMC->Get("cr2L"+srName+"/h_mtbinsAltFake");     
  histMap["h_crMCDilepton"]     = (TH1D*) fMC->Get("cr2L"+srName+"/h_mtbinsAltDilepton");    
  histMap["h_srMCDilepton"]     = (TH1D*) fMC->Get("srLep"+srName+"/h_mtbinsDilepton");  
  histMap["h_crData"]           = (TH1D*) fData->Get("cr2L"+srName+"/h_mtbinsAlt");      
  //integrated
  histMap["h_crMCInt"]          = (TH1D*) fMC->Get("cr2L"+srName+"/h_Events_wAlt");         
  histMap["h_crMCfakeInt"]      = (TH1D*) fMC->Get("cr2L"+srName+"/h_Events_wAltFake");    
  histMap["h_crMCDileptonInt"]  = (TH1D*) fMC->Get("cr2L"+srName+"/h_Events_wAltDilepton");   
  histMap["h_srMCDileptonInt"]  = (TH1D*) fMC->Get("srLep"+srName+"/h_Events_wDilepton"); 
  histMap["h_crDataInt"]        = (TH1D*) fData->Get("cr2L"+srName+"/h_Events_wAlt");

  //check for MC hist. should always exist
  if (!histMap["h_srMCDilepton"] || !histMap["h_srMCDileptonInt"]) {
    cerr << "ERROR: MC hist does not exist in region " << srName << "! Skipping..." << endl;
    return 1;
  }

  //check for empty hists, set names
  for ( std::map<string, TH1D*>::iterator iter = histMap.begin(); iter != histMap.end(); ++iter ) {
    if (!(iter->second)){
      cerr << "WARNING: hist " << iter->first << " does not exist in region " << srName << "! Setting to 0..." << endl;
	if (iter->first.find("Int") != std::string::npos) iter->second = sameBin(histMap["h_srMCDileptonInt"], "h");
	else iter->second = sameBin(histMap["h_srMCDilepton"], "h");
    }
    iter->second->SetName(iter->first.c_str());
  }

  
  //initialize map of histograms for 2nd SR 
  std::map<string, TH1D*> histMap2;
  histMap2["h_crMC"]             = 0;           
  histMap2["h_crMCfake"]         = 0;     
  histMap2["h_crMCDilepton"]     = 0;    
  histMap2["h_srMCDilepton"]     = 0;  
  histMap2["h_crData"]           = 0;      
  //integrated
  histMap2["h_crMCInt"]          = 0;         
  histMap2["h_crMCfakeInt"]      = 0;    
  histMap2["h_crMCDileptonInt"]  = 0;   
  histMap2["h_srMCDileptonInt"]  = 0; 
  histMap2["h_crDataInt"]        = 0;
  
   //checks to make sure other directories exist
  TDirectory * dirData2 = fData ->GetDirectory("cr2L"+srName2);
  TDirectory * dirMC2   = fMC   ->GetDirectory("cr2L"+srName2);
  
  //if second MET region exists, add hists
  if (doMETsum && dirData2 && dirMC2) {

    histMap2["h_crMC"]             = (TH1D*) fMC->Get("cr2L"+srName2+"/h_mtbinsAlt");           
    histMap2["h_crMCfake"]         = (TH1D*) fMC->Get("cr2L"+srName2+"/h_mtbinsAltFake");     
    histMap2["h_crMCDilepton"]     = (TH1D*) fMC->Get("cr2L"+srName2+"/h_mtbinsAltDilepton");    
    histMap2["h_srMCDilepton"]     = (TH1D*) fMC->Get("srLep"+srName2+"/h_mtbinsDilepton");  
    histMap2["h_crData"]           = (TH1D*) fData->Get("cr2L"+srName2+"/h_mtbinsAlt");      
    //integrated
    histMap2["h_crMCInt"]          = (TH1D*) fMC->Get("cr2L"+srName2+"/h_Events_wAlt");         
    histMap2["h_crMCfakeInt"]      = (TH1D*) fMC->Get("cr2L"+srName2+"/h_Events_wAltFake");    
    histMap2["h_crMCDileptonInt"]  = (TH1D*) fMC->Get("cr2L"+srName2+"/h_Events_wAltDilepton");   
    histMap2["h_srMCDileptonInt"]  = (TH1D*) fMC->Get("srLep"+srName2+"/h_Events_wDilepton"); 
    histMap2["h_crDataInt"]        = (TH1D*) fData->Get("cr2L"+srName2+"/h_Events_wAlt");  

    //check for MC hist. should always exist
    if (!histMap2["h_srMCDilepton"] || !histMap2["h_srMCDileptonInt"]) {
      cerr << "ERROR: MC hist does not exist in region " << srName2 << "! Skipping..." << endl;
      return 1;
    }
  
    //check for empty hists, set names, add to other region hist
    for ( std::map<string, TH1D*>::iterator iter = histMap2.begin(); iter != histMap2.end(); ++iter ) {
      if (!(iter->second)){
	cerr << "WARNING: hist " << iter->first << " does not exist in region " << srName2 << "! Setting to 0..." << endl;
	if (iter->first.find("Int") != std::string::npos) iter->second = sameBin(histMap2["h_srMCDileptonInt"], "h");
	else iter->second = sameBin(histMap2["h_srMCDilepton"], "h");
      }
      iter->second->SetName(iter->first.c_str());
      if (iter->second) histMap[iter->first]->Add(iter->second);
    }
  }
  else if (doMETsum && (!dirData2 || !dirMC2)) {
    cerr << "WARNING: Additional HT directory " << srName2 << " does not exist! Make sure this is expected..." << endl;
    doMETsum = false;
  }
  
  //calculate the ratio histogram, N(SR/CR) in MC
  histMap["h_ratio"] = sameBin(histMap["h_crMC"], "h_ratio");
  histMap["h_ratio"]->Divide(histMap["h_srMCDilepton"], histMap["h_crMCDilepton"]);
  histMap["h_ratioInt"] = sameBin(histMap["h_crMCInt"], "h_ratioInt");
  histMap["h_ratioInt"]->Divide(histMap["h_srMCDileptonInt"], histMap["h_crMCDileptonInt"]);

  //calculate the purity histogram, N(Fake/Total) in CR, directly from MC
  histMap["h_purity"] = sameBin(histMap["h_crMC"], "h_purity");
  histMap["h_purity"]->Divide(histMap["h_crMCDilepton"],histMap["h_crMC"]);
  histMap["h_purityInt"] = sameBin(histMap["h_crMCInt"], "h_purityInt");
  histMap["h_purityInt"]->Divide(histMap["h_crMCDileptonInt"],histMap["h_crMCInt"]);   
  // TEfficiency* h_purity = new TEfficiency( *h_crMCDilepton,  *h_crMC);
  // h_purity->SetName("h_purity");

  //normalized histogram to extrapolate in MET
  histMap["h_k_met"] = sameBin(histMap["h_crMCInt"], "h_k_met");
  if (doMETsum) {
    histMap["h_k_met"]->Add(histMap["h_srMCDileptonInt"],histMap2["h_srMCDileptonInt"],1,-1);
    histMap["h_k_met"]->Divide(histMap["h_srMCDileptonInt"]);
  }
  
  //initialize pred histogram
  histMap["h_pred"] = sameBin(histMap["h_crMC"], "h_mtbins");

  //calculate the prediction in each bin
  for (int ibin = 0; ibin <= histMap["h_crData"]->GetSize(); ibin++) {

    //get bin contents
    double binDataCR         = histMap["h_crData"]  ->GetBinContent(ibin);
    double binRatio          = histMap["h_ratio"]   ->GetBinContent(ibin);
    double binPurity         = histMap["h_purityInt"]  ->GetBinContent(1);
    //double binPurity         = h_purity  ->GetEfficiency(ibin);
    double bink_met           = histMap["h_k_met"]  ->GetBinContent(1);
    
    //get bin errors
    double binDataCR_err     = ( binDataCR != 0) ? histMap["h_crData"] ->GetBinError(ibin) / binDataCR : 0;
    double binRatio_err      = ( binRatio != 0) ? histMap["h_ratio"] ->GetBinError(ibin) / binRatio : 0;
    double binPurity_err     = ( binPurity != 0) ? histMap["h_purityInt"] ->GetBinError(1) / binPurity : 0;
    // double binPurity_errL    = h_purity  ->GetEfficiencyErrorLow(ibin); binPurity_errL = (binPurity_errL > 0) ? binPurity_errL : 0;
    // double binPurity_errU    = h_purity  ->GetEfficiencyErrorUp(ibin); binPurity_errU = (binPurity_errU > 0) ? binPurity_errU : 0;
    // double binPurity_err     = max(binPurity_errL, binPurity_errU);  //for now, just take larger of asymmetric errors
    double bink_met_err       = ( bink_met != 0) ? histMap["h_k_met"] ->GetBinError(1) / bink_met : 0;
  
    //calculate pred = N(CR, data) * Ratio(SR/CR, mc) * Purity
    double binPred = -1;
    if (doMETsum) binPred = binDataCR * binRatio * binPurity * bink_met;
    else binPred = binDataCR * binRatio * binPurity;
    
    //calculate total error in quadrature
    double binErrSq = binDataCR_err * binDataCR_err + binRatio_err * binRatio_err + binPurity_err * binPurity_err + bink_met_err * bink_met_err;

    //set bin content/error
    histMap["h_pred"]->SetBinContent(ibin, binPred);
    histMap["h_pred"]->SetBinError(ibin, binPred*sqrt(binErrSq));     

  }//loop over bins

  histMap["h_predMCshape"] = sameBin(histMap["h_crMC"], "h_mtbinsMCshape");
  for (int ibin = 0; ibin <= histMap["h_crData"]->GetSize(); ibin++) {
    histMap["h_predMCshape"]->SetBinContent(ibin, histMap["h_srMCDilepton"]->GetBinContent(ibin));
    histMap["h_predMCshape"]->SetBinError(ibin, histMap["h_srMCDilepton"]->GetBinError(ibin));
  }
  histMap["h_predMCshape"]->Scale(histMap["h_pred"]->Integral()/histMap["h_srMCDilepton"]->Integral());
  
  //write hists to output file
  cout << "Saving hists for " << dir_name << "..." << endl;
  fOut->cd();
  TDirectory* dir = fOut->mkdir(dir_name.c_str());
  dir->cd();
  for ( std::map<string, TH1D*>::iterator iter = histMap.begin(); iter != histMap.end(); ++iter ) {
    if (iter->second) iter->second->Write();
    delete iter->second;
    delete histMap2[iter->first];
  }

  return 0;
  
}

//_______________________________________________________________________________
void makeCR2LestimateALT(string input_dir = "../../SoftLepLooper/output/softLep", string dataname = "data_Run2015CD"){


  string output_name = input_dir+"/pred_CR2LALT.root";
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

