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


void plotIso()
{
  TFile* fni_ttbar = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/noiso_mt2g200/ttall_msdecays.root");
  TFile* fni_wjets = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/noiso_mt2g200/wjets_ht.root");

  TH1F* h_genmu_ttbar = (TH1F*) fni_ttbar->Get("srsmNewbase/h_allgenmu_pt");
  TH1F* h_genmu_wjets = (TH1F*) fni_wjets->Get("srsmNewbase/h_allgenmu_pt");

  TH1F* h_allgenmu_pt = new TH1F("allgenmu", "allgenmu", 200, 0, 1000);

  h_allgenmu_pt->Add(h_genmu_ttbar);
  h_allgenmu_pt->Add(h_genmu_wjets);

  // TCanvs* c1 = new TCanvas;
  // h_allgenmu_pt->Draw();

  std::cout << "Total count (scaled) for all gen muon: " << h_allgenmu_pt->Integral() << std::endl;

  TH1F* h_goodmu_ttbar = (TH1F*) fni_ttbar->Get("srsmNewbase/h_goodgenmu_pt");
  TH1F* h_goodmu_wjets = (TH1F*) fni_wjets->Get("srsmNewbase/h_goodgenmu_pt");

  TH1F* h_goodgenmu_pt = new TH1F("goodgenmu", "goodgenmu", 50, 0, 25);

  h_goodgenmu_pt->Add(h_goodmu_ttbar);
  h_goodgenmu_pt->Add(h_goodmu_wjets);

  // TCanvs* c2 = new TCanvas;
  // h_goodgenmu_pt->Draw();

  std::cout << "Total count (scaled) for good (pt+eta) gen muon: " << h_goodgenmu_pt->Integral() << std::endl;
  std::cout << "The efficiency (1) goodGenMu/allGenMu is: " << h_goodgenmu_pt->Integral() / h_allgenmu_pt->Integral() << std::endl;

  TH1F* h_recmu_ttbar = (TH1F*) fni_ttbar->Get("srsmNewbase/h_matMupt");
  TH1F* h_recmu_wjets = (TH1F*) fni_wjets->Get("srsmNewbase/h_matMupt");

  TH1F* h_recmu_pt = new TH1F("recogen_mupt", "rg_mupt", 50, 0, 25);

  h_recmu_pt->Add(h_recmu_ttbar);
  h_recmu_pt->Add(h_recmu_wjets);

  std::cout << "Total count (scaled) for reco (noiso) muon: " << h_recmu_pt->Integral() << std::endl;
  std::cout << "The efficiency (2) recoMu/goodGenMu is: " << h_recmu_pt->Integral() / h_goodgenmu_pt->Integral() << std::endl;

  TFile* fwi_ttbar = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/wiso_mt2g200/ttall_msdecays.root");
  TFile* fwi_wjets = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/wiso_mt2g200/wjets_ht.root");

  TH1F* h_wirecmu_ttbar = (TH1F*) fwi_ttbar->Get("srsmNewbase/h_matMupt");
  TH1F* h_wirecmu_wjets = (TH1F*) fwi_wjets->Get("srsmNewbase/h_matMupt");

  TH1F* h_wirecmu_pt = new TH1F("wini_mupt", "wini_mupt", 50, 0, 25);

  h_wirecmu_pt->Add(h_wirecmu_ttbar);
  h_wirecmu_pt->Add(h_wirecmu_wjets);

  std::cout << "Total count (scaled) for reco (withiso) muon: " << h_wirecmu_pt->Integral() << std::endl;
  std::cout << "The efficiency (3) withIsoRecoMu/noIsoRecoMu is: " << h_wirecmu_pt->Integral() / h_recmu_pt->Integral() << std::endl;

  TH1F* h_div_rec = (TH1F*) h_wirecmu_pt->Clone("h_div_rec");
  h_div_rec->Divide(h_recmu_pt);
  //h_wirecmu_pt->Divide(h_goodgenmu_pt);

  //h_recmu_pt->Draw();
  //h_wirecmu_pt->Draw();
  // Ploting the division hist
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

  // TCanvas* can = new TCanvas("wiso_over_noiso", "wiso_over_noiso", 600, 400);

  // h_div_rec->SetMarkerStyle(20);
  // //h_div_rec->SetMarkerSize(1.2);
  // h_div_rec->SetMaximum(1);

  // h_div_rec->SetTitle("");
  // float labelsize = 1.2 * h_div_rec->GetXaxis()->GetLabelSize();
  // h_div_rec->GetXaxis()->SetLabelSize(labelsize);
  // h_div_rec->GetYaxis()->SetLabelSize(labelsize);
  // h_div_rec->GetXaxis()->SetTitle("p_{T} [GeV]");
  // h_div_rec->GetXaxis()->SetTitleSize(labelsize);
  // // h_div_rec->GetXaxis()->CenterTitle();
  // h_div_rec->GetYaxis()->SetTitle("withIso/noIso");
  // h_div_rec->GetYaxis()->SetTitleSize(1.2*labelsize);
  // //TLine* l1 = new TLine(0, 1, 25, 1);
  // TLine* l2 = new TLine(0, 0.5, 25, 0.5);
  // h_div_rec->Draw("Phist");
  // //.ql1->Draw("same");
  // l2->Draw("same");
  // can->Print("div_wini.pdf");

  TFile* fai_ttbar = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/absiso_mt2g200/ttall_msdecays.root");
  TFile* fai_wjets = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/absiso_mt2g200/wjets_ht.root");

  TH1F* h_airecmu_ttbar = (TH1F*) fai_ttbar->Get("srsmNewbase/h_matMupt");
  TH1F* h_airecmu_wjets = (TH1F*) fai_wjets->Get("srsmNewbase/h_matMupt");

  TH1F* h_absdiv_ttbar = (TH1F*) h_airecmu_ttbar->Clone("eff_absIso_ttbar");
  TH1F* h_absdiv_wjets = (TH1F*) h_airecmu_wjets->Clone("eff_absIso_wjets");

  h_recmu_ttbar->Rebin(5);
  h_recmu_wjets->Rebin(5);

  h_absdiv_ttbar->Rebin(5);
  h_absdiv_wjets->Rebin(5);

  h_absdiv_ttbar->Divide(h_recmu_ttbar);
  h_absdiv_wjets->Divide(h_recmu_wjets);

  // plotDiv(h_absdiv_ttbar, "abs_ttbar", "absIso/noIso");
  // plotDiv(h_absdiv_wjets, "abs_wjets", "absIso/noIso");

  TH1F* h_mridiv_ttbar = (TH1F*) h_wirecmu_ttbar->Clone("eff_miniRelIso_ttbar");
  TH1F* h_mridiv_wjets = (TH1F*) h_wirecmu_wjets->Clone("eff_miniRelIso_wjets");

  h_mridiv_ttbar->Rebin(5);
  h_mridiv_wjets->Rebin(5);

  h_mridiv_ttbar->Divide(h_recmu_ttbar);
  h_mridiv_wjets->Divide(h_recmu_wjets);

  // plotDiv(h_mridiv_ttbar, "mri_ttbar", "miniRelIso/noIso");
  // plotDiv(h_mridiv_wjets, "mri_wjets", "miniRelIso/noIso");

  TH1F* h_mrigen_ttbar = (TH1F*) h_wirecmu_ttbar->Clone("effnew_miniRelIso_ttbar");
  TH1F* h_mrigen_wjets = (TH1F*) h_wirecmu_wjets->Clone("effnew_miniRelIso_wjets");

  h_goodmu_ttbar->Rebin(5);
  h_goodmu_wjets->Rebin(5);

  h_mrigen_ttbar->Rebin(5);
  h_mrigen_wjets->Rebin(5);

  h_mrigen_ttbar->Divide(h_goodmu_ttbar);
  h_mrigen_wjets->Divide(h_goodmu_wjets);


  // plotDiv(h_mrigen_ttbar, "mrigen_ttbar", "reco/gen");
  // plotDiv(h_mrigen_wjets, "mrigen_wjets", "reco/gen");

  
  TH1F* h_absgen_ttbar = (TH1F*) h_airecmu_ttbar->Clone("effnew_absIso_ttbar");
  TH1F* h_absgen_wjets = (TH1F*) h_airecmu_wjets->Clone("effnew_absIso_wjets");
  
  h_absgen_ttbar->Rebin(5);
  h_absgen_wjets->Rebin(5);

  h_absgen_ttbar->Divide(h_goodmu_ttbar);
  h_absgen_wjets->Divide(h_goodmu_wjets);

  // plotDiv(h_absgen_ttbar, "absgen_ttbar", "reco/gen");
  // plotDiv(h_absgen_wjets, "absgen_wjets", "reco/gen");


}
