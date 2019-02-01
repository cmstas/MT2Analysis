// C++
#include <iostream>
#include <vector>
#include <set>

// ROOT
#include "TDirectory.h"
#include "TTreeCache.h"
#include "Math/VectorUtil.h"
#include "TVector2.h"
#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TH2.h"
#include "TMinuit.h"

// CORE
#include "../CORE/CMS3.h"
#include "../CORE/Base.h"
#include "../CORE/JetSelections.h"
#include "../CORE/MuonSelections.h"
#include "../CORE/ElectronSelections.h"
#include "../CORE/PhotonSelections.h"
#include "../CORE/TriggerSelections.h"
#include "../CORE/MCSelections.h"
#include "../CORE/IsoTrackVeto.h"
#include "../CORE/IsolationTools.h"
#include "../CORE/MetSelections.h"

// Tools
#include "../CORE/Tools/utils.h"
#include "../CORE/Tools/hemJet.h" 
#include "../CORE/Tools/MT2/MT2.h"
#include "../CORE/Tools/JetCorrector.h"
#include "../CORE/Tools/jetcorr/FactorizedJetCorrector.h"
#include "../CORE/Tools/jetcorr/Utilities.icc"
#include "../CORE/Tools/jetcorr/JetCorrectionUncertainty.icc"
#include "../CORE/Tools/jetcorr/SimpleJetCorrectionUncertainty.icc"
#include "../CORE/Tools/goodrun.h"
#include "../CORE/Tools/btagsf/BTagCalibrationStandalone.h"
#include "../CORE/Tools/datasetinfo/getDatasetInfo.h"

// MT2CORE
#include "../MT2CORE/sampleID.h"
#include "../MT2CORE/applyWeights.h"
#include "../MT2CORE/configurations.h"

// header
#include "ScanChain.h"

using namespace std;
using namespace tas;

// turn on to apply json file to data (default true)
const bool applyJSON = true;
// turn on to save only isolated leptons (default true)
const bool applyLeptonIso = true;


DatasetInfoFromFile datasetInfoFromFile;

MT2Configuration config_;

ShortTrackTagAndProbeBabymaker *thisBabyMaker; //little sketchy, but need a global pointer to ShortTrackTagAndProbeBabymaker for use in minuitFunction (for doing rebalancing)

//--------------------------------------------------------------------

