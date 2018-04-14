#include "ShortTrackType.h"

using namespace std;

class mt2tree;
class sttree;

const float ht_limit = 100;
const float mt2_limit = 200;

const int maskHT = 1;
const int maskMET = 1<<1;
const int maskDphiDiff = 1<<2;
const int maskMT2 = 1<<3;
const int maskLep = 1<<4;
const int maskNj = 1<<5;
const int maskEta = 1<<6;
const int maskNt = 1<<7;
const int maskWpt = 1<<8;

const int SR = 0;
const int CRQCD = 1;
const int CRSL = 2;
const int CRDY = 3;

bool doIncl = false;
bool doNj = false;
bool doHT = false;
bool doMET = false;
bool doDphiDiff = false;
bool doMT2 = false;
bool doLep = false;
bool doEta = false;
bool doNt = false;
bool doWZpt = false;

string ShortTrackType::StringSelection(int selection, int region, bool isWjetsIncl, bool isDYjetsIncl) {
  vector<string> selections;
  if (doHT && ! doMET) {
    switch (region) {
    case SR : 
    case CRQCD : 
    case CRSL : selections.push_back(Form("(Sum$(jet_pt) > %f)",ht_limit)); break; 
    case CRDY : selections.push_back(Form("( (zll_ht - Sum$(lep_pt) > %f)",ht_limit)); break;
    }
  }
  if (doMET) {
    switch (region) {
    case SR : 
    case CRQCD : 
    case CRSL : selections.push_back(Form("((Sum$(jet_pt) > %f && met_pt > 250) || (met_pt > 30 && ht > 1000) )",ht_limit)); break; 
    case CRDY : selections.push_back(Form("( (zll_ht - Sum$(lep_pt) > %f && zll_met_pt > 250) || (zll_met_pt > 30 && zll_ht > 1000) )",ht_limit)); break;
    }    
  }
  if (doDphiDiff) {
    switch (region) {
    case SR : 
    case CRSL : selections.push_back("( deltaPhiMin > 0.3 && diffMetMht / met_pt < 0.5 )"); break;
    case CRDY : selections.push_back("( zll_deltaPhiMin > 0.3 && zll_diffMetMht / zll_met_pt < 0.5 )"); break;
    case CRQCD : selections.push_back("( deltaPhiMin < 0.3 && diffMetMht / met_pt < 0.5 )"); break;
    }
  }
  if (doMT2) {
    switch (region) {
    case SR :
    case CRQCD :
    case CRSL : selections.push_back(Form(" (mt2 > %f) ",mt2_limit)); break;
    case CRDY : selections.push_back(Form(" (zll_mt2 > %f) ",mt2_limit)); break;
    }
  }
  if (doLep) {
    switch(region) {
    case SR :
    case CRQCD : selections.push_back("( nElectrons10 + nMuons10 + nPFLep5LowMT + nPFHad10LowMT == 0)"); break;
    case CRSL : selections.push_back("( nLepLowMT == 1 )"); break;
    case CRDY : selections.push_back("( nlep == 2 )"); break;
    }
  }
  if (doNj) {
    selections.push_back("( nJet30 > 1 )");
  } else if (!doIncl) {
    selections.push_back("( nJet30 > 0 )");
  }
  if (doNt) {
    selections.push_back("( ntracks < 6 )");
  }

  if (doWZpt && isWjetsIncl) selections.push_back("( genStat23_pt > 100 && abs(genStat23_pdgId) == 24 )");
  if (doWZpt && isDYjetsIncl) selections.push_back("( genStat23_pt > 100 && abs(genStat23_pdgId) == 23 )");

  if (selections.size() == 0) return "";
  if (selections.size() == 1) return selections.at(0);
  string toReturn = selections.at(0);
  for (unsigned int i = 1; i < selections.size(); i++) {
    toReturn += " && " + selections.at(i);    
  }
  return "(" + toReturn + ")";
}

