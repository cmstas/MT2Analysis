// C++
// #include <vector>

inline double getHistBinAndErr(TFile* file, const TString hname, const int ibin, double& err, bool warning = false) {
  TH1D* hist = (TH1D*) file->Get(hname);
  if (hist == 0) {
    if (warning) cout << __LINE__ << ": The histogram " << hname << " does not exist!\n";
    return 0.;
  }
  err = hist->GetBinError(ibin);
  return hist->GetBinContent(ibin);
}

TH1F* getRatios(TFile* sample, vector<string> dirs, string suffix = "") {

  int n_srbins = 10;            // for now

  vector<double> bgtot(n_srbins, 0.);
  vector<double> bgerr(n_srbins, 0.);

  // TH1F* hist = new TH1F(Form("h_%s", suffix.c_str()), "R(Z/#gamma) Ratio Hist", n_srbins, 0, n_srbins);
  TH1F* hist = new TH1F(Form("h_%s", suffix.c_str()), "Photon Purity for CRs", n_srbins, 0, n_srbins);

  for (unsigned int idir = 0; idir < dirs.size(); ++idir) {
    string dir = "srh" + dirs.at(idir);

    // TString fullhistname = dir + "/h_mt2binsRatioInt";
    TString fullhistname = dir + "/h_mt2binspurityIntFailSieieData";
    TH1D* h = (TH1D*) sample->Get(fullhistname);
    double yield = 0.;
    double err = 0.;
    if (h) {
      yield = h->IntegralAndError(0, -1, err);
    }
    bgtot[idir] = yield;
    bgerr[idir] = err;
    cout << " & " << dirs.at(idir);

  } // loop over all samples
  cout << endl;

  for (int ibin = 0; ibin < n_srbins; ++ibin) {
    hist->SetBinContent(ibin+1, bgtot[ibin]);
    hist->SetBinError(ibin+1, bgerr[ibin]);
    cout << " & " << setprecision(2) << bgtot[ibin] << " \\pm " << setprecision(1) << bgerr[ibin];
  }
  cout << endl;

  return hist;
}