void ShortTrackTagAndProbeBabymaker::ScanChain(TChain* chain, std::string baby_name, const std::string config_tag, int max_events){

  // Benchmark
  TBenchmark *bmark = new TBenchmark();
  bmark->Start("benchmark");

  config_ = GetMT2Config(config_tag);
  cout << "Using configuration \"" << config_tag << "\"" << endl;
  // gconf is a GlobalConfig object in CORE
  // used to store CORE-specific values, working-points, etc
  gconf.ea_version = config_.ea_version;  // effective-area constants are year-specific. 

  if (baby_name.find("data_Run201") != std::string::npos || baby_name.find("dataRun2") != std::string::npos) {
    isDataFromFileName = true;
    cout << "running on DATA, based on file name" << endl;
  } else {
    isDataFromFileName = false;
    cout << "running on MC, based on file name" << endl;
  }

  MakeBabyNtuple( Form("%s.root", baby_name.c_str()) );

  if (isDataFromFileName && applyJSON) {
      string json_file = config_.json;
      cout << "Loading json file: " << json_file << endl;
      set_goodrun_file(json_file.c_str());
  }

  // Lepton Scale Factors
  setElSFfile("moriond17/scaleFactors_el_moriond_2017.root", "moriond17/egammaEffi.txt_EGM2D.root" );
  setMuSFfile("moriond17/TnP_NUM_LooseID_DENOM_generalTracks_VAR_map_pt_eta.root",
	      "moriond17/TnP_NUM_MiniIsoTight_DENOM_LooseID_VAR_map_pt_eta.root",
	      "moriond17/TnP_NUM_MediumIP2D_DENOM_LooseID_VAR_map_pt_eta.root",
	      "moriond17/Tracking_EfficienciesAndSF_BCDEFGH_hists.root");
  setVetoEffFile_fullsim("vetoeff_emu_etapt_lostlep.root");  // same values for Moriond17 as ICHEP16
  
  // File Loop
  int nEvents = chain->GetEntries();
  unsigned int nEventsChain = nEvents;
  cout << "Running on " << nEventsChain << " events" << endl;
  unsigned int nEventsTotal = 0;
  TObjArray *listOfFiles = chain->GetListOfFiles();
  TIter fileIter(listOfFiles);
  TFile *currentFile = 0;
  while ( (currentFile = (TFile*)fileIter.Next()) ) {
    cout << "running on file: " << currentFile->GetTitle() << endl;

    TString currentFileName(currentFile->GetTitle());

    // ----------------------------------
    // retrieve JEC from files, if using
    //   need to do within file loop to access sample names
    // ----------------------------------

    // Get File Content
    TFile* f = TFile::Open( currentFile->GetTitle() );
    TTree *tree = (TTree*)f->Get("Events");
    TTreeCache::SetLearnEntries(10);
    tree->SetCacheSize(128*1024*1024);
    cms3.Init(tree);

    // Event Loop
    unsigned int nEventsToLoop = tree->GetEntriesFast();
    if (max_events > 0) nEventsToLoop = (unsigned int) max_events;
    for( unsigned int event = 0; event < nEventsToLoop; ++event) {
      
      // Get Event Content
      tree->LoadTree(event);
      cms3.GetEntry(event);
      ++nEventsTotal;
      
      // Progress
      CMS3::progress( nEventsTotal, nEventsChain );

      InitBabyNtuple();

      isData = cms3.evt_isRealData();
      // sanity check on whether this is data
      if (bool(isData) != isDataFromFileName) {
        cout << "ERROR: file name (" << isDataFromFileName << ") and content (" << isData << ") disagree on whether this is real data!! Exiting" << endl;
        return;
      }

      // get CMS3 version number to use later
      TString cms3_version = cms3.evt_CMS3tag().at(0);     

      run  = cms3.evt_run();
      lumi = cms3.evt_lumiBlock();
      evt  = cms3.evt_event();
      
      if ( isData && applyJSON ) {
	if ( !goodrun(run, lumi) ) { 
	  continue;
	}
      }
      
      if(!isData){
	const string dataset_name = cms3.evt_dataset().at(0).Data();
	try {
	  evt_scale1fb = datasetInfoFromFile.getScale1fbFromFile(dataset_name, cms3_version.Data());
	  evt_xsec     = datasetInfoFromFile.getXsecFromFile(dataset_name, cms3_version.Data());
	} catch (const exception& e) {
	  if (event == 0) {
	    cout << "WARNING: " << e.what() << endl;
	    cout << "WARNING: Setting evt_scale1fb to 1.0, evt_xsec to 0" << endl;
	  }
	  evt_scale1fb = 1.0;
	  evt_xsec = 0.0;
	}
	evt_kfactor = 1.0;
	evt_filter = 1.0;
      }

      //VERTICES
      int nVert = 0;
      for(unsigned int ivtx=0; ivtx < cms3.evt_nvtxs(); ivtx++){
        if(cms3.vtxs_isFake().at(ivtx)) continue;
        if(cms3.vtxs_ndof().at(ivtx) <= 4) continue;
        if(fabs(cms3.vtxs_position().at(ivtx).z()) > 24) continue;
        if(cms3.vtxs_position().at(ivtx).Rho() > 2) continue;

        nVert++;

      }

      if (nVert == 0) continue;

      // Save both STs and STCs, dump the rest except PF mus, els
      ntracks = 0; int foundPFlep = 0;
      for (unsigned int i_it = 0; i_it < cms3.isotracks_p4().size(); i_it++) {
	if (!cms3.isotracks_fromPV().at(i_it)) continue;
	const float pt = cms3.isotracks_pttrk().at(i_it);
	if (pt < 15) continue;
	// simple preselection
	
	bool saveTrack = false;

	if (cms3.isotracks_isPFCand().at(i_it) && abs(cms3.isotracks_particleId().at(i_it)) == 13) {
	  track_isMuon[ntracks] = true; // it's a PF Muon
	  saveTrack = true;	  
	  foundPFlep = 13;
	}
	else {
	  for (unsigned int i_mu = 0; i_mu < cms3.mus_p4().size(); i_mu++) {
	    if (DeltaR(cms3.mus_p4().at(i_mu), cms3.isotracks_p4().at(i_it)) < 0.01) {
	      track_isMuon[ntracks] = true; // it's a reco muon
	      saveTrack = true;
	      break;
	    }
	  }
	}
	if (!track_isMuon[ntracks]) { // if it's not a muon, maybe it's an electron
	  if (cms3.isotracks_isPFCand().at(i_it) && abs(cms3.isotracks_particleId().at(i_it)) == 11) {
	    track_isElectron[ntracks] = true; // it's a PF electron
	    saveTrack = true;
	    foundPFlep = 11;
	  }
	  else {
	    for (unsigned int i_el = 0; i_el < cms3.els_p4().size(); i_el++) {
	      if (DeltaR(cms3.els_p4().at(i_el), cms3.isotracks_p4().at(i_it)) < 0.01) {
		track_isElectron[ntracks] = true; // it's a reco electron
		saveTrack = true;
		break;
	      }
	    }
	  }
	}

	if (saveTrack) {
	  track_iscand[ntracks] = false; // leps are not STCs
	}
	else if (!cms3.isotracks_isPFCand().at(i_it)) {
	  // not a PF Cand or reco lep, let's see if it's an ST or STC
	  if (!cms3.isotracks_isHighPurityTrack().at(i_it)) continue;
	  const int Nlayers = cms3.isotracks_trackerLayersWithMeasurement().at(i_it);
	  if (Nlayers < 7) continue;
	  if (cms3.isotracks_pixelLayersWithMeasurement().at(i_it) < 2) continue;
	  if (cms3.isotracks_numberOfLostPixelHitsInner().at(i_it) > 0) continue;
	  if (unlikely(cms3.isotracks_numberOfLostPixelHitsInner().at(i_it) > 0)) continue;
	  // Quality x3
	  if (fabs(cms3.isotracks_dz().at(i_it)) > 0.15 ) continue;
	  if (fabs(cms3.isotracks_dxy().at(i_it)) > 0.03) continue;
	  if (unlikely(cms3.isotracks_pterr().at(i_it) / (pt * pt) > 0.015)) continue;
	  
	  const float niso = cms3.isotracks_pfIso_nh().at(i_it) + cms3.isotracks_pfIso_em().at(i_it);
	  const float iso = cms3.isotracks_pfIso_ch().at(i_it) + niso - max(0.0,(double) (0.5 * cms3.isotracks_pfIso_db().at(i_it)) );
	  // iso x6
	  if (cms3.isotracks_pfNeutralSum().at(i_it) > 60 || cms3.isotracks_pfNeutralSum().at(i_it) / pt > 0.6 || iso  > 60 || iso / pt > 1.2) continue;
	  // Lepton veto
	  // Veto on ANY reco lepton
	  if (cms3.isotracks_nearestPF_DR().at(i_it) < 0.1 && (abs(cms3.isotracks_nearestPF_id().at(i_it)) == 11 || abs(cms3.isotracks_nearestPF_id().at(i_it)) == 13)) continue; // no nearby PF leps 	
	  float minrecodr = 0.2;
	  for (unsigned int i_el = 0; i_el < cms3.els_p4().size(); i_el++) {
	    float dr = DeltaR(cms3.els_p4().at(i_el).eta(),track_eta[ntracks],cms3.els_p4().at(i_el).phi(),track_phi[ntracks]);
	    if (dr < minrecodr) {
	      minrecodr = dr;
	    }
	  }
	  for (unsigned int i_mu = 0; i_mu < cms3.mus_p4().size(); i_mu++) {
	    float dr = DeltaR(cms3.mus_p4().at(i_mu).eta(),track_eta[ntracks],cms3.mus_p4().at(i_mu).phi(),track_phi[ntracks]);
	    if (dr < minrecodr) {
	      minrecodr = dr;
	    }
	  }
	  if (minrecodr < 0.1) continue; // no nearby reco leps
	  // if we get here, passed all selections needed to save the track
	  saveTrack = true;
	  // Save these branches for ST(C)s, no need to bother for leps
	  track_dz[ntracks] = cms3.isotracks_dz().at(i_it);
	  track_dxy[ntracks] = cms3.isotracks_dxy().at(i_it);
	  track_PtErrOverPt2[ntracks] = cms3.isotracks_pterr().at(i_it) / (pt * pt);
	  track_iso[ntracks] = iso;
	  track_pfNeutralSum[ntracks] = cms3.isotracks_pfNeutralSum().at(i_it);
	  
	  track_missingOuterHits[ntracks] = cms3.isotracks_numberOfLostHitsOuter().at(i_it);
	  track_nLayers[ntracks] = cms3.isotracks_trackerLayersWithMeasurement().at(i_it);
	  
	  // cand or full ST? Store bitwise which selections failed
	  track_iscand[ntracks] = 
	    (fabs(track_dz[ntracks]) > 0.05 ? 1<<6 : 0) + 
	    (fabs(track_dxy[ntracks]) > 0.01 ? 1<<5 : 0) + 
	    (track_PtErrOverPt2[ntracks] > 0.005 ? 1<<4 : 0) +
	    (track_pfNeutralSum[ntracks] > 10 ? 1<<3 : 0) +
	    (track_pfNeutralSum[ntracks] / pt > 10 ? 1<<2 : 0) +
	    (track_iso[ntracks] > 10 ? 1<<1 : 0 ) +
	    (track_iso[ntracks] / pt > 0.2 ? 1<<0 : 0);	    	  	    
	}
	
	if (saveTrack) {
	  // save these for both leps and ST(C)s
	  track_isPFCand[ntracks] = cms3.isotracks_isPFCand().at(i_it);
	  track_pt[ntracks] = pt;
	  track_eta[ntracks] = cms3.isotracks_etatrk().at(i_it);
	  track_phi[ntracks] = cms3.isotracks_phitrk().at(i_it);
	  track_charge[ntracks] = cms3.isotracks_charge().at(i_it);
	  track_mass[ntracks] = cms3.isotracks_p4().at(i_it).M();

	  for (unsigned int iCAL = 0; iCAL < cms3.isotracks_crossedEcalStatus().at(i_it).size(); iCAL++) {
	    if (cms3.isotracks_crossedEcalStatus().at(i_it).at(iCAL) != 0) {
	      track_DeadECAL[ntracks] = 1;
	      break;
	    }
	  }
	  for (unsigned int iCAL = 0; iCAL < cms3.isotracks_crossedHcalStatus().at(i_it).size(); iCAL++) {
	    if (cms3.isotracks_crossedHcalStatus().at(i_it).at(iCAL) != 0) {
	      track_DeadHCAL[ntracks] = 1;
	      break;
	    }
	  }

	  ntracks++;
	}
      } // End isotracks loop
      
      if (!foundPFlep) continue; // if there wasn't a single PF lep in this event, don't save it at all
      if (ntracks < 2) continue; // don't save events with no possiblity to make a Z pair

      // stand alone muons separately in case we want to take a look
      nsams = 0;
      for (unsigned int i_mu = 0; i_mu < cms3.mus_p4().size(); i_mu++) {
	if (! (cms3.mus_type().at(i_mu) & (1<<3) )) continue;
	sam_pt[nsams] = cms3.mus_p4().at(i_mu).pt();
	sam_eta[nsams] = cms3.mus_p4().at(i_mu).eta();
	sam_phi[nsams] = cms3.mus_p4().at(i_mu).phi();
	sam_charge[nsams] = cms3.mus_charge().at(i_mu);
	sam_mass[nsams] = cms3.mus_p4().at(i_mu).M();
      }

      //      cout << run << ":" << lumi << ":" << evt << endl;
      FillBabyNtuple();
      
    }//end loop on events in a file 
   
    delete tree;
    f->Close();
    
  }//end loop on files
  
  cout << "Processed " << nEventsTotal << " events" << endl;
  if ( nEventsChain != nEventsTotal ) {
    std::cout << "ERROR: number of events from files is not equal to total number of events" << std::endl;
  }
  
  CloseBabyNtuple();

  bmark->Stop("benchmark");
  cout << endl;
  cout << nEventsTotal << " Events Processed" << endl;
  cout << "------------------------------" << endl;
  cout << "CPU  Time:	" << Form( "%.01f s", bmark->GetCpuTime("benchmark")  ) << endl;
  cout << "Real Time:	" << Form( "%.01f s", bmark->GetRealTime("benchmark") ) << endl;
  cout << endl;
  delete bmark;

  return;
}

