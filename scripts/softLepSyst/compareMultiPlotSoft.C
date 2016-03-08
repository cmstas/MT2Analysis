#include <string>
#include <iostream>
#include <algorithm>

#include "TFile.h"
#include "TH1D.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TString.h"
#include "TLegend.h"

#include "CMS_lumi.C"
#include <vector>

using namespace std;

TCanvas *canv;
TString outname = "prova.root";

/////////////////////////

// USE: root -b -L compareOrig.C

/////////////////////////


void makeCMSPlot(  vector<TString> files,  vector<TString> labels, vector<TString> nplots , TString saveas, const string& xtitle , const string& ytitle , float xmin , float xmax , int rebin = 1 , bool logplot = false, float scalesig = -1., bool doRatio = false ) {
  
  cout << "-- plotting hist: " << nplots[0] << " etc "<<endl;
  
  cmsText = "CMS Preliminary";
  lumi_13TeV = "2.3 fb^{-1}";
  
  gStyle->SetOptStat("");
  gStyle->SetCanvasColor(0);
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetFrameBorderMode(0);
  
  // if ratio was requested, check if data is present.  If not, turn ratio off again
  //  const unsigned int n = samples.size();
  //  if (doRatio) {
  //    bool foundData = false;
  //    for( unsigned int i = 0 ; i < n ; ++i ) {
  //      if( TString(names.at(i)).Contains("data")  ) {
  //        foundData = true;
  //        break;
  //      }
  //    }
  //    if (!foundData) {
  //      cout << "ratio requested but no data hist found.  Not plotting ratio" << endl;
  //      doRatio = false;
  //    }
  //  } // if doRatio
  
  if (!doRatio) {
    //these 4 lines shift plot to make room for axis labels
    gStyle->SetPadTopMargin(0.08);
    gStyle->SetPadBottomMargin(0.12);
    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetPadRightMargin(0.05);
  }
  
  TString canvas_name = saveas;
  TCanvas* can = new TCanvas(canvas_name,canvas_name, 600, 600);
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
    plotpad->SetTopMargin(0.05);
    plotpad->SetRightMargin(0.05);
    plotpad->SetBottomMargin(0.05);
    plotpad->Draw();
    plotpad->cd();
    if( logplot ) plotpad->SetLogy();
  }
  
  //TLegend* leg = new TLegend(0.55,0.6,0.85,0.92);
  TLegend* leg = new TLegend(0.5,0.75,0.85,0.90);
  leg->SetFillColor(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.032);
  if (doRatio) leg->SetTextSize(0.05);
  
  TH1D* data_hist(0);
  //  string data_name;
  //  for( unsigned int i = 0 ; i < n ; ++i ) {
  //    if( !TString(names.at(i)).Contains("data")  ) continue;
  //    TString fullhistname = Form("%s/%s",histdir.c_str(),histname.c_str());
  //    if (histdir.size() == 0) fullhistname = TString(histname);
  //    TString newhistname = Form("%s_%s_%s",histname.c_str(),histdir.c_str(),names.at(i).c_str());
  //    TH1D* h_temp = (TH1D*) samples.at(i)->Get(fullhistname);
  //    if (h_temp == 0) continue;
  //    data_hist = (TH1D*) h_temp->Clone(newhistname);
  //    data_name = names.at(i);
  //    //    h->Sumw2();
  //    data_hist->SetLineColor(kBlack);
  //    data_hist->SetMarkerColor(kBlack);
  //    if (rebin > 1) data_hist->Rebin(rebin);
  //
  //    // fake data -> set error bars to correspond to data stats
  //    if (TString(data_name).Contains("fakedata")) {
  //      for (int ibin = 0; ibin <= data_hist->GetNbinsX(); ++ibin) {
  //        data_hist->SetBinError( ibin, sqrt(data_hist->GetBinContent(ibin)) );
  //      }
  //    } // if fakedata
  //
  //    // expect to only find 1 data hist
  //    break;
  //  }
  
  //  if (data_hist) leg->AddEntry(data_hist,getLegendName(data_name).c_str(),"pe");
  
  //  THStack* t = new THStack(Form("stack_%s_%s",histdir.c_str(),histname.c_str()),Form("stack_%s_%s",histdir.c_str(),histname.c_str()));
  TH1D* h_bgtot = 0;
  //
  //  // to make legend and find max yvalue
  //  vector<TH1D*> bg_hists;
  //  vector<string> bg_names;
  vector<TH1D*> sig_hists;
  vector<TString> sig_names;
  
  //  // background hists
  //  for( unsigned int i = 0 ; i < n ; ++i ) {
  //    if( TString(names.at(i)).Contains("data")  ) continue;
  //    if( TString(names.at(i)).Contains("sig")  ) continue;
  //    TString fullhistname = Form("%s/%s",histdir.c_str(),histname.c_str());
  //    if (histdir.size() == 0) fullhistname = TString(histname);
  //    TString newhistname = Form("%s_%s_%s",histname.c_str(),histdir.c_str(),names.at(i).c_str());
  //    TH1D* h_temp = (TH1D*) samples.at(i)->Get(fullhistname);
  //    if (h_temp == 0) continue;
  //    TH1D* h = (TH1D*) h_temp->Clone(newhistname);
  //    //    h->Sumw2();
  //    h->SetFillColor(getColor(names.at(i)));
  //    h->SetLineColor(getColor(names.at(i)));
  //    if (rebin > 1) h->Rebin(rebin);
  //    t->Add(h);
  //    if( h_bgtot==0 ) h_bgtot = (TH1D*) h->Clone(Form("%s_%s_bgtot",histname.c_str(),histdir.c_str()));
  //    else h_bgtot->Add(h);
  //
  //    bg_hists.push_back(h);
  //    bg_names.push_back(names.at(i));
  //  }
  
  //  // loop backwards to add to legend
  //  for (int ibg = (int) bg_hists.size()-1; ibg >= 0; --ibg) {
  //    leg->AddEntry(bg_hists.at(ibg),getLegendName(bg_names.at(ibg)).c_str(),"f");
  //  }
  //
  // signal hists - all samples must have "sig" in the name
  for( unsigned int i = 0 ; i < files.size() ; ++i ){
    
    TFile f1( files[i], "READ");
    if(!f1.IsOpen()){
      std::cout<<"File "<<files[i]<<" can't be found, will not be used for histogram "<<nplots[i]<<std::endl;
      continue;
    }
    if(f1.Get(nplots[i].Data())==0){
      std::cout<<"Histogram "<<nplots[i]<<" not found in file "<<files[i]<<std::endl;
      continue;
    }
    
    TH1D* h_temp = (TH1D*) f1.Get(nplots[i]);
    if (h_temp == 0) continue;
    // don't draw signal if the total yield in the plot is < 0.1 events
    //if (h_temp->Integral(0,-1) < 0.1) continue;
    TH1D* h = (TH1D*) h_temp->Clone("h1");
    h->SetDirectory(0);
    //    h->Sumw2();
    h->SetLineColor(1+i);
    h->SetMarkerColor(1+i);
    if (1+i==3) {
      h->SetLineColor(kGreen+1);
      h->SetMarkerColor(kGreen+1);
    }
    h->SetMarkerStyle(21+i);
    h->SetMarkerSize(1);
    h->SetLineWidth(2);
    if (rebin > 1) h->Rebin(rebin);
    if (scalesig > 0.) h->Scale(scalesig);
    sig_hists.push_back(h);
    sig_names.push_back(labels[i]);
  }
  
  float ymax = 0;
  //if(h_bgtot) ymax = h_bgtot->GetMaximum();
  // also check signals for max val
  for (unsigned int isig = 0; isig < sig_hists.size(); ++isig) {
    if (sig_hists.at(isig)->GetMaximum() > ymax) ymax = sig_hists.at(isig)->GetMaximum();
  }