int ZGRatioComparisonOld() {
  // TFile* f_tight = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/mMTnHcand/zinvFromGJ.root");
  // TFile* f_loose = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/minMTbmet/zinvFromGJ.root");

  TFile* f_tight = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/temp/purity.root");
  TFile* f_loose = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/minMTbmet/purity.root");

  // vector<string> dirs = {"1VL", "2VL", "1L", "2L", "1M", "2M", "3M", "1H", "2H", "3H"};
  // vector<string> dirs = {"1VL_H", "2VL_H", "1L_H", "2L_H", "1M_H", "2M_H", "3M_H", "1H_H", "2H_H", "3H_H"};
  vector<string> dirs = {"1VL_L", "2VL_L", "1L_L", "2L_L", "1M_L", "2M_L", "3M_L", "1H_L", "2H_L", "3H_L"};

  int n_srbins = 10;

  TH1F* h_tight = getRatios(f_tight, dirs, "tight");
  TH1F* h_loose = getRatios(f_loose, dirs, "loose");

  TCanvas* c0 = new TCanvas("c0", "c0", 800, 600);
  gStyle->SetOptStat("");
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetFrameBorderMode(0);
  TPad* mainPad = new TPad("1", "1", 0.0, 0.20, 1.0, 1.0);
  TPad* ratioPad = new TPad("2", "2", 0.0, 0.0, 1.0, 0.20);
  mainPad->SetTopMargin(0.08);
  mainPad->SetLeftMargin(0.12);
  mainPad->SetBottomMargin(0.08);
  ratioPad->SetTopMargin(0.05);
  ratioPad->SetLeftMargin(0.12);
  ratioPad->Draw();
  // mainPad->SetLogy();
  mainPad->Draw();
  mainPad->cd();

  h_loose->GetXaxis()->SetBinLabel(1, "1VL");
  h_loose->GetXaxis()->SetBinLabel(2, "2VL");
  h_loose->GetXaxis()->SetBinLabel(3, "1L");
  h_loose->GetXaxis()->SetBinLabel(4, "2L");
  h_loose->GetXaxis()->SetBinLabel(5, "1M");
  h_loose->GetXaxis()->SetBinLabel(6, "2M");
  h_loose->GetXaxis()->SetBinLabel(7, "3M");
  h_loose->GetXaxis()->SetBinLabel(8, "1H");
  h_loose->GetXaxis()->SetBinLabel(9, "2H");
  h_loose->GetXaxis()->SetBinLabel(10, "3H");
  // h_loose->GetXaxis()->LabelsOption("u");
  h_loose->GetXaxis()->SetLabelSize(0.06);
  // h_loose->GetYaxis()->SetRangeUser(0, 1.8);
  h_loose->GetYaxis()->SetRangeUser(0, 1.2);
  h_loose->SetMarkerStyle(20);
  h_tight->SetMarkerStyle(20);
  h_loose->SetMarkerColor(kRed);
  h_tight->SetMarkerColor(kBlack);
  h_loose->SetLineColor(kRed);
  h_tight->SetLineColor(kBlack);
  h_loose->Draw("PE");
  h_tight->Draw("PEsame");
  // TLegend* leg = new TLegend(0.47, 0.72, 0.73, 0.85);
  TLegend* leg = new TLegend(0.14, 0.42, 0.3, 0.55);
  leg->SetFillColor(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.042);
  leg->AddEntry(h_tight, "Cut on isHcand");
  leg->AddEntry(h_loose, "No cut on M(bb)");
  leg->Draw("same");

  ratioPad->cd();
  TH1F* h_ratio = (TH1F*) h_tight->Clone("h_ratio");
  h_ratio->Divide(h_tight, h_loose, 1, 1, "B");

  TH1F* h_axis_ratio = new TH1F("ratio_axis","", n_srbins, 0, n_srbins);
  h_axis_ratio->GetYaxis()->SetTitleOffset(0.25);
  h_axis_ratio->GetYaxis()->SetTitleSize(0.18);
  h_axis_ratio->GetYaxis()->SetLabelSize(0.15);
  h_axis_ratio->GetYaxis()->SetNdivisions(5);
  h_axis_ratio->GetYaxis()->SetRangeUser(0, 2.5);
  h_axis_ratio->GetYaxis()->SetTitle("Tight/Loose ");
  h_axis_ratio->GetXaxis()->SetTickLength(0.07);
  h_axis_ratio->GetXaxis()->SetTitleSize(0.);
  h_axis_ratio->GetXaxis()->SetLabelSize(0.);
  h_axis_ratio->Draw("axis");
  TLine* line1 = new TLine(0, 1, n_srbins, 1);
  line1->SetLineStyle(2);
  line1->SetLineColor(kRed);
  line1->Draw("same");
  h_ratio->SetMarkerStyle(20);
  h_ratio->Draw("PEsame");

  c0->SaveAs("ZGRatioComparison.pdf");

  return 0;
}

TH1F* getZGRatios(TFile* sample, vector<string> dirs, string suffix = "") {
  vector<double> bgtot;
  vector<double> bgerr;

  int n_srbins = dirs.size();

  for (string dir : dirs) {
    TString fullhistname = "sr" + dir + "/h_mt2binsRatioInt";
    double err = 0.;
    double yield = getHistBinAndErr(sample, fullhistname, 1, err, true);
    bgtot.push_back(yield);
    bgerr.push_back(err);
    cout << " & " << dir;
  } // loop over all samples
  cout << endl;

  TH1F* hist = new TH1F(Form("h_%s", suffix.c_str()), "R(Z/#gamma) Ratio Hist", n_srbins, 0, n_srbins);

  for (int ibin = 0; ibin < n_srbins; ++ibin) {
    hist->SetBinContent(ibin+1, bgtot[ibin]);
    hist->SetBinError(ibin+1, bgerr[ibin]);
    string label = dirs.at(ibin);
    label.erase(0, label.find("Incl")+4);
    hist->GetXaxis()->SetBinLabel(ibin+1, label.c_str());
    cout << " & " << setprecision(2) << bgtot[ibin] << " \\pm " << setprecision(1) << bgerr[ibin];
  }
  cout << endl;

  return hist;
}

TH1F* getPurities(TFile* sample, vector<string> dirs, string suffix = "") {
  vector<double> bgtot;
  vector<double> bgerr;

  int n_srbins = dirs.size();

  for (string dir : dirs) {
    TString fullhistname = "sr" + dir + "/h_mt2binspurityIntFailSieieData";
    double err = 0.;
    double yield = getHistBinAndErr(sample, fullhistname, 1, err, true);
    bgtot.push_back(yield);
    bgerr.push_back(err);
    cout << " & " << dir;
  } // loop over all samples
  cout << endl;

  TH1F* hist = new TH1F(Form("h_%s", suffix.c_str()), "Photon Purity for CRs", n_srbins, 0, n_srbins);

  for (int ibin = 0; ibin < n_srbins; ++ibin) {
    hist->SetBinContent(ibin+1, bgtot[ibin]);
    hist->SetBinError(ibin+1, bgerr[ibin]);
    string label = dirs.at(ibin);
    label.erase(0, label.find("Incl")+4);
    hist->GetXaxis()->SetBinLabel(ibin+1, label.c_str());
    cout << " & " << setprecision(2) << bgtot[ibin] << " \\pm " << setprecision(1) << bgerr[ibin];
  }
  cout << endl;

  return hist;
}

