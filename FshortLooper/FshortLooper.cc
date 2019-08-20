#include "FshortLooper.h"

using namespace std;
using namespace duplicate_removal;

class mt2tree;

const bool verbose = false;

const bool recalculate = true; // recalculate Fshort with non-standard (ie not in babies) isolation and quality cutoffs, see below
// only with recalculate = true
const int applyRecoVeto = 2; // 0: None, 1: use MT2 ID leptons for the reco veto, 2: use any Reco ID (Default: 2)
const bool increment17 = false;
const float isoSTC = 6, qualSTC = 3; // change these if recalculating Fshort

// turn on to apply json file to data
const bool applyJSON = true;

const bool applyISRWeights = true; // evt_id is messed up in current babies, and we don't have isr weights implemented for 2017 MC anyway

const bool doNTrueIntReweight = true; // evt_id is messed up in current babies

const bool applyDileptonTriggerWeights = true;

const bool blind = false;

const bool skipHighWeights = false; // turn on to skip MC events with weight > 1, to keep errors reasonable

const bool fillCutHists = true; // turn on to fill extra histograms
const bool fillUnimportantCutHists = false; // turn on to fill Nvertex, Eta, Ntag, DphiMet, and HitSignature histograms
const bool doMatching = true; // turn on gen-matching (chargino matching is always on, for signal)

const bool fillNM1Hists = true; // turn on to fill NM1 histograms (need to set recalculate and fillCutHists to true)

bool onlyMatchedTracks = true; // for fshort, use only matched chargino tracks if signal

bool doHEMveto = true;
int HEM_startRun = 319077; // affects 38.58 out of 58.83 fb-1 in 2018
uint HEM_fracNum = 1286, HEM_fracDen = 1961; // 38.58/58.82 ~= 1286/1961. Used for figuring out if we should veto MC events
float HEM_ptCut = 30.0;  // veto on jets above this threshold
float HEM_region[4] = {-4.7, -1.4, -1.6, -0.8}; // etalow, etahigh, philow, phihigh

// turn on to apply L1prefire inefficiency weights to MC (2016/17 only)
bool applyL1PrefireWeights = true;

TFile VetoFile("veto_etaphi.root");
TH2F* veto_etaphi_16 = (TH2F*) VetoFile.Get("etaphi_veto_16");
TH2F* veto_etaphi_17 = (TH2F*) VetoFile.Get("etaphi_veto_17");
TH2F* veto_etaphi_18 = (TH2F*) VetoFile.Get("etaphi_veto_18");

int FshortLooper::InEtaPhiVetoRegion(float eta, float phi, int year) {
  if (fabs(eta) > 2.4) return -1;
  TH2F* veto_hist = veto_etaphi_16;
  if (year == 2017) veto_hist = veto_etaphi_17;
  if (year == 2018) veto_hist = veto_etaphi_18;
  return (int) veto_hist->GetBinContent(veto_hist->FindBin(eta,phi));
}

bool FshortLooper::FillHists(const vector<TH2D*>& hists, const double weight, const int fill_type, const int len_index) {
  if (unlikely(len_index < 1 || len_index > 6)) return false;
  if (unlikely(fill_type < 1 || fill_type > 2)) return false;
  for (vector<TH2D*>::const_iterator hist = hists.begin(); hist != hists.end(); hist++) {
    (*hist)->Fill(len_index - 0.5,(double) fill_type, weight);
  }
  return true;
}

