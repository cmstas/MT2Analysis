// -*- C++ -*-
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <algorithm>

#include "TROOT.h"
#include "TLatex.h"
#include "TString.h"
#include "TH2.h"
#include "THStack.h"
#include "TColor.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1D.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TKey.h"
#include "TGraphAsymmErrors.h"

#include "plotUtilities.C"
#include "CMS_lumi.C"
#include "Math/QuantFuncMathCore.h"

using namespace std;

ofstream ofile;

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
//______________________________________________________________________________
// returns the error on C = A*B (or C = A/B)
float err_mult(float A, float B, float errA, float errB, float C) {
  return sqrt(C*C*(pow(errA/A,2) + pow(errB/B,2)));
}

void addOverflow(TH1* h) {
  Double_t err = 0;
  float lastBinPlusOverflow = h->IntegralAndError(h->GetXaxis()->GetNbins(), -1, err);
  h->SetBinContent(h->GetXaxis()->GetNbins(), lastBinPlusOverflow);
  h->SetBinError(h->GetXaxis()->GetNbins(), err);
  return;
}

void addOverflowToXmax(TH1* h, double xmax) {
  Double_t err = 0;
  Int_t lastbin = h->FindBin(xmax) - 1;
  float lastBinPlusOverflow = h->IntegralAndError(lastbin, -1, err);
  h->SetBinContent(lastbin, lastBinPlusOverflow);
  h->SetBinError(lastbin, err);
  return;
}


TGraphAsymmErrors* getPoissonGraph( TH1D* histo, bool drawZeros, const std::string& xerrType = "0", float nSigma = 1 ) {

  const double alpha = 1 - 0.6827;

  unsigned int nBins = histo->GetNbinsX();
  int emptyBins=0;
  for( unsigned i=1; i < nBins; ++i ) {
    if( histo->GetBinContent(i)==0 ) emptyBins += 1;
  }
  if( (float)emptyBins/(float)nBins > 0.4 ) drawZeros=false;

  TGraphAsymmErrors* graph = new TGraphAsymmErrors(0);

  for( int iBin=1; iBin<(histo->GetXaxis()->GetNbins()+1); ++iBin ) {

    int y; // these are data histograms, so y has to be integer
    double x, xerr, yerrplus, yerrminus;
    x = histo->GetBinCenter(iBin);
    if( xerrType=="0" )
      xerr = 0.;
    else if( xerrType=="binWidth" )
      xerr = histo->GetBinWidth(iBin)/2.;
    else if( xerrType=="sqrt12" )
      xerr = histo->GetBinWidth(iBin)/sqrt(12.);
    else {
      std::cout << "[getPoissonGraph] Unkown xerrType '" << xerrType << "'. Setting to bin width." << std::endl;
      xerr = histo->GetBinWidth(iBin);
    }

    y = (int)histo->GetBinContent(iBin);

    if( y==0 && !drawZeros ) continue;

    double ym =  (y==0) ? 0  : (ROOT::Math::gamma_quantile(alpha/2,y,1.));
    double yp =  ROOT::Math::gamma_quantile_c(alpha/2,y+1,1) ;
    yerrplus = yp - y;
    yerrminus = y - ym;


    int thisPoint = graph->GetN();
    graph->SetPoint( thisPoint, x, y );
    graph->SetPointError( thisPoint, xerr, xerr, yerrminus, yerrplus );
    //cout<<"Point: "<<y<<" +"<<yerrplus<<" -"<<yerrminus<<endl;

  }

  return graph;

}

TGraphAsymmErrors* getRatioGraph( TH1D* histo_data, TH1D* histo_mc ){

  if( !histo_data || !histo_mc ) return 0;

  TGraphAsymmErrors* graph  = new TGraphAsymmErrors();

  TGraphAsymmErrors* graph_data = getPoissonGraph(histo_data, false);

  for( int i=0; i < graph_data->GetN(); ++i){

    Double_t x_tmp, data;
    graph_data->GetPoint( i, x_tmp, data );

    Double_t data_errUp = graph_data->GetErrorYhigh(i);
    Double_t data_errDn = graph_data->GetErrorYlow(i);

    int iBin = histo_mc->FindBin(x_tmp);
    float mc = histo_mc->GetBinContent(iBin);
    float mc_err = histo_mc->GetBinError(iBin);


    float ratio = data/mc;
    float ratio_errUp = sqrt( data_errUp*data_errUp/(mc*mc) + mc_err*mc_err*data*data/(mc*mc*mc*mc) );
    float ratio_errDn = sqrt( data_errDn*data_errDn/(mc*mc) + mc_err*mc_err*data*data/(mc*mc*mc*mc) );

    graph->SetPoint(i, x_tmp, ratio );
    graph->SetPointEYhigh(i, ratio_errUp );
    graph->SetPointEYlow(i, ratio_errDn );


  }

  graph->SetLineColor(1);
  graph->SetMarkerColor(1);
  graph->SetMarkerStyle(20);

  return graph;

}


//_______________________________________________________________________________
TCanvas* makePlot( const vector<TFile*>& samples , const vector<string>& names , const string& histdir , const string& histname , const string& xtitle , const string& ytitle , float xmin , float xmax , int rebin = 1 , bool logplot = true, bool printplot = false, float scalesig = -1., bool doRatio = false, bool scaleBGtoData = false, bool drawBand = false) {


  cout << "-- plotting histdir: " << histdir << ", histname: " << histname << endl;

  gStyle->SetOptStat("");
  gStyle->SetCanvasColor(0);
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetFrameBorderMode(0);

  // if ratio was requested, check if data is present.  If not, turn ratio off again
  const unsigned int n = samples.size();
  if (doRatio) {
    bool foundData = false;
    for( unsigned int i = 0 ; i < n ; ++i ) {
      if( TString(names.at(i)).Contains("data")  ) {
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

  TString canvas_name = Form("c_%s_%s",histdir.c_str(),histname.c_str());
  if (logplot) canvas_name += "_log";
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
    plotpad->SetRightMargin(0.05);
    plotpad->SetBottomMargin(0.12);
    plotpad->Draw();
    plotpad->cd();
    if( logplot ) plotpad->SetLogy();
  }

  TLegend* leg = new TLegend(0.65,0.57,0.85,0.87);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.026);
  if (doRatio) leg->SetTextSize(0.06);

  TH1D* data_hist(0);
  string data_name;
  for( unsigned int i = 0 ; i < n ; ++i ) {
    if( !TString(names.at(i)).Contains("data")  ) continue;
    TString fullhistname = Form("%s/%s",histdir.c_str(),histname.c_str());
    if (histdir.size() == 0) fullhistname = TString(histname);
    TString newhistname = Form("%s_%s_%s",histname.c_str(),histdir.c_str(),names.at(i).c_str());
    TH1D* h_temp = (TH1D*) samples.at(i)->Get(fullhistname);
    if (h_temp == 0) continue;
    data_hist = (TH1D*) h_temp->Clone(newhistname);
    data_name = names.at(i);
    data_hist->SetLineColor(kBlack);
    data_hist->SetMarkerColor(kBlack);
    data_hist->SetMarkerStyle(20);
    // addOverflow(data_hist); // Add Overflow
    addOverflowToXmax(data_hist, xmax); // Add Overflow to visual max
    if (rebin > 1) data_hist->Rebin(rebin);

    // fake data -> set error bars to correspond to data stats
    if (TString(data_name).Contains("fakedata")) {
      for (int ibin = 0; ibin <= data_hist->GetNbinsX(); ++ibin) {
        data_hist->SetBinError( ibin, sqrt(data_hist->GetBinContent(ibin)) );
      }
    } // if fakedata

    // expect to only find 1 data hist
    break;
  }

  TGraphAsymmErrors* graph_data{0};
  if (data_hist) {
    graph_data = getPoissonGraph(data_hist, true);
    graph_data->SetLineColor(kBlack);
    graph_data->SetMarkerColor(kBlack);
    graph_data->SetMarkerStyle(20);
  }

  if (graph_data) leg->AddEntry(graph_data,getLegendName(data_name).c_str(),"pe1");

  THStack* t = new THStack(Form("stack_%s_%s",histdir.c_str(),histname.c_str()),Form("stack_%s_%s",histdir.c_str(),histname.c_str()));
  TH1D* h_bgtot = 0;

  // to make legend and find max yvalue
  vector<TH1D*> bg_hists;
  vector<string> bg_names;
  vector<TH1D*> sig_hists;
  vector<string> sig_names;

  // background hists
  for( unsigned int i = 0 ; i < n ; ++i ) {
    if( TString(names.at(i)).Contains("data")  ) continue;
    if( TString(names.at(i)).Contains("sig")  ) continue;
    TString fullhistname = Form("%s/%s",histdir.c_str(),histname.c_str());
    // Make sure we take the "Fake" labeled histograms from MC
    if (TString(names.at(i)).Contains("fakephotonMC") ) {
      if (!fullhistname.Contains("Loose") ) fullhistname+="Fake";
      else if (fullhistname.Contains("LooseSieieSB")) fullhistname.ReplaceAll("LooseSieieSB", "FakeLooseSieieSB");
      else if (fullhistname.Contains("Loose")) fullhistname.ReplaceAll("Loose", "FakeLoose");
    }
    if (TString(names.at(i)).Contains("fakeLep") ) {
      fullhistname+="Fake";
    }
    // Try data-driven fake estimates: LooseNotTight * FR / (1 - FR)
    bool DataDrivenQCD = false;
    if (TString(names.at(i)).Contains("fakephotonData") && !fullhistname.Contains("Loose")) DataDrivenQCD = true;

    if (DataDrivenQCD) fullhistname+="LooseNotTight";

    if (histdir.size() == 0) fullhistname = TString(histname);
    TString newhistname = Form("%s_%s_%s",histname.c_str(),histdir.c_str(),names.at(i).c_str());
    TH1D* h_temp = (TH1D*) samples.at(i)->Get(fullhistname);
    if (h_temp == 0) continue;
    if (DataDrivenQCD)  {
      TH1D* h_prompt = (TH1D*) samples.at(i+1)->Get(fullhistname);
      if (h_prompt == 0) cout<<"Can't do prompt subtraction for "<<fullhistname<<endl;
      else h_temp->Add(h_prompt, -1.5); // 50% Data/MC scale factor
      h_temp->Scale( 0.168 / (1 - 0.168) ); // 0.168 Data FR
    }
    TH1D* h = (TH1D*) h_temp->Clone(newhistname);
    h->SetFillColor(getColor(names.at(i)));
    h->SetLineColor(kBlack);
    // addOverflow(h); // Add Overflow
    addOverflowToXmax(h, xmax); // Add Overflow to the visual max
    if (rebin > 1) h->Rebin(rebin);
    if( h_bgtot==0 ) h_bgtot = (TH1D*) h->Clone(Form("%s_%s_bgtot",histname.c_str(),histdir.c_str()));
    else h_bgtot->Add(h);

    bg_hists.push_back(h);
    bg_names.push_back(names.at(i));
  }

  // loop backwards to add to legend
  for (int ibg = (int) bg_hists.size()-1; ibg >= 0; --ibg) {
    leg->AddEntry(bg_hists.at(ibg),getLegendName(bg_names.at(ibg)).c_str(),"f");
  }

  // signal hists - all samples must have "sig" in the name
  for( unsigned int i = 0 ; i < n ; ++i ){
    if( !TString(names.at(i)).Contains("sig") ) continue;
    TString fullhistname = Form("%s/%s",histdir.c_str(),histname.c_str());
    if (histdir.size() == 0) fullhistname = TString(histname);
    TString newhistname = Form("%s_%s_%s",histname.c_str(),histdir.c_str(),names.at(i).c_str());
    TH1D* h_temp = (TH1D*) samples.at(i)->Get(fullhistname);
    if (h_temp == 0) continue;
    // don't draw signal if the total yield in the plot is < 0.1 events
    if (h_temp->Integral(0,-1) < 0.1) continue;
    TH1D* h = (TH1D*) h_temp->Clone(newhistname);
    h->SetLineColor(getColor(names.at(i)));
    h->SetLineWidth(2);
    // addOverflow(h); // Add Overflow
    addOverflowToXmax(h, xmax); // Add Overflow to visual max
    if (rebin > 1) h->Rebin(rebin);
    if (scalesig > 0.) h->Scale(scalesig);
    sig_hists.push_back(h);
    sig_names.push_back(names.at(i));
  }

  // loop through backgrounds to add hists to stack
  Double_t bg_integral_err = 0.;
  float bg_integral = h_bgtot->IntegralAndError(0,h_bgtot->GetXaxis()->GetNbins(),bg_integral_err);
  float data_integral = 1.;
  float bg_sf = 1.;
  float bg_sf_err = 0.;
  if (data_hist) {
    data_integral = data_hist->Integral(0,data_hist->GetXaxis()->GetNbins());
    bg_sf = data_integral/bg_integral;
    bg_sf_err = err_mult(data_integral,bg_integral,sqrt(data_integral),bg_integral_err,bg_sf);
    std::cout << "Data/MC is: " << bg_sf << " +/- " << bg_sf_err << std::endl;
  }
  for (unsigned int ibg = 0; ibg < bg_hists.size(); ++ibg) {
    if (scaleBGtoData && data_hist) bg_hists.at(ibg)->Scale(bg_sf);
    t->Add(bg_hists.at(ibg));
  }
  if (scaleBGtoData && data_hist) {
    h_bgtot->Scale(bg_sf);
    std::cout << "Scaled background by: " << bg_sf << " +/- " << bg_sf_err << std::endl;
  }

  float ymax = 0;
  if(h_bgtot) ymax = h_bgtot->GetMaximum();
  // also check signals for max val
  for (unsigned int isig = 0; isig < sig_hists.size(); ++isig) {
    ymax = std::max(ymax,(float)sig_hists.at(isig)->GetMaximum());
  }
  if (data_hist) ymax = std::max(ymax,(float)data_hist->GetMaximum());

  if( logplot ) ymax*=30;
  else          ymax*=1.5;
  float ymin = 0.1;

  if (xmin == -9999) xmin = data_hist->GetXaxis()->GetXmin();
  if (xmax == -9999) xmax = data_hist->GetXaxis()->GetXmax();

  TH2F* h_axes = new TH2F(Form("%s_%s_axes",histname.c_str(),histdir.c_str()),"",1000,xmin,xmax,1000,ymin,ymax);
  h_axes->GetXaxis()->SetTitle(xtitle.c_str());
  h_axes->GetXaxis()->SetLabelSize(0.04);
  h_axes->GetXaxis()->SetTitleSize(0.05);
  h_axes->GetYaxis()->SetTitle(ytitle.c_str());
  h_axes->GetYaxis()->SetLabelSize(0.04);
  h_axes->GetYaxis()->SetTitleOffset(0.95);
  h_axes->GetYaxis()->SetTitleSize(0.05);
  h_axes->Draw();

  t->Draw("hist same");
  if (drawBand) {
    // first add the shape uncertainty: 0-40%, then readjust firts bin to compensate
    int nbins = h_bgtot->GetNbinsX();
    float firstBinUnc = 0;
    float origerrbin1 = h_bgtot->GetBinError( 1 );
    for (unsigned int k = 1; k <= h_bgtot->GetNbinsX(); k++) {
      float cont = h_bgtot->GetBinContent(k);
      float origerr = h_bgtot->GetBinError(k);
      float percenterr = 0.4 / (nbins-1) * (k-1);
      if (k>1) {
        h_bgtot->SetBinError( k, sqrt( percenterr*cont*percenterr*cont + origerr*origerr ) );
        firstBinUnc += percenterr*cont;
      }
    }
    if (firstBinUnc > h_bgtot->GetBinContent(1)) firstBinUnc = h_bgtot->GetBinContent(1); // max to 100%
    h_bgtot->SetBinError( 1,  sqrt(firstBinUnc*firstBinUnc + origerrbin1*origerrbin1) );
    // then draw
    h_bgtot->SetMarkerSize(0);
    h_bgtot->SetFillColor (kGray+2);
    h_bgtot->SetFillStyle (3244);
    h_bgtot->Draw("E2,same");
  }

  // add signal hists
  for (unsigned int isig = 0; isig < sig_hists.size(); ++isig) {
    sig_hists.at(isig)->Draw("hist same");
    TString legend_name = getLegendName(sig_names.at(isig));
    if (scalesig > 0.) legend_name += Form(" x %.0f",scalesig);
    leg->AddEntry(sig_hists.at(isig),legend_name,"l");
  }

  //if (data_hist) data_hist->Draw("pe1 x0 same");
  if (graph_data) graph_data->Draw("p same"); // POISSON

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
  TString ht_label = getHTPlotLabel(samples.at(0), histdir);
  TString region_label = getJetBJetPlotLabel(samples.at(0), histdir);
  TString region_label_line2 = getMT2PlotLabel(samples.at(0), histdir);
  //label.DrawLatex(0.2,0.85,ht_label);

  // ------- some hardcoded labels..
  // base region plots all have at least 2 jets
  if ((histdir.find("base") != std::string::npos)) region_label = "#geq 2j";
//  if ((histdir.find("baseIncl") != std::string::npos)) { region_label = "#geq 1j"; region_label_line2 = ""; }
  if ((histdir.find("baseIncl") != std::string::npos)) {
    region_label = "MET > 200 GeV ( 1j )";
    region_label_line2 = "M_{T2} > 200 GeV ( #geq 2j )";
    if (histdir.find("crrl") != std::string::npos) {
      region_label = "MET > 200 GeV ( 1j, 0b)";
      region_label_line2 = "M_{T2} > 200 GeV ( #geq 2j, 0b )";
    }
  }
  if ((histdir.find("baseJ") != std::string::npos)) {
    region_label = "1j";
    region_label_line2 = "";
  }
  if ((histdir.find("nocut") != std::string::npos)) {
    ht_label = "HT #geq 0";
    region_label = "#geq 0j";
    region_label_line2 = "";
  }


  // bkg control regions
  if ((histdir.find("crslwjets") != std::string::npos)) region_label = "#geq 2j, 0b";
  if ((histdir.find("crslttbar") != std::string::npos)) region_label = "#geq 2j, #geq 2b";
  // minMT plot always requires at least 2 bjets
  if ((histdir.find("srbase") != std::string::npos) && (histname.find("minMTBMet") != std::string::npos)) region_label = "#geq 2j, #geq 2b";
  // lostlepton CR
  if ((histdir.find("crsl") != std::string::npos)) {
    ht_label = "";
    if (histdir.find("J") != std::string::npos) region_label_line2 = "E_{T}^{miss} > 200 GeV";
    if (histdir.find("met50") != std::string::npos) region_label_line2 = "E_{T}^{miss} > 50 GeV";
    else if (histdir.find("met80") != std::string::npos) region_label_line2 = "E_{T}^{miss} > 80 GeV";
    if (histdir.find("mt30") != std::string::npos) region_label_line2 += ", M_{T} > 30 GeV";
    if (histdir.find("J") != std::string::npos) region_label = "exactly 1j";
    else if (histdir.find("VL") != std::string::npos) {region_label = "#geq 2j"; ht_label = "200 < H_{T} < 450 GeV";}
    else if (histdir.find("L") != std::string::npos) {region_label = "#geq 2j"; ht_label = "450 < H_{T} < 575 GeV";}
    else if (histdir.find("M") != std::string::npos) {region_label = "#geq 2j"; ht_label = "575 < H_{T} < 1000 GeV";}
    else if (histdir.find("UH") != std::string::npos) {region_label = "#geq 2j"; ht_label = "H_{T} > 1500 GeV";}
    else if (histdir.find("H") != std::string::npos) {region_label = "#geq 2j"; ht_label = "1000 < H_{T} < 1500 GeV";}
    if (histdir.find("nj2") != std::string::npos) region_label = "#geq 2j";
    if (histdir.find("nb0") != std::string::npos) region_label += ", 0b";
    else if (histdir.find("nb2") != std::string::npos) region_label += ", #geq 2b";
  }
  if ((histdir.find("crslel") != std::string::npos)) region_label += ", 1 electron";
  else if ((histdir.find("crslmu") != std::string::npos)) region_label += ", 1 muon";
  else if ((histdir.find("crsl") != std::string::npos)) region_label += ", 1 lepton";

  // zll mt CR
  if (histdir.find("crmt") != std::string::npos) {
    if (histdir.find("base") != std::string::npos) region_label = "";
    else region_label = "#geq 2j, ";
    if ((histdir.find("base") != std::string::npos) || (histdir.find("nj2nb0") != std::string::npos)) ht_label = "";
    else if (histdir.find("ht200") != std::string::npos) ht_label = "H_{T} > 200 GeV";
    else if (histdir.find("ht450") != std::string::npos) ht_label = "H_{T} > 450 GeV";
    if (histdir.find("crmtel") != std::string::npos) region_label += "Z(ee) w/ removed lepton";
    else if (histdir.find("crmtmu") != std::string::npos) region_label += "Z(#mu#mu) w/ removed lepton";
    else region_label += "Z(ll) w/ removed lepton";
    if (histname.find("mt2gt50") != std::string::npos) region_label_line2 = "M_{T2} > 50 GeV";
    else if (histname.find("mt2gt100") != std::string::npos) region_label_line2 = "M_{T2} > 100 GeV";
    else if (histname.find("mt2gt150") != std::string::npos) region_label_line2 = "M_{T2} > 150 GeV";
    else if (histname.find("mt2gt200") != std::string::npos) region_label_line2 = "M_{T2} > 200 GeV";
    else region_label_line2 = "";
  }

  // Macro Regions
  if (histdir.find("Macro") != std::string::npos) {
    ht_label = "";
    region_label_line2 = "";
    if (histdir.find("HT1") != std::string::npos) { ht_label = "H_{T} [200, 1000] GeV"; }
    if (histdir.find("HT2") != std::string::npos) { ht_label = "H_{T} #geq 1000 GeV"; }
    if (histdir.find("NJ1NB1") != std::string::npos) { region_label = "2-3 j, 0 b"; }
    if (histdir.find("NJ1NB2") != std::string::npos) { region_label = "2-3 j, 1-2 b"; }
    if (histdir.find("NJ2NB1") != std::string::npos) { region_label = "#geq 4 j, 0 b"; }
    if (histdir.find("NJ2NB2") != std::string::npos) { region_label = "#geq 4 j, 1-2 b"; }
    if (histdir.find("NJ0NB1") != std::string::npos) { region_label = "1 j, 0 b"; }
    if (histdir.find("NJ0NB2") != std::string::npos) { region_label = "1 j, #geq 1 b"; }
    if (histdir.find("HT1NJ1NB3") != std::string::npos) { ht_label = "H_{T} #geq 200 GeV"; region_label = "#geq 2 j, #geq 3 b"; }
  }

  if (ht_label.Length() > 0) label.DrawLatex(label_x_start,label_y_start,ht_label);
  if (region_label.Length() > 0) label.DrawLatex(label_x_start,label_y_start - label_y_spacing,region_label);
  if (region_label_line2.Length() > 0) label.DrawLatex(label_x_start,label_y_start - 2 * label_y_spacing,region_label_line2);

  if (scaleBGtoData && data_hist) {
    TString scale_label = Form("MC scaled by %.2f #pm %.2f",bg_sf,bg_sf_err);
    //label.DrawLatex(0.6,0.55,scale_label);
  }


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
    // h_ratio->Sumw2();
    // h_bgtot->Sumw2();


    // draw axis only
    TH1F* h_axis_ratio = new TH1F(Form("%s_axes",h_ratio->GetName()),"",100,xmin,xmax);
    h_axis_ratio->GetYaxis()->SetTitleOffset(0.25);
    h_axis_ratio->GetYaxis()->SetTitleSize(0.18);
    h_axis_ratio->GetYaxis()->SetNdivisions(5);
    h_axis_ratio->GetYaxis()->SetLabelSize(0.15);
    //h_axis_ratio->GetYaxis()->SetRangeUser(0.5,1.5);
    h_axis_ratio->GetYaxis()->SetRangeUser(0.001,2.0);
    if (histdir.find("NJ1NB") != std::string::npos || histdir.find("NJ2NB") != std::string::npos )
      h_axis_ratio->GetYaxis()->SetRangeUser(0.001,3.0);
    h_axis_ratio->GetYaxis()->SetTitle("Data/MC");
    h_axis_ratio->GetXaxis()->SetTickLength(0.07);
    h_axis_ratio->GetXaxis()->SetTitleSize(0.);
    h_axis_ratio->GetXaxis()->SetLabelSize(0.);
    h_axis_ratio->Draw("axis");

    // draw line at 1
    TLine* line1 = new TLine(xmin,1,xmax,1);
    line1->SetLineStyle(2);
    line1->Draw("same");


    // STANDARD ROOT UNCERTAINTIES
    /*
    if (!drawBand) h_ratio->Divide(h_bgtot); // let Divide combine the uncertainties
    else {
      //make ratio with only Data uncertainty
      TH1D* h_bgtotNoErr = (TH1D*) h_bgtot->Clone("bgtotNoErr");
      for (int ibin=0; ibin < h_bgtotNoErr->GetNbinsX(); ++ibin) {
        h_bgtotNoErr->SetBinError(ibin, 0.);
      }
      h_ratio->Divide(h_bgtotNoErr);
      // then draw the background uncertainty: just divide the histogram by the no-error version of itself
      TH1D* h_bgtotFlat = (TH1D*) h_bgtot->Clone("bgtotFlat");
      h_bgtotFlat->Divide(h_bgtotNoErr);
      h_bgtotFlat->SetMarkerSize(0);
      h_bgtotFlat->SetFillColor (kGray+2);
      h_bgtotFlat->SetFillStyle (3244);
      h_bgtotFlat->Draw("E2,same");
    }

    TGraphErrors* g_ratio = new TGraphErrors(h_ratio);
    g_ratio->SetName(Form("%s_graph",h_ratio->GetName()));
    for (int ibin=0; ibin < h_ratio->GetNbinsX(); ++ibin) {
      //      g_ratio->SetPointError(ibin, h_ratio->GetBinWidth(ibin+1)/2., h_ratio->GetBinError(ibin+1));
      g_ratio->SetPointError(ibin, 0., h_ratio->GetBinError(ibin+1));
    }
     */
    // END OF STANDARD ROOT UNCERTAINTIES

    // POISSON UNCERTAINTIES
    TGraphAsymmErrors* g_ratio = getRatioGraph( data_hist, h_bgtot );
    if (drawBand) { // Remove MC uncertainties, since they are drawn in the band
      TH1D* h_bgtotNoErr = (TH1D*) h_bgtot->Clone("bgtotNoErr");
      for (int ibin=0; ibin < h_bgtotNoErr->GetNbinsX(); ++ibin) {
        h_bgtotNoErr->SetBinError(ibin, 0.);
      }
      g_ratio = getRatioGraph( data_hist, h_bgtotNoErr );
      // then draw the background uncertainty: just divide the histogram by the no-error version of itself
      TH1D* h_bgtotFlat = (TH1D*) h_bgtot->Clone("bgtotFlat");
      h_bgtotFlat->Divide(h_bgtotNoErr);
      h_bgtotFlat->SetMarkerSize(0);
      h_bgtotFlat->SetFillColor (kGray+2);
      h_bgtotFlat->SetFillStyle (3244);
      h_bgtotFlat->Draw("E2,same");
    }
    // END OF POISSON UNCERTAINTIES

    g_ratio->SetLineColor(kBlack);
    g_ratio->SetMarkerColor(kBlack);
    g_ratio->SetMarkerStyle(20);
    g_ratio->Draw("p same");
  } // if (doRatio)

  gPad->Modified();
  gPad->Update();

  if( printplot ) {
    // can->Print(Form("plots/%s.pdf",canvas_name.Data()));
    can->Print(Form("plots/%s.png",canvas_name.Data()));
    //can->Print(Form("plots/%s.eps",canvas_name.Data()));
  }

  return can;
}

