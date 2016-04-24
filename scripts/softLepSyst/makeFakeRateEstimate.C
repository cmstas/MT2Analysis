//script to make purity.root file
//which contains purity histograms
//calculated from fake-rate predictions

#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TStyle.h"
#include "TH1F.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TString.h"
#include "TLegend.h"
#include "TKey.h"
#include "TEfficiency.h"
#include "TMath.h"

#include "../../MT2CORE/mt2tree.h"
#include "../../MT2CORE/sigSelections.h"
#include "../../MT2CORE/SR.h"
#include "../../MT2CORE/sigSelections.cc"
#include "../../MT2CORE/SR.cc"

using namespace std;
using namespace mt2;

bool verbose = true;
bool realData = false;


//converts float to string
std::string toString(float in){
  std::stringstream ss;
  ss << in;
  return ss.str();
}

//returns an empty hist with same binning
inline TH1F* sameBin(TH1F* h_in)
{
  TH1F* h_out = (TH1F*) h_in->Clone();
  h_out->Reset();
  return h_out;
}


//takes a FR histogram and signal region and calculates the predicted number of fakes
void makeFRpred(TFile* f_data ,TFile* f_mc ,TFile* f_out , TH1D* h_FRdataEewk, TH1D* h_FRdataMewk, TString srName, TString suffix ) {
  
  
  if (verbose) cout<<" ------ Looking at SR "<<srName<<" for type "<<suffix<<". Fake rate histo has "<<h_FRdataEewk->GetSize()<<" bins"<<endl;

  
  TH2F* h_LooseNotTightTemplate = (TH2F*) f_data->Get("srLepbase/h_mtbins_lepptElLooseNotTight");
  if (!h_LooseNotTightTemplate)   {
    cout << "srLepbase/h_mtbins_lepptElLooseNotTight hist is empty!"<<endl;
    return;
  }

  Double_t predsTot[5] = {0, 0, 0, 0, 0};
  Double_t predTotErrors[5] = {0, 0, 0, 0, 0};

  TString lep = "El";
  TH1D* h_FR = h_FRdataEewk;

  for (int ilep = 0; ilep < 2; ilep++) {

    if (ilep > 0) {
      lep = "Mu";
      h_FR = h_FRdataMewk;
      
    }

    // get LooseNotTight hist
    TH2F* h_LooseNotTight = (TH2F*) f_data->Get(srName+"/h_mtbins_leppt"+lep+"LooseNotTight");
    //check for empty hists
    if(!h_LooseNotTight){
      cout << "SR" << srName << " LooseNotTight hist is empty! Skip it"<<endl;
      continue;
//      cout << "SR" << srName << " LooseNotTight hist is empty! Setting yields to 1..." << endl;
//      h_LooseNotTight = h_LooseNotTightTemplate;
//      h_LooseNotTight->Reset();
//      h_LooseNotTight->SetBinContent(1, 1, 0.);
//      h_LooseNotTight->SetBinContent(2, 1, 0.);
//      h_LooseNotTight->SetBinContent(3, 1, 0.);
//      h_LooseNotTight->SetBinError(1, 1, 1.);
//      h_LooseNotTight->SetBinError(2, 1, 1.);
//      h_LooseNotTight->SetBinError(3, 1, 1.);

    }
    if(!h_FR){
      cout << "FR hist is null!" << endl;
      return;
    }
    
    // subtract prompts (if any)
    TH2F* h_LooseNotTightEWK = (TH2F*) f_mc->Get(srName+"/h_mtbins_leppt"+lep+"LooseNotTightOnelep");
    TH1D* h_LooseNotTightProjMT= h_LooseNotTight ? h_LooseNotTight->ProjectionX() : 0;
    TH1D* h_LooseNotTightPromptFrac= h_LooseNotTightEWK ? h_LooseNotTightEWK->ProjectionX("h_mtbins_"+lep+"PromptFraction"+suffix) : 0;
    if (h_LooseNotTightPromptFrac && h_LooseNotTightProjMT) h_LooseNotTightPromptFrac->Divide(h_LooseNotTightProjMT);
    if (h_LooseNotTightEWK) h_LooseNotTight->Add(h_LooseNotTightEWK, -1);

    
    if (verbose) cout<<"In bin 1: "<<h_LooseNotTight->ProjectionX()->GetBinContent(1)+h_LooseNotTightEWK->ProjectionX()->GetBinContent(1)<<" events, of which "<<h_LooseNotTightEWK->ProjectionX()->GetBinContent(1)<<" prompt"<<endl;
    if (verbose) cout<<"In bin 2: "<<h_LooseNotTight->ProjectionX()->GetBinContent(2)+h_LooseNotTightEWK->ProjectionX()->GetBinContent(2)<<" events, of which "<<h_LooseNotTightEWK->ProjectionX()->GetBinContent(2)<<" prompt"<<endl;
    if (verbose) cout<<"In bin 3: "<<h_LooseNotTight->ProjectionX()->GetBinContent(3)+h_LooseNotTightEWK->ProjectionX()->GetBinContent(3)<<" events, of which "<<h_LooseNotTightEWK->ProjectionX()->GetBinContent(3)<<" prompt"<<endl;




    //initialize prediction array
    //  const int n_mt2bins = sr.GetNumberOfMT2Bins();
    const int n_mtbins = h_LooseNotTight->GetNbinsX();
    Double_t preds[n_mtbins+2]; //first and last bin are over/underflow
    Double_t predErrors[n_mtbins+2]; //first and last bin are over/underflow
    for(int i=0; i<n_mtbins+2; i++){
      preds[i] = 0;
      predErrors[i] = 0;
    }
    
    //loop over mtbins
    for (Int_t xbin = 0; xbin < n_mtbins; xbin++)
    {
      if (verbose) cout<<" --- Looking at MT bin "<<xbin<<endl;
      //initialize pred_totals
      Float_t pred_total       = 0.0;
      Float_t pred_error_total = 0.0;
      
      //loop over FR bins
      for (Int_t FRbin = 1; FRbin < h_FR->GetSize()+1; FRbin++)
      {
        
        Float_t nFOs      = h_LooseNotTight->GetBinContent(xbin+1, FRbin);    // number of denominator not numerator objects for this bin

        if (nFOs<0) nFOs = 0;
        Float_t nFOsError = h_LooseNotTight->GetBinError(xbin+1, FRbin);      // number error on number of of denominator not numerator objects for this bin
        
        Float_t FRvalue   = h_FR->GetBinContent(FRbin); // get value of fake rate for this bin
        Float_t FRerror   = h_FR->GetBinError(FRbin);   // get error on fake rate for this bin
        
        //--- error propagation shamelessly stolen from frank golfs code---
        
        // ok, have all of the inputs, calculate prediction and error for this bin
        // start with calculating fr/(1-fr) for this bin
        const Float_t fr = FRvalue / (1 - FRvalue);
        const Float_t pred = fr * nFOs;

        if (verbose && nFOs>0) cout<<"FR bin "<<FRbin<<" has FR "<<FRvalue<<", nFO "<<nFOs<<" and predicted "<<pred<<endl;
        
        // now, need to get errors on these terms, be careful
        // start with hardest part, error on fr
        //
        // start with basic equation: (sigma_f/f)^2 = (sigma_num/num)^2 + (sigma_den/den)^2 - 2 * (sigma_num/num) * (sigma_den/den) * rho_num_den
        // f is the value fr from above
        // num is the value FRvalue from above
        // den is 1 - FRvalue
        //
        // sigma_num is the value FRerror from above
        // sigma_den is also the value FRerror from above (the error on 1 - A is just the error on A)
        //
        // the numerator and denominator are completely anti-correlated, so rho_num_den is -1
        //
        // combining all of this we can simplify the above to be
        //
        // (sigma_f/f)^2 = (sigma_num/num)^2 + (sigma_num/(1-num))^2 + 2 * sigma_num^2 * (1/num) * (1/(1-num))
        // (sigma_f/f)^2 = sigma_num^2 * [1/num + 1/(1-num)]^2
        // sigma_f = f * sigma_num * (1/(num * (1-num)))
        // sigma_f = sigma_num * (1 - num)^{-2}
        //
        Float_t frError = FRerror * pow((1 - FRvalue), -2);
        
        // now that we have the error on fr/(1-fr), the error on the prediction is
        // just the sum of this error and the error on FO count added in quadrature
        // (sigma_pred/pred)^2 = (sigma_fr/fr)^2 + (sigma_fo/fo)^2
        Float_t pred_error = (nFOs > 0) ? pred * sqrt(pow(frError/fr, 2) + pow(nFOsError/nFOs, 2)) : 0.;
        
        // now increment the total values
        if (pred > 0){
          pred_total += pred;
          pred_error_total += pow(pred_error, 2);
        }
        
      }//exit FR bin loop
      
      //fill the pred array with new pred and errors
      preds[xbin+1]=pred_total;
      predErrors[xbin+1]=sqrt(pred_error_total);
      predsTot[xbin+1]+=pred_total;
      predTotErrors[xbin+1]+=sqrt(pred_error_total);
      if (verbose) cout<<" --- Total for this MT bin: "<<pred_total<<" +/-"<<sqrt(pred_error_total)<<endl;
      
    }//exit mtbin loop
    
    //make and fill prediction histogram
    f_out->cd();
    TString directory = srName;
    TDirectory* dir = 0;
    dir = (TDirectory*)f_out->Get(directory.Data());
    if (dir == 0) {
      dir = f_out->mkdir(directory.Data());
    }
    dir->cd();
    
    TH1D* h_pred = h_LooseNotTight->ProjectionX();
    h_pred->Reset();
    h_pred->SetName("h_pred"+suffix+lep);
    h_pred->SetContent(preds);
    h_pred->SetError(predErrors);
    h_pred->Write();
    
    h_LooseNotTightPromptFrac->Write();
    
  }
  
  //make and fill El+Mu prediction histogram
  f_out->cd();
  TString directory = srName;
  TDirectory* dir = 0;
  dir = (TDirectory*)f_out->Get(directory.Data());
  if (dir == 0) {
    dir = f_out->mkdir(directory.Data());
  }
  dir->cd();
  
  TH1D* h_predTot = h_LooseNotTightTemplate->ProjectionX();
  h_predTot->Reset();
  h_predTot->SetName("h_pred"+suffix);
  h_predTot->SetContent(predsTot);
  h_predTot->SetError(predTotErrors);
  h_predTot->Write();
  
  
  // Also dump the MC prediction (would be nice to have e and mu separately)
  TH1F* MCpred = (TH1F*) f_mc->Get(srName+"/h_mtbinsFake");
  if (MCpred) {
    MCpred->SetName("h_predMC"+suffix);
    MCpred->Write();
  }
  TH1F* MCpredEl = (TH1F*) f_mc->Get(srName+"/h_mtbinsElTightFake");
  if (MCpredEl) {
    MCpredEl->SetName("h_predElMC"+suffix);
    MCpredEl->Write();
  }
  TH1F* MCpredMu = (TH1F*) f_mc->Get(srName+"/h_mtbinsMuTightFake");
  if (MCpredMu) {
    MCpredMu->SetName("h_predMuMC"+suffix);
    MCpredMu->Write();
  }
  
  //Make prediction with MC for bins 1 and 2, and data-driven (if available) for bin 3
  TH1D* h_predTotMC12 = (TH1D*) h_predTot->Clone("h_pred"+suffix+"MC12");
  if (MCpred) {
    h_predTotMC12->SetBinContent(1, MCpred->GetBinContent(1));
    h_predTotMC12->SetBinError(1, MCpred->GetBinError(1));
    h_predTotMC12->SetBinContent(2, MCpred->GetBinContent(2));
    h_predTotMC12->SetBinError(2, MCpred->GetBinError(2));
    if (h_predTot->GetBinContent(3)==0) {
      h_predTotMC12->SetBinContent(3, MCpred->GetBinContent(3));
      h_predTotMC12->SetBinError(3, MCpred->GetBinError(3));
    }
  }

  
  h_predTotMC12->Write();
  
  return;
  
}

