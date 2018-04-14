#include "STCandLooper.h"

using namespace std;

class mt2tree;
class sttree;

int STCandLooper::loop (char * infile_name, char * sample, int selectionMode, int vetoMode) {

  bool doHT = selectionMode == 1;
  bool doCRSL = selectionMode == 2;

  bool doGenVeto = vetoMode == 1;
  bool doEtaPhiVeto = vetoMode == 2;

  if (vetoMode < 0) {
    doGenVeto = true;
    doEtaPhiVeto = true;
  }

  string output_suffix("");
  if (doHT) output_suffix = "_HT";
  else if (doCRSL) output_suffix = "_CRSL";
    
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

  const float pt_min_st = 15;
  const float dxy_max_st = 0.02;
  const float dz_max_st = 0.05;
  const float iso_max_st = 10;
  const float reliso_max_st = 0.2;
  const float niso_max_st = 10;
  const float chiso_max_st = 10;
  const float nreliso_max_st = 0.1;
  const float chreliso_max_st = 0.1;

  // Book Histograms
  TH1::SetDefaultSumw2(true); // Makes histograms do proper error calculation automatically
  TH1F h_stc("h_stc","Short Track Candidate Count (All Selections)",4,0,4);
  TH1F h_st("h_st","True Short Track Count (All Selections)",4,0,4);

  TH2F h_etaphi("h_etaphi","All Tracks by #eta and #phi",1000,-2.4,2.4,1000,0,3.14159);
  TH2F h_st_etaphi("h_st_etaphi","All Short Tracks by #eta and #phi",1000,-2.4,2.4,1000,0,3.14159);

  TH1F h_inclusive("h_inclusive","Total Events in Sample",1,0,1);

  const int pt_n = 30; const float pt_min = 15; const float pt_max = 80;
  TH1F h_pt("h_pt","Distribution of Track p_{T}s",pt_n,pt_min,pt_max);
  TH1F h_pt_p("h_pt_p","Distribution of Track p_{T}s (pixel-only)",pt_n,pt_min,pt_max);
  TH1F h_pt_s("h_pt_s","Distribution of Track p_{T}s (not pixel-only, < 7 layers)",pt_n,pt_min,pt_max);
  TH1F h_pt_l("h_pt_l","Distribution of Track p_{T}s (>= 7 layers, > 1 lost outer hits)",pt_n,pt_min,pt_max);
  //TH1F h_pt_li("h_pt_li","Distribution of Track p_{T}s (>=7 layers)",pt_n,pt_min,pt_max);

  const int dxy_n = 40; const float dxy_min = 0; const float dxy_max = 0.2;
  TH1F h_bardxy("h_bardxy","Distribution of Barrel Track dxy",dxy_n,dxy_min,dxy_max);
  TH1F h_bardxy_p("h_bardxy_p","Distribution of Barrel Track dxy (pixel-only)",dxy_n,dxy_min,dxy_max);
  TH1F h_bardxy_s("h_bardxy_s","Distribution of Barrel Track dxy (not pixel-only, < 7 layers)",dxy_n,dxy_min,dxy_max);
  TH1F h_bardxy_l("h_bardxy_l","Distribution of Barrel Track dxy (>= 7 layers, > 1 lost outer hits)",dxy_n,dxy_min,dxy_max);
  //TH1F h_bardxy_li("h_bardxy_li","Distribution of Barrel Track dxy (>=7 layers)",dxy_n,dxy_min,dxy_max);

  const int dz_n = 30; const float dz_min = 0; const float dz_max = 0.5;
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

  const int iso_n = 30; const float iso_min = 0; const float iso_max = 50;
  TH1F h_iso("h_iso","Distribution of Track Isolation",iso_n,iso_min,iso_max);
  TH1F h_iso_p("h_iso_p","Distribution of Track Isolation (pixel-only)",iso_n,iso_min,iso_max);
  TH1F h_iso_s("h_iso_s","Distribution of Track Isolation (not pixel-only, < 7 layers)",iso_n,iso_min,iso_max);
  TH1F h_iso_l("h_iso_l","Distribution of Track Isolation (>= 7 layers, > 1 lost outer hits)",iso_n,iso_min,iso_max);
  //TH1F h_iso_li("h_iso_li","Distribution of Track Isolation (>=7 layers)",iso_n,iso_min,iso_max);

  const int niso_n = 30; const float niso_min = 0; const float niso_max = 50;
  TH1F h_niso("h_niso","Distribution of Track NeutIso (#DeltaR < 0.05)",niso_n,niso_min,niso_max);
  TH1F h_niso_p("h_niso_p","Distribution of Track NeutIso (#DeltaR < 0.05) (pixel-only)",niso_n,niso_min,niso_max);
  TH1F h_niso_s("h_niso_s","Distribution of Track NeutIso (#DeltaR < 0.05) (not pixel-only, < 7 layers)",niso_n,niso_min,niso_max);
  TH1F h_niso_l("h_niso_l","Distribution of Track NeutIso (#DeltaR < 0.05) (>= 7 layers, > 1 lost outer hits)",niso_n,niso_min,niso_max);
  //TH1F h_niso_li("h_niso_li","Distribution of Track NeutIso (#DeltaR < 0.05) (>=7 layers)",niso_n,niso_min,niso_max);

  const int chiso_n = 20; const float chiso_min = 0; const float chiso_max = 50;
  TH1F h_chiso("h_chiso","Distribution of Track ChIso (#DeltaR < 0.01)",chiso_n,chiso_min,chiso_max);
  TH1F h_chiso_p("h_chiso_p","Distribution of Track ChIso (#DeltaR < 0.01) (pixel-only)",chiso_n,chiso_min,chiso_max);
  TH1F h_chiso_s("h_chiso_s","Distribution of Track ChIso (#DeltaR < 0.01) (not pixel-only, < 7 layers)",chiso_n,chiso_min,chiso_max);
  TH1F h_chiso_l("h_chiso_l","Distribution of Track ChIso (#DeltaR < 0.01) (>= 7 layers, > 1 lost outer hits)",chiso_n,chiso_min,chiso_max);
  //TH1F h_chiso_li("h_chiso_li","Distribution of Track ChIso (#DeltaR < 0.01) (>=7 layers)",chiso_n,chiso_min,chiso_max);

  const int reliso_n = 30; const float reliso_min = 0; const float reliso_max = 0.8;
  TH1F h_reliso("h_reliso","Distribution of Track Relative Isolation",reliso_n,reliso_min,reliso_max);
  TH1F h_reliso_p("h_reliso_p","Distribution of Track Relative Isolation (pixel-only)",reliso_n,reliso_min,reliso_max);
  TH1F h_reliso_s("h_reliso_s","Distribution of Track Relative Isolation (not pixel-only, < 7 layers)",reliso_n,reliso_min,reliso_max);
  TH1F h_reliso_l("h_reliso_l","Distribution of Track Relative Isolation (>= 7 layers, > 1 lost outer hits)",reliso_n,reliso_min,reliso_max);
  //TH1F h_reliso_li("h_reliso_li","Distribution of Track Relative Isolation (>=7 layers)",reliso_n,reliso_min,reliso_max);

  const int chreliso_n = 30; const float chreliso_min = 0; const float chreliso_max = 0.5;
  TH1F h_chreliso("h_chreliso","Distribution of Track Ch Rel Iso",chreliso_n,chreliso_min,chreliso_max);
  TH1F h_chreliso_p("h_chreliso_p","Distribution of Track Ch Rel Iso (pixel-only)",chreliso_n,chreliso_min,chreliso_max);
  TH1F h_chreliso_s("h_chreliso_s","Distribution of Track Ch Rel Iso (not pixel-only, < 7 layers)",chreliso_n,chreliso_min,chreliso_max);
  TH1F h_chreliso_l("h_chreliso_l","Distribution of Track Ch Rel Iso (>= 7 layers, > 1 lost outer hits)",chreliso_n,chreliso_min,chreliso_max);
  //TH1F h_chreliso_li("h_chreliso_li","Distribution of Track Ch Rel Iso (>=7 layers)",chreliso_n,chreliso_min,chreliso_max);

  const int nreliso_n = 30; const float nreliso_min = 0; const float nreliso_max = 0.5;
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
  } else {
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

    if (doCRSL) {
      if (t.nLepLowMT != 1) continue;
      float sumJet = 0;
      for (int i_jet = 0; i_jet < t.njet; i_jet++) {
	sumJet += t.jet_pt[i_jet];
      }
      if (sumJet < 100) continue;
    }
    
    /*
     * Track Loop
     */
    const int ntracks = t.ntracks;
    int nstc = 0; int nst = 0;

    for (int i_trk = 0; i_trk < ntracks; i_trk++) {   

      h_etaphi.Fill(t.track_eta[i_trk],t.track_phi[i_trk],1);
      if (t.track_isshort[i_trk]) {
	h_st_etaphi.Fill(t.track_eta[i_trk],t.track_phi[i_trk],1);
      } 

      continue;

      // Lepton Veto
      bool lepOverlap = false;
      // Check reco leps
      for (int i_lep = 0; i_lep < t.nlep; i_lep++) {
	if (DeltaR(t.lep_eta[i_lep],t.lep_phi[i_lep],t.track_eta[i_trk],t.track_phi[i_trk]) < 0.1) {
	  lepOverlap = true;
	  break;
	}
      }
      if (lepOverlap) continue; // short track candidates can't overlap with leptons
      // If we get here, no reco leps overlapped. Now check PF leps
      for (int i_isoT = 0; i_isoT < t.nlep; i_isoT++) {
	int pdgId = abs(t.isoTrack_pdgId[i_isoT]);
	if (pdgId != 11 && pdgId != 13) continue;
	if (DeltaR(t.isoTrack_eta[i_isoT],t.isoTrack_phi[i_isoT],t.track_eta[i_trk],t.track_phi[i_trk]) < 0.1) {
	  lepOverlap = true;
	  break;
	}
      }
      if (lepOverlap) continue; // short track candidates can't overlap with leptons

      if (doGenVeto) {
	// Veto all lepton overlap tracks
	for (int i_lep = 0; i_lep < t.ngenLep; i_lep++) {
	  //	if (abs(t.genLep_pdgId[i_lep]) != 11) continue;
	  float lep_eta = t.genLep_eta[i_lep];
	  float lep_phi = t.genLep_phi[i_lep];
	  if (DeltaR(lep_eta, lep_phi, t.track_eta[i_trk], t.track_phi[i_trk]) > 0.1 ) continue;
	  float ratio_pt = t.genLep_pt[i_lep] / t.track_pt[i_trk];
	  int sourceId = fabs(t.genLep_sourceId[i_lep]);
	  if ( (ratio_pt < 0.5 || ratio_pt > 2.0) && t.track_isshort[i_trk] && (sourceId == 15 || sourceId == 23 || sourceId == 24) ) {
	    string source = "Z";
	    if (sourceId == 24) source = "W";
	    else if (sourceId == 15) source = "tau";
	    cout << "Gen lepton with large pt difference (" << ratio_pt << ") overlapped a short track. " 
		 << run << ":" << lumi << ":" << evt 
		 << " Track: " << i_trk << " Gen Lepton: " << i_lep << " Id: " << fabs(t.genLep_pdgId[i_lep]) << " Source: " << source
		 << endl;
	  }
	  // Matched to gen lepton, probably disappearing. Don't have the calorimeter reading needed to confirm but...
	  lepOverlap = true;
	  break;
	}
	if (lepOverlap) continue;
	
	// Veto all tau overlap tracks
	for (int i_tau = 0; i_tau < t.ngenTau; i_tau++) {
	  float tau_eta = t.genTau_eta[i_tau];
	  float tau_phi = t.genTau_phi[i_tau];
	  if (DeltaR(tau_eta, tau_phi, t.track_eta[i_trk], t.track_phi[i_trk]) > 0.1 ) continue;
	  float ratio_pt = t.genTau_pt[i_tau] / t.track_pt[i_trk];
	  int sourceId = fabs(t.genTau_sourceId[i_tau]);
	  if ( (ratio_pt < 0.5 || ratio_pt > 2.0) && t.track_isshort[i_trk] && (sourceId == 15 || sourceId == 23 || sourceId == 24) ) {
	    string source = "Z";
	    if (sourceId == 24) source = "W";
	    else if (sourceId == 15) source = "tau";
	    cout << "Gen tauton with large pt difference (" << ratio_pt << ") overlapped a short track. " 
		 << run << ":" << lumi << ":" << evt 
		 << " Track: " << i_trk << " Gen Tau: " << i_tau << " Id: " << fabs(t.genTau_pdgId[i_tau]) << " Source: " << source
		 << endl;
	  }
	  // Matched to gen tau, probably disappearing. Don't have the calorimeter reading needed to confirm but...
	  lepOverlap = true;
	  break;
	}
	if (lepOverlap) continue;
      }


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
      
  }//end loop on events in a file
  
  cout << "About to write" << endl;
  
  mkdir = "mkdir -p vetoHists" + output_suffix;
  system(mkdir.c_str());

  TFile vetofile(Form("vetoHists%s/%s.root",output_suffix.c_str(),sample),"RECREATE");
  vetofile.cd();
  h_etaphi.Write();
  h_st_etaphi.Write();
  vetofile.Close();
    
  return 0;

  TFile outfile_(output_name,"RECREATE"); 
  outfile_.cd();
  
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

  outfile_.Close();

  cout << "Wrote everything" << endl;
  
  return 0;
}

int main (int argc, char ** argv) {

  if (argc < 4) {
    cout << "Usage: ./STCandLooper.exe <indir> <sample> <doCRSL>" << endl;
    return 1;
  }

  STCandLooper * stcl = new STCandLooper();
  stcl->loop(argv[1],argv[2],atoi(argv[3]),atoi(argv[4]));
  return 0;
}

STCandLooper::STCandLooper() {}

STCandLooper::~STCandLooper() {};
