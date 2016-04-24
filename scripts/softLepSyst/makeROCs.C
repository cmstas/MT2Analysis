#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

#include "TROOT.h"
#include "TLatex.h"
#include "TString.h"
#include "TH2.h"
#include "THStack.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TKey.h"

#include "../plotUtilities.C"
#include "../../MT2CORE/Plotting/PlotUtilities.cc"
#include "../CMS_lumi.C"

using namespace std;

// -- for CMS_lumi label

const int iPeriod = 4; // 13 tev

// iPos drives the position of the CMS logo in the plot
// iPos=11 : top-left, left-aligned
// iPos=33 : top-right, right-aligned
// iPos=22 : center, centered
// mode generally :
//   iPos = 10*(alignement 1/2/3) + position (1/2/3 = left/center/right)
const int iPos = 3;
bool found_data = false;

std::vector<TH1F*> outputKht;
std::vector<TH1F*> outputBinnedSR;
std::vector<TH1F*> outputBinnedNormSR;
std::vector<TH1F*> outputHighMTSR;


//_______________________________________________________________________________
TCanvas* makePlot( const vector<TH1F*>& histos , const std::vector<TString>& names , const std::vector<TString>& purpose, const string& histname , const string& xtitle , const string& ytitle , bool logplot = true, bool doRatio = false ) {
  
  
  cout << "-- plotting histname: " << histname << endl;
  
  gStyle->SetOptStat("");
  gStyle->SetCanvasColor(0);
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetFrameBorderMode(0);
  
  // if ratio was requested, check if data is present.  If not, turn ratio off again
  const unsigned int n = histos.size();
  if (doRatio) {
    bool foundData = false;
    for( unsigned int i = 0 ; i < n ; ++i ) {
      //if( TString(names.at(i)).Contains("default")   || TString(names.at(i)).Contains("Data") || TString(names.at(i)).Contains("Truth")) {
      if ( TString(purpose.at(i)).Contains("black point") ) {
        foundData = true;
        break;
      }
    }
    if (!foundData) {
      cout << "ratio requested but no data hist found.  Not plotting ratio" << endl;
      doRatio = false;
    }
  } // if doRatio
  
  if (!doRatio) {
    //these 4 lines shift plot to make room for axis labels
    gStyle->SetPadTopMargin(0.08);
    gStyle->SetPadBottomMargin(0.12);
    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetPadRightMargin(0.05);
  }
  
  TString canvas_name = Form("c_%s",histname.c_str());
  if (logplot) canvas_name += "_log";
  TCanvas* can = new TCanvas(canvas_name,canvas_name, 1200, 600);
  can->cd();
  if (!doRatio) {
    if (logplot) can->SetLogy();
    gPad->SetRightMargin(0.05);
    gPad->Modified();
  }
  
  // splitting canvas for ratio plots
  TPad* fullpad(0);
  TPad* plotpad(0);
  
  if (doRatio) {
    // master pad
    fullpad = new TPad("fullpad","fullpad",0,0,1,1);
    fullpad->Draw();
    fullpad->cd();
    
    // main plot pad, for ratio on bottom
    plotpad = new TPad("plotpad","plotpad",0,0.2,1,0.99);
    plotpad->SetRightMargin(0.05);
//    plotpad->SetBottomMargin(0.12);
    plotpad->SetBottomMargin(0.26);
    plotpad->Draw();
    plotpad->cd();
    if( logplot ) plotpad->SetLogy();
  }
  
  TLegend* leg = new TLegend(0.55,0.75,0.85,0.87);
  leg->SetFillColor(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.032);
//  if (doRatio) leg->SetTextSize(0.06);
  if (doRatio) leg->SetTextSize(0.03);
  
  TH1D* data_hist(0);
  string data_name;
  for( unsigned int i = 0 ; i < n ; ++i ) {
    if ( !TString(purpose.at(i)).Contains("black point") ) continue;
    //if( !TString(names.at(i)).Contains("default")   && !TString(names.at(i)).Contains("Data")   && !TString(names.at(i)).Contains("Truth")) continue;
    data_hist = (TH1D*) histos.at(i)->Clone(names.at(i));
    data_name = names.at(i);
    data_hist->SetLineColor(kBlack);
    data_hist->SetMarkerColor(kBlack);
    data_hist->SetMarkerStyle(20);
    //addOverflow(data_hist); // Add Overflow
    
    // expect to only find 1 data hist
    break;
  }
  
  if (data_hist) leg->AddEntry(data_hist, data_name.c_str(),"pe1");
  
  THStack* t = new THStack(Form("stack_%s",histname.c_str()),Form("stack_%s",histname.c_str()));
  TH1D* h_bgtot = 0;

  // to make legend and find max yvalue
  vector<TH1D*> bg_hists;
  vector<string> bg_names;
  vector<TH1D*> sig_hists;
  vector<TString> sig_names;
  vector<TString> sig_purposes;

  // background hists
  for( unsigned int i = 0 ; i < n ; ++i ) {
    //if( !TString(names.at(i)).Contains("bkg")  ) continue;
    if ( !TString(purpose.at(i)).Contains("stack") ) continue;
    TH1D* h = (TH1D*) histos.at(i)->Clone(names.at(i));
    h->SetFillColor(getColor(names.at(i).Data()));
    h->SetLineColor(kBlack);
    //addOverflow(h); // Add Overflow
    if (!TString(purpose.at(i)).Contains("signal")) {
      if( h_bgtot==0 ) h_bgtot = (TH1D*) h->Clone("bgtot");
      else h_bgtot->Add(h);
    }
    bg_hists.push_back(h);
    bg_names.push_back(names.at(i).Data());
  }
  
  // loop backwards to add to legend
  for (int ibg = (int) bg_hists.size()-1; ibg >= 0; --ibg) {
    leg->AddEntry(bg_hists.at(ibg),getLegendName(bg_names.at(ibg)).c_str(),"f");
  }
  
  
  // signal hists - all samples must have "sig" in the name
  for( unsigned int i = 0 ; i < n ; ++i ){
    if ( !TString(purpose.at(i)).Contains("colored") ) continue;
    //if( TString(names.at(i)).Contains("default")  || TString(names.at(i)).Contains("Data") ||  TString(names.at(i)).Contains("bkg") || TString(names.at(i)).Contains("Truth")) continue;
    TH1D* h = (TH1D*) histos.at(i)->Clone(histname.c_str());
    h->SetLineColor(2+i);
    h->SetLineWidth(2);
    //addOverflow(h); // Add Overflow
    sig_hists.push_back(h);
    sig_names.push_back(names.at(i));
    sig_purposes.push_back(purpose.at(i));
  }
  
  // loop through backgrounds to add hists to stack
//  Double_t bg_integral_err = 0.;
//  float bg_integral = h_bgtot->IntegralAndError(0,h_bgtot->GetXaxis()->GetNbins(),bg_integral_err);
//  float data_integral = 1.;
//  float bg_sf = 1.;
//  float bg_sf_err = 0.;
//  if (data_hist) {
//    data_integral = data_hist->Integral(0,data_hist->GetXaxis()->GetNbins());
//    bg_sf = data_integral/bg_integral;
//    bg_sf_err = err_mult(data_integral,bg_integral,sqrt(data_integral),bg_integral_err,bg_sf);
//    std::cout << "Data/MC is: " << bg_sf << " +/- " << bg_sf_err << std::endl;
//  }
  for (unsigned int ibg = 0; ibg < bg_hists.size(); ++ibg) {
//    if (scaleBGtoData && data_hist) bg_hists.at(ibg)->Scale(bg_sf);
    t->Add(bg_hists.at(ibg));
  }
//  if (scaleBGtoData && data_hist) {
//    h_bgtot->Scale(bg_sf);
//    std::cout << "Scaled background by: " << bg_sf << " +/- " << bg_sf_err << std::endl;
//  }

  
  float ymax = 0;
  if(h_bgtot) ymax = h_bgtot->GetMaximum();
  // also check signals for max val
  for (unsigned int isig = 0; isig < sig_hists.size(); ++isig) {
    ymax = std::max(ymax,(float)sig_hists.at(isig)->GetMaximum());
  }
  if (data_hist) ymax = std::max(ymax,(float)data_hist->GetMaximum());
  
  if( logplot ) ymax*=50;
  else          ymax*=1.7;
  float ymin = 0.1;
  if( logplot ) ymin*=0.01;
  if ( logplot && TString(histname).Contains("TopVsW")) ymin=0.0001;
  if ( logplot && TString(histname).Contains("DataVs")) ymin=0.01;


  
  float xmin = data_hist->GetXaxis()->GetXmin();
  float xmax = data_hist->GetXaxis()->GetXmax();
  
  cout<<"xmin "<<xmin<<", xmax "<<xmax<<endl;
  
  TH2F* h_axes = new TH2F(Form("%s_axes",histname.c_str()),"",xmax,xmin,xmax,1000,ymin,ymax);
  for (int ibin=1; ibin <= h_axes->GetNbinsX(); ++ibin) {
    h_axes->GetXaxis()->SetBinLabel(ibin, data_hist->GetXaxis()->GetBinLabel(ibin));
  }
  h_axes->GetXaxis()->LabelsOption("v");
  h_axes->GetXaxis()->SetTitle(xtitle.c_str());
  h_axes->GetXaxis()->SetLabelSize(0.04);
  h_axes->GetXaxis()->SetTitleSize(0.05);
  h_axes->GetYaxis()->SetTitle(ytitle.c_str());
  h_axes->GetYaxis()->SetLabelSize(0.04);
  h_axes->GetYaxis()->SetTitleOffset(0.95);
  h_axes->GetYaxis()->SetTitleSize(0.05);
  h_axes->Draw();
  
  t->Draw("hist same");
  
  // add signal hists
  for (unsigned int isig = 0; isig < sig_hists.size(); ++isig) {
    if ( TString(sig_purposes.at(isig)).Contains("point") ) sig_hists.at(isig)->Draw("hist pe1 same");
    if ( TString(sig_purposes.at(isig)).Contains("line") ) sig_hists.at(isig)->Draw("hist same");
    TString legend_name = sig_names.at(isig);
    leg->AddEntry(sig_hists.at(isig),legend_name,"l");
  }
  
  if (data_hist) data_hist->Draw("pe1 x0 same");
  
  TLatex label;
  label.SetNDC();
  label.SetTextSize(0.032);
  float label_x_start = 0.2;
  float label_y_start = 0.82;
  float label_y_spacing = 0.04;
  if (doRatio) {
    label.SetTextSize(0.039);
    label_y_start = 0.84;
    label_y_spacing = 0.04;
  }
  
  //TString ht_label = getHTPlotLabel(histdir);
  //TString ht_label = getHTPlotLabel(samples.at(0), histdir);
  //TString region_label = getJetBJetPlotLabel(samples.at(0), histdir);
  //TString region_label_line2 = getMT2PlotLabel(samples.at(0), histdir);
  //label.DrawLatex(0.2,0.85,ht_label);
  
  
  //soft lepton regions
//  if (histdir.find("softl") != std::string::npos) {
//    if (histdir.find("base") != std::string::npos){
//      ht_label = "";
//      region_label = "";
//      region_label_line2 = "";
//      if (histdir.find("softlmu") != std::string::npos){
//        region_label = "Single Muon";
//      }
//      if (histdir.find("softlel") != std::string::npos){
//        region_label = "Single Electron";
//      }
//    }
//  }
//  if (histdir.find("doublel") != std::string::npos) {
//    if (histdir.find("base") != std::string::npos){
//      ht_label = "";
//      region_label = "";
//      region_label_line2 = "";
//    }
//  }
//  
//  if (ht_label.Length() > 0) label.DrawLatex(label_x_start,label_y_start,ht_label);
//  if (region_label.Length() > 0) label.DrawLatex(label_x_start,label_y_start - label_y_spacing,region_label);
//  if (region_label_line2.Length() > 0) label.DrawLatex(label_x_start,label_y_start - 2 * label_y_spacing,region_label_line2);
//  
  leg->Draw();
  h_axes->Draw("axissame");
  
  if (doRatio) {
    lumiTextSize     = 0.4;
    cmsTextSize      = 0.5;
    CMS_lumi( plotpad, iPeriod, iPos );
  }
  else {
    lumiTextSize     = 0.45;
    cmsTextSize      = 0.55;
    CMS_lumi( can, iPeriod, iPos );
  }
  
  // make ratio pad and plot
  if (doRatio) {
    // draw ratio pad
    fullpad->cd();
    TPad* ratiopad = new TPad("ratiopad","ratiopad",0.,0.,1,0.21);
    ratiopad->SetTopMargin(0.05);
    ratiopad->SetBottomMargin(0.1);
    ratiopad->SetRightMargin(0.05);
    ratiopad->SetGridy();  // doesn't actually appear for some reason..
    ratiopad->Draw();
    ratiopad->cd();
    
    TH1D* h_ratio = (TH1D*) data_hist->Clone(Form("ratio_%s",data_hist->GetName()));
    h_ratio->Sumw2();
    if (h_bgtot && h_bgtot->Integral()>0)    h_ratio->Divide(h_bgtot);
    else if (sig_hists.at(0)->Integral()>0)  h_ratio->Divide(sig_hists.at(0));
    else h_ratio->Divide(data_hist);
    
    // draw axis only
    TH1F* h_axis_ratio = new TH1F(Form("%s_axes",h_ratio->GetName()),"",100,xmin,xmax);
    h_axis_ratio->GetYaxis()->SetTitleOffset(0.25);
    h_axis_ratio->GetYaxis()->SetTitleSize(0.18);
    h_axis_ratio->GetYaxis()->SetNdivisions(5);
    h_axis_ratio->GetYaxis()->SetLabelSize(0.15);
    h_axis_ratio->GetYaxis()->SetRangeUser(0.5,1.5);
//    h_axis_ratio->GetYaxis()->SetRangeUser(0.001,2.0);
    h_axis_ratio->GetYaxis()->SetTitle("Var/Central");
    if (TString(histname).Contains("Closure") || TString(histname).Contains("cr2Lpreds") ) {
      h_axis_ratio->GetYaxis()->SetTitle("/ MC (Truth)");
      h_axis_ratio->GetYaxis()->SetRangeUser(0.1,9.99);
      ratiopad->SetLogy();
    }
    if (TString(histname).Contains("cr2LpredAlt")  ) {
      h_axis_ratio->GetYaxis()->SetTitle("Int/Bin");
      h_axis_ratio->GetYaxis()->SetRangeUser(0.1,1.99);
      //ratiopad->SetLogy();
    }
    if (TString(histname).Contains("DataVs")) h_axis_ratio->GetYaxis()->SetTitle("Data/Pred");
//    if (TString(histname).Contains("TopVsW")) {
//      h_axis_ratio->GetYaxis()->SetRangeUser(0.1,9.99);
//      ratiopad->SetLogy();
//    }
    h_axis_ratio->GetXaxis()->SetTickLength(0.07);
    h_axis_ratio->GetXaxis()->SetTitleSize(0.);
    h_axis_ratio->GetXaxis()->SetLabelSize(0.);
    h_axis_ratio->Draw("axis");
    
    // draw line at 1
    TLine* line1 = new TLine(xmin,1,xmax,1);
    line1->SetLineStyle(2);
    line1->Draw("same");
    
//    TGraphErrors* g_ratio = new TGraphErrors(h_ratio);
//    g_ratio->SetName(Form("%s_graph",h_ratio->GetName()));
//    for (int ibin=0; ibin < h_ratio->GetNbinsX(); ++ibin) {
//      //      g_ratio->SetPointError(ibin, h_ratio->GetBinWidth(ibin+1)/2., h_ratio->GetBinError(ibin+1));
//      g_ratio->SetPointError(ibin, 0., h_ratio->GetBinError(ibin+1));
//    }
//    g_ratio->SetLineColor(kBlack);
//    g_ratio->SetMarkerColor(kBlack);
//    g_ratio->SetMarkerStyle(20);
//    g_ratio->Draw("p same");
    
    if (TString(histname).Contains("Closure") || TString(histname).Contains("DataVs")) {
      h_ratio->Draw("E1,same");
    }
    else {
      for (unsigned int i=0; i<sig_hists.size(); ++i) {
        TH1D* h_ratio1 = (TH1D*) data_hist->Clone(Form("h_ratio_%s",sig_names.at(i).Data()));
        h_ratio1->Divide(sig_hists.at(i));
        h_ratio1->SetMarkerColor(3+i);
        h_ratio1->SetLineColor(3+i);
        h_ratio1->SetStats(0);
        h_ratio1->SetMarkerStyle(20);
        h_ratio1->SetMarkerSize(1.2);
        if ( TString(sig_purposes.at(i)).Contains("point") ) h_ratio1->Draw("E1,same");
        else h_ratio1->Draw("hist same");
      }
    }
    
    gPad->RedrawAxis();

    
    
  } // if (doRatio)
  
  gPad->Modified();
  gPad->Update();
  
    can->Print(Form("plots/%s.pdf",canvas_name.Data()));
    //can->Print(Form("plots/%s.eps",canvas_name.Data()));
  
  return can;
}




