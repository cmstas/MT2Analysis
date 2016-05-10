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
int makeCR1Lpred( TFile* fData , TFile* fMC , TFile* fMC_topUP , TFile* fMC_wUP , TFile* fOut ,  std::string dir_name ) {

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
  //W polarization histograms
  histMap["h_crMConelep_polW_UP"]     = (TH1D*) fMC->Get("cr1L"+srName+"/h_mtbins_polW_UPOnelep");    
  histMap["h_srMConelep_polW_UP"]     = (TH1D*) fMC->Get("srLep"+srName+"/h_mtbins_polW_UPOnelep");
  histMap["h_crMConelep_polW_DN"]     = (TH1D*) fMC->Get("cr1L"+srName+"/h_mtbins_polW_DNOnelep");    
  histMap["h_srMConelep_polW_DN"]     = (TH1D*) fMC->Get("srLep"+srName+"/h_mtbins_polW_DNOnelep");
  //Top UP histograms
  histMap["h_crMConelep_top_UP"]     = (TH1D*) fMC_topUP->Get("cr1L"+srName+"/h_mtbinsOnelep");    
  histMap["h_srMConelep_top_UP"]     = (TH1D*) fMC_topUP->Get("srLep"+srName+"/h_mtbinsOnelep");
  //W UP histograms
  histMap["h_crMConelep_w_UP"]       = (TH1D*) fMC_wUP->Get("cr1L"+srName+"/h_mtbinsOnelep");    
  histMap["h_srMConelep_w_UP"]       = (TH1D*) fMC_wUP->Get("srLep"+srName+"/h_mtbinsOnelep");
  //Renorm histograms
  histMap["h_crMConelep_renorm_UP"]     = (TH1D*) fMC->Get("cr1L"+srName+"/h_mtbins_renorm_UPOnelep");    
  histMap["h_srMConelep_renorm_UP"]     = (TH1D*) fMC->Get("srLep"+srName+"/h_mtbins_renorm_UPOnelep");
  histMap["h_crMConelep_renorm_DN"]     = (TH1D*) fMC->Get("cr1L"+srName+"/h_mtbins_renorm_DNOnelep");    
  histMap["h_srMConelep_renorm_DN"]     = (TH1D*) fMC->Get("srLep"+srName+"/h_mtbins_renorm_DNOnelep");
  //Top pT reweight histograms
  histMap["h_crMConelep_TopPt_UP"]       = (TH1D*) fMC->Get("cr1L"+srName+"/h_mtbins_TopPt_UPOnelep");    
  histMap["h_srMConelep_TopPt_UP"]       = (TH1D*) fMC->Get("srLep"+srName+"/h_mtbins_TopPt_UPOnelep");

  //fill Int histograms with integrated mtbins, i.e. event count
  for ( std::map<string, TH1D*>::iterator iter = histMap.begin(); iter != histMap.end(); ++iter ) {
    if (iter->first.find("Int") != std::string::npos) continue;
    histMap[Form("%sInt",iter->first.c_str())] = new TH1D(Form("%sInt",iter->first.c_str()),"Events",1,0,1);
    if (iter->second) {
      double err = 0;
      histMap[Form("%sInt",iter->first.c_str())]->SetBinContent(1,iter->second->IntegralAndError(0,-1,err));
      histMap[Form("%sInt",iter->first.c_str())]->SetBinError(1,err);
    }
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

  //calculate the ratio histogram, N(SR/CR) in MC
  histMap["h_ratio"] = sameBin(histMap["h_crMC"], "h_ratio");
  histMap["h_ratio"]->Divide(histMap["h_srMConelep"], histMap["h_crMConelep"]);
  histMap["h_ratioInt"] = sameBin(histMap["h_crMCInt"], "h_ratioInt");
  histMap["h_ratioInt"]->Divide(histMap["h_srMConelepInt"], histMap["h_crMConelepInt"]);
  //W polarization histograms
  histMap["h_ratio_polW_UP"] = sameBin(histMap["h_crMC"], "h_ratio_polW_UP");
  histMap["h_ratio_polW_UP"]->Divide(histMap["h_srMConelep_polW_UP"], histMap["h_crMConelep_polW_UP"]);
  histMap["h_ratio_polW_UPInt"] = sameBin(histMap["h_crMCInt"], "h_ratio_polW_UPInt");
  histMap["h_ratio_polW_UPInt"]->Divide(histMap["h_srMConelep_polW_UPInt"], histMap["h_crMConelep_polW_UPInt"]);
  histMap["h_ratio_polW_DN"] = sameBin(histMap["h_crMC"], "h_ratio_polW_DN");
  histMap["h_ratio_polW_DN"]->Divide(histMap["h_srMConelep_polW_DN"], histMap["h_crMConelep_polW_DN"]);
  histMap["h_ratio_polW_DNInt"] = sameBin(histMap["h_crMCInt"], "h_ratio_polW_DNInt");
  histMap["h_ratio_polW_DNInt"]->Divide(histMap["h_srMConelep_polW_DNInt"], histMap["h_crMConelep_polW_DNInt"]);
  //Top UP histograms
  histMap["h_ratio_top_UP"] = sameBin(histMap["h_crMC"], "h_ratio_top_UP");
  histMap["h_ratio_top_UP"]->Divide(histMap["h_srMConelep_top_UP"], histMap["h_crMConelep_top_UP"]);
  histMap["h_ratio_top_UPInt"] = sameBin(histMap["h_crMCInt"], "h_ratio_top_UPInt");
  histMap["h_ratio_top_UPInt"]->Divide(histMap["h_srMConelep_top_UPInt"], histMap["h_crMConelep_top_UPInt"]);
  //W UP histograms
  histMap["h_ratio_w_UP"] = sameBin(histMap["h_crMC"], "h_ratio_w_UP");
  histMap["h_ratio_w_UP"]->Divide(histMap["h_srMConelep_w_UP"], histMap["h_crMConelep_w_UP"]);
  histMap["h_ratio_w_UPInt"] = sameBin(histMap["h_crMCInt"], "h_ratio_w_UPInt");
  histMap["h_ratio_w_UPInt"]->Divide(histMap["h_srMConelep_w_UPInt"], histMap["h_crMConelep_w_UPInt"]);
  //Renorm histograms
  histMap["h_ratio_renorm_UP"] = sameBin(histMap["h_crMC"], "h_ratio_renorm_UP");
  histMap["h_ratio_renorm_UP"]->Divide(histMap["h_srMConelep_renorm_UP"], histMap["h_crMConelep_renorm_UP"]);
  histMap["h_ratio_renorm_UPInt"] = sameBin(histMap["h_crMCInt"], "h_ratio_renorm_UPInt");
  histMap["h_ratio_renorm_UPInt"]->Divide(histMap["h_srMConelep_renorm_UPInt"], histMap["h_crMConelep_renorm_UPInt"]);
  histMap["h_ratio_renorm_DN"] = sameBin(histMap["h_crMC"], "h_ratio_renorm_DN");
  histMap["h_ratio_renorm_DN"]->Divide(histMap["h_srMConelep_renorm_DN"], histMap["h_crMConelep_renorm_DN"]);
  histMap["h_ratio_renorm_DNInt"] = sameBin(histMap["h_crMCInt"], "h_ratio_renorm_DNInt");
  histMap["h_ratio_renorm_DNInt"]->Divide(histMap["h_srMConelep_renorm_DNInt"], histMap["h_crMConelep_renorm_DNInt"]);
  //Top Pt reweight histograms
  histMap["h_ratio_TopPt_UP"] = sameBin(histMap["h_crMC"], "h_ratio_TopPt_UP");
  histMap["h_ratio_TopPt_UP"]->Divide(histMap["h_srMConelep_TopPt_UP"], histMap["h_crMConelep_TopPt_UP"]);
  histMap["h_ratio_TopPt_UPInt"] = sameBin(histMap["h_crMCInt"], "h_ratio_TopPt_UPInt");
  histMap["h_ratio_TopPt_UPInt"]->Divide(histMap["h_srMConelep_TopPt_UPInt"], histMap["h_crMConelep_TopPt_UPInt"]);
  //W polarization systematic hist
  histMap["h_ratioIntSyst"] = sameBin(histMap["h_crMCInt"], "h_ratioIntSyst");
  histMap["h_ratioIntSyst"]->SetBinContent(1, histMap["h_ratioInt"]->GetBinContent(1));
  float polW_UP_err =  fabs(histMap["h_ratioInt"]->GetBinContent(1) - histMap["h_ratio_polW_UPInt"]->GetBinContent(1));
  float polW_DN_err =  fabs(histMap["h_ratioInt"]->GetBinContent(1) - histMap["h_ratio_polW_DNInt"]->GetBinContent(1));
  float polW_err = max(polW_UP_err,polW_DN_err);
  histMap["h_ratioIntSyst"]->SetBinError(1,polW_err); 
  //Top & W fraction systematic hist
  histMap["h_ratioIntTopW"] = sameBin(histMap["h_crMCInt"], "h_ratioIntTopW");
  histMap["h_ratioIntTopW"]->SetBinContent(1, histMap["h_ratioInt"]->GetBinContent(1));
  float top_UP_err =  fabs(histMap["h_ratioInt"]->GetBinContent(1) - histMap["h_ratio_top_UPInt"]->GetBinContent(1));
  float w_UP_err =  fabs(histMap["h_ratioInt"]->GetBinContent(1) - histMap["h_ratio_w_UPInt"]->GetBinContent(1));
  float TopW_err = max(top_UP_err,w_UP_err);
  histMap["h_ratioIntTopW"]->SetBinError(1,TopW_err); 
  //Renorm systematic hist
  histMap["h_ratioIntRenorm"] = sameBin(histMap["h_crMCInt"], "h_ratioIntRenorm");
  histMap["h_ratioIntRenorm"]->SetBinContent(1, histMap["h_ratioInt"]->GetBinContent(1));
  float renorm_UP_err =  fabs(histMap["h_ratioInt"]->GetBinContent(1) - histMap["h_ratio_renorm_UPInt"]->GetBinContent(1));
  float renorm_DN_err =  fabs(histMap["h_ratioInt"]->GetBinContent(1) - histMap["h_ratio_renorm_DNInt"]->GetBinContent(1));
  float renorm_err = max(renorm_UP_err,renorm_DN_err);
  histMap["h_ratioIntRenorm"]->SetBinError(1,renorm_err); 
  
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
  histMap["h_predAddErr"] = sameBin(histMap["h_crMC"], "h_mtbinsSyst");

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
    float mtBinErr = 0;
    if (ibin == 1) mtBinErr = 0;
    else if (ibin == 2) mtBinErr = 0.1;
    else if (ibin == 3) mtBinErr = 0.2;
    double binErrSqAdd = binDataCR_err * binDataCR_err + binRatio_err * binRatio_err + binPurity_err * binPurity_err + binkMT_err * binkMT_err + pow(0.2,2) + pow(mtBinErr,2);

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
  }

  
  return 0;
  
}

//_______________________________________________________________________________
void makeCR1Lestimate(string input_dir = "../../SoftLepLooper/output/softLep_unblind_skim_apr30/", string dataname = "data_Run2015CD"){


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
  TFile* f_mc_topUP   = new TFile(Form("%s/allBkg_topUP.root",input_dir.c_str()));
  TFile* f_mc_wUP     = new TFile(Form("%s/allBkg_wjetsUP.root",input_dir.c_str()));


  if(f_data->IsZombie() || f_mc->IsZombie() || f_mc_topUP->IsZombie() || f_mc_wUP->IsZombie() ) {
    std::cerr << "Input file does not exist" << std::endl;
    return;
  }

  TIter it(f_data->GetListOfKeys());
  TKey* k;
  while ((k = (TKey *)it())) {
    std::string dir_name = k->GetTitle();
    if(dir_name.find("srLep")==std::string::npos) continue; //to do only signal regions
    cout << "Calculating prediction for " << dir_name << "..." << endl;
    makeCR1Lpred( f_data , f_mc , f_mc_topUP, f_mc_wUP, f_out , dir_name );
  }

  return;
  
}