int ZGRatioDependencyCheck(TString indir = "/home/users/sicheng/working/MT2Analysis/MT2looper/output/dep_test") {

  TFile* f_zinv = new TFile(indir + "/zinvFromGJ.root");
  TFile* outfile = new TFile(indir + "/deptest_zgratio.root", "RECREATE");

  vector<vector<string>> list_dirs;
  list_dirs.push_back({"hAllIncl2b", "hAllIncl3b", "hAllInclgeq4b"});
  list_dirs.push_back({"hAllIncl2j", "hAllIncl3j", "hAllIncl4j", "hAllIncl5to6j", "hAllInclgeq7j"});
  list_dirs.push_back({"hAllInclhtVL", "hAllInclhtL", "hAllInclhtM", "hAllInclhtH", "hAllInclhtUH"});
  list_dirs.push_back({"hAllInclmetVL", "hAllInclmetL", "hAllInclmetM", "hAllInclmetH"});

  // TIter it(f_zinv->GetListOfKeys());
  // TKey* k;
  // while ((k = (TKey*)it())) {
  //   string sr_string = k->GetTitle();
  //   // sr_string.erase(0, 2);    //remove "sr" from front of string
  //   if (sr_string[0] < 'A' || sr_string[0] == 'b' || sr_string[0] == 'I') continue; // don't want the standard mt2 regions
  //   dirs.push_back(sr_string);
  // }
  outfile->cd();
  for (auto dirs : list_dirs) {
    string type = dirs[0];
    type.erase(0, 8);
    TH1F* h_dep = getZGRatios(f_zinv, dirs, type);
    h_dep->SetTitle((h_dep->GetTitle() + type).c_str());
    h_dep->Write();
    // delete h_dep;
    for (auto& dir : dirs) dir.replace(0, 4, "h");
    h_dep = getZGRatios(f_zinv, dirs, type);
    h_dep->SetName((h_dep->GetName() + type + "_h").c_str());
    h_dep->SetTitle((h_dep->GetTitle() + type + " hcand").c_str());
    h_dep->Write();
    // delete h_dep;
    for (auto& dir : dirs) dir.replace(0, 1, "H");
    h_dep = getZGRatios(f_zinv, dirs, type);
    h_dep->SetName((h_dep->GetName() + type + "_H").c_str());
    h_dep->SetTitle((h_dep->GetTitle() + type + " Hcand").c_str());
    h_dep->Write();
    // delete h_dep;
    for (auto& dir : dirs) dir.replace(0, 1, "Z");
    h_dep = getZGRatios(f_zinv, dirs, type);
    h_dep->SetName((h_dep->GetName() + type + "_Z").c_str());
    h_dep->SetTitle((h_dep->GetTitle() + type + " Zcand").c_str());
    h_dep->Write();
    // delete h_dep;
  }
  TH1F* h_mt2bins = (TH1F*) f_zinv->Get("srhAllInclmt2/h_mt2binsRatio");
  h_mt2bins->Write();
  h_mt2bins = (TH1F*) f_zinv->Get("srhInclmt2/h_mt2binsRatio");
  h_mt2bins->SetName(((string(h_mt2bins->GetName())) + "_h").c_str());
  h_mt2bins->SetTitle("mt2bins hcand");
  h_mt2bins->Write();
  h_mt2bins = (TH1F*) f_zinv->Get("srHInclmt2/h_mt2binsRatio");
  h_mt2bins->SetName(((string(h_mt2bins->GetName())) + "_H").c_str());
  h_mt2bins->SetTitle("mt2bins Hcand");
  h_mt2bins->Write();
  h_mt2bins = (TH1F*) f_zinv->Get("srZInclmt2/h_mt2binsRatio");
  h_mt2bins->SetName(((string(h_mt2bins->GetName())) + "_Z").c_str());
  h_mt2bins->SetTitle("mt2bins Zcand");
  h_mt2bins->Write();

  return 0;
}