//  if( logplot ) ymax*=30;
  if( logplot ) ymax*=2;
  else          ymax*=1.5;
  float ymin = 0.;
  
  
  if ( xmin==0 ) xmin = sig_hists.at(0)->GetXaxis()->GetXmin();
  if ( xmax==0 ) xmax = sig_hists.at(0)->GetXaxis()->GetXmax();
  
  
  TH2F* h_axes = new TH2F(nplots[0],"",1000,xmin,xmax,1000,ymin,ymax);
  h_axes->GetXaxis()->SetTitle(xtitle.c_str());
  h_axes->GetXaxis()->SetLabelSize(0.04);
  h_axes->GetXaxis()->SetTitleSize(0.05);
  h_axes->GetYaxis()->SetTitle(ytitle.c_str());
  h_axes->GetYaxis()->SetLabelSize(0.04);
  h_axes->GetYaxis()->SetTitleOffset(1.5);
//  h_axes->GetYaxis()->SetTitleOffset(1);
  h_axes->GetYaxis()->SetTitleSize(0.05);
  if (doRatio) {
    h_axes->GetXaxis()->SetLabelSize(0.);
    h_axes->GetXaxis()->SetTitleSize(0.);
  }
  h_axes->Draw();
  
  //t->Draw("hist same");
  
  // add signal hists
  for (unsigned int isig = 0; isig < sig_hists.size(); ++isig) {
    //sig_hists.at(isig)->Draw("hist same");
    sig_hists.at(isig)->Draw("same");
    leg->AddEntry(sig_hists.at(isig),sig_names.at(isig),"lp");
  }
  
  //if (data_hist) data_hist->Draw("pe same");
  
  TLatex label;
  label.SetNDC();
  label.SetTextSize(0.032);
  float label_y_start = 0.82;
  float label_y_spacing = 0.04;
  if (doRatio) {
    label.SetTextSize(0.039);
    label_y_start = 0.84;
    label_y_spacing = 0.04;
  }
  
  //  //TString ht_label = getHTPlotLabel(histdir);
  //  TString ht_label = getHTPlotLabel(samples.at(0), histdir);
  //  TString region_label = getJetBJetPlotLabel(samples.at(0), histdir);
  //  TString region_label_line2 = getMT2PlotLabel(samples.at(0), histdir);
  //  //label.DrawLatex(0.2,0.85,ht_label);
  //  label.DrawLatex(0.187,label_y_start,ht_label);
  //  // base region plots all have at least 2 jets
  //  if ((histdir.find("base") != std::string::npos)) region_label = "#geq 2j";
  //  // minMT plot always requires at least 2 bjets
  //  if ((histdir.find("srbase") != std::string::npos) && (histname.find("minMTBMet") != std::string::npos)) region_label = "#geq 2j, #geq 2b";
  //  // lostlepton CR
  //  if ((histdir.find("crsl") != std::string::npos)) region_label += ", 1 lepton";
  //
  //  if (region_label.Length() > 0) label.DrawLatex(0.187,label_y_start - label_y_spacing,region_label);
  //  if (region_label_line2.Length() > 0) label.DrawLatex(0.187,label_y_start - 2 * label_y_spacing,region_label_line2);
  
  leg->Draw();
  //h_axes->Draw("axissame");
  
  
  // -- for CMS_lumi label
  
  const int iPeriod = 4; // 13 tev
  
  // iPos drives the position of the CMS logo in the plot
  // iPos=11 : top-left, left-aligned
  // iPos=33 : top-right, right-aligned
  // iPos=22 : center, centered
  // mode generally :
  //   iPos = 10*(alignement 1/2/3) + position (1/2/3 = left/center/right)
  //const int iPos = 11;
  const int iPos = 3; // try out of frame
  
  
  if (doRatio) {
    lumiTextSize     = 0.8;
    cmsTextSize      = 1.0;
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
    TPad* ratiopad = new TPad("ratiopad","ratiopad",0.,0.,1,0.23);
    ratiopad->SetLeftMargin(0.15);
    ratiopad->SetRightMargin(0.05);
    ratiopad->SetTopMargin(0.08);
    ratiopad->SetBottomMargin(0.44);
    ratiopad->SetGridy();
    ratiopad->Draw();
    ratiopad->cd();
    
//    TH1D* h_ratio = (TH1D*) data_hist->Clone(Form("ratio_%s",data_hist->GetName()));
    TH1D* h_ratio = (TH1D*) sig_hists[0]->Clone(Form("ratio_%s",sig_hists[0]->GetName()));
    h_ratio->Sumw2();
    //sig_hists[1]->Sumw2();
    h_ratio->Divide(sig_hists[1]);
    
    // draw axis only
    TH1F* h_axis_ratio = new TH1F(Form("%s_axes",h_ratio->GetName()),"",100,xmin,xmax);
    h_axis_ratio->GetYaxis()->SetTitleOffset(0.3);
    h_axis_ratio->GetYaxis()->SetTitleSize(0.18);
    h_axis_ratio->GetYaxis()->SetNdivisions(5);
    h_axis_ratio->GetYaxis()->SetLabelSize(0.15);
    //h_axis_ratio->GetYaxis()->SetRangeUser(0.,1.5);
    h_axis_ratio->GetYaxis()->SetRangeUser(0.001,1.);
    h_axis_ratio->GetYaxis()->SetTitle("Z / #gamma");
    h_axis_ratio->GetXaxis()->SetTitle(sig_hists[1]->GetXaxis()->GetTitle());
    h_axis_ratio->GetXaxis()->SetTitleSize(0.17);
    h_axis_ratio->GetXaxis()->SetLabelSize(0.17);
    h_axis_ratio->GetXaxis()->SetTitleOffset(1.0);
    h_axis_ratio->GetXaxis()->SetTickLength(0.07);
    h_axis_ratio->Draw("axis");
    
    TGraphErrors* g_ratio = new TGraphErrors(h_ratio);
    g_ratio->SetName(Form("%s_graph",h_ratio->GetName()));
    for (int ibin=0; ibin < h_ratio->GetNbinsX(); ++ibin) {
      g_ratio->SetPointError(ibin, h_ratio->GetBinWidth(ibin+1)/2., h_ratio->GetBinError(ibin+1));
    }
    g_ratio->SetLineColor(kBlack);
    g_ratio->SetMarkerColor(kBlack);
    g_ratio->SetMarkerStyle(20);
    g_ratio->Draw("p0same");
    
  } // if (doRatio)
  
  gPad->Modified();
  
  can->Print(Form("compareMultiPlot/%s.pdf",canvas_name.Data()));
  
  
  return;
}



