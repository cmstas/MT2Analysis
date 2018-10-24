#include "ShortTrackLooper.h"

using namespace std;
using namespace duplicate_removal;

class mt2tree;

const bool recalculate = false; // to use a non standard (ie not in babies) Fshort, change the iso and qual cuts and provide a new Fshort input file below
const int applyRecoVeto = 2; // 0: None, 1: use MT2 ID leptons for the reco veto, 2: use any Reco ID (Default: 2)
const bool increment17 = false;
const float isoSTC = 6, qualSTC = 3;

// turn on to apply json file to data
const bool applyJSON = true;
const bool blind = true;

const char* FshortName16Data = "../FshortLooper/output/Fshort_data2016_fullveto.root";
const char* FshortName16MC = "../FshortLooper/output/Fshort_mc2016_fullveto.root";
const char* FshortName17Data = "../FshortLooper/output/Fshort_data2017_fullveto.root";
const char* FshortName17MC = "../FshortLooper/output/Fshort_mc2017_fullveto.root";
//const char* RmtptName16 = "../FshortLooper/output/Fshort_mc2016_fullveto.root";
const char* RmtptName16 = "../FshortLooper/output/Fshort_mc2017_fullveto.root"; // For now, use 2017 MC in 2016
const char* RmtptName17 = "../FshortLooper/output/Fshort_mc2017_fullveto.root";


