#include "FshortLooper.h"

using namespace std;
using namespace duplicate_removal;

class mt2tree;

const bool recalculate = true; // recalculate Fshort with non-standard (ie not in babies) isolation and quality cutoffs, see below
// only with recalculate = true
const int applyRecoVeto = 2; // 0: None, 1: use MT2 ID leptons for the reco veto, 2: use any Reco ID (Default: 2)
const bool increment17 = false;
const float isoSTC = 6, qualSTC = 3; // change these if recalculating Fshort

// turn on to apply json file to data
const bool applyJSON = true;

const bool applyISRWeights = false; // evt_id is messed up in current babies, and we don't have isr weights implemented for 2017 MC anyway

const bool doNTrueIntReweight = true; // evt_id is messed up in current babies

const bool blind = true;

TFile VetoFile("VetoHists.root");
TH2F* veto_bar = (TH2F*) VetoFile.Get("h_VetoEtaPhi_bar");
TH2F* veto_ecp = (TH2F*) VetoFile.Get("h_VetoEtaPhi_ecp");
TH2F* veto_ecn = (TH2F*) VetoFile.Get("h_VetoEtaPhi_ecn");

int FshortLooper::InEtaPhiVetoRegion(float eta, float phi) {
  if (fabs(eta) > 2.4) return -1;
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
  return 0;
}

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

  cout << "FshortLooper::loop" << endl;

  // Book histograms
  TH1::SetDefaultSumw2(true); // Makes histograms do proper error calculation automatically

  TH1D* h_ht = new TH1D("h_ht","H_{T} of Input Events",100,0,3000);

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

  std::vector<TH2D*> fsrhists = {h_fsFSR, h_fsFSR_23, h_fsFSR_4, 
			    h_fsVR, h_fsVR_23, h_fsVR_4, 
			    h_fsSR, h_fsSR_23, h_fsSR_4,
			    h_fsFSR_lt1000HT, h_fsFSR_gt1000HT, h_fsFSR_23_lt1000HT, h_fsFSR_4_gt1000HT, h_fsFSR_23_lt1000HT, h_fsFSR_4_gt1000HT};


  // Cut hists
  TH1D* h_reliso_ST_L = new TH1D("h_reliso_ST_L","RelIso of L STs",4,0,0.2);
  TH1D* h_reliso_STC_L = new TH1D("h_reliso_STC_L","RelIso of L STCs",12,0,1.2);
  TH1D* h_reliso_ewk_ST_L = new TH1D("h_reliso_ewk_ST_L","RelIso of Electroweak L STs",4,0,0.2);
  TH1D* h_reliso_ewk_STC_L = new TH1D("h_reliso_ewk_STC_L","RelIso of Electroweak L STCs",12,0,1.2);
  TH1D* h_reliso_newk_ST_L = new TH1D("h_reliso_newk_ST_L","RelIso of Non-Electroweak L STs",4,0,0.2);
  TH1D* h_reliso_newk_STC_L = new TH1D("h_reliso_newk_STC_L","RelIso of Non-Electroweak L STCs",12,0,1.2);

  TH1D* h_minireliso_ST_L = new TH1D("h_minireliso_ST_L","MiniRelIso of L STs",4,0,0.2);
  TH1D* h_minireliso_STC_L = new TH1D("h_minireliso_STC_L","MiniRelIso of L STCs",12,0,1.2);
  TH1D* h_minireliso_ewk_ST_L = new TH1D("h_minireliso_ewk_ST_L","MiniRelIso of Electroweak L STs",4,0,0.2);
  TH1D* h_minireliso_ewk_STC_L = new TH1D("h_minireliso_ewk_STC_L","MiniRelIso of Electroweak L STCs",12,0,1.2);
  TH1D* h_minireliso_newk_ST_L = new TH1D("h_minireliso_newk_ST_L","MiniRelIso of Non-Electroweak L STs",4,0,0.2);
  TH1D* h_minireliso_newk_STC_L = new TH1D("h_minireliso_newk_STC_L","MiniRelIso of Non-Electroweak L STCs",12,0,1.2);

  TH1D* h_iso_ST_L = new TH1D("h_iso_ST_L","AbsIso of L STs",4,0,10);
  TH1D* h_iso_STC_L = new TH1D("h_iso_STC_L","AbsIso of L STCs",12,0,60);
  TH1D* h_iso_ewk_ST_L = new TH1D("h_iso_ewk_ST_L","AbsIso of Electroweak L STs",4,0,10);
  TH1D* h_iso_ewk_STC_L = new TH1D("h_iso_ewk_STC_L","AbsIso of Electroweak L STCs",12,0,60);
  TH1D* h_iso_newk_ST_L = new TH1D("h_iso_newk_ST_L","AbsIso of Non-Electroweak L STs",4,0,10);
  TH1D* h_iso_newk_STC_L = new TH1D("h_iso_newk_STC_L","AbsIso of Non-Electroweak L STCs",12,0,60);

  TH1D* h_niso_ST_L = new TH1D("h_niso_ST_L","Tight Neutral Iso of L STs",4,0,10);
  TH1D* h_niso_STC_L = new TH1D("h_niso_STC_L","Tight Neutral Iso of L STCs",12,0,60);
  TH1D* h_niso_ewk_ST_L = new TH1D("h_niso_ewk_ST_L","Tight Neutral Iso of Electroweak L STs",4,0,10);
  TH1D* h_niso_ewk_STC_L = new TH1D("h_niso_ewk_STC_L","Tight Neutral Iso of Electroweak L STCs",12,0,60);
  TH1D* h_niso_newk_ST_L = new TH1D("h_niso_newk_ST_L","Tight Neutral Iso of Non-Electroweak L STs",4,0,10);
  TH1D* h_niso_newk_STC_L = new TH1D("h_niso_newk_STC_L","Tight Neutral Iso of Non-Electroweak L STCs",12,0,60);

  TH1D* h_nreliso_ST_L = new TH1D("h_nreliso_ST_L","Tight Neutral RelIso of L STs",4,0,0.1);
  TH1D* h_nreliso_STC_L = new TH1D("h_nreliso_STC_L","Tight Neutral RelIso of L STCs",12,0,0.6);
  TH1D* h_nreliso_ewk_ST_L = new TH1D("h_nreliso_ewk_ST_L","Tight Neutral RelIso of Electroweak L STs",4,0,0.1);
  TH1D* h_nreliso_ewk_STC_L = new TH1D("h_nreliso_ewk_STC_L","Tight Neutral RelIso of Electroweak L STCs",12,0,0.6);
  TH1D* h_nreliso_newk_ST_L = new TH1D("h_nreliso_newk_ST_L","Tight Neutral RelIso of Non-Electroweak L STs",4,0,0.1);
  TH1D* h_nreliso_newk_STC_L = new TH1D("h_nreliso_newk_STC_L","Tight Neutral RelIso of Non-Electroweak L STCs",12,0,0.6);

  TH1D* h_dxy_ST_L = new TH1D("h_dxy_ST_L","dxy of L STs",4,0,0.01);
  TH1D* h_dxy_STC_L = new TH1D("h_dxy_STC_L","dxy of L STCs",12,0,0.03);
  TH1D* h_dxy_ewk_ST_L = new TH1D("h_dxy_ewk_ST_L","dxy of Electroweak L STs",4,0,0.01);
  TH1D* h_dxy_ewk_STC_L = new TH1D("h_dxy_ewk_STC_L","dxy of Electroweak L STCs",12,0,0.03);
  TH1D* h_dxy_newk_ST_L = new TH1D("h_dxy_newk_ST_L","dxy of Non-Electroweak L STs",4,0,0.01);
  TH1D* h_dxy_newk_STC_L = new TH1D("h_dxy_newk_STC_L","dxy of Non-Electroweak L STCs",12,0,0.03);

  TH1D* h_dz_ST_L = new TH1D("h_dz_ST_L","dz of L STs",5,0,0.05);
  TH1D* h_dz_STC_L = new TH1D("h_dz_STC_L","dz of L STCs",15,0,0.15);
  TH1D* h_dz_ewk_ST_L = new TH1D("h_dz_ewk_ST_L","dz of Electroweak L STs",5,0,0.05);
  TH1D* h_dz_ewk_STC_L = new TH1D("h_dz_ewk_STC_L","dz of Electroweak L STCs",15,0,0.15);
  TH1D* h_dz_newk_ST_L = new TH1D("h_dz_newk_ST_L","dz of Non-Electroweak L STs",5,0,0.05);
  TH1D* h_dz_newk_STC_L = new TH1D("h_dz_newk_STC_L","dz of Non-Electroweak L STCs",15,0,0.15);

  TH1D* h_pterr_ST_L = new TH1D("h_pterr_ST_L","p_{T} Err / p_{T}^2 of L STs",5,0,0.005);
  TH1D* h_pterr_STC_L = new TH1D("h_pterr_STC_L","p_{T} Err / p_{T}^2 of L STCs",15,0,0.015);
  TH1D* h_pterr_ewk_ST_L = new TH1D("h_pterr_ewk_ST_L","p_{T} Err / p_{T}^2 of Electroweak L STs",5,0,0.005);
  TH1D* h_pterr_ewk_STC_L = new TH1D("h_pterr_ewk_STC_L","p_{T} Err / p_{T}^2 of Electroweak L STCs",15,0,0.015);
  TH1D* h_pterr_newk_ST_L = new TH1D("h_pterr_newk_ST_L","p_{T} Err / p_{T}^2 of Non-Electroweak L STs",5,0,0.005);
  TH1D* h_pterr_newk_STC_L = new TH1D("h_pterr_newk_STC_L","p_{T} Err / p_{T}^2 of Non-Electroweak L STCs",15,0,0.015);

  // Etaphi
  TH2D* h_etaphi_ST_P = new TH2D("h_etaphi_ST_P","#eta and #phi of P Short Tracks",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_ST_M = new TH2D("h_etaphi_ST_M","#eta and #phi of M Short Tracks",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_ST_L = new TH2D("h_etaphi_ST_L","#eta and #phi of L Short Tracks",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_ST_Lrej = new TH2D("h_etaphi_ST_Lrej","#eta and #phi of Rejected L Short Tracks",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());

  TH2D* h_etaphi_STC_P = new TH2D("h_etaphi_STC_P","#eta and #phi of P STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_STC_M = new TH2D("h_etaphi_STC_M","#eta and #phi of M STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_STC_L = new TH2D("h_etaphi_STC_L","#eta and #phi of L STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_STC_Lrej = new TH2D("h_etaphi_STC_Lrej","#eta and #phi of Rejected L STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());

  TH2D* h_etaphi_ewke_STC_L = new TH2D("h_etaphi_ewke_STC_L","#eta and #phi of Electroweak L STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_ewke_ST_L = new TH2D("h_etaphi_ewke_ST_L","#eta and #phi of Electroweak L STs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_tewke_STC_L = new TH2D("h_etaphi_tewke_STC_L","#eta and #phi of Tau Lep L STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_tewke_ST_L = new TH2D("h_etaphi_tewke_ST_L","#eta and #phi of Tau Lep L STs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_ewkm_STC_L = new TH2D("h_etaphi_ewkm_STC_L","#eta and #phi of Electroweak L STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_ewkm_ST_L = new TH2D("h_etaphi_ewkm_ST_L","#eta and #phi of Electroweak L STs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_tewkm_STC_L = new TH2D("h_etaphi_tewkm_STC_L","#eta and #phi of Tau Lep L STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_tewkm_ST_L = new TH2D("h_etaphi_tewkm_ST_L","#eta and #phi of Tau Lep L STs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_1newk_STC_L = new TH2D("h_etaphi_1newk_STC_L","#eta and #phi of 1-prong Tau L STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_1newk_ST_L = new TH2D("h_etaphi_1newk_ST_L","#eta and #phi of 1-prong Tau L STs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_3newk_STC_L = new TH2D("h_etaphi_3newk_STC_L","#eta and #phi of 1-prong Tau L STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_3newk_ST_L = new TH2D("h_etaphi_3newk_ST_L","#eta and #phi of 1-prong Tau L STs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_newk_STC_L = new TH2D("h_etaphi_newk_STC_L","#eta and #phi of Non-Electroweak L STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_newk_ST_L = new TH2D("h_etaphi_newk_ST_L","#eta and #phi of Non-Electroweak L STs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());


  // nb
  TH1D* h_nb_ST_L_30 = new TH1D("h_nb_ST_L_30","N_{Tag} in L ST Events",4,0,4);
  TH1D* h_nb_ST_L_100 = new TH1D("h_nb_ST_L_100","N_{Tag} in L ST Events",4,0,4);
  TH1D* h_nb_ST_L_200 = new TH1D("h_nb_ST_L_200","N_{Tag} in L ST Events",4,0,4);
  TH1D* h_nb_ST_L_250 = new TH1D("h_nb_ST_L_250","N_{Tag} in L ST Events",4,0,4);
  TH1D* h_nb_ST_L_300 = new TH1D("h_nb_ST_L_300","N_{Tag} in L ST Events",4,0,4);

  TH1D* h_rlnb_ST_L_30 = new TH1D("h_rlnb_ST_L_30","N_{Tag} in Removed Lepton Events",4,0,4);
  TH1D* h_rlnb_ST_L_100 = new TH1D("h_rlnb_ST_L_100","N_{Tag} in Removed Lepton Events",4,0,4);
  TH1D* h_rlnb_ST_L_200 = new TH1D("h_rlnb_ST_L_200","N_{Tag} in Removed Lepton Events",4,0,4);
  TH1D* h_rlnb_ST_L_250 = new TH1D("h_rlnb_ST_L_250","N_{Tag} in Removed Lepton Events",4,0,4);
  TH1D* h_rlnb_ST_L_300 = new TH1D("h_rlnb_ST_L_300","N_{Tag} in Removed Lepton Events",4,0,4);

  // MtPt
  TH2D* h_mtptFSR_ST_P = new TH2D("h_mtptFSR_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_P = new TH2D("h_mtptFSR_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_M = new TH2D("h_mtptFSR_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_M = new TH2D("h_mtptFSR_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_L = new TH2D("h_mtptFSR_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_L = new TH2D("h_mtptFSR_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ST_P_23L = new TH2D("h_mtptFSR_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_P_23L = new TH2D("h_mtptFSR_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_M_23L = new TH2D("h_mtptFSR_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_M_23L = new TH2D("h_mtptFSR_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_L_23L = new TH2D("h_mtptFSR_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_L_23L = new TH2D("h_mtptFSR_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ST_P_23H = new TH2D("h_mtptFSR_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_P_23H = new TH2D("h_mtptFSR_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_M_23H = new TH2D("h_mtptFSR_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_M_23H = new TH2D("h_mtptFSR_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_L_23H = new TH2D("h_mtptFSR_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_L_23H = new TH2D("h_mtptFSR_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ST_P_4L = new TH2D("h_mtptFSR_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_P_4L = new TH2D("h_mtptFSR_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_M_4L = new TH2D("h_mtptFSR_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_M_4L = new TH2D("h_mtptFSR_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_L_4L = new TH2D("h_mtptFSR_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_L_4L = new TH2D("h_mtptFSR_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ST_P_4H = new TH2D("h_mtptFSR_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_P_4H = new TH2D("h_mtptFSR_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_M_4H = new TH2D("h_mtptFSR_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_M_4H = new TH2D("h_mtptFSR_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ST_L_4H = new TH2D("h_mtptFSR_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_L_4H = new TH2D("h_mtptFSR_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ST_P = new TH2D("h_mtptVR_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_P = new TH2D("h_mtptVR_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_M = new TH2D("h_mtptVR_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_M = new TH2D("h_mtptVR_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_L = new TH2D("h_mtptVR_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_L = new TH2D("h_mtptVR_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ST_P_23L = new TH2D("h_mtptVR_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_P_23L = new TH2D("h_mtptVR_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_M_23L = new TH2D("h_mtptVR_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_M_23L = new TH2D("h_mtptVR_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_L_23L = new TH2D("h_mtptVR_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_L_23L = new TH2D("h_mtptVR_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ST_P_23H = new TH2D("h_mtptVR_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_P_23H = new TH2D("h_mtptVR_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_M_23H = new TH2D("h_mtptVR_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_M_23H = new TH2D("h_mtptVR_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_L_23H = new TH2D("h_mtptVR_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_L_23H = new TH2D("h_mtptVR_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ST_P_4L = new TH2D("h_mtptVR_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_P_4L = new TH2D("h_mtptVR_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_M_4L = new TH2D("h_mtptVR_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_M_4L = new TH2D("h_mtptVR_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_L_4L = new TH2D("h_mtptVR_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_L_4L = new TH2D("h_mtptVR_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ST_P_4H = new TH2D("h_mtptVR_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_P_4H = new TH2D("h_mtptVR_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_M_4H = new TH2D("h_mtptVR_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_M_4H = new TH2D("h_mtptVR_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ST_L_4H = new TH2D("h_mtptVR_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_L_4H = new TH2D("h_mtptVR_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ST_P = new TH2D("h_mtptSR_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_P = new TH2D("h_mtptSR_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_M = new TH2D("h_mtptSR_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_M = new TH2D("h_mtptSR_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_L = new TH2D("h_mtptSR_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_L = new TH2D("h_mtptSR_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ST_P_23L = new TH2D("h_mtptSR_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_P_23L = new TH2D("h_mtptSR_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_M_23L = new TH2D("h_mtptSR_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_M_23L = new TH2D("h_mtptSR_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_L_23L = new TH2D("h_mtptSR_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_L_23L = new TH2D("h_mtptSR_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ST_P_23H = new TH2D("h_mtptSR_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_P_23H = new TH2D("h_mtptSR_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_M_23H = new TH2D("h_mtptSR_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_M_23H = new TH2D("h_mtptSR_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_L_23H = new TH2D("h_mtptSR_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_L_23H = new TH2D("h_mtptSR_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ST_P_4L = new TH2D("h_mtptSR_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_P_4L = new TH2D("h_mtptSR_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_M_4L = new TH2D("h_mtptSR_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_M_4L = new TH2D("h_mtptSR_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_L_4L = new TH2D("h_mtptSR_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_L_4L = new TH2D("h_mtptSR_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ST_P_4H = new TH2D("h_mtptSR_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_P_4H = new TH2D("h_mtptSR_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_M_4H = new TH2D("h_mtptSR_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_M_4H = new TH2D("h_mtptSR_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ST_L_4H = new TH2D("h_mtptSR_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_L_4H = new TH2D("h_mtptSR_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  // EWK mtpt
  TH2D* h_mtptFSR_ewke_ST_P = new TH2D("h_mtptFSR_ewke_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_P = new TH2D("h_mtptFSR_ewke_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_ST_M = new TH2D("h_mtptFSR_ewke_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_M = new TH2D("h_mtptFSR_ewke_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_ST_L = new TH2D("h_mtptFSR_ewke_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_L = new TH2D("h_mtptFSR_ewke_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewke_ST_P_23L = new TH2D("h_mtptFSR_ewke_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_P_23L = new TH2D("h_mtptFSR_ewke_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_ST_M_23L = new TH2D("h_mtptFSR_ewke_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_M_23L = new TH2D("h_mtptFSR_ewke_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_ST_L_23L = new TH2D("h_mtptFSR_ewke_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_L_23L = new TH2D("h_mtptFSR_ewke_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewke_ST_P_23H = new TH2D("h_mtptFSR_ewke_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_P_23H = new TH2D("h_mtptFSR_ewke_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_ST_M_23H = new TH2D("h_mtptFSR_ewke_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_M_23H = new TH2D("h_mtptFSR_ewke_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_ST_L_23H = new TH2D("h_mtptFSR_ewke_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_L_23H = new TH2D("h_mtptFSR_ewke_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewke_ST_P_4L = new TH2D("h_mtptFSR_ewke_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_P_4L = new TH2D("h_mtptFSR_ewke_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_ST_M_4L = new TH2D("h_mtptFSR_ewke_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_M_4L = new TH2D("h_mtptFSR_ewke_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_ST_L_4L = new TH2D("h_mtptFSR_ewke_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_L_4L = new TH2D("h_mtptFSR_ewke_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewke_ST_P_4H = new TH2D("h_mtptFSR_ewke_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_P_4H = new TH2D("h_mtptFSR_ewke_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_ST_M_4H = new TH2D("h_mtptFSR_ewke_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_M_4H = new TH2D("h_mtptFSR_ewke_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_ST_L_4H = new TH2D("h_mtptFSR_ewke_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_L_4H = new TH2D("h_mtptFSR_ewke_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewke_ST_P = new TH2D("h_mtptVR_ewke_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_P = new TH2D("h_mtptVR_ewke_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_ST_M = new TH2D("h_mtptVR_ewke_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_M = new TH2D("h_mtptVR_ewke_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_ST_L = new TH2D("h_mtptVR_ewke_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_L = new TH2D("h_mtptVR_ewke_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewke_ST_P_23L = new TH2D("h_mtptVR_ewke_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_P_23L = new TH2D("h_mtptVR_ewke_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_ST_M_23L = new TH2D("h_mtptVR_ewke_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_M_23L = new TH2D("h_mtptVR_ewke_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_ST_L_23L = new TH2D("h_mtptVR_ewke_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_L_23L = new TH2D("h_mtptVR_ewke_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewke_ST_P_23H = new TH2D("h_mtptVR_ewke_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_P_23H = new TH2D("h_mtptVR_ewke_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_ST_M_23H = new TH2D("h_mtptVR_ewke_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_M_23H = new TH2D("h_mtptVR_ewke_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_ST_L_23H = new TH2D("h_mtptVR_ewke_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_L_23H = new TH2D("h_mtptVR_ewke_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewke_ST_P_4L = new TH2D("h_mtptVR_ewke_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_P_4L = new TH2D("h_mtptVR_ewke_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_ST_M_4L = new TH2D("h_mtptVR_ewke_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_M_4L = new TH2D("h_mtptVR_ewke_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_ST_L_4L = new TH2D("h_mtptVR_ewke_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_L_4L = new TH2D("h_mtptVR_ewke_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewke_ST_P_4H = new TH2D("h_mtptVR_ewke_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_P_4H = new TH2D("h_mtptVR_ewke_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_ST_M_4H = new TH2D("h_mtptVR_ewke_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_M_4H = new TH2D("h_mtptVR_ewke_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_ST_L_4H = new TH2D("h_mtptVR_ewke_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_L_4H = new TH2D("h_mtptVR_ewke_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewke_ST_P = new TH2D("h_mtptSR_ewke_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_P = new TH2D("h_mtptSR_ewke_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_ST_M = new TH2D("h_mtptSR_ewke_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_M = new TH2D("h_mtptSR_ewke_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_ST_L = new TH2D("h_mtptSR_ewke_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_L = new TH2D("h_mtptSR_ewke_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewke_ST_P_23L = new TH2D("h_mtptSR_ewke_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_P_23L = new TH2D("h_mtptSR_ewke_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_ST_M_23L = new TH2D("h_mtptSR_ewke_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_M_23L = new TH2D("h_mtptSR_ewke_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_ST_L_23L = new TH2D("h_mtptSR_ewke_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_L_23L = new TH2D("h_mtptSR_ewke_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewke_ST_P_23H = new TH2D("h_mtptSR_ewke_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_P_23H = new TH2D("h_mtptSR_ewke_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_ST_M_23H = new TH2D("h_mtptSR_ewke_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_M_23H = new TH2D("h_mtptSR_ewke_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_ST_L_23H = new TH2D("h_mtptSR_ewke_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_L_23H = new TH2D("h_mtptSR_ewke_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewke_ST_P_4L = new TH2D("h_mtptSR_ewke_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_P_4L = new TH2D("h_mtptSR_ewke_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_ST_M_4L = new TH2D("h_mtptSR_ewke_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_M_4L = new TH2D("h_mtptSR_ewke_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_ST_L_4L = new TH2D("h_mtptSR_ewke_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_L_4L = new TH2D("h_mtptSR_ewke_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewke_ST_P_4H = new TH2D("h_mtptSR_ewke_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_P_4H = new TH2D("h_mtptSR_ewke_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_ST_M_4H = new TH2D("h_mtptSR_ewke_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_M_4H = new TH2D("h_mtptSR_ewke_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_ST_L_4H = new TH2D("h_mtptSR_ewke_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_L_4H = new TH2D("h_mtptSR_ewke_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  // EWK mtpt
  TH2D* h_mtptFSR_ewkm_ST_P = new TH2D("h_mtptFSR_ewkm_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_P = new TH2D("h_mtptFSR_ewkm_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_ST_M = new TH2D("h_mtptFSR_ewkm_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_M = new TH2D("h_mtptFSR_ewkm_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_ST_L = new TH2D("h_mtptFSR_ewkm_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_L = new TH2D("h_mtptFSR_ewkm_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewkm_ST_P_23L = new TH2D("h_mtptFSR_ewkm_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_P_23L = new TH2D("h_mtptFSR_ewkm_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_ST_M_23L = new TH2D("h_mtptFSR_ewkm_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_M_23L = new TH2D("h_mtptFSR_ewkm_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_ST_L_23L = new TH2D("h_mtptFSR_ewkm_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_L_23L = new TH2D("h_mtptFSR_ewkm_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewkm_ST_P_23H = new TH2D("h_mtptFSR_ewkm_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_P_23H = new TH2D("h_mtptFSR_ewkm_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_ST_M_23H = new TH2D("h_mtptFSR_ewkm_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_M_23H = new TH2D("h_mtptFSR_ewkm_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_ST_L_23H = new TH2D("h_mtptFSR_ewkm_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_L_23H = new TH2D("h_mtptFSR_ewkm_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewkm_ST_P_4L = new TH2D("h_mtptFSR_ewkm_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_P_4L = new TH2D("h_mtptFSR_ewkm_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_ST_M_4L = new TH2D("h_mtptFSR_ewkm_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_M_4L = new TH2D("h_mtptFSR_ewkm_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_ST_L_4L = new TH2D("h_mtptFSR_ewkm_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_L_4L = new TH2D("h_mtptFSR_ewkm_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewkm_ST_P_4H = new TH2D("h_mtptFSR_ewkm_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_P_4H = new TH2D("h_mtptFSR_ewkm_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_ST_M_4H = new TH2D("h_mtptFSR_ewkm_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_M_4H = new TH2D("h_mtptFSR_ewkm_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_ST_L_4H = new TH2D("h_mtptFSR_ewkm_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_L_4H = new TH2D("h_mtptFSR_ewkm_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewkm_ST_P = new TH2D("h_mtptVR_ewkm_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_P = new TH2D("h_mtptVR_ewkm_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_ST_M = new TH2D("h_mtptVR_ewkm_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_M = new TH2D("h_mtptVR_ewkm_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_ST_L = new TH2D("h_mtptVR_ewkm_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_L = new TH2D("h_mtptVR_ewkm_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewkm_ST_P_23L = new TH2D("h_mtptVR_ewkm_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_P_23L = new TH2D("h_mtptVR_ewkm_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_ST_M_23L = new TH2D("h_mtptVR_ewkm_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_M_23L = new TH2D("h_mtptVR_ewkm_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_ST_L_23L = new TH2D("h_mtptVR_ewkm_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_L_23L = new TH2D("h_mtptVR_ewkm_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewkm_ST_P_23H = new TH2D("h_mtptVR_ewkm_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_P_23H = new TH2D("h_mtptVR_ewkm_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_ST_M_23H = new TH2D("h_mtptVR_ewkm_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_M_23H = new TH2D("h_mtptVR_ewkm_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_ST_L_23H = new TH2D("h_mtptVR_ewkm_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_L_23H = new TH2D("h_mtptVR_ewkm_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewkm_ST_P_4L = new TH2D("h_mtptVR_ewkm_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_P_4L = new TH2D("h_mtptVR_ewkm_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_ST_M_4L = new TH2D("h_mtptVR_ewkm_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_M_4L = new TH2D("h_mtptVR_ewkm_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_ST_L_4L = new TH2D("h_mtptVR_ewkm_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_L_4L = new TH2D("h_mtptVR_ewkm_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewkm_ST_P_4H = new TH2D("h_mtptVR_ewkm_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_P_4H = new TH2D("h_mtptVR_ewkm_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_ST_M_4H = new TH2D("h_mtptVR_ewkm_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_M_4H = new TH2D("h_mtptVR_ewkm_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_ST_L_4H = new TH2D("h_mtptVR_ewkm_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_L_4H = new TH2D("h_mtptVR_ewkm_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewkm_ST_P = new TH2D("h_mtptSR_ewkm_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_P = new TH2D("h_mtptSR_ewkm_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_ST_M = new TH2D("h_mtptSR_ewkm_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_M = new TH2D("h_mtptSR_ewkm_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_ST_L = new TH2D("h_mtptSR_ewkm_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_L = new TH2D("h_mtptSR_ewkm_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewkm_ST_P_23L = new TH2D("h_mtptSR_ewkm_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_P_23L = new TH2D("h_mtptSR_ewkm_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_ST_M_23L = new TH2D("h_mtptSR_ewkm_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_M_23L = new TH2D("h_mtptSR_ewkm_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_ST_L_23L = new TH2D("h_mtptSR_ewkm_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_L_23L = new TH2D("h_mtptSR_ewkm_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewkm_ST_P_23H = new TH2D("h_mtptSR_ewkm_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_P_23H = new TH2D("h_mtptSR_ewkm_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_ST_M_23H = new TH2D("h_mtptSR_ewkm_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_M_23H = new TH2D("h_mtptSR_ewkm_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_ST_L_23H = new TH2D("h_mtptSR_ewkm_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_L_23H = new TH2D("h_mtptSR_ewkm_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewkm_ST_P_4L = new TH2D("h_mtptSR_ewkm_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_P_4L = new TH2D("h_mtptSR_ewkm_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_ST_M_4L = new TH2D("h_mtptSR_ewkm_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_M_4L = new TH2D("h_mtptSR_ewkm_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_ST_L_4L = new TH2D("h_mtptSR_ewkm_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_L_4L = new TH2D("h_mtptSR_ewkm_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewkm_ST_P_4H = new TH2D("h_mtptSR_ewkm_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_P_4H = new TH2D("h_mtptSR_ewkm_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_ST_M_4H = new TH2D("h_mtptSR_ewkm_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_M_4H = new TH2D("h_mtptSR_ewkm_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_ST_L_4H = new TH2D("h_mtptSR_ewkm_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_L_4H = new TH2D("h_mtptSR_ewkm_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  // Tau EWK mtpt
  TH2D* h_mtptFSR_tewke_ST_P = new TH2D("h_mtptFSR_tewke_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_P = new TH2D("h_mtptFSR_tewke_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_ST_M = new TH2D("h_mtptFSR_tewke_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_M = new TH2D("h_mtptFSR_tewke_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_ST_L = new TH2D("h_mtptFSR_tewke_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_L = new TH2D("h_mtptFSR_tewke_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewke_ST_P_23L = new TH2D("h_mtptFSR_tewke_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_P_23L = new TH2D("h_mtptFSR_tewke_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_ST_M_23L = new TH2D("h_mtptFSR_tewke_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_M_23L = new TH2D("h_mtptFSR_tewke_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_ST_L_23L = new TH2D("h_mtptFSR_tewke_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_L_23L = new TH2D("h_mtptFSR_tewke_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewke_ST_P_23H = new TH2D("h_mtptFSR_tewke_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_P_23H = new TH2D("h_mtptFSR_tewke_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_ST_M_23H = new TH2D("h_mtptFSR_tewke_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_M_23H = new TH2D("h_mtptFSR_tewke_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_ST_L_23H = new TH2D("h_mtptFSR_tewke_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_L_23H = new TH2D("h_mtptFSR_tewke_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewke_ST_P_4L = new TH2D("h_mtptFSR_tewke_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_P_4L = new TH2D("h_mtptFSR_tewke_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_ST_M_4L = new TH2D("h_mtptFSR_tewke_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_M_4L = new TH2D("h_mtptFSR_tewke_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_ST_L_4L = new TH2D("h_mtptFSR_tewke_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_L_4L = new TH2D("h_mtptFSR_tewke_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewke_ST_P_4H = new TH2D("h_mtptFSR_tewke_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_P_4H = new TH2D("h_mtptFSR_tewke_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_ST_M_4H = new TH2D("h_mtptFSR_tewke_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_M_4H = new TH2D("h_mtptFSR_tewke_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_ST_L_4H = new TH2D("h_mtptFSR_tewke_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_L_4H = new TH2D("h_mtptFSR_tewke_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewke_ST_P = new TH2D("h_mtptVR_tewke_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_P = new TH2D("h_mtptVR_tewke_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_ST_M = new TH2D("h_mtptVR_tewke_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_M = new TH2D("h_mtptVR_tewke_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_ST_L = new TH2D("h_mtptVR_tewke_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_L = new TH2D("h_mtptVR_tewke_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewke_ST_P_23L = new TH2D("h_mtptVR_tewke_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_P_23L = new TH2D("h_mtptVR_tewke_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_ST_M_23L = new TH2D("h_mtptVR_tewke_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_M_23L = new TH2D("h_mtptVR_tewke_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_ST_L_23L = new TH2D("h_mtptVR_tewke_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_L_23L = new TH2D("h_mtptVR_tewke_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewke_ST_P_23H = new TH2D("h_mtptVR_tewke_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_P_23H = new TH2D("h_mtptVR_tewke_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_ST_M_23H = new TH2D("h_mtptVR_tewke_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_M_23H = new TH2D("h_mtptVR_tewke_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_ST_L_23H = new TH2D("h_mtptVR_tewke_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_L_23H = new TH2D("h_mtptVR_tewke_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewke_ST_P_4L = new TH2D("h_mtptVR_tewke_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_P_4L = new TH2D("h_mtptVR_tewke_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_ST_M_4L = new TH2D("h_mtptVR_tewke_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_M_4L = new TH2D("h_mtptVR_tewke_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_ST_L_4L = new TH2D("h_mtptVR_tewke_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_L_4L = new TH2D("h_mtptVR_tewke_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewke_ST_P_4H = new TH2D("h_mtptVR_tewke_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_P_4H = new TH2D("h_mtptVR_tewke_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_ST_M_4H = new TH2D("h_mtptVR_tewke_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_M_4H = new TH2D("h_mtptVR_tewke_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_ST_L_4H = new TH2D("h_mtptVR_tewke_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_L_4H = new TH2D("h_mtptVR_tewke_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewke_ST_P = new TH2D("h_mtptSR_tewke_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_P = new TH2D("h_mtptSR_tewke_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_ST_M = new TH2D("h_mtptSR_tewke_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_M = new TH2D("h_mtptSR_tewke_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_ST_L = new TH2D("h_mtptSR_tewke_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_L = new TH2D("h_mtptSR_tewke_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewke_ST_P_23L = new TH2D("h_mtptSR_tewke_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_P_23L = new TH2D("h_mtptSR_tewke_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_ST_M_23L = new TH2D("h_mtptSR_tewke_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_M_23L = new TH2D("h_mtptSR_tewke_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_ST_L_23L = new TH2D("h_mtptSR_tewke_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_L_23L = new TH2D("h_mtptSR_tewke_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewke_ST_P_23H = new TH2D("h_mtptSR_tewke_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_P_23H = new TH2D("h_mtptSR_tewke_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_ST_M_23H = new TH2D("h_mtptSR_tewke_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_M_23H = new TH2D("h_mtptSR_tewke_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_ST_L_23H = new TH2D("h_mtptSR_tewke_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_L_23H = new TH2D("h_mtptSR_tewke_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewke_ST_P_4L = new TH2D("h_mtptSR_tewke_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_P_4L = new TH2D("h_mtptSR_tewke_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_ST_M_4L = new TH2D("h_mtptSR_tewke_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_M_4L = new TH2D("h_mtptSR_tewke_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_ST_L_4L = new TH2D("h_mtptSR_tewke_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_L_4L = new TH2D("h_mtptSR_tewke_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewke_ST_P_4H = new TH2D("h_mtptSR_tewke_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_P_4H = new TH2D("h_mtptSR_tewke_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_ST_M_4H = new TH2D("h_mtptSR_tewke_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_M_4H = new TH2D("h_mtptSR_tewke_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_ST_L_4H = new TH2D("h_mtptSR_tewke_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_L_4H = new TH2D("h_mtptSR_tewke_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  // Tau EWK mtpt
  TH2D* h_mtptFSR_tewkm_ST_P = new TH2D("h_mtptFSR_tewkm_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_P = new TH2D("h_mtptFSR_tewkm_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_ST_M = new TH2D("h_mtptFSR_tewkm_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_M = new TH2D("h_mtptFSR_tewkm_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_ST_L = new TH2D("h_mtptFSR_tewkm_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_L = new TH2D("h_mtptFSR_tewkm_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewkm_ST_P_23L = new TH2D("h_mtptFSR_tewkm_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_P_23L = new TH2D("h_mtptFSR_tewkm_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_ST_M_23L = new TH2D("h_mtptFSR_tewkm_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_M_23L = new TH2D("h_mtptFSR_tewkm_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_ST_L_23L = new TH2D("h_mtptFSR_tewkm_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_L_23L = new TH2D("h_mtptFSR_tewkm_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewkm_ST_P_23H = new TH2D("h_mtptFSR_tewkm_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_P_23H = new TH2D("h_mtptFSR_tewkm_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_ST_M_23H = new TH2D("h_mtptFSR_tewkm_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_M_23H = new TH2D("h_mtptFSR_tewkm_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_ST_L_23H = new TH2D("h_mtptFSR_tewkm_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_L_23H = new TH2D("h_mtptFSR_tewkm_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewkm_ST_P_4L = new TH2D("h_mtptFSR_tewkm_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_P_4L = new TH2D("h_mtptFSR_tewkm_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_ST_M_4L = new TH2D("h_mtptFSR_tewkm_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_M_4L = new TH2D("h_mtptFSR_tewkm_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_ST_L_4L = new TH2D("h_mtptFSR_tewkm_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_L_4L = new TH2D("h_mtptFSR_tewkm_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewkm_ST_P_4H = new TH2D("h_mtptFSR_tewkm_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_P_4H = new TH2D("h_mtptFSR_tewkm_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_ST_M_4H = new TH2D("h_mtptFSR_tewkm_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_M_4H = new TH2D("h_mtptFSR_tewkm_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_ST_L_4H = new TH2D("h_mtptFSR_tewkm_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_L_4H = new TH2D("h_mtptFSR_tewkm_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewkm_ST_P = new TH2D("h_mtptVR_tewkm_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_P = new TH2D("h_mtptVR_tewkm_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_ST_M = new TH2D("h_mtptVR_tewkm_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_M = new TH2D("h_mtptVR_tewkm_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_ST_L = new TH2D("h_mtptVR_tewkm_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_L = new TH2D("h_mtptVR_tewkm_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewkm_ST_P_23L = new TH2D("h_mtptVR_tewkm_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_P_23L = new TH2D("h_mtptVR_tewkm_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_ST_M_23L = new TH2D("h_mtptVR_tewkm_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_M_23L = new TH2D("h_mtptVR_tewkm_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_ST_L_23L = new TH2D("h_mtptVR_tewkm_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_L_23L = new TH2D("h_mtptVR_tewkm_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewkm_ST_P_23H = new TH2D("h_mtptVR_tewkm_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_P_23H = new TH2D("h_mtptVR_tewkm_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_ST_M_23H = new TH2D("h_mtptVR_tewkm_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_M_23H = new TH2D("h_mtptVR_tewkm_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_ST_L_23H = new TH2D("h_mtptVR_tewkm_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_L_23H = new TH2D("h_mtptVR_tewkm_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewkm_ST_P_4L = new TH2D("h_mtptVR_tewkm_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_P_4L = new TH2D("h_mtptVR_tewkm_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_ST_M_4L = new TH2D("h_mtptVR_tewkm_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_M_4L = new TH2D("h_mtptVR_tewkm_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_ST_L_4L = new TH2D("h_mtptVR_tewkm_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_L_4L = new TH2D("h_mtptVR_tewkm_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewkm_ST_P_4H = new TH2D("h_mtptVR_tewkm_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_P_4H = new TH2D("h_mtptVR_tewkm_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_ST_M_4H = new TH2D("h_mtptVR_tewkm_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_M_4H = new TH2D("h_mtptVR_tewkm_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_ST_L_4H = new TH2D("h_mtptVR_tewkm_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_L_4H = new TH2D("h_mtptVR_tewkm_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewkm_ST_P = new TH2D("h_mtptSR_tewkm_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_P = new TH2D("h_mtptSR_tewkm_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_ST_M = new TH2D("h_mtptSR_tewkm_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_M = new TH2D("h_mtptSR_tewkm_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_ST_L = new TH2D("h_mtptSR_tewkm_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_L = new TH2D("h_mtptSR_tewkm_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewkm_ST_P_23L = new TH2D("h_mtptSR_tewkm_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_P_23L = new TH2D("h_mtptSR_tewkm_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_ST_M_23L = new TH2D("h_mtptSR_tewkm_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_M_23L = new TH2D("h_mtptSR_tewkm_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_ST_L_23L = new TH2D("h_mtptSR_tewkm_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_L_23L = new TH2D("h_mtptSR_tewkm_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewkm_ST_P_23H = new TH2D("h_mtptSR_tewkm_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_P_23H = new TH2D("h_mtptSR_tewkm_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_ST_M_23H = new TH2D("h_mtptSR_tewkm_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_M_23H = new TH2D("h_mtptSR_tewkm_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_ST_L_23H = new TH2D("h_mtptSR_tewkm_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_L_23H = new TH2D("h_mtptSR_tewkm_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewkm_ST_P_4L = new TH2D("h_mtptSR_tewkm_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_P_4L = new TH2D("h_mtptSR_tewkm_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_ST_M_4L = new TH2D("h_mtptSR_tewkm_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_M_4L = new TH2D("h_mtptSR_tewkm_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_ST_L_4L = new TH2D("h_mtptSR_tewkm_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_L_4L = new TH2D("h_mtptSR_tewkm_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewkm_ST_P_4H = new TH2D("h_mtptSR_tewkm_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_P_4H = new TH2D("h_mtptSR_tewkm_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_ST_M_4H = new TH2D("h_mtptSR_tewkm_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_M_4H = new TH2D("h_mtptSR_tewkm_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_ST_L_4H = new TH2D("h_mtptSR_tewkm_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_L_4H = new TH2D("h_mtptSR_tewkm_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  // Non-EWK mtpt
  TH2D* h_mtptFSR_newk_ST_P = new TH2D("h_mtptFSR_newk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_P = new TH2D("h_mtptFSR_newk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_ST_M = new TH2D("h_mtptFSR_newk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_M = new TH2D("h_mtptFSR_newk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_ST_L = new TH2D("h_mtptFSR_newk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_L = new TH2D("h_mtptFSR_newk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_newk_ST_P_23L = new TH2D("h_mtptFSR_newk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_P_23L = new TH2D("h_mtptFSR_newk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_ST_M_23L = new TH2D("h_mtptFSR_newk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_M_23L = new TH2D("h_mtptFSR_newk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_ST_L_23L = new TH2D("h_mtptFSR_newk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_L_23L = new TH2D("h_mtptFSR_newk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_newk_ST_P_23H = new TH2D("h_mtptFSR_newk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_P_23H = new TH2D("h_mtptFSR_newk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_ST_M_23H = new TH2D("h_mtptFSR_newk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_M_23H = new TH2D("h_mtptFSR_newk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_ST_L_23H = new TH2D("h_mtptFSR_newk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_L_23H = new TH2D("h_mtptFSR_newk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_newk_ST_P_4L = new TH2D("h_mtptFSR_newk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_P_4L = new TH2D("h_mtptFSR_newk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_ST_M_4L = new TH2D("h_mtptFSR_newk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_M_4L = new TH2D("h_mtptFSR_newk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_ST_L_4L = new TH2D("h_mtptFSR_newk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_L_4L = new TH2D("h_mtptFSR_newk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_newk_ST_P_4H = new TH2D("h_mtptFSR_newk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_P_4H = new TH2D("h_mtptFSR_newk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_ST_M_4H = new TH2D("h_mtptFSR_newk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_M_4H = new TH2D("h_mtptFSR_newk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_ST_L_4H = new TH2D("h_mtptFSR_newk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_L_4H = new TH2D("h_mtptFSR_newk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_newk_ST_P = new TH2D("h_mtptVR_newk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_P = new TH2D("h_mtptVR_newk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_ST_M = new TH2D("h_mtptVR_newk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_M = new TH2D("h_mtptVR_newk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_ST_L = new TH2D("h_mtptVR_newk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_L = new TH2D("h_mtptVR_newk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_newk_ST_P_23L = new TH2D("h_mtptVR_newk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_P_23L = new TH2D("h_mtptVR_newk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_ST_M_23L = new TH2D("h_mtptVR_newk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_M_23L = new TH2D("h_mtptVR_newk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_ST_L_23L = new TH2D("h_mtptVR_newk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_L_23L = new TH2D("h_mtptVR_newk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_newk_ST_P_23H = new TH2D("h_mtptVR_newk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_P_23H = new TH2D("h_mtptVR_newk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_ST_M_23H = new TH2D("h_mtptVR_newk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_M_23H = new TH2D("h_mtptVR_newk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_ST_L_23H = new TH2D("h_mtptVR_newk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_L_23H = new TH2D("h_mtptVR_newk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_newk_ST_P_4L = new TH2D("h_mtptVR_newk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_P_4L = new TH2D("h_mtptVR_newk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_ST_M_4L = new TH2D("h_mtptVR_newk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_M_4L = new TH2D("h_mtptVR_newk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_ST_L_4L = new TH2D("h_mtptVR_newk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_L_4L = new TH2D("h_mtptVR_newk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_newk_ST_P_4H = new TH2D("h_mtptVR_newk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_P_4H = new TH2D("h_mtptVR_newk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_ST_M_4H = new TH2D("h_mtptVR_newk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_M_4H = new TH2D("h_mtptVR_newk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_ST_L_4H = new TH2D("h_mtptVR_newk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_L_4H = new TH2D("h_mtptVR_newk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_newk_ST_P = new TH2D("h_mtptSR_newk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_P = new TH2D("h_mtptSR_newk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_ST_M = new TH2D("h_mtptSR_newk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_M = new TH2D("h_mtptSR_newk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_ST_L = new TH2D("h_mtptSR_newk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_L = new TH2D("h_mtptSR_newk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_newk_ST_P_23L = new TH2D("h_mtptSR_newk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_P_23L = new TH2D("h_mtptSR_newk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_ST_M_23L = new TH2D("h_mtptSR_newk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_M_23L = new TH2D("h_mtptSR_newk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_ST_L_23L = new TH2D("h_mtptSR_newk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_L_23L = new TH2D("h_mtptSR_newk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_newk_ST_P_23H = new TH2D("h_mtptSR_newk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_P_23H = new TH2D("h_mtptSR_newk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_ST_M_23H = new TH2D("h_mtptSR_newk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_M_23H = new TH2D("h_mtptSR_newk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_ST_L_23H = new TH2D("h_mtptSR_newk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_L_23H = new TH2D("h_mtptSR_newk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_newk_ST_P_4L = new TH2D("h_mtptSR_newk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_P_4L = new TH2D("h_mtptSR_newk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_ST_M_4L = new TH2D("h_mtptSR_newk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_M_4L = new TH2D("h_mtptSR_newk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_ST_L_4L = new TH2D("h_mtptSR_newk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_L_4L = new TH2D("h_mtptSR_newk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_newk_ST_P_4H = new TH2D("h_mtptSR_newk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_P_4H = new TH2D("h_mtptSR_newk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_ST_M_4H = new TH2D("h_mtptSR_newk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_M_4H = new TH2D("h_mtptSR_newk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_ST_L_4H = new TH2D("h_mtptSR_newk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_L_4H = new TH2D("h_mtptSR_newk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  // 1-prong Non-EWK mtpt
  TH2D* h_mtptFSR_1newk_ST_P = new TH2D("h_mtptFSR_1newk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_P = new TH2D("h_mtptFSR_1newk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_ST_M = new TH2D("h_mtptFSR_1newk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_M = new TH2D("h_mtptFSR_1newk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_ST_L = new TH2D("h_mtptFSR_1newk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_L = new TH2D("h_mtptFSR_1newk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_1newk_ST_P_23L = new TH2D("h_mtptFSR_1newk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_P_23L = new TH2D("h_mtptFSR_1newk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_ST_M_23L = new TH2D("h_mtptFSR_1newk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_M_23L = new TH2D("h_mtptFSR_1newk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_ST_L_23L = new TH2D("h_mtptFSR_1newk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_L_23L = new TH2D("h_mtptFSR_1newk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_1newk_ST_P_23H = new TH2D("h_mtptFSR_1newk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_P_23H = new TH2D("h_mtptFSR_1newk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_ST_M_23H = new TH2D("h_mtptFSR_1newk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_M_23H = new TH2D("h_mtptFSR_1newk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_ST_L_23H = new TH2D("h_mtptFSR_1newk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_L_23H = new TH2D("h_mtptFSR_1newk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_1newk_ST_P_4L = new TH2D("h_mtptFSR_1newk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_P_4L = new TH2D("h_mtptFSR_1newk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_ST_M_4L = new TH2D("h_mtptFSR_1newk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_M_4L = new TH2D("h_mtptFSR_1newk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_ST_L_4L = new TH2D("h_mtptFSR_1newk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_L_4L = new TH2D("h_mtptFSR_1newk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_1newk_ST_P_4H = new TH2D("h_mtptFSR_1newk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_P_4H = new TH2D("h_mtptFSR_1newk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_ST_M_4H = new TH2D("h_mtptFSR_1newk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_M_4H = new TH2D("h_mtptFSR_1newk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_ST_L_4H = new TH2D("h_mtptFSR_1newk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_L_4H = new TH2D("h_mtptFSR_1newk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_1newk_ST_P = new TH2D("h_mtptVR_1newk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_P = new TH2D("h_mtptVR_1newk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_ST_M = new TH2D("h_mtptVR_1newk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_M = new TH2D("h_mtptVR_1newk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_ST_L = new TH2D("h_mtptVR_1newk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_L = new TH2D("h_mtptVR_1newk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_1newk_ST_P_23L = new TH2D("h_mtptVR_1newk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_P_23L = new TH2D("h_mtptVR_1newk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_ST_M_23L = new TH2D("h_mtptVR_1newk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_M_23L = new TH2D("h_mtptVR_1newk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_ST_L_23L = new TH2D("h_mtptVR_1newk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_L_23L = new TH2D("h_mtptVR_1newk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_1newk_ST_P_23H = new TH2D("h_mtptVR_1newk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_P_23H = new TH2D("h_mtptVR_1newk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_ST_M_23H = new TH2D("h_mtptVR_1newk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_M_23H = new TH2D("h_mtptVR_1newk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_ST_L_23H = new TH2D("h_mtptVR_1newk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_L_23H = new TH2D("h_mtptVR_1newk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_1newk_ST_P_4L = new TH2D("h_mtptVR_1newk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_P_4L = new TH2D("h_mtptVR_1newk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_ST_M_4L = new TH2D("h_mtptVR_1newk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_M_4L = new TH2D("h_mtptVR_1newk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_ST_L_4L = new TH2D("h_mtptVR_1newk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_L_4L = new TH2D("h_mtptVR_1newk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_1newk_ST_P_4H = new TH2D("h_mtptVR_1newk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_P_4H = new TH2D("h_mtptVR_1newk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_ST_M_4H = new TH2D("h_mtptVR_1newk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_M_4H = new TH2D("h_mtptVR_1newk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_ST_L_4H = new TH2D("h_mtptVR_1newk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_L_4H = new TH2D("h_mtptVR_1newk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_1newk_ST_P = new TH2D("h_mtptSR_1newk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_P = new TH2D("h_mtptSR_1newk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_ST_M = new TH2D("h_mtptSR_1newk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_M = new TH2D("h_mtptSR_1newk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_ST_L = new TH2D("h_mtptSR_1newk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_L = new TH2D("h_mtptSR_1newk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_1newk_ST_P_23L = new TH2D("h_mtptSR_1newk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_P_23L = new TH2D("h_mtptSR_1newk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_ST_M_23L = new TH2D("h_mtptSR_1newk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_M_23L = new TH2D("h_mtptSR_1newk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_ST_L_23L = new TH2D("h_mtptSR_1newk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_L_23L = new TH2D("h_mtptSR_1newk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_1newk_ST_P_23H = new TH2D("h_mtptSR_1newk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_P_23H = new TH2D("h_mtptSR_1newk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_ST_M_23H = new TH2D("h_mtptSR_1newk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_M_23H = new TH2D("h_mtptSR_1newk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_ST_L_23H = new TH2D("h_mtptSR_1newk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_L_23H = new TH2D("h_mtptSR_1newk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_1newk_ST_P_4L = new TH2D("h_mtptSR_1newk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_P_4L = new TH2D("h_mtptSR_1newk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_ST_M_4L = new TH2D("h_mtptSR_1newk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_M_4L = new TH2D("h_mtptSR_1newk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_ST_L_4L = new TH2D("h_mtptSR_1newk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_L_4L = new TH2D("h_mtptSR_1newk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_1newk_ST_P_4H = new TH2D("h_mtptSR_1newk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_P_4H = new TH2D("h_mtptSR_1newk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_ST_M_4H = new TH2D("h_mtptSR_1newk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_M_4H = new TH2D("h_mtptSR_1newk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_ST_L_4H = new TH2D("h_mtptSR_1newk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_L_4H = new TH2D("h_mtptSR_1newk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  // 3-prong Non-EWK mtpt
  TH2D* h_mtptFSR_3newk_ST_P = new TH2D("h_mtptFSR_3newk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_P = new TH2D("h_mtptFSR_3newk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_ST_M = new TH2D("h_mtptFSR_3newk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_M = new TH2D("h_mtptFSR_3newk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_ST_L = new TH2D("h_mtptFSR_3newk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_L = new TH2D("h_mtptFSR_3newk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_3newk_ST_P_23L = new TH2D("h_mtptFSR_3newk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_P_23L = new TH2D("h_mtptFSR_3newk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_ST_M_23L = new TH2D("h_mtptFSR_3newk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_M_23L = new TH2D("h_mtptFSR_3newk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_ST_L_23L = new TH2D("h_mtptFSR_3newk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_L_23L = new TH2D("h_mtptFSR_3newk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_3newk_ST_P_23H = new TH2D("h_mtptFSR_3newk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_P_23H = new TH2D("h_mtptFSR_3newk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_ST_M_23H = new TH2D("h_mtptFSR_3newk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_M_23H = new TH2D("h_mtptFSR_3newk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_ST_L_23H = new TH2D("h_mtptFSR_3newk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_L_23H = new TH2D("h_mtptFSR_3newk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_3newk_ST_P_4L = new TH2D("h_mtptFSR_3newk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_P_4L = new TH2D("h_mtptFSR_3newk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_ST_M_4L = new TH2D("h_mtptFSR_3newk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_M_4L = new TH2D("h_mtptFSR_3newk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_ST_L_4L = new TH2D("h_mtptFSR_3newk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_L_4L = new TH2D("h_mtptFSR_3newk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_3newk_ST_P_4H = new TH2D("h_mtptFSR_3newk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_P_4H = new TH2D("h_mtptFSR_3newk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_ST_M_4H = new TH2D("h_mtptFSR_3newk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_M_4H = new TH2D("h_mtptFSR_3newk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_ST_L_4H = new TH2D("h_mtptFSR_3newk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_L_4H = new TH2D("h_mtptFSR_3newk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_3newk_ST_P = new TH2D("h_mtptVR_3newk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_P = new TH2D("h_mtptVR_3newk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_ST_M = new TH2D("h_mtptVR_3newk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_M = new TH2D("h_mtptVR_3newk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_ST_L = new TH2D("h_mtptVR_3newk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_L = new TH2D("h_mtptVR_3newk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_3newk_ST_P_23L = new TH2D("h_mtptVR_3newk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_P_23L = new TH2D("h_mtptVR_3newk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_ST_M_23L = new TH2D("h_mtptVR_3newk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_M_23L = new TH2D("h_mtptVR_3newk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_ST_L_23L = new TH2D("h_mtptVR_3newk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_L_23L = new TH2D("h_mtptVR_3newk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_3newk_ST_P_23H = new TH2D("h_mtptVR_3newk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_P_23H = new TH2D("h_mtptVR_3newk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_ST_M_23H = new TH2D("h_mtptVR_3newk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_M_23H = new TH2D("h_mtptVR_3newk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_ST_L_23H = new TH2D("h_mtptVR_3newk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_L_23H = new TH2D("h_mtptVR_3newk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_3newk_ST_P_4L = new TH2D("h_mtptVR_3newk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_P_4L = new TH2D("h_mtptVR_3newk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_ST_M_4L = new TH2D("h_mtptVR_3newk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_M_4L = new TH2D("h_mtptVR_3newk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_ST_L_4L = new TH2D("h_mtptVR_3newk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_L_4L = new TH2D("h_mtptVR_3newk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_3newk_ST_P_4H = new TH2D("h_mtptVR_3newk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_P_4H = new TH2D("h_mtptVR_3newk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_ST_M_4H = new TH2D("h_mtptVR_3newk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_M_4H = new TH2D("h_mtptVR_3newk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_ST_L_4H = new TH2D("h_mtptVR_3newk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_L_4H = new TH2D("h_mtptVR_3newk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_3newk_ST_P = new TH2D("h_mtptSR_3newk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_P = new TH2D("h_mtptSR_3newk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_ST_M = new TH2D("h_mtptSR_3newk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_M = new TH2D("h_mtptSR_3newk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_ST_L = new TH2D("h_mtptSR_3newk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_L = new TH2D("h_mtptSR_3newk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_3newk_ST_P_23L = new TH2D("h_mtptSR_3newk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_P_23L = new TH2D("h_mtptSR_3newk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_ST_M_23L = new TH2D("h_mtptSR_3newk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_M_23L = new TH2D("h_mtptSR_3newk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_ST_L_23L = new TH2D("h_mtptSR_3newk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_L_23L = new TH2D("h_mtptSR_3newk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_3newk_ST_P_23H = new TH2D("h_mtptSR_3newk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_P_23H = new TH2D("h_mtptSR_3newk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_ST_M_23H = new TH2D("h_mtptSR_3newk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_M_23H = new TH2D("h_mtptSR_3newk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_ST_L_23H = new TH2D("h_mtptSR_3newk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_L_23H = new TH2D("h_mtptSR_3newk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_3newk_ST_P_4L = new TH2D("h_mtptSR_3newk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_P_4L = new TH2D("h_mtptSR_3newk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_ST_M_4L = new TH2D("h_mtptSR_3newk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_M_4L = new TH2D("h_mtptSR_3newk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_ST_L_4L = new TH2D("h_mtptSR_3newk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_L_4L = new TH2D("h_mtptSR_3newk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_3newk_ST_P_4H = new TH2D("h_mtptSR_3newk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_P_4H = new TH2D("h_mtptSR_3newk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_ST_M_4H = new TH2D("h_mtptSR_3newk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_M_4H = new TH2D("h_mtptSR_3newk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_ST_L_4H = new TH2D("h_mtptSR_3newk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_L_4H = new TH2D("h_mtptSR_3newk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  // mtpt for lostOuterHit = 1 tracks
  TH2D* h_mtptFSR_ST_1L = new TH2D("h_mtptFSR_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_1L = new TH2D("h_mtptFSR_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ST_1L_23L = new TH2D("h_mtptFSR_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_1L_23L = new TH2D("h_mtptFSR_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ST_1L_23H = new TH2D("h_mtptFSR_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_1L_23H = new TH2D("h_mtptFSR_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ST_1L_4L = new TH2D("h_mtptFSR_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_1L_4L = new TH2D("h_mtptFSR_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ST_1L_4H = new TH2D("h_mtptFSR_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_STC_1L_4H = new TH2D("h_mtptFSR_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ST_1L = new TH2D("h_mtptVR_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_1L = new TH2D("h_mtptVR_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ST_1L_23L = new TH2D("h_mtptVR_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_1L_23L = new TH2D("h_mtptVR_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ST_1L_23H = new TH2D("h_mtptVR_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_1L_23H = new TH2D("h_mtptVR_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ST_1L_4L = new TH2D("h_mtptVR_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_1L_4L = new TH2D("h_mtptVR_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ST_1L_4H = new TH2D("h_mtptVR_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_STC_1L_4H = new TH2D("h_mtptVR_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ST_1L = new TH2D("h_mtptSR_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_1L = new TH2D("h_mtptSR_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ST_1L_23L = new TH2D("h_mtptSR_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_1L_23L = new TH2D("h_mtptSR_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ST_1L_23H = new TH2D("h_mtptSR_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_1L_23H = new TH2D("h_mtptSR_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ST_1L_4L = new TH2D("h_mtptSR_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_1L_4L = new TH2D("h_mtptSR_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ST_1L_4H = new TH2D("h_mtptSR_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_STC_1L_4H = new TH2D("h_mtptSR_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  // EWK e
  TH2D* h_mtptFSR_ewke_ST_1L = new TH2D("h_mtptFSR_ewke_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_1L = new TH2D("h_mtptFSR_ewke_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewke_ST_1L_23L = new TH2D("h_mtptFSR_ewke_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_1L_23L = new TH2D("h_mtptFSR_ewke_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewke_ST_1L_23H = new TH2D("h_mtptFSR_ewke_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_1L_23H = new TH2D("h_mtptFSR_ewke_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewke_ST_1L_4L = new TH2D("h_mtptFSR_ewke_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_1L_4L = new TH2D("h_mtptFSR_ewke_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewke_ST_1L_4H = new TH2D("h_mtptFSR_ewke_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewke_STC_1L_4H = new TH2D("h_mtptFSR_ewke_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewke_ST_1L = new TH2D("h_mtptVR_ewke_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_1L = new TH2D("h_mtptVR_ewke_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewke_ST_1L_23L = new TH2D("h_mtptVR_ewke_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_1L_23L = new TH2D("h_mtptVR_ewke_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewke_ST_1L_23H = new TH2D("h_mtptVR_ewke_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_1L_23H = new TH2D("h_mtptVR_ewke_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewke_ST_1L_4L = new TH2D("h_mtptVR_ewke_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_1L_4L = new TH2D("h_mtptVR_ewke_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewke_ST_1L_4H = new TH2D("h_mtptVR_ewke_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewke_STC_1L_4H = new TH2D("h_mtptVR_ewke_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewke_ST_1L = new TH2D("h_mtptSR_ewke_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_1L = new TH2D("h_mtptSR_ewke_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewke_ST_1L_23L = new TH2D("h_mtptSR_ewke_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_1L_23L = new TH2D("h_mtptSR_ewke_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewke_ST_1L_23H = new TH2D("h_mtptSR_ewke_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_1L_23H = new TH2D("h_mtptSR_ewke_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewke_ST_1L_4L = new TH2D("h_mtptSR_ewke_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_1L_4L = new TH2D("h_mtptSR_ewke_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewke_ST_1L_4H = new TH2D("h_mtptSR_ewke_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewke_STC_1L_4H = new TH2D("h_mtptSR_ewke_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  // Tau EWKE e
  TH2D* h_mtptFSR_tewke_ST_1L = new TH2D("h_mtptFSR_tewke_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_1L = new TH2D("h_mtptFSR_tewke_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewke_ST_1L_23L = new TH2D("h_mtptFSR_tewke_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_1L_23L = new TH2D("h_mtptFSR_tewke_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewke_ST_1L_23H = new TH2D("h_mtptFSR_tewke_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_1L_23H = new TH2D("h_mtptFSR_tewke_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewke_ST_1L_4L = new TH2D("h_mtptFSR_tewke_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_1L_4L = new TH2D("h_mtptFSR_tewke_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewke_ST_1L_4H = new TH2D("h_mtptFSR_tewke_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewke_STC_1L_4H = new TH2D("h_mtptFSR_tewke_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewke_ST_1L = new TH2D("h_mtptVR_tewke_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_1L = new TH2D("h_mtptVR_tewke_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewke_ST_1L_23L = new TH2D("h_mtptVR_tewke_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_1L_23L = new TH2D("h_mtptVR_tewke_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewke_ST_1L_23H = new TH2D("h_mtptVR_tewke_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_1L_23H = new TH2D("h_mtptVR_tewke_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewke_ST_1L_4L = new TH2D("h_mtptVR_tewke_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_1L_4L = new TH2D("h_mtptVR_tewke_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewke_ST_1L_4H = new TH2D("h_mtptVR_tewke_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewke_STC_1L_4H = new TH2D("h_mtptVR_tewke_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewke_ST_1L = new TH2D("h_mtptSR_tewke_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_1L = new TH2D("h_mtptSR_tewke_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewke_ST_1L_23L = new TH2D("h_mtptSR_tewke_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_1L_23L = new TH2D("h_mtptSR_tewke_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewke_ST_1L_23H = new TH2D("h_mtptSR_tewke_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_1L_23H = new TH2D("h_mtptSR_tewke_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewke_ST_1L_4L = new TH2D("h_mtptSR_tewke_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_1L_4L = new TH2D("h_mtptSR_tewke_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewke_ST_1L_4H = new TH2D("h_mtptSR_tewke_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewke_STC_1L_4H = new TH2D("h_mtptSR_tewke_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  // EWK m
  TH2D* h_mtptFSR_ewkm_ST_1L = new TH2D("h_mtptFSR_ewkm_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_1L = new TH2D("h_mtptFSR_ewkm_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewkm_ST_1L_23L = new TH2D("h_mtptFSR_ewkm_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_1L_23L = new TH2D("h_mtptFSR_ewkm_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewkm_ST_1L_23H = new TH2D("h_mtptFSR_ewkm_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_1L_23H = new TH2D("h_mtptFSR_ewkm_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewkm_ST_1L_4L = new TH2D("h_mtptFSR_ewkm_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_1L_4L = new TH2D("h_mtptFSR_ewkm_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_ewkm_ST_1L_4H = new TH2D("h_mtptFSR_ewkm_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_ewkm_STC_1L_4H = new TH2D("h_mtptFSR_ewkm_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewkm_ST_1L = new TH2D("h_mtptVR_ewkm_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_1L = new TH2D("h_mtptVR_ewkm_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewkm_ST_1L_23L = new TH2D("h_mtptVR_ewkm_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_1L_23L = new TH2D("h_mtptVR_ewkm_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewkm_ST_1L_23H = new TH2D("h_mtptVR_ewkm_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_1L_23H = new TH2D("h_mtptVR_ewkm_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewkm_ST_1L_4L = new TH2D("h_mtptVR_ewkm_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_1L_4L = new TH2D("h_mtptVR_ewkm_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_ewkm_ST_1L_4H = new TH2D("h_mtptVR_ewkm_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_ewkm_STC_1L_4H = new TH2D("h_mtptVR_ewkm_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewkm_ST_1L = new TH2D("h_mtptSR_ewkm_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_1L = new TH2D("h_mtptSR_ewkm_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewkm_ST_1L_23L = new TH2D("h_mtptSR_ewkm_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_1L_23L = new TH2D("h_mtptSR_ewkm_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewkm_ST_1L_23H = new TH2D("h_mtptSR_ewkm_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_1L_23H = new TH2D("h_mtptSR_ewkm_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewkm_ST_1L_4L = new TH2D("h_mtptSR_ewkm_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_1L_4L = new TH2D("h_mtptSR_ewkm_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_ewkm_ST_1L_4H = new TH2D("h_mtptSR_ewkm_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_ewkm_STC_1L_4H = new TH2D("h_mtptSR_ewkm_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  // Tau EWKM m
  TH2D* h_mtptFSR_tewkm_ST_1L = new TH2D("h_mtptFSR_tewkm_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_1L = new TH2D("h_mtptFSR_tewkm_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewkm_ST_1L_23L = new TH2D("h_mtptFSR_tewkm_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_1L_23L = new TH2D("h_mtptFSR_tewkm_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewkm_ST_1L_23H = new TH2D("h_mtptFSR_tewkm_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_1L_23H = new TH2D("h_mtptFSR_tewkm_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewkm_ST_1L_4L = new TH2D("h_mtptFSR_tewkm_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_1L_4L = new TH2D("h_mtptFSR_tewkm_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_tewkm_ST_1L_4H = new TH2D("h_mtptFSR_tewkm_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_tewkm_STC_1L_4H = new TH2D("h_mtptFSR_tewkm_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewkm_ST_1L = new TH2D("h_mtptVR_tewkm_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_1L = new TH2D("h_mtptVR_tewkm_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewkm_ST_1L_23L = new TH2D("h_mtptVR_tewkm_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_1L_23L = new TH2D("h_mtptVR_tewkm_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewkm_ST_1L_23H = new TH2D("h_mtptVR_tewkm_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_1L_23H = new TH2D("h_mtptVR_tewkm_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewkm_ST_1L_4L = new TH2D("h_mtptVR_tewkm_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_1L_4L = new TH2D("h_mtptVR_tewkm_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_tewkm_ST_1L_4H = new TH2D("h_mtptVR_tewkm_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_tewkm_STC_1L_4H = new TH2D("h_mtptVR_tewkm_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewkm_ST_1L = new TH2D("h_mtptSR_tewkm_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_1L = new TH2D("h_mtptSR_tewkm_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewkm_ST_1L_23L = new TH2D("h_mtptSR_tewkm_ST_1L_23L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_1L_23L = new TH2D("h_mtptSR_tewkm_STC_1L_23L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewkm_ST_1L_23H = new TH2D("h_mtptSR_tewkm_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_1L_23H = new TH2D("h_mtptSR_tewkm_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewkm_ST_1L_4L = new TH2D("h_mtptSR_tewkm_ST_1L_4L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_1L_4L = new TH2D("h_mtptSR_tewkm_STC_1L_4L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_tewkm_ST_1L_4H = new TH2D("h_mtptSR_tewkm_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_tewkm_STC_1L_4H = new TH2D("h_mtptSR_tewkm_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  // Non-EWK mtpt
  TH2D* h_mtptFSR_newk_ST_1L = new TH2D("h_mtptFSR_newk_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_1L = new TH2D("h_mtptFSR_newk_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_newk_ST_1L_23L = new TH2D("h_mtptFSR_newk_ST_1L_231L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_1L_23L = new TH2D("h_mtptFSR_newk_STC_1L_231L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_newk_ST_1L_23H = new TH2D("h_mtptFSR_newk_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_1L_23H = new TH2D("h_mtptFSR_newk_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_newk_ST_1L_4L = new TH2D("h_mtptFSR_newk_ST_1L_41L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_1L_4L = new TH2D("h_mtptFSR_newk_STC_1L_41L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_newk_ST_1L_4H = new TH2D("h_mtptFSR_newk_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_newk_STC_1L_4H = new TH2D("h_mtptFSR_newk_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_newk_ST_1L = new TH2D("h_mtptVR_newk_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_1L = new TH2D("h_mtptVR_newk_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_newk_ST_1L_23L = new TH2D("h_mtptVR_newk_ST_1L_231L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_1L_23L = new TH2D("h_mtptVR_newk_STC_1L_231L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_newk_ST_1L_23H = new TH2D("h_mtptVR_newk_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_1L_23H = new TH2D("h_mtptVR_newk_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_newk_ST_1L_4L = new TH2D("h_mtptVR_newk_ST_1L_41L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_1L_4L = new TH2D("h_mtptVR_newk_STC_1L_41L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_newk_ST_1L_4H = new TH2D("h_mtptVR_newk_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_newk_STC_1L_4H = new TH2D("h_mtptVR_newk_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_newk_ST_1L = new TH2D("h_mtptSR_newk_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_1L = new TH2D("h_mtptSR_newk_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_newk_ST_1L_23L = new TH2D("h_mtptSR_newk_ST_1L_231L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_1L_23L = new TH2D("h_mtptSR_newk_STC_1L_231L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_newk_ST_1L_23H = new TH2D("h_mtptSR_newk_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_1L_23H = new TH2D("h_mtptSR_newk_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_newk_ST_1L_4L = new TH2D("h_mtptSR_newk_ST_1L_41L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_1L_4L = new TH2D("h_mtptSR_newk_STC_1L_41L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_newk_ST_1L_4H = new TH2D("h_mtptSR_newk_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_newk_STC_1L_4H = new TH2D("h_mtptSR_newk_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  // 1-prong mtpt
  TH2D* h_mtptFSR_1newk_ST_1L = new TH2D("h_mtptFSR_1newk_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_1L = new TH2D("h_mtptFSR_1newk_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_1newk_ST_1L_23L = new TH2D("h_mtptFSR_1newk_ST_1L_231L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_1L_23L = new TH2D("h_mtptFSR_1newk_STC_1L_231L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_1newk_ST_1L_23H = new TH2D("h_mtptFSR_1newk_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_1L_23H = new TH2D("h_mtptFSR_1newk_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_1newk_ST_1L_4L = new TH2D("h_mtptFSR_1newk_ST_1L_41L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_1L_4L = new TH2D("h_mtptFSR_1newk_STC_1L_41L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_1newk_ST_1L_4H = new TH2D("h_mtptFSR_1newk_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_1newk_STC_1L_4H = new TH2D("h_mtptFSR_1newk_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_1newk_ST_1L = new TH2D("h_mtptVR_1newk_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_1L = new TH2D("h_mtptVR_1newk_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_1newk_ST_1L_23L = new TH2D("h_mtptVR_1newk_ST_1L_231L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_1L_23L = new TH2D("h_mtptVR_1newk_STC_1L_231L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_1newk_ST_1L_23H = new TH2D("h_mtptVR_1newk_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_1L_23H = new TH2D("h_mtptVR_1newk_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_1newk_ST_1L_4L = new TH2D("h_mtptVR_1newk_ST_1L_41L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_1L_4L = new TH2D("h_mtptVR_1newk_STC_1L_41L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_1newk_ST_1L_4H = new TH2D("h_mtptVR_1newk_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_1newk_STC_1L_4H = new TH2D("h_mtptVR_1newk_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_1newk_ST_1L = new TH2D("h_mtptSR_1newk_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_1L = new TH2D("h_mtptSR_1newk_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_1newk_ST_1L_23L = new TH2D("h_mtptSR_1newk_ST_1L_231L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_1L_23L = new TH2D("h_mtptSR_1newk_STC_1L_231L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_1newk_ST_1L_23H = new TH2D("h_mtptSR_1newk_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_1L_23H = new TH2D("h_mtptSR_1newk_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_1newk_ST_1L_4L = new TH2D("h_mtptSR_1newk_ST_1L_41L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_1L_4L = new TH2D("h_mtptSR_1newk_STC_1L_41L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_1newk_ST_1L_4H = new TH2D("h_mtptSR_1newk_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_1newk_STC_1L_4H = new TH2D("h_mtptSR_1newk_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  // 3-prong mtpt
  TH2D* h_mtptFSR_3newk_ST_1L = new TH2D("h_mtptFSR_3newk_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_1L = new TH2D("h_mtptFSR_3newk_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_3newk_ST_1L_23L = new TH2D("h_mtptFSR_3newk_ST_1L_231L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_1L_23L = new TH2D("h_mtptFSR_3newk_STC_1L_231L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_3newk_ST_1L_23H = new TH2D("h_mtptFSR_3newk_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_1L_23H = new TH2D("h_mtptFSR_3newk_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_3newk_ST_1L_4L = new TH2D("h_mtptFSR_3newk_ST_1L_41L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_1L_4L = new TH2D("h_mtptFSR_3newk_STC_1L_41L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptFSR_3newk_ST_1L_4H = new TH2D("h_mtptFSR_3newk_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptFSR_3newk_STC_1L_4H = new TH2D("h_mtptFSR_3newk_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_3newk_ST_1L = new TH2D("h_mtptVR_3newk_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_1L = new TH2D("h_mtptVR_3newk_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_3newk_ST_1L_23L = new TH2D("h_mtptVR_3newk_ST_1L_231L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_1L_23L = new TH2D("h_mtptVR_3newk_STC_1L_231L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_3newk_ST_1L_23H = new TH2D("h_mtptVR_3newk_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_1L_23H = new TH2D("h_mtptVR_3newk_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_3newk_ST_1L_4L = new TH2D("h_mtptVR_3newk_ST_1L_41L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_1L_4L = new TH2D("h_mtptVR_3newk_STC_1L_41L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptVR_3newk_ST_1L_4H = new TH2D("h_mtptVR_3newk_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptVR_3newk_STC_1L_4H = new TH2D("h_mtptVR_3newk_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_3newk_ST_1L = new TH2D("h_mtptSR_3newk_ST_1L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_1L = new TH2D("h_mtptSR_3newk_STC_1L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_3newk_ST_1L_23L = new TH2D("h_mtptSR_3newk_ST_1L_231L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_1L_23L = new TH2D("h_mtptSR_3newk_STC_1L_231L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_3newk_ST_1L_23H = new TH2D("h_mtptSR_3newk_ST_1L_23H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_1L_23H = new TH2D("h_mtptSR_3newk_STC_1L_23H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_3newk_ST_1L_4L = new TH2D("h_mtptSR_3newk_ST_1L_41L","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_1L_4L = new TH2D("h_mtptSR_3newk_STC_1L_41L","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  TH2D* h_mtptSR_3newk_ST_1L_4H = new TH2D("h_mtptSR_3newk_ST_1L_4H","p_{T} x M_{T}(ST,MET) of 1L STs",10,0,200,10,0,500);
  TH2D* h_mtptSR_3newk_STC_1L_4H = new TH2D("h_mtptSR_3newk_STC_1L_4H","p_{T} x M_{T}(STC,MET) of 1L STCs",10,0,200,10,0,500);

  // Zll hists 
  TH2D* h_zll_mtptFSR_ST_P = new TH2D("h_zll_mtptFSR_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_P = new TH2D("h_zll_mtptFSR_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ST_M = new TH2D("h_zll_mtptFSR_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_M = new TH2D("h_zll_mtptFSR_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ST_L = new TH2D("h_zll_mtptFSR_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_L = new TH2D("h_zll_mtptFSR_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptFSR_ST_P_23L = new TH2D("h_zll_mtptFSR_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_P_23L = new TH2D("h_zll_mtptFSR_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ST_M_23L = new TH2D("h_zll_mtptFSR_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_M_23L = new TH2D("h_zll_mtptFSR_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ST_L_23L = new TH2D("h_zll_mtptFSR_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_L_23L = new TH2D("h_zll_mtptFSR_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptFSR_ST_P_23H = new TH2D("h_zll_mtptFSR_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_P_23H = new TH2D("h_zll_mtptFSR_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ST_M_23H = new TH2D("h_zll_mtptFSR_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_M_23H = new TH2D("h_zll_mtptFSR_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ST_L_23H = new TH2D("h_zll_mtptFSR_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_L_23H = new TH2D("h_zll_mtptFSR_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptFSR_ST_P_4L = new TH2D("h_zll_mtptFSR_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_P_4L = new TH2D("h_zll_mtptFSR_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ST_M_4L = new TH2D("h_zll_mtptFSR_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_M_4L = new TH2D("h_zll_mtptFSR_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ST_L_4L = new TH2D("h_zll_mtptFSR_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_L_4L = new TH2D("h_zll_mtptFSR_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptFSR_ST_P_4H = new TH2D("h_zll_mtptFSR_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_P_4H = new TH2D("h_zll_mtptFSR_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ST_M_4H = new TH2D("h_zll_mtptFSR_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_M_4H = new TH2D("h_zll_mtptFSR_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ST_L_4H = new TH2D("h_zll_mtptFSR_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_STC_L_4H = new TH2D("h_zll_mtptFSR_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_ST_P = new TH2D("h_zll_mtptVR_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_P = new TH2D("h_zll_mtptVR_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ST_M = new TH2D("h_zll_mtptVR_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_M = new TH2D("h_zll_mtptVR_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ST_L = new TH2D("h_zll_mtptVR_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_L = new TH2D("h_zll_mtptVR_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_ST_P_23L = new TH2D("h_zll_mtptVR_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_P_23L = new TH2D("h_zll_mtptVR_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ST_M_23L = new TH2D("h_zll_mtptVR_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_M_23L = new TH2D("h_zll_mtptVR_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ST_L_23L = new TH2D("h_zll_mtptVR_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_L_23L = new TH2D("h_zll_mtptVR_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_ST_P_23H = new TH2D("h_zll_mtptVR_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_P_23H = new TH2D("h_zll_mtptVR_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ST_M_23H = new TH2D("h_zll_mtptVR_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_M_23H = new TH2D("h_zll_mtptVR_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ST_L_23H = new TH2D("h_zll_mtptVR_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_L_23H = new TH2D("h_zll_mtptVR_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_ST_P_4L = new TH2D("h_zll_mtptVR_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_P_4L = new TH2D("h_zll_mtptVR_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ST_M_4L = new TH2D("h_zll_mtptVR_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_M_4L = new TH2D("h_zll_mtptVR_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ST_L_4L = new TH2D("h_zll_mtptVR_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_L_4L = new TH2D("h_zll_mtptVR_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_ST_P_4H = new TH2D("h_zll_mtptVR_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_P_4H = new TH2D("h_zll_mtptVR_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ST_M_4H = new TH2D("h_zll_mtptVR_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_M_4H = new TH2D("h_zll_mtptVR_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ST_L_4H = new TH2D("h_zll_mtptVR_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_STC_L_4H = new TH2D("h_zll_mtptVR_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_ST_P = new TH2D("h_zll_mtptSR_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_P = new TH2D("h_zll_mtptSR_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ST_M = new TH2D("h_zll_mtptSR_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_M = new TH2D("h_zll_mtptSR_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ST_L = new TH2D("h_zll_mtptSR_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_L = new TH2D("h_zll_mtptSR_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_ST_P_23L = new TH2D("h_zll_mtptSR_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_P_23L = new TH2D("h_zll_mtptSR_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ST_M_23L = new TH2D("h_zll_mtptSR_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_M_23L = new TH2D("h_zll_mtptSR_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ST_L_23L = new TH2D("h_zll_mtptSR_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_L_23L = new TH2D("h_zll_mtptSR_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_ST_P_23H = new TH2D("h_zll_mtptSR_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_P_23H = new TH2D("h_zll_mtptSR_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ST_M_23H = new TH2D("h_zll_mtptSR_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_M_23H = new TH2D("h_zll_mtptSR_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ST_L_23H = new TH2D("h_zll_mtptSR_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_L_23H = new TH2D("h_zll_mtptSR_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_ST_P_4L = new TH2D("h_zll_mtptSR_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_P_4L = new TH2D("h_zll_mtptSR_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ST_M_4L = new TH2D("h_zll_mtptSR_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_M_4L = new TH2D("h_zll_mtptSR_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ST_L_4L = new TH2D("h_zll_mtptSR_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_L_4L = new TH2D("h_zll_mtptSR_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_ST_P_4H = new TH2D("h_zll_mtptSR_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_P_4H = new TH2D("h_zll_mtptSR_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ST_M_4H = new TH2D("h_zll_mtptSR_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_M_4H = new TH2D("h_zll_mtptSR_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ST_L_4H = new TH2D("h_zll_mtptSR_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_STC_L_4H = new TH2D("h_zll_mtptSR_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  // EWK mtpt
  TH2D* h_zll_mtptFSR_ewk_ST_P = new TH2D("h_zll_mtptFSR_ewk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_P = new TH2D("h_zll_mtptFSR_ewk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_ST_M = new TH2D("h_zll_mtptFSR_ewk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_M = new TH2D("h_zll_mtptFSR_ewk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_ST_L = new TH2D("h_zll_mtptFSR_ewk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_L = new TH2D("h_zll_mtptFSR_ewk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptFSR_ewk_ST_P_23L = new TH2D("h_zll_mtptFSR_ewk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_P_23L = new TH2D("h_zll_mtptFSR_ewk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_ST_M_23L = new TH2D("h_zll_mtptFSR_ewk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_M_23L = new TH2D("h_zll_mtptFSR_ewk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_ST_L_23L = new TH2D("h_zll_mtptFSR_ewk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_L_23L = new TH2D("h_zll_mtptFSR_ewk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptFSR_ewk_ST_P_23H = new TH2D("h_zll_mtptFSR_ewk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_P_23H = new TH2D("h_zll_mtptFSR_ewk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_ST_M_23H = new TH2D("h_zll_mtptFSR_ewk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_M_23H = new TH2D("h_zll_mtptFSR_ewk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_ST_L_23H = new TH2D("h_zll_mtptFSR_ewk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_L_23H = new TH2D("h_zll_mtptFSR_ewk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptFSR_ewk_ST_P_4L = new TH2D("h_zll_mtptFSR_ewk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_P_4L = new TH2D("h_zll_mtptFSR_ewk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_ST_M_4L = new TH2D("h_zll_mtptFSR_ewk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_M_4L = new TH2D("h_zll_mtptFSR_ewk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_ST_L_4L = new TH2D("h_zll_mtptFSR_ewk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_L_4L = new TH2D("h_zll_mtptFSR_ewk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptFSR_ewk_ST_P_4H = new TH2D("h_zll_mtptFSR_ewk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_P_4H = new TH2D("h_zll_mtptFSR_ewk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_ST_M_4H = new TH2D("h_zll_mtptFSR_ewk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_M_4H = new TH2D("h_zll_mtptFSR_ewk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_ST_L_4H = new TH2D("h_zll_mtptFSR_ewk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_ewk_STC_L_4H = new TH2D("h_zll_mtptFSR_ewk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_ewk_ST_P = new TH2D("h_zll_mtptVR_ewk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_P = new TH2D("h_zll_mtptVR_ewk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_ST_M = new TH2D("h_zll_mtptVR_ewk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_M = new TH2D("h_zll_mtptVR_ewk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_ST_L = new TH2D("h_zll_mtptVR_ewk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_L = new TH2D("h_zll_mtptVR_ewk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_ewk_ST_P_23L = new TH2D("h_zll_mtptVR_ewk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_P_23L = new TH2D("h_zll_mtptVR_ewk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_ST_M_23L = new TH2D("h_zll_mtptVR_ewk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_M_23L = new TH2D("h_zll_mtptVR_ewk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_ST_L_23L = new TH2D("h_zll_mtptVR_ewk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_L_23L = new TH2D("h_zll_mtptVR_ewk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_ewk_ST_P_23H = new TH2D("h_zll_mtptVR_ewk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_P_23H = new TH2D("h_zll_mtptVR_ewk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_ST_M_23H = new TH2D("h_zll_mtptVR_ewk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_M_23H = new TH2D("h_zll_mtptVR_ewk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_ST_L_23H = new TH2D("h_zll_mtptVR_ewk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_L_23H = new TH2D("h_zll_mtptVR_ewk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_ewk_ST_P_4L = new TH2D("h_zll_mtptVR_ewk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_P_4L = new TH2D("h_zll_mtptVR_ewk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_ST_M_4L = new TH2D("h_zll_mtptVR_ewk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_M_4L = new TH2D("h_zll_mtptVR_ewk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_ST_L_4L = new TH2D("h_zll_mtptVR_ewk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_L_4L = new TH2D("h_zll_mtptVR_ewk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_ewk_ST_P_4H = new TH2D("h_zll_mtptVR_ewk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_P_4H = new TH2D("h_zll_mtptVR_ewk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_ST_M_4H = new TH2D("h_zll_mtptVR_ewk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_M_4H = new TH2D("h_zll_mtptVR_ewk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_ST_L_4H = new TH2D("h_zll_mtptVR_ewk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_ewk_STC_L_4H = new TH2D("h_zll_mtptVR_ewk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_ewk_ST_P = new TH2D("h_zll_mtptSR_ewk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_P = new TH2D("h_zll_mtptSR_ewk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_ST_M = new TH2D("h_zll_mtptSR_ewk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_M = new TH2D("h_zll_mtptSR_ewk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_ST_L = new TH2D("h_zll_mtptSR_ewk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_L = new TH2D("h_zll_mtptSR_ewk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_ewk_ST_P_23L = new TH2D("h_zll_mtptSR_ewk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_P_23L = new TH2D("h_zll_mtptSR_ewk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_ST_M_23L = new TH2D("h_zll_mtptSR_ewk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_M_23L = new TH2D("h_zll_mtptSR_ewk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_ST_L_23L = new TH2D("h_zll_mtptSR_ewk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_L_23L = new TH2D("h_zll_mtptSR_ewk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_ewk_ST_P_23H = new TH2D("h_zll_mtptSR_ewk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_P_23H = new TH2D("h_zll_mtptSR_ewk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_ST_M_23H = new TH2D("h_zll_mtptSR_ewk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_M_23H = new TH2D("h_zll_mtptSR_ewk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_ST_L_23H = new TH2D("h_zll_mtptSR_ewk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_L_23H = new TH2D("h_zll_mtptSR_ewk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_ewk_ST_P_4L = new TH2D("h_zll_mtptSR_ewk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_P_4L = new TH2D("h_zll_mtptSR_ewk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_ST_M_4L = new TH2D("h_zll_mtptSR_ewk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_M_4L = new TH2D("h_zll_mtptSR_ewk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_ST_L_4L = new TH2D("h_zll_mtptSR_ewk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_L_4L = new TH2D("h_zll_mtptSR_ewk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_ewk_ST_P_4H = new TH2D("h_zll_mtptSR_ewk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_P_4H = new TH2D("h_zll_mtptSR_ewk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_ST_M_4H = new TH2D("h_zll_mtptSR_ewk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_M_4H = new TH2D("h_zll_mtptSR_ewk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_ST_L_4H = new TH2D("h_zll_mtptSR_ewk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_ewk_STC_L_4H = new TH2D("h_zll_mtptSR_ewk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  // Non-EWK mtpt
  TH2D* h_zll_mtptFSR_newk_ST_P = new TH2D("h_zll_mtptFSR_newk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_P = new TH2D("h_zll_mtptFSR_newk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_ST_M = new TH2D("h_zll_mtptFSR_newk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_M = new TH2D("h_zll_mtptFSR_newk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_ST_L = new TH2D("h_zll_mtptFSR_newk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_L = new TH2D("h_zll_mtptFSR_newk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptFSR_newk_ST_P_23L = new TH2D("h_zll_mtptFSR_newk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_P_23L = new TH2D("h_zll_mtptFSR_newk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_ST_M_23L = new TH2D("h_zll_mtptFSR_newk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_M_23L = new TH2D("h_zll_mtptFSR_newk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_ST_L_23L = new TH2D("h_zll_mtptFSR_newk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_L_23L = new TH2D("h_zll_mtptFSR_newk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptFSR_newk_ST_P_23H = new TH2D("h_zll_mtptFSR_newk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_P_23H = new TH2D("h_zll_mtptFSR_newk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_ST_M_23H = new TH2D("h_zll_mtptFSR_newk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_M_23H = new TH2D("h_zll_mtptFSR_newk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_ST_L_23H = new TH2D("h_zll_mtptFSR_newk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_L_23H = new TH2D("h_zll_mtptFSR_newk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptFSR_newk_ST_P_4L = new TH2D("h_zll_mtptFSR_newk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_P_4L = new TH2D("h_zll_mtptFSR_newk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_ST_M_4L = new TH2D("h_zll_mtptFSR_newk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_M_4L = new TH2D("h_zll_mtptFSR_newk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_ST_L_4L = new TH2D("h_zll_mtptFSR_newk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_L_4L = new TH2D("h_zll_mtptFSR_newk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptFSR_newk_ST_P_4H = new TH2D("h_zll_mtptFSR_newk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_P_4H = new TH2D("h_zll_mtptFSR_newk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_ST_M_4H = new TH2D("h_zll_mtptFSR_newk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_M_4H = new TH2D("h_zll_mtptFSR_newk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_ST_L_4H = new TH2D("h_zll_mtptFSR_newk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptFSR_newk_STC_L_4H = new TH2D("h_zll_mtptFSR_newk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_newk_ST_P = new TH2D("h_zll_mtptVR_newk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_P = new TH2D("h_zll_mtptVR_newk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_ST_M = new TH2D("h_zll_mtptVR_newk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_M = new TH2D("h_zll_mtptVR_newk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_ST_L = new TH2D("h_zll_mtptVR_newk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_L = new TH2D("h_zll_mtptVR_newk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_newk_ST_P_23L = new TH2D("h_zll_mtptVR_newk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_P_23L = new TH2D("h_zll_mtptVR_newk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_ST_M_23L = new TH2D("h_zll_mtptVR_newk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_M_23L = new TH2D("h_zll_mtptVR_newk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_ST_L_23L = new TH2D("h_zll_mtptVR_newk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_L_23L = new TH2D("h_zll_mtptVR_newk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_newk_ST_P_23H = new TH2D("h_zll_mtptVR_newk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_P_23H = new TH2D("h_zll_mtptVR_newk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_ST_M_23H = new TH2D("h_zll_mtptVR_newk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_M_23H = new TH2D("h_zll_mtptVR_newk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_ST_L_23H = new TH2D("h_zll_mtptVR_newk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_L_23H = new TH2D("h_zll_mtptVR_newk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_newk_ST_P_4L = new TH2D("h_zll_mtptVR_newk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_P_4L = new TH2D("h_zll_mtptVR_newk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_ST_M_4L = new TH2D("h_zll_mtptVR_newk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_M_4L = new TH2D("h_zll_mtptVR_newk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_ST_L_4L = new TH2D("h_zll_mtptVR_newk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_L_4L = new TH2D("h_zll_mtptVR_newk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptVR_newk_ST_P_4H = new TH2D("h_zll_mtptVR_newk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_P_4H = new TH2D("h_zll_mtptVR_newk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_ST_M_4H = new TH2D("h_zll_mtptVR_newk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_M_4H = new TH2D("h_zll_mtptVR_newk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_ST_L_4H = new TH2D("h_zll_mtptVR_newk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptVR_newk_STC_L_4H = new TH2D("h_zll_mtptVR_newk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_newk_ST_P = new TH2D("h_zll_mtptSR_newk_ST_P","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_P = new TH2D("h_zll_mtptSR_newk_STC_P","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_ST_M = new TH2D("h_zll_mtptSR_newk_ST_M","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_M = new TH2D("h_zll_mtptSR_newk_STC_M","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_ST_L = new TH2D("h_zll_mtptSR_newk_ST_L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_L = new TH2D("h_zll_mtptSR_newk_STC_L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_newk_ST_P_23L = new TH2D("h_zll_mtptSR_newk_ST_P_23L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_P_23L = new TH2D("h_zll_mtptSR_newk_STC_P_23L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_ST_M_23L = new TH2D("h_zll_mtptSR_newk_ST_M_23L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_M_23L = new TH2D("h_zll_mtptSR_newk_STC_M_23L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_ST_L_23L = new TH2D("h_zll_mtptSR_newk_ST_L_23L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_L_23L = new TH2D("h_zll_mtptSR_newk_STC_L_23L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_newk_ST_P_23H = new TH2D("h_zll_mtptSR_newk_ST_P_23H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_P_23H = new TH2D("h_zll_mtptSR_newk_STC_P_23H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_ST_M_23H = new TH2D("h_zll_mtptSR_newk_ST_M_23H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_M_23H = new TH2D("h_zll_mtptSR_newk_STC_M_23H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_ST_L_23H = new TH2D("h_zll_mtptSR_newk_ST_L_23H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_L_23H = new TH2D("h_zll_mtptSR_newk_STC_L_23H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_newk_ST_P_4L = new TH2D("h_zll_mtptSR_newk_ST_P_4L","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_P_4L = new TH2D("h_zll_mtptSR_newk_STC_P_4L","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_ST_M_4L = new TH2D("h_zll_mtptSR_newk_ST_M_4L","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_M_4L = new TH2D("h_zll_mtptSR_newk_STC_M_4L","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_ST_L_4L = new TH2D("h_zll_mtptSR_newk_ST_L_4L","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_L_4L = new TH2D("h_zll_mtptSR_newk_STC_L_4L","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

  TH2D* h_zll_mtptSR_newk_ST_P_4H = new TH2D("h_zll_mtptSR_newk_ST_P_4H","p_{T} x M_{T}(ST,MET) of P STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_P_4H = new TH2D("h_zll_mtptSR_newk_STC_P_4H","p_{T} x M_{T}(STC,MET) of P STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_ST_M_4H = new TH2D("h_zll_mtptSR_newk_ST_M_4H","p_{T} x M_{T}(ST,MET) of M STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_M_4H = new TH2D("h_zll_mtptSR_newk_STC_M_4H","p_{T} x M_{T}(STC,MET) of M STCs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_ST_L_4H = new TH2D("h_zll_mtptSR_newk_ST_L_4H","p_{T} x M_{T}(ST,MET) of L STs",10,0,200,10,0,500);
  TH2D* h_zll_mtptSR_newk_STC_L_4H = new TH2D("h_zll_mtptSR_newk_STC_L_4H","p_{T} x M_{T}(STC,MET) of L STCs",10,0,200,10,0,500);

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
  TH1D* h_mt2_ST_Lrej = new TH1D("h_mt2_ST_Lrej","MT2 of Rejected L Short Tracks",8,mt2bins);

  TH1D* h_mt2_STC_P = new TH1D("h_mt2_STC_P","MT2 of P STCs",8,mt2bins);
  TH1D* h_mt2_STC_M = new TH1D("h_mt2_STC_M","MT2 of M STCs",8,mt2bins);
  TH1D* h_mt2_STC_L = new TH1D("h_mt2_STC_L","MT2 of L STCs",8,mt2bins);
  TH1D* h_mt2_STC_Lrej = new TH1D("h_mt2_STC_Lrej","MT2 of Rejected L STCs",8,mt2bins);

  TH1D* h_mt2_ST_P_23 = new TH1D("h_mt2_ST_P_23","MT2 of P Short Tracks",8,mt2bins);
  TH1D* h_mt2_ST_M_23 = new TH1D("h_mt2_ST_M_23","MT2 of M Short Tracks",8,mt2bins);
  TH1D* h_mt2_ST_L_23 = new TH1D("h_mt2_ST_L_23","MT2 of L Short Tracks",8,mt2bins);
  TH1D* h_mt2_ST_Lrej_23 = new TH1D("h_mt2_ST_Lrej_23","MT2 of Rejected L Short Tracks",8,mt2bins);

  TH1D* h_mt2_STC_P_23 = new TH1D("h_mt2_STC_P_23","MT2 of P STCs",8,mt2bins);
  TH1D* h_mt2_STC_M_23 = new TH1D("h_mt2_STC_M_23","MT2 of M STCs",8,mt2bins);
  TH1D* h_mt2_STC_L_23 = new TH1D("h_mt2_STC_L_23","MT2 of L STCs",8,mt2bins);
  TH1D* h_mt2_STC_Lrej_23 = new TH1D("h_mt2_STC_Lrej_23","MT2 of Rejected L STCs",8,mt2bins);

  TH1D* h_mt2_ST_P_4 = new TH1D("h_mt2_ST_P_4","MT2 of P Short Tracks",8,mt2bins);
  TH1D* h_mt2_ST_M_4 = new TH1D("h_mt2_ST_M_4","MT2 of M Short Tracks",8,mt2bins);
  TH1D* h_mt2_ST_L_4 = new TH1D("h_mt2_ST_L_4","MT2 of L Short Tracks",8,mt2bins);
  TH1D* h_mt2_ST_Lrej_4 = new TH1D("h_mt2_ST_Lrej_4","MT2 of Rejected L Short Tracks",8,mt2bins);

  TH1D* h_mt2_STC_P_4 = new TH1D("h_mt2_STC_P_4","MT2 of P STCs",8,mt2bins);
  TH1D* h_mt2_STC_M_4 = new TH1D("h_mt2_STC_M_4","MT2 of M STCs",8,mt2bins);
  TH1D* h_mt2_STC_L_4 = new TH1D("h_mt2_STC_L_4","MT2 of L STCs",8,mt2bins);
  TH1D* h_mt2_STC_Lrej_4 = new TH1D("h_mt2_STC_Lrej_4","MT2 of Rejected L STCs",8,mt2bins);

  TH1D* h_dphiMet_ST_P = new TH1D("h_dphiMet_ST_P", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
  TH1D* h_dphiMet_ST_M = new TH1D("h_dphiMet_ST_M", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
  TH1D* h_dphiMet_ST_L = new TH1D("h_dphiMet_ST_L", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
  TH1D* h_dphiMet_ST_Lrej = new TH1D("h_dphiMet_ST_Lrej", "#Delta#phi(MET,ST)",10,0,TMath::Pi());

  TH1D* h_dphiMet_ST_P_23 = new TH1D("h_dphiMet_ST_P_23", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
  TH1D* h_dphiMet_ST_M_23 = new TH1D("h_dphiMet_ST_M_23", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
  TH1D* h_dphiMet_ST_L_23 = new TH1D("h_dphiMet_ST_L_23", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
  TH1D* h_dphiMet_ST_Lrej_23 = new TH1D("h_dphiMet_ST_Lrej_23", "#Delta#phi(MET,ST)",10,0,TMath::Pi());

  TH1D* h_dphiMet_ST_P_4 = new TH1D("h_dphiMet_ST_P_4", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
  TH1D* h_dphiMet_ST_M_4 = new TH1D("h_dphiMet_ST_M_4", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
  TH1D* h_dphiMet_ST_L_4 = new TH1D("h_dphiMet_ST_L_4", "#Delta#phi(MET,ST)",10,0,TMath::Pi());  
  TH1D* h_dphiMet_ST_Lrej_4 = new TH1D("h_dphiMet_ST_Lrej_4", "#Delta#phi(MET,ST)",10,0,TMath::Pi());  

  TH1D* h_dphiMet_STC_P = new TH1D("h_dphiMet_STC_P", "#Delta#phi(MET,STC)",10,0,TMath::Pi());
  TH1D* h_dphiMet_STC_M = new TH1D("h_dphiMet_STC_M", "#Delta#phi(MET,STC)",10,0,TMath::Pi());
  TH1D* h_dphiMet_STC_L = new TH1D("h_dphiMet_STC_L", "#Delta#phi(MET,STC)",10,0,TMath::Pi());
  TH1D* h_dphiMet_STC_Lrej = new TH1D("h_dphiMet_STC_Lrej", "#Delta#phi(MET,STC)",10,0,TMath::Pi());

  TH1D* h_dphiMet_STC_P_23 = new TH1D("h_dphiMet_STC_P_23", "#Delta#phi(MET,STC)",10,0,TMath::Pi());
  TH1D* h_dphiMet_STC_M_23 = new TH1D("h_dphiMet_STC_M_23", "#Delta#phi(MET,STC)",10,0,TMath::Pi());
  TH1D* h_dphiMet_STC_L_23 = new TH1D("h_dphiMet_STC_L_23", "#Delta#phi(MET,STC)",10,0,TMath::Pi());
  TH1D* h_dphiMet_STC_Lrej_23 = new TH1D("h_dphiMet_STC_Lrej_23", "#Delta#phi(MET,STC)",10,0,TMath::Pi());

  TH1D* h_dphiMet_STC_P_4 = new TH1D("h_dphiMet_STC_P_4", "#Delta#phi(MET,STC)",10,0,TMath::Pi());
  TH1D* h_dphiMet_STC_M_4 = new TH1D("h_dphiMet_STC_M_4", "#Delta#phi(MET,STC)",10,0,TMath::Pi());
  TH1D* h_dphiMet_STC_L_4 = new TH1D("h_dphiMet_STC_L_4", "#Delta#phi(MET,STC)",10,0,TMath::Pi());  
  TH1D* h_dphiMet_STC_Lrej_4 = new TH1D("h_dphiMet_STC_Lrej_4", "#Delta#phi(MET,STC)",10,0,TMath::Pi());  

  std::vector<TH1D*> nbhists = {h_nb_ST_L_30, h_nb_ST_L_100, h_nb_ST_L_200, h_nb_ST_L_250, h_nb_ST_L_300};
  std::vector<TH1D*> rlnbhists = {h_rlnb_ST_L_30, h_rlnb_ST_L_100, h_rlnb_ST_L_200, h_rlnb_ST_L_250, h_rlnb_ST_L_300};

  std::vector<TH2D*> mtpthists = { h_mtptFSR_ST_P, h_mtptFSR_ST_M, h_mtptFSR_ST_L, h_mtptFSR_ST_1L, h_mtptFSR_STC_P, h_mtptFSR_STC_M, h_mtptFSR_STC_L, h_mtptFSR_STC_1L,
				 h_mtptFSR_ST_P_23L, h_mtptFSR_ST_M_23L, h_mtptFSR_ST_L_23L, h_mtptFSR_ST_1L_23L, h_mtptFSR_ST_P_4L, h_mtptFSR_ST_M_4L, h_mtptFSR_ST_L_4L, h_mtptFSR_ST_1L_4L,
				 h_mtptFSR_ST_P_23H, h_mtptFSR_ST_M_23H, h_mtptFSR_ST_L_23H, h_mtptFSR_ST_1L_23H, h_mtptFSR_ST_P_4H, h_mtptFSR_ST_M_4H, h_mtptFSR_ST_L_4H, h_mtptFSR_ST_1L_4H,
				 h_mtptFSR_STC_P_23L, h_mtptFSR_STC_M_23L, h_mtptFSR_STC_L_23L, h_mtptFSR_STC_1L_23L, h_mtptFSR_STC_P_4L, h_mtptFSR_STC_M_4L, h_mtptFSR_STC_L_4L, h_mtptFSR_STC_1L_4L,
				 h_mtptFSR_STC_P_23H, h_mtptFSR_STC_M_23H, h_mtptFSR_STC_L_23H, h_mtptFSR_STC_1L_23H, h_mtptFSR_STC_P_4H, h_mtptFSR_STC_M_4H, h_mtptFSR_STC_L_4H, h_mtptFSR_STC_1L_4H,
				 h_mtptVR_ST_P, h_mtptVR_ST_M, h_mtptVR_ST_L, h_mtptVR_ST_1L, h_mtptVR_STC_P, h_mtptVR_STC_M, h_mtptVR_STC_L, h_mtptVR_STC_1L,
				 h_mtptVR_ST_P_23L, h_mtptVR_ST_M_23L, h_mtptVR_ST_L_23L, h_mtptVR_ST_1L_23L, h_mtptVR_ST_P_4L, h_mtptVR_ST_M_4L, h_mtptVR_ST_L_4L, h_mtptVR_ST_1L_4L,
				 h_mtptVR_ST_P_23H, h_mtptVR_ST_M_23H, h_mtptVR_ST_L_23H, h_mtptVR_ST_1L_23H, h_mtptVR_ST_P_4H, h_mtptVR_ST_M_4H, h_mtptVR_ST_L_4H, h_mtptVR_ST_1L_4H,
				 h_mtptVR_STC_P_23L, h_mtptVR_STC_M_23L, h_mtptVR_STC_L_23L, h_mtptVR_STC_1L_23L, h_mtptVR_STC_P_4L, h_mtptVR_STC_M_4L, h_mtptVR_STC_L_4L, h_mtptVR_STC_1L_4L,
				 h_mtptVR_STC_P_23H, h_mtptVR_STC_M_23H, h_mtptVR_STC_L_23H, h_mtptVR_STC_1L_23H, h_mtptVR_STC_P_4H, h_mtptVR_STC_M_4H, h_mtptVR_STC_L_4H, h_mtptVR_STC_1L_4H,
				 h_mtptSR_ST_P, h_mtptSR_ST_M, h_mtptSR_ST_L, h_mtptSR_ST_1L, h_mtptSR_STC_P, h_mtptSR_STC_M, h_mtptSR_STC_L, h_mtptSR_STC_1L,
				 h_mtptSR_ST_P_23L, h_mtptSR_ST_M_23L, h_mtptSR_ST_L_23L, h_mtptSR_ST_1L_23L, h_mtptSR_ST_P_4L, h_mtptSR_ST_M_4L, h_mtptSR_ST_L_4L, h_mtptSR_ST_1L_4L,
				 h_mtptSR_ST_P_23H, h_mtptSR_ST_M_23H, h_mtptSR_ST_L_23H, h_mtptSR_ST_1L_23H, h_mtptSR_ST_P_4H, h_mtptSR_ST_M_4H, h_mtptSR_ST_L_4H, h_mtptSR_ST_1L_4H,
				 h_mtptSR_STC_P_23L, h_mtptSR_STC_M_23L, h_mtptSR_STC_L_23L, h_mtptSR_STC_1L_23L, h_mtptSR_STC_P_4L, h_mtptSR_STC_M_4L, h_mtptSR_STC_L_4L, h_mtptSR_STC_1L_4L,
				 h_mtptSR_STC_P_23H, h_mtptSR_STC_M_23H, h_mtptSR_STC_L_23H, h_mtptSR_STC_1L_23H, h_mtptSR_STC_P_4H, h_mtptSR_STC_M_4H, h_mtptSR_STC_L_4H, h_mtptSR_STC_1L_4H};

  std::vector<TH2D*> mtptewkehists = { h_mtptFSR_ewke_ST_P, h_mtptFSR_ewke_ST_M, h_mtptFSR_ewke_ST_L, h_mtptFSR_ewke_ST_1L, h_mtptFSR_ewke_STC_P, h_mtptFSR_ewke_STC_M, h_mtptFSR_ewke_STC_L, h_mtptFSR_ewke_STC_1L,
				 h_mtptFSR_ewke_ST_P_23L, h_mtptFSR_ewke_ST_M_23L, h_mtptFSR_ewke_ST_L_23L, h_mtptFSR_ewke_ST_1L_23L, h_mtptFSR_ewke_ST_P_4L, h_mtptFSR_ewke_ST_M_4L, h_mtptFSR_ewke_ST_L_4L, h_mtptFSR_ewke_ST_1L_4L,
				 h_mtptFSR_ewke_ST_P_23H, h_mtptFSR_ewke_ST_M_23H, h_mtptFSR_ewke_ST_L_23H, h_mtptFSR_ewke_ST_1L_23H, h_mtptFSR_ewke_ST_P_4H, h_mtptFSR_ewke_ST_M_4H, h_mtptFSR_ewke_ST_L_4H, h_mtptFSR_ewke_ST_1L_4H,
				 h_mtptFSR_ewke_STC_P_23L, h_mtptFSR_ewke_STC_M_23L, h_mtptFSR_ewke_STC_L_23L, h_mtptFSR_ewke_STC_1L_23L, h_mtptFSR_ewke_STC_P_4L, h_mtptFSR_ewke_STC_M_4L, h_mtptFSR_ewke_STC_L_4L, h_mtptFSR_ewke_STC_1L_4L,
				 h_mtptFSR_ewke_STC_P_23H, h_mtptFSR_ewke_STC_M_23H, h_mtptFSR_ewke_STC_L_23H, h_mtptFSR_ewke_STC_1L_23H, h_mtptFSR_ewke_STC_P_4H, h_mtptFSR_ewke_STC_M_4H, h_mtptFSR_ewke_STC_L_4H, h_mtptFSR_ewke_STC_1L_4H,
				 h_mtptVR_ewke_ST_P, h_mtptVR_ewke_ST_M, h_mtptVR_ewke_ST_L, h_mtptVR_ewke_ST_1L, h_mtptVR_ewke_STC_P, h_mtptVR_ewke_STC_M, h_mtptVR_ewke_STC_L, h_mtptVR_ewke_STC_1L,
				 h_mtptVR_ewke_ST_P_23L, h_mtptVR_ewke_ST_M_23L, h_mtptVR_ewke_ST_L_23L, h_mtptVR_ewke_ST_1L_23L, h_mtptVR_ewke_ST_P_4L, h_mtptVR_ewke_ST_M_4L, h_mtptVR_ewke_ST_L_4L, h_mtptVR_ewke_ST_1L_4L,
				 h_mtptVR_ewke_ST_P_23H, h_mtptVR_ewke_ST_M_23H, h_mtptVR_ewke_ST_L_23H, h_mtptVR_ewke_ST_1L_23H, h_mtptVR_ewke_ST_P_4H, h_mtptVR_ewke_ST_M_4H, h_mtptVR_ewke_ST_L_4H, h_mtptVR_ewke_ST_1L_4H,
				 h_mtptVR_ewke_STC_P_23L, h_mtptVR_ewke_STC_M_23L, h_mtptVR_ewke_STC_L_23L, h_mtptVR_ewke_STC_1L_23L, h_mtptVR_ewke_STC_P_4L, h_mtptVR_ewke_STC_M_4L, h_mtptVR_ewke_STC_L_4L, h_mtptVR_ewke_STC_1L_4L,
				 h_mtptVR_ewke_STC_P_23H, h_mtptVR_ewke_STC_M_23H, h_mtptVR_ewke_STC_L_23H, h_mtptVR_ewke_STC_1L_23H, h_mtptVR_ewke_STC_P_4H, h_mtptVR_ewke_STC_M_4H, h_mtptVR_ewke_STC_L_4H, h_mtptVR_ewke_STC_1L_4H,
				 h_mtptSR_ewke_ST_P, h_mtptSR_ewke_ST_M, h_mtptSR_ewke_ST_L, h_mtptSR_ewke_ST_1L, h_mtptSR_ewke_STC_P, h_mtptSR_ewke_STC_M, h_mtptSR_ewke_STC_L, h_mtptSR_ewke_STC_1L,
				 h_mtptSR_ewke_ST_P_23L, h_mtptSR_ewke_ST_M_23L, h_mtptSR_ewke_ST_L_23L, h_mtptSR_ewke_ST_1L_23L, h_mtptSR_ewke_ST_P_4L, h_mtptSR_ewke_ST_M_4L, h_mtptSR_ewke_ST_L_4L, h_mtptSR_ewke_ST_1L_4L,
				 h_mtptSR_ewke_ST_P_23H, h_mtptSR_ewke_ST_M_23H, h_mtptSR_ewke_ST_L_23H, h_mtptSR_ewke_ST_1L_23H, h_mtptSR_ewke_ST_P_4H, h_mtptSR_ewke_ST_M_4H, h_mtptSR_ewke_ST_L_4H, h_mtptSR_ewke_ST_1L_4H,
				 h_mtptSR_ewke_STC_P_23L, h_mtptSR_ewke_STC_M_23L, h_mtptSR_ewke_STC_L_23L, h_mtptSR_ewke_STC_1L_23L, h_mtptSR_ewke_STC_P_4L, h_mtptSR_ewke_STC_M_4L, h_mtptSR_ewke_STC_L_4L, h_mtptSR_ewke_STC_1L_4L,
				 h_mtptSR_ewke_STC_P_23H, h_mtptSR_ewke_STC_M_23H, h_mtptSR_ewke_STC_L_23H, h_mtptSR_ewke_STC_1L_23H, h_mtptSR_ewke_STC_P_4H, h_mtptSR_ewke_STC_M_4H, h_mtptSR_ewke_STC_L_4H, h_mtptSR_ewke_STC_1L_4H};

  std::vector<TH2D*> mtptewkmhists = { h_mtptFSR_ewkm_ST_P, h_mtptFSR_ewkm_ST_M, h_mtptFSR_ewkm_ST_L, h_mtptFSR_ewkm_ST_1L, h_mtptFSR_ewkm_STC_P, h_mtptFSR_ewkm_STC_M, h_mtptFSR_ewkm_STC_L, h_mtptFSR_ewkm_STC_1L,
				 h_mtptFSR_ewkm_ST_P_23L, h_mtptFSR_ewkm_ST_M_23L, h_mtptFSR_ewkm_ST_L_23L, h_mtptFSR_ewkm_ST_1L_23L, h_mtptFSR_ewkm_ST_P_4L, h_mtptFSR_ewkm_ST_M_4L, h_mtptFSR_ewkm_ST_L_4L, h_mtptFSR_ewkm_ST_1L_4L,
				 h_mtptFSR_ewkm_ST_P_23H, h_mtptFSR_ewkm_ST_M_23H, h_mtptFSR_ewkm_ST_L_23H, h_mtptFSR_ewkm_ST_1L_23H, h_mtptFSR_ewkm_ST_P_4H, h_mtptFSR_ewkm_ST_M_4H, h_mtptFSR_ewkm_ST_L_4H, h_mtptFSR_ewkm_ST_1L_4H,
				 h_mtptFSR_ewkm_STC_P_23L, h_mtptFSR_ewkm_STC_M_23L, h_mtptFSR_ewkm_STC_L_23L, h_mtptFSR_ewkm_STC_1L_23L, h_mtptFSR_ewkm_STC_P_4L, h_mtptFSR_ewkm_STC_M_4L, h_mtptFSR_ewkm_STC_L_4L, h_mtptFSR_ewkm_STC_1L_4L,
				 h_mtptFSR_ewkm_STC_P_23H, h_mtptFSR_ewkm_STC_M_23H, h_mtptFSR_ewkm_STC_L_23H, h_mtptFSR_ewkm_STC_1L_23H, h_mtptFSR_ewkm_STC_P_4H, h_mtptFSR_ewkm_STC_M_4H, h_mtptFSR_ewkm_STC_L_4H, h_mtptFSR_ewkm_STC_1L_4H,
				 h_mtptVR_ewkm_ST_P, h_mtptVR_ewkm_ST_M, h_mtptVR_ewkm_ST_L, h_mtptVR_ewkm_ST_1L, h_mtptVR_ewkm_STC_P, h_mtptVR_ewkm_STC_M, h_mtptVR_ewkm_STC_L, h_mtptVR_ewkm_STC_1L,
				 h_mtptVR_ewkm_ST_P_23L, h_mtptVR_ewkm_ST_M_23L, h_mtptVR_ewkm_ST_L_23L, h_mtptVR_ewkm_ST_1L_23L, h_mtptVR_ewkm_ST_P_4L, h_mtptVR_ewkm_ST_M_4L, h_mtptVR_ewkm_ST_L_4L, h_mtptVR_ewkm_ST_1L_4L,
				 h_mtptVR_ewkm_ST_P_23H, h_mtptVR_ewkm_ST_M_23H, h_mtptVR_ewkm_ST_L_23H, h_mtptVR_ewkm_ST_1L_23H, h_mtptVR_ewkm_ST_P_4H, h_mtptVR_ewkm_ST_M_4H, h_mtptVR_ewkm_ST_L_4H, h_mtptVR_ewkm_ST_1L_4H,
				 h_mtptVR_ewkm_STC_P_23L, h_mtptVR_ewkm_STC_M_23L, h_mtptVR_ewkm_STC_L_23L, h_mtptVR_ewkm_STC_1L_23L, h_mtptVR_ewkm_STC_P_4L, h_mtptVR_ewkm_STC_M_4L, h_mtptVR_ewkm_STC_L_4L, h_mtptVR_ewkm_STC_1L_4L,
				 h_mtptVR_ewkm_STC_P_23H, h_mtptVR_ewkm_STC_M_23H, h_mtptVR_ewkm_STC_L_23H, h_mtptVR_ewkm_STC_1L_23H, h_mtptVR_ewkm_STC_P_4H, h_mtptVR_ewkm_STC_M_4H, h_mtptVR_ewkm_STC_L_4H, h_mtptVR_ewkm_STC_1L_4H,
				 h_mtptSR_ewkm_ST_P, h_mtptSR_ewkm_ST_M, h_mtptSR_ewkm_ST_L, h_mtptSR_ewkm_ST_1L, h_mtptSR_ewkm_STC_P, h_mtptSR_ewkm_STC_M, h_mtptSR_ewkm_STC_L, h_mtptSR_ewkm_STC_1L,
				 h_mtptSR_ewkm_ST_P_23L, h_mtptSR_ewkm_ST_M_23L, h_mtptSR_ewkm_ST_L_23L, h_mtptSR_ewkm_ST_1L_23L, h_mtptSR_ewkm_ST_P_4L, h_mtptSR_ewkm_ST_M_4L, h_mtptSR_ewkm_ST_L_4L, h_mtptSR_ewkm_ST_1L_4L,
				 h_mtptSR_ewkm_ST_P_23H, h_mtptSR_ewkm_ST_M_23H, h_mtptSR_ewkm_ST_L_23H, h_mtptSR_ewkm_ST_1L_23H, h_mtptSR_ewkm_ST_P_4H, h_mtptSR_ewkm_ST_M_4H, h_mtptSR_ewkm_ST_L_4H, h_mtptSR_ewkm_ST_1L_4H,
				 h_mtptSR_ewkm_STC_P_23L, h_mtptSR_ewkm_STC_M_23L, h_mtptSR_ewkm_STC_L_23L, h_mtptSR_ewkm_STC_1L_23L, h_mtptSR_ewkm_STC_P_4L, h_mtptSR_ewkm_STC_M_4L, h_mtptSR_ewkm_STC_L_4L, h_mtptSR_ewkm_STC_1L_4L,
				 h_mtptSR_ewkm_STC_P_23H, h_mtptSR_ewkm_STC_M_23H, h_mtptSR_ewkm_STC_L_23H, h_mtptSR_ewkm_STC_1L_23H, h_mtptSR_ewkm_STC_P_4H, h_mtptSR_ewkm_STC_M_4H, h_mtptSR_ewkm_STC_L_4H, h_mtptSR_ewkm_STC_1L_4H};

  std::vector<TH2D*> mtpttewkehists = { h_mtptFSR_tewke_ST_P, h_mtptFSR_tewke_ST_M, h_mtptFSR_tewke_ST_L, h_mtptFSR_tewke_ST_1L, h_mtptFSR_tewke_STC_P, h_mtptFSR_tewke_STC_M, h_mtptFSR_tewke_STC_L, h_mtptFSR_tewke_STC_1L,
				 h_mtptFSR_tewke_ST_P_23L, h_mtptFSR_tewke_ST_M_23L, h_mtptFSR_tewke_ST_L_23L, h_mtptFSR_tewke_ST_1L_23L, h_mtptFSR_tewke_ST_P_4L, h_mtptFSR_tewke_ST_M_4L, h_mtptFSR_tewke_ST_L_4L, h_mtptFSR_tewke_ST_1L_4L,
				 h_mtptFSR_tewke_ST_P_23H, h_mtptFSR_tewke_ST_M_23H, h_mtptFSR_tewke_ST_L_23H, h_mtptFSR_tewke_ST_1L_23H, h_mtptFSR_tewke_ST_P_4H, h_mtptFSR_tewke_ST_M_4H, h_mtptFSR_tewke_ST_L_4H, h_mtptFSR_tewke_ST_1L_4H,
				 h_mtptFSR_tewke_STC_P_23L, h_mtptFSR_tewke_STC_M_23L, h_mtptFSR_tewke_STC_L_23L, h_mtptFSR_tewke_STC_1L_23L, h_mtptFSR_tewke_STC_P_4L, h_mtptFSR_tewke_STC_M_4L, h_mtptFSR_tewke_STC_L_4L, h_mtptFSR_tewke_STC_1L_4L,
				 h_mtptFSR_tewke_STC_P_23H, h_mtptFSR_tewke_STC_M_23H, h_mtptFSR_tewke_STC_L_23H, h_mtptFSR_tewke_STC_1L_23H, h_mtptFSR_tewke_STC_P_4H, h_mtptFSR_tewke_STC_M_4H, h_mtptFSR_tewke_STC_L_4H, h_mtptFSR_tewke_STC_1L_4H,
				 h_mtptVR_tewke_ST_P, h_mtptVR_tewke_ST_M, h_mtptVR_tewke_ST_L, h_mtptVR_tewke_ST_1L, h_mtptVR_tewke_STC_P, h_mtptVR_tewke_STC_M, h_mtptVR_tewke_STC_L, h_mtptVR_tewke_STC_1L,
				 h_mtptVR_tewke_ST_P_23L, h_mtptVR_tewke_ST_M_23L, h_mtptVR_tewke_ST_L_23L, h_mtptVR_tewke_ST_1L_23L, h_mtptVR_tewke_ST_P_4L, h_mtptVR_tewke_ST_M_4L, h_mtptVR_tewke_ST_L_4L, h_mtptVR_tewke_ST_1L_4L,
				 h_mtptVR_tewke_ST_P_23H, h_mtptVR_tewke_ST_M_23H, h_mtptVR_tewke_ST_L_23H, h_mtptVR_tewke_ST_1L_23H, h_mtptVR_tewke_ST_P_4H, h_mtptVR_tewke_ST_M_4H, h_mtptVR_tewke_ST_L_4H, h_mtptVR_tewke_ST_1L_4H,
				 h_mtptVR_tewke_STC_P_23L, h_mtptVR_tewke_STC_M_23L, h_mtptVR_tewke_STC_L_23L, h_mtptVR_tewke_STC_1L_23L, h_mtptVR_tewke_STC_P_4L, h_mtptVR_tewke_STC_M_4L, h_mtptVR_tewke_STC_L_4L, h_mtptVR_tewke_STC_1L_4L,
				 h_mtptVR_tewke_STC_P_23H, h_mtptVR_tewke_STC_M_23H, h_mtptVR_tewke_STC_L_23H, h_mtptVR_tewke_STC_1L_23H, h_mtptVR_tewke_STC_P_4H, h_mtptVR_tewke_STC_M_4H, h_mtptVR_tewke_STC_L_4H, h_mtptVR_tewke_STC_1L_4H,
				 h_mtptSR_tewke_ST_P, h_mtptSR_tewke_ST_M, h_mtptSR_tewke_ST_L, h_mtptSR_tewke_ST_1L, h_mtptSR_tewke_STC_P, h_mtptSR_tewke_STC_M, h_mtptSR_tewke_STC_L, h_mtptSR_tewke_STC_1L,
				 h_mtptSR_tewke_ST_P_23L, h_mtptSR_tewke_ST_M_23L, h_mtptSR_tewke_ST_L_23L, h_mtptSR_tewke_ST_1L_23L, h_mtptSR_tewke_ST_P_4L, h_mtptSR_tewke_ST_M_4L, h_mtptSR_tewke_ST_L_4L, h_mtptSR_tewke_ST_1L_4L,
				 h_mtptSR_tewke_ST_P_23H, h_mtptSR_tewke_ST_M_23H, h_mtptSR_tewke_ST_L_23H, h_mtptSR_tewke_ST_1L_23H, h_mtptSR_tewke_ST_P_4H, h_mtptSR_tewke_ST_M_4H, h_mtptSR_tewke_ST_L_4H, h_mtptSR_tewke_ST_1L_4H,
				 h_mtptSR_tewke_STC_P_23L, h_mtptSR_tewke_STC_M_23L, h_mtptSR_tewke_STC_L_23L, h_mtptSR_tewke_STC_1L_23L, h_mtptSR_tewke_STC_P_4L, h_mtptSR_tewke_STC_M_4L, h_mtptSR_tewke_STC_L_4L, h_mtptSR_tewke_STC_1L_4L,
				 h_mtptSR_tewke_STC_P_23H, h_mtptSR_tewke_STC_M_23H, h_mtptSR_tewke_STC_L_23H, h_mtptSR_tewke_STC_1L_23H, h_mtptSR_tewke_STC_P_4H, h_mtptSR_tewke_STC_M_4H, h_mtptSR_tewke_STC_L_4H, h_mtptSR_tewke_STC_1L_4H};

  std::vector<TH2D*> mtpttewkmhists = { h_mtptFSR_tewkm_ST_P, h_mtptFSR_tewkm_ST_M, h_mtptFSR_tewkm_ST_L, h_mtptFSR_tewkm_ST_1L, h_mtptFSR_tewkm_STC_P, h_mtptFSR_tewkm_STC_M, h_mtptFSR_tewkm_STC_L, h_mtptFSR_tewkm_STC_1L,
				 h_mtptFSR_tewkm_ST_P_23L, h_mtptFSR_tewkm_ST_M_23L, h_mtptFSR_tewkm_ST_L_23L, h_mtptFSR_tewkm_ST_1L_23L, h_mtptFSR_tewkm_ST_P_4L, h_mtptFSR_tewkm_ST_M_4L, h_mtptFSR_tewkm_ST_L_4L, h_mtptFSR_tewkm_ST_1L_4L,
				 h_mtptFSR_tewkm_ST_P_23H, h_mtptFSR_tewkm_ST_M_23H, h_mtptFSR_tewkm_ST_L_23H, h_mtptFSR_tewkm_ST_1L_23H, h_mtptFSR_tewkm_ST_P_4H, h_mtptFSR_tewkm_ST_M_4H, h_mtptFSR_tewkm_ST_L_4H, h_mtptFSR_tewkm_ST_1L_4H,
				 h_mtptFSR_tewkm_STC_P_23L, h_mtptFSR_tewkm_STC_M_23L, h_mtptFSR_tewkm_STC_L_23L, h_mtptFSR_tewkm_STC_1L_23L, h_mtptFSR_tewkm_STC_P_4L, h_mtptFSR_tewkm_STC_M_4L, h_mtptFSR_tewkm_STC_L_4L, h_mtptFSR_tewkm_STC_1L_4L,
				 h_mtptFSR_tewkm_STC_P_23H, h_mtptFSR_tewkm_STC_M_23H, h_mtptFSR_tewkm_STC_L_23H, h_mtptFSR_tewkm_STC_1L_23H, h_mtptFSR_tewkm_STC_P_4H, h_mtptFSR_tewkm_STC_M_4H, h_mtptFSR_tewkm_STC_L_4H, h_mtptFSR_tewkm_STC_1L_4H,
				 h_mtptVR_tewkm_ST_P, h_mtptVR_tewkm_ST_M, h_mtptVR_tewkm_ST_L, h_mtptVR_tewkm_ST_1L, h_mtptVR_tewkm_STC_P, h_mtptVR_tewkm_STC_M, h_mtptVR_tewkm_STC_L, h_mtptVR_tewkm_STC_1L,
				 h_mtptVR_tewkm_ST_P_23L, h_mtptVR_tewkm_ST_M_23L, h_mtptVR_tewkm_ST_L_23L, h_mtptVR_tewkm_ST_1L_23L, h_mtptVR_tewkm_ST_P_4L, h_mtptVR_tewkm_ST_M_4L, h_mtptVR_tewkm_ST_L_4L, h_mtptVR_tewkm_ST_1L_4L,
				 h_mtptVR_tewkm_ST_P_23H, h_mtptVR_tewkm_ST_M_23H, h_mtptVR_tewkm_ST_L_23H, h_mtptVR_tewkm_ST_1L_23H, h_mtptVR_tewkm_ST_P_4H, h_mtptVR_tewkm_ST_M_4H, h_mtptVR_tewkm_ST_L_4H, h_mtptVR_tewkm_ST_1L_4H,
				 h_mtptVR_tewkm_STC_P_23L, h_mtptVR_tewkm_STC_M_23L, h_mtptVR_tewkm_STC_L_23L, h_mtptVR_tewkm_STC_1L_23L, h_mtptVR_tewkm_STC_P_4L, h_mtptVR_tewkm_STC_M_4L, h_mtptVR_tewkm_STC_L_4L, h_mtptVR_tewkm_STC_1L_4L,
				 h_mtptVR_tewkm_STC_P_23H, h_mtptVR_tewkm_STC_M_23H, h_mtptVR_tewkm_STC_L_23H, h_mtptVR_tewkm_STC_1L_23H, h_mtptVR_tewkm_STC_P_4H, h_mtptVR_tewkm_STC_M_4H, h_mtptVR_tewkm_STC_L_4H, h_mtptVR_tewkm_STC_1L_4H,
				 h_mtptSR_tewkm_ST_P, h_mtptSR_tewkm_ST_M, h_mtptSR_tewkm_ST_L, h_mtptSR_tewkm_ST_1L, h_mtptSR_tewkm_STC_P, h_mtptSR_tewkm_STC_M, h_mtptSR_tewkm_STC_L, h_mtptSR_tewkm_STC_1L,
				 h_mtptSR_tewkm_ST_P_23L, h_mtptSR_tewkm_ST_M_23L, h_mtptSR_tewkm_ST_L_23L, h_mtptSR_tewkm_ST_1L_23L, h_mtptSR_tewkm_ST_P_4L, h_mtptSR_tewkm_ST_M_4L, h_mtptSR_tewkm_ST_L_4L, h_mtptSR_tewkm_ST_1L_4L,
				 h_mtptSR_tewkm_ST_P_23H, h_mtptSR_tewkm_ST_M_23H, h_mtptSR_tewkm_ST_L_23H, h_mtptSR_tewkm_ST_1L_23H, h_mtptSR_tewkm_ST_P_4H, h_mtptSR_tewkm_ST_M_4H, h_mtptSR_tewkm_ST_L_4H, h_mtptSR_tewkm_ST_1L_4H,
				 h_mtptSR_tewkm_STC_P_23L, h_mtptSR_tewkm_STC_M_23L, h_mtptSR_tewkm_STC_L_23L, h_mtptSR_tewkm_STC_1L_23L, h_mtptSR_tewkm_STC_P_4L, h_mtptSR_tewkm_STC_M_4L, h_mtptSR_tewkm_STC_L_4L, h_mtptSR_tewkm_STC_1L_4L,
				 h_mtptSR_tewkm_STC_P_23H, h_mtptSR_tewkm_STC_M_23H, h_mtptSR_tewkm_STC_L_23H, h_mtptSR_tewkm_STC_1L_23H, h_mtptSR_tewkm_STC_P_4H, h_mtptSR_tewkm_STC_M_4H, h_mtptSR_tewkm_STC_L_4H, h_mtptSR_tewkm_STC_1L_4H};

  std::vector<TH2D*> mtptnewkhists = { h_mtptFSR_newk_ST_P, h_mtptFSR_newk_ST_M, h_mtptFSR_newk_ST_L, h_mtptFSR_newk_ST_1L, h_mtptFSR_newk_STC_P, h_mtptFSR_newk_STC_M, h_mtptFSR_newk_STC_L, h_mtptFSR_newk_STC_1L,
				 h_mtptFSR_newk_ST_P_23L, h_mtptFSR_newk_ST_M_23L, h_mtptFSR_newk_ST_L_23L, h_mtptFSR_newk_ST_1L_23L, h_mtptFSR_newk_ST_P_4L, h_mtptFSR_newk_ST_M_4L, h_mtptFSR_newk_ST_L_4L, h_mtptFSR_newk_ST_1L_4L,
				 h_mtptFSR_newk_ST_P_23H, h_mtptFSR_newk_ST_M_23H, h_mtptFSR_newk_ST_L_23H, h_mtptFSR_newk_ST_1L_23H, h_mtptFSR_newk_ST_P_4H, h_mtptFSR_newk_ST_M_4H, h_mtptFSR_newk_ST_L_4H, h_mtptFSR_newk_ST_1L_4H,
				 h_mtptFSR_newk_STC_P_23L, h_mtptFSR_newk_STC_M_23L, h_mtptFSR_newk_STC_L_23L, h_mtptFSR_newk_STC_1L_23L, h_mtptFSR_newk_STC_P_4L, h_mtptFSR_newk_STC_M_4L, h_mtptFSR_newk_STC_L_4L, h_mtptFSR_newk_STC_1L_4L,
				 h_mtptFSR_newk_STC_P_23H, h_mtptFSR_newk_STC_M_23H, h_mtptFSR_newk_STC_L_23H, h_mtptFSR_newk_STC_1L_23H, h_mtptFSR_newk_STC_P_4H, h_mtptFSR_newk_STC_M_4H, h_mtptFSR_newk_STC_L_4H, h_mtptFSR_newk_STC_1L_4H,
				 h_mtptVR_newk_ST_P, h_mtptVR_newk_ST_M, h_mtptVR_newk_ST_L, h_mtptVR_newk_ST_1L, h_mtptVR_newk_STC_P, h_mtptVR_newk_STC_M, h_mtptVR_newk_STC_L, h_mtptVR_newk_STC_1L,
				 h_mtptVR_newk_ST_P_23L, h_mtptVR_newk_ST_M_23L, h_mtptVR_newk_ST_L_23L, h_mtptVR_newk_ST_1L_23L, h_mtptVR_newk_ST_P_4L, h_mtptVR_newk_ST_M_4L, h_mtptVR_newk_ST_L_4L, h_mtptVR_newk_ST_1L_4L,
				 h_mtptVR_newk_ST_P_23H, h_mtptVR_newk_ST_M_23H, h_mtptVR_newk_ST_L_23H, h_mtptVR_newk_ST_1L_23H, h_mtptVR_newk_ST_P_4H, h_mtptVR_newk_ST_M_4H, h_mtptVR_newk_ST_L_4H, h_mtptVR_newk_ST_1L_4H,
				 h_mtptVR_newk_STC_P_23L, h_mtptVR_newk_STC_M_23L, h_mtptVR_newk_STC_L_23L, h_mtptVR_newk_STC_1L_23L, h_mtptVR_newk_STC_P_4L, h_mtptVR_newk_STC_M_4L, h_mtptVR_newk_STC_L_4L, h_mtptVR_newk_STC_1L_4L,
				 h_mtptVR_newk_STC_P_23H, h_mtptVR_newk_STC_M_23H, h_mtptVR_newk_STC_L_23H, h_mtptVR_newk_STC_1L_23H, h_mtptVR_newk_STC_P_4H, h_mtptVR_newk_STC_M_4H, h_mtptVR_newk_STC_L_4H, h_mtptVR_newk_STC_1L_4H,
				 h_mtptSR_newk_ST_P, h_mtptSR_newk_ST_M, h_mtptSR_newk_ST_L, h_mtptSR_newk_ST_1L, h_mtptSR_newk_STC_P, h_mtptSR_newk_STC_M, h_mtptSR_newk_STC_L, h_mtptSR_newk_STC_1L,
				 h_mtptSR_newk_ST_P_23L, h_mtptSR_newk_ST_M_23L, h_mtptSR_newk_ST_L_23L, h_mtptSR_newk_ST_1L_23L, h_mtptSR_newk_ST_P_4L, h_mtptSR_newk_ST_M_4L, h_mtptSR_newk_ST_L_4L, h_mtptSR_newk_ST_1L_4L,
				 h_mtptSR_newk_ST_P_23H, h_mtptSR_newk_ST_M_23H, h_mtptSR_newk_ST_L_23H, h_mtptSR_newk_ST_1L_23H, h_mtptSR_newk_ST_P_4H, h_mtptSR_newk_ST_M_4H, h_mtptSR_newk_ST_L_4H, h_mtptSR_newk_ST_1L_4H,
				 h_mtptSR_newk_STC_P_23L, h_mtptSR_newk_STC_M_23L, h_mtptSR_newk_STC_L_23L, h_mtptSR_newk_STC_1L_23L, h_mtptSR_newk_STC_P_4L, h_mtptSR_newk_STC_M_4L, h_mtptSR_newk_STC_L_4L, h_mtptSR_newk_STC_1L_4L,
				 h_mtptSR_newk_STC_P_23H, h_mtptSR_newk_STC_M_23H, h_mtptSR_newk_STC_L_23H, h_mtptSR_newk_STC_1L_23H, h_mtptSR_newk_STC_P_4H, h_mtptSR_newk_STC_M_4H, h_mtptSR_newk_STC_L_4H, h_mtptSR_newk_STC_1L_4H};

  std::vector<TH2D*> mtpt3newkhists = { h_mtptFSR_3newk_ST_P, h_mtptFSR_3newk_ST_M, h_mtptFSR_3newk_ST_L, h_mtptFSR_3newk_ST_1L, h_mtptFSR_3newk_STC_P, h_mtptFSR_3newk_STC_M, h_mtptFSR_3newk_STC_L, h_mtptFSR_3newk_STC_1L,
				 h_mtptFSR_3newk_ST_P_23L, h_mtptFSR_3newk_ST_M_23L, h_mtptFSR_3newk_ST_L_23L, h_mtptFSR_3newk_ST_1L_23L, h_mtptFSR_3newk_ST_P_4L, h_mtptFSR_3newk_ST_M_4L, h_mtptFSR_3newk_ST_L_4L, h_mtptFSR_3newk_ST_1L_4L,
				 h_mtptFSR_3newk_ST_P_23H, h_mtptFSR_3newk_ST_M_23H, h_mtptFSR_3newk_ST_L_23H, h_mtptFSR_3newk_ST_1L_23H, h_mtptFSR_3newk_ST_P_4H, h_mtptFSR_3newk_ST_M_4H, h_mtptFSR_3newk_ST_L_4H, h_mtptFSR_3newk_ST_1L_4H,
				 h_mtptFSR_3newk_STC_P_23L, h_mtptFSR_3newk_STC_M_23L, h_mtptFSR_3newk_STC_L_23L, h_mtptFSR_3newk_STC_1L_23L, h_mtptFSR_3newk_STC_P_4L, h_mtptFSR_3newk_STC_M_4L, h_mtptFSR_3newk_STC_L_4L, h_mtptFSR_3newk_STC_1L_4L,
				 h_mtptFSR_3newk_STC_P_23H, h_mtptFSR_3newk_STC_M_23H, h_mtptFSR_3newk_STC_L_23H, h_mtptFSR_3newk_STC_1L_23H, h_mtptFSR_3newk_STC_P_4H, h_mtptFSR_3newk_STC_M_4H, h_mtptFSR_3newk_STC_L_4H, h_mtptFSR_3newk_STC_1L_4H,
				 h_mtptVR_3newk_ST_P, h_mtptVR_3newk_ST_M, h_mtptVR_3newk_ST_L, h_mtptVR_3newk_ST_1L, h_mtptVR_3newk_STC_P, h_mtptVR_3newk_STC_M, h_mtptVR_3newk_STC_L, h_mtptVR_3newk_STC_1L,
				 h_mtptVR_3newk_ST_P_23L, h_mtptVR_3newk_ST_M_23L, h_mtptVR_3newk_ST_L_23L, h_mtptVR_3newk_ST_1L_23L, h_mtptVR_3newk_ST_P_4L, h_mtptVR_3newk_ST_M_4L, h_mtptVR_3newk_ST_L_4L, h_mtptVR_3newk_ST_1L_4L,
				 h_mtptVR_3newk_ST_P_23H, h_mtptVR_3newk_ST_M_23H, h_mtptVR_3newk_ST_L_23H, h_mtptVR_3newk_ST_1L_23H, h_mtptVR_3newk_ST_P_4H, h_mtptVR_3newk_ST_M_4H, h_mtptVR_3newk_ST_L_4H, h_mtptVR_3newk_ST_1L_4H,
				 h_mtptVR_3newk_STC_P_23L, h_mtptVR_3newk_STC_M_23L, h_mtptVR_3newk_STC_L_23L, h_mtptVR_3newk_STC_1L_23L, h_mtptVR_3newk_STC_P_4L, h_mtptVR_3newk_STC_M_4L, h_mtptVR_3newk_STC_L_4L, h_mtptVR_3newk_STC_1L_4L,
				 h_mtptVR_3newk_STC_P_23H, h_mtptVR_3newk_STC_M_23H, h_mtptVR_3newk_STC_L_23H, h_mtptVR_3newk_STC_1L_23H, h_mtptVR_3newk_STC_P_4H, h_mtptVR_3newk_STC_M_4H, h_mtptVR_3newk_STC_L_4H, h_mtptVR_3newk_STC_1L_4H,
				 h_mtptSR_3newk_ST_P, h_mtptSR_3newk_ST_M, h_mtptSR_3newk_ST_L, h_mtptSR_3newk_ST_1L, h_mtptSR_3newk_STC_P, h_mtptSR_3newk_STC_M, h_mtptSR_3newk_STC_L, h_mtptSR_3newk_STC_1L,
				 h_mtptSR_3newk_ST_P_23L, h_mtptSR_3newk_ST_M_23L, h_mtptSR_3newk_ST_L_23L, h_mtptSR_3newk_ST_1L_23L, h_mtptSR_3newk_ST_P_4L, h_mtptSR_3newk_ST_M_4L, h_mtptSR_3newk_ST_L_4L, h_mtptSR_3newk_ST_1L_4L,
				 h_mtptSR_3newk_ST_P_23H, h_mtptSR_3newk_ST_M_23H, h_mtptSR_3newk_ST_L_23H, h_mtptSR_3newk_ST_1L_23H, h_mtptSR_3newk_ST_P_4H, h_mtptSR_3newk_ST_M_4H, h_mtptSR_3newk_ST_L_4H, h_mtptSR_3newk_ST_1L_4H,
				 h_mtptSR_3newk_STC_P_23L, h_mtptSR_3newk_STC_M_23L, h_mtptSR_3newk_STC_L_23L, h_mtptSR_3newk_STC_1L_23L, h_mtptSR_3newk_STC_P_4L, h_mtptSR_3newk_STC_M_4L, h_mtptSR_3newk_STC_L_4L, h_mtptSR_3newk_STC_1L_4L,
				 h_mtptSR_3newk_STC_P_23H, h_mtptSR_3newk_STC_M_23H, h_mtptSR_3newk_STC_L_23H, h_mtptSR_3newk_STC_1L_23H, h_mtptSR_3newk_STC_P_4H, h_mtptSR_3newk_STC_M_4H, h_mtptSR_3newk_STC_L_4H, h_mtptSR_3newk_STC_1L_4H};

  std::vector<TH2D*> mtpt1newkhists = { h_mtptFSR_1newk_ST_P, h_mtptFSR_1newk_ST_M, h_mtptFSR_1newk_ST_L, h_mtptFSR_1newk_ST_1L, h_mtptFSR_1newk_STC_P, h_mtptFSR_1newk_STC_M, h_mtptFSR_1newk_STC_L, h_mtptFSR_1newk_STC_1L,
				 h_mtptFSR_1newk_ST_P_23L, h_mtptFSR_1newk_ST_M_23L, h_mtptFSR_1newk_ST_L_23L, h_mtptFSR_1newk_ST_1L_23L, h_mtptFSR_1newk_ST_P_4L, h_mtptFSR_1newk_ST_M_4L, h_mtptFSR_1newk_ST_L_4L, h_mtptFSR_1newk_ST_1L_4L,
				 h_mtptFSR_1newk_ST_P_23H, h_mtptFSR_1newk_ST_M_23H, h_mtptFSR_1newk_ST_L_23H, h_mtptFSR_1newk_ST_1L_23H, h_mtptFSR_1newk_ST_P_4H, h_mtptFSR_1newk_ST_M_4H, h_mtptFSR_1newk_ST_L_4H, h_mtptFSR_1newk_ST_1L_4H,
				 h_mtptFSR_1newk_STC_P_23L, h_mtptFSR_1newk_STC_M_23L, h_mtptFSR_1newk_STC_L_23L, h_mtptFSR_1newk_STC_1L_23L, h_mtptFSR_1newk_STC_P_4L, h_mtptFSR_1newk_STC_M_4L, h_mtptFSR_1newk_STC_L_4L, h_mtptFSR_1newk_STC_1L_4L,
				 h_mtptFSR_1newk_STC_P_23H, h_mtptFSR_1newk_STC_M_23H, h_mtptFSR_1newk_STC_L_23H, h_mtptFSR_1newk_STC_1L_23H, h_mtptFSR_1newk_STC_P_4H, h_mtptFSR_1newk_STC_M_4H, h_mtptFSR_1newk_STC_L_4H, h_mtptFSR_1newk_STC_1L_4H,
				 h_mtptVR_1newk_ST_P, h_mtptVR_1newk_ST_M, h_mtptVR_1newk_ST_L, h_mtptVR_1newk_ST_1L, h_mtptVR_1newk_STC_P, h_mtptVR_1newk_STC_M, h_mtptVR_1newk_STC_L, h_mtptVR_1newk_STC_1L,
				 h_mtptVR_1newk_ST_P_23L, h_mtptVR_1newk_ST_M_23L, h_mtptVR_1newk_ST_L_23L, h_mtptVR_1newk_ST_1L_23L, h_mtptVR_1newk_ST_P_4L, h_mtptVR_1newk_ST_M_4L, h_mtptVR_1newk_ST_L_4L, h_mtptVR_1newk_ST_1L_4L,
				 h_mtptVR_1newk_ST_P_23H, h_mtptVR_1newk_ST_M_23H, h_mtptVR_1newk_ST_L_23H, h_mtptVR_1newk_ST_1L_23H, h_mtptVR_1newk_ST_P_4H, h_mtptVR_1newk_ST_M_4H, h_mtptVR_1newk_ST_L_4H, h_mtptVR_1newk_ST_1L_4H,
				 h_mtptVR_1newk_STC_P_23L, h_mtptVR_1newk_STC_M_23L, h_mtptVR_1newk_STC_L_23L, h_mtptVR_1newk_STC_1L_23L, h_mtptVR_1newk_STC_P_4L, h_mtptVR_1newk_STC_M_4L, h_mtptVR_1newk_STC_L_4L, h_mtptVR_1newk_STC_1L_4L,
				 h_mtptVR_1newk_STC_P_23H, h_mtptVR_1newk_STC_M_23H, h_mtptVR_1newk_STC_L_23H, h_mtptVR_1newk_STC_1L_23H, h_mtptVR_1newk_STC_P_4H, h_mtptVR_1newk_STC_M_4H, h_mtptVR_1newk_STC_L_4H, h_mtptVR_1newk_STC_1L_4H,
				 h_mtptSR_1newk_ST_P, h_mtptSR_1newk_ST_M, h_mtptSR_1newk_ST_L, h_mtptSR_1newk_ST_1L, h_mtptSR_1newk_STC_P, h_mtptSR_1newk_STC_M, h_mtptSR_1newk_STC_L, h_mtptSR_1newk_STC_1L,
				 h_mtptSR_1newk_ST_P_23L, h_mtptSR_1newk_ST_M_23L, h_mtptSR_1newk_ST_L_23L, h_mtptSR_1newk_ST_1L_23L, h_mtptSR_1newk_ST_P_4L, h_mtptSR_1newk_ST_M_4L, h_mtptSR_1newk_ST_L_4L, h_mtptSR_1newk_ST_1L_4L,
				 h_mtptSR_1newk_ST_P_23H, h_mtptSR_1newk_ST_M_23H, h_mtptSR_1newk_ST_L_23H, h_mtptSR_1newk_ST_1L_23H, h_mtptSR_1newk_ST_P_4H, h_mtptSR_1newk_ST_M_4H, h_mtptSR_1newk_ST_L_4H, h_mtptSR_1newk_ST_1L_4H,
				 h_mtptSR_1newk_STC_P_23L, h_mtptSR_1newk_STC_M_23L, h_mtptSR_1newk_STC_L_23L, h_mtptSR_1newk_STC_1L_23L, h_mtptSR_1newk_STC_P_4L, h_mtptSR_1newk_STC_M_4L, h_mtptSR_1newk_STC_L_4L, h_mtptSR_1newk_STC_1L_4L,
				 h_mtptSR_1newk_STC_P_23H, h_mtptSR_1newk_STC_M_23H, h_mtptSR_1newk_STC_L_23H, h_mtptSR_1newk_STC_1L_23H, h_mtptSR_1newk_STC_P_4H, h_mtptSR_1newk_STC_M_4H, h_mtptSR_1newk_STC_L_4H, h_mtptSR_1newk_STC_1L_4H};

  std::vector<TH2D*> mtpthists_zll = { h_zll_mtptFSR_ST_P, h_zll_mtptFSR_ST_M, h_zll_mtptFSR_ST_L, h_zll_mtptFSR_STC_P, h_zll_mtptFSR_STC_M, h_zll_mtptFSR_STC_L,
				  h_zll_mtptFSR_ST_P_23L, h_zll_mtptFSR_ST_M_23L, h_zll_mtptFSR_ST_L_23L, h_zll_mtptFSR_ST_P_4L, h_zll_mtptFSR_ST_M_4L, h_zll_mtptFSR_ST_L_4L,
				  h_zll_mtptFSR_ST_P_23H, h_zll_mtptFSR_ST_M_23H, h_zll_mtptFSR_ST_L_23H, h_zll_mtptFSR_ST_P_4H, h_zll_mtptFSR_ST_M_4H, h_zll_mtptFSR_ST_L_4H,
				  h_zll_mtptFSR_STC_P_23L, h_zll_mtptFSR_STC_M_23L, h_zll_mtptFSR_STC_L_23L, h_zll_mtptFSR_STC_P_4L, h_zll_mtptFSR_STC_M_4L, h_zll_mtptFSR_STC_L_4L,
				  h_zll_mtptFSR_STC_P_23H, h_zll_mtptFSR_STC_M_23H, h_zll_mtptFSR_STC_L_23H, h_zll_mtptFSR_STC_P_4H, h_zll_mtptFSR_STC_M_4H, h_zll_mtptFSR_STC_L_4H,
				  h_zll_mtptVR_ST_P, h_zll_mtptVR_ST_M, h_zll_mtptVR_ST_L, h_zll_mtptVR_STC_P, h_zll_mtptVR_STC_M, h_zll_mtptVR_STC_L,
				  h_zll_mtptVR_ST_P_23L, h_zll_mtptVR_ST_M_23L, h_zll_mtptVR_ST_L_23L, h_zll_mtptVR_ST_P_4L, h_zll_mtptVR_ST_M_4L, h_zll_mtptVR_ST_L_4L,
				  h_zll_mtptVR_ST_P_23H, h_zll_mtptVR_ST_M_23H, h_zll_mtptVR_ST_L_23H, h_zll_mtptVR_ST_P_4H, h_zll_mtptVR_ST_M_4H, h_zll_mtptVR_ST_L_4H,
				  h_zll_mtptVR_STC_P_23L, h_zll_mtptVR_STC_M_23L, h_zll_mtptVR_STC_L_23L, h_zll_mtptVR_STC_P_4L, h_zll_mtptVR_STC_M_4L, h_zll_mtptVR_STC_L_4L,
				  h_zll_mtptVR_STC_P_23H, h_zll_mtptVR_STC_M_23H, h_zll_mtptVR_STC_L_23H, h_zll_mtptVR_STC_P_4H, h_zll_mtptVR_STC_M_4H, h_zll_mtptVR_STC_L_4H,
				  h_zll_mtptSR_ST_P, h_zll_mtptSR_ST_M, h_zll_mtptSR_ST_L, h_zll_mtptSR_STC_P, h_zll_mtptSR_STC_M, h_zll_mtptSR_STC_L,
				  h_zll_mtptSR_ST_P_23L, h_zll_mtptSR_ST_M_23L, h_zll_mtptSR_ST_L_23L, h_zll_mtptSR_ST_P_4L, h_zll_mtptSR_ST_M_4L, h_zll_mtptSR_ST_L_4L,
				  h_zll_mtptSR_ST_P_23H, h_zll_mtptSR_ST_M_23H, h_zll_mtptSR_ST_L_23H, h_zll_mtptSR_ST_P_4H, h_zll_mtptSR_ST_M_4H, h_zll_mtptSR_ST_L_4H,
				  h_zll_mtptSR_STC_P_23L, h_zll_mtptSR_STC_M_23L, h_zll_mtptSR_STC_L_23L, h_zll_mtptSR_STC_P_4L, h_zll_mtptSR_STC_M_4L, h_zll_mtptSR_STC_L_4L,
				  h_zll_mtptSR_STC_P_23H, h_zll_mtptSR_STC_M_23H, h_zll_mtptSR_STC_L_23H, h_zll_mtptSR_STC_P_4H, h_zll_mtptSR_STC_M_4H, h_zll_mtptSR_STC_L_4H};

  std::vector<TH2D*> mtptewkhists_zll = { h_zll_mtptFSR_ewk_ST_P, h_zll_mtptFSR_ewk_ST_M, h_zll_mtptFSR_ewk_ST_L, h_zll_mtptFSR_ewk_STC_P, h_zll_mtptFSR_ewk_STC_M, h_zll_mtptFSR_ewk_STC_L,
				  h_zll_mtptFSR_ewk_ST_P_23L, h_zll_mtptFSR_ewk_ST_M_23L, h_zll_mtptFSR_ewk_ST_L_23L, h_zll_mtptFSR_ewk_ST_P_4L, h_zll_mtptFSR_ewk_ST_M_4L, h_zll_mtptFSR_ewk_ST_L_4L,
				  h_zll_mtptFSR_ewk_ST_P_23H, h_zll_mtptFSR_ewk_ST_M_23H, h_zll_mtptFSR_ewk_ST_L_23H, h_zll_mtptFSR_ewk_ST_P_4H, h_zll_mtptFSR_ewk_ST_M_4H, h_zll_mtptFSR_ewk_ST_L_4H,
				  h_zll_mtptFSR_ewk_STC_P_23L, h_zll_mtptFSR_ewk_STC_M_23L, h_zll_mtptFSR_ewk_STC_L_23L, h_zll_mtptFSR_ewk_STC_P_4L, h_zll_mtptFSR_ewk_STC_M_4L, h_zll_mtptFSR_ewk_STC_L_4L,
				  h_zll_mtptFSR_ewk_STC_P_23H, h_zll_mtptFSR_ewk_STC_M_23H, h_zll_mtptFSR_ewk_STC_L_23H, h_zll_mtptFSR_ewk_STC_P_4H, h_zll_mtptFSR_ewk_STC_M_4H, h_zll_mtptFSR_ewk_STC_L_4H,
				  h_zll_mtptVR_ewk_ST_P, h_zll_mtptVR_ewk_ST_M, h_zll_mtptVR_ewk_ST_L, h_zll_mtptVR_ewk_STC_P, h_zll_mtptVR_ewk_STC_M, h_zll_mtptVR_ewk_STC_L,
				  h_zll_mtptVR_ewk_ST_P_23L, h_zll_mtptVR_ewk_ST_M_23L, h_zll_mtptVR_ewk_ST_L_23L, h_zll_mtptVR_ewk_ST_P_4L, h_zll_mtptVR_ewk_ST_M_4L, h_zll_mtptVR_ewk_ST_L_4L,
				  h_zll_mtptVR_ewk_ST_P_23H, h_zll_mtptVR_ewk_ST_M_23H, h_zll_mtptVR_ewk_ST_L_23H, h_zll_mtptVR_ewk_ST_P_4H, h_zll_mtptVR_ewk_ST_M_4H, h_zll_mtptVR_ewk_ST_L_4H,
				  h_zll_mtptVR_ewk_STC_P_23L, h_zll_mtptVR_ewk_STC_M_23L, h_zll_mtptVR_ewk_STC_L_23L, h_zll_mtptVR_ewk_STC_P_4L, h_zll_mtptVR_ewk_STC_M_4L, h_zll_mtptVR_ewk_STC_L_4L,
				  h_zll_mtptVR_ewk_STC_P_23H, h_zll_mtptVR_ewk_STC_M_23H, h_zll_mtptVR_ewk_STC_L_23H, h_zll_mtptVR_ewk_STC_P_4H, h_zll_mtptVR_ewk_STC_M_4H, h_zll_mtptVR_ewk_STC_L_4H,
				  h_zll_mtptSR_ewk_ST_P, h_zll_mtptSR_ewk_ST_M, h_zll_mtptSR_ewk_ST_L, h_zll_mtptSR_ewk_STC_P, h_zll_mtptSR_ewk_STC_M, h_zll_mtptSR_ewk_STC_L,
				  h_zll_mtptSR_ewk_ST_P_23L, h_zll_mtptSR_ewk_ST_M_23L, h_zll_mtptSR_ewk_ST_L_23L, h_zll_mtptSR_ewk_ST_P_4L, h_zll_mtptSR_ewk_ST_M_4L, h_zll_mtptSR_ewk_ST_L_4L,
				  h_zll_mtptSR_ewk_ST_P_23H, h_zll_mtptSR_ewk_ST_M_23H, h_zll_mtptSR_ewk_ST_L_23H, h_zll_mtptSR_ewk_ST_P_4H, h_zll_mtptSR_ewk_ST_M_4H, h_zll_mtptSR_ewk_ST_L_4H,
				  h_zll_mtptSR_ewk_STC_P_23L, h_zll_mtptSR_ewk_STC_M_23L, h_zll_mtptSR_ewk_STC_L_23L, h_zll_mtptSR_ewk_STC_P_4L, h_zll_mtptSR_ewk_STC_M_4L, h_zll_mtptSR_ewk_STC_L_4L,
				  h_zll_mtptSR_ewk_STC_P_23H, h_zll_mtptSR_ewk_STC_M_23H, h_zll_mtptSR_ewk_STC_L_23H, h_zll_mtptSR_ewk_STC_P_4H, h_zll_mtptSR_ewk_STC_M_4H, h_zll_mtptSR_ewk_STC_L_4H};

  std::vector<TH2D*> mtptnewkhists_zll = { h_zll_mtptFSR_newk_ST_P, h_zll_mtptFSR_newk_ST_M, h_zll_mtptFSR_newk_ST_L, h_zll_mtptFSR_newk_STC_P, h_zll_mtptFSR_newk_STC_M, h_zll_mtptFSR_newk_STC_L,
				  h_zll_mtptFSR_newk_ST_P_23L, h_zll_mtptFSR_newk_ST_M_23L, h_zll_mtptFSR_newk_ST_L_23L, h_zll_mtptFSR_newk_ST_P_4L, h_zll_mtptFSR_newk_ST_M_4L, h_zll_mtptFSR_newk_ST_L_4L,
				  h_zll_mtptFSR_newk_ST_P_23H, h_zll_mtptFSR_newk_ST_M_23H, h_zll_mtptFSR_newk_ST_L_23H, h_zll_mtptFSR_newk_ST_P_4H, h_zll_mtptFSR_newk_ST_M_4H, h_zll_mtptFSR_newk_ST_L_4H,
				  h_zll_mtptFSR_newk_STC_P_23L, h_zll_mtptFSR_newk_STC_M_23L, h_zll_mtptFSR_newk_STC_L_23L, h_zll_mtptFSR_newk_STC_P_4L, h_zll_mtptFSR_newk_STC_M_4L, h_zll_mtptFSR_newk_STC_L_4L,
				  h_zll_mtptFSR_newk_STC_P_23H, h_zll_mtptFSR_newk_STC_M_23H, h_zll_mtptFSR_newk_STC_L_23H, h_zll_mtptFSR_newk_STC_P_4H, h_zll_mtptFSR_newk_STC_M_4H, h_zll_mtptFSR_newk_STC_L_4H,
				  h_zll_mtptVR_newk_ST_P, h_zll_mtptVR_newk_ST_M, h_zll_mtptVR_newk_ST_L, h_zll_mtptVR_newk_STC_P, h_zll_mtptVR_newk_STC_M, h_zll_mtptVR_newk_STC_L,
				  h_zll_mtptVR_newk_ST_P_23L, h_zll_mtptVR_newk_ST_M_23L, h_zll_mtptVR_newk_ST_L_23L, h_zll_mtptVR_newk_ST_P_4L, h_zll_mtptVR_newk_ST_M_4L, h_zll_mtptVR_newk_ST_L_4L,
				  h_zll_mtptVR_newk_ST_P_23H, h_zll_mtptVR_newk_ST_M_23H, h_zll_mtptVR_newk_ST_L_23H, h_zll_mtptVR_newk_ST_P_4H, h_zll_mtptVR_newk_ST_M_4H, h_zll_mtptVR_newk_ST_L_4H,
				  h_zll_mtptVR_newk_STC_P_23L, h_zll_mtptVR_newk_STC_M_23L, h_zll_mtptVR_newk_STC_L_23L, h_zll_mtptVR_newk_STC_P_4L, h_zll_mtptVR_newk_STC_M_4L, h_zll_mtptVR_newk_STC_L_4L,
				  h_zll_mtptVR_newk_STC_P_23H, h_zll_mtptVR_newk_STC_M_23H, h_zll_mtptVR_newk_STC_L_23H, h_zll_mtptVR_newk_STC_P_4H, h_zll_mtptVR_newk_STC_M_4H, h_zll_mtptVR_newk_STC_L_4H,
				  h_zll_mtptSR_newk_ST_P, h_zll_mtptSR_newk_ST_M, h_zll_mtptSR_newk_ST_L, h_zll_mtptSR_newk_STC_P, h_zll_mtptSR_newk_STC_M, h_zll_mtptSR_newk_STC_L,
				  h_zll_mtptSR_newk_ST_P_23L, h_zll_mtptSR_newk_ST_M_23L, h_zll_mtptSR_newk_ST_L_23L, h_zll_mtptSR_newk_ST_P_4L, h_zll_mtptSR_newk_ST_M_4L, h_zll_mtptSR_newk_ST_L_4L,
				  h_zll_mtptSR_newk_ST_P_23H, h_zll_mtptSR_newk_ST_M_23H, h_zll_mtptSR_newk_ST_L_23H, h_zll_mtptSR_newk_ST_P_4H, h_zll_mtptSR_newk_ST_M_4H, h_zll_mtptSR_newk_ST_L_4H,
				  h_zll_mtptSR_newk_STC_P_23L, h_zll_mtptSR_newk_STC_M_23L, h_zll_mtptSR_newk_STC_L_23L, h_zll_mtptSR_newk_STC_P_4L, h_zll_mtptSR_newk_STC_M_4L, h_zll_mtptSR_newk_STC_L_4L,
				  h_zll_mtptSR_newk_STC_P_23H, h_zll_mtptSR_newk_STC_M_23H, h_zll_mtptSR_newk_STC_L_23H, h_zll_mtptSR_newk_STC_P_4H, h_zll_mtptSR_newk_STC_M_4H, h_zll_mtptSR_newk_STC_L_4H};


  std::vector<TH2D*> rlmtpthists = { h_rlmtptFSR, h_rlmtptFSR_23, h_rlmtptFSR_4,
				h_rlmtptVR, h_rlmtptVR_23, h_rlmtptVR_4, 
				h_rlmtptSR, h_rlmtptSR_23, h_rlmtptSR_4};

  std::vector<TH1D*> mt2hists =  { h_mt2_ST_P, h_mt2_ST_M, h_mt2_ST_L, h_mt2_ST_Lrej, 
				   h_mt2_STC_P, h_mt2_STC_M, h_mt2_STC_L, h_mt2_STC_Lrej,
				   h_mt2_ST_P_23, h_mt2_ST_M_23, h_mt2_ST_L_23, h_mt2_ST_Lrej_23, 
				   h_mt2_STC_P_23, h_mt2_STC_M_23, h_mt2_STC_L_23, h_mt2_STC_Lrej_23,
				   h_mt2_ST_P_4, h_mt2_ST_M_4, h_mt2_ST_L_4, h_mt2_ST_Lrej_4, 
				   h_mt2_STC_P_4, h_mt2_STC_M_4, h_mt2_STC_L_4, h_mt2_STC_Lrej_4};

  std::vector<TH1D*> dphiMethists =  { h_dphiMet_ST_P, h_dphiMet_ST_M, h_dphiMet_ST_L, h_dphiMet_ST_Lrej, 
				   h_dphiMet_STC_P, h_dphiMet_STC_M, h_dphiMet_STC_L, h_dphiMet_STC_Lrej,
				   h_dphiMet_ST_P_23, h_dphiMet_ST_M_23, h_dphiMet_ST_L_23, h_dphiMet_ST_Lrej_23, 
				   h_dphiMet_STC_P_23, h_dphiMet_STC_M_23, h_dphiMet_STC_L_23, h_dphiMet_STC_Lrej_23,
				   h_dphiMet_ST_P_4, h_dphiMet_ST_M_4, h_dphiMet_ST_L_4, h_dphiMet_ST_Lrej_4, 
				   h_dphiMet_STC_P_4, h_dphiMet_STC_M_4, h_dphiMet_STC_L_4, h_dphiMet_STC_Lrej_4};

  std::vector<TH2D*> etaphihists =  { h_etaphi_ST_P, h_etaphi_ST_M, h_etaphi_ST_L, h_etaphi_ST_Lrej, 
				      h_etaphi_STC_P, h_etaphi_STC_M, h_etaphi_STC_L, h_etaphi_STC_Lrej};


  std::vector<TH1D*> cuthists = {h_reliso_ST_L, h_reliso_STC_L, h_reliso_ewk_ST_L, h_reliso_ewk_STC_L, h_reliso_newk_ST_L, h_reliso_newk_STC_L,
			    h_minireliso_ST_L, h_minireliso_STC_L, h_minireliso_ewk_ST_L, h_minireliso_ewk_STC_L, h_minireliso_newk_ST_L, h_minireliso_newk_STC_L,
			    h_iso_ST_L, h_iso_STC_L, h_iso_ewk_ST_L, h_iso_ewk_STC_L, h_iso_newk_ST_L, h_iso_newk_STC_L,
			    h_niso_ST_L, h_niso_STC_L, h_niso_ewk_ST_L, h_niso_ewk_STC_L, h_niso_newk_ST_L, h_niso_newk_STC_L,
			    h_nreliso_ST_L, h_nreliso_STC_L, h_nreliso_ewk_ST_L, h_nreliso_ewk_STC_L, h_nreliso_newk_ST_L, h_nreliso_newk_STC_L,
			    h_dxy_ST_L, h_dxy_STC_L, h_dxy_ewk_ST_L, h_dxy_ewk_STC_L, h_dxy_newk_ST_L, h_dxy_newk_STC_L,
			    h_dz_ST_L, h_dz_STC_L, h_dz_ewk_ST_L, h_dz_ewk_STC_L, h_dz_newk_ST_L, h_dz_newk_STC_L,
			    h_pterr_ST_L, h_pterr_STC_L, h_pterr_ewk_ST_L, h_pterr_ewk_STC_L, h_pterr_newk_ST_L, h_pterr_newk_STC_L};

  mt2tree t;
  t.Init(ch);

  // Load the configuration and output to screen
  config_ = GetMT2Config(config_tag);
  cout << "[FshortLooper::loop] using configuration tag: " << config_tag << endl;
  cout << "                  JSON: " << config_.json << endl;
  cout << "                  lumi: " << config_.lumi << " fb-1" << endl;

  cout << "                  Filters applied:" << endl;
  for(map<string,bool>::iterator it=config_.filters.begin(); it!=config_.filters.end(); it++){
      if(it->second)
          cout << "                      " << it->first << endl;
  }


  if (applyJSON && config_.json != "") {
    cout << "[FshortLooper::loop] Loading json file: " << config_.json << endl;
    set_goodrun_file(("../babymaker/jsons/"+config_.json).c_str());
    //    set_goodrun_file((config_.json).c_str());
  }

  vector<const Int_t*> trigs_SR_;
  vector<const Int_t*> trigs_prescaled_;
  vector<const Int_t*> trigs_singleLep_;
  vector<const Int_t*> trigs_doubleLep_;

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
    fillTriggerVector(t, trigs_doubleLep_, config_.triggers["DilepSF"]);
  } else if (config_tag.find("mc") != std::string::npos) {
    std::string data_config_tag = "";
    if (config_tag == "mc_94x_Fall17") data_config_tag = "data_2017_31Mar2018";
    else if (config_tag == "mc_94x_Summer16") data_config_tag = "data_2016_94x";

    cout << "Detected we are running on MC corresponding to data config: " << data_config_tag << endl;
					 
    data_config_ = GetMT2Config(data_config_tag);
    if( data_config_.triggers.find("SR")       == data_config_.triggers.end() ||
        data_config_.triggers.find("prescaledHT")       == data_config_.triggers.end() ||
        data_config_.triggers.find("SingleMu") == data_config_.triggers.end() ||
        data_config_.triggers.find("SingleEl") == data_config_.triggers.end() ){
      cout << "[FshortLooper::loop] ERROR: invalid trigger map in configuration '" << data_config_tag << "'!" << endl;
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
    fillTriggerVector(t, trigs_singleLep_, data_config_.triggers["SingleMu"]);
    fillTriggerVector(t, trigs_singleLep_, data_config_.triggers["SingleEl"]);
    fillTriggerVector(t, trigs_doubleLep_, data_config_.triggers["DilepSF"]);

  }    else {
    cout << "                  No triggers provided and no \"mc\" in config name. Weight calculation may be inaccurate." << endl;
    if(config_tag.find("data") != string::npos){
      cout << "[FshortLooper::loop] WARNING! it looks like you are using data and didn't supply any triggers in the configuration." <<
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
  
  const int year = config_.year;
  const float lumi = config_.lumi;

  const unsigned int nEventsTree = ch->GetEntries();
  int nDuplicates = 0;
  for( unsigned int event = 0; event < nEventsTree; ++event) {    
  //  for( unsigned int event = 0; event < 10; ++event) {    
    if (event % 100000 == 0) cout << 100.0 * event / nEventsTree  << "%" << endl;

    t.GetEntry(event); 
    /*
    if (passTrigger(t, trigs_prescaled_, true)) cout << "Prescaled" << endl;
    if (passTrigger(t, trigs_SR_, true)) cout << "SR" << endl;
    if (t.HLT_PFHT1050) cout << "PFHT1050" << endl;
    */

    /*
    if ( (t.run == 302240 && t.lumi == 477 && t.evt == 484948818) ||
	 (t.run == 306125 && t.lumi == 1129 && t.evt == 2027235240) ||
	 (t.run == 305313 && t.lumi == 284 && t.evt == 495985454)) {
      std::cout << t.run << ":" << t.lumi << ":" << t.evt << std::endl;
    }
    else continue;

    if (passTrigger(t, trigs_prescaled_, true)) cout << "Prescaled" << endl;
    if (passTrigger(t, trigs_SR_, true)) cout << "SR" << endl;
    if (t.HLT_PFHT1050) cout << "PFHT1050" << endl;
    if (t.HLT_PFHT890_Prescale) cout << "890" << endl;
    if (t.HLT_PFHT780_Prescale) cout << "780" << endl;
    if (t.HLT_PFHT680_Prescale) cout << "680" << endl;
    if (t.HLT_PFHT590_Prescale) cout << "590" << endl;
    if (t.HLT_PFHT510_Prescale) cout << "510" << endl;
    if (t.HLT_PFHT430_Prescale) cout << "430" << endl;
    if (t.HLT_PFHT370_Prescale) cout << "370" << endl;
    if (t.HLT_PFHT250_Prescale) cout << "250" << endl;
    if (t.HLT_PFHT180_Prescale) cout << "180" << endl;
    if (t.HLT_PFHT500_PFMET100_PFMHT100) cout << "PFHT500_PFMET100_PFMHT100" << endl;
    if (t.HLT_PFMET120_PFMHT120_PFHT60) cout << "PFMET120_PFMHT120_PFHT60" << endl;
    if (t.HLT_PFMET120_PFMHT120) cout << "PFMET120_PFMHT120" << endl;
    if (t.HLT_PFMETNoMu120_PFMHTNoMu120) cout << "PFMETNoMu120_PFMHTNoMu120" << endl;
    if (t.HLT_PFMETNoMu120_PFMHTNoMu120_PFHT60) cout << "PFMETNoMu120_PFMHTNoMu120_PFHT60" << endl;
    if (t.HLT_PFHT800_PFMET75_PFMHT75) cout << "PFHT800_PFMET75_PFMHT75" << endl;

    continue;
    */
    
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

    if (!t.isData) h_ht->Fill(t.ht,(t.evt_scale1fb == 1 ? 1.8863e-06 : t.evt_scale1fb) * lumi); // manually correct high HT WJets

    //---------------------
    // basic event selection and cleaning
    //---------------------
    
    if( applyJSON && t.isData && !goodrun(t.run, t.lumi) ) continue;

    // changed from == 0 for cross-check
    if (unlikely(t.nVert <= 0)) {
      //      if (t.nVert < 0) cout << "CHANGE: nVert < 0" << endl;
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

    if (t.nJet30 < 2) {
      continue;
    }
    
    // catch events with unphysical jet pt
    if(t.jet_pt[0] > 13000.){
      cout << endl << "WARNING: bad event with unphysical jet pt! " << t.run << ":" << t.lumi << ":" << t.evt
	   << ", met=" << t.met_pt << ", ht=" << t.ht << ", jet_pt=" << t.jet_pt[0] << endl;
      continue;
    }
    
    if (unlikely(t.nJet30FailId != 0)) {
      continue;
    }
    // changed from > 5.0 for cross-check
    if (unlikely(t.met_miniaodPt / t.met_caloPt >= 5.0)) {
      //      if (t.met_miniaodPt / t.met_caloPt == 5.0) cout << "CHANGE: met_miniaodPt / met_caloPt == 5.0" << endl;
      continue;
    }
    if (unlikely(t.nJet200MuFrac50DphiMet > 0)) {
      continue;
    }

    const bool lepveto = t.nMuons10 + t.nElectrons10 + t.nPFLep5LowMT + t.nPFHad10LowMT > 0;

    // if we would veto a lepton, we just want to fill the removed lepton and DY plots if appropriate and then continue
    if (lepveto && false) {
      float weight = t.isData ? 1.0 : (t.evt_scale1fb == 1 ? 1.8863e-06 : t.evt_scale1fb) * lumi; // manually correct high HT WJets

      // Make sure there's exactly 1 lepton passing IDs, and apply analysis lepton veto (allowing only the reco lepton we're deleting)
      if (t.nlep == 1 && t.nMuons10 + t.nElectrons10 <= 1 && t.nPFLep5LowMT + t.nPFHad10LowMT == 0) {
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

	// nb hists
	h_rlnb_ST_L_30->Fill(t.nBJet20, weight);
	if (t.met_pt > 100) {
	  h_rlnb_ST_L_100->Fill(t.nBJet20, weight);
	  if (t.met_pt > 200) {
	    h_rlnb_ST_L_200->Fill(t.nBJet20, weight);
	    if (t.met_pt > 250) {
	      h_rlnb_ST_L_250->Fill(t.nBJet20, weight);
	      if (t.met_pt > 300) {
		h_rlnb_ST_L_300->Fill(t.nBJet20, weight);
	      }
	    }
	  }
	}
      }
      // DY study
      else if (t.nlep == 2 && false) {
	if (t.isData && !passTrigger(t, trigs_doubleLep_)) continue; 
	if (t.zll_ht < 250) continue;
	if (t.zll_mt2 < 60) continue;
	if (t.zll_met_pt < 30) continue;
	if (t.zll_diffMetMht / t.zll_met_pt > 0.5) continue;
	if (t.zll_deltaPhiMin < 0.3) continue;
	if ( fabs(t.zll_mass - 91.19) > 10 ) continue;
	if (t.lep_charge[0] == t.lep_charge[1]) continue;
	if (t.lep_pdgId[0] != -t.lep_pdgId[1]) continue;
	if (t.lep_pt[1] < 35) continue; // Ensure 100% trigger efficiency
	for (int i_trk = 0; i_trk < t.ntracks; i_trk++) {
	  bool isSTC = false;
	  bool isST = false;
	  int fillIndex = -1;
	  if (t.track_iscandidate[i_trk]) {
	    isSTC = true;
	    fillIndex = t.track_ispixelonlycandidate[i_trk] ? 1 : (t.track_ismediumcandidate[i_trk] ? 2 : 3);	  
	  }
	  else if (t.track_isshort[i_trk]) {
	    isST = true;
	    fillIndex = t.track_ispixelonly[i_trk] ? 1 : (t.track_ismedium[i_trk] ? 2 : 3);
	  }
	  if (!isST && !isSTC) continue;
	  const float mt = MT( t.track_pt[i_trk], t.track_phi[i_trk], t.zll_met_pt, t.zll_met_phi );
	  if (fillIndex == 3) {
	    if (isSTC) {
	      if (t.zll_mt2 < 100) {
		h_zll_mtptFSR_STC_L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.zll_mt2 < 200) {
		h_zll_mtptVR_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_zll_mtptSR_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.zll_mt2 < 100) {
		h_zll_mtptFSR_ST_L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.zll_mt2 < 200) {
		h_zll_mtptVR_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_zll_mtptSR_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isL
	  else if (fillIndex == 2) {
	    if (isSTC) {
	      if (t.zll_mt2 < 100) {
		h_zll_mtptFSR_STC_M->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.zll_mt2 < 200) {
		h_zll_mtptVR_STC_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_zll_mtptSR_STC_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.zll_mt2 < 100) {
		h_zll_mtptFSR_ST_M->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.zll_mt2 < 200) {
		h_zll_mtptVR_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_zll_mtptSR_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isM
	  else if (fillIndex == 1) {
	    if (isSTC) {
	      if (t.zll_mt2 < 100) {
		h_zll_mtptFSR_STC_P->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.zll_mt2 < 200) {
		h_zll_mtptVR_STC_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_zll_mtptSR_STC_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.zll_mt2 < 100) {
		h_zll_mtptFSR_ST_P->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.zll_mt2 < 200) {
		h_zll_mtptVR_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_zll_mtptSR_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isP
	
	  // Check gen match
	  if (!t.isData) {
	    bool genPromptLep = false;
	    for (int i_gl = 0; i_gl < t.ngenLep; i_gl++) {
	      if (DeltaR(t.genLep_eta[i_gl], t.track_eta[i_trk], t.genLep_phi[i_gl], t.track_phi[i_trk]) < 0.05) {
		genPromptLep = true;
	      }
	    }
	    for (int i_gl = 0; i_gl < t.ngenLepFromTau; i_gl++) {
	      if (DeltaR(t.genLepFromTau_eta[i_gl], t.track_eta[i_trk], t.genLepFromTau_phi[i_gl], t.track_phi[i_trk]) < 0.05) {
		genPromptLep = true;
	      }
	    }
	    if (genPromptLep) {
	      if (fillIndex == 3) {
		if (isSTC) {
		  if (t.zll_mt2 < 100) {
		    h_zll_mtptFSR_ewk_STC_L->Fill(mt, t.track_pt[i_trk], weight);
		    if      (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_ewk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_ewk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_ewk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_ewk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		  else if (t.zll_mt2 < 200) {
		    h_zll_mtptVR_ewk_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_ewk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_ewk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_ewk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_ewk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
		  } 
		  else {
		    h_zll_mtptSR_ewk_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_ewk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_ewk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_ewk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_ewk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		}
		else if (isST) {
		  if (t.zll_mt2 < 100) {
		    h_zll_mtptFSR_ewk_ST_L->Fill(mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_ewk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_ewk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_ewk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_ewk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		  else if (t.zll_mt2 < 200) {
		    h_zll_mtptVR_ewk_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_ewk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_ewk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_ewk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_ewk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
		  } 
		  else {
		    h_zll_mtptSR_ewk_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_ewk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_ewk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_ewk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_ewk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		}
	      } // isL
	      else if (fillIndex == 2) {
		if (isSTC) {
		  if (t.zll_mt2 < 100) {
		    h_zll_mtptFSR_ewk_STC_M->Fill(mt, t.track_pt[i_trk], weight);
		    if      (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_ewk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_ewk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_ewk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_ewk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		  else if (t.zll_mt2 < 200) {
		    h_zll_mtptVR_ewk_STC_M->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_ewk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_ewk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_ewk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_ewk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
		  } 
		  else {
		    h_zll_mtptSR_ewk_STC_M->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_ewk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_ewk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_ewk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_ewk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		}
		else if (isST) {
		  if (t.zll_mt2 < 100) {
		    h_zll_mtptFSR_ewk_ST_M->Fill(mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_ewk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_ewk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_ewk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_ewk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		  else if (t.zll_mt2 < 200) {
		    h_zll_mtptVR_ewk_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_ewk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_ewk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_ewk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_ewk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
		  } 
		  else {
		    h_zll_mtptSR_ewk_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_ewk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_ewk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_ewk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_ewk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		}
	      } // isM
	      else if (fillIndex == 1) {
		if (isSTC) {
		  if (t.zll_mt2 < 100) {
		    h_zll_mtptFSR_ewk_STC_P->Fill(mt, t.track_pt[i_trk], weight);
		    if      (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_ewk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_ewk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_ewk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_ewk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		  else if (t.zll_mt2 < 200) {
		    h_zll_mtptVR_ewk_STC_P->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_ewk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_ewk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_ewk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_ewk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
		  } 
		  else {
		    h_zll_mtptSR_ewk_STC_P->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_ewk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_ewk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_ewk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_ewk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		}
		else if (isST) {
		  if (t.zll_mt2 < 100) {
		    h_zll_mtptFSR_ewk_ST_P->Fill(mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_ewk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_ewk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_ewk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_ewk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		  else if (t.zll_mt2 < 200) {
		    h_zll_mtptVR_ewk_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_ewk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_ewk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_ewk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_ewk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
		  } 
		  else {
		    h_zll_mtptSR_ewk_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_ewk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_ewk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_ewk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_ewk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		}
	      } // isP
	    } // matches gen prompt lep	
	    else {
	      if (fillIndex == 3) {
		if (isSTC) {
		  if (t.zll_mt2 < 100) {
		    h_zll_mtptFSR_newk_STC_L->Fill(mt, t.track_pt[i_trk], weight);
		    if      (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_newk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_newk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_newk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_newk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		  else if (t.zll_mt2 < 200) {
		    h_zll_mtptVR_newk_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_newk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_newk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_newk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_newk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
		  } 
		  else {
		    h_zll_mtptSR_newk_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_newk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_newk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_newk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_newk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		}
		else if (isST) {
		  if (t.zll_mt2 < 100) {
		    h_zll_mtptFSR_newk_ST_L->Fill(mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_newk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_newk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_newk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_newk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		  else if (t.zll_mt2 < 200) {
		    h_zll_mtptVR_newk_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_newk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_newk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_newk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_newk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
		  } 
		  else {
		    h_zll_mtptSR_newk_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_newk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_newk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_newk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_newk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		}
	      } // isL
	      else if (fillIndex == 2) {
		if (isSTC) {
		  if (t.zll_mt2 < 100) {
		    h_zll_mtptFSR_newk_STC_M->Fill(mt, t.track_pt[i_trk], weight);
		    if      (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_newk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_newk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_newk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_newk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		  else if (t.zll_mt2 < 200) {
		    h_zll_mtptVR_newk_STC_M->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_newk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_newk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_newk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_newk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
		  } 
		  else {
		    h_zll_mtptSR_newk_STC_M->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_newk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_newk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_newk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_newk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		}
		else if (isST) {
		  if (t.zll_mt2 < 100) {
		    h_zll_mtptFSR_newk_ST_M->Fill(mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_newk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_newk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_newk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_newk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		  else if (t.zll_mt2 < 200) {
		    h_zll_mtptVR_newk_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_newk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_newk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_newk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_newk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
		  } 
		  else {
		    h_zll_mtptSR_newk_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_newk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_newk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_newk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_newk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		}
	      } // isM
	      else if (fillIndex == 1) {
		if (isSTC) {
		  if (t.zll_mt2 < 100) {
		    h_zll_mtptFSR_newk_STC_P->Fill(mt, t.track_pt[i_trk], weight);
		    if      (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_newk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_newk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_newk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_newk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		  else if (t.zll_mt2 < 200) {
		    h_zll_mtptVR_newk_STC_P->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_newk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_newk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_newk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_newk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
		  } 
		  else {
		    h_zll_mtptSR_newk_STC_P->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_newk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_newk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_newk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_newk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		}
		else if (isST) {
		  if (t.zll_mt2 < 100) {
		    h_zll_mtptFSR_newk_ST_P->Fill(mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptFSR_newk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptFSR_newk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptFSR_newk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptFSR_newk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		  else if (t.zll_mt2 < 200) {
		    h_zll_mtptVR_newk_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptVR_newk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptVR_newk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptVR_newk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptVR_newk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
		  } 
		  else {
		    h_zll_mtptSR_newk_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		    if (t.nJet30 < 4 && t.zll_ht < 1000) h_zll_mtptSR_newk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 < 4 && t.zll_ht > 1000) h_zll_mtptSR_newk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht < 1000) h_zll_mtptSR_newk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		    else if (t.nJet30 > 4 && t.zll_ht > 1000) h_zll_mtptSR_newk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
		  }
		}
	      } // isP
	    } // matches gen prompt lep
	  } // not data
	} // track loop	
      } // DY
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

    //if (! (passPrescaleTrigger || passSRTrigger)) continue;
    //    if (! passSRTrigger) continue;
        if ( ! passPrescaleTrigger) continue;
    //if(!((t.HLT_PFHT500_PFMET100_PFMHT100 || t.HLT_PFMET120_PFMHT120 || t.HLT_PFMETNoMu120_PFMHTNoMu120 || t.HLT_PFMETNoMu120_PFMHTNoMu120_PFHT60 || t.HLT_PFHT800_PFMET75_PFMHT75))) continue;

    bool isSignal_ = t.evt_id >= 1000;

    float weight = t.isData ? 1.0 : (t.evt_scale1fb == 1 ? 1.8863e-06 : t.evt_scale1fb) * lumi; // manually correct high HT WJets
    if (!t.isData) {
      if (isSignal_ && applyISRWeights) {
	/* // Not processing signal for now
	int binx = h_sig_avgweight_isr_->GetXaxis()->FindBin(t.GenSusyMScan1);
	int biny = h_sig_avgweight_isr_->GetYaxis()->FindBin(t.GenSusyMScan2);
	float avgweight_isr = h_sig_avgweight_isr_->GetBinContent(binx,biny);
	weight_ *= t.weight_isr / avgweight_isr;
	*/
      }
      else if (applyISRWeights && t.evt_id >= 301 && t.evt_id <= 303) { // ttbar
 	float avgweight_isr = getAverageISRWeight(t.evt_id, 0);
	weight *= t.weight_isr / avgweight_isr;
      }
      
      weight *= t.weight_btagsf;

      if (doNTrueIntReweight) {
	if(h_nTrueInt_weights_==0){
	  cout << "[FshortLooper::loop] WARNING! You requested to do nTrueInt reweighting but didn't supply a PU weights file. Turning off." << endl;
	}else{
	  int nTrueInt_input = t.nTrueInt;
	  float puWeight = h_nTrueInt_weights_->GetBinContent(h_nTrueInt_weights_->FindBin(nTrueInt_input));
	  weight *= puWeight;
	}
      }

    }
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
	// BUG
	else if (year == 2017 && (t.met_pt < 250 && t.ht < 1200)) {
	  // use same values for 2017 for now
	  if (t.met_pt < 75) {
	    // assume 0.1 for everything below 100 GeV
	    weight *= 0.1;
	    //weight *= ( (0.1/25) * (t.met_pt - 100) ); 
	    //weight = ( (0.1/25) * (t.met_pt - 100) ); 
	  }
	  else if (t.met_pt < 100) {
	    // assume 0.1 for everything below 100 GeV
	    weight *= 0.1;
	    // assume PFHT800_PFMET75_PFMHT75 goes down to 75; let's just take that as the cutoff
	    //weight *= ( (0.1/25) * (t.met_pt - 75) ); 
	    //weight *= ( (0.1/25) * (t.met_pt - 75) ); 
	    //weight = ( (0.1/25) * (t.met_pt - 100) ); 
	  }
	  else if (t.met_pt < 200) {
	    weight *= (((0.8/100) * (t.met_pt - 100)) + 0.1);
	    //weight = (((0.8/100) * (t.met_pt - 100)) + 0.1);
	  } else {
	    weight *= (((0.1/50) * (t.met_pt - 200)) + 0.9);
	    //weight = (((0.1/50) * (t.met_pt - 200)) + 0.9);
	  }	  
	}
      }
    }

    if (weight < 0.0) {
      cout << "Negative weight: " << weight << ", " << t.run << ":" << t.lumi << ":" << t.evt << endl;
    }

    //weight = 1.0;
    // VR and SR are at MET > 250 GeV
    //if (t.mt2 > 100 && t.met_pt < 250 && t.ht < 1200) continue;

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

    // Main Looper

    const int ntracks = t.ntracks;
    for (int i_trk = 0; i_trk < ntracks; i_trk++) {   

      int fillIndex = -1;
      bool isST = false, isSTC = false, isST1 = false, isSTC1 = false;
      if (recalculate) {

	// Apply basic selections
	const bool CaloSel = !(t.track_DeadECAL[i_trk] || t.track_DeadHCAL[i_trk]) && InEtaPhiVetoRegion(t.track_eta[i_trk],t.track_phi[i_trk]) == 0 
	  && (year == 2016 || !(t.track_eta[i_trk] < -0.7 && t.track_eta[i_trk] > -0.9 && t.track_phi[i_trk] > 1.5 && t.track_phi[i_trk] < 1.7 ) )
	  && (year == 2016 || !(t.track_eta[i_trk] < 0.30 && t.track_eta[i_trk] > 0.10 && t.track_phi[i_trk] > 2.2 && t.track_phi[i_trk] < 2.5 ) )
	  && (year == 2016 || !(t.track_eta[i_trk] < 0.50 && t.track_eta[i_trk] > 0.40 && t.track_phi[i_trk] > -0.7 && t.track_phi[i_trk] < -0.5  ) )
	  && (year == 2016 || !(t.track_eta[i_trk] < 0.70 && t.track_eta[i_trk] > 0.60 && t.track_phi[i_trk] > -1.1 && t.track_phi[i_trk] < -0.9  ) );
	//	const bool CaloSel = !(t.track_DeadECAL[i_trk] || t.track_DeadHCAL[i_trk]);
	const float pt = t.track_pt[i_trk];
	// changed from pt > 15
	const bool ptSel = pt >= 15.0;
	//if (pt == 15.0) cout << "CHANGE: pt == 15.0" << endl;
	const bool etaSel = fabs(t.track_eta[i_trk]) <= 2.4;
	//if (fabs(t.track_eta[i_trk]) == 2.4) cout << "CHANGE: eta == 2.4" << endl;
	const bool BaseSel = CaloSel && ptSel && etaSel;
	
	if (!BaseSel) {
	  continue;
	}
	
	FillHists(histsToFill,weight,3); // No length yet, just filling inclusive tracks
	
	// Length and category
	// Length and categorization
	const bool lostOuterHitsSel = t.track_nLostOuterHits[i_trk] >= 2;
	const int nLayers = t.track_nLayersWithMeasurement[i_trk];
	const bool isP = nLayers == t.track_nPixelLayersWithMeasurement[i_trk] && lostOuterHitsSel && nLayers >= (increment17 ? (year == 2016 ? 3 : 4) : 3);
	const bool isLong = nLayers >= (increment17 ? (year == 2016 ? 7 : 8) : 7);
	const bool isM = !isP && !isLong && lostOuterHitsSel;
	const bool isL = isLong && lostOuterHitsSel;
	const bool is1L = isLong && t.track_nLostOuterHits[i_trk] == 1;
	const bool isShort = isP || isM || isL;

	if (!isShort && !is1L) {
	  continue;
	}

	fillIndex = isP ? 1 : (isM ? 2 : 3); // use L requirements for 1-MOH tracks for now, then switch 3 to 4 for these tracks later
	
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
	const bool pixLayersSelLoose = t.track_nPixelLayersWithMeasurement[i_trk] >= (increment17 ? (year == 2016 ? 2 : 3) : 2);
	const bool pixLayersSelTight = t.track_nPixelLayersWithMeasurement[i_trk] >= (increment17 ? (year == 2016 ? 3 : 4) : 3);
	const bool pixLayersSel4 = (nLayers > (increment17 ? (year == 2016 ? 4 : 5) : 4) ? pixLayersSelLoose : pixLayersSelTight); // Apply 2 layer selection to tracks with 5 (6) or more layers in 2016 (17/18)
	
	const int lostInnerHits = t.track_nLostInnerPixelHits[i_trk];
	const bool lostInnerHitsSel = lostInnerHits == 0;
	const float pterr = t.track_ptErr[i_trk];
	const float pterrOverPt2 = pterr / (pt*pt);
	const bool pterrSelLoose = pterrOverPt2 < 0.2;
	const bool pterrSelMedium = pterrOverPt2 < 0.02;
	const bool pterrSelTight = pterrOverPt2 < 0.005;
	const bool pterrSelLooseSTC = pterrOverPt2 < 0.2 * qualSTC;
	const bool pterrSelMediumSTC = pterrOverPt2 < 0.02 * qualSTC;
	const bool pterrSelTightSTC = pterrOverPt2 < 0.005 * qualSTC;
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
	isST = PassesFullIsoSel && isQualityTrack && !is1L;
	isST1 = PassesFullIsoSel && isQualityTrack && is1L;

	// Candidate (loosened isolation, quality)...already checked for Iso and Quality above
	isSTC = !isST && !is1L;
	isSTC1 = !isST && is1L;
	// Put a floor on isolation at 30 GeV or reliso 0.4 to suppress electroweak STCs, for M and L tracks
	//isSTC = !isST && (isP || iso > 30 || reliso > 0.4) && !is1L;
	//isSTC1 = !isST1 && (iso > 30 || reliso > 0.4) && is1L;
	/*
	if  (isST && !t.track_isshort[i_trk]) {
	  cout << "New ST, i_trk " << i_trk << " $Entry " << event << endl;
	}
	if  (isSTC && !t.track_iscandidate[i_trk]) {
	  cout << "New STC, i_trk " << i_trk << " $Entry " << event << endl;
	}
	if  (!isST && t.track_isshort[i_trk]) {
	  cout << "No longer ST, i_trk " << i_trk << " $Entry " << event << endl;
	}
	if  (!isSTC && t.track_iscandidate[i_trk]) {
	  cout << "No longer STC, i_trk " << i_trk << " $Entry " << event << endl;
	}
	*/
	
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
	// more else ifs for 1-MOH tracks
      }

      if (!isST && !isSTC && !isST1 && !isSTC1) {
	continue;
      }
      /*
      // debug printouts
      if (fillIndex == 1 && t.mt2 < 100 && t.nJet30 >= 4) {
	if (isSTC) std::cout << "P STC F4: run == " << t.run << " && lumi == " << t.lumi << " && evt == " << t.evt << " && pt == " << t.track_pt[i_trk] << std::endl;
      }
      if (fillIndex == 1 && t.mt2 < 100 && t.nJet30 < 4) {
	if (isSTC) std::cout << "P STC F23: run == " << t.run << " && lumi == " << t.lumi << " && evt == " << t.evt << " && pt == " << t.track_pt[i_trk] << std::endl;
      }
      if (fillIndex == 1 && t.mt2 > 100 && t.mt2 < 200 && t.nJet30 < 4) {
	if (isST) std::cout << "P ST VR23: run == " << t.run << " && lumi == " << t.lumi << " && evt == " << t.evt << " && pt == " << t.track_pt[i_trk] << std::endl;
      }
      */
      if (isSTC1 || isST1) fillIndex = 4;

      const float mt = MT( t.track_pt[i_trk], t.track_phi[i_trk], t.met_pt, t.met_phi );

      if (fillIndex == 3) {
	isST ? h_reliso_ST_L->Fill(t.track_reliso[i_trk], weight) : h_reliso_STC_L->Fill(t.track_reliso[i_trk], weight);
	isST ? h_minireliso_ST_L->Fill(t.track_minireliso[i_trk], weight) : h_minireliso_STC_L->Fill(t.track_minireliso[i_trk], weight);
	isST ? h_iso_ST_L->Fill(t.track_iso[i_trk], weight) : h_iso_STC_L->Fill(t.track_iso[i_trk], weight);
	isST ? h_niso_ST_L->Fill(t.track_neuRelIso0p05[i_trk], weight) : h_niso_STC_L->Fill(t.track_neuRelIso0p05[i_trk], weight);
	isST ? h_nreliso_ST_L->Fill(t.track_neuRelIso0p05[i_trk]/t.track_pt[i_trk], weight) : h_nreliso_STC_L->Fill(t.track_neuRelIso0p05[i_trk]/t.track_pt[i_trk], weight);
	isST ? h_dxy_ST_L->Fill(t.track_dxy[i_trk], weight) : h_dxy_STC_L->Fill(t.track_dxy[i_trk], weight);
	isST ? h_dz_ST_L->Fill(t.track_dz[i_trk], weight) : h_dz_STC_L->Fill(t.track_dz[i_trk], weight);
	isST ? h_pterr_ST_L->Fill(t.track_ptErr[i_trk]/(t.track_pt[i_trk]*t.track_pt[i_trk]), weight) : h_pterr_STC_L->Fill(t.track_ptErr[i_trk]/(t.track_pt[i_trk]*t.track_pt[i_trk]), weight);
      }

      // Check gen match
      if (!t.isData) {
	bool genPromptE = false, genPromptM = false, genTauE = false, genTauM = false, genTau1 = false, genTau3 = false;
	for (int i_gl = 0; i_gl < t.ngenLep; i_gl++) {
	  if (DeltaR(t.genLep_eta[i_gl], t.track_eta[i_trk], t.genLep_phi[i_gl], t.track_phi[i_trk]) < 0.05) {
	    if (fabs(t.genLep_pdgId[i_gl]) == 11) genPromptE = true;
	    else genPromptM = true;
	  }
	}
	for (int i_gl = 0; i_gl < t.ngenLepFromTau; i_gl++) {
	  if (DeltaR(t.genLepFromTau_eta[i_gl], t.track_eta[i_trk], t.genLepFromTau_phi[i_gl], t.track_phi[i_trk]) < 0.05) {
	    if (fabs(t.genLepFromTau_pdgId[i_gl]) == 11) genTauE = true;
	    else genTauM = true;
	  }
	}
	for (int i_gl = 0; i_gl < t.ngenTau; i_gl++) {
	  if (DeltaR(t.genTau_eta[i_gl], t.track_eta[i_trk], t.genTau_phi[i_gl], t.track_phi[i_trk]) < 0.2) { // Only require 0.2 for rough matching to hadronic taus
	    if (t.genTau_decayMode[i_gl] == 1) {
	      genTau1 = true;
	    }
	    else if (t.genTau_decayMode[i_gl] == 3) {
	      genTau3 = true;
	    }
	  }
	}
	// Cut properties of ST(C)s based on whether they are matched to prompt gen lep or not
	if (fillIndex == 3) {
	  if (genPromptE || genPromptM || genTauE || genTauM){
	    isST ? h_reliso_ewk_ST_L->Fill(t.track_reliso[i_trk], weight) : h_reliso_ewk_STC_L->Fill(t.track_reliso[i_trk], weight);
	    isST ? h_minireliso_ewk_ST_L->Fill(t.track_minireliso[i_trk], weight) : h_minireliso_ewk_STC_L->Fill(t.track_minireliso[i_trk], weight);
	    isST ? h_iso_ewk_ST_L->Fill(t.track_iso[i_trk], weight) : h_iso_ewk_STC_L->Fill(t.track_iso[i_trk], weight);
	    isST ? h_niso_ewk_ST_L->Fill(t.track_neuRelIso0p05[i_trk], weight) : h_niso_ewk_STC_L->Fill(t.track_neuRelIso0p05[i_trk], weight);
	    isST ? h_nreliso_ewk_ST_L->Fill(t.track_neuRelIso0p05[i_trk]/t.track_pt[i_trk], weight) : h_nreliso_ewk_STC_L->Fill(t.track_neuRelIso0p05[i_trk]/t.track_pt[i_trk], weight);
	    isST ? h_dxy_ewk_ST_L->Fill(t.track_dxy[i_trk], weight) : h_dxy_ewk_STC_L->Fill(t.track_dxy[i_trk], weight);
	    isST ? h_dz_ewk_ST_L->Fill(t.track_dz[i_trk], weight) : h_dz_ewk_STC_L->Fill(t.track_dz[i_trk], weight);
	    isST ? h_pterr_ewk_ST_L->Fill(t.track_ptErr[i_trk]/(t.track_pt[i_trk]*t.track_pt[i_trk]), weight) : h_pterr_ewk_STC_L->Fill(t.track_ptErr[i_trk]/(t.track_pt[i_trk]*t.track_pt[i_trk]), weight);	    
	  }
	  else {
	    isST ? h_reliso_newk_ST_L->Fill(t.track_reliso[i_trk], weight) : h_reliso_newk_STC_L->Fill(t.track_reliso[i_trk], weight);
	    isST ? h_minireliso_newk_ST_L->Fill(t.track_minireliso[i_trk], weight) : h_minireliso_newk_STC_L->Fill(t.track_minireliso[i_trk], weight);
	    isST ? h_iso_newk_ST_L->Fill(t.track_iso[i_trk], weight) : h_iso_newk_STC_L->Fill(t.track_iso[i_trk], weight);
	    isST ? h_niso_newk_ST_L->Fill(t.track_neuRelIso0p05[i_trk], weight) : h_niso_newk_STC_L->Fill(t.track_neuRelIso0p05[i_trk], weight);
	    isST ? h_nreliso_newk_ST_L->Fill(t.track_neuRelIso0p05[i_trk]/t.track_pt[i_trk], weight) : h_nreliso_newk_STC_L->Fill(t.track_neuRelIso0p05[i_trk]/t.track_pt[i_trk], weight);
	    isST ? h_dxy_newk_ST_L->Fill(t.track_dxy[i_trk], weight) : h_dxy_newk_STC_L->Fill(t.track_dxy[i_trk], weight);
	    isST ? h_dz_newk_ST_L->Fill(t.track_dz[i_trk], weight) : h_dz_newk_STC_L->Fill(t.track_dz[i_trk], weight);
	    isST ? h_pterr_newk_ST_L->Fill(t.track_ptErr[i_trk]/(t.track_pt[i_trk]*t.track_pt[i_trk]), weight) : h_pterr_newk_STC_L->Fill(t.track_ptErr[i_trk]/(t.track_pt[i_trk]*t.track_pt[i_trk]), weight);
	  }
	}
	// mtpt distributions, based on prompt gen lep or not
	if (genPromptE) {
	  if (fillIndex == 4) {
	    if (isSTC1) {
	      if (t.mt2 < 100) {
		h_mtptFSR_ewke_STC_1L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewke_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewke_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewke_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewke_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_ewke_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewke_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewke_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewke_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewke_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewke_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewke_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewke_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewke_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewke_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    } // isSTC1
	    else if (isST1) {
	      if (t.mt2 < 100) {
		h_mtptFSR_ewke_ST_1L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewke_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewke_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewke_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewke_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_ewke_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewke_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewke_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewke_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewke_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewke_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewke_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewke_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewke_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewke_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    } // isST
	  } // is1L
	  else if (fillIndex == 3) {
	    if (isSTC) {
	      h_etaphi_ewke_STC_L->Fill( t.track_eta[i_trk], t.track_phi[i_trk] );
	      if (t.mt2 < 100) {
		h_mtptFSR_ewke_STC_L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewke_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewke_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewke_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewke_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_ewke_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewke_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewke_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewke_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewke_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewke_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewke_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewke_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewke_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewke_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      h_etaphi_ewke_ST_L->Fill( t.track_eta[i_trk], t.track_phi[i_trk] );
	      if (t.mt2 < 100) {
		h_mtptFSR_ewke_ST_L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewke_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewke_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewke_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewke_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_ewke_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewke_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewke_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewke_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewke_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewke_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewke_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewke_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewke_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewke_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    } // isST
	  } // isL
	  else if (fillIndex == 2) {
	    if (isSTC) {
	      if (t.mt2 < 100) {
		h_mtptFSR_ewke_STC_M->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewke_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewke_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewke_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewke_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_ewke_STC_M->Fill( mt, t.track_pt[i_trk], weight);
	      if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewke_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
	      else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewke_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewke_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewke_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewke_STC_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewke_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewke_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewke_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewke_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.mt2 < 100) {
		h_mtptFSR_ewke_ST_M->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewke_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewke_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewke_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewke_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_ewke_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewke_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewke_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewke_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewke_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewke_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewke_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewke_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewke_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewke_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isM
	  else if (fillIndex == 1) {
	    if (isSTC) {
	      if (t.mt2 < 100) {
		h_mtptFSR_ewke_STC_P->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewke_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewke_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewke_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewke_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_ewke_STC_P->Fill( mt, t.track_pt[i_trk], weight);
	      if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewke_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
	      else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewke_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewke_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewke_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewke_STC_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewke_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewke_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewke_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewke_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.mt2 < 100) {
		h_mtptFSR_ewke_ST_P->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewke_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewke_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewke_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewke_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_ewke_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewke_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewke_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewke_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewke_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewke_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewke_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewke_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewke_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewke_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isP
	} // matches gen prompt lep		
	else if (genPromptM) {
	  if (fillIndex == 4) {
	    if (isSTC1) {
	      if (t.mt2 < 100) {
		h_mtptFSR_ewkm_STC_1L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewkm_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewkm_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewkm_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewkm_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_ewkm_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewkm_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewkm_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewkm_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewkm_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewkm_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewkm_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewkm_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewkm_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewkm_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    } // isSTC1
	    else if (isST1) {
	      if (t.mt2 < 100) {
		h_mtptFSR_ewkm_ST_1L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewkm_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewkm_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewkm_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewkm_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_ewkm_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewkm_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewkm_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewkm_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewkm_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewkm_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewkm_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewkm_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewkm_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewkm_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    } // isST
	  } // is1L
	  else if (fillIndex == 3) {
	    if (isSTC) {
	      h_etaphi_ewkm_STC_L->Fill( t.track_eta[i_trk], t.track_phi[i_trk] );
	      if (t.mt2 < 100) {
		h_mtptFSR_ewkm_STC_L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewkm_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewkm_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewkm_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewkm_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_ewkm_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewkm_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewkm_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewkm_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewkm_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewkm_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewkm_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewkm_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewkm_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewkm_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      h_etaphi_ewkm_ST_L->Fill( t.track_eta[i_trk], t.track_phi[i_trk] );
	      if (t.mt2 < 100) {
		h_mtptFSR_ewkm_ST_L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewkm_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewkm_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewkm_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewkm_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_ewkm_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewkm_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewkm_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewkm_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewkm_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewkm_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewkm_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewkm_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewkm_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewkm_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    } // isST
	  } // isL
	  else if (fillIndex == 2) {
	    if (isSTC) {
	      if (t.mt2 < 100) {
		h_mtptFSR_ewkm_STC_M->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewkm_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewkm_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewkm_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewkm_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_ewkm_STC_M->Fill( mt, t.track_pt[i_trk], weight);
	      if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewkm_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
	      else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewkm_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewkm_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewkm_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewkm_STC_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewkm_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewkm_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewkm_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewkm_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.mt2 < 100) {
		h_mtptFSR_ewkm_ST_M->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewkm_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewkm_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewkm_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewkm_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_ewkm_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewkm_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewkm_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewkm_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewkm_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewkm_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewkm_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewkm_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewkm_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewkm_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isM
	  else if (fillIndex == 1) {
	    if (isSTC) {
	      if (t.mt2 < 100) {
		h_mtptFSR_ewkm_STC_P->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewkm_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewkm_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewkm_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewkm_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_ewkm_STC_P->Fill( mt, t.track_pt[i_trk], weight);
	      if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewkm_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
	      else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewkm_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewkm_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewkm_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewkm_STC_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewkm_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewkm_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewkm_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewkm_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.mt2 < 100) {
		h_mtptFSR_ewkm_ST_P->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ewkm_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ewkm_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ewkm_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ewkm_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_ewkm_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ewkm_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ewkm_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ewkm_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ewkm_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_ewkm_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ewkm_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ewkm_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ewkm_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ewkm_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isP
	} // matches gen prompt lep	
	else if (genTauE) {
	  if (fillIndex == 4) {
	    if (isSTC1) {
	      if (t.mt2 < 100) {
		h_mtptFSR_tewke_STC_1L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewke_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewke_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewke_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewke_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_tewke_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewke_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewke_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewke_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewke_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewke_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewke_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewke_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewke_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewke_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    } // isSTC1
	    else if (isST1) {
	      if (t.mt2 < 100) {
		h_mtptFSR_tewke_ST_1L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewke_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewke_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewke_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewke_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_tewke_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewke_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewke_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewke_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewke_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewke_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewke_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewke_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewke_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewke_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    } // isST
	  } // is1L
	  else if (fillIndex == 3) {
	    if (isSTC) {
	      h_etaphi_tewke_STC_L->Fill( t.track_eta[i_trk], t.track_phi[i_trk] );
	      if (t.mt2 < 100) {
		h_mtptFSR_tewke_STC_L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewke_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewke_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewke_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewke_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_tewke_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewke_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewke_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewke_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewke_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewke_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewke_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewke_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewke_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewke_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      h_etaphi_tewke_ST_L->Fill( t.track_eta[i_trk], t.track_phi[i_trk] );
	      if (t.mt2 < 100) {
		h_mtptFSR_tewke_ST_L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewke_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewke_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewke_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewke_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_tewke_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewke_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewke_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewke_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewke_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewke_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewke_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewke_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewke_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewke_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    } // isST
	  } // isL
	  else if (fillIndex == 2) {
	    if (isSTC) {
	      if (t.mt2 < 100) {
		h_mtptFSR_tewke_STC_M->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewke_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewke_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewke_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewke_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_tewke_STC_M->Fill( mt, t.track_pt[i_trk], weight);
	      if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewke_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
	      else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewke_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewke_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewke_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewke_STC_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewke_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewke_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewke_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewke_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.mt2 < 100) {
		h_mtptFSR_tewke_ST_M->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewke_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewke_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewke_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewke_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_tewke_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewke_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewke_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewke_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewke_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewke_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewke_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewke_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewke_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewke_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isM
	  else if (fillIndex == 1) {
	    if (isSTC) {
	      if (t.mt2 < 100) {
		h_mtptFSR_tewke_STC_P->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewke_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewke_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewke_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewke_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_tewke_STC_P->Fill( mt, t.track_pt[i_trk], weight);
	      if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewke_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
	      else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewke_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewke_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewke_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewke_STC_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewke_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewke_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewke_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewke_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.mt2 < 100) {
		h_mtptFSR_tewke_ST_P->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewke_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewke_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewke_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewke_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_tewke_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewke_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewke_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewke_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewke_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewke_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewke_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewke_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewke_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewke_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isP
	} // matches gen tau lep	
	else if (genTauM) {
	  if (fillIndex == 4) {
	    if (isSTC1) {
	      if (t.mt2 < 100) {
		h_mtptFSR_tewkm_STC_1L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewkm_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewkm_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewkm_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewkm_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_tewkm_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewkm_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewkm_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewkm_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewkm_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewkm_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewkm_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewkm_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewkm_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewkm_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    } // isSTC1
	    else if (isST1) {
	      if (t.mt2 < 100) {
		h_mtptFSR_tewkm_ST_1L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewkm_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewkm_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewkm_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewkm_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_tewkm_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewkm_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewkm_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewkm_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewkm_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewkm_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewkm_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewkm_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewkm_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewkm_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    } // isST
	  } // is1L
	  else if (fillIndex == 3) {
	    if (isSTC) {
	      h_etaphi_tewkm_STC_L->Fill( t.track_eta[i_trk], t.track_phi[i_trk] );
	      if (t.mt2 < 100) {
		h_mtptFSR_tewkm_STC_L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewkm_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewkm_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewkm_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewkm_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_tewkm_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewkm_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewkm_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewkm_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewkm_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewkm_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewkm_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewkm_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewkm_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewkm_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      h_etaphi_tewkm_ST_L->Fill( t.track_eta[i_trk], t.track_phi[i_trk] );
	      if (t.mt2 < 100) {
		h_mtptFSR_tewkm_ST_L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewkm_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewkm_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewkm_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewkm_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_tewkm_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewkm_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewkm_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewkm_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewkm_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewkm_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewkm_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewkm_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewkm_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewkm_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    } // isST
	  } // isL
	  else if (fillIndex == 2) {
	    if (isSTC) {
	      if (t.mt2 < 100) {
		h_mtptFSR_tewkm_STC_M->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewkm_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewkm_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewkm_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewkm_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_tewkm_STC_M->Fill( mt, t.track_pt[i_trk], weight);
	      if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewkm_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
	      else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewkm_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewkm_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewkm_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewkm_STC_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewkm_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewkm_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewkm_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewkm_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.mt2 < 100) {
		h_mtptFSR_tewkm_ST_M->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewkm_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewkm_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewkm_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewkm_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_tewkm_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewkm_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewkm_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewkm_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewkm_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewkm_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewkm_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewkm_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewkm_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewkm_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isM
	  else if (fillIndex == 1) {
	    if (isSTC) {
	      if (t.mt2 < 100) {
		h_mtptFSR_tewkm_STC_P->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewkm_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewkm_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewkm_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewkm_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_tewkm_STC_P->Fill( mt, t.track_pt[i_trk], weight);
	      if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewkm_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
	      else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewkm_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewkm_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewkm_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewkm_STC_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewkm_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewkm_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewkm_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewkm_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.mt2 < 100) {
		h_mtptFSR_tewkm_ST_P->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_tewkm_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_tewkm_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_tewkm_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_tewkm_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_tewkm_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_tewkm_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_tewkm_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_tewkm_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_tewkm_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_tewkm_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_tewkm_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_tewkm_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_tewkm_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_tewkm_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isP
	} // matches gen tau lep	
	else if (genTau1) {
	  if (fillIndex == 4) {
	    if (isSTC1) {
	      if (t.mt2 < 100) {
		h_mtptFSR_1newk_STC_1L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_1newk_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_1newk_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_1newk_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_1newk_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_1newk_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_1newk_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_1newk_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_1newk_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_1newk_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_1newk_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_1newk_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_1newk_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_1newk_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_1newk_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST1) {
	      if (t.mt2 < 100) {
		h_mtptFSR_1newk_ST_1L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_1newk_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_1newk_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_1newk_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_1newk_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_1newk_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_1newk_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_1newk_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_1newk_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_1newk_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_1newk_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_1newk_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_1newk_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_1newk_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_1newk_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // is1L
	  if (fillIndex == 3) {
	    if (isSTC) {
	      h_etaphi_1newk_STC_L->Fill( t.track_eta[i_trk], t.track_phi[i_trk] );
	      if (t.mt2 < 100) {
		h_mtptFSR_1newk_STC_L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_1newk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_1newk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_1newk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_1newk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_1newk_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_1newk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_1newk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_1newk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_1newk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_1newk_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_1newk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_1newk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_1newk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_1newk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      h_etaphi_1newk_ST_L->Fill( t.track_eta[i_trk], t.track_phi[i_trk] );
	      if (t.mt2 < 100) {
		h_mtptFSR_1newk_ST_L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_1newk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_1newk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_1newk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_1newk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_1newk_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_1newk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_1newk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_1newk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_1newk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_1newk_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_1newk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_1newk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_1newk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_1newk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isL
	  else if (fillIndex == 2) {
	    if (isSTC) {
	      if (t.mt2 < 100) {
		h_mtptFSR_1newk_STC_M->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_1newk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_1newk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_1newk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_1newk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_1newk_STC_M->Fill( mt, t.track_pt[i_trk], weight);
	      if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_1newk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
	      else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_1newk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_1newk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_1newk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_1newk_STC_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_1newk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_1newk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_1newk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_1newk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.mt2 < 100) {
		h_mtptFSR_1newk_ST_M->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_1newk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_1newk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_1newk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_1newk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_1newk_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_1newk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_1newk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_1newk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_1newk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_1newk_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_1newk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_1newk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_1newk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_1newk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isM
	  else if (fillIndex == 1) {
	    if (isSTC) {
	      if (t.mt2 < 100) {
		h_mtptFSR_1newk_STC_P->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_1newk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_1newk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_1newk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_1newk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_1newk_STC_P->Fill( mt, t.track_pt[i_trk], weight);
	      if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_1newk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
	      else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_1newk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_1newk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_1newk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_1newk_STC_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_1newk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_1newk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_1newk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_1newk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.mt2 < 100) {
		h_mtptFSR_1newk_ST_P->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_1newk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_1newk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_1newk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_1newk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_1newk_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_1newk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_1newk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_1newk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_1newk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_1newk_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_1newk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_1newk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_1newk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_1newk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isP
	} // matches gen tau 1
	else if (genTau3) {
	  if (fillIndex == 4) {
	    if (isSTC1) {
	      if (t.mt2 < 100) {
		h_mtptFSR_3newk_STC_1L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_3newk_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_3newk_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_3newk_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_3newk_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_3newk_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_3newk_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_3newk_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_3newk_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_3newk_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_3newk_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_3newk_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_3newk_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_3newk_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_3newk_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST1) {
	      if (t.mt2 < 100) {
		h_mtptFSR_3newk_ST_1L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_3newk_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_3newk_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_3newk_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_3newk_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_3newk_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_3newk_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_3newk_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_3newk_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_3newk_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_3newk_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_3newk_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_3newk_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_3newk_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_3newk_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // is1L
	  if (fillIndex == 3) {
	    if (isSTC) {
	      h_etaphi_3newk_STC_L->Fill( t.track_eta[i_trk], t.track_phi[i_trk] );
	      if (t.mt2 < 100) {
		h_mtptFSR_3newk_STC_L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_3newk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_3newk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_3newk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_3newk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_3newk_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_3newk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_3newk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_3newk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_3newk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_3newk_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_3newk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_3newk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_3newk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_3newk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      h_etaphi_3newk_ST_L->Fill( t.track_eta[i_trk], t.track_phi[i_trk] );
	      if (t.mt2 < 100) {
		h_mtptFSR_3newk_ST_L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_3newk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_3newk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_3newk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_3newk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_3newk_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_3newk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_3newk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_3newk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_3newk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_3newk_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_3newk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_3newk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_3newk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_3newk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isL
	  else if (fillIndex == 2) {
	    if (isSTC) {
	      if (t.mt2 < 100) {
		h_mtptFSR_3newk_STC_M->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_3newk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_3newk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_3newk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_3newk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_3newk_STC_M->Fill( mt, t.track_pt[i_trk], weight);
	      if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_3newk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
	      else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_3newk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_3newk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_3newk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_3newk_STC_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_3newk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_3newk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_3newk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_3newk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.mt2 < 100) {
		h_mtptFSR_3newk_ST_M->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_3newk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_3newk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_3newk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_3newk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_3newk_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_3newk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_3newk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_3newk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_3newk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_3newk_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_3newk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_3newk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_3newk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_3newk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isM
	  else if (fillIndex == 1) {
	    if (isSTC) {
	      if (t.mt2 < 100) {
		h_mtptFSR_3newk_STC_P->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_3newk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_3newk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_3newk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_3newk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_3newk_STC_P->Fill( mt, t.track_pt[i_trk], weight);
	      if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_3newk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
	      else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_3newk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_3newk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_3newk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_3newk_STC_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_3newk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_3newk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_3newk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_3newk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.mt2 < 100) {
		h_mtptFSR_3newk_ST_P->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_3newk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_3newk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_3newk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_3newk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_3newk_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_3newk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_3newk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_3newk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_3newk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_3newk_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_3newk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_3newk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_3newk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_3newk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isP
	} // matches gen tau 3
	else {
	  if (fillIndex == 4) {
	    if (isSTC1) {
	      if (t.mt2 < 100) {
		h_mtptFSR_newk_STC_1L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_newk_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_newk_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_newk_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_newk_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_newk_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_newk_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_newk_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_newk_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_newk_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_newk_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_newk_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_newk_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_newk_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_newk_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST1) {
	      if (t.mt2 < 100) {
		h_mtptFSR_newk_ST_1L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_newk_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_newk_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_newk_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_newk_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_newk_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_newk_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_newk_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_newk_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_newk_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_newk_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_newk_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_newk_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_newk_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_newk_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // is1L
	  if (fillIndex == 3) {
	    if (isSTC) {
	      h_etaphi_newk_STC_L->Fill( t.track_eta[i_trk], t.track_phi[i_trk] );
	      if (t.mt2 < 100) {
		h_mtptFSR_newk_STC_L->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_newk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_newk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_newk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_newk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_newk_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_newk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_newk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_newk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_newk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_newk_STC_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_newk_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_newk_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_newk_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_newk_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      h_etaphi_newk_ST_L->Fill( t.track_eta[i_trk], t.track_phi[i_trk] );
	      if (t.mt2 < 100) {
		h_mtptFSR_newk_ST_L->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_newk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_newk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_newk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_newk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_newk_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_newk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_newk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_newk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_newk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_newk_ST_L->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_newk_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_newk_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_newk_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_newk_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isL
	  else if (fillIndex == 2) {
	    if (isSTC) {
	      if (t.mt2 < 100) {
		h_mtptFSR_newk_STC_M->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_newk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_newk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_newk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_newk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_newk_STC_M->Fill( mt, t.track_pt[i_trk], weight);
	      if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_newk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
	      else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_newk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_newk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_newk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_newk_STC_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_newk_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_newk_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_newk_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_newk_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.mt2 < 100) {
		h_mtptFSR_newk_ST_M->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_newk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_newk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_newk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_newk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_newk_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_newk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_newk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_newk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_newk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_newk_ST_M->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_newk_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_newk_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_newk_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_newk_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isM
	  else if (fillIndex == 1) {
	    if (isSTC) {
	      if (t.mt2 < 100) {
		h_mtptFSR_newk_STC_P->Fill(mt, t.track_pt[i_trk], weight);
		if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_newk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_newk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_newk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_newk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	    }
	    else if (t.mt2 < 200) {
	      h_mtptVR_newk_STC_P->Fill( mt, t.track_pt[i_trk], weight);
	      if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_newk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
	      else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_newk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_newk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_newk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_newk_STC_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_newk_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_newk_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_newk_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_newk_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	    else if (isST) {
	      if (t.mt2 < 100) {
		h_mtptFSR_newk_ST_P->Fill(mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_newk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_newk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_newk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_newk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	      else if (t.mt2 < 200) {
		h_mtptVR_newk_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_newk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_newk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_newk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_newk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      } 
	      else {
		h_mtptSR_newk_ST_P->Fill( mt, t.track_pt[i_trk], weight);
		if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_newk_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_newk_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_newk_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
		else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_newk_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	      }
	    }
	  } // isP
	} // matches gen prompt lep
      } // not data

      if (fillIndex == 3) {
	h_nb_ST_L_30->Fill(t.nBJet20, weight);
	if (t.met_pt > 100) {
	  h_nb_ST_L_100->Fill(t.nBJet20, weight);
	  if (t.met_pt > 200) {
	    h_nb_ST_L_200->Fill(t.nBJet20, weight);
	    if (t.met_pt > 250) {
	      h_nb_ST_L_250->Fill(t.nBJet20, weight);
	      if (t.met_pt > 300) {
		h_nb_ST_L_300->Fill(t.nBJet20, weight);
	      }
	    }
	  }
	}
      }

      if (fillIndex == 1) {
	if (isSTC) {
	  if (t.mt2 < 100) {
	    h_mtptFSR_STC_P->Fill(mt, t.track_pt[i_trk], weight);
	    if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	  else if (t.mt2 < 200) {
	    h_mtptVR_STC_P->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	  } 
	  else {
	    h_mtptSR_STC_P->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_STC_P_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_STC_P_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_STC_P_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_STC_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	}
	else if (isST) {
	  if (t.mt2 < 100) {
	    h_mtptFSR_ST_P->Fill(mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	  else if (t.mt2 < 200) {
	    h_mtptVR_ST_P->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	  } 
	  else {
	    h_mtptSR_ST_P->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ST_P_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ST_P_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ST_P_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ST_P_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	}
      } // isP
      if (fillIndex == 2) {
	if (isSTC) {
	  if (t.mt2 < 100) {
	    h_mtptFSR_STC_M->Fill(mt, t.track_pt[i_trk], weight);
	    if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	  else if (t.mt2 < 200) {
	    h_mtptVR_STC_M->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	  } 
	  else {
	    h_mtptSR_STC_M->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_STC_M_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_STC_M_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_STC_M_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_STC_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	}
	else if (isST) {
	  if (t.mt2 < 100) {
	    h_mtptFSR_ST_M->Fill(mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	  else if (t.mt2 < 200) {
	    h_mtptVR_ST_M->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	  } 
	  else {
	    h_mtptSR_ST_M->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ST_M_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ST_M_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ST_M_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ST_M_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	}
      } // isM
      if (fillIndex == 3) {
	if (isSTC) {
	  if (t.mt2 < 100) {
	    h_mtptFSR_STC_L->Fill(mt, t.track_pt[i_trk], weight);
	    if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	  else if (t.mt2 < 200) {
	    h_mtptVR_STC_L->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	  } 
	  else {
	    h_mtptSR_STC_L->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_STC_L_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_STC_L_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_STC_L_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_STC_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	}
	else if (isST) {
	  if (t.mt2 < 100) {
	    h_mtptFSR_ST_L->Fill(mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	  else if (t.mt2 < 200) {
	    h_mtptVR_ST_L->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	  } 
	  else {
	    h_mtptSR_ST_L->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ST_L_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ST_L_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ST_L_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ST_L_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	}
      } // isL
      if (fillIndex == 4) {
	if (isSTC1) {
	  if (t.mt2 < 100) {
	    h_mtptFSR_STC_1L->Fill(mt, t.track_pt[i_trk], weight);
	    if      (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	  else if (t.mt2 < 200) {
	    h_mtptVR_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	  } 
	  else {
	    h_mtptSR_STC_1L->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_STC_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_STC_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_STC_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_STC_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	}
	else if (isST1) {
	  if (t.mt2 < 100) {
	    h_mtptFSR_ST_1L->Fill(mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptFSR_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptFSR_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptFSR_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptFSR_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	  else if (t.mt2 < 200) {
	    h_mtptVR_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptVR_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptVR_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptVR_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptVR_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	  } 
	  else {
	    h_mtptSR_ST_1L->Fill( mt, t.track_pt[i_trk], weight);
	    if (t.nJet30 < 4 && t.ht < 1000) h_mtptSR_ST_1L_23L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 < 4 && t.ht > 1000) h_mtptSR_ST_1L_23H->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht < 1000) h_mtptSR_ST_1L_4L->Fill(mt, t.track_pt[i_trk], weight);
	    else if (t.nJet30 > 4 && t.ht > 1000) h_mtptSR_ST_1L_4H->Fill(mt, t.track_pt[i_trk], weight);
	  }
	}
      } // isP
      
      // One final cut for L tracks to catch lost leptons
      // These tracks are probably background, likely a lost muon from W+jets
      // in this case, just fill the mtpt hists and continue
      bool rejected = (t.track_pt[i_trk] < 150 && mt < 100);
      if (fillIndex == 3 && rejected) fillIndex = 4;
      
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
	    break;
	  case 2: 
	    h_mt2_STC_M->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_STC_M_23->Fill(t.mt2,weight) : h_mt2_STC_M_4->Fill(t.mt2,weight); 
	    h_dphiMet_STC_M->Fill(dphiMet,weight); 
	    t.nJet30 < 4 ? h_dphiMet_STC_M_23->Fill(dphiMet,weight) : h_dphiMet_STC_M_4->Fill(dphiMet,weight); 
	    h_etaphi_STC_M->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight); 
	    break;
	  case 3: 
	    h_mt2_STC_L->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_STC_L_23->Fill(t.mt2,weight) : h_mt2_STC_L_4->Fill(t.mt2,weight); 
	    h_dphiMet_STC_L->Fill(dphiMet,weight); 
	    t.nJet30 < 4 ? h_dphiMet_STC_L_23->Fill(dphiMet,weight) : h_dphiMet_STC_L_4->Fill(dphiMet,weight); 
	    h_etaphi_STC_L->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight); 
	    break;
	  case 4: 
	    h_mt2_STC_Lrej->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_STC_Lrej_23->Fill(t.mt2,weight) : h_mt2_STC_Lrej_4->Fill(t.mt2,weight); 
	    h_dphiMet_STC_Lrej->Fill(dphiMet,weight); 
	    t.nJet30 < 4 ? h_dphiMet_STC_Lrej_23->Fill(dphiMet,weight) : h_dphiMet_STC_Lrej_4->Fill(dphiMet,weight); 
	    h_etaphi_STC_Lrej->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight); 
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
	    break;
	  case 2: 
	    h_mt2_ST_M->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_ST_M_23->Fill(t.mt2,weight) : h_mt2_ST_M_4->Fill(t.mt2,weight); 
	    h_dphiMet_ST_M->Fill(dphiMet,weight); 
	    t.nJet30 < 4 ? h_dphiMet_ST_M_23->Fill(dphiMet,weight) : h_dphiMet_ST_M_4->Fill(dphiMet,weight); 
	    h_etaphi_ST_M->Fill(t.track_eta[i_trk], t.track_phi[i_trk], weight); 
	    break;
	  case 3: 
	    h_mt2_ST_L->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_ST_L_23->Fill(t.mt2,weight) : h_mt2_ST_L_4->Fill(t.mt2,weight); 
	    h_dphiMet_ST_L->Fill(dphiMet,weight); 
	    t.nJet30 < 4 ? h_dphiMet_ST_L_23->Fill(dphiMet,weight) : h_dphiMet_ST_L_4->Fill(dphiMet,weight); 
	    h_etaphi_ST_L->Fill(t.track_eta[i_trk],t.track_phi[i_trk], weight); 
	    break;
	  case 4: 
	    h_mt2_ST_Lrej->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_ST_Lrej_23->Fill(t.mt2,weight) : h_mt2_ST_Lrej_4->Fill(t.mt2,weight); 
	    h_dphiMet_ST_Lrej->Fill(dphiMet,weight); 
	    t.nJet30 < 4 ? h_dphiMet_ST_Lrej_23->Fill(dphiMet,weight) : h_dphiMet_ST_Lrej_4->Fill(dphiMet,weight); 
	    h_etaphi_ST_Lrej->Fill(t.track_eta[i_trk],t.track_phi[i_trk], weight); 
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
  h_ht->Write();
  for (vector<TH2D*>::iterator hist = fsrhists.begin(); hist != fsrhists.end(); hist++) (*hist)->Write();
  for (vector<TH1D*>::iterator hist = nbhists.begin(); hist != nbhists.end(); hist++) (*hist)->Write();
  for (vector<TH1D*>::iterator hist = rlnbhists.begin(); hist != rlnbhists.end(); hist++) (*hist)->Write();

  for (vector<TH2D*>::iterator hist = mtpthists.begin(); hist != mtpthists.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = mtptewkmhists.begin(); hist != mtptewkmhists.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = mtpttewkmhists.begin(); hist != mtpttewkmhists.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = mtptewkehists.begin(); hist != mtptewkehists.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = mtpttewkehists.begin(); hist != mtpttewkehists.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = mtptnewkhists.begin(); hist != mtptnewkhists.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = mtpt1newkhists.begin(); hist != mtpt1newkhists.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = mtpt3newkhists.begin(); hist != mtpt3newkhists.end(); hist++) (*hist)->Write();

  for (vector<TH2D*>::iterator hist = mtpthists_zll.begin(); hist != mtpthists_zll.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = mtptewkhists_zll.begin(); hist != mtptewkhists_zll.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = mtptnewkhists_zll.begin(); hist != mtptnewkhists_zll.end(); hist++) (*hist)->Write();

  for (vector<TH2D*>::iterator hist = rlmtpthists.begin(); hist != rlmtpthists.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = etaphihists.begin(); hist != etaphihists.end(); hist++) (*hist)->Write();
  for (vector<TH1D*>::iterator hist = mt2hists.begin(); hist != mt2hists.end(); hist++) (*hist)->Write();
  for (vector<TH1D*>::iterator hist = dphiMethists.begin(); hist != dphiMethists.end(); hist++) (*hist)->Write();
  for (vector<TH1D*>::iterator hist = cuthists.begin(); hist != cuthists.end(); hist++) (*hist)->Write();
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

float FshortLooper::getAverageISRWeight(const int evt_id, const int var) {

  // madgraph ttsl, from RunIISummer16MiniAODv2
  if (evt_id == 301 || evt_id == 302) {
    if (var == 0) return 0.909; // nominal
    else if (var == 1) return 0.954; // UP
    else if (var == -1) return 0.863; // DN
  }
  // madgraph ttdl, from RunIISummer16MiniAODv2
  else if (evt_id == 303) {
    if (var == 0) return 0.895; // nominal
    else if (var == 1) return 0.948; // UP
    else if (var == -1) return 0.843; // DN
  }

  std::cout << "WARNING: FshortLooper::getAverageISRWeight: didn't recognize either evt_id: " << evt_id
	    << " or variation: " << var << std::endl;
  return 1.;
}

FshortLooper::FshortLooper() {}
 
FshortLooper::~FshortLooper() {};