void Superimp(TString file1, TString file2, TString la1, TString la2, TString nplot, TString xname, int reb =1, TString fix="")
{
  TFile f1( file1, "READ");
  TFile f2( file2, "READ");

  TH1D* h1= (TH1D*) f1.Get(nplot)->Clone("h1");
  TH1D* h2= (TH1D*) f2.Get(nplot+fix)->Clone("h2");
  h1->Rebin(reb);
  h2->Rebin(reb);
  TLegend *l = new TLegend(0.80,0.75, 0.88,0.88);
//  TLegend *l = new TLegend(0.15,0.60, 0.45,0.85);
  l->AddEntry(h1, la1, "lp");
  l->AddEntry(h2, la2, "lp");
  l->SetShadowColor(0);
  l->SetFillColor(0);
  l->SetBorderSize(0);
  l->SetTextFont(22);
  l->SetTextSize(0.04);

  canv = new TCanvas();
  canv->cd();
  gStyle->SetOptStat(0);

  h1->SetMarkerStyle(23);
  h2->SetMarkerStyle(22);
  h2->SetLineColor(2);
  h2->SetMarkerColor(2);
  h1->GetXaxis()->SetTitle(xname);
  h2->GetXaxis()->SetTitle(xname);
  h1->GetYaxis()->SetRangeUser(0, h1->GetMaximum()*1.3);

  if ( xname.Contains("ntrk") )h1->GetXaxis()->SetRangeUser(0,20);
  if ( xname.Contains("Mass") )h1->GetXaxis()->SetRangeUser(0,7);
  h1->SetTitle("");
  h2->SetTitle("");
  h1->DrawNormalized();
  h2->DrawNormalized("same");
  //h1->Draw();
  //h2->Draw("same");
  l->Draw("same");

  canv->Print("compare/" + nplot + fix + ".pdf"); 
  delete canv;
  delete l   ;
  return;
}

