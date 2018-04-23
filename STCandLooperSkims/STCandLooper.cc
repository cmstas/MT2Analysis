#include "STCandLooper.h"

using namespace std;

class mt2tree;
class sttree;

TFile VetoFile("VetoEtaPhi/VetoHists.root");
TH1F* veto_bar = (TH1F*) VetoFile.Get("h_VetoEtaPhi_bar");
TH1F* veto_ecp = (TH1F*) VetoFile.Get("h_VetoEtaPhi_ecp");
TH1F* veto_ecn = (TH1F*) VetoFile.Get("h_VetoEtaPhi_ecn");

int STCandLooper::InEtaPhiVetoRegion(float eta, float phi) {
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
  return 0;
}

int STCandLooper::loop (char * infile_name, char * sample, int selectionMode, int vetoMode) {

  bool doHT = selectionMode == 1;
  bool doCRSL = selectionMode == 2;
  bool doSR = selectionMode == 3;

  bool doGenVeto = vetoMode == 1;
  bool doEtaPhiVeto = vetoMode == 2;

  if (vetoMode < 0) {
    doGenVeto = true;
    doEtaPhiVeto = true;
  }

  string output_suffix("");
  if (doHT) output_suffix = "_HT";
  else if (doCRSL) output_suffix = "_CRSL";
  else if (doSR) output_suffix = "_SR";
    
  if (doGenVeto) output_suffix += "_GV";
  if (doEtaPhiVeto) output_suffix += "_EPV";

  string mkdir = "mkdir -p output" + output_suffix;
  system(mkdir.c_str());
  TString output_name = Form("output%s/%s.root",output_suffix.c_str(),sample);
  //TString output_name = Form("output_test/%s.root",sample);

  // Containers for our trees
  TChain* ch_mt2 = new TChain("mt2st");
  TChain* ch_st = new TChain("st");

  cout << "Input file: " << infile_name << endl;
  ch_mt2->Add( infile_name ); // Load mt2 inputs
  ch_st->Add( infile_name ); // Load short track inputs

  // Get file Content
  TTree *tree_mt2 = (TTree*)ch_mt2->Clone("mt2st");
  TTree *tree_st = (TTree*)ch_st->Clone("st");

  /*
  tree_mt2->SetBranchStatus("*",0);
  tree_mt2->SetBranchStatus("run",1);
  tree_mt2->SetBranchStatus("lumi",1);
  tree_mt2->SetBranchStatus("evt",1);
  tree_mt2->SetBranchStatus("nVert",1);
  tree_mt2->SetBranchStatus("met_pt",1);
  tree_mt2->SetBranchStatus("ht",1);
  tree_mt2->SetBranchStatus("lep_pt",1);
  tree_mt2->SetBranchStatus("lep_eta",1);
  tree_mt2->SetBranchStatus("lep_phi",1);
  tree_mt2->SetBranchStatus("lep_pdgId",1);
  tree_mt2->SetBranchStatus("genLep_pt",1);
  tree_mt2->SetBranchStatus("genLep_eta",1);
  tree_mt2->SetBranchStatus("genLep_phi",1);
  tree_mt2->SetBranchStatus("genLep_pdgId",1);
  tree_mt2->SetBranchStatus("genLep_sourceId",1);
  tree_mt2->SetBranchStatus("genTau_pt",1);
  tree_mt2->SetBranchStatus("genTau_eta",1);
  tree_mt2->SetBranchStatus("genTau_phi",1);
  tree_mt2->SetBranchStatus("genTau_pdgId",1);
  tree_mt2->SetBranchStatus("genTau_sourceId",1);
  tree_mt2->SetBranchStatus("isoTrack_pt",1);
  tree_mt2->SetBranchStatus("isoTrack_eta",1);
  tree_mt2->SetBranchStatus("isoTrack_phi",1);
  tree_mt2->SetBranchStatus("isoTrack_pdgId",1);
  tree_mt2->SetBranchStatus("jet_pt",1);
  tree_mt2->SetBranchStatus("njet",1);
  tree_mt2->SetBranchStatus("nLepLowMT",1);
  tree_st->SetBranchStatus("*",0);
  tree_st->SetBranchStatus("track_*",1);
  tree_st->SetBranchStatus("ntracks",1);
  */

  // sttree is a subtype of mt2tree. mt2tree functions can be called on an sttree, but not the reverse.
  // This line incorporates branches from tree_mt2 into tree_st.
  tree_st->AddFriend(tree_mt2);

  sttree t; // See MT2CORE/sttree.h

  // Our baby tree is an sttree so that we can call both mt2 and st methods mindlessly.
  t.Init(tree_st); // The Init function from mt2tree, for mt2 branches.
  t.Init_ST(tree_st); // The Init function from sttree, for st branches.

  // Event Loop
  // Sanity check:
  const unsigned int nEventsTree = tree_mt2->GetEntries();
  if (nEventsTree != tree_st->GetEntries()) {
    cout << "st and mt2 trees have different numbers of entries. This should not be possible if the input file was produced by treefriend.py and never edited. Aborting..." << endl;
    return 2;
  }

  // For short track
  const float pt_min_st = 15;
  const float dxy_max_st = 0.02;
  const float dz_max_st = 0.05;
  const float iso_max_st = 10;
  const float reliso_max_st = 0.2;
  const float niso_max_st = 10;
  const float chiso_max_st = 10;
  const float nreliso_max_st = 0.1;
  const float chreliso_max_st = 0.1;
  // For cands
  const float pt_min = 15;
  const float dxy_max = 0.06;
  const float dz_max = 0.15;
  const float iso_max = 30;
  const float reliso_max = 0.6;
  const float niso_max = 30;
  const float chiso_max = 30;
  const float nreliso_max = 0.3;
  const float chreliso_max = 0.3;

  // Book Histograms
  TH1::SetDefaultSumw2(true); // Makes histograms do proper error calculation automatically
  TH1F h_stc("h_stc","Short Track Candidate Count (All Selections)",4,0,4);
  TH1F h_st("h_st","True Short Track Count (All Selections)",4,0,4);

  // none, e, mu, taue, taumu, tau1, tau3, 
  // ee, emu, etaue, etaumu, etau1, etau3, 
  // mumu, mutaue, mutaumu, mutau1, mutau3, 
  // tauetaue, tauetaumu, tauetau1, tauetau3, 
  // taumutaumu, taumutau1, taumutau3, 
  // tau1tau1, tau1tau3, 
  // tau3tau3
  TH1F h_lepType("h_lepType","Lepton Type",28,0,28);
  TH1F h_ngenLep("h_ngenLep","ngenLep in N_{STC} > 0 Events",6,0,6);

  const float pi = TMath::Pi();
  TH2F h_etaphi_bar("h_etaphi_bar","All Tracks by #eta and #phi",170,-1.4,1.4,360,-pi,pi);
  TH2F h_st_etaphi_bar("h_st_etaphi_bar","Long Short Tracks by #eta and #phi",170,-1.4,1.4,360,-pi,pi);
  TH2F h_etaphi_ecn("h_etaphi_ecn","All Tracks by #eta and #phi",100,-2.4,-1.4,100,-pi,pi);
  TH2F h_st_etaphi_ecn("h_st_etaphi_ecn","Long Short Tracks by #eta and #phi",100,-2.4,-1.4,100,-pi,pi);
  TH2F h_etaphi_ecp("h_etaphi_ecp","All Tracks by #eta and #phi",100,1.4,2.4,100,-pi,pi);
  TH2F h_st_etaphi_ecp("h_st_etaphi_ecp","Long Short Tracks by #eta and #phi",100,1.4,2.4,100,-pi,pi);

  TH1F h_inclusive("h_inclusive","Total Events in Sample",1,0,1);

  TH1F h_el_dr("h_el_dr","Nearest Lepton #DeltaR (e)",50,0,1.0);
  TH1F h_mu_dr("h_mu_dr","Nearest Lepton #DeltaR (#mu)",50,0,1.0);
  TH1F h_tau_dr("h_tau_dr","Nearest Lepton #DeltaR (#tau)",50,0,1.0);

  TH1F h_el_ptr("h_el_ptr","#Deltap_{T}/p_{T}^{trk} (e)",10,-1,1);
  TH1F h_mu_ptr("h_mu_ptr","#Deltap_{T}/p_{T}^{trk} (#mu)",10,-1,1);
  TH1F h_tau_ptr("h_tau_ptr","#Deltap_{T}/p_{T}^{trk} (#tau)",10,-1,1);

  TH1F h_sh_el_dr("h_sh_el_dr","(ST) Nearest Lepton #DeltaR (e)",50,0,1.0);
  TH1F h_sh_mu_dr("h_sh_mu_dr","(ST) Nearest Lepton #DeltaR (#mu)",50,0,1.0);
  TH1F h_sh_tau_dr("h_sh_tau_dr","(ST) Nearest Lepton #DeltaR (#tau)",50,0,1.0);

  TH1F h_sh_el_ptr("h_sh_el_ptr","(ST) #Deltap_{T}/p_{T}^{trk} (e)",10,-1,1);
  TH1F h_sh_mu_ptr("h_sh_mu_ptr","(ST) #Deltap_{T}/p_{T}^{trk} (#mu)",10,-1,1);
  TH1F h_sh_tau_ptr("h_sh_tau_ptr","(ST) #Deltap_{T}/p_{T}^{trk} (#tau)",10,-1,1);

  const int pt_n = 30;  const float pt_max = 80;
  TH1F h_pt("h_pt","Distribution of Track p_{T}s",pt_n,pt_min,pt_max);
  TH1F h_pt_p("h_pt_p","Distribution of Track p_{T}s (pixel-only)",pt_n,pt_min,pt_max);
  TH1F h_pt_s("h_pt_s","Distribution of Track p_{T}s (not pixel-only, < 7 layers)",pt_n,pt_min,pt_max);
  TH1F h_pt_l("h_pt_l","Distribution of Track p_{T}s (>= 7 layers, > 1 lost outer hits)",pt_n,pt_min,pt_max);
  //TH1F h_pt_li("h_pt_li","Distribution of Track p_{T}s (>=7 layers)",pt_n,pt_min,pt_max);

  const int dxy_n = 40; const float dxy_min = 0;
  TH1F h_bardxy("h_bardxy","Distribution of Barrel Track dxy",dxy_n,dxy_min,dxy_max);
  TH1F h_bardxy_p("h_bardxy_p","Distribution of Barrel Track dxy (pixel-only)",dxy_n,dxy_min,dxy_max);
  TH1F h_bardxy_s("h_bardxy_s","Distribution of Barrel Track dxy (not pixel-only, < 7 layers)",dxy_n,dxy_min,dxy_max);
  TH1F h_bardxy_l("h_bardxy_l","Distribution of Barrel Track dxy (>= 7 layers, > 1 lost outer hits)",dxy_n,dxy_min,dxy_max);
  //TH1F h_bardxy_li("h_bardxy_li","Distribution of Barrel Track dxy (>=7 layers)",dxy_n,dxy_min,dxy_max);

  const int dz_n = 30; const float dz_min = 0; 
  TH1F h_bardz("h_bardz","Distribution of Barrel Track dz",dz_n,dz_min,dz_max);
  TH1F h_bardz_p("h_bardz_p","Distribution of Barrel Track dz (pixel-only)",dz_n,dz_min,dz_max);
  TH1F h_bardz_s("h_bardz_s","Distribution of Barrel Track dz (not pixel-only, < 7 layers)",dz_n,dz_min,dz_max);
  TH1F h_bardz_l("h_bardz_l","Distribution of Barrel Track dz (>= 7 layers, > 1 lost outer hits)",dz_n,dz_min,dz_max);
  //TH1F h_bardz_li("h_ecdz_li","Distribution of Barrel Track dz (>=7 layers)",dz_n,dz_min,dz_max);

  TH1F h_ecdxy("h_ecdxy","Distribution of Endcap Track dxy",dxy_n,dxy_min,dxy_max);
  TH1F h_ecdxy_p("h_ecdxy_p","Distribution of Endcap Track dxy (pixel-only)",dxy_n,dxy_min,dxy_max);
  TH1F h_ecdxy_s("h_ecdxy_s","Distribution of Endcap Track dxy (not pixel-only, < 7 layers)",dxy_n,dxy_min,dxy_max);
  TH1F h_ecdxy_l("h_ecdxy_l","Distribution of Endcap Track dxy (>= 7 layers, > 1 lost outer hits)",dxy_n,dxy_min,dxy_max);
  //TH1F h_ecdxy_li("h_ecdxy_li","Distribution of Endcap Track dxy (>=7 layers)",dxy_n,dxy_min,dxy_max);

  TH1F h_ecdz("h_ecdz","Distribution of Endcap Track dz",dz_n,dz_min,dz_max);
  TH1F h_ecdz_p("h_ecdz_p","Distribution of Endcap Track dz (pixel-only)",dz_n,dz_min,dz_max);
  TH1F h_ecdz_s("h_ecdz_s","Distribution of Endcap Track dz (not pixel-only, < 7 layers)",dz_n,dz_min,dz_max);
  TH1F h_ecdz_l("h_ecdz_l","Distribution of Endcap Track dz (>= 7 layers, > 1 lost outer hits)",dz_n,dz_min,dz_max);
  //TH1F h_ecdz_li("h_ecdz_li","Distribution of Endcap Track dz (>=7 layers)",dz_n,dz_min,dz_max);

  const int iso_n = 30; const float iso_min = 0;
  TH1F h_iso("h_iso","Distribution of Track Isolation",iso_n,iso_min,iso_max);
  TH1F h_iso_p("h_iso_p","Distribution of Track Isolation (pixel-only)",iso_n,iso_min,iso_max);
  TH1F h_iso_s("h_iso_s","Distribution of Track Isolation (not pixel-only, < 7 layers)",iso_n,iso_min,iso_max);
  TH1F h_iso_l("h_iso_l","Distribution of Track Isolation (>= 7 layers, > 1 lost outer hits)",iso_n,iso_min,iso_max);
  //TH1F h_iso_li("h_iso_li","Distribution of Track Isolation (>=7 layers)",iso_n,iso_min,iso_max);

  const int niso_n = 30; const float niso_min = 0;
  TH1F h_niso("h_niso","Distribution of Track NeutIso (#DeltaR < 0.05)",niso_n,niso_min,niso_max);
  TH1F h_niso_p("h_niso_p","Distribution of Track NeutIso (#DeltaR < 0.05) (pixel-only)",niso_n,niso_min,niso_max);
  TH1F h_niso_s("h_niso_s","Distribution of Track NeutIso (#DeltaR < 0.05) (not pixel-only, < 7 layers)",niso_n,niso_min,niso_max);
  TH1F h_niso_l("h_niso_l","Distribution of Track NeutIso (#DeltaR < 0.05) (>= 7 layers, > 1 lost outer hits)",niso_n,niso_min,niso_max);
  //TH1F h_niso_li("h_niso_li","Distribution of Track NeutIso (#DeltaR < 0.05) (>=7 layers)",niso_n,niso_min,niso_max);

  const int chiso_n = 20; const float chiso_min = 0;
  TH1F h_chiso("h_chiso","Distribution of Track ChIso (#DeltaR < 0.01)",chiso_n,chiso_min,chiso_max);
  TH1F h_chiso_p("h_chiso_p","Distribution of Track ChIso (#DeltaR < 0.01) (pixel-only)",chiso_n,chiso_min,chiso_max);
  TH1F h_chiso_s("h_chiso_s","Distribution of Track ChIso (#DeltaR < 0.01) (not pixel-only, < 7 layers)",chiso_n,chiso_min,chiso_max);
  TH1F h_chiso_l("h_chiso_l","Distribution of Track ChIso (#DeltaR < 0.01) (>= 7 layers, > 1 lost outer hits)",chiso_n,chiso_min,chiso_max);
  //TH1F h_chiso_li("h_chiso_li","Distribution of Track ChIso (#DeltaR < 0.01) (>=7 layers)",chiso_n,chiso_min,chiso_max);

  const int reliso_n = 30; const float reliso_min = 0;
  TH1F h_reliso("h_reliso","Distribution of Track Relative Isolation",reliso_n,reliso_min,reliso_max);
  TH1F h_reliso_p("h_reliso_p","Distribution of Track Relative Isolation (pixel-only)",reliso_n,reliso_min,reliso_max);
  TH1F h_reliso_s("h_reliso_s","Distribution of Track Relative Isolation (not pixel-only, < 7 layers)",reliso_n,reliso_min,reliso_max);
  TH1F h_reliso_l("h_reliso_l","Distribution of Track Relative Isolation (>= 7 layers, > 1 lost outer hits)",reliso_n,reliso_min,reliso_max);
  //TH1F h_reliso_li("h_reliso_li","Distribution of Track Relative Isolation (>=7 layers)",reliso_n,reliso_min,reliso_max);

  const int chreliso_n = 30; const float chreliso_min = 0;
  TH1F h_chreliso("h_chreliso","Distribution of Track Ch Rel Iso",chreliso_n,chreliso_min,chreliso_max);
  TH1F h_chreliso_p("h_chreliso_p","Distribution of Track Ch Rel Iso (pixel-only)",chreliso_n,chreliso_min,chreliso_max);
  TH1F h_chreliso_s("h_chreliso_s","Distribution of Track Ch Rel Iso (not pixel-only, < 7 layers)",chreliso_n,chreliso_min,chreliso_max);
  TH1F h_chreliso_l("h_chreliso_l","Distribution of Track Ch Rel Iso (>= 7 layers, > 1 lost outer hits)",chreliso_n,chreliso_min,chreliso_max);
  //TH1F h_chreliso_li("h_chreliso_li","Distribution of Track Ch Rel Iso (>=7 layers)",chreliso_n,chreliso_min,chreliso_max);

  const int nreliso_n = 30; const float nreliso_min = 0;
  TH1F h_nreliso("h_nreliso","Distribution of Track Neut Rel Iso",nreliso_n,nreliso_min,nreliso_max);
  TH1F h_nreliso_p("h_nreliso_p","Distribution of Track Neut Rel Iso (pixel-only)",nreliso_n,nreliso_min,nreliso_max);
  TH1F h_nreliso_s("h_nreliso_s","Distribution of Track Neut Rel Iso (not pixel-only, < 7 layers)",nreliso_n,nreliso_min,nreliso_max);
  TH1F h_nreliso_l("h_nreliso_l","Distribution of Track Neut Rel Iso (>= 7 layers, > 1 lost outer hits)",nreliso_n,nreliso_min,nreliso_max);
  //TH1F h_nreliso_li("h_nreliso_li","Distribution of Track Neut Rel Iso (>=7 layers)",nreliso_n,nreliso_min,nreliso_max);

  // Short Track versions
  TH1F h_sh_pt("h_sh_pt","Distribution of Track p_{T}s",pt_n,pt_min,pt_max);
  TH1F h_sh_pt_p("h_sh_pt_p","Distribution of Track p_{T}s (pixel-only)",pt_n,pt_min,pt_max);
  TH1F h_sh_pt_s("h_sh_pt_s","Distribution of Track p_{T}s (not pixel-only, < 7 layers)",pt_n,pt_min,pt_max);
  TH1F h_sh_pt_l("h_sh_pt_l","Distribution of Track p_{T}s (>= 7 layers, > 1 lost outer hits)",pt_n,pt_min,pt_max);
  //TH1F h_sh_pt_li("h_sh_pt_li","Distribution of Track p_{T}s (>=7 layers)",pt_n,pt_min,pt_max);

  TH1F h_sh_bardxy("h_sh_bardxy","Distribution of Barrel Track dxy",dxy_n,dxy_min,dxy_max);
  TH1F h_sh_bardxy_p("h_sh_bardxy_p","Distribution of Barrel Track dxy (pixel-only)",dxy_n,dxy_min,dxy_max);
  TH1F h_sh_bardxy_s("h_sh_bardxy_s","Distribution of Barrel Track dxy (not pixel-only, < 7 layers)",dxy_n,dxy_min,dxy_max);
  TH1F h_sh_bardxy_l("h_sh_bardxy_l","Distribution of Barrel Track dxy (>= 7 layers, > 1 lost outer hits)",dxy_n,dxy_min,dxy_max);
  //TH1F h_sh_bardxy_li("h_sh_bardxy_li","Distribution of Barrel Track dxy (>=7 layers)",dxy_n,dxy_min,dxy_max);

  TH1F h_sh_bardz("h_sh_bardz","Distribution of Barrel Track dz",dz_n,dz_min,dz_max);
  TH1F h_sh_bardz_p("h_sh_bardz_p","Distribution of Barrel Track dz (pixel-only)",dz_n,dz_min,dz_max);
  TH1F h_sh_bardz_s("h_sh_bardz_s","Distribution of Barrel Track dz (not pixel-only, < 7 layers)",dz_n,dz_min,dz_max);
  TH1F h_sh_bardz_l("h_sh_bardz_l","Distribution of Barrel Track dz (>= 7 layers, > 1 lost outer hits)",dz_n,dz_min,dz_max);
  //TH1F h_sh_bardz_li("h_sh_ecdz_li","Distribution of Barrel Track dz (>=7 layers)",dz_n,dz_min,dz_max);

  TH1F h_sh_ecdxy("h_sh_ecdxy","Distribution of Endcap Track dxy",dxy_n,dxy_min,dxy_max);
  TH1F h_sh_ecdxy_p("h_sh_ecdxy_p","Distribution of Endcap Track dxy (pixel-only)",dxy_n,dxy_min,dxy_max);
  TH1F h_sh_ecdxy_s("h_sh_ecdxy_s","Distribution of Endcap Track dxy (not pixel-only, < 7 layers)",dxy_n,dxy_min,dxy_max);
  TH1F h_sh_ecdxy_l("h_sh_ecdxy_l","Distribution of Endcap Track dxy (>= 7 layers, > 1 lost outer hits)",dxy_n,dxy_min,dxy_max);
  //TH1F h_sh_ecdxy_li("h_sh_ecdxy_li","Distribution of Endcap Track dxy (>=7 layers)",dxy_n,dxy_min,dxy_max);

  TH1F h_sh_ecdz("h_sh_ecdz","Distribution of Endcap Track dz",dz_n,dz_min,dz_max);
  TH1F h_sh_ecdz_p("h_sh_ecdz_p","Distribution of Endcap Track dz (pixel-only)",dz_n,dz_min,dz_max);
  TH1F h_sh_ecdz_s("h_sh_ecdz_s","Distribution of Endcap Track dz (not pixel-only, < 7 layers)",dz_n,dz_min,dz_max);
  TH1F h_sh_ecdz_l("h_sh_ecdz_l","Distribution of Endcap Track dz (>= 7 layers, > 1 lost outer hits)",dz_n,dz_min,dz_max);
  //TH1F h_sh_ecdz_li("h_sh_ecdz_li","Distribution of Endcap Track dz (>=7 layers)",dz_n,dz_min,dz_max);

  TH1F h_sh_iso("h_sh_iso","Distribution of Track Isolation",iso_n,iso_min,iso_max);
  TH1F h_sh_iso_p("h_sh_iso_p","Distribution of Track Isolation (pixel-only)",iso_n,iso_min,iso_max);
  TH1F h_sh_iso_s("h_sh_iso_s","Distribution of Track Isolation (not pixel-only, < 7 layers)",iso_n,iso_min,iso_max);
  TH1F h_sh_iso_l("h_sh_iso_l","Distribution of Track Isolation (>= 7 layers, > 1 lost outer hits)",iso_n,iso_min,iso_max);
  //TH1F h_sh_iso_li("h_sh_iso_li","Distribution of Track Isolation (>=7 layers)",iso_n,iso_min,iso_max);

  TH1F h_sh_niso("h_sh_niso","Distribution of Track NeutIso (#DeltaR < 0.05)",niso_n,niso_min,niso_max);
  TH1F h_sh_niso_p("h_sh_niso_p","Distribution of Track NeutIso (#DeltaR < 0.05) (pixel-only)",niso_n,niso_min,niso_max);
  TH1F h_sh_niso_s("h_sh_niso_s","Distribution of Track NeutIso (#DeltaR < 0.05) (not pixel-only, < 7 layers)",niso_n,niso_min,niso_max);
  TH1F h_sh_niso_l("h_sh_niso_l","Distribution of Track NeutIso (#DeltaR < 0.05) (>= 7 layers, > 1 lost outer hits)",niso_n,niso_min,niso_max);
  //TH1F h_sh_niso_li("h_sh_niso_li","Distribution of Track NeutIso (#DeltaR < 0.05) (>=7 layers)",niso_n,niso_min,niso_max);

  TH1F h_sh_chiso("h_sh_chiso","Distribution of Track ChIso (#DeltaR < 0.01)",chiso_n,chiso_min,chiso_max);
  TH1F h_sh_chiso_p("h_sh_chiso_p","Distribution of Track ChIso (#DeltaR < 0.01) (pixel-only)",chiso_n,chiso_min,chiso_max);
  TH1F h_sh_chiso_s("h_sh_chiso_s","Distribution of Track ChIso (#DeltaR < 0.01) (not pixel-only, < 7 layers)",chiso_n,chiso_min,chiso_max);
  TH1F h_sh_chiso_l("h_sh_chiso_l","Distribution of Track ChIso (#DeltaR < 0.01) (>= 7 layers, > 1 lost outer hits)",chiso_n,chiso_min,chiso_max);
  //TH1F h_sh_chiso_li("h_sh_chiso_li","Distribution of Track ChIso (#DeltaR < 0.01) (>=7 layers)",chiso_n,chiso_min,chiso_max);

  TH1F h_sh_reliso("h_sh_reliso","Distribution of Track Relative Isolation",reliso_n,reliso_min,reliso_max);
  TH1F h_sh_reliso_p("h_sh_reliso_p","Distribution of Track Relative Isolation (pixel-only)",reliso_n,reliso_min,reliso_max);
  TH1F h_sh_reliso_s("h_sh_reliso_s","Distribution of Track Relative Isolation (not pixel-only, < 7 layers)",reliso_n,reliso_min,reliso_max);
  TH1F h_sh_reliso_l("h_sh_reliso_l","Distribution of Track Relative Isolation (>= 7 layers, > 1 lost outer hits)",reliso_n,reliso_min,reliso_max);
  //TH1F h_sh_reliso_li("h_sh_reliso_li","Distribution of Track Relative Isolation (>=7 layers)",reliso_n,reliso_min,reliso_max);

  TH1F h_sh_chreliso("h_sh_chreliso","Distribution of Track Ch Rel Iso",chreliso_n,chreliso_min,chreliso_max);
  TH1F h_sh_chreliso_p("h_sh_chreliso_p","Distribution of Track Ch Rel Iso (pixel-only)",chreliso_n,chreliso_min,chreliso_max);
  TH1F h_sh_chreliso_s("h_sh_chreliso_s","Distribution of Track Ch Rel Iso (not pixel-only, < 7 layers)",chreliso_n,chreliso_min,chreliso_max);
  TH1F h_sh_chreliso_l("h_sh_chreliso_l","Distribution of Track Ch Rel Iso (>= 7 layers, > 1 lost outer hits)",chreliso_n,chreliso_min,chreliso_max);
  //TH1F h_sh_chreliso_li("h_sh_chreliso_li","Distribution of Track Ch Rel Iso (>=7 layers)",chreliso_n,chreliso_min,chreliso_max);

  TH1F h_sh_nreliso("h_sh_nreliso","Distribution of Track Neut Rel Iso",nreliso_n,nreliso_min,nreliso_max);
  TH1F h_sh_nreliso_p("h_sh_nreliso_p","Distribution of Track Neut Rel Iso (pixel-only)",nreliso_n,nreliso_min,nreliso_max);
  TH1F h_sh_nreliso_s("h_sh_nreliso_s","Distribution of Track Neut Rel Iso (not pixel-only, < 7 layers)",nreliso_n,nreliso_min,nreliso_max);
  TH1F h_sh_nreliso_l("h_sh_nreliso_l","Distribution of Track Neut Rel Iso (>= 7 layers, > 1 lost outer hits)",nreliso_n,nreliso_min,nreliso_max);
  //TH1F h_sh_nreliso_li("h_sh_nreliso_li","Distribution of Track Neut Rel Iso (>=7 layers)",nreliso_n,nreliso_min,nreliso_max);

  if (doCRSL) {
    h_inclusive.Fill(0.5,ch_mt2->GetEntries("nLepLowMT == 1 && Sum$(jet_pt) > 100"));
  } else if (doHT) {
    h_inclusive.Fill(0.5,ch_mt2->GetEntries("Sum$(jet_pt) > 100"));
  } else if (doSR) {
    h_inclusive.Fill(0.5,ch_mt2->GetEntries("nPFLep5LowMT + nMuons10 + nElectrons10 + nPFHad10LowMT == 0 && Sum$(jet_pt) > 100"));
  }  else {
    h_inclusive.Fill(0.5,nEventsTree);
  }

   for( unsigned int event = 0; event < nEventsTree; ++event) {    
  //  for( unsigned int event = 0; event < 10; ++event) {
    // AddFriend above makes this pull both mt2 and st branches for the same event, assuming the trees were properly (identically) sorted. 
    t.GetEntry(event); 

    //    const float w_ = t.evt_xsec * 1000 / nEventsTree; // weight events to 1 fb^-1 luminosity
    const float w_ = 1.0; // Just count occurences without wrinkle of event weight

    //---------------------
    // basic event selection and cleaning
    //---------------------
    
    if (t.nVert == 0) continue;
    
    const int run = t.run;
    const int lumi = t.lumi;
    const int evt = t.evt;

    // random events with ht or met=="inf" or "nan" that don't get caught by the filters...
    if(isinf(t.met_pt) || isnan(t.met_pt) || isinf(t.ht) || isnan(t.ht)){
      cout << "WARNING: bad event with infinite MET/HT! " << run << ":" << lumi << ":" << evt
	   << ", met=" << t.met_pt << ", ht=" << t.ht << endl;
      continue;
    }
    
    // catch events with unphysical jet pt
    if(t.jet_pt[0] > 13000.){
      cout << "WARNING: bad event with unphysical jet pt! " << run << ":" << lumi << ":" << evt
	   << ", met=" << t.met_pt << ", ht=" << t.ht << ", jet_pt=" << t.jet_pt[0] << endl;
      continue;
    }
    /*
    if (event < 30) {
      cout << t.run << ":" << t.lumi << ":" << t.evt << endl;
    }
    else return 0;
    */

    // Analysis code

    float sumJet = 0;
    for (int i_jet = 0; i_jet < t.njet; i_jet++) {
      sumJet += t.jet_pt[i_jet];
    }

    if (doCRSL) {
      if (t.nLepLowMT != 1) continue;
      if (sumJet < 100) continue;
    } else if (doSR) {
      if (t.nPFLep5LowMT + t.nMuons10 + t.nElectrons10 + t.nPFHad10LowMT != 0) continue;
      if (sumJet < 100) continue;
    } else if (doHT && sumJet < 100) continue;
    
    /*
     * Track Loop
     */
    const int ntracks = t.ntracks;
    int nstc = 0; int nst = 0;
    for (int i_trk = 0; i_trk < ntracks; i_trk++) {   

      // Veto tracks outside fiducial region
      if (doEtaPhiVeto && InEtaPhiVetoRegion(t.track_eta[i_trk],t.track_phi[i_trk]) != 0) continue;

      // Find nearest gen lepton
      float minDR_tau = 100;
      float minDR_lep = 100;
      int minIndex = -1;
      float minDR = 100;
      float ratio_pt = 0;
      int pdgId = 0;
      for (int i_lep = 0; i_lep < t.ngenLep; i_lep++) {
	float lep_eta = t.genLep_eta[i_lep];
	float lep_phi = t.genLep_phi[i_lep];
	float dr = DeltaR(lep_eta, t.track_eta[i_trk], lep_phi, t.track_phi[i_trk]);
	if (dr < minDR) {
	  minDR = dr;
	  minIndex = i_lep;
	}
      } // End loop over genLeps to find closest to track
      if (minIndex >= 0) {
	ratio_pt = t.genLep_pt[minIndex] / t.track_pt[i_trk] - 1;
	pdgId = fabs(t.genLep_pdgId[minIndex]);
	if (t.track_isshort[i_trk]) {
	  if (pdgId == 11) {
	    h_sh_el_dr.Fill(minDR,w_);
	    h_sh_el_ptr.Fill(ratio_pt,w_);
	  }
	  else if (pdgId == 13) {
	    h_sh_mu_dr.Fill(minDR,w_);
	    h_sh_mu_ptr.Fill(ratio_pt,w_);
	  }
	} // End short track fills 
	if (pdgId == 11) {
	  h_el_dr.Fill(minDR,w_);
	  h_el_ptr.Fill(ratio_pt,w_);
	}
	else if (pdgId == 13) {
	  h_mu_dr.Fill(minDR,w_);
	  h_mu_ptr.Fill(ratio_pt,w_);
	}
      } // End fills if lep was found
      minDR_lep = minDR;
      
      minDR = 100;
      minIndex = -1;
      for (int i_lep = 0; i_lep < t.ngenLepFromTau; i_lep++) {
	float lep_eta = t.genLepFromTau_eta[i_lep];
	float lep_phi = t.genLepFromTau_phi[i_lep];
	float dr = DeltaR(lep_eta, t.track_eta[i_trk], lep_phi, t.track_phi[i_trk]);
	if (dr < minDR) {
	  minDR = dr;
	  minIndex = i_lep;
	}
      } // End loop over gen leps from tau decay looking for closest to track
      if (minIndex >= 0) {
	ratio_pt = t.genLepFromTau_pt[minIndex] / t.track_pt[i_trk] - 1;
	pdgId = fabs(t.genLepFromTau_pdgId[minIndex]);
	if (t.track_isshort[i_trk]) {
	  if (pdgId == 11) {
	    h_sh_el_dr.Fill(minDR,w_);
	    h_sh_el_ptr.Fill(ratio_pt,w_);
	  }
	  else if (pdgId == 13) {
	    h_sh_mu_dr.Fill(minDR,w_);
	    h_sh_mu_ptr.Fill(ratio_pt,w_);
	  }
	} // End short track fills
	if (pdgId == 11) {
	  h_el_dr.Fill(minDR,w_);
	  h_el_ptr.Fill(ratio_pt,w_);
	}
	else if (pdgId == 13) {
	  h_mu_dr.Fill(minDR,w_);
	  h_mu_ptr.Fill(ratio_pt,w_);
	}
      } // End fills if found a tau lep 
      if (minDR < minDR_lep) minDR_lep = minDR;
      
      // Find nearest gen tau
      minDR = 100;
      minIndex = -1;
      for (int i_tau = 0; i_tau < t.ngenTau; i_tau++) {
	// Decay Mode == 1 means 1 prong
	// 2 means 3 prong
	// Else, pdgId of lepton
	int decayMode = abs(t.genTau_decayMode[i_tau]);
	if (decayMode > 3) continue; // Count leptons separately
	//	if (decayMode > 0) continue; // No tau veto
	float tau_eta = t.genTau_eta[i_tau];
	float tau_phi = t.genTau_phi[i_tau];
	float dr = DeltaR(tau_eta, t.track_eta[i_trk], tau_phi, t.track_phi[i_trk]);
	if (dr < minDR) {
	  minDR = dr;
	  minIndex = i_tau;
	}
      }
      if (minIndex >= 0) {
	ratio_pt = t.genTau_pt[minIndex] / t.track_pt[i_trk] - 1;
	if (t.track_isshort[i_trk]) {
	  h_sh_tau_dr.Fill(minDR,w_);
	  h_sh_tau_ptr.Fill(ratio_pt,w_);
	} // End fill if track is short
	h_tau_dr.Fill(minDR,w_);
	h_tau_ptr.Fill(ratio_pt,w_);
      }
      minDR_tau = minDR;
      
      if (minDR_tau < 0.1 || minDR_lep < 0.1) { // Use Gen Veto to find short track hotspots
	if (fabs(t.track_eta[i_trk]) < 1.4) {
	  h_etaphi_bar.Fill(t.track_eta[i_trk],t.track_phi[i_trk]);
	  if (t.track_isshort[i_trk]) {
	    h_st_etaphi_bar.Fill(t.track_eta[i_trk],t.track_phi[i_trk]);
	  } 
	} else if (t.track_eta[i_trk] > 1.4) {
	  h_etaphi_ecp.Fill(t.track_eta[i_trk],t.track_phi[i_trk]);
	  if (t.track_isshort[i_trk]) {
	    h_st_etaphi_ecp.Fill(t.track_eta[i_trk],t.track_phi[i_trk]);
	  } 
	} else if (t.track_eta[i_trk] < -1.4) {
	  h_etaphi_ecn.Fill(t.track_eta[i_trk],t.track_phi[i_trk]);
	  if (t.track_isshort[i_trk]) {
	    h_st_etaphi_ecn.Fill(t.track_eta[i_trk],t.track_phi[i_trk]);
	  } 
	}
	if (doGenVeto) continue;// Apply Gen veto
      } // End fills if gen lepton nearby

      minDR = 100;
      // Also check genStat23 for DR cases where genLeps not saved due to no Z
      for (int i_s23 = 0; i_s23 < t.ngenStat23; i_s23++) {
	int pdgId = abs(t.genStat23_pdgId[i_s23]);
	if (pdgId != 15 && pdgId != 13 && pdgId != 11) continue;
	float gen_eta = t.genStat23_eta[i_s23];
	float gen_phi = t.genStat23_phi[i_s23];
	float dr = DeltaR(gen_eta, t.track_eta[i_trk], gen_phi, t.track_phi[i_trk]);
	if (dr < minDR) {
	  minDR = dr;
	  minIndex = i_s23;
	}	
      }
      if (minDR < 0.1 && doGenVeto) continue;

      // Check reco leps
      bool lepOverlap = false;
      for (int i_lep = 0; i_lep < t.nlep; i_lep++) {
	if (DeltaR(t.lep_eta[i_lep],t.track_eta[i_trk],t.lep_phi[i_trk],t.track_phi[i_trk]) < 0.1) {
	  lepOverlap = true;
	  break;
	}
      }
      if (lepOverlap) continue; // short track candidates can't overlap with leptons
      // If we get here, no reco leps overlapped. Now check PF leps
      for (int i_isoT = 0; i_isoT < t.nlep; i_isoT++) {
	int pdgId = abs(t.isoTrack_pdgId[i_isoT]);
	if (pdgId != 11 && pdgId != 13) continue;
	if (DeltaR(t.isoTrack_eta[i_isoT],t.track_eta[i_trk],t.isoTrack_phi[i_isoT],t.track_phi[i_trk]) < 0.1) {
	  lepOverlap = true;
	  break;
	}
      }
      if (lepOverlap) continue; // short track candidates can't overlap with leptons

      const float eta = fabs(t.track_eta[i_trk]);
      if (eta > 2.4) continue;

      const int pixel_layers = t.track_pixelLayersWithMeasurement[i_trk];
      if (pixel_layers < 2) continue;
      const int total_layers = t.track_trackerLayersWithMeasurement[i_trk];
      const float pt = t.track_pt[i_trk];
      const float dz = fabs(t.track_dz[i_trk]);
      const float dxy = fabs(t.track_dxy[i_trk]);
      const float iso = t.track_iso[i_trk];
      const float niso = t.track_sumNeuH0p05[i_trk] + t.track_sumPh0p05[i_trk];
      const float chiso = t.track_sumChP[i_trk];
      const float reliso = t.track_reliso[i_trk];
      const float nreliso = niso/pt;
      const float chreliso = chiso/pt;
      const int lostOuterHits = t.track_lostOuterHits[i_trk];

      bool p = pixel_layers == total_layers;
      bool s = total_layers < 7;
      bool l = lostOuterHits >= 2;

      if (pt > pt_min && dz < dz_max && dxy < dxy_max && iso < iso_max && niso < niso_max && chiso < chiso_max && reliso < reliso_max && nreliso < nreliso_max && chreliso < chreliso_max) {
	nstc++;
      }
      
      /*
       * N-1 short track candidate distributions
       */
      // pt
      if (dz < dz_max && dxy < dxy_max && iso < iso_max && niso < niso_max && chiso < chiso_max && reliso < reliso_max && nreliso < nreliso_max && chreliso < chreliso_max) {
	h_pt.Fill(pt,w_);
	// pixel
	if (p) {
	  h_pt_p.Fill(pt,w_);
	  // tracker short
	} else if (s) {
	  h_pt_s.Fill(pt,w_);
	  // tracker long
	} else if (l) {
	  h_pt_l.Fill(pt,w_);
	}
      }
      // dz
      if (pt > pt_min && dxy < dxy_max && iso < iso_max && niso < niso_max && chiso < chiso_max && reliso < reliso_max && nreliso < nreliso_max && chreliso < chreliso_max) {
	if (eta < 1.5) {
	  h_bardz.Fill(dz,w_);
	  // pixel
	  if (p) {
	    h_bardz_p.Fill(dz,w_);
	    // trkshort
	  } else if (s) {
	    h_bardz_s.Fill(dz,w_);
	    // trklong
	  } else if (l) {
	    h_bardz_l.Fill(dz,w_);
	  }  
	}
	else {
	  h_ecdz.Fill(dz,w_);
	  // pixel
	  if (p) {
	    h_ecdz_p.Fill(dz,w_);
	    // trkshort
	  } else if (s) {
	    h_ecdz_s.Fill(dz,w_);
	    // trklong
	  } else if (l) {
	    h_ecdz_l.Fill(dz,w_);
	  }
	}
      }
      // dxy
      if (dz < dz_max && pt > pt_min && iso < iso_max && niso < niso_max && chiso < chiso_max && reliso < reliso_max && nreliso < nreliso_max && chreliso < chreliso_max) {
	if (eta < 1.5) {
	  h_bardxy.Fill(dxy,w_);
	  // pixel
	  if (p) {
	    h_bardxy_p.Fill(dxy,w_);
	    // trkshort
	  } else if (s) {
	    h_bardxy_s.Fill(dxy,w_);
	  }
	  // trklong
	  else if (l) {
	    h_bardxy_l.Fill(dxy,w_);
	  }
	}
	else {
	  h_ecdxy.Fill(dxy,w_);
	  // pixel
	  if (p) {
	    h_ecdxy_p.Fill(dxy,w_);
	    // trkshort
	  } else if (s) {
	    h_ecdxy_s.Fill(dxy,w_);
	    // trklong
	  } else if (l) {
	    h_ecdxy_l.Fill(dxy,w_);
	  }
	}
      }
      // Abs Iso 0.3
      if (dz < dz_max && dxy < dxy_max && pt > pt_min && niso < niso_max && chiso < chiso_max && reliso < reliso_max && nreliso < nreliso_max && chreliso < chreliso_max) {
	h_iso.Fill(iso,w_);
	// pixel
	if (p) {
	  h_iso_p.Fill(iso,w_);
	  // tracker short
	} else if (s) {
	  h_iso_s.Fill(iso,w_);
	  // tracker long
	} else if (l) {
	  h_iso_l.Fill(iso,w_);
	}
      }
      // Neut 0.05 Iso 
      if (dz < dz_max && dxy < dxy_max && iso < iso_max && pt > pt_min && chiso < chiso_max && reliso < reliso_max && nreliso < nreliso_max && chreliso < chreliso_max) {
	h_niso.Fill(niso,w_);
	// pixel
	if (p) {
	  h_niso_p.Fill(niso,w_);
	  // tracker short
	} else if (s) {
	  h_niso_s.Fill(niso,w_);
	  // tracker long
	} else if (l) {
	  h_niso_l.Fill(niso,w_);
	}
      }
      // ChIso
      if (dz < dz_max && dxy < dxy_max && iso < iso_max && niso < niso_max && pt > pt_min && reliso < reliso_max && nreliso < nreliso_max && chreliso < chreliso_max) {
	h_chiso.Fill(chiso,w_);
	// pixel
	if (p) {
	  h_chiso_p.Fill(chiso,w_);
	  // tracker short
	} else if (s) {
	  h_chiso_s.Fill(chiso,w_);
	  // tracker long
	} else if (l) {
	  h_chiso_l.Fill(chiso,w_);
	}
      }
      // RelIso 0.3
      if (dz < dz_max && dxy < dxy_max && iso < iso_max && niso < niso_max && chiso < chiso_max && pt > pt_min && nreliso < nreliso_max && chreliso < chreliso_max) {
	h_reliso.Fill(reliso,w_);
	// pixel
	if (p) {
	  h_reliso_p.Fill(reliso,w_);
	  // tracker short
	} else if (s) {
	  h_reliso_s.Fill(reliso,w_);
	  // tracker long
	} else if (l) {
	  h_reliso_l.Fill(reliso,w_);
	}
      }
      // NeuRelIso 0.05
      if (dz < dz_max && dxy < dxy_max && iso < iso_max && niso < niso_max && chiso < chiso_max && reliso < reliso_max && pt > pt_min && chreliso < chreliso_max) {
	h_nreliso.Fill(nreliso,w_);
	// pixel
	if (p) {
	  h_nreliso_p.Fill(nreliso,w_);
	  // tracker short
	} else if (s) {
	  h_nreliso_s.Fill(nreliso,w_);
	  // tracker long
	} else if (l) {
	  h_nreliso_l.Fill(nreliso,w_);
	}
      }
      // ChRelIso 0.05
      if (dz < dz_max && dxy < dxy_max && iso < iso_max && niso < niso_max && chiso < chiso_max && reliso < reliso_max && nreliso < nreliso_max && pt > pt_min) {
	h_chreliso.Fill(chreliso,w_);
	// pixel
	if (p) {
	  h_chreliso_p.Fill(chreliso,w_);
	  // tracker short
	} else if (s) {
	  h_chreliso_s.Fill(chreliso,w_);
	  // tracker long
	} else if (l) {
	  h_chreliso_l.Fill(chreliso,w_);
	}
      }
      
      // A few selections that apply to all short tracks, and not candidates
      const float ptErr = t.track_ptErr[i_trk];
      const float pt2 = pt*pt;
      if (t.track_highPurity[i_trk] != 1) continue;
      if (t.track_lostInnerHits[i_trk] != 0) continue;
      if (t.track_lostInnerPixHits[i_trk] != 0) continue;
      if (ptErr / pt2 > 0.02) continue;
      // Update length selections
      bool s_NoDxyTight = s && (ptErr / pt2 < 0.02);
      bool l_NoDxyTight = l && (ptErr / pt2 < 0.005);
      s =  s_NoDxyTight && dxy < 0.01;
      l =  l_NoDxyTight && dxy < 0.01;

      if (t.track_isshort[i_trk]) {
	nst++;
	cout << run << ":" << lumi << ":" << evt << " track: " << i_trk << endl;
      }
      
      /*
       * Short track N-1 plots
       */
      // pt
      if (dz < dz_max_st && dxy < dxy_max_st && iso < iso_max_st && niso < niso_max_st && chiso < chiso_max_st && reliso < reliso_max_st && nreliso < nreliso_max_st && chreliso < chreliso_max_st) {
	h_sh_pt.Fill(pt,w_);
	// pixel
	if (p) {
	  h_sh_pt_p.Fill(pt,w_);
	  // tracker short
	} else if (s) {
	  h_sh_pt_s.Fill(pt,w_);
	  // tracker long
	} else if (l) {
	  h_sh_pt_l.Fill(pt,w_);
	}
      }
      // dz
      if (pt > pt_min_st && dxy < dxy_max_st && iso < iso_max_st && niso < niso_max_st && chiso < chiso_max_st && reliso < reliso_max_st && nreliso < nreliso_max_st && chreliso < chreliso_max_st) {
	if (eta < 1.5) {
	  h_sh_bardz.Fill(dz,w_);
	  // pixel
	  if (p) {
	    h_sh_bardz_p.Fill(dz,w_);
	    // trkshort
	  } else if (s) {
	    h_sh_bardz_s.Fill(dz,w_);
	  }
	  // trklong
	  else if (l) {
	    h_sh_bardz_l.Fill(dz,w_);
	  } 
	}
	else {
	  h_sh_ecdz.Fill(dz,w_);
	  // pixel
	  if (p) {
	    h_sh_ecdz_p.Fill(dz,w_);
	    // trkshort
	  } else if (s) {
	    h_sh_ecdz_s.Fill(dz,w_);
	    // trklong
	  } else if (l) {
	    h_sh_ecdz_l.Fill(dz,w_);
	  }	 
	}
      }
      // dxy
      if (dz < dz_max_st && pt > pt_min_st && iso < iso_max_st && niso < niso_max_st && chiso < chiso_max_st && reliso < reliso_max_st && nreliso < nreliso_max_st && chreliso < chreliso_max_st) {
	if (eta < 1.5) {
	  h_sh_bardxy.Fill(dxy,w_);
	  // pixel
	  if (p) {
	    h_sh_bardxy_p.Fill(dxy,w_);
	    // trkshort
	  } else if (s_NoDxyTight) {
	    h_sh_bardxy_s.Fill(dxy,w_);
	  }
	  // trklong
	  else if (l_NoDxyTight) {
	    h_sh_bardxy_l.Fill(dxy,w_);
	  }
	}
	else {
	  h_sh_ecdxy.Fill(dxy,w_);
	  // pixel
	  if (p) {
	    h_sh_ecdxy_p.Fill(dxy,w_);
	    // trkshort
	  } else if (s_NoDxyTight) {
	    h_sh_ecdxy_s.Fill(dxy,w_);
	    // trklong
	  } else if (l_NoDxyTight) {
	    h_sh_ecdxy_l.Fill(dxy,w_);
	  }
	}
      }
      // Abs Iso 0.3
      if (dz < dz_max_st && dxy < dxy_max_st && pt > pt_min_st && niso < niso_max_st && chiso < chiso_max_st && reliso < reliso_max_st && nreliso < nreliso_max_st && chreliso < chreliso_max_st) {
	h_sh_iso.Fill(iso,w_);
	// pixel
	if (p) {
	  h_sh_iso_p.Fill(iso,w_);
	  // tracker short
	} else if (s) {
	  h_sh_iso_s.Fill(iso,w_);
	  // tracker long
	} else if (l) {
	  h_sh_iso_l.Fill(iso,w_);
	}
      }
      // Neut 0.05 Iso 
      if (dz < dz_max_st && dxy < dxy_max_st && iso < iso_max_st && pt > pt_min_st && chiso < chiso_max_st && reliso < reliso_max_st && nreliso < nreliso_max_st && chreliso < chreliso_max_st) {
	h_sh_niso.Fill(niso,w_);
	// pixel
	if (p) {
	  h_sh_niso_p.Fill(niso,w_);
	  // tracker short
	} else if (s) {
	  h_sh_niso_s.Fill(niso,w_);
	  // tracker long
	} else if (l) {
	  h_sh_niso_l.Fill(niso,w_);
	}
      }
      // ChIso
      if (dz < dz_max_st && dxy < dxy_max_st && iso < iso_max_st && niso < niso_max_st && pt > pt_min_st && reliso < reliso_max_st && nreliso < nreliso_max_st && chreliso < chreliso_max_st) {
	h_sh_chiso.Fill(chiso,w_);
	// pixel
	if (p) {
	  h_sh_chiso_p.Fill(chiso,w_);
	  // tracker short
	} else if (s) {
	  h_sh_chiso_s.Fill(chiso,w_);
	  // tracker long
	} else if (l) {
	  h_sh_chiso_l.Fill(chiso,w_);
	}
      }
      // RelIso 0.3
      if (dz < dz_max_st && dxy < dxy_max_st && iso < iso_max_st && niso < niso_max_st && chiso < chiso_max_st && pt > pt_min_st && nreliso < nreliso_max_st && chreliso < chreliso_max_st) {
	h_sh_reliso.Fill(reliso,w_);
	// pixel
	if (p) {
	  h_sh_reliso_p.Fill(reliso,w_);
	  // tracker short
	} else if (s) {
	  h_sh_reliso_s.Fill(reliso,w_);
	  // tracker long
	  } else if (l) {
	  h_sh_reliso_l.Fill(reliso,w_);
	}
      }
      // NeuRelIso 0.05
      if (dz < dz_max_st && dxy < dxy_max_st && iso < iso_max_st && niso < niso_max_st && chiso < chiso_max_st && reliso < reliso_max_st && pt > pt_min_st && chreliso < chreliso_max_st) {
	h_sh_nreliso.Fill(nreliso,w_);
	// pixel
	if (p) {
	  h_sh_nreliso_p.Fill(nreliso,w_);
	  // tracker short
	} else if (s) {
	  h_sh_nreliso_s.Fill(nreliso,w_);
	  // tracker long
	  } else if (l) {
	  h_sh_nreliso_l.Fill(nreliso,w_);
	}
      }
      // ChRelIso 0.05
      if (dz < dz_max_st && dxy < dxy_max_st && iso < iso_max_st && niso < niso_max_st && chiso < chiso_max_st && reliso < reliso_max_st && nreliso < nreliso_max_st && pt > pt_min_st) {
	h_sh_chreliso.Fill(chreliso,w_);
	// pixel
	if (p) {
	  h_sh_chreliso_p.Fill(chreliso,w_);
	  // tracker short
	} else if (s) {
	  h_sh_chreliso_s.Fill(chreliso,w_);
	  // tracker long
	} else if (l) {
	  h_sh_chreliso_l.Fill(chreliso,w_);
	  }
      }
      
    } // End loop on tracks
    
    h_st.Fill(nst,w_);
    h_stc.Fill(nstc,w_);

    if (nstc > 0) {
      h_ngenLep.Fill(t.ngenLep + t.ngenTau,w_);
    }
    // Note, because we don't check stat23 (tau decay mode undefined) there's a ~5% error here for DY. Best to look at other dilep samples.
    if (nst > 0) {
      // none, e, mu, taue, taumu, tau1, tau3, 
      // ee, emu, etaue, etaumu, etau1, etau3, 
      // mumu, mutaue, mutaumu, mutau1, mutau3, 
      // tauetaue, tauetaumu, tauetau1, tauetau3, 
      // taumutaumu, taumutau1, taumutau3, 
      // tau1tau1, tau1tau3, 
      // tau3tau3      
      bool founde = false;
      bool foundmu = false;
      bool foundtaue = false;
      bool foundtaumu = false;
      bool foundtau1 = false;
      bool foundtau3 = false;
      const int ngenLep = t.ngenLep + t.ngenTau;
      for (int i_lep = 0; i_lep < t.ngenLep; i_lep++) {
	int pdgId = abs(t.genLep_pdgId[i_lep]);
	if (pdgId == 11) founde = true;
	else foundmu = true;
      }
      for (int i_tau = 0; i_tau < t.ngenTau; i_tau++) {
	int decayMode = abs(t.genTau_decayMode[i_tau]);
	if (decayMode == 11) foundtaue = true;
	else if (decayMode == 13) foundtaumu = true;
	else if (decayMode == 1) foundtau1 = true;
	else foundtau3 = true;
      }
      int lep_type = 0;
      if (ngenLep == 1) {
	if (founde) lep_type = 1;
	else if (foundmu) lep_type = 2;
	else if (foundtaue) lep_type = 3;
	else if (foundtaumu) lep_type = 4;
	else if (foundtau1) lep_type = 5;
	else lep_type = 6; // foundtau3
      } else if (ngenLep == 2) {
	if (founde) {
	  if (foundmu) lep_type = 8;
	  else if (foundtaue) lep_type = 9;
	  else if (foundtaumu) lep_type = 10;
	  else if (foundtau1) lep_type = 11;
	  else if (foundtau3) lep_type = 12;
	  else lep_type = 7; // founde was true twice
	}
	else if (foundmu) {
	  if (foundtaue) lep_type = 14;
	  else if (foundtaumu) lep_type = 15;
	  else if (foundtau1) lep_type = 16;
	  else if (foundtau3) lep_type = 17;
	  else lep_type = 13; // foundmu twice
	}
	else if (foundtaue) {
	  if (foundtaumu) lep_type = 19;
	  else if (foundtau1) lep_type = 20;
	  else if (foundtau3) lep_type = 21;
	  else lep_type = 18; // foundtaue twice
	}
	else if (foundtaumu) {
	  if (foundtau1) lep_type = 23;
	  else if (foundtau3) lep_type = 24;
	  else lep_type = 22; // foundtaumu twice
	}
	else if (foundtau1) {
	  if (foundtau3) lep_type = 26;
	  else lep_type = 25; // foundtau1 twice
	}
	else {
	  lep_type = 27; // foundtau3 twice
	}	
      } // ngenLep + ngenTau == 2
      h_lepType.Fill(lep_type,w_);
    } // if nst > 0
      
  }//end loop on events in a file
  
  cout << "About to write" << endl;
  
  mkdir = "mkdir -p drHists" + output_suffix;
  system(mkdir.c_str());

  TFile drfile(Form("drHists%s/%s.root",output_suffix.c_str(),sample),"RECREATE");
  h_el_dr.Write();
  h_mu_dr.Write();
  h_tau_dr.Write();
  h_el_ptr.Write();
  h_mu_ptr.Write();
  h_tau_ptr.Write();
  h_sh_el_dr.Write();
  h_sh_mu_dr.Write();
  h_sh_tau_dr.Write();
  h_sh_el_ptr.Write();
  h_sh_mu_ptr.Write();
  h_sh_tau_ptr.Write();
  drfile.Close();

  mkdir = "mkdir -p vetoHists" + output_suffix;
  system(mkdir.c_str());

  TFile vetofile(Form("vetoHists%s/%s.root",output_suffix.c_str(),sample),"RECREATE");
  vetofile.cd();
  h_etaphi_bar.Write();
  h_st_etaphi_bar.Write();
  h_etaphi_ecn.Write();
  h_st_etaphi_ecn.Write();
  h_etaphi_ecp.Write();
  h_st_etaphi_ecp.Write();
  vetofile.Close();
    
  TFile outfile_(output_name,"RECREATE"); 
  outfile_.cd();

  h_lepType.Write();
  
  h_st.Write();
  h_stc.Write();
  h_inclusive.Write();
  
  // STCands
  h_pt.Write();
  h_bardz.Write();
  h_bardxy.Write();
  h_ecdz.Write();
  h_ecdxy.Write();
  h_iso.Write();
  h_niso.Write();
  h_chiso.Write();
  h_reliso.Write();
  h_nreliso.Write();
  h_chreliso.Write();
  
  h_pt_p.Write();
  h_bardz_p.Write();
  h_bardxy_p.Write();
  h_ecdz_p.Write();
  h_ecdxy_p.Write();
  h_iso_p.Write();
  h_niso_p.Write();
  h_chiso_p.Write();
  h_reliso_p.Write();
  h_nreliso_p.Write();
  h_chreliso_p.Write();
  
  h_pt_s.Write();
  h_bardz_s.Write();
  h_bardxy_s.Write();
  h_ecdz_s.Write();
  h_ecdxy_s.Write();
  h_iso_s.Write();
  h_niso_s.Write();
  h_chiso_s.Write();
  h_reliso_s.Write();
  h_nreliso_s.Write();
  h_chreliso_s.Write();
  
  h_pt_l.Write();
  h_bardz_l.Write();
  h_bardxy_l.Write();
  h_ecdz_l.Write();
  h_ecdxy_l.Write();
  h_iso_l.Write();
  h_niso_l.Write();
  h_chiso_l.Write();
  h_reliso_l.Write();  
  h_nreliso_l.Write();  
  h_chreliso_l.Write();  
  
  /*
  h_pt_li.Write();
  h_bardz_li.Write();
  h_bardxy_li.Write();
  h_ecdz_li.Write();
  h_ecdxy_li.Write();
  h_iso_li.Write();
  h_niso_li.Write();
  h_chiso_li.Write();
  h_reliso_li.Write();  
  h_nreliso_li.Write();  
  h_chreliso_li.Write();  
  */

  // True Short Tracks
  h_sh_pt.Write();
  h_sh_bardz.Write();
  h_sh_bardxy.Write();
  h_sh_ecdz.Write();
  h_sh_ecdxy.Write();
  h_sh_iso.Write();
  h_sh_niso.Write();
  h_sh_chiso.Write();
  h_sh_reliso.Write();
  h_sh_nreliso.Write();
  h_sh_chreliso.Write();

  h_sh_pt_p.Write();
  h_sh_bardz_p.Write();
  h_sh_bardxy_p.Write();
  h_sh_ecdz_p.Write();
  h_sh_ecdxy_p.Write();
  h_sh_iso_p.Write();
  h_sh_niso_p.Write();
  h_sh_chiso_p.Write();
  h_sh_reliso_p.Write();
  h_sh_nreliso_p.Write();
  h_sh_chreliso_p.Write();

  h_sh_pt_s.Write();
  h_sh_bardz_s.Write();
  h_sh_bardxy_s.Write();
  h_sh_ecdz_s.Write();
  h_sh_ecdxy_s.Write();
  h_sh_iso_s.Write();
  h_sh_niso_s.Write();
  h_sh_chiso_s.Write();
  h_sh_reliso_s.Write();
  h_sh_nreliso_s.Write();
  h_sh_chreliso_s.Write();

  h_sh_pt_l.Write();
  h_sh_bardz_l.Write();
  h_sh_bardxy_l.Write();
  h_sh_ecdz_l.Write();
  h_sh_ecdxy_l.Write();
  h_sh_iso_l.Write();
  h_sh_niso_l.Write();
  h_sh_chiso_l.Write();
  h_sh_reliso_l.Write();  
  h_sh_nreliso_l.Write();  
  h_sh_chreliso_l.Write();  

  h_ngenLep.Write();
  
  outfile_.Close();

  cout << "Wrote everything" << endl;
  
  return 0;
}

int main (int argc, char ** argv) {

  if (argc < 4) {
    cout << "Usage: ./STCandLooper.exe <indir> <sample> <selection>" << endl;
    return 1;
  }

  STCandLooper * stcl = new STCandLooper();
  stcl->loop(argv[1],argv[2],atoi(argv[3]),atoi(argv[4]));
  return 0;
}

STCandLooper::STCandLooper() {}

STCandLooper::~STCandLooper() {};
