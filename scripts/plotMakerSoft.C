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
#include "TGraphErrors.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TKey.h"

#include "plotUtilities.C"
#include "CMS_lumi.C"

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

//_______________________________________________________________________________
TCanvas* makePlot( const vector<TFile*>& samples , const vector<string>& names , const string& histdir , const string& histname , const string& xtitle , const string& ytitle , float xmin , float xmax , int rebin = 1 , bool logplot = true, bool printplot = false, float scalesig = -1., bool doRatio = false, bool scaleBGtoData = false ) {

  
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
  
  TLegend* leg = new TLegend(0.50,0.60,0.90,0.86);
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

  if (data_hist) leg->AddEntry(data_hist,getLegendName(data_name).c_str(),"pe1");
  
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
    //suffixes for soft lepton
    if (TString(names.at(i)).Contains("fakeLep") ) {
      fullhistname+="Fake";
    }
    if (TString(names.at(i)).Contains("onelep") ) {
      fullhistname+="Onelep";
    }
    if (TString(names.at(i)).Contains("dilep") && !fullhistname.Contains("missing")) {
      fullhistname+="Dilepton";
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
    h->SetLineWidth(4);
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

  if( logplot ) ymax*=50;
  else          ymax*=1.7;
  float ymin = 0.1;

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

  // add signal hists
  for (unsigned int isig = 0; isig < sig_hists.size(); ++isig) {
    sig_hists.at(isig)->Draw("hist same");
    TString legend_name = getLegendName(sig_names.at(isig));
    if (scalesig > 0.) legend_name += Form(" x %.0f",scalesig);
    leg->AddEntry(sig_hists.at(isig),legend_name,"l");
  }

  if (data_hist) data_hist->Draw("pe1 x0 same");

  TLatex label;
  label.SetNDC();
  label.SetTextSize(0.032);
  float label_x_start = 0.2;
  float label_y_start = 0.86;
  float label_y_spacing = 0.04;
  if (doRatio) {
    label.SetTextSize(0.039);
    label_y_start = 0.84;
    label_y_spacing = 0.04;
  }
  
  //TString ht_label = getHTPlotLabel(histdir);
  TString ht_label = getHTPlotLabel(samples.at(0), histdir);
  TString region_label = getMETPlotLabel(samples.at(0), histdir);
  TString region_label_line2 = getJetBJetPlotLabel2(samples.at(0), histdir);
  TString region_label_line3 = getMTPlotLabel(samples.at(0), histdir);

  //hardcode nocut fakeRate labels
  if (histdir.find("nocut") != std::string::npos) {
    ht_label = "";
    region_label = "E_{T}^{miss} < 200 GeV";
    region_label_line2 = "#geq 0j, #geq 0b";
    region_label_line3 = "";
  }
  else if (histdir.find("cr1L") != std::string::npos) {
    region_label = "E_{T}^{miss} < 60 GeV";
  }
    
  if (ht_label.Length() > 0) label.DrawLatex(label_x_start,label_y_start,ht_label);
  if (region_label.Length() > 0) label.DrawLatex(label_x_start,label_y_start - label_y_spacing,region_label);
  if (region_label_line2.Length() > 0) label.DrawLatex(label_x_start,label_y_start - 2 * label_y_spacing,region_label_line2);
  if (region_label_line3.Length() > 0) label.DrawLatex(label_x_start,label_y_start - 3 * label_y_spacing,region_label_line3);

  if (scaleBGtoData && data_hist) {
    TString scale_label = Form("MC scaled by %.2f #pm %.2f",bg_sf,bg_sf_err);
    label.DrawLatex(0.6,0.50,scale_label);
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
    h_ratio->Divide(h_bgtot);
    
    // draw axis only
    TH1F* h_axis_ratio = new TH1F(Form("%s_axes",h_ratio->GetName()),"",100,xmin,xmax);
    h_axis_ratio->GetYaxis()->SetTitleOffset(0.25);
    h_axis_ratio->GetYaxis()->SetTitleSize(0.18);
    h_axis_ratio->GetYaxis()->SetNdivisions(5);
    h_axis_ratio->GetYaxis()->SetLabelSize(0.15);
    //h_axis_ratio->GetYaxis()->SetRangeUser(0.5,1.5);
    h_axis_ratio->GetYaxis()->SetRangeUser(0.001,2.0);
    h_axis_ratio->GetYaxis()->SetTitle("Data/MC");
    h_axis_ratio->GetXaxis()->SetTickLength(0.07);
    h_axis_ratio->GetXaxis()->SetTitleSize(0.);
    h_axis_ratio->GetXaxis()->SetLabelSize(0.);
    h_axis_ratio->Draw("axis");

    // draw line at 1
    TLine* line1 = new TLine(xmin,1,xmax,1);
    line1->SetLineStyle(2);
    line1->Draw("same");

    TGraphErrors* g_ratio = new TGraphErrors(h_ratio);
    g_ratio->SetName(Form("%s_graph",h_ratio->GetName()));
    for (int ibin=0; ibin < h_ratio->GetNbinsX(); ++ibin) {
      //      g_ratio->SetPointError(ibin, h_ratio->GetBinWidth(ibin+1)/2., h_ratio->GetBinError(ibin+1));
      g_ratio->SetPointError(ibin, 0., h_ratio->GetBinError(ibin+1));
    }
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
void printTable( vector<TFile*> samples , vector<string> names , vector<string> dirs, int mt2bin = -1 ) {

  // read off yields from h_mt2bins hist in each topological region

  const unsigned int n = samples.size();
  const unsigned int ndirs = dirs.size();

  vector<double> bgtot(ndirs,0.);
  vector<double> bgerr(ndirs,0.);

  std::cout << "\\begin{table}[!ht]" << std::endl;
  std::cout << "\\scriptsize" << std::endl;
  std::cout << "\\centering" << std::endl;
  std::cout << "\\begin{tabular}{r";
  for (unsigned int idir=0; idir < ndirs; ++idir) std::cout << "|c";
  std::cout << "}" << std::endl;
  std::cout << "\\hline" << std::endl;

  cout << "\\hline" << endl
    << "Sample";

  // header
  for (unsigned int idir = 0; idir < ndirs; ++idir) {
    //cout << " & " << getRegionName(dirs.at(idir));
    cout << " & " << getJetBJetTableLabel(samples.at(0), dirs.at(idir));
  }
  cout << " \\\\" << endl
    << "\\hline\\hline" << endl;

  bool found_data = false;
  // backgrounds first -- loop backwards
  for( int i = n-1 ; i >= 0 ; --i ){
    if( TString(names.at(i)).Contains("data")  ) {found_data = true; continue;}
    if( TString(names.at(i)).Contains("sig")  ) continue;
    cout << getTableName(names.at(i));
    for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
      TString fullhistname = Form("%s/h_mtbins",dirs.at(idir).c_str());
      // if (names.at(i)=="gjets" || names.at(i)=="gjetsqcd") fullhistname.ReplaceAll("sr","crgj");
      // if (names.at(i)=="dyjets") fullhistname.ReplaceAll("sr","crdy");
      // if (names.at(i)=="wjets") fullhistname.ReplaceAll("sr","crrl");
      if (names.at(i)=="onelep") fullhistname += "Onelep";
      if (names.at(i)=="dilep") fullhistname += "Dilepton";
      if (names.at(i)=="fake") fullhistname += "Fake";
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
        //  	cout << "  &  " << Form("%.0f $\\pm$ %.0f",yield,err);
        cout << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
      } else {
        //  	cout << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
        cout << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
      }
    }
    cout << " \\\\" << endl;
  } // loop over samples

  // print bg totals
  cout << "\\hline" << endl;
  cout << "Total SM";
  for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
    double yield = bgtot.at(idir);
    double err = bgerr.at(idir);
    if (yield > 10.) {
      //  	cout << "  &  " << Form("%.0f $\\pm$ %.0f",yield,err);
      cout << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
    } else {
      //  	cout << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
      cout << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
    }
  }
  cout << " \\\\" << endl;
  cout << "\\hline" << endl;

  // next print data, if it exists
  if (found_data) {
    for( unsigned int i = 0 ; i < n ; i++ ){
      if( !TString(names.at(i)).Contains("data") ) continue;
      cout << getTableName(names.at(i));
      for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
	TString fullhistname = Form("%s/h_mtbins",dirs.at(idir).c_str());
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
	cout << "  &  " << Form("%d",(int)yield);
      }
      cout << " \\\\" << endl;
    } // loop over samples
    cout << "\\hline" << endl;
  } // if found_data
  
  // finally print signals
  for( unsigned int i = 0 ; i < n ; i++ ){
    if( !TString(names.at(i)).Contains("sig") ) continue;
    cout << getTableName(names.at(i));
    for ( unsigned int idir = 0; idir < ndirs; ++idir ) {
      TString fullhistname = Form("%s/h_mtbins",dirs.at(idir).c_str());
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
        //  	cout << "  &  " << Form("%.0f $\\pm$ %.0f",yield,err);
        cout << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
      } else {
        //  	cout << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
        cout << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
      }
    }
    cout << " \\\\" << endl;
  } // loop over samples

  std::cout << "\\end{tabular}" << std::endl;
  std::cout << "\\caption{}" << std::endl;
  std::cout << "\\end{table}" << std::endl;

  cout << endl;
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
void printDetailedTable( vector<TFile*> samples , vector<string> names , string dir) {

  // read off yields from h_mt2bins hist in each topological region

  const unsigned int n = samples.size();


  TString binshistname = Form("%s/h_mtbins",dir.c_str());
  TH1D* h_mt2bins(0);
  for(unsigned int i=0; i<samples.size(); i++){//need to find a sample that has this hist filled
    h_mt2bins = (TH1D*) samples.at(i)->Get(binshistname);
    if(h_mt2bins) break;
  }
  
  if(!h_mt2bins) std::cout << "Couldn't get the mt2 binning" << std::endl;
  int n_mt2bins = h_mt2bins->GetNbinsX();
  vector<double> bgtot(n_mt2bins,0.);
  vector<double> bgerr(n_mt2bins,0.);

  std::cout << "\\begin{table}[htbp]" << std::endl;
//  std::cout << "\\caption{Background and signal yields in bins of \\mttwo for ";
//  std::cout << getHTTableLabel(samples.at(0), dir) << ", ";
//  std::cout << getJetBJetTableLabel(samples.at(0), dir) << ". ";
//  std::cout << "The yields are normalized to \\lumival.}" << std::endl;
  std::cout << "\\scriptsize" << std::endl;
  std::cout << "\\centering" << std::endl;
  std::cout << "\\makebox[\\textwidth][c]{" << std::endl;
  std::cout << "\\begin{tabular}{r";
  for (int ibin=0; ibin < n_mt2bins; ++ibin) std::cout << "|c";
  std::cout << "}" << std::endl;
  std::cout << "\\hline" << std::endl;
  std::cout << "\\multicolumn{" << n_mt2bins+1 << "}{c}{";
  std::cout << getHTTableLabel(samples.at(0), dir) << ", ";
  std::cout << getMETTableLabel(samples.at(0), dir) << ", ";
  std::cout << getJetBJetTableLabel2(samples.at(0), dir);
  std::cout << "} \\\\" << std::endl;
  std::cout << "\\hline" << std::endl;
  std::cout << "\\hline" << endl
    << "Sample";

  // header
  for (int ibin = 1; ibin < n_mt2bins; ++ibin) {
    cout << " & " << h_mt2bins->GetXaxis()->GetBinLowEdge(ibin) << " $<$ $M_{T}$ $<$ " << h_mt2bins->GetXaxis()->GetBinLowEdge(ibin+1) << " GeV";
  }
  cout << " & $M_{T}$ $>$ " << h_mt2bins->GetXaxis()->GetBinLowEdge(n_mt2bins) << " GeV";
  cout << " \\\\" << endl
    << "\\hline\\hline" << endl;

  // backgrounds first -- loop backwards
  for( int isamp = n-1 ; isamp >= 0 ; --isamp ){
    if( TString(names.at(isamp)).Contains("data")  ) continue;
    if( TString(names.at(isamp)).Contains("sig")  ) continue;
    cout << getTableName(names.at(isamp));
    for (int ibin = 1; ibin <= n_mt2bins; ++ibin) {
      TString fullhistname = Form("%s/h_mtbins",dir.c_str());
      if (names.at(isamp)=="gjets" || names.at(isamp)=="gjetsqcd") fullhistname.ReplaceAll("sr","crgj");
      if (names.at(isamp)=="dyjets") fullhistname.ReplaceAll("sr","crdy");
      if (names.at(isamp)=="onelep") fullhistname += "Onelep";
      if (names.at(isamp)=="dilep") fullhistname += "Dilepton";
      if (names.at(isamp)=="fake") fullhistname += "Fake";
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
          std::cout << "Shouldn't get here" << std::endl;
          return;
        }
      }
      if (yield > 10.) {
        //  	cout << "  &  " << Form("%.0f $\\pm$ %.0f",yield,err);
        cout << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
      } else {
        //  	cout << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
        cout << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
      }
    }
    cout << " \\\\" << endl;
  } // loop over samples

  // print bg totals
  cout << "\\hline" << endl;
  cout << "Total SM";
  for ( int ibin = 0; ibin < n_mt2bins; ++ibin ) {
    double yield = bgtot.at(ibin);
    double err = bgerr.at(ibin);
    if (yield > 10.) {
      //  	cout << "  &  " << Form("%.0f $\\pm$ %.0f",yield,err);
      cout << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
    } else {
      //  	cout << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
      cout << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
    }
  }
  cout << " \\\\" << endl;
  cout << "\\hline" << endl;

  for( unsigned int jsamp = 0 ; jsamp < n ; jsamp++ ){
    if( !TString(names.at(jsamp)).Contains("sig") ) continue;
    cout << getTableName(names.at(jsamp));
    for (int ibin = 1; ibin <= n_mt2bins; ++ibin) {
      TString fullhistname = Form("%s/h_mtbins",dir.c_str());
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
          std::cout << "Shouldn't get here" << std::endl;
          return;
        }
      }
      if (yield > 10.) {
        //  	cout << "  &  " << Form("%.0f $\\pm$ %.0f",yield,err);
        cout << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
      } else {
        //  	cout << "  &  " << Form("%.1f $\\pm$ %.1f",yield,err);
        cout << "  &  " << Form("%.2f $\\pm$ %.2f",yield,err);
      }
    }
    cout << " \\\\" << endl;
  } // loop over samples


  std::cout << "\\hline" << std::endl;
  std::cout << "\\end{tabular}}" << std::endl;
  std::cout << "\\end{table}" << std::endl;
  //std::cout << "\\pagebreak" << std::endl;
  std::cout << std::endl;

  return;
}  