void SuperimpVec(vector<TString> files, vector<TString> labels, vector<TString> nplots, TString xname, float maxY, TString saveas, bool norm=0, int X1 = 30, int X2 = 150, bool doLog = false, int reb=1)
{
  cout << "-- plotting hist: " << nplots[0] << " etc "<<endl;
  
  cmsText = "CMS Preliminary";
  lumi_13TeV = "3 fb^{-1}";
  
  canv = new TCanvas("c1_n48", "c1_n48",600, 600);
  canv->SetBottomMargin(0.14);
  canv->cd();
  gStyle->SetOptStat(0);
  
  gStyle->SetOptStat("");
  gStyle->SetCanvasColor(0);
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadTopMargin(0.08);
  gStyle->SetPadBottomMargin(0.12);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.05);
  
  
  TLegend *l = new TLegend(0.40,0.65, 0.88,0.88);
  l->SetShadowColor(0);
  l->SetFillColor(0);
  l->SetBorderSize(0);
  l->SetTextFont(22);
  l->SetTextSize(0.04);
  //float maxY = 0;
  bool firstToDraw = true;
  int skipped = 0;
  for (unsigned int i = 0; i < files.size(); i++) {
    if (files[i]=="") {
      l->SetHeader(labels[i]);
      skipped++;
    }
    TFile f1( files[i], "READ");
    if(!f1.IsOpen()){
      std::cout<<"File "<<files[i]<<" can't be found, will not be used for histogram "<<nplots[i]<<std::endl;
      continue;
    }
    if((TH1D*) f1.Get(nplots[i])==0){
      std::cout<<"Histogram "<<nplots[i]<<" not found in file "<<files[i]<<std::endl;
      continue;
    }
    TH1D* h1= (TH1D*) f1.Get(nplots[i])->Clone("h1");
    
    TString nameTitle = files[i] + nplots[i] + labels[i];
    h1->SetNameTitle("","");
    h1->SetDirectory(0);
    h1->Rebin(reb);
    if ( labels[i].Contains("True Z inv") ) l->AddEntry(h1, labels[i], "f");
    else l->AddEntry(h1, labels[i], "lpe");
    h1->SetMarkerStyle(21+i-skipped);
    h1->SetLineColor(i+1-skipped);
    h1->SetMarkerColor(i+1-skipped);
    h1->GetXaxis()->SetTitle(xname);
    h1->GetXaxis()->SetTitleOffset(0.9);
    //    if (maxY < h1->GetMaximum()*1.3) maxY = h1->GetMaximum()*1.3;
    //  h1->GetYaxis()->SetRangeUser(0, 0.15);
    h1->GetXaxis()->SetRangeUser(X1, X2);
    
    
    if (norm) {
      h1->Scale(1./h1->Integral());
    }
    
    TString drawing = "";
    
    if ( labels[i].Contains("True Z inv") ) {
      h1->SetFillColor(i+1);
      h1->SetLineStyle(0);
      h1->SetMarkerSize(0);
      drawing = "E2";
    }
    if (firstToDraw) {h1->Draw(drawing); firstToDraw=false;}
    else h1->Draw("same");
    if (maxY==0) maxY = h1->GetMaximum()*1.3;
    if (doLog) {  
      canv->SetLogy(1);   
      h1->GetYaxis()->SetRangeUser(0.1, maxY);
    }
    else h1->GetYaxis()->SetRangeUser(0, maxY);
    
    if (nplots[i].Contains("dijetflav")) {
      h1->GetXaxis()->SetBinLabel(1, "jj");
      h1->GetXaxis()->SetBinLabel(2, "cj");
      h1->GetXaxis()->SetBinLabel(3, "bj");
      h1->GetXaxis()->SetBinLabel(4, "cc");
      h1->GetXaxis()->SetBinLabel(5, "bc");      
      h1->GetXaxis()->SetBinLabel(6, "bb"); 
      h1->GetXaxis()->SetLabelSize(0.05);
      //h1->GetXaxis()->SetLabelOffset(0.02);
      
    }
    h1->GetXaxis()->SetTitle(xname);
    h1->GetXaxis()->SetLabelSize(0.04);
    h1->GetXaxis()->SetTitleSize(0.05);
    h1->GetYaxis()->SetTitle("Z/#gamma Ratio");
    h1->GetYaxis()->SetLabelSize(0.04);
    h1->GetYaxis()->SetTitleOffset(1.1);
    h1->GetYaxis()->SetTitleSize(0.05);
    
    
    f1.Close();
  }
  l->Draw("same");
  TLatex label;
  label.SetNDC();
  label.SetTextSize(0.032);
  float label_y_start = 0.82;
  float label_y_spacing = 0.04;
  const int iPeriod = 4; // 13 tev
  
  // iPos drives the position of the CMS logo in the plot
  // iPos=11 : top-left, left-aligned
  // iPos=33 : top-right, right-aligned
  // iPos=22 : center, centered
  // mode generally :
  //   iPos = 10*(alignement 1/2/3) + position (1/2/3 = left/center/right)
  //const int iPos = 11;
  const int iPos = 0;
  lumiTextSize     = 0.45;
  cmsTextSize      = 0.55;
  CMS_lumi( canv, iPeriod, iPos );
  
  canv->Print("compareMultiPlot/" + saveas+ ".pdf");
  //delete canv;
  //delete l   ;
  return;

}


