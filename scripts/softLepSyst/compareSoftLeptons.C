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




//_______________________________________________________________________________
string getHTPlotLabelShort(TFile* f, std::string dir_str) {
  
  TString dir= TString(dir_str);
  
  TH1D* h_ht_LOW = (TH1D*) f->Get(dir+"/h_ht_LOW");
  TH1D* h_ht_HI = (TH1D*) f->Get(dir+"/h_ht_HI");
  int ht_LOW;
  int ht_HI;
  if(h_ht_LOW && h_ht_HI){
    ht_LOW = h_ht_LOW->GetBinContent(1);
    ht_HI = h_ht_HI->GetBinContent(1);
  }
  else return "Low H_{T}";
//  if (ht_LOW == 200) return "Low H_{T}";
//  if (ht_LOW == 300) return "Med H_{T}";
//  if (ht_LOW == 600) return "Hi H_{T}";
  
//  if(ht_HI != -1) return toString(ht_LOW) + " < H_{T} < " + toString(ht_HI) + " GeV";
//  else  return "H_{T} > " + toString(ht_LOW) + " GeV";
  return "H_{T}>" + toString(ht_LOW);
  
}

//_______________________________________________________________________________
string getMETPlotLabelShort(TFile* f, std::string dir_str) {
  
  TString dir= TString(dir_str);
  
  TH1D* h_ht_LOW = (TH1D*) f->Get(dir+"/h_met_LOW");
  TH1D* h_ht_HI = (TH1D*) f->Get(dir+"/h_met_HI");
  int ht_LOW;
  int ht_HI;
  if(h_ht_LOW && h_ht_HI){
    ht_LOW = h_ht_LOW->GetBinContent(1);
    ht_HI = h_ht_HI->GetBinContent(1);
  }
  else return "Low H_{T}";
  //  if (ht_LOW == 200) return "Low H_{T}";
  //  if (ht_LOW == 300) return "Med H_{T}";
  //  if (ht_LOW == 600) return "Hi H_{T}";
  
  //  if(ht_HI != -1) return toString(ht_LOW) + " < H_{T} < " + toString(ht_HI) + " GeV";
  //  else  return "H_{T} > " + toString(ht_LOW) + " GeV";
  return "MET>" + toString(ht_LOW);
  
}