//_______________________________________________________________________________
void printComparisonTable(vector<TFile*> samples, vector<string> names, vector<string> selecs, vector<string> dirs, string caption = "", bool dataMCratio = false, int colorReduction = 0, int mt2bin = -1) {

  // read off yields from h_mt2bins hist in each topological region

  unsigned int n = samples.size();
  unsigned int ndirs = dirs.size();
  unsigned int nselecs = selecs.size();
  // float scale = 40/12.9;
  float scale = 1;

  if (nselecs < 2) cout << "Use printTable instead!\n";

  if (caption == "Detailed") { // temporary
    caption = dirs[0] + ": ";
    caption += getJetBJetTableLabel(samples.at(0), dirs[0]) + ", ";
    caption += getHTTableLabel(samples.at(0), dirs[0]) + ", ";
    caption += getMETTableLabel(samples.at(0), dirs[0]);
    caption += ", MT2 $>$ 200 GeV";
  }

  // vector<string> dirs_all = dirs;
  vector<vector<string>> dirsVec;
  int nTabulars = ndirs*nselecs/8;
  if ((ndirs*nselecs)%8 > 0) ++nTabulars;
  int ndir_line = (ndirs%nTabulars == 0)? ndirs/nTabulars : ndirs/nTabulars+1;
  auto it = dirs.begin();
  for (int i = 0; i < nTabulars; ++i) {
    vector<string> temp_dirs;
    for (int j = 0; j < ndir_line && it != dirs.end(); ++j, ++it)
      temp_dirs.push_back(*it);
    dirsVec.push_back(temp_dirs);
  }

  ofile << "\\begin{table}[H]" << std::endl;
  ofile << "\\scriptsize" << std::endl;
  ofile << "\\centering" << std::endl;
  ofile << "\\caption{" << caption << "}" << std::endl;

  for (int itab = 0; itab < nTabulars; ++itab) {
    dirs = dirsVec[itab];
    ndirs = dirs.size();

    vector<double> bgtot(ndirs*nselecs, 0);
    vector<double> bgerr(ndirs*nselecs, 0);
    double yield_base = 0;

    ofile << "\\begin{tabular}{r";
    for (unsigned int idir=0; idir < ndirs*nselecs; ++idir) ofile << "|c";
    ofile << "}" << std::endl;
    ofile << "\\hline\\hline" << endl;
    ofile << "\\multirow{2}{*}{Sample}";
    for (unsigned int idir = 0; idir < ndirs; ++idir)
      ofile << " & \\multicolumn{" << nselecs << "}{c" << ((idir == ndirs-1)? "" : "|") << "}{ "
            << getJetBJetTableLabel(samples.at(0), dirs.at(idir)) << ", $\\mttwo > 200$~GeV" << " }";
    ofile << " \\\\" << endl
          << "\\cline{2-" << ndirs*nselecs+1 << "}" << endl;
    for (unsigned int i = 0; i < ndirs; ++i)
      for (auto it = selecs.begin(); it != selecs.end(); ++it)
        ofile << " & " << *it;
    ofile << " \\\\" << endl;
    ofile << "\\hline\\hline" << endl;

    // backgrounds first -- loop backwards
    for ( int i = n-1 ; i >= 0 ; --i ) {
      if ( TString(names.at(i)).Contains("data") ) {found_data = true; continue;}
      if ( TString(names.at(i)).Contains("sig") ) continue;
      ofile << getTableName(names.at(i));
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins_%s", dirs.at(idir).c_str(), selecs.at(isel).c_str());
          TH1D* h = (TH1D*) samples.at(i)->Get(fullhistname);
          double yield = 0.;
          double err = 0.;
          int idx = idir*nselecs + isel;
          if (h) {
            // use all bins
            if (mt2bin < 0) {
              yield = h->IntegralAndError(0,-1,err);
              bgtot.at(idx) += yield;
              bgerr.at(idx) = sqrt(pow(bgerr.at(idx),2) + pow(err,2));
            }
            // last bin: include overflow
            else if (mt2bin == h->GetXaxis()->GetNbins()) {
              yield = h->IntegralAndError(mt2bin,-1,err);
              bgtot.at(idx) += yield;
              bgerr.at(idx) = sqrt(pow(bgerr.at(idx),2) + pow(err,2));
            }
            // single bin, not last bin
            else {
              yield = h->GetBinContent(mt2bin);
              err = h->GetBinError(mt2bin);
              bgtot.at(idx) += yield;
              bgerr.at(idx) = sqrt(pow(bgerr.at(idx),2) + pow(err,2));
            }
          }
          if (isel == 0) yield_base = yield;
          ofile << "  &  ";
          if (colorReduction && isel > 0 && yield_base/yield > colorReduction) {
            int colorFactor;
            if (yield == 0)
              colorFactor = 0;
            else if ((yield_base/yield)/(2*colorReduction) > 1)
              colorFactor = 25;
            else
              colorFactor = 30*(yield_base/yield)/(2*colorReduction);
            ofile << "\\cellcolor{cyan!" << colorFactor << "} ";
          }
          yield *= scale;
          err *= scale;
          if (yield > 10.)
            ofile << Form("%.1f $\\pm$ %.1f", yield, err);
          else
            ofile << Form("%.2f $\\pm$ %.2f", yield, err);
        }
      }
      ofile << " \\\\" << endl;
    } // loop over samples

    // print bg totals
    ofile << "\\hline" << endl;
    ofile << "Total SM";
    for ( unsigned int idx = 0; idx < bgtot.size(); ++idx ) {
      double yield = bgtot.at(idx);
      double err = bgerr.at(idx);
      ofile << "  &  ";

      if (colorReduction) {
        if (idx%nselecs == 0)
          yield_base = yield;
        else if (yield_base/yield > colorReduction) {
          int colorFactor;
          if (yield == 0)
            colorFactor = 0;
          else if ((yield_base/yield)/(2*colorReduction) > 1)
            colorFactor = 25;
          else
            colorFactor = 30*(1-(yield_base/yield)/(2*colorReduction));
          ofile << "\\cellcolor{cyan!" << colorFactor << "} ";
        }
      }
      yield *= scale;
      err *= scale;
      if (yield > 10.)
        ofile << Form("%.1f $\\pm$ %.1f", yield, err);
      else
        ofile << Form("%.2f $\\pm$ %.2f", yield, err);
    }
    ofile << " \\\\" << endl;
    ofile << "\\hline" << endl;

    // next print data, if it exists
    for( unsigned int i = 0 ; i < n ; i++ ){
      if( !TString(names.at(i)).Contains("data") ) continue;
      found_data = true;
      ofile << getTableName(names.at(i));
      vector<int> datatot;
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins_%s", dirs.at(idir).c_str(), selecs.at(isel).c_str());
          TH1D* h = (TH1D*) samples.at(i)->Get(fullhistname);
          double yield = 0.;
          if (h) {
            // use all bins
            if (mt2bin < 0) {
              yield = h->Integral(0,-1);
            }
            // last bin: include overflow
            else if (mt2bin == h->GetXaxis()->GetNbins()) {
              yield = h->Integral(mt2bin,-1);
            }
            // single bin, not last bin
            else {
              yield = h->GetBinContent(mt2bin);
            }
          }
          ofile << "  &  " << Form("%d",(int)yield);
          if (dataMCratio) datatot.push_back(yield);
          if (colorReduction) {
            if (isel == 0)
              yield_base = yield;
            else if (yield_base/yield > colorReduction) {
              int colorFactor;
              if (yield == 0)
                colorFactor = 0;
              else if ((yield_base/yield)/(2*colorReduction) > 1)
                colorFactor = 25;
              else
                colorFactor = 30*(1-(yield_base/yield)/(2*colorReduction));
              ofile << "\\cellcolor{cyan!" << colorFactor << "} ";
            }
          }
        }
      }
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;
      if (dataMCratio) {
        ofile << "data/MC";
        for (unsigned int idx = 0; idx < bgtot.size(); ++idx)
          ofile << "  &  " << setprecision(3) << datatot[idx]/bgtot[idx];
        ofile << " \\\\" << endl;
        ofile << "\\hline" << endl;
      }
    } // loop over samples

    // finally print signals
    for( unsigned int i = 0 ; i < n ; i++ ){
      if( !TString(names.at(i)).Contains("sig") ) continue;
      ofile << getTableName(names.at(i));
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins_%s", dirs.at(idir).c_str(), selecs.at(isel).c_str());
          TH1D* h = (TH1D*) samples.at(i)->Get(fullhistname);
          double yield = 0.;
          double err = 0.;
          if (h) {
            // use all bins
            if (mt2bin < 0) {
              yield = h->IntegralAndError(0,-1,err);
            }
            // last bin: include overflow
            else if (mt2bin == h->GetXaxis()->GetNbins()) {
              yield = h->IntegralAndError(mt2bin,-1,err);
            }
            // single bin, not last bin
            else {
              yield = h->GetBinContent(mt2bin);
              err = h->GetBinError(mt2bin);
            }
          }
          ofile << "  &  ";
          if (colorReduction) {
            if (isel == 0)
              yield_base = yield;
            else if (yield_base/yield > colorReduction) {
              int colorFactor;
              if (yield == 0)
                colorFactor = 0;
              else if ((yield_base/yield)/(2*colorReduction) > 1)
                colorFactor = 25;
              else
                colorFactor = 30*(1-(yield_base/yield)/(2*colorReduction));
              ofile << "\\cellcolor{cyan!" << colorFactor << "} ";
            }
          }
          if (yield > 10.)
            ofile << Form("%.1f $\\pm$ %.1f", yield, err);
          else
            ofile << Form("%.2f $\\pm$ %.2f", yield, err);
        }
      }
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;
    } // loop over samples

    if (itab == nTabulars-1) ofile << "\\hline" << std::endl;
    else ofile << "\\multicolumn{" << ndirs*nselecs+1 << "}{c}{} \\\\" << endl;
    ofile << "\\end{tabular}" << std::endl;
  }

  ofile << "\\end{table}" << std::endl;

  ofile << endl;
  return;
}

//_______________________________________________________________________________
void printDetailedComparisonTable(vector<TFile*> samples, vector<string> names, vector<string> selecs, vector<string> dirs, string caption = "", int colorReduction = 0) {

  // read off yields from h_mt2bins hist in each topological region
  string dir = dirs[0];

  const unsigned int n = samples.size();

  unsigned int ndirs = dirs.size();
  unsigned int nselecs = selecs.size();

  // vector<vector<string>> dirsVec;
  // int nTabulars = ndirs*nselecs/8;
  // if ((ndirs*nselecs)%8 > 0) ++nTabulars;
  // int ndir_line = (ndirs%nTabulars == 0)? ndirs/nTabulars : ndirs/nTabulars+1;
  // auto it = dirs.begin();
  // for (int i = 0; i < nTabulars; ++i) {
  //   vector<string> temp_dirs;
  //   for (int j = 0; j < ndir_line && it != dirs.end(); ++j, ++it)
  //     temp_dirs.push_back(*it);
  //   dirsVec.push_back(temp_dirs);
  // }
  if (caption == "") {
    caption += (string(dir)).replace(dir.size()-2, 1, " ") + ": ";
    caption += getJetBJetTableLabel(samples.at(0), dir) + ", ";
    caption += getHTTableLabel(samples.at(0), dir) + ", ";
    caption += getMETTableLabel(samples.at(0), dir);
  }

  ofile << "\\begin{table}[H]" << std::endl;
  // ofile << "\\caption{Background and signal yields in bins of \\mttwo for ";
  // ofile << getHTTableLabel(samples.at(0), dir) << ", ";
  // ofile << getJetBJetTableLabel(samples.at(0), dir) << ". ";
  // ofile << "The yields are normalized to \\lumival.}" << std::endl;
  ofile << "\\caption{" << caption << "}" << endl;
  ofile << "\\scriptsize" << std::endl;
  ofile << "\\centering" << std::endl;

  TString nbinshistname = Form("%s/h_n_mt2bins", dir.c_str());
  TH1D* h_n_mt2bins = (TH1D*) samples.at(0)->Get(nbinshistname);
  int n_mt2bins;
  if (h_n_mt2bins) {
    n_mt2bins = h_n_mt2bins->GetBinContent(1);
  } else {
    ofile << "Couldn't get number of mt2 bins" << std::endl;
    return;
  }

  int nTabulars = n_mt2bins; // temporarily

  for (int itab = 0; itab < nTabulars; ++itab) {
    // dirs = dirsVec[itab];
    // ndirs = dirs.size();
    // vector<double> bgtot(n_mt2bins,0.);
    // vector<double> bgerr(n_mt2bins,0.);
    vector<double> bgtot(nselecs, 0.);
    vector<double> bgerr(nselecs, 0.);

    TString binshistname = Form("%s/h_mt2bins",dir.c_str());
    TH1D* h_mt2bins(0);
    for(unsigned int i=0; i<samples.size(); i++){ //need to find a sample that has this hist filled
      h_mt2bins = (TH1D*) samples.at(i)->Get(binshistname);
      if (h_mt2bins) break;
    }

    if (!h_mt2bins) {
      h_mt2bins = (TH1D*) samples.at(0)->Get(Form("%s/h_mt2bins_original",dir.c_str()));
      if (!h_mt2bins) {
        ofile << "Couldn't get the mt2 binning" << std::endl;
        return;
      }
    }

    ofile << "\\begin{tabular}{r";
    for (int ibin=0; ibin < nselecs; ++ibin) ofile << "|c";
    ofile << "}" << std::endl;
    ofile << "\\hline\\hline" << std::endl;
    ofile << "\\multirow{2}{*}{Sample}";
    // for (unsigned int idir = 0; idir < ndirs; ++idir)
    // ofile << " & \\multicolumn{" << nselecs << "}{c" << ((idir == ndirs-1)? "" : "|") << "}{ " << getJetBJetTableLabel(samples.at(0), dirs.at(idir)) << " }";
    ofile << " & \\multicolumn{" << nselecs << "}{c}{";
    ofile << getJetBJetTableLabel(samples.at(0), dirs.at(0)) << ", ";
    int ibin = itab+1; // temporary
    if (ibin == n_mt2bins)
      ofile << "\\mttwo $>$ " << h_mt2bins->GetXaxis()->GetBinLowEdge(n_mt2bins) << " GeV";
    else
      ofile << h_mt2bins->GetXaxis()->GetBinLowEdge(ibin) << " $<$ \\mttwo $<$ " << h_mt2bins->GetXaxis()->GetBinLowEdge(ibin+1) << " GeV";
    // ofile << "\\multicolumn{" << n_mt2bins+1 << "}{c}{";
    ofile << "} \\\\" << std::endl;
    ofile << "\\cline{2-" << ndirs*nselecs+1 << "}" << endl;
    for (unsigned int i = 0; i < ndirs; ++i)
      for (auto it = selecs.begin(); it != selecs.end(); ++it)
        ofile << " & " << *it;
    ofile << " \\\\" << endl;
    ofile << "\\hline\\hline" << endl;


    // backgrounds first -- loop backwards
    for( int isamp = 0 ; isamp < n ; ++isamp ){
      if( TString(names.at(isamp)).Contains("data")  ) continue;
      if( TString(names.at(isamp)).Contains("sig")  ) continue;
      ofile << getTableName(names.at(isamp));
      // for (int ibin = 1; ibin <= n_mt2bins; ++ibin) {
      for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
        TString fullhistname = Form("%s/h_mt2bins_%s", dir.c_str(), selecs.at(isel).c_str());
        if (names.at(isamp)=="gjets" || names.at(isamp)=="gjetsqcd") fullhistname.ReplaceAll("sr","crgj");
        if (names.at(isamp)=="dyjets") fullhistname.ReplaceAll("sr","crdy");
        TH1D* h = (TH1D*) samples.at(isamp)->Get(fullhistname);
        double yield = 0.;
        double err = 0.;
        int idx = isel;
        if (h) {
          // not last bin
          if(ibin != n_mt2bins) {
            yield = h->GetBinContent(ibin);
            err = h->GetBinError(ibin);
            bgtot.at(idx) += yield;
            bgerr.at(idx) = sqrt(pow(bgerr.at(idx),2) + pow(err,2));
          }
          // last bin: include overflow
          else if (ibin == h->GetXaxis()->GetNbins()) {
            yield = h->IntegralAndError(ibin,-1,err);
            bgtot.at(idx) += yield;
            bgerr.at(idx) = sqrt(pow(bgerr.at(idx),2) + pow(err,2));
          }
          else {
            ofile << "Shouldn't get here" << std::endl;
            ofile << "n_mt2bins: " << n_mt2bins << " h->XaxisNbins: " << h->GetXaxis()->GetNbins() << endl;
            return;
          }
        }
        if (yield > 10.)
          ofile << "  &  " << Form("%.1f $\\pm$ %.1f", yield, err);
        else
          ofile << "  &  " << Form("%.2f $\\pm$ %.2f", yield, err);
      }
      ofile << " \\\\" << endl;
    } // loop over samples

    // print bg totals
    ofile << "\\hline" << endl;
    ofile << "Total SM";
    for ( unsigned int idx = 0; idx < bgtot.size(); ++idx ) {
      double yield = bgtot.at(idx);
      double err = bgerr.at(idx);
      if (yield > 10.)
        ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
      else
        ofile << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
    }
    ofile << " \\\\" << endl;
    ofile << "\\hline" << endl;

    for( unsigned int i = 0 ; i < n ; i++ ){
      if( !TString(names.at(i)).Contains("data") ) continue;
      ofile << getTableName(names.at(i));
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins_%s", dirs.at(idir).c_str(), selecs.at(isel).c_str());
          TH1D* h = (TH1D*) samples.at(i)->Get(fullhistname);
          double yield = 0.;
          if (h) {
            // use all bins
            if(ibin != n_mt2bins) {
              yield = h->GetBinContent(ibin);
            }
            // last bin: include overflow
            else if (ibin == h->GetXaxis()->GetNbins()) {
              yield = h->Integral(ibin, -1);
            }
            else {
              ofile << "Shouldn't get here" << std::endl;
              return;
            }
          }
          ofile << "  &  " << Form("%d",(int)yield);
        }
      }
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;
    }

    // for( unsigned int jsamp = 0 ; jsamp < n ; jsamp++ ){
    //   if( !TString(names.at(jsamp)).Contains("sig") ) continue;
    //   ofile << getTableName(names.at(jsamp));
    //   for (int ibin = 1; ibin <= n_mt2bins; ++ibin) {
    //     TString fullhistname = Form("%s/h_mt2bins",dir.c_str());
    //     TH1D* h = (TH1D*) samples.at(jsamp)->Get(fullhistname);
    //     double yield = 0.;
    //     double err = 0.;
    //     if (h) {
    //       // not last bin
    //       if(ibin != n_mt2bins) {
    //         yield = h->GetBinContent(ibin);
    //         err = h->GetBinError(ibin);
    //       }
    //       // last bin: include overflow
    //       else if (ibin == h->GetXaxis()->GetNbins()) {
    //         yield = h->IntegralAndError(ibin,-1,err);
    //       }
    //       else {
    //         ofile << "Shouldn't get here" << std::endl;
    //         return;
    //       }
    //     }
    //     if (yield > 10.) {
    //       //  	ofile << "  &  " << Form("%.0f $\\pm$ %.0f",yield,err);
    //       ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
    //     } else {
    //       //  	ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
    //       ofile << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
    //     }
    //   }
    //   ofile << " \\\\" << endl;
    // } // loop over samples

    if (itab == nTabulars-1) ofile << "\\hline" << std::endl;
    else ofile << "\\multicolumn{" << ndirs*nselecs+1 << "}{c}{} \\\\" << endl;
    ofile << "\\end{tabular}" << std::endl;
  }

  ofile << "\\end{table}" << std::endl;
  //ofile << "\\pagebreak" << std::endl;
  ofile << std::endl;

  return;
}