void SuperimpRatios(vector<TString> files, vector<TString> labels, vector<TString> nplots, TString xname, float maxY, TString saveas, bool norm=0, int X1 = 30, int X2 = 150, bool unsetSumW2 = false, int rebin = 1, TString yname="")
{
  
  canv = new TCanvas("c1_n48", "c1_n48");
  canv->SetBottomMargin(0.14);
  canv->cd();
  gStyle->SetOptStat(0);
  TLegend *l = new TLegend(0.50,0.65, 0.88,0.88);
  l->SetShadowColor(0);
  l->SetFillColor(0);
  l->SetBorderSize(0);
  l->SetTextFont(22);
  l->SetTextSize(0.04);
  //float maxY = 0;
  bool firstToDraw = true;
  int skipped = 0;
  for (unsigned int i = 0; i < files.size(); i++) {
    if (files[i]=="") {
      l->SetHeader(labels[i]);
      skipped++;
    }
    
    TFile f1( files[i], "READ");
    if(!f1.IsOpen()){
      std::cout<<"File "<<files[i]<<" can't be found, will not be used for histogram "<<nplots[i]<<std::endl;
      continue;
    }
    if((TH1D*) f1.Get(nplots[i])==0){
      std::cout<<"Histogram "<<nplots[i]<<" not found in file "<<files[i]<<std::endl;
      continue;
    }
    TH1D* h1= (TH1D*) f1.Get(nplots[i])->Clone("h1");
    
    TFile f2( files[i+1], "READ");
    if(!f2.IsOpen()){
      std::cout<<"File "<<files[i+1]<<" can't be found, will not be used for histogram "<<nplots[i]<<std::endl;
      continue;
    }
    if((TH1D*) f2.Get(nplots[i+1])==0){
      std::cout<<"Histogram "<<nplots[i+1]<<" not found in file "<<files[i+1]<<std::endl;
      continue;
    }
    TH1D* h2= (TH1D*) f2.Get(nplots[i+1])->Clone("h2");
    h1->Rebin(rebin);
    h2->Rebin(rebin);
    
    if (unsetSumW2) {
      h1->GetSumw2()->Set(0);   
      h2->GetSumw2()->Set(0); 
      h1->Sumw2(); // need to reset these so that the division is done correctly
      h2->Sumw2();
    }

    h1->Divide(h2);
    
    TString nameTitle = files[i] + nplots[i] + labels[i];
    h1->SetNameTitle("","");
    h1->SetDirectory(0);
    //h1->Rebin(reb);
    l->AddEntry(h1, labels[i], "lp");
    h1->SetMarkerStyle(21+i-skipped);
    h1->SetLineColor(i+1-skipped);
    h1->SetMarkerColor(i+1-skipped);
    h1->GetXaxis()->SetTitle(xname);
    h1->GetXaxis()->SetTitleOffset(0.9);
    //    if (maxY < h1->GetMaximum()*1.3) maxY = h1->GetMaximum()*1.3;
    //  h1->GetYaxis()->SetRangeUser(0, 0.15);
    h1->GetXaxis()->SetRangeUser(X1, X2);
    h1->GetYaxis()->SetTitleOffset(0.83);
    h1->GetYaxis()->SetTitle(yname);
    
    TString drawing = "";

    if (unsetSumW2) drawing = "E1";
    
    if (firstToDraw) {h1->Draw(drawing); firstToDraw=false;}
    else h1->Draw(drawing+"same");
    


    
    if (maxY==0) maxY = h1->GetMaximum()*1.3;
    
    h1->GetYaxis()->SetRangeUser(0, maxY);
    
    f1.Close();
    f2.Close();
    i++;
  }
  l->Draw("same");
  canv->Print("compareMultiPlot/" + saveas+ ".pdf"); 
  //delete canv;
  //delete l   ;
  return;
  
}



