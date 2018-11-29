#include "ShortTrackLooper.h"

using namespace std;
using namespace duplicate_removal;

class mt2tree;

const bool recalculate = true; // to use a non standard (ie not in babies) Fshort, change the iso and qual cuts and provide a new Fshort input file below
const int applyRecoVeto = 2; // 0: None, 1: use MT2 ID leptons for the reco veto, 2: use any Reco ID (Default: 2)
const bool increment17 = true;
const float isoSTC = 6, qualSTC = 3;
const bool adjL = true; // use fshort'(M) = fshort(M) * fshort(L)_mc / fshort(M)_mc instead of raw fshort(M) in place of fshort(L)

// turn on to apply json file to data
const bool applyJSON = true;
const bool blind = true;

const bool applyISRWeights = false; // evt_id is messed up in current babies, and we don't have isr weights implemented for 2017 MC anyway

const bool doNTrueIntReweight = true; // evt_id is messed up in current babies

TFile VetoFile("VetoHists.root");
TH2F* veto_bar = (TH2F*) VetoFile.Get("h_VetoEtaPhi_bar");
TH2F* veto_ecp = (TH2F*) VetoFile.Get("h_VetoEtaPhi_ecp");
TH2F* veto_ecn = (TH2F*) VetoFile.Get("h_VetoEtaPhi_ecn");

