TH1F* getRatios(TFile* sample, vector<string> dirs, string suffix = "") {

  int n_srbins = 10;            // for now

  vector<double> bgtot(n_srbins, 0.);
  vector<double> bgerr(n_srbins, 0.);

  TH1F* hist = new TH1F(Form("h_%s", suffix.c_str()), "R(Z/#gamma) Ratio Hist", n_srbins, 0, n_srbins);
  // TH1F* hist = new TH1F(Form("h_%s", suffix.c_str()), "Photo Purity for CRs", n_srbins, 0, n_srbins);

  for (unsigned int idir = 0; idir < dirs.size(); ++idir) {
    string dir = "srh" + dirs.at(idir);

    TString fullhistname = dir + "/h_mt2binsRatioInt";
    // TString fullhistname = dir + "/h_mt2binspurityIntFailSieieData";
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
    cout << " & " << setprecision(2) << bgtot[ibin] << " \\pm " << bgerr[ibin];
  }
  cout << endl;

  return hist;
}


int ZGRatioComparison()
{
  TFile* f_tight = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/mMTnHcand/zinvFromGJ.root");
  TFile* f_loose = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/minMTbmet/zinvFromGJ.root");

  // TFile* f_tight = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/mMTnHcand/purity.root");
  // TFile* f_loose = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/minMTbmet/purity.root");

  // vector<string> dirs = {"1VL", "2VL", "1L", "2L", "1M", "2M", "3M", "1H", "2H", "3H"};
  vector<string> dirs = {"1VL", "2VL", "1L", "2L", "1M", "2M", "3M", "1H", "2H", "3H"};

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
  h_loose->GetYaxis()->SetRangeUser(0, 1.8);
  h_loose->SetMarkerStyle(20);
  h_tight->SetMarkerStyle(20);
  h_loose->SetMarkerColor(kRed);
  h_tight->SetMarkerColor(kBlack);
  h_loose->SetLineColor(kRed);
  h_tight->SetLineColor(kBlack);
  h_loose->Draw("PE");
  h_tight->Draw("PEsame");
  TLegend* leg = new TLegend(0.47, 0.72, 0.73, 0.85);
  // TLegend* leg = new TLegend(0.14, 0.42, 0.3, 0.55);
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
