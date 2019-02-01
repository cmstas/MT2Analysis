#include "ShortTrackTagAndProbeLooper.h"

using namespace std;
using namespace duplicate_removal;

class MHtree;

// turn on to apply json file to data
const bool applyJSON = true;

int ShortTrackTagAndProbeLooper::loop (TChain* ch, char * outtag, char* config_tag) {
  string tag(outtag);

  // Book histograms
  TH1::SetDefaultSumw2(true); // Makes histograms do proper error calculation automatically

  TH2D* etaphi_all_mu = new TH2D("etaphi_all_mu","#eta and #phi of Isotracks",500,-2.4,2.4,500,-TMath::Pi(),TMath::Pi());
  TH2D* etaphi_nonPF_mu = new TH2D("etaphi_nonPF_mu","#eta and #phi of non-PF Cand, non-Lepton; Isotracks",500,-2.4,2.4,500,-TMath::Pi(),TMath::Pi());
  TH2D* etaphi_STC_mu = new TH2D("etaphi_STC_mu","#eta and #phi of STCs",500,-2.4,2.4,500,-TMath::Pi(),TMath::Pi());
  TH2D* etaphi_ST_mu = new TH2D("etaphi_ST_mu","#eta and #phi of STs",500,-2.4,2.4,500,-TMath::Pi(),TMath::Pi());

  TH2D* etaphi_all_el = new TH2D("etaphi_all_el","#eta and #phi of Isotracks",500,-2.4,2.4,500,-TMath::Pi(),TMath::Pi());
  TH2D* etaphi_nonPF_el = new TH2D("etaphi_nonPF_el","#eta and #phi of non-PF Cand, non-Lepton Isotracks",500,-2.4,2.4,500,-TMath::Pi(),TMath::Pi());
  TH2D* etaphi_STC_el = new TH2D("etaphi_STC_el","#eta and #phi of STCs",500,-2.4,2.4,500,-TMath::Pi(),TMath::Pi());
  TH2D* etaphi_ST_el = new TH2D("etaphi_ST_el","#eta and #phi of STs",500,-2.4,2.4,500,-TMath::Pi(),TMath::Pi());
			    
  TH1D* mLT_all_mu = new TH1D("mLT_all_mu","Masses of PF Muon-Isotrack Systems;m_{#muTrack} (GeV);Count",40,50,130);
  TH1D* mLT_nonPF_mu = new TH1D("mLT_nonPF_mu","Masses of PF Muon-Isotrack Systems when the Isotrack is not a PF Cand or Reco Muon;m_{#muTrack};Count",40,50,130);
  TH1D* mLT_STC_mu = new TH1D("mLT_STC_mu","Masses of PF Muon-Isotrack Systems when the Isotrack is a STC;m_{#muTrack};Count",40,50,130);
  TH1D* mLT_ST_mu = new TH1D("mLT_ST_mu","Masses of PF Muon-Isotrack Systems when the Isotrack is a ST;m_{#muTrack};Count",40,50,130);

  TH1D* mLT_all_el = new TH1D("mLT_all_el","Masses of PF Electron-Isotrack Systems;m_{#elTrack} (GeV);Count",40,50,130);
  TH1D* mLT_nonPF_el = new TH1D("mLT_nonPF_el","Masses of PF Electron-Isotrack Systems when the Isotrack is not a PF Cand or Reco Electron;m_{#elTrack};Count",40,50,130);
  TH1D* mLT_STC_el = new TH1D("mLT_STC_el","Masses of PF Electron-Isotrack Systems when the Isotrack is a STC;m_{#elTrack};Count",40,50,130);
  TH1D* mLT_ST_el = new TH1D("mLT_ST_el","Masses of PF Electron-Isotrack Systems when the Isotrack is a ST;m_{#elTrack};Count",40,50,130);

  TH1D* why_STC_mu = new TH1D("why_STC_mu","ST Selections Failed (Muon Pair)",256,0,256);
  TH1D* why_STC_el = new TH1D("why_STC_el","ST Selections Failed (Electron Pair)",256,0,256);

  TH1D* mu_flow = new TH1D("mu_flow","Selection Counts",6,0,6);
  mu_flow->GetXaxis()->SetBinLabel(1,"Seeking Pair for Muon");
  mu_flow->GetXaxis()->SetBinLabel(2,"Not Electron or Neutral");
  mu_flow->GetXaxis()->SetBinLabel(3,"Opposite Charge");
  mu_flow->GetXaxis()->SetBinLabel(4,"Not PF Cand or Reco Lep");
  mu_flow->GetXaxis()->SetBinLabel(5,"2+ MOH");
  mu_flow->GetXaxis()->SetBinLabel(6,"Good E/HCAL");
  TH1D* el_flow = new TH1D("el_flow","Selection Counts",6,0,6);
  el_flow->GetXaxis()->SetBinLabel(1,"Seeking Pair for Electron");
  el_flow->GetXaxis()->SetBinLabel(2,"Not Muon or Neutral");
  el_flow->GetXaxis()->SetBinLabel(3,"Opposite Charge");
  el_flow->GetXaxis()->SetBinLabel(4,"Not PF Cand or Reco Lep");
  el_flow->GetXaxis()->SetBinLabel(5,"2+ MOH");
  el_flow->GetXaxis()->SetBinLabel(6,"Good E/HCAL");

  TH1D* h_moh = new TH1D("h_moh","MOHs in ST(C)-like Tracks",6,0,6);

  MHtree t;
  t.Init(ch);

  // Load the configuration and output to screen
  config_ = GetMT2Config(string(config_tag));
  cout << "[MT2Looper::loop] using configuration tag: " << config_tag << endl;
  cout << "                  JSON: " << config_.json << endl;
  cout << "                  lumi: " << config_.lumi << " fb-1" << endl;


  if (applyJSON && config_.json != "") {
    cout << "[ShortTrackTagAndProbeLooper::loop] Loading json file: " << config_.json << endl;
    set_goodrun_file(("../babymaker/jsons/"+config_.json).c_str());
  }

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
    
    // Triggers
    /*    const bool passSingleLepTrigger = t.HLT_SingleMu || t.HLT_SingleEl;
    const bool passPrescaleTrigger = t.HLT_PFHT125_Prescale || t.HLT_PFHT200_Prescale || t.HLT_PFHT300_Prescale || t.HLT_PFHT350_Prescale || t.HLT_PFHT475_Prescale || t.HLT_PFHT600_Prescale;
    const bool passUnPrescaleTrigger = t.HLT_PFHT900 || t.HLT_PFJet450;
    const bool passMetTrigger = t.HLT_PFHT300_PFMET110 || t.HLT_PFMET120_PFMHT120 || t.HLT_PFMETNoMu120_PFMHTNoMu120;
    if (! (passSingleLepTrigger || passPrescaleTrigger || passUnPrescaleTrigger || passMetTrigger) ) {
      continue;
    }

    const float lumi = config_.lumi;
    double weight = t.isData ? 1.0 : t.evt_scale1fb * lumi;
    // if these are true, assume that data trigger is prescaled JetHT trigger
    if (!t.isData && (passPrescaleTrigger && !(passUnPrescaleTrigger || passMetTrigger)) ) {
      if (t.HLT_PFHT475_Prescale) weight *= 1.0 / 115.2;
      else if (t.HLT_PFHT350_Prescale) weight *= 1.0 / 460.6;
      else weight *= 1.0 / 9200.0; // t.HLT_PFHT125_Prescale
    }
    */
   
    // Analysis code

    const int ntracks = t.ntracks;
    for (int i_trk = 0; i_trk < ntracks; i_trk++) {         
      if (!t.track_isPFCand[i_trk] && !t.track_isElectron[i_trk] && !t.track_isMuon[i_trk]) h_moh->Fill(t.track_missingOuterHits[i_trk] > 5 ? 5 : t.track_missingOuterHits[i_trk]);
      if (fabs(t.track_eta[i_trk]) > 2.4) continue;
      // only looking at PF candidates as our tag tracks
      if (!t.track_isPFCand[i_trk]) continue;
      // First look for PF muons. 
      if (t.track_isMuon[i_trk]) {
	for (int j_trk = 0; j_trk < ntracks; j_trk++) {         
	  mu_flow->Fill(0);
	  if (i_trk == j_trk) continue; // Don't pair the muon with itself
	  if (t.track_charge[j_trk] == 0) continue; // Only want charged particles
	  if (t.track_isElectron[j_trk]) continue; // SF
	  if (fabs(t.track_eta[j_trk]) > 2.4) continue; // Restrict to tracker acceptance

	  mu_flow->Fill(1);

	  // Check for Z consistency
	  
	  // Opposite charge
	  if ( t.track_charge[j_trk] * t.track_charge[i_trk] > 0.0) {
	    continue;
	  }
	  
	  mu_flow->Fill(2);

	  // Close to Z mass
	  TLorentzVector l_lep(0,0,0,0);
	  TLorentzVector l_trk(0,0,0,0);
	  l_lep.SetPtEtaPhiM(t.track_pt[i_trk],t.track_eta[i_trk],t.track_phi[i_trk],t.track_mass[i_trk]);
	  l_trk.SetPtEtaPhiM(t.track_pt[j_trk],t.track_eta[j_trk],t.track_phi[j_trk],t.track_mass[j_trk]);
	  TLorentzVector LepTrk = l_lep + l_trk;
	  
	  bool Zlike = fabs(LepTrk.M() - 91.19) < 12;

	  mLT_all_mu->Fill(LepTrk.M());
	  if (Zlike) etaphi_all_mu->Fill( t.track_eta[j_trk], t.track_phi[j_trk] );
	  if (!t.track_isPFCand[j_trk] && !t.track_isMuon[j_trk] && !t.track_isElectron[j_trk]) {
	    mu_flow->Fill(3);
	    if (t.track_missingOuterHits[j_trk] < 2) continue;
	    mu_flow->Fill(4);
	    why_STC_mu->Fill(t.track_iscand[j_trk]);
	    mLT_nonPF_mu->Fill(LepTrk.M());
	    if (Zlike) etaphi_nonPF_mu->Fill( t.track_eta[j_trk], t.track_phi[j_trk] );
	    if (t.track_DeadECAL[j_trk] || t.track_DeadHCAL[j_trk]) continue;
	    mu_flow->Fill(5);
	    if (t.track_iscand[j_trk] != 0) {
	      mLT_STC_mu->Fill(LepTrk.M());
	      if (Zlike) etaphi_STC_mu->Fill( t.track_eta[j_trk], t.track_phi[j_trk] );
	    }
	    else {
	      mLT_ST_mu->Fill(LepTrk.M());
	      if (Zlike) etaphi_ST_mu->Fill( t.track_eta[j_trk], t.track_phi[j_trk] );
	    }
	  }
	}      
	
      }      
      // Now look for PF electrons. 
      if (t.track_isElectron[i_trk]) {
	for (int j_trk = 0; j_trk < ntracks; j_trk++) {         
	  el_flow->Fill(0);
	  if (i_trk == j_trk) continue; // Don't pair the electron with itself
	  if (t.track_charge[j_trk] == 0) continue; // Only want charged particles
	  if (t.track_isMuon[j_trk]) continue; // SF
	  if (fabs(t.track_eta[j_trk]) > 2.4) continue; // Restrict to tracker acceptance

	  el_flow->Fill(1);

	  // Check for Z consistency
	  
	  // Opposite charge
	  if ( t.track_charge[j_trk] * t.track_charge[i_trk] > 0.0) {
	    continue;
	  }
	  
	  el_flow->Fill(2);

	  // Close to Z mass
	  TLorentzVector l_lep(0,0,0,0);
	  TLorentzVector l_trk(0,0,0,0);
	  l_lep.SetPtEtaPhiM(t.track_pt[i_trk],t.track_eta[i_trk],t.track_phi[i_trk],t.track_mass[i_trk]);
	  l_trk.SetPtEtaPhiM(t.track_pt[j_trk],t.track_eta[j_trk],t.track_phi[j_trk],t.track_mass[j_trk]);
	  TLorentzVector LepTrk = l_lep + l_trk;
	  
	  bool Zlike = fabs(LepTrk.M() - 91.19) < 12;

	  mLT_all_el->Fill(LepTrk.M());
	  if (Zlike) etaphi_all_el->Fill( t.track_eta[j_trk], t.track_phi[j_trk] );
	  if (!t.track_isPFCand[j_trk] && !t.track_isMuon[j_trk] && !t.track_isElectron[j_trk]) {
	    el_flow->Fill(3);
	    if (t.track_missingOuterHits[j_trk] < 2) continue;
	    el_flow->Fill(4);
	    why_STC_el->Fill(t.track_iscand[j_trk]);
	    mLT_nonPF_el->Fill(LepTrk.M());
	    if (Zlike) etaphi_nonPF_el->Fill( t.track_eta[j_trk], t.track_phi[j_trk] );
	    if (t.track_DeadECAL[j_trk] || t.track_DeadHCAL[j_trk]) continue;
	    el_flow->Fill(5);
	    if (t.track_iscand[j_trk] != 0) {
	      mLT_STC_el->Fill(LepTrk.M());
	      if (Zlike) etaphi_STC_el->Fill( t.track_eta[j_trk], t.track_phi[j_trk] );
	    }
	    else {
	      mLT_ST_el->Fill(LepTrk.M());
	      if (Zlike) etaphi_ST_el->Fill( t.track_eta[j_trk], t.track_phi[j_trk] );
	    }
	  }
	}      
	
      }      

    }

  }//end loop on events in a file

  // Post-processing

  TH2D* ratio_mu = (TH2D*) etaphi_nonPF_mu->Clone("nonPF_over_all_mu");
  ratio_mu->SetTitle("Non-PF, non-Muon / All Isotracks, of Tracks Consistent with Muon from Z;#eta;#phi");
  ratio_mu->Divide(etaphi_all_mu);

  TH2D* ratio_el = (TH2D*) etaphi_nonPF_el->Clone("nonPF_over_all_el");
  ratio_el->SetTitle("Non-PF, non-Muon / All Isotracks, of Tracks Consistent with Electron from Z;#eta;#phi");
  ratio_el->Divide(etaphi_all_el);

  cout << "Processed " << nEventsTree << " events." << endl;
  cout << nDuplicates << " duplicates skipped." << endl;


  cout << "About to write" << endl;

  TFile outfile_(Form("%s.root",outtag),"RECREATE"); 
  outfile_.cd();
  etaphi_all_mu->Write();
  etaphi_nonPF_mu->Write();
  etaphi_STC_mu->Write();
  etaphi_ST_mu->Write();
  etaphi_all_el->Write();
  etaphi_nonPF_el->Write();
  etaphi_STC_el->Write();
  etaphi_ST_el->Write();
  mLT_all_mu->Write();
  mLT_nonPF_mu->Write();
  mLT_STC_mu->Write();
  mLT_ST_mu->Write();
  mLT_all_el->Write();
  mLT_nonPF_el->Write();
  mLT_STC_el->Write();
  mLT_ST_el->Write();
  ratio_mu->Write();
  ratio_el->Write();
  why_STC_mu->Write();
  why_STC_el->Write();
  el_flow->Write();
  mu_flow->Write();
  h_moh->Write();
  outfile_.Close();
  cout << "Wrote everything" << endl;

  return 0;
}

int main (int argc, char ** argv) {

  if (argc < 5) {
    cout << "Usage: ./ShortTrackTagAndProbeLooper.exe <outdir> <indir> <sample> <config>" << endl;
    return 1;
  }

  TChain *ch = new TChain("stp");
  TString input_string = Form("%s/*%s*.root",argv[2],argv[3]);
  cout << input_string.Data() << endl;
  ch->Add(input_string);
  cout << ch->GetEntries() << endl;

  ShortTrackTagAndProbeLooper * tpl = new ShortTrackTagAndProbeLooper();
  tpl->loop(ch,argv[1],argv[4]);
  return 0;
}

ShortTrackTagAndProbeLooper::ShortTrackTagAndProbeLooper() {}

ShortTrackTagAndProbeLooper::~ShortTrackTagAndProbeLooper() {};