void makeEfficiency(){
  
  cmsText = "CMS Simulation";
  cmsTextSize = 0.5;
  lumiTextSize = 0.4;
  writeExtraText = false;
  lumi_13TeV = "2.3 fb^{-1}";

  
  TString dir = "/Users/giovannizevidellaporta/UCSD/MT2lepton/MarkBabies3Mar16/";
  
  TH1::SetDefaultSumw2();
  
  TChain* t_ttdl = new TChain("mt2");
  TChain* t_wjets = new TChain("mt2");
  
  t_ttdl->Add((dir+"ttdl.root").Data());
  t_wjets->Add((dir+"wjets_ht.root").Data());
  
  TFile* f_out = new TFile("Efficiencies.root","RECREATE");
  
  TH1D* h_DeltaR = new TH1D("h_DeltaR",";DR ",20,0,4);
  TH1D* h_DeltaRpass = (TH1D*) h_DeltaR->Clone("h_DeltaRpass");
  TH1D* h_DeltaReff = (TH1D*) h_DeltaR->Clone("h_DeltaReff");

  TH1D* h_LeppT = new TH1D("h_LeppT",";p_{T} ",15,5,20);
  TH1D* h_LeppTpass = (TH1D*) h_LeppT->Clone("h_LeppTpass");
  TH1D* h_LeppTeff = (TH1D*) h_LeppT->Clone("h_LeppTeff");

  TH1D* h_MupT = new TH1D("h_MupT",";p_{T} ",25,200,600);
  TH1D* h_MupTpass = (TH1D*) h_MupT->Clone("h_MupTpass");
  TH1D* h_MupTeff = (TH1D*) h_MupT->Clone("h_MupTeff");
  
  TCanvas* c = new TCanvas("c","c");
  c->SetGrid(0,1);
  c->cd();
  
  TString drawDeltaR = "sqrt((lep_eta[0]-lep_eta[1])*(lep_eta[0]-lep_eta[1]) + TVector2::Phi_mpi_pi(lep_phi[0]-lep_phi[1])*TVector2::Phi_mpi_pi(lep_phi[0]-lep_phi[1]))";
  TCut cr2l = "ngenLep==2 && met_pt > 200 && lep_pt[0]>25 && lep_pt[1]<20 && diffMetMht/met_pt<0.5 && deltaPhiMin>0.3 && ht > 200 && abs(lep_eta[0])<1.5 && abs(lep_eta[1])<1.5";
  TCut passIso = "lep_relIso03[0]<0.2 && lep_relIso03[1]<0.2";
  
  TCut base = "met_pt>200 && lep_pt[0]<20 && abs(lep_pdgId[0])==11 && abs(lep_eta[0])<1.479";
  TCut truth = base + "ngenLep==1 && genLep_pt>5 && genLep_pt<20";
  TCut ID = "lep_tightIdNoIso[0]>0";
  TCut isolated = "lep_miniRelIso[0]<0.1 && lep_relIso03[0]<0.2";
  TCut IP = "abs(lep_dxy)<0.02 && abs(lep_dz)<0.17";

  TCut genMu = "abs(genStat23_eta)<2.5 && abs(genStat23_pdgId)==13 && ht > 200 && met_pt < 60";
  TCut recoIdIsoIpMu = genMu + "lep_pt[0]>180 && lep_relIso03[0]<0.2 && abs(lep_dxy[0]<0.02) && abs(lep_dz[0]<0.02) ";

  t_ttdl->Draw(TString(drawDeltaR+">>h_DeltaR"), cr2l);
  t_ttdl->Draw(TString(drawDeltaR+">>h_DeltaRpass"), cr2l + passIso);

  t_wjets->Draw("lep_pt[0]>>h_LeppT", truth);
  t_wjets->Draw("lep_pt[0]>>h_LeppTpass", truth + ID + isolated);
  
  t_wjets->Draw("genStat23_pt>>h_MupT", genMu);
  t_wjets->Draw("genStat23_pt>>h_MupTpass", recoIdIsoIpMu);
  
  h_DeltaReff->Divide(h_DeltaRpass, h_DeltaR, 1, 1, "B");
  h_DeltaReff->GetYaxis()->SetRangeUser(0.5, 1);
  h_DeltaReff->GetYaxis()->SetTitle("Isolation Efficiency");
  h_DeltaReff->Draw();
  gStyle->SetOptStat("");
  c->Write();
  c->SaveAs("ROCs/EffVsDeltaR.pdf");
  
  h_LeppTeff->Divide(h_LeppTpass, h_LeppT, 1, 1, "B");
  h_LeppTeff->GetYaxis()->SetRangeUser(0, 1);
  h_LeppTeff->GetYaxis()->SetTitle("Iso + LooseID Efficiency");
  h_LeppTeff->Draw();
  gStyle->SetOptStat("");
  c->Write();
  c->SaveAs("ROCs/EffVsLeppT.pdf");
  
  h_MupTeff->Divide(h_MupTpass, h_MupT, 1, 1, "B");
  h_MupTeff->GetYaxis()->SetRangeUser(0, 1);
  h_MupTeff->GetYaxis()->SetTitle("Iso+IP Efficiency");
  h_MupTeff->Draw();
  gStyle->SetOptStat("");
  c->Write();
  c->SaveAs("ROCs/EffVsMupT.pdf");
  
  f_out->Write();
  f_out->Close();

  
  return;
}



