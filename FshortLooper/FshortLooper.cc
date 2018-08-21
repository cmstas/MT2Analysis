#include "FshortLooper.h"

using namespace std;
using namespace duplicate_removal;

class mt2tree;

const bool recalculate = true; // recalculate Fshort with non-standard (ie not in babies) isolation and quality cutoffs, see below
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

int FshortLooper::loop (TChain* ch_st, char * outtag, char* config_tag) {
  string tag(outtag);

  // Book histograms
  TH1::SetDefaultSumw2(true); // Makes histograms do proper error calculation automatically

  TH2D* h_etaphi_ST_P = new TH2D("h_etaphi_ST_P","#eta and #phi of P Short Tracks",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_ST_M = new TH2D("h_etaphi_ST_M","#eta and #phi of M Short Tracks",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_ST_L = new TH2D("h_etaphi_ST_L","#eta and #phi of L Short Tracks",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());

  TH2D* h_etaphi_STC_P = new TH2D("h_etaphi_STC_P","#eta and #phi of P STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_STC_M = new TH2D("h_etaphi_STC_M","#eta and #phi of M STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
  TH2D* h_etaphi_STC_L = new TH2D("h_etaphi_STC_L","#eta and #phi of L STCs",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());

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

  TH2D* h_FSR_60to100MT2 = new TH2D("h_FSR_60to100MT2","ST and STC Counts by Length",4,0,4,4,0,4);
  h_FSR_60to100MT2->GetXaxis()->SetBinLabel(1,"Incl");
  h_FSR_60to100MT2->GetXaxis()->SetBinLabel(2,"P");
  h_FSR_60to100MT2->GetXaxis()->SetBinLabel(3,"M");
  h_FSR_60to100MT2->GetXaxis()->SetBinLabel(4,"L");
  h_FSR_60to100MT2->GetYaxis()->SetBinLabel(1,"f_{short}");
  h_FSR_60to100MT2->GetYaxis()->SetBinLabel(2,"ST");
  h_FSR_60to100MT2->GetYaxis()->SetBinLabel(3,"STC");
  h_FSR_60to100MT2->GetYaxis()->SetBinLabel(4,"Tracks");
  TH2D* h_FSR_100to200MT2 = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_100to200MT2");
  TH2D* h_FSR_100to200MT2_23 = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_100to200MT2_23");
  TH2D* h_FSR_100to200MT2_4 = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_100to200MT2_4");
  TH2D* h_FSR_gt200MT2 = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_gt200MT2");
  TH2D* h_FSR_gt200MT2_23 = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_gt200MT2_23");
  TH2D* h_FSR_gt200MT2_4 = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_gt200MT2_4");
  //  TH2D* h_FSR_gt0p3dphi = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_gt0p3dphi");
  //TH2D* h_FSR_lt0p3dphi = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_lt0p3dphi");
  //TH2D* h_FSR_0nlep = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_0nlep");
  //TH2D* h_FSR_gt0nlep = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_gt0nlep");
  TH2D* h_FSR_2to3njet = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_2to3njet");
  TH2D* h_FSR_gt3njet = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_gt3njet");
  TH2D* h_FSR_gt1000HT = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_gt1000HT");
  TH2D* h_FSR_lt1000HT = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_lt1000HT");
  TH2D* h_FSR_gt1000HT_23 = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_gt1000HT_23");
  TH2D* h_FSR_lt1000HT_23 = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_lt1000HT_23");
  TH2D* h_FSR_gt1000HT_4 = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_gt1000HT_4");
  TH2D* h_FSR_lt1000HT_4 = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_lt1000HT_4");

  TH2D* h_FSR_4to6njet = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_4to6njet");
  TH2D* h_FSR_gt6njet = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_gt6njet");
  TH2D* h_FSR_100to200MT2_46 = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_100to200MT2_46");
  TH2D* h_FSR_100to200MT2_7 = (TH2D*) h_FSR_60to100MT2->Clone("h_FSR_100to200MT2_7");

  //vector<TH2D*> fsrhists = {h_FSR_60to100MT2, h_FSR_100to200MT2, h_FSR_gt200MT2, h_FSR_gt0p3dphi, h_FSR_lt0p3dphi, h_FSR_0nlep, h_FSR_gt0nlep, h_FSR_2to3njet, h_FSR_gt3njet, h_FSR_lt1000HT, h_FSR_gt1000HT};

  vector<TH2D*> fsrhists = {h_FSR_60to100MT2, h_FSR_2to3njet, h_FSR_gt3njet, 
			    h_FSR_100to200MT2, h_FSR_100to200MT2_23, h_FSR_100to200MT2_4, 
			    h_FSR_gt200MT2, h_FSR_gt200MT2_23, h_FSR_gt200MT2_4,
			    h_FSR_lt1000HT, h_FSR_gt1000HT, h_FSR_lt1000HT_23, h_FSR_lt1000HT_4, h_FSR_gt1000HT_23, h_FSR_gt1000HT_4,
			    h_FSR_4to6njet, h_FSR_gt6njet, h_FSR_100to200MT2_46, h_FSR_100to200MT2_7};

  vector<TH1D*> mt2hists =  { h_mt2_ST_P, h_mt2_ST_M, h_mt2_ST_L, h_mt2_STC_P, h_mt2_STC_M, h_mt2_STC_L,
			      h_mt2_ST_P_23, h_mt2_ST_M_23, h_mt2_ST_L_23, h_mt2_STC_P_23, h_mt2_STC_M_23, h_mt2_STC_L_23,
			      h_mt2_ST_P_4, h_mt2_ST_M_4, h_mt2_ST_L_4, h_mt2_STC_P_4, h_mt2_STC_M_4, h_mt2_STC_L_4};

  vector<TH2D*> etaphihists = {h_etaphi_ST_P, h_etaphi_ST_M, h_etaphi_ST_L,h_etaphi_STC_P, h_etaphi_STC_M, h_etaphi_STC_L};
			    
  mt2tree t;
  t.Init(ch_st);

  // Load the configuration and output to screen
  config_ = GetMT2Config(string(config_tag));
  cout << "[MT2Looper::loop] using configuration tag: " << config_tag << endl;
  cout << "                  JSON: " << config_.json << endl;
  cout << "                  lumi: " << config_.lumi << " fb-1" << endl;


  if (applyJSON && config_.json != "") {
    cout << "[FshortLooper::loop] Loading json file: " << config_.json << endl;
    //set_goodrun_file(("../babymaker/jsons/"+config_.json).c_str());
    set_goodrun_file((config_.json).c_str());
  }

  const unsigned int nEventsTree = ch_st->GetEntries();
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
    if (t.ht < 300) {
      continue;
    }
    if (unlikely(t.nJet30FailId != 0)) {
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

    if (unlikely(t.met_miniaodPt / t.met_caloPt > 5.0)) {
      continue;
    }
    if (unlikely(t.nJet200MuFrac50DphiMet > 0)) {
      continue;
    }

    const bool lepveto = t.nMuons10 + t.nElectrons10 + t.nPFLep5LowMT + t.nPFHad10LowMT > 0;

    if (lepveto) {
      continue;
    }

    if (t.deltaPhiMin < 0.3) {
      continue;
    }

    // MET Filters
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
    
    // Not applied in MT2Looper
    // if (!t.Flag_METFilters) continue;
    // if (!t.Flag_trkPOG_manystripclus53X) continue;
    // if (!t.Flag_ecalLaserCorrFilter) continue;
    // if (!t.Flag_trkPOG_toomanystripclus53X) continue;
    // if (!t.Flag_hcalLaserEventFilter) continue;
    // if (!t.Flag_trkPOG_logErrorTooManyClusters) continue;
    // if (!t.Flag_trkPOG_Filters) continue;
    // if (!t.Flag_trackingFailureFilter) continue;
    // if (!t.Flag_CSCTightHaloFilter) continue;
    // if (!t.Flag_CSCTightHalo2015Filter) continue;

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


    // Triggers
    bool passPrescaleTrigger = t.HLT_PFHT125_Prescale || t.HLT_PFHT200_Prescale || t.HLT_PFHT300_Prescale || t.HLT_PFHT350_Prescale || t.HLT_PFHT475_Prescale || t.HLT_PFHT600_Prescale;
    bool passUnPrescaleTrigger = t.HLT_PFHT900 || t.HLT_PFJet450;
    bool passMetTrigger = t.HLT_PFHT300_PFMET110 || t.HLT_PFMET120_PFMHT120 || t.HLT_PFMETNoMu120_PFMHTNoMu120;
    
    if (! (passPrescaleTrigger || passUnPrescaleTrigger || passMetTrigger) ) {
      continue;
    }

    const float lumi = config_.lumi; // 2016
    double weight = t.isData ? 1.0 : t.evt_scale1fb * lumi;
    // if these are true, assume that data trigger is prescaled JetHT trigger
    if (!t.isData && (passPrescaleTrigger && !(passUnPrescaleTrigger || passMetTrigger)) ) {
      if (t.HLT_PFHT475_Prescale) weight *= 1.0 / 115.2;
      else if (t.HLT_PFHT350_Prescale) weight *= 1.0 / 460.6;
      else weight *= 1.0 / 9200.0; // t.HLT_PFHT125_Prescale
    }

    // 60to100MT2, 100to200MT2, gt200MT2, gt0p3dphi, lt0p3dphi, 0nlep, gt0nlep, 2to3njet, gt3njet, gt1000HT, lt1000HT
    vector<TH2D*> histsToFill;
    // Fshort region
    if (t.mt2 < 100) {
      histsToFill.push_back(h_FSR_60to100MT2);
      //      t.deltaPhiMin < 0.3 ? histsToFill.push_back(h_FSR_lt0p3dphi) : histsToFill.push_back(h_FSR_gt0p3dphi);
      //t.nElectrons10 + t.nMuons10 + t.nPFLep5LowMT + t.nPFHad10LowMT == 0 ? histsToFill.push_back(h_FSR_0nlep) : histsToFill.push_back(h_FSR_gt0nlep);
      if (t.nJet30 < 4) {
	histsToFill.push_back(h_FSR_2to3njet);
	t.ht < 1000 ? histsToFill.push_back(h_FSR_lt1000HT_23) : histsToFill.push_back(h_FSR_gt1000HT_23);
      } else {
	histsToFill.push_back(h_FSR_gt3njet);
	t.ht < 1000 ? histsToFill.push_back(h_FSR_lt1000HT_4) : histsToFill.push_back(h_FSR_gt1000HT_4);
      }

      if (t.nJet30 >= 4 && t.nJet30 <= 6) {
	histsToFill.push_back(h_FSR_4to6njet);
      } else if (t.nJet30 >= 7) {
	histsToFill.push_back(h_FSR_gt6njet);
      }

      t.ht < 1000 ? histsToFill.push_back(h_FSR_lt1000HT) : histsToFill.push_back(h_FSR_gt1000HT);
    }
    // Validation Region
    else if (t.mt2 < 200) {
      histsToFill.push_back(h_FSR_100to200MT2);
      t.nJet30 < 4 ? histsToFill.push_back(h_FSR_100to200MT2_23) : histsToFill.push_back(h_FSR_100to200MT2_4);

      if (t.nJet30 >= 4 && t.nJet30 <= 6) {
	histsToFill.push_back(h_FSR_100to200MT2_46);
      } else if (t.nJet30 >= 7) {
	histsToFill.push_back(h_FSR_100to200MT2_7);
      }

    }
    // Signal Region
    else if ( !(blind && t.isData) ) {
      histsToFill.push_back(h_FSR_gt200MT2); // Only look at MT2 > 200 GeV in data if unblinded
      t.nJet30 < 4 ? histsToFill.push_back(h_FSR_gt200MT2_23) : histsToFill.push_back(h_FSR_gt200MT2_4);
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
	const float absEta = fabs(t.track_eta[i_trk]);
	//	const bool etaSel = absEta < 2.4 && !( (absEta < 0.35 && absEta > 0.15) || (absEta < 1.65 && absEta > 1.42) || (absEta < 1.85 && absEta > 1.55) );
	const bool etaSel = absEta < 2.4 && !(absEta < 1.65 && absEta > 1.42);
	const bool BaseSel = CaloSel && ptSel && etaSel;
	
	if (!BaseSel) {
	  continue;
	}
	
	FillHists(histsToFill,weight,3); // No length yet, just filling inclusive tracks
	
	// Length and category
	// Length and categorization
	const bool lostOuterHitsSel = t.track_nLostOuterHits[i_trk] >= 2;
	const int nLayers = t.track_nLayersWithMeasurement[i_trk];
	const bool isP = nLayers == t.track_nPixelLayersWithMeasurement[ntracks] && lostOuterHitsSel;
	const bool isLong = nLayers >= 7;
	const bool isM = !isP && !isLong && lostOuterHitsSel;
	const bool isL = isLong && lostOuterHitsSel;
	const bool isShort = isP || isM || isL;
	if (!isShort) {
	  continue;
	}

	fillIndex = isP ? 1 : (isM ? 2 : 3);
	
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
	  if (t.track_isshort[i_trk]) {
	    cout << "ST: " << endl;
	  }
	  else if (t.track_iscandidate[i_trk]) {
	    cout << "STC: " << endl;
	  }
	  else continue;
	  cout << "LepOverlap: " << t.track_isLepOverlap[i_trk] << endl;
	  cout << "Reco<0.1: " << recoVeto << endl;
	  cout << "nearestPF: " << nearestPFSel << endl;
	  continue;
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
	const bool pixLayersSelLoose = t.track_nPixelLayersWithMeasurement[i_trk] >= 2;
	const bool pixLayersSelTight = t.track_nPixelLayersWithMeasurement[i_trk] >= 3;
	const bool pixLayersSel4 = nLayers > 4 ? pixLayersSelLoose : pixLayersSelTight; // Apply 2 layer selection to tracks with 5+ layers
	
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

	const bool isInHotspot = 
	  ((t.track_eta[i_trk] < -2.1 && t.track_eta[i_trk] > -2.4) && (t.track_phi[i_trk] < -1.2 && t.track_phi[i_trk] > -1.6)) 
	  || ( (t.track_eta[i_trk] < -1.9 && t.track_eta[i_trk] > -2.2) && (t.track_phi[i_trk] < -2.2 && t.track_phi[i_trk] > -2.6) );
	
	if (isInHotspot) {
	  if (isSTC) cout << "HOTSPOT: STC " << t.run << ":" << t.lumi << ":" << t.evt;
	  else if (isST) cout << "HOTSPOT: ST " << t.run << ":" << t.lumi << ":" << t.evt;
	  continue;
	} 


	if (isST && isL) {
	  cout << "Long ST: " << t.run << ":" << t.lumi << ":" << t.evt << endl;
	}
	if (isSTC && isL) {
	  cout << "Long STC: " << t.run << ":" << t.lumi << ":" << t.evt << endl;
	}
	
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

      // Fills
      if (isSTC) {
	FillHists(histsToFill,weight,2,fillIndex);
	if (!blind || t.mt2 < 200 || !t.isData) {
	  switch (fillIndex) {
	  case 1: 
	    h_mt2_STC_P->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_STC_P_23->Fill(t.mt2,weight) : h_mt2_STC_P_4->Fill(t.mt2,weight); 
	    h_etaphi_STC_P->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight); 
	    break;
	  case 2: 
	    h_mt2_STC_M->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_STC_M_23->Fill(t.mt2,weight) : h_mt2_STC_M_4->Fill(t.mt2,weight); 
	    h_etaphi_STC_M->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight); 
	    break;
	  case 3: 
	    h_mt2_STC_L->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_STC_L_23->Fill(t.mt2,weight) : h_mt2_STC_L_4->Fill(t.mt2,weight); 
	    h_etaphi_STC_L->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight); 
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
	    h_etaphi_ST_P->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight); 
	    break;
	  case 2: 
	    h_mt2_ST_M->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_ST_M_23->Fill(t.mt2,weight) : h_mt2_ST_M_4->Fill(t.mt2,weight); 
	    h_etaphi_ST_M->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight); 
	    break;
	  case 3: 
	    h_mt2_ST_L->Fill(t.mt2,weight); 
	    t.nJet30 < 4 ? h_mt2_ST_L_23->Fill(t.mt2,weight) : h_mt2_ST_L_4->Fill(t.mt2,weight); 
	    h_etaphi_ST_L->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight); 
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
  for (vector<TH2D*>::iterator hist = etaphihists.begin(); hist != etaphihists.end(); hist++) (*hist)->Write();
  for (vector<TH1D*>::iterator hist = mt2hists.begin(); hist != mt2hists.end(); hist++) (*hist)->Write();
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
  ch->Add(argv[2]);

  FshortLooper * fsl = new FshortLooper();
  fsl->loop(ch,argv[1],argv[3]);
  return 0;
}

FshortLooper::FshortLooper() {}

FshortLooper::~FshortLooper() {};
