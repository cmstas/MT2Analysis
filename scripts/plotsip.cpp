#include "TFile.h"
#include "../../tas/Software/dataMCplotMaker/PlotMaker2D.h"
#include "TH1F.h"
#include "TLine.h"
#include <iostream>
#include <string>

inline void plotDiv(TH1F* h_div, string name = "div", string ytitle = "withIso/noIso")
{
  gStyle->SetOptStat("");
  gStyle->SetCanvasColor(0);
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetLineStyle(2);
  gStyle->SetFrameBorderMode(0);

  gStyle->SetPadTopMargin(0.08);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.05);

  TCanvas* can = new TCanvas("c1", "c1", 600, 400);

  h_div->SetMarkerStyle(20);
  //h_div->SetMarkerSize(1.2);
  h_div->SetMaximum(1);

  //h_div->SetTitle("");
  float labelsize = 1.5 * h_div->GetXaxis()->GetLabelSize();
  h_div->GetXaxis()->SetLabelSize(labelsize);
  h_div->GetYaxis()->SetLabelSize(labelsize);
  h_div->GetXaxis()->SetTitle("p_{T} [GeV]");
  h_div->GetXaxis()->SetTitleSize(labelsize);
  // h_div->GetXaxis()->CenterTitle();
  h_div->GetYaxis()->SetTitle(ytitle.c_str());
  h_div->GetYaxis()->SetTitleSize(1.2*labelsize);
  //TLine* l1 = new TLine(0, 1, 25, 1);
  TLine* l2 = new TLine(0, 0.5, 25, 0.5);
  h_div->Draw("Phist");
  //.ql1->Draw("same");
  l2->Draw("same");
  can->Print(Form("plots/c_%s.pdf", name.c_str()));
  can->Clear();
  delete can;
}


void plotsip()
{
  TFile* fni_ttbar = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/noiso_mt2g200/zinv_ht.root");
  TFile* fni_wjets = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/noiso_mt2g200/wjets_ht.root");

  TH1F* h_smusip_zinv = (TH1F*) fni_ttbar->Get("srsmNewbase/h_smu_sip");
  TH1F* h_smusip_wjets = (TH1F*) fni_wjets->Get("srsmNewbase/h_smu_sip");

  h_smusip_zinv->Scale(1./h_smusip_zinv->Integral());
  h_smusip_wjets->Scale(1./h_smusip_wjets->Integral());

  h_smusip_zinv->SetMarkerColor(kRed);
  h_smusip_zinv->SetMarkerStyle(20);
  h_smusip_zinv->Draw();
  h_smusip_wjets->Draw("same");

}