void SuperimpRatiosFlat(vector<TString> files, vector<TString> labels, vector<TString> nplots, TString xname, float maxY, TString saveas, bool norm=0, int X1 = 30, int X2 = 150, bool unsetSumW2 = false, int rebin = 1, TString yname="")
{
  
  canv = new TCanvas("c1_n48", "c1_n48");
  canv->SetBottomMargin(0.14);
  canv->cd();
  gStyle->SetOptStat(0);
  TLegend *l = new TLegend(0.50,0.65, 0.88,0.88);
  l->SetShadowColor(0);
  l->SetFillColor(0);
  l->SetBorderSize(0);
  l->SetTextFont(22);
  l->SetTextSize(0.04);
  //float maxY = 0;
  bool firstToDraw = true;
  int skipped = 0;
  for (unsigned int i = 0; i < files.size(); i++) {
    if (files[i]=="") {
      l->SetHeader(labels[i]);
      skipped++;
    }
    
    TFile f1( files[i], "READ");
    if(!f1.IsOpen()){
      std::cout<<"File "<<files[i]<<" can't be found, will not be used for histogram "<<nplots[i]<<std::endl;
      continue;
    }
    if((TH1D*) f1.Get(nplots[i])==0){
      std::cout<<"Histogram "<<nplots[i]<<" not found in file "<<files[i]<<std::endl;
      continue;
    }
    TH1D* h1= (TH1D*) f1.Get(nplots[i])->Clone("h1");
    
    TFile f2( files[i+1], "READ");
    if(!f2.IsOpen()){
      std::cout<<"File "<<files[i+1]<<" can't be found, will not be used for histogram "<<nplots[i]<<std::endl;
      continue;
    }
    if((TH1D*) f2.Get(nplots[i+1])==0){
      std::cout<<"Histogram "<<nplots[i+1]<<" not found in file "<<files[i+1]<<std::endl;
      continue;
    }
    TH1D* h2= (TH1D*) f2.Get(nplots[i+1])->Clone("h2");
    h1->Rebin(rebin);
    h2->Rebin(rebin);
    
    if (unsetSumW2) {
      h1->GetSumw2()->Set(0);   
      h2->GetSumw2()->Set(0); 
      h1->Sumw2(); // need to reset these so that the division is done correctly
      h2->Sumw2();
    }
    
    h1->Divide(h2);
    
    // Now we make the magic: set bin content to 1, and bin error to relative error of h1
    for (int ib = 1; ib <= h1->GetNbinsX(); ib++) {
      if (h1->GetBinContent(ib)!=0) {
        float relerror = h1->GetBinError(ib)/h1->GetBinContent(ib);
        h1->SetBinContent(ib, 1.);
        h1->SetBinError(ib, relerror);
      }
    }    
    
    TString nameTitle = files[i] + nplots[i] + labels[i];
    h1->SetNameTitle("","");
    h1->SetDirectory(0);
    //h1->Rebin(reb);
    l->AddEntry(h1, labels[i], "lp");
    h1->SetMarkerStyle(21+i-skipped);
    h1->SetLineColor(i+1-skipped);
    h1->SetMarkerColor(i+1-skipped);
    h1->GetXaxis()->SetTitle(xname);
    h1->GetXaxis()->SetTitleOffset(0.9);
    //    if (maxY < h1->GetMaximum()*1.3) maxY = h1->GetMaximum()*1.3;
    //  h1->GetYaxis()->SetRangeUser(0, 0.15);
    h1->GetXaxis()->SetRangeUser(X1, X2);
    h1->GetYaxis()->SetTitleOffset(0.83);
    h1->GetYaxis()->SetTitle(yname);
    gPad->SetGridy();
    
    TString drawing = "";
    
    if (unsetSumW2) drawing = "E1";
    
    if (firstToDraw) {h1->Draw(drawing); firstToDraw=false;}
    else h1->Draw(drawing+"same");
    
    
    
    
    if (maxY==0) maxY = h1->GetMaximum()*1.3;
    
    h1->GetYaxis()->SetRangeUser(1+ -1. * maxY, 1+maxY);
    
    f1.Close();
    f2.Close();
    i++;
  }
  l->Draw("same");
  canv->Print("compareMultiPlot/" + saveas+ ".pdf"); 
  //delete canv;
  //delete l   ;
  return;
  
}

