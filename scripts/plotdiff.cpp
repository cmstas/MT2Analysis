#include "TFile.h"
#include "TH1F.h"
#include "THStack.h"
#include "TLine.h"
#include "TStyle.h"
#include <iostream>
#include <string>
#include "/home/users/sicheng/tas/Software/dataMCplotMaker/dataMCplotMaker.h"

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

inline void plotDiv(TH1F* h1, TH1F* h2, string name = "div", string title1 = "", string title2 = "")
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

  h1->SetMarkerStyle(20);
  h2->SetMarkerStyle(21);
  h2->SetMarkerColor(kRed);
  //h_div->SetMarkerSize(1.2);
  h1->SetMaximum(1);

  //h1->SetTitle("");
  float labelsize = 1.5 * h1->GetXaxis()->GetLabelSize();
  h1->GetXaxis()->SetLabelSize(labelsize);
  h1->GetYaxis()->SetLabelSize(labelsize);
  h1->GetXaxis()->SetTitle("p_{T} [GeV]");
  h1->GetXaxis()->SetTitleSize(labelsize);
  // h1->GetXaxis()->CenterTitle();
  h1->GetYaxis()->SetTitle(title1.c_str());
  h1->GetYaxis()->SetTitleSize(1.2*labelsize);
  //TLine* l1 = new TLine(0, 1, 25, 1);
  TLine* l2 = new TLine(0, 0.5, 25, 0.5);
  TLegend* leg = new TLegend(0.75, 0.6, 0.92, 0.7);
  leg->AddEntry(h1, title1.c_str());
  leg->AddEntry(h2, title2.c_str());

  h1->Draw("Phist");
  h2->Draw("Phistsame");
  //l1->Draw("same");
  l2->Draw("same");
  leg->Draw("same");

  can->Print(Form("plots/c_%s.pdf", name.c_str()));
  can->Clear();
  delete can;

}

