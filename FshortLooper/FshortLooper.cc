#include "FshortLooper.h"

using namespace std;
using namespace duplicate_removal;

class mt2tree;

const bool recalculate = false; // recalculate Fshort with non-standard (ie not in babies) isolation and quality cutoffs, see below
// only with recalculate = true
const int applyRecoVeto = 2; // 0: None, 1: use MT2 ID leptons for the reco veto, 2: use any Reco ID (Default: 2)
const bool increment17 = false;
const float isoSTC = 6, qualSTC = 3; // change these if recalculating Fshort

// turn on to apply json file to data
const bool applyJSON = true;
const bool blind = true;

bool FshortLooper::FillHists(const vector<TH2D*> hists, const double weight, const int fill_type, const int len_index) {
  if (unlikely(len_index < 1 || len_index > 3)) return false;
  if (unlikely(fill_type < 1 || fill_type > 3)) return false;
  for (vector<TH2D*>::const_iterator hist = hists.begin(); hist != hists.end(); hist++) {
    (*hist)->Fill(0.0,(double) fill_type, weight);
    if (fill_type == 3) continue;
    (*hist)->Fill((double) len_index,(double) fill_type, weight);
  }
  return true;
}

int FshortLooper::loop (TChain* ch, char * outtag, std::string config_tag) {
  string tag(outtag);

  // Book histograms
  TH1::SetDefaultSumw2(true); // Makes histograms do proper error calculation automatically

  TH2D* h_etaphi_ST_P = new TH2D("h_etaphi_ST_P","#eta and #phi of P Short Tracks",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_ST_M = new TH2D("h_etaphi_ST_M","#eta and #phi of M Short Tracks",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_ST_L = new TH2D("h_etaphi_ST_L","#eta and #phi of L Short Tracks",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());

  TH2D* h_etaphi_STC_P = new TH2D("h_etaphi_STC_P","#eta and #phi of P STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_STC_M = new TH2D("h_etaphi_STC_M","#eta and #phi of M STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_STC_L = new TH2D("h_etaphi_STC_L","#eta and #phi of L STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());

  TH2D* h_mtptFSR_ST_P = new TH2D("h_mtptFSR_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_P = new TH2D("h_mtptFSR_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_M = new TH2D("h_mtptFSR_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_M = new TH2D("h_mtptFSR_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_L = new TH2D("h_mtptFSR_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_L = new TH2D("h_mtptFSR_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ST_P_23 = new TH2D("h_mtptFSR_ST_P_23","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_P_23 = new TH2D("h_mtptFSR_STC_P_23","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_M_23 = new TH2D("h_mtptFSR_ST_M_23","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_M_23 = new TH2D("h_mtptFSR_STC_M_23","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_L_23 = new TH2D("h_mtptFSR_ST_L_23","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_L_23 = new TH2D("h_mtptFSR_STC_L_23","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ST_P_4 = new TH2D("h_mtptFSR_ST_P_4","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_P_4 = new TH2D("h_mtptFSR_STC_P_4","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_M_4 = new TH2D("h_mtptFSR_ST_M_4","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_M_4 = new TH2D("h_mtptFSR_STC_M_4","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_L_4 = new TH2D("h_mtptFSR_ST_L_4","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_L_4 = new TH2D("h_mtptFSR_STC_L_4","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ST_P = new TH2D("h_mtptVR_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_P = new TH2D("h_mtptVR_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_M = new TH2D("h_mtptVR_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_M = new TH2D("h_mtptVR_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_L = new TH2D("h_mtptVR_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_L = new TH2D("h_mtptVR_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ST_P_23 = new TH2D("h_mtptVR_ST_P_23","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_P_23 = new TH2D("h_mtptVR_STC_P_23","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_M_23 = new TH2D("h_mtptVR_ST_M_23","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_M_23 = new TH2D("h_mtptVR_STC_M_23","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_L_23 = new TH2D("h_mtptVR_ST_L_23","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_L_23 = new TH2D("h_mtptVR_STC_L_23","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ST_P_4 = new TH2D("h_mtptVR_ST_P_4","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_P_4 = new TH2D("h_mtptVR_STC_P_4","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_M_4 = new TH2D("h_mtptVR_ST_M_4","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_M_4 = new TH2D("h_mtptVR_STC_M_4","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_L_4 = new TH2D("h_mtptVR_ST_L_4","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_L_4 = new TH2D("h_mtptVR_STC_L_4","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ST_P = new TH2D("h_mtptSR_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_P = new TH2D("h_mtptSR_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_M = new TH2D("h_mtptSR_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_M = new TH2D("h_mtptSR_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_L = new TH2D("h_mtptSR_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_L = new TH2D("h_mtptSR_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ST_P_23 = new TH2D("h_mtptSR_ST_P_23","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_P_23 = new TH2D("h_mtptSR_STC_P_23","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_M_23 = new TH2D("h_mtptSR_ST_M_23","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_M_23 = new TH2D("h_mtptSR_STC_M_23","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_L_23 = new TH2D("h_mtptSR_ST_L_23","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_L_23 = new TH2D("h_mtptSR_STC_L_23","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ST_P_4 = new TH2D("h_mtptSR_ST_P_4","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_P_4 = new TH2D("h_mtptSR_STC_P_4","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_M_4 = new TH2D("h_mtptSR_ST_M_4","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_M_4 = new TH2D("h_mtptSR_STC_M_4","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_L_4 = new TH2D("h_mtptSR_ST_L_4","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_L_4 = new TH2D("h_mtptSR_STC_L_4","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  // MtPt of removed lepton events
  TH2D* h_rlmtptFSR = new TH2D("h_rlmtptFSR","p_{T} x M_{T}(ST,MET) of Removed Lepton \"STs\"",10,0,200,10,0,500);
  TH2D* h_rlmtptFSR_23 = new TH2D("h_rlmtptFSR_23","p_{T} x M_{T}(ST,MET) of Removed Lepton \"STs\"",10,0,200,10,0,500);
  TH2D* h_rlmtptFSR_4 = new TH2D("h_rlmtptFSR_4","p_{T} x M_{T}(ST,MET) of Removed Lepton \"STs\"",10,0,200,10,0,500);

  TH2D* h_rlmtptVR = new TH2D("h_rlmtptVR","p_{T} x M_{T}(ST,MET) of Removed Lepton \"STs\"",10,0,200,10,0,500);
  TH2D* h_rlmtptVR_23 = new TH2D("h_rlmtptVR_23","p_{T} x M_{T}(ST,MET) of Removed Lepton \"STs\"",10,0,200,10,0,500);
  TH2D* h_rlmtptVR_4 = new TH2D("h_rlmtptVR_4","p_{T} x M_{T}(ST,MET) of Removed Lepton \"STs\"",10,0,200,10,0,500);

  TH2D* h_rlmtptSR = new TH2D("h_rlmtptSR","p_{T} x M_{T}(ST,MET) of Removed Lepton \"STs\"",10,0,200,10,0,500);
  TH2D* h_rlmtptSR_23 = new TH2D("h_rlmtptSR_23","p_{T} x M_{T}(ST,MET) of Removed Lepton \"STs\"",10,0,200,10,0,500);
  TH2D* h_rlmtptSR_4 = new TH2D("h_rlmtptSR_4","p_{T} x M_{T}(ST,MET) of Removed Lepton \"STs\"",10,0,200,10,0,500);


  double mt2bins[9] = {60,68,76,84,92,100,120,140,200};

  TH1D* h_mt2_ST_P = new TH1D("h_mt2_ST_P","MT2 of P Short Tracks",8,mt2bins);
  TH1D* h_mt2_ST_M = new TH1D("h_mt2_ST_M","MT2 of M Short Tracks",8,mt2bins);
  TH1D* h_mt2_ST_L = new TH1D("h_mt2_ST_L","MT2 of L Short Tracks",8,mt2bins);

  TH1D* h_mt2_STC_P = new TH1D("h_mt2_STC_P","MT2 of P STCs",8,mt2bins);
  TH1D* h_mt2_STC_M = new TH1D("h_mt2_STC_M","MT2 of M STCs",8,mt2bins);
  TH1D* h_mt2_STC_L = new TH1D("h_mt2_STC_L","MT2 of L STCs",8,mt2bins);

  TH1D* h_mt2_ST_P_23 = new TH1D("h_mt2_ST_P_23","MT2 of P Short Tracks",8,mt2bins);
  TH1D* h_mt2_ST_M_23 = new TH1D("h_mt2_ST_M_23","MT2 of M Short Tracks",8,mt2bins);
  TH1D* h_mt2_ST_L_23 = new TH1D("h_mt2_ST_L_23","MT2 of L Short Tracks",8,mt2bins);

  TH1D* h_mt2_STC_P_23 = new TH1D("h_mt2_STC_P_23","MT2 of P STCs",8,mt2bins);
  TH1D* h_mt2_STC_M_23 = new TH1D("h_mt2_STC_M_23","MT2 of M STCs",8,mt2bins);
  TH1D* h_mt2_STC_L_23 = new TH1D("h_mt2_STC_L_23","MT2 of L STCs",8,mt2bins);

  TH1D* h_mt2_ST_P_4 = new TH1D("h_mt2_ST_P_4","MT2 of P Short Tracks",8,mt2bins);
  TH1D* h_mt2_ST_M_4 = new TH1D("h_mt2_ST_M_4","MT2 of M Short Tracks",8,mt2bins);
  TH1D* h_mt2_ST_L_4 = new TH1D("h_mt2_ST_L_4","MT2 of L Short Tracks",8,mt2bins);

  TH1D* h_mt2_STC_P_4 = new TH1D("h_mt2_STC_P_4","MT2 of P STCs",8,mt2bins);
  TH1D* h_mt2_STC_M_4 = new TH1D("h_mt2_STC_M_4","MT2 of M STCs",8,mt2bins);
  TH1D* h_mt2_STC_L_4 = new TH1D("h_mt2_STC_L_4","MT2 of L STCs",8,mt2bins);

  TH1D* h_dphiMet_ST_P = new TH1D("h_dphiMet_ST_P", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
  TH1D* h_dphiMet_ST_M = new TH1D("h_dphiMet_ST_M", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
  TH1D* h_dphiMet_ST_L = new TH1D("h_dphiMet_ST_L", "#Delta#phi(MET,ST)",10,0,TMath::Pi());

  TH1D* h_dphiMet_ST_P_23 = new TH1D("h_dphiMet_ST_P_23", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
  TH1D* h_dphiMet_ST_M_23 = new TH1D("h_dphiMet_ST_M_23", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
  TH1D* h_dphiMet_ST_L_23 = new TH1D("h_dphiMet_ST_L_23", "#Delta#phi(MET,ST)",10,0,TMath::Pi());

  TH1D* h_dphiMet_ST_P_4 = new TH1D("h_dphiMet_ST_P_4", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
  TH1D* h_dphiMet_ST_M_4 = new TH1D("h_dphiMet_ST_M_4", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
  TH1D* h_dphiMet_ST_L_4 = new TH1D("h_dphiMet_ST_L_4", "#Delta#phi(MET,ST)",10,0,TMath::Pi());  

  TH1D* h_dphiMet_STC_P = new TH1D("h_dphiMet_STC_P", "#Delta#phi(MET,STC)",10,0,TMath::Pi());
  TH1D* h_dphiMet_STC_M = new TH1D("h_dphiMet_STC_M", "#Delta#phi(MET,STC)",10,0,TMath::Pi());
  TH1D* h_dphiMet_STC_L = new TH1D("h_dphiMet_STC_L", "#Delta#phi(MET,STC)",10,0,TMath::Pi());

  TH1D* h_dphiMet_STC_P_23 = new TH1D("h_dphiMet_STC_P_23", "#Delta#phi(MET,STC)",10,0,TMath::Pi());
  TH1D* h_dphiMet_STC_M_23 = new TH1D("h_dphiMet_STC_M_23", "#Delta#phi(MET,STC)",10,0,TMath::Pi());
  TH1D* h_dphiMet_STC_L_23 = new TH1D("h_dphiMet_STC_L_23", "#Delta#phi(MET,STC)",10,0,TMath::Pi());

  TH1D* h_dphiMet_STC_P_4 = new TH1D("h_dphiMet_STC_P_4", "#Delta#phi(MET,STC)",10,0,TMath::Pi());
  TH1D* h_dphiMet_STC_M_4 = new TH1D("h_dphiMet_STC_M_4", "#Delta#phi(MET,STC)",10,0,TMath::Pi());
  TH1D* h_dphiMet_STC_L_4 = new TH1D("h_dphiMet_STC_L_4", "#Delta#phi(MET,STC)",10,0,TMath::Pi());  

  TH2D* h_fsFSR = new TH2D("h_fsFSR","ST and STC Counts by Length",4,0,4,4,0,4);
  h_fsFSR->GetXaxis()->SetBinLabel(1,"Incl");
  h_fsFSR->GetXaxis()->SetBinLabel(2,"P");
  h_fsFSR->GetXaxis()->SetBinLabel(3,"M");
  h_fsFSR->GetXaxis()->SetBinLabel(4,"L");
  h_fsFSR->GetYaxis()->SetBinLabel(1,"f_{short}");
  h_fsFSR->GetYaxis()->SetBinLabel(2,"ST");
  h_fsFSR->GetYaxis()->SetBinLabel(3,"STC");
  h_fsFSR->GetYaxis()->SetBinLabel(4,"Tracks");
  TH2D* h_fsFSR_23 = (TH2D*) h_fsFSR->Clone("h_fsFSR_23");
  TH2D* h_fsFSR_4 = (TH2D*) h_fsFSR->Clone("h_fsFSR_4");
  TH2D* h_fsVR = (TH2D*) h_fsFSR->Clone("h_fsVR");
  TH2D* h_fsVR_23 = (TH2D*) h_fsFSR->Clone("h_fsVR_23");
  TH2D* h_fsVR_4 = (TH2D*) h_fsFSR->Clone("h_fsVR_4");
  TH2D* h_fsSR = (TH2D*) h_fsFSR->Clone("h_fsSR");
  TH2D* h_fsSR_23 = (TH2D*) h_fsFSR->Clone("h_fsSR_23");
  TH2D* h_fsSR_4 = (TH2D*) h_fsFSR->Clone("h_fsSR_4");
  TH2D* h_fsFSR_gt1000HT = (TH2D*) h_fsFSR->Clone("h_fsFSR_gt1000HT");
  TH2D* h_fsFSR_lt1000HT = (TH2D*) h_fsFSR->Clone("h_fsFSR_lt1000HT");
  TH2D* h_fsFSR_23_gt1000HT = (TH2D*) h_fsFSR->Clone("h_fsFSR_23_gt1000HT");
  TH2D* h_fsFSR_23_lt1000HT = (TH2D*) h_fsFSR->Clone("h_fsFSR_23_lt1000HT");
  TH2D* h_fsFSR_4_gt1000HT = (TH2D*) h_fsFSR->Clone("h_fsFSR_4_gt1000HT");
  TH2D* h_fsFSR_4_lt1000HT = (TH2D*) h_fsFSR->Clone("h_fsFSR_4_lt1000HT");

  vector<TH2D*> fsrhists = {h_fsFSR, h_fsFSR_23, h_fsFSR_4, 
			    h_fsVR, h_fsVR_23, h_fsVR_4, 
			    h_fsSR, h_fsSR_23, h_fsSR_4,
			    h_fsFSR_lt1000HT, h_fsFSR_gt1000HT, h_fsFSR_23_lt1000HT, h_fsFSR_4_gt1000HT, h_fsFSR_23_lt1000HT, h_fsFSR_4_gt1000HT};

  vector<TH2D*> mtpthists = { h_mtptFSR_ST_P, h_mtptFSR_ST_M, h_mtptFSR_ST_L, h_mtptFSR_STC_P, h_mtptFSR_STC_M, h_mtptFSR_STC_L,
			      h_mtptFSR_ST_P_23, h_mtptFSR_ST_M_23, h_mtptFSR_ST_L_23, h_mtptFSR_STC_P_23, h_mtptFSR_STC_M_23, h_mtptFSR_STC_L_23,
			      h_mtptFSR_ST_P_4, h_mtptFSR_ST_M_4, h_mtptFSR_ST_L_4, h_mtptFSR_STC_P_4, h_mtptFSR_STC_M_4, h_mtptFSR_STC_L_4,
			      h_mtptVR_ST_P, h_mtptVR_ST_M, h_mtptVR_ST_L, h_mtptVR_STC_P, h_mtptVR_STC_M, h_mtptVR_STC_L,
			      h_mtptVR_ST_P_23, h_mtptVR_ST_M_23, h_mtptVR_ST_L_23, h_mtptVR_STC_P_23, h_mtptVR_STC_M_23, h_mtptVR_STC_L_23,
			      h_mtptVR_ST_P_4, h_mtptVR_ST_M_4, h_mtptVR_ST_L_4, h_mtptVR_STC_P_4, h_mtptVR_STC_M_4, h_mtptVR_STC_L_4,
			      h_mtptSR_ST_P, h_mtptSR_ST_M, h_mtptSR_ST_L, h_mtptSR_STC_P, h_mtptSR_STC_M, h_mtptSR_STC_L,
			      h_mtptSR_ST_P_23, h_mtptSR_ST_M_23, h_mtptSR_ST_L_23, h_mtptSR_STC_P_23, h_mtptSR_STC_M_23, h_mtptSR_STC_L_23,
			      h_mtptSR_ST_P_4, h_mtptSR_ST_M_4, h_mtptSR_ST_L_4, h_mtptSR_STC_P_4, h_mtptSR_STC_M_4, h_mtptSR_STC_L_4};

  vector<TH2D*> rlmtpthists = { h_rlmtptFSR, h_rlmtptFSR_23, h_rlmtptFSR_4,
				h_rlmtptVR, h_rlmtptVR_23, h_rlmtptVR_4, 
				h_rlmtptSR, h_rlmtptSR_23, h_rlmtptSR_4};

  vector<TH1D*> mt2hists =  { h_mt2_ST_P, h_mt2_ST_M, h_mt2_ST_L, h_mt2_STC_P, h_mt2_STC_M, h_mt2_STC_L,
			      h_mt2_ST_P_23, h_mt2_ST_M_23, h_mt2_ST_L_23, h_mt2_STC_P_23, h_mt2_STC_M_23, h_mt2_STC_L_23,
			      h_mt2_ST_P_4, h_mt2_ST_M_4, h_mt2_ST_L_4, h_mt2_STC_P_4, h_mt2_STC_M_4, h_mt2_STC_L_4};

  vector<TH1D*> dphiMethists =  { h_dphiMet_ST_P, h_dphiMet_ST_M, h_dphiMet_ST_L, h_dphiMet_STC_P, h_dphiMet_STC_M, h_dphiMet_STC_L,
			      h_dphiMet_ST_P_23, h_dphiMet_ST_M_23, h_dphiMet_ST_L_23, h_dphiMet_STC_P_23, h_dphiMet_STC_M_23, h_dphiMet_STC_L_23,
			      h_dphiMet_ST_P_4, h_dphiMet_ST_M_4, h_dphiMet_ST_L_4, h_dphiMet_STC_P_4, h_dphiMet_STC_M_4, h_dphiMet_STC_L_4};

  vector<TH2D*> etaphihists = {h_etaphi_ST_P, h_etaphi_ST_M, h_etaphi_ST_L,h_etaphi_STC_P, h_etaphi_STC_M, h_etaphi_STC_L};
			    
  mt2tree t;
  t.Init(ch);



  // Load the configuration and output to screen
  config_ = GetMT2Config(config_tag);
  cout << "[MT2Looper::loop] using configuration tag: " << config_tag << endl;
  cout << "                  JSON: " << config_.json << endl;
  cout << "                  lumi: " << config_.lumi << " fb-1" << endl;


  if (applyJSON && config_.json != "") {
    cout << "[FshortLooper::loop] Loading json file: " << config_.json << endl;
    set_goodrun_file(("../babymaker/jsons/"+config_.json).c_str());
    //    set_goodrun_file((config_.json).c_str());
  }

  vector<const Int_t*> trigs_SR_;
  vector<const Int_t*> trigs_prescaled_;
  vector<const Int_t*> trigs_singleLep_;

  // load the triggers we want
  if(config_.triggers.size() > 0){
    if( config_.triggers.find("SR")       == config_.triggers.end() || 
	config_.triggers.find("prescaledHT")       == config_.triggers.end() || 
	config_.triggers.find("SingleMu") == config_.triggers.end() || 
	config_.triggers.find("SingleEl") == config_.triggers.end() ){
      cout << "[FshortLooper::loop] ERROR: invalid trigger map in configuration '" << config_tag << "'!" << endl;
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
  }else{
    cout << "                  No triggers provided (OK if this is MC)" << endl;
    if(config_tag.find("data") != string::npos){
      cout << "[FshortLooper::loop] WARNING! it looks like you are using data and didn't supply any triggers in the configuration." <<
	"\n                  Every event is going to fail the trigger selection!" << endl;
    }
  }

  const int year = config_.year;
  const float lumi = config_.lumi;

  const unsigned int nEventsTree = ch->GetEntries();
  int nDuplicates = 0;
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

    if (unlikely(t.nVert == 0)) {
      continue;
    }
    if (t.nJet30 < 2) {
      continue;
    }
    if (unlikely(t.nJet30FailId != 0)) {
      continue;
    }
    if (unlikely(t.met_miniaodPt / t.met_caloPt > 5.0)) {
      continue;
    }
    if (unlikely(t.nJet200MuFrac50DphiMet > 0)) {
      continue;
    }

    const bool lepveto = t.nMuons10 + t.nElectrons10 + t.nPFLep5LowMT + t.nPFHad10LowMT > 0;

    // if we would veto a lepton, we just want to fill the removed lepton plots if appropriate and then continue
    if (lepveto) {
      if (t.nlep == 1) {
	// We'll do the best we can in data to use only good leptons. In MC, we want to cheat and look at only actual W leptons.
	// The trigger turn-on for leptons is probably going to mess up data-MC and data-data (lep, ST) agreement, as there's no reason 
	// to expect lepton loss rates are independent of the variables that affect trigger efficiency.
	if (t.isData && !passTrigger(t, trigs_singleLep_)) continue; 
	if (t.rl_ht < 250) continue;
	if (t.rl_mt2 < 60) continue;
	if (t.rl_met_pt < 30) continue;
	if (t.rl_diffMetMht / t.rl_met_pt > 0.5) continue;
	if (t.rl_deltaPhiMin < 0.3) continue;

	// We can hopefully assume that any lep passing one of the MT2 analysis' IDs has quality/isolation to pass the ST selection...
	if (t.lep_pt[0] < 15) continue;
	if (fabs(t.lep_eta[0]) > 2.4) continue;

	if (!t.isData) {
	  // we only want the real electroweak contribution from MC;
	  // make sure that the reco lep corresponds to a prompt gen lep
	  bool match = false;
	  for (int i_gl = 0; i_gl < t.ngenLep && !match; i_gl++) {
	    match = DeltaR(t.lep_eta[0], t.genLep_eta[i_gl], t.lep_phi[0], t.genLep_phi[i_gl]) < 0.01;
	  }
	  if (!match) continue;
	}
			
	float weight = t.isData ? 1.0 : t.evt_scale1fb * lumi;

	const float lepMT = MT( t.lep_pt[0], t.lep_phi[0], t.rl_met_pt, t.rl_met_phi );
	if (t.rl_mt2 < 100) {
	  h_rlmtptFSR->Fill( lepMT, t.lep_pt[0], weight );
	  t.nJet30 < 4 ? h_rlmtptFSR_23->Fill( lepMT, t.lep_pt[0], weight ): h_rlmtptFSR_4->Fill( lepMT, t.lep_pt[0], weight );
	}
	else if (t.rl_mt2 < 200) {
	  h_rlmtptVR->Fill( lepMT, t.lep_pt[0], weight);
	  t.nJet30 < 4 ? h_rlmtptVR_23->Fill( lepMT, t.lep_pt[0], weight ): h_rlmtptVR_4->Fill( lepMT, t.lep_pt[0], weight );
	}
	else if (! (t.isData && blind) ){
	  h_rlmtptSR->Fill( lepMT, t.lep_pt[0], weight);
	  t.nJet30 < 4 ? h_rlmtptSR_23->Fill( lepMT, t.lep_pt[0], weight ): h_rlmtptSR_4->Fill( lepMT, t.lep_pt[0], weight );
	}
      }
      continue;
    }
    

    if (t.ht < 250) {
      continue;
    }

    if (t.mt2 < 60) {
      continue;
    }

    if (t.met_pt < 30) {
      continue;
    }

    if (t.diffMetMht / t.met_pt > 0.5) {
      continue;
    }

    if (t.deltaPhiMin < 0.3) {
      continue;
    }


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

    const bool passPrescaleTrigger = passTrigger(t, trigs_prescaled_);
    const bool passSRTrigger = passTrigger(t, trigs_SR_);

    if (! (passPrescaleTrigger || passSRTrigger)) continue;

    double weight = t.isData ? 1.0 : t.evt_scale1fb * lumi;
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
	  if (t.HLT_PFHT180_Prescale) {
	    weight /= 1316.0;
	  }
	  else if (t.HLT_PFHT250_Prescale) {
	    weight /= 824.0;
	  }
	  else if (t.HLT_PFHT370_Prescale) {
	    weight /= 707.0;
	  }
	  else if (t.HLT_PFHT430_Prescale) {
	    weight /= 307.0;
	  }
	  else if (t.HLT_PFHT510_Prescale) {
	    weight /= 145.0;
	  }
	  else if (t.HLT_PFHT590_Prescale) {
	    weight /= 67.4;
	  }
	  else if (t.HLT_PFHT680_Prescale) {
	    weight /= 47.4;
	  }	
	  else if (t.HLT_PFHT780_Prescale) {
	    weight /= 29.5;
	  }	
	  else { // 890
	    weight /= 17.1;
	  }
	}
      }
      else { // not prescaled, but may not be in the 100% efficiency region
	if (year == 2016 && (t.met_pt < 250 && t.ht < 1000)) {
	  if (t.met_pt < 200) {
	    weight = ((0.8/100) * (t.met_pt - 100)) + 0.1;
	  } else {
	    weight = ((0.1/50) * (t.met_pt - 200)) + 0.9;
	  }
	}
	else if (year == 2017 && (t.met_pt < 250 && t.ht < 1200)) {
	  // use same values for 2017 for now
	  if (t.met_pt < 200) {
	    weight = ((0.8/100) * (t.met_pt - 100)) + 0.1;
	  } else {
	    weight = ((0.1/50) * (t.met_pt - 200)) + 0.9;
	  }	  
	}
      }
    }

    vector<TH2D*> histsToFill;
    // Fshort region
    if (t.mt2 < 100) {
      histsToFill.push_back(h_fsFSR);
      if (t.nJet30 < 4) {
	histsToFill.push_back(h_fsFSR_23);
	t.ht < 1000 ? histsToFill.push_back(h_fsFSR_23_lt1000HT) : histsToFill.push_back(h_fsFSR_23_gt1000HT);
      } else {
	histsToFill.push_back(h_fsFSR_4);
	t.ht < 1000 ? histsToFill.push_back(h_fsFSR_4_lt1000HT) : histsToFill.push_back(h_fsFSR_4_gt1000HT);
      }
      t.ht < 1000 ? histsToFill.push_back(h_fsFSR_lt1000HT) : histsToFill.push_back(h_fsFSR_gt1000HT);
    }
    // Validation Region
    else if (t.mt2 < 200) {
      histsToFill.push_back(h_fsVR);
      t.nJet30 < 4 ? histsToFill.push_back(h_fsVR_23) : histsToFill.push_back(h_fsVR_4);
    }
    // Signal Region
    else if ( !(blind && t.isData) ) { // Only look at MT2 > 200 GeV in data if unblinded
      histsToFill.push_back(h_fsSR); 
      t.nJet30 < 4 ? histsToFill.push_back(h_fsSR_23) : histsToFill.push_back(h_fsSR_4);
    }
    // Should only get here if looking at data while blinded
    else {
      continue; // No need to consider this event if not filling any hists
    }

    // Analysis code

    const int ntracks = t.ntracks;
    for (int i_trk = 0; i_trk < ntracks; i_trk++) {   

      int fillIndex = -1;
      bool isST = false, isSTC = false;
      if (recalculate) {

	// Apply basic selections
	const bool CaloSel = !(t.track_DeadECAL[i_trk] || t.track_DeadHCAL[i_trk]);
	const float pt = t.track_pt[i_trk];
	const bool ptSel = pt > 15;
	const bool etaSel = fabs(t.track_eta[i_trk]) < 2.4;
	const bool BaseSel = CaloSel && ptSel && etaSel;
	
	if (!BaseSel) {
	  continue;
	}
	
	FillHists(histsToFill,weight,3); // No length yet, just filling inclusive tracks
	
	// Length and category
	// Length and categorization
	const bool lostOuterHitsSel = t.track_nLostOuterHits[i_trk] >= 2;
	const int nLayers = t.track_nLayersWithMeasurement[i_trk];
	const bool isP = nLayers == t.track_nPixelLayersWithMeasurement[i_trk] && lostOuterHitsSel && nLayers == (increment17 ? (year == 2016 ? 3 : 4) : 3);
	const bool isLong = nLayers >= (increment17 ? (year == 2016 ? 7 : 8) : 7);
	const bool isM = !isP && !isLong && lostOuterHitsSel;
	const bool isL = isLong && lostOuterHitsSel;
	const bool isShort = isP || isM || isL;
	if (!isShort) {
	  continue;
	}

	fillIndex = isP ? 1 : (isM ? 2 : 3);
	
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
	const bool nreliso = t.track_neuRelIso0p05[i_trk];
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
	const bool pixLayersSelLoose = t.track_nPixelLayersWithMeasurement[i_trk] >= (increment17 ? (year == 2016 ? 2 : 3) : 2);
	const bool pixLayersSelTight = t.track_nPixelLayersWithMeasurement[i_trk] >= (increment17 ? (year == 2016 ? 3 : 4) : 3);
	const bool pixLayersSel4 = nLayers > ((increment17 ? (year == 2016 ? 4 : 5) : 4) ? pixLayersSelLoose : pixLayersSelTight); // Apply 2 layer selection to tracks with 5 (6) or more layers in 2016 (17/18)
	
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
      }
      else {
	if (t.track_iscandidate[i_trk]) {
	  isSTC = true;
	  fillIndex = t.track_ispixelonlycandidate[i_trk] ? 1 : (t.track_ismediumcandidate[i_trk] ? 2 : 3);	  
	}
	else if (t.track_isshort[i_trk]) {
	  isST = true;
	  fillIndex = t.track_ispixelonly[i_trk] ? 1 : (t.track_ismedium[i_trk] ? 2 : 3);
	}
      }

      if (!isST && !isSTC) {
	continue;
      }

      if (!t.track_isHighPurity[i_trk]) {
	continue;
      }

      // One final cut for L tracks to catch lost leptons
      // These tracks are probably background, likely a lost muon from W+jets
      const float mt = MT( t.track_pt[i_trk], t.track_phi[i_trk], t.met_pt, t.met_phi );
      if (fillIndex == 3 && (t.track_pt[i_trk] < 150 && mt < 100)) {
	// in this case, just fill the mtpt hists and continue
	if (isSTC) {
	  if (t.mt2 < 100) {
	    h_mtptFSR_STC_L->Fill(mt, t.track_pt[i_trk], weight);
	    t.nJet30 < 4 ? h_mtptFSR_STC_L_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptFSR_STC_L_4->Fill(mt, t.track_pt[i_trk], weight);
	  }
	  else if (t.mt2 < 200) {
	    h_mtptVR_STC_L->Fill( mt, t.track_pt[i_trk], weight);
	    t.nJet30 < 4 ? h_mtptVR_STC_L_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptVR_STC_L_4->Fill(mt, t.track_pt[i_trk], weight);
	  } 
	  else {
	    h_mtptSR_STC_L->Fill( mt, t.track_pt[i_trk], weight);
	    t.nJet30 < 4 ? h_mtptSR_STC_L_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptSR_STC_L_4->Fill(mt, t.track_pt[i_trk], weight);
	  }
	}
	else {
	  if (t.mt2 < 100) {
	    h_mtptFSR_ST_L->Fill(mt, t.track_pt[i_trk], weight);
	    t.nJet30 < 4 ? h_mtptFSR_ST_L_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptFSR_ST_L_4->Fill(mt, t.track_pt[i_trk], weight);
	  }
	  else if (t.mt2 < 200) {
	    h_mtptVR_ST_L->Fill(mt, t.track_pt[i_trk], weight);
	    t.nJet30 < 4 ? h_mtptVR_ST_L_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptVR_ST_L_4->Fill(mt, t.track_pt[i_trk], weight);
	  }
	  else {
	    h_mtptSR_ST_L->Fill(mt, t.track_pt[i_trk], weight);
	    t.nJet30 < 4 ? h_mtptSR_ST_L_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptSR_ST_L_4->Fill(mt, t.track_pt[i_trk], weight);
	  }
	}
	continue;
      }

      const float dphiMet = DeltaPhi(t.track_phi[i_trk],t.met_phi);

      // Fills
      if (isSTC) {
	FillHists(histsToFill,weight,2,fillIndex);
	if (!blind || t.mt2 < 200 || !t.isData) {
	  switch (fillIndex) {
	  case 1: 
	    h_mt2_STC_P->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_STC_P_23->Fill(t.mt2,weight) : h_mt2_STC_P_4->Fill(t.mt2,weight); 
	    h_dphiMet_STC_P->Fill(dphiMet,weight); 
	    t.nJet30 < 4 ? h_dphiMet_STC_P_23->Fill(dphiMet,weight) : h_dphiMet_STC_P_4->Fill(dphiMet,weight); 
	    h_etaphi_STC_P->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight); 
	    if (t.mt2 < 100) {
	      h_mtptFSR_STC_P->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptFSR_STC_P_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptFSR_STC_P_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_STC_P->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptVR_STC_P_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptVR_STC_P_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else {
	      h_mtptSR_STC_P->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptSR_STC_P_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptSR_STC_P_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    break;
	  case 2: 
	    h_mt2_STC_M->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_STC_M_23->Fill(t.mt2,weight) : h_mt2_STC_M_4->Fill(t.mt2,weight); 
	    h_dphiMet_STC_M->Fill(dphiMet,weight); 
	    t.nJet30 < 4 ? h_dphiMet_STC_M_23->Fill(dphiMet,weight) : h_dphiMet_STC_M_4->Fill(dphiMet,weight); 
	    h_etaphi_STC_M->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight); 
	    if (t.mt2 < 100) {
	      h_mtptFSR_STC_M->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptFSR_STC_M_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptFSR_STC_M_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_STC_M->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptVR_STC_M_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptVR_STC_M_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else {
	      h_mtptSR_STC_M->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptSR_STC_M_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptSR_STC_M_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    break;
	  case 3: 
	    h_mt2_STC_L->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_STC_L_23->Fill(t.mt2,weight) : h_mt2_STC_L_4->Fill(t.mt2,weight); 
	    h_dphiMet_STC_L->Fill(dphiMet,weight); 
	    t.nJet30 < 4 ? h_dphiMet_STC_L_23->Fill(dphiMet,weight) : h_dphiMet_STC_L_4->Fill(dphiMet,weight); 
	    h_etaphi_STC_L->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight); 
	    if (t.mt2 < 100) {
	      h_mtptFSR_STC_L->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptFSR_STC_L_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptFSR_STC_L_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_STC_L->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptVR_STC_L_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptVR_STC_L_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else {
	      h_mtptSR_STC_L->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptSR_STC_L_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptSR_STC_L_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    break;
	  }
	}
      }
      if (isST) {
	FillHists(histsToFill,weight,1,fillIndex);
	if (!blind || t.mt2 < 200 || !t.isData) {
	  switch (fillIndex) {
	  case 1: 
	    h_mt2_ST_P->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_ST_P_23->Fill(t.mt2,weight) : h_mt2_ST_P_4->Fill(t.mt2,weight); 
	    h_dphiMet_ST_P->Fill(dphiMet,weight); 
	    t.nJet30 < 4 ? h_dphiMet_ST_P_23->Fill(dphiMet,weight) : h_dphiMet_ST_P_4->Fill(dphiMet,weight); 
	    h_etaphi_ST_P->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight); 
	    if (t.mt2 < 100) {
	      h_mtptFSR_ST_P->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptFSR_ST_P_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptFSR_ST_P_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_ST_P->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptVR_ST_P_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptVR_ST_P_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else {
	      h_mtptSR_ST_P->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptSR_ST_P_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptSR_ST_P_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    break;
	  case 2: 
	    h_mt2_ST_M->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_ST_M_23->Fill(t.mt2,weight) : h_mt2_ST_M_4->Fill(t.mt2,weight); 
	    h_dphiMet_ST_M->Fill(dphiMet,weight); 
	    t.nJet30 < 4 ? h_dphiMet_ST_M_23->Fill(dphiMet,weight) : h_dphiMet_ST_M_4->Fill(dphiMet,weight); 
	    h_etaphi_ST_M->Fill(t.track_eta[i_trk], t.track_phi[i_trk], weight); 
	    if (t.mt2 < 100) {
	      h_mtptFSR_ST_M->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptFSR_ST_M_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptFSR_ST_M_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_ST_M->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptVR_ST_M_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptVR_ST_M_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else {
	      h_mtptSR_ST_M->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptSR_ST_M_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptSR_ST_M_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    break;
	  case 3: 
	    h_mt2_ST_L->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_ST_L_23->Fill(t.mt2,weight) : h_mt2_ST_L_4->Fill(t.mt2,weight); 
	    h_dphiMet_ST_L->Fill(dphiMet,weight); 
	    t.nJet30 < 4 ? h_dphiMet_ST_L_23->Fill(dphiMet,weight) : h_dphiMet_ST_L_4->Fill(dphiMet,weight); 
	    h_etaphi_ST_L->Fill(t.track_eta[i_trk],t.track_phi[i_trk], weight); 
	    if (t.mt2 < 100) {
	      h_mtptFSR_ST_L->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptFSR_ST_L_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptFSR_ST_L_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_ST_L->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptVR_ST_L_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptVR_ST_L_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else {
	      h_mtptSR_ST_L->Fill(mt, t.track_pt[i_trk], weight);
	      t.nJet30 < 4 ? h_mtptSR_ST_L_23->Fill(mt, t.track_pt[i_trk], weight) : h_mtptSR_ST_L_4->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    break;
	  }
	}
      }


    } // Track loop

  }//end loop on events in a file

  // Post-processing

  // Calculate Fshort for this particular sample
  for (vector<TH2D*>::iterator hist = fsrhists.begin(); hist != fsrhists.end(); hist++) {
    TH2D* h = *hist;
    for (int len = 1; len < 5; len++) {
      double den = h->GetBinContent((double) len, 3); // STC count
      if (den == 0.0) {
	h->SetBinContent((double) len, 1.0, -1.0);
	h->SetBinError((double) len, 1.0, 0.0);
	continue;
      }
      double num = h->GetBinContent((double) len, 2); // ST count
      double fs = num / den; // fshort = ST / STC
      h->SetBinContent((double) len, 1.0, fs);
      double derr = h->GetBinError( (double) len, 3);
      double nerr = h->GetBinError( (double) len, 2);
      double numsq = num * num;
      double densq = den * den;
      double newerr = sqrt((derr*derr * numsq + nerr*nerr * densq) / (densq * densq));
      h->SetBinError((double) len, 1.0, newerr);
    }
  }

  cout << "Processed " << nEventsTree << " events." << endl;
  cout << nDuplicates << " duplicates skipped." << endl;


  cout << "About to write" << endl;

  TFile outfile_(Form("%s.root",outtag),"RECREATE"); 
  outfile_.cd();
  for (vector<TH2D*>::iterator hist = fsrhists.begin(); hist != fsrhists.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = mtpthists.begin(); hist != mtpthists.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = rlmtpthists.begin(); hist != rlmtpthists.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = etaphihists.begin(); hist != etaphihists.end(); hist++) (*hist)->Write();
  for (vector<TH1D*>::iterator hist = mt2hists.begin(); hist != mt2hists.end(); hist++) (*hist)->Write();
  for (vector<TH1D*>::iterator hist = dphiMethists.begin(); hist != dphiMethists.end(); hist++) (*hist)->Write();
  outfile_.Close();
  cout << "Wrote everything" << endl;

  return 0;
}

int main (int argc, char ** argv) {

  if (argc < 4) {
    cout << "Usage: ./FshortLooper.exe <outtag> <infile> <config>" << endl;
    return 1;
  }

  TChain *ch = new TChain("mt2");
  ch->Add(Form("%s*.root",argv[2]));

  FshortLooper * fsl = new FshortLooper();
  fsl->loop(ch,argv[1],string(argv[3]));
  return 0;
}

FshortLooper::FshortLooper() {}

FshortLooper::~FshortLooper() {};