//_______________________________________________________________________________
void printComparisonTableCR(vector<TFile*> samples, vector<string> names, vector<string> selecs, vector<string> dirs, string caption = "", bool dataMCratio = false, int colorReduction = 0, int mt2bin = -1) {

  // read off yields from h_mt2bins hist in each topological region
  unsigned int n = samples.size();
  unsigned int ndirs = dirs.size();
  unsigned int nselecs = selecs.size();

  if (nselecs < 2) cout << "Use printTable instead!\n";

  string crname;
  if (TString(dirs.at(0)).Contains("crhsl") || TString(dirs.at(0)).Contains("crsl"))
    crname = "CRSL";
  else if (TString(dirs.at(0)).Contains("crhgj") || TString(dirs.at(0)).Contains("crgj"))
    crname = "CRGJ";
  else if (TString(dirs.at(0)).Contains("crhqcd") || TString(dirs.at(0)).Contains("crqcd"))
    crname = "CRQCD";

  if (caption == "Detailed") { // temporary
    caption = dirs[0] + ": ";
    if (crname == "CRSL") caption += "[Single Lepton] ";
    else if (crname == "CRGJ") caption += "[Gamma + Jets] ";
    else if (crname == "CRQCD") caption += "[QCD] ";
    // caption += getJetBJetTableLabel(samples.at(0), dirs[0]) + ", ";
    caption += getHTTableLabel(samples.at(0), dirs[0]) + ", ";
    caption += getMETTableLabel(samples.at(0), dirs[0]);
    caption += ", MT2 $>$ 200 GeV";
  }

  // vector<string> dirs_all = dirs;
  vector<vector<string>> dirsVec;
  int nTabulars = ndirs*nselecs/8;
  if ((ndirs*nselecs)%8 > 0) ++nTabulars;
  int ndir_line = (ndirs%nTabulars == 0)? ndirs/nTabulars : ndirs/nTabulars+1;
  auto it = dirs.begin();
  for (int i = 0; i < nTabulars; ++i) {
    vector<string> temp_dirs;
    for (int j = 0; j < ndir_line && it != dirs.end(); ++j, ++it)
      temp_dirs.push_back(*it);
    dirsVec.push_back(temp_dirs);
  }

  ofile << "\\begin{table}[H]" << std::endl;
  ofile << "\\scriptsize" << std::endl;
  ofile << "\\centering" << std::endl;
  ofile << "\\caption{" << caption << "}" << std::endl;

  for (int itab = 0; itab < nTabulars; ++itab) {
    dirs = dirsVec[itab];
    ndirs = dirs.size();

    vector<double> bgtot_cr(ndirs*nselecs, 0);
    vector<double> bgerr_cr(ndirs*nselecs, 0);
    vector<double> bgtot_sr(ndirs*nselecs, 0);
    vector<double> bgerr_sr(ndirs*nselecs, 0);

    ofile << "\\begin{tabular}{r";
    for (unsigned int idir=0; idir < ndirs*nselecs; ++idir) ofile << "|c";
    ofile << "}" << std::endl;
    ofile << "\\hline\\hline" << endl;
    ofile << "\\multirow{2}{*}{Sample}";
    for (unsigned int idir = 0; idir < ndirs; ++idir)
      ofile << " & \\multicolumn{" << nselecs << "}{c" << ((idir == ndirs-1)? "" : "|") << "}{ "
            // << " SR: " << getJetBJetTableLabel(samples.at(0), string(dirs.at(idir)).replace(0,5,"srh")) << ", 0 lep. "
            << crname << ": " << ((crname == "CRQCD")? "Inclusive $\\geq$ 4j, $\\geq$ 2b" : getJetBJetTableLabel(samples.at(0), dirs.at(idir)))
            << ((crname == "CRSL")? ", 1 lep ($M_{\\rm{T}} < 100$)." : "")
            << " }";
    ofile << " \\\\" << endl
          << "\\cline{2-" << ndirs*nselecs+1 << "}" << endl;
    for (unsigned int i = 0; i < ndirs; ++i)
      for (auto it = selecs.begin(); it != selecs.end(); ++it)
        ofile << " & " << *it;
    ofile << " \\\\" << endl;
    ofile << "\\hline\\hline" << endl;

    for ( unsigned int idir = 0; idir < ndirs; ++idir ) {

      // backgrounds first -- loop backwards
      for ( int i = n-1 ; i >= 0 ; --i ) {
        if ( TString(names.at(i)).Contains("data") ) {found_data = true; continue;}
        if ( TString(names.at(i)).Contains("sig") ) continue;
        // ofile << getTableName(names.at(i));
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistnameCR = Form("%s/h_mt2bins_%s", dirs.at(idir).c_str(), selecs.at(isel).c_str());
          TString fullhistnameSR = Form("%s/h_mt2bins_%s", string(dirs.at(idir)).replace(0,5,"srh").c_str(), selecs.at(isel).c_str());
          TH1D* h_cr = (TH1D*) samples.at(i)->Get(fullhistnameCR);
          TH1D* h_sr = (TH1D*) samples.at(i)->Get(fullhistnameSR);
          double yield_cr = 0.;
          double err_cr = 0.;
          double yield_sr = 0.;
          double err_sr = 0.;
          int idx = idir*nselecs + isel;
          if (h_cr) {
            // use all bins
            yield_cr = h_cr->IntegralAndError(0,-1,err_cr);
            bgtot_cr.at(idx) += yield_cr;
            bgerr_cr.at(idx) = sqrt(pow(bgerr_cr.at(idx),2) + pow(err_cr,2));
          }
          if (h_sr) {
            // use all bins
            yield_sr = h_sr->IntegralAndError(0,-1,err_sr);
            bgtot_sr.at(idx) += yield_sr;
            bgerr_sr.at(idx) = sqrt(pow(bgerr_sr.at(idx),2) + pow(err_sr,2));
          }
        }
      } // loop over samples

      // print bg_sr totals
      // ofile << "LostLep SR";
      // for ( unsigned int idx = 0; idx < bgtot_sr.size(); ++idx ) {
      //   double yield = bgtot_sr.at(idx);
      //   double err = bgerr_sr.at(idx);
      //   ofile << "  &  ";
      //   if (yield > 10.)
      //     ofile << Form("%.1f $\\pm$ %.1f", yield, err);
      //   else
      //     ofile << Form("%.2f $\\pm$ %.2f", yield, err);
      // }
      // ofile << " \\\\" << endl;
      // ofile << "\\hline" << endl;

      // print bg_cr totals
      // ofile << "LostLep";
      if (crname == "CRSL")
        ofile << "LostLep";
      else if (crname == "CRGJ")
        ofile << "$\\gamma$+Jets";
      else if (crname == "CRQCD")
        ofile << "QCD";

      for ( unsigned int idx = 0; idx < bgtot_cr.size(); ++idx ) {
        double yield = bgtot_cr.at(idx);
        double err = bgerr_cr.at(idx);
        ofile << "  &  ";
        if (yield > 10.)
          ofile << Form("%.1f $\\pm$ %.1f", yield, err);
        else
          ofile << Form("%.2f $\\pm$ %.2f", yield, err);
      }
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;

      // next print data, if it exists
      vector<int> datatot;
      for( unsigned int i = 0 ; i < n ; i++ ){
        if( !TString(names.at(i)).Contains("data") ) continue;
        found_data = true;
        ofile << "Data";
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins_%s", dirs.at(idir).c_str(), selecs.at(isel).c_str());
          TH1D* h = (TH1D*) samples.at(i)->Get(fullhistname);
          double yield = 0.;
          if (h) {
            // use all bins
            yield = h->Integral(0,-1);
          }
          ofile << "  &  " << Form("%d",(int)yield);
          if (dataMCratio) datatot.push_back(yield);
        }
        ofile << " \\\\" << endl;
        ofile << "\\hline" << endl;
      } // loop over samples

      // print the data/MC and transfer factor ratios
      if (dataMCratio) {
        // ofile << "\\hline" << endl;
        // ofile << "TF (SR/CR)";
        // for (unsigned int idx = 0; idx < bgtot_cr.size(); ++idx)
        //   ofile << "  &  " << ((bgtot_sr[idx] > bgtot_cr[idx])? setprecision(3) : setprecision(2) )<< bgtot_sr[idx]/bgtot_cr[idx];
        // ofile << " \\\\" << endl;
        // ofile << "\\hline" << endl;

        ofile << "Data/MC";
        for (unsigned int idx = 0; idx < bgtot_cr.size(); ++idx)
          ofile << "  &  " << ((datatot[idx] > bgtot_cr[idx])? setprecision(3) : setprecision(2)) << datatot[idx]/bgtot_cr[idx];
        ofile << " \\\\" << endl;
        ofile << "\\hline" << endl;

      }
    }

    if (itab == nTabulars-1) ofile << "\\hline" << std::endl;
    else ofile << "\\multicolumn{" << ndirs*nselecs+1 << "}{c}{} \\\\" << endl;
    ofile << "\\end{tabular}" << std::endl;
  }

  ofile << "\\end{table}" << std::endl;

  ofile << endl;
  return;
}

//_______________________________________________________________________________
void printComparisonTable(vector<vector<TFile*>> samplesVec, vector<string> names, vector<string> selecs, vector<string> dirs, string caption = "", bool dataMCratio = false, int colorReduction = 0, int mt2bin = -1) {

  // read off yields from h_mt2bins hist in each topological region
  vector<TFile*> samples = samplesVec[0];

  unsigned int n = samples.size();
  unsigned int ndirs = dirs.size();
  unsigned int nselecs = selecs.size();

  if (nselecs < 2) cout << "Use printTable instead!\n";

  if (caption == "Detailed") { // temporary
    caption = dirs[0] + ": ";
    caption += getJetBJetTableLabel(samples.at(0), dirs[0]) + ", ";
    caption += getHTTableLabel(samples.at(0), dirs[0]) + ", ";
    caption += getMETTableLabel(samples.at(0), dirs[0]);
    caption += ", MT2 $>$ 200 GeV";
  }

  // vector<string> dirs_all = dirs;
  vector<vector<string>> dirsVec;
  int nTabulars = ndirs*nselecs/8;
  if ((ndirs*nselecs)%8 > 0) ++nTabulars;
  int ndir_line = (ndirs%nTabulars == 0)? ndirs/nTabulars : ndirs/nTabulars+1;
  auto it = dirs.begin();
  for (int i = 0; i < nTabulars; ++i) {
    vector<string> temp_dirs;
    for (int j = 0; j < ndir_line && it != dirs.end(); ++j, ++it)
      temp_dirs.push_back(*it);
    dirsVec.push_back(temp_dirs);
  }

  ofile << "\\begin{table}[H]" << std::endl;
  ofile << "\\scriptsize" << std::endl;
  ofile << "\\centering" << std::endl;
  ofile << "\\caption{" << caption << "}" << std::endl;

  for (int itab = 0; itab < nTabulars; ++itab) {
    dirs = dirsVec[itab];
    ndirs = dirs.size();

    vector<double> bgtot(ndirs*nselecs, 0);
    vector<double> bgerr(ndirs*nselecs, 0);
    double yield_base = 0;

    ofile << "\\begin{tabular}{r";
    for (unsigned int idir=0; idir < ndirs*nselecs; ++idir) ofile << "|c";
    ofile << "}" << std::endl;
    ofile << "\\hline\\hline" << endl;
    ofile << "\\multirow{2}{*}{Sample}";
    for (unsigned int idir = 0; idir < ndirs; ++idir)
      ofile << " & \\multicolumn{" << nselecs << "}{c" << ((idir == ndirs-1)? "" : "|") << "}{ "
            << getJetBJetTableLabel(samples.at(0), dirs.at(idir)) << ", $\\mttwo > 200$~GeV" << " }";
    ofile << " \\\\" << endl
          << "\\cline{2-" << ndirs*nselecs+1 << "}" << endl;
    for (unsigned int i = 0; i < ndirs; ++i)
      for (auto it = selecs.begin(); it != selecs.end(); ++it)
        ofile << " & " << *it;
    ofile << " \\\\" << endl;
    ofile << "\\hline\\hline" << endl;

    // backgrounds first -- loop backwards
    for ( int i = n-1 ; i >= 0 ; --i ) {
      if ( TString(names.at(i)).Contains("data") ) {found_data = true; continue;}
      if ( TString(names.at(i)).Contains("sig") ) continue;
      ofile << getTableName(names.at(i));
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins", dirs.at(idir).c_str());
          TH1D* h = (TH1D*) samplesVec[isel].at(i)->Get(fullhistname);
          double yield = 0.;
          double err = 0.;
          int idx = idir*nselecs + isel;
          if (h) {
            // use all bins
            if (mt2bin < 0) {
              yield = h->IntegralAndError(0,-1,err);
              bgtot.at(idx) += yield;
              bgerr.at(idx) = sqrt(pow(bgerr.at(idx),2) + pow(err,2));
            }
            // last bin: include overflow
            else if (mt2bin == h->GetXaxis()->GetNbins()) {
              yield = h->IntegralAndError(mt2bin,-1,err);
              bgtot.at(idx) += yield;
              bgerr.at(idx) = sqrt(pow(bgerr.at(idx),2) + pow(err,2));
            }
            // single bin, not last bin
            else {
              yield = h->GetBinContent(mt2bin);
              err = h->GetBinError(mt2bin);
              bgtot.at(idx) += yield;
              bgerr.at(idx) = sqrt(pow(bgerr.at(idx),2) + pow(err,2));
            }
          }
          if (isel == 0) yield_base = yield;
          ofile << "  &  ";
          if (colorReduction && isel > 0 && yield_base/yield > colorReduction) {
            int colorFactor;
            if (yield == 0)
              colorFactor = 0;
            else if ((yield_base/yield)/(2*colorReduction) > 1)
              colorFactor = 25;
            else
              colorFactor = 30*(yield_base/yield)/(2*colorReduction);
            ofile << "\\cellcolor{cyan!" << colorFactor << "} ";
          }
          if (yield > 10.)
            ofile << Form("%.1f $\\pm$ %.1f", yield, err);
          else
            ofile << Form("%.2f $\\pm$ %.2f", yield, err);
        }
      }
      ofile << " \\\\" << endl;
    } // loop over samples

    // print bg totals
    ofile << "\\hline" << endl;
    ofile << "Total SM";
    for ( unsigned int idx = 0; idx < bgtot.size(); ++idx ) {
      double yield = bgtot.at(idx);
      double err = bgerr.at(idx);
      ofile << "  &  ";

      if (colorReduction) {
        if (idx%nselecs == 0)
          yield_base = yield;
        else if (yield_base/yield > colorReduction) {
          int colorFactor;
          if (yield == 0)
            colorFactor = 0;
          else if ((yield_base/yield)/(2*colorReduction) > 1)
            colorFactor = 25;
          else
            colorFactor = 30*(1-(yield_base/yield)/(2*colorReduction));
          ofile << "\\cellcolor{cyan!" << colorFactor << "} ";
        }
      }
      if (yield > 10.)
        ofile << Form("%.1f $\\pm$ %.1f", yield, err);
      else
        ofile << Form("%.2f $\\pm$ %.2f", yield, err);
    }
    ofile << " \\\\" << endl;
    ofile << "\\hline" << endl;

    // next print data, if it exists
    for( unsigned int i = 0 ; i < n ; i++ ){
      if( !TString(names.at(i)).Contains("data") ) continue;
      found_data = true;
      ofile << getTableName(names.at(i));
      vector<int> datatot;
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins", dirs.at(idir).c_str());
          TH1D* h = (TH1D*) samplesVec[isel].at(i)->Get(fullhistname);
          double yield = 0.;
          if (h) {
            // use all bins
            if (mt2bin < 0) {
              yield = h->Integral(0,-1);
            }
            // last bin: include overflow
            else if (mt2bin == h->GetXaxis()->GetNbins()) {
              yield = h->Integral(mt2bin,-1);
            }
            // single bin, not last bin
            else {
              yield = h->GetBinContent(mt2bin);
            }
          }
          ofile << "  &  " << Form("%d",(int)yield);
          if (dataMCratio) datatot.push_back(yield);
          if (colorReduction) {
            if (isel == 0)
              yield_base = yield;
            else if (yield_base/yield > colorReduction) {
              int colorFactor;
              if (yield == 0)
                colorFactor = 0;
              else if ((yield_base/yield)/(2*colorReduction) > 1)
                colorFactor = 25;
              else
                colorFactor = 30*(1-(yield_base/yield)/(2*colorReduction));
              ofile << "\\cellcolor{cyan!" << colorFactor << "} ";
            }
          }
        }
      }
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;
      if (dataMCratio) {
        ofile << "data/MC";
        for (unsigned int idx = 0; idx < bgtot.size(); ++idx)
          ofile << "  &  " << setprecision(3) << datatot[idx]/bgtot[idx];
        ofile << " \\\\" << endl;
        ofile << "\\hline" << endl;
      }
    } // loop over samples

    // finally print signals
    for( unsigned int i = 0 ; i < n ; i++ ){
      if( !TString(names.at(i)).Contains("sig") ) continue;
      ofile << getTableName(names.at(i));
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins",dirs.at(idir).c_str());
          TH1D* h = (TH1D*) samplesVec[isel].at(i)->Get(fullhistname);
          double yield = 0.;
          double err = 0.;
          if (h) {
            // use all bins
            if (mt2bin < 0) {
              yield = h->IntegralAndError(0,-1,err);
            }
            // last bin: include overflow
            else if (mt2bin == h->GetXaxis()->GetNbins()) {
              yield = h->IntegralAndError(mt2bin,-1,err);
            }
            // single bin, not last bin
            else {
              yield = h->GetBinContent(mt2bin);
              err = h->GetBinError(mt2bin);
            }
          }
          ofile << "  &  ";
          if (colorReduction) {
            if (isel == 0)
              yield_base = yield;
            else if (yield_base/yield > colorReduction) {
              int colorFactor;
              if (yield == 0)
                colorFactor = 0;
              else if ((yield_base/yield)/(2*colorReduction) > 1)
                colorFactor = 25;
              else
                colorFactor = 30*(1-(yield_base/yield)/(2*colorReduction));
              ofile << "\\cellcolor{cyan!" << colorFactor << "} ";
            }
          }
          if (yield > 10.)
            ofile << Form("%.1f $\\pm$ %.1f", yield, err);
          else
            ofile << Form("%.2f $\\pm$ %.2f", yield, err);
        }
      }
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;
    } // loop over samples

    if (itab == nTabulars-1) ofile << "\\hline" << std::endl;
    else ofile << "\\multicolumn{" << ndirs*nselecs+1 << "}{c}{} \\\\" << endl;
    ofile << "\\end{tabular}" << std::endl;
  }

  ofile << "\\end{table}" << std::endl;

  ofile << endl;
  return;
}

void printComparisonTableCRSL(vector<TFile*> samples, vector<string> names, vector<string> selecs, vector<string> dirs, string caption = "", bool dataMCratio = false, int colorReduction = 0, int mt2bin = -1) {

  // read off yields from h_mt2bins hist in each topological region

  unsigned int n = samples.size();
  unsigned int ndirs = dirs.size();
  unsigned int nselecs = selecs.size();

  string sr = "sr";
  string cr = "crsl";

  if (nselecs < 2) cout << "Use printTable instead!\n";

  if (caption == "Detailed") { // temporary
    caption = dirs[0] + ": ";
    // caption += getJetBJetTableLabel(samples.at(0), dirs[0]) + ", ";
    caption += getHTTableLabel(samples.at(0), dirs[0]) + ", ";
    caption += getMETTableLabel(samples.at(0), dirs[0]);
    caption += ", MT2 $>$ 200 GeV";
  }

  // vector<string> dirs_all = dirs;
  vector<vector<string>> dirsVec;
  int nTabulars = ndirs*nselecs/8;
  if ((ndirs*nselecs)%8 > 0) ++nTabulars;
  int ndir_line = (ndirs%nTabulars == 0)? ndirs/nTabulars : ndirs/nTabulars+1;
  auto it = dirs.begin();
  for (int i = 0; i < nTabulars; ++i) {
    vector<string> temp_dirs;
    for (int j = 0; j < ndir_line && it != dirs.end(); ++j, ++it)
      temp_dirs.push_back(*it);
    dirsVec.push_back(temp_dirs);
  }

  ofile << "\\begin{table}[H]" << std::endl;
  ofile << "\\scriptsize" << std::endl;
  ofile << "\\centering" << std::endl;
  ofile << "\\caption{" << caption << "}" << std::endl;

  for (int itab = 0; itab < nTabulars; ++itab) {
    dirs = dirsVec[itab];
    ndirs = dirs.size();

    vector<double> bgtot_cr(ndirs*nselecs, 0);
    vector<double> bgerr_cr(ndirs*nselecs, 0);
    vector<double> bgtot_sr(ndirs*nselecs, 0);
    vector<double> bgerr_sr(ndirs*nselecs, 0);

    ofile << "\\begin{tabular}{r";
    for (unsigned int idir=0; idir < ndirs*nselecs; ++idir) ofile << "|c";
    ofile << "}" << std::endl;
    ofile << "\\hline\\hline" << endl;
    // ofile << "\\multirow{2}{*}{Sample}";
    // for (unsigned int idir = 0; idir < ndirs; ++idir)
    //   ofile << " & \\multicolumn{" << nselecs << "}{c" << ((idir == ndirs-1)? "" : "|") << "}{ "
    //         // << " SR: " << getJetBJetTableLabel(samples.at(0), string(dirs.at(idir)).replace(0,5,"srh")) << ", 0 lep. "
    //         // << " CR: " << getJetBJetTableLabel(samples.at(0), dirs.at(idir)) << ", 1 lep ($M_{\\rm{T}} < 100$)."
    //         << " }";
    // ofile << " \\\\" << endl
    //       << "\\cline{2-" << ndirs*nselecs+1 << "}" << endl;
    ofile << "Sample";
    for (unsigned int idir = 0; idir < ndirs; ++idir)
      ofile << "  &  " << dirs.at(idir);
    // for (unsigned int i = 0; i < ndirs; ++i)
    //   for (auto it = selecs.begin(); it != selecs.end(); ++it)
    //     ofile << " & " << *it;
    ofile << " \\\\" << endl;
    ofile << "\\hline\\hline" << endl;

    // backgrounds first -- loop backwards
    for ( int i = n-1 ; i >= 0 ; --i ) {
      if ( TString(names.at(i)).Contains("data") ) {found_data = true; continue;}
      if ( TString(names.at(i)).Contains("sig") ) continue;
      if ( !TString(names.at(i)).Contains("lostlepFromCRs") ) continue;
      // if ( !TString(names.at(i)).Contains("lostlep") ) continue;
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistnameCR = Form("%s/h_lostlepMC_cr", (sr+dirs.at(idir)).c_str());
          TString fullhistnameSR = Form("%s/h_lostlepMC_sr", (sr+dirs.at(idir)).c_str());
          // TString fullhistnameCR = Form("%s/h_mt2binsCRyield", (sr+dirs.at(idir)).c_str());
          // TString fullhistnameSR = Form("%s/h_mt2binsMCyield", (sr+dirs.at(idir)).c_str());
          TH1D* h_cr = (TH1D*) samples.at(i)->Get(fullhistnameCR);
          TH1D* h_sr = (TH1D*) samples.at(i)->Get(fullhistnameSR);
          double yield_cr = 0.;
          double err_cr = 0.;
          double yield_sr = 0.;
          double err_sr = 0.;
          int idx = idir*nselecs + isel;
          if (h_cr) {
            // use all bins
            yield_cr = h_cr->IntegralAndError(0,-1,err_cr);
            bgtot_cr.at(idx) += yield_cr;
            bgerr_cr.at(idx) = sqrt(pow(bgerr_cr.at(idx),2) + pow(err_cr,2));
          }
          if (h_sr) {
            // use all bins
            yield_sr = h_sr->IntegralAndError(0,-1,err_sr);
            bgtot_sr.at(idx) += yield_sr;
            bgerr_sr.at(idx) = sqrt(pow(bgerr_sr.at(idx),2) + pow(err_sr,2));
          }
        }
      }
    } // loop over samples

    // print bg_sr totals
    ofile << "LostLep SR";
    for ( unsigned int idx = 0; idx < bgtot_sr.size(); ++idx ) {
      double yield = bgtot_sr.at(idx);
      double err = bgerr_sr.at(idx);
      ofile << "  &  ";
      if (yield > 10.)
        ofile << Form("%.1f $\\pm$ %.1f", yield, err);
      else
        ofile << Form("%.2f $\\pm$ %.2f", yield, err);
    }
    ofile << " \\\\" << endl;
    ofile << "\\hline" << endl;

    // print bg_cr totals
    ofile << "LostLep CR";
    for ( unsigned int idx = 0; idx < bgtot_cr.size(); ++idx ) {
      double yield = bgtot_cr.at(idx);
      double err = bgerr_cr.at(idx);
      ofile << "  &  ";
      if (yield > 10.)
        ofile << Form("%.1f $\\pm$ %.1f", yield, err);
      else
        ofile << Form("%.2f $\\pm$ %.2f", yield, err);
    }
    ofile << " \\\\" << endl;
    ofile << "\\hline" << endl;

    // next print data, if it exists
    vector<int> datatot;
    for( unsigned int i = 0 ; i < n ; i++ ){
      // if( !TString(names.at(i)).Contains("data") ) continue;
      if (!TString(names.at(i)).Contains("lostlepFromCRs")) continue;
      found_data = true;
      ofile << "Data CR";
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2binsCRyield", (sr+dirs.at(idir)).c_str());
          TH1D* h = (TH1D*) samples.at(i)->Get(fullhistname);
          double yield = 0.;
          if (h) {
            // use all bins
            yield = round(h->Integral(0,-1)); // deal with weird rounding errors
          }
          ofile << "  &  " << Form("%d",(int)yield);
          if (dataMCratio) datatot.push_back(yield);
        }
      }
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;
    } // loop over samples

    // print the data/MC and transfer factor ratios
    if (dataMCratio) {
      ofile << "\\hline" << endl;
      ofile << "TF (SR/CR)";
      for (unsigned int idx = 0; idx < bgtot_cr.size(); ++idx)
        ofile << "  &  " << ((bgtot_sr[idx] > bgtot_cr[idx])? setprecision(3) : setprecision(2) )<< bgtot_sr[idx]/bgtot_cr[idx];
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;

      ofile << "Data/MC";
      for (unsigned int idx = 0; idx < bgtot_cr.size(); ++idx)
        ofile << "  &  " << ((datatot[idx] > bgtot_cr[idx])? setprecision(3) : setprecision(2)) << datatot[idx]/bgtot_cr[idx];
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;

    }

    for( unsigned int i = 0 ; i < n ; i++ ){
      if (!TString(names.at(i)).Contains("lostlepFromCRs") &&
          !TString(names.at(i)).Contains("zinvDataDriven")) continue;
      ofile << "\\hline" << endl;
      ofile << "DD Estimates";
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins", (sr+dirs.at(idir)).c_str());
          TH1D* h = (TH1D*) samples.at(i)->Get(fullhistname);
          double yield = 0.;
          double err = 0.;
          if (h) {
            yield = h->IntegralAndError(0, -1, err);
          }
          ofile << "  &  ";
          if (yield > 10.)
            ofile << Form("%.1f $\\pm$ %.1f", yield, err);
          else
            ofile << Form("%.2f $\\pm$ %.2f", yield, err);
        }
      }
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;
    } // loop over samples

    if (itab == nTabulars-1) ofile << "\\hline" << std::endl;
    else ofile << "\\multicolumn{" << ndirs*nselecs+1 << "}{c}{} \\\\" << endl;
    ofile << "\\end{tabular}" << std::endl;
  }

  ofile << "\\end{table}" << std::endl;

  ofile << endl;
  return;
}

