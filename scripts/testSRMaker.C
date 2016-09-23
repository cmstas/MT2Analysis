// -*- C++ -*-

/***************************************************************
 ****** The following needs to be added into MT2Looper.cc ******
 ***************************************************************

  --*-- LINE: 318 --*--
  testSR0 = SRBaseHcand;
  testSR1 = SRBaseHcand;
  testSR2 = SRBaseHcand;
  testSR3 = SRBaseHcand;
  testSR4 = SRBaseHcand;
  testSR5 = SRBaseHcand;
  testSR6 = SRBaseHcand;
  testSR7 = SRBaseHcand;
  testInclSR = SRBaseHcand;

  --*-- LINE: 1879 --*--
  std::map<std::string, float> valuesTest = values;
  if (testSR0.PassesSelection(valuesTest)) {
    if (t.nJet30 >= 4 && t.nJet30 < 6)
      fillHistosMT2Higgs(testSR0.crqcdHistMap, testSR0.GetNumberOfMT2Bins(), testSR0.GetMT2Bins(), "testsr0", suffix);
  }
  valuesTest = values;
  valuesTest["deltaPhiMin"] = (t.deltaPhiMin < 0.3)? 0.4 : 0.2;
  if (testSR1.PassesSelection(valuesTest)) {
    if (t.nJet30 >= 4 && t.nJet30 < 6)
      fillHistosMT2Higgs(testSR1.crqcdHistMap, testSR1.GetNumberOfMT2Bins(), testSR1.GetMT2Bins(), "testsr1", suffix);
  }
  valuesTest = values;
  valuesTest["mt2"] = (hcand_mt2_ < 200)? 300 : 100;
  if (testSR2.PassesSelection(valuesTest)) {
    if (t.nJet30 >= 4 && t.nJet30 < 6)
      fillHistosMT2Higgs(testSR2.crqcdHistMap, testSR2.GetNumberOfMT2Bins(), testSR2.GetMT2Bins(), "testsr2", suffix);
  }
  valuesTest = values;
  valuesTest["deltaPhiMin"] = (t.deltaPhiMin < 0.3)? 0.4 : 0.2;
  valuesTest["mt2"] = (hcand_mt2_ < 200)? 300 : 100;
  if (testSR3.PassesSelection(valuesTest)) {
    if (t.nJet30 >= 4 && t.nJet30 < 6)
      fillHistosMT2Higgs(testSR3.crqcdHistMap, testSR3.GetNumberOfMT2Bins(), testSR3.GetMT2Bins(), "testsr3", suffix);
  }
  valuesTest = values;
  valuesTest["mt2"] = (hcand_mt2_ < 100)? 300 : 100;
  if (testSR4.PassesSelection(valuesTest)) {
    if (t.nJet30 >= 4 && t.nJet30 < 6)
      fillHistosMT2Higgs(testSR4.crqcdHistMap, testSR4.GetNumberOfMT2Bins(), testSR4.GetMT2Bins(), "testsr4", suffix);
  }
  valuesTest = values;
  valuesTest["mt2"] = (hcand_mt2_ < 200)? 300 : 100;
  valuesTest["passesHtMet"] = ( (t.ht > 200. && t.met_pt > 100.) || (t.ht > 1000. && t.met_pt > 30.) );
  if (testSR5.PassesSelection(valuesTest)) {
    if (t.nJet30 >= 4 && t.nJet30 < 6)
      fillHistosMT2Higgs(testSR5.crqcdHistMap, testSR5.GetNumberOfMT2Bins(), testSR5.GetMT2Bins(), "testsr5", suffix);
  }
  // valuesTest = values;
  valuesTest["passesHtMet"] = ( (t.ht > 200. && t.met_pt > 50.) || (t.ht > 1000. && t.met_pt > 30.) );
  if (testSR6.PassesSelection(valuesTest)) {
    if (t.nJet30 >= 4 && t.nJet30 < 6)
      fillHistosMT2Higgs(testSR6.crqcdHistMap, testSR6.GetNumberOfMT2Bins(), testSR6.GetMT2Bins(), "testsr6", suffix);
  }
  valuesTest = values;
  valuesTest["deltaPhiMin"] = (t.deltaPhiMin < 0.3)? 0.4 : 0.2;
  valuesTest["mt2"] = (hcand_mt2_ < 200)? 300 : 100;
  valuesTest["passesHtMet"] = ( (t.ht > 200. && t.met_pt > 50.) || (t.ht > 1000. && t.met_pt > 30.) );
  if (testSR7.PassesSelection(valuesTest)) {
    if (t.nJet30 >= 4 && t.nJet30 < 6)
      fillHistosMT2Higgs(testSR7.crqcdHistMap, testSR7.GetNumberOfMT2Bins(), testSR7.GetMT2Bins(), "testsr7", suffix);
  }

  --*-- LINE: 1973 --*--
  bool fillInclSR = false;
  for (unsigned int srN = 4; srN < SRVecHcand.size(); srN++)
    if (SRVecHcand.at(srN).PassesSelection(values))
      fillInclSR = true;

  if (fillInclSR)
    fillHistosMT2Higgs(testInclSR.srHistMap, testInclSR.GetNumberOfMT2Bins(), testInclSR.GetMT2Bins(), "testInclSR", suffix);

  --*-- MT2Looper.h --*--
  SR testSR0;
  SR testSR1;
  SR testSR2;
  SR testSR3;
  SR testSR4;
  SR testSR5;
  SR testSR6;
  SR testSR7;
  SR testInclSR;

*********************************************************************/

