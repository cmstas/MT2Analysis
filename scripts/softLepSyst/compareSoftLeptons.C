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
std::vector<TH1F*> outputHighMTSR;


void setRegions(std::vector<TString> &regions) {
  
  regions.push_back("srLep1L");
  regions.push_back("srLep2L");
  regions.push_back("srLep3L");
  regions.push_back("srLep4L");
  regions.push_back("srLep5L");
  regions.push_back("srLep6L");
  regions.push_back("srLep7L");
  regions.push_back("srLep1M");
  regions.push_back("srLep2M");
  regions.push_back("srLep3M");
  regions.push_back("srLep4M");
  regions.push_back("srLep5M");
  regions.push_back("srLep6M");
  regions.push_back("srLep7M");
  regions.push_back("srLepJ1L");
  regions.push_back("srLepJ1M");
  regions.push_back("srLepJ2L");
  regions.push_back("srLepJ3L");
  regions.push_back("srLepB3");
  regions.push_back("srLepMET");
  regions.push_back("srLepHT");
  
  
}
std::string getFullLabel(TString region) {
  if (region == "srLep1L") return "MET>200, 2-3j, 0b";
  if (region == "srLep2L") return "MET>200, 2-3j, #geq 1b";
  if (region == "srLep3L") return "MET>200, 2-3j, #geq 1B";
  if (region == "srLep4L") return "MET>200, 4-5j, 0b";
  if (region == "srLep5L") return "MET>200, 4-5j, #geq 1b";
  if (region == "srLep6L") return "MET>200, 4-5j, #geq 1B";
  if (region == "srLep7L") return "MET>200, 6+j, 0-2B";
  if (region == "srLep1M") return "MET>300, 2-3j, 0b";
  if (region == "srLep2M") return "MET>300, 2-3j, #geq 1b";
  if (region == "srLep3M") return "MET>300, 2-3j, #geq 1B";
  if (region == "srLep4M") return "MET>300, 4-5j, 0b";
  if (region == "srLep5M") return "MET>300, 4-5j, #geq 1b";
  if (region == "srLep6M") return "MET>300, 4-5j, #geq 1B";
  if (region == "srLep7M") return "MET>300, 6+j, 0-2B";
  if (region == "srLepJ1L") return "MET>200, 1j, 0b";
  if (region == "srLepJ1M") return "MET>300, 1j, 0b";
  if (region == "srLepJ2L") return "MET>200, 1j, #geq 1b";
  if (region == "srLepJ3L") return "MET>200, 1j, #geq 1B";
  if (region == "srLepB3") return "MET>200, #geq 3B";
  if (region == "srLepMET") return "MET>500, #geq 2j, 0-2B";
  if (region == "srLepHT") return "H_{T}>1000, #geq 2j, 0-2B";
  else return "";
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
TCanvas* makePlot( const vector<TH1F*>& histos , const std::vector<TString>& names , const std::vector<TString>& purpose, const string& histname , const string& xtitle , const string& ytitle , bool logplot = true, bool doRatio = false , bool drawBand = false) {
  
  
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
  
  // drawband
  if ( drawBand ) {
    h_bgtot->SetMarkerSize(0);
    h_bgtot->SetFillColor (kGray+2);
    h_bgtot->SetFillStyle (3244);
    h_bgtot->Draw("E2,same");
  }
  
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
    TH1D* divideByThis;
    if (h_bgtot && h_bgtot->Integral()>0)    divideByThis = (TH1D*) h_bgtot->Clone("DivideByThis");
    else if (sig_hists.at(0)->Integral()>0)  divideByThis = (TH1D*) sig_hists.at(0)->Clone("DivideByThis");
    else divideByThis = (TH1D*) data_hist->Clone("DivideByThis");
    
    if (drawBand) { //remove uncertainties from denominator, since they will have to be drawn separately
      for (int ibin=0; ibin < divideByThis->GetNbinsX(); ++ibin) {
        divideByThis->SetBinError(ibin, 0.);
      }
    }
    h_ratio->Divide(divideByThis);
    
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

    if ( drawBand ) { // draw the background uncertainty: just divide the histogram by the no-error version of itself
      TH1D* h_bgtotFlat = (TH1D*) h_bgtot->Clone("bgtotFlat");
      h_bgtotFlat->Divide(divideByThis);
      h_bgtotFlat->SetMarkerSize(0);
      h_bgtotFlat->SetFillColor (kGray+2);
      h_bgtotFlat->SetFillStyle (3244);
      h_bgtotFlat->Draw("E2,same");
    }
    
  } // if (doRatio)
  
  gPad->Modified();
  gPad->Update();
  
    can->Print(Form("plots/%s.pdf",canvas_name.Data()));
    //can->Print(Form("plots/%s.eps",canvas_name.Data()));
  
  return can;
}



