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

const bool applyISRWeights = true; // evt_id is messed up in current babies, and we don't have isr weights implemented for 2017 MC anyway

const bool doNTrueIntReweight = true; // evt_id is messed up in current babies

const bool blind = true;

const bool skipHighWeights = true; // turn on to skip MC events with weight > 1, to keep errors reasonable

const bool fillDiagnosticHists = false; // turn on to fill extra histograms

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

bool FshortLooper::FillHists(const vector<TH2D*>& hists, const double weight, const int fill_type, const int len_index) {
  if (unlikely(len_index < 1 || len_index > 6)) return false;
  if (unlikely(fill_type < 1 || fill_type > 2)) return false;
  for (vector<TH2D*>::const_iterator hist = hists.begin(); hist != hists.end(); hist++) {
    (*hist)->Fill(len_index - 0.5,(double) fill_type, weight);
  }
  return true;
}

int FshortLooper::loop (TChain* ch, char * outtag, std::string config_tag) {
  string tag(outtag);

  bool isSignal = tag.find("sim") != std::string::npos;

  cout << (isSignal ? "isSignal True" : "isSignal False") << endl;

  TH1F* h_xsec = 0;
  if (isSignal) {
    TFile * f_xsec = TFile::Open("../babymaker/data/xsec_susy_13tev.root");
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

  // Signal efficiency
  TH1D* h_sigeffST = new TH1D("h_sigeff","Signal ST Efficiency vs. Transverse Decay Length, Starting from p_{T} > 15 GeV lostTracks",10,0,100);
  TH1D* h_sigeffSTC = new TH1D("h_sigeffSTC","Signal STC Efficiency vs. Transverse Decay Length, Starting from p_{T} > 15 GeV lostTracks",10,0,100);
  TH1D* h_CharLength = new TH1D("h_CharLength","Chargino Track Length",10,0,100);

  unordered_map<string,TH2D*> mtptHists;
  unordered_map<string,TH2D*> etaphiHists;
  unordered_map<string,TH1D*> cutHists;
  string categories[] = {"ST","STC"};
  //  string lengths[] = {"P","P3","P4","M","L","Lrej","1L","1Lrej"};
  string lengths[] = {"P","P3","P4","M","L","Lrej"};
  string njethts[] = {"","_23","_4","_L","_H","_23L","_23H","_4L","_4H"};
  string matches[] = {"_","_ewke_","_ewkm_","_tewke_","_tewkm_","_newk_","_1newk_","_3newk_"};
  if (fillDiagnosticHists) {
    TH2D* h_mtpt_base = new TH2D("h_mtpt_base","p_{T} x M_{T}(Track,MET)",10,0,200,10,0,500);
    TH2D* h_etaphi_base = new TH2D("h_etaphi_base","#eta and #phi",100,-2.4,2.4,100,-TMath::Pi(),TMath::Pi());
    // Full hp takes too long
    //  TH1D* h_hp_base = new TH1D("h_hp_base","Hit Signature",(1<<17)-1,0,(1<<17)-1); // 2^17 - 1 = 131071; there are 18 layers (at most, ie 2^19 - 1 max hit pattern) in 2017, minus 2 for 2 lohs
    TH1D* h_hp_base = new TH1D("h_hp_base","Hit Signature",(1<<7)-1,0,(1<<7)-1); // store only inner 6 hits (M)
    for (int ibin = 0; ibin < h_hp_base->GetNbinsX(); ibin++) {
      h_hp_base->GetXaxis()->SetBinLabel(ibin+1,HitSigStr(ibin,6).c_str());
    }
    TH1D* h_dz_base = new TH1D("h_dz_base","dz",10,0,0.05);
    TH1D* h_dxy_base = new TH1D("h_dxy_base","dxy",10,0,0.02);
    TH1D* h_nv_base = new TH1D("h_nv_base","N_{V}",8,0,40);
    TH1D* h_pterr_base = new TH1D("h_pterr_base","#delta p_{T}/p_{T}^{2}",10,0,0.2);
    TH1D* h_iso_base = new TH1D("h_iso_base","AbsIso",20,0,10);
    TH1D* h_reliso_base = new TH1D("h_reliso_base","RelIso",20,0,0.2);
    TH1D* h_niso_base = new TH1D("h_niso_3c","Tight Cone Neutral Iso",20,0,10);
    TH1D* h_nreliso_base = new TH1D("h_nreliso_base","Tight Cone Neu RelIso",20,0,0.1);
    TH1D* h_eta_base = new TH1D("h_eta_base","|#eta|",24,0,2.4);
    TH1D* h_pt_base = new TH1D("h_pt_base","p_{T}",20,0,100);
    double mt2bins[9] = {60,68,76,84,92,100,120,140,200};
    TH1D* h_mt2_base = new TH1D("h_mt2_base","MT2",8,mt2bins);
    TH1D* h_dphiMet_base = new TH1D("h_dphiMet_base", "#Delta#phi(MET,ST)",10,0,TMath::Pi());
    TH1D* h_nb_base = new TH1D("h_nb_base","N_{Tag}",4,0,4);
    for (int cat = 0; cat < 2; cat++) {
      string category = categories[cat];
      for (int len = 0; len < 6; len++) {
	string length = lengths[len];
	cout << "Booked cut hists for " << category + " " + length << endl;
	for (int matchidx = 0; matchidx < 8; matchidx++) {
	  string match = matches[matchidx];
	  for (int njht = 0; njht < 9; njht++) {
	    string njetht = njethts[njht];
	    for (int reg = 0; reg < 4; reg++) {
	      string region = regions[reg];
	      string suffix = region+match+category+"_"+length+njetht;
	      // MtPt
	      string hname = "h_mtpt_"+suffix;
	      mtptHists[hname] = (TH2D*) h_mtpt_base->Clone(hname.c_str());
	      mtptHists[hname]->SetTitle( Form("%s of %s %ss;M_{T} (GeV);p_{T} (GeV)",h_mtpt_base->GetTitle(),length.c_str(),category.c_str()) );
	      // EtaPhi
	      hname = "h_etaphi_"+suffix;
	      etaphiHists[hname] = (TH2D*) h_etaphi_base->Clone(hname.c_str());
	      // Hit Pattern
	      hname = "h_hp_"+suffix;
	      cutHists[hname] = (TH1D*) h_hp_base->Clone(hname.c_str());
	      if (len < 3) cutHists[hname]->GetXaxis()->SetRange(0,(1<<5)-1); // At most 4 layers for P tracks
	      else if (len == 4) cutHists[hname]->GetXaxis()->SetRange(0,(1<<7)-1); // At most 6 layers for P tracks
	      // Dz
	      hname = "h_dz_"+suffix;
	      cutHists[hname] = (TH1D*) h_dz_base->Clone(hname.c_str());
	      // Dxy
	      hname = "h_dxy_"+suffix;
	      cutHists[hname] = (TH1D*) h_dxy_base->Clone(hname.c_str());
	      // N vertex
	      hname = "h_nv_"+suffix;
	      cutHists[hname] = (TH1D*) h_nv_base->Clone(hname.c_str());
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
	      // Eta
	      hname = "h_eta_"+suffix;
	      cutHists[hname] = (TH1D*) h_eta_base->Clone(hname.c_str());	    
	      // Pt
	      hname = "h_pt_"+suffix;
	      cutHists[hname] = (TH1D*) h_pt_base->Clone(hname.c_str());	    
	      // MT2
	      hname = "h_mt2_"+suffix;
	      cutHists[hname] = (TH1D*) h_mt2_base->Clone(hname.c_str());	    
	      // dphi Met
	      hname = "h_dphiMet_"+suffix;
	      cutHists[hname] = (TH1D*) h_dphiMet_base->Clone(hname.c_str());	    
	      // Ntag
	      hname = "h_nb_"+suffix;
	      cutHists[hname] = (TH1D*) h_nb_base->Clone(hname.c_str());	    
	    }
	  }
	}
      }
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
  cout << "processing " << nEventsTree << " events" << endl;
  int nDuplicates = 0;
  for( unsigned int event = 0; event < nEventsTree; ++event) {    
  //  for( unsigned int event = 0; event < 10; ++event) {    
    if (event % 100000 == 0) cout << 100.0 * event / nEventsTree  << "%" << endl;

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
    if (unlikely(t.met_miniaodPt / t.met_caloPt >= 5.0)) {
      continue;
    }
    if (unlikely(t.nJet200MuFrac50DphiMet > 0)) {
      continue;
    }

    const bool lepveto = t.nMuons10 + t.nElectrons10 + t.nPFLep5LowMT + t.nPFHad10LowMT > 0;

    if (lepveto) continue;
    
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

    //    const bool passPrescaleTrigger = passTrigger(t, trigs_prescaled_);
    const bool passSRTrigger = passTrigger(t, trigs_SR_);

    if (!passSRTrigger && !isSignal) continue;

    //    bool isSignal_ = t.evt_id >= 1000;

    float weight = t.isData ? 1.0 : (t.evt_scale1fb == 1 ? 1.8863e-06 : t.evt_scale1fb) * lumi; // manually correct high HT WJets

    if (isSignal) {
      int bin = h_xsec->GetXaxis()->FindBin(1800);      
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

      if (skipHighWeights && weight > 1.0) continue; // skip high weighted events

    }
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
    */

    if (weight < 0.0) {
      cout << "Negative weight: " << weight << ", " << t.run << ":" << t.lumi << ":" << t.evt << endl;
    }

    vector<TH2D*> histsToFill; 
    vector<string> regsToFill = {"Incl"};
    vector<string> njhtToFill = {""};
    bool lowJ = t.nJet30 < 4;
    bool lowHT = t.ht < 1200;
    if (lowJ) {
      njhtToFill.push_back("_23");
      if (lowHT) {
	njhtToFill.push_back("_23L");	
      }
      else {
	njhtToFill.push_back("_23H");	
      }
    }
    else {
      njhtToFill.push_back("_4");
      if (lowHT) {
	njhtToFill.push_back("_4L");	
      }
      else {
	njhtToFill.push_back("_4H");	
      }      
    }
    if (lowHT) njhtToFill.push_back("_L");
    else njhtToFill.push_back("_H");
    // Fshort region
    if (t.mt2 < 100) {
      regsToFill.push_back("MR");
      histsToFill.push_back(fsHists["h_fsMR_Baseline"]);
      t.ht < 1200 ? histsToFill.push_back(fsHists["h_fsMR_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsMR_gt1200HT"]);
      t.ht < 450 ? histsToFill.push_back(fsHists["h_fsMR_HT250"]) : histsToFill.push_back(fsHists["h_fsMR_HT450"]);
      if (t.met_pt < 100) histsToFill.push_back(fsHists["h_fsMR_MET30"]);
      else if (t.met_pt < 250) histsToFill.push_back(fsHists["h_fsMR_MET100"]);
      else histsToFill.push_back(fsHists["h_fsMR_MET250"]);
      if (t.met_pt >= 100 && t.ht >= 450) histsToFill.push_back(fsHists["h_fsMR_HT450MET100"]);
      if (t.nJet30 < 4) {
	histsToFill.push_back(fsHists["h_fsMR_23_Baseline"]);
	t.ht < 1200 ? histsToFill.push_back(fsHists["h_fsMR_23_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsMR_23_gt1200HT"]);
	t.ht < 450 ? histsToFill.push_back(fsHists["h_fsMR_23_HT250"]) : histsToFill.push_back(fsHists["h_fsMR_23_HT450"]);
	if (t.met_pt < 100) histsToFill.push_back(fsHists["h_fsMR_23_MET30"]);
	else if (t.met_pt < 250) histsToFill.push_back(fsHists["h_fsMR_23_MET100"]);
	else histsToFill.push_back(fsHists["h_fsMR_23_MET250"]);
	if (t.met_pt >= 100 && t.ht >= 450) histsToFill.push_back(fsHists["h_fsMR_23_HT450MET100"]);
      } else {
	histsToFill.push_back(fsHists["h_fsMR_4_Baseline"]);
	t.ht < 1200 ? histsToFill.push_back(fsHists["h_fsMR_4_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsMR_4_gt1200HT"]);
	t.ht < 450 ? histsToFill.push_back(fsHists["h_fsMR_4_HT250"]) : histsToFill.push_back(fsHists["h_fsMR_4_HT450"]);
	if (t.met_pt < 100) histsToFill.push_back(fsHists["h_fsMR_4_MET30"]);
	else if (t.met_pt < 250) histsToFill.push_back(fsHists["h_fsMR_4_MET100"]);
	else histsToFill.push_back(fsHists["h_fsMR_4_MET250"]);
	if (t.met_pt >= 100 && t.ht >= 450) histsToFill.push_back(fsHists["h_fsMR_4_HT450MET100"]);
      }
    }
    else if (t.mt2 < 200) {
      regsToFill.push_back("VR");
      histsToFill.push_back(fsHists["h_fsVR_Baseline"]);
      t.ht < 1200 ? histsToFill.push_back(fsHists["h_fsVR_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsVR_gt1200HT"]);
      t.ht < 450 ? histsToFill.push_back(fsHists["h_fsVR_HT250"]) : histsToFill.push_back(fsHists["h_fsVR_HT450"]);
      if (t.met_pt < 100) histsToFill.push_back(fsHists["h_fsVR_MET30"]);
      else if (t.met_pt < 250) histsToFill.push_back(fsHists["h_fsVR_MET100"]);
      else histsToFill.push_back(fsHists["h_fsVR_MET250"]);
      if (t.met_pt >= 100 && t.ht >= 450) histsToFill.push_back(fsHists["h_fsVR_HT450MET100"]);
      if (t.nJet30 < 4) {
	histsToFill.push_back(fsHists["h_fsVR_23_Baseline"]);
	t.ht < 1200 ? histsToFill.push_back(fsHists["h_fsVR_23_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsVR_23_gt1200HT"]);
	t.ht < 450 ? histsToFill.push_back(fsHists["h_fsVR_23_HT250"]) : histsToFill.push_back(fsHists["h_fsVR_23_HT450"]);
	if (t.met_pt < 100) histsToFill.push_back(fsHists["h_fsVR_23_MET30"]);
	else if (t.met_pt < 250) histsToFill.push_back(fsHists["h_fsVR_23_MET100"]);
	else histsToFill.push_back(fsHists["h_fsVR_23_MET250"]);
	if (t.met_pt >= 100 && t.ht >= 450) histsToFill.push_back(fsHists["h_fsVR_23_HT450MET100"]);
      } else {
	histsToFill.push_back(fsHists["h_fsVR_4_Baseline"]);
	t.ht < 1200 ? histsToFill.push_back(fsHists["h_fsVR_4_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsVR_4_gt1200HT"]);
	t.ht < 450 ? histsToFill.push_back(fsHists["h_fsVR_4_HT250"]) : histsToFill.push_back(fsHists["h_fsVR_4_HT450"]);
	if (t.met_pt < 100) histsToFill.push_back(fsHists["h_fsVR_4_MET30"]);
	else if (t.met_pt < 250) histsToFill.push_back(fsHists["h_fsVR_4_MET100"]);
	else histsToFill.push_back(fsHists["h_fsVR_4_MET250"]);
	if (t.met_pt >= 100 && t.ht >= 450) histsToFill.push_back(fsHists["h_fsVR_4_HT450MET100"]);
      }
    }
    // Signal Region
    else if ( !(blind && t.isData) ) { // Only look at MT2 > 200 GeV in data if unblinded
      regsToFill.push_back("SR");
      histsToFill.push_back(fsHists["h_fsSR_Baseline"]);
      t.ht < 1200 ? histsToFill.push_back(fsHists["h_fsSR_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsSR_gt1200HT"]);
      t.ht < 450 ? histsToFill.push_back(fsHists["h_fsSR_HT250"]) : histsToFill.push_back(fsHists["h_fsSR_HT450"]);
      if (t.met_pt < 100) histsToFill.push_back(fsHists["h_fsSR_MET30"]);
      else if (t.met_pt < 250) histsToFill.push_back(fsHists["h_fsSR_MET100"]);
      else histsToFill.push_back(fsHists["h_fsSR_MET250"]);
      if (t.met_pt >= 100 && t.ht >= 450) histsToFill.push_back(fsHists["h_fsSR_HT450MET100"]);
      if (t.nJet30 < 4) {
	histsToFill.push_back(fsHists["h_fsSR_23_Baseline"]);
	t.ht < 1200 ? histsToFill.push_back(fsHists["h_fsSR_23_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsSR_23_gt1200HT"]);
	t.ht < 450 ? histsToFill.push_back(fsHists["h_fsSR_23_HT250"]) : histsToFill.push_back(fsHists["h_fsSR_23_HT450"]);
	if (t.met_pt < 100) histsToFill.push_back(fsHists["h_fsSR_23_MET30"]);
	else if (t.met_pt < 250) histsToFill.push_back(fsHists["h_fsSR_23_MET100"]);
	else histsToFill.push_back(fsHists["h_fsSR_23_MET250"]);
	if (t.met_pt >= 100 && t.ht >= 450) histsToFill.push_back(fsHists["h_fsSR_23_HT450MET100"]);
      } else {
	histsToFill.push_back(fsHists["h_fsSR_4_Baseline"]);
	t.ht < 1200 ? histsToFill.push_back(fsHists["h_fsSR_4_lt1200HT"]) : histsToFill.push_back(fsHists["h_fsSR_4_gt1200HT"]);
	t.ht < 450 ? histsToFill.push_back(fsHists["h_fsSR_4_HT250"]) : histsToFill.push_back(fsHists["h_fsSR_4_HT450"]);
	if (t.met_pt < 100) histsToFill.push_back(fsHists["h_fsSR_4_MET30"]);
	else if (t.met_pt < 250) histsToFill.push_back(fsHists["h_fsSR_4_MET100"]);
	else histsToFill.push_back(fsHists["h_fsSR_4_MET250"]);
	if (t.met_pt >= 100 && t.ht >= 450) histsToFill.push_back(fsHists["h_fsSR_4_HT450MET100"]);
      }
    }
    // Should only get here if looking at data while blinded
    else {
      continue; // No need to consider this event if not filling any hists
    }

    // Analysis code

    // Main Looper

    const int ntracks = t.ntracks;
    for (int i_trk = 0; i_trk < ntracks; i_trk++) {   

      // Apply basic selections
      const bool CaloSel = !(t.track_DeadECAL[i_trk] || t.track_DeadHCAL[i_trk]) && InEtaPhiVetoRegion(t.track_eta[i_trk],t.track_phi[i_trk]) == 0 
	&& (year == 2016 || !(t.track_eta[i_trk] < -0.7 && t.track_eta[i_trk] > -0.9 && t.track_phi[i_trk] > 1.5 && t.track_phi[i_trk] < 1.7 ) )
	&& (year == 2016 || !(t.track_eta[i_trk] < 0.30 && t.track_eta[i_trk] > 0.10 && t.track_phi[i_trk] > 2.2 && t.track_phi[i_trk] < 2.5 ) )
	&& (year == 2016 || !(t.track_eta[i_trk] < 0.50 && t.track_eta[i_trk] > 0.40 && t.track_phi[i_trk] > -0.7 && t.track_phi[i_trk] < -0.5  ) )
	&& (year == 2016 || !(t.track_eta[i_trk] < 0.70 && t.track_eta[i_trk] > 0.60 && t.track_phi[i_trk] > -1.1 && t.track_phi[i_trk] < -0.9  ) )
	&& (year != 2018 || !(t.track_eta[i_trk] < 2.45 && t.track_eta[i_trk] > 2.00 && t.track_phi[i_trk] > 1.7 && t.track_phi[i_trk] < 1.9));
      //	const bool CaloSel = !(t.track_DeadECAL[i_trk] || t.track_DeadHCAL[i_trk]);
      if (!CaloSel) continue;

      int lenIndex = -1;
      const bool isChargino = isSignal && t.track_matchedCharginoIdx[i_trk] >= 0;
      if (isChargino) {
	h_CharLength->Fill( t.track_decayXY[i_trk], weight );
      }
      string ptstring = t.track_pt[i_trk] < 50 ? "_lowpt" : "_hipt";
      bool isST = false, isSTC = false;//, isST1 = false, isSTC1 = false;
      if (recalculate) {

	const float pt = t.track_pt[i_trk];
	// changed from pt > 15
	const bool ptSel = pt >= 15.0;
	//if (pt == 15.0) cout << "CHANGE: pt == 15.0" << endl;
	const bool etaSel = fabs(t.track_eta[i_trk]) <= 2.4;
	//if (fabs(t.track_eta[i_trk]) == 2.4) cout << "CHANGE: eta == 2.4" << endl;
	const bool BaseSel = ptSel && etaSel;
	
	if (!BaseSel || (isSignal && t.track_isBadCharginoTrack[i_trk])) {
	  if (isChargino) {
	    cout << "Chargino failed base selection" << endl;
	  }
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
	//	const bool is1L = isLong && t.track_nLostOuterHits[i_trk] == 1;
	const bool isShort = isP || isM || isL;

	//	if (!isShort && !is1L) {
	if (!isShort) {
	  if (isChargino && !isShort) cout << "Chargino failed shortness selection. Layers: " << nLayers << ", MOH: " << t.track_nLostOuterHits[i_trk] << endl;
	  continue;
	}

	lenIndex = isP ? 1 : (isM ? 2 : 3); // use L requirements for 1-MOH tracks for now, then switch 3 to 4 for these tracks later
	
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
	    if (isChargino) cout << "Chargino failed reco veto, " << t.track_recoveto[i_trk] << endl;
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
	  if (isChargino) cout << "Chargino failed isolation" << endl;
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
	  if (isChargino) cout << "Chargino failed quality" << endl;
	  continue;
	}

	// Full Short Track
	//	isST = PassesFullIsoSel && isQualityTrack && !is1L;
	//	isST1 = PassesFullIsoSel && isQualityTrack && is1L;
	isST = PassesFullIsoSel && isQualityTrack;

	// Candidate (loosened isolation, quality)...already checked for Iso and Quality above
	//	isSTC = !isST && !is1L;
	//	isSTC1 = !isST && is1L;
	isSTC = !isST;

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
	// more else ifs for 1-MOH tracks
      }

      //      if (!isST && !isSTC && !isST1 && !isSTC1) {
      if (!isST && !isSTC) {
	if (isChargino) {
	  cout << "Chargino failed selection" << endl;
	}
	continue;
      }

      if (isChargino) {
	if (isST) {
	  h_sigeffST->Fill( t.track_decayXY[i_trk], weight);
	}
	else if (isSTC) {
	  h_sigeffSTC->Fill( t.track_decayXY[i_trk], weight);
	}
      }

      vector<string> matches = {"_"};
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
	if (genPromptE)      matches.push_back("_ewke_");
	else if (genPromptM) matches.push_back("_ewkm_");
	else if (genTauE)    matches.push_back("_tewke_");
	else if (genTauM)    matches.push_back("_tewkm_");
	else if (genTau1)    matches.push_back("_1newk_");
	else if (genTau3)    matches.push_back("_3newk_");
	else                 matches.push_back("_newk_");
      }

      // One final cut for L tracks to catch lost leptons
      // These tracks are probably background, likely a lost muon from W+jets
      // in this case, just fill the mtpt hists and continue
      const float mt = MT( t.track_pt[i_trk], t.track_phi[i_trk], t.met_pt, t.met_phi );
      bool rejected = (t.track_pt[i_trk] < 150 && mt < 100);

      vector<int> fillIndices; vector<string> lengths;
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
	  lengths.push_back("L");
	} else {
	  fillIndices.push_back(6);
	  lengths.push_back("Lrej");
	}
      }
      
      /*
      if (isST && lenIndex == 3 && t.mt2 > 100 && t.mt2 < 200 && t.nJet30 > 3 && t.ht < 1000) {
	cout << "L HL VR event: " << t.run << " && " << t.lumi << " && " << t.evt << endl;
      }
      */

      const float dphiMet = DeltaPhi(t.track_phi[i_trk],t.met_phi);
      
      // Fills
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
      if (fillDiagnosticHists) {
	string category = isST ? "ST" : "STC";
	for (vector<string>::iterator length = lengths.begin(); length != lengths.end(); length++) {
	  for (vector<string>::iterator match = matches.begin(); match != matches.end(); match++) {
	    for (vector<string>::iterator njht = njhtToFill.begin(); njht != njhtToFill.end(); njht++) {
	      for (vector<string>::iterator reg = regsToFill.begin(); reg != regsToFill.end(); reg++) {
		string suffix = (*reg)+(*match)+category+"_"+(*length)+(*njht);
		// MtPt
		string hname = "h_mtpt_"+suffix;
		mtptHists[hname]->Fill(mt,t.track_pt[i_trk],weight);
		// EtaPhi
		hname = "h_etaphi_"+suffix;
		etaphiHists[hname]->Fill(t.track_eta[i_trk],t.track_phi[i_trk],weight);
		// Hit Pattern
		hname = "h_hp_"+suffix;
		cutHists[hname]->Fill(t.track_HitSignature[i_trk] & ((1<<7)-1),weight); // Mask so we save only first 6 hits
		// Dz
		hname = "h_dz_"+suffix;
		cutHists[hname]->Fill(fabs(t.track_dz[i_trk]),weight);
		// Dxy
		hname = "h_dxy_"+suffix;
		cutHists[hname]->Fill(fabs(t.track_dxy[i_trk]),weight);
		// N vertex
		hname = "h_nv_"+suffix;
		cutHists[hname]->Fill(t.nVert,weight);
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
		// Eta
		hname = "h_eta_"+suffix;
		cutHists[hname]->Fill(fabs(t.track_eta[i_trk]),weight);
		// Pt
		hname = "h_pt_"+suffix;
		cutHists[hname]->Fill(t.track_pt[i_trk],weight);
		// MT2
		hname = "h_mt2_"+suffix;
		cutHists[hname]->Fill(t.mt2,weight);
		// dphi Met
		hname = "h_dphiMet_"+suffix;
		cutHists[hname]->Fill(dphiMet,weight);
		// Ntag
		hname = "h_nb_"+suffix;
		cutHists[hname]->Fill(t.nBJet20,weight);
	      }
	    }
	  }
	}
      }
    } // Track loop
    
  }//end loop on events in a file
    
  // Post-processing

  // Chargino efficiency division
  h_sigeffST->Divide(h_CharLength);
  h_sigeffSTC->Divide(h_CharLength);

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
  if (fillDiagnosticHists) {
    for (unordered_map<string,TH2D*>::iterator hist = etaphiHists.begin(); hist != etaphiHists.end(); hist++) (hist->second)->Write();
    for (unordered_map<string,TH1D*>::iterator hist = cutHists.begin(); hist != cutHists.end(); hist++) (hist->second)->Write();
  }

  h_sigeffST->Write();
  h_sigeffSTC->Write();
  h_CharLength->Write();

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
