//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Apr 30 01:34:58 2018 by ROOT version 6.10/09
// from TTree mt2st/A mt2 baby-tree edition
// found on file: /hadoop/cms/store/user/dpgilber/Stat1Leps_Friends/TTDL/MT2-10_ST-nonext_friend.root
//////////////////////////////////////////////////////////

#ifndef mt2tree_h
#define mt2tree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

// Header file for the classes stored in the TTree if any.

class mt2tree : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           run;
   Int_t           lumi;
   ULong64_t       evt;
   Int_t           isData;
   Int_t           isGolden;
   Float_t         evt_scale1fb;
   Float_t         evt_xsec;
   Float_t         evt_kfactor;
   Float_t         evt_filter;
   ULong64_t       evt_nEvts;
   Int_t           evt_id;
   Float_t         genWeight;
   Float_t         puWeight;
   Int_t           nVert;
   Int_t           nTrueInt;
   Float_t         rho;
   Int_t           nJet30;
   Int_t           nJet30JECup;
   Int_t           nJet30JECdn;
   Int_t           nJet40;
   Int_t           nBJet20;
   Int_t           nBJet20csv;
   Int_t           nBJet20mva;
   Int_t           nBJet20JECup;
   Int_t           nBJet20JECdn;
   Int_t           nBJet25;
   Int_t           nBJet30;
   Int_t           nBJet30csv;
   Int_t           nBJet30mva;
   Int_t           nBJet40;
   Int_t           nJet30FailId;
   Int_t           nJet100FailId;
   Int_t           nJet20BadFastsim;
   Int_t           nJet200MuFrac50DphiMet;
   Int_t           nMuons10;
   Int_t           nBadMuons20;
   Int_t           nElectrons10;
   Int_t           nLepLowMT;
   Int_t           nTaus20;
   Int_t           nGammas20;
   Int_t           nPFCHCand3;
   Float_t         deltaPhiMin;
   Float_t         deltaPhiMin_genmet;
   Float_t         diffMetMht;
   Float_t         diffMetMht_genmet;
   Float_t         deltaPhiMinJECup;
   Float_t         deltaPhiMinJECdn;
   Float_t         diffMetMhtJECup;
   Float_t         diffMetMhtJECdn;
   Float_t         minMTBMet;
   Float_t         zll_minMTBMet;
   Float_t         gamma_minMTBMet;
   Float_t         ht;
   Float_t         htJECup;
   Float_t         htJECdn;
   Float_t         mt2;
   Float_t         mt2JECup;
   Float_t         mt2JECdn;
   Float_t         mt2_gen;
   Float_t         mt2_genmet;
   Float_t         jet1_pt;
   Float_t         jet2_pt;
   Float_t         jet1_ptJECup;
   Float_t         jet2_ptJECup;
   Float_t         jet1_ptJECdn;
   Float_t         jet2_ptJECdn;
   Int_t           jet_failFSveto;
   Float_t         gamma_jet1_pt;
   Float_t         gamma_jet2_pt;
   Float_t         pseudoJet1_pt;
   Float_t         pseudoJet1_eta;
   Float_t         pseudoJet1_phi;
   Float_t         pseudoJet1_mass;
   Float_t         pseudoJet2_pt;
   Float_t         pseudoJet2_eta;
   Float_t         pseudoJet2_phi;
   Float_t         pseudoJet2_mass;
   Float_t         mht_pt;
   Float_t         mht_phi;
   Float_t         mht_ptJECup;
   Float_t         mht_phiJECup;
   Float_t         mht_ptJECdn;
   Float_t         mht_phiJECdn;
   Float_t         met_pt;
   Float_t         met_phi;
   Float_t         met_ptJECup;
   Float_t         met_phiJECup;
   Float_t         met_ptJECdn;
   Float_t         met_phiJECdn;
   Float_t         met_rawPt;
   Float_t         met_rawPhi;
   Float_t         met_caloPt;
   Float_t         met_caloPhi;
   Float_t         met_trkPt;
   Float_t         met_trkPhi;
   Float_t         met_genPt;
   Float_t         met_genPhi;
   Float_t         met_miniaodPt;
   Float_t         met_miniaodPhi;
   Int_t           Flag_EcalDeadCellTriggerPrimitiveFilter;
   Int_t           Flag_trkPOG_manystripclus53X;
   Int_t           Flag_ecalLaserCorrFilter;
   Int_t           Flag_trkPOG_toomanystripclus53X;
   Int_t           Flag_hcalLaserEventFilter;
   Int_t           Flag_trkPOG_logErrorTooManyClusters;
   Int_t           Flag_trkPOGFilters;
   Int_t           Flag_trackingFailureFilter;
   Int_t           Flag_CSCTightHalo2015Filter;
   Int_t           Flag_CSCTightHaloFilter;
   Int_t           Flag_globalTightHalo2016Filter;
   Int_t           Flag_globalSuperTightHalo2016Filter;
   Int_t           Flag_HBHENoiseFilter;
   Int_t           Flag_HBHENoiseIsoFilter;
   Int_t           Flag_goodVertices;
   Int_t           Flag_eeBadScFilter;
   Int_t           Flag_badMuonFilter;
   Int_t           Flag_badMuonFilterV2;
   Int_t           Flag_badMuons;
   Int_t           Flag_duplicateMuons;
   Int_t           Flag_noBadMuons;
   Int_t           Flag_badChargedHadronFilter;
   Int_t           Flag_badChargedHadronFilterV2;
   Int_t           Flag_METFilters;
   Int_t           HLT_PFHT800;
   Int_t           HLT_PFHT900;
   Int_t           HLT_PFMET170;
   Int_t           HLT_PFHT300_PFMET100;
   Int_t           HLT_PFHT300_PFMET110;
   Int_t           HLT_PFHT350_PFMET100;
   Int_t           HLT_PFHT350_PFMET120;
   Int_t           HLT_PFMETNoMu90_PFMHTNoMu90;
   Int_t           HLT_MonoCentralPFJet80_PFMETNoMu90_PFMHTNoMu90;
   Int_t           HLT_PFMETNoMu100_PFMHTNoMu100;
   Int_t           HLT_PFMETNoMu110_PFMHTNoMu110;
   Int_t           HLT_PFMETNoMu120_PFMHTNoMu120;
   Int_t           HLT_PFMET90_PFMHT90;
   Int_t           HLT_PFMET100_PFMHT100;
   Int_t           HLT_PFMET110_PFMHT110;
   Int_t           HLT_PFMET120_PFMHT120;
   Int_t           HLT_PFJet450;
   Int_t           HLT_PFJet500;
   Int_t           HLT_ECALHT800;
   Int_t           HLT_SingleMu;
   Int_t           HLT_SingleMu_NonIso;
   Int_t           HLT_SingleEl;
   Int_t           HLT_SingleEl_NonIso;
   Int_t           HLT_DoubleEl;
   Int_t           HLT_DoubleEl33;
   Int_t           HLT_MuX_Ele12;
   Int_t           HLT_Mu8_EleX;
   Int_t           HLT_Mu30_Ele30_NonIso;
   Int_t           HLT_Mu33_Ele33_NonIso;
   Int_t           HLT_DoubleMu;
   Int_t           HLT_DoubleMu_NonIso;
   Int_t           HLT_Photon120;
   Int_t           HLT_Photon165_HE10;
   Int_t           HLT_Photon250_NoHE;
   Int_t           HLT_PFHT125_Prescale;
   Int_t           HLT_PFHT200_Prescale;
   Int_t           HLT_PFHT300_Prescale;
   Int_t           HLT_PFHT350_Prescale;
   Int_t           HLT_PFHT475_Prescale;
   Int_t           HLT_PFHT600_Prescale;
   Int_t           HLT_DiCentralPFJet70_PFMET120;
   Int_t           HLT_DiCentralPFJet55_PFMET110;
   Int_t           nlep;
   Float_t         lep_pt[4];   //[nlep]
   Float_t         lep_eta[4];   //[nlep]
   Float_t         lep_phi[4];   //[nlep]
   Float_t         lep_mass[4];   //[nlep]
   Int_t           lep_charge[4];   //[nlep]
   Int_t           lep_pdgId[4];   //[nlep]
   Float_t         lep_dxy[4];   //[nlep]
   Float_t         lep_dz[4];   //[nlep]
   Int_t           lep_tightId[4];   //[nlep]
   Int_t           lep_heepId[4];   //[nlep]
   Float_t         lep_highPtFit_pt[4];   //[nlep]
   Float_t         lep_highPtFit_eta[4];   //[nlep]
   Float_t         lep_highPtFit_phi[4];   //[nlep]
   Float_t         lep_relIso03[4];   //[nlep]
   Float_t         lep_relIso04[4];   //[nlep]
   Float_t         lep_miniRelIso[4];   //[nlep]
   Float_t         lep_relIsoAn04[4];   //[nlep]
   Int_t           lep_mcMatchId[4];   //[nlep]
   Int_t           lep_lostHits[4];   //[nlep]
   Int_t           lep_convVeto[4];   //[nlep]
   Int_t           lep_tightCharge[4];   //[nlep]
   Int_t           nisoTrack;
   Float_t         isoTrack_pt[6];   //[nisoTrack]
   Float_t         isoTrack_eta[6];   //[nisoTrack]
   Float_t         isoTrack_phi[6];   //[nisoTrack]
   Float_t         isoTrack_mass[6];   //[nisoTrack]
   Float_t         isoTrack_absIso[6];   //[nisoTrack]
   Float_t         isoTrack_relIsoAn04[6];   //[nisoTrack]
   Float_t         isoTrack_dz[6];   //[nisoTrack]
   Int_t           isoTrack_pdgId[6];   //[nisoTrack]
   Int_t           isoTrack_mcMatchId[6];   //[nisoTrack]
   Int_t           nhighPtPFcands;
   Float_t         highPtPFcands_pt[4];   //[nhighPtPFcands]
   Float_t         highPtPFcands_eta[4];   //[nhighPtPFcands]
   Float_t         highPtPFcands_phi[4];   //[nhighPtPFcands]
   Float_t         highPtPFcands_mass[4];   //[nhighPtPFcands]
   Float_t         highPtPFcands_absIso[4];   //[nhighPtPFcands]
   Float_t         highPtPFcands_relIsoAn04[4];   //[nhighPtPFcands]
   Float_t         highPtPFcands_dz[4];   //[nhighPtPFcands]
   Int_t           highPtPFcands_pdgId[4];   //[nhighPtPFcands]
   Int_t           highPtPFcands_mcMatchId[4];   //[nhighPtPFcands]
   Int_t           nPFLep5LowMT;
   Int_t           nPFHad10LowMT;
   Int_t           ntau;
   Float_t         tau_pt[4];   //[ntau]
   Float_t         tau_eta[4];   //[ntau]
   Float_t         tau_phi[4];   //[ntau]
   Float_t         tau_mass[4];   //[ntau]
   Int_t           tau_charge[4];   //[ntau]
   Float_t         tau_dxy[4];   //[ntau]
   Float_t         tau_dz[4];   //[ntau]
   Int_t           tau_idCI3hit[4];   //[ntau]
   Float_t         tau_isoCI3hit[4];   //[ntau]
   Int_t           tau_mcMatchId[4];   //[ntau]
   Int_t           ngamma;
   Float_t         gamma_pt[5];   //[ngamma]
   Float_t         gamma_eta[5];   //[ngamma]
   Float_t         gamma_phi[5];   //[ngamma]
   Float_t         gamma_mass[5];   //[ngamma]
   Int_t           gamma_mcMatchId[5];   //[ngamma]
   Float_t         gamma_genIso04[5];   //[ngamma]
   Float_t         gamma_drMinParton[5];   //[ngamma]
   Float_t         gamma_chHadIso[5];   //[ngamma]
   Float_t         gamma_neuHadIso[5];   //[ngamma]
   Float_t         gamma_phIso[5];   //[ngamma]
   Float_t         gamma_sigmaIetaIeta[5];   //[ngamma]
   Float_t         gamma_r9[5];   //[ngamma]
   Float_t         gamma_hOverE[5];   //[ngamma]
   Float_t         gamma_hOverE015[5];   //[ngamma]
   Int_t           gamma_idCutBased[5];   //[ngamma]
   Float_t         gamma_mt2;
   Int_t           gamma_nJet30;
   Int_t           gamma_nJet40;
   Int_t           gamma_nJet30FailId;
   Int_t           gamma_nJet100FailId;
   Int_t           gamma_nBJet20;
   Int_t           gamma_nBJet20csv;
   Int_t           gamma_nBJet20mva;
   Int_t           gamma_nBJet25;
   Int_t           gamma_nBJet30;
   Int_t           gamma_nBJet40;
   Float_t         gamma_ht;
   Float_t         gamma_deltaPhiMin;
   Float_t         gamma_diffMetMht;
   Float_t         gamma_mht_pt;
   Float_t         gamma_mht_phi;
   Float_t         gamma_met_pt;
   Float_t         gamma_met_phi;
   Float_t         zll_mt2;
   Float_t         zll_deltaPhiMin;
   Float_t         zll_diffMetMht;
   Float_t         zll_met_pt;
   Float_t         zll_met_phi;
   Float_t         zll_mht_pt;
   Float_t         zll_mht_phi;
   Float_t         zll_mass;
   Float_t         zll_pt;
   Float_t         zll_eta;
   Float_t         zll_phi;
   Float_t         zll_ht;
   Float_t         zll_mt2JECup;
   Float_t         zll_deltaPhiMinJECup;
   Float_t         zll_diffMetMhtJECup;
   Float_t         zll_met_ptJECup;
   Float_t         zll_met_phiJECup;
   Float_t         zll_mht_ptJECup;
   Float_t         zll_mht_phiJECup;
   Float_t         zll_htJECup;
   Float_t         zll_mt2JECdn;
   Float_t         zll_deltaPhiMinJECdn;
   Float_t         zll_diffMetMhtJECdn;
   Float_t         zll_met_ptJECdn;
   Float_t         zll_met_phiJECdn;
   Float_t         zll_mht_ptJECdn;
   Float_t         zll_mht_phiJECdn;
   Float_t         zll_htJECdn;
   Float_t         zllmt_mt2;
   Float_t         zllmt_deltaPhiMin;
   Float_t         zllmt_diffMetMht;
   Float_t         zllmt_met_pt;
   Float_t         zllmt_met_phi;
   Float_t         zllmt_mht_pt;
   Float_t         zllmt_mht_phi;
   Float_t         zllmt_ht;
   Float_t         zllmt_mt;
   Float_t         rl_mt2;
   Float_t         rl_deltaPhiMin;
   Float_t         rl_diffMetMht;
   Float_t         rl_met_pt;
   Float_t         rl_met_phi;
   Float_t         rl_mht_pt;
   Float_t         rl_mht_phi;
   Float_t         rl_mass;
   Float_t         rl_pt;
   Float_t         rl_eta;
   Float_t         rl_phi;
   Float_t         rl_ht;
   Int_t           ngenLep;
   Float_t         genLep_pt[3];   //[ngenLep]
   Float_t         genLep_eta[3];   //[ngenLep]
   Float_t         genLep_phi[3];   //[ngenLep]
   Float_t         genLep_mass[3];   //[ngenLep]
   Int_t           genLep_pdgId[3];   //[ngenLep]
   Int_t           genLep_status[3];   //[ngenLep]
   Float_t         genLep_charge[3];   //[ngenLep]
   Int_t           genLep_sourceId[3];   //[ngenLep]
   Int_t           ngenStat23;
   Float_t         genStat23_pt[13];   //[ngenStat23]
   Float_t         genStat23_eta[13];   //[ngenStat23]
   Float_t         genStat23_phi[13];   //[ngenStat23]
   Float_t         genStat23_mass[13];   //[ngenStat23]
   Int_t           genStat23_pdgId[13];   //[ngenStat23]
   Int_t           genStat23_status[13];   //[ngenStat23]
   Float_t         genStat23_charge[13];   //[ngenStat23]
   Int_t           genStat23_sourceId[13];   //[ngenStat23]
   Int_t           ngenGamma;
   Float_t         genGamma_pt[7];   //[ngenGamma]
   Float_t         genGamma_eta[7];   //[ngenGamma]
   Float_t         genGamma_phi[7];   //[ngenGamma]
   Int_t           genGamma_motherId[7];   //[ngenGamma]
   Int_t           ngenTau;
   Int_t           ngenTau1Prong;
   Int_t           ngenTau3Prong;
   Float_t         genTau_pt[2];   //[ngenTau]
   Float_t         genTau_eta[2];   //[ngenTau]
   Float_t         genTau_phi[2];   //[ngenTau]
   Float_t         genTau_mass[2];   //[ngenTau]
   Int_t           genTau_pdgId[2];   //[ngenTau]
   Int_t           genTau_status[2];   //[ngenTau]
   Float_t         genTau_charge[2];   //[ngenTau]
   Int_t           genTau_sourceId[2];   //[ngenTau]
   Int_t           genTau_decayMode[2];   //[ngenTau]
   Float_t         genTau_leadTrackPt[2];   //[ngenTau]
   Int_t           genTau_neutralDaughters[2];   //[ngenTau]
   Int_t           ngenLepFromTau;
   Float_t         genLepFromTau_pt[3];   //[ngenLepFromTau]
   Float_t         genLepFromTau_eta[3];   //[ngenLepFromTau]
   Float_t         genLepFromTau_phi[3];   //[ngenLepFromTau]
   Float_t         genLepFromTau_mass[3];   //[ngenLepFromTau]
   Int_t           genLepFromTau_pdgId[3];   //[ngenLepFromTau]
   Int_t           genLepFromTau_status[3];   //[ngenLepFromTau]
   Float_t         genLepFromTau_charge[3];   //[ngenLepFromTau]
   Int_t           genLepFromTau_sourceId[3];   //[ngenLepFromTau]
   Int_t           ngenLepFromZ;
   Int_t           ngenNuFromZ;
   Int_t           GenSusyMScan1;
   Int_t           GenSusyMScan2;
   Int_t           GenSusyMScan3;
   Int_t           GenSusyMScan4;
   Int_t           nLHEweight;
   Float_t         LHEweight_wgt[9];   //[nLHEweight]
   Int_t           njet;
   Float_t         jet_pt[18];   //[njet]
   Float_t         jet_eta[18];   //[njet]
   Float_t         jet_phi[18];   //[njet]
   Float_t         jet_mass[18];   //[njet]
   Float_t         jet_btagCSV[18];   //[njet]
   Float_t         jet_btagMVA[18];   //[njet]
   Float_t         jet_chFrac[18];   //[njet]
   Float_t         jet_nhFrac[18];   //[njet]
   Float_t         jet_cemFrac[18];   //[njet]
   Float_t         jet_nemFrac[18];   //[njet]
   Float_t         jet_muFrac[18];   //[njet]
   Float_t         jet_rawPt[18];   //[njet]
   Float_t         jet_mcPt[18];   //[njet]
   Int_t           jet_mcFlavour[18];   //[njet]
   Int_t           jet_hadronFlavour[18];   //[njet]
   Float_t         jet_qgl[18];   //[njet]
   Float_t         jet_area[18];   //[njet]
   Int_t           jet_id[18];   //[njet]
   Int_t           jet_puId[18];   //[njet]
   Float_t         jet_muf[18];   //[njet]
   Float_t         weight_lepsf;
   Float_t         weight_lepsf_UP;
   Float_t         weight_lepsf_DN;
   Float_t         weight_lepsf_0l;
   Float_t         weight_lepsf_0l_UP;
   Float_t         weight_lepsf_0l_DN;
   Float_t         weight_btagsf;
   Float_t         weight_btagsf_heavy_UP;
   Float_t         weight_btagsf_light_UP;
   Float_t         weight_btagsf_heavy_DN;
   Float_t         weight_btagsf_light_DN;
   Float_t         weight_sigtrigsf;
   Float_t         weight_dileptrigsf;
   Float_t         weight_phottrigsf;
   Float_t         weight_pu;
   Float_t         weight_isr;
   Float_t         weight_isr_UP;
   Float_t         weight_isr_DN;
   Float_t         weight_scales_UP;
   Float_t         weight_scales_DN;
   Float_t         weight_pdfs_UP;
   Float_t         weight_pdfs_DN;
   Float_t         weight_toppt;
   Float_t         genRecoil_pt;
   Float_t         genTop_pt;
   Float_t         genTbar_pt;
   Int_t           genProd_pdgId;
   Float_t         weight_pol_L;
   Float_t         weight_pol_R;
   Int_t           nisrMatch;

   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_lumi;   //!
   TBranch        *b_evt;   //!
   TBranch        *b_isData;   //!
   TBranch        *b_isGolden;   //!
   TBranch        *b_evt_scale1fb;   //!
   TBranch        *b_evt_xsec;   //!
   TBranch        *b_evt_kfactor;   //!
   TBranch        *b_evt_filter;   //!
   TBranch        *b_evt_nEvts;   //!
   TBranch        *b_evt_id;   //!
   TBranch        *b_genWeight;   //!
   TBranch        *b_puWeight;   //!
   TBranch        *b_nVert;   //!
   TBranch        *b_nTrueInt;   //!
   TBranch        *b_rho;   //!
   TBranch        *b_nJet30;   //!
   TBranch        *b_nJet30JECup;   //!
   TBranch        *b_nJet30JECdn;   //!
   TBranch        *b_nJet40;   //!
   TBranch        *b_nBJet20;   //!
   TBranch        *b_nBJet20csv;   //!
   TBranch        *b_nBJet20mva;   //!
   TBranch        *b_nBJet20JECup;   //!
   TBranch        *b_nBJet20JECdn;   //!
   TBranch        *b_nBJet25;   //!
   TBranch        *b_nBJet30;   //!
   TBranch        *b_nBJet30csv;   //!
   TBranch        *b_nBJet30mva;   //!
   TBranch        *b_nBJet40;   //!
   TBranch        *b_nJet30FailId;   //!
   TBranch        *b_nJet100FailId;   //!
   TBranch        *b_nJet20BadFastsim;   //!
   TBranch        *b_nJet200MuFrac50DphiMet;   //!
   TBranch        *b_nMuons10;   //!
   TBranch        *b_nBadMuons20;   //!
   TBranch        *b_nElectrons10;   //!
   TBranch        *b_nLepLowMT;   //!
   TBranch        *b_nTaus20;   //!
   TBranch        *b_nGammas20;   //!
   TBranch        *b_nPFCHCand3;   //!
   TBranch        *b_deltaPhiMin;   //!
   TBranch        *b_deltaPhiMin_genmet;   //!
   TBranch        *b_diffMetMht;   //!
   TBranch        *b_diffMetMht_genmet;   //!
   TBranch        *b_deltaPhiMinJECup;   //!
   TBranch        *b_deltaPhiMinJECdn;   //!
   TBranch        *b_diffMetMhtJECup;   //!
   TBranch        *b_diffMetMhtJECdn;   //!
   TBranch        *b_minMTBMet;   //!
   TBranch        *b_zll_minMTBMet;   //!
   TBranch        *b_gamma_minMTBMet;   //!
   TBranch        *b_ht;   //!
   TBranch        *b_htJECup;   //!
   TBranch        *b_htJECdn;   //!
   TBranch        *b_mt2;   //!
   TBranch        *b_mt2JECup;   //!
   TBranch        *b_mt2JECdn;   //!
   TBranch        *b_mt2_gen;   //!
   TBranch        *b_mt2_genmet;   //!
   TBranch        *b_jet1_pt;   //!
   TBranch        *b_jet2_pt;   //!
   TBranch        *b_jet1_ptJECup;   //!
   TBranch        *b_jet2_ptJECup;   //!
   TBranch        *b_jet1_ptJECdn;   //!
   TBranch        *b_jet2_ptJECdn;   //!
   TBranch        *b_jet_failFSveto;   //!
   TBranch        *b_gamma_jet1_pt;   //!
   TBranch        *b_gamma_jet2_pt;   //!
   TBranch        *b_pseudoJet1_pt;   //!
   TBranch        *b_pseudoJet1_eta;   //!
   TBranch        *b_pseudoJet1_phi;   //!
   TBranch        *b_pseudoJet1_mass;   //!
   TBranch        *b_pseudoJet2_pt;   //!
   TBranch        *b_pseudoJet2_eta;   //!
   TBranch        *b_pseudoJet2_phi;   //!
   TBranch        *b_pseudoJet2_mass;   //!
   TBranch        *b_mht_pt;   //!
   TBranch        *b_mht_phi;   //!
   TBranch        *b_mht_ptJECup;   //!
   TBranch        *b_mht_phiJECup;   //!
   TBranch        *b_mht_ptJECdn;   //!
   TBranch        *b_mht_phiJECdn;   //!
   TBranch        *b_met_pt;   //!
   TBranch        *b_met_phi;   //!
   TBranch        *b_met_ptJECup;   //!
   TBranch        *b_met_phiJECup;   //!
   TBranch        *b_met_ptJECdn;   //!
   TBranch        *b_met_phiJECdn;   //!
   TBranch        *b_met_rawPt;   //!
   TBranch        *b_met_rawPhi;   //!
   TBranch        *b_met_caloPt;   //!
   TBranch        *b_met_caloPhi;   //!
   TBranch        *b_met_trkPt;   //!
   TBranch        *b_met_trkPhi;   //!
   TBranch        *b_met_genPt;   //!
   TBranch        *b_met_genPhi;   //!
   TBranch        *b_met_miniaodPt;   //!
   TBranch        *b_met_miniaodPhi;   //!
   TBranch        *b_Flag_EcalDeadCellTriggerPrimitiveFilter;   //!
   TBranch        *b_Flag_trkPOG_manystripclus53X;   //!
   TBranch        *b_Flag_ecalLaserCorrFilter;   //!
   TBranch        *b_Flag_trkPOG_toomanystripclus53X;   //!
   TBranch        *b_Flag_hcalLaserEventFilter;   //!
   TBranch        *b_Flag_trkPOG_logErrorTooManyClusters;   //!
   TBranch        *b_Flag_trkPOGFilters;   //!
   TBranch        *b_Flag_trackingFailureFilter;   //!
   TBranch        *b_Flag_CSCTightHalo2015Filter;   //!
   TBranch        *b_Flag_CSCTightHaloFilter;   //!
   TBranch        *b_Flag_globalTightHalo2016Filter;   //!
   TBranch        *b_Flag_globalSuperTightHalo2016Filter;   //!
   TBranch        *b_Flag_HBHENoiseFilter;   //!
   TBranch        *b_Flag_HBHENoiseIsoFilter;   //!
   TBranch        *b_Flag_goodVertices;   //!
   TBranch        *b_Flag_eeBadScFilter;   //!
   TBranch        *b_Flag_badMuonFilter;   //!
   TBranch        *b_Flag_badMuonFilterV2;   //!
   TBranch        *b_Flag_badMuons;   //!
   TBranch        *b_Flag_duplicateMuons;   //!
   TBranch        *b_Flag_noBadMuons;   //!
   TBranch        *b_Flag_badChargedHadronFilter;   //!
   TBranch        *b_Flag_badChargedHadronFilterV2;   //!
   TBranch        *b_Flag_METFilters;   //!
   TBranch        *b_HLT_PFHT800;   //!
   TBranch        *b_HLT_PFHT900;   //!
   TBranch        *b_HLT_PFMET170;   //!
   TBranch        *b_HLT_PFHT300_PFMET100;   //!
   TBranch        *b_HLT_PFHT300_PFMET110;   //!
   TBranch        *b_HLT_PFHT350_PFMET100;   //!
   TBranch        *b_HLT_PFHT350_PFMET120;   //!
   TBranch        *b_HLT_PFMETNoMu90_PFMHTNoMu90;   //!
   TBranch        *b_HLT_MonoCentralPFJet80_PFMETNoMu90_PFMHTNoMu90;   //!
   TBranch        *b_HLT_PFMETNoMu100_PFMHTNoMu100;   //!
   TBranch        *b_HLT_PFMETNoMu110_PFMHTNoMu110;   //!
   TBranch        *b_HLT_PFMETNoMu120_PFMHTNoMu120;   //!
   TBranch        *b_HLT_PFMET90_PFMHT90;   //!
   TBranch        *b_HLT_PFMET100_PFMHT100;   //!
   TBranch        *b_HLT_PFMET110_PFMHT110;   //!
   TBranch        *b_HLT_PFMET120_PFMHT120;   //!
   TBranch        *b_HLT_PFJet450;   //!
   TBranch        *b_HLT_PFJet500;   //!
   TBranch        *b_HLT_ECALHT800;   //!
   TBranch        *b_HLT_SingleMu;   //!
   TBranch        *b_HLT_SingleMu_NonIso;   //!
   TBranch        *b_HLT_SingleEl;   //!
   TBranch        *b_HLT_SingleEl_NonIso;   //!
   TBranch        *b_HLT_DoubleEl;   //!
   TBranch        *b_HLT_DoubleEl33;   //!
   TBranch        *b_HLT_MuX_Ele12;   //!
   TBranch        *b_HLT_Mu8_EleX;   //!
   TBranch        *b_HLT_Mu30_Ele30_NonIso;   //!
   TBranch        *b_HLT_Mu33_Ele33_NonIso;   //!
   TBranch        *b_HLT_DoubleMu;   //!
   TBranch        *b_HLT_DoubleMu_NonIso;   //!
   TBranch        *b_HLT_Photon120;   //!
   TBranch        *b_HLT_Photon165_HE10;   //!
   TBranch        *b_HLT_Photon250_NoHE;   //!
   TBranch        *b_HLT_PFHT125_Prescale;   //!
   TBranch        *b_HLT_PFHT200_Prescale;   //!
   TBranch        *b_HLT_PFHT300_Prescale;   //!
   TBranch        *b_HLT_PFHT350_Prescale;   //!
   TBranch        *b_HLT_PFHT475_Prescale;   //!
   TBranch        *b_HLT_PFHT600_Prescale;   //!
   TBranch        *b_HLT_DiCentralPFJet70_PFMET120;   //!
   TBranch        *b_HLT_DiCentralPFJet55_PFMET110;   //!
   TBranch        *b_nlep;   //!
   TBranch        *b_lep_pt;   //!
   TBranch        *b_lep_eta;   //!
   TBranch        *b_lep_phi;   //!
   TBranch        *b_lep_mass;   //!
   TBranch        *b_lep_charge;   //!
   TBranch        *b_lep_pdgId;   //!
   TBranch        *b_lep_dxy;   //!
   TBranch        *b_lep_dz;   //!
   TBranch        *b_lep_tightId;   //!
   TBranch        *b_lep_heepId;   //!
   TBranch        *b_lep_highPtFit_pt;   //!
   TBranch        *b_lep_highPtFit_eta;   //!
   TBranch        *b_lep_highPtFit_phi;   //!
   TBranch        *b_lep_relIso03;   //!
   TBranch        *b_lep_relIso04;   //!
   TBranch        *b_lep_miniRelIso;   //!
   TBranch        *b_lep_relIsoAn04;   //!
   TBranch        *b_lep_mcMatchId;   //!
   TBranch        *b_lep_lostHits;   //!
   TBranch        *b_lep_convVeto;   //!
   TBranch        *b_lep_tightCharge;   //!
   TBranch        *b_nisoTrack;   //!
   TBranch        *b_isoTrack_pt;   //!
   TBranch        *b_isoTrack_eta;   //!
   TBranch        *b_isoTrack_phi;   //!
   TBranch        *b_isoTrack_mass;   //!
   TBranch        *b_isoTrack_absIso;   //!
   TBranch        *b_isoTrack_relIsoAn04;   //!
   TBranch        *b_isoTrack_dz;   //!
   TBranch        *b_isoTrack_pdgId;   //!
   TBranch        *b_isoTrack_mcMatchId;   //!
   TBranch        *b_nhighPtPFcands;   //!
   TBranch        *b_highPtPFcands_pt;   //!
   TBranch        *b_highPtPFcands_eta;   //!
   TBranch        *b_highPtPFcands_phi;   //!
   TBranch        *b_highPtPFcands_mass;   //!
   TBranch        *b_highPtPFcands_absIso;   //!
   TBranch        *b_highPtPFcands_relIsoAn04;   //!
   TBranch        *b_highPtPFcands_dz;   //!
   TBranch        *b_highPtPFcands_pdgId;   //!
   TBranch        *b_highPtPFcands_mcMatchId;   //!
   TBranch        *b_nPFLep5LowMT;   //!
   TBranch        *b_nPFHad10LowMT;   //!
   TBranch        *b_ntau;   //!
   TBranch        *b_tau_pt;   //!
   TBranch        *b_tau_eta;   //!
   TBranch        *b_tau_phi;   //!
   TBranch        *b_tau_mass;   //!
   TBranch        *b_tau_charge;   //!
   TBranch        *b_tau_dxy;   //!
   TBranch        *b_tau_dz;   //!
   TBranch        *b_tau_idCI3hit;   //!
   TBranch        *b_tau_isoCI3hit;   //!
   TBranch        *b_tau_mcMatchId;   //!
   TBranch        *b_ngamma;   //!
   TBranch        *b_gamma_pt;   //!
   TBranch        *b_gamma_eta;   //!
   TBranch        *b_gamma_phi;   //!
   TBranch        *b_gamma_mass;   //!
   TBranch        *b_gamma_mcMatchId;   //!
   TBranch        *b_gamma_genIso04;   //!
   TBranch        *b_gamma_drMinParton;   //!
   TBranch        *b_gamma_chHadIso;   //!
   TBranch        *b_gamma_neuHadIso;   //!
   TBranch        *b_gamma_phIso;   //!
   TBranch        *b_gamma_sigmaIetaIeta;   //!
   TBranch        *b_gamma_r9;   //!
   TBranch        *b_gamma_hOverE;   //!
   TBranch        *b_gamma_hOverE015;   //!
   TBranch        *b_gamma_idCutBased;   //!
   TBranch        *b_gamma_mt2;   //!
   TBranch        *b_gamma_nJet30;   //!
   TBranch        *b_gamma_nJet40;   //!
   TBranch        *b_gamma_nJet30FailId;   //!
   TBranch        *b_gamma_nJet100FailId;   //!
   TBranch        *b_gamma_nBJet20;   //!
   TBranch        *b_gamma_nBJet20csv;   //!
   TBranch        *b_gamma_nBJet20mva;   //!
   TBranch        *b_gamma_nBJet25;   //!
   TBranch        *b_gamma_nBJet30;   //!
   TBranch        *b_gamma_nBJet40;   //!
   TBranch        *b_gamma_ht;   //!
   TBranch        *b_gamma_deltaPhiMin;   //!
   TBranch        *b_gamma_diffMetMht;   //!
   TBranch        *b_gamma_mht_pt;   //!
   TBranch        *b_gamma_mht_phi;   //!
   TBranch        *b_gamma_met_pt;   //!
   TBranch        *b_gamma_met_phi;   //!
   TBranch        *b_zll_mt2;   //!
   TBranch        *b_zll_deltaPhiMin;   //!
   TBranch        *b_zll_diffMetMht;   //!
   TBranch        *b_zll_met_pt;   //!
   TBranch        *b_zll_met_phi;   //!
   TBranch        *b_zll_mht_pt;   //!
   TBranch        *b_zll_mht_phi;   //!
   TBranch        *b_zll_mass;   //!
   TBranch        *b_zll_pt;   //!
   TBranch        *b_zll_eta;   //!
   TBranch        *b_zll_phi;   //!
   TBranch        *b_zll_ht;   //!
   TBranch        *b_zll_mt2JECup;   //!
   TBranch        *b_zll_deltaPhiMinJECup;   //!
   TBranch        *b_zll_diffMetMhtJECup;   //!
   TBranch        *b_zll_met_ptJECup;   //!
   TBranch        *b_zll_met_phiJECup;   //!
   TBranch        *b_zll_mht_ptJECup;   //!
   TBranch        *b_zll_mht_phiJECup;   //!
   TBranch        *b_zll_htJECup;   //!
   TBranch        *b_zll_mt2JECdn;   //!
   TBranch        *b_zll_deltaPhiMinJECdn;   //!
   TBranch        *b_zll_diffMetMhtJECdn;   //!
   TBranch        *b_zll_met_ptJECdn;   //!
   TBranch        *b_zll_met_phiJECdn;   //!
   TBranch        *b_zll_mht_ptJECdn;   //!
   TBranch        *b_zll_mht_phiJECdn;   //!
   TBranch        *b_zll_htJECdn;   //!
   TBranch        *b_zllmt_mt2;   //!
   TBranch        *b_zllmt_deltaPhiMin;   //!
   TBranch        *b_zllmt_diffMetMht;   //!
   TBranch        *b_zllmt_met_pt;   //!
   TBranch        *b_zllmt_met_phi;   //!
   TBranch        *b_zllmt_mht_pt;   //!
   TBranch        *b_zllmt_mht_phi;   //!
   TBranch        *b_zllmt_ht;   //!
   TBranch        *b_zllmt_mt;   //!
   TBranch        *b_rl_mt2;   //!
   TBranch        *b_rl_deltaPhiMin;   //!
   TBranch        *b_rl_diffMetMht;   //!
   TBranch        *b_rl_met_pt;   //!
   TBranch        *b_rl_met_phi;   //!
   TBranch        *b_rl_mht_pt;   //!
   TBranch        *b_rl_mht_phi;   //!
   TBranch        *b_rl_mass;   //!
   TBranch        *b_rl_pt;   //!
   TBranch        *b_rl_eta;   //!
   TBranch        *b_rl_phi;   //!
   TBranch        *b_rl_ht;   //!
   TBranch        *b_ngenLep;   //!
   TBranch        *b_genLep_pt;   //!
   TBranch        *b_genLep_eta;   //!
   TBranch        *b_genLep_phi;   //!
   TBranch        *b_genLep_mass;   //!
   TBranch        *b_genLep_pdgId;   //!
   TBranch        *b_genLep_status;   //!
   TBranch        *b_genLep_charge;   //!
   TBranch        *b_genLep_sourceId;   //!
   TBranch        *b_ngenStat23;   //!
   TBranch        *b_genStat23_pt;   //!
   TBranch        *b_genStat23_eta;   //!
   TBranch        *b_genStat23_phi;   //!
   TBranch        *b_genStat23_mass;   //!
   TBranch        *b_genStat23_pdgId;   //!
   TBranch        *b_genStat23_status;   //!
   TBranch        *b_genStat23_charge;   //!
   TBranch        *b_genStat23_sourceId;   //!
   TBranch        *b_ngenGamma;   //!
   TBranch        *b_genGamma_pt;   //!
   TBranch        *b_genGamma_eta;   //!
   TBranch        *b_genGamma_phi;   //!
   TBranch        *b_genGamma_motherId;   //!
   TBranch        *b_ngenTau;   //!
   TBranch        *b_ngenTau1Prong;   //!
   TBranch        *b_ngenTau3Prong;   //!
   TBranch        *b_genTau_pt;   //!
   TBranch        *b_genTau_eta;   //!
   TBranch        *b_genTau_phi;   //!
   TBranch        *b_genTau_mass;   //!
   TBranch        *b_genTau_pdgId;   //!
   TBranch        *b_genTau_status;   //!
   TBranch        *b_genTau_charge;   //!
   TBranch        *b_genTau_sourceId;   //!
   TBranch        *b_genTau_decayMode;   //!
   TBranch        *b_genTau_leadTrackPt;   //!
   TBranch        *b_genTau_neutralDaughters;   //!
   TBranch        *b_ngenLepFromTau;   //!
   TBranch        *b_genLepFromTau_pt;   //!
   TBranch        *b_genLepFromTau_eta;   //!
   TBranch        *b_genLepFromTau_phi;   //!
   TBranch        *b_genLepFromTau_mass;   //!
   TBranch        *b_genLepFromTau_pdgId;   //!
   TBranch        *b_genLepFromTau_status;   //!
   TBranch        *b_genLepFromTau_charge;   //!
   TBranch        *b_genLepFromTau_sourceId;   //!
   TBranch        *b_ngenLepFromZ;   //!
   TBranch        *b_ngenNuFromZ;   //!
   TBranch        *b_GenSusyMScan1;   //!
   TBranch        *b_GenSusyMScan2;   //!
   TBranch        *b_GenSusyMScan3;   //!
   TBranch        *b_GenSusyMScan4;   //!
   TBranch        *b_nLHEweight;   //!
   TBranch        *b_LHEweight_wgt;   //!
   TBranch        *b_njet;   //!
   TBranch        *b_jet_pt;   //!
   TBranch        *b_jet_eta;   //!
   TBranch        *b_jet_phi;   //!
   TBranch        *b_jet_mass;   //!
   TBranch        *b_jet_btagCSV;   //!
   TBranch        *b_jet_btagMVA;   //!
   TBranch        *b_jet_chFrac;   //!
   TBranch        *b_jet_nhFrac;   //!
   TBranch        *b_jet_cemFrac;   //!
   TBranch        *b_jet_nemFrac;   //!
   TBranch        *b_jet_muFrac;   //!
   TBranch        *b_jet_rawPt;   //!
   TBranch        *b_jet_mcPt;   //!
   TBranch        *b_jet_mcFlavour;   //!
   TBranch        *b_jet_hadronFlavour;   //!
   TBranch        *b_jet_qgl;   //!
   TBranch        *b_jet_area;   //!
   TBranch        *b_jet_id;   //!
   TBranch        *b_jet_puId;   //!
   TBranch        *b_jet_muf;   //!
   TBranch        *b_weight_lepsf;   //!
   TBranch        *b_weight_lepsf_UP;   //!
   TBranch        *b_weight_lepsf_DN;   //!
   TBranch        *b_weight_lepsf_0l;   //!
   TBranch        *b_weight_lepsf_0l_UP;   //!
   TBranch        *b_weight_lepsf_0l_DN;   //!
   TBranch        *b_weight_btagsf;   //!
   TBranch        *b_weight_btagsf_heavy_UP;   //!
   TBranch        *b_weight_btagsf_light_UP;   //!
   TBranch        *b_weight_btagsf_heavy_DN;   //!
   TBranch        *b_weight_btagsf_light_DN;   //!
   TBranch        *b_weight_sigtrigsf;   //!
   TBranch        *b_weight_dileptrigsf;   //!
   TBranch        *b_weight_phottrigsf;   //!
   TBranch        *b_weight_pu;   //!
   TBranch        *b_weight_isr;   //!
   TBranch        *b_weight_isr_UP;   //!
   TBranch        *b_weight_isr_DN;   //!
   TBranch        *b_weight_scales_UP;   //!
   TBranch        *b_weight_scales_DN;   //!
   TBranch        *b_weight_pdfs_UP;   //!
   TBranch        *b_weight_pdfs_DN;   //!
   TBranch        *b_weight_toppt;   //!
   TBranch        *b_genRecoil_pt;   //!
   TBranch        *b_genTop_pt;   //!
   TBranch        *b_genTbar_pt;   //!
   TBranch        *b_genProd_pdgId;   //!
   TBranch        *b_weight_pol_L;   //!
   TBranch        *b_weight_pol_R;   //!
   TBranch        *b_nisrMatch;   //!

   mt2tree(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~mt2tree() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   ClassDef(mt2tree,0);
};