int ShortTrackLooper::InEtaPhiVetoRegion(float eta, float phi) {
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

int ShortTrackLooper::loop (TChain* ch, char * outtag, std::string config_tag, char * runtag) {
  string tag(outtag);

  cout << "Using runtag: " << runtag << endl;

  const TString FshortName16Data = Form("../FshortLooper/output/Fshort_data_2016_%s.root",runtag);
  const TString FshortName16MC = Form("../FshortLooper/output/Fshort_mc_2016_%s.root",runtag);
  const TString FshortName17Data = Form("../FshortLooper/output/Fshort_data_2017_%s.root",runtag);
  const TString FshortName17MC = Form("../FshortLooper/output/Fshort_mc_2017_%s.root",runtag);

  // Book histograms
  TH1::SetDefaultSumw2(true); // Makes histograms do proper error calculation automatically

  TH2D h_eventwise_counts ("h_eventwise_counts","Events Counts by Length",4,0,4,3,0,3);
  h_eventwise_counts.GetYaxis()->SetTitleOffset(3.0);
  h_eventwise_counts.GetXaxis()->SetBinLabel(1,"P");
  h_eventwise_counts.GetXaxis()->SetBinLabel(2,"M");
  h_eventwise_counts.GetXaxis()->SetBinLabel(3,"L (rej)");
  h_eventwise_counts.GetXaxis()->SetBinLabel(4,"L (acc)");
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


  // Save Fshorts used with errors
  TH1D* h_FS = new TH1D("h_FS","f_{short}",3,0,3);
  TH1D* h_FS_23 = (TH1D*) h_FS->Clone("h_FS_23");
  TH1D* h_FS_4 = (TH1D*) h_FS->Clone("h_FS_4");

  mt2tree t;

  t.Init(ch);

  // Load the configuration and output to screen
  config_ = GetMT2Config(config_tag);
  cout << "[ShortTrackLooper::loop] using configuration tag: " << config_tag << endl;
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
  }else if (config_tag.find("mc") != std::string::npos) {
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
    fillTriggerVector(t, trigs_singleLep_, data_config_.triggers["SingleEl"]);
    fillTriggerVector(t, trigs_singleLep_, data_config_.triggers["SingleMu"]);

  }    else {
    cout << "                  No triggers provided and no \"mc\" in config name. Weight calculation may be inaccurate." << endl;
    if(config_tag.find("data") != string::npos){
      cout << "[ShortTrackLooper::loop] WARNING! it looks like you are using data and didn't supply any triggers in the configuration." <<
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

  if (applyJSON && config_.json != "") {
    cout << "[ShortTrackLooper::loop] Loading json file: " << config_.json << endl;
    set_goodrun_file(("../babymaker/jsons/"+config_.json).c_str());
  }

  const int year = config_.year;  
  const bool isMC = config_tag.find("mc") != std::string::npos;

  cout << "Getting transfer factors" << endl;

  TString FshortName = "", FshortNameMC = "";
  if (year == 2016) {
    if (isMC) {
      FshortName = FshortName16MC;
    } else {
      FshortName = FshortName16Data;
    }
    FshortNameMC = FshortName16MC;
  }
  else if (year == 2017) {
    if (isMC) {
      FshortName = FshortName17MC;
    } else {
      FshortName = FshortName17Data;
    }
    FshortNameMC = FshortName17MC;
  }
  else {
    cout << "Check configuration" << endl;
    return -1;
  }

  TFile* FshortFile = TFile::Open(FshortName,"READ");
  TFile* FshortFileMC = TFile::Open(FshortNameMC,"READ");
  TH1D* h_fs = ((TH2D*) FshortFile->Get("h_fsFSR"))->ProjectionX("h_fs",1,1);
  TH1D* h_fs_Nj23 = ((TH2D*) FshortFile->Get("h_fsFSR_23"))->ProjectionX("h_fs_23",1,1);
  TH1D* h_fs_Nj4 = ((TH2D*) FshortFile->Get("h_fsFSR_4"))->ProjectionX("h_fs_4",1,1);
  TH1D* h_fs_MC = ((TH2D*) FshortFileMC->Get("h_fsFSR"))->ProjectionX("h_fs_MC",1,1);
  TH1D* h_fs_Nj23_MC = ((TH2D*) FshortFileMC->Get("h_fsFSR_23"))->ProjectionX("h_fs_23_MC",1,1);
  TH1D* h_fs_Nj4_MC = ((TH2D*) FshortFileMC->Get("h_fsFSR_4"))->ProjectionX("h_fs_4_MC",1,1);
  const float mc_L_corr = adjL ? h_fs_MC->GetBinContent(4) / h_fs_MC->GetBinContent(3) : 1.0; // fs_L / fs_M
  const float mc_L_corr_err = sqrt( pow((h_fs_MC->GetBinError(4) / h_fs_MC->GetBinContent(4)),2) + pow((h_fs_MC->GetBinError(3) / h_fs_MC->GetBinContent(3)),2)) * mc_L_corr;
  const float mc_L_corr_23 = adjL ? h_fs_Nj23_MC->GetBinContent(4) / h_fs_Nj23_MC->GetBinContent(3) : 1.0; // fs_L / fs_M
  const float mc_L_corr_err_23 = sqrt( pow((h_fs_Nj23_MC->GetBinError(4) / h_fs_Nj23_MC->GetBinContent(4)),2) + pow((h_fs_Nj23_MC->GetBinError(3) / h_fs_Nj23_MC->GetBinContent(3)),2)) * mc_L_corr;
  const float mc_L_corr_4 = adjL ? h_fs_Nj4_MC->GetBinContent(4) / h_fs_Nj4_MC->GetBinContent(3) : 1.0; // fs_L / fs_M
  const float mc_L_corr_err_4 = sqrt( pow((h_fs_Nj4_MC->GetBinError(4) / h_fs_Nj4_MC->GetBinContent(4)),2) + pow((h_fs_Nj4_MC->GetBinError(3) / h_fs_Nj4_MC->GetBinContent(3)),2)) * mc_L_corr;
  // bin 1 is inclusive, then P, M, L
  const double fs_P = h_fs->GetBinContent(2);
  const double fs_P_err = h_fs->GetBinError(2);
  const double fs_M = h_fs->GetBinContent(3);
  const double fs_M_err = h_fs->GetBinError(3);
  const double fs_L = h_fs->GetBinContent(3) * mc_L_corr;
  const double fs_L_err = adjL ? sqrt( pow((fs_M_err/fs_M),2) + pow((mc_L_corr_err / mc_L_corr),2) ) * fs_L : fs_M_err;
  const double fs_Nj23_P = h_fs_Nj23->GetBinContent(2);
  const double fs_Nj23_P_err = h_fs_Nj23->GetBinError(2);
  const double fs_Nj23_M = h_fs_Nj23->GetBinContent(3);
  const double fs_Nj23_M_err = h_fs_Nj23->GetBinError(3);
  const double fs_Nj23_L = h_fs_Nj23->GetBinContent(3) * mc_L_corr;
  const double fs_Nj23_L_err = adjL ? sqrt( pow((fs_Nj23_M_err/fs_Nj23_M),2) + pow((mc_L_corr_err_23 / mc_L_corr_23),2) ) * fs_Nj23_L : fs_Nj23_M_err;
  const double fs_Nj4_P = h_fs_Nj4->GetBinContent(2);
  const double fs_Nj4_P_err = h_fs_Nj4->GetBinError(2);
  const double fs_Nj4_M = h_fs_Nj4->GetBinContent(3);
  const double fs_Nj4_M_err = h_fs_Nj4->GetBinError(3);
  const double fs_Nj4_L = h_fs_Nj4->GetBinContent(3) * mc_L_corr;
  const double fs_Nj4_L_err = adjL ? sqrt( pow((fs_Nj4_M_err/fs_Nj4_M),2) + pow((mc_L_corr_err_4 / mc_L_corr_4),2) ) * fs_Nj4_L : fs_Nj4_M_err;

  cout << "Loaded transfer factors" << endl;

  cout << "fs_P    : " << fs_P << endl;
  cout << "fs_P err: " << fs_P_err << endl;
  cout << "fs_M    : " << fs_M << endl;
  cout << "fs_M err: " << fs_M_err << endl;
  cout << "fs_L    : " << fs_L << endl;
  cout << "fs_L err: " << fs_L_err << endl;
  cout << "fs_Nj23_P    : " << fs_Nj23_P << endl;
  cout << "fs_Nj23_P err: " << fs_Nj23_P_err << endl;
  cout << "fs_Nj23_M    : " << fs_Nj23_M << endl;
  cout << "fs_Nj23_M err: " << fs_Nj23_M_err << endl;
  cout << "fs_Nj23_L    : " << fs_Nj23_L << endl;
  cout << "fs_Nj23_L err: " << fs_Nj23_L_err << endl;
  cout << "fs_Nj4_P    : " << fs_Nj4_P << endl;
  cout << "fs_Nj4_P err: " << fs_Nj4_P_err << endl;
  cout << "fs_Nj4_M    : " << fs_Nj4_M << endl;
  cout << "fs_Nj4_M err: " << fs_Nj4_M_err << endl;
  cout << "fs_Nj4_L    : " << fs_Nj4_L << endl;
  cout << "fs_Nj4_L err: " << fs_Nj4_L_err << endl;
  
  FshortFile->Close();
  FshortFileMC->Close();

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
    if (t.ht < 250) {
    //if (t.ht < 250 || t.ht > 450) {
    //if (t.ht < 450) {
      continue;
    }
    if (unlikely(t.nJet30FailId != 0)) {
      continue;
    }
    if (t.mt2 < 100) {
      continue;
    }
    if (t.met_pt < 30) {
    //    if (t.met_pt < 30 || t.met_pt > 100) {
    //if (t.met_pt < 100 || t.met_pt > 250) {
    //    if (t.met_pt < 250) {
    //if (t.met_pt < 100) {
	continue;
    }

    if (t.diffMetMht / t.met_pt > 0.5) {
      continue;
    }

    if (unlikely(t.met_miniaodPt / t.met_caloPt >= 5.0)) {
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

    const float lumi = config_.lumi; 
    float weight = t.isData ? 1.0 : (t.evt_scale1fb == 1 ? 1.8863e-06 : t.evt_scale1fb) * lumi; // manually correct high HT WJets

    if (!t.isData) {
      weight *= t.weight_btagsf;

      if (doNTrueIntReweight) {
	if(h_nTrueInt_weights_==0){
	  cout << "[ShortTrackLooper::loop] WARNING! You requested to do nTrueInt reweighting but didn't supply a PU weights file. Turning off." << endl;
	}else{
	  int nTrueInt_input = t.nTrueInt;
	  float puWeight = h_nTrueInt_weights_->GetBinContent(h_nTrueInt_weights_->FindBin(nTrueInt_input));
	  weight *= puWeight;
	}
      }
    }

    if (weight > 1.0) continue;

    /*
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
	else if (year == 2017 && (t.met_pt < 250 && t.ht < 1200)) {
	  // use same values for 2017 for now
	  if (t.met_pt < 75) {
	    // assume 0.1 for everything below 100 GeV
	    weight *= 0.1;
	  }
	  else if (t.met_pt < 100) {
	    // assume PFHT800_PFMET75_PFMHT75 goes down to 75; let's just take that as the cutoff
	    weight *= ( (0.1/25) * (t.met_pt - 75) ); 
	  }
	  else if (t.met_pt < 200) {
	    weight *= (((0.8/100) * (t.met_pt - 100)) + 0.1);
	  } else {
	    weight *= (((0.1/50) * (t.met_pt - 200)) + 0.9);
	  }	  
	}
      }
    }
    */

    TH2D* hist;
    TH2D* hist_Nj;
    
    // L*
    if (t.nJet30 < 4) {
      // LH
      if (t.ht >= 1000) {
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
      if (t.ht >= 1000) {
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
	const bool CaloSel = !(t.track_DeadECAL[i_trk] || t.track_DeadHCAL[i_trk]) && InEtaPhiVetoRegion(t.track_eta[i_trk],t.track_phi[i_trk]) == 0 
	  && (year == 2016 || !(t.track_eta[i_trk] < -0.7 && t.track_eta[i_trk] > -0.9 && t.track_phi[i_trk] > 1.5 && t.track_phi[i_trk] < 1.7 ) )
	  && (year == 2016 || !(t.track_eta[i_trk] < 0.30 && t.track_eta[i_trk] > 0.10 && t.track_phi[i_trk] > 2.2 && t.track_phi[i_trk] < 2.5 ) )
	  && (year == 2016 || !(t.track_eta[i_trk] < 0.50 && t.track_eta[i_trk] > 0.40 && t.track_phi[i_trk] > -0.7 && t.track_phi[i_trk] < -0.5  ) )
	  && (year == 2016 || !(t.track_eta[i_trk] < 0.70 && t.track_eta[i_trk] > 0.60 && t.track_phi[i_trk] > -1.1 && t.track_phi[i_trk] < -0.9  ) );
	const float pt = t.track_pt[i_trk];
	const bool ptSel = pt >= 15.0;
	const bool etaSel = fabs(t.track_eta[i_trk]) <= 2.4;
	const bool BaseSel = CaloSel && ptSel && etaSel;
	
	if (!BaseSel) {
	  continue;
	}
	
	// Length and category
	// Length and categorization
	const bool lostOuterHitsSel = t.track_nLostOuterHits[i_trk] >= 2;
	const int nLayers = t.track_nLayersWithMeasurement[i_trk];
	const bool isP = nLayers == t.track_nPixelLayersWithMeasurement[i_trk] && lostOuterHitsSel && nLayers >= (increment17 ? (year == 2016 ? 3 : 4) : 3);
	const bool isLong = nLayers >= 7;
	const bool isM = nLayers != t.track_nPixelLayersWithMeasurement[i_trk] && !isLong && lostOuterHitsSel;
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
	const bool isQualityTrack = pixLayersSel4 && QualityTrackBase;
	const bool isQualityTrackSTC = pixLayersSel4 && QualityTrackSTCBase;
	
	if (!isQualityTrackSTC) {
	  continue;
	}
	
	// Full Short Track
	isST = PassesFullIsoSel && isQualityTrack;
	
	// Candidate (loosened isolation, quality)...already checked for Iso and Quality above
	isSTC = !isST;
	//	isSTC = !isST && (isP || iso > 30 || reliso > 0.4);
	if (! (isST || isSTC)) continue;
	
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
	else if (isSTC) {
	  switch(lenIndex) {
	  case 1: nSTCp++; break;
	  case 2: nSTCm++; break;
	  }
	}
	else if (isST) {
	  switch(lenIndex) {
	  case 1: nSTp++; break;
	  case 2: nSTm++; break;
	  }
	}
      } // Track loop
    } // recalculate
    
    // Eventwise STC Region fills
    // Chance of at least 1 track of given length being tagged is 1 - (1-p)^n
    /*
    if (nSTCl_rej > 0) {
      // For L tracks, we fill separately based on MtPt veto region, and extrapolate from STCs to STs with the Lfshort
      hist->Fill(2.0,2.0,weight); // Fill CR
      hist_Nj->Fill(2.0,2.0,weight); // Fill CR      
      hist->Fill(2.0,1.0, weight * ( 1 - pow(1-fs_L,nSTCl_rej) ) ); // Fill expected SR count
      hist_Nj->Fill(2.0,1.0, weight * ( 1 - pow(1-(t.nJet30 > 3 ? fs_L_4 : fs_L_23),nSTCl_rej) ) ); // Fill expected SR count with separate Nj fshort
    }
    else 
    */
    if (nSTCl_acc > 0) {
      // For L tracks, we fill separately based on MtPt veto region, and extrapolate from STCs to STs based on M fshort
      hist->Fill(2.0,2.0,weight); // Fill CR
      hist_Nj->Fill(2.0,2.0,weight); // Fill CR      
      hist->Fill(2.0,1.0, weight * ( 1 - pow(1-fs_L,nSTCl_acc) ) ); // Fill expected SR count
      hist_Nj->Fill(2.0,1.0, weight * ( 1 - pow(1-(t.nJet30 > 3 ? fs_Nj4_L : fs_Nj23_L),nSTCl_acc) ) ); // Fill expected SR count with separate Nj fshort
    }
    if (nSTCm > 0) {
      hist->Fill(1.0,2.0,weight); // Fill CR
      hist_Nj->Fill(1.0,2.0,weight); // Fill CR
      hist->Fill(1.0,1.0, weight * ( 1 - pow(1-fs_M,nSTCm) ) ); // Fill expected SR count
      hist_Nj->Fill(1.0,1.0, weight * ( 1 - pow(1-(t.nJet30 > 3 ? fs_Nj4_M : fs_Nj23_M),nSTCm) ) ); // Fill expected SR count with separate Nj fs
    }
    if (nSTCp > 0) {
      hist->Fill(0.0,2.0,weight); // Fill CR
      hist_Nj->Fill(0.0,2.0,weight); // Fill CR
      hist->Fill(0.0,1.0, weight * ( 1 - pow(1-fs_P,nSTCp) ) ); // Fill expected SR count
      hist_Nj->Fill(0.0,1.0, weight * ( 1 - pow(1-(t.nJet30 > 3 ? fs_Nj4_P : fs_Nj23_P),nSTCp) ) ); // Fill expected SR count with separate Nj fs
    }
    // Eventwise ST fills
    // Only full STs when counting "observed" SR
    /*
    if (nSTl_rej > 0) {
      hist->Fill(2.0,0.0,weight);
      hist_Nj->Fill(2.0,0.0,weight);
    }
    else 
    */
    if (nSTl_acc > 0) {
      hist->Fill(2.0,0.0,weight);
      hist_Nj->Fill(2.0,0.0,weight);
    }
    if (nSTm > 0) {
      hist->Fill(1.0,0.0,weight);
      hist_Nj->Fill(1.0,0.0,weight);
    }
    if (nSTp > 0) {
      hist->Fill(0.0,0.0,weight);
      hist_Nj->Fill(0.0,0.0,weight);
    }
  }//end loop on events in a file
  
  // Post-processing
  
  vector<TH2D*> hists   = {h_LL_SR, h_LH_SR, h_HL_SR, h_HH_SR, h_LL_VR, h_LH_VR, h_HL_VR, h_HH_VR};
  vector<TH2D*> hists23 = {h_LL_SR_23, h_LH_SR_23, h_LL_VR_23, h_LH_VR_23};
  vector<TH2D*> hists4  = {h_HL_SR_4, h_HH_SR_4, h_HL_VR_4, h_HH_VR_4};
  
  // save fshorts so we can easily propagate errors on fshort to errors on the predicted ST counts
  h_FS->SetBinContent(1,fs_P);
  h_FS->SetBinContent(2,fs_M);
  h_FS->SetBinContent(3,fs_L);
  h_FS->SetBinError(1,fs_P_err);
  h_FS->SetBinError(2,fs_M_err);
  h_FS->SetBinError(3,fs_L_err);
  
  h_FS_23->SetBinContent(1,fs_Nj23_P);
  h_FS_23->SetBinContent(2,fs_Nj23_M);
  h_FS_23->SetBinContent(3,fs_Nj23_L);
  h_FS_23->SetBinError(1,fs_Nj23_P_err);
  h_FS_23->SetBinError(2,fs_Nj23_M_err);
  h_FS_23->SetBinError(3,fs_Nj23_L_err);
  
  h_FS_4->SetBinContent(1,fs_Nj4_P);
  h_FS_4->SetBinContent(2,fs_Nj4_M);
  h_FS_4->SetBinContent(3,fs_Nj4_L);
  h_FS_4->SetBinError(1,fs_Nj4_P_err);
  h_FS_4->SetBinError(2,fs_Nj4_M_err);
  h_FS_4->SetBinError(3,fs_Nj4_L_err);
  
  cout << "About to write" << endl;
  
  TFile outfile_(Form("%s.root",outtag),"RECREATE"); 
  outfile_.cd();
  for (vector<TH2D*>::iterator hist = hists.begin(); hist != hists.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = hists23.begin(); hist != hists23.end(); hist++) (*hist)->Write();
  for (vector<TH2D*>::iterator hist = hists4.begin(); hist != hists4.end(); hist++) (*hist)->Write();
  h_FS->Write();
  h_FS_23->Write();
  h_FS_4->Write();
  outfile_.Close();
  cout << "Wrote everything" << endl;

  return 0;
}

int main (int argc, char ** argv) {

  if (argc < 5) {
    cout << "Usage: ./ShortTrackLooper.exe <outtag> <infile> <config> <runtag>" << endl;
    return 1;
  }

  TChain *ch = new TChain("mt2");
  ch->Add(Form("%s*.root",argv[2]));

  ShortTrackLooper * stl = new ShortTrackLooper();
  stl->loop(ch,argv[1],string(argv[3]),argv[4]);
  return 0;
}

ShortTrackLooper::ShortTrackLooper() {}

ShortTrackLooper::~ShortTrackLooper() {};
