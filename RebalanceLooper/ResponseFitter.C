#include "TMath.h"
#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"

//fits with three fucking gaussians
Double_t TripleGaussian(Double_t *x, Double_t *par){
  return par[0]*TMath::Exp(-0.5*((x[0]-par[1])/par[2])*((x[0]-par[1])/par[2])) + par[3]*TMath::Exp(-0.5*((x[0]-par[4])/par[5])*((x[0]-par[4])/par[5])) + par[6]*TMath::Exp(-0.5*((x[0]-par[7])/par[8])*((x[0]-par[7])/par[8]));
}

Double_t SingleGaussian(Double_t *x, Double_t *par){
  return par[0]*TMath::Exp(-0.5*((x[0]-par[1])/par[2])*((x[0]-par[1])/par[2]));
}

void ResponseFitter(){

  TFile* outfile = new TFile("template_fits.root", "RECREATE");
  TFile* responseFile = new TFile("/home/users/jgran/QCD_13TeV_MGMLM_Spring15_bestMatching_angles_withNeutrinos.root", "READ");
  TCanvas* c1 = new TCanvas();
  TIter it(responseFile->GetListOfKeys());
  TKey* key;
  std::string keep = "h_tot_JetAll_ResponsePt";
  while ( (key = (TKey *)it()) ) {
    if (strncmp (key->GetTitle(), keep.c_str(), keep.length()) != 0) continue;
    std::string hist_name = (key->GetTitle());
    TH1F* h = (TH1F*) responseFile->Get(TString(hist_name));
    if(h->GetEntries() < 100) continue;
    h->Scale(1.0/h->Integral());
    TF1* fit = new TF1(Form("fit_%s", hist_name.c_str()), SingleGaussian, 0.03, 1.0, 3);
    fit->SetTitle(Form("fit_%s", hist_name.c_str()));
    float mean = h->GetBinCenter(h->GetMaximumBin());
    float max = h->GetBinContent(h->GetMaximumBin());
    
    fit->SetParameters(max, mean, 0.1);
    fit->SetParLimits(0, max - 0.0005, max + 0.0005);
    fit->SetParLimits(1, mean - 0.02, mean + 0.02);
    float side = h->GetRMS();
    if(side > 0.1) side *= 0.5;
    fit->SetRange(mean - side, mean + side);
    h->Fit(Form("fit_%s", hist_name.c_str()), "R");
    fit->SetRange(0, 3);
    fit->Print();
    fit->Update();
    c1->cd();
    c1->Print(Form("fits/fit_%s.pdf", hist_name.c_str()));
    outfile->cd();
    fit->Write();
    responseFile->cd();
    delete fit;
  }
  responseFile->Close();
  delete responseFile;
  delete c1;

  outfile->Close();
  delete outfile;
}