int ShortTrackLooper::loop (TChain* ch, char * outtag, std::string config_tag) {
  string tag(outtag);

  // Book histograms
  TH1::SetDefaultSumw2(true); // Makes histograms do proper error calculation automatically

  TH2D h_eventwise_counts ("h_eventwise_counts","Events Counts by Length",5,0,5,3,0,3);
  h_eventwise_counts.GetYaxis()->SetTitleOffset(3.0);
  h_eventwise_counts.GetXaxis()->SetBinLabel(1,"P");
  h_eventwise_counts.GetXaxis()->SetBinLabel(2,"M");
  h_eventwise_counts.GetXaxis()->SetBinLabel(3,"L (rej)");
  h_eventwise_counts.GetXaxis()->SetBinLabel(4,"L (acc)");
  h_eventwise_counts.GetXaxis()->SetBinLabel(5,"L (acc; plus EWK)");
  h_eventwise_counts.GetYaxis()->SetBinLabel(1,"Obs SR Events");
  h_eventwise_counts.GetYaxis()->SetBinLabel(2,"Pred SR Events");
  h_eventwise_counts.GetYaxis()->SetBinLabel(3,"STC CR Events");

  TH2D h_eventwise_countsVR = *((TH2D*) h_eventwise_counts.Clone("h_eventwise_countsVR"));
  h_eventwise_countsVR.GetYaxis()->SetBinLabel(1,"Obs VR Events");
  h_eventwise_countsVR.GetYaxis()->SetBinLabel(2,"Pred VR Events");

  // NjHt
  TH2D* h_LL_VR = (TH2D*)h_eventwise_countsVR.Clone("h_LL_VR");
  TH2D* h_LH_VR = (TH2D*)h_eventwise_countsVR.Clone("h_LH_VR");
  TH2D* h_HL_VR = (TH2D*)h_eventwise_countsVR.Clone("h_HL_VR");
  TH2D* h_HH_VR = (TH2D*)h_eventwise_countsVR.Clone("h_HH_VR");

  TH2D* h_LL_SR = (TH2D*)h_eventwise_counts.Clone("h_LL_SR");
  TH2D* h_LH_SR = (TH2D*)h_eventwise_counts.Clone("h_LH_SR");
  TH2D* h_HL_SR = (TH2D*)h_eventwise_counts.Clone("h_HL_SR");
  TH2D* h_HH_SR = (TH2D*)h_eventwise_counts.Clone("h_HH_SR");

  // Separate fshorts
  TH2D* h_LL_VR_23 = (TH2D*)h_eventwise_countsVR.Clone("h_LL_VR_23");
  TH2D* h_LH_VR_23 = (TH2D*)h_eventwise_countsVR.Clone("h_LH_VR_23");
  TH2D* h_HL_VR_4 = (TH2D*)h_eventwise_countsVR.Clone("h_HL_VR_4");
  TH2D* h_HH_VR_4 = (TH2D*)h_eventwise_countsVR.Clone("h_HH_VR_4");

  TH2D* h_LL_SR_23 = (TH2D*)h_eventwise_counts.Clone("h_LL_SR_23");
  TH2D* h_LH_SR_23 = (TH2D*)h_eventwise_counts.Clone("h_LH_SR_23");
  TH2D* h_HL_SR_4 = (TH2D*)h_eventwise_counts.Clone("h_HL_SR_4");
  TH2D* h_HH_SR_4 = (TH2D*)h_eventwise_counts.Clone("h_HH_SR_4");

  mt2tree t;

  t.Init(ch);

  // Load the configuration and output to screen
  config_ = GetMT2Config(config_tag);
  cout << "[MT2Looper::loop] using configuration tag: " << config_tag << endl;
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

  if (applyJSON && config_.json != "") {
    cout << "[ShortTrackLooper::loop] Loading json file: " << config_.json << endl;
    set_goodrun_file(("../babymaker/jsons/"+config_.json).c_str());
  }

  const int year = config_.year;  
  const bool isMC = config_tag.find("mc") != std::string::npos;

  cout << "Getting transfer factors" << endl;

  string FshortName = "", RmtptName = "";
  if (year == 2016) {
    if (isMC) {
      FshortName = FshortName16MC;
    } else {
      FshortName = FshortName16Data;
    }
    RmtptName = RmtptName16;
  }
  else if (year == 2017) {
    if (isMC) {
      FshortName = FshortName17MC;
    } else {
      FshortName = FshortName17Data;
    }
    RmtptName = RmtptName17;
  }
  else {
    cout << "Check configuration" << endl;
    return -1;
  }

  TFile* FshortFile = TFile::Open(FshortName.c_str(),"READ");
  TFile* RmtptFile  = TFile::Open(RmtptName.c_str(),"READ");
  TH1D* h_fs = ((TH2D*) FshortFile->Get("h_fsFSR"))->ProjectionX("h_fs",1,1);
  TH1D* h_fs_Nj23 = ((TH2D*) FshortFile->Get("h_fsFSR_23"))->ProjectionX("h_fs_23",1,1);
  TH1D* h_fs_Nj4 = ((TH2D*) FshortFile->Get("h_fsFSR_4"))->ProjectionX("h_fs_4",1,1);
  // bin 1 is inclusive, then P, M, L
  const double fs_P = h_fs->GetBinContent(2);
  const double fs_M = h_fs->GetBinContent(3);
  //  const double fs_L = h_fs->GetBinContent(4);
  const double fs_P_23 = h_fs_Nj23->GetBinContent(2);
  const double fs_M_23 = h_fs_Nj23->GetBinContent(3);
  //  const double fs_L_23 = h_fs_Nj23->GetBinContent(4);
  const double fs_P_4 = h_fs_Nj4->GetBinContent(2);
  const double fs_M_4 = h_fs_Nj4->GetBinContent(3);
  //  const double fs_L_4 = h_fs_Nj4->GetBinContent(4);

  // Rmtpt is the ratio of accepted to vetoed electroweak tracks, taken from removed-lepton MC for each (rl_)MT2 band.
  TH2D* h_RmtptFSR = (TH2D*) RmtptFile->Get("h_RmtptFSR");
  TH2D* h_RmtptFSR_Nj23 = (TH2D*) RmtptFile->Get("h_RmtptFSR_23");
  TH2D* h_RmtptFSR_Nj4 = (TH2D*) RmtptFile->Get("h_RmtptFSR_4");
  const double RmtptFSR = h_RmtptFSR->GetBinContent(1,1);
  const double RmtptFSR_23 = h_RmtptFSR_Nj23->GetBinContent(1,1);
  const double RmtptFSR_4 = h_RmtptFSR_Nj4->GetBinContent(1,1);

  TH2D* h_RmtptVR = (TH2D*) RmtptFile->Get("h_RmtptVR");
  TH2D* h_RmtptVR_Nj23 = (TH2D*) RmtptFile->Get("h_RmtptVR_23");
  TH2D* h_RmtptVR_Nj4 = (TH2D*) RmtptFile->Get("h_RmtptVR_4");
  const double RmtptVR = h_RmtptVR->GetBinContent(1,1);
  const double RmtptVR_23 = h_RmtptVR_Nj23->GetBinContent(1,1);
  const double RmtptVR_4 = h_RmtptVR_Nj4->GetBinContent(1,1);

  TH2D* h_RmtptSR = (TH2D*) RmtptFile->Get("h_RmtptSR");
  TH2D* h_RmtptSR_Nj23 = (TH2D*) RmtptFile->Get("h_RmtptSR_23");
  TH2D* h_RmtptSR_Nj4 = (TH2D*) RmtptFile->Get("h_RmtptSR_4");
  const double RmtptSR = h_RmtptSR->GetBinContent(1,1);
  const double RmtptSR_23 = h_RmtptSR_Nj23->GetBinContent(1,1);
  const double RmtptSR_4 = h_RmtptSR_Nj4->GetBinContent(1,1);

  cout << "Loaded transfer factors" << endl;

  FshortFile->Close();
  RmtptFile->Close();

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
    if (t.ht < 300) {
      continue;
    }
    if (unlikely(t.nJet30FailId != 0)) {
      continue;
    }
    if (t.mt2 < 100) {
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
      
    // Triggers
    const bool passPrescaleTrigger = (year == 2016) ? 
      t.HLT_PFHT125_Prescale || t.HLT_PFHT350_Prescale || t.HLT_PFHT475_Prescale : 
      t.HLT_PFHT180_Prescale || t.HLT_PFHT370_Prescale || t.HLT_PFHT430_Prescale || t.HLT_PFHT510_Prescale || t.HLT_PFHT590_Prescale || t.HLT_PFHT680_Prescale || t.HLT_PFHT780_Prescale || t.HLT_PFHT890_Prescale;
    const bool passUnPrescaleTrigger = (year == 2016) ? 
      t.HLT_PFHT900 || t.HLT_PFJet450 :
      t.HLT_PFHT1050 || t.HLT_PFJet500;
    const bool passMetTrigger = t.HLT_PFHT300_PFMET110 || t.HLT_PFMET120_PFMHT120 || t.HLT_PFMETNoMu120_PFMHTNoMu120;
    
    if (! (passPrescaleTrigger || passUnPrescaleTrigger || passMetTrigger) ) {
      continue;
    }

    const bool passPrescaleTrigger = passTrigger(t, trigs_prescaled_);
    const bool passSRTrigger = passTrigger(t, trigs_SR_);

    if (! (passPrescaleTrigger || passSRTrigger)) continue;

    const float lumi = config_.lumi; // 2016
    float weight = t.isData ? 1.0 : (t.evt_scale1fb == 1 ? 1.8863e-06 : t.evt_scale1fb) * lumi; // manually correct high HT WJets
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

    TH2D* hist;
    TH2D* hist_Nj;
    
    // L*
    if (t.nJet30 < 4) {
      // LH
      if (t.ht > 1000) {
	// VR
	if (t.mt2 < 200) {
	  hist = h_LH_VR;
	  hist_Nj = h_LH_VR_23;
	}
	// SR 
	else if ( !(blind && t.isData) ) {
	  hist = h_LH_SR;
	  hist_Nj = h_LH_SR_23;
	}
	else continue;
      }
      // LL
      else {
	// VR
	if (t.mt2 < 200) {
	  hist = h_LL_VR;
	  hist_Nj = h_LL_VR_23;
	}
	// SR 
	else if (! (blind && t.isData) ){
	  hist = h_LL_SR;
	  hist_Nj = h_LL_SR_23;
	}
	else continue;
      }
    } 
    // H*
    else {
      // HH
      if (t.ht > 1000) {
	// VR
	if (t.mt2 < 200) {
	  hist = h_HH_VR;
	  hist_Nj = h_HH_VR_4;
	}
	// SR 
	else if (! (blind && t.isData) ) {
	    hist = h_HH_SR;
	    hist_Nj = h_HH_SR_4;
	}
	else continue;
      }
      // HL
      else {
	// VR
	if (t.mt2 < 200) {
	  hist = h_HL_VR;
	  hist_Nj = h_HL_VR_4;
	}
	// SR 
	else if (! (blind && t.isData) ){
	  hist = h_HL_SR;
	  hist_Nj = h_HL_SR_4;
	}
	else continue;
      }
    }

    // Analysis code

    int nSTCp = t.nshorttrackcandidates_P;
    int nSTCm = t.nshorttrackcandidates_M;
    int nSTp = t.nshorttracks_P;
    int nSTm = t.nshorttracks_M;

    int nSTCl_acc = 0; // L STCs, accepted by mtpt
    int nSTCl_rej = 0; // L STCs, rejected by mtpt
    int nSTl_acc = 0; // L STs, accepted by mtpt
    int nSTl_rej = 0; // L STs, rejected by mtpt

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
      nSTCm = 0;
      nSTp = 0;
      nSTm = 0;
      
      nSTCl_rej = 0;
      nSTCl_acc = 0;
      nSTl_rej = 0;
      nSTl_acc = 0;
      
      for (int i_trk = 0; i_trk < t.ntracks; i_trk++) {   

	int lenIndex = -1;
	bool isST = false, isSTC = false;
	
	// Apply basic selections
	const bool CaloSel = !(t.track_DeadECAL[i_trk] || t.track_DeadHCAL[i_trk]);
	const float pt = t.track_pt[i_trk];
	const bool ptSel = pt > 15;
	const bool etaSel = fabs(t.track_eta[i_trk]) < 2.4;
	const bool BaseSel = CaloSel && ptSel && etaSel;
	
	if (!BaseSel) {
	  continue;
	}
	
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
	
	const float mt = MT( t.track_pt[i_trk], t.track_phi[i_trk], t.met_pt, t.met_phi );	
	if (lenIndex == 3) {
	  if (mt < 100 && t.track_pt[i_trk] < 150) {
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
	    }
	    else {
	      nSTCl_acc++;
	    }
	  }
	}
	
	if (isSTC) {
	  switch(lenIndex) {
	  case 1: nSTCp++; break;
	  case 2: nSTCm++; break;
	  }
	}
	if (isST) {
	  switch(lenIndex) {
	  case 1: nSTp++; break;
	  case 2: nSTm++; break;
	  }
	}
      } // Track loop
    } // recalculate

    // Eventwise STC Region fills
    // Chance of at least 1 track of given length being tagged is 1 - (1-p)^n
    if (nSTCl_rej > 0) {
      // For L tracks, we fill separately based on MtPt veto region, and extrapolate from STCs to STs with the M fshort
      hist->Fill(2.0,2.0,weight); // Fill CR
      hist_Nj->Fill(2.0,2.0,weight); // Fill CR      
      hist->Fill(2.0,1.0, weight * ( 1 - pow(1-fs_M,nSTCl_rej) ) ); // Fill expected SR count
      hist_Nj->Fill(2.0,1.0, weight * ( 1 - pow(1-(t.nJet30 > 3 ? fs_M_4 : fs_M_23),nSTCl_rej) ) ); // Fill expected SR count with separate Nj fshort
    }
    else if (nSTCl_acc > 0) {
      // For L tracks, we fill separately based on MtPt veto region, and extrapolate from STCs to STs with the M fshort
      hist->Fill(3.0,2.0,weight); // Fill CR
      hist_Nj->Fill(3.0,2.0,weight); // Fill CR      
      hist->Fill(3.0,1.0, weight * ( 1 - pow(1-fs_M,nSTCl_acc) ) ); // Fill expected SR count
      hist_Nj->Fill(3.0,1.0, weight * ( 1 - pow(1-(t.nJet30 > 3 ? fs_M_4 : fs_M_23),nSTCl_acc) ) ); // Fill expected SR count with separate Nj fshort
    }
    else if (nSTCm > 0) {
      hist->Fill(1.0,2.0,weight); // Fill CR
      hist_Nj->Fill(1.0,2.0,weight); // Fill CR
      hist->Fill(1.0,1.0, weight * ( 1 - pow(1-fs_M,nSTCm) ) ); // Fill expected SR count
      hist_Nj->Fill(1.0,1.0, weight * ( 1 - pow(1-(t.nJet30 > 3 ? fs_M_4 : fs_M_23),nSTCm) ) ); // Fill expected SR count with separate Nj fs
    }
    else if (nSTCp > 0) {
      hist->Fill(0.0,2.0,weight); // Fill CR
      hist_Nj->Fill(0.0,2.0,weight); // Fill CR
      hist->Fill(0.0,1.0, weight * ( 1 - pow(1-fs_P,nSTCp) ) ); // Fill expected SR count
      hist_Nj->Fill(0.0,1.0, weight * ( 1 - pow(1-(t.nJet30 > 3 ? fs_P_4 : fs_P_23),nSTCp) ) ); // Fill expected SR count with separate Nj fs
    }
    // Eventwise ST fills
    // Only full STs when counting "observed" SR
    if (nSTl_rej > 0) {
      hist->Fill(2.0,0.0,weight);
      hist_Nj->Fill(2.0,0.0,weight);
    }
    else if (nSTl_acc > 0) {
      hist->Fill(3.0,0.0,weight);
      hist_Nj->Fill(3.0,0.0,weight);
    }
    else if (nSTm > 0) {
      hist->Fill(1.0,0.0,weight);
      hist_Nj->Fill(1.0,0.0,weight);
    }
    else if (nSTp > 0) {
      hist->Fill(0.0,0.0,weight);
      hist_Nj->Fill(0.0,0.0,weight);
    }
  }//end loop on events in a file

  // Post-processing

  // Now we subtract the STC->ST expected counts from the observed counts at in the mtpt veto region, and add some more L tracks to the accepted mtpt region

  vector<TH2D*> hists = {h_LL_SR, h_LH_SR, h_HL_SR, h_HH_SR, h_LL_VR, h_LH_VR, h_HL_VR, h_HH_VR,
			 h_LL_SR_23, h_LH_SR_23, h_HL_SR_4, h_HH_SR_4, h_LL_VR_23, h_LH_VR_23, h_HL_VR_4, h_HH_VR_4};

  for (vector<TH2D*>::iterator hist = hists.begin(); hist != hists.end(); hist++) {
    TH2D* h = (*hist);
    TString name = h->GetName();
    bool nj23 = name.Contains("23");
    bool nj4 = name.Contains("4");
    bool isFSR = name.Contains("FSR");
    bool isVR = name.Contains("VR");
    float Rmtpt = 0;
    if (nj23) {
      if (isFSR) {
	Rmtpt = RmtptFSR_23;
      }
      else if (isVR) {
	Rmtpt = RmtptVR_23;
      }
      else {
	Rmtpt = RmtptSR_23;
      }
    }
    else if (nj4) {
      if (isFSR) {
	Rmtpt = RmtptFSR_4;
      }
      else if (isVR) {
	Rmtpt = RmtptVR_4;
      }
      else {
	Rmtpt = RmtptSR_4;
      }
    }
    else {
      if (isFSR) {
	Rmtpt = RmtptFSR;
      }
      else if (isVR) {
	Rmtpt = RmtptVR;
      }
      else {
	Rmtpt = RmtptSR;
      }
    }
    const float obsRejST = h->GetBinContent(3,1);
    const float expRejST = h->GetBinContent(3,2);
    const float ewkRejST = max(obsRejST - expRejST, 0.0f);
    // Assume the excess rejected STs are from electroweak sources, and extrapolate these to the accepted region with Rmtpt from MC
    const float extraAccST = ewkRejST * Rmtpt;
    // 4,2 is the expectation of non-electroweak accepted STs. Add this to the expected accepted STs from electroweak sources.
    const float corrected_exp_Acc_ST = h->GetBinContent(4,2) + extraAccST;
    h->SetBinContent(5,1,h->GetBinContent(4,1)); // observed accepted STs is unchanged
    h->SetBinError(5,1,h->GetBinError(4,1)); // observed accepted STs is unchanged
    h->SetBinContent(5,2,corrected_exp_Acc_ST); // set the expected counts to the counts extrapolated from STCs plus the counts extrapolated from electroweak residual in mtpt veto region
    h->SetBinContent(5,3,ewkRejST); // The CR in this case is the residual STs in the mtpt veto region
  }



  cout << "About to write" << endl;

  TFile outfile_(Form("%s.root",outtag),"RECREATE"); 
  outfile_.cd();
  for (vector<TH2D*>::iterator hist = hists.begin(); hist != hists.end(); hist++) (*hist)->Write();
  outfile_.Close();
  cout << "Wrote everything" << endl;

  return 0;
}

int main (int argc, char ** argv) {

  if (argc < 4) {
    cout << "Usage: ./ShortTrackLooper.exe <outtag> <infile> <config>" << endl;
    return 1;
  }

  TChain *ch = new TChain("mt2");
  ch->Add(Form("%s*.root",argv[2]));

  ShortTrackLooper * stl = new ShortTrackLooper();
  stl->loop(ch,argv[1],string(argv[3]));
  return 0;
}

ShortTrackLooper::ShortTrackLooper() {}

ShortTrackLooper::~ShortTrackLooper() {};