/*
void printComparisonTableCRGJ(TFile* f_zinvdd, vector<string> dirs, string caption = "", bool dataMCratio = false, int colorReduction = 0, int mt2bin = -1) {

  // read off yields from h_mt2bins hist in each topological region

  unsigned int ndirs = dirs.size();

  if (nselecs < 2) cout << "Use printTable instead!\n";

  if (caption == "Detailed") { // temporary
    caption = dirs[0] + ": ";
    // caption += getJetBJetTableLabel(samples.at(0), dirs[0]) + ", ";
    caption += getHTTableLabel(samples.at(0), dirs[0]) + ", ";
    caption += getMETTableLabel(samples.at(0), dirs[0]);
    caption += ", MT2 $>$ 200 GeV";
  }

  // vector<string> dirs_all = dirs;
  vector<vector<string>> dirsVec;
  int nTabulars = ndirs*nselecs/8;
  if ((ndirs*nselecs)%8 > 0) ++nTabulars;
  int ndir_line = (ndirs%nTabulars == 0)? ndirs/nTabulars : ndirs/nTabulars+1;
  auto it = dirs.begin();
  for (int i = 0; i < nTabulars; ++i) {
    vector<string> temp_dirs;
    for (int j = 0; j < ndir_line && it != dirs.end(); ++j, ++it)
      temp_dirs.push_back(*it);
    dirsVec.push_back(temp_dirs);
  }

  ofile << "\\begin{table}[H]" << std::endl;
  ofile << "\\scriptsize" << std::endl;
  ofile << "\\centering" << std::endl;
  ofile << "\\caption{" << caption << "}" << std::endl;

  for (int itab = 0; itab < nTabulars; ++itab) {
    dirs = dirsVec[itab];
    ndirs = dirs.size();

    vector<double> bgtot_cr(ndirs*nselecs, 0);
    vector<double> bgerr_cr(ndirs*nselecs, 0);
    vector<double> bgtot_sr(ndirs*nselecs, 0);
    vector<double> bgerr_sr(ndirs*nselecs, 0);

    ofile << "\\begin{tabular}{r";
    for (unsigned int idir=0; idir < ndirs*nselecs; ++idir) ofile << "|c";
    ofile << "}" << std::endl;
    ofile << "\\hline\\hline" << endl;
    // ofile << "\\multirow{2}{*}{Sample}";
    // for (unsigned int idir = 0; idir < ndirs; ++idir)
    //   ofile << " & \\multicolumn{" << nselecs << "}{c" << ((idir == ndirs-1)? "" : "|") << "}{ "
    //         // << " SR: " << getJetBJetTableLabel(samples.at(0), string(dirs.at(idir)).replace(0,5,"srh")) << ", 0 lep. "
    //         // << " CR: " << getJetBJetTableLabel(samples.at(0), dirs.at(idir)) << ", 1 lep ($M_{\\rm{T}} < 100$)."
    //         << " }";
    // ofile << " \\\\" << endl
    //       << "\\cline{2-" << ndirs*nselecs+1 << "}" << endl;
    ofile << "Sample";
    for (unsigned int idir = 0; idir < ndirs; ++idir)
      ofile << "  &  " << dirs.at(idir);
    // for (unsigned int i = 0; i < ndirs; ++i)
    //   for (auto it = selecs.begin(); it != selecs.end(); ++it)
    //     ofile << " & " << *it;
    ofile << " \\\\" << endl;
    ofile << "\\hline\\hline" << endl;

    // backgrounds first -- loop backwards
    for ( int i = n-1 ; i >= 0 ; --i ) {
      if ( TString(names.at(i)).Contains("data") ) {found_data = true; continue;}
      if ( TString(names.at(i)).Contains("sig") ) continue;
      if ( TString(names.at(i)).Contains("lostlepFromCRs") ) continue;
      if ( !TString(names.at(i)).Contains("zinvDataDriven") ) continue;
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          // TString fullhistnameCR = Form("%s/h_mt2bins", (cr+dirs.at(idir)).c_str());
          // TString fullhistnameSR = Form("%s/h_mt2bins", (sr+dirs.at(idir)).c_str());
          TString fullhistnameCR = Form("%s/h_mt2binsCRyield", (sr+dirs.at(idir)).c_str());
          TString fullhistnameSR = Form("%s/h_mt2binsMCyield", (sr+dirs.at(idir)).c_str());
          TH1D* h_cr = (TH1D*) samples.at(i)->Get(fullhistnameCR);
          TH1D* h_sr = (TH1D*) samples.at(i)->Get(fullhistnameSR);
          double yield_cr = 0.;
          double err_cr = 0.;
          double yield_sr = 0.;
          double err_sr = 0.;
          int idx = idir*nselecs + isel;
          if (h_cr) {
            // use all bins
            yield_cr = h_cr->IntegralAndError(0,-1,err_cr);
            bgtot_cr.at(idx) += yield_cr;
            bgerr_cr.at(idx) = sqrt(pow(bgerr_cr.at(idx),2) + pow(err_cr,2));
          }
          if (h_sr) {
            // use all bins
            yield_sr = h_sr->IntegralAndError(0,-1,err_sr);
            bgtot_sr.at(idx) += yield_sr;
            bgerr_sr.at(idx) = sqrt(pow(bgerr_sr.at(idx),2) + pow(err_sr,2));
          }
        }
      }
    } // loop over samples

    // print bg_sr totals
    ofile << "GJets SR";
    for ( unsigned int idx = 0; idx < bgtot_sr.size(); ++idx ) {
      double yield = bgtot_sr.at(idx);
      double err = bgerr_sr.at(idx);
      ofile << "  &  ";
      if (yield > 10.)
        ofile << Form("%.1f $\\pm$ %.1f", yield, err);
      else
        ofile << Form("%.2f $\\pm$ %.2f", yield, err);
    }
    ofile << " \\\\" << endl;
    ofile << "\\hline" << endl;

    // print bg_cr totals
    ofile << "GJets CR";
    for ( unsigned int idx = 0; idx < bgtot_cr.size(); ++idx ) {
      double yield = bgtot_cr.at(idx);
      double err = bgerr_cr.at(idx);
      ofile << "  &  ";
      if (yield > 10.)
        ofile << Form("%.1f $\\pm$ %.1f", yield, err);
      else
        ofile << Form("%.2f $\\pm$ %.2f", yield, err);
    }
    ofile << " \\\\" << endl;
    ofile << "\\hline" << endl;

    // next print data, if it exists
    vector<int> datatot;
    for( unsigned int i = 0 ; i < n ; i++ ){
      if( !TString(names.at(i)).Contains("data") ) continue;
      found_data = true;
      ofile << "Data CR";
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins", (cr+dirs.at(idir)).c_str());
          TH1D* h = (TH1D*) samples.at(i)->Get(fullhistname);
          double yield = 0.;
          if (h) {
            // use all bins
            yield = round(h->Integral(0,-1)); // deal with weird rounding errors
          }
          ofile << "  &  " << Form("%d",(int)yield);
          if (dataMCratio) datatot.push_back(yield);
        }
      }
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;
    } // loop over samples

    // print the data/MC and transfer factor ratios
    if (dataMCratio) {
      ofile << "\\hline" << endl;
      ofile << "TF (SR/CR)";
      for (unsigned int idx = 0; idx < bgtot_cr.size(); ++idx)
        ofile << "  &  " << ((bgtot_sr[idx] > bgtot_cr[idx])? setprecision(3) : setprecision(2) )<< bgtot_sr[idx]/bgtot_cr[idx];
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;

      ofile << "Data/MC";
      for (unsigned int idx = 0; idx < bgtot_cr.size(); ++idx)
        ofile << "  &  " << ((datatot[idx] > bgtot_cr[idx])? setprecision(3) : setprecision(2)) << datatot[idx]/bgtot_cr[idx];
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;

    }

    for( unsigned int i = 0 ; i < n ; i++ ){
      if (!TString(names.at(i)).Contains("lostlepFromCRs") &&
          !TString(names.at(i)).Contains("zinvDataDriven")) continue;
      ofile << "\\hline" << endl;
      ofile << "DD Estimates";
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins", (sr+dirs.at(idir)).c_str());
          TH1D* h = (TH1D*) samples.at(i)->Get(fullhistname);
          double yield = 0.;
          double err = 0.;
          if (h) {
            yield = h->IntegralAndError(0, -1, err);
          }
          ofile << "  &  ";
          if (yield > 10.)
            ofile << Form("%.1f $\\pm$ %.1f", yield, err);
          else
            ofile << Form("%.2f $\\pm$ %.2f", yield, err);
        }
      }
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;
    } // loop over samples

    if (itab == nTabulars-1) ofile << "\\hline" << std::endl;
    else ofile << "\\multicolumn{" << ndirs*nselecs+1 << "}{c}{} \\\\" << endl;
    ofile << "\\end{tabular}" << std::endl;
  }

  ofile << "\\end{table}" << std::endl;

  ofile << endl;
  return;
}
*/

//_______________________________________________________________________________
void printComparisonRatioTable(vector<vector<TFile*>> samplesVec, vector<string> names, vector<string> selecs, vector<string> dirs, string caption = "", int colorReduction = 0, int mt2bin = -1) {

  // read off yields from h_mt2bins hist in each topological region
  vector<TFile*> samples = samplesVec[0];

  unsigned int n = samples.size();
  unsigned int ndirs = dirs.size();
  unsigned int nselecs = selecs.size();

  if (nselecs < 2) cout << "Use printTable instead!\n";

  // vector<string> dirs_all = dirs;
  vector<vector<string>> dirsVec;
  int nTabulars = ndirs*nselecs/8;
  if ((ndirs*nselecs)%8 > 0) ++nTabulars;
  int ndir_line = (ndirs%nTabulars == 0)? ndirs/nTabulars : ndirs/nTabulars+1;
  auto it = dirs.begin();
  for (int i = 0; i < nTabulars; ++i) {
    vector<string> temp_dirs;
    for (int j = 0; j < ndir_line && it != dirs.end(); ++j, ++it)
      temp_dirs.push_back(*it);
    dirsVec.push_back(temp_dirs);
  }

  ofile << "\\begin{table}[H]" << std::endl;
  ofile << "\\scriptsize" << std::endl;
  ofile << "\\centering" << std::endl;
  ofile << "\\caption{" << caption << "}" << std::endl;

  for (int itab = 0; itab < nTabulars; ++itab) {
    dirs = dirsVec[itab];
    ndirs = dirs.size();

    vector<double> bgtot(ndirs*nselecs, 0);
    vector<double> bgerr(ndirs*nselecs, 0);
    double yield_base = 0;

    ofile << "\\begin{tabular}{r";
    for (unsigned int idir=0; idir < ndirs*nselecs; ++idir) ofile << "|c";
    ofile << "}" << std::endl;
    ofile << "\\hline\\hline" << endl;
    ofile << "\\multirow{2}{*}{Sample}";
    for (unsigned int idir = 0; idir < ndirs; ++idir)
      ofile << " & \\multicolumn{" << nselecs << "}{c" << ((idir == ndirs-1)? "" : "|") << "}{ " << getJetBJetTableLabel(samples.at(0), dirs.at(idir)) << " }";
    ofile << " \\\\" << endl
          << "\\cline{2-" << ndirs*nselecs+1 << "}" << endl;
    for (unsigned int i = 0; i < ndirs; ++i)
      for (auto it = selecs.begin(); it != selecs.end(); ++it)
        ofile << " & " << *it;
    ofile << " \\\\" << endl;
    ofile << "\\hline\\hline" << endl;

    // backgrounds first -- loop backwards
    for ( int i = n-1 ; i >= 0 ; --i ) {
      if ( TString(names.at(i)).Contains("data") ) {found_data = true; continue;}
      if ( TString(names.at(i)).Contains("sig") ) continue;
      ofile << getTableName(names.at(i));
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins", dirs.at(idir).c_str());
          TH1D* h = (TH1D*) samplesVec[isel].at(i)->Get(fullhistname);
          double yield = 0.;
          double err = 0.;
          int idx = idir*nselecs + isel;
          if (h) {
            // use all bins
            if (mt2bin < 0) {
              yield = h->IntegralAndError(0,-1,err);
              bgtot.at(idx) += yield;
              bgerr.at(idx) = sqrt(pow(bgerr.at(idx),2) + pow(err,2));
            }
            // last bin: include overflow
            else if (mt2bin == h->GetXaxis()->GetNbins()) {
              yield = h->IntegralAndError(mt2bin,-1,err);
              bgtot.at(idx) += yield;
              bgerr.at(idx) = sqrt(pow(bgerr.at(idx),2) + pow(err,2));
            }
            // single bin, not last bin
            else {
              yield = h->GetBinContent(mt2bin);
              err = h->GetBinError(mt2bin);
              bgtot.at(idx) += yield;
              bgerr.at(idx) = sqrt(pow(bgerr.at(idx),2) + pow(err,2));
            }
          }
          if (isel == 0) {
            yield_base = yield;
            if (yield > 10.)
              ofile << "  &  " << Form("%.1f $\\pm$ %.1f", yield, err);
            else
              ofile << "  &  " << Form("%.2f $\\pm$ %.2f", yield, err);
          }
          else{
            if (colorReduction && yield_base/yield > colorReduction) {
              int colorFactor;
              if (yield == 0)
                colorFactor = 0;
              else if ((yield_base/yield)/(2*colorReduction) > 1)
                colorFactor = 25;
              else
                colorFactor = 30*(yield_base/yield)/(2*colorReduction);
              ofile << "\\cellcolor{cyan!" << colorFactor << "} ";
            }
            float ratio = (yield_base == 0)? 0 : yield/yield_base*100;
            if (ratio > 10.)
              ofile << "  &  " << Form("%.1f\\%%", ratio);
            else
              ofile << "  &  " << Form("%.2f\\%%", ratio);
          }
        }
      }
      ofile << " \\\\" << endl;
    } // loop over samples

    // print bg totals
    ofile << "\\hline" << endl;
    ofile << "Total SM";
    for ( unsigned int idx = 0; idx < bgtot.size(); ++idx ) {
      double yield = bgtot.at(idx);
      double err = bgerr.at(idx);
      ofile << "  &  ";

      if (idx%nselecs == 0) {
        yield_base = yield;
        if (yield > 10.)
          ofile << Form("%.1f $\\pm$ %.1f", yield, err);
        else
          ofile << Form("%.2f $\\pm$ %.2f", yield, err);
      }
      else {
        if (colorReduction && yield_base/yield > colorReduction) {
          int colorFactor;
          if (yield == 0)
            colorFactor = 0;
          else if ((yield_base/yield)/(2*colorReduction) > 1)
            colorFactor = 25;
          else
            colorFactor = 30*(1-(yield_base/yield)/(2*colorReduction));
          ofile << "\\cellcolor{cyan!" << colorFactor << "} ";
        }
        float ratio = (yield_base == 0)? 0 : yield/yield_base*100;
        if (ratio > 10.)
          ofile << Form("%.1f\\%%", ratio);
        else
          ofile << Form("%.2f\\%%", ratio);
      }
    }
    ofile << " \\\\" << endl;
    ofile << "\\hline" << endl;

    // next print data, if it exists
    for( unsigned int i = 0 ; i < n ; i++ ){
      if( !TString(names.at(i)).Contains("data") ) continue;
      found_data = true;
      ofile << "\\hline" << endl;
      ofile << getTableName(names.at(i));
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins", dirs.at(idir).c_str());
          TH1D* h = (TH1D*) samplesVec[isel].at(i)->Get(fullhistname);
          double yield = 0.;
          if (h) {
            // use all bins
            if (mt2bin < 0) {
              yield = h->Integral(0,-1);
            }
            // last bin: include overflow
            else if (mt2bin == h->GetXaxis()->GetNbins()) {
              yield = h->Integral(mt2bin,-1);
            }
            // single bin, not last bin
            else {
              yield = h->GetBinContent(mt2bin);
            }
          }
          if (colorReduction) {
            if (isel == 0)
              yield_base = yield;
            else if (yield_base/yield > colorReduction) {
              int colorFactor;
              if (yield == 0)
                colorFactor = 0;
              else if ((yield_base/yield)/(2*colorReduction) > 1)
                colorFactor = 25;
              else
                colorFactor = 30*(1-(yield_base/yield)/(2*colorReduction));
              ofile << "\\cellcolor{red!" << colorFactor << "} ";
            }
          }
          if (isel == 0) {
            yield_base = yield;
            ofile << "  &  " << Form("%d", (int) yield);
          }
          else {
            if (colorReduction && yield_base/yield > colorReduction) {
              int colorFactor;
              if (yield == 0)
                colorFactor = 0;
              else if ((yield_base/yield)/(2*colorReduction) > 1)
                colorFactor = 25;
              else
                colorFactor = 30*(yield_base/yield)/(2*colorReduction);
              ofile << "\\cellcolor{cyan!" << colorFactor << "} ";
            }
            float ratio = (yield_base == 0)? 0 : yield/yield_base*100;
            if (ratio > 10.)
              ofile << "  &  " << Form("%.1f\\%%", ratio);
            else
              ofile << "  &  " << Form("%.2f\\%%", ratio);
          }

        }
      }
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;
    } // loop over samples

    // finally print signals
    for( unsigned int i = 0 ; i < n ; i++ ){
      if( !TString(names.at(i)).Contains("sig") ) continue;
      ofile << getTableName(names.at(i));
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins",dirs.at(idir).c_str());
          TH1D* h = (TH1D*) samplesVec[isel].at(i)->Get(fullhistname);
          double yield = 0.;
          double err = 0.;
          if (h) {
            // use all bins
            if (mt2bin < 0) {
              yield = h->IntegralAndError(0,-1,err);
            }
            // last bin: include overflow
            else if (mt2bin == h->GetXaxis()->GetNbins()) {
              yield = h->IntegralAndError(mt2bin,-1,err);
            }
            // single bin, not last bin
            else {
              yield = h->GetBinContent(mt2bin);
              err = h->GetBinError(mt2bin);
            }
          }
          if (isel == 0) {
            yield_base = yield;
            if (yield > 10.)
              ofile << "  &  " << Form("%.1f $\\pm$ %.1f", yield, err);
            else
              ofile << "  &  " << Form("%.2f $\\pm$ %.2f", yield, err);
          }
          else {
            if (colorReduction && yield_base/yield > colorReduction) {
              int colorFactor;
              if (yield == 0)
                colorFactor = 0;
              else if ((yield_base/yield)/(2*colorReduction) > 1)
                colorFactor = 25;
              else
                colorFactor = 30*(yield_base/yield)/(2*colorReduction);
              ofile << "\\cellcolor{red!" << colorFactor << "} ";
            }
            float ratio = (yield_base == 0)? 0 : yield/yield_base*100;
            if (ratio > 10.)
              ofile << "  &  " << Form("%.1f\\%%", ratio);
            else
              ofile << "  &  " << Form("%.2f\\%%", ratio);
          }
        }
      }
      ofile << " \\\\" << endl;
      ofile << "\\hline" << endl;
    } // loop over samples

    if (itab == nTabulars-1) ofile << "\\hline" << std::endl;
    else ofile << "\\multicolumn{" << ndirs*nselecs+1 << "}{c}{} \\\\" << endl;
    ofile << "\\end{tabular}" << std::endl;
  }

  ofile << "\\end{table}" << std::endl;

  ofile << endl;
  return;
}