void SuperimpVecRatio(vector<TString> files, vector<TString> labels, vector<TString> nplots, TString xname, float maxY, TString saveas, bool norm=0, int X1 = 30, int X2 = 150, bool doLog = false)
{
  
  canv = new TCanvas("c1_n48", "c1_n48",10,32,787,573);
  canv->cd();
  gStyle->SetOptStat(0);
  TLegend *l = new TLegend(0.60,0.65, 0.88,0.88);
  l->SetShadowColor(0);
  l->SetFillColor(0);
  l->SetBorderSize(0);
  l->SetTextFont(22);
  l->SetTextSize(0.04);
  //float maxY = 0;
  
  
  TPad* p01=new TPad("p01","p01",0,0.25,1,1);
  p01->SetFillStyle(4000);
  p01->SetFillColor(0);
  p01->SetFrameFillStyle(4000);
  p01->SetFrameFillColor(0);
  p01->SetFrameLineColor(0);
  p01->Range(-70.32233,-8.217202,421.3903,1071.598);
  p01->SetBorderMode(0);
  p01->SetBorderSize(2);
  p01->SetTickx(1);
  p01->SetTicky(1);
  p01->SetLeftMargin(0.1430151);
  p01->SetRightMargin(0.04350161);
  p01->SetTopMargin(0.05269226);
  p01->SetBottomMargin(0.007609824);
  p01->SetFrameBorderMode(0);
  p01->SetFrameBorderMode(0);
  p01->Draw();
  p01->cd();

  float autoMaxY = -999;
  bool firstToDraw = true;
  for (unsigned int i = 0; i < files.size(); i++) {
    TFile f1( files[i], "READ");
    if(!f1.IsOpen()){
      std::cout<<"File "<<files[i]<<" can't be found, will not be used for histogram "<<nplots[i]<<std::endl;
      continue;
    }
    if(f1.GetKey(nplots[i].Data())==0){
      std::cout<<"Histogram "<<nplots[i]<<" not found in file "<<files[i]<<std::endl;
      continue;
    }
    TH1D* h1= (TH1D*) f1.Get(nplots[i])->Clone("h1");                
    
    TString nameTitle = files[i] + nplots[i] + labels[i];
    h1->SetNameTitle("","");
    h1->SetDirectory(0);
    //h1->Rebin(reb);
    l->AddEntry(h1, labels[i], "lp");
    h1->SetMarkerStyle(21+i);
    h1->SetLineColor(i+1);
    h1->SetMarkerColor(i+1);
    h1->GetXaxis()->SetTitle(xname);
    //    if (maxY < h1->GetMaximum()*1.3) maxY = h1->GetMaximum()*1.3;
    //  h1->GetYaxis()->SetRangeUser(0, 0.15);
    h1->GetYaxis()->SetTitleSize(0.05);
    h1->GetXaxis()->SetRangeUser(X1, X2);
    if (norm) {
      h1->Scale(1./h1->Integral());
    }
    if (firstToDraw) {h1->Draw(); firstToDraw=false;}
    else h1->Draw("same");
    
    if (autoMaxY < h1->GetMaximum()*1.3 ) autoMaxY = h1->GetMaximum()*1.3;
    if (doLog) {  
      p01->SetLogy(1);   
      h1->GetYaxis()->SetRangeUser(1, maxY==0 ? autoMaxY*2 : maxY*2);
    }
    else h1->GetYaxis()->SetRangeUser(0, maxY==0 ? autoMaxY : maxY); // this doesn't quite work. Only first histogram is taken into account
    

    f1.Close();
  }
  l->Draw("same");
  
  canv->cd();
  TPad* p02=new TPad("p02","p02",0,0,1,0.25);
  p02->SetFillStyle(4000);
  p02->SetFillColor(0);
  p02->SetFrameFillStyle(4000);
  p02->SetFrameFillColor(0);
  p02->SetFrameLineColor(0);
  p02->Range(-69.27659,-17.79753,420.5338,19.06095);
  p02->SetLeftMargin(0.1414355);
  p02->SetRightMargin(0.04192203);
  p02->SetTopMargin(0.01176064);
  p02->SetBottomMargin(0.39156);
  p02->SetTickx(1);
  p02->SetTicky(1);
  p02->SetGridy();
  p02->SetBorderMode(0);
  p02->SetFrameBorderMode(0);
  p02->SetFrameBorderMode(0);
  p02->Draw();
  p02->cd();
  
  TFile f( files[0], "READ");
  if(!f.IsOpen()){
    std::cout<<"File "<<files[0]<<" can't be found, will not be used for ratio histogram "<<nplots[0]<<std::endl;
    return;
  }
  if(f.GetKey(nplots[0].Data())==0){
    std::cout<<"Histogram "<<nplots[0]<<" for ratio not found in file "<<files[0]<<std::endl;
    return;
  }
  TH1D* h0= (TH1D*) f.Get(nplots[0])->Clone("h0"); 
  TString nameTitle = files[0] + nplots[0] + labels[0];
  h0->SetNameTitle("","");
  h0->SetDirectory(0);
  f.Close();
  
   firstToDraw = true;
  for (unsigned int i = 1; i < files.size(); i++) {
    TFile f1( files[i], "READ");
    if(!f1.IsOpen()){
      std::cout<<"File "<<files[i]<<" can't be found, will not be used for ratio histogram "<<nplots[i]<<std::endl;
      continue;
    }
    if(f1.GetKey(nplots[i].Data())==0){
      std::cout<<"Histogram "<<nplots[i]<<" not found in file "<<files[i]<<std::endl;
      continue;
    }
    TH1D* h1= (TH1D*) f1.Get(nplots[i])->Clone("h1");                
    h1->Divide(h0);
    
    TString nameTitle = files[i] + nplots[i] + labels[i];
    h1->SetNameTitle("","");
    h1->SetDirectory(0);
    //h1->Rebin(reb);
    //l->AddEntry(h1, labels[i], "lp");
    h1->SetMarkerStyle(21+i);
    h1->SetLineColor(i+1);
    h1->SetMarkerColor(i+1);
    h1->GetXaxis()->SetTitle(xname);
    
    h1->GetXaxis()->SetLabelSize(0.16);
    h1->GetXaxis()->SetTitleSize(0.17);
    //diff->GetXaxis()->SetTitleFont(42);
    h1->GetYaxis()->SetTitle("/ black");
    h1->GetYaxis()->SetNdivisions(505);
    //diff->GetYaxis()->SetLabelFont(42);
    h1->GetYaxis()->SetLabelSize(0.1);
    h1->GetYaxis()->SetTitleSize(0.12);
    h1->GetYaxis()->SetTitleOffset(0.38);
    //diff->GetYaxis()->SetTitleFont(42);
    if (norm || nplots[i].Contains("eff") || nplots[i].Contains("fr"))     
      h1->GetYaxis()->SetRangeUser(0.2, 1.8);

    if ( saveas.Contains("Ratio_FOpt") )
      h1->GetYaxis()->SetRangeUser(-0.1, 0.5);
    
    //    if (maxY < h1->GetMaximum()*1.3) maxY = h1->GetMaximum()*1.3;
    //  h1->GetYaxis()->SetRangeUser(0, 0.15);
    h1->GetXaxis()->SetRangeUser(X1, X2);
    if (nplots[i].Contains("fo_ID")) {
      h1->GetXaxis()->SetBinLabel(1, "");
      h1->GetXaxis()->SetBinLabel(2, "other");
      h1->GetXaxis()->SetBinLabel(3, "C-had");
      h1->GetXaxis()->SetBinLabel(4, "B-had");      
      h1->GetXaxis()->SetBinLabel(5, "fake"); 
      h1->GetXaxis()->SetLabelSize(0.20);
      h1->GetXaxis()->SetLabelOffset(0.02);

    }

    if (firstToDraw) {h1->Draw(); firstToDraw=false;}
    else h1->Draw("same");
    //h1->GetYaxis()->SetRangeUser(0, maxY);
    
    f1.Close();
  }
  //l->Draw("same");

  
  
  
  canv->Print("compareMultiPlot/" + saveas+ ".pdf"); 
  //delete canv;
  //delete l   ;
  return;
  
}

