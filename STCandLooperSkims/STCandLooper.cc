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

int STCandLooper::loop (char * indir, char * sample, int selectionMode, int vetoMode) {

  const bool doHT = selectionMode == 1;
  const bool doCRSL = selectionMode == 2;
  const bool doSR = selectionMode == 3;

  const bool doGenVeto = vetoMode == 1 || vetoMode < 0;
  const bool doEtaPhiVeto = vetoMode == 2 || vetoMode < 0;

  string output_suffix("");
  if (doHT) output_suffix = "_HT";
  else if (doCRSL) output_suffix = "_CRSL";
  else if (doSR) output_suffix = "_SR";
    
  if (doGenVeto) output_suffix += "_GV";
  if (doEtaPhiVeto) output_suffix += "_EPV";

  string mkdir = "mkdir -p output" + output_suffix;
  system(mkdir.c_str());
  TString output_name = Form("output%s/%s.root",output_suffix.c_str(),sample);

  // Containers for our trees
  TChain* ch_mt2 = new TChain("mt2st");
  TChain* ch_st = new TChain("st");

  cout << "Input directory: " << indir << endl;

  TString filestring = Form("%s/%s/*.root",indir,sample);

  cout << filestring.Data() << endl;
  
  ch_mt2->Add( filestring ); // Load mt2 inputs
  ch_st->Add( filestring ); // Load short track inputs

  // sttree is a subtype of mt2tree. mt2tree functions can be called on an sttree, but not the reverse.
  // This line incorporates branches from ch_mt2 into ch_st.
  ch_st->AddFriend(ch_mt2);

  sttree t; // See MT2CORE/sttree.h

  // Our baby tree is an sttree so that we can call both mt2 and st methods mindlessly.
  t.Init(ch_st); // The Init function from sttree calls the Init function from mt2tree as well.

  // Event Loop
  // Sanity check:
  const unsigned int nEventsTree = ch_mt2->GetEntries();
  if (nEventsTree != ch_st->GetEntries()) {
    cout << "st and mt2 trees have different numbers of entries. This should not be possible if the input file was produced by treefriend.py and never edited. Aborting..." << endl;
    return 2;
  }

  // For cands
  const float pt_min = pt_min_st;
  const float dxy_max = 3* dxy_max_st;
  const float dz_max = 3* dz_max_st;
  const float iso_max = 3* iso_max_st;
  const float reliso_max = 3* reliso_max_st;
  const float niso_max = 3* niso_max_st;
  const float chiso_max = 3* chiso_max_st;
  const float nreliso_max = 3* nreliso_max_st;
  const float chreliso_max = 3* chreliso_max_st;
  const float pterr_max = 3 * pterr_max_st;
  const float pterr_max_s = 3 * pterr_max_st_s;
  const float pterr_max_l = 3 *pterr_max_st_l;
  const float genlepDR_min = genlepDR_min_st;
  const float recolepDR_min = recolepDR_min_st;

  const int pt_n = 30;  const float pt_max = 80;
  const int dxy_n = 40; const float dxy_min = 0;
  const int dz_n = 30; const float dz_min = 0; 
  const int iso_n = 30; const float iso_min = 0;
  const int niso_n = 30; const float niso_min = 0;
  const int chiso_n = 20; const float chiso_min = 0;
  const int reliso_n = 30; const float reliso_min = 0;
  const int chreliso_n = 30; const float chreliso_min = 0;
  const int nreliso_n = 30; const float nreliso_min = 0;
  const int pterr_n = 20; const float pterr_min = 0;
  const int genlepDR_n = 10; const float genlepDR_max = 1;
  const int recolepDR_n = 10; const float recolepDR_max = 1;

  map<TString,tuple<int,int,int> > vars;
  vars["pt"] = make_tuple(pt_n,pt_min,pt_max);
  vars["bardxy"] = make_tuple(dxy_n,dxy_min,dxy_max);
  vars["bardz"] = make_tuple(dz_n,dz_min,dz_max);
  vars["ecdxy"] = make_tuple(dxy_n,dxy_min,dxy_max);
  vars["ecdz"] = make_tuple(dz_n,dz_min,dz_max);
  vars["iso"] = make_tuple(iso_n,iso_min,iso_max);
  vars["niso"] = make_tuple(niso_n,niso_min,niso_max);
  vars["chiso"] = make_tuple(chiso_n,chiso_min,chiso_max);
  vars["reliso"] = make_tuple(reliso_n,reliso_min,reliso_max);
  vars["chreliso"] = make_tuple(chreliso_n,chreliso_min,chreliso_max);
  vars["nreliso"] = make_tuple(nreliso_n,nreliso_min,nreliso_max);
  vars["N"] = make_tuple(4,0,4);
  vars["mu_genDR"] = make_tuple(genlepDR_n,0,genlepDR_max);
  vars["mu_ptr"] = make_tuple(10,-1,1);
  vars["el_genDR"] = make_tuple(genlepDR_n,0,genlepDR_max);
  vars["el_ptr"] = make_tuple(10,-1,1);
  vars["tau1_genDR"] = make_tuple(genlepDR_n,0,genlepDR_max);
  vars["tau1_ptr"] = make_tuple(10,-1,1);
  vars["tau3_genDR"] = make_tuple(genlepDR_n,0,genlepDR_max);
  vars["tau3_ptr"] = make_tuple(10,-1,1);
  vars["tauel_genDR"] = make_tuple(genlepDR_n,0,genlepDR_max);
  vars["tauel_ptr"] = make_tuple(10,-1,1);
  vars["taumu_genDR"] = make_tuple(genlepDR_n,0,genlepDR_max);
  vars["taumu_ptr"] = make_tuple(10,-1,1);
  vars["eta"] = make_tuple(6,0,2.4);
  vars["pterr"] = make_tuple(pterr_n,pterr_min,pterr_max);
  vars["purity"] = make_tuple(2,0,2);
  vars["el_recoDR"] = make_tuple(recolepDR_n,0,recolepDR_max);
  vars["mu_recoDR"] = make_tuple(recolepDR_n,0,recolepDR_max);
  // Book Histograms
  TH1::SetDefaultSumw2(true); // Makes histograms do proper error calculation automatically
  map<TString,TH1*> hists;
  // Add count hists, split by ht or length
  const vector<TString> htsplit_vector = {"I","L","M","H"};
  const vector<TString> lensplit_vector = {"i","p","s","l"};
  const vector<TString> sh_vector = {"st","stc"};
  for (const auto& htsplit : htsplit_vector) {
    for (const auto& lensplit : lensplit_vector) {
      for (const auto& sh : sh_vector) {
	for (const auto& [var,limit_tuple] : vars) {
	  TString name = sh+"_"+htsplit+"_"+lensplit+"_"+var;
	  const auto& [n,min,max] = limit_tuple;
	  hists[name] = new TH1F("h_"+name,Form("%s %s %s %s",sh.Data(), var.Data(), htsplit.Data(), lensplit.Data()),n,min,max);
	}
      }
    }
  }
  // none, e, mu, taue, taumu, tau1, tau3, 
  // ee, emu, etaue, etaumu, etau1, etau3, 
  // mumu, mutaue, mutaumu, mutau1, mutau3, 
  // tauetaue, tauetaumu, tauetau1, tauetau3, 
  // taumutaumu, taumutau1, taumutau3, 
  // tau1tau1, tau1tau3, 
  // tau3tau3
  hists["nem"] = new TH1F("h_nem","NEM JEFs in Events with Gen Electrons",20,0,1);
  hists["chem"] = new TH1F("h_chem","ChEM JEFs in Events with Gen Electrons",20,0,1);
  hists["em"] = new TH1F("h_em","EM JEFs in Events with Gen Electrons",20,0,1);
  hists["lepType"] = new TH1F("h_lepType","Lepton Type",28,0,28);
  hists["lepTypePS"] = new TH1F("h_lepTypePS","Lepton Type (Pre_ST)",28,0,28);
  hists["lep1DR"] = new TH1F("h_lep1DR","Electron 1 #DeltaR with ST",20,0,2);
  hists["lep2DR"] = new TH1F("h_lep2DR","Electron 2 #DeltaR with ST",20,0,2);
  hists["lep1ptr"] = new TH1F("h_lep1ptr","Electron 1 pt Ratio with ST",20,0,2);
  hists["lep2ptr"] = new TH1F("h_lep2ptr","Electron 2 pt Ratio with ST",20,0,2);
  hists["ngenLep"] = new TH1F ("h_ngenLep","ngenLep in N_{STC} > 0 Events",6,0,6);

  const float pi = TMath::Pi();
  hists["etaphi_bar"] = new TH2F("h_etaphi_bar","All Tracks by #eta and #phi",170,-1.4,1.4,360,-pi,pi);
  hists["st_etaphi_bar"] = new TH2F ("h_st_etaphi_bar","Long Short Tracks by #eta and #phi",170,-1.4,1.4,360,-pi,pi);
  hists["etaphi_ecn"] = new TH2F("h_etaphi_ecn","All Tracks by #eta and #phi",100,-2.4,-1.4,100,-pi,pi);
  hists["st_etaphi_ecn"] = new TH2F("h_st_etaphi_ecn","Long Short Tracks by #eta and #phi",100,-2.4,-1.4,100,-pi,pi);
  hists["etaphi_ecp"] = new TH2F("h_etaphi_ecp","All Tracks by #eta and #phi",100,1.4,2.4,100,-pi,pi);
  hists["st_etaphi_ecp"] = new TH2F("h_st_etaphi_ecp","Long Short Tracks by #eta and #phi",100,1.4,2.4,100,-pi,pi);

  hists["inclusive"] = new TH1F("h_inclusive","Total Events in Sample",1,0,1);

  // Apply region selections
  /*  if (doCRSL) {
    hists["inclusive"]->Fill(0.5,ch_mt2->GetEntries("nLepLowMT == 1 && Sum$(jet_pt) > 100"));
  } else if (doHT) {
    hists["inclusive"]->Fill(0.5,ch_mt2->GetEntries("Sum$(jet_pt) > 100"));
  } else if (doSR) {
    hists["inclusive"]->Fill(0.5,ch_mt2->GetEntries("nPFLep5LowMT + nMuons10 + nElectrons10 + nPFHad10LowMT == 0 && Sum$(jet_pt) > 100"));
  }  else {
    hists["inclusive"]->Fill(0.5,nEventsTree);
    }*/


  int nselected = 0;
  for( unsigned int event = 0; event < nEventsTree; ++event) {    
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

    nselected++;

    TString HTsplit = "";
    if (sumJet < 175) HTsplit = "L";
    else if (sumJet < 250) HTsplit = "M";
    else HTsplit = "H";

    /*
     * Track Loop
     */
    const int ntracks = t.ntracks;
    int nstc = 0; int nstcp = 0; int nstcs = 0; int nstcl = 0;
    int nst = 0; int nstp = 0; int nsts = 0; int nstl = 0;
    int st_idx = -1;
    for (int i_trk = 0; i_trk < ntracks; i_trk++) {   

      // Veto tracks outside fiducial region
      if (doEtaPhiVeto && InEtaPhiVetoRegion(t.track_eta[i_trk],t.track_phi[i_trk]) != 0) continue;
      // Veto if not high purity
      if (t.track_highPurity[i_trk] != 1) continue;

      map <TString,float> fillSTC;
      map <TString,float> fillST;
      for (const auto& KeyValue : vars) {
	fillSTC[KeyValue.first] = -99;
	fillST[KeyValue.first] = -99;
      }

      // Check reco leps
      float minDR = 100;
      float recolepDR = 100;
      int recominDRID = -1;
      float recolepPTR = 100;
      int minIndex = -1;
      for (int i_lep = 0; i_lep < t.nlep; i_lep++) {
	float DR = DeltaR(t.lep_eta[i_lep],t.track_eta[i_trk],t.lep_phi[i_trk],t.track_phi[i_trk]);
	if (DR < minDR) {
	    minDR = DR;
	    minIndex = i_lep;
	  }
      }
      if (minIndex >= 0) {
	recominDRID = abs(t.lep_pdgId[minIndex]);
	recolepPTR = t.lep_pt[minIndex] / t.track_pt[i_trk] - 1;
      }
      minIndex = -1;
      for (int i_isoT = 0; i_isoT < t.nlep; i_isoT++) {
	int pdgId = abs(t.isoTrack_pdgId[i_isoT]);
	if (pdgId != 11 && pdgId != 13) continue;
	float DR = DeltaR(t.isoTrack_eta[i_isoT],t.track_eta[i_trk],t.isoTrack_phi[i_isoT],t.track_phi[i_trk]);
	if (DR < minDR) {
	  minDR = DR;
	  minIndex = i_isoT;
	}     
      }
      recolepDR = minDR;
      if (minIndex >= 0) {	
	recominDRID = abs(t.isoTrack_pdgId[minIndex]);
	recolepPTR = t.isoTrack_pt[minIndex] / t.track_pt[i_trk] - 1;
      }
      TString RecoLepCat = recominDRID == 11 ? "el" : "mu";

      // Find nearest gen lepton
      float genlepDR = 100;
      float genlepPTR = -1;
      int genlepID = -1;

      minDR = 100;
      minIndex = -1;
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
	genlepPTR = t.genLep_pt[minIndex] / t.track_pt[i_trk] - 1;
	genlepDR = minDR;
	genlepID = abs(t.genLep_pdgId[minIndex]);
      } 
      
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
      if (minDR < genlepDR) {
	genlepDR = minDR;
	genlepID = abs(t.genLepFromTau_pdgId[minIndex]) + 1500;
	genlepPTR = t.genLepFromTau_pt[minIndex] / t.track_pt[i_trk] - 1;
      }
      
      // Find nearest gen tau
      minDR = 100;
      minIndex = -1;
      for (int i_tau = 0; i_tau < t.ngenTau; i_tau++) {
	// Decay Mode == 1 means 1 prong
	// 2 means 3 prong
	// Else, pdgId of lepton
	int decayMode = abs(t.genTau_decayMode[i_tau]);
	if (decayMode > 3) continue; // Count leptons separately
	float tau_eta = t.genTau_eta[i_tau];
	float tau_phi = t.genTau_phi[i_tau];
	float dr = DeltaR(tau_eta, t.track_eta[i_trk], tau_phi, t.track_phi[i_trk]);
	if (dr < minDR) {
	  minDR = dr;
	  minIndex = i_tau;
	}
      }
      if (minDR < genlepDR) {
	genlepDR = minDR;
	genlepID = t.genTau_decayMode[minIndex];
	genlepPTR = t.genTau_pt[minIndex] / t.track_pt[i_trk] - 1;;
      }
      
      if (genlepDR < 0.1) { // Use Gen Veto to find short track hotspots
	if (fabs(t.track_eta[i_trk]) < 1.4) {
	  hists["etaphi_bar"]->Fill(t.track_eta[i_trk],t.track_phi[i_trk]);
	  if (t.track_isshort[i_trk]) {
	    hists["st_etaphi_bar"]->Fill(t.track_eta[i_trk],t.track_phi[i_trk]);
	  } 
	} else if (t.track_eta[i_trk] > 1.4) {
	  hists["etaphi_ecp"]->Fill(t.track_eta[i_trk],t.track_phi[i_trk]);
	  if (t.track_isshort[i_trk]) {
	    hists["st_etaphi_ecp"]->Fill(t.track_eta[i_trk],t.track_phi[i_trk]);
	  } 
	} else if (t.track_eta[i_trk] < -1.4) {
	  hists["etaphi_ecn"]->Fill(t.track_eta[i_trk],t.track_phi[i_trk]);
	  if (t.track_isshort[i_trk]) {
	    hists["st_etaphi_ecn"]->Fill(t.track_eta[i_trk],t.track_phi[i_trk]);
	  } 
	}
      } // End etaphi fills if gen lepton nearby

      TString GenLepCat = "";
      if (genlepID == 11) GenLepCat = "el";
      else if (genlepID == 13) GenLepCat = "mu";
      else if (genlepID == 1) GenLepCat = "tau1";
      else if (genlepID == 3) GenLepCat = "tau3";
      else if (genlepID == 1511) GenLepCat = "taue";
      else if (genlepID == 1513) GenLepCat = "taumu";

      // Also check genStat23 for DR cases where genLeps not saved due to no Z
      if (t.ngenLep + t.ngenTau == 0) {
	minDR = 100;
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
	// Double cut size to be safe, since no FSR
	if (minDR < 0.2) {
	  continue; // Veto this track, but don't store its leps (no FSR, would be distortive, and no access to tau decay mode; small effect anyway, but don't want it throwing off DY)
	}
      }
      
      const float eta = fabs(t.track_eta[i_trk]);
      
      const float ptErr = t.track_ptErr[i_trk];
      const float pt = t.track_pt[i_trk];
      const float pt2 = pt*pt;
      const float PtErrOverPt2 = ptErr / pt2;

      const int pixel_layers = t.track_pixelLayersWithMeasurement[i_trk];
      if (pixel_layers < 2) continue; // STCs aren't allowed to be *this* fake
      const int total_layers = t.track_trackerLayersWithMeasurement[i_trk];
      const float dz = fabs(t.track_dz[i_trk]);
      const float dxy = fabs(t.track_dxy[i_trk]);
      const float iso = t.track_iso[i_trk];
      const float niso = t.track_sumNeuH0p05[i_trk] + t.track_sumPh0p05[i_trk];
      const float chiso = t.track_sumChP[i_trk];
      const float reliso = t.track_reliso[i_trk];
      const float nreliso = niso/pt;
      const float chreliso = chiso/pt;
      const int lostOuterHits = t.track_lostOuterHits[i_trk];

      // Loose mutually exclusive length selections suitable for sorting STCs and STs for N-1 plots. (STCs will usually not pass any.)
      bool p = pixel_layers == total_layers;
      bool s = total_layers < 7 && pixel_layers < total_layers && PtErrOverPt2 < pterr_max_s;
      bool l = total_layers >= 7 && lostOuterHits >= 2 && PtErrOverPt2 < pterr_max_l;

      if (genlepDR > genlepDR_min
	  && recolepDR > recolepDR_min
	  && pt > pt_min 
	  && dz < dz_max 
	  && dxy < dxy_max 
	  && iso < iso_max 
	  && niso < niso_max 
	  && chiso < chiso_max 
	  && reliso < reliso_max 
	  && nreliso < nreliso_max 
	  && chreliso < chreliso_max 
	  && PtErrOverPt2 < pterr_max) {	 
	nstc++;
	fillSTC["eta"] = eta; // Always fill eta
	if (p) nstcp++;
	else if (s) nstcs++;
	else if (l) nstcl++;
      }
      
      /*
       * N-1 short track candidate distributions
       */
      // Gen Lep DR
      if (pt > pt_min
	  && recolepDR > recolepDR_min
	  && dz < dz_max 
	  && dxy < dxy_max 
	  && iso < iso_max 
	  && niso < niso_max 
	  && chiso < chiso_max 
	  && reliso < reliso_max 
	  && nreliso < nreliso_max 
	  && chreliso < chreliso_max 
	  && PtErrOverPt2) {
	fillSTC[GenLepCat+"_genDR"] = genlepDR;
	fillSTC[GenLepCat+"_genPTR"] = genlepPTR;
      }
      // Reco Lep DR
      if (genlepDR > genlepDR_min
	  && pt > pt_min
	  && dz < dz_max 
	  && dxy < dxy_max 
	  && iso < iso_max 
	  && niso < niso_max 
	  && chiso < chiso_max 
	  && reliso < reliso_max 
	  && nreliso < nreliso_max 
	  && chreliso < chreliso_max 
	  && PtErrOverPt2) {
	fillSTC[RecoLepCat+"_recoDR"] = recolepDR;
	fillSTC[RecoLepCat+"_recoPTR"] = recolepPTR;
      }
      // pt
      if (genlepDR > genlepDR_min
	  && recolepDR > recolepDR_min
	  && dz < dz_max 
	  && dxy < dxy_max 
	  && iso < iso_max 
	  && niso < niso_max 
	  && chiso < chiso_max 
	  && reliso < reliso_max 
	  && nreliso < nreliso_max 
	  && chreliso < chreliso_max 
	  && PtErrOverPt2) {
	fillSTC["pt"] = pt;
      }
      // dz
      if (genlepDR > genlepDR_min
	  && recolepDR > recolepDR_min
	  && pt > pt_min 
	  && dxy < dxy_max 
	  && iso < iso_max 
	  && niso < niso_max 
	  && chiso < chiso_max 
	  && reliso < reliso_max 
	  && nreliso < nreliso_max 
	  && chreliso < chreliso_max
	  && PtErrOverPt2 < pterr_max) {
	if (eta > 1.4) fillSTC["ecdz"] = dz;
	else fillSTC["bardz"] = dz;
      }
      // dxy
      if (genlepDR > genlepDR_min
	  && recolepDR > recolepDR_min
	  && dz < dz_max 
	  && pt > pt_min 
	  && iso < iso_max 
	  && niso < niso_max 
	  && chiso < chiso_max 
	  && reliso < reliso_max 
	  && nreliso < nreliso_max 
	  && chreliso < chreliso_max
	  && PtErrOverPt2 < pterr_max) {
	if (eta > 1.4) fillSTC["ecdxy"] = dxy;
	else fillSTC["bardxy"] = dxy;
      }
      // Abs Iso 0.3
      if (genlepDR > genlepDR_min
	  && recolepDR > recolepDR_min
	  && dz < dz_max 
	  && dxy < dxy_max 
	  && pt > pt_min 
	  && niso < niso_max 
	  && chiso < chiso_max 
	  && reliso < reliso_max 
	  && nreliso < nreliso_max 
	  && chreliso < chreliso_max
	  && PtErrOverPt2 < pterr_max) {
	fillSTC["iso"] = iso;
      }
      // Neut 0.05 Iso 
      if (genlepDR > genlepDR_min
	  && recolepDR > recolepDR_min
	  && dz < dz_max 
	  && dxy < dxy_max 
	  && iso < iso_max 
	  && pt > pt_min 
	  && chiso < chiso_max 
	  && reliso < reliso_max 
	  && nreliso < nreliso_max 
	  && chreliso < chreliso_max
	  && PtErrOverPt2 < pterr_max) {
	fillSTC["niso"] = niso;
      }
      // ChIso
      if (genlepDR > genlepDR_min
	  && recolepDR > recolepDR_min
	  && dz < dz_max 
	  && dxy < dxy_max 
	  && iso < iso_max 
	  && niso < niso_max 
	  && pt > pt_min 
	  && reliso < reliso_max 
	  && nreliso < nreliso_max 
	  && chreliso < chreliso_max
	  && PtErrOverPt2 < pterr_max) {
	fillSTC["chiso"] = chiso;
      }
      // RelIso 0.3
      if (genlepDR > genlepDR_min
	  && recolepDR > recolepDR_min
	  && dz < dz_max 
	  && dxy < dxy_max 
	  && iso < iso_max 
	  && niso < niso_max 
	  && chiso < chiso_max 
	  && pt > pt_min 
	  && nreliso < nreliso_max 
	  && chreliso < chreliso_max
	  && PtErrOverPt2 < pterr_max) {
	fillSTC["reliso"] = reliso;
      }
      // NeuRelIso 0.05
      if (genlepDR > genlepDR_min
	  && recolepDR > recolepDR_min
	  && dz < dz_max 
	  && dxy < dxy_max 
	  && iso < iso_max 
	  && niso < niso_max 
	  && chiso < chiso_max 
	  && reliso < reliso_max 
	  && pt > pt_min 
	  && chreliso < chreliso_max
	  && PtErrOverPt2 < pterr_max) {
	fillSTC["nreliso"] = nreliso;
      }
      // ChRelIso 0.05
      if (genlepDR > genlepDR_min
	  && recolepDR > recolepDR_min
	  && dz < dz_max 
	  && dxy < dxy_max 
	  && iso < iso_max 
	  && niso < niso_max 
	  && chiso < chiso_max 
	  && reliso < reliso_max 
	  && nreliso < nreliso_max 
	  && pt > pt_min
	  && PtErrOverPt2 < pterr_max) {
	fillSTC["chreliso"] = chreliso;
      }
      // PtErrOverPt2
      if (genlepDR > genlepDR_min
	  && recolepDR > recolepDR_min
	  && dz < dz_max 
	  && dxy < dxy_max 
	  && iso < iso_max 
	  && niso < niso_max 
	  && chiso < chiso_max 
	  && reliso < reliso_max 
	  && nreliso < nreliso_max 
	  && chreliso < chreliso_max 
	  && pt > pt_min) {
	fillSTC["PtErrOverPt2"] = PtErrOverPt2;
      }
      
    // A few selections that apply to all short tracks, and not candidates
    if (t.track_lostInnerHits[i_trk] == 0 && t.track_lostInnerPixHits[i_trk] == 0) {

      // Update length selections. These are for dxy N-1s
      bool p_NoDxyTight = p && PtErrOverPt2 < pterr_max_st;
      bool s_NoDxyTight = s && PtErrOverPt2 < pterr_max_st_s;
      bool l_NoDxyTight = l && PtErrOverPt2 < pterr_max_st_l;
      // Add the dxy tight selections in where appropriate
      bool p_st = p_NoDxyTight;
      bool s_st = s_NoDxyTight && dxy < 0.01;
      bool l_st = l_NoDxyTight && dxy < 0.01;
      // Also need separate selection for PtErrOverPt2
      bool p_NoPtErrTight = p;
      bool s_NoPtErrTight = s && dxy < 0.01;
      bool l_NoPtErrTight = l && dxy < 0.01;

      bool LengthSelectionST = p_st || s_st || l_st;
      bool LengthSelectionST_RelaxedDxy = p_NoDxyTight || s_NoDxyTight || l_NoDxyTight;
      bool LengthSelectionST_RelaxedPtErr = p_NoPtErrTight || s_NoPtErrTight || l_NoPtErrTight;

      if (t.track_isshort[i_trk] ) {
	nst++;
	fillST["eta"] = eta; // Always fill eta
	if (st_idx < 0) st_idx = i_trk; // save hardest short track index
	if (t.track_ispixelonly[i_trk]) {
	  nstp++;
	}
	else if (t.track_istrkshort[i_trk]) {
	  nsts++;
	}
	else {
	  nstl++;
	}
      }

      /*
       * Short track N-1 plots
       */
      // Gen Lep DR
      if (pt > pt_min
	  && recolepDR > recolepDR_min_st
	  && dz < dz_max_st
	  && dxy < dxy_max_st
	  && iso < iso_max_st 
	  && niso < niso_max_st 
	  && chiso < chiso_max_st 
	  && reliso < reliso_max_st 
	  && nreliso < nreliso_max_st 
	  && chreliso < chreliso_max_st 
	  && PtErrOverPt2) {
	fillST[GenLepCat+"_genDR"] = genlepDR;
	fillST[GenLepCat+"_genPTR"] = genlepPTR;
      }
      // Reco Lep DR
      if (genlepDR > genlepDR_min_st
	  && pt > pt_min_st
	  && dz < dz_max_st 
	  && dxy < dxy_max_st 
	  && iso < iso_max_st 
	  && niso < niso_max_st 
	  && chiso < chiso_max_st 
	  && reliso < reliso_max_st 
	  && nreliso < nreliso_max_st 
	  && chreliso < chreliso_max_st 
	  && PtErrOverPt2) {
	fillST[RecoLepCat+"_recoDR"] = recolepDR;
	fillST[RecoLepCat+"_recoPTR"] = recolepPTR;
      }
      // pt
      if (genlepDR > genlepDR_min_st
	  && recolepDR > recolepDR_min_st
	  && dz < dz_max_st 
	  && dxy < dxy_max_st 
	  && iso < iso_max_st 
	  && niso < niso_max_st 
	  && chiso < chiso_max_st 
	  && reliso < reliso_max_st 
	  && nreliso < nreliso_max_st 
	  && chreliso < chreliso_max_st
	  //	  && PtOverPt2 < pterr_max_st // Placeholder, but empty selection since it's already in LengthSelectionST
	  && LengthSelectionST) {
	fillST["pt"] = pt;
      }
      // dz
      if (genlepDR > genlepDR_min_st
	  && recolepDR > recolepDR_min_st
	  && pt > pt_min_st 
	  && dxy < dxy_max_st 
	  && iso < iso_max_st 
	  && niso < niso_max_st 
	  && chiso < chiso_max_st 
	  && reliso < reliso_max_st 
	  && nreliso < nreliso_max_st 
	  && chreliso < chreliso_max_st
	  //	  && PtOverPt2 < pterr_max_st // Placeholder, but empty selection since it's already in LengthSelectionST
	  && LengthSelectionST) {
	if (eta > 1.4) {
	  fillST["ecdz"] = dz;
	} else {
	fillST["bardz"] = dz;
	}
      }
      // dxy
      if (genlepDR > genlepDR_min_st
	  && recolepDR > recolepDR_min_st
	  && dz < dz_max_st
	  && pt > pt_min_st 
	  && iso < iso_max_st 
	  && niso < niso_max_st 
	  && chiso < chiso_max_st 
	  && reliso < reliso_max_st 
	  && nreliso < nreliso_max_st 
	  && chreliso < chreliso_max_st
	  //	  && PtOverPt2 < pterr_max_st // Placeholder, but empty selection since it's already in LengthSelectionST
	  && LengthSelectionST_RelaxedDxy) {
	if (eta > 1.4) {
	  fillST["ecdxy"] = dxy;
	} else {
	  fillST["bardxy"] = dxy;
	}
      }
      // Abs Iso 0.3
      if (genlepDR > genlepDR_min_st
	  && recolepDR > recolepDR_min_st
	  && dz < dz_max_st 
	  && dxy < dxy_max_st 
	  && pt > pt_min_st 
	  && niso < niso_max_st 
	  && chiso < chiso_max_st 
	  && reliso < reliso_max_st 
	  && nreliso < nreliso_max_st 
	  && chreliso < chreliso_max_st
	  //	  && PtOverPt2 < pterr_max_st // Placeholder, but empty selection since it's already in LengthSelectionST
	  && LengthSelectionST) {
	fillST["iso"] = iso;
      }
      // Neut 0.05 Iso 
      if (genlepDR > genlepDR_min_st
	  && recolepDR > recolepDR_min_st
	  && dz < dz_max_st 
	  && dxy < dxy_max_st 
	  && iso < iso_max_st 
	  && pt > pt_min_st 
	  && chiso < chiso_max_st 
	  && reliso < reliso_max_st 
	  && nreliso < nreliso_max_st 
	  && chreliso < chreliso_max_st
	  //	  && PtOverPt2 < pterr_max_st // Placeholder, but empty selection since it's already in LengthSelectionST
	  && LengthSelectionST) {
	fillST["niso"] = niso;
      }
      // ChIso
      if (genlepDR > genlepDR_min_st
	  && recolepDR > recolepDR_min_st
	  && dz < dz_max_st 
	  && dxy < dxy_max_st 
	  && iso < iso_max_st 
	  && niso < niso_max_st 
	  && pt > pt_min_st 
	  && reliso < reliso_max_st 
	  && nreliso < nreliso_max_st 
	  && chreliso < chreliso_max_st
	  //	  && PtOverPt2 < pterr_max_st // Placeholder, but empty selection since it's already in LengthSelectionST
	  && LengthSelectionST) {
      fillST["chiso"] = chiso;
      }
      // RelIso 0.3
      if (genlepDR > genlepDR_min_st
	  && recolepDR > recolepDR_min_st
	  && dz < dz_max_st 
	  && dxy < dxy_max_st 
	  && iso < iso_max_st 
	  && niso < niso_max_st 
	  && chiso < chiso_max_st 
	  && pt > pt_min_st 
	  && nreliso < nreliso_max_st 
	  && chreliso < chreliso_max_st
	  //	  && PtOverPt2 < pterr_max_st // Placeholder, but empty selection since it's already in LengthSelectionST
	  && LengthSelectionST) {
	fillST["reliso"] = reliso;
      }
      // NeuRelIso 0.05
      if (genlepDR > genlepDR_min_st
	  && recolepDR > recolepDR_min_st
	  && dz < dz_max_st 
	  && dxy < dxy_max_st 
	  && iso < iso_max_st 
	  && niso < niso_max_st 
	  && chiso < chiso_max_st 
	  && reliso < reliso_max_st 
	  && pt > pt_min_st 
	  && chreliso < chreliso_max_st
	  //	  && PtOverPt2 < pterr_max_st // Placeholder, but empty selection since it's already in LengthSelectionST
	  && LengthSelectionST) {
	fillST["nreliso"] = nreliso;
      }
      // ChRelIso 0.05
      if (genlepDR > genlepDR_min_st
	  && recolepDR > recolepDR_min_st
	  && dz < dz_max_st 
	  && dxy < dxy_max_st 
	  && iso < iso_max_st 
	  && niso < niso_max_st 
	  && chiso < chiso_max_st 
	  && reliso < reliso_max_st 
	  && nreliso < nreliso_max_st 
	  && pt > pt_min_st
	  //	  && PtOverPt2 < pterr_max_st // Placeholder, but empty selection since it's already in LengthSelectionST
	  && LengthSelectionST) {
	fillST["chreliso"] = chreliso;
      }
      // PtErrOverPt2
      if (genlepDR > genlepDR_min_st
	  && recolepDR > recolepDR_min_st
	  && dz < dz_max_st 
	  && dxy < dxy_max_st 
	  && iso < iso_max_st 
	  && niso < niso_max_st 
	  && chiso < chiso_max_st 
	  && reliso < reliso_max_st 
	  && nreliso < nreliso_max_st 
	  && chreliso < chreliso_max_st
	  && pt > pt_min_st
	  && LengthSelectionST_RelaxedPtErr) {
	fillST["pterr"] = PtErrOverPt2;
      }
    } // End lostInnerHits (ST N-1) block

    // Now we need to execute the fills
    
    // The STC length selections are strictly looser versions of the short track length selections, and they are mutually exclusive.
    // A true short track will pass exactly one. Although this is not sufficient to be a short track, it's sufficient to sort tracks that may be short
    // into their possible category. 
    // Candidates may or may not pass one (no strict requirement on length for candidates).
    TString lensplit = "";
    if (p) lensplit = "p";
    else if (s) lensplit = "s";
    else if (l) lensplit = "l";
    // format is: sh+"_"+htsplit+"_"+lensplit+"_"+var;
    // Inclusive splits always apply
    TString prefix_i = HTsplit+"_i_";
    TString prefix_Ii = "I_i_";
    vector<TString> fill_prefixes = {prefix_i,prefix_Ii};
    // Not guaranteed to satisfy any length requirement. Only add these if we did.
    if (lensplit.Length() > 0) {
      TString prefix_I = "I_"+lensplit+"_";
      fill_prefixes.push_back(prefix_I);
      TString prefix_narrow = HTsplit+"_"+lensplit+"_";
      fill_prefixes.push_back(prefix_narrow);
    }
    for (const auto& var : vars) {
      const TString& varname = var.first;
      const float& stval = fillST[varname];
      const float& stcval = fillSTC[varname];
      if (stval > -99) {
	for (const auto& prefix : fill_prefixes) {
	  const TString name_to_fill = "st_"+prefix+varname;
	  hists[name_to_fill]->Fill(stval);
	}	
      }
      if (stcval > -99) {
	for (const auto& prefix : fill_prefixes) {
	  const TString name_to_fill = "stc_"+prefix+varname;
	  hists[name_to_fill]->Fill(stcval);
	}		
      }
    }
      
    } // End loop on tracks
    
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
    float ptA = -1; float ptB = -1;
    float drA = -1; float drB = -1;
    const int ngenLep = t.ngenLep + t.ngenTau;
    for (int i_lep = 0; i_lep < t.ngenLep; i_lep++) {
      const int pdgId = abs(t.genLep_pdgId[i_lep]);
      if (pdgId == 11) {
	founde = true;
      }
      else foundmu = true;
    }
    for (int i_tau = 0; i_tau < t.ngenTau; i_tau++) {
      const int decayMode = abs(t.genTau_decayMode[i_tau]);
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


    if (nstc > 0) {
      hists["ngenLep"]->Fill(t.ngenLep + t.ngenTau,w_);
    }

    hists["lepTypePS"]->Fill(lep_type,w_);
    if (nst > 0) {
      hists["lepType"]->Fill(lep_type,w_);
      const float st_pt = t.track_pt[st_idx];
      for (int i_lep = 0; i_lep < t.ngenLep; i_lep++) {
	const int pdgId = abs(t.genLep_pdgId[i_lep]);
	if (pdgId == 11) {
	  if (ptA > 0) {
	    ptB = t.genLep_pt[i_lep] / st_pt;
	    drB = DeltaR(t.genLep_eta[i_lep],t.track_eta[st_idx],t.genLep_phi[st_idx],t.track_eta[st_idx]);
	  } else {
	    ptA = t.genLep_pt[i_lep] / st_pt;
	    drA = DeltaR(t.genLep_eta[i_lep],t.track_eta[st_idx],t.genLep_phi[st_idx],t.track_eta[st_idx]);
	  }
	}
      }
      for (int i_lep = 0; i_lep < t.ngenLepFromTau; i_lep++) {
	const int pdgId = abs(t.genLep_pdgId[i_lep]);
	if (pdgId == 11) {
	  if (ptA > 0) {
	    ptB = t.genLepFromTau_pt[i_lep] / st_pt;
	    drB = DeltaR(t.genLepFromTau_eta[i_lep],t.track_eta[st_idx],t.genLepFromTau_phi[st_idx],t.track_eta[st_idx]);
	  } else {
	    ptA = t.genLepFromTau_pt[i_lep] / st_pt;
	    drA = DeltaR(t.genLepFromTau_eta[i_lep],t.track_eta[st_idx],t.genLepFromTau_phi[st_idx],t.track_eta[st_idx]);
	  }
	}
      }
      if (ptA > 0) {
	if (ptA > ptB) {
	  hists["lep1ptr"]->Fill(ptA,w_);
	  hists["lep1DR"]->Fill(drA,w_);
	  if (ptB > 0) {
	    hists["lep2ptr"]->Fill(ptB,w_);
	    hists["lep2DR"]->Fill(drB,w_);
	  }
	}
	else {
	  hists["lep1ptr"]->Fill(ptB,w_);
	  hists["lep1DR"]->Fill(drB,w_);
	  hists["lep2ptr"]->Fill(ptA,w_);
	  hists["lep2DR"]->Fill(drA,w_);
	}
      }
    } // if nst > 0

    if (false && founde) {
      for (int ijet = 0; ijet < t.njet; ijet++) {
	hists["chem"]->Fill(t.jet_cemFrac[ijet],w_);
	hists["nem"]->Fill(t.jet_nemFrac[ijet],w_);
	hists["em"]->Fill(t.jet_cemFrac[ijet] + t.jet_nemFrac[ijet],w_);
      }
    }
      
  }//end loop on events in a file

  hists["inclusive"]->Fill(0.5,nselected);
  
  cout << "About to write" << endl;
  
  //  mkdir = "mkdir -p drHists" + output_suffix;
  //  system(mkdir.c_str());

  //  TFile drfile(Form("drHists%s/%s.root",output_suffix.c_str(),sample),"RECREATE");

  //  mkdir = "mkdir -p vetoHists" + output_suffix;
  //system(mkdir.c_str());

  //TFile vetofile(Form("vetoHists%s/%s.root",output_suffix.c_str(),sample),"RECREATE");
    
  TFile outfile_(output_name,"RECREATE"); 
  outfile_.cd();

  for (const auto& [name,hist] : hists) {
    cout << "Writing " << name << endl;
    hist->Write();
  }
  outfile_.Close();

  cout << "Wrote everything" << endl;

  delete ch_mt2;
  delete ch_st;
  
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