int ShortTrackType::loop (char * indir, char * sample, char * unskimmed_dir, int selection, int region) {
  doIncl = selection == 0;
  doHT = maskHT & selection;
  doMET = maskMET & selection;
  doDphiDiff = maskDphiDiff & selection;
  doMT2 = maskMT2 & selection;
  doLep = maskLep & selection;
  doNj = maskNj & selection;
  doEta = maskEta & selection;
  doNt = maskNt & selection;
  doWZpt = maskWpt & selection;

  string outdir = "";
  if (doIncl) outdir = "incl";
  else {
    if (doHT) outdir += "HT";
    if (doMET) outdir += "MET";
    if (doDphiDiff) outdir += "DphiDiff";
    if (doMT2) outdir += "MT2";
    if (doLep) outdir += "Lep";
    if (doNj) outdir += "Nj";
    if (doEta) outdir += "Eta";
    if (doNt) outdir += "Nt";
    if (doWZpt) outdir += "WZpt";
  }

  string output_prefix;
  switch(region) {
  case SR : output_prefix = "output_SR"; break;
  case CRQCD : output_prefix = "output_CRQCD"; break;
  case CRSL : output_prefix = "output_CRSL"; break;
  case CRDY : output_prefix = "output_CRDY"; break;
  }
  string fulloutdir = output_prefix+"/"+outdir;
  string mkdir = "mkdir -p " + fulloutdir;
  system(mkdir.c_str());
  TString output_name = Form("%s/%s.root",fulloutdir.c_str(),sample);

  TChain* ch_mt2 = new TChain("mt2st");
  TChain* ch_st = new TChain("st");

  TChain* ch_us_mt2 = new TChain("mt2st");
  TChain* ch_us_st = new TChain("st");

  TString infile_name = Form("%s/%s_incl_skim_friend.root",indir,sample);
  cout << "Skimmed file: " << infile_name.Data() << endl;
  ch_mt2->Add( infile_name );
  ch_st->Add( infile_name );

  TString unskimmed_name = Form("%s/%s.root",unskimmed_dir,sample);
  cout << "Unskimmed file: " << unskimmed_name.Data() << endl;
  ch_us_mt2->Add( unskimmed_name );
  ch_us_st->Add( unskimmed_name );
  TTree *tree_us_mt2 = (TTree*)ch_us_mt2->Clone("mt2st");
  TTree *tree_us_st = (TTree*)ch_us_st->Clone("st");
  tree_us_mt2->AddFriend(tree_us_st);

  // Get File Content
  TTree *tree_mt2 = (TTree*)ch_mt2->Clone("mt2st");
  TTree *tree_st = (TTree*)ch_st->Clone("st");

  string wjets_incl = "wjets_incl";
  string dyjets_incl = "dyjetstoll_incl_ext1";
  string sample_str = sample;
  bool isWjetsIncl = ! wjets_incl.compare(sample_str);
  bool isDYjetsIncl = ! dyjets_incl.compare(sample_str);

  string sel_string = this->StringSelection(selection,region,isWjetsIncl,isDYjetsIncl);
  int total_events = tree_us_mt2->GetEntries(sel_string.c_str());
  cout << "Total events in unskimmed file: " << total_events << endl;
  cout << "Total events is subject to adjustment if short tracks affect region definition." << endl;
  // sttree is a subtype of mt2tree. mt2tree functions can be called on an sttree, but not the reverse.
  // This line incorporates branches from tree_mt2 into tree_st.
  tree_st->AddFriend(tree_mt2);

  sttree t;

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

  // Book Histograms
  TH1::SetDefaultSumw2(true);
  TH1F h_unskimmed("h_unskimmed","Events in Unskimmed File",1,0,1);
  TH1F h_unskimmed_byNt("h_unskimmed_byNt","Events in Unskimmed File, by N_{trk}",10,0,10);

  // all trk lengths
  TH1F h_el_pix("h_el_pix","Electron STs, Pixel Layers",2,2,4);
  TH1F h_el_ext("h_el_ext","Electron STs, Non-Pixel Layers",8,0,8);
  TH1F h_el_tot("h_el_tot","Electron STs, Total Layers",11,0,11);
  TH1F h_fake_pix("h_fake_pix","(Mostly) Fake STs, Pixel Layers",2,2,4);
  TH1F h_fake_ext("h_fake_ext","(Mostly) Fake STs, Non-Pixel Layers",8,0,8);
  TH1F h_fake_tot("h_fake_tot","(Mostly) Fake STs, Total Layers",11,0,11);
  TH2F h_el_etaphi("h_el_etaphi","Electron STs, #eta and #phi",1200,-2.4,2.4,1200,-3.14,3.14);
  TH2F h_fake_etaphi("h_fake_etaphi","(Mostly) Fake STs, #eta and #phi",1200,-2.4,2.4,1200,-3.14,3.14);
  TH1F h_el_ratio("h_el_ratio","Gen e / ST p_T Ratio",15,0.5,2.0);
  TH1F h_el_met("h_el_met","Electron STs, MET",20,0,300);
  TH1F h_fake_met("h_fake_met","(Mostly) Fake STs, MET",20,0,300);

  TH1F h_el_byNt("h_el_byNt","Events with Electron STs, by N_{trk} (RelIso < 0.2 and Sum(ChPFCands within 0.01) < 0.01)",10,0,10);
  TH1F h_fake_byNt("h_fake_byNt","Events with (Mostly) Fake STs, by N_{trk} (RelIso < 0.2 and Sum(ChPFCands within 0.01) < 0.01)",10,0,10);

  TH1F h_all_nt("h_all_nt","N_{t}",10,0,10);
  TH1F h_el_nt("h_el_nt","N_{t}",10,0,10);
  TH1F h_fake_nt("h_fake_nt","N_{t}",10,0,10);

  // isshort
  TH1F h_el_pix_p("h_el_pix_p","Electron STs, Pixel Layers",2,2,4);
  TH1F h_el_ext_p("h_el_ext_p","Electron STs, Non-Pixel Layers",8,0,8);
  TH1F h_el_tot_p("h_el_tot_p","Electron STs, Total Layers",11,0,11);
  TH1F h_fake_pix_p("h_fake_pix_p","(Mostly) Fake STs, Pixel Layers",2,2,4);
  TH1F h_fake_ext_p("h_fake_ext_p","(Mostly) Fake STs, Non-Pixel Layers",8,0,8);
  TH1F h_fake_tot_p("h_fake_tot_p","(Mostly) Fake STs, Total Layers",11,0,11);
  TH2F h_el_etaphi_p("h_el_etaphi_p","Electron STs, #eta and #phi",1200,-2.4,2.4,1200,-3.14,3.14);
  TH2F h_fake_etaphi_p("h_fake_etaphi_p","(Mostly) Fake STs, #eta and #phi",1200,-2.4,2.4,1200,-3.14,3.14);
  TH1F h_el_ratio_p("h_el_ratio_p","Gen e / ST p_T Ratio",15,0.5,2.0);
  TH1F h_el_met_p("h_el_met_p","Electron STs, MET",20,0,300);
  TH1F h_fake_met_p("h_fake_met_p","(Mostly) Fake STs, MET",20,0,300);

  TH1F h_all_chDR_p("h_all_chDR_p","All STs, Nearest Charged DR",20,0,1.0);
  TH1F h_all_anyDR_p("h_all_anyDR_p","All STs, Nearest DR",20,0,1.0);
  TH1F h_all_chID_p("h_all_chID_p","All STs, Nearest Charged pdgID",250,0,250);
  TH1F h_all_anyID_p("h_all_anyID_p","All STs, Nearest pdgID",250,0,250);
  TH1F h_all_nearby_p("h_all_nearby_p","All STs, #DeltaR of Nearest Long Track",20,0,1.0);

  // istrkshort
  TH1F h_el_pix_ts("h_el_pix_ts","Electron STs, Pixel Layers",2,2,4);
  TH1F h_el_ext_ts("h_el_ext_ts","Electron STs, Non-Pixel Layers",8,0,8);
  TH1F h_el_tot_ts("h_el_tot_ts","Electron STs, Total Layers",11,0,11);
  TH1F h_fake_pix_ts("h_fake_pix_ts","(Mostly) Fake STs, Pixel Layers",2,2,4);
  TH1F h_fake_ext_ts("h_fake_ext_ts","(Mostly) Fake STs, Non-Pixel Layers",8,0,8);
  TH1F h_fake_tot_ts("h_fake_tot_ts","(Mostly) Fake STs, Total Layers",11,0,11);
  TH2F h_el_etaphi_ts("h_el_etaphi_ts","Electron STs, #eta and #phi",1200,-2.4,2.4,1200,-3.14,3.14);
  TH2F h_fake_etaphi_ts("h_fake_etaphi_ts","(Mostly) Fake STs, #eta and #phi",1200,-2.4,2.4,1200,-3.14,3.14);
  TH1F h_el_ratio_ts("h_el_ratio_ts","Gen e / ST p_T Ratio",15,0.5,2.0);
  TH1F h_el_met_ts("h_el_met_ts","Electron STs, MET",20,0,300);
  TH1F h_fake_met_ts("h_fake_met_ts","(Mostly) Fake STs, MET",20,0,300);

  // istrklong
  TH1F h_el_pix_tl("h_el_pix_tl","Electron STs, Pixel Layers",2,2,4);
  TH1F h_el_ext_tl("h_el_ext_tl","Electron STs, Non-Pixel Layers",8,0,8);
  TH1F h_el_tot_tl("h_el_tot_tl","Electron STs, Total Layers",11,0,11);
  TH1F h_fake_pix_tl("h_fake_pix_tl","(Mostly) Fake STs, Pixel Layers",2,2,4);
  TH1F h_fake_ext_tl("h_fake_ext_tl","(Mostly) Fake STs, Non-Pixel Layers",8,0,8);
  TH1F h_fake_tot_tl("h_fake_tot_tl","(Mostly) Fake STs, Total Layers",11,0,11);
  TH2F h_el_etaphi_tl("h_el_etaphi_tl","Electron STs, #eta and #phi",1200,-2.4,2.4,1200,-3.14,3.14);
  TH2F h_fake_etaphi_tl("h_fake_etaphi_tl","(Mostly) Fake STs, #eta and #phi",1200,-2.4,2.4,1200,-3.14,3.14);
  TH1F h_el_ratio_tl("h_el_ratio_tl","Gen e / ST p_T Ratio",15,0.5,2.0);
  TH1F h_el_met_tl("h_el_met_tl","Electron STs, MET",20,0,300);
  TH1F h_fake_met_tl("h_fake_met_tl","(Mostly) Fake STs, MET",20,0,300);

  for( unsigned int event = 0; event < nEventsTree; ++event) {
    
    // AddFriend above makes this pull both mt2 and st branches for the same event, assuming the trees were properly (identically) sorted. 
    t.GetEntry(event); 
    
    //    const float w_ = t.evt_xsec * 1000 / nEventsTree;
    const float w_ = 1.0; // Just count occurences without wrinkle of event weight

    //---------------------
    // basic event selection and cleaning
    //---------------------
    
    if (t.nVert == 0) continue;
    
    // random events with ht or met=="inf" or "nan" that don't get caught by the filters...
    if(isinf(t.met_pt) || isnan(t.met_pt) || isinf(t.ht) || isnan(t.ht)){
      cout << "WARNING: bad event with infinite MET/HT! " << t.run << ":" << t.lumi << ":" << t.evt
	   << ", met=" << t.met_pt << ", ht=" << t.ht << endl;
      continue;
    }
    
    // catch events with unphysical jet pt
    if(t.jet_pt[0] > 13000.){
      cout << "WARNING: bad event with unphysical jet pt! " << t.run << ":" << t.lumi << ":" << t.evt
	   << ", met=" << t.met_pt << ", ht=" << t.ht << ", jet_pt=" << t.jet_pt[0] << endl;
      continue;
    }

    // Collect some short track info
    int st_idx = -1;
    double st_phi = -999;
    double st_eta = -999;
    double st_pt = -1;
    int st_pix = -1;
    int st_ext = -1;
    int st_tot = -1;
    for (int i_trk = 0; i_trk < t.ntracks; i_trk++) {
      if (t.track_isshort[i_trk]) {
	st_idx = i_trk;
	st_phi = t.track_phi[i_trk];
	st_eta = t.track_eta[i_trk];
	st_pt = t.track_pt[i_trk];
	st_pix = t.track_pixelLayersWithMeasurement[i_trk];
	st_tot = t.track_trackerLayersWithMeasurement[i_trk];
	st_ext = st_tot - st_pix;
	break;
      }
    }

    if (st_idx < 0 || fabs(st_phi) > M_PI || fabs(st_eta) > 2.4 || st_pt < 0 || st_pix < 0 || st_ext < 0 || st_tot < 0) {
      cout << "Something went wrong reading from the short track in event index " << event << endl;
      cout << "st_idx = " << st_idx << endl;
      cout << "st_phi = " << st_phi << endl;
      cout << "st_eta = " << st_eta << endl;
      cout << "st_pt = " << st_pt << endl;
      cout << "st_pix = " << st_pix << endl;
      cout << "st_tot = " << st_tot << endl;
      t.Show(event);
      return 3;
    }

    if (doWZpt) {
      if (isWjetsIncl) {
	bool foundGoodW = false;
	for (int i = 0; i < t.ngenStat23 && ! foundGoodW; i++) {
	  if (abs(t.genStat23_pdgId[i]) != 24) continue;
	  if (t.genStat23_pt[i] > 100) foundGoodW = true;      
	}
	if (!foundGoodW) continue;
      }
      else if (isDYjetsIncl) {
	bool foundGoodZ = false;
	for (int i = 0; i < t.ngenStat23 && ! foundGoodZ; i++) {
	  if (abs(t.genStat23_pdgId[i]) != 23) continue;
	  if (t.genStat23_pt[i] > 100) foundGoodZ = true;      
	}
	if (!foundGoodZ) continue;
      }
    }

    if (!doIncl && t.nJet30 == 0) continue;

    if (doEta && st_eta > 1.4) continue;

    if (doNj && t.nJet30 < 2) continue;

    if (doNt && t.ntracks > 5) continue;

    if (doMET) {
      if (region != CRDY && ((t.ht < 1000 && t.met_pt < 250) || t.met_pt < 30 || t.ht < ht_limit) ) continue;
      // CRDY
      if (region == CRDY && ((t.zll_ht < 1000 && t.zll_met_pt < 250) || t.zll_met_pt < 30 || t.zll_ht < ht_limit) ) continue;
    }
    else if (region != CRDY && doHT && t.ht < 250) continue;
    else if (region == CRDY && doHT && t.zll_ht < 250) continue;

    // SR and CRSL
    if (region != CRQCD && region != CRDY && doDphiDiff && (t.deltaPhiMin < 0.3 || t.diffMetMht / t.met_pt > 0.5) ) continue;
    // CRQCD
    if (region == CRQCD && doDphiDiff && (t.deltaPhiMin > 0.3 || t.diffMetMht / t.met_pt > 0.5) ) continue;
    // CRDY
    if (region == CRDY && doDphiDiff && (t.zll_deltaPhiMin < 0.3 || t.zll_diffMetMht / t.zll_met_pt > 0.5) ) continue;

    // Not CRDY
    if (region != CRDY && doMT2 && t.mt2 < 200) continue;
    // CRDY
    if (region == CRDY && doMT2 && t.zll_mt2 < 200) continue;

    // In some cases, useful to store the index of a track matched to a lepton
    int i_lowMT_trk = -1;
    //int i_best_matched_trk1 = -1; 
    //int i_best_matched_trk2 = -1; 
    int i_lowMT_lep = -1;
    // SR
    if ( (region == SR || region == CRQCD) && doLep && (t.nElectrons10 + t.nMuons10 + t.nPFLep5LowMT + t.nPFHad10LowMT > 0)) continue;
    // CRSL
    if (region == CRSL && doLep) {
      if (t.nLepLowMT != 1) continue;
      // Find lowMT lep
      float lmt_phi = -999;
      float lmt_eta = -999;
      float met_pt = t.met_pt;
      float met_phi = t.met_phi;      
      for (int ilep = 0; ilep < t.nlep; ilep++) {
	float lep_pt = t.lep_pt[ilep];
	float lep_phi = t.lep_phi[ilep];
	float lep_eta = t.lep_eta[ilep];
	float mt = MT(lep_pt,lep_phi,met_pt,met_phi);
	if (mt < 100) {
	  // This guy is the LowMT lepton.
	  lmt_phi = lep_phi;
	  lmt_eta = lep_eta;
	  i_lowMT_lep = ilep;
	  break;
	}
      }
      // If the closest track is the short track, don't count this event as a short track event
      float smallestDR = 10;       
      for (int itrk = 0; itrk < t.ntracks; itrk++) {
	float trk_eta = t.track_eta[itrk];
	float trk_phi = t.track_phi[itrk];
	float thisDR = DeltaR(lmt_eta,lmt_phi,trk_eta,trk_phi);
	if (thisDR < smallestDR) {
	  smallestDR = thisDR;
	  i_lowMT_trk = itrk;	      
	}
      }     
      if (i_lowMT_trk == st_idx && smallestDR < 0.1) {
	cout << "Disqualifed short track: Low MT lepton." << endl;
	continue; // Best match is the short track, disqualified!
      }
      /*
      bool lepOverlap = false;
      for (int ilep = 0; ilep < t.nlep; ilep++) {
	//	float lep_pt = t.lep_pt[ilep];
	float lep_phi = t.lep_phi[ilep];
	float lep_eta = t.lep_eta[ilep];
	float DR = DeltaR(lep_phi, lep_eta, st_eta, st_phi);
	if (DR < 0.1) {
	  cout << "Disqualifed short track: within 0.1 of lepton" << endl;
	  lepOverlap = true;
	  break;
	}
      }
      if (lepOverlap) continue;
      */
    }
    // CRDY
    if (region == CRDY && doLep) {
      if (t.nlep != 2) continue;
      // If short track is associated to lepton, it doesn't count
      bool matched_st = false;
      for (int ilep = 0; ilep < t.nlep && ! matched_st; ilep++) {
	float lep_phi = t.lep_phi[ilep];
	float lep_eta = t.lep_eta[ilep];
	float smallestDR = 10; 
	int i_best_matched_trk = -1;
	for (int itrk = 0; itrk < t.ntracks; itrk++) {
	  float trk_eta = t.track_eta[itrk];
	  float trk_phi = t.track_phi[itrk];
	  float thisDR = DeltaR(lep_eta,lep_phi,trk_eta,trk_phi);
	  if (thisDR < smallestDR) {
	    smallestDR = thisDR;
	    i_best_matched_trk = itrk;	      
	  }
	}
	matched_st = i_best_matched_trk == st_idx; // Best match is short track, disqualified!
	//	if (ilep == 0) i_best_matched_trk1 = i_best_matched_trk;
	//else i_best_matched_trk2 = i_best_matched_trk;
      }
      if (matched_st) {
	cout << "Disqualified short track: lepton" << endl;
	continue;
      }
    }
    
    // Search for disappearing electron close to track.
    // Else assume track is fake since we don't have non-lep gen particles in babies.
    bool e_match = false;
    for (int i_lep = 0; i_lep < t.ngenLep; i_lep++) {
      if (abs(t.genLep_pdgId[i_lep]) != 11) continue;
      float lep_eta = t.genLep_eta[i_lep];
      float lep_phi = t.genLep_phi[i_lep];
      if (sqrt( pow(lep_eta - st_eta,2) + pow(lep_phi - st_phi,2) ) > 0.1 ) continue;
      float ratio_pt = t.genLep_pt[i_lep] / st_pt;
      if (ratio_pt < 0.5 || ratio_pt > 2.0) continue;
      h_el_ratio.Fill(ratio_pt,w_);
      // Matched to electron, probably disappearing. Don't have the calorimeter reading needed to confirm but...
      e_match = true;
      break;
    }

    if (t.track_ispixelonly[st_idx]) {
      h_all_chDR_p.Fill(abs(t.track_pfDR[st_idx]),w_);
      h_all_anyDR_p.Fill(abs(t.track_anypfDR[st_idx]),w_);
      h_all_chID_p.Fill(abs(t.track_pfPdgId[st_idx]),w_);
      h_all_anyID_p.Fill(abs(t.track_anypfPdgId[st_idx]),w_);
      float nearestDR = 10;
      for (int i_trk = 0; i_trk < t.ntracks; i_trk++) {
	if (t.track_isshort[i_trk]) continue;
	float track_eta = t.track_eta[i_trk];
	float track_phi = t.track_phi[i_trk];
	float DR = DeltaR(track_eta,track_phi,st_eta,st_phi);
	if (DR < nearestDR) {
	  nearestDR = DR;
	}
      }
      h_all_nearby_p.Fill(nearestDR,w_);
    }

    int nShortTrackCandidates = 0;
    for (int i_trk = 0; i_trk < t.ntracks; i_trk++) {
      if (t.track_pt[i_trk]>10 && fabs(t.track_eta[i_trk])<2.4 && fabs(t.track_dxy[i_trk])<0.03 && fabs(t.track_dz[i_trk])<0.06 
	  && (t.track_sumNeuH0p05[i_trk]+t.track_sumPh0p05[i_trk])<15 && (t.track_sumNeuH0p05[i_trk]+t.track_sumPh0p05[i_trk])/t.track_pt[i_trk]<0.2 
	  && t.track_sumChP[i_trk]<15 && t.track_sumChP[i_trk]/t.track_pt[i_trk]<0.2 && !(t.track_pfDR[i_trk]<0.1 && (abs(t.track_pfPdgId[i_trk])==11 || abs(t.track_pfPdgId[i_trk])==13))
	  && t.track_reliso[i_trk]<0.4 && t.track_iso[i_trk]<15.0 && t.track_ptErr[i_trk]/(t.track_pt[i_trk]*t.track_pt[i_trk])<0.2 && t.track_highPurity[i_trk]) {

	/*
	bool lepOverlap = false;
	// Only go up to 4th hardest lepton to keep parity with GetEntries() cut on denominator
	for (int i_lep = 0; i_lep < min(t.nlep, 4); i_lep++) {
	  if (DeltaR(t.lep_eta[i_lep],t.lep_phi[i_lep],t.track_eta[i_trk],t.track_phi[i_trk]) < 0.1) {
	    lepOverlap = true;
	    break;
	  }
	}
	if (lepOverlap) continue; // short track candidates can't overlap with leptons
	*/
	nShortTrackCandidates++;
      }
    }


    if (e_match) {
      h_el_pix.Fill(st_pix,w_);
      h_el_ext.Fill(st_ext,w_);
      h_el_tot.Fill(st_tot,w_);
      h_el_etaphi.Fill(st_eta,st_phi,w_);
      h_el_met.Fill(t.met_pt,w_);
      h_el_byNt.Fill(nShortTrackCandidates,w_);
      if (t.track_ispixelonly[st_idx]) {
	h_el_pix_p.Fill(st_pix,w_);
	h_el_ext_p.Fill(st_ext,w_);
	h_el_tot_p.Fill(st_tot,w_);
	h_el_etaphi_p.Fill(st_eta,st_phi,w_);
	h_el_met_p.Fill(t.met_pt);
      }
      else if (t.track_istrkshort[st_idx]) {
	h_el_pix_ts.Fill(st_pix,w_);
	h_el_ext_ts.Fill(st_ext,w_);
	h_el_tot_ts.Fill(st_tot,w_);
	h_el_etaphi_ts.Fill(st_eta,st_phi,w_);
	h_el_met_ts.Fill(t.met_pt);
      }
      else if (t.track_istrklong[st_idx]) {
	h_el_pix_tl.Fill(st_pix,w_);
	h_el_ext_tl.Fill(st_ext,w_);
	h_el_tot_tl.Fill(st_tot,w_);
	h_el_etaphi_tl.Fill(st_eta,st_phi,w_);
	h_el_met_tl.Fill(t.met_pt);
      } else {
	cout << "A short track is neither short, trkshort, or trklong. Something went wrong: evt = " << t.evt << endl;
      }

    } else {
      //cout << t.run << ":" << t.lumi << ":" << t.evt << endl;
      h_fake_pix.Fill(st_pix,w_);
      h_fake_ext.Fill(st_ext,w_);
      h_fake_tot.Fill(st_tot,w_);
      h_fake_etaphi.Fill(st_eta,st_phi,w_);
      h_fake_met.Fill(t.met_pt);
      
      h_fake_byNt.Fill(nShortTrackCandidates,w_);

      // The LowMT lepton's track can be as soft as 5 GeV, but we only store tracks > 10 GeV (and short tracks need 15 GeV)
      // So, there is a chance, but not guaranteed, that the lowMT lepton's track is contributing to our 10 GeV track count.
      // Subtract it if so.
      int ntracks = t.ntracks;
      if (region == CRSL && doLep) {
	float trk_eta = t.track_eta[i_lowMT_trk];
	float trk_phi = t.track_phi[i_lowMT_trk];
	float lep_eta = t.lep_phi[i_lowMT_lep];
	float lep_phi = t.lep_phi[i_lowMT_lep];
	float DR = DeltaR(trk_eta,trk_phi,lep_eta,lep_phi);
	if (DR < 0.1) {
	  ntracks--; // If the lowMT lep is matched to a track in the collection, remove from ntracks
	  // check for plausibility
	  if (t.lep_pt[i_lowMT_lep] < 10) {
	    cout << "Track removed from count despite LowMT lepton having pT < 10 GeV" << endl;
	    cout << "Track had pT = " << t.track_pt[i_lowMT_trk] << endl;
	  }
	  
	}
      } // Shouldn't need to adjust CRDY since its leptons ("nlep") are pT > 10 GeV

      h_fake_nt.Fill(ntracks);

      if (t.track_ispixelonly[st_idx]) {
	h_fake_pix_p.Fill(st_pix,w_);
	h_fake_ext_p.Fill(st_ext,w_);
	h_fake_tot_p.Fill(st_tot,w_);
	h_fake_etaphi_p.Fill(st_eta,st_phi,w_);
	h_fake_met_p.Fill(t.met_pt);
      }
      else if (t.track_istrkshort[st_idx]) {
	h_fake_pix_ts.Fill(st_pix,w_);
	h_fake_ext_ts.Fill(st_ext,w_);
	h_fake_tot_ts.Fill(st_tot,w_);
	h_fake_etaphi_ts.Fill(st_eta,st_phi,w_);
	h_fake_met_ts.Fill(t.met_pt);
      }
      else if (t.track_istrklong[st_idx]) {
	h_fake_pix_tl.Fill(st_pix,w_);
	h_fake_ext_tl.Fill(st_ext,w_);
	h_fake_tot_tl.Fill(st_tot,w_);
	h_fake_etaphi_tl.Fill(st_eta,st_phi,w_);
	h_fake_met_tl.Fill(t.met_pt);
      } else{
	cout << "A short track is neither short, trkshort, or trklong. Something went wrong: evt = " << t.evt << endl;
      }
    }

  }//end loop on events in a file
  
  h_unskimmed.SetBinContent(1,total_events);
  
  for (int ncand = 0; ncand < 4; ncand++) {
    stringstream ss;
    ss << "(Sum$(track_pt>10 && fabs(track_eta)<2.4 && fabs(track_dxy)<0.03 && fabs(track_dz)<0.06 && (track_sumNeuH0p05+track_sumPh0p05)<15 && (track_sumNeuH0p05+track_sumPh0p05)/track_pt<0.2 && track_sumChP<15 && track_sumChP/track_pt<0.2 && !(track_pfDR<0.1 && (abs(track_pfPdgId)==11 || abs(track_pfPdgId)==13)) && track_reliso<0.4 && track_iso<15.0 && track_ptErr/(track_pt*track_pt)<0.2 && track_highPurity";

    // Close Sum$, then == ncand, then close full nCandTrack selection to prepare for ANDing with runtime selection(s)
    ss << ") == " << ncand << ")";
    string nt_selection = ss.str();
    string sel_string_nt = sel_string + " && " + nt_selection;
    int toFill = tree_us_mt2->GetEntries(sel_string_nt.c_str());
    int bin = ncand+1; // bin 0 is underflow bin, bin 1 the zero bin...
    
    cout << sel_string_nt << ": " << toFill << endl;
    
    h_unskimmed_byNt.SetBinContent(bin,toFill);
    h_unskimmed_byNt.SetBinError(bin,sqrt(toFill));
  }
  
  cout << "About to write" << endl;

  TFile outfile_(output_name,"RECREATE"); 
  h_unskimmed.Write();
  h_unskimmed_byNt.Write();

  h_el_byNt.Write();
  h_fake_byNt.Write();

  h_el_pix.Write();
  h_el_ext.Write();
  h_el_tot.Write();
  h_el_etaphi.Write();
  h_el_ratio.Write();
  h_fake_pix.Write();
  h_fake_ext.Write();
  h_fake_tot.Write();
  h_fake_etaphi.Write();
  h_el_met.Write();
  h_fake_met.Write();

  h_all_nt.Write();
  h_el_nt.Write();
  h_fake_nt.Write();

  h_el_pix_p.Write();
  h_el_ext_p.Write();
  h_el_tot_p.Write();
  h_el_etaphi_p.Write();
  h_el_ratio_p.Write();
  h_fake_pix_p.Write();
  h_fake_ext_p.Write();
  h_fake_tot_p.Write();
  h_fake_etaphi_p.Write();
  h_el_met_p.Write();
  h_fake_met_p.Write();

  h_all_chDR_p.Write();
  h_all_anyDR_p.Write();
  h_all_chID_p.Write();
  h_all_anyID_p.Write();
  h_all_nearby_p.Write();

  h_el_pix_ts.Write();
  h_el_ext_ts.Write();
  h_el_tot_ts.Write();
  h_el_etaphi_ts.Write();
  h_el_ratio_ts.Write();
  h_fake_pix_ts.Write();
  h_fake_ext_ts.Write();
  h_fake_tot_ts.Write();
  h_fake_etaphi_ts.Write();
  h_el_met_ts.Write();
  h_fake_met_ts.Write();

  h_el_pix_tl.Write();
  h_el_ext_tl.Write();
  h_el_tot_tl.Write();
  h_el_etaphi_tl.Write();
  h_el_ratio_tl.Write();
  h_fake_pix_tl.Write();
  h_fake_ext_tl.Write();
  h_fake_tot_tl.Write();
  h_fake_etaphi_tl.Write();
  h_el_met_tl.Write();
  h_fake_met_tl.Write();

  outfile_.Close();

  cout << "Wrote everything" << endl;
  
  return 0;
}

int main (int argc, char ** argv) {

  if (argc < 6) {
    cout << "Usage: ./ShortTrackType.exe <indir> <sample> <unskimmed_dir> <selection> <region>" << endl;
    return 1;
  }

  const int selection = strtol(argv[4],NULL,2);
  const int region = atoi(argv[5]);
  cout << "selection: " << selection << endl;
  cout << "region: " << region << endl;
  ShortTrackType * stt = new ShortTrackType();
  stt->loop(argv[1],argv[2],argv[3],selection,region);
  return 0;
}

ShortTrackType::ShortTrackType() {}

ShortTrackType::~ShortTrackType() {};