//_______________________________________________________________________________
void printTable( vector<TFile*> samples , vector<string> names , vector<string> dirs, string caption = "", int compare = 0, int mt2bin = -1 ) {

  // read off yields from h_mt2bins hist in each topological region

  const unsigned int n = samples.size();
  const unsigned int ndirs = dirs.size();

  vector<double> bgtot(ndirs,0.);
  vector<double> bgerr(ndirs,0.);

  if (compare == 1) caption += " (" + getJetBJetTableLabel(samples.at(0), dirs.at(0)) + ")";

  ofile << "\\begin{table}[H]" << std::endl;
  ofile << "\\scriptsize" << std::endl;
  ofile << "\\centering" << std::endl;
  ofile << "\\caption{" << caption << "}" << std::endl;
  ofile << "\\begin{tabular}{r";
  for (unsigned int idir=0; idir < ndirs; ++idir) ofile << "|c";
  ofile << "}" << std::endl;
  ofile << "\\hline\\hline" << std::endl;

  // header
  if (compare > 1) {
    ofile << "\\multirow{2}{*}{Sample}";
    if (ndirs % compare != 0) cout << "WARNING: Receive " << ndirs << " dirs for " << compare << " comparison!" << endl;
    for (unsigned int idir = 0; idir < ndirs/compare; ++idir)
      ofile << " & \\multicolumn{" << compare << "}{c|}{ " << getJetBJetTableLabel(samples.at(0), dirs.at(idir*compare)) << " }";
    ofile << " \\\\" << endl
          << "\\cline{2-" << ndirs+1 << "}" << endl;
    for (unsigned int idir = 0; idir < ndirs; ++idir)
      ofile << " & " << dirs.at(idir);
  }
  else {
    ofile << "Sample";
    for (unsigned int idir = 0; idir < ndirs; ++idir) {
      if (compare) ofile << " & " << dirs.at(idir);
      else ofile << " & " << getJetBJetTableLabel(samples.at(0), dirs.at(idir));
    }
  }
  ofile << " \\\\" << endl;
  ofile << "\\hline\\hline" << endl;

  // backgrounds first -- loop backwards
  for( int i = n-1 ; i >= 0 ; --i ){
    if( TString(names.at(i)).Contains("data")  ) {found_data = true; continue;}
    if( TString(names.at(i)).Contains("sig")  ) continue;
    ofile << getTableName(names.at(i));
    for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
      TString fullhistname = Form("%s/h_mt2bins",dirs.at(idir).c_str());
       // if (names.at(i)=="gjets" || names.at(i)=="gjetsqcd") fullhistname.ReplaceAll("sr","crgj");
       // if (names.at(i)=="dyjets") fullhistname.ReplaceAll("sr","crdy");
      // if (names.at(i)=="wjets") fullhistname.ReplaceAll("sr","crrl");
      TH1D* h = (TH1D*) samples.at(i)->Get(fullhistname);
      double yield = 0.;
      double err = 0.;
      if (h) {
        // use all bins
        if (mt2bin < 0) {
          yield = h->IntegralAndError(0,-1,err);
          bgtot.at(idir) += yield;
          bgerr.at(idir) = sqrt(pow(bgerr.at(idir),2) + pow(err,2));
        }
        // last bin: include overflow
        else if (mt2bin == h->GetXaxis()->GetNbins()) {
          yield = h->IntegralAndError(mt2bin,-1,err);
          bgtot.at(idir) += yield;
          bgerr.at(idir) = sqrt(pow(bgerr.at(idir),2) + pow(err,2));
        }
        // single bin, not last bin
        else {
          yield = h->GetBinContent(mt2bin);
          err = h->GetBinError(mt2bin);
          bgtot.at(idir) += yield;
          bgerr.at(idir) = sqrt(pow(bgerr.at(idir),2) + pow(err,2));
        }
      }
      if (yield > 10.) {
        //  	ofile << "  &  " << Form("%.0f $\\pm$ %.0f",yield,err);
        ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
      } else {
        //  	ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
        ofile << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
      }
    }
    ofile << " \\\\" << endl;
  } // loop over samples

  // print bg totals
  ofile << "\\hline" << endl;
  ofile << "Total SM";
  for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
    double yield = bgtot.at(idir);
    double err = bgerr.at(idir);
    if (yield > 10.) {
      //  	ofile << "  &  " << Form("%.0f $\\pm$ %.0f",yield,err);
      ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
    } else {
      //  	ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
      ofile << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
    }
  }
  ofile << " \\\\" << endl;
  ofile << "\\hline" << endl;

  // next print data, if it exists
  if (found_data) {
    for( unsigned int i = 0 ; i < n ; i++ ){
      if( !TString(names.at(i)).Contains("data") ) continue;
      ofile << getTableName(names.at(i));
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
	TString fullhistname = Form("%s/h_mt2bins",dirs.at(idir).c_str());
	TH1D* h = (TH1D*) samples.at(i)->Get(fullhistname);
	double yield = 0.;
	if (h) {
	  // use all bins
	  if (mt2bin < 0) {
	    yield = h->Integral(0,-1);
	  }
	  // last bin: include overflow
	  else if (mt2bin == h->GetXaxis()->GetNbins()) {
	    yield = h->Integral(mt2bin,-1);
	  }
	  // single bin, not last bin
	  else {
	    yield = h->GetBinContent(mt2bin);
	  }
	}
	ofile << "  &  " << Form("%d",(int)yield);
      }
      ofile << " \\\\" << endl;
    } // loop over samples
    ofile << "\\hline" << endl;
  } // if found_data

  // finally print signals
  for( unsigned int i = 0 ; i < n ; i++ ){
    if( !TString(names.at(i)).Contains("sig") ) continue;
    ofile << getTableName(names.at(i));
    for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
      TString fullhistname = Form("%s/h_mt2bins",dirs.at(idir).c_str());
      TH1D* h = (TH1D*) samples.at(i)->Get(fullhistname);
      double yield = 0.;
      double err = 0.;
      if (h) {
        // use all bins
        if (mt2bin < 0) {
          yield = h->IntegralAndError(0,-1,err);
        }
        // last bin: include overflow
        else if (mt2bin == h->GetXaxis()->GetNbins()) {
          yield = h->IntegralAndError(mt2bin,-1,err);
        }
        // single bin, not last bin
        else {
          yield = h->GetBinContent(mt2bin);
          err = h->GetBinError(mt2bin);
        }
      }
      if (yield > 10.) {
        //  	ofile << "  &  " << Form("%.0f $\\pm$ %.0f",yield,err);
        ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
      } else {
        //  	ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
        ofile << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
      }
    }
    ofile << " \\\\" << endl;
  } // loop over samples

  ofile << "\\hline" << std::endl;
  ofile << "\\end{tabular}" << std::endl;
  ofile << "\\end{table}" << std::endl;

  ofile << endl;
  return;
}

//_______________________________________________________________________________
void printRatioTable( vector<TFile*> samples , vector<string> names , vector<string> dirs_num, vector<string> dirs_denom, int mt2bin = -1 ) {

  // read off yields from h_mt2bins hist in each topological region

  const unsigned int n = samples.size();
  const unsigned int ndirs = dirs_num.size();

  if (dirs_num.size() != dirs_denom.size()) {
    std::cout << "ERROR: mismatch in number of directories" << std::endl;
    return;
  }

  vector<double> bgtot_num(ndirs,0.);
  vector<double> bgerr_num(ndirs,0.);
  vector<double> bgtot_denom(ndirs,0.);
  vector<double> bgerr_denom(ndirs,0.);

  std::cout << "\\begin{table}[!ht]" << std::endl;
  std::cout << "\\scriptsize" << std::endl;
  std::cout << "\\centering" << std::endl;
  std::cout << "\\begin{tabular}{r";
  for (unsigned int idir=0; idir < ndirs; ++idir) std::cout << "|c";
  std::cout << "}" << std::endl;
  std::cout << "\\hline" << std::endl;

  cout << endl << "\\hline" << endl
    << "Sample";

  // header
  for (unsigned int idir = 0; idir < ndirs; ++idir) {
    //cout << " & " << getRegionName(dirs.at(idir));
    cout << " & " << getJetBJetTableLabel(samples.at(0), dirs_num.at(idir));
  }
  cout << " \\\\" << endl
    << "\\hline\\hline" << endl;

  // backgrounds first -- loop backwards
  for( int i = n-1 ; i >= 0 ; --i ){
    if( TString(names.at(i)).Contains("data")  ) continue;
    if( TString(names.at(i)).Contains("sig")  ) continue;
    cout << getTableName(names.at(i));
    for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
      TString fullhistname_num = Form("%s/h_mt2bins",dirs_num.at(idir).c_str());
      TH1D* h_num = (TH1D*) samples.at(i)->Get(fullhistname_num);
      double yield_num = 0.;
      double err_num = 0.;
      if (h_num) {
        // use all bins
        if (mt2bin < 0) {
          yield_num = h_num->IntegralAndError(0,-1,err_num);
          bgtot_num.at(idir) += yield_num;
          bgerr_num.at(idir) = sqrt(pow(bgerr_num.at(idir),2) + pow(err_num,2));
        }
        // last bin: include overflow
        else if (mt2bin == h_num->GetXaxis()->GetNbins()) {
          yield_num = h_num->IntegralAndError(mt2bin,-1,err_num);
          bgtot_num.at(idir) += yield_num;
          bgerr_num.at(idir) = sqrt(pow(bgerr_num.at(idir),2) + pow(err_num,2));
        }
        // single bin, not last bin
        else {
          yield_num = h_num->GetBinContent(mt2bin);
          err_num = h_num->GetBinError(mt2bin);
          bgtot_num.at(idir) += yield_num;
          bgerr_num.at(idir) = sqrt(pow(bgerr_num.at(idir),2) + pow(err_num,2));
        }
      }

      TString fullhistname_denom = Form("%s/h_mt2bins",dirs_denom.at(idir).c_str());
      TH1D* h_denom = (TH1D*) samples.at(i)->Get(fullhistname_denom);
      double yield_denom = 0.;
      double err_denom = 0.;
      if (h_denom) {
        // use all bins
        if (mt2bin < 0) {
          yield_denom = h_denom->IntegralAndError(0,-1,err_denom);
          bgtot_denom.at(idir) += yield_denom;
          bgerr_denom.at(idir) = sqrt(pow(bgerr_denom.at(idir),2) + pow(err_denom,2));
        }
        // last bin: include overflow
        else if (mt2bin == h_denom->GetXaxis()->GetNbins()) {
          yield_denom = h_denom->IntegralAndError(mt2bin,-1,err_denom);
          bgtot_denom.at(idir) += yield_denom;
          bgerr_denom.at(idir) = sqrt(pow(bgerr_denom.at(idir),2) + pow(err_denom,2));
        }
        // single bin, not last bin
        else {
          yield_denom = h_denom->GetBinContent(mt2bin);
          err_denom = h_denom->GetBinError(mt2bin);
          bgtot_denom.at(idir) += yield_denom;
          bgerr_denom.at(idir) = sqrt(pow(bgerr_denom.at(idir),2) + pow(err_denom,2));
        }
      }

      if (yield_denom == 0.) {
        cout << "  &  - ";
      } else {
	double ratio = yield_num / yield_denom;
	double ratio_err = ratio * sqrt(pow(err_num/yield_num,2) + pow(err_denom/yield_denom,2));
        cout << "  &  " << Form("%.2f $\\pm$ %.2f",ratio,ratio_err);
      }
    }
    cout << " \\\\" << endl;
  } // loop over samples

  // print bg totals
  cout << "\\hline" << endl;
  cout << "Total SM";
  for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
    double yield_num = bgtot_num.at(idir);
    double err_num = bgerr_num.at(idir);
    double yield_denom = bgtot_denom.at(idir);
    double err_denom = bgerr_denom.at(idir);
    if (yield_denom == 0.) {
      cout << "  &  - ";
    } else {
      double ratio = yield_num / yield_denom;
      double ratio_err = ratio * sqrt(pow(err_num/yield_num,2) + pow(err_denom/yield_denom,2));
      cout << "  &  " << Form("%.2f $\\pm$ %.2f",ratio,ratio_err);
    }
  }
  cout << " \\\\" << endl;
  cout << "\\hline" << endl;

  std::cout << "\\end{tabular}" << std::endl;
  std::cout << "\\caption{}" << std::endl;
  std::cout << "\\end{table}" << std::endl;

  cout << endl;
  return;
}

//_______________________________________________________________________________
void printDetailedComparisonTable(vector<vector<TFile*>> samplesVec, vector<string> names, vector<string> selecs, vector<string> dirs, string caption = "", int colorReduction = 0) {

  // read off yields from h_mt2bins hist in each topological region
  vector<TFile*> samples = samplesVec[0];
  string dir = dirs[0];

  const unsigned int n = samples.size();

  unsigned int ndirs = dirs.size();
  unsigned int nselecs = selecs.size();
  if (nselecs < 2) cout << "Use printTable instead!\n";

  // vector<vector<string>> dirsVec;
  // int nTabulars = ndirs*nselecs/8;
  // if ((ndirs*nselecs)%8 > 0) ++nTabulars;
  // int ndir_line = (ndirs%nTabulars == 0)? ndirs/nTabulars : ndirs/nTabulars+1;
  // auto it = dirs.begin();
  // for (int i = 0; i < nTabulars; ++i) {
  //   vector<string> temp_dirs;
  //   for (int j = 0; j < ndir_line && it != dirs.end(); ++j, ++it)
  //     temp_dirs.push_back(*it);
  //   dirsVec.push_back(temp_dirs);
  // }
  if (caption == "") {
    caption += dir + ": ";
    caption += getJetBJetTableLabel(samples.at(0), dir) + ", ";
    caption += getHTTableLabel(samples.at(0), dir) + ", ";
    caption += getMETTableLabel(samples.at(0), dir);
  }

  ofile << "\\begin{table}[H]" << std::endl;
  // ofile << "\\caption{Background and signal yields in bins of \\mttwo for ";
  // ofile << getHTTableLabel(samples.at(0), dir) << ", ";
  // ofile << getJetBJetTableLabel(samples.at(0), dir) << ". ";
  // ofile << "The yields are normalized to \\lumival.}" << std::endl;
  ofile << "\\caption{" << caption << "}" << endl;
  ofile << "\\scriptsize" << std::endl;
  ofile << "\\centering" << std::endl;

  TString nbinshistname = Form("%s/h_n_mt2bins", dir.c_str());
  TH1D* h_n_mt2bins = (TH1D*) samples.at(0)->Get(nbinshistname);
  int n_mt2bins;
  if(h_n_mt2bins){
    n_mt2bins = h_n_mt2bins->GetBinContent(1);
  }
  else{
    ofile << "Couldn't get number of mt2 bins" << std::endl;
    return;
  }

  int nTabulars = n_mt2bins; // temporarily

  for (int itab = 0; itab < nTabulars; ++itab) {
    // dirs = dirsVec[itab];
    // ndirs = dirs.size();
    // vector<double> bgtot(n_mt2bins,0.);
    // vector<double> bgerr(n_mt2bins,0.);
    vector<double> bgtot(nselecs, 0.);
    vector<double> bgerr(nselecs, 0.);

    TString binshistname = Form("%s/h_mt2bins",dir.c_str());
    TH1D* h_mt2bins(0);
    for(unsigned int i=0; i<samples.size(); i++){ //need to find a sample that has this hist filled
      h_mt2bins = (TH1D*) samples.at(i)->Get(binshistname);
      if (h_mt2bins) break;
    }

    if(!h_mt2bins) ofile << "Couldn't get the mt2 binning" << std::endl;

    ofile << "\\begin{tabular}{r";
    for (int ibin=0; ibin < nselecs; ++ibin) ofile << "|c";
    ofile << "}" << std::endl;
    ofile << "\\hline\\hline" << std::endl;
    ofile << "\\multirow{2}{*}{Sample}";
    // for (unsigned int idir = 0; idir < ndirs; ++idir)
    // ofile << " & \\multicolumn{" << nselecs << "}{c" << ((idir == ndirs-1)? "" : "|") << "}{ " << getJetBJetTableLabel(samples.at(0), dirs.at(idir)) << " }";
    ofile << " & \\multicolumn{" << nselecs << "}{c}{";
    ofile << getJetBJetTableLabel(samples.at(0), dirs.at(0)) << ", ";
    int ibin = itab+1; // temporary
    if (ibin == n_mt2bins)
      ofile << "\\mttwo $>$ " << h_mt2bins->GetXaxis()->GetBinLowEdge(n_mt2bins) << " GeV";
    else
      ofile << h_mt2bins->GetXaxis()->GetBinLowEdge(ibin) << " $<$ \\mttwo $<$ " << h_mt2bins->GetXaxis()->GetBinLowEdge(ibin+1) << " GeV";
    // ofile << "\\multicolumn{" << n_mt2bins+1 << "}{c}{";
    ofile << "} \\\\" << std::endl;
    ofile << "\\cline{2-" << ndirs*nselecs+1 << "}" << endl;
    for (unsigned int i = 0; i < ndirs; ++i)
      for (auto it = selecs.begin(); it != selecs.end(); ++it)
        ofile << " & " << *it;
    ofile << " \\\\" << endl;
    ofile << "\\hline\\hline" << endl;


    // backgrounds first -- loop backwards
    for( int isamp = n-1 ; isamp >= 0 ; --isamp ){
      if( TString(names.at(isamp)).Contains("data")  ) continue;
      if( TString(names.at(isamp)).Contains("sig")  ) continue;
      ofile << getTableName(names.at(isamp));
      // for (int ibin = 1; ibin <= n_mt2bins; ++ibin) {
      for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
        TString fullhistname = Form("%s/h_mt2bins",dir.c_str());
        if (names.at(isamp)=="gjets" || names.at(isamp)=="gjetsqcd") fullhistname.ReplaceAll("sr","crgj");
        if (names.at(isamp)=="dyjets") fullhistname.ReplaceAll("sr","crdy");
        TH1D* h = (TH1D*) samplesVec[isel].at(isamp)->Get(fullhistname);
        double yield = 0.;
        double err = 0.;
        int idx = isel;
        if (h) {
          // not last bin
          if(ibin != n_mt2bins) {
            yield = h->GetBinContent(ibin);
            err = h->GetBinError(ibin);
            bgtot.at(idx) += yield;
            bgerr.at(idx) = sqrt(pow(bgerr.at(idx),2) + pow(err,2));
          }
          // last bin: include overflow
          else if (ibin == h->GetXaxis()->GetNbins()) {
            yield = h->IntegralAndError(ibin,-1,err);
            bgtot.at(idx) += yield;
            bgerr.at(idx) = sqrt(pow(bgerr.at(idx),2) + pow(err,2));
          }
          else {
            ofile << "Shouldn't get here" << std::endl;
            return;
          }
        }
        if (yield > 10.)
          ofile << "  &  " << Form("%.1f $\\pm$ %.1f", yield, err);
        else
          ofile << "  &  " << Form("%.2f $\\pm$ %.2f", yield, err);
      }
      ofile << " \\\\" << endl;
    } // loop over samples

    // print bg totals
    ofile << "\\hline" << endl;
    ofile << "Total SM";
    for ( unsigned int idx = 0; idx < bgtot.size(); ++idx ) {
      double yield = bgtot.at(idx);
      double err = bgerr.at(idx);
      if (yield > 10.)
        ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
      else
        ofile << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
    }
    ofile << " \\\\" << endl;
    ofile << "\\hline" << endl;

    // for( unsigned int jsamp = 0 ; jsamp < n ; jsamp++ ){
    //   if( !TString(names.at(jsamp)).Contains("sig") ) continue;
    //   ofile << getTableName(names.at(jsamp));
    //   for (int ibin = 1; ibin <= n_mt2bins; ++ibin) {
    //     TString fullhistname = Form("%s/h_mt2bins",dir.c_str());
    //     TH1D* h = (TH1D*) samples.at(jsamp)->Get(fullhistname);
    //     double yield = 0.;
    //     double err = 0.;
    //     if (h) {
    //       // not last bin
    //       if(ibin != n_mt2bins) {
    //         yield = h->GetBinContent(ibin);
    //         err = h->GetBinError(ibin);
    //       }
    //       // last bin: include overflow
    //       else if (ibin == h->GetXaxis()->GetNbins()) {
    //         yield = h->IntegralAndError(ibin,-1,err);
    //       }
    //       else {
    //         ofile << "Shouldn't get here" << std::endl;
    //         return;
    //       }
    //     }
    //     if (yield > 10.) {
    //       //  	ofile << "  &  " << Form("%.0f $\\pm$ %.0f",yield,err);
    //       ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
    //     } else {
    //       //  	ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
    //       ofile << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
    //     }
    //   }
    //   ofile << " \\\\" << endl;
    // } // loop over samples

    if (itab == nTabulars-1) ofile << "\\hline" << std::endl;
    else ofile << "\\multicolumn{" << ndirs*nselecs+1 << "}{c}{} \\\\" << endl;
    ofile << "\\end{tabular}" << std::endl;
  }

  ofile << "\\end{table}" << std::endl;
  //ofile << "\\pagebreak" << std::endl;
  ofile << std::endl;

  return;
}