//_______________________________________________________________________________

void plotMakerSoftLepSRMissingLep(){

  //  gROOT->LoadMacro("CMS_lumi.C");
  cmsText = "CMS Preliminary";
  cmsTextSize = 0.5;
  lumiTextSize = 0.4;
  writeExtraText = false;
  lumi_13TeV = "2.26 fb^{-1}";

  string input_dir = "../SoftLepLooper/output/softLepSR";
  
  // ----------------------------------------
  //  samples definition
  // ----------------------------------------

  // get input files

  // TFile* f_tt1l = new TFile(Form("%s/ttsl.root",input_dir.c_str()));
  // TFile* f_tt2l = new TFile(Form("%s/ttdl.root",input_dir.c_str())); 
  // TFile* f_st = new TFile(Form("%s/singletop.root",input_dir.c_str())); 
  // TFile* f_wjets = new TFile(Form("%s/wjets_ht.root",input_dir.c_str()));
  TFile* f_dyjets = new TFile(Form("%s/dyjetsll.root",input_dir.c_str()));

  vector<TFile*> samples;
  vector<string>  names;

  //samples.push_back(f_st); names.push_back("st");
  //samples.push_back(f_tt1l); names.push_back("tt+1l");
  // samples.push_back(f_wjets); names.push_back("wjets");
  samples.push_back(f_dyjets); names.push_back("dyjets");
  // samples.push_back(f_tt2l); names.push_back("tt+2l");

  
  // ----------------------------------------
  //  plots definitions
  // ----------------------------------------

  float scalesig = -1.;
  //float scalesig = 50.;
  bool printplots = true;
  bool doRatio = true;
  bool scaleBGtoData = false;

  if(printplots){
    TIter it(f_dyjets->GetListOfKeys());
    TKey* k;
    std::string cr_skip = "cr";
    std::string sr_skip = "sr";
    while ((k = (TKey *)it())) {
      //if (strncmp (k->GetTitle(), cr_skip.c_str(), cr_skip.length()) == 0) continue; //skip control regions
      //if (strncmp (k->GetTitle(), sr_skip.c_str(), sr_skip.length()) == 0) continue; //skip signal regions and srbase
      std::string dir_name = k->GetTitle();
      if(dir_name == "srLepbase") {
      
	makePlot( samples , names , dir_name , "h_mtMissing" , "M_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 5 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_mtMissingLep" , "M_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 5 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_mtMissingLepFromTau" , "M_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 5 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_mtMissingTau" , "M_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 5 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_mtMissingTauDecay1" , "M_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 5 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_mtMissingTauDecay3" , "M_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 5 , false , printplots, scalesig, doRatio, scaleBGtoData );

	makePlot( samples , names , dir_name , "h_missinglepptMissing" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_missinglepptMissingLep" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_missinglepptMissingLepFromTau" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_missinglepptMissingTau" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_missinglepptMissingTauDecay1" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_missinglepptMissingTauDecay3" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );

	makePlot( samples , names , dir_name , "h_nJet30Missing" , "N(jets)" , "Events" , 0 , 15 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_nJet30MissingLep" , "N(jets)" , "Events" , 0 , 15 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_nJet30MissingLepFromTau" , "N(jets)" , "Events" , 0 , 15 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_nJet30MissingTau" , "N(jets)" , "Events" , 0 , 15 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_nJet30MissingTauDecay1" , "N(jets)" , "Events" , 0 , 15 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_nJet30MissingTauDecay3" , "N(jets)" , "Events" , 0 , 15 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );

	
	makePlot( samples , names , dir_name , "h_Events_wMissing" , "N(jets)" , "Events" , 0 , 15 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_Events_wMissingLep" , "N(jets)" , "Events" , 0 , 15 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_Events_wMissingLepFromTau" , "N(jets)" , "Events" , 0 , 15 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_Events_wMissingTau" , "N(jets)" , "Events" , 0 , 15 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_Events_wMissingTauDecay1" , "N(jets)" , "Events" , 0 , 15 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_Events_wMissingTauDecay3" , "N(jets)" , "Events" , 0 , 15 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
	
	makePlot( samples , names , dir_name , "h_missingdilepmllMissing" , "m_{ll} [GeV]" , "Events" , 0 , 150 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_missingdilepmllMissingLep" , "m_{ll} [GeV]" , "Events" , 0 , 150 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_missingdilepmllMissingLepFromTau" , "m_{ll} [GeV]" , "Events" , 0 , 150 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_missingdilepmllMissingTau" , "m_{ll} [GeV]" , "Events" , 0 , 150 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_missingdilepmllMissingTauDecay1" , "m_{ll} [GeV]" , "Events" , 0 , 150 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_missingdilepmllMissingTauDecay3" , "m_{ll} [GeV]" , "Events" , 0 , 150 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );

	makePlot( samples , names , dir_name , "h_trackdilepmllMissing" , "m_{ll} [GeV]" , "Events" , 0 , 150 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_trackdilepmllMissingLep" , "m_{ll} [GeV]" , "Events" , 0 , 150 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_trackdilepmllMissingLepFromTau" , "m_{ll} [GeV]" , "Events" , 0 , 150 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_trackdilepmllMissingTau" , "m_{ll} [GeV]" , "Events" , 0 , 150 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_trackdilepmllMissingTauDecay1" , "m_{ll} [GeV]" , "Events" , 0 , 150 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_trackdilepmllMissingTauDecay3" , "m_{ll} [GeV]" , "Events" , 0 , 150 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
    }
      
      if(dir_name == "cr2Lbase") {

	makePlot( samples , names , dir_name , "h_highlepptMissing" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_highlepptMissingLep" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_highlepptMissingLepFromTau" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_highlepptMissingTau" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_highlepptMissingTauDecay1" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_highlepptMissingTauDecay3" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , false , printplots, scalesig, doRatio, scaleBGtoData );

	//	log plots
	makePlot( samples , names , dir_name , "h_highlepptMissing" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , true , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_highlepptMissingLep" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , true , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_highlepptMissingLepFromTau" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , true , printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_highlepptMissingTau" , "Lepton p_{T} [GeV]" , "Events / 5 GeV" , 0 , 500 , 1 , true , printplots, scalesig, doRatio, scaleBGtoData );

	makePlot( samples , names , dir_name , "h_nJet30Missing" , "N(jets)" , "Events" , 0 , 15 , 1 , true, printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_nJet30MissingLep" , "N(jets)" , "Events" , 0 , 15 , 1 , true, printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_nJet30MissingLepFromTau" , "N(jets)" , "Events" , 0 , 15 , 1 , true, printplots, scalesig, doRatio, scaleBGtoData );
	makePlot( samples , names , dir_name , "h_nJet30MissingTau" , "N(jets)" , "Events" , 0 , 15 , 1 , true, printplots, scalesig, doRatio, scaleBGtoData );

      }
      
    }
  }
}

void plotMakerSoftLepFR(){

  //  gROOT->LoadMacro("CMS_lumi.C");
  cmsText = "CMS Preliminary";
  cmsTextSize = 0.5;
  lumiTextSize = 0.4;
  writeExtraText = false;
  lumi_13TeV = "2.26 fb^{-1}";

 //bool doOnlyTrue = false;
  bool doData = true;
  bool doAll = true;
  bool doSRMC = false;
  string input_dir = "../SoftLepLooper/output/softLepFake";

  string prefix = "EWKcuts";
  
  vector<string> suffixes;
  if (!doSRMC && doAll) {
    suffixes.push_back("HT800ElTight");
    suffixes.push_back("HT600ElTight");
    suffixes.push_back("HT475ElTight");
    suffixes.push_back("HT350ElTight");
    suffixes.push_back("HT200ElTight");
    suffixes.push_back("HT800MuTight");
    suffixes.push_back("HT600MuTight");
    suffixes.push_back("HT475MuTight");
    suffixes.push_back("HT350MuTight");
    suffixes.push_back("HT200MuTight");
  }
  else if (doSRMC) {
    suffixes.push_back("ElTightFake");
    suffixes.push_back("MuTightFake");
  }
  else suffixes.push_back("HT800ElTight");
 
  // ----------------------------------------
  //  samples definition
  // ----------------------------------------

  // get input files
  
  TFile* f_top = new TFile(Form("%s/top.root",input_dir.c_str())); //hadd'ing of ttbar, ttw, ttz, tth, singletop
  // TFile* f_tt1l = new TFile(Form("%s/ttsl.root",input_dir.c_str()));
  // TFile* f_tt2l = new TFile(Form("%s/ttdl.root",input_dir.c_str())); 
  // TFile* f_st = new TFile(Form("%s/singletop.root",input_dir.c_str())); 
  TFile* f_wjets = new TFile(Form("%s/wjets_ht.root",input_dir.c_str()));
  TFile* f_qcd = new TFile(Form("%s/qcd_ht.root",input_dir.c_str()));
  TFile* f_zinv = new TFile(Form("%s/zinv_ht.root",input_dir.c_str()));
  TFile* f_ww = new TFile(Form("%s/ww.root",input_dir.c_str()));
  TFile* f_wz = new TFile(Form("%s/wz.root",input_dir.c_str()));
  TFile* f_zz = new TFile(Form("%s/zz.root",input_dir.c_str()));
  TFile* f_diboson = new TFile(Form("%s/diboson.root",input_dir.c_str()));
  TFile* f_dy = new TFile(Form("%s/dyjetsll.root",input_dir.c_str()));
  TFile* f_bkg = new TFile(Form("%s/allBkg.root",input_dir.c_str())); 
  TFile* f_data = new TFile(Form("%s/data_Run2015CD.root",input_dir.c_str())); 

  vector<TFile*> samples;
  vector<string>  names;
  
  if (!doData) {
    samples.push_back(f_qcd); names.push_back("qcd");
    samples.push_back(f_zinv); names.push_back("zinv");
    samples.push_back(f_dy); names.push_back("dyjets");
    samples.push_back(f_diboson); names.push_back("diboson");
    samples.push_back(f_top); names.push_back("top");
    samples.push_back(f_wjets); names.push_back("wjets");
  }

  else if (!doSRMC) {   
    // samples.push_back(f_qcd); names.push_back("qcd");
    // samples.push_back(f_zinv); names.push_back("zinv");
    samples.push_back(f_diboson); names.push_back("diboson");
    samples.push_back(f_dy); names.push_back("dyjets");
    samples.push_back(f_top); names.push_back("top");
    samples.push_back(f_wjets); names.push_back("wjets");
    samples.push_back(f_bkg); names.push_back("fakeLep");
    samples.push_back(f_data); names.push_back("data");
  }
  else if (doSRMC) {
    // samples.push_back(f_qcd); names.push_back("qcd");
    // samples.push_back(f_zinv); names.push_back("zinv");
    samples.push_back(f_diboson); names.push_back("diboson");
    samples.push_back(f_dy); names.push_back("dyjets");
    samples.push_back(f_top); names.push_back("top");
    samples.push_back(f_wjets); names.push_back("wjets");
    samples.push_back(f_bkg); names.push_back("fakeLep");
  }
  
  // ----------------------------------------
  //  plots definitions
  // ----------------------------------------

  float scalesig = -1.;
  //float scalesig = 50.;
  bool printplots = true;
  bool doRatio = true;
  bool scaleBGtoData = true;

  if(printplots){
    TIter it(f_wjets->GetListOfKeys());
    TKey* k;
    std::string cr_skip = "cr";
    std::string sr_skip = "sr";
    while ((k = (TKey *)it())) {
      //if (strncmp (k->GetTitle(), cr_skip.c_str(), cr_skip.length()) == 0) continue; //skip control regions
      //if (strncmp (k->GetTitle(), sr_skip.c_str(), sr_skip.length()) == 0) continue; //skip signal regions and srbase
      std::string dir_name = k->GetTitle();
      if(dir_name != "nocut" && dir_name != "srLepbase") continue; //to do only this dir

      //loop once for log, once for no log
      if (dir_name == "nocut" && !doSRMC) {
	for (unsigned int isuf = 0; isuf < suffixes.size(); isuf++) {
	  for (unsigned int ilog = 0; ilog < 2; ilog++) {
	    bool doLog = ilog==0;
	    makePlot( samples , names , dir_name , "h_lepptshort"+prefix+suffixes[isuf] , "Lepton p_{T} [GeV]" , "Events / 1 GeV" , 3 , 22 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	    makePlot( samples , names , dir_name , "h_softlepht"+prefix+suffixes[isuf]  , "H_{T} [GeV]" , "Events / 25 GeV" , 0 , 2000 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	    makePlot( samples , names , dir_name , "h_met"+prefix+suffixes[isuf]  , "E_{T}^{miss} [GeV]" , "Events / 10 GeV" , 0 , 200 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	    // makePlot( samples , names , dir_name , "h_nJet30"+prefix+suffixes[isuf] , "N(jets)" , "Events" , 0 , 15 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	    // makePlot( samples , names , dir_name , "h_nBJet20"+prefix+suffixes[isuf] , "N(b jets)" , "Events" , 0 , 6 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	    // makePlot( samples , names , dir_name , "h_categoryB"+prefix+suffixes[isuf] , "B-Category" , "Events" , 0 , 4 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	    makePlot( samples , names , dir_name , "h_mt"+prefix+suffixes[isuf] , "M_{T} [GeV]" , "Events / 5 GeV" , 0 , 200 , 5 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	    makePlot( samples , names , dir_name , "h_Events_w"+prefix+suffixes[isuf] , "Events" , "Events" , 0 , 2 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	    makePlot( samples , names , dir_name , "h_Events"+prefix+suffixes[isuf] , "Events" , "Events" , 0 , 2 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  }    
	}
      }

      if (dir_name == "srLepbase" && doSRMC) {
	for (unsigned int isuf = 0; isuf < suffixes.size(); isuf++) {
	  for (unsigned int ilog = 0; ilog < 2; ilog++) {
	    bool doLog = ilog==0;
	    makePlot( samples , names , dir_name , "h_lepptshort"+prefix+suffixes[isuf] , "Lepton p_{T} [GeV]" , "Events / 1 GeV" , 3 , 22 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	    makePlot( samples , names , dir_name , "h_softlepht"+prefix+suffixes[isuf]  , "H_{T} [GeV]" , "Events / 25 GeV" , 0 , 2000 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	    makePlot( samples , names , dir_name , "h_met"+prefix+suffixes[isuf]  , "E_{T}^{miss} [GeV]" , "Events / 10 GeV" , 0 , 1000 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	    // makePlot( samples , names , dir_name , "h_nJet30"+prefix+suffixes[isuf] , "N(jets)" , "Events" , 0 , 15 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	    // makePlot( samples , names , dir_name , "h_nBJet20"+prefix+suffixes[isuf] , "N(b jets)" , "Events" , 0 , 6 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	    // makePlot( samples , names , dir_name , "h_categoryB"+prefix+suffixes[isuf] , "B-Category" , "Events" , 0 , 4 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	    makePlot( samples , names , dir_name , "h_mt"+prefix+suffixes[isuf] , "M_{T} [GeV]" , "Events / 5 GeV" , 0 , 200 , 5 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	    makePlot( samples , names , dir_name , "h_Events_w"+prefix+suffixes[isuf] , "Events" , "Events" , 0 , 2 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  }    
	}
      }
      
    }
  }
}

void plotMakerSoftLepSR(){

  //  gROOT->LoadMacro("CMS_lumi.C");
  cmsText = "CMS Preliminary";
  cmsTextSize = 0.5;
  lumiTextSize = 0.4;
  writeExtraText = false;
  lumi_13TeV = "2.26 fb^{-1}";

  string suffix = "";
  bool doFake = false;
  bool doOnelep = false;
  bool doDilep = false;
  bool doSamples = false;
  bool doTotal = true;
  string input_dir = "../SoftLepLooper/output/softLep";
  
  // ----------------------------------------
  //  samples definition
  // ----------------------------------------

  // get input files

 
  TFile* f_top = new TFile(Form("%s/top.root",input_dir.c_str())); //hadd'ing of ttbar, ttw, ttz, tth, singletop
  // TFile* f_tt1l = new TFile(Form("%s/ttsl.root",input_dir.c_str()));
  // TFile* f_tt2l = new TFile(Form("%s/ttdl.root",input_dir.c_str())); 
  // TFile* f_st = new TFile(Form("%s/singletop.root",input_dir.c_str())); 
  TFile* f_wjets = new TFile(Form("%s/wjets_ht.root",input_dir.c_str()));
  TFile* f_qcd = new TFile(Form("%s/qcd_ht.root",input_dir.c_str()));
  TFile* f_zinv = new TFile(Form("%s/zinv_ht.root",input_dir.c_str()));
  TFile* f_ww = new TFile(Form("%s/ww.root",input_dir.c_str()));
  TFile* f_wz = new TFile(Form("%s/wz.root",input_dir.c_str()));
  TFile* f_zz = new TFile(Form("%s/zz.root",input_dir.c_str()));
  TFile* f_diboson = new TFile(Form("%s/diboson.root",input_dir.c_str()));
  TFile* f_dy = new TFile(Form("%s/dyjetsll.root",input_dir.c_str()));
  TFile* f_bkg = new TFile(Form("%s/allBkg.root",input_dir.c_str())); 
  TFile* f_bkg2 = new TFile(Form("%s/allBkg_noDiboson.root",input_dir.c_str())); 
  TFile* f_T2_4bd_275_235 = new TFile(Form("%s/T2-4bd_275_235.root",input_dir.c_str())); 
  TFile* f_T5qqqqWW_1025_775_old = new TFile(Form("%s/T5qqqqWW_1025_775_old.root",input_dir.c_str())); 
  TFile* f_T5qqqqWW_1100_500_old = new TFile(Form("%s/T5qqqqWW_1100_500_old.root",input_dir.c_str())); 
  TFile* f_T5qqqqWW_1025_775 = new TFile(Form("%s/T5qqqqWW_1025_775.root",input_dir.c_str())); 
  TFile* f_T5qqqqWW_1100_500 = new TFile(Form("%s/T5qqqqWW_1100_500.root",input_dir.c_str())); 
  //TFile* f_data = new TFile(Form("%s/data_Run2015CD.root",input_dir.c_str())); 

  vector<TFile*> samples;
  vector<string>  names;
  
  vector<TFile*> samples2;
  vector<string>  names2;
  samples2.push_back(f_qcd);                    names2.push_back("qcd");
  samples2.push_back(f_zinv);                   names2.push_back("zinv");
  samples2.push_back(f_dy);                     names2.push_back("dyjets");
  samples2.push_back(f_diboson);                names2.push_back("diboson");
  samples2.push_back(f_top);                    names2.push_back("top");
  samples2.push_back(f_wjets);                  names2.push_back("wjets");
  samples2.push_back(f_T2_4bd_275_235);         names2.push_back("T2-4bd_275_235 sig");
  samples2.push_back(f_T5qqqqWW_1025_775);      names2.push_back("T5qqqqWW_1025_775 sig");
  samples2.push_back(f_T5qqqqWW_1100_500);      names2.push_back("T5qqqqWW_1100_500 sig");
  samples2.push_back(f_T5qqqqWW_1025_775_old);  names2.push_back("T5qqqqWW_1025_775_old sig");
  samples2.push_back(f_T5qqqqWW_1100_500_old);  names2.push_back("T5qqqqWW_1100_500_old sig");

  if (doFake){
    // samples.push_back(f_tt2l); names.push_back("tt+2l fakeLep");
    // samples.push_back(f_tt1l); names.push_back("tt+1l fakeLep");
    // samples.push_back(f_st); names.push_back("st fakeLep");
    samples.push_back(f_qcd); names.push_back("qcd fakeLep");
    samples.push_back(f_zinv); names.push_back("zinv fakeLep");
    samples.push_back(f_dy); names.push_back("dyjets fakeLep");
    // samples.push_back(f_ww); names.push_back("ww fakeLep");
    // samples.push_back(f_wz); names.push_back("wz fakeLep");
    // samples.push_back(f_zz); names.push_back("zz fakeLep");
    samples.push_back(f_diboson); names.push_back("diboson fakeLep");
    samples.push_back(f_top); names.push_back("top fakeLep");
    samples.push_back(f_wjets); names.push_back("wjets fakeLep");
  }

  if (doOnelep){
    // samples.push_back(f_tt2l); names.push_back("tt+2l onelep");
    // samples.push_back(f_tt1l); names.push_back("tt+1l onelep");
    // samples.push_back(f_st); names.push_back("st onelep");
    samples.push_back(f_qcd); names.push_back("qcd onelep");
    samples.push_back(f_zinv); names.push_back("zinv onelep");
    samples.push_back(f_dy); names.push_back("dyjets onelep");
    // samples.push_back(f_ww); names.push_back("ww onelep");
    // samples.push_back(f_wz); names.push_back("wz onelep");
    // samples.push_back(f_zz); names.push_back("zz onelep");
    samples.push_back(f_diboson); names.push_back("diboson onelep");
    samples.push_back(f_top); names.push_back("top onelep");
    samples.push_back(f_wjets); names.push_back("wjets onelep");
  }

  if (doDilep){
    // samples.push_back(f_tt2l); names.push_back("tt+2l dilep");
    // samples.push_back(f_tt1l); names.push_back("tt+1l dilep");
    // samples.push_back(f_st); names.push_back("st dilep");
    samples.push_back(f_qcd); names.push_back("qcd dilep");
    samples.push_back(f_zinv); names.push_back("zinv dilep");
    samples.push_back(f_dy); names.push_back("dyjets dilep");
    // samples.push_back(f_ww); names.push_back("ww dilep");
    // samples.push_back(f_wz); names.push_back("wz dilep");
    // samples.push_back(f_zz); names.push_back("zz dilep");
    samples.push_back(f_diboson); names.push_back("diboson dilep");
    samples.push_back(f_top); names.push_back("top dilep");
    samples.push_back(f_wjets); names.push_back("wjets dilep");
  }

  if (doSamples){
    // samples.push_back(f_tt2l); names.push_back("tt+2l");
    // samples.push_back(f_tt1l); names.push_back("tt+1l");
    // samples.push_back(f_st); names.push_back("st");
    samples.push_back(f_qcd); names.push_back("qcd");
    samples.push_back(f_zinv); names.push_back("zinv");
    samples.push_back(f_dy); names.push_back("dyjets");
    // samples.push_back(f_ww); names.push_back("ww");
    // samples.push_back(f_wz); names.push_back("wz");
    // samples.push_back(f_zz); names.push_back("zz");
    samples.push_back(f_diboson); names.push_back("diboson");
    samples.push_back(f_top); names.push_back("top");
    samples.push_back(f_wjets); names.push_back("wjets");
    samples.push_back(f_T2_4bd_275_235); names.push_back("T2-4bd_275_235 sig");
    samples.push_back(f_T5qqqqWW_1025_775); names.push_back("T5qqqqWW_1025_775 sig");
  }

  if (doTotal){
    samples.push_back(f_bkg2); names.push_back("fakeLep");

    samples.push_back(f_qcd); names.push_back("qcd dilep"); //empty
    samples.push_back(f_qcd); names.push_back("qcd onelep"); //empty
    samples.push_back(f_zinv); names.push_back("zinv onelep"); //empty
    samples.push_back(f_zinv); names.push_back("zinv dilep"); //empty
    // samples.push_back(f_dy); names.push_back("dyjets onelep");
    // samples.push_back(f_dy); names.push_back("dyjets dilep");
    samples.push_back(f_dy); names.push_back("dyjets");
    // samples.push_back(f_diboson); names.push_back("diboson onelep");
    // samples.push_back(f_diboson); names.push_back("diboson dilep");
    samples.push_back(f_diboson); names.push_back("diboson");
    samples.push_back(f_top); names.push_back("top dilep");
    samples.push_back(f_top); names.push_back("top onelep");
 
    samples.push_back(f_wjets); names.push_back("wjets onelep"); //empty
    samples.push_back(f_wjets); names.push_back("wjets dilep");
    
    //samples.push_back(f_T2_4bd_275_235); names.push_back("T2-4bd_275_235 sig");
    samples.push_back(f_T5qqqqWW_1025_775_old); names.push_back("T5qqqqWW_1025_775_old sig");
    samples.push_back(f_T5qqqqWW_1100_500_old); names.push_back("T5qqqqWW_1100_500_old sig");
    samples.push_back(f_T5qqqqWW_1025_775); names.push_back("T5qqqqWW_1025_775 sig");
    samples.push_back(f_T5qqqqWW_1100_500); names.push_back("T5qqqqWW_1100_500 sig");
  }
    
  // ----------------------------------------
  //  plots definitions
  // ----------------------------------------

  float scalesig = -1.;
  //float scalesig = 50.;
  bool printplots = true;
  bool doRatio = true;
  bool scaleBGtoData = false;

  //inclusive lepton pt plot
  // makePlot( samples2 , names2 , "" , "h_leppt" , "Lepton p_{T} [GeV]" , "Events / 10 GeV" , 0 , 500 , 10 , false , printplots, scalesig, doRatio, scaleBGtoData );
  // makePlot( samples2 , names2 , "" , "h_leppt" , "Lepton p_{T} [GeV]" , "Events / 10 GeV" , 0 , 500 , 10 , true , printplots, scalesig, doRatio, scaleBGtoData );
  // makePlot( samples2 , names2 , "" , "h_mt"    , "M_{T} [GeV]" , "Events / 10 GeV" , 0 , 250 , 10 , false , printplots, scalesig, doRatio, scaleBGtoData );
  // makePlot( samples2 , names2 , "" , "h_mt"    , "M_{T} [GeV]" , "Events / 10 GeV" , 0 , 250 , 10 , true , printplots, scalesig, doRatio, scaleBGtoData );

  
  if(printplots){
    TIter it(f_wjets->GetListOfKeys());
    TKey* k;
    std::string cr_skip = "cr";
    std::string sr_skip = "sr";
    while ((k = (TKey *)it())) {
      //if (strncmp (k->GetTitle(), cr_skip.c_str(), cr_skip.length()) == 0) continue; //skip control regions
      //if (strncmp (k->GetTitle(), sr_skip.c_str(), sr_skip.length()) == 0) continue; //skip signal regions and srbase
      std::string dir_name = k->GetTitle();
      if(dir_name != "srLepbase") continue; //to do only this dir
      //if(dir_name.find("srLep") == std::string::npos) continue; //to do only this dir
      //if(dir_name != "srLepbase" && dir_name != "srLepbaseJ" && dir_name != "srLepbaseJ1") continue; //to do only this dir
      //if(dir_name != "srLep1M" && dir_name != "srLep2M" && dir_name != "srLepMET" && dir_name != "srLepHT") continue; //to do only this dir
      //if(dir_name != "srsoftlmubase" && dir_name != "srsoftlelbase") continue; //to do only this dir

      for (unsigned int ilog = 0; ilog < 2; ilog++) {
	  bool doLog = ilog==0;
	  makePlot( samples , names , dir_name , "h_lepptshort"+suffix , "Lepton p_{T} [GeV]" , "Events / 1 GeV" , 0 , 25 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_lepeta"+suffix , "#eta" , "Events / ? GeV" , -3 , 3 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_softlepht"+suffix  , "H_{T} [GeV]" , "Events / 100 GeV" , 0 , 2000 , 4 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_mt2"+suffix , "M_{T2} [GeV]" , "Events / 50 GeV" , 0 , 1000 , 5 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_met"+suffix  , "E_{T}^{miss} [GeV]" , "Events / 50 GeV" , 0 , 1000 , 5 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_nJet30"+suffix , "N(jets)" , "Events" , 0 , 15 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_nBJet20"+suffix , "N(b jets)" , "Events" , 0 , 6 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_J0pt"+suffix , "p_{T}(jet1) [GeV]" , "Events / 10 GeV" , 0 , 1000 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_J1pt"+suffix , "p_{T}(jet2) [GeV]" , "Events / 10 GeV" , 0 , 1000 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_BJetpt"+suffix , "p_{T}(bjet1) [GeV]" , "Events / 10 GeV" , 20 , 1000 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  // makePlot( samples , names , dir_name , "h_BJetptLead"+suffix , "p_{T} bjet (leading) [GeV]" , "Events / 10 GeV" , 20 , 1000 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  // makePlot( samples , names , dir_name , "h_BJetptNotLead"+suffix ,"p_{T} bjet (non-leading) [GeV]" , "Events / 10 GeV" , 20 , 1000 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_BJetptshort"+suffix , "p_{T}(bjet1) [GeV]" , "Events / 1 GeV" , 20 , 100 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_categoryB"+suffix , "B-Category" , "Events" , 0 , 4 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_mt"+suffix , "M_{T} [GeV]" , "Events / 10 GeV" , 0 , 250 , 10 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_mtbins"+suffix , "M_{T} [GeV]" , "Events / bin" , 0 , 250 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_Wpt"+suffix , "W p_{T} [GeV]" , "Events / 20 GeV" , 0 , 800 , 4 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples2 , names2 , dir_name , "h_Events_w"+suffix , "Events" , "Events" , 0 , 2 , 1 , doLog , printplots, scalesig, false, scaleBGtoData );
	  //makePlot( samples , names , dir_name , "h_missingdilepmll"+suffix , "m_{ll} [GeV]" , "Events" , 0 , 150 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
      }
      
    }
  }
}


void plotMakerSoftLepCR(){

  //  gROOT->LoadMacro("CMS_lumi.C");
  cmsText = "CMS Preliminary";
  cmsTextSize = 0.5;
  lumiTextSize = 0.4;
  writeExtraText = false;
  lumi_13TeV = "2.26 fb^{-1}";

  string suffix = "";
  bool doOnlyTrue = false;
  bool doData = true;

  bool doValidate = false;
  if (doValidate) suffix = "Validate2";
  
  string input_dir = "../SoftLepLooper/output/softLep";
  
  // ----------------------------------------
  //  samples definition
  // ----------------------------------------

  // get input files
  
  TFile* f_top = new TFile(Form("%s/top.root",input_dir.c_str())); //hadd'ing of ttbar, ttw, ttz, tth, singletop
  TFile* f_tt1l = new TFile(Form("%s/ttsl.root",input_dir.c_str()));
  TFile* f_tt2l = new TFile(Form("%s/ttdl.root",input_dir.c_str())); 
  TFile* f_st = new TFile(Form("%s/singletop.root",input_dir.c_str())); 
  TFile* f_wjets = new TFile(Form("%s/wjets_ht.root",input_dir.c_str()));
  TFile* f_qcd = new TFile(Form("%s/qcd_ht.root",input_dir.c_str()));
  TFile* f_zinv = new TFile(Form("%s/zinv_ht.root",input_dir.c_str()));
  TFile* f_ww = new TFile(Form("%s/ww.root",input_dir.c_str()));
  TFile* f_wz = new TFile(Form("%s/wz.root",input_dir.c_str()));
  TFile* f_zz = new TFile(Form("%s/zz.root",input_dir.c_str()));
  TFile* f_diboson = new TFile(Form("%s/diboson.root",input_dir.c_str()));
  TFile* f_dy = new TFile(Form("%s/dyjetsll.root",input_dir.c_str()));
  TFile* f_bkg = new TFile(Form("%s/allBkg.root",input_dir.c_str())); 
  TFile* f_T2_4bd_275_235 = new TFile(Form("%s/T2-4bd_275_235.root",input_dir.c_str())); 
  TFile* f_T5qqqqWW_1025_775_test = new TFile(Form("%s/../test/T5qqqqWW_1025_775.root",input_dir.c_str())); 
  TFile* f_data = new TFile(Form("%s/data_Run2015CD.root",input_dir.c_str())); 

  vector<TFile*> samples;
  vector<string>  names;

  if (!doOnlyTrue && !doData){
    // samples.push_back(f_tt2l); names.push_back("tt+2l fakeLep");
    // samples.push_back(f_tt1l); names.push_back("tt+1l fakeLep");
    // samples.push_back(f_st); names.push_back("st fakeLep");
    samples.push_back(f_qcd); names.push_back("qcd fakeLep");
    samples.push_back(f_zinv); names.push_back("zinv fakeLep");
    samples.push_back(f_dy); names.push_back("dyjets fakeLep");
    // samples.push_back(f_ww); names.push_back("ww fakeLep");
    // samples.push_back(f_wz); names.push_back("wz fakeLep");
    // samples.push_back(f_zz); names.push_back("zz fakeLep");
    samples.push_back(f_diboson); names.push_back("diboson fakeLep");
    samples.push_back(f_top); names.push_back("top fakeLep");
    samples.push_back(f_wjets); names.push_back("wjets fakeLep");
  }
  
  else if (!doData) {
    // samples.push_back(f_tt2l); names.push_back("tt+2l onelep");
    // samples.push_back(f_tt1l); names.push_back("tt+1l onelep");
    // samples.push_back(f_st); names.push_back("st onelep");
    samples.push_back(f_qcd); names.push_back("qcd onelep");
    samples.push_back(f_zinv); names.push_back("zinv onelep");
    samples.push_back(f_dy); names.push_back("dyjets onelep");
    // samples.push_back(f_ww); names.push_back("ww onelep");
    // samples.push_back(f_wz); names.push_back("wz onelep");
    // samples.push_back(f_zz); names.push_back("zz onelep");
    samples.push_back(f_diboson); names.push_back("diboson onelep");
    samples.push_back(f_top); names.push_back("top onelep");
    samples.push_back(f_wjets); names.push_back("wjets onelep");
  }

  else if (doValidate) {
    // samples.push_back(f_bkg); names.push_back("fakeLep");
    // samples.push_back(f_ww); names.push_back("ww onelep");
    // samples.push_back(f_wz); names.push_back("wz onelep");
    // samples.push_back(f_zz); names.push_back("zz onelep");
    samples.push_back(f_dy); names.push_back("dyjets");
    samples.push_back(f_tt2l); names.push_back("tt+2l");
    samples.push_back(f_diboson); names.push_back("diboson");
    // samples.push_back(f_top); names.push_back("top");
    samples.push_back(f_st); names.push_back("st");
    samples.push_back(f_tt1l); names.push_back("tt+1l");
    samples.push_back(f_wjets); names.push_back("wjets");
    samples.push_back(f_T2_4bd_275_235); names.push_back("T2-4bd_275_235 sig");
    //samples.push_back(f_T5qqqqWW_1025_775_test); names.push_back("T5qqqqWW_1025_775_fixed sig");
    samples.push_back(f_data); names.push_back("data");    
  }
  
  else {   
    samples.push_back(f_bkg); names.push_back("fakeLep");
    // samples.push_back(f_ww); names.push_back("ww onelep");
    // samples.push_back(f_wz); names.push_back("wz onelep");
    // samples.push_back(f_zz); names.push_back("zz onelep");
    samples.push_back(f_diboson); names.push_back("diboson onelep");
    samples.push_back(f_dy); names.push_back("dyjets onelep");
    samples.push_back(f_top); names.push_back("top onelep");
    // samples.push_back(f_tt2l); names.push_back("tt+2l onelep");
    // samples.push_back(f_tt1l); names.push_back("tt+1l onelep");
    samples.push_back(f_wjets); names.push_back("wjets onelep");
    samples.push_back(f_T2_4bd_275_235); names.push_back("T2-4bd_275_235 sig");
    //samples.push_back(f_T5qqqqWW_1025_775_test); names.push_back("T5qqqqWW_1025_775_fixed sig");
    samples.push_back(f_data); names.push_back("data");
  }
  
  // ----------------------------------------
  //  plots definitions
  // ----------------------------------------

  float scalesig = -1.;
  //float scalesig = 50.;
  bool printplots = true;
  bool doRatio = true;
  bool scaleBGtoData = true;

  if(printplots){
    TIter it(f_wjets->GetListOfKeys());
    TKey* k;
    std::string cr_skip = "cr";
    std::string sr_skip = "sr";
    while ((k = (TKey *)it())) {
      //if (strncmp (k->GetTitle(), cr_skip.c_str(), cr_skip.length()) == 0) continue; //skip control regions
      //if (strncmp (k->GetTitle(), sr_skip.c_str(), sr_skip.length()) == 0) continue; //skip signal regions and srbase
      std::string dir_name = k->GetTitle();
      //if(dir_name != "cr1Lbase" && dir_name != "srLepbase" && dir_name != "cr1LbaseZeroB" && dir_name != "cr1LbaseWithB") continue; //to do only this dir
      //if(dir_name != "cr1Lmubase" && dir_name != "cr1Lelbase") continue; //to do only this dir

      //loop once for log, once for no log
      if (dir_name == "cr1LbaseZeroB" && doValidate) {
	for (unsigned int ilog = 0; ilog < 2; ilog++) {
	  bool doLog = ilog==0;
	  makePlot( samples , names , dir_name , "h_leppt"+suffix , "Lepton p_{T} [GeV]" , "Events / 1 GeV" , 20 , 45 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_lepeta"+suffix , "#eta" , "Events / ? GeV" , -3 , 3 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_softlepht"+suffix , "H_{T} [GeV]" , "Events / 100 GeV" , 0 , 2000 , 4 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  //makePlot( samples , names , dir_name , "h_mt2"+suffix , "M_{T2} [GeV]" , "Events / 10 GeV" , 0 , 100 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_met"+suffix  , "E_{T}^{miss} [GeV]" , "Events / 50 GeV" , 0 , 1000 , 5 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_nJet30"+suffix , "N(jets)" , "Events" , 0 , 15 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_nBJet20"+suffix , "N(b jets)" , "Events" , 0 , 6 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_categoryB"+suffix , "B-Category" , "Events" , 0 , 4 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  //makePlot( samples , names , dir_name , "h_J0pt"+suffix , "p_{T}(jet1) [GeV]" , "Events / 20 GeV" , 0 , 1000 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  //makePlot( samples , names , dir_name , "h_J1pt"+suffix , "p_{T}(jet2) [GeV]" , "Events / 20 GeV" , 0 , 1000 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_mt"+suffix , "M_{T} [GeV]" , "Events / 10 GeV" , 0 , 250 , 10 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_mtbins"+suffix , "M_{T} [GeV]" , "Events / bin" , 0 , 250 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  //makePlot( samples , names , dir_name , "h_softlepmt2"+suffix , "M_{T2} [GeV]" , "Events / 50 GeV" , 100 , 1000 , 5 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_Wpt"+suffix , "W p_{T} [GeV]" , "Events / 20 GeV" , 0 , 800 , 4 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_Events_w"+suffix , "Events" , "Events" , 0 , 2 , 1 , doLog , printplots, scalesig, false, scaleBGtoData );
	}
      }

      else if (!doValidate && (dir_name == "cr1Lbase" || dir_name == "cr1LbaseZeroB" || dir_name == "cr1LbaseWithB")) {
	for (unsigned int ilog = 0; ilog < 2; ilog++) {
	  bool doLog = ilog==0;
	  makePlot( samples , names , dir_name , "h_leppt"+suffix , "Lepton p_{T} [GeV]" , "Events / 10 GeV" , 200 , 1000 , 10 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_lepeta"+suffix , "#eta" , "Events / ? GeV" , -3 , 3 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_softlepht"+suffix , "H_{T} [GeV]" , "Events / 100 GeV" , 0 , 2000 , 4 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_deltaPhiMin"+suffix , "#Delta#phi_{min}" , "Events / ? GeV" , 0 , 3.2 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  //makePlot( samples , names , dir_name , "h_mt2"+suffix , "M_{T2} [GeV]" , "Events / 10 GeV" , 0 , 100 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_met"+suffix  , "E_{T}^{miss} [GeV]" , "Events / 10 GeV" , 0 , 100 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_nJet30"+suffix , "N(jets)" , "Events" , 0 , 15 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_nBJet20"+suffix , "N(b jets)" , "Events" , 0 , 6 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_categoryB"+suffix , "B-Category" , "Events" , 0 , 4 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  //makePlot( samples , names , dir_name , "h_J0pt"+suffix , "p_{T}(jet1) [GeV]" , "Events / 20 GeV" , 0 , 1000 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  //makePlot( samples , names , dir_name , "h_J1pt"+suffix , "p_{T}(jet2) [GeV]" , "Events / 20 GeV" , 0 , 1000 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_mt"+suffix , "M_{T} [GeV]" , "Events / 10 GeV" , 0 , 250 , 10 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_mtbins"+suffix , "M_{T} [GeV]" , "Events / bin" , 0 , 250 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  //makePlot( samples , names , dir_name , "h_softlepmt2"+suffix , "M_{T2} [GeV]" , "Events / 50 GeV" , 100 , 1000 , 5 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_Wpt"+suffix , "W p_{T} [GeV]" , "Events / 20 GeV" , 0 , 800 , 4 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_Events_w"+suffix , "Events" , "Events" , 0 , 2 , 1 , doLog , printplots, scalesig, false, scaleBGtoData );
	}
      }
 
    }
  }
}

void plotMakerDoubleLepCR(){

  //  gROOT->LoadMacro("CMS_lumi.C");
  cmsText = "CMS Preliminary";
  cmsTextSize = 0.5;
  lumiTextSize = 0.4;
  writeExtraText = false;
  lumi_13TeV = "2.26 fb^{-1}";

  string suffix = "";
  bool doSRDilepton = false;
  bool doOnlyTrue = false;
  bool doData = true;
  string input_dir = "../SoftLepLooper/output/softLep";
  
  // ----------------------------------------
  //  samples definition
  // ----------------------------------------

  
  // get input files

  TFile* f_top = new TFile(Form("%s/top.root",input_dir.c_str())); //hadd'ing of ttbar, ttw, ttz, tth, singletop
  // TFile* f_tt1l = new TFile(Form("%s/ttsl.root",input_dir.c_str()));
  // TFile* f_tt2l = new TFile(Form("%s/ttdl.root",input_dir.c_str())); 
  // TFile* f_st = new TFile(Form("%s/singletop.root",input_dir.c_str())); 
  TFile* f_wjets = new TFile(Form("%s/wjets_ht.root",input_dir.c_str()));
  TFile* f_qcd = new TFile(Form("%s/qcd_ht.root",input_dir.c_str()));
  TFile* f_zinv = new TFile(Form("%s/zinv_ht.root",input_dir.c_str()));
  TFile* f_ww = new TFile(Form("%s/ww.root",input_dir.c_str()));
  TFile* f_wz = new TFile(Form("%s/wz.root",input_dir.c_str()));
  TFile* f_zz = new TFile(Form("%s/zz.root",input_dir.c_str()));
  TFile* f_diboson = new TFile(Form("%s/diboson.root",input_dir.c_str()));
  TFile* f_dy = new TFile(Form("%s/dyjetsll.root",input_dir.c_str()));
  TFile* f_bkg = new TFile(Form("%s/allBkg.root",input_dir.c_str())); 
  TFile* f_T2_4bd_275_235 = new TFile(Form("%s/T2-4bd_275_235.root",input_dir.c_str())); 
  TFile* f_data = new TFile(Form("%s/data_Run2015CD.root",input_dir.c_str())); 

  vector<TFile*> samples;
  vector<string>  names;

  if (!doOnlyTrue && !doData){
    // samples.push_back(f_tt2l); names.push_back("tt+2l fakeLep");
    // samples.push_back(f_tt1l); names.push_back("tt+1l fakeLep");
    // samples.push_back(f_st); names.push_back("st fakeLep");
    samples.push_back(f_qcd); names.push_back("qcd fakeLep");
    samples.push_back(f_zinv); names.push_back("zinv fakeLep");
    samples.push_back(f_dy); names.push_back("dyjets fakeLep");
    // samples.push_back(f_ww); names.push_back("ww fakeLep");
    // samples.push_back(f_wz); names.push_back("wz fakeLep");
    // samples.push_back(f_zz); names.push_back("zz fakeLep");
    samples.push_back(f_diboson); names.push_back("diboson fakeLep");
    samples.push_back(f_wjets); names.push_back("wjets fakeLep");
    samples.push_back(f_top); names.push_back("top fakeLep");
  }
  
  if (!doData) {
    // samples.push_back(f_tt2l); names.push_back("tt+2l dilep");
    // samples.push_back(f_tt1l); names.push_back("tt+1l dilep");
    // samples.push_back(f_st); names.push_back("st dilep");
    samples.push_back(f_qcd); names.push_back("qcd dilep");
    samples.push_back(f_zinv); names.push_back("zinv dilep");
    samples.push_back(f_dy); names.push_back("dyjets dilep");
    // samples.push_back(f_ww); names.push_back("ww dilep");
    // samples.push_back(f_wz); names.push_back("wz dilep");
    // samples.push_back(f_zz); names.push_back("zz dilep");
    samples.push_back(f_diboson); names.push_back("diboson dilep");
    samples.push_back(f_wjets); names.push_back("wjets dilep");
    samples.push_back(f_top); names.push_back("top dilep");
  }

  else {    
    // samples.push_back(f_tt2l); names.push_back("tt+2l");
    // samples.push_back(f_tt1l); names.push_back("tt+1l");
    // samples.push_back(f_st); names.push_back("st");
    // samples.push_back(f_qcd); names.push_back("qcd");
    // samples.push_back(f_zinv); names.push_back("zinv");
    // samples.push_back(f_dy); names.push_back("dyjets");
    // samples.push_back(f_ww); names.push_back("ww");
    // samples.push_back(f_wjets); names.push_back("wjets");
    // samples.push_back(f_top); names.push_back("top");

    samples.push_back(f_bkg); names.push_back("fakeLep");
    samples.push_back(f_dy); names.push_back("dyjets dilep");
    // samples.push_back(f_ww); names.push_back("ww dilep");
    // samples.push_back(f_wz); names.push_back("wz dilep");
    // samples.push_back(f_zz); names.push_back("zz dilep");
    samples.push_back(f_diboson); names.push_back("diboson dilep");
    samples.push_back(f_top); names.push_back("top dilep");
    samples.push_back(f_T2_4bd_275_235); names.push_back("T2-4bd_275_235 sig");
    samples.push_back(f_data); names.push_back("data");
  }
  
  // ----------------------------------------
  //  plots definitions
  // ----------------------------------------

  float scalesig = -1.;
  //float scalesig = 50.;
  bool printplots = true;
  bool doRatio = true;
  bool scaleBGtoData = true;

  if(printplots){
    TIter it(f_top->GetListOfKeys());
    TKey* k;
    std::string cr_skip = "cr";
    std::string sr_skip = "sr";
    while ((k = (TKey *)it())) {
      //if (strncmp (k->GetTitle(), cr_skip.c_str(), cr_skip.length()) == 0) continue; //skip control regions
      //if (strncmp (k->GetTitle(), sr_skip.c_str(), sr_skip.length()) == 0) continue; //skip signal regions and srbase
      std::string dir_name = k->GetTitle();
      //if(dir_name != "srLepbase") continue; //to do only this dir
      if(dir_name != "cr2Lbase" && dir_name != "srLepbase") continue; //to do only this dir
      
      if(dir_name == "cr2Lbase"){
	for (unsigned int ilog = 0; ilog < 2; ilog++) {
	  bool doLog = ilog==0;
	  makePlot( samples , names , dir_name , "h_lowleppt"+suffix , "Soft Lepton p_{T} [GeV]" , "Events / 1 GeV" , 0 , 30 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_lep1eta"+suffix , "Lepton1 #eta" , "Events / ? GeV" , -3 , 3 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_highleppt"+suffix , "Hard Lepton p_{T} [GeV]" , "Events / 25 GeV" , 0 , 500 , 5 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_lep2eta"+suffix , "Lepton2 #eta" , "Events / ? GeV" , -3 , 3 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_softlepht"+suffix  , "H_{T} [GeV]" , "Events / 100 GeV" , 0 , 2000 , 4 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_met"+suffix  , "E_{T}^{miss} [GeV]" , "Events / 50 GeV" , 0 , 1000 , 5 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_nJet30"+suffix , "N(jets)" , "Events" , 0 , 15 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_nBJet20"+suffix , "N(b jets)" , "Events" , 0 , 6 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_categoryB"+suffix , "B-Category" , "Events" , 0 , 4 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  // makePlot( samples , names , dir_name , "h_J0pt"+suffix , "p_{T}(jet1) [GeV]" , "Events / 20 GeV" , 0 , 1000 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  // makePlot( samples , names , dir_name , "h_J1pt"+suffix , "p_{T}(jet2) [GeV]" , "Events / 20 GeV" , 0 , 1000 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_mt"+suffix , "M_{T} [GeV]" , "Events / 10 GeV" , 0 , 250 , 10 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_mtAlt"+suffix , "M_{T} [GeV]" , "Events / 10 GeV" , 0 , 250 , 10 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_mtHard"+suffix , "M_{T} [GeV]" , "Events / 10 GeV" , 0 , 250 , 10 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  //makePlot( samples , names , dir_name , "h_mt2"+suffix , "M_{T2} [GeV]" , "Events / 50 GeV" , 100 , 1000 , 5 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  //makePlot( samples , names , dir_name , "h_type"+suffix , "type" , "Events" , 0 , 3 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_Events_w"+suffix , "Events" , "Events" , 0 , 2 , 1 , doLog , printplots, scalesig, false, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_dilepmll"+suffix , "m_{ll} [GeV]" , "Events / 5 GeV" , 0 , 150 , 5 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_deltaEtaLep"+suffix , "#Delta#eta(lep1-lep2)" , "Events / 0.5" , -4 , 4 , 5 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_deltaPhiLep"+suffix , "#Delta#phi(lep1-lep2)" , "Events / 1.0" , -7 , 7 , 10 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_deltaPtLep"+suffix , "#Deltap_{T}(lep1-lep2)" , "Events / 10 GeV" , 0 , 200 , 10 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_dilepPt"+suffix , "p_{T}^{ll}" , "Events / 10 GeV" , 0 , 200 , 10 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	}
      }
      

      if(dir_name == "srLepbase" && doSRDilepton){
	for (unsigned int ilog = 0; ilog < 2; ilog++) {
	  bool doLog = ilog==0;
	  makePlot( samples , names , dir_name , "h_lepptshort"+suffix , "Lepton p_{T} [GeV]" , "Events / 1 GeV" , 0 , 30 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_softlepht"+suffix  , "H_{T} [GeV]" , "Events / 100 GeV" , 0 , 2000 , 4 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_met"+suffix  , "E_{T}^{miss} [GeV]" , "Events / 50 GeV" , 0 , 1000 , 5 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_nJet30"+suffix , "N(jets)" , "Events" , 0 , 15 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_nBJet20"+suffix , "N(b jets)" , "Events" , 0 , 6 , 1 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_categoryB"+suffix , "B-Category" , "Events" , 0 , 4 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  // makePlot( samples , names , dir_name , "h_J0pt"+suffix , "p_{T}(jet1) [GeV]" , "Events / 20 GeV" , 0 , 1000 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  // makePlot( samples , names , dir_name , "h_J1pt"+suffix , "p_{T}(jet2) [GeV]" , "Events / 20 GeV" , 0 , 1000 , 1 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_mt"+suffix , "M_{T} [GeV]" , "Events / 10 GeV" , 0 , 250 , 10 , doLog , printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_mt2"+suffix , "M_{T2} [GeV]" , "Events / 50 GeV" , 100 , 1000 , 5 , doLog, printplots, scalesig, doRatio, scaleBGtoData );
	  makePlot( samples , names , dir_name , "h_Events_w"+suffix , "Events" , "Events" , 0 , 2 , 1 , doLog , printplots, scalesig, false, scaleBGtoData );
	}
      }
      
    }
  }
}

//_______________________________________________________________________________
void plotMakerSoft(){
  //tableMakerZinvCR(); return;
  //plotMakerDY(); return;
  //plotMakerZllMT(); return;
  //plotMakerGJets(); return;
  //plotMakerRemovedLep(); return;
  //plotMakerCRSL(); return;
  //plotMakerSoftLepSRMissingLep(); return;
  //plotMakerSoftLepFR(); return;
  //plotMakerSoftLepSR(); return;
  plotMakerSoftLepCR(); return;
  //plotMakerDoubleLepCR(); return;
  //plotMakerSoftLepSR(); plotMakerSoftLepCR(); plotMakerDoubleLepCR(); return;
  
  cmsText = "CMS Preliminary";
  cmsTextSize = 0.5;
  lumiTextSize = 0.4;
  writeExtraText = false;
  lumi_13TeV = "2.2 fb^{-1}";
  
  // string input_dir = "/Users/giovannizevidellaporta/UCSD/MT2lepton/HistFolder/SR6Jan16/";
  string input_dir = "../SoftLepLooper/output/softLep/";
  
  
  // ----------------------------------------
  //  samples definition
  // ----------------------------------------
  
  // get input files
  
  // TFile* f_ttbar = new TFile(Form("%s/top.root",input_dir.c_str()));
  // TFile* f_qcd = new TFile(Form("%s/qcd_ht.root",input_dir.c_str()));
  // TFile* f_wjets = new TFile(Form("%s/wjets_ht.root",input_dir.c_str()));
  // TFile* f_zjets = new TFile(Form("%s/zinv_ht.root",input_dir.c_str()));
  //TFile* f_data = new TFile(Form("%s/data_Run2015C.root",input_dir.c_str()));

  
  // TFile* f_tt1l = new TFile(Form("%s/ttsl.root",input_dir.c_str()));
  // TFile* f_tt2l = new TFile(Form("%s/ttdl.root",input_dir.c_str())); 
  // TFile* f_st = new TFile(Form("%s/singletop.root",input_dir.c_str())); 
  // TFile* f_wjets = new TFile(Form("%s/wjets_ht.root",input_dir.c_str()));
  // TFile* f_zinv = new TFile(Form("%s/zinv_ht.root",input_dir.c_str()));
  // TFile* f_dy = new TFile(Form("%s/dyjetsll.root",input_dir.c_str()));
  // TFile* f_qcd = new TFile(Form("%s/qcd_ht.root",input_dir.c_str()));
  // TFile* f_ww = new TFile(Form("%s/ww.root",input_dir.c_str()));
  
  vector<TFile*> samples;
  vector<string>  names;
  // samples.push_back(f_qcd); names.push_back("qcd");
  // samples.push_back(f_zjets); names.push_back("zinv");
  // samples.push_back(f_ttbar); names.push_back("top");
  // samples.push_back(f_wjets); names.push_back("wjets");
  

  
  // samples.push_back(f_st); names.push_back("st");
  // samples.push_back(f_ww); names.push_back("ww");
  // samples.push_back(f_dy); names.push_back("dyjets");
  // samples.push_back(f_tt2l); names.push_back("tt+2l");

  TFile* f_bkg = new TFile(Form("%s/allBkg.root",input_dir.c_str()));
  TFile* f_T2_4bd_275 = new TFile(Form("%s/T2-4bd_275.root",input_dir.c_str())); 
  TFile* f_T2_4bd_375 = new TFile(Form("%s/T2-4bd_375.root",input_dir.c_str())); 
  samples.push_back(f_bkg); names.push_back("onelep");
  samples.push_back(f_bkg); names.push_back("dilep");
  samples.push_back(f_bkg); names.push_back("fake");
  samples.push_back(f_T2_4bd_275); names.push_back("T2-4bd_275_235 sig");
  samples.push_back(f_T2_4bd_375); names.push_back("T2-4bd_375_335 sig");
  
  
  //samples.push_back(f_data); names.push_back("data");
  
  
  //  TFile* f_T2tt_400_325 = new TFile(Form("%s/T2tt_mStop-400to475_mLSP-1to400.root",input_dir.c_str()));
  //  TFile* f_T2tt_275_200 = new TFile(Form("%s/T2tt_mStop-275_mLSP-75to200.root",input_dir.c_str()));
  //  TFile* f_T5qqqqVV_1100_975 = new TFile(Form("%s/T5qqqqVV_mGluino-1100To1175_mLSP-1to1050.root",input_dir.c_str()));
  //  samples.push_back(f_T2tt_400_325); names.push_back("sig_T2tt_400_325");
  //  samples.push_back(f_T2tt_275_200); names.push_back("sig_T2tt_275_200");
  //  samples.push_back(f_T5qqqqVV_1100_975); names.push_back("sig_T5qqqqVV_1100_975");
  
  // ----------------------------------------
  //  plots definitions
  // ----------------------------------------
  
  float scalesig = -1.;
  //float scalesig = 50.;
  bool printplots = false;
  bool doRatio = false;
  bool scaleBGtoData = true;
  
  if(printplots){
    TIter it(f_bkg->GetListOfKeys());
    TKey* k;
    std::string cr_skip = "cr";
    std::string sr_skip = "sr";
    while ((k = (TKey *)it())) {
      if (strncmp (k->GetTitle(), cr_skip.c_str(), cr_skip.length()) == 0) continue; //skip control regions
      //if (strncmp (k->GetTitle(), sr_skip.c_str(), sr_skip.length()) == 0) continue; //skip signal regions and srbase
      std::string dir_name = k->GetTitle();
      if(dir_name == "") continue;
      //if(dir_name != "srbase") continue; //to do only this dir
      if(dir_name != "srLepbase") continue; //for testing
      //      if(! TString(dir_name).Contains("srLep")) continue; //for testing
      
      makePlot( samples , names , dir_name , "h_ht"  , "H_{T} [GeV]" , "Events / 50 GeV" , 0 , 1500 , 2 , false, printplots, scalesig, doRatio, scaleBGtoData );
      makePlot( samples , names , dir_name , "h_mt2" , "M_{T2} [GeV]" , "Events / 50 GeV" , 100 , 1000 , 5 , false, printplots, scalesig, doRatio, scaleBGtoData );
      makePlot( samples , names , dir_name , "h_met"  , "E_{T}^{miss} [GeV]" , "Events / 50 GeV" , 0 , 800 , 5 , false, printplots, scalesig, doRatio, scaleBGtoData );
      makePlot( samples , names , dir_name , "h_nJet30" , "N(jets)" , "Events" , 0 , 15 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      makePlot( samples , names , dir_name , "h_nBJet20" , "N(b jets)" , "Events" , 0 , 6 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      makePlot( samples , names , dir_name , "h_mtbins" , "M_{T} [GeV]" , "Events / Bin" , 0 , 300 , 1 , true, printplots, scalesig, doRatio, scaleBGtoData );
      makePlot( samples , names , dir_name , "h_lepptshort" , "p_{T} [GeV]" , "Events / Bin" , 0 , 20 , 1 , false, printplots, scalesig, doRatio, scaleBGtoData );
      
    }
  }
  
  std::cout << "\\documentclass[landscape, 10pt]{article}" << std::endl;
  std::cout << "\\usepackage{amsmath}" << std::endl;
  std::cout << "\\usepackage{amssymb}" << std::endl;
  std::cout << "\\usepackage{graphicx}" << std::endl;
  std::cout << "\\usepackage[left=.1in,top=1in,right=.1in,bottom=.1in,nohead]{geometry}" << std::endl;
  std::cout << "\\begin{document}" << std::endl;
  
  vector<string> dirs;
  dirs.push_back("srLep1L");
  dirs.push_back("srLep2L");
  dirs.push_back("srLep3L");
  dirs.push_back("srLep4L");
  dirs.push_back("srLep5L");
  dirs.push_back("srLep6L");
  dirs.push_back("srLep1M");
  dirs.push_back("srLep2M");
  dirs.push_back("srLep3M");
  dirs.push_back("srLep4M");
  dirs.push_back("srLep5M");
  dirs.push_back("srLep6M");
  dirs.push_back("srLepB3");
  dirs.push_back("srLepMET");
  dirs.push_back("srLepHT");
  dirs.push_back("srLepJ1L");
  dirs.push_back("srLepJ1M");
  dirs.push_back("srLepJ2L");
  dirs.push_back("srLepJ3L");
  dirs.push_back("srLepbaseJ");
  dirs.push_back("srLepbaseJ1");
  dirs.push_back("srLepbase");
  
  for(unsigned int i=0; i<dirs.size(); i++){
    printDetailedTable(samples, names, dirs.at(i));
    if(i % 3 == 0) std::cout << "\\pagebreak" << std::endl; //two tables per page
  }
  
  
  vector<string> dirsH;
  
  // dirsH.push_back("srLep1L");
  // dirsH.push_back("srLep2L");
  // dirsH.push_back("srLep3L");
  // dirsH.push_back("srLep4L");
  // dirsH.push_back("srLep5L");
  // dirsH.push_back("srLep6L");
  // printTable(samples, names, dirsH, -1);
  // dirsH.clear();

  
  // dirsH.push_back("srLep1M");
  // dirsH.push_back("srLep2M");
  // dirsH.push_back("srLep3M");
  // dirsH.push_back("srLep4M");
  // dirsH.push_back("srLep5M");
  // dirsH.push_back("srLep6M");
  // printTable(samples, names, dirsH, -1);
  // dirsH.clear();

  
  // dirsH.push_back("cr2Lbase");
  // dirsH.push_back("cr2L1L");
  // dirsH.push_back("cr2L2L");
  // dirsH.push_back("cr2L3L");
  // dirsH.push_back("cr2L4L");
  // dirsH.push_back("cr2L5L");
  // dirsH.push_back("cr2L6L");
  // dirsH.push_back("cr2L1M");
  // dirsH.push_back("cr2L2M");
  // dirsH.push_back("cr2L3M");
  // dirsH.push_back("cr2L4M");
  // dirsH.push_back("cr2L5M");
  // dirsH.push_back("cr2L6M");
  //printTable(samples, names, dirsH, -1);

  for(unsigned int i=0; i<dirsH.size(); i++){
    printDetailedTable(samples, names, dirsH.at(i));
    //if(i % 3 == 0) std::cout << "\\pagebreak" << std::endl; //two tables per page
  }

  
  dirsH.clear();

    
  std::cout << "\\end{document}" << std::endl;

}