void ShortTrackTagAndProbeBabymaker::MakeBabyNtuple(const char *BabyFilename){

  cout << "BabyFilename is: " << BabyFilename << endl;
  BabyFile_ = new TFile(Form("%s", BabyFilename), "RECREATE");
  BabyFile_->cd();
  BabyTree_ = new TTree("stp", "A Baby Ntuple");
  
  BabyTree_->Branch("run", &run );
  BabyTree_->Branch("lumi", &lumi );
  BabyTree_->Branch("evt", &evt, "evt/l" );
  BabyTree_->Branch("isData", &isData );
  BabyTree_->Branch("evt_scale1fb", &evt_scale1fb);
  BabyTree_->Branch("evt_xsec", &evt_xsec );
  BabyTree_->Branch("evt_kfactor", &evt_kfactor );
  BabyTree_->Branch("evt_filter", &evt_filter );

  BabyTree_->Branch("ntracks", &ntracks);
  BabyTree_->Branch("track_pt", track_pt, "track_pt[ntracks]/F");
  BabyTree_->Branch("track_eta", track_eta, "track_eta[ntracks]/F");
  BabyTree_->Branch("track_phi", track_phi, "track_phi[ntracks]/F");
  BabyTree_->Branch("track_mass", track_mass, "track_mass[ntracks]/F");
  BabyTree_->Branch("track_charge", track_charge, "track_charge[ntracks]/I");
  BabyTree_->Branch("track_isMuon", track_isMuon, "track_isMuon[ntracks]/I");
  BabyTree_->Branch("track_isElectron", track_isElectron, "track_isElectron[ntracks]/I");
  BabyTree_->Branch("track_isPFCand", track_isPFCand, "track_isPFCand[ntracks]/I");

  BabyTree_->Branch("track_dz", track_dz, "track_dz[ntracks]/F");
  BabyTree_->Branch("track_dxy", track_dxy, "track_dxy[ntracks]/F");
  BabyTree_->Branch("track_PtErrOverPt2", track_PtErrOverPt2, "track_PtErrOverPt2[ntracks]/F");
  BabyTree_->Branch("track_iso", track_iso, "track_iso[ntracks]/F");
  BabyTree_->Branch("track_pfNeutralSum", track_pfNeutralSum, "track_PfNeutralSum[ntracks]/F");
  
  BabyTree_->Branch("track_iscand", track_iscand, "track_iscand[ntracks]/I");
  BabyTree_->Branch("track_missingOuterHits", track_missingOuterHits, "track_missingOuterHits[ntracks]/I");
  BabyTree_->Branch("track_nLayers", track_nLayers, "track_nLayers[ntracks]/I");

  BabyTree_->Branch("track_DeadECAL", track_DeadECAL, "track_DeadECAL[ntracks]/I");
  BabyTree_->Branch("track_DeadHCAL", track_DeadHCAL, "track_DeadHCAL[ntracks]/I");

  BabyTree_->Branch("nsams", &nsams);
  BabyTree_->Branch("sam_pt", sam_pt, "sam_pt[nsams]/F");
  BabyTree_->Branch("sam_eta", sam_eta, "sam_eta[nsams]/F");
  BabyTree_->Branch("sam_phi", sam_phi, "sam_phi[nsams]/F");
  BabyTree_->Branch("sam_mass", sam_mass, "sam_mass[nsams]/F");
  BabyTree_->Branch("sam_charge", sam_charge, "sam_charge[nsams]/I");
  

  return;
}