void printDetailedTableDataDriven(vector<TFile*> samplesMC, vector<string> namesMC, vector<TFile*> samplesDD, vector<string> namesDD, vector<string> dirs, string caption = "") {

  // read off yields from h_mt2bins hist in each topological region
  // cout << "Reaching heres: " << __LINE__ << endl;

  const unsigned int nmc = samplesMC.size();
  const unsigned int ndirs = dirs.size();
  TFile* sample = samplesMC.at(nmc-1);

  if (caption == "Detailed") { // temporary
    // caption = string(dirs[0]).replace(dirs[0].size()-2, 1, " ") + ": ";
    caption += getJetBJetTableLabel(sample, dirs[0]) + ", ";
    caption += getHTTableLabel(sample, dirs[0]) + ", ";
    caption += getMETTableLabel(sample, dirs[0]);
    caption += ", MT2 $>$ 200 GeV";
  }

  int nmt2bins[ndirs];
  vector<int> mt2lower;
  vector<int> mt2upper;
  for (unsigned int idir = 0; idir < ndirs; ++idir) {
    TH1D* h_mt2bins(0);
    TString binshistname = Form("%s/h_mt2bins", dirs.at(idir).c_str());
    for (unsigned int i = 0; i < nmc; i++) { // need to find a sample that has this hist filled
      h_mt2bins = (TH1D*) samplesMC.at(i)->Get(binshistname);
      if (h_mt2bins) {
        nmt2bins[idir] = h_mt2bins->GetNbinsX();
        for (int i = 1; i <= nmt2bins[idir]; ++i) {
          mt2lower.push_back(h_mt2bins->GetXaxis()->GetBinLowEdge(i));
          mt2upper.push_back(h_mt2bins->GetXaxis()->GetBinLowEdge(i+1));
        }
        break;
      }
    }
    if (!h_mt2bins) { cout << "Couldn't get the mt2 binning" << std::endl; return; }
  }

  int ncols = mt2lower.size();

  ofile << "\\begin{table}[H]" << std::endl;
  ofile << "\\scriptsize" << std::endl;
  ofile << "\\centering" << std::endl;
  // ofile << "\\caption{" << caption << "}" << std::endl;

  ofile << "\\begin{tabular}{r";
  for (int i = 0; i < ncols; ++i) ofile << "|c";
  ofile << "}" << std::endl;
  ofile << "\\hline\\hline" << endl;
  ofile << "\\multirow{2}{*}{Sample}";
  for (unsigned int idir = 0; idir < ndirs; ++idir)
    ofile << " & \\multicolumn{" << nmt2bins[idir] << "}{c" << ((idir == ndirs-1)? "" : "|") << "}{ " << dirs[idir] << " }";
          // << getJetBJetTableLabel(sample, dirs[idir]) << ", "
          // << getHTTableLabel(sample, dirs[idir]) << " }";
  ofile << " \\\\" << endl
        << "\\cline{2-" << ncols+1 << "}" << endl;
  for (unsigned int i = 0; i < ncols; ++i) {
    if (mt2upper[i] == 1500)
      ofile << " & " << " \\mttwo $>$ " << mt2lower[i];// << " GeV";
    else
      ofile << " & " << mt2lower[i] << " $<$ \\mttwo $<$ " << mt2upper[i];// << " GeV";
  }
  ofile << " \\\\" << endl;
  ofile << "\\hline\\hline" << endl;

  // backgrounds MC first
  vector<double> bgtot(ncols, 0);
  vector<double> bgerr(ncols, 0);
  for (int isamp = 0; isamp < nmc; ++isamp) {
    if (TString(namesMC.at(isamp)).Contains("data")) continue;
    if (TString(namesMC.at(isamp)).Contains("sig")) continue;
    ofile << getTableName(namesMC.at(isamp));
    for (unsigned int idir = 0, icol = 0; idir < ndirs; ++idir) {
      int n_mt2bins = nmt2bins[idir];
      for (int ibin = 1; ibin <= n_mt2bins; ++ibin, ++icol) {
        TString fullhistname = Form("%s/h_mt2bins", dirs[idir].c_str());
        // if (namesMC.at(isamp)=="gjets" || names.at(isamp)=="gjetsqcd") fullhistname.ReplaceAll("sr","crgj");
        // if (namesMC.at(isamp)=="dyjets") fullhistname.ReplaceAll("sr","crdy");
        TH1D* h = (TH1D*) samplesMC.at(isamp)->Get(fullhistname);
        double yield = 0.;
        double err = 0.;
        if (h) {
          // not last bin
          if(ibin != n_mt2bins) {
            yield = h->GetBinContent(ibin);
            err = h->GetBinError(ibin);
            bgtot.at(icol) += yield;
            bgerr.at(icol) = sqrt(pow(bgerr.at(icol),2) + pow(err,2));
          }
          // last bin: include overflow
          else if (ibin == h->GetXaxis()->GetNbins()) {
            yield = h->IntegralAndError(ibin, -1, err);
            bgtot.at(icol) += yield;
            bgerr.at(icol) = sqrt(pow(bgerr.at(icol),2) + pow(err,2));
          }
          else {
            ofile << "Shouldn't get here" << std::endl;
            return;
          }
        }
        if (yield > 10.) 
          ofile << "  &  " << Form("%.1f $\\pm$ %.1f", yield, err);
        else 
          ofile << "  &  " << Form("%.2f $\\pm$ %.2f", yield, err);
      }
    }
    ofile << " \\\\" << endl;
  } // loop over MC samples

  // print bg totals
  ofile << "\\hline" << endl;
  ofile << "Total MC";
  for (int icol = 0; icol < ncols; ++icol) {
    double yield = bgtot.at(icol);
    double err = bgerr.at(icol);
    if (yield > 10.) 
      ofile << "  &  " << Form("%.1f $\\pm$ %.1f", yield, err);
    else 
      ofile << "  &  " << Form("%.2f $\\pm$ %.2f", yield, err);
  }
  ofile << " \\\\" << endl;
  ofile << "\\hline" << endl;

  ofile << "\\hline" << endl;
  std::fill(bgtot.begin(), bgtot.end(), 0);
  std::fill(bgerr.begin(), bgerr.end(), 0);
  for (unsigned int isamp = 0 ; isamp < samplesDD.size() ; ++isamp) {
    if (TString(namesDD.at(isamp)).Contains("data")) continue;
    if (TString(namesDD.at(isamp)).Contains("lostlep"))
      ofile << "LostLep DD";
    if (TString(namesDD.at(isamp)).Contains("zinv"))
      ofile << "$Z$-inv DD";
    for (unsigned int idir = 0, icol = 0; idir < ndirs; ++idir) {
      int n_mt2bins = nmt2bins[idir];
      for (int ibin = 1; ibin <= n_mt2bins; ++ibin, ++icol) {
        TString fullhistname = Form("%s/h_mt2bins", dirs[idir].c_str());
        TH1D* h = (TH1D*) samplesDD.at(isamp)->Get(fullhistname);
        double yield = 0.;
        double err = 0.;
        if (h) {
          // not last bin
          if(ibin != n_mt2bins) {
            yield = h->GetBinContent(ibin);
            err = h->GetBinError(ibin);
            bgtot.at(icol) += yield;
            bgerr.at(icol) = sqrt(pow(bgerr.at(icol),2) + pow(err,2));
          }
          // last bin: include overflow
          else if (ibin == h->GetXaxis()->GetNbins()) {
            yield = h->IntegralAndError(ibin, -1, err);
            bgtot.at(icol) += yield;
            bgerr.at(icol) = sqrt(pow(bgerr.at(icol),2) + pow(err,2));
          }
          else {
            ofile << "Shouldn't get here" << std::endl;
            return;
          }
        }
        if (yield > 10.) 
          ofile << "  &  " << Form("%.1f $\\pm$ %.1f", yield, err);
        else 
          ofile << "  &  " << Form("%.2f $\\pm$ %.2f", yield, err);
      }
    }
    ofile << " \\\\" << endl;
  } // loop over DD samples

  // print DD Estimate totals
  ofile << "\\hline" << endl;
  ofile << "Total DD";
  for (int icol = 0; icol < ncols; ++icol ) {
    double yield = bgtot.at(icol);
    double err = bgerr.at(icol);
    if (yield > 10.) 
      ofile << "  &  " << Form("%.1f $\\pm$ %.1f", yield, err);
    else 
      ofile << "  &  " << Form("%.2f $\\pm$ %.2f", yield, err);
  }
  ofile << " \\\\" << endl;
  ofile << "\\hline" << endl;

  ofile << "\\hline" << endl;
  for (unsigned int i = 0 ; i < nmc; i++) {
    if (!TString(namesMC.at(i)).Contains("data") ) continue;
    ofile << getTableName(namesMC.at(i));
    for (unsigned int idir = 0; idir < ndirs; ++idir ) {
      for (unsigned int ibin = 1; ibin <= nmt2bins[idir]; ++ibin) {
        TString fullhistname = Form("%s/h_mt2bins", dirs.at(idir).c_str());
        TH1D* h = (TH1D*) samplesMC.at(i)->Get(fullhistname);
        int yield = 0;
        if (h) {
          // use all bins
          if(ibin != nmt2bins[idir]) {
            yield = round(h->GetBinContent(ibin));
          }
          // last bin: include overflow
          else if (ibin == h->GetXaxis()->GetNbins()) {
            yield = round(h->Integral(ibin, -1));
          }
          else {
            ofile << "Shouldn't get here" << std::endl;
            return;
          }
        }
        ofile << "  &  " << Form("%d", yield);
      }
    }
    ofile << " \\\\" << endl;
    ofile << "\\hline" << endl;
  }

  ofile << "\\hline" << std::endl;
  ofile << "\\end{tabular}" << std::endl;
  ofile << "\\end{table}" << std::endl;
  //ofile << "\\pagebreak" << std::endl;
  ofile << std::endl;

  return;
}

void printDetailedTable( vector<TFile*> samples , vector<string> names , string dir) {

  // read off yields from h_mt2bins hist in each topological region

  const unsigned int n = samples.size();


  TString nbinshistname = Form("%s/h_n_mt2bins",dir.c_str());
  TH1D* h_n_mt2bins = (TH1D*) samples.at(0)->Get(nbinshistname);
  int n_mt2bins;
  if(h_n_mt2bins){
    n_mt2bins = h_n_mt2bins->GetBinContent(1);
  }
  else{
    ofile << "Couldn't get number of mt2 bins" << std::endl;
    return;
  }
  vector<double> bgtot(n_mt2bins,0.);
  vector<double> bgerr(n_mt2bins,0.);

  TString binshistname = Form("%s/h_mt2bins",dir.c_str());
  TH1D* h_mt2bins(0);
  for(unsigned int i=0; i<samples.size(); i++){//need to find a sample that has this hist filled
    h_mt2bins = (TH1D*) samples.at(i)->Get(binshistname);
    if(h_mt2bins) break;
  }

  if(!h_mt2bins) ofile << "Couldn't get the mt2 binning" << std::endl;

  ofile << "\\begin{table}[htbp]" << std::endl;
  // ofile << "\\caption{Background and signal yields in bins of \\mttwo for ";
  // ofile << getHTTableLabel(samples.at(0), dir) << ", ";
  // ofile << getJetBJetTableLabel(samples.at(0), dir) << ". ";
  // ofile << "The yields are normalized to \\lumival.}" << std::endl;
  ofile << "\\scriptsize" << std::endl;
  ofile << "\\centering" << std::endl;
  // ofile << "\\makebox[\\textwidth][c]{" << std::endl;
  ofile << "\\begin{tabular}{r";
  for (int ibin=0; ibin < n_mt2bins; ++ibin) ofile << "|c";
  ofile << "}" << std::endl;
  ofile << "\\hline" << std::endl;
  ofile << "\\multicolumn{" << n_mt2bins+1 << "}{c}{";
  ofile << getHTTableLabel(samples.at(0), dir) << ", ";
  ofile << getMETTableLabel(samples.at(0), dir) << ", ";
  ofile << getJetBJetTableLabel(samples.at(0), dir);
  ofile << "} \\\\" << std::endl;
  ofile << "\\hline" << std::endl;
  ofile << "\\hline" << endl
    << "Sample";

  // header
  for (int ibin = 1; ibin < n_mt2bins; ++ibin) {
    ofile << " & " << h_mt2bins->GetXaxis()->GetBinLowEdge(ibin) << " $<$ \\mttwo $<$ " << h_mt2bins->GetXaxis()->GetBinLowEdge(ibin+1) << " GeV";
  }
  ofile << " & \\mttwo $>$ " << h_mt2bins->GetXaxis()->GetBinLowEdge(n_mt2bins) << " GeV";
  ofile << " \\\\" << endl
    << "\\hline\\hline" << endl;

  // backgrounds first -- loop backwards
  for( int isamp = n-1 ; isamp >= 0 ; --isamp ){
    if( TString(names.at(isamp)).Contains("data")  ) continue;
    if( TString(names.at(isamp)).Contains("sig")  ) continue;
    ofile << getTableName(names.at(isamp));
    for (int ibin = 1; ibin <= n_mt2bins; ++ibin) {
      TString fullhistname = Form("%s/h_mt2bins",dir.c_str());
      if (names.at(isamp)=="gjets" || names.at(isamp)=="gjetsqcd") fullhistname.ReplaceAll("sr","crgj");
      if (names.at(isamp)=="dyjets") fullhistname.ReplaceAll("sr","crdy");
      TH1D* h = (TH1D*) samples.at(isamp)->Get(fullhistname);
      double yield = 0.;
      double err = 0.;
      if (h) {
        // not last bin
        if(ibin != n_mt2bins) {
          yield = h->GetBinContent(ibin);
          err = h->GetBinError(ibin);
          bgtot.at(ibin-1) += yield;
          bgerr.at(ibin-1) = sqrt(pow(bgerr.at(ibin-1),2) + pow(err,2));
        }
        // last bin: include overflow
        else if (ibin == h->GetXaxis()->GetNbins()) {
          yield = h->IntegralAndError(ibin,-1,err);
          bgtot.at(ibin-1) += yield;
          bgerr.at(ibin-1) = sqrt(pow(bgerr.at(ibin-1),2) + pow(err,2));
        }
        else {
          ofile << "Shouldn't get here" << std::endl;
          return;
        }
      }
      if (yield > 10.) {
        //  	ofile << "  &  " << Form("%.0f $\\pm$ %.0f",yield,err);
        ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
      } else {
        //  	ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
        ofile << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
      }
    }
    ofile << " \\\\" << endl;
  } // loop over samples

  // print bg totals
  ofile << "\\hline" << endl;
  ofile << "Total SM";
  for ( int ibin = 0; ibin < n_mt2bins; ++ibin ) {
    double yield = bgtot.at(ibin);
    double err = bgerr.at(ibin);
    if (yield > 10.) {
      //  	ofile << "  &  " << Form("%.0f $\\pm$ %.0f",yield,err);
      ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
    } else {
      //  	ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
      ofile << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
    }
  }
  ofile << " \\\\" << endl;
  ofile << "\\hline" << endl;

  for( unsigned int jsamp = 0 ; jsamp < n ; jsamp++ ){
    if( !TString(names.at(jsamp)).Contains("data") ) continue;
    ofile << getTableName(names.at(jsamp));
    for (int ibin = 1; ibin <= n_mt2bins; ++ibin) {
      TString fullhistname = Form("%s/h_mt2bins",dir.c_str());
      TH1D* h = (TH1D*) samples.at(jsamp)->Get(fullhistname);
      int yield = 0;
      if (h) {
        // not last bin
        if(ibin != n_mt2bins) {
          yield = round(h->GetBinContent(ibin));
        }
        // last bin: include overflow
        else if (ibin == h->GetXaxis()->GetNbins()) {
          yield = round(h->Integral(ibin, -1));
        }
        else {
          ofile << "Shouldn't get here" << std::endl;
          return;
        }
      }
      ofile << "  &  " << yield;
    }
    ofile << " \\\\" << endl;
  } // loop over samples

  for( unsigned int jsamp = 0 ; jsamp < n ; jsamp++ ){
    if( !TString(names.at(jsamp)).Contains("sig") ) continue;
    ofile << getTableName(names.at(jsamp));
    for (int ibin = 1; ibin <= n_mt2bins; ++ibin) {
      TString fullhistname = Form("%s/h_mt2bins",dir.c_str());
      TH1D* h = (TH1D*) samples.at(jsamp)->Get(fullhistname);
      double yield = 0.;
      double err = 0.;
      if (h) {
        // not last bin
        if(ibin != n_mt2bins) {
          yield = h->GetBinContent(ibin);
          err = h->GetBinError(ibin);
        }
        // last bin: include overflow
        else if (ibin == h->GetXaxis()->GetNbins()) {
          yield = h->IntegralAndError(ibin,-1,err);
        }
        else {
          ofile << "Shouldn't get here" << std::endl;
          return;
        }
      }
      if (yield > 10.) {
        //  	ofile << "  &  " << Form("%.0f $\\pm$ %.0f",yield,err);
        ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
      } else {
        //  	ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
        ofile << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
      }
    }
    ofile << " \\\\" << endl;
  } // loop over samples


  ofile << "\\hline" << std::endl;
  ofile << "\\end{tabular}" << std::endl;
  ofile << "\\end{table}" << std::endl;
  //ofile << "\\pagebreak" << std::endl;
  ofile << std::endl;

  return;
}

TH1F* fillSRYieldsPlot(vector<TFile*> samples, vector<string> dirs, string suffix = "", int n_srbins = 21) {
  const unsigned int nsamp = samples.size();
  const unsigned int ndirs = dirs.size();

  TH1F* srhist = new TH1F(Form("h_sryields%s", suffix.c_str()), "SR Yields Hist", n_srbins, 0, n_srbins);

  vector<double> bgtot(n_srbins, 0.);
  vector<double> bgerr(n_srbins, 0.);

  int srbin = 0;
  for (unsigned int idir = 0; idir < ndirs; ++idir) {
    string dir = dirs.at(idir);
    TString nbinshistname = Form("%s/h_n_mt2bins", dir.c_str());
    TH1F* h_n_mt2bins = (TH1F*) samples.at(0)->Get(nbinshistname);
    int n_mt2bins;
    if (h_n_mt2bins) {
      n_mt2bins = h_n_mt2bins->GetBinContent(1);
    } else {
      ofile << "Couldn't get number of mt2 bins" << std::endl;
      return srhist;
    }

    for (int ibin = 1; ibin <= n_mt2bins; ++ibin) {
      for (int isamp = 0 ; isamp < nsamp; ++isamp) {
        // if (TString(names.at(isamp)).Contains("data")) continue;
        // if (TString(names.at(isamp)).Contains("sig")) continue;
        // TString fullhistname = Form("%s/h_mt2bins%s", dir.c_str(), suffix.c_str());
        TString fullhistname = dir + "/h_mt2bins" + suffix;
        TH1D* h = (TH1D*) samples.at(isamp)->Get(fullhistname);
        double yield = 0.;
        double err = 0.;
        if (h) {
          // not last bin
          if(ibin != n_mt2bins) {
            yield = h->GetBinContent(ibin);
            err = h->GetBinError(ibin);
            bgtot.at(srbin) += yield;
            bgerr.at(srbin) = sqrt(pow(bgerr.at(srbin),2) + pow(err,2));
          }
          // last bin: include overflow
          else if (ibin == h->GetXaxis()->GetNbins()) {
            yield = h->IntegralAndError(ibin, -1, err);
            bgtot.at(srbin) += yield;
            bgerr.at(srbin) = sqrt(pow(bgerr.at(srbin),2) + pow(err,2));
          }
          else {
            ofile << "Shouldn't get here" << std::endl;
            ofile << "n_mt2bins: " << n_mt2bins << " h->XaxisNbins: " << h->GetXaxis()->GetNbins() << endl;
            return srhist;
          }
          // srhist->Fill(srbin, yield);
        }
      } // loop over all samples
      ++srbin;
    }

    // print bg totals
    // ofile << "\\hline" << endl;
    // ofile << "Total SM";
    // for ( unsigned int idx = 0; idx < bgtot.size(); ++idx ) {
    //   double yield = bgtot.at(idx);
    //   double err = bgerr.at(idx);
    //   if (yield > 10.)
    //     ofile << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
    //   else
    //     ofile << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
    // }
    // ofile << " \\\\" << endl;
    // ofile << "\\hline" << endl;

  }

  for (int ibin = 0; ibin < n_srbins; ++ibin) {
    srhist->SetBinContent(ibin+1, bgtot[ibin]);
    srhist->SetBinError(ibin+1, bgerr[ibin]);
  }

  return srhist;
}

THStack* fillSRYieldsStackHist(vector<TFile*> samples, vector<string> dirs, vector<Color_t> colors, string suffix = "", int n_srbins = 21, float scale = 1.) {
  const unsigned int nsamp = samples.size();
  const unsigned int ndirs = dirs.size();

  THStack* srhstack = new THStack(Form("stack_sryields%s", suffix.c_str()), "SR Yields Hist");

  for (int isamp = 0 ; isamp < nsamp; ++isamp) {
    TH1F* srhist = new TH1F(Form("h_sryields%s_%d", suffix.c_str(), isamp), "SR Yields Hist", n_srbins, 0, n_srbins);
    int srbin = 0;
    vector<double> bgtot(n_srbins, 0.);
    vector<double> bgerr(n_srbins, 0.);
    for (unsigned int idir = 0; idir < ndirs; ++idir) {
      string dir = dirs.at(idir);
      TString nbinshistname = Form("%s/h_n_mt2bins", dir.c_str());
      TH1F* h_n_mt2bins = (TH1F*) samples.at(0)->Get(nbinshistname);
      int n_mt2bins;
      if (h_n_mt2bins) {
        n_mt2bins = h_n_mt2bins->GetBinContent(1);
      } else {
        cout << "Couldn't get number of mt2 bins!! in dir: " << dir << std::endl;
        return srhstack;
      }
      for (int ibin = 1; ibin <= n_mt2bins; ++ibin) {
        TString fullhistname = dir + "/h_mt2bins" + suffix;
        TH1D* h = (TH1D*) samples.at(isamp)->Get(fullhistname);
        double yield = 0.;
        double err = 0.;
        if (h) {
          // not last bin
          if(ibin != n_mt2bins) {
            bgtot.at(srbin) = h->GetBinContent(ibin);
            bgerr.at(srbin) = h->GetBinError(ibin);
          }
          // last bin: include overflow
          else if (ibin == h->GetXaxis()->GetNbins()) {
            bgtot.at(srbin) = h->IntegralAndError(ibin, -1, err);
            bgerr.at(srbin) = err;
          }
          else {
            cout << "Shouldn't get here" << std::endl;
            cout << "n_mt2bins: " << n_mt2bins << " h->XaxisNbins: " << h->GetXaxis()->GetNbins() << endl;
            return srhstack;
          }
          // srhist->Fill(srbin, yield);
        }
        ++srbin;
      } // loop over all samples
    }

    for (int ibin = 0; ibin < n_srbins; ++ibin) {
      srhist->SetBinContent(ibin+1, bgtot[ibin]*scale);
      srhist->SetBinError(ibin+1, bgerr[ibin]*scale);
    }
    srhist->SetFillColor(colors.at(isamp));
    srhstack->Add(srhist);
  }

  return srhstack;
}

