#include "ShortTrackLooper.h"

using namespace std;

class mt2tree;

const bool recalculate = false; // to use a non standard (ie not in babies) Fshort, change the iso and qual cuts and provide a new Fshort input file below
const float isoSTC = 6, qualSTC = 3;

TFile FshortFile("Fshort.root");
TH1D* h_fs = ((TH2F*) FshortFile.Get("h_FSR_60to100MT2"))->ProjectionX("h_fs",1,1);
// bin 1 is inclusive, then P, M, L
const double fs_P = h_fs->GetBinContent(2);
const double fs_M = h_fs->GetBinContent(3);
const double fs_L = h_fs->GetBinContent(4);

int ShortTrackLooper::loop (TChain* ch, char * outtag) {
  string tag(outtag);

  // Book histograms
  TH1::SetDefaultSumw2(true); // Makes histograms do proper error calculation automatically

  TH2F h_eventwise_counts ("h_eventwise_counts","Events Counts by Length",3,0,3,3,0,3);
  h_eventwise_counts.GetYaxis()->SetTitleOffset(3.0);
  h_eventwise_counts.GetXaxis()->SetBinLabel(1,"P");
  h_eventwise_counts.GetXaxis()->SetBinLabel(2,"M");
  h_eventwise_counts.GetXaxis()->SetBinLabel(3,"L");
  h_eventwise_counts.GetYaxis()->SetBinLabel(1,"Obs SR Events (SR)");
  h_eventwise_counts.GetYaxis()->SetBinLabel(2,"Pred SR Events (SR)");
  h_eventwise_counts.GetYaxis()->SetBinLabel(3,"CR STC Events");

  // NjHt
  TH2F* h_LL_VR = (TH2F*)h_eventwise_counts.Clone("h_LL_VR");
  TH2F* h_LH_VR = (TH2F*)h_eventwise_counts.Clone("h_LH_VR");
  TH2F* h_HL_VR = (TH2F*)h_eventwise_counts.Clone("h_HL_VR");
  TH2F* h_HH_VR = (TH2F*)h_eventwise_counts.Clone("h_HH_VR");

  TH2F* h_LL_SR = (TH2F*)h_eventwise_counts.Clone("h_LL_SR");
  TH2F* h_LH_SR = (TH2F*)h_eventwise_counts.Clone("h_LH_SR");
  TH2F* h_HL_SR = (TH2F*)h_eventwise_counts.Clone("h_HL_SR");
  TH2F* h_HH_SR = (TH2F*)h_eventwise_counts.Clone("h_HH_SR");

  mt2tree t;

  t.Init(ch);

  const unsigned int nEventsTree = ch->GetEntries();
  for( unsigned int event = 0; event < nEventsTree; ++event) {    
    //    if (event % 1000 == 0) cout << 100.0 * event / nEventsTree  << "%" << endl;

    t.GetEntry(event); 

    //---------------------
    // basic event selection and cleaning
    //---------------------
    
    if (unlikely(t.nVert == 0)) {
      continue;
    }
    if (t.nJet30 < 2) {
      continue;
    }
    if (t.ht < 300 && t.zll_ht < 300) {
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

    if (t.met_miniaodPt / t.met_caloPt > 5.0) {
      continue;
    }
    if (t.nJet200MuFrac50DphiMet > 0) {
      continue;
    }

    const bool lepveto = t.nMuons10 + t.nElectrons10 + t.nPFLep5LowMT + t.nPFHad10LowMT > 0;

    if (lepveto) {
      continue;
    }

    if (t.deltaPhiMin < 0.3) {
      continue;
    }
      
    const float lumi = 35.9; // 2016
    double weight = t.evt_scale1fb * lumi;
    // If no lepton and low MET, assume that data trigger is prescaled JetHT trigger.
    // These are 2016 numbers.
    if (t.met_pt < 250 && t.nlep == 0 && t.ht < 1000) { 
      if (t.ht < 450) weight = 1.0 / 9200.0;
      else if (t.ht < 575) weight = 1.0 / 460.6;
      else weight = 1.0 / 115.2;
    }

    TH2F* hist;
    
    // L*
    if (t.nJet30 < 4) {
      // LH
      if (t.ht > 1000) {
	// VR
	if (t.mt2 < 200) {
	  hist = h_LH_VR;
	}
	// SR 
	else {
	  hist = h_LH_SR;
	}
      }
      // LL
      else {
	// VR
	if (t.mt2 < 200) {
	  hist = h_LL_VR;
	}
	// SR 
	else {
	  hist = h_LL_SR;
	}
      }
    } 
    // H*
    else {
      // HH
      if (t.ht > 1000) {
	// VR
	if (t.mt2 < 200) {
	  hist = h_HH_VR;
	}
	// SR 
	else  {
	    hist = h_HH_SR;
	  }
      }
      // HL
      else {
	// VR
	if (t.mt2 < 200) {
	  hist = h_HL_VR;
	}
	// SR 
	else {
	  hist = h_HL_SR;
	}
      }
    }

    // Analysis code
    
    int nSTCp = t.nshorttrackcandidates_P;
    int nSTCm = t.nshorttrackcandidates_M;
    int nSTCl = t.nshorttrackcandidates_L;
    int nSTp = t.nshorttracks_P;
    int nSTm = t.nshorttracks_M;
    int nSTl = t.nshorttracks_L;

    if (recalculate) {
      // Manual calculation for testing new ST/STC definitions not coded in babymaker
      int nSTCp = 0;
      int nSTCm = 0;
      int nSTCl = 0;
      int nSTp = 0;
      int nSTm = 0;
      int nSTl = 0;

      const int ntracks = t.ntracks;
      for (int i_trk = 0; i_trk < ntracks; i_trk++) {   
	// Apply basic selections
	const bool EtaPhiSel = t.track_DeadECAL[i_trk] || t.track_DeadHCAL[i_trk];
	const float pt = t.track_pt[i_trk];
	const bool ptSel = pt > 15;
	const bool BaseSel = EtaPhiSel && ptSel;
	
	if (!BaseSel) continue;
	
	// Length and category
	const int Nlayers = t.track_nLayersWithMeasurement[i_trk];
	const bool lostOuterHitsSel = t.track_nLostOuterHits[i_trk] >= 2;
	const bool PixEqTracker = t.track_nPixelLayersWithMeasurement[i_trk] == Nlayers;
	const bool PixLtTracker = t.track_nPixelLayersWithMeasurement[i_trk] < Nlayers;
	const bool shortSel = t.track_nLayersWithMeasurement[i_trk] < 7;
	const bool isP = PixEqTracker;
	const bool isM = PixLtTracker && shortSel;
	const bool isL = PixLtTracker && !shortSel;
	const bool isShort = (isP || isM || isL) && lostOuterHitsSel;     
	
	if (!isShort) continue;
	
	const int lenIndex = isP ? 1 : (isM ? 2 : 3);
	
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
      
	if (!PassesRecoVeto) continue;
      
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
	const float reliso = iso / pt;
	const bool relisoSel = reliso < 0.2;
	const bool relisoSelSTC = reliso < 0.2 * isoSTC;
	const bool PassesFullIsoSel = NeuIso0p05Sel && NeuRelIso0p05Sel && isoSel && relisoSel;
	const bool PassesFullIsoSelSTC = NeuIso0p05SelSTC && NeuRelIso0p05SelSTC && isoSelSTC && relisoSelSTC;
	
	// Quality
	const bool pixLayersSelLoose = t.track_nPixelLayersWithMeasurement[i_trk] >= 2;
	const bool pixLayersSelTight = t.track_nPixelLayersWithMeasurement[i_trk] >= 3;
	const bool pixLayersSel4 = Nlayers > 4 ? pixLayersSelLoose : pixLayersSelTight; // Apply 2 layer selection to tracks with 5+ layers
	
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
	// Assume long track
	bool pterrSel = pterrSelTight;
	bool pterrSelSTC = pterrSelTightSTC;
	if (PixEqTracker) {pterrSel = pterrSelLoose; pterrSelSTC = pterrSelLooseSTC;}
	else if (Nlayers < 7) {pterrSel = pterrSelMedium; pterrSelSTC = pterrSelMediumSTC;}
	const float dxy = fabs(t.track_dxy[i_trk]);
	const bool dxySelLoose = dxy < 0.02;
	const bool dxySelLooseSTC = dxy < 0.02 * qualSTC;
	const bool dxySelTight = dxy < 0.01;
	const bool dxySelTightSTC = dxy < 0.01 * qualSTC;
	const bool dxySel = PixEqTracker ? dxySelLoose : dxySelTight;
	const bool dxySelSTC = PixEqTracker ? dxySelLooseSTC : dxySelTightSTC;
	const float dz = fabs(t.track_dz[i_trk]);
	const bool dzSel = dz < 0.05;
	const bool dzSelSTC = dz < 0.05*qualSTC;
	const bool isHighPurity = t.track_isHighPurity[i_trk] == 1;
	const bool QualityTrackBase = lostInnerHitsSel && pterrSel && isHighPurity && dxySel && dzSel;
	const bool QualityTrackSTCBase =  lostInnerHitsSel && pterrSelSTC && isHighPurity && dxySelSTC && dzSelSTC;
	const bool isQualityTrack = pixLayersSel4 && QualityTrackBase;
	const bool isQualityTrackSTC = pixLayersSel4 && QualityTrackSTCBase;
	
	// Candidate (loosened isolation, quality)
	const bool isSTC = PassesFullIsoSelSTC && isQualityTrackSTC;
	// Full Short Track
	const bool isST = PassesFullIsoSel && isQualityTrack;

	if (isSTC) {
	  switch(lenIndex) {
	  case 1: nSTCp++; break;
	  case 2: nSTCm++; break;
	  case 3: nSTCl++; break;
	  }
	}
	if (isST) {
	  switch(lenIndex) {
	  case 1: nSTp++; break;
	  case 2: nSTm++; break;
	  case 3: nSTl++; break;
	  }
	}
      } // Track loop
    } // recalculate
    
    // Eventwise STC Region fills
    // Chance of at least 1 track of given length being tagged is 1 - (1-p)^n
    if (nSTCl > 0) {
      hist->Fill(2.0,2.0,weight); // Fill CR
      hist->Fill(2.0,1.0, weight * ( 1 - pow(1-fs_L,nSTCl) ) ); // Fill expected SR count
    }
    else if (nSTCm > 0) {
      hist->Fill(1.0,2.0,weight); // Fill CR
      hist->Fill(1.0,1.0, weight * ( 1 - pow(1-fs_M,nSTCm) ) ); // Fill expected SR count
    }
    else if (nSTCp > 0) {
      hist->Fill(0.0,2.0,weight); // Fill CR
      hist->Fill(0.0,1.0, weight * ( 1 - pow(1-fs_P,nSTCp) ) ); // Fill expected SR count
    }
    // Eventwise ST fills
    // Only full STs when counting "observed" SR
    if (nSTl > 0) hist->Fill(2.0,0.0,weight);
    else if (nSTm > 0) hist->Fill(1.0,0.0,weight);
    else if (nSTp > 0) hist->Fill(0.0,0.0,weight);
  }//end loop on events in a file

  // Post-processing
    
  cout << "About to write" << endl;

  TFile outfile_(Form("%s.root",outtag),"RECREATE"); 
  outfile_.cd();
  h_LL_SR->Write();
  h_LH_SR->Write();
  h_HL_SR->Write();
  h_HH_SR->Write();
  h_LL_VR->Write();
  h_LH_VR->Write();
  h_HL_VR->Write();
  h_HH_VR->Write();
  outfile_.Close();
  cout << "Wrote everything" << endl;

  return 0;
}

int main (int argc, char ** argv) {

  if (argc < 3) {
    cout << "Usage: ./ShortTrackLooper.exe <outtag> <infile>" << endl;
    return 1;
  }

  TChain *ch = new TChain("mt2");
  ch->Add(argv[2]);

  ShortTrackLooper * stl = new ShortTrackLooper();
  stl->loop(ch,argv[1]);
  return 0;
}

ShortTrackLooper::ShortTrackLooper() {}

ShortTrackLooper::~ShortTrackLooper() {};
