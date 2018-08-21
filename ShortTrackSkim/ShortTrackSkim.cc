#include "ShortTrackSkim.h"

using namespace std;
using namespace duplicate_removal;

class mt2tree;

const bool recalculate = true; // recalculate Fshort with non-standard (ie not in babies) isolation and quality cutoffs, see below
const float isoSTC = 6, qualSTC = 3; // change these if recalculating Fshort

// turn on to apply json file to data
const bool applyJSON = true;
const bool blind = true;


int ShortTrackSkim::loop (TChain* ch_st, char * outtag, char* config_tag) {
  string tag(outtag);

  mt2tree t;
  t.Init(ch_st);

  // Load the configuration and output to screen
  config_ = GetMT2Config(string(config_tag));
  cout << "[MT2Looper::loop] using configuration tag: " << config_tag << endl;
  cout << "                  JSON: " << config_.json << endl;
  cout << "                  lumi: " << config_.lumi << " fb-1" << endl;


  if (applyJSON && config_.json != "") {
    cout << "[ShortTrackSkim::loop] Loading json file: " << config_.json << endl;
    //set_goodrun_file(("../babymaker/jsons/"+config_.json).c_str());
    set_goodrun_file((config_.json).c_str());
  }

  TFile outfile_(Form("%s.root",outtag),"RECREATE"); 
  outfile_.cd();

  TTree* outtree;
  TEventList* saved = new TEventList();

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
    //    if (t.ht < 300) {
    //      continue;
    //    }
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

    //    const bool lepveto = t.nMuons10 + t.nElectrons10 + t.nPFLep5LowMT + t.nPFHad10LowMT > 0;

    //    if (lepveto) {
    //      continue;
    //    }

    if (t.nlep == 0) {
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

    // Signal Region
    if ( blind && t.isData && t.mt2 > 200 ) {
      continue; // No need to consider this event if blinded to it
    }

    // Analysis code

    bool save = false;

    const int ntracks = t.ntracks;
    for (int i_trk = 0; i_trk < ntracks && !save; i_trk++) {   

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
	
	if (isSTC && isInHotspot) {
	  cout << "HOTSPOT: STC " << t.run << ":" << t.lumi << ":" << t.evt << endl;
	  continue;
	} 
	else if (isST && isInHotspot) {
	  cout << "HOTSPOT: ST " << t.run << ":" << t.lumi << ":" << t.evt << endl;
	  continue;
	} 

      }
      else {
	if (t.track_iscandidate[i_trk]) {
	  isSTC = true;
	}
	else if (t.track_isshort[i_trk]) {
	  isST = true;
	}
      }

      save = isST || isSTC;

    } // Track loop

    if (save) {
      saved->Enter(event);
      cout << event << endl;
    }

  }//end loop on events in a file

  // Post-processing

  cout << "Processed " << nEventsTree << " events." << endl;
  cout << nDuplicates << " duplicates skipped." << endl;


  cout << "About to write" << endl;

  ch_st->SetEventList(saved);
  outtree = ch_st->CopyTree("");

  outtree->Write();

  outfile_.Close();
  cout << "Wrote everything" << endl;

  return 0;
}

int main (int argc, char ** argv) {

  if (argc < 4) {
    cout << "Usage: ./ShortTrackSkim.exe <outtag> <infile> <config>" << endl;
    return 1;
  }

  TChain *ch = new TChain("mt2");
  ch->Add(argv[2]);

  ShortTrackSkim * sts = new ShortTrackSkim();
  sts->loop(ch,argv[1],argv[3]);
  return 0;
}

ShortTrackSkim::ShortTrackSkim() {}

ShortTrackSkim::~ShortTrackSkim() {};