#include <iostream>
#include <utility>
#include <vector>

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
#include "TKey.h"

using namespace std;


void testSRMaker(string input_dir, string dataname, string selec){

  // Running the script along
  // input_dir = "/home/users/sicheng/MT2Analysis/MT2looper/output/testsr" + selec;
  input_dir = "/home/users/sicheng/MT2Analysis/MT2looper/output/looseb" + selec;

  // string output_name = input_dir+"/qcdFromCRs.root";
  // std::cout << "Writing to file: " << output_name << std::endl;

  // get input files -- default to faking data with same MC file
  TFile* f_data = new TFile(Form("%s/%s.root", input_dir.c_str(), dataname.c_str()));
  TFile* f_qcd = new TFile(Form("%s/2015qcd_ht.root", input_dir.c_str()));
  TFile* f_zinv = new TFile(Form("%s/2015zinv_ht.root", input_dir.c_str()));
  // TFile* f_top = new TFile(Form("%s/top_ht.root", input_dir.c_str()));
  // TFile* f_wjets = new TFile(Form("%s/wjets_ht.root", input_dir.c_str()));
  TFile* f_lostlep = new TFile(Form("%s/lostlep.root", input_dir.c_str()));

  if(f_data->IsZombie() || f_qcd->IsZombie()) {
    std::cerr << "Input file does not exist" << std::endl;
    return;
  }

  TCanvas* c0 = new TCanvas("c0", "c0", 800, 600);

  TH1F* h_qcdratios = new TH1F("h_qcdratios", "Ratios of SR/CR", 8, 0, 8);
  for (int i = 0; i < 8; ++i) {
    double sryields = 0.;
    double srerr = 0.;
    double cryields = 0.;
    double crerr = 0.;
    TH1F* h_sryields = (TH1F*) f_qcd->Get(Form("testsr%d/h_Mbbhcand", i));
    if (h_sryields) sryields = h_sryields->IntegralAndError(0, -1, srerr);
    TH1F* h_cryields = (TH1F*) f_qcd->Get(Form("testsr%d/h_Mbbhcand_MbbCRall", i));
    if (h_cryields) cryields = h_cryields->IntegralAndError(0, -1, crerr);
    else {
      cout << "The CR has no statistics at testsr" << i << " !!" << endl;
      continue;
    }

    double ratio = sryields/cryields;
    srerr /= sryields;
    crerr /= cryields;
    double error = sqrt(srerr*srerr + crerr*crerr);
    cout << "At testsr" << i << ", the SR/CR ratio is: " << setprecision(3) << ratio << " +- " << error
         << " (" << sryields << "/" << cryields << ")." << endl;

    h_qcdratios->SetBinContent(i+1, ratio);
    h_qcdratios->SetBinError(i+1, ratio*error);
  }
  h_qcdratios->GetYaxis()->SetRangeUser(0, 3);
  h_qcdratios->SetLineColor(kRed-7);
  if (selec == "geq3b")
    h_qcdratios->SetTitle("QCD In/Out Ratio with #geq 3b");
  else if (selec == "geq7j")
    h_qcdratios->SetTitle("QCD In/Out Ratio with #geq 7j");
  else if (selec == "4to6j")
    h_qcdratios->SetTitle("QCD In/Out Ratio with 4-6 j");
  h_qcdratios->Draw("E1");

  TH1F* h_allratios = new TH1F("h_allratios", "Ratios of SR/CR", 8, 0, 8);
  for (int i = 0; i < 8; ++i) {
    double sryields = 0.;
    double srerr = 0.;
    double cryields = 0.;
    double crerr = 0.;
    double err = 0.;
    TH1F* h_sryields = (TH1F*) f_qcd->Get(Form("testsr%d/h_Mbbhcand", i));
    if (h_sryields) sryields += h_sryields->IntegralAndError(0, -1, err);
    srerr = sqrt(srerr*srerr + err*err);
    TH1F* h_cryields = (TH1F*) f_qcd->Get(Form("testsr%d/h_Mbbhcand_MbbCRall", i));
    if (h_cryields) cryields += h_cryields->IntegralAndError(0, -1, err);
    else err = 0.;
    crerr = sqrt(crerr*crerr + err*err);

    h_sryields = (TH1F*) f_lostlep->Get(Form("testsr%d/h_Mbbhcand", i));
    if (h_sryields) sryields += h_sryields->IntegralAndError(0, -1, err);
    srerr = sqrt(srerr*srerr + err*err);
    h_cryields = (TH1F*) f_lostlep->Get(Form("testsr%d/h_Mbbhcand_MbbCRall", i));
    if (h_cryields) cryields += h_cryields->IntegralAndError(0, -1, err);
    else err = 0.;
    crerr = sqrt(crerr*crerr + err*err);

    h_sryields = (TH1F*) f_zinv->Get(Form("testsr%d/h_Mbbhcand", i));
    if (h_sryields) sryields += h_sryields->IntegralAndError(0, -1, err);
    srerr = sqrt(srerr*srerr + err*err);
    h_cryields = (TH1F*) f_zinv->Get(Form("testsr%d/h_Mbbhcand_MbbCRall", i));
    if (h_cryields) cryields += h_cryields->IntegralAndError(0, -1, err);
    else err = 0.;
    crerr = sqrt(crerr*crerr + err*err);

    if (cryields == 0.)
      cout << "The CR has no statistics at testsr" << i << " !!" << endl;
    double ratio = sryields/cryields;
    srerr /= sryields;
    crerr /= cryields;
    double error = sqrt(srerr*srerr + crerr*crerr);
    cout << "At testsr" << i << ", the SR/CR ratio is: " << setprecision(3) << ratio << " +- " << error
         << " (" << sryields << "/" << cryields << ")." << endl;

    h_allratios->SetBinContent(i+1, ratio);
    h_allratios->SetBinError(i+1, ratio*error);
  }
  h_allratios->SetLineColor(kTeal+6);
  h_allratios->Draw("E1same");

  TH1F* h_dataratios = new TH1F("h_dataratios", "Ratios of SR/CR", 8, 0, 8);
  for (int i = 0; i < 8; ++i) {
    double sryields = 0.;
    double srerr = 0.;
    double cryields = 0.;
    double crerr = 0.;
    TH1F* h_sryields = (TH1F*) f_data->Get(Form("testsr%d/h_Mbbhcand", i));
    if (h_sryields) sryields = h_sryields->IntegralAndError(0, -1, srerr);
    TH1F* h_cryields = (TH1F*) f_data->Get(Form("testsr%d/h_Mbbhcand_MbbCRall", i));
    if (h_cryields) cryields = h_cryields->IntegralAndError(0, -1, crerr);
    else {
      cout << "The CR has no statistics at testsr" << i << " !!" << endl;
      continue;
    }
    double overlap = 0.;
    TH1F* h_overlap = (TH1F*) f_data->Get(Form("testsr%d/h_Mbbhcand_overlap", i));
    if (h_overlap) overlap = h_overlap->Integral(0, -1);

    double ratio = sryields/cryields;
    srerr /= sryields;
    crerr /= cryields;
    double error = sqrt(srerr*srerr + crerr*crerr);
    cout << "At testsr" << i << ", the SR/CR ratio is: " << setprecision(3) << ratio << " +- " << error
         << " (" << sryields << "/" << cryields << "), and overlap = " << overlap << " (" << overlap/sryields*100 << "%)" << endl;

    h_dataratios->SetBinContent(i+1, ratio);
    h_dataratios->SetBinError(i+1, ratio*error);
  }
  h_dataratios->SetLineColor(kBlack);
  h_dataratios->Draw("E1same");

  TH1F* h_corratios = new TH1F("h_corratios", "Ratios of SR/CR", 8, 0, 8);
  for (int i = 0; i < 8; ++i) {
    double sryields = 0.;
    double srerr = 0.;
    double cryields = 0.;
    double crerr = 0.;
    double err = 0.;
    TH1F* h_sryields = (TH1F*) f_data->Get(Form("testsr%d/h_Mbbhcand", i));
    if (h_sryields) sryields += h_sryields->IntegralAndError(0, -1, err);
    srerr = sqrt(srerr*srerr + err*err);
    TH1F* h_cryields = (TH1F*) f_data->Get(Form("testsr%d/h_Mbbhcand_MbbCRall", i));
    if (h_cryields) cryields += h_cryields->IntegralAndError(0, -1, err);
    else err = 0.;
    crerr = sqrt(crerr*crerr + err*err);
    double datasr = sryields;
    double datacr = cryields;

    h_sryields = (TH1F*) f_lostlep->Get(Form("testsr%d/h_Mbbhcand", i));
    if (h_sryields) sryields -= h_sryields->IntegralAndError(0, -1, err);
    srerr = sqrt(srerr*srerr + err*err);
    h_cryields = (TH1F*) f_lostlep->Get(Form("testsr%d/h_Mbbhcand_MbbCRall", i));
    if (h_cryields) cryields -= h_cryields->IntegralAndError(0, -1, err);
    else err = 0.;
    crerr = sqrt(crerr*crerr + err*err);

    h_sryields = (TH1F*) f_zinv->Get(Form("testsr%d/h_Mbbhcand", i));
    if (h_sryields) sryields -= h_sryields->IntegralAndError(0, -1, err);
    srerr = sqrt(srerr*srerr + err*err);
    h_cryields = (TH1F*) f_zinv->Get(Form("testsr%d/h_Mbbhcand_MbbCRall", i));
    if (h_cryields) cryields -= h_cryields->IntegralAndError(0, -1, err);
    else err = 0.;
    crerr = sqrt(crerr*crerr + err*err);

    if (cryields == 0.)
      cout << "The CR has no statistics at testsr" << i << " !!" << endl;
    double ratio = sryields/cryields;
    srerr /= sryields;
    crerr /= cryields;
    double error = sqrt(srerr*srerr + crerr*crerr);
    cout << "At testsr" << i << ", the SR/CR ratio is: " << setprecision(3) << ratio << " +- " << error
         << " (" << sryields << "/" << cryields << ")."
         << " The non-QCD/data ratio In is: " << sryields/datasr << " (" << sryields << "/" << datasr << "), "
         << "Out is: " << cryields/datacr << " (" << cryields << "/" << datacr << ")" << endl;

    h_corratios->SetBinContent(i+1, ratio);
    h_corratios->SetBinError(i+1, ratio*error);
  }
  h_corratios->SetLineColor(kAzure-3);
  h_corratios->Draw("E1same");

  gStyle->SetOptStat("");
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetFrameBorderMode(0);
  TLegend* leg = new TLegend(0.70, 0.2, 0.88, 0.33);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->AddEntry(h_qcdratios, "qcd ratios");
  leg->AddEntry(h_corratios, "corrected ratios");
  leg->AddEntry(h_dataratios, "data ratios");
  leg->Draw("same");

  c0->SaveAs(Form("qcdInNOutRatio_%s.pdf", selec.c_str()));
  //Loop through list of every directory in the signal file.
  //if directory begins with "sr", excluding "srbase", add it to vector signal regions.

}

void testSRMaker(string input_dir = "/home/users/sicheng/MT2Analysis/MT2looper/output/temp", string dataname = "data_Run2016") {
  testSRMaker(input_dir, dataname, "4to6j");
  testSRMaker(input_dir, dataname, "geq7j");
  // testSRMaker(input_dir, dataname, "geq3b");
}