void makeSRyieldsHist(vector<TFile*> samples, vector<string> names, string prefix, string selec = "", string bmetsuf = "", string mbbsuf = "", string sr = "sr") {
  float scale = 40/12.9;
  int n_srbins = 0;
  vector<string> dirsAll;
  TFile* samp = *(samples.end()-1);

  TIter it(samples[0]->GetListOfKeys());
  TKey* k;
  string keep = sr + prefix;
  string skip = keep + "base";
  while ((k = (TKey *)it())) {
    if (strncmp (k->GetTitle(), skip.c_str(), skip.length()) == 0) continue;
    if (strncmp (k->GetTitle(), keep.c_str(), keep.length()) == 0) { // it is a signal region
      string sr_string = k->GetTitle();
      if (bmetsuf != "" && *(sr_string.end()-1) != bmetsuf) continue;
      if (mbbsuf != "" && sr_string[3+sr.length()] != mbbsuf && sr_string != sr+"H1L") continue;
      dirsAll.push_back(sr_string);
      TH1F* nsrbin_hist = (TH1F*) samp->Get((sr_string + "/h_n_mt2bins").c_str());
      if (nsrbin_hist) n_srbins += nsrbin_hist->GetBinContent(1);
    }
  }

  TFile* f_data(0);
  TH1F* hSR_data(0);
  TH1F* h_ratio(0);
  for (int i = names.size()-1; i >= 0; --i) {
    if (names[i].find("data") == 0) {
      cout << "Found data!\n";
      f_data = samples[i];
      samples.erase(samples.begin()+i);
      names.erase(names.begin()+i);
      break;
    }
  }
  if (selec != "") selec = "_" + selec;
  vector<Color_t> colorsAll = {kRed-7, kOrange-2, kSpring-5, kAzure+7, kCyan-7, kMagenta-7, kTeal+6, kGray+2};
  vector<Color_t> colors;
  int color_shift = 1;
  if (sr == "crsl") color_shift = 2;
  if (sr == "crdy") color_shift = 3;
  if (sr == "crgj") color_shift = 0;
  for (unsigned int i = 0; i < samples.size(); ++i) colors.push_back(colorsAll[i+color_shift]);

  THStack* hSR_stk = fillSRYieldsStackHist(samples, dirsAll, colors, selec, n_srbins, scale);
  TH1F* hSR_all = fillSRYieldsPlot(samples, dirsAll, selec, n_srbins);
  hSR_all->SetName("h_sryields_all");
  hSR_all->Scale(scale);

  if (f_data) {
    cout << "There is data!!\n";
    hSR_data = fillSRYieldsPlot(vector<TFile*>{f_data}, dirsAll, selec, n_srbins);
    hSR_data->Scale(scale);
    h_ratio = (TH1F*) hSR_data->Clone("h_ratio");
    if (true) {                 // waiting for a good name for the bool
      for (int i = 1; i <= h_ratio->GetNbinsX(); ++i) {
        float bkgyield = hSR_all->GetBinContent(i);
        if (bkgyield == 0) bkgyield = 0.0001; // prevent divide by 0 <-- necessary?
        h_ratio->SetBinContent(i, h_ratio->GetBinContent(i)/bkgyield);
        h_ratio->SetBinError(i, h_ratio->GetBinError(i)/bkgyield);
      }
    } else {
      h_ratio->Divide(hSR_all);
    }
  }

  TCanvas* c0 = new TCanvas("c0", "c0", 800, 640);
  gStyle->SetOptStat("");
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetFrameBorderMode(0);
  // c0->SetCanvasSize(600, 700);
  TPad* mainPad;
  TPad* ratioPad(0);
  if (h_ratio) {
    mainPad = new TPad("1", "1", 0.0, 0.20, 1.0, 1.0);
    ratioPad = new TPad("2", "2", 0.0, 0.0, 1.0, 0.20);
  } else {
    c0->SetCanvasSize(800, 560);
    mainPad = new TPad("1", "1", 0.0, 0.0, 1.0, 1.0);
  }
  mainPad->SetTopMargin(0.08);
  mainPad->SetLeftMargin(0.12);
  mainPad->SetBottomMargin(0.22);
  mainPad->SetLogy();
  mainPad->Draw();
  if (ratioPad) {
    ratioPad->SetTopMargin(0.05);
    ratioPad->SetLeftMargin(0.12);
    ratioPad->Draw();
  }
  mainPad->cd();

  // hSR_all->SetLineColor(kOrange+1);
  // hSR_all->SetFillColor(kOrange+1);
  // hSR_all->SetFillStyle(3002);
  TString histtitle("SRyields Hist");
  if (sr != "sr") histtitle = sr + " yields Hist";
  if (prefix == "h") histtitle += " for hcand regions";
  if (prefix == "H") histtitle += " for Mbbmax regions";
  if (prefix == "Z") histtitle += " for Zcand regions";
  if (mbbsuf  != "") histtitle += " with mbb " + mbbsuf;
  if (bmetsuf != "") histtitle += " with bmet " + bmetsuf;
  hSR_all->SetTitle(histtitle);
  // for (unsigned int i = 0; i < dirsAll.size(); ++i)

  int idx = 1;
  for (int i = 0; i < dirsAll.size(); ++i) {
    int n_mt2bins = ((TH1F*) samp->Get((dirsAll[i] + "/h_n_mt2bins").c_str()))->GetBinContent(1);
    TH1F* h_mt2bins = (TH1F*) samp->Get((dirsAll[i] + "/h_mt2bins").c_str());
    for (int j = 1; j <= n_mt2bins; ++j) {
      string label;
      if (h_mt2bins) {
        int lower_edge = h_mt2bins->GetBinLowEdge(j);
        int upper_edge = h_mt2bins->GetBinLowEdge(j+1);
        label = string(dirsAll[i]).erase(0, sr.length()+1) + ": [" + to_string(lower_edge) + "," + ((upper_edge == 1500)? "#infty" : to_string(upper_edge)) + "]";
      } else {
        cout << "In " << dirsAll[i] << " i = " << i << " the h_mt2bins hist is not found!!\n";
      }
      hSR_all->GetXaxis()->SetBinLabel(idx, label.c_str());
      idx++;
    }
  }
  hSR_all->GetXaxis()->LabelsOption("v");

  hSR_all->GetYaxis()->SetRangeUser(0.1, 5000);
  hSR_all->Draw("hist");
  // hSR_data->SetFillColor(kAzure+7);
  // hSR_data->Draw("axissame");
  hSR_stk->Draw("histsame");
  hSR_stk->Draw("axissame");
  hSR_all->SetFillStyle(3244);
  hSR_all->SetFillColor(kGray+3);
  hSR_all->Draw("E2same");
  if (hSR_data) {
    hSR_data->SetMarkerStyle(20);
    hSR_data->SetMarkerColor(kBlack);
    hSR_data->Draw("same");
  }

  TLegend* leg = new TLegend(0.56, 0.76, 0.87, 0.89);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.042);
  // leg->SetNColumns(2);
  // leg->AddEntry(hSR_all, "original");
  if (hSR_stk->GetHists() == nullptr) { cout << "SR stack not getting histograms!\n"; return; }
  for (unsigned int i = 0; i < names.size(); ++i)
    leg->AddEntry(hSR_stk->GetHists()->At(i), names.at(i).c_str());
  leg->Draw("same");

  TH1F* h_axis_ratio(0);
  TH1F* h_bkgerr_ratio(0);
  TLine* line1(0);
  if (h_ratio) {
    h_axis_ratio = new TH1F("ratio_axis","", n_srbins, 0, n_srbins);
    line1 = new TLine(0, 1, n_srbins, 1);
    cout << "There is Ratio!\n";
    ratioPad->cd();
    h_ratio->SetMarkerStyle(20);
    h_axis_ratio->GetYaxis()->SetTitleOffset(0.24);
    h_axis_ratio->GetYaxis()->SetTitleSize(0.18);
    h_axis_ratio->GetYaxis()->SetNdivisions(7);
    h_axis_ratio->GetYaxis()->SetLabelSize(0.15);
    h_axis_ratio->GetYaxis()->SetRangeUser(0, 2);
    h_axis_ratio->GetYaxis()->SetTitle("data/MC  ");
    h_axis_ratio->GetXaxis()->SetTickLength(0.07);
    h_axis_ratio->GetXaxis()->SetTitleSize(0.);
    h_axis_ratio->GetXaxis()->SetLabelSize(0.);
    h_axis_ratio->Draw("axis");
    if (true) {
      h_bkgerr_ratio = (TH1F*) hSR_all->Clone("h_bkgerr_ratio");
      for (int i = 1; i <= h_bkgerr_ratio->GetNbinsX(); ++i) {
        float bkyield = h_bkgerr_ratio->GetBinContent(i);
        if (bkyield == 0) h_bkgerr_ratio->SetBinError(i, 0);
        else h_bkgerr_ratio->SetBinError(i, h_bkgerr_ratio->GetBinError(i)/bkyield);
        h_bkgerr_ratio->SetBinContent(i, 1);
        h_bkgerr_ratio->SetFillStyle(1003);
        h_bkgerr_ratio->SetFillColor(kGray);
        h_bkgerr_ratio->Draw("E2same");
      }
    }
    h_axis_ratio->Draw("axissame");
    line1->SetLineStyle(2);
    line1->Draw("same");
    h_ratio->Draw("same");
  }
  c0->SetTitle("Title");
  c0->SaveAs(Form("SRyieldsHist_%s%s%s%s%s.pdf", sr.c_str(), prefix.c_str(), bmetsuf.c_str(), mbbsuf.c_str(), selec.c_str()));

  delete hSR_all;
  delete hSR_data;
  delete hSR_stk;
  delete c0;
  delete leg;
  delete h_axis_ratio;
  delete h_bkgerr_ratio;
  delete line1;
}

