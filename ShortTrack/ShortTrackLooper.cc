#include "ShortTrackLooper.h"

using namespace std;
using namespace duplicate_removal;

class mt2tree;

const bool recalculate = true; // to use a non standard (ie not in babies) Fshort, change the iso and qual cuts and provide a new Fshort input file below
const int applyRecoVeto = 2; // 0: None, 1: use MT2 ID leptons for the reco veto, 2: use any Reco ID (Default: 2)
const bool increment17 = false;
const float isoSTC = 6, qualSTC = 3;
bool adjL = true; // use fshort'(M) = fshort(M) * fshort(L)_mc / fshort(M)_mc instead of raw fshort(M) in place of fshort(L)
const bool EventWise = false; // Count events with Nst == 1 and Nst == 2, or just counts STs?

// turn on to apply json file to data
const bool applyJSON = true;
const bool blind = false;

const bool applyISRWeights = true; // evt_id is messed up in current babies, and we don't have isr weights implemented for 2017 MC anyway

const bool doNTrueIntReweight = true; // evt_id is messed up in current babies

bool doHEMveto = true;
int HEM_startRun = 319077; // affects 38.58 out of 58.83 fb-1 in 2018
uint HEM_fracNum = 1286, HEM_fracDen = 1961; // 38.58/58.82 ~= 1286/1961. Used for figuring out if we should veto MC events
float HEM_ptCut = 30.0;  // veto on jets above this threshold
float HEM_region[4] = {-4.7, -1.4, -1.6, -0.8}; // etalow, etahigh, philow, phihigh

// turn on to apply L1prefire inefficiency weights to MC (2016/17 only)
bool applyL1PrefireWeights = false;

TFile VetoFile("../FshortLooper/veto_etaphi.root");
TH2F* veto_etaphi_16 = (TH2F*) VetoFile.Get("etaphi_veto_16");
TH2F* veto_etaphi_17 = (TH2F*) VetoFile.Get("etaphi_veto_17");
TH2F* veto_etaphi_18 = (TH2F*) VetoFile.Get("etaphi_veto_18");

int ShortTrackLooper::InEtaPhiVetoRegion(float eta, float phi, int year) {
  if (fabs(eta) > 2.4) return -1;
  /*
  else if (eta > 1.4) {
    float bc = veto_ecp->GetBinContent(veto_ecp->FindBin(eta,phi));
    if (bc > 0) return 3;
  } else if (eta < -1.4) {
    float bc = veto_ecn->GetBinContent(veto_ecn->FindBin(eta,phi));
    if (bc > 0) return 2;
  } else {
    float bc = veto_bar->GetBinContent(veto_bar->FindBin(eta,phi));
    if (bc > 0) return 1;
  }
  if (eta < -1.05 && eta > -1.15 && phi < -1.8 && phi > -2.1) return 4;  
  */
  TH2F* veto_hist = veto_etaphi_16;
  if (year == 2017) veto_hist = veto_etaphi_17;
  if (year == 2018) veto_hist = veto_etaphi_18;
  return (int) veto_hist->GetBinContent(veto_hist->FindBin(eta,phi));
}