void ShortTrackTagAndProbeBabymaker::InitBabyNtuple () {

  run    = -999;
  lumi   = -999;
  evt    = -999;
  isData = -999;
  evt_scale1fb = 0;
  evt_xsec = -999.0;
  evt_kfactor = -999.0;
  evt_filter = -999.0;
  ntracks = -999;
  for (int i=0; i < 20; i++){
    track_pt[i] = -999;
    track_eta[i] = -999;
    track_phi[i] = -999;
    track_mass[i] = -999;
    track_charge[i] = -999;
    track_isMuon[i] = 0;
    track_isElectron[i] = 0;
    track_isPFCand[i] = -999;
    track_dz[i] = -999;
    track_dxy[i] = -999;
    track_PtErrOverPt2[i] = -999;
    track_iso[i] = -999;
    track_pfNeutralSum[i] = -999;
    track_iscand[i] = 0;
    track_missingOuterHits[i] = -999;
    track_nLayers[i] = -999;
    track_DeadECAL[i] = 0;
    track_DeadHCAL[i] = 0;
  }
  for (int i=0; i< 10; i++) {
    sam_pt[i] = -999;
    sam_eta[i] = -999;
    sam_phi[i] = -999;
    sam_mass[i] = -999;
    sam_charge[i] = 0;
  }

  return;
}

void ShortTrackTagAndProbeBabymaker::FillBabyNtuple(){
  BabyTree_->Fill();
  return;
}

void ShortTrackTagAndProbeBabymaker::CloseBabyNtuple(){
  BabyFile_->cd();
  BabyTree_->Write();
  BabyFile_->Close();
  return;
}