TH1D* measureFR(TFile * f_out, TFile * file, TFile * fileEWK, TString cuts = "EWKcuts", TString HT = "HT800", TString lep = "El", bool data = true, bool subtractEWK = true) {
  
  // Get numerator and denominator
  TString name ="nocut/h_lepptshortBins"+cuts+HT+lep;
  TString nameLoose = name + "Loose";
  TString nameTight = name + "Tight";
  if (!data) { nameLoose += "Fake"; nameTight += "Fake"; }
  TH1D* num = (TH1D*) file->Get(nameTight.Data());
  TH1D* den = (TH1D*) file->Get(nameLoose.Data());
  if (!num) cout<<"Could not find "<<nameTight<<" in "<<file->GetName()<<endl;
  if (!den) cout<<"Could not find "<<nameLoose<<" in "<<file->GetName()<<endl;
  
  if (subtractEWK) {
    // First normalize
    TString nameMT = "nocut/h_mtMET40"+HT+lep+"Tight";
    TH1D* mt    = (TH1D*) file->Get(nameMT.Data());
    TH1D* mtEWK = (TH1D*) fileEWK->Get(nameMT.Data());
    if (!mt)    cout<<"Could not find "<<nameMT<<" in "<<file->GetName()<<endl;
    if (!mtEWK) cout<<"Could not find "<<nameMT<<" in "<<fileEWK->GetName()<<endl;
    TAxis *axis = mt->GetXaxis();
    int bmin = axis->FindBin(70);
    int bmax = axis->FindBin(100);
    double errEWK = 0;
    double intEWK = mtEWK->IntegralAndError(bmin, bmax, errEWK);
    double errData = 0;
    double intData = mt->IntegralAndError(bmin, bmax, errData);
    double scaleEwkToData = intData / intEWK;
    double errRatio = scaleEwkToData * sqrt( pow(errData/intData, 2) + pow(errEWK/intEWK, 2) );
    cout << "EWK component scaled by "<<scaleEwkToData<<" +/- "<<errRatio<<endl;
    
    // Then subtract
    TString nameLooseEWK = name + "Loose";
    TString nameTightEWK = name + "Tight";
    TH1D* numEWK = (TH1D*) fileEWK->Get(nameTightEWK.Data());
    TH1D* denEWK = (TH1D*) fileEWK->Get(nameLooseEWK.Data());
    if (!numEWK) cout<<"Could not find "<<nameTightEWK<<" in "<<fileEWK->GetName()<<endl;
    if (!denEWK) cout<<"Could not find "<<nameLooseEWK<<" in "<<fileEWK->GetName()<<endl;
    if (numEWK) { numEWK->Scale(scaleEwkToData); num->Add(numEWK, -1); }
    if (denEWK) { denEWK->Scale(scaleEwkToData); den->Add(denEWK, -1); }
  }
  
  // Divide
  TH1D* fr = (TH1D*) num->Clone("FR"+cuts+HT+lep);
  TString newname = fr->GetName();
  if (data) newname += "_Data";
  else newname += "_MC";
  if (subtractEWK) newname += "_EwkSubtr";
  fr->SetName(newname);
  fr->Divide(num, den, 1, 1, "B");
  
  // Add uncertainty
  
  
  // Save in output file
  f_out->cd();
  fr->Write();
  
  // Return
  return fr;
  
}