void makeSRyieldsComparisonHist(vector<TFile*> samples, vector<string> names, string prefix, string selec = "", string bmetsuf = "", string mbbsuf = "") {
  int n_srbins = 0;
  vector<string> dirsAll;

  TIter it(samples[0]->GetListOfKeys());
  TKey* k;
  string keep = "sr" + prefix;
  string skip = keep + "base";
  while ((k = (TKey *)it())) {
    if (strncmp (k->GetTitle(), skip.c_str(), skip.length()) == 0) continue;
    if (strncmp (k->GetTitle(), keep.c_str(), keep.length()) == 0) { // it is a signal region
      string sr_string = k->GetTitle();
      if (bmetsuf != "" && *(sr_string.end()-1) != bmetsuf) continue;
      if (mbbsuf != "" && sr_string[5] != mbbsuf) continue;
      dirsAll.push_back(sr_string);
      n_srbins += ((TH1F*) samples[0]->Get((sr_string + "/h_n_mt2bins").c_str()))->GetBinContent(1);
    }
  }

  // // vector<string> dirsAll = {"1VL", "2VL", "1L", "2L", "1M", "2M", "3M", "1H", "2H", "3H"};
  // if (prefix == "h") {
  //   // dirsAll = vector<string>{"1VL", "1L", "1M", "3M", "1H", "3H"};
  //   if      (bmetsuf == "H") n_srbins = 21;
  //   else if (bmetsuf == "L") n_srbins = 23;
  // }
  // else if (prefix == "H") {
  //   dirsAll = vector<string>{"1L", "2L", "1M", "2M", "3M", "1H", "2H", "3H"};
  //   n_srbins = 21;
  //   // if (bmetsuf == "") {
  // }
  // for (auto it = dirsAll.begin(); it ! = dirsAll.end(); ++it) *it = prefix + *it + mbbsuf + bmetsuf;

  if (selec != "") selec = "_" + selec;
  vector<Color_t> colorsAll = {kOrange-2, kRed-7, kSpring-5, kAzure+7, kCyan-7, kMagenta-7, kTeal+6, kGray+2};
  vector<Color_t> colors;
  for (unsigned int i = 0; i < samples.size(); ++i) colors.push_back(colorsAll[i]);
  THStack* hSR_stk = fillSRYieldsStackHist(samples, dirsAll, colors, selec, n_srbins, 40/12.9);

  TH1F* hSR_org = fillSRYieldsPlot(samples, dirsAll, "_original", n_srbins);
  TH1F* hSR_mMT = fillSRYieldsPlot(samples, dirsAll, selec, n_srbins);
  TH1F* hRatio = (TH1F*) hSR_mMT->Clone("h_ratio");
  hRatio->Divide(hSR_mMT, hSR_org, 1, 1, "B");

  hSR_org->Scale(40/12.9);
  hSR_mMT->Scale(40/12.9);

  TCanvas* c0 = new TCanvas("c0", "c0", 800, 600);
  gStyle->SetOptStat("");
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetFrameBorderMode(0);
  // c0->SetCanvasSize(600, 700);
  TPad* mainPad = new TPad("1", "1", 0.0, 0.20, 1.0, 1.0);
  TPad* ratioPad = new TPad("2", "2", 0.0, 0.0, 1.0, 0.20);
  mainPad->SetTopMargin(0.08);
  mainPad->SetLeftMargin(0.12);
  mainPad->SetBottomMargin(0.22);
  ratioPad->SetTopMargin(0.05);
  ratioPad->SetLeftMargin(0.12);
  ratioPad->Draw();
  mainPad->SetLogy();
  mainPad->Draw();
  mainPad->cd();

  hSR_org->SetLineColor(kOrange+1);
  hSR_org->SetFillColor(kOrange+1);
  hSR_org->SetFillStyle(3002);
  // for (unsigned int i = 0; i < dirsAll.size(); ++i)
  if (prefix == "h" && bmetsuf == "H") {
    hSR_org->GetXaxis()->SetLabelSize(0.05);
    hSR_org->GetXaxis()->SetBinLabel(1, "1VL: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(2, "1VL: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(3, "2VL: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(4, "2VL: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(5, "1L: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(6, "1L: [350,450]");
    hSR_org->GetXaxis()->SetBinLabel(7, "1L: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(8, "2L: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(9, "2L: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(10, "1M: [200,450]");
    hSR_org->GetXaxis()->SetBinLabel(11, "1M: [450,550]");
    hSR_org->GetXaxis()->SetBinLabel(12, "1M: [550,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(13, "2M: [200,450]");
    hSR_org->GetXaxis()->SetBinLabel(14, "2M: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(15, "3M: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(16, "3M: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(17, "1H: [200,450]");
    hSR_org->GetXaxis()->SetBinLabel(18, "1H: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(19, "2H: [200,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(20, "3H: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(21, "3H: [350,#infty]");
  } else if (prefix == "h" && bmetsuf == "L") {
    hSR_org->GetXaxis()->SetLabelSize(0.05);
    hSR_org->GetXaxis()->SetBinLabel(1, "1VL: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(2, "1VL: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(3, "2VL: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(4, "2VL: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(5, "1L: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(6, "1L: [350,450]");
    hSR_org->GetXaxis()->SetBinLabel(7, "1L: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(8, "2L: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(9, "2L: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(10, "1M: [200,450]");
    hSR_org->GetXaxis()->SetBinLabel(11, "1M: [450,550]");
    hSR_org->GetXaxis()->SetBinLabel(12, "1M: [550,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(13, "2M: [200,450]");
    hSR_org->GetXaxis()->SetBinLabel(14, "2M: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(15, "3M: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(16, "3M: [350,450]");
    hSR_org->GetXaxis()->SetBinLabel(17, "3M: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(18, "1H: [200,450]");
    hSR_org->GetXaxis()->SetBinLabel(19, "1H: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(20, "2H: [200,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(21, "3H: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(22, "3H: [350,450]");
    hSR_org->GetXaxis()->SetBinLabel(23, "3H: [450,#infty]");
  } else if (bmetsuf == "") {
    hSR_org->GetXaxis()->SetLabelSize(0.05);
    // hSR_org->GetXaxis()->SetBinLabel(1, "1VL: [200,350]");
    // hSR_org->GetXaxis()->SetBinLabel(2, "1VL: [350,#infty]");
    // hSR_org->GetXaxis()->SetBinLabel(3, "2VL: [200,300]");
    // hSR_org->GetXaxis()->SetBinLabel(4, "2VL: [300,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(1, "1L: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(2, "1L: [350,450]");
    hSR_org->GetXaxis()->SetBinLabel(3, "1L: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(4, "2L: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(5, "2L: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(6, "1M: [200,400]");
    hSR_org->GetXaxis()->SetBinLabel(7, "1M: [400,600]");
    hSR_org->GetXaxis()->SetBinLabel(8, "1M: [600,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(9, "2M: [200,500]");
    hSR_org->GetXaxis()->SetBinLabel(10, "2M: [500,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(11, "3M: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(12, "3M: [350,500]");
    hSR_org->GetXaxis()->SetBinLabel(13, "3M: [500,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(14, "1H: [200,400]");
    hSR_org->GetXaxis()->SetBinLabel(15, "1H: [400,700]");
    hSR_org->GetXaxis()->SetBinLabel(16, "1H: [700,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(17, "2H: [200,500]");
    hSR_org->GetXaxis()->SetBinLabel(18, "2H: [500,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(19, "3H: [200,400]");
    hSR_org->GetXaxis()->SetBinLabel(20, "3H: [400,600]");
    hSR_org->GetXaxis()->SetBinLabel(21, "3H: [600,#infty]");
  }
  hSR_org->GetXaxis()->LabelsOption("v");

  hSR_org->GetYaxis()->SetRangeUser(0.1, 5000);
  hSR_org->Draw("hist");
  // hSR_mMT->SetFillColor(kAzure+7);
  // hSR_mMT->Draw("histsame");
  // hSR_mMT->Draw("axissame");
  hSR_stk->Draw("histsame");
  hSR_stk->Draw("axissame");

  TLegend* leg = new TLegend(0.56, 0.76, 0.87, 0.89);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.042);
  leg->SetNColumns(2);
  leg->AddEntry(hSR_org, "original");
  // if (selec == "mMTnHcand")
  //   leg->AddEntry(hSR_mMT, "HMTnHcand");
  // else if (selec == "ivmMTnHcand")
  //   leg->AddEntry(hSR_mMT, "LMTnHcand");
  for (unsigned int i = 0; i < names.size(); ++i) // hardcoded alert!
    leg->AddEntry(hSR_stk->GetHists()->At(i), names.at(i).c_str());
  // leg->AddEntry(hSR_stk->GetHists()->At(1), "lostlep");
  // leg->AddEntry(hSR_stk->GetHists()->At(2), "Zinv");
  leg->Draw("same");

  ratioPad->cd();
  hRatio->SetMarkerStyle(20);
  TH1F* h_axis_ratio = new TH1F("ratio_axis","", n_srbins, 0, n_srbins);
  h_axis_ratio->GetYaxis()->SetTitleOffset(0.24);
  h_axis_ratio->GetYaxis()->SetTitleSize(0.18);
  h_axis_ratio->GetYaxis()->SetNdivisions(7);
  h_axis_ratio->GetYaxis()->SetLabelSize(0.15);
  h_axis_ratio->GetYaxis()->SetRangeUser(0, 0.65);
  // if (selec == "mMTnHcand")
  //   h_axis_ratio->GetYaxis()->SetRangeUser(0, 0.4);
  // else if (selec == "ivmMTnHcand")
  //   h_axis_ratio->GetYaxis()->SetRangeUser(0, 0.8);
  h_axis_ratio->GetYaxis()->SetTitle("new/old  ");
  h_axis_ratio->GetXaxis()->SetTickLength(0.07);
  h_axis_ratio->GetXaxis()->SetTitleSize(0.);
  h_axis_ratio->GetXaxis()->SetLabelSize(0.);
  h_axis_ratio->Draw("axis");
  hRatio->Draw("same");
  c0->SaveAs(Form("SRyieldsHist_%s.pdf", selec.c_str()));
  // dataMCplotMaker(hSR_mMT, hbg, vector<string>{"original"});

  delete hSR_org;
  delete hSR_mMT;
  delete hSR_stk;
  delete c0;
  delete leg;
  delete h_axis_ratio;
}

void makeSigYieldsComparisonHist(vector<TFile*> samples, vector<TFile*> sigs, vector<string> signames, string bmetsuf, string selec = "") {
  int n_srbins = 21;

  // vector<string> dirsAll = {"1VL", "2VL", "1L", "2L", "1M", "2M", "3M", "1H", "2H", "3H"};
  vector<string> dirsAll = {"1VL", "2VL", "1L", "2L", "1M", "2M", "3M", "1H", "2H", "3H"};
  if (bmetsuf == "L") n_srbins = 23;
  else if (bmetsuf == "") {
    n_srbins = 21;
    dirsAll = vector<string>{"1L", "2L", "1M", "2M", "3M", "1H", "2H", "3H"};
  }

  if (selec != "") selec = "_" + selec;
  vector<Color_t> colorsAll = {kOrange-2, kRed-7, kSpring-5, kAzure+7, kCyan-7, kMagenta-7, kTeal+6, kGray+2};
  vector<Color_t> colors;
  for (unsigned int i = 0; i < samples.size(); ++i) colors.push_back(colorsAll[i]);
  THStack* hSR_stk = fillSRYieldsStackHist(samples, dirsAll, colors, "_" + selec, n_srbins, 40/12.9);

  // TH1F* hSR_org = fillSRYieldsPlot(samples, dirsAll, bmetsuf, "_original", n_srbins);
  TH1F* hSR_org = fillSRYieldsPlot(samples, dirsAll, selec, n_srbins);
  hSR_org->SetName("h_SRyields_org");
  // Only works for having only 1 signal for now. Should be using individual 
  TH1F* hSR_mMT = fillSRYieldsPlot(sigs, dirsAll, selec, n_srbins);
  TH1F* hRatio = (TH1F*) hSR_mMT->Clone("h_ratio");

  hSR_org->Scale(40/12.9);
  // hSR_mMT->Scale(40/12.9);
  hRatio->Divide(hSR_mMT, hSR_org, 1, 1, "B");

  TCanvas* c0 = new TCanvas("c0", "c0", 800, 600);
  gStyle->SetOptStat("");
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetFrameBorderMode(0);
  // c0->SetCanvasSize(600, 700);
  TPad* mainPad = new TPad("1", "1", 0.0, 0.20, 1.0, 1.0);
  TPad* ratioPad = new TPad("2", "2", 0.0, 0.0, 1.0, 0.20);
  mainPad->SetTopMargin(0.08);
  mainPad->SetLeftMargin(0.12);
  mainPad->SetBottomMargin(0.22);
  ratioPad->SetTopMargin(0.05);
  ratioPad->SetLeftMargin(0.12);
  ratioPad->Draw();
  mainPad->SetLogy();
  mainPad->Draw();
  mainPad->cd();

  hSR_org->SetLineColor(kOrange+1);
  hSR_org->SetFillColor(kOrange+1);
  // hSR_org->SetFillStyle(3002);
  // for (unsigned int i = 0; i < dirsAll.size(); ++i)
  if (bmetsuf == "_H") {
    hSR_org->GetXaxis()->SetLabelSize(0.05);
    hSR_org->GetXaxis()->SetBinLabel(1, "1VL: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(2, "1VL: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(3, "2VL: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(4, "2VL: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(5, "1L: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(6, "1L: [350,450]");
    hSR_org->GetXaxis()->SetBinLabel(7, "1L: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(8, "2L: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(9, "2L: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(10, "1M: [200,450]");
    hSR_org->GetXaxis()->SetBinLabel(11, "1M: [450,550]");
    hSR_org->GetXaxis()->SetBinLabel(12, "1M: [550,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(13, "2M: [200,450]");
    hSR_org->GetXaxis()->SetBinLabel(14, "2M: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(15, "3M: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(16, "3M: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(17, "1H: [200,450]");
    hSR_org->GetXaxis()->SetBinLabel(18, "1H: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(19, "2H: [200,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(20, "3H: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(21, "3H: [350,#infty]");
  } else if (bmetsuf == "_L") {
    hSR_org->GetXaxis()->SetLabelSize(0.05);
    hSR_org->GetXaxis()->SetBinLabel(1, "1VL: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(2, "1VL: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(3, "2VL: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(4, "2VL: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(5, "1L: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(6, "1L: [350,450]");
    hSR_org->GetXaxis()->SetBinLabel(7, "1L: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(8, "2L: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(9, "2L: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(10, "1M: [200,450]");
    hSR_org->GetXaxis()->SetBinLabel(11, "1M: [450,550]");
    hSR_org->GetXaxis()->SetBinLabel(12, "1M: [550,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(13, "2M: [200,450]");
    hSR_org->GetXaxis()->SetBinLabel(14, "2M: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(15, "3M: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(16, "3M: [350,450]");
    hSR_org->GetXaxis()->SetBinLabel(17, "3M: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(18, "1H: [200,450]");
    hSR_org->GetXaxis()->SetBinLabel(19, "1H: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(20, "2H: [200,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(21, "3H: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(22, "3H: [350,450]");
    hSR_org->GetXaxis()->SetBinLabel(23, "3H: [450,#infty]");
  } else if (bmetsuf == "") {
    hSR_org->GetXaxis()->SetLabelSize(0.05);
    // hSR_org->GetXaxis()->SetBinLabel(1, "1VL: [200,350]");
    // hSR_org->GetXaxis()->SetBinLabel(2, "1VL: [350,#infty]");
    // hSR_org->GetXaxis()->SetBinLabel(3, "2VL: [200,300]");
    // hSR_org->GetXaxis()->SetBinLabel(4, "2VL: [300,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(1, "1L: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(2, "1L: [350,450]");
    hSR_org->GetXaxis()->SetBinLabel(3, "1L: [450,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(4, "2L: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(5, "2L: [350,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(6, "1M: [200,400]");
    hSR_org->GetXaxis()->SetBinLabel(7, "1M: [400,600]");
    hSR_org->GetXaxis()->SetBinLabel(8, "1M: [600,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(9, "2M: [200,500]");
    hSR_org->GetXaxis()->SetBinLabel(10, "2M: [500,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(11, "3M: [200,350]");
    hSR_org->GetXaxis()->SetBinLabel(12, "3M: [350,500]");
    hSR_org->GetXaxis()->SetBinLabel(13, "3M: [500,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(14, "1H: [200,400]");
    hSR_org->GetXaxis()->SetBinLabel(15, "1H: [400,700]");
    hSR_org->GetXaxis()->SetBinLabel(16, "1H: [700,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(17, "2H: [200,500]");
    hSR_org->GetXaxis()->SetBinLabel(18, "2H: [500,#infty]");
    hSR_org->GetXaxis()->SetBinLabel(19, "3H: [200,400]");
    hSR_org->GetXaxis()->SetBinLabel(20, "3H: [400,600]");
    hSR_org->GetXaxis()->SetBinLabel(21, "3H: [600,#infty]");
  }
  hSR_org->GetXaxis()->LabelsOption("v");

  hSR_org->GetYaxis()->SetRangeUser(0.1, 2000);
  hSR_org->Draw("hist");
  // hSR_mMT->SetFillColor(kAzure+7);
  hSR_mMT->SetLineColor(kAzure-3);
  hSR_mMT->Draw("histsame");
  hSR_mMT->Draw("axissame");
  // hSR_stk->Draw("histsame");
  // hSR_stk->Draw("axissame");

  TLegend* leg = new TLegend(0.56, 0.76, 0.87, 0.89);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.042);
  // leg->SetNColumns(2);
  leg->AddEntry(hSR_org, "Total SM");
  leg->AddEntry(hSR_mMT, signames[0].c_str()); // hardcoded alert
  // if (selec == "mMTnHcand")
  //   leg->AddEntry(hSR_mMT, "HMTnHcand");
  // else if (selec == "ivmMTnHcand")
  //   leg->AddEntry(hSR_mMT, "LMTnHcand");
  // for (unsigned int i = 0; i < names.size(); ++i) // hardcoded alert!
    // leg->AddEntry(hSR_stk->GetHists()->At(i), names.at(i).c_str());
  // leg->AddEntry(hSR_stk->GetHists()->At(1), "lostlep");
  // leg->AddEntry(hSR_stk->GetHists()->At(2), "Zinv");
  leg->Draw("same");

  ratioPad->cd();
  hRatio->SetMarkerStyle(20);
  TH1F* h_axis_ratio = new TH1F("ratio_axis","", n_srbins, 0, n_srbins);
  h_axis_ratio->GetYaxis()->SetTitleOffset(0.24);
  h_axis_ratio->GetYaxis()->SetTitleSize(0.18);
  h_axis_ratio->GetYaxis()->SetNdivisions(7);
  h_axis_ratio->GetYaxis()->SetLabelSize(0.15);
  h_axis_ratio->GetYaxis()->SetRangeUser(0, 1);
  // if (selec == "mMTnHcand")
  //   h_axis_ratio->GetYaxis()->SetRangeUser(0, 0.4);
  // else if (selec == "ivmMTnHcand")
  //   h_axis_ratio->GetYaxis()->SetRangeUser(0, 0.8);
  h_axis_ratio->GetYaxis()->SetTitle("sig/SM bkg");
  h_axis_ratio->GetXaxis()->SetTickLength(0.07);
  h_axis_ratio->GetXaxis()->SetTitleSize(0.);
  h_axis_ratio->GetXaxis()->SetLabelSize(0.);
  h_axis_ratio->Draw("axis");
  hRatio->Draw("same");
  c0->SaveAs(Form("sigYieldsCompareHist_%s.pdf", signames[0].c_str()));
  // dataMCplotMaker(hSR_mMT, hbg, vector<string>{"original"});

  bool printHistContent = true;
  if (printHistContent) {
    for (int i = 1; i <= hSR_org->GetNbinsX(); i++) {
      if (hSR_org->GetXaxis()->GetBinLabel(i)[0] == '1') continue;
      cout << "SR " << hSR_org->GetXaxis()->GetBinLabel(i) << " has bkgd: " << setprecision(3) << hSR_org->GetBinContent(i) << " +- "
           << hSR_org->GetBinError(i) << " while signal: " << hSR_mMT->GetBinContent(i) << " +- " << hSR_mMT->GetBinError(i) << endl;
    }
  }

  delete hSR_org;
  delete hSR_mMT;
  delete hSR_stk;
  delete c0;
  delete leg;
  delete h_axis_ratio;
}

vector<TFile*> getSamples(vector<string>& names, string input_dir) {
  vector<TFile*> samples;
  for (unsigned int i = 0; i < names.size(); ++i) {
    TFile* f_sample = new TFile(Form("%s/%s.root", input_dir.c_str(), names[i].c_str()));
    if (f_sample == nullptr) {
      cout << "Error: Cannot find file ", names[i], ".root ! Removing!\n";
      names.erase(names.begin() + i--);
    }
    else
      samples.push_back(f_sample);
  }

  return samples;
}


//______________Main_Looper_____________________________________
void plotMakerHcand() {

  //  gROOT->LoadMacro("CMS_lumi.C");
  cmsText = "CMS Preliminary";
  cmsTextSize = 0.5;
  lumiTextSize = 0.4;
  writeExtraText = false;
  //lumi_13TeV = "42 pb^{-1}";
  lumi_13TeV = "35.9 fb^{-1}";

  // ----------------------------------------
  //  control sequences
  // ----------------------------------------

  bool printplots = false;
  bool printtables = true;

  string input_dir = "/home/users/sicheng/working/MT2Analysis/MT2looper/output/temp";

  // ----------------------------------------
  //  samples definition
  // ----------------------------------------

  // vector<string> names = {"ttsl", "ttdl", "wjets_ht", "2015zinv_ht", "2015qcd_ht", "sig_T5qqqqWH_1400_700", "sig_T5qqqqWH_1400_200"};
  // vector<string> names = {"zinvDataDriven", "lostlepFromCRs", "qcd_ht", "data_Run2016"};
  vector<string> names = {"top", "wjets_ht", "zinv_ht", "qcdplusgjet", "dyjetsll_ht", "data_Run2016"};
  // vector<string> names = {"fakephoton", "qcdplusgjet", "data_Run2016"};
  vector<TFile*> samples = getSamples(names, input_dir);

  // ----------------------------------------
  //  plots definitions
  // ----------------------------------------

  float scalesig = -1.;
  // float scalesig = 30;
  // printplots = true;
  // bool doRatio = false;
  bool doRatio = true;
  bool scaleBGtoData = true;

  if (printplots) {
    TIter it(samples[0]->GetListOfKeys());
    TKey* k;
    std::string cr_skip = "cr";
    std::string sr_skip = "sr";
    while ((k = (TKey *)it())) {
      // if (strncmp (k->GetTitle(), cr_skip.c_str(), cr_skip.length()) == 0) continue; //skip control regions
      // if (strncmp (k->GetTitle(), sr_skip.c_str(), sr_skip.length()) == 0) continue; //skip signal regions and srbase
      std::string dir_name = k->GetTitle();
      // if (dir_name == "") continue;
      // if (dir_name != "srh2H") continue; //to do only this dir
      if (dir_name != "crgjhbase" && dir_name != "crslhbase" && dir_name != "crdyhbase" &&
          dir_name != "crgjHbase" && dir_name != "crslHbase" && dir_name != "crdyHbase" &&
          dir_name != "crgjZbase" && dir_name != "crslZbase" && dir_name != "crdyZbase") continue; //to do only these dirs
      // if (dir_name.find("crgjbase") != 0) continue; //for testing
      // string s = "_noHcandCR";
      string s = "";

      makePlot( samples , names , dir_name , "h_ht"+s  , "H_{T} [GeV]" , "Events / 50 GeV" , 0 , 1500 , 2 , true, printplots, scalesig, doRatio, scaleBGtoData );
      makePlot( samples , names , dir_name , "h_mt2"+s , "M_{T2} [GeV]" , "Events / 50 GeV" , 0 , 1500 , 2 , true, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_mt2"+s , "M_{T2} [GeV]" , "Events / 50 GeV" , 0 , 1000 , 2 , true, printplots, scalesig, doRatio, scaleBGtoData );
      makePlot( samples , names , dir_name , "h_met"+s  , "E_{T}^{miss} [GeV]" , "Events / 50 GeV" , 0 , 800 , 5 , true, printplots, scalesig, doRatio, scaleBGtoData );
      // // makePlot( samples , names , dir_name , "h_nlepveto"+s , "N(leptons)" , "Events" , 0 , 10 , 1 , true, printplots, scalesig, doRatio, scaleBGtoData );
      makePlot( samples , names , dir_name , "h_nJet30"+s , "N(jets)" , "Events" , 2 , 15 , 1 , true, printplots, scalesig, doRatio, scaleBGtoData );
      makePlot( samples , names , dir_name , "h_nBJet20"+s , "N(b jets)" , "Events" , 0 , 6 , 1 , true, printplots, scalesig, doRatio, scaleBGtoData );
      // // makePlot( samples , names , dir_name , "h_mt2bins"+s , "M_{T2} [GeV]" , "Events / Bin" , 200 , 1500 , 1 , true, printplots, scalesig, doRatio, scaleBGtoData );
      // // makePlot( samples , names , dir_name , "h_nJet30Eta3"+s , "N(jets, |#eta| > 3.0)" , "Events" , 0 , 5 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );

      // makePlot( samples , names , dir_name , "h_hcand_mt2"+s , "M_{T2} [GeV]" , "Events / 50 GeV" , 0 , 1000 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_minMTbmet"+s , "min(M_{T}^{bMET}) [GeV]" , "Events" , 0 , 600 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_MbbMax"+s , "max(M(bb)) [GeV]" , "Events" , 0 , 1000 , 2 , true, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_Mbbhcand"+s , "M(bb) (Hcand) [GeV]" , "Events" , 0 , 800 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_Mbbhcand"+s , "M(bb) (Hcand) [GeV]" , "Events" , 0 , 1000 , 3 , true, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_MbbZcand"+s , "M(bb) (Zcand) [GeV]" , "Events" , 0 , 1000 , 3 , true, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_nHcand"+s , "num H cands" , "Events" , 0 , 6 , 1 , true, printplots, scalesig, doRatio, scaleBGtoData );

      // makePlot( samples , names , dir_name , "h_deltaPhiminMTbmet"+s, "#Delta#phi (b, met)", "Events", -0.26, 3.4, 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_deltaPhiMinbmet"+s  , "#Delta#phi (b, met)", "Events", -0.26, 3.4, 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_deltaPhibbHcand"+s  , "#Delta#phi (b, met)", "Events", -0.26, 3.4, 1 , false, printplots, scalesig, doRatio, scaleBGtoData );

      // makePlot( samples , names , dir_name , "h_deltaPhiMinGenbmet"+s, "#Delta#phi (b, met)", "Events", -0.26, 3.4, 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_deltaPhiminMTgenbmet"+s, "#Delta#phi (b, met)", "Events", -0.26, 3.4, 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_deltaPhigenbgenb"+s, "#Delta#phi (b, b)", "Events", -0.26, 3.4, 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_test_diffmt2"+s , "M_{T2} [GeV]" , "Events / 50 GeV" , 0 , 400 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );

      if (dir_name.find("crslh") == 0) {
        makePlot( samples , names , dir_name , "h_mt"+s , "M_{T} [GeV]" , "Events" , 0 , 150 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
        makePlot( samples , names , dir_name , "h_leppt"+s , "p_{T}(lep) [GeV]" , "Events" , 0 , 200 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      }
    }
  }

  // Make SR Yields hist
  // makeSRyieldsComparisonHist(samples, names, "_H", "noHcandCR");
  // makeSRyieldsComparisonHist(samples, names, "_L", "isHcand");
  // makeSRyieldsComparisonHist(samples, names, "", "isZcand");
  // makeSRyieldsComparisonHist(samples, names, "", "MbbMax300");
  // makeSRyieldsComparisonHist(samples, names, "", "MbbMaxM");
  // makeSRyieldsComparisonHist(samples, names, "", "MbbMaxH");
  // makeSRyieldsComparisonHist(samples, names, "", "mMTnMbbM");
  // makeSRyieldsComparisonHist(samples, names, "", "ivmMTnMbbM");
  // makeSRyieldsComparisonHist(samples, names, "", "ivmMTnMbb300");

  // vector<string> signame = {"T5qqqqWH_1400_700_2h"};
  // vector<TFile*> sigs = getSamples(signame, input_dir);
  // makeSigYieldsComparisonHist(samples, sigs, signame,  "_H", "isHcand");

  // makeSRyieldsHist(samples, names, "h", "", "H");


  // Start outputing to file of yields table
  ofile.open("tables/table.tex");
  ofile << "\\documentclass[landscape,11pt]{article}" << std::endl;
  ofile << "\\usepackage{amsmath}" << std::endl;
  ofile << "\\usepackage{amssymb}" << std::endl;
  ofile << "\\usepackage{graphicx}" << std::endl;
  ofile << "\\usepackage[left=1in,top=1in,right=1in,bottom=1in,nohead]{geometry}" << std::endl;
  ofile << "\\usepackage{multirow}" << std::endl;
  ofile << "\\usepackage[table]{xcolor}" << std::endl;
  ofile << "\\usepackage{float}" << std::endl;
  ofile << "\\newcommand{\\MET}{\\rm{MET}}" << std::endl;
  ofile << "\\newcommand{\\mttwo}{\\ensuremath{M_{\\rm{T2}}\\,}}" << std::endl;
  ofile << "\\begin{document}" << std::endl << std::endl;

  // vector<string> dirs;
  // dirs.push_back("2bVL");
  // dirs.push_back("3bVL");
  // dirs.push_back("4bVL");
  // dirs.push_back("5bVL");

  // for(unsigned int i=0; i<dirs.size(); i++){
  //   printDetailedTable(samples, names, dirs.at(i));
  //   if(i % 2 != 0) std::cout << "\\pagebreak" << std::endl; //two tables per page
  // }

  vector<string> dirsH;
  // names = vector<string> {"ttsl", "ttdl", "wjets_ht", "singletop", "ttw", "ttz", "ttg", "2015zinv_ht"};

  vector<string> selecs{"original"};
  // vector<string> selecs = {"original", "minMTbmet", "isHcand", "mMTnHcand", "ivmMTnHcand"};
                           // "MbbMax200", "mMTnMbb200", "MbbMax300", "mMTnMbb300"};
  // vector<vector<TFile*>> samplesVec;
  // for (auto it = selecs.begin(); it != selecs.end(); ++it)
  //   samplesVec.push_back(getSamples(names, "/home/users/sicheng/MT2Analysis/MT2looper/output/" + *it));


  vector<string> names4 = {"qcdplusgjet", "zinvDataDriven", "lostlepFromCRs"};
  // vector<string> names4 = {"qcdplusgjet", "zinvDataDriven", "lostlepFromCRs", "data_Run2016"};
  vector<TFile*> samples4= getSamples(names4, input_dir);
  // names4 = vector<string>{"lostlep", "Zinv"};
  // makeSRyieldsHist(samples4, names4, "h", "", "H");
  // makeSRyieldsHist(samples4, names4, "h", "", "L");
  // makeSRyieldsHist(samples4, names4, "H", "", "", "U");
  // makeSRyieldsHist(samples4, names4, "H", "", "", "M");
  // makeSRyieldsHist(samples4, names4, "Z");

  // vector<string> names2 = {"qcdplusgjet", "wjets_ht", "top", "dyjetsll_ht", "zinv_ht", "data_Run2016"};
  vector<string> names2 = {"top", "dyjetsll_ht", "data_Run2016"};
  vector<TFile*> samples2= getSamples(names2, input_dir);
  // makeSRyieldsHist(samples2, names2, "h", "", "H", "", "crsl");
  // makeSRyieldsHist(samples2, names2, "h", "", "L", "", "crsl");
  // makeSRyieldsHist(samples2, names2, "H", "", "", "U", "crsl");
  // makeSRyieldsHist(samples2, names2, "H", "", "", "M", "crsl");
  // makeSRyieldsHist(samples2, names2, "Z", "", "", "",  "crsl");

  makeSRyieldsHist(samples2, names2, "h", "", "H", "", "crdy");
  makeSRyieldsHist(samples2, names2, "h", "", "L", "", "crdy");
  makeSRyieldsHist(samples2, names2, "H", "", "", "U", "crdy");
  makeSRyieldsHist(samples2, names2, "H", "", "", "M", "crdy");
  makeSRyieldsHist(samples2, names2, "Z", "", "", "",  "crdy");


  vector<string> names3 = {"fakephoton", "qcdplusgjet", "data_Run2016"};
  // vector<string> names3 = {"qcdplusgjet", "data_Run2016"};
  vector<TFile*> samples3= getSamples(names3, input_dir);
  // makeSRyieldsHist(samples3, names3, "h", "", "H", "", "crgj");
  // makeSRyieldsHist(samples3, names3, "h", "", "L", "", "crgj");
  // makeSRyieldsHist(samples3, names3, "H", "", "", "U", "crgj");
  // makeSRyieldsHist(samples3, names3, "H", "", "", "M", "crgj");
  // makeSRyieldsHist(samples3, names3, "Z", "", "", "",  "crgj");

  dirsH.push_back("srhbase");
  dirsH.push_back("srHbase");
  dirsH.push_back("srZbase");
  // printTable(samples, names, dirsH, "");
  // printComparisonTable(samples, names, vector<string>{"original"}, dirsH, "srhbase");
  // printComparisonRatioTable(samplesVec, names, selecs, dirsH, "srbase");
  dirsH.clear();

  // dirsH.push_back("crslhbase");
  // printComparisonTableCR(samples2, names2, selecs, dirsH, "crslhbase");
  // // printComparisonRatioTable(samplesVec, names, selecs, dirsH, "srbase");
  // dirsH.clear();

  // dirsH.push_back("crgjhbase");
  // printComparisonTableCR(samples3, names3, selecs, dirsH, "crgjhbase");
  // // printComparisonRatioTable(samplesVec, names, selecs, dirsH, "srbase");
  // dirsH.clear();

  // ofile << "\\newpage\n";

  // vector<string> names5 = {"lostlepFromCRs", "lostlep", "zinvDataDriven", "2015zinv_ht"};
  vector<string> mc_names = {"top", "wjets_ht", "zinv_ht", "qcd_ht"};
  vector<TFile*> mc_samples = getSamples(mc_names, input_dir);

  vector<string> dd_names = {"lostlepFromCRs", "zinvDataDriven"};
  vector<TFile*> dd_samples = getSamples(dd_names, input_dir);

  // dirsH.push_back("sr6VL");
  // dirsH.push_back("sr6L");
  // printDetailedTableDataDriven(dd_samples, dd_names, dirsH);
  // dirsH.clear();

  // dirsH.push_back("sr6M");
  // dirsH.push_back("sr6H");
  // printDetailedTableDataDriven(dd_samples, dd_names, dirsH);
  // dirsH.clear();

  dirsH.push_back("srh1VLH");
  dirsH.push_back("srh1LH");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();

  dirsH.push_back("srh1MH");
  dirsH.push_back("srh3MH");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();

  dirsH.push_back("srh1HH");
  dirsH.push_back("srh3HH");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();


  dirsH.push_back("srh1VLL");
  dirsH.push_back("srh1LL");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();

  dirsH.push_back("srh1ML");
  dirsH.push_back("srh3ML");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();

  dirsH.push_back("srh1HL");
  dirsH.push_back("srh3HL");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();

  ofile << "\\newpage\n";       // Here start the MbbMax (srH) regions

  dirsH.push_back("srH1L");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();

  dirsH.push_back("srH1MM");
  dirsH.push_back("srH3MM");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();

  dirsH.push_back("srH1MU");
  dirsH.push_back("srH3MU");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();

  dirsH.push_back("srH1HML");
  dirsH.push_back("srH3HML");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();

  dirsH.push_back("srH1HMH");
  dirsH.push_back("srH3HMH");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();

  dirsH.push_back("srH1HUL");
  dirsH.push_back("srH3HUL");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();

  dirsH.push_back("srH1HUH");
  dirsH.push_back("srH3HUH");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();


  ofile << "\\newpage\n";       // Here start the Zcand (srZ) regions

  dirsH.push_back("srZ1VL");
  dirsH.push_back("srZ1L");
  dirsH.push_back("srZ3L");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();

  dirsH.push_back("srZ1M");
  dirsH.push_back("srZ3M");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();

  dirsH.push_back("srZ1H");
  dirsH.push_back("srZ3H");
  // printDetailedTableDataDriven(samples, names, dd_samples, dd_names, dirsH);
  printDetailedTableDataDriven(mc_samples, mc_names, dd_samples, dd_names, dirsH);
  dirsH.clear();

  vector<string> sl_names = {"lostlepFromCRs", "lostlep", "data_Run2016"};
  vector<TFile*> sl_samples = getSamples(sl_names, input_dir);

  vector<string> gj_names = {"gjets_dr0p05_ht", "zinvDataDriven", "data_Run2016"};
  vector<TFile*> gj_samples = getSamples(gj_names, input_dir);

  // ofile << "\\newpage\n";       // Here start the CR yields for hcand

  dirsH.push_back("h1VLH");
  dirsH.push_back("h1LH");
  dirsH.push_back("h1MH");
  dirsH.push_back("h3MH");
  dirsH.push_back("h1HH");
  dirsH.push_back("h3HH");
  dirsH.push_back("h1VLL");
  dirsH.push_back("h1LL");
  dirsH.push_back("h1ML");
  dirsH.push_back("h3ML");
  dirsH.push_back("h1HL");
  dirsH.push_back("h3HL");
  printComparisonTableCRSL(sl_samples, sl_names, vector<string>{""}, dirsH, "", true);
  dirsH.clear();

  dirsH.push_back("H1L");
  dirsH.push_back("H1MU");
  dirsH.push_back("H3MU");
  dirsH.push_back("H1HUH");
  dirsH.push_back("H1HUL");
  dirsH.push_back("H3HUH");
  dirsH.push_back("H3HUL");
  dirsH.push_back("H1MM");
  dirsH.push_back("H3MM");
  dirsH.push_back("H1HMH");
  dirsH.push_back("H1HML");
  dirsH.push_back("H3HMH");
  dirsH.push_back("H3HML");
  printComparisonTableCRSL(sl_samples, sl_names, vector<string>{""}, dirsH, "", true);
  dirsH.clear();

  dirsH.push_back("h1VLH");
  dirsH.push_back("h1LH");
  dirsH.push_back("h1MH");
  dirsH.push_back("h3MH");
  dirsH.push_back("h1HH");
  dirsH.push_back("h3HH");
  dirsH.push_back("h1VLL");
  dirsH.push_back("h1LL");
  dirsH.push_back("h1ML");
  dirsH.push_back("h3ML");
  dirsH.push_back("h1HL");
  dirsH.push_back("h3HL");
  printComparisonTableCRSL(sl_samples, sl_names, vector<string>{""}, dirsH, "", true);
  dirsH.clear();

  dirsH.push_back("h1VLH");
  dirsH.push_back("h1LH");
  dirsH.push_back("h1MH");
  dirsH.push_back("h3MH");
  dirsH.push_back("h1HH");
  dirsH.push_back("h3HH");
  dirsH.push_back("h1VLL");
  dirsH.push_back("h1LL");
  dirsH.push_back("h1ML");
  dirsH.push_back("h3ML");
  dirsH.push_back("h1HL");
  dirsH.push_back("h3HL");
  // printComparisonTableCRGJ(gj_samples, gj_names, vector<string>{""}, dirsH, "", true);
  dirsH.clear();

  ofile << "\\newpage\n";       // Here start the CR yields for MbbMax

  // dirsH.push_back("H1L");
  // dirsH.push_back("H1MH");
  // dirsH.push_back("H3MH");
  // dirsH.push_back("H1HH");
  // dirsH.push_back("H3HH");
  // dirsH.push_back("H1VL");
  // dirsH.push_back("H1LL");
  // dirsH.push_back("H1ML");
  // dirsH.push_back("H3ML");
  // dirsH.push_back("H1HL");
  // dirsH.push_back("H3HL");
  // printComparisonTableCRSL(samples2, names2, vector<string>{""}, dirsH, "", true);
  // dirsH.clear();


  // dirsH.push_back("srH1VL");
  // dirsH.push_back("srH2VL");
  // dirsH.push_back("srH2VL");
  // printDetailedComparisonTable(samples, names, selecs2, dirsH);
  // dirsH.clear();


  ofile << "\\end{document}" << std::endl;

}