void makeROCs(){
  
  makeEfficiency(); return;
  
  cmsText = "CMS Simulation";
  cmsTextSize = 0.5;
  lumiTextSize = 0.4;
  writeExtraText = false;
  lumi_13TeV = "2.3 fb^{-1}";
  
  
  TString dir = "/Users/giovannizevidellaporta/UCSD/MT2lepton/MarkBabies3Mar16/";
  
  TH1::SetDefaultSumw2();
  
  TChain* t_wjets = new TChain("mt2");
  TChain* t_zinv  = new TChain("mt2");
  
  t_wjets->Add((dir+"wjets_ht.root").Data());
  t_zinv->Add((dir+"zinv_ht.root").Data());
  
  TFile* f_out = new TFile("ROCs/RocCurves.root","RECREATE");
  
  TH1D* h_miniRelIsoTrue = new TH1D("h_miniRelIsoTrue",";miniRelIso ",50,0,0.5);
  TH1D* h_miniRelIsoFake = (TH1D*) h_miniRelIsoTrue->Clone("h_miniRelIsoFake");
  TH1D* h_relIsoTrue = new TH1D("h_relIsoTrue",";miniRelIso ",50,0,2);
  TH1D* h_relIsoFake = (TH1D*) h_relIsoTrue->Clone("h_relIsoFake");
  TH1D* h_absIsoTrue = new TH1D("h_absIsoTrue",";miniRelIso ",50,0,5);
  TH1D* h_absIsoFake = (TH1D*) h_absIsoTrue->Clone("h_absIsoFake");
  TH1D* h_mvaTrue = new TH1D("h_mvaTrue",";MVA ",50,-1,1);
  TH1D* h_mvaFake = (TH1D*) h_mvaTrue->Clone("h_mvaFake");
  TH1D* h_cutTrue = new TH1D("h_cutTrue",";ID ",4,0,4);
  TH1D* h_cutFake = (TH1D*) h_cutTrue->Clone("h_cutFake");
  
  
  
  
  TCanvas* c = new TCanvas("c","c");
  c->SetGrid(1,1);
  c->cd();
  
  TCut base = "met_pt>200 && lep_pt[0]<20 && abs(lep_pdgId[0])==11 && abs(lep_eta[0])<1.479";
  TCut truth = base + "ngenLep==1 && genLep_pt>5 && genLep_pt<20";
  TCut lessThan10 = "lep_pt[0]<10";
  TCut isolated = "lep_miniRelIso[0]<0.1 && lep_relIso03[0]<0.2";
  TCut IP = "abs(lep_dxy)<0.02 && abs(lep_dz)<0.17";
  //TCut muonbase = "lep_pt[0]>200 && abs(lep_pdgId[0])==13"
  
  t_wjets->Draw("lep_miniRelIso[0]>>h_miniRelIsoTrue", truth + lessThan10);
  t_zinv->Draw("lep_miniRelIso[0]>>h_miniRelIsoFake", base + lessThan10);
  t_wjets->Draw("lep_relIso03[0]>>h_relIsoTrue", truth + lessThan10);
  t_zinv->Draw("lep_relIso03[0]>>h_relIsoFake", base + lessThan10);
  t_wjets->Draw("lep_relIso03[0]*lep_pt[0]>>h_absIsoTrue", truth + lessThan10);
  t_zinv->Draw("lep_relIso03[0]*lep_pt[0]>>h_absIsoFake", base + lessThan10);
  t_wjets->Draw("lep_mva[0]>>h_mvaTrue", truth+isolated+IP + lessThan10);
  t_zinv->Draw("lep_mva[0]>>h_mvaFake", base+isolated+IP + lessThan10);
  t_wjets->Draw("lep_tightIdNoIso[0]>>h_cutTrue", truth+isolated+IP + lessThan10);
  t_zinv->Draw("lep_tightIdNoIso[0]>>h_cutFake", base+isolated+IP + lessThan10);
  
  //  //Make Cut istogram inclusive (Veto should also include events passing Loose, etc)
  //  h_cutTrue->SetBinContent(1, h_cutTrue->GetBinContent(1)+h_cutTrue->GetBinContent(2)+h_cutTrue->GetBinContent(3)+h_cutTrue->GetBinContent(4));
  //  h_cutTrue->SetBinContent(2, h_cutTrue->GetBinContent(2)+h_cutTrue->GetBinContent(3)+h_cutTrue->GetBinContent(4));
  //  h_cutTrue->SetBinContent(3, h_cutTrue->GetBinContent(3)+h_cutTrue->GetBinContent(4));
  //  h_cutFake->SetBinContent(1, h_cutFake->GetBinContent(1)+h_cutFake->GetBinContent(2)+h_cutFake->GetBinContent(3)+h_cutFake->GetBinContent(4));
  //  h_cutFake->SetBinContent(2, h_cutFake->GetBinContent(2)+h_cutFake->GetBinContent(3)+h_cutFake->GetBinContent(4));
  //  h_cutFake->SetBinContent(3, h_cutFake->GetBinContent(3)+h_cutFake->GetBinContent(4));
  
  f_out->Write();
  
  
  //TGraph GetROC(TFile *f, const char *hist1, const char *hist2, bool increasing)
  //TFile * f = new TFile(dir+"TestMiniIso.root");
  TGraph a1 = GetROC(f_out, "h_miniRelIsoTrue", "h_miniRelIsoFake", false);
  a1.SetNameTitle("miniRelIso","miniRelIso"); a1.Write(); a1.SetFillColor(kWhite);
  TGraph a2 = GetROC(f_out, "h_relIsoTrue", "h_relIsoFake", false);
  a2.SetNameTitle("relIso","relIso"); a2.Write(); a2.SetLineColor(kRed); a2.SetFillColor(kWhite);
  TGraph a3 = GetROC(f_out, "h_absIsoTrue", "h_absIsoFake", false);
  a3.SetNameTitle("absIso","absIso"); a3.Write(); a3.SetLineColor(kBlue); a3.SetFillColor(kWhite);
  TCanvas myCanvas("IsoCanvas"); myCanvas.SetGridx(); myCanvas.SetGridy();
  a1.GetYaxis()->SetTitle("Eff. Background");
  a1.GetXaxis()->SetTitle("Eff. Signal");
  a1.Draw();
  a2.Draw("same");
  a3.Draw("same");
  myCanvas.BuildLegend(0.15, 0.67, 0.5, 0.88, "");
  myCanvas.Write();
  myCanvas.SaveAs("ROCs/IsoRocs5to10.pdf");
  
  
  TGraph a4 = GetROC(f_out, "h_mvaTrue", "h_mvaFake", true);
  a4.SetNameTitle("MVA","MVA"); a4.Write(); a4.SetFillColor(kWhite);
  TGraph a5 = GetROC(f_out, "h_cutTrue", "h_cutFake", true);
  a5.SetNameTitle("Cut","Cut"); a5.Write(); a5.SetLineColor(kRed); a5.SetFillColor(kWhite);
  TCanvas myCanvas2("IDCanvas"); myCanvas2.SetGridx(); myCanvas2.SetGridy();
  a4.GetYaxis()->SetTitle("Eff. Background");
  a4.GetXaxis()->SetTitle("Eff. Signal");
  a4.Draw();
  a5.Draw("same");
  myCanvas2.BuildLegend(0.15, 0.67, 0.5, 0.88, "");
  myCanvas2.Write();
  myCanvas2.SaveAs("ROCs/IDRocs5to10.pdf");
  
  f_out->Write();
  f_out->Close();
  
  
  return;
}