// int PurityDependencyCheck(TString indir = "/home/users/sicheng/working/MT2Analysis/MT2looper/output/dep_test") {

//   TFile* f_purity = new TFile(indir + "/purity.root");
//   TFile* outfile = new TFile(indir + "/deptest_purity.root", "RECREATE");

//   vector<vector<string>> list_dirs;
//   list_dirs.push_back({"hAllIncl2b", "hAllIncl3b", "hAllInclgeq4b"});
//   list_dirs.push_back({"hAllIncl2j", "hAllIncl3j", "hAllIncl4j", "hAllIncl5to6j", "hAllInclgeq7j"});
//   list_dirs.push_back({"hAllInclhtVL", "hAllInclhtL", "hAllInclhtM", "hAllInclhtH", "hAllInclhtUH"});
//   list_dirs.push_back({"hAllInclmetVL", "hAllInclmetL", "hAllInclmetM", "hAllInclmetH"});

//   // TIter it(f_zinv->GetListOfKeys());
//   // TKey* k;
//   // while ((k = (TKey*)it())) {
//   //   string sr_string = k->GetTitle();
//   //   // sr_string.erase(0, 2);    //remove "sr" from front of string
//   //   if (sr_string[0] < 'A' || sr_string[0] == 'b' || sr_string[0] == 'I') continue; // don't want the standard mt2 regions
//   //   dirs.push_back(sr_string);
//   // }
//   outfile->cd();
//   for (auto dirs : list_dirs) {
//     string type = dirs[0];
//     type.erase(0, 8);
//     TH1F* h_dep = getPurities(f_zinv, dirs, type);
//     h_dep->SetTitle((h_dep->GetTitle() + type).c_str());
//     h_dep->Write();
//     // delete h_dep;
//     for (auto& dir : dirs) dir.replace(0, 4, "h");
//     h_dep = getPurities(f_zinv, dirs, type);
//     h_dep->SetName((h_dep->GetName() + type + "_h").c_str());
//     h_dep->SetTitle((h_dep->GetTitle() + type + " hcand").c_str());
//     h_dep->Write();
//     // delete h_dep;
//     for (auto& dir : dirs) dir.replace(0, 1, "H");
//     h_dep = getPurities(f_zinv, dirs, type);
//     h_dep->SetName((h_dep->GetName() + type + "_H").c_str());
//     h_dep->SetTitle((h_dep->GetTitle() + type + " Hcand").c_str());
//     h_dep->Write();
//     // delete h_dep;
//     for (auto& dir : dirs) dir.replace(0, 1, "Z");
//     h_dep = getPurities(f_zinv, dirs, type);
//     h_dep->SetName((h_dep->GetName() + type + "_Z").c_str());
//     h_dep->SetTitle((h_dep->GetTitle() + type + " Zcand").c_str());
//     h_dep->Write();
//     // delete h_dep;
//   }
//   TH1F* h_mt2bins = (TH1F*) f_zinv->Get("srhAllInclmt2/h_mt2binsRatio");
//   h_mt2bins->Write();
//   h_mt2bins = (TH1F*) f_zinv->Get("srhInclmt2/h_mt2binsRatio");
//   h_mt2bins->SetName(((string(h_mt2bins->GetName())) + "_h").c_str());
//   h_mt2bins->SetTitle("mt2bins hcand");
//   h_mt2bins->Write();
//   h_mt2bins = (TH1F*) f_zinv->Get("srHInclmt2/h_mt2binsRatio");
//   h_mt2bins->SetName(((string(h_mt2bins->GetName())) + "_H").c_str());
//   h_mt2bins->SetTitle("mt2bins Hcand");
//   h_mt2bins->Write();
//   h_mt2bins = (TH1F*) f_zinv->Get("srZInclmt2/h_mt2binsRatio");
//   h_mt2bins->SetName(((string(h_mt2bins->GetName())) + "_Z").c_str());
//   h_mt2bins->SetTitle("mt2bins Zcand");
//   h_mt2bins->Write();

//   return 0;
// }


