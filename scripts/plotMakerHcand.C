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
#include "TLegend.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TKey.h"

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

  TLegend* leg = new TLegend(0.75,0.57,0.95,0.87);
  leg->SetFillColor(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.032);
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
    addOverflow(data_hist); // Add Overflow
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
    addOverflow(h); // Add Overflow
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
    addOverflow(h); // Add Overflow
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
    h_ratio->Sumw2();
    h_bgtot->Sumw2();


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
    can->Print(Form("plots/%s.pdf",canvas_name.Data()));
    //can->Print(Form("plots/%s.eps",canvas_name.Data()));
  }

  return can;
}

//_______________________________________________________________________________
void printComparisonTable(vector< vector<TFile*> > samplesVec, vector<string> names, vector<string> selecs, vector<string> dirs, string caption = "", bool dataMCratio = false, int colorReduction = 0, int mt2bin = -1) {

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

void printComparisonTableCRSL(vector< vector<TFile*> > samplesVec, vector<string> names, vector<string> selecs, vector<string> dirs, string caption = "", bool dataMCratio = false, int colorReduction = 0, int mt2bin = -1) {

  // read off yields from h_mt2bins hist in each topological region
  vector<TFile*> samples = samplesVec[0];

  unsigned int n = samples.size();
  unsigned int ndirs = dirs.size();
  unsigned int nselecs = selecs.size();

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
    ofile << "\\multirow{2}{*}{Sample}";
    for (unsigned int idir = 0; idir < ndirs; ++idir)
      ofile << " & \\multicolumn{" << nselecs << "}{c" << ((idir == ndirs-1)? "" : "|") << "}{ "
            // << " SR: " << getJetBJetTableLabel(samples.at(0), string(dirs.at(idir)).replace(0,5,"srh")) << ", 0 lep. "
            << " CR: " << getJetBJetTableLabel(samples.at(0), dirs.at(idir)) << ", 1 lep ($M_{\\rm{T}} < 100$)."
            << " }";
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
      // ofile << getTableName(names.at(i));
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistnameCR = Form("%s/h_mt2bins", dirs.at(idir).c_str());
          TString fullhistnameSR = Form("%s/h_mt2bins", string(dirs.at(idir)).replace(0,5,"srh").c_str());
          TH1D* h_cr = (TH1D*) samplesVec[isel].at(i)->Get(fullhistnameCR);
          TH1D* h_sr = (TH1D*) samplesVec[isel].at(i)->Get(fullhistnameSR);
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
      if( !TString(names.at(i)).Contains("data") ) continue;
      found_data = true;
      ofile << "Data CR";
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
        for ( unsigned int isel = 0; isel < nselecs; ++isel ) {
          TString fullhistname = Form("%s/h_mt2bins", dirs.at(idir).c_str());
          TH1D* h = (TH1D*) samplesVec[isel].at(i)->Get(fullhistname);
          double yield = 0.;
          if (h) {
            // use all bins
            yield = h->Integral(0,-1);
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

    if (itab == nTabulars-1) ofile << "\\hline" << std::endl;
    else ofile << "\\multicolumn{" << ndirs*nselecs+1 << "}{c}{} \\\\" << endl;
    ofile << "\\end{tabular}" << std::endl;
  }

  ofile << "\\end{table}" << std::endl;

  ofile << endl;
  return;
}

//_______________________________________________________________________________
void printComparisonRatioTable(vector< vector<TFile*> > samplesVec, vector<string> names, vector<string> selecs, vector<string> dirs, string caption = "", int colorReduction = 0, int mt2bin = -1) {

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
void printDetailedComparisonTable(vector< vector<TFile*> > samplesVec, vector<string> names, vector<string> selecs, vector<string> dirs, string caption = "", int colorReduction = 0) {
// void printDetailedComparisonTable( vector<TFile*> samples , vector<string> names , string dir) {

  // read off yields from h_mt2bins hist in each topological region
  vector<TFile*> samples = samplesVec[0];
  string dir = dirs[0];

  const unsigned int n = samples.size();

  unsigned int ndirs = dirs.size();
  unsigned int nselecs = selecs.size();
  if (nselecs < 2) cout << "Use printTable instead!\n";

  // vector< vector<string> > dirsVec;
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

vector<TFile*> getSamples(vector<string>& names, string input_dir) {
  vector<TFile*> samples;
  for (unsigned int i = 0; i < names.size(); ++i) {
    TFile* f_sample = new TFile(Form("%s/%s.root", input_dir.c_str(), names[i].c_str()));
    if (f_sample == nullptr) {
      cerr << "Error: Cannot find file ", names[i], ".root ! Removing!\n";
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
  lumi_13TeV = "20 fb^{-1}";

  // ----------------------------------------
  //  control sequences
  // ----------------------------------------

  bool printplots = false;
  bool printtables = true;

  // string input_dir = "/home/users/sicheng/MT2Analysis/MT2looper/output/V00-08-00_json_Cert_271036-273730_skim_base_mt2gt200_ZinvV4";
  string input_dir = "/home/users/sicheng/MT2Analysis/MT2looper/output/original";
  // string input_dir = "/home/users/sicheng/MT2Analysis/MT2looper/output/minMTBMet";
  // string input_dir = "/home/users/sicheng/MT2Analysis/MT2looper/output/MbbMax";
  // string input_dir = "/home/users/sicheng/MT2Analysis/MT2looper/output/temp";
  // string input_dir = "/home/users/sicheng/MT2Analysis/MT2looper/output/orgtrueb";

  // ----------------------------------------
  //  samples definition
  // ----------------------------------------

  vector<string> names = {"ttsl", "ttdl", "wjets_ht", "2015zinv_ht"};
  // vector<string> names{"ttsl", "ttdl", "T5qqqqWH_1400_700", "T5qqqqWH_1100_950", "T5qqqqWH_1400_200", "T2ttZH_800_400", "T2ttZH_800_200"};
  // vector<string> names{"ttsl", "ttdl", "wjets_ht", "singletop", "ttw", "ttz", "ttg", "data_Run2016B"};
  vector<TFile*> samples = getSamples(names, input_dir);

  // ----------------------------------------
  //  plots definitions
  // ----------------------------------------

  float scalesig = -1.;
  //float scalesig = 50.;
  // printplots = true;
  bool doRatio = false;
  bool scaleBGtoData = false;

  if(printplots){
    TIter it(samples[0]->GetListOfKeys());
    TKey* k;
    std::string cr_skip = "cr";
    std::string sr_skip = "sr";
    while ((k = (TKey *)it())) {
      // if (strncmp (k->GetTitle(), cr_skip.c_str(), cr_skip.length()) == 0) continue; //skip control regions
      if (strncmp (k->GetTitle(), sr_skip.c_str(), sr_skip.length()) == 0) continue; //skip signal regions and srbase
      std::string dir_name = k->GetTitle();
      if(dir_name == "") continue;
      if(dir_name != "crhslbase") continue; //to do only this dir
      //if(dir_name != "sr1H") continue; //for testing

      makePlot( samples , names , dir_name , "h_ht"  , "H_{T} [GeV]" , "Events / 50 GeV" , 0 , 1500 , 2 , false, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_mt2" , "M_{T2} [GeV]" , "Events / 50 GeV" , 100 , 1000 , 5 , false, printplots, scalesig, doRatio, scaleBGtoData );
      makePlot( samples , names , dir_name , "h_met"  , "E_{T}^{miss} [GeV]" , "Events / 50 GeV" , 0 , 800 , 5 , false, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_nlepveto" , "N(leptons)" , "Events" , 0 , 10 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      makePlot( samples , names , dir_name , "h_nJet30" , "N(jets)" , "Events" , 0 , 15 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      makePlot( samples , names , dir_name , "h_nBJet20" , "N(b jets)" , "Events" , 0 , 6 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_mt2bins" , "M_{T2} [GeV]" , "Events / Bin" , 200 , 1500 , 1 , true, printplots, scalesig, doRatio, scaleBGtoData );
      // makePlot( samples , names , dir_name , "h_nJet30Eta3" , "N(jets, |#eta| > 3.0)" , "Events" , 0 , 5 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );

      makePlot( samples , names , dir_name , "h_minMTbmet" , "M_{T}^{bMet} [GeV]" , "Events" , 0 , 600 , 2 , false, printplots, scalesig, doRatio, scaleBGtoData );
      makePlot( samples , names , dir_name , "h_MbbMax" , "M(bb) [GeV]" , "Events" , 0 , 600 , 2 , false, printplots, scalesig, doRatio, scaleBGtoData );

      if (dir_name.find("crhsl") == 0) {
        makePlot( samples , names , dir_name , "h_mt" , "M_{T} [GeV]" , "Events" , 0 , 150 , 2 , false, printplots, scalesig, doRatio, scaleBGtoData );
        makePlot( samples , names , dir_name , "h_leppt" , "p_{T}(lep) [GeV]" , "Events" , 0 , 200 , 2 , false, printplots, scalesig, doRatio, scaleBGtoData );
      }
    }
  }

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

  // vector<string> selecs{"original", "minMTBMet", "MbbMax"};
  vector<string> selecs = {"original", "minMTbmet", "isHcand", "mMTnHcand",
                           "MbbMax200", "mMTnMbb200", "MbbMax300", "mMTnMbb300"};
  vector< vector<TFile*> > samplesVec;
  for (auto it = selecs.begin(); it != selecs.end(); ++it)
    samplesVec.push_back(getSamples(names, "/home/users/sicheng/MT2Analysis/MT2looper/output/" + *it));

  // vector<string> names2 = {"lostlep", "data_Run2016B"};
  // vector< vector<TFile*> > samplesVec2;
  // for (auto it = selecs.begin(); it != selecs.end(); ++it)
  //   samplesVec2.push_back(getSamples(names2, "/home/users/sicheng/MT2Analysis/MT2looper/output/" + *it));

  vector<string> names3 = {"gjets_dr0p05_ht", "data_Run2016B"};
  vector< vector<TFile*> > samplesVec3;
  for (auto it = selecs.begin(); it != selecs.end(); ++it)
    samplesVec3.push_back(getSamples(names3, "/home/users/sicheng/MT2Analysis/MT2looper/output/" + *it));

  // dirsH.push_back("srbase");
  // dirsH.push_back("srbaseHcand");
  // // dirsH.push_back("srbaseInclHcand");
  // printTable(samples, names, dirsH);
  // dirsH.clear();
  // names = vector<string>{"ttsl", "ttdl", "sig_T5qqqqWH_1400_700", "sig_T5qqqqWH_1100_950", "sig_T5qqqqWH_1400_200", "sig_T2ttZH_800_400", "sig_T2ttZH_800_200"};

  dirsH.push_back("srhbase");
  printComparisonTable(samplesVec, names, selecs, dirsH, "srhbase");
  // printComparisonRatioTable(samplesVec, names, selecs, dirsH, "srbase");
  dirsH.clear();

  dirsH.push_back("crhgjbase");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "crhgjbase");
  // printComparisonRatioTable(samplesVec, names, selecs, dirsH, "srbase");
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl1VL");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj1VL");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh1VL");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();
  dirsH.push_back("crgj6VL");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl2VL");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj2VL");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh2VL");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl3VL");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj3VL");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh3VL");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl1L");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj1L");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh1L");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl2L");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj2L");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh2L");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl3L");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj3L");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh3L");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl1M");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj1M");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh1M");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl2M");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj2M");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh2M");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl3M");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj3M");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh3M");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl1H");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj1H");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh1H");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl2H");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj2H");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh2H");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl3H");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj3H");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh3H");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl1UH");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj1UH");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh1UH");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl2UH");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj2UH");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh2UH");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();

  ofile << "\\newpage\n";
  // dirsH.push_back("crhsl3UH");
  // printComparisonTableCRSL(samplesVec2, names2, selecs, dirsH, "Detailed");
  // dirsH.clear();
  dirsH.push_back("crhgj3UH");
  printComparisonTable(samplesVec3, names3, selecs, dirsH, "Detailed");
  dirsH.clear();
  dirsH.push_back("srh3UH");
  printDetailedComparisonTable(samplesVec, names, selecs, dirsH);
  dirsH.clear();

  ofile << "\\end{document}" << std::endl;

}
