// C++
#include <iostream>
#include <set>
#include <cmath>

// ROOT
#include "TDirectory.h"
#include "TTreeCache.h"
#include "TH1D.h"

//define input files
string inputDir = "output/noskim_apr28";
//define reweight binning
double ptBins[32] ={200,210,220,230,240,250,260,270,280,290,300,310,320,330,340,350,360,370,380,390,400,420,440,460,480,500,550,600,650,700,800,1000};

void makeReweight(){

  TFile* f_data = new TFile(Form("%s/data_Run2015CD.root",inputDir.c_str()));
  TFile* f_MC = new TFile(Form("%s/gjet_ht.root",inputDir.c_str()));
  TFile* f_out = new TFile("data/reweight.root","RECREATE");

  TH1D* h_data = (TH1D*) f_data->Get("crgjetbase/h_gammaPt");
  TH1D* h_mc   = (TH1D*) f_MC->Get("crgjetbase/h_gammaPt");

  f_out->cd();
    
  h_data->Rebin(31,"h_dataRebin",ptBins);
  h_mc->Rebin(31,"h_mcRebin",ptBins);
  
  TH1D* h_dataRebin = (TH1D*) f_out->Get("h_dataRebin");
  TH1D* h_mcRebin   = (TH1D*) f_out->Get("h_mcRebin");
  
  TH1D* h_ratio = (TH1D*) h_dataRebin->Clone();

  h_ratio->SetName("h_gammaPt_ratio");

  h_ratio->Reset();

  h_ratio->Divide(h_dataRebin,h_mcRebin);

  h_ratio->Write();
  f_out->Write();

  f_out->Close();

  return;
}