void doubleRatioComparison(TFile* f_data, TFile* f_zinv, TString sr = "h") {

  vector<TString> inclPlots {"h_njbins", "h_nbjbins", "h_htbins", "h_htbins2", "h_mt2bins"};
  // if (sr == "") inclPlots.erase(inclPlots.end()-1); // temporary for not seg fault

  TCanvas* c0 = new TCanvas("c0", "c0", 800, 600);
  gStyle->SetOptStat("");
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetFrameBorderMode(0);

  for (TString plot : inclPlots) {
    TH1D* h_data = (TH1D*) f_data->Get("sr"+sr+"base/"+plot+"DGRatio");
    TH1D* h_zinv = (TH1D*) f_zinv->Get("sr"+sr+"base/"+plot+"DGRatio");

    if (h_data == 0) cout << "???: " << "sr"+sr+"base/"+plot+"DGRatio" << "  " << __LINE__ << endl;
    if (h_zinv == 0) cout << "???: " << __LINE__ << endl;
    c0->Clear();
    TPad* mainPad = new TPad("1", "1", 0.0, 0.20, 1.0, 1.0);
    TPad* ratioPad = new TPad("2", "2", 0.0, 0.0, 1.0, 0.20);
    mainPad->SetTopMargin(0.08);
    mainPad->SetLeftMargin(0.12);
    mainPad->SetBottomMargin(0.08);
    ratioPad->SetTopMargin(0.05);
    ratioPad->SetLeftMargin(0.12);
    ratioPad->Draw();
    // mainPad->SetLogy();
    mainPad->Draw();
    mainPad->cd();

    // h_zinv->GetXaxis()->SetLabelSize(0.06);
    float ymax = max(h_zinv->GetMaximum(), h_data->GetMaximum()) * 1.2;

    h_zinv->GetYaxis()->SetRangeUser(0, ymax);
    h_zinv->SetMarkerStyle(20);
    h_data->SetMarkerStyle(20);
    h_zinv->SetMarkerColor(kRed);
    h_data->SetMarkerColor(kBlack);
    h_zinv->SetLineColor(kRed);
    h_data->SetLineColor(kBlack);
    h_zinv->Draw("PE");
    h_data->Draw("PEsame");

    TLegend* leg = new TLegend(0.4, 0.8, 0.55, 0.89);
    leg->SetFillColor(0);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.042);
    leg->AddEntry(h_data, " Data R(Z_{ll}/#gamma)");
    leg->AddEntry(h_zinv, " MC  R(Z_{ll}/#gamma)");
    leg->Draw("same");

    ratioPad->cd();
    TH1F* h_ratio = (TH1F*) h_data->Clone("h_ratio");
    // h_ratio->Divide(h_data, h_zinv, 1, 1, "B");
    h_ratio->Divide(h_zinv);

    float xup = h_zinv->GetBinLowEdge(h_zinv->GetNbinsX())+h_zinv->GetBinWidth(h_zinv->GetNbinsX()); // what's easy way to get this??
    float xlow = h_zinv->GetBinLowEdge(1);
    TH1F* h_axis_ratio = new TH1F("ratio_axis","", h_zinv->GetNbinsX(), xlow, xup);
    h_axis_ratio->GetYaxis()->SetTitleOffset(0.25);
    h_axis_ratio->GetYaxis()->SetTitleSize(0.18);
    h_axis_ratio->GetYaxis()->SetLabelSize(0.15);
    h_axis_ratio->GetYaxis()->SetNdivisions(5);
    h_axis_ratio->GetYaxis()->SetRangeUser(0, 2);
    h_axis_ratio->GetYaxis()->SetTitle("Data/MC ");
    h_axis_ratio->GetXaxis()->SetTickLength(0.07);
    h_axis_ratio->GetXaxis()->SetTitleSize(0.);
    h_axis_ratio->GetXaxis()->SetLabelSize(0.);
    h_axis_ratio->Draw("axis");
    TLine* line1 = new TLine(xlow, 1, xup, 1);
    line1->SetLineStyle(2);
    line1->SetLineColor(kRed);
    line1->Draw("same");
    h_ratio->SetMarkerStyle(20);
    h_ratio->Draw("PEsame");

    c0->SaveAs("ZGRatioComparison_"+plot+"_"+sr+".pdf");

    delete h_axis_ratio;
  }
}

int ZGRatioComparison() {

  TFile* f_drat = new TFile("../MT2looper/output/temp/doubleRatio.root");
  TFile* f_zinv = new TFile("../MT2looper/output/temp/zinvFromGJ.root");
  doubleRatioComparison(f_drat, f_zinv, "h");
  doubleRatioComparison(f_drat, f_zinv, "H");
  doubleRatioComparison(f_drat, f_zinv, "Z");
  doubleRatioComparison(f_drat, f_zinv, "");

  // ZGRatioDependencyCheck();
  return 0;

}