int FshortLooper::loop(TChain* ch, char * outtag, std::string config_tag, std::string mode) {
  string tag(outtag);

  cout << "mode: " << mode << endl;

  bool applyCaloSel = mode.find("skipCaloSel") == std::string::npos;
  cout << (applyCaloSel ? "applying" : "skipping") << " calo sel" << endl;
  bool applyKinematicPreselection = mode.find("skipKinSel") == std::string::npos;
  cout << (applyKinematicPreselection ? "applying" : "skipping") << " kinematic preselection" << endl;
  bool doNoLeps = mode.find("useZll") == std::string::npos;
  cout << (doNoLeps ? "All-hadronic" : "Zll") << endl;

  //bool isSignal = t.evt_id >= 1000;
  bool isSignal = tag.find("T1") != std::string::npos || tag.find("T2") != std::string::npos;

  cout << (isSignal ? "isSignal True" : "isSignal False") << endl;

  if (!isSignal) onlyMatchedTracks = false;

  TH1F* h_xsec = 0;
  if (isSignal) {
    TFile * f_xsec = TFile::Open("../babymaker/data/xsec_susy_13tev_run2.root");
    TH1F* h_xsec_orig = (TH1F*) f_xsec->Get("h_xsec_gluino");
    h_xsec = (TH1F*) h_xsec_orig->Clone("h_xsec");
    h_xsec->SetDirectory(0);
    f_xsec->Close();
  }

  cout << "FshortLooper::loop" << endl;

  // Book histograms
  TH1::SetDefaultSumw2(true); // Makes histograms do proper error calculation automatically

  TH2D* h_fsMR_Base = new TH2D("h_fsMR_Base","ST and STC Counts by Length",6,0,6,3,0,3);
  h_fsMR_Base->GetXaxis()->SetBinLabel(1,"P");
  h_fsMR_Base->GetXaxis()->SetBinLabel(2,"P3");
  h_fsMR_Base->GetXaxis()->SetBinLabel(3,"P4");
  h_fsMR_Base->GetXaxis()->SetBinLabel(4,"M");
  h_fsMR_Base->GetXaxis()->SetBinLabel(5,"L");
  h_fsMR_Base->GetXaxis()->SetBinLabel(6,"Lrej");
  h_fsMR_Base->GetYaxis()->SetBinLabel(1,"f_{short}");
  h_fsMR_Base->GetYaxis()->SetBinLabel(2,"ST");
  h_fsMR_Base->GetYaxis()->SetBinLabel(3,"STC");

  // Multijet
  unordered_map<string,TH2D*> fsHists;
  string variations[] = {"Baseline","gt1200HT","lt1200HT","MET30","MET100","MET250","HT250","HT450","HT450MET100"};
  string njets[] = {"","_23","_4"};
  string pts[] = {"","_hipt","_lowpt"};
  string regions[] = {"Incl","MR","VR","SR"};
  for (int var = 0; var < 9; var++) {
    string variation = variations[var];
    for (int nj = 0; nj < 4; nj++) {
      string njet = njets[nj];
      for (int reg = 0; reg < 4; reg++) {
	string region = regions[reg];
	for (int ptbin = 0; ptbin < 3; ptbin++) {
	  string hname = "h_fs"+region+njet+"_"+variation+pts[ptbin];
	  cout << hname << endl;
	  fsHists[hname] = (TH2D*) h_fsMR_Base->Clone(hname.c_str());
	}
      }
    }
  }
  // Monojet
  string jetpt[] = {"SR","VR","MR_Baseline","MR_pt200","MR_pt250","MR_MetHt250","MR_MetHtPt250"};
  string trackpt[] = {"","_hipt","_lowpt"};
  for (int jetptbin = 0; jetptbin < 7; jetptbin++) {
    string jpt = jetpt[jetptbin];
    for (int trackptbin = 0; trackptbin < 3; trackptbin++) {
      string hname = "h_fs_1_"+jpt+trackpt[trackptbin];
      cout << hname << endl;
      fsHists[hname] = (TH2D*) h_fsMR_Base->Clone(hname.c_str());
    }
  }
  // Monojet stability TProfiles
  TH1D * h_jet1_phi_MR_ST = new TH1D("h_jet1_phi_MR_ST","#phi_{Jet 1} in MR (ST);#phi_{Jet 1};Count",20,-TMath::Pi(),TMath::Pi());
  TH1D * h_jet1_phi_VR_ST = new TH1D("h_jet1_phi_VR_ST","#phi_{Jet 1} in VR (ST);#phi_{Jet 1};Count",20,-TMath::Pi(),TMath::Pi());
  TH1D * h_jet1_phi_SR_ST = new TH1D("h_jet1_phi_SR_ST","#phi_{Jet 1} in SR (ST);#phi_{Jet 1};Count",20,-TMath::Pi(),TMath::Pi());
  TH1D * h_jet1_phi_MR_STC = new TH1D("h_jet1_phi_MR_STC","#phi_{Jet 1} in MR (STC);#phi_{Jet 1};Count",20,-TMath::Pi(),TMath::Pi());
  TH1D * h_jet1_phi_VR_STC = new TH1D("h_jet1_phi_VR_STC","#phi_{Jet 1} in VR (STC);#phi_{Jet 1};Count",20,-TMath::Pi(),TMath::Pi());
  TH1D * h_jet1_phi_SR_STC = new TH1D("h_jet1_phi_SR_STC","#phi_{Jet 1} in SR (STC);#phi_{Jet 1};Count",20,-TMath::Pi(),TMath::Pi());

  TH1D * h_jet1_eta_MR_ST = new TH1D("h_jet1_eta_MR_ST","#eta_{Jet 1} in MR (ST);#eta_{Jet 1};Count",24,-2.4,2.4);
  TH1D * h_jet1_eta_VR_ST = new TH1D("h_jet1_eta_VR_ST","#eta_{Jet 1} in VR (ST);#eta_{Jet 1};Count",24,-2.4,2.4);
  TH1D * h_jet1_eta_SR_ST = new TH1D("h_jet1_eta_SR_ST","#eta_{Jet 1} in SR (ST);#eta_{Jet 1};Count",24,-2.4,2.4);
  TH1D * h_jet1_eta_MR_STC = new TH1D("h_jet1_eta_MR_STC","#eta_{Jet 1} in MR (STC);#eta_{Jet 1};Count",24,-2.4,2.4);
  TH1D * h_jet1_eta_VR_STC = new TH1D("h_jet1_eta_VR_STC","#eta_{Jet 1} in VR (STC);#eta_{Jet 1};Count",24,-2.4,2.4);
  TH1D * h_jet1_eta_SR_STC = new TH1D("h_jet1_eta_SR_STC","#eta_{Jet 1} in SR (STC);#eta_{Jet 1};Count",24,-2.4,2.4);

  std::unordered_map<TH2D*, TH1D*> max_stc_weights;
  std::unordered_map<TH2D*, TH1D*> max_st_weights;
  for (std::unordered_map<string,TH2D*>::iterator fshist = fsHists.begin(); fshist != fsHists.end(); fshist++) {
    TH2D* fsh = fshist->second;
    int nbins = fsh->GetNbinsX();
    TH1D* max_weight_STC = new TH1D(Form("%s_max_weight_STC",fsh->GetName()),"Maximum STC Weight",nbins,0,nbins);
    TH1D* max_weight_ST = new TH1D(Form("%s_max_weight_ST",fsh->GetName()),"Maximum ST Weight",nbins,0,nbins);
    max_stc_weights[fsh] = max_weight_STC;
    max_st_weights[fsh] = max_weight_ST;
  }

  cout << "Booked fs hists" << endl;

  const int n_xy_bins = 16;
  const float xy_bins[n_xy_bins + 1] = {5, 7.5, 10, 12.5, 15.0, 17.5, 20, 30, 40, 50, 60, 70, 80, 90, 95, 97.5, 100};
  // Signal efficiency
  TH1D* h_sigpassST = new TH1D("h_sigpass","Signal Tracks Passing Selections vs. Transverse Decay Length, Starting from p_{T} > 15 GeV lostTracks",n_xy_bins,xy_bins);
  TH1D* h_sigpassSTC = new TH1D("h_sigpassSTC","Signal Tracks Passing Selections vs. Transverse Decay Length, Starting from p_{T} > 15 GeV lostTracks",n_xy_bins,xy_bins);
  TH1D* h_sigeffST = new TH1D("h_sigeff","Signal ST Efficiency vs. Transverse Decay Length, Starting from p_{T} > 15 GeV lostTracks",n_xy_bins,xy_bins);
  TH1D* h_sigeffSTC = new TH1D("h_sigeffSTC","Signal STC Efficiency vs. Transverse Decay Length, Starting from p_{T} > 15 GeV lostTracks",n_xy_bins,xy_bins);
  TH1D* h_CharLength = new TH1D("h_CharLength","Chargino Track Length",n_xy_bins,xy_bins);
  TH1D* h_CharLength_etaphi = new TH1D("h_CharLength_etaphi","Chargino Track Length (within #eta-#phi Acceptance)",n_xy_bins,xy_bins);

  unordered_map<string,TH2D*> mtptHists;
  unordered_map<string,TH2D*> mtmetHists;
  unordered_map<string,TH2D*> mt2metHists;
  unordered_map<string,TH2D*> mtmt2Hists;
  unordered_map<string,TH2D*> etaphiHists;
  unordered_map<string,TH1D*> cutHists;
  string categories[] = {"ST","STC"};
  //  string lengths[] = {"P","P3","P4","M","L","Lrej","1L","1Lrej"};
  string lengths[] = {"All","P","P3","P4","M","L","Lrej"};
  //  string njethts[] = {"","_23","_4","_L","_H","_23L","_23H","_4L","_4H"};
  string njethts[] = {""};
  vector<string> matches;
  matches.push_back("_");
  if (isSignal || doMatching) matches.push_back("_chargino_");
  if (doMatching) {
    matches.push_back("_ewke_");
    matches.push_back("_ewkm_");
    matches.push_back("_tewke_");
    matches.push_back("_tewkm_");
    matches.push_back("_newk_");
    matches.push_back("_1newk_");
    matches.push_back("_3newk_");
  }
  TH1D* h_nv_fullincl = new TH1D("h_nv_fullincl","N_{V}",14,0,70);
  if (fillCutHists) {
    TH2D* h_mtpt_base = new TH2D("h_mtpt_base","p_{T} x M_{T}(Track,MET)",10,0,200,10,0,500);
    TH2D* h_mtmet_base = new TH2D("h_mtmet_base","MET x M_{T}(Track,MET);M_{T} (GeV);MET (GeV)",10,0,500,10,0,500);
    TH2D* h_mt2met_base = new TH2D("h_mt2met_base","MET x M_{T2};M_{T2} (GeV);MET (GeV)",10,0,500,10,0,500);
    TH2D* h_mtmt2_base = new TH2D("h_mtmt2_base","M_{T2} x M_{T}(Track,MET);M_{T} (GeV);M_{T2} (GeV)",10,0,500,10,0,500);
    TH2D* h_etaphi_base = new TH2D("h_etaphi_base","#eta and #phi",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
    // Full hp takes too long
    //  TH1D* h_hp_base = new TH1D("h_hp_base","Hit Signature",(1<<17)-1,0,(1<<17)-1); // 2^17 - 1 = 131071; there are 18 layers (at most, ie 2^19 - 1 max hit pattern) in 2017, minus 2 for 2 lohs
    TH1D* h_hp_base = new TH1D("h_hp_base","Hit Signature",(1<<7)-1,0,(1<<7)-1); // store only inner 6 hits (M)
    for (int ibin = 0; ibin < h_hp_base->GetNbinsX(); ibin++) {
      h_hp_base->GetXaxis()->SetBinLabel(ibin+1,HitSigStr(ibin,6).c_str());
    }
    TH1D* h_dz_base = new TH1D("h_dz_base","dz",10,0,0.05);
    TH1D* h_dxy_base = new TH1D("h_dxy_base","dxy",10,0,0.02);
    TH1D* h_nv_base = new TH1D("h_nv_base","N_{V}",14,0,70);
    TH1D* h_nlayer_base = new TH1D("h_nlayer_base","N_{Layer}",11,3,14);
    TH1D* h_pterr_base = new TH1D("h_pterr_base","#delta p_{T}/p_{T}^{2}",10,0,0.2);
    TH1D* h_iso_base = new TH1D("h_iso_base","AbsIso",20,0,10);
    TH1D* h_reliso_base = new TH1D("h_reliso_base","RelIso",20,0,0.2);
    TH1D* h_niso_base = new TH1D("h_niso_3c","Tight Cone Neutral Iso",20,0,10);
    TH1D* h_nreliso_base = new TH1D("h_nreliso_base","Tight Cone Neu RelIso",20,0,0.1);
    TH1D* h_eta_base = new TH1D("h_eta_base","|#eta|",24,0,2.4);
    TH1D* h_pt_base = new TH1D("h_pt_base","p_{T}",10,15,190);
    TH1D* h_mt_base = new TH1D("h_mt_base","M_{T}(Track,MET)",6,0,300);
    double mt2bins[14] = {60,68,76,84,92,100,120,140,200,250,300,350,400,500};
    TH1D* h_mt2_base = new TH1D("h_mt2_base","MT2",13,mt2bins);
    TH1D* h_met_base = new TH1D("h_met_base","MET",10,250,750);
    TH1D* h_ht_base = new TH1D("h_ht_base","H_{T}",10,250,1250);
    TH1D* h_nj_base = new TH1D("h_nj_base","N_{Jet}",8,2,10);
    TH1D* h_dphiMet_base = new TH1D("h_dphiMet_base", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
    TH1D* h_adjMet_base = new TH1D("h_adjMet_base", "MET-ST",15,0,750);
    TH1D* h_deltaMet_base = new TH1D("h_deltaMet_base", "#DeltaMET, after subtracting ST",9,-1,1);
    TH1D* h_nb_base = new TH1D("h_nb_base","N_{Tag}",4,0,4);
    for (int cat = 0; cat < 2; cat++) {
      string category = categories[cat];
      for (int len = 0; len < 7; len++) {
	string length = lengths[len];
	cout << "Booked cut hists for " << category + " " + length << endl;
	for (unsigned int matchidx = 0; matchidx < matches.size(); matchidx++) {
	  string match = matches[matchidx];
	  //	  for (int njht = 0; njht < 9; njht++) {
	  for (int njht = 0; njht < 1; njht++) {
	    string njetht = njethts[njht];
	    for (int reg = 0; reg < 4; reg++) {
	      string region = regions[reg];
	      string suffix = region+match+category+"_"+length+njetht;
	      // MtPt
	      string hname = "h_mtpt_"+suffix;
	      mtptHists[hname] = (TH2D*) h_mtpt_base->Clone(hname.c_str());
	      mtptHists[hname]->SetTitle( Form("%s of %s %ss;M_{T} (GeV);p_{T} (GeV)",h_mtpt_base->GetTitle(),length.c_str(),category.c_str()) );
	      // MtMET
	      hname = "h_mtmet_"+suffix;
	      mtmetHists[hname] = (TH2D*) h_mtmet_base->Clone(hname.c_str());
	      mtmetHists[hname]->SetTitle( Form("%s of %s %ss;M_{T} (GeV);MET (GeV)",h_mtmet_base->GetTitle(),length.c_str(),category.c_str()) );
	      // MT2 MET
	      hname = "h_mt2met_"+suffix;
	      mt2metHists[hname] = (TH2D*) h_mt2met_base->Clone(hname.c_str());
	      mt2metHists[hname]->SetTitle( Form("%s of %s %ss;M_{T2} (GeV);MET (GeV)",h_mt2met_base->GetTitle(),length.c_str(),category.c_str()) );
	      // MtMT2
	      hname = "h_mtmt2_"+suffix;
	      mtmt2Hists[hname] = (TH2D*) h_mtmt2_base->Clone(hname.c_str());
	      mtmt2Hists[hname]->SetTitle( Form("%s of %s %ss;M_{T} (GeV);M_{T2} (GeV)",h_mtmt2_base->GetTitle(),length.c_str(),category.c_str()) );
	      // EtaPhi
	      hname = "h_etaphi_"+suffix;
	      etaphiHists[hname] = (TH2D*) h_etaphi_base->Clone(hname.c_str());
	      // Dz
	      hname = "h_dz_"+suffix;
	      cutHists[hname] = (TH1D*) h_dz_base->Clone(hname.c_str());
	      // Dxy
	      hname = "h_dxy_"+suffix;
	      cutHists[hname] = (TH1D*) h_dxy_base->Clone(hname.c_str());
	      // pterr / pt^2
	      hname = "h_pterr_"+suffix;
	      cutHists[hname] = (TH1D*) h_pterr_base->Clone(hname.c_str());
	      // Abs Iso 0.3
	      hname = "h_iso_"+suffix;
	      cutHists[hname] = (TH1D*) h_iso_base->Clone(hname.c_str());
	      // Rel Iso 0.3
	      hname = "h_reliso_"+suffix;
	      cutHists[hname] = (TH1D*) h_reliso_base->Clone(hname.c_str());
	      // N Iso 0.05
	      hname = "h_niso_"+suffix;
	      cutHists[hname] = (TH1D*) h_niso_base->Clone(hname.c_str());
	      // N RelIso 0.05
	      hname = "h_nreliso_"+suffix;
	      cutHists[hname] = (TH1D*) h_nreliso_base->Clone(hname.c_str());
	      // Pt
	      hname = "h_pt_"+suffix;
	      cutHists[hname] = (TH1D*) h_pt_base->Clone(hname.c_str());	    
	      // Eta
	      hname = "h_eta_"+suffix;
	      cutHists[hname] = (TH1D*) h_eta_base->Clone(hname.c_str());	    
	      // Mt
	      hname = "h_mt_"+suffix;
	      cutHists[hname] = (TH1D*) h_mt_base->Clone(hname.c_str());	    
	      // MT2
	      hname = "h_mt2_"+suffix;
	      cutHists[hname] = (TH1D*) h_mt2_base->Clone(hname.c_str());	    
	      // MET
	      hname = "h_met_"+suffix;
	      cutHists[hname] = (TH1D*) h_met_base->Clone(hname.c_str());	    
	      // HT
	      hname = "h_ht_"+suffix;
	      cutHists[hname] = (TH1D*) h_ht_base->Clone(hname.c_str());	    
	      // NJ
	      hname = "h_nj_"+suffix;
	      cutHists[hname] = (TH1D*) h_nj_base->Clone(hname.c_str());	    
	      // N layer
	      hname = "h_nlayer_"+suffix;
	      cutHists[hname] = (TH1D*) h_nlayer_base->Clone(hname.c_str());
	      // dphi Met
	      hname = "h_dphiMet_"+suffix;
	      cutHists[hname] = (TH1D*) h_dphiMet_base->Clone(hname.c_str());	    
	      // adj Met
	      hname = "h_adjMet_"+suffix;
	      cutHists[hname] = (TH1D*) h_adjMet_base->Clone(hname.c_str());	    
	      // delta Met
	      hname = "h_deltaMet_"+suffix;
	      cutHists[hname] = (TH1D*) h_deltaMet_base->Clone(hname.c_str());	    
	      // N vertex
	      hname = "h_nv_"+suffix;
	      cutHists[hname] = (TH1D*) h_nv_base->Clone(hname.c_str());
	      if (fillUnimportantCutHists) {
		// Ntag
		hname = "h_nb_"+suffix;
		cutHists[hname] = (TH1D*) h_nb_base->Clone(hname.c_str());	    
		// Hit Pattern
		hname = "h_hp_"+suffix;
		cutHists[hname] = (TH1D*) h_hp_base->Clone(hname.c_str());
		if (len < 3) cutHists[hname]->GetXaxis()->SetRange(0,(1<<5)-1); // At most 4 layers for P tracks
		else if (len == 4) cutHists[hname]->GetXaxis()->SetRange(0,(1<<7)-1); // At most 6 layers for M tracks
	      }
	    }
	  }
	}
      }
    }
  }
  unordered_map<string,TH1D*> cutHistsNM1;
  if (fillNM1Hists) {
    cout << "Booking NM1 hists" << endl;
    for (unordered_map<string,TH1D*>::iterator hist = cutHists.begin(); hist != cutHists.end(); hist++) {
      string hname = (hist->first)+"_NM1"; 
      if (hname.find("iso") == string::npos && hname.find("pterr") == string::npos && hname.find("dz") == string::npos && hname.find("dxy") == string::npos) continue;
      TH1D* cutHistNM1 = (TH1D*) (hist->second)->Clone(hname.c_str());
      cutHistsNM1[hname] = cutHistNM1;
    }
  }


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
    fillTriggerVector(t, trigs_doubleLep_, config_.triggers["DilepOF"]);
  }
  else if (config_tag.find("mc") == std::string::npos) {
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
  cout << "processing " << nEventsTree << " events" << endl;
  int nDuplicates = 0;
  int denomChargino15 = 0; int denomChargino50 = 0;
  int chSTp15 = 0; int chSTm15 = 0; int chSTl15 = 0;
  int chSTp50 = 0; int chSTm50 = 0; int chSTl50 = 0;
  for( unsigned int event = 0; event < nEventsTree; ++event) {    
  //for( unsigned int event = 0; event < 10; ++event) {    
    //if (event % 100000 == 0) cout << 100.0 * event / nEventsTree  << "%" << endl;

    if (verbose) cout << ">>>Event: " << event << endl;

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

    if (unlikely(t.nVert <= 0)) {
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
    //    if (config_.filters["badChargedCandidateFilter"] && !t.Flag_badChargedCandidateFilter) continue; 
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
    
    if (verbose) cout << "Passed all filters and sanity checks" << endl;

    // apply HEM veto, and simulate effects in MC
    if(doHEMveto && config_.year == 2018){
      bool hasHEMjet = false;
      float lostHEMtrackPt = 0;
      // monojet, VL, and L HT are handled specially (lower pt threshold, and apply lostTrack veto)
      bool isLowHT = (t.nJet30==1 || (t.nlep != 2 && t.ht < 575) || (t.nlep == 2 && t.zll_ht < 575));
      float actual_HEM_ptCut = isLowHT ? 20.0 : HEM_ptCut;
      if((t.isData && t.run >= HEM_startRun) || (!t.isData && t.evt % HEM_fracDen < HEM_fracNum)){ 
	for(int i=0; i<t.njet; i++){
	  if(t.jet_pt[i] < actual_HEM_ptCut)
	    break;
	  if(t.jet_eta[i] > HEM_region[0] && t.jet_eta[i] < HEM_region[1] && 
	     t.jet_phi[i] > HEM_region[2] && t.jet_phi[i] < HEM_region[3])
	    hasHEMjet = true;
	}
	for (int i=0; i<t.ntracks; i++){
	  // Is the track in the HEM region?
	  if ( !(t.track_eta[i] > HEM_region[0] && t.track_eta[i] < HEM_region[1] &&
		 t.track_phi[i] > HEM_region[2] && t.track_phi[i] < HEM_region[3]))
	    continue;
	  // Found a track in the HEM region. Is is high enough quality to veto the event?
	  // if (!t.track_isHighPurity[i]) continue;
	  if (fabs(t.track_dz[i]) > 0.20 || fabs(t.track_dxy[i]) > 0.10) continue;
	  // if (t.track_nPixelLayersWithMeasurement[i] < 3) continue;
	  // if (t.track_nLostInnerPixelHits[i] > 0) continue;
	  // if (t.track_nLostOuterHits[i] > 2) continue;
	  // May wish to add this guy
	  //if (t.track_ptErr[i_trk] / (t.track_pt[i_trk]*t.track_pt[i_trk]) > 0.02) continue;
	  lostHEMtrackPt += t.track_pt[i];
	}
      }
      // May wish to set lostHEMtrackPt threshold to some higher value; this is equivalent to "any lost track" (saving only pT > 15 GeV tracks in babies for now)
      if(hasHEMjet || (isLowHT && lostHEMtrackPt > 0)){// cout << endl << "SKIPPED HEM EVT: " << t.run << ":" << t.lumi << ":" << t.evt << endl;
	if (verbose) cout << "Failed HEM" << endl;
	continue;
      }
    }

    if (unlikely(t.nJet30FailId != 0)) {
      if (verbose) cout << "Failed JetID" << endl;
      continue;
    }

    if ( t.nJet30 == 1 && !(isSignal || (t.jet_id[0] >= 4)) ) {
      if (verbose) cout << "Failed Monojet" << endl;
      continue;
    }
    if (isSignal && t.nJet20BadFastsim > 0) {
      if (verbose) cout << "Bad Fastsim Jet" << endl;
      continue;
    }
    if (unlikely(t.met_miniaodPt / t.met_caloPt >= 5.0)) {
      if (verbose) cout << "Failed DiffMet" << endl;
      continue;
    }
    if (unlikely(t.nJet200MuFrac50DphiMet > 0)) {
      if (verbose) cout << "Failed Ad Hoc Mu Filter" << endl;
      continue;
    }    
    
    const float met = doNoLeps ? t.met_pt : t.rl_met_pt;//t.zll_met_pt;
    const float met_phi = doNoLeps ? t.met_phi : t.rl_met_phi;//t.zll_met_phi;
    const float mt2 = doNoLeps ? t.mt2 : t.rl_mt2;//t.zll_mt2;
    const float ht = doNoLeps ? t.ht : t.rl_ht;//t.zll_ht;
    const float diffMet = doNoLeps ? t.diffMetMht / t.met_pt : t.rl_diffMetMht / t.rl_met_pt;//t.zll_diffMetMht / t.zll_met_pt;
    const float deltaPhiMin = doNoLeps ? t.deltaPhiMin : t.rl_deltaPhiMin;//t.zll_deltaPhiMin;

    const bool lepveto = doNoLeps ? t.nMuons10 + t.nElectrons10 + t.nPFLep5LowMT + t.nPFHad10LowMT > 0 : t.nLepLowMT == 1; //t.nlep != 2;
      
    int nLongCharginos = 0;
    for (int i_ch = 0; i_ch < t.nCharginos; i_ch++) {
      if (t.chargino_decayXY[i_ch] >= 490 || fabs(t.chargino_decayZ[i_ch]) >= 820) nLongCharginos++;
    }

    if (lepveto || (nLongCharginos > 0 && config_.year != 2016)) {
      if (verbose) cout << "Failed Lep Veto" << endl;
      continue;
    }
      
    bool passMonojet = met >= 200 && t.jet1_pt >= 200 && ht >= 200 && t.nJet30 == 1 && diffMet < 0.5;
    bool passMultijet = diffMet < 0.5 && t.nJet30 > 1 && ht >= 250 && mt2 >= 60 && met >= 30 && deltaPhiMin > 0.3;

    if (applyKinematicPreselection && !(passMonojet || passMultijet)) {
      if (verbose) cout << "Failed Kinematic Preselection" << endl;
      continue;
    }

    if (t.isData) {
      //const bool passPrescaleTrigger = passTrigger(t, trigs_prescaled_);
      const bool passSRTrigger = passTrigger(t, trigs_SR_);
      //    const bool passDilepTrigger = passTrigger(t, trigs_doubleLep_);
      const bool passMonolepTrigger = passTrigger(t, trigs_singleLep_);
      //    const bool passTrigger = passPrescaleTrigger || passSRTrigger;
      const bool passTrigger = doNoLeps ? passSRTrigger : passSRTrigger || passMonolepTrigger;//passDilepTrigger;
      
      if (!passTrigger) {
	if (verbose) cout << "Failed Trigger" << endl;
	continue;
      }
    }

    float weight = t.isData ? 1.0 : (t.evt_scale1fb == 1 ? 1.8863e-06 : t.evt_scale1fb) * lumi; // manually correct high HT WJets

    if (isSignal) {
      int bin = h_xsec->GetXaxis()->FindBin(t.GenSusyMScan1);      
      weight = h_xsec->GetBinContent(bin) * 1000 * lumi / nEventsTree;
    }

    if (!t.isData) {
      /*
      if (isSignal_ && applyISRWeights) {
        // Not processing signal for now
	int binx = h_sig_avgweight_isr_->GetXaxis()->FindBin(t.GenSusyMScan1);
	int biny = h_sig_avgweight_isr_->GetYaxis()->FindBin(t.GenSusyMScan2);
	float avgweight_isr = h_sig_avgweight_isr_->GetBinContent(binx,biny);
	weight_ *= t.weight_isr / avgweight_isr;
      }
      else if (applyISRWeights && t.evt_id >= 301 && t.evt_id <= 303) { // ttbar
 	float avgweight_isr = getAverageISRWeight(t.evt_id, 0);
	weight *= t.weight_isr / avgweight_isr;
      }
      */

      if (applyISRWeights && (tag == "ttsl" || tag == "ttdl")) {
	weight *= t.weight_isr / getAverageISRWeight(outtag,config_tag);
      }

      if(applyL1PrefireWeights && (config_.year==2016 || config_.year==2017) && config_.cmssw_ver!=80) {
	weight *= t.weight_L1prefire;
      }
      //      weight *= t.weight_btagsf;

      if (doNTrueIntReweight) {
	if(h_nTrueInt_weights_==0){
	  cout << "[FshortLooper::loop] WARNING! You requested to do nTrueInt reweighting but didn't supply a PU weights file. Turning off." << endl;
	}else{
	  int nTrueInt_input = t.nTrueInt;
	  float puWeight = h_nTrueInt_weights_->GetBinContent(h_nTrueInt_weights_->FindBin(nTrueInt_input));
	  weight *= puWeight;
	}
      }

      if (!t.isData && !doNoLeps && applyDileptonTriggerWeights){
	weight *= getDileptonTriggerWeight(t.lep_pt[0], t.lep_pdgId[0], t.lep_pt[1], t.lep_pdgId[1], 0);
      }

      if (verbose) cout << weight << endl;

      if (skipHighWeights && weight > 1.0) continue; // skip high weighted events

    }

    if (weight < 0.0) {
      cout << "Negative weight: " << weight << ", " << t.run << ":" << t.lumi << ":" << t.evt << endl;
    }

    // Turn off monojet
    if (t.nJet30 <= 1) continue;

    // Fully inclusive pileup distribution
    h_nv_fullincl->Fill(t.nVert-1,weight);

    vector<TH2D*> histsToFill; 
    vector<string> regsToFill;
    vector<string> njhtToFill;

    if (passMultijet || !applyKinematicPreselection) {
      regsToFill.push_back("Incl");
      njhtToFill.push_back("");
      // Measurement Region
      if (mt2 < 100) {
	regsToFill.push_back("MR");
	histsToFill.push_back(fsHists["h_fsMR_Baseline"]);
	ht < 1200 ? histsToFill.push_back(fsHists["h_fsMR_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsMR_gt1200HT"]);
	ht < 450 ? histsToFill.push_back(fsHists["h_fsMR_HT250"]) : histsToFill.push_back(fsHists["h_fsMR_HT450"]);
	if (met < 100) histsToFill.push_back(fsHists["h_fsMR_MET30"]);
	else if (met < 250) histsToFill.push_back(fsHists["h_fsMR_MET100"]);
	else histsToFill.push_back(fsHists["h_fsMR_MET250"]);
	if (met >= 100 && ht >= 450) histsToFill.push_back(fsHists["h_fsMR_HT450MET100"]);
	if (t.nJet30 < 4) {
	  histsToFill.push_back(fsHists["h_fsMR_23_Baseline"]);
	  ht < 1200 ? histsToFill.push_back(fsHists["h_fsMR_23_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsMR_23_gt1200HT"]);
	  ht < 450 ? histsToFill.push_back(fsHists["h_fsMR_23_HT250"]) : histsToFill.push_back(fsHists["h_fsMR_23_HT450"]);
	  if (met < 100) histsToFill.push_back(fsHists["h_fsMR_23_MET30"]);
	  else if (met < 250) histsToFill.push_back(fsHists["h_fsMR_23_MET100"]);
	  else histsToFill.push_back(fsHists["h_fsMR_23_MET250"]);
	  if (met >= 100 && ht >= 450) histsToFill.push_back(fsHists["h_fsMR_23_HT450MET100"]);
	} else {
	  histsToFill.push_back(fsHists["h_fsMR_4_Baseline"]);
	  ht < 1200 ? histsToFill.push_back(fsHists["h_fsMR_4_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsMR_4_gt1200HT"]);
	  ht < 450 ? histsToFill.push_back(fsHists["h_fsMR_4_HT250"]) : histsToFill.push_back(fsHists["h_fsMR_4_HT450"]);
	  if (met < 100) histsToFill.push_back(fsHists["h_fsMR_4_MET30"]);
	  else if (met < 250) histsToFill.push_back(fsHists["h_fsMR_4_MET100"]);
	  else histsToFill.push_back(fsHists["h_fsMR_4_MET250"]);
	  if (met >= 100 && ht >= 450) histsToFill.push_back(fsHists["h_fsMR_4_HT450MET100"]);
	}
      }
      else if (mt2 < 200 && mt2 >= 100) {
	regsToFill.push_back("VR");
	histsToFill.push_back(fsHists["h_fsVR_Baseline"]);
	ht < 1200 ? histsToFill.push_back(fsHists["h_fsVR_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsVR_gt1200HT"]);
	ht < 450 ? histsToFill.push_back(fsHists["h_fsVR_HT250"]) : histsToFill.push_back(fsHists["h_fsVR_HT450"]);
	if (met < 100) histsToFill.push_back(fsHists["h_fsVR_MET30"]);
	else if (met < 250) histsToFill.push_back(fsHists["h_fsVR_MET100"]);
	else histsToFill.push_back(fsHists["h_fsVR_MET250"]);
	if (met >= 100 && ht >= 450) histsToFill.push_back(fsHists["h_fsVR_HT450MET100"]);
	if (t.nJet30 < 4) {
	  histsToFill.push_back(fsHists["h_fsVR_23_Baseline"]);
	  ht < 1200 ? histsToFill.push_back(fsHists["h_fsVR_23_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsVR_23_gt1200HT"]);
	  ht < 450 ? histsToFill.push_back(fsHists["h_fsVR_23_HT250"]) : histsToFill.push_back(fsHists["h_fsVR_23_HT450"]);
	  if (met < 100) histsToFill.push_back(fsHists["h_fsVR_23_MET30"]);
	  else if (met < 250) histsToFill.push_back(fsHists["h_fsVR_23_MET100"]);
	  else histsToFill.push_back(fsHists["h_fsVR_23_MET250"]);
	  if (met >= 100 && ht >= 450) histsToFill.push_back(fsHists["h_fsVR_23_HT450MET100"]);
	} else {
	  histsToFill.push_back(fsHists["h_fsVR_4_Baseline"]);
	  ht < 1200 ? histsToFill.push_back(fsHists["h_fsVR_4_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsVR_4_gt1200HT"]);
	  ht < 450 ? histsToFill.push_back(fsHists["h_fsVR_4_HT250"]) : histsToFill.push_back(fsHists["h_fsVR_4_HT450"]);
	  if (met < 100) histsToFill.push_back(fsHists["h_fsVR_4_MET30"]);
	  else if (met < 250) histsToFill.push_back(fsHists["h_fsVR_4_MET100"]);
	  else histsToFill.push_back(fsHists["h_fsVR_4_MET250"]);
	  if (met >= 100 && ht >= 450) histsToFill.push_back(fsHists["h_fsVR_4_HT450MET100"]);
	}
      }
      // Signal Region
      else if ( !(blind && t.isData && doNoLeps) ) { // Only look at MT2 > 200 GeV in data if unblinded or looking in lepton CR
	if (met > 250 || (ht > 1200 && met > 30)) regsToFill.push_back("SR"); // Apply full selection for cut hists for SR points
	histsToFill.push_back(fsHists["h_fsSR_Baseline"]);
	ht < 1200 ? histsToFill.push_back(fsHists["h_fsSR_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsSR_gt1200HT"]);
	ht < 450 ? histsToFill.push_back(fsHists["h_fsSR_HT250"]) : histsToFill.push_back(fsHists["h_fsSR_HT450"]);
	if (met < 100) histsToFill.push_back(fsHists["h_fsSR_MET30"]);
	else if (met < 250) histsToFill.push_back(fsHists["h_fsSR_MET100"]);
	else histsToFill.push_back(fsHists["h_fsSR_MET250"]);
	if (met >= 100 && ht >= 450) histsToFill.push_back(fsHists["h_fsSR_HT450MET100"]);
	if (t.nJet30 < 4) {
	  histsToFill.push_back(fsHists["h_fsSR_23_Baseline"]);
	  ht < 1200 ? histsToFill.push_back(fsHists["h_fsSR_23_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsSR_23_gt1200HT"]);
	  ht < 450 ? histsToFill.push_back(fsHists["h_fsSR_23_HT250"]) : histsToFill.push_back(fsHists["h_fsSR_23_HT450"]);
	  if (met < 100) histsToFill.push_back(fsHists["h_fsSR_23_MET30"]);
	  else if (met < 250) histsToFill.push_back(fsHists["h_fsSR_23_MET100"]);
	  else histsToFill.push_back(fsHists["h_fsSR_23_MET250"]);
	  if (met >= 100 && ht >= 450) histsToFill.push_back(fsHists["h_fsSR_23_HT450MET100"]);
	} else {
	  histsToFill.push_back(fsHists["h_fsSR_4_Baseline"]);
	  ht < 1200 ? histsToFill.push_back(fsHists["h_fsSR_4_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsSR_4_gt1200HT"]);
	  ht < 450 ? histsToFill.push_back(fsHists["h_fsSR_4_HT250"]) : histsToFill.push_back(fsHists["h_fsSR_4_HT450"]);
	  if (met < 100) histsToFill.push_back(fsHists["h_fsSR_4_MET30"]);
	  else if (met < 250) histsToFill.push_back(fsHists["h_fsSR_4_MET100"]);
	  else histsToFill.push_back(fsHists["h_fsSR_4_MET250"]);
	  if (met >= 100 && ht >= 450) histsToFill.push_back(fsHists["h_fsSR_4_HT450MET100"]);
	}
      }
      // Should only get here if looking at data while blinded
      else {
	continue; // No need to consider this event if not filling any hists
      }
    }
    // Monojet
    else {
      // MR
      if (t.jet1_pt < 275 && met > 200 && ht > 200) { // relax the MET/HT cuts for the measurement region
	histsToFill.push_back(fsHists["h_fs_1_MR_Baseline"]);	  
	if (t.jet1_pt < 250) histsToFill.push_back(fsHists["h_fs_1_MR_pt200"]);	  
	else histsToFill.push_back(fsHists["h_fs_1_MR_pt250"]);	  
	if (met > 250 && ht > 250) histsToFill.push_back(fsHists["h_fs_1_MR_MetHt250"]);	  
	if (met > 250 && ht > 250 && t.jet1_pt > 250) histsToFill.push_back(fsHists["h_fs_1_MR_MetHtPt250"]);	  
      }
      else if (t.jet1_pt < 350 && met > 250 && ht > 250) {
	  histsToFill.push_back(fsHists["h_fs_1_VR"]);
	}
      // SR 
      else if (t.jet1_pt > 350 && met > 250 && ht > 250 && !(blind && t.isData)) {
	histsToFill.push_back(fsHists["h_fs_1_SR"]);
      }
      else {
	continue; // to get here, must be blinded and in data, or see a weird "monojet" event where jet1 pt and met/ht are very different
      }
    }

    int match_track_index_1 = -1; int match_track_index_2 = -1;
    for (int i_chargino = 0; i_chargino < t.nCharginos; i_chargino++) {
      h_CharLength->Fill( t.chargino_decayXY[i_chargino], weight );
      const float char_phi = t.chargino_phi[i_chargino];
      const float char_eta = t.chargino_eta[i_chargino];
      bool chargino_calosel = InEtaPhiVetoRegion(char_eta,char_phi,year) == 0;
      if (chargino_calosel) {
	h_CharLength_etaphi->Fill( t.chargino_decayXY[i_chargino], weight );
	float chargino_pt = t.chargino_pt[i_chargino];
	if (t.chargino_decayXY[i_chargino] > 10 && t.chargino_decayXY[i_chargino] < 100 && chargino_pt > 15) {
	  if (chargino_pt < 50) {
	    denomChargino15++;
	  }
	  else {
	    denomChargino50++;
	  }
	  if (match_track_index_1 >= 0) match_track_index_1 = t.chargino_matchedTrackIdx[i_chargino];
	  else match_track_index_2 = t.chargino_matchedTrackIdx[i_chargino];
	}
      }
    }

    const int ntracks = t.ntracks;
    bool overlapping_track[ntracks];
    for (int i_trk = 0; i_trk < ntracks; i_trk++) {
      overlapping_track[i_trk] = false;
    }
    for (int i_trk = 0; i_trk < ntracks; i_trk++) {
      for (int j_trk = i_trk + 1; j_trk < ntracks; j_trk++) {
	if (DeltaR(t.track_eta[i_trk],t.track_eta[j_trk],t.track_phi[i_trk],t.track_phi[j_trk]) < 0.1) {
	  overlapping_track[i_trk] = true;
	  overlapping_track[j_trk] = true;
	}
      }
    }
    for (int i_trk = 0; i_trk < ntracks; i_trk++) {   

      if (overlapping_track[i_trk]) {
	if (verbose) cout << "Overlapping" << endl;
	continue; // veto any tracks closely overlapping other lost tracks (pt > 15 GeV, with iso cut for 15-20 GeV)
      }

      bool PassesFullIsoSel = false; bool PassesFullIsoSelSTC = false;
      bool isQualityTrack = false; bool isQualityTrackSTC = false;

      // Apply basic selections
      if (applyCaloSel) {
	const bool CaloSel = !(t.track_DeadECAL[i_trk] || t.track_DeadHCAL[i_trk]) && InEtaPhiVetoRegion(t.track_eta[i_trk],t.track_phi[i_trk],year) == 0;
	if (!CaloSel) {
	  if (verbose) cout << "Failed CaloSel" << endl;
	  continue;
	}
      }

      int lenIndex = -1;
      bool isChargino = isSignal && t.track_matchedCharginoIdx[i_trk] >= 0;
      if (isSignal && !isChargino) {
	for (int i_chargino = 0; i_chargino < t.nCharginos; i_chargino++) {
	  if (DeltaR(t.track_eta[i_trk],t.chargino_eta[i_chargino],t.track_phi[i_trk],t.chargino_phi[i_chargino]) < 0.1) {
	    isChargino = true;
	    break;
	  }
	}
      }

      string ptstring = t.track_pt[i_trk] < 50 ? "_lowpt" : "_hipt";
      bool isST = false, isSTC = false;//, isST1 = false, isSTC1 = false;
      if (recalculate) {

	const float pt = t.track_pt[i_trk];
	const bool ptSel = pt >= 15.0;
	const bool etaSel = fabs(t.track_eta[i_trk]) <= 2.4;
	const bool BaseSel = ptSel && etaSel;
	
	if (!BaseSel || (isSignal && t.track_isBadCharginoTrack[i_trk])) {
	  if (isChargino) {
	    cout << "Chargino failed base selection" << endl;
	  }
	  if (verbose) cout << "Failed base selection" << endl;	  
	  continue;
	}

	// Length and categorization
	const bool lostOuterHitsSel = t.track_nLostOuterHits[i_trk] >= 2;
	const int nLayers = t.track_nLayersWithMeasurement[i_trk];
	const bool isP = nLayers == t.track_nPixelLayersWithMeasurement[i_trk] && lostOuterHitsSel && nLayers >= (increment17 ? (year == 2016 ? 3 : 4) : 3);
	const bool isLong = nLayers >= 7;
	const bool isM = nLayers != t.track_nPixelLayersWithMeasurement[i_trk] && !isLong && lostOuterHitsSel;
	const bool isL = isLong && lostOuterHitsSel;
	const bool isShort = isP || isM || isL;

	if (!isShort) {
	  if (isChargino) cout << "Chargino failed shortness selection. Layers: " << nLayers << ", MOH: " << t.track_nLostOuterHits[i_trk] << endl;
	  if (verbose) cout << "Failed shortness selection" << endl;	  
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
	    if (verbose) cout << "Failed reco veto" << endl;	  
	    continue;
	  }	  
	}
	else if (applyRecoVeto == 2) {
	  const bool PassesRecoVeto = t.track_recoveto[i_trk] == 0;
	  if (!PassesRecoVeto) {
	    if (isChargino) cout << "Chargino failed reco veto, " << t.track_recoveto[i_trk] << endl;
	    if (verbose) cout << "Failed reco veto" << endl;	  
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
	PassesFullIsoSel = NeuIso0p05Sel && NeuRelIso0p05Sel && isoSel && relisoSel;
	PassesFullIsoSelSTC = NeuIso0p05SelSTC && NeuRelIso0p05SelSTC && isoSelSTC && relisoSelSTC;

	if (!fillNM1Hists && !PassesFullIsoSelSTC) {
	  if (isChargino) cout << "Chargino failed isolation" << endl;
	  if (verbose) cout << "Failed iso selection" << endl;	  
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
	isQualityTrack = pixLayersSel4 && QualityTrackBase;
	isQualityTrackSTC = pixLayersSel4 && QualityTrackSTCBase;

	if (!fillNM1Hists && !isQualityTrackSTC) {
	  if (isChargino) cout << "Chargino failed quality" << endl;
	  if (verbose) cout << "Failed quality selection" << endl;	  
	  continue;
	}

	// Full Short Track
	isST = PassesFullIsoSel && isQualityTrack;

	// Candidate (loosened isolation, quality)...already checked for Iso and Quality above
	isSTC = !isST && PassesFullIsoSelSTC && isQualityTrackSTC;


	if (isST && (i_trk == match_track_index_1 || i_trk == match_track_index_2)) {
	  bool loPt = t.track_pt[i_trk] < 50;
	  if (isP) {
	    loPt ? chSTp15++ : chSTp50++;
	  }
	  else if (isM) {
	    loPt ? chSTm15++ : chSTm50++;
	  }
	  else {
	    loPt ? chSTl15++ : chSTl50++;
	  }
	}

	/*
	if (isSTC || isST) {	  
	  if (isST) cout << "Found " << (t.track_isChargino[i_trk] ? "Chargino " : "") << "ST: " << nLayers << endl;
	  else {
	    cout << "Found " << (t.track_isChargino[i_trk] ? "Chargino " : "") << "STC: " << nLayers << endl;
	    cout << (PassesFullIsoSel ? "Passes Iso" : "Fails Iso") << " and " << (isQualityTrack ? "Passes Quality" : "Fails Quality") << endl;
	    if (!PassesFullIsoSel) {
	      cout << "neu: " << niso << ", nrel: " << nreliso << ", abs: " << iso << ", rel: " << reliso << endl;
	    } 
	    if (!isQualityTrack) {
	      cout << "dxy: " << t.track_dxy[i_trk] << ", dz: " << t.track_dz[i_trk] << ", pterr/pt^2: " << pterrOverPt2 << endl;
	    }
	  }
	  cout << "Fill Index: " << lenIndex << endl;
	}
	*/
	
      }
      else {
	if (t.track_iscandidate[i_trk]) {
	  isSTC = true;
	  lenIndex = t.track_ispixelonlycandidate[i_trk] ? 1 : (t.track_ismediumcandidate[i_trk] ? 2 : 3);	  
	}
	else if (t.track_isshort[i_trk]) {
	  isST = true;
	  lenIndex = t.track_ispixelonly[i_trk] ? 1 : (t.track_ismedium[i_trk] ? 2 : 3);
	}
      }

      if (isChargino) {
	if (isST) {
	  h_sigpassST->Fill( t.chargino_decayXY[t.track_matchedCharginoIdx[i_trk]], weight);
	}
	else if (isSTC) {
	  h_sigpassSTC->Fill( t.chargino_decayXY[t.track_matchedCharginoIdx[i_trk]], weight);
	}
      }

      vector<string> matches = {"_"};
      // Check gen match
      if (!t.isData) {
	if (isChargino) {
	  matches.push_back("_chargino_");
	}
	else if (doMatching) {
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
	  if (genPromptE)      matches.push_back("_ewke_");
	  else if (genPromptM) matches.push_back("_ewkm_");
	  else if (genTauE)    matches.push_back("_tewke_");
	  else if (genTauM)    matches.push_back("_tewkm_");
	  else if (genTau1)    matches.push_back("_1newk_");
	  else if (genTau3)    matches.push_back("_3newk_");
	  else                 matches.push_back("_newk_");
	}
      }

      // One final cut for L tracks to catch lost leptons
      // These tracks are probably background, likely a lost muon from W+jets
      // in this case, just fill the mtpt hists and continue
      const float mt = MT( t.track_pt[i_trk], t.track_phi[i_trk], met, met_phi );
      bool rejected = (t.track_pt[i_trk] < 150 && mt < 100);

      if (verbose && isST && lenIndex == 3) {
	if (rejected) {
	  cout << "MtPt rejected" << endl;	  
	}
	else {
	  cout << "L ST: MT2 = " << t.mt2 << " met " << t.met_pt << " nj " << t.nJet30 << " ht " << t.ht << endl;
	}
      }

      vector<int> fillIndices; vector<string> lengths;
      lengths.push_back("All");
      if (lenIndex == 1) {
	fillIndices.push_back(1);
	lengths.push_back("P");
	if (t.track_nLayersWithMeasurement[i_trk] == 3) {
	  fillIndices.push_back(2);
	  lengths.push_back("P3");
	}
	else {
	  fillIndices.push_back(3);
	  lengths.push_back("P4");
	}
      }
      else if (lenIndex == 2) {
	fillIndices.push_back(4);
	lengths.push_back("M");
      }
      else { // lenIndex == 3
	if (!rejected) {
	  fillIndices.push_back(5);
	  if (isST) cout << "L ST with weight " << weight << " " << t.run << ":" << t.lumi << ":" << t.evt << endl;
	  lengths.push_back("L");
	} else {
	  fillIndices.push_back(6);
	  lengths.push_back("Lrej");
	}
      }
      
      /*
      if (isST && lenIndex == 3 && mt2 > 100 && mt2 < 200 && t.nJet30 > 3 && ht < 1000) {
	cout << "L HL VR event: " << t.run << " && " << t.lumi << " && " << t.evt << endl;
      }
      */

      const float dphiMet = DeltaPhi(t.track_phi[i_trk],met_phi);
      const float met_x = cos(met_phi) * met;
      const float met_y = sin(met_phi) * met;
      const float trk_x = cos(t.track_phi[i_trk]) * t.track_pt[i_trk];
      const float trk_y = sin(t.track_phi[i_trk]) * t.track_pt[i_trk];
      const float adj_x = met_x - trk_x;
      const float adj_y = met_y - trk_y;
      const float adjMet = sqrt( adj_x*adj_x + adj_y*adj_y );
      
      // Fills
      if (fillNM1Hists && passMultijet) {
	for (vector<string>::iterator length = lengths.begin(); length != lengths.end(); length++) {
	  for (vector<string>::iterator match = matches.begin(); match != matches.end(); match++) {
	    for (vector<string>::iterator njht = njhtToFill.begin(); njht != njhtToFill.end(); njht++) {
	      for (vector<string>::iterator reg = regsToFill.begin(); reg != regsToFill.end(); reg++) {
		string suffixST = (*reg)+(*match)+"ST_"+(*length)+(*njht)+"_NM1";
		string suffixSTC = (*reg)+(*match)+"STC_"+(*length)+(*njht)+"_NM1";
		if (PassesFullIsoSelSTC) { // Fill NM1 quality hists
		  string suffix = PassesFullIsoSel ? suffixST : suffixSTC;
		  // Dz
		  string hname = "h_dz_"+suffix;
		  cutHistsNM1[hname]->Fill(fabs(t.track_dz[i_trk]),weight);
		  // Dxy
		  hname = "h_dxy_"+suffix;
		  cutHistsNM1[hname]->Fill(fabs(t.track_dxy[i_trk]),weight);
		  // pterr / pt^2
		  hname = "h_pterr_"+suffix;
		  cutHistsNM1[hname]->Fill(t.track_ptErr[i_trk]/(t.track_pt[i_trk]*t.track_pt[i_trk]),weight);
		}
		if (isQualityTrackSTC) { // Fill NM1 iso hists
		  string suffix = isQualityTrack ? suffixST : suffixSTC;
		  // Abs Iso 0.3
		  string hname = "h_iso_"+suffix;
		  cutHistsNM1[hname]->Fill(t.track_iso[i_trk],weight);
		  // Rel Iso 0.3
		  hname = "h_reliso_"+suffix;
		  cutHistsNM1[hname]->Fill(t.track_reliso[i_trk],weight);
		  // N Iso 0.05
		  hname = "h_niso_"+suffix;
		  cutHistsNM1[hname]->Fill(t.track_neuIso0p05[i_trk],weight);
		  // N RelIso 0.05
		  hname = "h_nreliso_"+suffix;
		  cutHistsNM1[hname]->Fill(t.track_neuRelIso0p05[i_trk],weight);
		}
	      }
	    }
	  }
	}
      }

      if (!isST && !isSTC) {
	if (isChargino) {
	  cout << "Chargino failed selection" << endl;
	}
	continue;
      }

      // Monojet distribution checks
      if (t.nJet30 == 1) {
	bool fillMR = histsToFill[0] == fsHists["h_fs_1_MR_Baseline"];	  
	bool fillVR = histsToFill[0] == fsHists["h_fs_1_VR"];	  
	bool fillSR = histsToFill[0] == fsHists["h_fs_1_SR"];	  
	if (isST) {
	  if (fillMR) {
	    h_jet1_phi_MR_ST->Fill(t.jet_phi[0],weight);
	    h_jet1_eta_MR_ST->Fill(t.jet_eta[0],weight);
	  }
	  else if (fillVR) {
	    h_jet1_phi_VR_ST->Fill(t.jet_phi[0],weight);
	    h_jet1_eta_VR_ST->Fill(t.jet_eta[0],weight);
	  }
	  else if (fillSR) {
	    h_jet1_phi_SR_ST->Fill(t.jet_phi[0],weight);
	    h_jet1_eta_SR_ST->Fill(t.jet_eta[0],weight);
	  }
	}
	else if (isSTC) {
	  if (fillMR) {
	    h_jet1_phi_MR_STC->Fill(t.jet_phi[0],weight);
	    h_jet1_eta_MR_STC->Fill(t.jet_eta[0],weight);
	  }
	  else if (fillVR) {
	    h_jet1_phi_VR_STC->Fill(t.jet_phi[0],weight);
	    h_jet1_eta_VR_STC->Fill(t.jet_eta[0],weight);
	  }
	  else if (fillSR) {
	    h_jet1_phi_SR_STC->Fill(t.jet_phi[0],weight);
	    h_jet1_eta_SR_STC->Fill(t.jet_eta[0],weight);
	  }
	}

      }

      // If we want to find fshort for matched chargino tracks and this is not one, then skip this fill
      if (!(onlyMatchedTracks && !isChargino)) {
	vector<TH2D*> ptHistsToFill; 
	for (vector<TH2D*>::iterator hist = histsToFill.begin(); hist != histsToFill.end(); hist++) {
	  string hname = string((*hist)->GetName())+ptstring;
	  ptHistsToFill.push_back(fsHists[hname]);
	}
	int fillRow = isST ? 1 : 2; unordered_map<TH2D*,TH1D*> max_weights = isST ? max_st_weights : max_stc_weights;      
	for (std::vector<int>::iterator fillIndex = fillIndices.begin(); fillIndex != fillIndices.end(); fillIndex++) {
	  FillHists(histsToFill,weight,fillRow,*fillIndex);
	  FillHists(ptHistsToFill,weight,fillRow,*fillIndex);
	  // keep track of max weight we've filled for STC
	  for (std::vector<TH2D*>::iterator fshist = histsToFill.begin(); fshist != histsToFill.end(); fshist++) {
	    TH2D* fsh = *fshist;	
	    float max_weight = max_weights[fsh]->GetBinContent(*fillIndex);
	    if (weight > max_weight) max_weights[fsh]->SetBinContent(*fillIndex,weight);
	  }
	}
      }

      if (fillCutHists && passMultijet) {
	string category = isST ? "ST" : "STC";
	for (vector<string>::iterator length = lengths.begin(); length != lengths.end(); length++) {
	  for (vector<string>::iterator match = matches.begin(); match != matches.end(); match++) {
	    for (vector<string>::iterator njht = njhtToFill.begin(); njht != njhtToFill.end(); njht++) {
	      for (vector<string>::iterator reg = regsToFill.begin(); reg != regsToFill.end(); reg++) {
		
		string suffix = (*reg)+(*match)+category+"_"+(*length)+(*njht);
		// MtPt
		string hname = "h_mtpt_"+suffix;
		mtptHists[hname]->Fill(mt,t.track_pt[i_trk],weight);
		// MtMET
		hname = "h_mtmet_"+suffix;
		mtmetHists[hname]->Fill(mt,met,weight);
		// MT2 MET
		hname = "h_mt2met_"+suffix;
		mt2metHists[hname]->Fill(mt2,met,weight);
		// MtMT2
		hname = "h_mtmt2_"+suffix;
		mtmt2Hists[hname]->Fill(mt,mt2,weight);
		// EtaPhi
		hname = "h_etaphi_"+suffix;
		etaphiHists[hname]->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight);
		// Dz
		hname = "h_dz_"+suffix;
		cutHists[hname]->Fill(fabs(t.track_dz[i_trk]),weight);
		// Dxy
		hname = "h_dxy_"+suffix;
		cutHists[hname]->Fill(fabs(t.track_dxy[i_trk]),weight);
		// pterr / pt^2
		hname = "h_pterr_"+suffix;
		cutHists[hname]->Fill(t.track_ptErr[i_trk]/(t.track_pt[i_trk]*t.track_pt[i_trk]),weight);
		// Abs Iso 0.3
		hname = "h_iso_"+suffix;
		cutHists[hname]->Fill(t.track_iso[i_trk],weight);
		// Rel Iso 0.3
		hname = "h_reliso_"+suffix;
		cutHists[hname]->Fill(t.track_reliso[i_trk],weight);
		// N Iso 0.05
		hname = "h_niso_"+suffix;
		cutHists[hname]->Fill(t.track_neuIso0p05[i_trk],weight);
		// N RelIso 0.05
		hname = "h_nreliso_"+suffix;
		cutHists[hname]->Fill(t.track_neuRelIso0p05[i_trk],weight);
		// Pt
		hname = "h_pt_"+suffix;
		cutHists[hname]->Fill(t.track_pt[i_trk],weight);
		// Eta
		hname = "h_eta_"+suffix;
		cutHists[hname]->Fill(fabs(t.track_eta[i_trk]),weight);
		// Mt
		hname = "h_mt_"+suffix;
		cutHists[hname]->Fill(mt,weight);
		// MT2
		hname = "h_mt2_"+suffix;
		cutHists[hname]->Fill(mt2,weight);
		// MET
		hname = "h_met_"+suffix;
		cutHists[hname]->Fill(met,weight);
		// HT
		hname = "h_ht_"+suffix;
		cutHists[hname]->Fill(ht,weight);
		// NJ
		hname = "h_nj_"+suffix;
		cutHists[hname]->Fill(t.nJet30,weight);
		// N layer
		hname = "h_nlayer_"+suffix;
		cutHists[hname]->Fill(t.track_nLayersWithMeasurement[i_trk],weight);
		// dphi Met
		hname = "h_dphiMet_"+suffix;
		cutHists[hname]->Fill(dphiMet,weight);
		// adj Met
		hname = "h_adjMet_"+suffix;
		cutHists[hname]->Fill(adjMet,weight);
		// delta Met
		hname = "h_deltaMet_"+suffix;
		cutHists[hname]->Fill((met - adjMet)/met,weight);
		// N vertex
		hname = "h_nv_"+suffix;
		cutHists[hname]->Fill(t.nVert,weight);
		if (fillUnimportantCutHists) {
		  // Ntag
		  hname = "h_nb_"+suffix;
		  cutHists[hname]->Fill(t.nBJet20,weight);
		  // Hit Pattern
		  hname = "h_hp_"+suffix;
		  cutHists[hname]->Fill(t.track_HitSignature[i_trk] & ((1<<7)-1),weight); // Mask so we save only first 6 hits
		}
	      }
	    }
	  }
	}
      }
    } // Track loop
    
  }//end loop on events in a file

  cout << "15 to 50 GeV charginos/tracks:" << endl;
  cout << "P: " << chSTp15 << " / " << denomChargino15 << endl;
  cout << "M: " << chSTm15 << " / " << denomChargino15 << endl;
  cout << "L: " << chSTl15 << " / " << denomChargino15 << endl;
  cout << "> 50 GeV charginos/tracks:" << endl;
  cout << "P: " << chSTp50 << " / " << denomChargino50 << endl;
  cout << "M: " << chSTm50 << " / " << denomChargino50 << endl;
  cout << "L: " << chSTl50 << " / " << denomChargino50 << endl;

    
  // Post-processing

  // Chargino efficiency division, need the complex divide to get binomial errors
  h_sigeffST->Divide(h_sigpassST,h_CharLength,1,1,"B");
  h_sigeffSTC->Divide(h_sigpassSTC,h_CharLength,1,1,"B");

  // Calculate Fshort for this particular sample
  for (unordered_map<string,TH2D*>::iterator hist = fsHists.begin(); hist != fsHists.end(); hist++) {
    TH2D* h = hist->second;
    for (int len = 1; len <= h->GetNbinsX(); len++) {
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
  for (unordered_map<string,TH2D*>::iterator hist = fsHists.begin(); hist != fsHists.end(); hist++) {
    TH2D* h = hist->second;
    h->Write();
    max_st_weights[h]->Write();
    max_stc_weights[h]->Write();   
  }
  if (fillCutHists) {
    for (unordered_map<string,TH2D*>::iterator hist = mtptHists.begin(); hist != mtptHists.end(); hist++) (hist->second)->Write();
    for (unordered_map<string,TH2D*>::iterator hist = mtmetHists.begin(); hist != mtmetHists.end(); hist++) (hist->second)->Write();
    for (unordered_map<string,TH2D*>::iterator hist = mt2metHists.begin(); hist != mt2metHists.end(); hist++) (hist->second)->Write();
    for (unordered_map<string,TH2D*>::iterator hist = mtmt2Hists.begin(); hist != mtmt2Hists.end(); hist++) (hist->second)->Write();
    for (unordered_map<string,TH2D*>::iterator hist = etaphiHists.begin(); hist != etaphiHists.end(); hist++) (hist->second)->Write();
    for (unordered_map<string,TH1D*>::iterator hist = cutHists.begin(); hist != cutHists.end(); hist++) (hist->second)->Write();
  }
  if (fillNM1Hists) {
    for (unordered_map<string,TH1D*>::iterator hist = cutHistsNM1.begin(); hist != cutHistsNM1.end(); hist++) (hist->second)->Write();
  }

  h_jet1_phi_MR_ST->Write();
  h_jet1_eta_MR_ST->Write();
  h_jet1_phi_VR_ST->Write();
  h_jet1_eta_VR_ST->Write();
  h_jet1_phi_SR_ST->Write();
  h_jet1_eta_SR_ST->Write();
  h_jet1_phi_MR_STC->Write();
  h_jet1_eta_MR_STC->Write();
  h_jet1_phi_VR_STC->Write();
  h_jet1_eta_VR_STC->Write();
  h_jet1_phi_SR_STC->Write();
  h_jet1_eta_SR_STC->Write();

  h_sigeffST->Write();
  h_sigeffSTC->Write();
  h_sigpassST->Write();
  h_sigpassSTC->Write();
  h_CharLength->Write();
  h_CharLength_etaphi->Write();

  h_nv_fullincl->Write();

  outfile_.Close();
  cout << "Wrote everything" << endl;

  return 0;
}

int main (int argc, char ** argv) {

  if (argc < 5) {
    cout << "Usage: ./FshortLooper.exe <outtag> <infile> <config> <mode>" << endl;
    return 1;
  }

  TChain *ch = new TChain("mt2");
  ch->Add(Form("%s*.root",argv[2]));

  FshortLooper * fsl = new FshortLooper();
  fsl->loop(ch,argv[1],string(argv[3]),string(argv[4]));
  return 0;
}

/*
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
*/
float FshortLooper::getAverageISRWeight(const string sample, const string config_tag) {
  // 2016 94x ttsl 0.908781126143
  // 2016 94x ttdl 0.895473127249

  cout << "Running on " << sample << endl;

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

FshortLooper::FshortLooper() {}
 
FshortLooper::~FshortLooper() {};