//_______________________________________________________________________________
void makeFakeRateEstimate(string input_dir = "../../SoftLepLooper/output/softLep/", string input_dir_FR = "../../SoftLepLooper/output/softLepFake/", string dataname = "data_Run2015CD"){
  
  
  string output_name = input_dir+"/pred_FakeRate.root";
  // ----------------------------------------
  //  samples definition
  // ----------------------------------------
  
  std::cout << "Writing to file: " << output_name << std::endl;
  
  TFile * f_out = new TFile(output_name.c_str(),"RECREATE") ;
  
  // get input files
  TString datanamestring(dataname);
  
  if (datanamestring.Contains("Data") || datanamestring.Contains("data")) realData = true;
  TFile* f_data = new TFile(Form("%s/%s.root",input_dir.c_str(),dataname.c_str())); //data or dummy-data file
  TFile* f_mc   = new TFile(Form("%s/allBkg.root",input_dir.c_str()));
  TFile* f_dataFR = new TFile(Form("%s/%s.root",input_dir_FR.c_str(),dataname.c_str())); //data or dummy-data file
  TFile* f_mcFR   = new TFile(Form("%s/allBkg.root",input_dir_FR.c_str()));
 // TFile* f_mcFakes   = new TFile(Form("%s/zinv_ht.root",input_dir_FR.c_str()));
  
//  if(f_data->IsZombie() || f_mc->IsZombie() || f_dataFR->IsZombie() || f_mcFR->IsZombie()) {
  
    if(f_mcFR->IsZombie()) {
    std::cerr << "Input file does not exist" << std::endl;
    return;
  }
  
  // Measure Fake Rate
  TH1D* h_FRmcE = measureFR(f_out, f_mcFR, f_mcFR, "EWKcuts", "HT800", "El", false, false);
  TH1D* h_FRmcM = measureFR(f_out, f_mcFR, f_mcFR, "EWKcuts", "HT800", "Mu", false, false);
  h_FRmcE->Print("all");
  h_FRmcM->Print("all");
  
  TH1D* h_FRdataE = measureFR(f_out, f_dataFR, f_mcFR, "EWKcuts", "HT800", "El", true, false);
  TH1D* h_FRdataM = measureFR(f_out, f_dataFR, f_mcFR, "EWKcuts", "HT800", "Mu", true, false);
  h_FRdataE->Print("all");
  h_FRdataM->Print("all");
  TH1D* h_FRdataEewk = measureFR(f_out, f_dataFR, f_mcFR, "EWKcuts", "HT800", "El", true, true);
  TH1D* h_FRdataMewk = measureFR(f_out, f_dataFR, f_mcFR, "EWKcuts", "HT800", "Mu", true, true);
  h_FRdataEewk->Print("all");
  h_FRdataMewk->Print("all");
  
  
  // Apply Fake Rate
  TIter it(f_data->GetListOfKeys());
  TKey* k;
  while ((k = (TKey *)it())) {
    std::string dir_name = k->GetTitle();
    if(dir_name.find("srLep")==std::string::npos) continue; //to do only signal regions
    //if (dir_name != "srLepbase") continue; // test
    cout << "Calculating prediction for " << dir_name << "..." << endl;
    //void makeFRpred(TFile* f_data ,TFile* f_mc ,TFile* f_out , TH1D* h_FRdataEewk, TH1D* h_FRdataMewk, TString srName, TString suffix ) {
    makeFRpred( f_data , f_mc , f_out , h_FRdataEewk, h_FRdataMewk, TString(dir_name), "" );
    makeFRpred( f_mc , f_mc , f_out , h_FRmcE, h_FRmcM, TString(dir_name), "MCClosure" );
    
  }
  
  return;
  
}