int ShortTrackLooper::loop (TChain* ch, char * outtag, std::string config_tag, char * runtag) {
  string tag(outtag);
  bool isSignal = tag.find("sim") != std::string::npos;
  const bool isMC = isSignal || config_tag.find("mc") != std::string::npos;


  TH1F* h_xsec = 0;
  if (isSignal) {
    TFile * f_xsec = TFile::Open("../babymaker/data/xsec_susy_13tev.root");
    TH1F* h_xsec_orig = (TH1F*) f_xsec->Get("h_xsec_gluino");
    h_xsec = (TH1F*) h_xsec_orig->Clone("h_xsec");
    h_xsec->SetDirectory(0);
    f_xsec->Close();
  }

  cout << "Using runtag: " << runtag << endl;

  const TString FshortName16Data = Form("../FshortLooper/output/Fshort_data_2016_%s.root",runtag);
  const TString FshortName16MC = Form("../FshortLooper/output/Fshort_mc_2016_%s.root",runtag);
  //const TString FshortName17Data = Form("../FshortLooper/output/Fshort_data_2017_%s.root",runtag);
  const TString FshortName17Data = Form("../FshortLooper/output/Fshort_data_2017and2018_%s.root",runtag);
  const TString FshortName17MC = Form("../FshortLooper/output/Fshort_mc_2017_%s.root",runtag);
  //const TString FshortName18Data = Form("../FshortLooper/output/Fshort_data_2018_%s.root",runtag);
  const TString FshortName18Data = Form("../FshortLooper/output/Fshort_data_2017and2018_%s.root",runtag);
  //  const TString FshortName18MC = Form("../FshortLooper/output/Fshort_mc_2018_%s.root",runtag);

  // Book histograms
  TH1::SetDefaultSumw2(true); // Makes histograms do proper error calculation automatically

  const int Pidx = 1;
  const int P3idx = 2;
  const int P4idx = 3;
  const int Midx = 4;
  const int Lidx = 5;
  //  const int LREJidx = 6;

  TH2D h_eventwise_counts ("h_eventwise_counts","Events Counts by Length",5,0,5,3,0,3);
  h_eventwise_counts.GetYaxis()->SetTitleOffset(3.0);
  h_eventwise_counts.GetXaxis()->SetBinLabel(Pidx,"P");
  h_eventwise_counts.GetXaxis()->SetBinLabel(P3idx,"P");
  h_eventwise_counts.GetXaxis()->SetBinLabel(P4idx,"P");
  h_eventwise_counts.GetXaxis()->SetBinLabel(Midx,"M");
  h_eventwise_counts.GetXaxis()->SetBinLabel(Lidx,"L (acc)");
  h_eventwise_counts.GetYaxis()->SetBinLabel(1,"Obs SR Events");
  h_eventwise_counts.GetYaxis()->SetBinLabel(2,"Pred SR Events");
  h_eventwise_counts.GetYaxis()->SetBinLabel(3,"STC CR Events");

  TH2D h_eventwise_countsVR = *((TH2D*) h_eventwise_counts.Clone("h_eventwise_countsVR"));
  h_eventwise_countsVR.GetYaxis()->SetBinLabel(1,"Obs VR Events");
  h_eventwise_countsVR.GetYaxis()->SetBinLabel(2,"Pred VR Events");

  // Nlep = 0 hists

  // NjHt
  TH2D* h_LL_MR = (TH2D*)h_eventwise_countsVR.Clone("h_LL_MR");
  TH2D* h_LM_MR = (TH2D*)h_eventwise_countsVR.Clone("h_LM_MR");
  TH2D* h_LH_MR = (TH2D*)h_eventwise_countsVR.Clone("h_LH_MR");
  TH2D* h_HL_MR = (TH2D*)h_eventwise_countsVR.Clone("h_HL_MR");
  TH2D* h_HM_MR = (TH2D*)h_eventwise_countsVR.Clone("h_HM_MR");
  TH2D* h_HH_MR = (TH2D*)h_eventwise_countsVR.Clone("h_HH_MR");

  TH2D* h_LL_VR = (TH2D*)h_eventwise_countsVR.Clone("h_LL_VR");
  TH2D* h_LM_VR = (TH2D*)h_eventwise_countsVR.Clone("h_LM_VR");
  TH2D* h_LH_VR = (TH2D*)h_eventwise_countsVR.Clone("h_LH_VR");
  TH2D* h_HL_VR = (TH2D*)h_eventwise_countsVR.Clone("h_HL_VR");
  TH2D* h_HM_VR = (TH2D*)h_eventwise_countsVR.Clone("h_HM_VR");
  TH2D* h_HH_VR = (TH2D*)h_eventwise_countsVR.Clone("h_HH_VR");

  TH2D* h_LL_SR = (TH2D*)h_eventwise_counts.Clone("h_LL_SR");
  TH2D* h_LM_SR = (TH2D*)h_eventwise_counts.Clone("h_LM_SR");
  TH2D* h_LH_SR = (TH2D*)h_eventwise_counts.Clone("h_LH_SR");
  TH2D* h_HL_SR = (TH2D*)h_eventwise_counts.Clone("h_HL_SR");
  TH2D* h_HM_SR = (TH2D*)h_eventwise_counts.Clone("h_HM_SR");
  TH2D* h_HH_SR = (TH2D*)h_eventwise_counts.Clone("h_HH_SR");

  // Separate fshorts
  TH2D* h_LL_MR_23 = (TH2D*)h_eventwise_countsVR.Clone("h_LL_MR_23");
  TH2D* h_LM_MR_23 = (TH2D*)h_eventwise_countsVR.Clone("h_LM_MR_23");
  TH2D* h_LH_MR_23 = (TH2D*)h_eventwise_countsVR.Clone("h_LH_MR_23");
  TH2D* h_HL_MR_4 = (TH2D*)h_eventwise_countsVR.Clone("h_HL_MR_4");
  TH2D* h_HM_MR_4 = (TH2D*)h_eventwise_countsVR.Clone("h_HM_MR_4");
  TH2D* h_HH_MR_4 = (TH2D*)h_eventwise_countsVR.Clone("h_HH_MR_4");

  TH2D* h_LL_VR_23 = (TH2D*)h_eventwise_countsVR.Clone("h_LL_VR_23");
  TH2D* h_LM_VR_23 = (TH2D*)h_eventwise_countsVR.Clone("h_LM_VR_23");
  TH2D* h_LH_VR_23 = (TH2D*)h_eventwise_countsVR.Clone("h_LH_VR_23");
  TH2D* h_HL_VR_4 = (TH2D*)h_eventwise_countsVR.Clone("h_HL_VR_4");
  TH2D* h_HM_VR_4 = (TH2D*)h_eventwise_countsVR.Clone("h_HM_VR_4");
  TH2D* h_HH_VR_4 = (TH2D*)h_eventwise_countsVR.Clone("h_HH_VR_4");

  TH2D* h_LL_SR_23 = (TH2D*)h_eventwise_counts.Clone("h_LL_SR_23");
  TH2D* h_LM_SR_23 = (TH2D*)h_eventwise_counts.Clone("h_LM_SR_23");
  TH2D* h_LH_SR_23 = (TH2D*)h_eventwise_counts.Clone("h_LH_SR_23");
  TH2D* h_HL_SR_4 = (TH2D*)h_eventwise_counts.Clone("h_HL_SR_4");
  TH2D* h_HM_SR_4 = (TH2D*)h_eventwise_counts.Clone("h_HM_SR_4");
  TH2D* h_HH_SR_4 = (TH2D*)h_eventwise_counts.Clone("h_HH_SR_4");

  // Hi pt

  // NjHt
  TH2D* h_LL_MR_hi = (TH2D*)h_eventwise_countsVR.Clone("h_LL_MR_hi");
  TH2D* h_LM_MR_hi = (TH2D*)h_eventwise_countsVR.Clone("h_LM_MR_hi");
  TH2D* h_LH_MR_hi = (TH2D*)h_eventwise_countsVR.Clone("h_LH_MR_hi");
  TH2D* h_HL_MR_hi = (TH2D*)h_eventwise_countsVR.Clone("h_HL_MR_hi");
  TH2D* h_HM_MR_hi = (TH2D*)h_eventwise_countsVR.Clone("h_HM_MR_hi");
  TH2D* h_HH_MR_hi = (TH2D*)h_eventwise_countsVR.Clone("h_HH_MR_hi");

  TH2D* h_LL_VR_hi = (TH2D*)h_eventwise_countsVR.Clone("h_LL_VR_hi");
  TH2D* h_LM_VR_hi = (TH2D*)h_eventwise_countsVR.Clone("h_LM_VR_hi");
  TH2D* h_LH_VR_hi = (TH2D*)h_eventwise_countsVR.Clone("h_LH_VR_hi");
  TH2D* h_HL_VR_hi = (TH2D*)h_eventwise_countsVR.Clone("h_HL_VR_hi");
  TH2D* h_HM_VR_hi = (TH2D*)h_eventwise_countsVR.Clone("h_HM_VR_hi");
  TH2D* h_HH_VR_hi = (TH2D*)h_eventwise_countsVR.Clone("h_HH_VR_hi");

  TH2D* h_LL_SR_hi = (TH2D*)h_eventwise_counts.Clone("h_LL_SR_hi");
  TH2D* h_LM_SR_hi = (TH2D*)h_eventwise_counts.Clone("h_LM_SR_hi");
  TH2D* h_LH_SR_hi = (TH2D*)h_eventwise_counts.Clone("h_LH_SR_hi");
  TH2D* h_HL_SR_hi = (TH2D*)h_eventwise_counts.Clone("h_HL_SR_hi");
  TH2D* h_HM_SR_hi = (TH2D*)h_eventwise_counts.Clone("h_HM_SR_hi");
  TH2D* h_HH_SR_hi = (TH2D*)h_eventwise_counts.Clone("h_HH_SR_hi");

  // Separate fshorts
  TH2D* h_LL_MR_23_hi = (TH2D*)h_eventwise_countsVR.Clone("h_LL_MR_23_hi");
  TH2D* h_LM_MR_23_hi = (TH2D*)h_eventwise_countsVR.Clone("h_LM_MR_23_hi");
  TH2D* h_LH_MR_23_hi = (TH2D*)h_eventwise_countsVR.Clone("h_LH_MR_23_hi");
  TH2D* h_HL_MR_4_hi = (TH2D*)h_eventwise_countsVR.Clone("h_HL_MR_4_hi");
  TH2D* h_HM_MR_4_hi = (TH2D*)h_eventwise_countsVR.Clone("h_HM_MR_4_hi");
  TH2D* h_HH_MR_4_hi = (TH2D*)h_eventwise_countsVR.Clone("h_HH_MR_4_hi");

  TH2D* h_LL_VR_23_hi = (TH2D*)h_eventwise_countsVR.Clone("h_LL_VR_23_hi");
  TH2D* h_LM_VR_23_hi = (TH2D*)h_eventwise_countsVR.Clone("h_LM_VR_23_hi");
  TH2D* h_LH_VR_23_hi = (TH2D*)h_eventwise_countsVR.Clone("h_LH_VR_23_hi");
  TH2D* h_HL_VR_4_hi = (TH2D*)h_eventwise_countsVR.Clone("h_HL_VR_4_hi");
  TH2D* h_HM_VR_4_hi = (TH2D*)h_eventwise_countsVR.Clone("h_HM_VR_4_hi");
  TH2D* h_HH_VR_4_hi = (TH2D*)h_eventwise_countsVR.Clone("h_HH_VR_4_hi");

  TH2D* h_LL_SR_23_hi = (TH2D*)h_eventwise_counts.Clone("h_LL_SR_23_hi");
  TH2D* h_LM_SR_23_hi = (TH2D*)h_eventwise_counts.Clone("h_LM_SR_23_hi");
  TH2D* h_LH_SR_23_hi = (TH2D*)h_eventwise_counts.Clone("h_LH_SR_23_hi");
  TH2D* h_HL_SR_4_hi = (TH2D*)h_eventwise_counts.Clone("h_HL_SR_4_hi");
  TH2D* h_HM_SR_4_hi = (TH2D*)h_eventwise_counts.Clone("h_HM_SR_4_hi");
  TH2D* h_HH_SR_4_hi = (TH2D*)h_eventwise_counts.Clone("h_HH_SR_4_hi");

  // Lo pt
  // NjHt
  TH2D* h_LL_MR_lo = (TH2D*)h_eventwise_countsVR.Clone("h_LL_MR_lo");
  TH2D* h_LM_MR_lo = (TH2D*)h_eventwise_countsVR.Clone("h_LM_MR_lo");
  TH2D* h_LH_MR_lo = (TH2D*)h_eventwise_countsVR.Clone("h_LH_MR_lo");
  TH2D* h_HL_MR_lo = (TH2D*)h_eventwise_countsVR.Clone("h_HL_MR_lo");
  TH2D* h_HM_MR_lo = (TH2D*)h_eventwise_countsVR.Clone("h_HM_MR_lo");
  TH2D* h_HH_MR_lo = (TH2D*)h_eventwise_countsVR.Clone("h_HH_MR_lo");

  TH2D* h_LL_VR_lo = (TH2D*)h_eventwise_countsVR.Clone("h_LL_VR_lo");
  TH2D* h_LM_VR_lo = (TH2D*)h_eventwise_countsVR.Clone("h_LM_VR_lo");
  TH2D* h_LH_VR_lo = (TH2D*)h_eventwise_countsVR.Clone("h_LH_VR_lo");
  TH2D* h_HL_VR_lo = (TH2D*)h_eventwise_countsVR.Clone("h_HL_VR_lo");
  TH2D* h_HM_VR_lo = (TH2D*)h_eventwise_countsVR.Clone("h_HM_VR_lo");
  TH2D* h_HH_VR_lo = (TH2D*)h_eventwise_countsVR.Clone("h_HH_VR_lo");

  TH2D* h_LL_SR_lo = (TH2D*)h_eventwise_counts.Clone("h_LL_SR_lo");
  TH2D* h_LM_SR_lo = (TH2D*)h_eventwise_counts.Clone("h_LM_SR_lo");
  TH2D* h_LH_SR_lo = (TH2D*)h_eventwise_counts.Clone("h_LH_SR_lo");
  TH2D* h_HL_SR_lo = (TH2D*)h_eventwise_counts.Clone("h_HL_SR_lo");
  TH2D* h_HM_SR_lo = (TH2D*)h_eventwise_counts.Clone("h_HM_SR_lo");
  TH2D* h_HH_SR_lo = (TH2D*)h_eventwise_counts.Clone("h_HH_SR_lo");

  // Separate fshorts
  TH2D* h_LL_MR_23_lo = (TH2D*)h_eventwise_countsVR.Clone("h_LL_MR_23_lo");
  TH2D* h_LM_MR_23_lo = (TH2D*)h_eventwise_countsVR.Clone("h_LM_MR_23_lo");
  TH2D* h_LH_MR_23_lo = (TH2D*)h_eventwise_countsVR.Clone("h_LH_MR_23_lo");
  TH2D* h_HL_MR_4_lo = (TH2D*)h_eventwise_countsVR.Clone("h_HL_MR_4_lo");
  TH2D* h_HM_MR_4_lo = (TH2D*)h_eventwise_countsVR.Clone("h_HM_MR_4_lo");
  TH2D* h_HH_MR_4_lo = (TH2D*)h_eventwise_countsVR.Clone("h_HH_MR_4_lo");

  TH2D* h_LL_VR_23_lo = (TH2D*)h_eventwise_countsVR.Clone("h_LL_VR_23_lo");
  TH2D* h_LM_VR_23_lo = (TH2D*)h_eventwise_countsVR.Clone("h_LM_VR_23_lo");
  TH2D* h_LH_VR_23_lo = (TH2D*)h_eventwise_countsVR.Clone("h_LH_VR_23_lo");
  TH2D* h_HL_VR_4_lo = (TH2D*)h_eventwise_countsVR.Clone("h_HL_VR_4_lo");
  TH2D* h_HM_VR_4_lo = (TH2D*)h_eventwise_countsVR.Clone("h_HM_VR_4_lo");
  TH2D* h_HH_VR_4_lo = (TH2D*)h_eventwise_countsVR.Clone("h_HH_VR_4_lo");

  TH2D* h_LL_SR_23_lo = (TH2D*)h_eventwise_counts.Clone("h_LL_SR_23_lo");
  TH2D* h_LM_SR_23_lo = (TH2D*)h_eventwise_counts.Clone("h_LM_SR_23_lo");
  TH2D* h_LH_SR_23_lo = (TH2D*)h_eventwise_counts.Clone("h_LH_SR_23_lo");
  TH2D* h_HL_SR_4_lo = (TH2D*)h_eventwise_counts.Clone("h_HL_SR_4_lo");
  TH2D* h_HM_SR_4_lo = (TH2D*)h_eventwise_counts.Clone("h_HM_SR_4_lo");
  TH2D* h_HH_SR_4_lo = (TH2D*)h_eventwise_counts.Clone("h_HH_SR_4_lo");

  unordered_map<TH2D*,TH2D*> low_hists;
  low_hists[h_LL_MR] = h_LL_MR_lo;
  low_hists[h_LM_MR] = h_LM_MR_lo;
  low_hists[h_LH_MR] = h_LH_MR_lo;
  low_hists[h_HL_MR] = h_HL_MR_lo;
  low_hists[h_HM_MR] = h_HM_MR_lo;
  low_hists[h_HH_MR] = h_HH_MR_lo;
  low_hists[h_LL_VR] = h_LL_VR_lo;
  low_hists[h_LM_VR] = h_LM_VR_lo;
  low_hists[h_LH_VR] = h_LH_VR_lo;
  low_hists[h_HL_VR] = h_HL_VR_lo;
  low_hists[h_HM_VR] = h_HM_VR_lo;
  low_hists[h_HH_VR] = h_HH_VR_lo;
  low_hists[h_LL_SR] = h_LL_SR_lo;
  low_hists[h_LM_SR] = h_LM_SR_lo;
  low_hists[h_LH_SR] = h_LH_SR_lo;
  low_hists[h_HL_SR] = h_HL_SR_lo;
  low_hists[h_HM_SR] = h_HM_SR_lo;
  low_hists[h_HH_SR] = h_HH_SR_lo;
  low_hists[h_LL_MR_23] = h_LL_MR_23_lo;
  low_hists[h_LM_MR_23] = h_LM_MR_23_lo;
  low_hists[h_LH_MR_23] = h_LH_MR_23_lo;
  low_hists[h_LL_VR_23] = h_LL_VR_23_lo;
  low_hists[h_LM_VR_23] = h_LM_VR_23_lo;
  low_hists[h_LH_VR_23] = h_LH_VR_23_lo;
  low_hists[h_HL_MR_4] = h_HL_MR_4_lo;
  low_hists[h_HM_MR_4] = h_HM_MR_4_lo;
  low_hists[h_HH_MR_4] = h_HH_MR_4_lo;
  low_hists[h_HL_VR_4] = h_HL_VR_4_lo;
  low_hists[h_HM_VR_4] = h_HM_VR_4_lo;
  low_hists[h_HH_VR_4] = h_HH_VR_4_lo;
  low_hists[h_LL_SR_23] = h_LL_SR_23_lo;
  low_hists[h_LM_SR_23] = h_LM_SR_23_lo;
  low_hists[h_LH_SR_23] = h_LH_SR_23_lo;
  low_hists[h_HL_SR_4] = h_HL_SR_4_lo;
  low_hists[h_HM_SR_4] = h_HM_SR_4_lo;
  low_hists[h_HH_SR_4] = h_HH_SR_4_lo;

  unordered_map<TH2D*,TH2D*> hi_hists;
  hi_hists[h_LL_MR] = h_LL_MR_hi;
  hi_hists[h_LM_MR] = h_LM_MR_hi;
  hi_hists[h_LH_MR] = h_LH_MR_hi;
  hi_hists[h_LL_VR] = h_LL_VR_hi;
  hi_hists[h_LM_VR] = h_LM_VR_hi;
  hi_hists[h_LH_VR] = h_LH_VR_hi;
  hi_hists[h_HL_MR] = h_HL_MR_hi;
  hi_hists[h_HM_MR] = h_HM_MR_hi;
  hi_hists[h_HH_MR] = h_HH_MR_hi;
  hi_hists[h_HL_VR] = h_HL_VR_hi;
  hi_hists[h_HM_VR] = h_HM_VR_hi;
  hi_hists[h_HH_VR] = h_HH_VR_hi;
  hi_hists[h_LL_SR] = h_LL_SR_hi;
  hi_hists[h_LM_SR] = h_LM_SR_hi;
  hi_hists[h_LH_SR] = h_LH_SR_hi;
  hi_hists[h_HL_SR] = h_HL_SR_hi;
  hi_hists[h_HM_SR] = h_HM_SR_hi;
  hi_hists[h_HH_SR] = h_HH_SR_hi;
  hi_hists[h_LL_MR_23] = h_LL_MR_23_hi;
  hi_hists[h_LM_MR_23] = h_LM_MR_23_hi;
  hi_hists[h_LH_MR_23] = h_LH_MR_23_hi;
  hi_hists[h_HL_MR_4] = h_HL_MR_4_hi;
  hi_hists[h_HM_MR_4] = h_HM_MR_4_hi;
  hi_hists[h_HH_MR_4] = h_HH_MR_4_hi;
  hi_hists[h_LL_VR_23] = h_LL_VR_23_hi;
  hi_hists[h_LM_VR_23] = h_LM_VR_23_hi;
  hi_hists[h_LH_VR_23] = h_LH_VR_23_hi;
  hi_hists[h_HL_VR_4] = h_HL_VR_4_hi;
  hi_hists[h_HM_VR_4] = h_HM_VR_4_hi;
  hi_hists[h_HH_VR_4] = h_HH_VR_4_hi;
  hi_hists[h_LL_SR_23] = h_LL_SR_23_hi;
  hi_hists[h_LM_SR_23] = h_LM_SR_23_hi;
  hi_hists[h_LH_SR_23] = h_LH_SR_23_hi;
  hi_hists[h_HL_SR_4] = h_HL_SR_4_hi;
  hi_hists[h_HM_SR_4] = h_HM_SR_4_hi;
  hi_hists[h_HH_SR_4] = h_HH_SR_4_hi;

  // 2 ST 
  TH2D* h_2STC_MR = new TH2D("h_2STC_MR","2 Track Event Counts",1,0,1,3,0,3);
  h_2STC_MR->GetYaxis()->SetTitleOffset(3.0);
  h_2STC_MR->GetXaxis()->SetBinLabel(1,"2 STC Events");
  h_2STC_MR->GetYaxis()->SetBinLabel(1,"Obs SR 2ST Events");
  h_2STC_MR->GetYaxis()->SetBinLabel(2,"Pred SR 2ST Events");
  h_2STC_MR->GetYaxis()->SetBinLabel(3,"2 STC CR Events");
  TH2D* h_2STC_VR = (TH2D*)h_2STC_MR->Clone("h_2STC_VR");
  TH2D* h_2STC_SR = (TH2D*)h_2STC_MR->Clone("h_2STC_SR");

  // Nlep = 1 hists

  // NjHt
  TH2D* h_LLlep_VR = (TH2D*)h_eventwise_countsVR.Clone("h_LLlep_VR");
  TH2D* h_LHlep_VR = (TH2D*)h_eventwise_countsVR.Clone("h_LHlep_VR");
  TH2D* h_HLlep_VR = (TH2D*)h_eventwise_countsVR.Clone("h_HLlep_VR");
  TH2D* h_HHlep_VR = (TH2D*)h_eventwise_countsVR.Clone("h_HHlep_VR");

  TH2D* h_LLlep_SR = (TH2D*)h_eventwise_counts.Clone("h_LLlep_SR");
  TH2D* h_LHlep_SR = (TH2D*)h_eventwise_counts.Clone("h_LHlep_SR");
  TH2D* h_HLlep_SR = (TH2D*)h_eventwise_counts.Clone("h_HLlep_SR");
  TH2D* h_HHlep_SR = (TH2D*)h_eventwise_counts.Clone("h_HHlep_SR");

  // Separate fshorts
  TH2D* h_LLlep_VR_23 = (TH2D*)h_eventwise_countsVR.Clone("h_LLlep_VR_23");
  TH2D* h_LHlep_VR_23 = (TH2D*)h_eventwise_countsVR.Clone("h_LHlep_VR_23");
  TH2D* h_HLlep_VR_4 = (TH2D*)h_eventwise_countsVR.Clone("h_HLlep_VR_4");
  TH2D* h_HHlep_VR_4 = (TH2D*)h_eventwise_countsVR.Clone("h_HHlep_VR_4");

  TH2D* h_LLlep_SR_23 = (TH2D*)h_eventwise_counts.Clone("h_LLlep_SR_23");
  TH2D* h_LHlep_SR_23 = (TH2D*)h_eventwise_counts.Clone("h_LHlep_SR_23");
  TH2D* h_HLlep_SR_4 = (TH2D*)h_eventwise_counts.Clone("h_HLlep_SR_4");
  TH2D* h_HHlep_SR_4 = (TH2D*)h_eventwise_counts.Clone("h_HHlep_SR_4");

  // Save Fshorts used with errors
  TH1D* h_FS = new TH1D("h_FS","f_{short}",5,0,5);
  TH1D* h_FS_23 = (TH1D*) h_FS->Clone("h_FS_23");
  TH1D* h_FS_4 = (TH1D*) h_FS->Clone("h_FS_4");
  TH1D* h_FS_hi = (TH1D*) h_FS->Clone("h_FS_hi");
  TH1D* h_FS_23_hi = (TH1D*) h_FS->Clone("h_FS_23_hi");
  TH1D* h_FS_4_hi = (TH1D*) h_FS->Clone("h_FS_4_hi");
  TH1D* h_FS_lo = (TH1D*) h_FS->Clone("h_FS_lo");
  TH1D* h_FS_23_lo = (TH1D*) h_FS->Clone("h_FS_23_lo");
  TH1D* h_FS_4_lo = (TH1D*) h_FS->Clone("h_FS_4_lo");

  TH1D* h_FS_up = (TH1D*) h_FS->Clone("h_FS_up");
  TH1D* h_FS_23_up = (TH1D*) h_FS->Clone("h_FS_23_up");
  TH1D* h_FS_4_up = (TH1D*) h_FS->Clone("h_FS_4_up");
  TH1D* h_FS_hi_up = (TH1D*) h_FS->Clone("h_FS_hi_up");
  TH1D* h_FS_23_hi_up = (TH1D*) h_FS->Clone("h_FS_23_hi_up");
  TH1D* h_FS_4_hi_up = (TH1D*) h_FS->Clone("h_FS_4_hi_up");
  TH1D* h_FS_lo_up = (TH1D*) h_FS->Clone("h_FS_lo_up");
  TH1D* h_FS_23_lo_up = (TH1D*) h_FS->Clone("h_FS_23_lo_up");
  TH1D* h_FS_4_lo_up = (TH1D*) h_FS->Clone("h_FS_4_lo_up");

  TH1D* h_FS_dn = (TH1D*) h_FS->Clone("h_FS_dn");
  TH1D* h_FS_23_dn = (TH1D*) h_FS->Clone("h_FS_23_dn");
  TH1D* h_FS_4_dn = (TH1D*) h_FS->Clone("h_FS_4_dn");
  TH1D* h_FS_hi_dn = (TH1D*) h_FS->Clone("h_FS_hi_dn");
  TH1D* h_FS_23_hi_dn = (TH1D*) h_FS->Clone("h_FS_23_hi_dn");
  TH1D* h_FS_4_hi_dn = (TH1D*) h_FS->Clone("h_FS_4_hi_dn");
  TH1D* h_FS_lo_dn = (TH1D*) h_FS->Clone("h_FS_lo_dn");
  TH1D* h_FS_23_lo_dn = (TH1D*) h_FS->Clone("h_FS_23_lo_dn");
  TH1D* h_FS_4_lo_dn = (TH1D*) h_FS->Clone("h_FS_4_lo_dn");

  mt2tree t;

  t.Init(ch);

  // Load the configuration and output to screen
  config_ = GetMT2Config(config_tag);
  cout << "[ShortTrackLooper::loop] using configuration tag: " << config_tag << endl;
  cout << "                  JSON: " << config_.json << endl;
  cout << "                  lumi: " << config_.lumi << " fb-1" << endl;

  vector<const Int_t*> trigs_SR_;
  vector<const Int_t*> trigs_prescaled_;
  vector<const Int_t*> trigs_singleLep_;

  // load the triggers we want
  if(config_.triggers.size() > 0){
    if( config_.triggers.find("SR")       == config_.triggers.end() || 
	config_.triggers.find("prescaledHT")       == config_.triggers.end() || 
	config_.triggers.find("SingleMu") == config_.triggers.end() || 
	config_.triggers.find("SingleEl") == config_.triggers.end() ){
      cout << "[ShortTrackLooper::loop] ERROR: invalid trigger map in configuration '" << config_tag << "'!" << endl;
      cout << "                         Make sure you have trigger vectors for 'SR', 'SingleMu', 'SingleEl', 'prescaledHT" << endl;
      return -1;
    }
    cout << "                  Triggers:" << endl;
    for(map<string,vector<string> >::iterator it=config_.triggers.begin(); it!=config_.triggers.end(); it++){
      cout << "                    " << it->first << ":" << endl;
      for(uint i=0; i<it->second.size(); i++){
	if(i==0)
	  cout << "                      " << it->second.at(i);
	else
	  cout << " || " << it->second.at(i);
      }
      cout << endl;
    }
    fillTriggerVector(t, trigs_SR_,        config_.triggers["SR"]);
    fillTriggerVector(t, trigs_prescaled_, config_.triggers["prescaledHT"]);
    fillTriggerVector(t, trigs_singleLep_, config_.triggers["SingleMu"]);
    fillTriggerVector(t, trigs_singleLep_, config_.triggers["SingleEl"]);
  }else if (config_tag.find("mc") != std::string::npos) {
    std::string data_config_tag = "";
    if (config_tag == "mc_94x_Fall17") data_config_tag = "data_2017_31Mar2018";
    else if (config_tag == "mc_94x_Summer16") data_config_tag = "data_2016_94x";

    cout << "Detected we are running on MC corresponding to data config: " << data_config_tag << endl;
					 
    data_config_ = GetMT2Config(data_config_tag);
    if( data_config_.triggers.find("SR")       == data_config_.triggers.end() ||
        data_config_.triggers.find("prescaledHT")       == data_config_.triggers.end() ||
        data_config_.triggers.find("SingleMu") == data_config_.triggers.end() ||
        data_config_.triggers.find("SingleEl") == data_config_.triggers.end() ){
      cout << "[ShortTrackLooper::loop] ERROR: invalid trigger map in configuration '" << data_config_tag << "'!" << endl;
      cout << "                         Make sure you have trigger vectors for 'SR', 'SingleMu', 'SingleEl', 'prescaledHT" << endl;
      return -1;
    }
    cout << "                  Triggers:" << endl;
    for(map<string,vector<string> >::iterator it=data_config_.triggers.begin(); it!=data_config_.triggers.end(); it++){
      cout << "                    " << it->first << ":" << endl;
      for(uint i=0; i<it->second.size(); i++){
        if(i==0)
          cout << "                      " << it->second.at(i);
        else
          cout << " || " << it->second.at(i);
      }
      cout << endl;
    }
    fillTriggerVector(t, trigs_SR_,        data_config_.triggers["SR"]);
    fillTriggerVector(t, trigs_prescaled_, data_config_.triggers["prescaledHT"]);
    fillTriggerVector(t, trigs_singleLep_, data_config_.triggers["SingleEl"]);
    fillTriggerVector(t, trigs_singleLep_, data_config_.triggers["SingleMu"]);

  }    else {
    cout << "                  No triggers provided and no \"mc\" in config name. Weight calculation may be inaccurate." << endl;
    if(config_tag.find("data") != string::npos){
      cout << "[ShortTrackLooper::loop] WARNING! it looks like you are using data and didn't supply any triggers in the configuration." <<
	"\n                  Every event is going to fail the trigger selection!" << endl;
    }
  }

  TH1D* h_nTrueInt_weights_ = 0;
  if (doNTrueIntReweight && config_.pu_weights_file != "") {
    TFile* f_weights = new TFile(("../babymaker/data/" + config_.pu_weights_file).c_str());
    TH1D* h_nTrueInt_weights_temp = (TH1D*) f_weights->Get("pileupWeight");
    //	outfile_->cd();
    h_nTrueInt_weights_ = (TH1D*) h_nTrueInt_weights_temp->Clone("h_pileupWeight");
    h_nTrueInt_weights_->SetDirectory(0);
    f_weights->Close();
    delete f_weights;
  }

  if (applyJSON && config_.json != "") {
    cout << "[ShortTrackLooper::loop] Loading json file: " << config_.json << endl;
    set_goodrun_file(("../babymaker/jsons/"+config_.json).c_str());
  }

  const int year = config_.year;  

  if (isMC) adjL = false;

  cout << "Getting transfer factors" << endl;

  TString FshortName = "", FshortNameMC = "";
  if (year == 2016) {
    if (isMC) {
      FshortName = FshortName16MC;
    } else {
      FshortName = FshortName16Data;
    }
    FshortNameMC = FshortName16MC;
  }
  else if (year == 2017) {
    if (isMC) {
      FshortName = FshortName17MC;
    } else {
      FshortName = FshortName17Data;
    }
    FshortNameMC = FshortName17MC;
  }
  else if (year == 2018) {
    if (isMC) {
      FshortName = FshortName17MC; // For now, use 2017 MC in 2018
    } else {
      FshortName = FshortName18Data;
    }
    FshortNameMC = FshortName17MC; // For now, use 2017 MC in 2018
  }
  else {
    cout << "Check configuration" << endl;
    return -1;
  }

  TFile* FshortFile = TFile::Open(FshortName,"READ");
  TFile* FshortFileMC = TFile::Open(FshortNameMC,"READ");
  TH1D* h_fs = ((TH2D*) FshortFile->Get("h_fsMR_Baseline"))->ProjectionX("h_fs",1,1);
  TH1D* h_fs_Nj23 = ((TH2D*) FshortFile->Get("h_fsMR_23_Baseline"))->ProjectionX("h_fs_23",1,1);
  TH1D* h_fs_Nj4 = ((TH2D*) FshortFile->Get("h_fsMR_4_Baseline"))->ProjectionX("h_fs_4",1,1);
  TH1D* h_fs_up = ((TH2D*) FshortFile->Get("h_fsMR_Baseline_up"))->ProjectionX("h_fs_up",1,1);
  TH1D* h_fs_Nj23_up = ((TH2D*) FshortFile->Get("h_fsMR_23_Baseline_up"))->ProjectionX("h_fs_23_up",1,1);
  TH1D* h_fs_Nj4_up = ((TH2D*) FshortFile->Get("h_fsMR_4_Baseline_up"))->ProjectionX("h_fs_4_up",1,1);
  TH1D* h_fs_dn = ((TH2D*) FshortFile->Get("h_fsMR_Baseline_dn"))->ProjectionX("h_fs_dn",1,1);
  TH1D* h_fs_Nj23_dn = ((TH2D*) FshortFile->Get("h_fsMR_23_Baseline_dn"))->ProjectionX("h_fs_23_dn",1,1);
  TH1D* h_fs_Nj4_dn = ((TH2D*) FshortFile->Get("h_fsMR_4_Baseline_dn"))->ProjectionX("h_fs_4_dn",1,1);
  TH1D* h_fs_MC = ((TH2D*) FshortFileMC->Get("h_fsMR_Baseline"))->ProjectionX("h_fs_MC",1,1);
  TH1D* h_fs_MC_up = ((TH2D*) FshortFileMC->Get("h_fsMR_Baseline_up"))->ProjectionX("h_fs_MC_up",1,1);
  TH1D* h_fs_MC_dn = ((TH2D*) FshortFileMC->Get("h_fsMR_Baseline_dn"))->ProjectionX("h_fs_MC_dn",1,1);
  TH1D* h_fs_Nj23_MC = ((TH2D*) FshortFileMC->Get("h_fsMR_23_Baseline"))->ProjectionX("h_fs_23_MC",1,1);
  TH1D* h_fs_Nj23_MC_up = ((TH2D*) FshortFileMC->Get("h_fsMR_23_Baseline_up"))->ProjectionX("h_fs_23_MC_up",1,1);
  TH1D* h_fs_Nj23_MC_dn = ((TH2D*) FshortFileMC->Get("h_fsMR_23_Baseline_dn"))->ProjectionX("h_fs_23_MC_dn",1,1);
  TH1D* h_fs_Nj4_MC = ((TH2D*) FshortFileMC->Get("h_fsMR_4_Baseline"))->ProjectionX("h_fs_4_MC",1,1);
  TH1D* h_fs_Nj4_MC_up = ((TH2D*) FshortFileMC->Get("h_fsMR_4_Baseline_up"))->ProjectionX("h_fs_4_MC_up",1,1);
  TH1D* h_fs_Nj4_MC_dn = ((TH2D*) FshortFileMC->Get("h_fsMR_4_Baseline_dn"))->ProjectionX("h_fs_4_MC_dn",1,1);

  const float mc_L_corr = adjL ? h_fs_MC->GetBinContent(Lidx) / h_fs_MC->GetBinContent(Midx) : 1.0; // fs_L / fs_M
  const float mc_L_corr_err_up = adjL ? sqrt( pow((h_fs_MC_up->GetBinError(Lidx) / h_fs_MC->GetBinContent(Lidx)),2) + pow((h_fs_MC_up->GetBinError(Midx) / h_fs_MC->GetBinContent(Midx)),2)) * mc_L_corr : 0;
  const float mc_L_corr_err_dn = adjL ? sqrt( pow((h_fs_MC_dn->GetBinError(Lidx) / h_fs_MC->GetBinContent(Lidx)),2) + pow((h_fs_MC_dn->GetBinError(Midx) / h_fs_MC->GetBinContent(Midx)),2)) * mc_L_corr : 0;
  const float mc_L_corr_23 = adjL ? h_fs_Nj23_MC->GetBinContent(Lidx) / h_fs_Nj23_MC->GetBinContent(Midx) : 1.0; // fs_L / fs_M
  const float mc_L_corr_err_23_up = adjL ? sqrt( pow((h_fs_Nj23_MC_up->GetBinError(4) / h_fs_Nj23_MC->GetBinContent(4)),2) + pow((h_fs_Nj23_MC_up->GetBinError(3) / h_fs_Nj23_MC->GetBinContent(3)),2)) * mc_L_corr_23 : 0;
  const float mc_L_corr_err_23_dn = adjL ? sqrt( pow((h_fs_Nj23_MC_dn->GetBinError(4) / h_fs_Nj23_MC->GetBinContent(4)),2) + pow((h_fs_Nj23_MC_dn->GetBinError(3) / h_fs_Nj23_MC->GetBinContent(3)),2)) * mc_L_corr_23 : 0;
  const float mc_L_corr_4 = adjL ? h_fs_Nj4_MC->GetBinContent(Lidx) / h_fs_Nj4_MC->GetBinContent(Midx) : 1.0; // fs_L / fs_M
  const float mc_L_corr_err_4_up = adjL ? sqrt( pow((h_fs_Nj4_MC_up->GetBinError(Lidx) / h_fs_Nj4_MC->GetBinContent(Lidx)),2) + pow((h_fs_Nj4_MC_up->GetBinError(Midx) / h_fs_Nj4_MC->GetBinContent(Midx)),2)) * mc_L_corr_4 : 0;
  const float mc_L_corr_err_4_dn = adjL ? sqrt( pow((h_fs_Nj4_MC_dn->GetBinError(Lidx) / h_fs_Nj4_MC->GetBinContent(Lidx)),2) + pow((h_fs_Nj4_MC_dn->GetBinError(Midx) / h_fs_Nj4_MC->GetBinContent(Midx)),2)) * mc_L_corr_4 : 0;
  const double fs_P = h_fs->GetBinContent(Pidx);
  const double fs_P_err_up = h_fs_up->GetBinError(Pidx);
  const double fs_P_err_dn = h_fs_dn->GetBinError(Pidx);
  const double fs_P3 = h_fs->GetBinContent(P3idx);
  const double fs_P3_err_up = h_fs_up->GetBinError(P3idx);
  const double fs_P3_err_dn = h_fs_dn->GetBinError(P3idx);
  const double fs_P4 = h_fs->GetBinContent(P4idx);
  const double fs_P4_err_up = h_fs_up->GetBinError(P4idx);
  const double fs_P4_err_dn = h_fs_dn->GetBinError(P4idx);
  const double fs_M = h_fs->GetBinContent(Midx);
  const double fs_M_err_up = h_fs_up->GetBinError(Midx);
  const double fs_M_err_dn = h_fs_dn->GetBinError(Midx);
  const double fs_L = adjL ? h_fs->GetBinContent(Midx) * mc_L_corr : h_fs->GetBinContent(Lidx);
  const double fs_L_err_up = adjL ? sqrt( pow((fs_M_err_up/fs_M),2) + pow((mc_L_corr_err_up / mc_L_corr),2) ) * fs_L : h_fs->GetBinError(Lidx);
  const double fs_L_err_dn = adjL ? sqrt( pow((fs_M_err_dn/fs_M),2) + pow((mc_L_corr_err_dn / mc_L_corr),2) ) * fs_L : h_fs->GetBinError(Lidx);
  const double fs_Nj23_P = h_fs_Nj23->GetBinContent(Pidx);
  const double fs_Nj23_P_err_up = h_fs_Nj23_up->GetBinError(Pidx);
  const double fs_Nj23_P_err_dn = h_fs_Nj23_dn->GetBinError(Pidx);
  const double fs_Nj23_P3 = h_fs_Nj23->GetBinContent(P3idx);
  const double fs_Nj23_P3_err_up = h_fs_Nj23_up->GetBinError(P3idx);
  const double fs_Nj23_P3_err_dn = h_fs_Nj23_dn->GetBinError(P3idx);
  const double fs_Nj23_P4 = h_fs_Nj23->GetBinContent(P4idx);
  const double fs_Nj23_P4_err_up = h_fs_Nj23_up->GetBinError(P4idx);
  const double fs_Nj23_P4_err_dn = h_fs_Nj23_dn->GetBinError(P4idx);
  const double fs_Nj23_M = h_fs_Nj23->GetBinContent(Midx);
  const double fs_Nj23_M_err_up = h_fs_Nj23_up->GetBinError(Midx);
  const double fs_Nj23_M_err_dn = h_fs_Nj23_dn->GetBinError(Midx);
  const double fs_Nj23_L = adjL ? h_fs_Nj23->GetBinContent(Midx) * mc_L_corr_23 : h_fs_Nj23->GetBinContent(Lidx);
  const double fs_Nj23_L_err_up = adjL ? sqrt( pow((fs_Nj23_M_err_up/fs_Nj23_M),2) + pow((mc_L_corr_err_23_up / mc_L_corr_23),2) ) * fs_Nj23_L : h_fs_Nj23->GetBinError(Lidx);
  const double fs_Nj23_L_err_dn = adjL ? sqrt( pow((fs_Nj23_M_err_dn/fs_Nj23_M),2) + pow((mc_L_corr_err_23_dn / mc_L_corr_23),2) ) * fs_Nj23_L : h_fs_Nj23->GetBinError(Lidx);
  const double fs_Nj4_P = h_fs_Nj4->GetBinContent(Pidx);
  const double fs_Nj4_P_err_up = h_fs_Nj4_up->GetBinError(Pidx);
  const double fs_Nj4_P_err_dn = h_fs_Nj4_dn->GetBinError(Pidx);
  const double fs_Nj4_P3 = h_fs_Nj4->GetBinContent(P3idx);
  const double fs_Nj4_P3_err_up = h_fs_Nj4_up->GetBinError(P3idx);
  const double fs_Nj4_P3_err_dn = h_fs_Nj4_dn->GetBinError(P3idx);
  const double fs_Nj4_P4 = h_fs_Nj4->GetBinContent(P4idx);
  const double fs_Nj4_P4_err_up = h_fs_Nj4_up->GetBinError(P4idx);
  const double fs_Nj4_P4_err_dn = h_fs_Nj4_dn->GetBinError(P4idx);
  const double fs_Nj4_M = h_fs_Nj4->GetBinContent(Midx);
  const double fs_Nj4_M_err_up = h_fs_Nj4_up->GetBinError(Midx);
  const double fs_Nj4_M_err_dn = h_fs_Nj4_dn->GetBinError(Midx);
  const double fs_Nj4_L = adjL ? h_fs_Nj4->GetBinContent(Midx) * mc_L_corr_23 : h_fs_Nj4->GetBinContent(Lidx);
  const double fs_Nj4_L_err_up = adjL ? sqrt( pow((fs_Nj4_M_err_up/fs_Nj4_M),2) + pow((mc_L_corr_err_23_up / mc_L_corr_23),2) ) * fs_Nj4_L : h_fs_Nj4->GetBinError(Lidx);
  const double fs_Nj4_L_err_dn = adjL ? sqrt( pow((fs_Nj4_M_err_dn/fs_Nj4_M),2) + pow((mc_L_corr_err_23_dn / mc_L_corr_23),2) ) * fs_Nj4_L : h_fs_Nj4->GetBinError(Lidx);

  TH1D* h_fs_hi = ((TH2D*) FshortFile->Get("h_fsMR_Baseline_hipt"))->ProjectionX("h_fs_hi",1,1);
  TH1D* h_fs_hi_up = ((TH2D*) FshortFile->Get("h_fsMR_Baseline_hipt_up"))->ProjectionX("h_fs_hi_up",1,1);
  TH1D* h_fs_hi_dn = ((TH2D*) FshortFile->Get("h_fsMR_Baseline_hipt_dn"))->ProjectionX("h_fs_hi_dn",1,1);
  TH1D* h_fs_Nj23_hi = ((TH2D*) FshortFile->Get("h_fsMR_23_Baseline_hipt"))->ProjectionX("h_fs_23_hi",1,1);
  TH1D* h_fs_Nj23_hi_up = ((TH2D*) FshortFile->Get("h_fsMR_23_Baseline_hipt_up"))->ProjectionX("h_fs_23_hi_up",1,1);
  TH1D* h_fs_Nj23_hi_dn = ((TH2D*) FshortFile->Get("h_fsMR_23_Baseline_hipt_dn"))->ProjectionX("h_fs_23_hi_dn",1,1);
  TH1D* h_fs_Nj4_hi = ((TH2D*) FshortFile->Get("h_fsMR_4_Baseline_hipt"))->ProjectionX("h_fs_4_hi",1,1);
  TH1D* h_fs_Nj4_hi_up = ((TH2D*) FshortFile->Get("h_fsMR_4_Baseline_hipt_up"))->ProjectionX("h_fs_4_hi_up",1,1);
  TH1D* h_fs_Nj4_hi_dn = ((TH2D*) FshortFile->Get("h_fsMR_4_Baseline_hipt_dn"))->ProjectionX("h_fs_4_hi_dn",1,1);

  const double fs_P_hi = h_fs_hi->GetBinContent(Pidx);
  const double fs_P_err_hi_up = h_fs_hi_up->GetBinError(Pidx);
  const double fs_P_err_hi_dn = h_fs_hi_dn->GetBinError(Pidx);
  const double fs_P3_hi = h_fs_hi->GetBinContent(P3idx);
  const double fs_P3_err_hi_up = h_fs_hi_up->GetBinError(P3idx);
  const double fs_P3_err_hi_dn = h_fs_hi_dn->GetBinError(P3idx);
  const double fs_P4_hi = h_fs_hi->GetBinContent(P4idx);
  const double fs_P4_err_hi_up = h_fs_hi_up->GetBinError(P4idx);
  const double fs_P4_err_hi_dn = h_fs_hi_dn->GetBinError(P4idx);
  const double fs_M_hi = h_fs_hi->GetBinContent(Midx);
  const double fs_M_err_hi_up = h_fs_hi_up->GetBinError(Midx);
  const double fs_M_err_hi_dn = h_fs_hi_dn->GetBinError(Midx);
  const double fs_Nj23_P_hi = h_fs_Nj23_hi->GetBinContent(Pidx);
  const double fs_Nj23_P_err_hi_up = h_fs_Nj23_hi_up->GetBinError(Pidx);
  const double fs_Nj23_P_err_hi_dn = h_fs_Nj23_hi_dn->GetBinError(Pidx);
  const double fs_Nj23_P3_hi = h_fs_Nj23_hi->GetBinContent(P3idx);
  const double fs_Nj23_P3_err_hi_up = h_fs_Nj23_hi_up->GetBinError(P3idx);
  const double fs_Nj23_P3_err_hi_dn = h_fs_Nj23_hi_dn->GetBinError(P3idx);
  const double fs_Nj23_P4_hi = h_fs_Nj23_hi->GetBinContent(P4idx);
  const double fs_Nj23_P4_err_hi_up = h_fs_Nj23_hi_up->GetBinError(P4idx);
  const double fs_Nj23_P4_err_hi_dn = h_fs_Nj23_hi_dn->GetBinError(P4idx);
  const double fs_Nj23_M_hi = h_fs_Nj23_hi->GetBinContent(Midx);
  const double fs_Nj23_M_err_hi_up = h_fs_Nj23_hi_up->GetBinError(Midx);
  const double fs_Nj23_M_err_hi_dn = h_fs_Nj23_hi_dn->GetBinError(Midx);
  const double fs_Nj4_P_hi = h_fs_Nj4_hi->GetBinContent(Pidx);
  const double fs_Nj4_P_err_hi_up = h_fs_Nj4_hi_up->GetBinError(Pidx);
  const double fs_Nj4_P_err_hi_dn = h_fs_Nj4_hi_dn->GetBinError(Pidx);
  const double fs_Nj4_P3_hi = h_fs_Nj4_hi->GetBinContent(P3idx);
  const double fs_Nj4_P3_err_hi_up = h_fs_Nj4_hi_up->GetBinError(P3idx);
  const double fs_Nj4_P3_err_hi_dn = h_fs_Nj4_hi_dn->GetBinError(P3idx);
  const double fs_Nj4_P4_hi = h_fs_Nj4_hi->GetBinContent(P4idx);
  const double fs_Nj4_P4_err_hi_up = h_fs_Nj4_hi_up->GetBinError(P4idx);
  const double fs_Nj4_P4_err_hi_dn = h_fs_Nj4_hi_dn->GetBinError(P4idx);
  const double fs_Nj4_M_hi = h_fs_Nj4_hi->GetBinContent(Midx);
  const double fs_Nj4_M_err_hi_up = h_fs_Nj4_hi_up->GetBinError(Midx);
  const double fs_Nj4_M_err_hi_dn = h_fs_Nj4_hi_dn->GetBinError(Midx);

  TH1D* h_fs_lo = ((TH2D*) FshortFile->Get("h_fsMR_Baseline_lowpt"))->ProjectionX("h_fs_lo",1,1);
  TH1D* h_fs_lo_up = ((TH2D*) FshortFile->Get("h_fsMR_Baseline_lowpt_up"))->ProjectionX("h_fs_lo_up",1,1);
  TH1D* h_fs_lo_dn = ((TH2D*) FshortFile->Get("h_fsMR_Baseline_lowpt_dn"))->ProjectionX("h_fs_lo_dn",1,1);
  TH1D* h_fs_Nj23_lo = ((TH2D*) FshortFile->Get("h_fsMR_23_Baseline_lowpt"))->ProjectionX("h_fs_23_lo",1,1);
  TH1D* h_fs_Nj23_lo_up = ((TH2D*) FshortFile->Get("h_fsMR_23_Baseline_lowpt_up"))->ProjectionX("h_fs_23_lo_up",1,1);
  TH1D* h_fs_Nj23_lo_dn = ((TH2D*) FshortFile->Get("h_fsMR_23_Baseline_lowpt_dn"))->ProjectionX("h_fs_23_lo_dn",1,1);
  TH1D* h_fs_Nj4_lo = ((TH2D*) FshortFile->Get("h_fsMR_4_Baseline_lowpt"))->ProjectionX("h_fs_4_lo",1,1);
  TH1D* h_fs_Nj4_lo_up = ((TH2D*) FshortFile->Get("h_fsMR_4_Baseline_lowpt_up"))->ProjectionX("h_fs_4_lo_up",1,1);
  TH1D* h_fs_Nj4_lo_dn = ((TH2D*) FshortFile->Get("h_fsMR_4_Baseline_lowpt_dn"))->ProjectionX("h_fs_4_lo_dn",1,1);

  const double fs_P_lo = h_fs_lo->GetBinContent(Pidx);
  const double fs_P_err_lo_up = h_fs_lo_up->GetBinError(Pidx);
  const double fs_P_err_lo_dn = h_fs_lo_dn->GetBinError(Pidx);
  const double fs_P3_lo = h_fs_lo->GetBinContent(P3idx);
  const double fs_P3_err_lo_up = h_fs_lo_up->GetBinError(P3idx);
  const double fs_P3_err_lo_dn = h_fs_lo_dn->GetBinError(P3idx);
  const double fs_P4_lo = h_fs_lo->GetBinContent(P4idx);
  const double fs_P4_err_lo_up = h_fs_lo_up->GetBinError(P4idx);
  const double fs_P4_err_lo_dn = h_fs_lo_dn->GetBinError(P4idx);
  const double fs_M_lo = h_fs_lo->GetBinContent(Midx);
  const double fs_M_err_lo_up = h_fs_lo_up->GetBinError(Midx);
  const double fs_M_err_lo_dn = h_fs_lo_dn->GetBinError(Midx);
  const double fs_Nj23_P_lo = h_fs_Nj23_lo->GetBinContent(Pidx);
  const double fs_Nj23_P_err_lo_up = h_fs_Nj23_lo_up->GetBinError(Pidx);
  const double fs_Nj23_P_err_lo_dn = h_fs_Nj23_lo_dn->GetBinError(Pidx);
  const double fs_Nj23_P3_lo = h_fs_Nj23_lo->GetBinContent(P3idx);
  const double fs_Nj23_P3_err_lo_up = h_fs_Nj23_lo_up->GetBinError(P3idx);
  const double fs_Nj23_P3_err_lo_dn = h_fs_Nj23_lo_dn->GetBinError(P3idx);
  const double fs_Nj23_P4_lo = h_fs_Nj23_lo->GetBinContent(P4idx);
  const double fs_Nj23_P4_err_lo_up = h_fs_Nj23_lo_up->GetBinError(P4idx);
  const double fs_Nj23_P4_err_lo_dn = h_fs_Nj23_lo_dn->GetBinError(P4idx);
  const double fs_Nj23_M_lo = h_fs_Nj23_lo->GetBinContent(Midx);
  const double fs_Nj23_M_err_lo_up = h_fs_Nj23_lo_up->GetBinError(Midx);
  const double fs_Nj23_M_err_lo_dn = h_fs_Nj23_lo_dn->GetBinError(Midx);
  const double fs_Nj4_P_lo = h_fs_Nj4_lo->GetBinContent(Pidx);
  const double fs_Nj4_P_err_lo_up = h_fs_Nj4_lo_up->GetBinError(Pidx);
  const double fs_Nj4_P_err_lo_dn = h_fs_Nj4_lo_dn->GetBinError(Pidx);
  const double fs_Nj4_P3_lo = h_fs_Nj4_lo->GetBinContent(P3idx);
  const double fs_Nj4_P3_err_lo_up = h_fs_Nj4_lo_up->GetBinError(P3idx);
  const double fs_Nj4_P3_err_lo_dn = h_fs_Nj4_lo_dn->GetBinError(P3idx);
  const double fs_Nj4_P4_lo = h_fs_Nj4_lo->GetBinContent(P4idx);
  const double fs_Nj4_P4_err_lo_up = h_fs_Nj4_lo_up->GetBinError(P4idx);
  const double fs_Nj4_P4_err_lo_dn = h_fs_Nj4_lo_dn->GetBinError(P4idx);
  const double fs_Nj4_M_lo = h_fs_Nj4_lo->GetBinContent(Midx);
  const double fs_Nj4_M_err_lo_up = h_fs_Nj4_lo_up->GetBinError(Midx);
  const double fs_Nj4_M_err_lo_dn = h_fs_Nj4_lo_dn->GetBinError(Midx);


  cout << "Loaded transfer factors" << endl;

  cout << "corr             : " << mc_L_corr << endl;
  cout << "corr err up      : " << mc_L_corr_err_up << endl;
  cout << "corr err dn      : " << mc_L_corr_err_dn << endl;
  cout << "corr 23          : " << mc_L_corr_23 << endl;
  cout << "corr 23 err up   : " << mc_L_corr_err_23_up << endl;
  cout << "corr 23 err dn   : " << mc_L_corr_err_23_dn << endl;
  cout << "corr4            : " << mc_L_corr_4 << endl;
  cout << "corr 4 err up    : " << mc_L_corr_err_4_up << endl;
  cout << "corr 4 err dn    : " << mc_L_corr_err_4_dn << endl;

  cout << "fs_P             : " << fs_P << endl;
  cout << "fs_P err up      : " << fs_P_err_up << endl;
  cout << "fs_P err dn      : " << fs_P_err_dn << endl;
  cout << "fs_P3            : " << fs_P3 << endl;
  cout << "fs_P3 err up     : " << fs_P3_err_up << endl;
  cout << "fs_P3 err dn     : " << fs_P3_err_dn << endl;
  cout << "fs_P4            : " << fs_P4 << endl;
  cout << "fs_P4 err up     : " << fs_P4_err_up << endl;
  cout << "fs_P4 err dn     : " << fs_P4_err_dn << endl;
  cout << "fs_M             : " << fs_M << endl;
  cout << "fs_M err up      : " << fs_M_err_up << endl;
  cout << "fs_M err dn      : " << fs_M_err_dn << endl;
  cout << "fs_L (raw)       : " << h_fs->GetBinContent(4) << endl;
  cout << "fs_L             : " << fs_L << endl;
  cout << "fs_L err up      : " << fs_L_err_up << endl;
  cout << "fs_L err dn      : " << fs_L_err_dn << endl;
  cout << "fs_Nj23_P        : " << fs_Nj23_P << endl;
  cout << "fs_Nj23_P err up : " << fs_Nj23_P_err_up << endl;
  cout << "fs_Nj23_P err dn : " << fs_Nj23_P_err_dn << endl;
  cout << "fs_Nj23_P3       : " << fs_Nj23_P3 << endl;
  cout << "fs_Nj23_P3 err up: " << fs_Nj23_P3_err_up << endl;
  cout << "fs_Nj23_P3 err dn: " << fs_Nj23_P3_err_up << endl;
  cout << "fs_Nj23_P4       : " << fs_Nj23_P4 << endl;
  cout << "fs_Nj23_P4 err up: " << fs_Nj23_P4_err_up << endl;
  cout << "fs_Nj23_P4 err dn: " << fs_Nj23_P4_err_dn << endl;
  cout << "fs_Nj23_M        : " << fs_Nj23_M << endl;
  cout << "fs_Nj23_M err up : " << fs_Nj23_M_err_up << endl;
  cout << "fs_Nj23_M err dn : " << fs_Nj23_M_err_dn << endl;
  cout << "fs_Nj23_L (raw)  : " << h_fs_Nj23->GetBinContent(4) << endl;
  cout << "fs_Nj23_L        : " << fs_Nj23_L << endl;
  cout << "fs_Nj23_L err up : " << fs_Nj23_L_err_up << endl;
  cout << "fs_Nj23_L err dn : " << fs_Nj23_L_err_dn << endl;
  cout << "fs_Nj4_P         : " << fs_Nj4_P << endl;
  cout << "fs_Nj4_P err up  : " << fs_Nj4_P_err_up << endl;
  cout << "fs_Nj4_P err dn  : " << fs_Nj4_P_err_dn << endl;
  cout << "fs_Nj4_P3        : " << fs_Nj4_P3 << endl;
  cout << "fs_Nj4_P3 err up : " << fs_Nj4_P3_err_up << endl;
  cout << "fs_Nj4_P3 err dn : " << fs_Nj4_P3_err_dn << endl;
  cout << "fs_Nj4_P4        : " << fs_Nj4_P4 << endl;
  cout << "fs_Nj4_P4 err up : " << fs_Nj4_P4_err_up << endl;
  cout << "fs_Nj4_P4 err dn : " << fs_Nj4_P4_err_dn << endl;
  cout << "fs_Nj4_M         : " << fs_Nj4_M << endl;
  cout << "fs_Nj4_M err up  : " << fs_Nj4_M_err_up << endl;
  cout << "fs_Nj4_M err dn  : " << fs_Nj4_M_err_dn << endl;
  cout << "fs_Nj4_L (raw)   : " << h_fs_Nj4->GetBinContent(4) << endl;
  cout << "fs_Nj4_L         : " << fs_Nj4_L << endl;
  cout << "fs_Nj4_L err up  : " << fs_Nj4_L_err_up << endl;
  cout << "fs_Nj4_L err dn  : " << fs_Nj4_L_err_dn << endl;

  cout << "/nHi Pt/n" << endl;

  cout << "fs_P             : " << fs_P_hi << endl;
  cout << "fs_P err up      : " << fs_P_err_hi_up << endl;
  cout << "fs_P err dn      : " << fs_P_err_hi_dn << endl;
  cout << "fs_P3            : " << fs_P3_hi << endl;
  cout << "fs_P3 err up     : " << fs_P3_err_hi_up << endl;
  cout << "fs_P3 err dn     : " << fs_P3_err_hi_dn << endl;
  cout << "fs_P4            : " << fs_P4_hi << endl;
  cout << "fs_P4 err up     : " << fs_P4_err_hi_up << endl;
  cout << "fs_P4 err dn     : " << fs_P4_err_hi_dn << endl;
  cout << "fs_M             : " << fs_M_hi << endl;
  cout << "fs_M err up      : " << fs_M_err_hi_up << endl;
  cout << "fs_M err dn      : " << fs_M_err_hi_dn << endl;
  cout << "fs_Nj23_P        : " << fs_Nj23_P_hi << endl;
  cout << "fs_Nj23_P err up : " << fs_Nj23_P_err_hi_up << endl;
  cout << "fs_Nj23_P err dn : " << fs_Nj23_P_err_hi_dn << endl;
  cout << "fs_Nj23_P3       : " << fs_Nj23_P3_hi << endl;
  cout << "fs_Nj23_P3 err up: " << fs_Nj23_P3_err_hi_up << endl;
  cout << "fs_Nj23_P3 err dn: " << fs_Nj23_P3_err_hi_dn << endl;
  cout << "fs_Nj23_P4       : " << fs_Nj23_P4_hi << endl;
  cout << "fs_Nj23_P4 err up: " << fs_Nj23_P4_err_hi_up << endl;
  cout << "fs_Nj23_P4 err dn: " << fs_Nj23_P4_err_hi_dn << endl;
  cout << "fs_Nj23_M        : " << fs_Nj23_M_hi << endl;
  cout << "fs_Nj23_M err up : " << fs_Nj23_M_err_hi_up << endl;
  cout << "fs_Nj23_M err dn : " << fs_Nj23_M_err_hi_dn << endl;
  cout << "fs_Nj4_P         : " << fs_Nj4_P_hi << endl;
  cout << "fs_Nj4_P err up  : " << fs_Nj4_P_err_hi_up << endl;
  cout << "fs_Nj4_P err dn  : " << fs_Nj4_P_err_hi_dn << endl;
  cout << "fs_Nj4_P3        : " << fs_Nj4_P3_hi << endl;
  cout << "fs_Nj4_P3 err up : " << fs_Nj4_P3_err_hi_up << endl;
  cout << "fs_Nj4_P3 err dn : " << fs_Nj4_P3_err_hi_dn << endl;
  cout << "fs_Nj4_P4        : " << fs_Nj4_P4_hi << endl;
  cout << "fs_Nj4_P4 err up : " << fs_Nj4_P4_err_hi_up << endl;
  cout << "fs_Nj4_P4 err dn : " << fs_Nj4_P4_err_hi_dn << endl;
  cout << "fs_Nj4_M         : " << fs_Nj4_M_hi << endl;
  cout << "fs_Nj4_M err dn: " << fs_Nj4_M_err_hi_up << endl;
  cout << "fs_Nj4_M err up: " << fs_Nj4_M_err_hi_dn << endl;

  cout << "/nLow Pt/n" << endl;

  cout << "fs_P             : " << fs_P_lo << endl;
  cout << "fs_P err up      : " << fs_P_err_lo_up << endl;
  cout << "fs_P err dn      : " << fs_P_err_lo_dn << endl;
  cout << "fs_P3            : " << fs_P3_lo << endl;
  cout << "fs_P3 err up     : " << fs_P3_err_lo_up << endl;
  cout << "fs_P3 err dn     : " << fs_P3_err_lo_dn << endl;
  cout << "fs_P4            : " << fs_P4_lo << endl;
  cout << "fs_P4 err up     : " << fs_P4_err_lo_up << endl;
  cout << "fs_P4 err dn     : " << fs_P4_err_lo_dn << endl;
  cout << "fs_M             : " << fs_M_lo << endl;
  cout << "fs_M err up      : " << fs_M_err_lo_up << endl;
  cout << "fs_M err dn      : " << fs_M_err_lo_dn << endl;
  cout << "fs_Nj23_P        : " << fs_Nj23_P_lo << endl;
  cout << "fs_Nj23_P err up : " << fs_Nj23_P_err_lo_up << endl;
  cout << "fs_Nj23_P err dn : " << fs_Nj23_P_err_lo_dn << endl;
  cout << "fs_Nj23_P3       : " << fs_Nj23_P3_lo << endl;
  cout << "fs_Nj23_P3 err up: " << fs_Nj23_P3_err_lo_up << endl;
  cout << "fs_Nj23_P3 err dn: " << fs_Nj23_P3_err_lo_dn << endl;
  cout << "fs_Nj23_P4       : " << fs_Nj23_P4_lo << endl;
  cout << "fs_Nj23_P4 err up: " << fs_Nj23_P4_err_lo_up << endl;
  cout << "fs_Nj23_P4 err dn: " << fs_Nj23_P4_err_lo_dn << endl;
  cout << "fs_Nj23_M        : " << fs_Nj23_M_lo << endl;
  cout << "fs_Nj23_M err up : " << fs_Nj23_M_err_lo_up << endl;
  cout << "fs_Nj23_M err dn : " << fs_Nj23_M_err_lo_dn << endl;
  cout << "fs_Nj4_P         : " << fs_Nj4_P_lo << endl;
  cout << "fs_Nj4_P err up  : " << fs_Nj4_P_err_lo_up << endl;
  cout << "fs_Nj4_P err dn  : " << fs_Nj4_P_err_lo_dn << endl;
  cout << "fs_Nj4_P3        : " << fs_Nj4_P3_lo << endl;
  cout << "fs_Nj4_P3 err up : " << fs_Nj4_P3_err_lo_up << endl;
  cout << "fs_Nj4_P3 err dn : " << fs_Nj4_P3_err_lo_dn << endl;
  cout << "fs_Nj4_P4        : " << fs_Nj4_P4_lo << endl;
  cout << "fs_Nj4_P4 err up : " << fs_Nj4_P4_err_lo_up << endl;
  cout << "fs_Nj4_P4 err dn : " << fs_Nj4_P4_err_lo_dn << endl;
  cout << "fs_Nj4_M         : " << fs_Nj4_M_lo << endl;
  cout << "fs_Nj4_M err dn: " << fs_Nj4_M_err_lo_up << endl;
  cout << "fs_Nj4_M err up: " << fs_Nj4_M_err_lo_dn << endl;

  // Now get systematics
  TH1D* h_fs_syst = (TH1D*) FshortFile->Get("h_fsMR_Baseline_syst");
  TH1D* h_fs_syst_Nj23 = (TH1D*) FshortFile->Get("h_fsMR_23_Baseline_syst");
  TH1D* h_fs_syst_Nj4 = (TH1D*) FshortFile->Get("h_fsMR_4_Baseline_syst");
  TH1D* h_fs_syst_hi = (TH1D*) FshortFile->Get("h_fsMR_Baseline_hipt_syst");
  TH1D* h_fs_syst_Nj23_hi = (TH1D*) FshortFile->Get("h_fsMR_23_Baseline_hipt_syst");
  TH1D* h_fs_syst_Nj4_hi = (TH1D*) FshortFile->Get("h_fsMR_4_Baseline_hipt_syst");
  TH1D* h_fs_syst_low = (TH1D*) FshortFile->Get("h_fsMR_Baseline_lowpt_syst");
  TH1D* h_fs_syst_Nj23_low = (TH1D*) FshortFile->Get("h_fsMR_23_Baseline_lowpt_syst");
  TH1D* h_fs_syst_Nj4_low = (TH1D*) FshortFile->Get("h_fsMR_4_Baseline_lowpt_syst");

  const float fs_P_syst  = h_fs_syst->GetBinContent(Pidx);
  const float fs_P3_syst = h_fs_syst->GetBinContent(P3idx);
  const float fs_P4_syst = h_fs_syst->GetBinContent(P4idx);
  const float fs_M_syst  = h_fs_syst->GetBinContent(Midx);
  const float fs_L_syst  = fs_L;  // Always take 100% error for L tracks

  const float fs_P_syst_Nj23  = h_fs_syst_Nj23->GetBinContent(Pidx);
  const float fs_P3_syst_Nj23 = h_fs_syst_Nj23->GetBinContent(P3idx);
  const float fs_P4_syst_Nj23 = h_fs_syst_Nj23->GetBinContent(P4idx);
  const float fs_M_syst_Nj23  = h_fs_syst_Nj23->GetBinContent(Midx);
  const float fs_L_syst_Nj23  = fs_Nj23_L;  // Always take 100% error for L tracks

  const float fs_P_syst_Nj4  = h_fs_syst_Nj4->GetBinContent(Pidx);
  const float fs_P3_syst_Nj4 = h_fs_syst_Nj4->GetBinContent(P3idx);
  const float fs_P4_syst_Nj4 = h_fs_syst_Nj4->GetBinContent(P4idx);
  const float fs_M_syst_Nj4  = h_fs_syst_Nj4->GetBinContent(Midx);
  const float fs_L_syst_Nj4  = fs_Nj4_L;  // Always take 100% error for L tracks

  const float fs_P_syst_lo  = h_fs_syst_low->GetBinContent(Pidx);
  const float fs_P3_syst_lo = h_fs_syst_low->GetBinContent(P3idx);
  const float fs_P4_syst_lo = h_fs_syst_low->GetBinContent(P4idx);
  const float fs_M_syst_lo  = h_fs_syst_low->GetBinContent(Midx);
  const float fs_L_syst_lo  = 0; // no split L 

  const float fs_P_syst_Nj23_lo  = h_fs_syst_Nj23_low->GetBinContent(Pidx);
  const float fs_P3_syst_Nj23_lo = h_fs_syst_Nj23_low->GetBinContent(P3idx);
  const float fs_P4_syst_Nj23_lo = h_fs_syst_Nj23_low->GetBinContent(P4idx);
  const float fs_M_syst_Nj23_lo  = h_fs_syst_Nj23_low->GetBinContent(Midx);
  const float fs_L_syst_Nj23_lo  = 0; // no split L

  const float fs_P_syst_Nj4_lo  = h_fs_syst_Nj4_low->GetBinContent(Pidx);
  const float fs_P3_syst_Nj4_lo = h_fs_syst_Nj4_low->GetBinContent(P3idx);
  const float fs_P4_syst_Nj4_lo = h_fs_syst_Nj4_low->GetBinContent(P4idx);
  const float fs_M_syst_Nj4_lo  = h_fs_syst_Nj4_low->GetBinContent(Midx);
  const float fs_L_syst_Nj4_lo  = 0; // no split L

  const float fs_P_syst_hi  = h_fs_syst_hi->GetBinContent(Pidx);
  const float fs_P3_syst_hi = h_fs_syst_hi->GetBinContent(P3idx);
  const float fs_P4_syst_hi = h_fs_syst_hi->GetBinContent(P4idx);
  const float fs_M_syst_hi  = h_fs_syst_hi->GetBinContent(Midx);
  const float fs_L_syst_hi  = 0; // no split L

  const float fs_P_syst_Nj23_hi  = h_fs_syst_Nj23_hi->GetBinContent(Pidx);
  const float fs_P3_syst_Nj23_hi = h_fs_syst_Nj23_hi->GetBinContent(P3idx);
  const float fs_P4_syst_Nj23_hi = h_fs_syst_Nj23_hi->GetBinContent(P4idx);
  const float fs_M_syst_Nj23_hi  = h_fs_syst_Nj23_hi->GetBinContent(Midx);
  const float fs_L_syst_Nj23_hi  = 0;  // No split L

  const float fs_P_syst_Nj4_hi  = h_fs_syst_Nj4_hi->GetBinContent(Pidx);
  const float fs_P3_syst_Nj4_hi = h_fs_syst_Nj4_hi->GetBinContent(P3idx);
  const float fs_P4_syst_Nj4_hi = h_fs_syst_Nj4_hi->GetBinContent(P4idx);
  const float fs_M_syst_Nj4_hi  = h_fs_syst_Nj4_hi->GetBinContent(Midx);
  const float fs_L_syst_Nj4_hi  = 0;  // No split L

  FshortFile->Close();
  FshortFileMC->Close();

  const unsigned int nEventsTree = ch->GetEntries();
  int nDuplicates = 0; int numberOfAllSTs = 0; int numberOfAllSTCs = 0;
  int GoodEventCounter = 0;
  for( unsigned int event = 0; event < nEventsTree; ++event) {    
    //    if (event % 1000 == 0) cout << 100.0 * event / nEventsTree  << "%" << endl;

    t.GetEntry(event); 

    //---------------------
    // skip duplicates -- needed when running on mutiple streams in data
    //---------------------
    if( t.isData ) {
      DorkyEventIdentifier id(t.run, t.evt, t.lumi);
      if (is_duplicate(id) ){
	++nDuplicates;
	continue;
      }
    }

    //---------------------
    // basic event selection and cleaning
    //---------------------
    
    if( applyJSON && t.isData && !goodrun(t.run, t.lumi) ) continue;

    // Not a duplicate, and a good event. Counts towards our unblinded luminosity
    GoodEventCounter++;
    // 10/fb in 2016 = 10/35.9 = about 1 event in 3.6, call it 1 event in 4
    // 10/fb in 2017 = 10/42 = about 1 event in 4 
    // 10/fb in 2018 = 10/58.83 = about 1 event in 6
    bool partial_unblind = false;
    if (year == 2016 && GoodEventCounter % 4 == 0) partial_unblind = true; // about 9/fb
    else if (year == 2017 && GoodEventCounter % 4 == 0) partial_unblind = true; // about 10.5/fb
    else if (year == 2018 && GoodEventCounter % 6 == 0) partial_unblind = true; // about 9.8/fb
    /*
    if (year == 2016 && GoodEventCounter % 7 == 0) partial_unblind = true;
    else if (year == 2017 && GoodEventCounter % 8 == 0) partial_unblind = true;
    */

    if (unlikely(t.nVert == 0)) {
      continue;
    }

    if (config_.filters["eeBadScFilter"] && !t.Flag_eeBadScFilter) continue; 
    if (config_.filters["globalSuperTightHalo2016Filter"] && !t.Flag_globalSuperTightHalo2016Filter) continue; 
    if (config_.filters["globalTightHalo2016Filter"] && !t.Flag_globalTightHalo2016Filter) continue; 
    if (config_.filters["goodVertices"] && !t.Flag_goodVertices) continue;
    if (config_.filters["HBHENoiseFilter"] && !t.Flag_HBHENoiseFilter) continue;
    if (config_.filters["HBHENoiseIsoFilter"] && !t.Flag_HBHENoiseIsoFilter) continue;
    if (config_.filters["EcalDeadCellTriggerPrimitiveFilter"] && !t.Flag_EcalDeadCellTriggerPrimitiveFilter) continue;
    if (config_.filters["ecalBadCalibFilter"] && !t.Flag_ecalBadCalibFilter) continue;
    if (config_.filters["badMuonFilter"] && !t.Flag_badMuonFilter) continue;
    if (config_.filters["badChargedCandidateFilter"] && !t.Flag_badChargedCandidateFilter) continue; 
    if (config_.filters["badMuonFilterV2"] && !t.Flag_badMuonFilterV2) continue;
    if (config_.filters["badChargedHadronFilterV2"] && !t.Flag_badChargedHadronFilterV2) continue; 
    
    // random events with ht or met=="inf" or "nan" that don't get caught by the filters...
    if(isinf(t.met_pt) || isnan(t.met_pt) || isinf(t.ht) || isnan(t.ht)){
      cout << "WARNING: bad event with infinite MET/HT! " << t.run << ":" << t.lumi << ":" << t.evt
	   << ", met=" << t.met_pt << ", ht=" << t.ht << endl;
      continue;
    }

    // catch events with unphysical jet pt
    if(t.jet_pt[0] > 13000.){
      cout << endl << "WARNING: bad event with unphysical jet pt! " << t.run << ":" << t.lumi << ":" << t.evt
	   << ", met=" << t.met_pt << ", ht=" << t.ht << ", jet_pt=" << t.jet_pt[0] << endl;
      continue;
    }

    // apply HEM veto, and simulate effects in MC
    if(doHEMveto && config_.year == 2018){
      bool hasHEMjet = false;
      if((t.isData && t.run >= HEM_startRun) || (!t.isData && t.evt % HEM_fracDen < HEM_fracNum)){ 
	for(int i=0; i<t.njet; i++){
	  if(t.jet_pt[i] < HEM_ptCut)
	    break;
	  if(t.jet_eta[i] > HEM_region[0] && t.jet_eta[i] < HEM_region[1] && 
	     t.jet_phi[i] > HEM_region[2] && t.jet_phi[i] < HEM_region[3])
	    hasHEMjet = true;
	}
      }
      if(hasHEMjet){
	// cout << endl << "SKIPPED HEM EVT: " << t.run << ":" << t.lumi << ":" << t.evt << endl;
	continue;
      }
    }

    if (t.nJet30 < 2) {
      continue;
    }
    if (t.ht < 250) {
    //if (t.ht < 250 || t.ht > 450) {
    //if (t.ht < 450) {
      continue;
    }
    if (unlikely(t.nJet30FailId != 0)) {
      continue;
    }
    if (t.mt2 < 60) {
      continue;
    }
    //if (t.met_pt < 30) {
    //if (t.met_pt < 100) {
    if (t.met_pt < 30 || (t.ht < 1200 && t.met_pt < 250)) {
	continue;
    }

    if (t.diffMetMht / t.met_pt > 0.5) {
      continue;
    }

    if (unlikely(t.met_miniaodPt / t.met_caloPt >= 5.0)) {
      continue;
    }
    if (unlikely(t.nJet200MuFrac50DphiMet > 0)) {
      continue;
    }

    const bool lepveto = t.nMuons10 + t.nElectrons10 + t.nPFLep5LowMT + t.nPFHad10LowMT > 0;

    /*
    if (lepveto) {
      continue;
    }

    if (t.deltaPhiMin < 0.3) {
      continue;
    }
    */

    // Triggers
    /*    const bool passPrescaleTrigger = (year == 2016) ? 
      t.HLT_PFHT125_Prescale || t.HLT_PFHT350_Prescale || t.HLT_PFHT475_Prescale : 
      t.HLT_PFHT180_Prescale || t.HLT_PFHT370_Prescale || t.HLT_PFHT430_Prescale || t.HLT_PFHT510_Prescale || t.HLT_PFHT590_Prescale || t.HLT_PFHT680_Prescale || t.HLT_PFHT780_Prescale || t.HLT_PFHT890_Prescale;
    const bool passUnPrescaleTrigger = (year == 2016) ? 
      t.HLT_PFHT900 || t.HLT_PFJet450 :
      t.HLT_PFHT1050 || t.HLT_PFJet500;
    const bool passMetTrigger = t.HLT_PFHT300_PFMET110 || t.HLT_PFMET120_PFMHT120 || t.HLT_PFMETNoMu120_PFMHTNoMu120;
    
    if (! (passPrescaleTrigger || passUnPrescaleTrigger || passMetTrigger) ) {
      continue;
    }
    */
    //    const bool passPrescaleTrigger = passTrigger(t, trigs_prescaled_);
    const bool passSRTrigger = passTrigger(t, trigs_SR_);

    //    if (! (passPrescaleTrigger || passSRTrigger)) continue;
    if (!passSRTrigger) continue;

    const float lumi = config_.lumi; 
    float weight = t.isData ? 1.0 : (t.evt_scale1fb == 1 ? 1.8863e-06 : t.evt_scale1fb) * lumi; // manually correct high HT WJets

    if (isSignal) {
      int bin = h_xsec->GetXaxis()->FindBin(1800);      
      weight = h_xsec->GetBinContent(bin) * 1000 * lumi / nEventsTree;
    }


    if (!t.isData) {
      //      weight *= t.weight_btagsf;


      if (applyISRWeights && (tag == "ttsl" || tag == "ttdl")) {
	weight *= t.weight_isr / getAverageISRWeight(outtag,config_tag);
      }

      if(applyL1PrefireWeights && (config_.year==2016 || config_.year==2017) && config_.cmssw_ver!=80) {
	weight *= t.weight_L1prefire;
      }

      if (doNTrueIntReweight) {
	if(h_nTrueInt_weights_==0){
	  cout << "[ShortTrackLooper::loop] WARNING! You requested to do nTrueInt reweighting but didn't supply a PU weights file. Turning off." << endl;
	}else{
	  int nTrueInt_input = t.nTrueInt;
	  float puWeight = h_nTrueInt_weights_->GetBinContent(h_nTrueInt_weights_->FindBin(nTrueInt_input));
	  weight *= puWeight;
	}
      }
    }

    if (weight > 1.0) continue;

    /*
    // simulate turn-on curves and prescales in MC
    if (!t.isData) {
      if ( !passSRTrigger ) {
	if (year == 2016) {
	  if (t.HLT_PFHT475_Prescale) {
	    weight /= 115.2;
	  }
	  else if (t.HLT_PFHT350_Prescale) {
	    weight /= 460.6;
	  }
	  else { //125
	    weight /= 9200.0; 
	  }
	}
	else { // 2017 and 2018
	  if (t.HLT_PFHT890_Prescale) {
	    weight /= 17.1;
	  }
	  else if (t.HLT_PFHT780_Prescale) {
	    weight /= 29.5;
	  }	
	  else if (t.HLT_PFHT680_Prescale) {
	    weight /= 47.4;
	  }	
	  else if (t.HLT_PFHT590_Prescale) {
	    weight /= 67.4;
	  }
	  else if (t.HLT_PFHT510_Prescale) {
	    weight /= 145.0;
	  }
	  else if (t.HLT_PFHT430_Prescale) {
	    weight /= 307.0;
	  }
	  else if (t.HLT_PFHT370_Prescale) {
	    weight /= 707.0;
	  }
	  else if (t.HLT_PFHT250_Prescale) {
	    weight /= 824.0;
	  }
	  else if (t.HLT_PFHT180_Prescale) {
	    weight /= 1316.0;
	  }
	}
      }
      else { // not prescaled, but may not be in the 100% efficiency region
	if (year == 2016 && (t.met_pt < 250 && t.ht < 1000)) {
	  if (t.met_pt < 100) {
	    // assume 0.1 for everything below 100 GeV
	    weight *= 0.1;
	  }
	  else if (t.met_pt < 200) {
	    weight *= (((0.8/100) * (t.met_pt - 100)) + 0.1);
	  } else {
	    weight *= (((0.1/50) * (t.met_pt - 200)) + 0.9);
	  }
	}
	else if (year == 2017 && (t.met_pt < 250 && t.ht < 1200)) {
	  // use same values for 2017 for now
	  if (t.met_pt < 75) {
	    // assume 0.1 for everything below 100 GeV
	    weight *= 0.1;
	  }
	  else if (t.met_pt < 100) {
	    // assume PFHT800_PFMET75_PFMHT75 goes down to 75; let's just take that as the cutoff
	    weight *= ( (0.1/25) * (t.met_pt - 75) ); 
	  }
	  else if (t.met_pt < 200) {
	    weight *= (((0.8/100) * (t.met_pt - 100)) + 0.1);
	  } else {
	    weight *= (((0.1/50) * (t.met_pt - 200)) + 0.9);
	  }	  
	}
      }
    }
    */
    
    TH2D* hist;    
    TH2D* hist_Nj;
    
    if (!lepveto) {
      // L*
      if (t.nJet30 < 4) {
	// LH
	if (t.ht >= 1200) {
	  // MR
	  if (t.mt2 < 100) {
	    hist = h_LH_MR;
	    hist_Nj = h_LH_MR_23;
	  }
	  // VR
	  else if (t.mt2 < 200) {
	    hist = h_LH_VR;
	    hist_Nj = h_LH_VR_23;
	  }
	  // SR 
	  else { //if ( !(blind && t.isData) ) {
	    hist = h_LH_SR;
	    hist_Nj = h_LH_SR_23;
	  }
	}
	// LM
	else if (t.ht >= 450) {
	  // MR
	  if (t.mt2 < 100) {
	    hist = h_LM_MR;
	    hist_Nj = h_LM_MR_23;
	  }
	  // VR
	  else if (t.mt2 < 200) {
	    hist = h_LM_VR;
	    hist_Nj = h_LM_VR_23;
	  }
	  // SR 
	  else { //if (! (blind && t.isData) ){
	    hist = h_LM_SR;
	    hist_Nj = h_LM_SR_23;
	  }
	}
	// LL
	else {
	  // MR
	  if (t.mt2 < 100) {
	    hist = h_LL_MR;
	    hist_Nj = h_LL_MR_23;
	  }
	  // VR
	  else if (t.mt2 < 200) {
	    hist = h_LL_VR;
	    hist_Nj = h_LL_VR_23;
	  }
	  // SR 
	  else { //if (! (blind && t.isData) ){
	    hist = h_LL_SR;
	    hist_Nj = h_LL_SR_23;
	  }
	}
      } 
      // H*
      else {
	// HH
	if (t.ht >= 1200) {
	  // MR
	  if (t.mt2 < 100) {
	    hist = h_HH_MR;
	    hist_Nj = h_HH_MR_4;
	  }
	  // VR
	  else if (t.mt2 < 200) {
	    hist = h_HH_VR;
	    hist_Nj = h_HH_VR_4;
	  }
	  // SR 
	  else { //if (! (blind && t.isData) ) {
	    hist = h_HH_SR;
	    hist_Nj = h_HH_SR_4;
	  }
	}
	// HM
	else if (t.ht >= 450) {
	  // MR
	  if (t.mt2 < 100) {
	    hist = h_HM_MR;
	    hist_Nj = h_HM_MR_4;
	  }
	  // VR
	  else if (t.mt2 < 200) {
	    hist = h_HM_VR;
	    hist_Nj = h_HM_VR_4;
	  }
	  // SR 
	  else { //if (! (blind && t.isData) ){
	    hist = h_HM_SR;
	    hist_Nj = h_HM_SR_4;
	  }
	}
	// HL
	else {
	  // MR
	  if (t.mt2 < 100) {
	    hist = h_HL_MR;
	    hist_Nj = h_HL_MR_4;
	  }
	  // VR
	  else if (t.mt2 < 200) {
	    hist = h_HL_VR;
	    hist_Nj = h_HL_VR_4;
	  }
	  // SR 
	  else { //if (! (blind && t.isData) ){
	    hist = h_HL_SR;
	    hist_Nj = h_HL_SR_4;
	  }
	}
      }
    }
    else if (t.nlep == 1) {
      continue;
      // L*
      if (t.nJet30 < 4) {
	// LH
	if (t.ht >= 1200) {
	  // VR
	  if (t.mt2 < 200) {
	    hist = h_LHlep_VR;
	    hist_Nj = h_LHlep_VR_23;
	  }
	  // SR 
	  // don't need to blind in data at nlep = 1
	  else {
	    hist = h_LHlep_SR;
	    hist_Nj = h_LHlep_SR_23;
	  }
	}
	// LL
	else {
	  // VR
	  if (t.mt2 < 200) {
	    hist = h_LLlep_VR;
	    hist_Nj = h_LLlep_VR_23;
	  }
	  // SR 
	  // don't need to blind in data at nlep = 1
	  else {
	    hist = h_LLlep_SR;
	    hist_Nj = h_LLlep_SR_23;
	  }
	}
      } 
      // H*
      else {
	// HH
	if (t.ht >= 1200) {
	  // VR
	  if (t.mt2 < 200) {
	    hist = h_HHlep_VR;
	    hist_Nj = h_HHlep_VR_4;
	  }
	  // SR 
	  // don't need to blind in data at nlep = 1
	  else {
	    hist = h_HHlep_SR;
	    hist_Nj = h_HHlep_SR_4;
	  }
	}
	// HL
	else {
	  // VR
	  if (t.mt2 < 200) {
	    hist = h_HLlep_VR;
	    hist_Nj = h_HLlep_VR_4;
	  }
	  // SR
	  // don't need to blind in data at nlep = 1
	  else {
	    hist = h_HLlep_SR;
	    hist_Nj = h_HLlep_SR_4;
	  }
	}
      }
    }        
    else continue;

    // Analysis code

    int nSTCp = t.nshorttrackcandidates_P;
    // TODO: add these guys to babies
    int nSTCp3 = 0;
    int nSTCp4 = 0;
    int nSTCm = t.nshorttrackcandidates_M;
    int nSTCl_acc = 0; // L STCs, accepted by mtpt
    int nSTCl_rej = 0; // L STCs, rejected by mtpt
    int nSTp = t.nshorttracks_P;
    int nSTp3 = 0;
    int nSTp4 = 0;
    int nSTm = t.nshorttracks_M;
    int nSTl_acc = 0; // L STs, accepted by mtpt
    int nSTl_rej = 0; // L STs, rejected by mtpt
    // hi pt
    int nSTCp_hi = 0;
    int nSTCp3_hi = 0;
    int nSTCp4_hi = 0;
    int nSTCm_hi = 0;
    int nSTp_hi = 0;
    int nSTp3_hi = 0;
    int nSTp4_hi = 0;
    int nSTm_hi = 0;
    // lo pt
    int nSTCp_lo = 0;
    int nSTCp3_lo = 0;
    int nSTCp4_lo = 0;
    int nSTCm_lo = 0;
    int nSTp_lo = 0;
    int nSTp3_lo = 0;
    int nSTp4_lo = 0;
    int nSTm_lo = 0;


    // Need to do a little loop for L ST(C)s even if not recalculating, to see who passed and didn't pass mtpt veto
    if (!recalculate) {
      for (int i_trk = 0; i_trk < t.ntracks; i_trk++) {
	// L ST
	if (t.track_islong[i_trk]) {
	  MT( t.track_pt[i_trk], t.track_phi[i_trk], t.met_pt, t.met_phi ) < 100 && t.track_pt[i_trk] < 150 ? nSTl_rej++ : nSTl_acc++;
	} 
	else if (t.track_islongcandidate[i_trk]) {
	  MT( t.track_pt[i_trk], t.track_phi[i_trk], t.met_pt, t.met_phi ) < 100 && t.track_pt[i_trk] < 150 ? nSTCl_rej++ : nSTCl_acc++;
	}
      }
    }
    else {
      // Manual calculation for testing new ST/STC definitions not coded in babymaker
      nSTCp = 0;
      nSTCp3 = 0;
      nSTCp4 = 0;
      nSTCm = 0;
      nSTCl_rej = 0;
      nSTCl_acc = 0;
      nSTp = 0;
      nSTp3 = 0;
      nSTp4 = 0;
      nSTm = 0;      
      nSTl_rej = 0;
      nSTl_acc = 0;
      
      for (int i_trk = 0; i_trk < t.ntracks; i_trk++) {   

	bool isChargino = isSignal && t.track_matchedCharginoIdx[i_trk] >= 0;
	if (isSignal && !isChargino) {
	  for (int i_chargino = 0; i_chargino < t.nCharginos; i_chargino++) {
	    if (DeltaR(t.track_eta[i_trk],t.chargino_eta[i_chargino],t.track_phi[i_trk],t.chargino_phi[i_chargino]) < 0.1) {
	      isChargino = true;
	      break;
	    }
	  }
	}

	int lenIndex = -1;
	bool isST = false, isSTC = false;
	
	// Apply basic selections
	/*
	const bool CaloSel = !(t.track_DeadECAL[i_trk] || t.track_DeadHCAL[i_trk]) && InEtaPhiVetoRegion(t.track_eta[i_trk],t.track_phi[i_trk]) == 0 
	  && (year == 2016 || !(t.track_eta[i_trk] < -0.7 && t.track_eta[i_trk] > -0.9 && t.track_phi[i_trk] > 1.5 && t.track_phi[i_trk] < 1.7 ) )
	  && (year == 2016 || !(t.track_eta[i_trk] < 0.30 && t.track_eta[i_trk] > 0.10 && t.track_phi[i_trk] > 2.2 && t.track_phi[i_trk] < 2.5 ) )
	  && (year == 2016 || !(t.track_eta[i_trk] < 0.50 && t.track_eta[i_trk] > 0.40 && t.track_phi[i_trk] > -0.7 && t.track_phi[i_trk] < -0.5  ) )
	  && (year == 2016 || !(t.track_eta[i_trk] < 0.70 && t.track_eta[i_trk] > 0.60 && t.track_phi[i_trk] > -1.1 && t.track_phi[i_trk] < -0.9  ) )
	  && (year != 2018 || !(t.track_eta[i_trk] < 2.45 && t.track_eta[i_trk] > 2.00 && t.track_phi[i_trk] > 1.7 && t.track_phi[i_trk] < 1.9));
	*/
	const bool CaloSel = !(t.track_DeadECAL[i_trk] || t.track_DeadHCAL[i_trk]) && InEtaPhiVetoRegion(t.track_eta[i_trk],t.track_phi[i_trk],year) == 0;
	const float pt = t.track_pt[i_trk];
	const bool ptSel = pt >= 15.0;
	const bool etaSel = fabs(t.track_eta[i_trk]) <= 2.4;
	const bool BaseSel = CaloSel && ptSel && etaSel;
	//const bool BaseSel = ptSel && etaSel;
	
	if (!BaseSel) {
	  continue;
	}
	
	// Length and category
	// Length and categorization
	const bool lostOuterHitsSel = t.track_nLostOuterHits[i_trk] >= 2;
	const int nLayers = t.track_nLayersWithMeasurement[i_trk];
	const bool isP = nLayers == t.track_nPixelLayersWithMeasurement[i_trk] && lostOuterHitsSel && nLayers >= (increment17 ? (year == 2016 ? 3 : 4) : 3);
	const bool isLong = nLayers >= 7;
	const bool isM = nLayers != t.track_nPixelLayersWithMeasurement[i_trk] && !isLong && lostOuterHitsSel;
	const bool isL = isLong && lostOuterHitsSel;
	const bool isShort = isP || isM || isL;
	if (!isShort) {
	  continue;
	}
	
	lenIndex = isP ? 1 : (isM ? 2 : 3);
	
	if (applyRecoVeto == 1) {	
	  const float nearestPF_DR = t.track_nearestPF_DR[i_trk];
	  const int nearestPF_id = abs(t.track_nearestPF_id[i_trk]);
	  const bool nearestPFSel = !(nearestPF_DR < 0.1 && (nearestPF_id == 11 || nearestPF_id == 13));
	  float mindr = 100;
	  for (int iL = 0; iL < t.nlep; iL++) {
	    float dr = DeltaR(t.lep_eta[iL],t.track_eta[i_trk],t.lep_phi[iL],t.track_phi[i_trk]);
	    if (mindr < dr) mindr = dr;
	  }
	  const float minrecodr = mindr;
	  const bool recoVeto = minrecodr < 0.1;
	  const bool PassesRecoVeto = !recoVeto && nearestPFSel && !t.track_isLepOverlap[i_trk];
	  if (!PassesRecoVeto) {
	    continue;
	  }	  
	}
	else if (applyRecoVeto == 2) {
	  const bool PassesRecoVeto = t.track_recoveto[i_trk] == 0;
	  if (!PassesRecoVeto) {
	    continue;
	  }
	} else {
	  // do nothing
	}
	
	// Isolation
	const float niso = t.track_neuIso0p05[i_trk];
	const bool NeuIso0p05Sel = niso < 10;
	const bool NeuIso0p05SelSTC = niso < 10*isoSTC;
	const float nreliso = t.track_neuRelIso0p05[i_trk];
	const bool NeuRelIso0p05Sel =  nreliso < 0.1;
	const bool NeuRelIso0p05SelSTC = nreliso < 0.1*isoSTC;
	const float iso = t.track_iso[i_trk];
	const bool isoSel = iso < 10;
	const bool isoSelSTC = iso < 10 * isoSTC;
	const float reliso = t.track_reliso[i_trk];
	const bool relisoSel = reliso < 0.2;
	const bool relisoSelSTC = reliso < 0.2 * isoSTC;
	const bool PassesFullIsoSel = NeuIso0p05Sel && NeuRelIso0p05Sel && isoSel && relisoSel;
	const bool PassesFullIsoSelSTC = NeuIso0p05SelSTC && NeuRelIso0p05SelSTC && isoSelSTC && relisoSelSTC;
	
	if (!PassesFullIsoSelSTC) {
	  continue;
	}
	
	// Quality
	const bool pixLayersSelLoose = t.track_nPixelLayersWithMeasurement[i_trk] >= 2;
	const bool pixLayersSelTight = t.track_nPixelLayersWithMeasurement[i_trk] >= 3;
	const bool pixLayersSel4 = nLayers > 4 ? pixLayersSelLoose : pixLayersSelTight; // Apply 2 layer selection to tracks with 5 or more layers
	
	const int lostInnerHits = t.track_nLostInnerPixelHits[i_trk];
	const bool lostInnerHitsSel = lostInnerHits == 0;
	const float pterr = t.track_ptErr[i_trk];
	const float pterrOverPt2 = pterr / (pt*pt);
	const bool pterrSelLoose = pterrOverPt2 <= 0.2;
	const bool pterrSelMedium = pterrOverPt2 <= 0.02;
	const bool pterrSelTight = pterrOverPt2 <= 0.005;
	const bool pterrSelLooseSTC = pterrOverPt2 <= 0.2 * qualSTC;
	const bool pterrSelMediumSTC = pterrOverPt2 <= 0.02 * qualSTC;
	const bool pterrSelTightSTC = pterrOverPt2 <= 0.005 * qualSTC;
	bool pterrSel = pterrSelTight;
	bool pterrSelSTC = pterrSelTightSTC;
	if (isP) {pterrSel = pterrSelLoose; pterrSelSTC = pterrSelLooseSTC;}
	else if (nLayers < 7) {pterrSel = pterrSelMedium; pterrSelSTC = pterrSelMediumSTC;}
	const float dxy = fabs(t.track_dxy[i_trk]);
	const bool dxySelLoose = dxy < 0.02;
	const bool dxySelLooseSTC = dxy < 0.02 * qualSTC;
	const bool dxySelTight = dxy < 0.01;
	const bool dxySelTightSTC = dxy < 0.01 * qualSTC;
	const bool dxySel = isP ? dxySelLoose : dxySelTight;
	const bool dxySelSTC = isP ? dxySelLooseSTC : dxySelTightSTC;
	const float dz = fabs(t.track_dz[i_trk]);
	const bool dzSel = dz < 0.05;
	const bool dzSelSTC = dz < 0.05*qualSTC;
	const bool isHighPurity = t.track_isHighPurity[i_trk] == 1;
	const bool QualityTrackBase = lostInnerHitsSel && pterrSel && isHighPurity && dxySel && dzSel;
	const bool QualityTrackSTCBase =  lostInnerHitsSel && pterrSelSTC && isHighPurity && dxySelSTC && dzSelSTC;
	const bool isQualityTrack = pixLayersSel4 && QualityTrackBase;
	const bool isQualityTrackSTC = pixLayersSel4 && QualityTrackSTCBase;
	
	if (!isQualityTrackSTC) {
	  continue;
	}
	
	// Full Short Track
	isST = PassesFullIsoSel && isQualityTrack;
	
	// Candidate (loosened isolation, quality)...already checked for Iso and Quality above
	isSTC = !isST;
	//	isSTC = !isST && (isP || iso > 30 || reliso > 0.4);
	if (! (isST || isSTC)) continue;       

	if ( isST && (blind && t.isData && t.mt2 >= 200 && !partial_unblind)) {
	  continue;
	}

	const float mt = MT( t.track_pt[i_trk], t.track_phi[i_trk], t.met_pt, t.met_phi );	
	const bool vetoMtPt = mt < 100 && t.track_pt[i_trk] < 150;

	if (isSignal && !isChargino && !(lenIndex == 3 && vetoMtPt)) {
	  bool TwoMatches = t.chargino_matchedTrackIdx[0] >= 0 && t.chargino_matchedTrackIdx[1] >= 0;
	  cout << "Unmatched " << (isST ? "ST" : "STC") << ", " << (TwoMatches  ? "2 matched charginos" : "<2 matched charginos");
	  cout << "Track: " << t.track_eta[i_trk] << ", " << t.track_phi[i_trk] << " // "
	       << t.chargino_eta[0] << ", " << t.chargino_phi[0] << ": " << DeltaR(t.track_eta[i_trk], t.chargino_eta[0], t.track_phi[i_trk], t.chargino_phi[0]) << " // "
	       << t.chargino_eta[1] << ", " << t.chargino_phi[1] << ": " << DeltaR(t.track_eta[i_trk], t.chargino_eta[1], t.track_phi[i_trk], t.chargino_phi[1]) << endl;
	  cout << hist_Nj->GetName() << (t.track_pt[i_trk] < 50 ? " low" : " high") << endl;
	  continue;
	}

	if (lenIndex == 3) {
	  if (vetoMtPt) {
	    if (isST) {
	      nSTl_rej++;
	    }
	    else {
	      nSTCl_rej++;
	    }
	  }
	  else {
	    if (isST) {
	      nSTl_acc++;
	      numberOfAllSTs++;
	    }
	    else {
	      nSTCl_acc++;
	      numberOfAllSTCs++;
	    }
	  }
	}	
	else if (isSTC) {
	  numberOfAllSTCs++;
	  if (lenIndex == 1) {
	    nSTCp++; 
	    t.track_pt[i_trk] < 50 ? nSTCp_lo++ : nSTCp_hi++;
	    if (t.track_nLayersWithMeasurement[i_trk] == 3) {
	      nSTCp3++;
	      t.track_pt[i_trk] < 50 ? nSTCp3_lo++ : nSTCp3_hi++;
	      if (hist_Nj == h_LM_VR_23 && t.track_pt[i_trk] > 50) cout << "P3 LH VR hi STC: pt = " << t.track_pt[i_trk] << " pterr = " << t.track_ptErr[i_trk] << endl;
	    }
	    else {
	      nSTCp4++;
	      t.track_pt[i_trk] < 50 ? nSTCp4_lo++ : nSTCp4_hi++;
	    }
	  }
	  else if (lenIndex == 2) {
	    nSTCm++;
	    t.track_pt[i_trk] < 50 ? nSTCm_lo++ : nSTCm_hi++;
	  }
	}
	else if (isST) {
	  numberOfAllSTs++;
	  if (lenIndex == 1) {
	    nSTp++; 
	    t.track_pt[i_trk] < 50 ? nSTp_lo++ : nSTp_hi++;
	    if (t.track_nLayersWithMeasurement[i_trk] == 3) {
	      nSTp3++;
	      t.track_pt[i_trk] < 50 ? nSTp3_lo++ : nSTp3_hi++;
	      if (hist_Nj == h_LM_VR_23 && t.track_pt[i_trk] > 50) cout << "P3 LH VR hi ST: pt = " << t.track_pt[i_trk] << " pterr = " << t.track_ptErr[i_trk] << endl;
	    }
	    else {
	      nSTp4++;
	      t.track_pt[i_trk] < 50 ? nSTp4_lo++ : nSTp4_hi++;
	    }
	  }
	  else if (lenIndex == 2) {
	    nSTm++;
	    t.track_pt[i_trk] < 50 ? nSTm_lo++ : nSTm_hi++;
	  }
	}
      } // Track loop
    } // recalculate
    
//if (nSTCp3 > 0 && nSTCp4 > 0) cout << "Double P event: " << t.run << ":" << t.lumi << ":" << t.evt << endl;

    // Eventwise STC Region fills
    // Chance of at least 1 track of given length being tagged is 1 - (1-p)^n
    /*
    if (nSTCl_rej > 0) {
      // For L tracks, we fill separately based on MtPt veto region, and extrapolate from STCs to STs with the Lfshort
      hist->Fill(2.0,2.0,weight); // Fill CR
      hist_Nj->Fill(2.0,2.0,weight); // Fill CR      
      hist->Fill(2.0,1.0, weight * ( 1 - pow(1-fs_L,nSTCl_rej) ) ); // Fill expected SR count
      hist_Nj->Fill(2.0,1.0, weight * ( 1 - pow(1-(t.nJet30 > 3 ? fs_L_4 : fs_L_23),nSTCl_rej) ) ); // Fill expected SR count with separate Nj fshort
    }
    else 
    */
    if (nSTCl_acc + nSTCm + nSTCp == 2 && EventWise) {
      cout << "2 STC, P3: " << nSTCp3 << " P4: " << nSTCp4 << " M: " << nSTCm << " L: " << nSTCl_acc << " in " << t.run << ":" << t.lumi << ":" << t.evt;
      float pred_weight = weight;
      pred_weight *= pow(fs_L,nSTCl_acc);
      pred_weight *= pow(fs_M,nSTCm);
      if (year == 2016) {
	pred_weight *= pow(fs_P,nSTCp);
      }
      else {
	pred_weight *= pow(fs_P3,nSTCp3);
	pred_weight *= pow(fs_P4,nSTCp4);
      }
      if (t.mt2 < 100) {
	cout << ", MR" << endl;
	h_2STC_MR->Fill(0.0,2.0,weight);
	h_2STC_MR->Fill(0.0,1.0,pred_weight);
      }
      else if (t.mt2 < 200) {
	cout << ", VR" << endl;
	h_2STC_VR->Fill(0.0,2.0,weight);
	h_2STC_VR->Fill(0.0,1.0,pred_weight);
      }
      else {
	cout << ", SR" << endl;
	h_2STC_SR->Fill(0.0,2.0,weight);
	h_2STC_SR->Fill(0.0,1.0,pred_weight);
      }
    }
    else if (nSTCl_acc + nSTCm + nSTCp >= 1) { // we fill here if track count == 1 if filling EventWise, and for any track count if filling TrackWise. Can safely multiply by Nstc.
      if (nSTCl_acc > 0) {
	// For L tracks, we fill separately based on MtPt veto region, and extrapolate from STCs to STs based on M fshort
	hist->Fill(Lidx-0.5,2.0,weight*nSTCl_acc); // Fill CR
	hist_Nj->Fill(Lidx-0.5,2.0,weight*nSTCl_acc); // Fill CR      
	hist->Fill(Lidx-0.5,1.0, weight * ( fs_L*nSTCl_acc ) ); // Fill expected SR count
	hist_Nj->Fill(Lidx-0.5,1.0, weight * ( (t.nJet30 > 3 ? fs_Nj4_L : fs_Nj23_L) * nSTCl_acc ) ); // Fill expected SR count with separate Nj fshort
      }
      if (nSTCm > 0) {
	hist->Fill(Midx-0.5,2.0,weight*nSTCm); // Fill CR
	hist_Nj->Fill(Midx-0.5,2.0,weight*nSTCm); // Fill CR
	hist->Fill(Midx-0.5,1.0, weight * ( fs_M * nSTCm ) ); // Fill expected SR count
	hist_Nj->Fill(Midx-0.5,1.0, weight * ( (t.nJet30 > 3 ? fs_Nj4_M : fs_Nj23_M) * nSTCm) ); // Fill expected SR count with separate Nj fs
      }
      if (nSTCp > 0) {
	hist->Fill(Pidx-0.5,2.0,weight*nSTCp); // Fill CR
	hist_Nj->Fill(Pidx-0.5,2.0,weight*nSTCp); // Fill CR
	hist->Fill(Pidx-0.5,1.0, weight * ( fs_P * nSTCp ) ); // Fill expected SR count
	hist_Nj->Fill(Pidx-0.5,1.0, weight * ( (t.nJet30 > 3 ? fs_Nj4_P : fs_Nj23_P) * nSTCp) ); // Fill expected SR count with separate Nj fs
      }
      if (nSTCp3 > 0) {
	hist->Fill(P3idx-0.5,2.0,weight*nSTCp3); // Fill CR
	hist_Nj->Fill(P3idx-0.5,2.0,weight*nSTCp3); // Fill CR
	hist->Fill(P3idx-0.5,1.0, weight * ( fs_P3 * nSTCp3 ) ); // Fill expected SR count
	hist_Nj->Fill(P3idx-0.5,1.0, weight * ( (t.nJet30 > 3 ? fs_Nj4_P3 : fs_Nj23_P3) * nSTCp3 ) ); // Fill expected SR count with separate Nj fs
      }
      if (nSTCp4 > 0) {
	hist->Fill(P4idx-0.5,2.0,weight*nSTCp4); // Fill CR
	hist_Nj->Fill(P4idx-0.5,2.0,weight*nSTCp4); // Fill CR
	hist->Fill(P4idx-0.5,1.0, weight * ( fs_P4 * nSTCp4 ) ); // Fill expected SR count
	hist_Nj->Fill(P4idx-0.5,1.0, weight * ( (t.nJet30 > 3 ? fs_Nj4_P4 : fs_Nj23_P4) * nSTCp4 ) ); // Fill expected SR count with separate Nj fs
      }
    }
    else if (nSTCl_acc + nSTCm + nSTCp > 2) {
      cout << "More than 2 STCs!" << endl;
    }
    
    // Eventwise ST fills
    // Only full STs when counting "observed" SR
    /*
    if (nSTl_rej > 0) {
      hist->Fill(2.0,0.0,weight);
      hist_Nj->Fill(2.0,0.0,weight);
    }
    else 
    */

    /*
    if (nSTl_acc > 0 && t.mt2 > 200) {
      cout << "Found a L ST in unblinded data: " << t.run << ":" << t.lumi << ":" << t.evt << endl;
    }
    */

    if (nSTl_acc + nSTm + nSTp > 1 && EventWise) {
      cout << "2 ST, P3: " << nSTp3 << " P4: " << nSTp4 << " M: " << nSTm << " L: " << nSTl_acc << " in " << t.run << ":" << t.lumi << ":" << t.evt;
      if (t.mt2 < 100) {
	cout << ", MR" << endl;
	h_2STC_MR->Fill(0.0,0.0,weight);
      }
      else if (t.mt2 < 200) {
	cout << ", VR" << endl;
	h_2STC_VR->Fill(0.0,0.0,weight);
      }
      else {
	cout << ", SR" << endl;
	h_2STC_SR->Fill(0.0,0.0,weight);
      }
    }
    else { // If nST > 1, we only fill here if filling trackwise; can safely multiply by Nst.
      if (nSTl_acc > 0) {
	hist->Fill(Lidx-0.5,0.0,weight*nSTl_acc);
	hist_Nj->Fill(Lidx-0.5,0.0,weight*nSTl_acc);
      }
      if (nSTm > 0) {
	if (t.mt2 > 200) cout << "M ST event: " << t.run << ":" << t.lumi << ":" << t.evt << endl;
	hist->Fill(Midx-0.5,0.0,weight*nSTm);
	hist_Nj->Fill(Midx-0.5,0.0,weight*nSTm);
      }
      if (nSTp > 0) {
	hist->Fill(Pidx-0.5,0.0,weight*nSTp);
	hist_Nj->Fill(Pidx-0.5,0.0,weight*nSTp);
      }
      if (nSTp3 > 0) {
	hist->Fill(P3idx-0.5,0.0,weight*nSTp3);
	hist_Nj->Fill(P3idx-0.5,0.0,weight*nSTp3);
      }
      if (nSTp4 > 0) {
	hist->Fill(P4idx-0.5,0.0,weight*nSTp4);
	hist_Nj->Fill(P4idx-0.5,0.0,weight*nSTp4);
      }
    }

    // Low pt
    TH2D* hist_lo = low_hists[hist];
    TH2D* hist_Nj_lo = low_hists[hist_Nj];
    if (nSTCm_lo > 0) {
      hist_lo->Fill(Midx-0.5,2.0,weight*nSTCm_lo); // Fill CR
      hist_Nj_lo->Fill(Midx-0.5,2.0,weight*nSTCm_lo); // Fill CR
      hist_lo->Fill(Midx-0.5,1.0, weight * ( fs_M_lo * nSTCm_lo ) ); // Fill expected SR count
      hist_Nj_lo->Fill(Midx-0.5,1.0, weight * ( t.nJet30 > 3 ? fs_Nj4_M_lo : fs_Nj23_M_lo) * nSTCm_lo ); // Fill expected SR count with separate Nj fs
    }
    if (nSTCp_lo > 0) {
      hist_lo->Fill(Pidx-0.5,2.0,weight*nSTCp_lo); // Fill CR
      hist_Nj_lo->Fill(Pidx-0.5,2.0,weight*nSTCp_lo); // Fill CR
      hist_lo->Fill(Pidx-0.5,1.0, weight * ( fs_P_lo * nSTCp_lo ) ); // Fill expected SR count
      hist_Nj_lo->Fill(Pidx-0.5,1.0, weight * (t.nJet30 > 3 ? fs_Nj4_P_lo : fs_Nj23_P_lo) * nSTCp_lo ); // Fill expected SR count with separate Nj fs
    }
    if (nSTCp3_lo > 0) {
      hist_lo->Fill(P3idx-0.5,2.0,weight*nSTCp3_lo); // Fill CR
      hist_Nj_lo->Fill(P3idx-0.5,2.0,weight*nSTCp3_lo); // Fill CR
      hist_lo->Fill(P3idx-0.5,1.0, weight * ( fs_P3_lo * nSTCp3_lo ) ); // Fill expected SR count
      hist_Nj_lo->Fill(P3idx-0.5,1.0, weight * ( t.nJet30 > 3 ? fs_Nj4_P3_lo : fs_Nj23_P3_lo) * nSTCp3_lo ); // Fill expected SR count with separate Nj fs
    }
    if (nSTCp4_lo > 0) {
      hist_lo->Fill(P4idx-0.5,2.0,weight*nSTCp4_lo); // Fill CR
      hist_Nj_lo->Fill(P4idx-0.5,2.0,weight*nSTCp4_lo); // Fill CR
      hist_lo->Fill(P4idx-0.5,1.0, weight * ( fs_P4 * nSTCp4_lo) ); // Fill expected SR count
      hist_Nj_lo->Fill(P4idx-0.5,1.0, weight * (t.nJet30 > 3 ? fs_Nj4_P4_lo : fs_Nj23_P4_lo) * nSTCp4_lo ); // Fill expected SR count with separate Nj fs
    }
    
    // Eventwise ST fills
    if (nSTm_lo > 0) {
      hist_lo->Fill(Midx-0.5,0.0,weight*nSTm_lo);
      hist_Nj_lo->Fill(Midx-0.5,0.0,weight*nSTm_lo);
    }
    if (nSTp_lo > 0) {
      hist_lo->Fill(Pidx-0.5,0.0,weight*nSTp_lo);
      hist_Nj_lo->Fill(Pidx-0.5,0.0,weight*nSTp_lo);
    }
    if (nSTp3_lo > 0) {
      hist_lo->Fill(P3idx-0.5,0.0,weight*nSTp3_lo);
      hist_Nj_lo->Fill(P3idx-0.5,0.0,weight*nSTp3_lo);
    }
    if (nSTp4_lo > 0) {
      hist_lo->Fill(P4idx-0.5,0.0,weight*nSTp4_lo);
      hist_Nj_lo->Fill(P4idx-0.5,0.0,weight*nSTp4_lo);
    }

    // High pt
    TH2D* hist_hi = hi_hists[hist];
    TH2D* hist_Nj_hi = hi_hists[hist_Nj];
    if (nSTCm_hi > 0) {
      hist_hi->Fill(Midx-0.5,2.0,weight*nSTCm_hi); // Fill CR
      hist_Nj_hi->Fill(Midx-0.5,2.0,weight*nSTCm_hi); // Fill CR
      hist_hi->Fill(Midx-0.5,1.0, weight * ( fs_M_hi * nSTCm_hi) ); // Fill expected SR count
      hist_Nj_hi->Fill(Midx-0.5,1.0, weight * ( t.nJet30 > 3 ? fs_Nj4_M_hi : fs_Nj23_M_hi) * nSTCm_hi ); // Fill expected SR count with separate Nj fs
    }
    if (nSTCp_hi > 0) {
      hist_hi->Fill(Pidx-0.5,2.0,weight*nSTCp_hi); // Fill CR
      hist_Nj_hi->Fill(Pidx-0.5,2.0,weight*nSTCp_hi); // Fill CR
      hist_hi->Fill(Pidx-0.5,1.0, weight * ( fs_P_hi * nSTCp_hi ) ); // Fill expected SR count
      hist_Nj_hi->Fill(Pidx-0.5,1.0, weight * (t.nJet30 > 3 ? fs_Nj4_P_hi : fs_Nj23_P_hi) * nSTCp_hi ); // Fill expected SR count with separate Nj fs
    }
    if (nSTCp3_hi > 0) {
      hist_hi->Fill(P3idx-0.5,2.0,weight*nSTCp3_hi); // Fill CR
      hist_Nj_hi->Fill(P3idx-0.5,2.0,weight*nSTCp3_hi); // Fill CR
      if (hist_Nj_hi == h_LM_VR_23_hi) cout << "P3 LH VR hi STC event, with nSTCp3_hi = " << nSTCp3_hi << endl;
      hist_hi->Fill(P3idx-0.5,1.0, weight * ( fs_P3_hi * nSTCp3_hi) ); // Fill expected SR count
      hist_Nj_hi->Fill(P3idx-0.5,1.0, weight * (t.nJet30 > 3 ? fs_Nj4_P3_hi : fs_Nj23_P3_hi) * nSTCp3_hi ); // Fill expected SR count with separate Nj fs
    }
    if (nSTCp4_hi > 0) {
      hist_hi->Fill(P4idx-0.5,2.0,weight*nSTCp4_hi); // Fill CR
      hist_Nj_hi->Fill(P4idx-0.5,2.0,weight*nSTCp4_hi); // Fill CR
      hist_hi->Fill(P4idx-0.5,1.0, weight * ( fs_P4 * nSTCp4_hi) ); // Fill expected SR count
      hist_Nj_hi->Fill(P4idx-0.5,1.0, weight * (t.nJet30 > 3 ? fs_Nj4_P4_hi : fs_Nj23_P4_hi) * nSTCp4_hi ); // Fill expected SR count with separate Nj fs
    }
    
    // Eventwise ST fills
    if (nSTm_hi > 0) {
      hist_hi->Fill(Midx-0.5,0.0,weight*nSTm_hi);
      hist_Nj_hi->Fill(Midx-0.5,0.0,weight*nSTm_hi);
    }
    if (nSTp_hi > 0) {
      hist_hi->Fill(Pidx-0.5,0.0,weight*nSTp_hi);
      hist_Nj_hi->Fill(Pidx-0.5,0.0,weight*nSTp_hi);
    }
    if (nSTp3_hi > 0) {
      if (t.mt2 > 100 && t.mt2 < 200 && hist_Nj_hi == h_LM_VR_23_hi) cout << "P3 LM VR hi ST event: " << t.run << ":" << t.lumi << ":" << t.evt << endl;
      hist_hi->Fill(P3idx-0.5,0.0,weight*nSTp3_hi);
      hist_Nj_hi->Fill(P3idx-0.5,0.0,weight*nSTp3_hi);
    }
    if (nSTp4_hi > 0) {
      hist_hi->Fill(P4idx-0.5,0.0,weight*nSTp4_hi);
      hist_Nj_hi->Fill(P4idx-0.5,0.0,weight*nSTp4_hi);
    }

  }//end loop on events in a file
  
  // Post-processing
  
  TH2D* h_LLM_SR = (TH2D*) h_LL_SR->Clone("h_LLM_SR");
  h_LLM_SR->Add(h_LM_SR);
  TH2D* h_LLM_VR = (TH2D*) h_LL_VR->Clone("h_LLM_VR");
  h_LLM_VR->Add(h_LM_VR);
  TH2D* h_LLM_MR = (TH2D*) h_LL_MR->Clone("h_LLM_MR");
  h_LLM_MR->Add(h_LM_MR);
  TH2D* h_HLM_SR = (TH2D*) h_HL_SR->Clone("h_HLM_SR");
  h_HLM_SR->Add(h_HM_SR);
  TH2D* h_HLM_VR = (TH2D*) h_HL_VR->Clone("h_HLM_VR");
  h_HLM_VR->Add(h_HM_VR);
  TH2D* h_HLM_MR = (TH2D*) h_HL_MR->Clone("h_HLM_MR");
  h_HLM_MR->Add(h_HM_MR);

  TH2D* h_LLM_SR_hi = (TH2D*) h_LL_SR_hi->Clone("h_LLM_SR_hi");
  h_LLM_SR_hi->Add(h_LM_SR_hi);
  hi_hists[h_LLM_SR] = h_LLM_SR_hi;
  TH2D* h_LLM_VR_hi = (TH2D*) h_LL_VR_hi->Clone("h_LLM_VR_hi");
  h_LLM_VR_hi->Add(h_LM_VR_hi);
  hi_hists[h_LLM_VR] = h_LLM_VR_hi;
  TH2D* h_LLM_MR_hi = (TH2D*) h_LL_MR_hi->Clone("h_LLM_MR_hi");
  h_LLM_MR_hi->Add(h_LM_MR_hi);
  hi_hists[h_LLM_MR] = h_LLM_MR_hi;
  TH2D* h_HLM_SR_hi = (TH2D*) h_HL_SR_hi->Clone("h_HLM_SR_hi");
  h_HLM_SR_hi->Add(h_HM_SR_hi);
  hi_hists[h_HLM_SR] = h_HLM_SR_hi;
  TH2D* h_HLM_VR_hi = (TH2D*) h_HL_VR_hi->Clone("h_HLM_VR_hi");
  h_HLM_VR_hi->Add(h_HM_VR_hi);
  hi_hists[h_HLM_VR] = h_HLM_VR_hi;
  TH2D* h_HLM_MR_hi = (TH2D*) h_HL_MR_hi->Clone("h_HLM_MR_hi");
  h_HLM_MR_hi->Add(h_HM_MR_hi);
  hi_hists[h_HLM_MR] = h_HLM_MR_hi;

  TH2D* h_LLM_SR_lo = (TH2D*) h_LL_SR_lo->Clone("h_LLM_SR_lo");
  h_LLM_SR_lo->Add(h_LM_SR_lo);
  low_hists[h_LLM_SR] = h_LLM_SR_lo;
  TH2D* h_LLM_VR_lo = (TH2D*) h_LL_VR_lo->Clone("h_LLM_VR_lo");
  h_LLM_VR_lo->Add(h_LM_VR_lo);
  low_hists[h_LLM_VR] = h_LLM_VR_lo;
  TH2D* h_LLM_MR_lo = (TH2D*) h_LL_MR_lo->Clone("h_LLM_MR_lo");
  h_LLM_MR_lo->Add(h_LM_MR_lo);
  low_hists[h_LLM_MR] = h_LLM_MR_lo;
  TH2D* h_HLM_SR_lo = (TH2D*) h_HL_SR_lo->Clone("h_HLM_SR_lo");
  h_HLM_SR_lo->Add(h_HM_SR_lo);
  low_hists[h_HLM_SR] = h_HLM_SR_lo;
  TH2D* h_HLM_VR_lo = (TH2D*) h_HL_VR_lo->Clone("h_HLM_VR_lo");
  h_HLM_VR_lo->Add(h_HM_VR_lo);
  low_hists[h_HLM_VR] = h_HLM_VR_lo;
  TH2D* h_HLM_MR_lo = (TH2D*) h_HL_MR_lo->Clone("h_HLM_MR_lo");
  h_HLM_MR_lo->Add(h_HM_MR_lo);
  low_hists[h_HLM_MR] = h_HLM_MR_lo;

  vector<TH2D*> hists   = {h_LL_SR, h_LM_SR, h_LLM_SR, h_LH_SR, h_HL_SR, h_HM_SR, h_HLM_SR, h_HH_SR, h_LL_VR, h_LM_VR, h_LLM_VR, h_LH_VR, h_HL_VR, h_HM_VR, h_HLM_VR, h_HH_VR,
			   h_LL_MR, h_LM_MR, h_LLM_MR, h_LH_MR, h_HL_MR, h_HM_MR, h_HLM_MR, h_HH_MR};


  TH2D* h_LLM_SR_23 = (TH2D*) h_LL_SR_23->Clone("h_LLM_SR_23");
  h_LLM_SR_23->Add(h_LM_SR_23);
  TH2D* h_LLM_VR_23 = (TH2D*) h_LL_VR_23->Clone("h_LLM_VR_23");
  h_LLM_VR_23->Add(h_LM_VR_23);
  TH2D* h_LLM_MR_23 = (TH2D*) h_LL_MR_23->Clone("h_LLM_MR_23");
  h_LLM_MR_23->Add(h_LM_MR_23);

  TH2D* h_LLM_SR_23_hi = (TH2D*) h_LL_SR_23_hi->Clone("h_LLM_SR_23_hi");
  h_LLM_SR_23_hi->Add(h_LM_SR_23_hi);
  hi_hists[h_LLM_SR_23] = h_LLM_SR_23_hi;
  TH2D* h_LLM_VR_23_hi = (TH2D*) h_LL_VR_23_hi->Clone("h_LLM_VR_23_hi");
  h_LLM_VR_23_hi->Add(h_LM_VR_23_hi);
  hi_hists[h_LLM_VR_23] = h_LLM_VR_23_hi;
  TH2D* h_LLM_MR_23_hi = (TH2D*) h_LL_MR_23_hi->Clone("h_LLM_MR_23_hi");
  h_LLM_MR_23_hi->Add(h_LM_MR_23_hi);
  hi_hists[h_LLM_MR_23] = h_LLM_MR_23_hi;

  TH2D* h_LLM_SR_23_lo = (TH2D*) h_LL_SR_23_lo->Clone("h_LLM_SR_23_lo");
  h_LLM_SR_23_lo->Add(h_LM_SR_23_lo);
  low_hists[h_LLM_SR_23] = h_LLM_SR_23_lo;
  TH2D* h_LLM_VR_23_lo = (TH2D*) h_LL_VR_23_lo->Clone("h_LLM_VR_23_lo");
  h_LLM_VR_23_lo->Add(h_LM_VR_23_lo);
  low_hists[h_LLM_VR_23] = h_LLM_VR_23_lo;
  TH2D* h_LLM_MR_23_lo = (TH2D*) h_LL_MR_23_lo->Clone("h_LLM_MR_23_lo");
  h_LLM_MR_23_lo->Add(h_LM_MR_23_lo);
  low_hists[h_LLM_MR_23] = h_LLM_MR_23_lo;

  vector<TH2D*> hists23   = {h_LL_SR_23, h_LM_SR_23, h_LLM_SR_23, h_LH_SR_23, h_LL_VR_23, h_LM_VR_23, h_LLM_VR_23, h_LH_VR_23, h_LL_MR_23, h_LM_MR_23, h_LLM_MR_23, h_LH_MR_23};

  TH2D* h_HLM_SR_4 = (TH2D*) h_HL_SR_4->Clone("h_HLM_SR_4");
  h_HLM_SR_4->Add(h_HM_SR_4);
  TH2D* h_HLM_VR_4 = (TH2D*) h_HL_VR_4->Clone("h_HLM_VR_4");
  h_HLM_VR_4->Add(h_HM_VR_4);
  TH2D* h_HLM_MR_4 = (TH2D*) h_HL_MR_4->Clone("h_HLM_MR_4");
  h_HLM_MR_4->Add(h_HM_MR_4);

  TH2D* h_HLM_SR_4_hi = (TH2D*) h_HL_SR_4_hi->Clone("h_HLM_SR_4_hi");
  h_HLM_SR_4_hi->Add(h_HM_SR_4_hi);
  hi_hists[h_HLM_SR_4] = h_HLM_SR_4_hi;
  TH2D* h_HLM_VR_4_hi = (TH2D*) h_HL_VR_4_hi->Clone("h_HLM_VR_4_hi");
  h_HLM_VR_4_hi->Add(h_HM_VR_4_hi);
  hi_hists[h_HLM_VR_4] = h_HLM_VR_4_hi;
  TH2D* h_HLM_MR_4_hi = (TH2D*) h_HL_MR_4_hi->Clone("h_HLM_MR_4_hi");
  h_HLM_MR_4_hi->Add(h_HM_MR_4_hi);
  hi_hists[h_HLM_MR_4] = h_HLM_MR_4_hi;

  TH2D* h_HLM_SR_4_lo = (TH2D*) h_HL_SR_4_lo->Clone("h_HLM_SR_4_lo");
  h_HLM_SR_4_lo->Add(h_HM_SR_4_lo);
  low_hists[h_HLM_SR_4] = h_HLM_SR_4_lo;
  TH2D* h_HLM_VR_4_lo = (TH2D*) h_HL_VR_4_lo->Clone("h_HLM_VR_4_lo");
  h_HLM_VR_4_lo->Add(h_HM_VR_4_lo);
  low_hists[h_HLM_VR_4] = h_HLM_VR_4_lo;
  TH2D* h_HLM_MR_4_lo = (TH2D*) h_HL_MR_4_lo->Clone("h_HLM_MR_4_lo");
  h_HLM_MR_4_lo->Add(h_HM_MR_4_lo);
  low_hists[h_HLM_MR_4] = h_HLM_MR_4_lo;

  vector<TH2D*> hists4   = {h_HL_SR_4, h_HM_SR_4, h_HLM_SR_4, h_HH_SR_4, h_HL_VR_4, h_HM_VR_4, h_HLM_VR_4, h_HH_VR_4, h_HL_MR_4, h_HM_MR_4, h_HLM_MR_4, h_HH_MR_4};
  
  // save fshorts so we can easily propagate errors on fshort to errors on the predicted ST counts
  h_FS->SetBinContent(Pidx,fs_P);
  h_FS->SetBinContent(P3idx,fs_P3);
  h_FS->SetBinContent(P4idx,fs_P4);
  h_FS->SetBinContent(Midx,fs_M);
  h_FS->SetBinContent(Lidx,fs_L);
  h_FS_up->SetBinError(Pidx,fs_P_err_up);
  h_FS_dn->SetBinError(Pidx,fs_P_err_dn);
  h_FS_up->SetBinError(P3idx,fs_P3_err_up);
  h_FS_dn->SetBinError(P3idx,fs_P3_err_dn);
  h_FS_up->SetBinError(P4idx,fs_P4_err_up);
  h_FS_dn->SetBinError(P4idx,fs_P4_err_dn);
  h_FS_up->SetBinError(Midx,fs_M_err_up);
  h_FS_dn->SetBinError(Midx,fs_M_err_dn);
  h_FS_up->SetBinError(Lidx,fs_L_err_up);
  h_FS_dn->SetBinError(Lidx,fs_L_err_dn);

  h_FS_23->SetBinContent(Pidx,fs_Nj23_P);
  h_FS_23->SetBinContent(P3idx,fs_Nj23_P3);
  h_FS_23->SetBinContent(P4idx,fs_Nj23_P4);
  h_FS_23->SetBinContent(Midx,fs_Nj23_M);
  h_FS_23->SetBinContent(Lidx,fs_Nj23_L);
  h_FS_23_up->SetBinError(Pidx,fs_Nj23_P_err_up);
  h_FS_23_dn->SetBinError(Pidx,fs_Nj23_P_err_dn);
  h_FS_23_up->SetBinError(P3idx,fs_Nj23_P3_err_up);
  h_FS_23_dn->SetBinError(P3idx,fs_Nj23_P3_err_dn);
  h_FS_23_up->SetBinError(P4idx,fs_Nj23_P4_err_up);
  h_FS_23_dn->SetBinError(P4idx,fs_Nj23_P4_err_dn);
  h_FS_23_up->SetBinError(Midx,fs_Nj23_M_err_up);
  h_FS_23_dn->SetBinError(Midx,fs_Nj23_M_err_dn);
  h_FS_23_up->SetBinError(Lidx,fs_Nj23_L_err_up);
  h_FS_23_dn->SetBinError(Lidx,fs_Nj23_L_err_dn);

  h_FS_4->SetBinContent(Pidx,fs_Nj4_P);
  h_FS_4->SetBinContent(P3idx,fs_Nj4_P3);
  h_FS_4->SetBinContent(P4idx,fs_Nj4_P4);
  h_FS_4->SetBinContent(Midx,fs_Nj4_M);
  h_FS_4->SetBinContent(Lidx,fs_Nj4_L);
  h_FS_4_up->SetBinError(Pidx,fs_Nj4_P_err_up);
  h_FS_4_dn->SetBinError(Pidx,fs_Nj4_P_err_dn);
  h_FS_4_up->SetBinError(P3idx,fs_Nj4_P3_err_up);
  h_FS_4_dn->SetBinError(P3idx,fs_Nj4_P3_err_dn);
  h_FS_4_up->SetBinError(P4idx,fs_Nj4_P4_err_up);
  h_FS_4_dn->SetBinError(P4idx,fs_Nj4_P4_err_dn);
  h_FS_4_up->SetBinError(Midx,fs_Nj4_M_err_up);
  h_FS_4_dn->SetBinError(Midx,fs_Nj4_M_err_dn);
  h_FS_4_up->SetBinError(Lidx,fs_Nj4_L_err_up);
  h_FS_4_dn->SetBinError(Lidx,fs_Nj4_L_err_dn);


  // Hi pt
  h_FS_hi->SetBinContent(Pidx,fs_P_hi);
  h_FS_hi->SetBinContent(P3idx,fs_P3_hi);
  h_FS_hi->SetBinContent(P4idx,fs_P4_hi);
  h_FS_hi->SetBinContent(Midx,fs_M_hi);
  h_FS_hi->SetBinContent(Lidx,fs_L);
  h_FS_hi_up->SetBinError(Pidx,fs_P_err_hi_up);
  h_FS_hi_dn->SetBinError(Pidx,fs_P_err_hi_dn);
  h_FS_hi_up->SetBinError(P3idx,fs_P3_err_hi_up);
  h_FS_hi_dn->SetBinError(P3idx,fs_P3_err_hi_dn);
  h_FS_hi_up->SetBinError(P4idx,fs_P4_err_hi_up);
  h_FS_hi_dn->SetBinError(P4idx,fs_P4_err_hi_dn);
  h_FS_hi_up->SetBinError(Midx,fs_M_err_hi_up);
  h_FS_hi_dn->SetBinError(Midx,fs_M_err_hi_dn);
  h_FS_hi_up->SetBinError(Lidx,fs_L_err_up);
  h_FS_hi_dn->SetBinError(Lidx,fs_L_err_dn);

  h_FS_23_hi->SetBinContent(Pidx,fs_Nj23_P_hi);
  h_FS_23_hi->SetBinContent(P3idx,fs_Nj23_P3_hi);
  h_FS_23_hi->SetBinContent(P4idx,fs_Nj23_P4_hi);
  h_FS_23_hi->SetBinContent(Midx,fs_Nj23_M_hi);
  h_FS_23_hi->SetBinContent(Lidx,fs_Nj23_L);
  h_FS_23_hi_up->SetBinError(Pidx,fs_Nj23_P_err_hi_up);
  h_FS_23_hi_dn->SetBinError(Pidx,fs_Nj23_P_err_hi_dn);
  h_FS_23_hi_up->SetBinError(P3idx,fs_Nj23_P3_err_hi_up);
  h_FS_23_hi_dn->SetBinError(P3idx,fs_Nj23_P3_err_hi_dn);
  h_FS_23_hi_up->SetBinError(P4idx,fs_Nj23_P4_err_hi_up);
  h_FS_23_hi_dn->SetBinError(P4idx,fs_Nj23_P4_err_hi_dn);
  h_FS_23_hi_up->SetBinError(Midx,fs_Nj23_M_err_hi_up);
  h_FS_23_hi_dn->SetBinError(Midx,fs_Nj23_M_err_hi_dn);
  h_FS_23_hi_up->SetBinError(Lidx,fs_Nj23_L_err_up);
  h_FS_23_hi_dn->SetBinError(Lidx,fs_Nj23_L_err_dn);

  h_FS_4_hi->SetBinContent(Pidx,fs_Nj4_P_hi);
  h_FS_4_hi->SetBinContent(P3idx,fs_Nj4_P3_hi);
  h_FS_4_hi->SetBinContent(P4idx,fs_Nj4_P4_hi);
  h_FS_4_hi->SetBinContent(Midx,fs_Nj4_M_hi);
  h_FS_4_hi->SetBinContent(Lidx,fs_Nj4_L);
  h_FS_4_hi_up->SetBinError(Pidx,fs_Nj4_P_err_hi_up);
  h_FS_4_hi_dn->SetBinError(Pidx,fs_Nj4_P_err_hi_dn);
  h_FS_4_hi_up->SetBinError(P3idx,fs_Nj4_P3_err_hi_up);
  h_FS_4_hi_dn->SetBinError(P3idx,fs_Nj4_P3_err_hi_dn);
  h_FS_4_hi_up->SetBinError(P4idx,fs_Nj4_P4_err_hi_up);
  h_FS_4_hi_dn->SetBinError(P4idx,fs_Nj4_P4_err_hi_dn);
  h_FS_4_hi_up->SetBinError(Midx,fs_Nj4_M_err_hi_up);
  h_FS_4_hi_dn->SetBinError(Midx,fs_Nj4_M_err_hi_dn);
  h_FS_4_hi_up->SetBinError(Lidx,fs_Nj4_L_err_up);
  h_FS_4_hi_dn->SetBinError(Lidx,fs_Nj4_L_err_dn);


  // Lo pt
  h_FS_lo->SetBinContent(Pidx,fs_P_lo);
  h_FS_lo->SetBinContent(P3idx,fs_P3_lo);
  h_FS_lo->SetBinContent(P4idx,fs_P4_lo);
  h_FS_lo->SetBinContent(Midx,fs_M_lo);
  h_FS_lo->SetBinContent(Lidx,fs_L);
  h_FS_lo_up->SetBinError(Pidx,fs_P_err_lo_up);
  h_FS_lo_dn->SetBinError(Pidx,fs_P_err_lo_dn);
  h_FS_lo_up->SetBinError(P3idx,fs_P3_err_lo_up);
  h_FS_lo_dn->SetBinError(P3idx,fs_P3_err_lo_dn);
  h_FS_lo_up->SetBinError(P4idx,fs_P4_err_lo_up);
  h_FS_lo_dn->SetBinError(P4idx,fs_P4_err_lo_dn);
  h_FS_lo_up->SetBinError(Midx,fs_M_err_lo_up);
  h_FS_lo_dn->SetBinError(Midx,fs_M_err_lo_dn);
  h_FS_lo_up->SetBinError(Lidx,fs_L_err_up);
  h_FS_lo_dn->SetBinError(Lidx,fs_L_err_dn);

  h_FS_23_lo->SetBinContent(Pidx,fs_Nj23_P_lo);
  h_FS_23_lo->SetBinContent(P3idx,fs_Nj23_P3_lo);
  h_FS_23_lo->SetBinContent(P4idx,fs_Nj23_P4_lo);
  h_FS_23_lo->SetBinContent(Midx,fs_Nj23_M_lo);
  h_FS_23_lo->SetBinContent(Lidx,fs_Nj23_L);
  h_FS_23_lo_up->SetBinError(Pidx,fs_Nj23_P_err_lo_up);
  h_FS_23_lo_dn->SetBinError(Pidx,fs_Nj23_P_err_lo_dn);
  h_FS_23_lo_up->SetBinError(P3idx,fs_Nj23_P3_err_lo_up);
  h_FS_23_lo_dn->SetBinError(P3idx,fs_Nj23_P3_err_lo_dn);
  h_FS_23_lo_up->SetBinError(P4idx,fs_Nj23_P4_err_lo_up);
  h_FS_23_lo_dn->SetBinError(P4idx,fs_Nj23_P4_err_lo_dn);
  h_FS_23_lo_up->SetBinError(Midx,fs_Nj23_M_err_lo_up);
  h_FS_23_lo_dn->SetBinError(Midx,fs_Nj23_M_err_lo_dn);
  h_FS_23_lo_up->SetBinError(Lidx,fs_Nj23_L_err_up);
  h_FS_23_lo_dn->SetBinError(Lidx,fs_Nj23_L_err_dn);

  h_FS_4_lo->SetBinContent(Pidx,fs_Nj4_P_lo);
  h_FS_4_lo->SetBinContent(P3idx,fs_Nj4_P3_lo);
  h_FS_4_lo->SetBinContent(P4idx,fs_Nj4_P4_lo);
  h_FS_4_lo->SetBinContent(Midx,fs_Nj4_M_lo);
  h_FS_4_lo->SetBinContent(Lidx,fs_Nj4_L);
  h_FS_4_lo_up->SetBinError(Pidx,fs_Nj4_P_err_lo_up);
  h_FS_4_lo_dn->SetBinError(Pidx,fs_Nj4_P_err_lo_dn);
  h_FS_4_lo_up->SetBinError(P3idx,fs_Nj4_P3_err_lo_up);
  h_FS_4_lo_dn->SetBinError(P3idx,fs_Nj4_P3_err_lo_dn);
  h_FS_4_lo_up->SetBinError(P4idx,fs_Nj4_P4_err_lo_up);
  h_FS_4_lo_dn->SetBinError(P4idx,fs_Nj4_P4_err_lo_dn);
  h_FS_4_lo_up->SetBinError(Midx,fs_Nj4_M_err_lo_up);
  h_FS_4_lo_dn->SetBinError(Midx,fs_Nj4_M_err_lo_dn);
  h_FS_4_lo_up->SetBinError(Lidx,fs_Nj4_L_err_up);
  h_FS_4_lo_dn->SetBinError(Lidx,fs_Nj4_L_err_dn);

  // Now save the systematics
  TH1D* h_FS_syst = (TH1D*) h_FS->Clone("h_FS_syst");
  TH1D* h_FS_23_syst = (TH1D*) h_FS_23->Clone("h_FS_23_syst");
  TH1D* h_FS_4_syst = (TH1D*) h_FS_4->Clone("h_FS_4_syst");
  TH1D* h_FS_hi_syst = (TH1D*) h_FS_hi->Clone("h_FS_hi_syst");
  TH1D* h_FS_23_hi_syst = (TH1D*) h_FS_23_hi->Clone("h_FS_23_hi_syst");
  TH1D* h_FS_4_hi_syst = (TH1D*) h_FS_4_hi->Clone("h_FS_4_hi_syst");
  TH1D* h_FS_lo_syst = (TH1D*) h_FS_lo->Clone("h_FS_lo_syst");
  TH1D* h_FS_23_lo_syst = (TH1D*) h_FS_23_lo->Clone("h_FS_23_lo_syst");
  TH1D* h_FS_4_lo_syst = (TH1D*) h_FS_4_lo->Clone("h_FS_4_lo_syst");

  h_FS_syst->SetBinError(Pidx,fs_P_syst);
  h_FS_syst->SetBinError(P3idx,fs_P3_syst);
  h_FS_syst->SetBinError(P4idx,fs_P4_syst);
  h_FS_syst->SetBinError(Midx,fs_M_syst);
  h_FS_syst->SetBinError(Lidx,fs_L_syst);

  h_FS_23_syst->SetBinError(Pidx,fs_P_syst_Nj23);
  h_FS_23_syst->SetBinError(P3idx,fs_P3_syst_Nj23);
  h_FS_23_syst->SetBinError(P4idx,fs_P4_syst_Nj23);
  h_FS_23_syst->SetBinError(Midx,fs_M_syst_Nj23);
  h_FS_23_syst->SetBinError(Lidx,fs_L_syst_Nj23);

  h_FS_4_syst->SetBinError(Pidx,fs_P_syst_Nj4);
  h_FS_4_syst->SetBinError(P3idx,fs_P3_syst_Nj4);
  h_FS_4_syst->SetBinError(P4idx,fs_P4_syst_Nj4);
  h_FS_4_syst->SetBinError(Midx,fs_M_syst_Nj4);
  h_FS_4_syst->SetBinError(Lidx,fs_L_syst_Nj4);

  h_FS_hi_syst->SetBinError(Pidx,fs_P_syst_hi);
  h_FS_hi_syst->SetBinError(P3idx,fs_P3_syst_hi);
  h_FS_hi_syst->SetBinError(P4idx,fs_P4_syst_hi);
  h_FS_hi_syst->SetBinError(Midx,fs_M_syst_hi);
  h_FS_hi_syst->SetBinError(Lidx,fs_L_syst_hi);

  h_FS_23_hi_syst->SetBinError(Pidx,fs_P_syst_Nj23_hi);
  h_FS_23_hi_syst->SetBinError(P3idx,fs_P3_syst_Nj23_hi);
  h_FS_23_hi_syst->SetBinError(P4idx,fs_P4_syst_Nj23_hi);
  h_FS_23_hi_syst->SetBinError(Midx,fs_M_syst_Nj23_hi);
  h_FS_23_hi_syst->SetBinError(Lidx,fs_L_syst_Nj23_hi);

  h_FS_4_hi_syst->SetBinError(Pidx,fs_P_syst_Nj4_hi);
  h_FS_4_hi_syst->SetBinError(P3idx,fs_P3_syst_Nj4_hi);
  h_FS_4_hi_syst->SetBinError(P4idx,fs_P4_syst_Nj4_hi);
  h_FS_4_hi_syst->SetBinError(Midx,fs_M_syst_Nj4_hi);
  h_FS_4_hi_syst->SetBinError(Lidx,fs_L_syst_Nj4_hi);

  h_FS_lo_syst->SetBinError(Pidx,fs_P_syst_lo);
  h_FS_lo_syst->SetBinError(P3idx,fs_P3_syst_lo);
  h_FS_lo_syst->SetBinError(P4idx,fs_P4_syst_lo);
  h_FS_lo_syst->SetBinError(Midx,fs_M_syst_lo);
  h_FS_lo_syst->SetBinError(Lidx,fs_L_syst_lo);

  h_FS_23_lo_syst->SetBinError(Pidx,fs_P_syst_Nj23_lo);
  h_FS_23_lo_syst->SetBinError(P3idx,fs_P3_syst_Nj23_lo);
  h_FS_23_lo_syst->SetBinError(P4idx,fs_P4_syst_Nj23_lo);
  h_FS_23_lo_syst->SetBinError(Midx,fs_M_syst_Nj23_lo);
  h_FS_23_lo_syst->SetBinError(Lidx,fs_L_syst_Nj23_lo);

  h_FS_4_lo_syst->SetBinError(Pidx,fs_P_syst_Nj4_lo);
  h_FS_4_lo_syst->SetBinError(P3idx,fs_P3_syst_Nj4_lo);
  h_FS_4_lo_syst->SetBinError(P4idx,fs_P4_syst_Nj4_lo);
  h_FS_4_lo_syst->SetBinError(Midx,fs_M_syst_Nj4_lo);
  h_FS_4_lo_syst->SetBinError(Lidx,fs_L_syst_Nj4_lo);

  cout << "About to write" << endl;
  
  TFile outfile_(Form("%s.root",outtag),"RECREATE"); 
  outfile_.cd();
  for (vector<TH2D*>::iterator hist = hists.begin(); hist != hists.end(); hist++) {
    (*hist)->Write();
    low_hists[(*hist)]->Write();
    hi_hists[(*hist)]->Write();
  }
  for (vector<TH2D*>::iterator hist = hists23.begin(); hist != hists23.end(); hist++) {
    (*hist)->Write();
    low_hists[(*hist)]->Write();
    hi_hists[(*hist)]->Write();
  }
  for (vector<TH2D*>::iterator hist = hists4.begin(); hist != hists4.end(); hist++) {
    (*hist)->Write();
    low_hists[(*hist)]->Write();
    hi_hists[(*hist)]->Write();
  }

  h_2STC_MR->Write();
  h_2STC_VR->Write();
  h_2STC_SR->Write();

  h_FS->Write();
  h_FS_23->Write();
  h_FS_4->Write();
  h_FS_hi->Write();
  h_FS_23_hi->Write();
  h_FS_4_hi->Write();
  h_FS_lo->Write();
  h_FS_23_lo->Write();
  h_FS_4_lo->Write();

  h_FS_up->Write();
  h_FS_23_up->Write();
  h_FS_4_up->Write();
  h_FS_hi_up->Write();
  h_FS_23_hi_up->Write();
  h_FS_4_hi_up->Write();
  h_FS_lo_up->Write();
  h_FS_23_lo_up->Write();
  h_FS_4_lo_up->Write();

  h_FS_dn->Write();
  h_FS_23_dn->Write();
  h_FS_4_dn->Write();
  h_FS_hi_dn->Write();
  h_FS_23_hi_dn->Write();
  h_FS_4_hi_dn->Write();
  h_FS_lo_dn->Write();
  h_FS_23_lo_dn->Write();
  h_FS_4_lo_dn->Write();

  h_FS_syst->Write();
  h_FS_23_syst->Write();
  h_FS_4_syst->Write();
  h_FS_hi_syst->Write();
  h_FS_23_hi_syst->Write();
  h_FS_4_hi_syst->Write();
  h_FS_lo_syst->Write();
  h_FS_23_lo_syst->Write();
  h_FS_4_lo_syst->Write();

  outfile_.Close();
  cout << "Wrote everything" << endl;

  cout << "Total short track count: " << numberOfAllSTs << endl;
  cout << "Total short track candidate count: " << numberOfAllSTCs << endl;


  return 0;
}