//_______________________________________________________________________________
TCanvas* makePlot( const vector<TH1F*>& histos , const std::vector<TString>& names , const string& histname , const string& xtitle , const string& ytitle , bool logplot = true, bool doRatio = false ) {
  
  
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
      if( TString(names.at(i)).Contains("default")  ) {
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
  
  TLegend* leg = new TLegend(0.55,0.67,0.85,0.87);
  leg->SetFillColor(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.032);
//  if (doRatio) leg->SetTextSize(0.06);
  if (doRatio) leg->SetTextSize(0.03);
  
  TH1D* data_hist(0);
  string data_name;
  for( unsigned int i = 0 ; i < n ; ++i ) {
    if( !TString(names.at(i)).Contains("default")  ) continue;
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
  
  
  // to make legend and find max yvalue
  vector<TH1D*> sig_hists;
  vector<TString> sig_names;
  
  // signal hists - all samples must have "sig" in the name
  for( unsigned int i = 0 ; i < n ; ++i ){
    if( TString(names.at(i)).Contains("default") ) continue;
    TH1D* h = (TH1D*) histos.at(i)->Clone(histname.c_str());
    h->SetLineColor(2+i);
    h->SetLineWidth(2);
    //addOverflow(h); // Add Overflow
    sig_hists.push_back(h);
    sig_names.push_back(names.at(i));
  }
  
  float ymax = 0;
  // also check signals for max val
  for (unsigned int isig = 0; isig < sig_hists.size(); ++isig) {
    ymax = std::max(ymax,(float)sig_hists.at(isig)->GetMaximum());
  }
  if (data_hist) ymax = std::max(ymax,(float)data_hist->GetMaximum());
  
  if( logplot ) ymax*=50;
  else          ymax*=1.7;
  float ymin = 0.1;
  if( logplot ) ymin*=0.01;

  
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
  
  //t->Draw("hist same");
  
  // add signal hists
  for (unsigned int isig = 0; isig < sig_hists.size(); ++isig) {
    sig_hists.at(isig)->Draw("hist same");
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
    h_ratio->Divide(sig_hists.at(0));
    
    // draw axis only
    TH1F* h_axis_ratio = new TH1F(Form("%s_axes",h_ratio->GetName()),"",100,xmin,xmax);
    h_axis_ratio->GetYaxis()->SetTitleOffset(0.25);
    h_axis_ratio->GetYaxis()->SetTitleSize(0.18);
    h_axis_ratio->GetYaxis()->SetNdivisions(5);
    h_axis_ratio->GetYaxis()->SetLabelSize(0.15);
    h_axis_ratio->GetYaxis()->SetRangeUser(0.5,1.5);
//    h_axis_ratio->GetYaxis()->SetRangeUser(0.001,2.0);
    h_axis_ratio->GetYaxis()->SetTitle("Var/Central");
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
    
    for (unsigned int i=0; i<sig_hists.size(); ++i) {
      TH1D* h_ratio1 = (TH1D*) data_hist->Clone(Form("h_ratio_%s",sig_names.at(i).Data()));
      h_ratio1->Divide(sig_hists.at(i));
      h_ratio1->SetMarkerColor(3+i);
      h_ratio1->SetLineColor(3+i);
      h_ratio1->SetStats(0);
      h_ratio1->SetMarkerStyle(20);
      h_ratio1->SetMarkerSize(1.2);
      h_ratio1->Draw("hist same");
    }
    
    gPad->RedrawAxis();

    
    
  } // if (doRatio)
  
  gPad->Modified();
  gPad->Update();
  
    can->Print(Form("plots/%s.pdf",canvas_name.Data()));
    //can->Print(Form("plots/%s.eps",canvas_name.Data()));
  
  return can;
}



std::vector<TH1F*> makeTFHistos(std::vector<TFile*> filesSR, std::vector<TFile*> filesCR, std::vector<TString> regions, std::vector<TString> legendNames, std::vector<TString> histoNames, TString CRdir, bool integratedYield ) {
 
  
  std::vector<TH1F*> output;

  if (filesSR.size() != filesCR.size()) {
    cout<<"Files lists have different sizes: "<< filesSR.size() << " and "<< filesCR.size() <<endl;
    return output;
  }
 
  std::vector<std::string> labels;

  binMultiplier = 3;
  
  for(unsigned int i = 0; i < filesSR.size(); i++) {
    TH1F* h_sr_yields = new TH1F("h_sr_yields", "h_sr_yields", regions.size(), 0, regions.size());
    TH1F* h_sr_yields_binned = new TH1F("h_sr_yields_binned", "h_sr_yields_binned", regions.size()*binMultiplier, 0, regions.size()*binMultiplier);
    TH1F* h_sr_yields_binned_norm = new TH1F("h_sr_yields_binned_norm", "h_sr_yields_binned_norm", regions.size()*binMultiplier, 0, regions.size()*binMultiplier);
    TH1F* h_single_sr_yields = new TH1F("h_single_sr_yields", "h_single_sr_yields", regions.size(), 0, regions.size()); // needed for k_HT
    TH1F* h_cr_yields = new TH1F("h_cr_yields", "h_cr_yields", regions.size(), 0, regions.size());
    for(unsigned int j = 0; j < regions.size(); j++) {
      cout<<"looking at region "<<regions.at(j)<<endl;
      TString fullhistname = Form("%s/%s", regions.at(j).Data(), histoNames.at(i).Data());
      TString fullhistnameCR = fullhistname;
      fullhistnameCR.ReplaceAll("srLep", CRdir);
      fullhistnameCR.ReplaceAll("Dilepton", ""); // use the entire CR yield. TF is (dilepton events in SR) / (entire dilepton CR)
      fullhistnameCR.ReplaceAll("Onelep", ""); // use the entire CR yield. TF is (onelepton events in SR) / (entire onelepton CR)
      TH1F* hSR = (TH1F*) filesSR.at(i)->Get(fullhistname);
      TH1F* hSRsingle;
      if (hSR) hSRsingle = (TH1F*) filesSR.at(i)->Get(fullhistname)->Clone("single");
      TH1F* hCR = (TH1F*) filesCR.at(i)->Get(fullhistnameCR);
      
      if (CRdir == "cr2L") { // special treatment for 2L control regions: combine "L" and "M"
        // Add regions
        TString fullhistnameCRadded = fullhistnameCR;
        fullhistnameCRadded.ReplaceAll("cr2L", "");
        TString fullhistnameSRadded = fullhistname;
        fullhistnameSRadded.ReplaceAll("srLep", "");
        if (fullhistnameSRadded.Contains("L") || fullhistnameCRadded.Contains("L")) {
          fullhistnameCRadded.ReplaceAll("L", "M");
          fullhistnameCRadded = "cr2L" + fullhistnameCRadded;
          fullhistnameSRadded.ReplaceAll("L", "M");
          fullhistnameSRadded = "srLep" + fullhistnameSRadded;
          cout<<"Adding region "<<fullhistnameCRadded<<endl;
          TH1F* hSRadded = (TH1F*) filesSR.at(i)->Get(fullhistnameSRadded);
          if (hSRadded) hSR->Add(hSRadded);
          else cout<<"Could not find "<<fullhistnameSRadded<<endl;
          TH1F* hCRadded = (TH1F*) filesCR.at(i)->Get(fullhistnameCRadded);
          if (hCRadded) hCR->Add(hCRadded);
          else cout<<"Could not find "<<fullhistnameCRadded<<endl;

        }
        
        // Create the extra "k_HT" factor
        
        
      }
      
        
      double yield = 0.;
      double err = 0.;
      if (!hSR) yield = 0.0;
      else yield = hSR->IntegralAndError(0,-1,err);
      h_sr_yields->SetBinContent(j+1, yield);
      h_sr_yields->SetBinError(j+1, err);
      
      if (!hCR) { yield = 0.0; err = 0.0; }
      else yield = hCR->IntegralAndError(0,-1,err);
      h_cr_yields->SetBinContent(j+1, yield);
      h_cr_yields->SetBinError(j+1, err);
      
      if (!hSRsingle) { yield = 0.0; err = 0.0; }
      else yield = hSRsingle->IntegralAndError(0,-1,err);
      h_single_sr_yields->SetBinContent(j+1, yield);
      h_single_sr_yields->SetBinError(j+1, err);
      
      std::string label = getJetBJetPlotLabel(filesSR.at(0), regions.at(j).Data());
      std::string HTlabel = getMETPlotLabelShort(filesSR.at(0), regions.at(j).Data());
      if (regions.at(j).Contains("J") || regions.at(j).Contains("HT"))
        HTlabel = getHTPlotLabelShort(filesSR.at(0), regions.at(j).Data());
      std::string fullLabel = HTlabel + ", " + label;
      h_sr_yields->GetXaxis()->SetBinLabel(j+1, fullLabel.c_str());
      h_single_sr_yields->GetXaxis()->SetBinLabel(j+1, fullLabel.c_str());
      
    
      // fill binned histograms, useful for k_MT
      // make it normalized as well as not-normalized
      for(unsigned int k = 0; k < 3; k++) {
        if (hSR) {
          yield = hSR->IntegralAndError(0,-1,err);
          if (yield > 0) {
            h_sr_yields_binned_norm->SetBinContent(3*j+1+k, hSR->GetBinContent(k+1)/yield);
            h_sr_yields_binned_norm->SetBinError(3*j+1+k, hSR->GetBinError(k+1)/yield);
          }
          else h_sr_yields_binned_norm->SetBinContent(3*j+1+k, 0);
          h_sr_yields_binned->SetBinContent(3*j+1+k, hSR->GetBinContent(k+1));
          h_sr_yields_binned->SetBinError(3*j+1+k, hSR->GetBinError(k+1));
        }
        else {
          h_sr_yields_binned->SetBinContent( 3*j+1+k, 0.);
          h_sr_yields_binned_norm->SetBinContent( 3*j+1+k, 0.);
        }
      
        std::string label = getJetBJetPlotLabel(filesSR.at(0), regions.at(j).Data());
        //        std::string HTlabel = getHTPlotLabelShort(filesSR.at(0), regions.at(j).Data());
        std::string HTlabel = getMETPlotLabelShort(filesSR.at(0), regions.at(j).Data());
        if (regions.at(j).Contains("J") || regions.at(j).Contains("HT"))
          HTlabel = getHTPlotLabelShort(filesSR.at(0), regions.at(j).Data());
        std::string fullLabel = HTlabel + ", " + label + ", " + toString(k+1);
        h_sr_yields_binned->GetXaxis()->SetBinLabel(3*j+1+k, fullLabel.c_str());
        h_sr_yields_binned_norm->GetXaxis()->SetBinLabel(3*j+1+k, fullLabel.c_str());
      }
        
      
      delete hSR;
      delete hCR;
        
    }// end of loop over regions
    
    
    TH1F* h_R_sr_cr = (TH1F*) h_sr_yields->Clone("h_R_sr_cr");
    h_R_sr_cr->Divide(h_cr_yields);
    h_R_sr_cr->SetDirectory(0);
    h_R_sr_cr->SetName(legendNames.at(i));
    //h_R_sr_cr->Print("all");
    output.push_back(h_R_sr_cr);
    //h_R_sr_cr->Draw();
    if (CRdir == "cr2L") {
      TH1F* h_K_srsingle_sr = (TH1F*) h_single_sr_yields->Clone("h_K_srsingle_sr");
      h_K_srsingle_sr->Divide(h_sr_yields);
      h_K_srsingle_sr->SetDirectory(0);
      h_K_srsingle_sr->SetName(legendNames.at(i));
      h_K_srsingle_sr->Print("all");
      outputKht.push_back(h_K_srsingle_sr);
      h_K_srsingle_sr->Draw();
    }
    
    outputBinnedSR.push_back(h_sr_yields_binned);
    outputBinnedNormSR.push_back(h_sr_yields_binned_norm);
    
    
  }//end of loop over samples
  
  return output;
}
/*
std::vector<TH1F*> makeTFHistosMTbins(std::vector<TFile*> filesSR, std::vector<TFile*> filesCR, std::vector<TString> regions, std::vector<TString> legendNames, std::vector<TString> histoNames) {
  
  std::vector<TH1F*> output;
  
  if (filesSR.size() != filesCR.size()) {
    cout<<"Files lists have different sizes: "<< filesSR.size() << " and "<< filesCR.size() <<endl;
    return output;
  }
  
  std::vector<std::string> labels;
  
  
  for(unsigned int i = 0; i < filesSR.size(); i++) {
    TH1F* h_sr_yields = new TH1F("h_sr_yields", "h_sr_yields", regions.size()*3, 0, regions.size()*3);
    TH1F* h_cr_yields = new TH1F("h_cr_yields", "h_cr_yields", regions.size()*3, 0, regions.size()*3);
    for(unsigned int j = 0; j < regions.size(); j++) {
      cout<<"looking at region "<<regions.at(j)<<endl;
      TString fullhistname = Form("%s/h_mt2binsDilepton", regions.at(j).Data());
      TString fullhistnameCR = fullhistname;
      fullhistnameCR.ReplaceAll("srLep", "cr2L");
      fullhistnameCR.ReplaceAll("Dilepton", "");
      TH1F* hSR = (TH1F*) filesSR.at(i)->Get(fullhistname);
      TH1F* hCR = (TH1F*) filesCR.at(i)->Get(fullhistnameCR);
      //hSR->Print("all");
      //hCR->Print("all");
      for(unsigned int k = 0; k < 3; k++) {
        if (hSR) {
          h_sr_yields->SetBinContent(3*j+1+k, hSR->GetBinContent(k+1));
          h_sr_yields->SetBinError(3*j+1+k, hSR->GetBinError(k+1));
        }
        else h_sr_yields->SetBinContent( 3*j+1+k, 0.);
        if (hCR) {
          h_cr_yields->SetBinContent(3*j+1+k, hCR->GetBinContent(k+1));
          h_cr_yields->SetBinError(3*j+1+k, hCR->GetBinError(k+1));
        }
        else h_cr_yields->SetBinContent( 3*j+1+k, 0.);
        
        std::string label = getJetBJetPlotLabel(filesSR.at(0), regions.at(j).Data());
//        std::string HTlabel = getHTPlotLabelShort(filesSR.at(0), regions.at(j).Data());
        std::string HTlabel = getMETPlotLabelShort(filesSR.at(0), regions.at(j).Data());
        if (regions.at(j).Contains("J") || regions.at(j).Contains("HT"))
          HTlabel = getHTPlotLabelShort(filesSR.at(0), regions.at(j).Data());
        std::string fullLabel = HTlabel + ", " + label + ", " + toString(k+1);
        h_sr_yields->GetXaxis()->SetBinLabel(3*j+1+k, fullLabel.c_str());
        
      }
      delete hSR;
      delete hCR;
      
    }// end of loop over regions
    
    
    TH1F* h_R_sr_cr = (TH1F*) h_sr_yields->Clone("h_R_sr_cr");
    h_R_sr_cr->Divide(h_cr_yields);
    h_R_sr_cr->SetDirectory(0);
    h_R_sr_cr->SetName(legendNames.at(i));
    //h_R_sr_cr->Print("all");
    output.push_back(h_R_sr_cr);
    //h_R_sr_cr->Draw();
    
    
  }//end of loop over samples
  
  return output;
}
*/

void setRegions(std::vector<TString> &regions) {

  regions.push_back("srLep1L");
  regions.push_back("srLep2L");
  regions.push_back("srLep3L");
  regions.push_back("srLep4L");
  regions.push_back("srLep5L");
  regions.push_back("srLep6L");
  regions.push_back("srLep1M");
  regions.push_back("srLep2M");
  regions.push_back("srLep3M");
  regions.push_back("srLep4M");
  regions.push_back("srLep5M");
  regions.push_back("srLep6M");
  regions.push_back("srLepJ1L");
  regions.push_back("srLepJ1M");
  regions.push_back("srLepJ2L");
  regions.push_back("srLepB3");
  regions.push_back("srLepMET");
  regions.push_back("srLepHT");


}

void setRegionsDilep(std::vector<TString> &regions) {
  
  regions.push_back("srLep1L"); // +M
  regions.push_back("srLep2L"); // +M
  regions.push_back("srLep3L"); // +M
  regions.push_back("srLep4L"); // +M
  regions.push_back("srLep5L"); // +M
  regions.push_back("srLep6L"); // +M
  regions.push_back("srLepJ1L"); // +M
  regions.push_back("srLepJ2L"); // +M will not work
  regions.push_back("srLepB3");
  regions.push_back("srLepMET");
  regions.push_back("srLepHT");
  
}


void compareSoftLeptons(){
  
  cmsText = "CMS Simulation";
  cmsTextSize = 0.5;
  lumiTextSize = 0.4;
  writeExtraText = false;
  lumi_13TeV = "2.3 fb^{-1}";
  
  std::vector<TString> regions;
  std::vector<TString> regionsDilep;
  std::vector<TString> legendNames;
  std::vector<TString> histoNames;
  std::vector<TFile*> filesSR;
  std::vector<TFile*> filesCR;
  
  
  string input_dir = "/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/SR19Jan16/";
  string input_dir2 = "/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/CR19Jan16/";

  TFile* sr1 = new TFile(Form("%s/bkg.root",input_dir.c_str()));
  TFile* cr1 = new TFile(Form("%s/bkg.root",input_dir.c_str()));
  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("default"); histoNames.push_back("h_mtbinsDilepton");
  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("LepEff Up"); histoNames.push_back("h_mtbins_lepeff_UPDilepton");
  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("LepEff Dn"); histoNames.push_back("h_mtbins_lepeff_DNDilepton");
  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Tau1prong Up"); histoNames.push_back("h_mtbins_tau1p_UPDilepton");
  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Tau1prong Dn"); histoNames.push_back("h_mtbins_tau1p_DNDilepton");
  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Tau3prong Up"); histoNames.push_back("h_mtbins_tau3p_UPDilepton");
  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Tau3prong Dn"); histoNames.push_back("h_mtbins_tau3p_DNDilepton");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Btag HF Up"); histoNames.push_back("h_mtbins__btagsf_heavy_UPDilepton");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Btag HF Dn"); histoNames.push_back("h_mtbins__btagsf_heavy_DNDilepton");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Btag LF Up"); histoNames.push_back("h_mtbins__btagsf_light_UPDilepton");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Btag LF Dn"); histoNames.push_back("h_mtbins__btagsf_light_DNDilepton");
  
  setRegions(regions);
  setRegionsDilep(regionsDilep);

  // make histograms with few regions to get R and k_MET
  outputKht.clear(); outputBinnedSR.clear(); outputBinnedNormSR.clear();
  std::vector<TH1F*> histos       = makeTFHistos(filesSR, filesCR, regionsDilep, legendNames, histoNames, "cr2L", true);
  makePlot( histos , legendNames ,  "DilepTFs" ,  "" ,  "Transfer Factor" ,  false ,  true  );
  makePlot( outputKht , legendNames ,  "DilepKmet" ,  "" ,  "k_{MET}" ,  false ,  true  );

  // remake histograms with more regions to look at k_MT
  outputKht.clear(); outputBinnedSR.clear(); outputBinnedNormSR.clear();
  histos       = makeTFHistos(filesSR, filesCR, regions, legendNames, histoNames, "cr2L", true);
  makePlot( outputBinnedSR , legendNames ,  "DilepTFsMTbinYield" ,  "" ,  "Entries" ,  true ,  true  );
  makePlot( outputBinnedNormSR , legendNames ,  "DilepTFsMTbinNorm" ,  "" ,  "k_{MT}" ,  true ,  true  );

  
  
  TFile* cr2 = new TFile(Form("%s/bkg.root",input_dir2.c_str()));
  filesSR.clear();  filesCR.clear(); legendNames.clear(); histoNames.clear();
  filesSR.push_back(sr1);  filesCR.push_back(cr2); legendNames.push_back("default"); histoNames.push_back("h_mtbinsOnelep");
  filesSR.push_back(sr1);  filesCR.push_back(cr2); legendNames.push_back("LepEff Up"); histoNames.push_back("h_mtbins_lepeff_UPOnelep");
  filesSR.push_back(sr1);  filesCR.push_back(cr2); legendNames.push_back("LepEff Dn"); histoNames.push_back("h_mtbins_lepeff_DNOnelep");
//  filesSR.push_back(sr1);  filesCR.push_back(cr2); legendNames.push_back("Tau1prong Up"); histoNames.push_back("h_mtbins_tau1p_UPOnelep");
//  filesSR.push_back(sr1);  filesCR.push_back(cr2); legendNames.push_back("Tau1prong Dn"); histoNames.push_back("h_mtbins_tau1p_DNOnelep");
//  filesSR.push_back(sr1);  filesCR.push_back(cr2); legendNames.push_back("Tau3prong Up"); histoNames.push_back("h_mtbins_tau3p_UPOnelep");
//  filesSR.push_back(sr1);  filesCR.push_back(cr2); legendNames.push_back("Tau3prong Dn"); histoNames.push_back("h_mtbins_tau3p_DNOnelep");
  
  outputKht.clear(); outputBinnedSR.clear(); outputBinnedNormSR.clear();
  std::vector<TH1F*> histosCR1       = makeTFHistos(filesSR, filesCR, regions, legendNames, histoNames, "cr1L", true);
  makePlot( histosCR1 , legendNames ,  "OnelepTFs" ,  "" ,  "Transfer Factor" ,  false ,  true  );
  makePlot( outputBinnedSR , legendNames ,  "OnelepTFsMTbinYield" ,  "" ,  "Entries" ,  true ,  true  );
  makePlot( outputBinnedNormSR , legendNames ,  "OnelepTFsMTbinNorm" ,  "" ,  "k_{MT}" ,  true ,  true  );
  
  
  
  
  return;
}