void plotdiff()
{
  TFile* f_ttbar = new TFile("/home/users/sicheng/tas/MT2Analysis/MT2looper/output/skim_elec5_absiso/ttall_mg_lo.root");
  TFile* f_wjets = new TFile("/home/users/sicheng/tas/MT2Analysis/MT2looper/output/skim_elec5_absiso/wjets_ht.root");
  TFile* f_zinv  = new TFile("/home/users/sicheng/tas/MT2Analysis/MT2looper/output/skim_elec5_absiso/zinv_ht.root");
  TFile* f_qcd   = new TFile("/home/users/sicheng/tas/MT2Analysis/MT2looper/output/skim_elec5_absiso/qcd_pt.root");
  TFile* f_sig   = new TFile("/home/users/sicheng/tas/MT2Analysis/MT2looper/output/skim_elec5_absiso/T5qqqqWW_1000_775_750.root");
  
  TH1F* h_mupt_ttbar = (TH1F*) f_ttbar->Get("srsmbase/h_mupt");
  TH1F* h_mupt_wjets = (TH1F*) f_wjets->Get("srsmbase/h_mupt");
  TH1F* h_mupt_zinv  = (TH1F*) f_zinv ->Get("srsmbase/h_mupt");
  TH1F* h_mupt_qcd   = (TH1F*) f_qcd  ->Get("srsmbase/h_mupt");

  TH1F* h_ept_ttbar  = (TH1F*) f_ttbar->Get("srsebase/h_mupt");
  TH1F* h_ept_wjets  = (TH1F*) f_wjets->Get("srsebase/h_mupt");
  TH1F* h_ept_zinv   = (TH1F*) f_zinv ->Get("srsebase/h_mupt");
  TH1F* h_ept_qcd    = (TH1F*) f_qcd  ->Get("srsebase/h_mupt");

  THStack* stack_mupt = new THStack("stack_mupt", "Comparison of p_{T} between #mu and e ");
  THStack* stack_ept  = new THStack("stack_ept" , "stack of ept" );


  h_mupt_zinv ->SetFillColor(419);
  h_mupt_wjets->SetFillColor(417);
  h_mupt_ttbar->SetFillColor(855);

  h_mupt_zinv ->Rebin(5);
  h_mupt_ttbar->Rebin(5);
  h_mupt_wjets->Rebin(5);
  //h_mupt_qcd  ->Rebin(2);
  
  stack_mupt->Add(h_mupt_zinv );
  stack_mupt->Add(h_mupt_ttbar);
  stack_mupt->Add(h_mupt_wjets);
  //stack_mupt->Add(h_mupt_qcd  );
  
  // h_ept_zinv ->SetMarkerColor(kGreen+3);
  // h_ept_wjets->SetMarkerColor(kCyan+2);
  // h_ept_ttbar->SetMarkerColor(kGreen-4);
  // h_ept_qcd  ->SetMarkerColor(kOrange+2);
  h_ept_zinv ->Rebin(5);
  h_ept_ttbar->Rebin(5);
  h_ept_wjets->Rebin(5);
  //h_ept_qcd  ->Rebin(2);

  h_ept_zinv ->SetMarkerStyle(22);
  h_ept_ttbar->SetMarkerStyle(20);
  h_ept_wjets->SetMarkerStyle(21);
  h_ept_qcd  ->SetMarkerStyle(23);

  stack_ept->Add(h_ept_zinv );
  stack_ept->Add(h_ept_ttbar);
  stack_ept->Add(h_ept_wjets);
  //stack_ept->Add(h_ept_qcd  );
  h_mupt_ttbar->GetXaxis()->SetTitle("p_{T} [GeV]");
  
  TCanvas* c0 = new TCanvas("c0", "c0", 600, 600);
  //c1->SetLogy();
  TLegend* leg = new TLegend(0.75, 0.6, 0.9, 0.9);
  leg->AddEntry(h_mupt_zinv , "mu_zinv ");
  leg->AddEntry(h_mupt_ttbar, "mu_ttbar");
  leg->AddEntry(h_mupt_wjets, "mu_wjets");

  leg->AddEntry(h_ept_zinv , "e_zinv ");
  leg->AddEntry(h_ept_ttbar, "e_ttbar");
  leg->AddEntry(h_ept_wjets, "e_wjets");


  stack_mupt->Draw("hist");
  stack_ept->Draw("same");
  leg->Draw("same");

  // ---------------------------
  // Get Separate Yields in mupt
  // ---------------------------
  TH1F* h_mupt1_ttbar = (TH1F*) f_ttbar->Get("srsmbase/h_mt_mpt5-10");
  TH1F* h_mupt1_wjets = (TH1F*) f_wjets->Get("srsmbase/h_mt_mpt5-10");
  TH1F* h_mupt1_zinv  = (TH1F*) f_zinv ->Get("srsmbase/h_mt_mpt5-10");
  TH1F* h_mupt1_qcd   = (TH1F*) f_qcd  ->Get("srsmbase/h_mt_mpt5-10");
  TH1F* h_mupt1_sig   = (TH1F*) f_sig  ->Get("srsmbase/h_mt_mpt5-10");

  TH1F* h_mupt2_ttbar = (TH1F*) f_ttbar->Get("srsmbase/h_mt_mpt10-15");
  TH1F* h_mupt2_wjets = (TH1F*) f_wjets->Get("srsmbase/h_mt_mpt10-15");
  TH1F* h_mupt2_zinv  = (TH1F*) f_zinv ->Get("srsmbase/h_mt_mpt10-15");
  TH1F* h_mupt2_qcd   = (TH1F*) f_qcd  ->Get("srsmbase/h_mt_mpt10-15");
  TH1F* h_mupt2_sig   = (TH1F*) f_sig  ->Get("srsmbase/h_mt_mpt10-15");

  TH1F* h_mupt3_ttbar = (TH1F*) f_ttbar->Get("srsmbase/h_mt_mpt15-20");
  TH1F* h_mupt3_wjets = (TH1F*) f_wjets->Get("srsmbase/h_mt_mpt15-20");
  TH1F* h_mupt3_zinv  = (TH1F*) f_zinv ->Get("srsmbase/h_mt_mpt15-20");
  TH1F* h_mupt3_qcd   = (TH1F*) f_qcd  ->Get("srsmbase/h_mt_mpt15-20");
  TH1F* h_mupt3_sig   = (TH1F*) f_sig  ->Get("srsmbase/h_mt_mpt15-20");

  TH1F* h_smbg1 = new TH1F("sepmBG_1","Background with mupt5-10 ", 200, 0, 500);
  TH1F* h_smbg2 = new TH1F("sepmBG_2","Background with mupt10-15", 200, 0, 500);
  TH1F* h_smbg3 = new TH1F("sepmBG_3","Background with mupt15-20", 200, 0, 500);

  h_smbg1->Add(h_mupt1_ttbar);
  h_smbg1->Add(h_mupt1_wjets);
  h_smbg1->Add(h_mupt1_zinv );
  // h_smbg1->Add(h_mupt1_qcd  );

  h_smbg2->Add(h_mupt2_ttbar);
  h_smbg2->Add(h_mupt2_wjets);
  h_smbg2->Add(h_mupt2_zinv );
  // h_smbg2->Add(h_mupt2_qcd  );

  h_smbg3->Add(h_mupt3_ttbar);
  h_smbg3->Add(h_mupt3_wjets);
  h_smbg3->Add(h_mupt3_zinv );
  // h_smbg3->Add(h_mupt3_qcd  );

  TH1F* h_ept1_ttbar = (TH1F*) f_ttbar->Get("srsebase/h_mt_mpt5-10");
  TH1F* h_ept1_wjets = (TH1F*) f_wjets->Get("srsebase/h_mt_mpt5-10");
  TH1F* h_ept1_zinv  = (TH1F*) f_zinv ->Get("srsebase/h_mt_mpt5-10");
  TH1F* h_ept1_qcd   = (TH1F*) f_qcd  ->Get("srsebase/h_mt_mpt5-10");
  TH1F* h_ept1_sig   = (TH1F*) f_sig  ->Get("srsebase/h_mt_mpt5-10");

  TH1F* h_ept2_ttbar = (TH1F*) f_ttbar->Get("srsebase/h_mt_mpt10-15");
  TH1F* h_ept2_wjets = (TH1F*) f_wjets->Get("srsebase/h_mt_mpt10-15");
  TH1F* h_ept2_zinv  = (TH1F*) f_zinv ->Get("srsebase/h_mt_mpt10-15");
  TH1F* h_ept2_qcd   = (TH1F*) f_qcd  ->Get("srsebase/h_mt_mpt10-15");
  TH1F* h_ept2_sig   = (TH1F*) f_sig  ->Get("srsebase/h_mt_mpt10-15");

  TH1F* h_ept3_ttbar = (TH1F*) f_ttbar->Get("srsebase/h_mt_mpt15-20");
  TH1F* h_ept3_wjets = (TH1F*) f_wjets->Get("srsebase/h_mt_mpt15-20");
  TH1F* h_ept3_zinv  = (TH1F*) f_zinv ->Get("srsebase/h_mt_mpt15-20");
  TH1F* h_ept3_qcd   = (TH1F*) f_qcd  ->Get("srsebase/h_mt_mpt15-20");
  TH1F* h_ept3_sig   = (TH1F*) f_sig  ->Get("srsebase/h_mt_mpt15-20");

  TH1F* h_sebg1 = new TH1F("sepeBG_1","Background with ept5-10 ", 200, 0, 500);
  TH1F* h_sebg2 = new TH1F("sepeBG_2","Background with ept10-15", 200, 0, 500);
  TH1F* h_sebg3 = new TH1F("sepeBG_3","Background with ept15-20", 200, 0, 500);

  h_sebg1->Add(h_ept1_ttbar);
  h_sebg1->Add(h_ept1_wjets);
  h_sebg1->Add(h_ept1_zinv );
  h_sebg1->Add(h_ept1_qcd  );

  h_sebg2->Add(h_ept2_ttbar);
  h_sebg2->Add(h_ept2_wjets);
  h_sebg2->Add(h_ept2_zinv );
  // h_sebg2->Add(h_ept2_qcd  );

  h_sebg3->Add(h_ept3_ttbar);
  h_sebg3->Add(h_ept3_wjets);
  h_sebg3->Add(h_ept3_zinv );
  h_sebg3->Add(h_ept3_qcd  );


  cout << "\\begin{table}[H]" << endl
       << "\\footnotesize" << endl
       << "\\centering" << endl
       << "\\begin{tabular}{r|c|c|c||c|c|c}" << endl
       << "\\hline" << endl << "\\hline" << endl
       << "Sample & \\multicolumn{3}{|c||}{srsmbase} & \\multicolumn{3}{|c}{srsebase} \\\\" << endl
       << " & $5 < p_{\\mathrm{T}}^\\mu < 10$ & $10 < p_{\\mathrm{T}}^\\mu < 15$ & $15 < p_{\\mathrm{T}}^\\mu < 20$ & "
       << "$5 < p_{\\mathrm{T}}^e < 10$ & $10 < p_{\\mathrm{T}}^e < 15$ & $15 < p_{\\mathrm{T}}^e < 20$ \\\\" << endl
       << "\\hline" << endl << "\\hline" << endl
       << "ttbar & " << h_mupt1_ttbar->Integral() << " & " << h_mupt2_ttbar->Integral() << " & " << h_mupt3_ttbar->Integral()
       << " & " << h_ept1_ttbar->Integral() << " & " << h_ept2_ttbar->Integral() << " & " << h_ept3_ttbar->Integral() << " \\\\" << endl
       << "wjets & " << h_mupt1_wjets->Integral() << " & " << h_mupt2_wjets->Integral() << " & " << h_mupt3_wjets->Integral()
       << " & " << h_ept1_wjets->Integral() << " & " << h_ept2_wjets->Integral() << " & " << h_ept3_wjets->Integral() << " \\\\" << endl
       << "zinv & " << h_mupt1_zinv->Integral() << " & " << h_mupt2_zinv->Integral() << " & " << h_mupt3_zinv->Integral()
       << " & " << h_ept1_zinv->Integral() << " & " << h_ept2_zinv->Integral() << " & " << h_ept3_zinv->Integral() << " \\\\" << endl
       // << "qcd & " << h_mupt1_qcd->Integral() << " & " << h_mupt2_qcd->Integral() << " & " << h_mupt3_qcd->Integral()
       // << " & " << h_ept1_qcd->Integral() << " & " << h_ept2_qcd->Integral() << " & " << h_ept3_qcd->Integral() << "\\\\" << endl
       << "\\hline" << endl
       << "Total SM & " << h_smbg1->Integral() << " & " << h_smbg2->Integral() << " & " << h_smbg3->Integral() << " & "
       << h_sebg1->Integral() << " & " << h_sebg2->Integral() << " & " << h_sebg3->Integral() << " \\\\" << endl
       << "\\hline" << endl
       << "1000 775, 750 & " <<  h_mupt1_sig->Integral() << " & " << h_mupt2_sig->Integral() << " & " << h_mupt3_sig->Integral()
       << " & " << h_ept1_sig->Integral() << " & " << h_ept2_sig->Integral() << " & " << h_ept3_sig->Integral() << " \\\\" << endl
       << "\\hline" << endl
       << "\\end{tabular}" << endl
       << "\\caption{Num. of yields in separate of lep pt}" << endl
       << "\\end{table}" << endl;

}