int main (int argc, char ** argv) {

  if (argc < 5) {
    cout << "Usage: ./ShortTrackLooper.exe <outtag> <infile> <config> <runtag>" << endl;
    return 1;
  }

  TChain *ch = new TChain("mt2");
  ch->Add(Form("%s*.root",argv[2]));

  ShortTrackLooper * stl = new ShortTrackLooper();
  stl->loop(ch,argv[1],string(argv[3]),argv[4]);
  return 0;
}

float ShortTrackLooper::getAverageISRWeight(const string sample, const string config_tag) {
  // 2016 94x ttsl 0.908781126143
  // 2016 94x ttdl 0.895473127249
  if (config_tag == "mc_94x_Summer16") {
    if (sample.compare("ttsl") == 0) {
      return 0.909;
    }
    else {
      return 0.895;
    }
  }
  // 2017 94x ttsl 0.915829735522
  // 2017 94x ttdl 0.903101272148
  else if (config_tag == "mc_94x_Fall17") {
    if (sample == "ttsl") {
      return 0.916;
    }
    else {
      return 0.903;
    }
  }
  else {
    cout << "Didn't recognize config " << config_tag << endl;
    return 1.0;
  }
}


ShortTrackLooper::ShortTrackLooper() {}

ShortTrackLooper::~ShortTrackLooper() {};