#endif

#ifdef mt2tree_cxx
void mt2tree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("run", &run, &b_run);
   fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
   fChain->SetBranchAddress("evt", &evt, &b_evt);
   fChain->SetBranchAddress("isData", &isData, &b_isData);
   fChain->SetBranchAddress("isGolden", &isGolden, &b_isGolden);
   fChain->SetBranchAddress("evt_scale1fb", &evt_scale1fb, &b_evt_scale1fb);
   fChain->SetBranchAddress("evt_xsec", &evt_xsec, &b_evt_xsec);
   fChain->SetBranchAddress("evt_kfactor", &evt_kfactor, &b_evt_kfactor);
   fChain->SetBranchAddress("evt_filter", &evt_filter, &b_evt_filter);
   fChain->SetBranchAddress("evt_nEvts", &evt_nEvts, &b_evt_nEvts);
   fChain->SetBranchAddress("evt_id", &evt_id, &b_evt_id);
   fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
   fChain->SetBranchAddress("puWeight", &puWeight, &b_puWeight);
   fChain->SetBranchAddress("nVert", &nVert, &b_nVert);
   fChain->SetBranchAddress("nTrueInt", &nTrueInt, &b_nTrueInt);
   fChain->SetBranchAddress("rho", &rho, &b_rho);
   fChain->SetBranchAddress("nJet30", &nJet30, &b_nJet30);
   fChain->SetBranchAddress("nJet30JECup", &nJet30JECup, &b_nJet30JECup);
   fChain->SetBranchAddress("nJet30JECdn", &nJet30JECdn, &b_nJet30JECdn);
   fChain->SetBranchAddress("nJet40", &nJet40, &b_nJet40);
   fChain->SetBranchAddress("nBJet20", &nBJet20, &b_nBJet20);
   fChain->SetBranchAddress("nBJet20csv", &nBJet20csv, &b_nBJet20csv);
   fChain->SetBranchAddress("nBJet20mva", &nBJet20mva, &b_nBJet20mva);
   fChain->SetBranchAddress("nBJet20JECup", &nBJet20JECup, &b_nBJet20JECup);
   fChain->SetBranchAddress("nBJet20JECdn", &nBJet20JECdn, &b_nBJet20JECdn);
   fChain->SetBranchAddress("nBJet25", &nBJet25, &b_nBJet25);
   fChain->SetBranchAddress("nBJet30", &nBJet30, &b_nBJet30);
   fChain->SetBranchAddress("nBJet30csv", &nBJet30csv, &b_nBJet30csv);
   fChain->SetBranchAddress("nBJet30mva", &nBJet30mva, &b_nBJet30mva);
   fChain->SetBranchAddress("nBJet40", &nBJet40, &b_nBJet40);
   fChain->SetBranchAddress("nJet30FailId", &nJet30FailId, &b_nJet30FailId);
   fChain->SetBranchAddress("nJet100FailId", &nJet100FailId, &b_nJet100FailId);
   fChain->SetBranchAddress("nJet20BadFastsim", &nJet20BadFastsim, &b_nJet20BadFastsim);
   fChain->SetBranchAddress("nJet200MuFrac50DphiMet", &nJet200MuFrac50DphiMet, &b_nJet200MuFrac50DphiMet);
   fChain->SetBranchAddress("nMuons10", &nMuons10, &b_nMuons10);
   fChain->SetBranchAddress("nBadMuons20", &nBadMuons20, &b_nBadMuons20);
   fChain->SetBranchAddress("nElectrons10", &nElectrons10, &b_nElectrons10);
   fChain->SetBranchAddress("nLepLowMT", &nLepLowMT, &b_nLepLowMT);
   fChain->SetBranchAddress("nTaus20", &nTaus20, &b_nTaus20);
   fChain->SetBranchAddress("nGammas20", &nGammas20, &b_nGammas20);
   fChain->SetBranchAddress("nPFCHCand3", &nPFCHCand3, &b_nPFCHCand3);
   fChain->SetBranchAddress("deltaPhiMin", &deltaPhiMin, &b_deltaPhiMin);
   fChain->SetBranchAddress("deltaPhiMin_genmet", &deltaPhiMin_genmet, &b_deltaPhiMin_genmet);
   fChain->SetBranchAddress("diffMetMht", &diffMetMht, &b_diffMetMht);
   fChain->SetBranchAddress("diffMetMht_genmet", &diffMetMht_genmet, &b_diffMetMht_genmet);
   fChain->SetBranchAddress("deltaPhiMinJECup", &deltaPhiMinJECup, &b_deltaPhiMinJECup);
   fChain->SetBranchAddress("deltaPhiMinJECdn", &deltaPhiMinJECdn, &b_deltaPhiMinJECdn);
   fChain->SetBranchAddress("diffMetMhtJECup", &diffMetMhtJECup, &b_diffMetMhtJECup);
   fChain->SetBranchAddress("diffMetMhtJECdn", &diffMetMhtJECdn, &b_diffMetMhtJECdn);
   fChain->SetBranchAddress("minMTBMet", &minMTBMet, &b_minMTBMet);
   fChain->SetBranchAddress("zll_minMTBMet", &zll_minMTBMet, &b_zll_minMTBMet);
   fChain->SetBranchAddress("gamma_minMTBMet", &gamma_minMTBMet, &b_gamma_minMTBMet);
   fChain->SetBranchAddress("ht", &ht, &b_ht);
   fChain->SetBranchAddress("htJECup", &htJECup, &b_htJECup);
   fChain->SetBranchAddress("htJECdn", &htJECdn, &b_htJECdn);
   fChain->SetBranchAddress("mt2", &mt2, &b_mt2);
   fChain->SetBranchAddress("mt2JECup", &mt2JECup, &b_mt2JECup);
   fChain->SetBranchAddress("mt2JECdn", &mt2JECdn, &b_mt2JECdn);
   fChain->SetBranchAddress("mt2_gen", &mt2_gen, &b_mt2_gen);
   fChain->SetBranchAddress("mt2_genmet", &mt2_genmet, &b_mt2_genmet);
   fChain->SetBranchAddress("jet1_pt", &jet1_pt, &b_jet1_pt);
   fChain->SetBranchAddress("jet2_pt", &jet2_pt, &b_jet2_pt);
   fChain->SetBranchAddress("jet1_ptJECup", &jet1_ptJECup, &b_jet1_ptJECup);
   fChain->SetBranchAddress("jet2_ptJECup", &jet2_ptJECup, &b_jet2_ptJECup);
   fChain->SetBranchAddress("jet1_ptJECdn", &jet1_ptJECdn, &b_jet1_ptJECdn);
   fChain->SetBranchAddress("jet2_ptJECdn", &jet2_ptJECdn, &b_jet2_ptJECdn);
   fChain->SetBranchAddress("jet_failFSveto", &jet_failFSveto, &b_jet_failFSveto);
   fChain->SetBranchAddress("gamma_jet1_pt", &gamma_jet1_pt, &b_gamma_jet1_pt);
   fChain->SetBranchAddress("gamma_jet2_pt", &gamma_jet2_pt, &b_gamma_jet2_pt);
   fChain->SetBranchAddress("pseudoJet1_pt", &pseudoJet1_pt, &b_pseudoJet1_pt);
   fChain->SetBranchAddress("pseudoJet1_eta", &pseudoJet1_eta, &b_pseudoJet1_eta);
   fChain->SetBranchAddress("pseudoJet1_phi", &pseudoJet1_phi, &b_pseudoJet1_phi);
   fChain->SetBranchAddress("pseudoJet1_mass", &pseudoJet1_mass, &b_pseudoJet1_mass);
   fChain->SetBranchAddress("pseudoJet2_pt", &pseudoJet2_pt, &b_pseudoJet2_pt);
   fChain->SetBranchAddress("pseudoJet2_eta", &pseudoJet2_eta, &b_pseudoJet2_eta);
   fChain->SetBranchAddress("pseudoJet2_phi", &pseudoJet2_phi, &b_pseudoJet2_phi);
   fChain->SetBranchAddress("pseudoJet2_mass", &pseudoJet2_mass, &b_pseudoJet2_mass);
   fChain->SetBranchAddress("mht_pt", &mht_pt, &b_mht_pt);
   fChain->SetBranchAddress("mht_phi", &mht_phi, &b_mht_phi);
   fChain->SetBranchAddress("mht_ptJECup", &mht_ptJECup, &b_mht_ptJECup);
   fChain->SetBranchAddress("mht_phiJECup", &mht_phiJECup, &b_mht_phiJECup);
   fChain->SetBranchAddress("mht_ptJECdn", &mht_ptJECdn, &b_mht_ptJECdn);
   fChain->SetBranchAddress("mht_phiJECdn", &mht_phiJECdn, &b_mht_phiJECdn);
   fChain->SetBranchAddress("met_pt", &met_pt, &b_met_pt);
   fChain->SetBranchAddress("met_phi", &met_phi, &b_met_phi);
   fChain->SetBranchAddress("met_ptJECup", &met_ptJECup, &b_met_ptJECup);
   fChain->SetBranchAddress("met_phiJECup", &met_phiJECup, &b_met_phiJECup);
   fChain->SetBranchAddress("met_ptJECdn", &met_ptJECdn, &b_met_ptJECdn);
   fChain->SetBranchAddress("met_phiJECdn", &met_phiJECdn, &b_met_phiJECdn);
   fChain->SetBranchAddress("met_rawPt", &met_rawPt, &b_met_rawPt);
   fChain->SetBranchAddress("met_rawPhi", &met_rawPhi, &b_met_rawPhi);
   fChain->SetBranchAddress("met_caloPt", &met_caloPt, &b_met_caloPt);
   fChain->SetBranchAddress("met_caloPhi", &met_caloPhi, &b_met_caloPhi);
   fChain->SetBranchAddress("met_trkPt", &met_trkPt, &b_met_trkPt);
   fChain->SetBranchAddress("met_trkPhi", &met_trkPhi, &b_met_trkPhi);
   fChain->SetBranchAddress("met_genPt", &met_genPt, &b_met_genPt);
   fChain->SetBranchAddress("met_genPhi", &met_genPhi, &b_met_genPhi);
   fChain->SetBranchAddress("met_miniaodPt", &met_miniaodPt, &b_met_miniaodPt);
   fChain->SetBranchAddress("met_miniaodPhi", &met_miniaodPhi, &b_met_miniaodPhi);
   fChain->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilter", &Flag_EcalDeadCellTriggerPrimitiveFilter, &b_Flag_EcalDeadCellTriggerPrimitiveFilter);
   fChain->SetBranchAddress("Flag_trkPOG_manystripclus53X", &Flag_trkPOG_manystripclus53X, &b_Flag_trkPOG_manystripclus53X);
   fChain->SetBranchAddress("Flag_ecalLaserCorrFilter", &Flag_ecalLaserCorrFilter, &b_Flag_ecalLaserCorrFilter);
   fChain->SetBranchAddress("Flag_trkPOG_toomanystripclus53X", &Flag_trkPOG_toomanystripclus53X, &b_Flag_trkPOG_toomanystripclus53X);
   fChain->SetBranchAddress("Flag_hcalLaserEventFilter", &Flag_hcalLaserEventFilter, &b_Flag_hcalLaserEventFilter);
   fChain->SetBranchAddress("Flag_trkPOG_logErrorTooManyClusters", &Flag_trkPOG_logErrorTooManyClusters, &b_Flag_trkPOG_logErrorTooManyClusters);
   fChain->SetBranchAddress("Flag_trkPOGFilters", &Flag_trkPOGFilters, &b_Flag_trkPOGFilters);
   fChain->SetBranchAddress("Flag_trackingFailureFilter", &Flag_trackingFailureFilter, &b_Flag_trackingFailureFilter);
   fChain->SetBranchAddress("Flag_CSCTightHalo2015Filter", &Flag_CSCTightHalo2015Filter, &b_Flag_CSCTightHalo2015Filter);
   fChain->SetBranchAddress("Flag_CSCTightHaloFilter", &Flag_CSCTightHaloFilter, &b_Flag_CSCTightHaloFilter);
   fChain->SetBranchAddress("Flag_globalTightHalo2016Filter", &Flag_globalTightHalo2016Filter, &b_Flag_globalTightHalo2016Filter);
   fChain->SetBranchAddress("Flag_globalSuperTightHalo2016Filter", &Flag_globalSuperTightHalo2016Filter, &b_Flag_globalSuperTightHalo2016Filter);
   fChain->SetBranchAddress("Flag_HBHENoiseFilter", &Flag_HBHENoiseFilter, &b_Flag_HBHENoiseFilter);
   fChain->SetBranchAddress("Flag_HBHENoiseIsoFilter", &Flag_HBHENoiseIsoFilter, &b_Flag_HBHENoiseIsoFilter);
   fChain->SetBranchAddress("Flag_goodVertices", &Flag_goodVertices, &b_Flag_goodVertices);
   fChain->SetBranchAddress("Flag_eeBadScFilter", &Flag_eeBadScFilter, &b_Flag_eeBadScFilter);
   fChain->SetBranchAddress("Flag_badMuonFilter", &Flag_badMuonFilter, &b_Flag_badMuonFilter);
   fChain->SetBranchAddress("Flag_badMuonFilterV2", &Flag_badMuonFilterV2, &b_Flag_badMuonFilterV2);
   fChain->SetBranchAddress("Flag_badMuons", &Flag_badMuons, &b_Flag_badMuons);
   fChain->SetBranchAddress("Flag_duplicateMuons", &Flag_duplicateMuons, &b_Flag_duplicateMuons);
   fChain->SetBranchAddress("Flag_noBadMuons", &Flag_noBadMuons, &b_Flag_noBadMuons);
   fChain->SetBranchAddress("Flag_badChargedHadronFilter", &Flag_badChargedHadronFilter, &b_Flag_badChargedHadronFilter);
   fChain->SetBranchAddress("Flag_badChargedHadronFilterV2", &Flag_badChargedHadronFilterV2, &b_Flag_badChargedHadronFilterV2);
   fChain->SetBranchAddress("Flag_METFilters", &Flag_METFilters, &b_Flag_METFilters);
   fChain->SetBranchAddress("HLT_PFHT800", &HLT_PFHT800, &b_HLT_PFHT800);
   fChain->SetBranchAddress("HLT_PFHT900", &HLT_PFHT900, &b_HLT_PFHT900);
   fChain->SetBranchAddress("HLT_PFMET170", &HLT_PFMET170, &b_HLT_PFMET170);
   fChain->SetBranchAddress("HLT_PFHT300_PFMET100", &HLT_PFHT300_PFMET100, &b_HLT_PFHT300_PFMET100);
   fChain->SetBranchAddress("HLT_PFHT300_PFMET110", &HLT_PFHT300_PFMET110, &b_HLT_PFHT300_PFMET110);
   fChain->SetBranchAddress("HLT_PFHT350_PFMET100", &HLT_PFHT350_PFMET100, &b_HLT_PFHT350_PFMET100);
   fChain->SetBranchAddress("HLT_PFHT350_PFMET120", &HLT_PFHT350_PFMET120, &b_HLT_PFHT350_PFMET120);
   fChain->SetBranchAddress("HLT_PFMETNoMu90_PFMHTNoMu90", &HLT_PFMETNoMu90_PFMHTNoMu90, &b_HLT_PFMETNoMu90_PFMHTNoMu90);
   fChain->SetBranchAddress("HLT_MonoCentralPFJet80_PFMETNoMu90_PFMHTNoMu90", &HLT_MonoCentralPFJet80_PFMETNoMu90_PFMHTNoMu90, &b_HLT_MonoCentralPFJet80_PFMETNoMu90_PFMHTNoMu90);
   fChain->SetBranchAddress("HLT_PFMETNoMu100_PFMHTNoMu100", &HLT_PFMETNoMu100_PFMHTNoMu100, &b_HLT_PFMETNoMu100_PFMHTNoMu100);
   fChain->SetBranchAddress("HLT_PFMETNoMu110_PFMHTNoMu110", &HLT_PFMETNoMu110_PFMHTNoMu110, &b_HLT_PFMETNoMu110_PFMHTNoMu110);
   fChain->SetBranchAddress("HLT_PFMETNoMu120_PFMHTNoMu120", &HLT_PFMETNoMu120_PFMHTNoMu120, &b_HLT_PFMETNoMu120_PFMHTNoMu120);
   fChain->SetBranchAddress("HLT_PFMET90_PFMHT90", &HLT_PFMET90_PFMHT90, &b_HLT_PFMET90_PFMHT90);
   fChain->SetBranchAddress("HLT_PFMET100_PFMHT100", &HLT_PFMET100_PFMHT100, &b_HLT_PFMET100_PFMHT100);
   fChain->SetBranchAddress("HLT_PFMET110_PFMHT110", &HLT_PFMET110_PFMHT110, &b_HLT_PFMET110_PFMHT110);
   fChain->SetBranchAddress("HLT_PFMET120_PFMHT120", &HLT_PFMET120_PFMHT120, &b_HLT_PFMET120_PFMHT120);
   fChain->SetBranchAddress("HLT_PFJet450", &HLT_PFJet450, &b_HLT_PFJet450);
   fChain->SetBranchAddress("HLT_PFJet500", &HLT_PFJet500, &b_HLT_PFJet500);
   fChain->SetBranchAddress("HLT_ECALHT800", &HLT_ECALHT800, &b_HLT_ECALHT800);
   fChain->SetBranchAddress("HLT_SingleMu", &HLT_SingleMu, &b_HLT_SingleMu);
   fChain->SetBranchAddress("HLT_SingleMu_NonIso", &HLT_SingleMu_NonIso, &b_HLT_SingleMu_NonIso);
   fChain->SetBranchAddress("HLT_SingleEl", &HLT_SingleEl, &b_HLT_SingleEl);
   fChain->SetBranchAddress("HLT_SingleEl_NonIso", &HLT_SingleEl_NonIso, &b_HLT_SingleEl_NonIso);
   fChain->SetBranchAddress("HLT_DoubleEl", &HLT_DoubleEl, &b_HLT_DoubleEl);
   fChain->SetBranchAddress("HLT_DoubleEl33", &HLT_DoubleEl33, &b_HLT_DoubleEl33);
   fChain->SetBranchAddress("HLT_MuX_Ele12", &HLT_MuX_Ele12, &b_HLT_MuX_Ele12);
   fChain->SetBranchAddress("HLT_Mu8_EleX", &HLT_Mu8_EleX, &b_HLT_Mu8_EleX);
   fChain->SetBranchAddress("HLT_Mu30_Ele30_NonIso", &HLT_Mu30_Ele30_NonIso, &b_HLT_Mu30_Ele30_NonIso);
   fChain->SetBranchAddress("HLT_Mu33_Ele33_NonIso", &HLT_Mu33_Ele33_NonIso, &b_HLT_Mu33_Ele33_NonIso);
   fChain->SetBranchAddress("HLT_DoubleMu", &HLT_DoubleMu, &b_HLT_DoubleMu);
   fChain->SetBranchAddress("HLT_DoubleMu_NonIso", &HLT_DoubleMu_NonIso, &b_HLT_DoubleMu_NonIso);
   fChain->SetBranchAddress("HLT_Photon120", &HLT_Photon120, &b_HLT_Photon120);
   fChain->SetBranchAddress("HLT_Photon165_HE10", &HLT_Photon165_HE10, &b_HLT_Photon165_HE10);
   fChain->SetBranchAddress("HLT_Photon250_NoHE", &HLT_Photon250_NoHE, &b_HLT_Photon250_NoHE);
   fChain->SetBranchAddress("HLT_PFHT125_Prescale", &HLT_PFHT125_Prescale, &b_HLT_PFHT125_Prescale);
   fChain->SetBranchAddress("HLT_PFHT200_Prescale", &HLT_PFHT200_Prescale, &b_HLT_PFHT200_Prescale);
   fChain->SetBranchAddress("HLT_PFHT300_Prescale", &HLT_PFHT300_Prescale, &b_HLT_PFHT300_Prescale);
   fChain->SetBranchAddress("HLT_PFHT350_Prescale", &HLT_PFHT350_Prescale, &b_HLT_PFHT350_Prescale);
   fChain->SetBranchAddress("HLT_PFHT475_Prescale", &HLT_PFHT475_Prescale, &b_HLT_PFHT475_Prescale);
   fChain->SetBranchAddress("HLT_PFHT600_Prescale", &HLT_PFHT600_Prescale, &b_HLT_PFHT600_Prescale);
   fChain->SetBranchAddress("HLT_DiCentralPFJet70_PFMET120", &HLT_DiCentralPFJet70_PFMET120, &b_HLT_DiCentralPFJet70_PFMET120);
   fChain->SetBranchAddress("HLT_DiCentralPFJet55_PFMET110", &HLT_DiCentralPFJet55_PFMET110, &b_HLT_DiCentralPFJet55_PFMET110);
   fChain->SetBranchAddress("nlep", &nlep, &b_nlep);
   fChain->SetBranchAddress("lep_pt", lep_pt, &b_lep_pt);
   fChain->SetBranchAddress("lep_eta", lep_eta, &b_lep_eta);
   fChain->SetBranchAddress("lep_phi", lep_phi, &b_lep_phi);
   fChain->SetBranchAddress("lep_mass", lep_mass, &b_lep_mass);
   fChain->SetBranchAddress("lep_charge", lep_charge, &b_lep_charge);
   fChain->SetBranchAddress("lep_pdgId", lep_pdgId, &b_lep_pdgId);
   fChain->SetBranchAddress("lep_dxy", lep_dxy, &b_lep_dxy);
   fChain->SetBranchAddress("lep_dz", lep_dz, &b_lep_dz);
   fChain->SetBranchAddress("lep_tightId", lep_tightId, &b_lep_tightId);
   fChain->SetBranchAddress("lep_heepId", lep_heepId, &b_lep_heepId);
   fChain->SetBranchAddress("lep_highPtFit_pt", lep_highPtFit_pt, &b_lep_highPtFit_pt);
   fChain->SetBranchAddress("lep_highPtFit_eta", lep_highPtFit_eta, &b_lep_highPtFit_eta);
   fChain->SetBranchAddress("lep_highPtFit_phi", lep_highPtFit_phi, &b_lep_highPtFit_phi);
   fChain->SetBranchAddress("lep_relIso03", lep_relIso03, &b_lep_relIso03);
   fChain->SetBranchAddress("lep_relIso04", lep_relIso04, &b_lep_relIso04);
   fChain->SetBranchAddress("lep_miniRelIso", lep_miniRelIso, &b_lep_miniRelIso);
   fChain->SetBranchAddress("lep_relIsoAn04", lep_relIsoAn04, &b_lep_relIsoAn04);
   fChain->SetBranchAddress("lep_mcMatchId", lep_mcMatchId, &b_lep_mcMatchId);
   fChain->SetBranchAddress("lep_lostHits", lep_lostHits, &b_lep_lostHits);
   fChain->SetBranchAddress("lep_convVeto", lep_convVeto, &b_lep_convVeto);
   fChain->SetBranchAddress("lep_tightCharge", lep_tightCharge, &b_lep_tightCharge);
   fChain->SetBranchAddress("nisoTrack", &nisoTrack, &b_nisoTrack);
   fChain->SetBranchAddress("isoTrack_pt", isoTrack_pt, &b_isoTrack_pt);
   fChain->SetBranchAddress("isoTrack_eta", isoTrack_eta, &b_isoTrack_eta);
   fChain->SetBranchAddress("isoTrack_phi", isoTrack_phi, &b_isoTrack_phi);
   fChain->SetBranchAddress("isoTrack_mass", isoTrack_mass, &b_isoTrack_mass);
   fChain->SetBranchAddress("isoTrack_absIso", isoTrack_absIso, &b_isoTrack_absIso);
   fChain->SetBranchAddress("isoTrack_relIsoAn04", isoTrack_relIsoAn04, &b_isoTrack_relIsoAn04);
   fChain->SetBranchAddress("isoTrack_dz", isoTrack_dz, &b_isoTrack_dz);
   fChain->SetBranchAddress("isoTrack_pdgId", isoTrack_pdgId, &b_isoTrack_pdgId);
   fChain->SetBranchAddress("isoTrack_mcMatchId", isoTrack_mcMatchId, &b_isoTrack_mcMatchId);
   fChain->SetBranchAddress("nhighPtPFcands", &nhighPtPFcands, &b_nhighPtPFcands);
   fChain->SetBranchAddress("highPtPFcands_pt", highPtPFcands_pt, &b_highPtPFcands_pt);
   fChain->SetBranchAddress("highPtPFcands_eta", highPtPFcands_eta, &b_highPtPFcands_eta);
   fChain->SetBranchAddress("highPtPFcands_phi", highPtPFcands_phi, &b_highPtPFcands_phi);
   fChain->SetBranchAddress("highPtPFcands_mass", highPtPFcands_mass, &b_highPtPFcands_mass);
   fChain->SetBranchAddress("highPtPFcands_absIso", highPtPFcands_absIso, &b_highPtPFcands_absIso);
   fChain->SetBranchAddress("highPtPFcands_relIsoAn04", highPtPFcands_relIsoAn04, &b_highPtPFcands_relIsoAn04);
   fChain->SetBranchAddress("highPtPFcands_dz", highPtPFcands_dz, &b_highPtPFcands_dz);
   fChain->SetBranchAddress("highPtPFcands_pdgId", highPtPFcands_pdgId, &b_highPtPFcands_pdgId);
   fChain->SetBranchAddress("highPtPFcands_mcMatchId", highPtPFcands_mcMatchId, &b_highPtPFcands_mcMatchId);
   fChain->SetBranchAddress("nPFLep5LowMT", &nPFLep5LowMT, &b_nPFLep5LowMT);
   fChain->SetBranchAddress("nPFHad10LowMT", &nPFHad10LowMT, &b_nPFHad10LowMT);
   fChain->SetBranchAddress("ntau", &ntau, &b_ntau);
   fChain->SetBranchAddress("tau_pt", tau_pt, &b_tau_pt);
   fChain->SetBranchAddress("tau_eta", tau_eta, &b_tau_eta);
   fChain->SetBranchAddress("tau_phi", tau_phi, &b_tau_phi);
   fChain->SetBranchAddress("tau_mass", tau_mass, &b_tau_mass);
   fChain->SetBranchAddress("tau_charge", tau_charge, &b_tau_charge);
   fChain->SetBranchAddress("tau_dxy", tau_dxy, &b_tau_dxy);
   fChain->SetBranchAddress("tau_dz", tau_dz, &b_tau_dz);
   fChain->SetBranchAddress("tau_idCI3hit", tau_idCI3hit, &b_tau_idCI3hit);
   fChain->SetBranchAddress("tau_isoCI3hit", tau_isoCI3hit, &b_tau_isoCI3hit);
   fChain->SetBranchAddress("tau_mcMatchId", tau_mcMatchId, &b_tau_mcMatchId);
   fChain->SetBranchAddress("ngamma", &ngamma, &b_ngamma);
   fChain->SetBranchAddress("gamma_pt", gamma_pt, &b_gamma_pt);
   fChain->SetBranchAddress("gamma_eta", gamma_eta, &b_gamma_eta);
   fChain->SetBranchAddress("gamma_phi", gamma_phi, &b_gamma_phi);
   fChain->SetBranchAddress("gamma_mass", gamma_mass, &b_gamma_mass);
   fChain->SetBranchAddress("gamma_mcMatchId", gamma_mcMatchId, &b_gamma_mcMatchId);
   fChain->SetBranchAddress("gamma_genIso04", gamma_genIso04, &b_gamma_genIso04);
   fChain->SetBranchAddress("gamma_drMinParton", gamma_drMinParton, &b_gamma_drMinParton);
   fChain->SetBranchAddress("gamma_chHadIso", gamma_chHadIso, &b_gamma_chHadIso);
   fChain->SetBranchAddress("gamma_neuHadIso", gamma_neuHadIso, &b_gamma_neuHadIso);
   fChain->SetBranchAddress("gamma_phIso", gamma_phIso, &b_gamma_phIso);
   fChain->SetBranchAddress("gamma_sigmaIetaIeta", gamma_sigmaIetaIeta, &b_gamma_sigmaIetaIeta);
   fChain->SetBranchAddress("gamma_r9", gamma_r9, &b_gamma_r9);
   fChain->SetBranchAddress("gamma_hOverE", gamma_hOverE, &b_gamma_hOverE);
   fChain->SetBranchAddress("gamma_hOverE015", gamma_hOverE015, &b_gamma_hOverE015);
   fChain->SetBranchAddress("gamma_idCutBased", gamma_idCutBased, &b_gamma_idCutBased);
   fChain->SetBranchAddress("gamma_mt2", &gamma_mt2, &b_gamma_mt2);
   fChain->SetBranchAddress("gamma_nJet30", &gamma_nJet30, &b_gamma_nJet30);
   fChain->SetBranchAddress("gamma_nJet40", &gamma_nJet40, &b_gamma_nJet40);
   fChain->SetBranchAddress("gamma_nJet30FailId", &gamma_nJet30FailId, &b_gamma_nJet30FailId);
   fChain->SetBranchAddress("gamma_nJet100FailId", &gamma_nJet100FailId, &b_gamma_nJet100FailId);
   fChain->SetBranchAddress("gamma_nBJet20", &gamma_nBJet20, &b_gamma_nBJet20);
   fChain->SetBranchAddress("gamma_nBJet20csv", &gamma_nBJet20csv, &b_gamma_nBJet20csv);
   fChain->SetBranchAddress("gamma_nBJet20mva", &gamma_nBJet20mva, &b_gamma_nBJet20mva);
   fChain->SetBranchAddress("gamma_nBJet25", &gamma_nBJet25, &b_gamma_nBJet25);
   fChain->SetBranchAddress("gamma_nBJet30", &gamma_nBJet30, &b_gamma_nBJet30);
   fChain->SetBranchAddress("gamma_nBJet40", &gamma_nBJet40, &b_gamma_nBJet40);
   fChain->SetBranchAddress("gamma_ht", &gamma_ht, &b_gamma_ht);
   fChain->SetBranchAddress("gamma_deltaPhiMin", &gamma_deltaPhiMin, &b_gamma_deltaPhiMin);
   fChain->SetBranchAddress("gamma_diffMetMht", &gamma_diffMetMht, &b_gamma_diffMetMht);
   fChain->SetBranchAddress("gamma_mht_pt", &gamma_mht_pt, &b_gamma_mht_pt);
   fChain->SetBranchAddress("gamma_mht_phi", &gamma_mht_phi, &b_gamma_mht_phi);
   fChain->SetBranchAddress("gamma_met_pt", &gamma_met_pt, &b_gamma_met_pt);
   fChain->SetBranchAddress("gamma_met_phi", &gamma_met_phi, &b_gamma_met_phi);
   fChain->SetBranchAddress("zll_mt2", &zll_mt2, &b_zll_mt2);
   fChain->SetBranchAddress("zll_deltaPhiMin", &zll_deltaPhiMin, &b_zll_deltaPhiMin);
   fChain->SetBranchAddress("zll_diffMetMht", &zll_diffMetMht, &b_zll_diffMetMht);
   fChain->SetBranchAddress("zll_met_pt", &zll_met_pt, &b_zll_met_pt);
   fChain->SetBranchAddress("zll_met_phi", &zll_met_phi, &b_zll_met_phi);
   fChain->SetBranchAddress("zll_mht_pt", &zll_mht_pt, &b_zll_mht_pt);
   fChain->SetBranchAddress("zll_mht_phi", &zll_mht_phi, &b_zll_mht_phi);
   fChain->SetBranchAddress("zll_mass", &zll_mass, &b_zll_mass);
   fChain->SetBranchAddress("zll_pt", &zll_pt, &b_zll_pt);
   fChain->SetBranchAddress("zll_eta", &zll_eta, &b_zll_eta);
   fChain->SetBranchAddress("zll_phi", &zll_phi, &b_zll_phi);
   fChain->SetBranchAddress("zll_ht", &zll_ht, &b_zll_ht);
   fChain->SetBranchAddress("zll_mt2JECup", &zll_mt2JECup, &b_zll_mt2JECup);
   fChain->SetBranchAddress("zll_deltaPhiMinJECup", &zll_deltaPhiMinJECup, &b_zll_deltaPhiMinJECup);
   fChain->SetBranchAddress("zll_diffMetMhtJECup", &zll_diffMetMhtJECup, &b_zll_diffMetMhtJECup);
   fChain->SetBranchAddress("zll_met_ptJECup", &zll_met_ptJECup, &b_zll_met_ptJECup);
   fChain->SetBranchAddress("zll_met_phiJECup", &zll_met_phiJECup, &b_zll_met_phiJECup);
   fChain->SetBranchAddress("zll_mht_ptJECup", &zll_mht_ptJECup, &b_zll_mht_ptJECup);
   fChain->SetBranchAddress("zll_mht_phiJECup", &zll_mht_phiJECup, &b_zll_mht_phiJECup);
   fChain->SetBranchAddress("zll_htJECup", &zll_htJECup, &b_zll_htJECup);
   fChain->SetBranchAddress("zll_mt2JECdn", &zll_mt2JECdn, &b_zll_mt2JECdn);
   fChain->SetBranchAddress("zll_deltaPhiMinJECdn", &zll_deltaPhiMinJECdn, &b_zll_deltaPhiMinJECdn);
   fChain->SetBranchAddress("zll_diffMetMhtJECdn", &zll_diffMetMhtJECdn, &b_zll_diffMetMhtJECdn);
   fChain->SetBranchAddress("zll_met_ptJECdn", &zll_met_ptJECdn, &b_zll_met_ptJECdn);
   fChain->SetBranchAddress("zll_met_phiJECdn", &zll_met_phiJECdn, &b_zll_met_phiJECdn);
   fChain->SetBranchAddress("zll_mht_ptJECdn", &zll_mht_ptJECdn, &b_zll_mht_ptJECdn);
   fChain->SetBranchAddress("zll_mht_phiJECdn", &zll_mht_phiJECdn, &b_zll_mht_phiJECdn);
   fChain->SetBranchAddress("zll_htJECdn", &zll_htJECdn, &b_zll_htJECdn);
   fChain->SetBranchAddress("zllmt_mt2", &zllmt_mt2, &b_zllmt_mt2);
   fChain->SetBranchAddress("zllmt_deltaPhiMin", &zllmt_deltaPhiMin, &b_zllmt_deltaPhiMin);
   fChain->SetBranchAddress("zllmt_diffMetMht", &zllmt_diffMetMht, &b_zllmt_diffMetMht);
   fChain->SetBranchAddress("zllmt_met_pt", &zllmt_met_pt, &b_zllmt_met_pt);
   fChain->SetBranchAddress("zllmt_met_phi", &zllmt_met_phi, &b_zllmt_met_phi);
   fChain->SetBranchAddress("zllmt_mht_pt", &zllmt_mht_pt, &b_zllmt_mht_pt);
   fChain->SetBranchAddress("zllmt_mht_phi", &zllmt_mht_phi, &b_zllmt_mht_phi);
   fChain->SetBranchAddress("zllmt_ht", &zllmt_ht, &b_zllmt_ht);
   fChain->SetBranchAddress("zllmt_mt", &zllmt_mt, &b_zllmt_mt);
   fChain->SetBranchAddress("rl_mt2", &rl_mt2, &b_rl_mt2);
   fChain->SetBranchAddress("rl_deltaPhiMin", &rl_deltaPhiMin, &b_rl_deltaPhiMin);
   fChain->SetBranchAddress("rl_diffMetMht", &rl_diffMetMht, &b_rl_diffMetMht);
   fChain->SetBranchAddress("rl_met_pt", &rl_met_pt, &b_rl_met_pt);
   fChain->SetBranchAddress("rl_met_phi", &rl_met_phi, &b_rl_met_phi);
   fChain->SetBranchAddress("rl_mht_pt", &rl_mht_pt, &b_rl_mht_pt);
   fChain->SetBranchAddress("rl_mht_phi", &rl_mht_phi, &b_rl_mht_phi);
   fChain->SetBranchAddress("rl_mass", &rl_mass, &b_rl_mass);
   fChain->SetBranchAddress("rl_pt", &rl_pt, &b_rl_pt);
   fChain->SetBranchAddress("rl_eta", &rl_eta, &b_rl_eta);
   fChain->SetBranchAddress("rl_phi", &rl_phi, &b_rl_phi);
   fChain->SetBranchAddress("rl_ht", &rl_ht, &b_rl_ht);
   fChain->SetBranchAddress("ngenLep", &ngenLep, &b_ngenLep);
   fChain->SetBranchAddress("genLep_pt", genLep_pt, &b_genLep_pt);
   fChain->SetBranchAddress("genLep_eta", genLep_eta, &b_genLep_eta);
   fChain->SetBranchAddress("genLep_phi", genLep_phi, &b_genLep_phi);
   fChain->SetBranchAddress("genLep_mass", genLep_mass, &b_genLep_mass);
   fChain->SetBranchAddress("genLep_pdgId", genLep_pdgId, &b_genLep_pdgId);
   fChain->SetBranchAddress("genLep_status", genLep_status, &b_genLep_status);
   fChain->SetBranchAddress("genLep_charge", genLep_charge, &b_genLep_charge);
   fChain->SetBranchAddress("genLep_sourceId", genLep_sourceId, &b_genLep_sourceId);
   fChain->SetBranchAddress("ngenStat23", &ngenStat23, &b_ngenStat23);
   fChain->SetBranchAddress("genStat23_pt", genStat23_pt, &b_genStat23_pt);
   fChain->SetBranchAddress("genStat23_eta", genStat23_eta, &b_genStat23_eta);
   fChain->SetBranchAddress("genStat23_phi", genStat23_phi, &b_genStat23_phi);
   fChain->SetBranchAddress("genStat23_mass", genStat23_mass, &b_genStat23_mass);
   fChain->SetBranchAddress("genStat23_pdgId", genStat23_pdgId, &b_genStat23_pdgId);
   fChain->SetBranchAddress("genStat23_status", genStat23_status, &b_genStat23_status);
   fChain->SetBranchAddress("genStat23_charge", genStat23_charge, &b_genStat23_charge);
   fChain->SetBranchAddress("genStat23_sourceId", genStat23_sourceId, &b_genStat23_sourceId);
   fChain->SetBranchAddress("ngenGamma", &ngenGamma, &b_ngenGamma);
   fChain->SetBranchAddress("genGamma_pt", genGamma_pt, &b_genGamma_pt);
   fChain->SetBranchAddress("genGamma_eta", genGamma_eta, &b_genGamma_eta);
   fChain->SetBranchAddress("genGamma_phi", genGamma_phi, &b_genGamma_phi);
   fChain->SetBranchAddress("genGamma_motherId", genGamma_motherId, &b_genGamma_motherId);
   fChain->SetBranchAddress("ngenTau", &ngenTau, &b_ngenTau);
   fChain->SetBranchAddress("ngenTau1Prong", &ngenTau1Prong, &b_ngenTau1Prong);
   fChain->SetBranchAddress("ngenTau3Prong", &ngenTau3Prong, &b_ngenTau3Prong);
   fChain->SetBranchAddress("genTau_pt", genTau_pt, &b_genTau_pt);
   fChain->SetBranchAddress("genTau_eta", genTau_eta, &b_genTau_eta);
   fChain->SetBranchAddress("genTau_phi", genTau_phi, &b_genTau_phi);
   fChain->SetBranchAddress("genTau_mass", genTau_mass, &b_genTau_mass);
   fChain->SetBranchAddress("genTau_pdgId", genTau_pdgId, &b_genTau_pdgId);
   fChain->SetBranchAddress("genTau_status", genTau_status, &b_genTau_status);
   fChain->SetBranchAddress("genTau_charge", genTau_charge, &b_genTau_charge);
   fChain->SetBranchAddress("genTau_sourceId", genTau_sourceId, &b_genTau_sourceId);
   fChain->SetBranchAddress("genTau_decayMode", genTau_decayMode, &b_genTau_decayMode);
   fChain->SetBranchAddress("genTau_leadTrackPt", genTau_leadTrackPt, &b_genTau_leadTrackPt);
   fChain->SetBranchAddress("genTau_neutralDaughters", genTau_neutralDaughters, &b_genTau_neutralDaughters);
   fChain->SetBranchAddress("ngenLepFromTau", &ngenLepFromTau, &b_ngenLepFromTau);
   fChain->SetBranchAddress("genLepFromTau_pt", genLepFromTau_pt, &b_genLepFromTau_pt);
   fChain->SetBranchAddress("genLepFromTau_eta", genLepFromTau_eta, &b_genLepFromTau_eta);
   fChain->SetBranchAddress("genLepFromTau_phi", genLepFromTau_phi, &b_genLepFromTau_phi);
   fChain->SetBranchAddress("genLepFromTau_mass", genLepFromTau_mass, &b_genLepFromTau_mass);
   fChain->SetBranchAddress("genLepFromTau_pdgId", genLepFromTau_pdgId, &b_genLepFromTau_pdgId);
   fChain->SetBranchAddress("genLepFromTau_status", genLepFromTau_status, &b_genLepFromTau_status);
   fChain->SetBranchAddress("genLepFromTau_charge", genLepFromTau_charge, &b_genLepFromTau_charge);
   fChain->SetBranchAddress("genLepFromTau_sourceId", genLepFromTau_sourceId, &b_genLepFromTau_sourceId);
   fChain->SetBranchAddress("ngenLepFromZ", &ngenLepFromZ, &b_ngenLepFromZ);
   fChain->SetBranchAddress("ngenNuFromZ", &ngenNuFromZ, &b_ngenNuFromZ);
   fChain->SetBranchAddress("GenSusyMScan1", &GenSusyMScan1, &b_GenSusyMScan1);
   fChain->SetBranchAddress("GenSusyMScan2", &GenSusyMScan2, &b_GenSusyMScan2);
   fChain->SetBranchAddress("GenSusyMScan3", &GenSusyMScan3, &b_GenSusyMScan3);
   fChain->SetBranchAddress("GenSusyMScan4", &GenSusyMScan4, &b_GenSusyMScan4);
   fChain->SetBranchAddress("nLHEweight", &nLHEweight, &b_nLHEweight);
   fChain->SetBranchAddress("LHEweight_wgt", LHEweight_wgt, &b_LHEweight_wgt);
   fChain->SetBranchAddress("njet", &njet, &b_njet);
   fChain->SetBranchAddress("jet_pt", jet_pt, &b_jet_pt);
   fChain->SetBranchAddress("jet_eta", jet_eta, &b_jet_eta);
   fChain->SetBranchAddress("jet_phi", jet_phi, &b_jet_phi);
   fChain->SetBranchAddress("jet_mass", jet_mass, &b_jet_mass);
   fChain->SetBranchAddress("jet_btagCSV", jet_btagCSV, &b_jet_btagCSV);
   fChain->SetBranchAddress("jet_btagMVA", jet_btagMVA, &b_jet_btagMVA);
   fChain->SetBranchAddress("jet_chFrac", jet_chFrac, &b_jet_chFrac);
   fChain->SetBranchAddress("jet_nhFrac", jet_nhFrac, &b_jet_nhFrac);
   fChain->SetBranchAddress("jet_cemFrac", jet_cemFrac, &b_jet_cemFrac);
   fChain->SetBranchAddress("jet_nemFrac", jet_nemFrac, &b_jet_nemFrac);
   fChain->SetBranchAddress("jet_muFrac", jet_muFrac, &b_jet_muFrac);
   fChain->SetBranchAddress("jet_rawPt", jet_rawPt, &b_jet_rawPt);
   fChain->SetBranchAddress("jet_mcPt", jet_mcPt, &b_jet_mcPt);
   fChain->SetBranchAddress("jet_mcFlavour", jet_mcFlavour, &b_jet_mcFlavour);
   fChain->SetBranchAddress("jet_hadronFlavour", jet_hadronFlavour, &b_jet_hadronFlavour);
   fChain->SetBranchAddress("jet_qgl", jet_qgl, &b_jet_qgl);
   fChain->SetBranchAddress("jet_area", jet_area, &b_jet_area);
   fChain->SetBranchAddress("jet_id", jet_id, &b_jet_id);
   fChain->SetBranchAddress("jet_puId", jet_puId, &b_jet_puId);
   fChain->SetBranchAddress("jet_muf", jet_muf, &b_jet_muf);
   fChain->SetBranchAddress("weight_lepsf", &weight_lepsf, &b_weight_lepsf);
   fChain->SetBranchAddress("weight_lepsf_UP", &weight_lepsf_UP, &b_weight_lepsf_UP);
   fChain->SetBranchAddress("weight_lepsf_DN", &weight_lepsf_DN, &b_weight_lepsf_DN);
   fChain->SetBranchAddress("weight_lepsf_0l", &weight_lepsf_0l, &b_weight_lepsf_0l);
   fChain->SetBranchAddress("weight_lepsf_0l_UP", &weight_lepsf_0l_UP, &b_weight_lepsf_0l_UP);
   fChain->SetBranchAddress("weight_lepsf_0l_DN", &weight_lepsf_0l_DN, &b_weight_lepsf_0l_DN);
   fChain->SetBranchAddress("weight_btagsf", &weight_btagsf, &b_weight_btagsf);
   fChain->SetBranchAddress("weight_btagsf_heavy_UP", &weight_btagsf_heavy_UP, &b_weight_btagsf_heavy_UP);
   fChain->SetBranchAddress("weight_btagsf_light_UP", &weight_btagsf_light_UP, &b_weight_btagsf_light_UP);
   fChain->SetBranchAddress("weight_btagsf_heavy_DN", &weight_btagsf_heavy_DN, &b_weight_btagsf_heavy_DN);
   fChain->SetBranchAddress("weight_btagsf_light_DN", &weight_btagsf_light_DN, &b_weight_btagsf_light_DN);
   fChain->SetBranchAddress("weight_sigtrigsf", &weight_sigtrigsf, &b_weight_sigtrigsf);
   fChain->SetBranchAddress("weight_dileptrigsf", &weight_dileptrigsf, &b_weight_dileptrigsf);
   fChain->SetBranchAddress("weight_phottrigsf", &weight_phottrigsf, &b_weight_phottrigsf);
   fChain->SetBranchAddress("weight_pu", &weight_pu, &b_weight_pu);
   fChain->SetBranchAddress("weight_isr", &weight_isr, &b_weight_isr);
   fChain->SetBranchAddress("weight_isr_UP", &weight_isr_UP, &b_weight_isr_UP);
   fChain->SetBranchAddress("weight_isr_DN", &weight_isr_DN, &b_weight_isr_DN);
   fChain->SetBranchAddress("weight_scales_UP", &weight_scales_UP, &b_weight_scales_UP);
   fChain->SetBranchAddress("weight_scales_DN", &weight_scales_DN, &b_weight_scales_DN);
   fChain->SetBranchAddress("weight_pdfs_UP", &weight_pdfs_UP, &b_weight_pdfs_UP);
   fChain->SetBranchAddress("weight_pdfs_DN", &weight_pdfs_DN, &b_weight_pdfs_DN);
   fChain->SetBranchAddress("weight_toppt", &weight_toppt, &b_weight_toppt);
   fChain->SetBranchAddress("genRecoil_pt", &genRecoil_pt, &b_genRecoil_pt);
   fChain->SetBranchAddress("genTop_pt", &genTop_pt, &b_genTop_pt);
   fChain->SetBranchAddress("genTbar_pt", &genTbar_pt, &b_genTbar_pt);
   fChain->SetBranchAddress("genProd_pdgId", &genProd_pdgId, &b_genProd_pdgId);
   fChain->SetBranchAddress("weight_pol_L", &weight_pol_L, &b_weight_pol_L);
   fChain->SetBranchAddress("weight_pol_R", &weight_pol_R, &b_weight_pol_R);
   fChain->SetBranchAddress("nisrMatch", &nisrMatch, &b_nisrMatch);
}

Bool_t mt2tree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef mt2tree_cxx