std::vector<TH1F*> makeTFHistos(std::vector<TFile*> filesSR, std::vector<TFile*> filesCR, std::vector<TString> regions, std::vector<TString> legendNames, std::vector<TString> histoNames, TString CRdir, bool integratedYield, int mtbin = -1 ) {
 
  
  std::vector<TH1F*> output;

  if (filesSR.size() != filesCR.size()) {
    cout<<"Files lists have different sizes: "<< filesSR.size() << " and "<< filesCR.size() <<endl;
    return output;
  }
 
  std::vector<std::string> labels;

  binMultiplier = 3;
  if (mtbin != -1) binMultiplier = 1;
  
  
  for(unsigned int i = 0; i < filesSR.size(); i++) {
    TH1F* h_sr_yields = new TH1F("h_sr_yields", "h_sr_yields", regions.size(), 0, regions.size());
    TH1F* h_sr_yields_binned = new TH1F("h_sr_yields_binned", "h_sr_yields_binned", regions.size()*binMultiplier, 0, regions.size()*binMultiplier);
    TH1F* h_sr_yields_binned_norm = new TH1F("h_sr_yields_binned_norm", "h_sr_yields_binned_norm", regions.size()*binMultiplier, 0, regions.size()*binMultiplier);
    TH1F* h_single_sr_yields = new TH1F("h_single_sr_yields", "h_single_sr_yields", regions.size(), 0, regions.size()); // needed for k_HT
    TH1F* h_cr_yields = new TH1F("h_cr_yields", "h_cr_yields", regions.size(), 0, regions.size());

    float renormalizeCRafterModifying = 0;

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
      if (legendNames.at(i).Contains("x2")) {
        TH1F* hCRrenormalize = (TH1F*) filesSR.at(i)->Get(fullhistnameCR);
        if (hCRrenormalize) renormalizeCRafterModifying += hCRrenormalize->Integral();
      }

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
          if (hSRadded && hSR) hSR->Add(hSRadded);
          else cout<<"Could not find "<<fullhistnameSRadded<<endl;
          TH1F* hCRadded = (TH1F*) filesCR.at(i)->Get(fullhistnameCRadded);
          if (hCRadded && hCR) hCR->Add(hCRadded);
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
      
//      std::string label = getJetBJetPlotLabel(filesSR.at(0), regions.at(j).Data());
//      std::string HTlabel = getMETPlotLabelShort(filesSR.at(0), regions.at(j).Data());
//      if (regions.at(j).Contains("J") || regions.at(j).Contains("HT"))
//        HTlabel = getHTPlotLabelShort(filesSR.at(0), regions.at(j).Data());
//      std::string fullLabel = HTlabel + ", " + label;
//      h_sr_yields->GetXaxis()->SetBinLabel(j+1, fullLabel.c_str());
//      h_single_sr_yields->GetXaxis()->SetBinLabel(j+1, fullLabel.c_str());
      
      std::string fullLabel = getFullLabel(regions.at(j));
      h_sr_yields->GetXaxis()->SetBinLabel(j+1, fullLabel.c_str());
      h_single_sr_yields->GetXaxis()->SetBinLabel(j+1, fullLabel.c_str());
      
    
      // fill binned histograms, useful for k_MT
      // make it normalized as well as not-normalized
      for(unsigned int k = 0; k < 3; k++) {
        int outputbin = 3*j+1+k;
        if (mtbin != -1) {
          if (mtbin != k) continue;
          if (mtbin == k) outputbin = j+1; // use yield from kth bin, but don't increment binning in output
        }
        if (hSR) {
          yield = hSR->IntegralAndError(0,-1,err);
          if (yield > 0) {
            h_sr_yields_binned_norm->SetBinContent(outputbin, hSR->GetBinContent(k+1)/yield);
            h_sr_yields_binned_norm->SetBinError(outputbin, hSR->GetBinError(k+1)/yield);
          }
          else h_sr_yields_binned_norm->SetBinContent(outputbin, 0);
          h_sr_yields_binned->SetBinContent(outputbin, hSR->GetBinContent(k+1));
          h_sr_yields_binned->SetBinError(outputbin, hSR->GetBinError(k+1));
        }
        else {
          h_sr_yields_binned->SetBinContent( outputbin, 0.);
          h_sr_yields_binned_norm->SetBinContent( outputbin, 0.);
        }
        
        std::string fullLabel = getFullLabel(regions.at(j))+ ", " + toString(k+1);;
        h_sr_yields_binned->GetXaxis()->SetBinLabel(outputbin, fullLabel.c_str());
        h_sr_yields_binned_norm->GetXaxis()->SetBinLabel(outputbin, fullLabel.c_str());
      }
        
      
      delete hSR;
      delete hCR;
        
    }// end of loop over regions
    
    TH1F* h_R_sr_cr = (TH1F*) h_sr_yields->Clone("h_R_sr_cr");
    h_R_sr_cr->Print("all");
    h_cr_yields->Print("all");
    if (renormalizeCRafterModifying != 0) {
      cout<<"Renormalizing CR by a factor of "<< renormalizeCRafterModifying / h_cr_yields->Integral() << endl;
      h_cr_yields->Scale(renormalizeCRafterModifying / h_cr_yields->Integral());
    }
    h_cr_yields->Print("all");
    h_R_sr_cr->Divide(h_cr_yields);
    h_R_sr_cr->Print("all");
    h_R_sr_cr->SetDirectory(0);
    h_R_sr_cr->SetName(legendNames.at(i));
    h_R_sr_cr->Print("all");
    output.push_back(h_R_sr_cr);
    h_R_sr_cr->Draw();
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

std::vector<TH1F*> makeYieldsHistos(std::vector<TFile*> filesSR, std::vector<TString> regions, std::vector<TString> legendNames, std::vector<TString> histoNames, bool integratedYield ) {
  
  
  std::vector<TH1F*> output;
  
  std::vector<std::string> labels;
  
  int binMultiplier = 3;
  
  for(unsigned int i = 0; i < filesSR.size(); i++) {
    TH1F* h_sr_yields = new TH1F("h_sr_yields", "h_sr_yields", regions.size(), 0, regions.size());
    TH1F* h_sr_yields_binned = new TH1F("h_sr_yields_binned", "h_sr_yields_binned", regions.size()*binMultiplier, 0, regions.size()*binMultiplier);
    TH1F* h_sr_yields_binned_norm = new TH1F("h_sr_yields_binned_norm", "h_sr_yields_binned_norm", regions.size()*binMultiplier, 0, regions.size()*binMultiplier);
    TH1F* h_highmt_sr_yields = new TH1F("h_highmt_sr_yields", "h_highmt_sr_yields", regions.size(), 0, regions.size()); // fill with last MT bin only
    for(unsigned int j = 0; j < regions.size(); j++) {
      cout<<"looking at region "<<regions.at(j)<<endl;
      TString fullhistname = Form("%s/%s", regions.at(j).Data(), histoNames.at(i).Data());
      TString fullhistnameCR = fullhistname;
      TH1F* hSR = (TH1F*) filesSR.at(i)->Get(fullhistname);
      TH1F* hSRhighmt;
      if (hSR) hSRhighmt = (TH1F*) filesSR.at(i)->Get(fullhistname)->Clone("highmt");
      
      double yield = 0.;
      double err = 0.;
      if (!hSR) yield = 0.0;
      else yield = hSR->IntegralAndError(0,-1,err);
      h_sr_yields->SetBinContent(j+1, yield);
      h_sr_yields->SetBinError(j+1, err);
      
      if (!hSRhighmt) { yield = 0.0; err = 0.0; }
      else yield = hSRhighmt->IntegralAndError(3,-1,err);
      h_highmt_sr_yields->SetBinContent(j+1, yield);
      h_highmt_sr_yields->SetBinError(j+1, err);
      
      std::string fullLabel = getFullLabel(regions.at(j));
      h_sr_yields->GetXaxis()->SetBinLabel(j+1, fullLabel.c_str());
      std::string fullLabelHighMT = fullLabel + ", 3";
      h_highmt_sr_yields->GetXaxis()->SetBinLabel(j+1, fullLabelHighMT.c_str());
      
      
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
        
        std::string fullLabel = getFullLabel(regions.at(j))+ ", " + toString(k+1);;
        h_sr_yields_binned->GetXaxis()->SetBinLabel(3*j+1+k, fullLabel.c_str());
        h_sr_yields_binned_norm->GetXaxis()->SetBinLabel(3*j+1+k, fullLabel.c_str());
      }
      
      
      delete hSR;
      
    }// end of loop over regions
    
    
    h_sr_yields->SetDirectory(0);
    h_sr_yields->SetName(legendNames.at(i));
    //h_R_sr_cr->Print("all");
    output.push_back(h_sr_yields);
    //h_R_sr_cr->Draw();
    outputBinnedSR.push_back(h_sr_yields_binned);
    outputBinnedNormSR.push_back(h_sr_yields_binned_norm);
    outputHighMTSR.push_back(h_highmt_sr_yields);
    
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



void compareSoftLeptons1L2L(){
  
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
  std::vector<TString> purpose;

  
  
  string input_dir = "/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/softLep25Feb16/";
  string input_dir2 = "/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/softLep12Apr16/";

  TFile* sr1 = new TFile(Form("%s/allBkg.root",input_dir.c_str()));
  TFile* cr1 = new TFile(Form("%s/allBkg.root",input_dir.c_str()));
  TFile* wfile = new TFile(Form("%s/wjets_ht.root",input_dir.c_str()));
  TFile* wPolfile = new TFile(Form("%s/WTopSL.root",input_dir2.c_str()));
  TFile* dibfile = new TFile(Form("%s/diboson.root",input_dir.c_str()));
  TFile* tfile = new TFile(Form("%s/top.root",input_dir.c_str()));
  TFile* twfile = new TFile(Form("%s/tslWjets.root",input_dir.c_str()));
  TFile* ttwfile = new TFile(Form("%s/topTopWjets.root",input_dir.c_str()));
  TFile* twwfile = new TFile(Form("%s/topWjetsWjets.root",input_dir.c_str()));
  TFile* ttwwfile = new TFile(Form("%s/topTopWjetsWjets.root",input_dir.c_str()));
  TFile* ttwwwfile = new TFile(Form("%s/tslTslWjetsWjetsWjets.root",input_dir.c_str()));
  TFile* tttwwfile = new TFile(Form("%s/tslTslTslWjetsWjets.root",input_dir.c_str()));
  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("default"); histoNames.push_back("h_mtbins_tau3p_DNDilepton"); purpose.push_back("black point"); //h_mtbinsDilepton"); // something is broken here
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("LepEff Up"); histoNames.push_back("h_mtbins_lepeff_UPDilepton"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("LepEff Dn"); histoNames.push_back("h_mtbins_lepeff_DNDilepton"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Tau1prong Up"); histoNames.push_back("h_mtbins_tau1p_UPDilepton"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Tau1prong Dn"); histoNames.push_back("h_mtbins_tau1p_DNDilepton"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Tau3prong Up"); histoNames.push_back("h_mtbins_tau3p_UPDilepton"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Tau3prong Dn"); histoNames.push_back("h_mtbins_tau3p_DNDilepton"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Btag HF Up"); histoNames.push_back("h_mtbins__btagsf_heavy_UPDilepton"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Btag HF Dn"); histoNames.push_back("h_mtbins__btagsf_heavy_DNDilepton"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Btag LF Up"); histoNames.push_back("h_mtbins__btagsf_light_UPDilepton"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Btag LF Dn"); histoNames.push_back("h_mtbins__btagsf_light_DNDilepton"); purpose.push_back("colored point");
  filesSR.push_back(tfile);  filesCR.push_back(cr1); legendNames.push_back("100% Top"); histoNames.push_back("h_mtbins_tau3p_DNDilepton"); purpose.push_back("colored point");
//  filesSR.push_back(dibfile);  filesCR.push_back(cr1); legendNames.push_back("100% Diboson"); histoNames.push_back("h_mtbins_tau3p_DNDilepton"); purpose.push_back("colored point");
  
  setRegions(regions);
  setRegionsDilep(regionsDilep);
  // Possibilities: LepEff, TauEff, TopVsW, Wpol
  std::string syst = "TopVsW";
  // make histograms with few regions to get R and k_MET
  outputKht.clear(); outputBinnedSR.clear(); outputBinnedNormSR.clear();
  std::vector<TH1F*> histos       = makeTFHistos(filesSR, filesCR, regionsDilep, legendNames, histoNames, "cr2L", true);
  makePlot( histos , legendNames , purpose,   "DilepTFs_"+syst ,  "" ,  "Transfer Factor" ,  false ,  true  );
  makePlot( outputKht , legendNames , purpose,   "DilepKmet_"+syst ,  "" ,  "k_{MET}" ,  false ,  true  );

  // remake histograms with more regions to look at k_MT
  outputKht.clear(); outputBinnedSR.clear(); outputBinnedNormSR.clear();
  histos       = makeTFHistos(filesSR, filesCR, regions, legendNames, histoNames, "cr2L", true);
  makePlot( outputBinnedSR , legendNames , purpose,   "DilepTFsMTbinYield_"+syst ,  "" ,  "Entries" ,  true ,  true  );
  makePlot( outputBinnedNormSR , legendNames , purpose,   "DilepTFsMTbinNorm_"+syst ,  "" ,  "k_{MT}" ,  true ,  true  );

  
  
  filesSR.clear();  filesCR.clear(); legendNames.clear(); histoNames.clear(); purpose.clear();
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("default"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("black point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("LepEff Up"); histoNames.push_back("h_mtbins_lepeff_UPOnelep"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("LepEff Dn"); histoNames.push_back("h_mtbins_lepeff_DNOnelep"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Tau1prong Up"); histoNames.push_back("h_mtbins_tau1p_UPOnelep"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Tau1prong Dn"); histoNames.push_back("h_mtbins_tau1p_DNOnelep"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Tau3prong Up"); histoNames.push_back("h_mtbins_tau3p_UPOnelep"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Tau3prong Dn"); histoNames.push_back("h_mtbins_tau3p_DNOnelep"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Btag HF Up"); histoNames.push_back("h_mtbins__btagsf_heavy_UPOnelep"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Btag HF Dn"); histoNames.push_back("h_mtbins__btagsf_heavy_DNOnelep"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Btag LF Up"); histoNames.push_back("h_mtbins__btagsf_light_UPOnelep"); purpose.push_back("colored point");
//  filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("Btag LF Dn"); histoNames.push_back("h_mtbins__btagsf_light_DNOnelep"); purpose.push_back("colored point");

  filesSR.push_back(twfile);  filesCR.push_back(twfile); legendNames.push_back("default"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("black point");
  //filesSR.push_back(sr1);  filesCR.push_back(cr1); legendNames.push_back("really default"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("colored point");

  //filesSR.push_back(twfile);  filesCR.push_back(tfile); legendNames.push_back("100% Top x2"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("colored point");
  //filesSR.push_back(twfile);  filesCR.push_back(wfile); legendNames.push_back("100% W+jets x2"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("colored point");
  //filesSR.push_back(ttwfile);  filesCR.push_back(twfile); legendNames.push_back("Top x2"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("colored point");
  //filesSR.push_back(ttwfile);  filesCR.push_back(twfile); legendNames.push_back("Top x2"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("colored point");
  ///filesSR.push_back(twfile);  filesCR.push_back(ttwfile); legendNames.push_back("Top x3"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("colored point");
  //filesSR.push_back(twwfile);  filesCR.push_back(twfile); legendNames.push_back("W+jets x2"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("colored point");
  filesSR.push_back(ttwwwfile);  filesCR.push_back(ttwwwfile); legendNames.push_back("W+jets +50%"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("colored point");
  filesSR.push_back(tttwwfile);  filesCR.push_back(tttwwfile); legendNames.push_back("Top +50%"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("colored point");
  //filesSR.push_back(twfile);  filesCR.push_back(twwfile); legendNames.push_back("W+jets x3"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("colored point");
  //filesSR.push_back(twfile);  filesCR.push_back(ttwwfile); legendNames.push_back("All x2"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("colored point");
//  filesSR.push_back(wPolfile);  filesCR.push_back(wPolfile); legendNames.push_back("default"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("black point");
//  filesSR.push_back(wPolfile);  filesCR.push_back(wPolfile); legendNames.push_back("PolW Up"); histoNames.push_back("h_mtbins_polW_UPOnelep"); purpose.push_back("colored point");
//  filesSR.push_back(wPolfile);  filesCR.push_back(wPolfile); legendNames.push_back("PolW Dn"); histoNames.push_back("h_mtbins_polW_DNOnelep"); purpose.push_back("colored point");
  //filesSR.push_back(wPolfile);  filesCR.push_back(wPolfile); legendNames.push_back("PolW+ Up"); histoNames.push_back("h_mtbins_polWp_UPOnelep"); purpose.push_back("colored point");
  //filesSR.push_back(wPolfile);  filesCR.push_back(wPolfile); legendNames.push_back("PolW+ Dn"); histoNames.push_back("h_mtbins_polWp_DNOnelep"); purpose.push_back("colored point");
  //filesSR.push_back(wPolfile);  filesCR.push_back(wPolfile); legendNames.push_back("PolW- Up"); histoNames.push_back("h_mtbins_polWm_UPOnelep"); purpose.push_back("colored point");
  //filesSR.push_back(wPolfile);  filesCR.push_back(wPolfile); legendNames.push_back("PolW- Dn"); histoNames.push_back("h_mtbins_polWm_DNOnelep"); purpose.push_back("colored point");
  outputKht.clear(); outputBinnedSR.clear(); outputBinnedNormSR.clear(); outputHighMTSR.clear();
  std::vector<TH1F*> histosCR1       = makeTFHistos(filesSR, filesCR, regions, legendNames, histoNames, "cr1L", true);
  makePlot( histosCR1 , legendNames , purpose,   "OnelepTFs_"+syst ,  "" ,  "Transfer Factor" ,  false ,  true  );
  makePlot( outputBinnedSR , legendNames , purpose,   "OnelepTFsMTbinYield_"+syst ,  "" ,  "Entries" ,  true ,  true  );
  makePlot( outputBinnedNormSR , legendNames , purpose,   "OnelepTFsMTbinNorm_"+syst ,  "" ,  "k_{MT}" ,  true ,  true  );
  
  outputKht.clear(); outputBinnedSR.clear(); outputBinnedNormSR.clear(); outputHighMTSR.clear();
  std::vector<TH1F*> histosCR1mtbin3       = makeTFHistos(filesSR, filesCR, regions, legendNames, histoNames, "cr1L", true, /*mtbin*/ 2);
  makePlot( outputBinnedNormSR , legendNames , purpose,   "OnelepTFsMTbinNormHighMT_"+syst ,  "" ,  "k_{MT}" ,  true ,  true  );
  
  outputKht.clear(); outputBinnedSR.clear(); outputBinnedNormSR.clear(); outputHighMTSR.clear();
  std::vector<TH1F*> histosCR1mtbin2       = makeTFHistos(filesSR, filesCR, regions, legendNames, histoNames, "cr1L", true, /*mtbin*/ 1);
  makePlot( outputBinnedNormSR , legendNames , purpose,   "OnelepTFsMTbinNormMedMT_"+syst ,  "" ,  "k_{MT}" ,  true ,  true  );
  
  
  
  
  return;
}

void compareSoftLeptonsClosure1L2L(){
  
  cmsText = "CMS Preliminary";
  cmsTextSize = 0.5;
  lumiTextSize = 0.4;
  writeExtraText = false;
  lumi_13TeV = "2.3 fb^{-1}";
  
  std::vector<TString> regions;
  std::vector<TString> regionsDilep;
  std::vector<TString> legendNames;
  std::vector<TString> histoNames;
  std::vector<TFile*> filesSR;
  std::vector<TString> purpose;

  
  //string input_dir = "/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/softLep25Feb16/";
  string input_dir = "/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/softLepMark15Apr16/";
  //string input_dir = "../../SoftLepLooper/output/softLep/";
  

  TFile* bkg = new TFile(Form("%s/allBkg.root",input_dir.c_str()));
  TFile* pred2L = new TFile(Form("%s/pred_CR2L.root",input_dir.c_str()));
  TFile* pred2LAlt = new TFile(Form("%s/pred_CR2LALT.root",input_dir.c_str()));
  TFile* pred1L = new TFile(Form("%s/pred_CR1L.root",input_dir.c_str()));

  setRegions(regions);

  filesSR.clear(); legendNames.clear(); histoNames.clear(); purpose.clear();
  filesSR.push_back(bkg);  legendNames.push_back("MC Truth"); histoNames.push_back("h_mtbinsDilepton"); purpose.push_back("black point");
  filesSR.push_back(pred2L);  legendNames.push_back("Dilepton Prediction"); histoNames.push_back("h_mtbins"); purpose.push_back("colored line");
  outputBinnedSR.clear(); outputBinnedNormSR.clear(); outputHighMTSR.clear();
  std::vector<TH1F*>  histos       = makeYieldsHistos(filesSR, regions, legendNames, histoNames, true);
  makePlot( outputBinnedSR , legendNames , purpose,   "cr2Lpred" ,  "" ,  "Entries" ,  true ,  true  );

  filesSR.clear(); legendNames.clear(); histoNames.clear(); purpose.clear();
//  filesSR.push_back(bkg);  legendNames.push_back("MC Truth"); histoNames.push_back("h_mtbinsDilepton"); purpose.push_back("black point");
//  filesSR.push_back(pred2L);  legendNames.push_back("Dilepton Prediction"); histoNames.push_back("h_mtbins");  purpose.push_back("black point");
  filesSR.push_back(pred2LAlt);  legendNames.push_back("Integrate over MT"); histoNames.push_back("h_mtbinsMCshape");  purpose.push_back("black point");
  filesSR.push_back(pred2LAlt);  legendNames.push_back("Bin in MT"); histoNames.push_back("h_mtbins");  purpose.push_back("colored point");
  outputBinnedSR.clear(); outputBinnedNormSR.clear(); outputHighMTSR.clear();
  histos       = makeYieldsHistos(filesSR, regions, legendNames, histoNames, true);
  makePlot( outputBinnedSR , legendNames , purpose,   "cr2LpredAlt" ,  "" ,  "Entries" ,  true ,  true  );

  filesSR.clear(); legendNames.clear(); histoNames.clear(); purpose.clear();
  filesSR.push_back(bkg);  legendNames.push_back("MC Truth"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("black point");
  filesSR.push_back(pred1L);  legendNames.push_back("Single Lepton Predicton"); histoNames.push_back("h_mtbins");  purpose.push_back("colored line");
  outputBinnedSR.clear(); outputBinnedNormSR.clear(); outputHighMTSR.clear();
  histos       = makeYieldsHistos(filesSR, regions, legendNames, histoNames, true);
  makePlot( outputBinnedSR , legendNames , purpose,   "cr1Lpred" ,  "" ,  "Entries" ,  true ,  true  );

  
  return;
}

void compareSoftLeptonsFake(){
  
  
  cmsText = "CMS Preliminary";
  cmsTextSize = 0.5;
  lumiTextSize = 0.4;
  writeExtraText = false;
  lumi_13TeV = "2.3 fb^{-1}";
  
  std::vector<TString> regions;
  std::vector<TString> regionsDilep;
  std::vector<TString> legendNames;
  std::vector<TString> histoNames;
  std::vector<TFile*> filesSR;
  std::vector<TString> purpose;
  
  string input_dir = "/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/softLep25Feb16/";
  //string input_dir = "../../SoftLepLooper/output/softLep/";
  
  setRegions(regions);


  TFile* sr1 = new TFile(Form("%s/pred_FakeRate.root",input_dir.c_str()));
//  filesSR.push_back(sr1);  legendNames.push_back("MC (FR method, treat as Data)"); histoNames.push_back("h_predMCClosure");
//  filesSR.push_back(sr1);  legendNames.push_back("MC (Truth)"); histoNames.push_back("h_predMC");
  filesSR.clear();  legendNames.clear(); histoNames.clear(); purpose.clear();
  filesSR.push_back(sr1);  legendNames.push_back("Data (FR method)"); histoNames.push_back("h_pred"); purpose.push_back("black point");
  filesSR.push_back(sr1);  legendNames.push_back("MC (Truth)"); histoNames.push_back("h_predMC"); purpose.push_back("colored point");
   outputBinnedSR.clear(); outputBinnedNormSR.clear(); outputHighMTSR.clear();
   histos2       = makeYieldsHistos(filesSR, regions, legendNames, histoNames, true);
   makePlot( outputBinnedSR , legendNames , purpose,  "FakeClosureData" ,  "" ,  "Entries" ,  true ,  true  );
   makePlot( outputHighMTSR , legendNames , purpose,  "FakeClosureDataHighMT" ,  "" ,  "Entries" ,  true ,  true  );

   return;
}


void compareSoftLeptonsEstimatesToData(){
  cmsText = "CMS Preliminary";
  cmsTextSize = 0.5;
  lumiTextSize = 0.4;
  writeExtraText = false;
  lumi_13TeV = "2.3 fb^{-1}";
  
  std::vector<TString> regions;
  std::vector<TString> regionsDilep;
  std::vector<TString> legendNames;
  std::vector<TString> histoNames;
  std::vector<TFile*> filesSR;
  std::vector<TString> purpose;

  
  string input_dir = "/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/softLep25Feb16/";
  string input_dir2 = "/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/softLep12Apr16/";
  
  TFile* fake = new TFile(Form("%s/pred_FakeRate.root",input_dir.c_str()));
  TFile* onelep = new TFile(Form("%s/pred_CR1L.root",input_dir.c_str()));
  TFile* dilep = new TFile(Form("%s/pred_CR2L.root",input_dir.c_str()));
  TFile* all = new TFile(Form("%s/allBkg.root",input_dir.c_str()));
//  TFile* T24bd = new TFile(Form("%s/T2-4bd_275_235.root",input_dir.c_str()));
//  TFile* T5qqqqWW = new TFile(Form("%s/T5qqqqWW_1025_775_custom.root",input_dir.c_str()));
  //  TFile* T5qqqqWW2 = new TFile(Form("%s/T5qqqqWW_1100_500_custom.root",input_dir.c_str()));
  TFile* TChiNeu = new TFile(Form("%s/TChiNeu_100_90.root",input_dir.c_str()));
  
  
  filesSR.push_back(all);  legendNames.push_back("MC (shown as Data)"); histoNames.push_back("h_mtbins"); purpose.push_back("black point");
  filesSR.push_back(all);  legendNames.push_back("fakeLep bkg"); histoNames.push_back("h_mtbinsFake"); purpose.push_back("stack");
  filesSR.push_back(all);  legendNames.push_back("diLep bkg"); histoNames.push_back("h_mtbinsDilepton"); purpose.push_back("stack");
  filesSR.push_back(all);  legendNames.push_back("oneLep bkg"); histoNames.push_back("h_mtbinsOnelep"); purpose.push_back("stack");
//  filesSR.push_back(T24bd);  legendNames.push_back("T2-4bd_275_235"); histoNames.push_back("h_mtbins"); purpose.push_back("stack signal");
  filesSR.push_back(TChiNeu);  legendNames.push_back("TChiNeu 100 90"); histoNames.push_back("h_mtbins"); purpose.push_back("colored point");
//  filesSR.push_back(T5qqqqWW);  legendNames.push_back("T5qqqqWW_1025_775"); histoNames.push_back("h_mtbins"); purpose.push_back("stack signal");
//  filesSR.push_back(T5qqqqWW2);  legendNames.push_back("T5qqqqWW_1100_500"); histoNames.push_back("h_mtbins"); purpose.push_back("stack signal");

  setRegions(regions);
  
  outputBinnedSR.clear(); outputBinnedNormSR.clear(); outputHighMTSR.clear();
  histos       = makeYieldsHistos(filesSR, regions, legendNames, histoNames, true);
  makePlot( outputBinnedSR , legendNames , purpose,   "DataVsMC_TChiNeu" ,  "" ,  "Entries" ,  true ,  true , true );
  
  filesSR.clear();  legendNames.clear(); histoNames.clear(); purpose.clear();
  filesSR.push_back(all);  legendNames.push_back("MC (shown as Data)"); histoNames.push_back("h_mtbins"); purpose.push_back("black point");
  filesSR.push_back(fake);  legendNames.push_back("fakeLep bkg"); histoNames.push_back("h_predMC12"); purpose.push_back("stack");
  filesSR.push_back(dilep);  legendNames.push_back("diLep bkg"); histoNames.push_back("h_mtbins"); purpose.push_back("stack");
  filesSR.push_back(onelep);  legendNames.push_back("oneLep bkg"); histoNames.push_back("h_mtbins"); purpose.push_back("stack");

  
  outputBinnedSR.clear(); outputBinnedNormSR.clear(); outputHighMTSR.clear();
  histos       = makeYieldsHistos(filesSR, regions, legendNames, histoNames, true);
  makePlot( outputBinnedSR , legendNames , purpose,   "DataVsPrediction" ,  "" ,  "Entries" ,  true ,  true , true );
  
  return;
}


void compareSoftLeptons(){
  //compareSoftLeptons1L2L();
  //compareSoftLeptonsClosure1L2L();
  //compareSoftLeptonsFake();
  compareSoftLeptonsEstimatesToData();
  return;
}

