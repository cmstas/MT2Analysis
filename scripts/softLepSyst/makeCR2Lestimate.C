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
int makeCR2Lpred( TFile* fData , TFile* fMC , TFile* fMC_dyUP , TFile* fMC_dyDN , TFile* fOut ,  std::string dir_name ) {

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

  //initialize map of histograms
  std::map<string, TH1D*> histMap;
  histMap["h_crMC"]             = (TH1D*) fMC->Get("cr2L"+srName+"/h_mtbins");           
  histMap["h_crMCfake"]         = (TH1D*) fMC->Get("cr2L"+srName+"/h_mtbinsFake");     
  histMap["h_crMCDilepton"]     = (TH1D*) fMC->Get("cr2L"+srName+"/h_mtbinsDilepton");    
  histMap["h_srMCDilepton"]     = (TH1D*) fMC->Get("srLep"+srName+"/h_mtbinsDilepton");  
  histMap["h_crData"]           = (TH1D*) fData->Get("cr2L"+srName+"/h_mtbins");
  //DY UP histogram
  histMap["h_crMCDilepton_dyUP"]     = (TH1D*) fMC_dyUP->Get("cr2L"+srName+"/h_mtbinsDilepton");    
  histMap["h_srMCDilepton_dyUP"]     = (TH1D*) fMC_dyUP->Get("srLep"+srName+"/h_mtbinsDilepton");  
  //DY DN histogram
  histMap["h_crMCDilepton_dyDN"]     = (TH1D*) fMC_dyDN->Get("cr2L"+srName+"/h_mtbinsDilepton");    
  histMap["h_srMCDilepton_dyDN"]     = (TH1D*) fMC_dyDN->Get("srLep"+srName+"/h_mtbinsDilepton");  
  //Renorm histograms
  histMap["h_crMCDilepton_renorm_UP"]     = (TH1D*) fMC->Get("cr2L"+srName+"/h_mtbins_renorm_UPDilepton");    
  histMap["h_srMCDilepton_renorm_UP"]     = (TH1D*) fMC->Get("srLep"+srName+"/h_mtbins_renorm_UPDilepton");
  histMap["h_crMCDilepton_renorm_DN"]     = (TH1D*) fMC->Get("cr2L"+srName+"/h_mtbins_renorm_DNDilepton");    
  histMap["h_srMCDilepton_renorm_DN"]     = (TH1D*) fMC->Get("srLep"+srName+"/h_mtbins_renorm_DNDilepton");

  //fill Int histograms with integrated mtbins, i.e. event count
  for ( std::map<string, TH1D*>::iterator iter = histMap.begin(); iter != histMap.end(); ++iter ) {
    if (iter->first.find("Int") != std::string::npos) continue;
    histMap[Form("%sInt",iter->first.c_str())] = new TH1D(Form("%sInt",iter->first.c_str()),"Events",1,0,1);
    if (iter->second) histMap[Form("%sInt",iter->first.c_str())]->SetBinContent(1,iter->second->Integral(0,-1));
  }

  //check for MC hist. should always exist
  if (!histMap["h_crMC"] || !histMap["h_crMCInt"]) {
    cerr << "ERROR: MC hist does not exist in region " << srName << "! Skipping..." << endl;
    return 1;
  }

  //check for empty hists, set names
  for ( std::map<string, TH1D*>::iterator iter = histMap.begin(); iter != histMap.end(); ++iter ) {
    if (!(iter->second)){
      cerr << "WARNING: hist " << iter->first << " does not exist in region " << srName << "! Setting to 0..." << endl;
	if (iter->first.find("Int") != std::string::npos) iter->second = sameBin(histMap["h_crMCInt"], "h");
	else iter->second = sameBin(histMap["h_crMC"], "h");
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
  //DY UP histogram
  histMap2["h_crMCDilepton_dyUP"] = 0;    
  histMap2["h_srMCDilepton_dyUP"] = 0;
  //DY DN histogram
  histMap2["h_crMCDilepton_dyDN"] = 0;    
  histMap2["h_srMCDilepton_dyDN"] = 0;
  //Renorm histograms
  histMap2["h_crMCDilepton_renorm_UP"] = 0;    
  histMap2["h_srMCDilepton_renorm_UP"] = 0;
  histMap2["h_crMCDilepton_renorm_DN"] = 0;
  histMap2["h_srMCDilepton_renorm_DN"] = 0;

  //checks to make sure other directories exist
  TDirectory * dirData2 = fData ->GetDirectory("cr2L"+srName2);
  TDirectory * dirMC2   = fMC   ->GetDirectory("cr2L"+srName2);

  //if second HT region exists, add hists
  if (doHTsum && dirData2 && dirMC2) {

    histMap2["h_crMC"]             = (TH1D*) fMC->Get("cr2L"+srName2+"/h_mtbins");           
    histMap2["h_crMCfake"]         = (TH1D*) fMC->Get("cr2L"+srName2+"/h_mtbinsFake");     
    histMap2["h_crMCDilepton"]     = (TH1D*) fMC->Get("cr2L"+srName2+"/h_mtbinsDilepton");    
    histMap2["h_srMCDilepton"]     = (TH1D*) fMC->Get("srLep"+srName2+"/h_mtbinsDilepton");  
    histMap2["h_crData"]           = (TH1D*) fData->Get("cr2L"+srName2+"/h_mtbins");  
    //DY UP histogram
    histMap2["h_crMCDilepton_dyUP"]     = (TH1D*) fMC_dyUP->Get("cr2L"+srName2+"/h_mtbinsDilepton");    
    histMap2["h_srMCDilepton_dyUP"]     = (TH1D*) fMC_dyUP->Get("srLep"+srName2+"/h_mtbinsDilepton");  
    //DY DN histogram
    histMap2["h_crMCDilepton_dyDN"]     = (TH1D*) fMC_dyDN->Get("cr2L"+srName2+"/h_mtbinsDilepton");    
    histMap2["h_srMCDilepton_dyDN"]     = (TH1D*) fMC_dyDN->Get("srLep"+srName2+"/h_mtbinsDilepton"); 
    //Renorm histograms
    histMap2["h_crMCDilepton_renorm_UP"]     = (TH1D*) fMC->Get("cr2L"+srName2+"/h_mtbins_renorm_UPDilepton");    
    histMap2["h_srMCDilepton_renorm_UP"]     = (TH1D*) fMC->Get("srLep"+srName2+"/h_mtbins_renorm_UPDilepton");
    histMap2["h_crMCDilepton_renorm_DN"]     = (TH1D*) fMC->Get("cr2L"+srName2+"/h_mtbins_renorm_DNDilepton");    
    histMap2["h_srMCDilepton_renorm_DN"]     = (TH1D*) fMC->Get("srLep"+srName2+"/h_mtbins_renorm_DNDilepton");     

    //fill Int histograms with integrated mtbins, i.e. event count
    for ( std::map<string, TH1D*>::iterator iter = histMap2.begin(); iter != histMap2.end(); ++iter ) {
      if (iter->first.find("Int") != std::string::npos) continue;
      histMap2[Form("%sInt",iter->first.c_str())] = new TH1D(Form("%s2Int",iter->first.c_str()),"Events",1,0,1);
      if (iter->second) histMap2[Form("%sInt",iter->first.c_str())]->SetBinContent(1,iter->second->Integral(0,-1));
    }

    //check for MC hist. should always exist
    if (!histMap2["h_crMC"] || !histMap2["h_crMCInt"]) {
      cerr << "ERROR: MC hist does not exist in region " << srName2 << "! Skipping..." << endl;
      return 1;
    }
  
    //check for empty hists, set names, add to other region hist
    for ( std::map<string, TH1D*>::iterator iter = histMap2.begin(); iter != histMap2.end(); ++iter ) {
      if (!(iter->second)){
	cerr << "WARNING: hist " << iter->first << " does not exist in region " << srName2 << "! Setting to 0..." << endl;
	if (iter->first.find("Int") != std::string::npos) iter->second = sameBin(histMap2["h_crMCInt"], "h");
	else iter->second = sameBin(histMap2["h_crMC"], "h");
      }
      iter->second->SetName(iter->first.c_str());
      if (iter->second) histMap[iter->first]->Add(iter->second);
    }
  }
  else if (doHTsum && (!dirData2 || !dirMC2)) {
    cerr << "WARNING: Additional HT directory " << srName2 << " does not exist! Make sure this is expected..." << endl;
    doHTsum = false;
  }
  
  //calculate the ratio histogram, N(SR/CR) in MC
  histMap["h_ratio"] = sameBin(histMap["h_crMC"], "h_ratio");
  histMap["h_ratio"]->Divide(histMap["h_srMCDilepton"], histMap["h_crMCDilepton"]);
  histMap["h_ratioInt"] = sameBin(histMap["h_crMCInt"], "h_ratioInt");
  histMap["h_ratioInt"]->Divide(histMap["h_srMCDileptonInt"], histMap["h_crMCDileptonInt"]);
  //DY UP histograms
  histMap["h_ratio_dyUP"] = sameBin(histMap["h_crMC"], "h_ratio_dyUP");
  histMap["h_ratio_dyUP"]->Divide(histMap["h_srMCDilepton_dyUP"], histMap["h_crMCDilepton_dyUP"]);
  histMap["h_ratio_dyUPInt"] = sameBin(histMap["h_crMCInt"], "h_ratio_dyUPInt");
  histMap["h_ratio_dyUPInt"]->Divide(histMap["h_srMCDilepton_dyUPInt"], histMap["h_crMCDilepton_dyUPInt"]);
  //DY DN histograms
  histMap["h_ratio_dyDN"] = sameBin(histMap["h_crMC"], "h_ratio_dyDN");
  histMap["h_ratio_dyDN"]->Divide(histMap["h_srMCDilepton_dyDN"], histMap["h_crMCDilepton_dyDN"]);
  histMap["h_ratio_dyDNInt"] = sameBin(histMap["h_crMCInt"], "h_ratio_dyDNInt");
  histMap["h_ratio_dyDNInt"]->Divide(histMap["h_srMCDilepton_dyDNInt"], histMap["h_crMCDilepton_dyDNInt"]);
  //Renorm histograms
  histMap["h_ratio_renorm_UP"] = sameBin(histMap["h_crMC"], "h_ratio_renorm_UP");
  histMap["h_ratio_renorm_UP"]->Divide(histMap["h_srMCDilepton_renorm_UP"], histMap["h_crMCDilepton_renorm_UP"]);
  histMap["h_ratio_renorm_UPInt"] = sameBin(histMap["h_crMCInt"], "h_ratio_renorm_UPInt");
  histMap["h_ratio_renorm_UPInt"]->Divide(histMap["h_srMCDilepton_renorm_UPInt"], histMap["h_crMCDilepton_renorm_UPInt"]);
  histMap["h_ratio_renorm_DN"] = sameBin(histMap["h_crMC"], "h_ratio_renorm_DN");
  histMap["h_ratio_renorm_DN"]->Divide(histMap["h_srMCDilepton_renorm_DN"], histMap["h_crMCDilepton_renorm_DN"]);
  histMap["h_ratio_renorm_DNInt"] = sameBin(histMap["h_crMCInt"], "h_ratio_renorm_DNInt");
  histMap["h_ratio_renorm_DNInt"]->Divide(histMap["h_srMCDilepton_renorm_DNInt"], histMap["h_crMCDilepton_renorm_DNInt"]);
  //DY UP/DN systematic hist
  histMap["h_ratioIntdyUPDN"] = sameBin(histMap["h_crMCInt"], "h_ratioIntdyUPDN");
  histMap["h_ratioIntdyUPDN"]->SetBinContent(1, histMap["h_ratioInt"]->GetBinContent(1));
  float dyUP_err =  fabs(histMap["h_ratioInt"]->GetBinContent(1) - histMap["h_ratio_dyUPInt"]->GetBinContent(1));
  float dyDN_err =  fabs(histMap["h_ratioInt"]->GetBinContent(1) - histMap["h_ratio_dyDNInt"]->GetBinContent(1));
  float dy_err = max(dyUP_err,dyDN_err);
  histMap["h_ratioIntdyUPDN"]->SetBinError(1,dy_err); 
  //Renorm systematic hist
  histMap["h_ratioIntRenorm"] = sameBin(histMap["h_crMCInt"], "h_ratioIntRenorm");
  histMap["h_ratioIntRenorm"]->SetBinContent(1, histMap["h_ratioInt"]->GetBinContent(1));
  float renorm_UP_err =  fabs(histMap["h_ratioInt"]->GetBinContent(1) - histMap["h_ratio_renorm_UPInt"]->GetBinContent(1));
  float renorm_DN_err =  fabs(histMap["h_ratioInt"]->GetBinContent(1) - histMap["h_ratio_renorm_DNInt"]->GetBinContent(1));
  float renorm_err = max(renorm_UP_err,renorm_DN_err);
  histMap["h_ratioIntRenorm"]->SetBinError(1,renorm_err); 

  //calculate the purity histogram, N(Fake/Total) in CR, directly from MC
  histMap["h_purity"] = sameBin(histMap["h_crMC"], "h_purity");
  histMap["h_purity"]->Divide(histMap["h_crMCDilepton"],histMap["h_crMC"]);
  histMap["h_purityInt"] = sameBin(histMap["h_crMCInt"], "h_purityInt");
  histMap["h_purityInt"]->Divide(histMap["h_crMCDileptonInt"],histMap["h_crMCInt"]);   
  // TEfficiency* h_purity = new TEfficiency( *h_crMCDilepton,  *h_crMC);
  // h_purity->SetName("h_purity");

  //normalized histogram to extrapolate in HT
  histMap["h_k_ht"] = sameBin(histMap["h_crMCInt"], "h_k_ht");
  if (doHTsum) {
    histMap["h_k_ht"]->Add(histMap["h_srMCDileptonInt"],histMap2["h_srMCDileptonInt"],1,-1);
    histMap["h_k_ht"]->Divide(histMap["h_srMCDileptonInt"]);
  }
  
  //normalized histogram to extrapolate in MT, given HT bin
  histMap["h_k_mt"] = sameBin(histMap["h_crMC"], "h_k_mt");
  if (doHTsum) histMap["h_k_mt"]->Add(histMap["h_srMCDilepton"],histMap2["h_srMCDilepton"],1,-1);
  else histMap["h_k_mt"]->Add(histMap["h_srMCDilepton"]);
  histMap["h_k_mt"]->Scale(1/histMap["h_k_mt"]->Integral(0,-1));
  
  //initialize pred histogram
  histMap["h_pred"] = sameBin(histMap["h_crMC"], "h_mtbins");
  histMap["h_predAddErr"] = sameBin(histMap["h_crMC"], "h_mtbinsSyst");

  //calculate the prediction in each bin
  for (int ibin = 0; ibin <= histMap["h_crData"]->GetSize(); ibin++) {

    //get bin contents
    double binDataCR         = histMap["h_crDataInt"]  ->GetBinContent(1);
    double binRatio          = histMap["h_ratioInt"]   ->GetBinContent(1);
    double binPurity         = histMap["h_purityInt"]  ->GetBinContent(1);
    //double binPurity         = h_purity  ->GetEfficiency(ibin);
    double bink_ht           = histMap["h_k_ht"]  ->GetBinContent(1);
    double bink_mt           = histMap["h_k_mt"]  ->GetBinContent(ibin);

    //get bin errors
    double binDataCR_err     = ( binDataCR != 0) ? histMap["h_crDataInt"] ->GetBinError(1) / binDataCR : 0;
    double binRatio_err      = ( binRatio != 0) ? histMap["h_ratioInt"] ->GetBinError(1) / binRatio : 0;
    double binPurity_err     = ( binPurity != 0) ? histMap["h_purityInt"] ->GetBinError(1) / binPurity : 0;
    // double binPurity_errL    = h_purity  ->GetEfficiencyErrorLow(ibin); binPurity_errL = (binPurity_errL > 0) ? binPurity_errL : 0;
    // double binPurity_errU    = h_purity  ->GetEfficiencyErrorUp(ibin); binPurity_errU = (binPurity_errU > 0) ? binPurity_errU : 0;
    // double binPurity_err     = max(binPurity_errL, binPurity_errU);  //for now, just take larger of asymmetric errors
    double bink_ht_err       = ( bink_ht != 0) ? histMap["h_k_ht"] ->GetBinError(1) / bink_ht : 0;
    double bink_mt_err       = ( bink_mt != 0) ? histMap["h_k_mt"] ->GetBinError(ibin) / bink_mt : 0;
    
    //calculate pred = N(CR, data) * Ratio(SR/CR, mc) * Purity * k_ht * k_mt
    double binPred = -1;
    if (doHTsum) binPred = binDataCR * binRatio * binPurity * bink_ht * bink_mt;
    else binPred = binDataCR * binRatio * binPurity * bink_mt;
    
    //calculate total error in quadrature
    double binErrSq = binDataCR_err * binDataCR_err + binRatio_err * binRatio_err + binPurity_err * binPurity_err + bink_ht_err * bink_ht_err + bink_mt_err * bink_mt_err;
    float mtBinErr = 0;
    if (ibin == 1) mtBinErr = 0;
    else if (ibin == 2) mtBinErr = 0.1;
    else if (ibin == 3) mtBinErr = 0.2;
    double binErrSqAdd = binDataCR_err * binDataCR_err + binRatio_err * binRatio_err + binPurity_err * binPurity_err + bink_ht_err * bink_ht_err + bink_mt_err * bink_mt_err + pow(0.2,2) + pow(mtBinErr,2);

    //set bin content/error
    histMap["h_pred"]->SetBinContent(ibin, binPred);
    histMap["h_pred"]->SetBinError(ibin, binPred*sqrt(binErrSq));     
    histMap["h_predAddErr"]->SetBinContent(ibin, binPred);
    histMap["h_predAddErr"]->SetBinError(ibin, binPred*sqrt(binErrSqAdd)); //additional 20% error   

  }//loop over bins

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
void makeCR2Lestimate(string input_dir = "../../SoftLepLooper/output/softLep_unblind_skim_apr30", string dataname = "data_Run2015CD"){


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
  TFile* f_mc_dyUP   = new TFile(Form("%s/allBkg_dyUP.root",input_dir.c_str()));
  TFile* f_mc_dyDN   = new TFile(Form("%s/allBkg_dyDN.root",input_dir.c_str()));


  if(f_data->IsZombie() || f_mc->IsZombie() || f_mc_dyUP->IsZombie() || f_mc_dyDN->IsZombie()) {
    std::cerr << "Input file does not exist" << std::endl;
    return;
  }

  TIter it(f_data->GetListOfKeys());
  TKey* k;
  while ((k = (TKey *)it())) {
    std::string dir_name = k->GetTitle();
    if(dir_name.find("srLep")==std::string::npos) continue; //to do only signal regions
    cout << "----- Calculating prediction for " << dir_name << " -----" << endl;
    makeCR2Lpred( f_data , f_mc , f_mc_dyUP , f_mc_dyDN , f_out , dir_name );
  }

  return;
  
}