void compareMultiPlotSoft()
{

  TString dir = "/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/softLep25Feb16/";

  vector<TString> files; vector<TString> labels; vector<TString> nplots;
  
  files.clear(); labels.clear(); nplots.clear();
  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("El Data (EWK subtracted)");                 nplots.push_back("FREWKcutsHT800El_Data_EwkSubtr");
  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("El Data");                                  nplots.push_back("FREWKcutsHT800El_Data");
  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("El MC");                                  nplots.push_back("FREWKcutsHT800El_MC");
  makeCMSPlot(  files, labels, nplots, "FR_el", /*xtitle*/ "p_{T}^{lep} [GeV]" , /*ytitle*/ "FR" , /*xmin*/ 0, /*xmax*/20 , /*rebin*/ 1 , /*logplot*/ 0, /*scalesig*/ -1., /*doRatio*/ 0 );
  
  
  files.clear(); labels.clear(); nplots.clear();
  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("Mu Data (EWK subtracted)");                 nplots.push_back("FREWKcutsHT800Mu_Data_EwkSubtr");
  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("Mu Data");                                  nplots.push_back("FREWKcutsHT800Mu_Data");
  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("Mu MC");                                    nplots.push_back("FREWKcutsHT800Mu_MC");
  makeCMSPlot(  files, labels, nplots, "FR_mu", /*xtitle*/ "p_{T}^{lep} [GeV]" , /*ytitle*/ "FR" , /*xmin*/ 0, /*xmax*/20 , /*rebin*/ 1 , /*logplot*/ 0, /*scalesig*/ -1., /*doRatio*/ 0 );
  
  files.clear(); labels.clear(); nplots.clear();
//  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("Mu Data");                 nplots.push_back("srLepbase/h_mtbins_MuPromptFraction");
  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("Mu MC");                   nplots.push_back("srLepbase/h_mtbins_MuPromptFractionMCClosure");
  makeCMSPlot(  files, labels, nplots, "PromptContMu", /*xtitle*/ "m_{T} [GeV]" , /*ytitle*/ "Prompt fraction in sideband" , /*xmin*/ 0, /*xmax*/300 , /*rebin*/ 1 , /*logplot*/ 0, /*scalesig*/ -1., /*doRatio*/ 0 );

  files.clear(); labels.clear(); nplots.clear();
//  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("El Data");                 nplots.push_back("srLepbase/h_mtbins_ElPromptFraction");
  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("El MC");                   nplots.push_back("srLepbase/h_mtbins_ElPromptFractionMCClosure");
  makeCMSPlot(  files, labels, nplots, "PromptContEl", /*xtitle*/ "m_{T} [GeV]" , /*ytitle*/ "Prompt fraction in sideband" , /*xmin*/ 0, /*xmax*/300 , /*rebin*/ 1 , /*logplot*/ 0, /*scalesig*/ -1., /*doRatio*/ 0 );

  files.clear(); labels.clear(); nplots.clear();
  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("Mu Data (FR method)");                 nplots.push_back("srLepbase/h_predMu");
  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("Mu MC (FR method)");                 nplots.push_back("srLepbase/h_predMCClosureMu");
  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("Mu MC (Truth)");                     nplots.push_back("srLepbase/h_predMuMC");
  makeCMSPlot(  files, labels, nplots, "MCClosure_mu", /*xtitle*/ "m_{T} [GeV]" , /*ytitle*/ "Entries" , /*xmin*/ 0, /*xmax*/300 , /*rebin*/ 1 , /*logplot*/ 1, /*scalesig*/ -1., /*doRatio*/ 0 );

  files.clear(); labels.clear(); nplots.clear();
  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("El Data (FR method)");                 nplots.push_back("srLepbase/h_predEl");
  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("El MC (FR method)");                 nplots.push_back("srLepbase/h_predMCClosureEl");
  files.push_back( dir+"pred_FakeRate.root");            labels.push_back("El MC (Truth)");                     nplots.push_back("srLepbase/h_predElMC");
  makeCMSPlot(  files, labels, nplots, "MCClosure_El", /*xtitle*/ "m_{T} [GeV]" , /*ytitle*/ "Entries" , /*xmin*/ 0, /*xmax*/300 , /*rebin*/ 1 , /*logplot*/ 1, /*scalesig*/ -1., /*doRatio*/ 0 );

  
  
  return;
}



