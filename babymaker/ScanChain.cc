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
#include "TObject.h"

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

// turn on to add debugging statements (default false) 
const bool verbose = false;
// turn on to apply JEC from text files (default true)
bool applyJECfromFile = true;
// change to do unclustered energy uncertainty MET variations. 0 = DEFAULT, 1 = UP, -1 = DN
const int applyUnclusteredUnc = 0;
// turn on to apply btag SFs (default true)
const bool applyBtagSFs = true;
// turn on to recompute type1 MET using JECs from file (default true)
const bool recomputeT1MET = true;
// turn on to save prunedGenParticle collection (default false)
const bool saveGenParticles = false;
// turn on to apply trigger cuts to ntuples -> OR of all triggers used (default false)
const bool applyTriggerCuts = false;
// turn on to apply dummy weights for lepton SFs, btag SFs, etc (default false)
const bool applyDummyWeights = false;
// turn on to apply lepton SF
const bool applyLeptonSFs = true;
// turn on to apply json file to data (default true)
const bool applyJSON = true;
// for testing purposes, running on unmerged files (default false)
const bool removePostProcVars = false;
// turn on to remove jets overlapping with leptons (default true)
const bool doJetLepOverlapRemoval = true;
// turn on to save only isolated leptons (default true)
const bool applyLeptonIso = true;
// turn on to save MC scale and PDF weights (default false, makes babies ~4x as large)
const bool saveLHEweights = false;
// turn on to save MC scale weights (default false, small size impact)
const bool saveLHEweightsScaleOnly = true;
// use isotracks collection in cms4 for veto counting (as opposed to pfcands)
const bool useIsotrackCollectionForVeto = false;
// save high-pT PF cands
const bool saveHighPtPFcands = true;
const bool savePFphotons = false;

float HEM_region[4] = {-4.7, -1.5, -1.6, -0.8}; // region with bad HEM modules in 2018. etalow, etahigh, philow, phihigh

float rebal_sigma_soft_ = 20.0;

// Short Track Candidate isolation and quality loosening factors
const float isoSTC = 6, qualSTC = 3;

DatasetInfoFromFile datasetInfoFromFile;

MT2Configuration config_;

babyMaker *thisBabyMaker; //little sketchy, but need a global pointer to babyMaker for use in minuitFunction (for doing rebalancing)

//--------------------------------------------------------------------

// This is meant to be passed as the third argument, the predicate, of the standard library sort algorithm
inline bool sortByPt(const LorentzVector &vec1, const LorentzVector &vec2 ) {
  return vec1.pt() > vec2.pt();
}

// This is meant to be passed as the third argument, the predicate, of the standard library sort algorithm
inline bool sortByValue(const std::pair<int,float>& pair1, const std::pair<int,float>& pair2 ) {
  return pair1.second > pair2.second;
}

//--------------------------------------------------------------------

void babyMaker::ScanChain(TChain* chain, std::string baby_name, const std::string config_tag, bool isFastsim, int max_events){

  // Benchmark
  TBenchmark *bmark = new TBenchmark();
  bmark->Start("benchmark");

  config_ = GetMT2Config(config_tag);
  cout << "Using configuration \"" << config_tag << "\"" << endl;
  // gconf is a GlobalConfig object in CORE
  // used to store CORE-specific values, working-points, etc
  gconf.year = config_.year;  
  gconf.ea_version = config_.ea_version;  // effective-area constants are year-specific. 
  gconf.cmssw_ver = config_.cmssw_ver;

  if (baby_name.find("data_Run201") != std::string::npos || baby_name.find("dataRun2") != std::string::npos) {
    isDataFromFileName = true;
    cout << "running on DATA, based on file name" << endl;
  } else {
    isDataFromFileName = false;
    cout << "running on MC, based on file name" << endl;
  }

  if (doRecomputeRawPFMET_) cout << "Will recompute rawPFMET using PF cands" << endl;

  MakeBabyNtuple( Form("%s.root", baby_name.c_str()) );

  if (isDataFromFileName && applyJSON) {
      string json_file = "jsons/" + config_.json;
      cout << "Loading json file: " << json_file << endl;
      set_goodrun_file(json_file.c_str());
  }

  if(doRebal){
      rebal_reader.SetCoreScale(1.0);
      rebal_reader.SetTailScale(1.0);
      rebal_reader.SetMeanShift(0.0);
      // don't want to widen cores for rebalancing, so treat as MC always
      string rebal_file = "rebal/" + config_.JRT_file;
      rebal_reader.Init(rebal_file.c_str(), false, 0, config_tag);
      cout << "Initialized rebalancing template reader with template file: " << rebal_file << endl;
  }

  TDirectory *rootdir = gDirectory->GetDirectory("Rint:");

  if (applyBtagSFs) {
    // setup btag calibration readers
    cout << "Applying btag scale factors from btagsf/" << config_.btagcalib_csv << endl;
    calib = new BTagCalibration("csvv2", "btagsf/"+config_.btagcalib_csv); // 94X version
    // https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation80X
    reader_fullsim = new BTagCalibrationReader(BTagEntry::OP_MEDIUM, "central",{"up","down"});
    reader_fullsim->load(*calib, BTagEntry::JetFlavor::FLAV_B, "comb");
    reader_fullsim->load(*calib, BTagEntry::JetFlavor::FLAV_C, "comb");
    reader_fullsim->load(*calib, BTagEntry::JetFlavor::FLAV_UDSG, "incl");
    // get btag efficiencies
    cout << "Using btag efficiencies from btagsf/" << config_.btageff_file << endl;
    TFile* f_btag_eff = new TFile(("btagsf/"+config_.btageff_file).c_str());
    TH2D* h_btag_eff_b_temp = (TH2D*) f_btag_eff->Get("h2_BTaggingEff_csv_med_Eff_b");
    TH2D* h_btag_eff_c_temp = (TH2D*) f_btag_eff->Get("h2_BTaggingEff_csv_med_Eff_c");
    TH2D* h_btag_eff_udsg_temp = (TH2D*) f_btag_eff->Get("h2_BTaggingEff_csv_med_Eff_udsg");
    h_btag_eff_b = (TH2D*) h_btag_eff_b_temp->Clone("h_btag_eff_b");
    h_btag_eff_b->SetDirectory(rootdir);
    h_btag_eff_c = (TH2D*) h_btag_eff_c_temp->Clone("h_btag_eff_c");
    h_btag_eff_c->SetDirectory(rootdir);
    h_btag_eff_udsg = (TH2D*) h_btag_eff_udsg_temp->Clone("h_btag_eff_udsg");
    h_btag_eff_udsg->SetDirectory(rootdir);
    f_btag_eff->Close();

    std::cout << "loaded fullsim btag SFs" << std::endl;
    
    // extra copy for fastsim -> fullsim SFs
    if (isFastsim) {
      // setup btag calibration readers
      calib_fastsim = new BTagCalibration("CSV", "btagsf/fastsim_csvv2_ttbar_26_1_2017.csv"); // 80x Moriond17 fastsim version of SFs
      reader_fastsim = new BTagCalibrationReader(BTagEntry::OP_MEDIUM, "central",{"up","down"});
      reader_fastsim->load(*calib_fastsim, BTagEntry::JetFlavor::FLAV_UDSG, "fastsim");
      reader_fastsim->load(*calib_fastsim, BTagEntry::JetFlavor::FLAV_B, "fastsim");
      reader_fastsim->load(*calib_fastsim, BTagEntry::JetFlavor::FLAV_C, "fastsim");

      // get btag efficiencies
      TFile* f_btag_eff_fastsim = new TFile("btagsf/btageff__SMS-T1bbbb-T1qqqq_25ns_Moriond17.root");
      TH2D* h_btag_eff_b_fastsim_temp = (TH2D*) f_btag_eff_fastsim->Get("h2_BTaggingEff_csv_med_Eff_b");
      TH2D* h_btag_eff_c_fastsim_temp = (TH2D*) f_btag_eff_fastsim->Get("h2_BTaggingEff_csv_med_Eff_c");
      TH2D* h_btag_eff_udsg_fastsim_temp = (TH2D*) f_btag_eff_fastsim->Get("h2_BTaggingEff_csv_med_Eff_udsg");
      h_btag_eff_b_fastsim = (TH2D*) h_btag_eff_b_fastsim_temp->Clone("h_btag_eff_b_fastsim");
      h_btag_eff_b_fastsim->SetDirectory(rootdir);
     h_btag_eff_c_fastsim = (TH2D*) h_btag_eff_c_fastsim_temp->Clone("h_btag_eff_c_fastsim");
      h_btag_eff_c_fastsim->SetDirectory(rootdir);
      h_btag_eff_udsg_fastsim = (TH2D*) h_btag_eff_udsg_fastsim_temp->Clone("h_btag_eff_udsg_fastsim");
      h_btag_eff_udsg_fastsim->SetDirectory(rootdir);
      f_btag_eff_fastsim->Close();
      
      std::cout << "loaded fastsim btag SFs" << std::endl;
    } // if (isFastsim)
  }

  // Lepton Scale Factors
  if (!isDataFromFileName && applyLeptonSFs) {
      cout << "Applying lepton scale factors from the following files:\n";
      cout << "    els IDISO: " << config_.elSF_IDISOfile << ": " << config_.elSF_IDhistName << " (id), " << config_.elSF_ISOhistName << " (iso)" << endl;
      cout << "    els TRK  : " << config_.elSF_TRKfile << endl;
      cout << "    muons ID : " << config_.muSF_IDfile << ": " << config_.muSF_IDhistName << endl;
      cout << "    muons ISO: " << config_.muSF_ISOfile << ": " << config_.muSF_ISOhistName << endl;
      if(config_.muSF_IPfile!="")
          cout << "    muons IP : " << config_.muSF_IPfile << ": " << config_.muSF_IPhistName << endl;
      else
          cout << "    muons IP : not applying" << endl;
      if(config_.muSF_TRKfile!="")
          cout << "    muons TRK: " << config_.muSF_TRKfile << ": " << config_.muSF_TRKLT10histName << " (pT<10), " << config_.muSF_TRKGT10histName << " (pt>10)" << endl;
      else
          cout << "    muons TRK: not applying" << endl;
      setElSFfile(config_.elSF_IDISOfile, config_.elSF_TRKfile, config_.elSF_IDhistName, config_.elSF_ISOhistName);
      setMuSFfile(config_.muSF_IDfile, config_.muSF_ISOfile, config_.muSF_IPfile, config_.muSF_TRKfile,
                  config_.muSF_IDhistName, config_.muSF_ISOhistName, config_.muSF_IPhistName, config_.muSF_TRKLT10histName, config_.muSF_TRKGT10histName);
      setVetoEffFile_fullsim("lepsf/vetoeff_emu_etapt_lostlep.root");  // same values for Moriond17 as ICHEP16
      if (isFastsim) {
          setElSFfile_fastsim("lepsf/moriond17/sf_el_vetoCB_mini01.root");
          setMuSFfile_fastsim("lepsf/moriond17/sf_mu_looseID.root",
                              "lepsf/moriond17/sf_mu_looseID_mini02.root",
                              "lepsf/moriond17/sf_mu_mediumID_looseIP2D.root");
          setVetoEffFile_fastsim("lepsf/vetoeff_emu_etapt_T1tttt.root");  
      }
  }

  // get susy xsec file
  TH1F* h_sig_xsec(0);
  if ((baby_name.find("T1") != std::string::npos) || (baby_name.find("T2") != std::string::npos) || (baby_name.find("T5") != std::string::npos) || (baby_name.find("T6") != std::string::npos)) {
    // determine which susy particle is being produced
    TString sparticle = "";
    if ((baby_name.find("T1") != std::string::npos) || (baby_name.find("T5") != std::string::npos)) sparticle = "gluino";
    else if ((baby_name.find("T2tt") != std::string::npos) || (baby_name.find("T2bb") != std::string::npos) || (baby_name.find("T2cc") != std::string::npos) || (baby_name.find("T2bW") != std::string::npos) || (baby_name.find("T2bt") != std::string::npos)) sparticle = "stop";
    else if ((baby_name.find("T2qq") != std::string::npos) || (baby_name.find("T6qq") != std::string::npos)) sparticle = "squark";
    if (sparticle == "") std::cout << "WARNING: didn't recognize signal sample from name: " << baby_name << std::endl;
    
    TFile* f_xsec = new TFile("data/xsec_susy_13tev.root");
    TH1F* h_sig_xsec_temp = (TH1F*) f_xsec->Get(Form("h_xsec_%s",sparticle.Data()));
    h_sig_xsec = (TH1F*) h_sig_xsec_temp->Clone("h_sig_xsec");
    h_sig_xsec->SetDirectory(rootdir);
    f_xsec->Close();
  }
  
  // File Loop
  int nDuplicates = 0;
  int nFailJSON = 0;
  int nEvents = chain->GetEntries();
  unsigned int nEventsChain = nEvents;
  cout << "Running on " << nEventsChain << " events" << endl;
  unsigned int nEventsTotal = 0;
  TObjArray *listOfFiles = chain->GetListOfFiles();
  TIter fileIter(listOfFiles);
  TFile *currentFile = 0;
  while ( (currentFile = (TFile*)fileIter.Next()) ) {
    cout << "running on file: " << currentFile->GetTitle() << endl;

    evt_id = sampleID(currentFile->GetTitle());
    TString currentFileName(currentFile->GetTitle());

    // ----------------------------------
    // retrieve JEC from files, if using
    //   need to do within file loop to access sample names
    // ----------------------------------

    // stores current corrections for a given event
    FactorizedJetCorrector *jet_corrector_pfL1FastJetL2L3_current(0);
    
    // default corrections
    std::vector<std::string> jetcorr_filenames_pfL1FastJetL2L3;
    FactorizedJetCorrector *jet_corrector_pfL1FastJetL2L3(0);
    JetCorrectionUncertainty* jetcorr_uncertainty(0);

    // corrections for later runs in 2016F
    std::vector<std::string> jetcorr_filenames_pfL1FastJetL2L3_postrun278802;
    FactorizedJetCorrector *jet_corrector_pfL1FastJetL2L3_postrun278802(0);
    
    if (applyJECfromFile) {
      jetcorr_filenames_pfL1FastJetL2L3.clear();
      std::string jetcorr_uncertainty_filename;
      
      for(uint ijec=0; ijec < config_.JECs.size(); ijec++){
	string keyword = config_.JECs.at(ijec).first;
	string jecname = config_.JECs.at(ijec).second;
	if(keyword == "" || currentFileName.Contains(keyword)){
	  jetcorr_filenames_pfL1FastJetL2L3.clear();
	  jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/" + jecname + "_L1FastJet_AK4PFchs.txt");
	  jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/" + jecname + "_L2Relative_AK4PFchs.txt");
	  jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/" + jecname + "_L3Absolute_AK4PFchs.txt");
          if(!isFastsim)
              jetcorr_filenames_pfL1FastJetL2L3.push_back  ("jetCorrections/" + jecname + "_L2L3Residual_AK4PFchs.txt");
	  if(!isDataFromFileName){
	    jetcorr_uncertainty_filename = "jetCorrections/" + jecname + "_Uncertainty_AK4PFchs.txt";
	  }
	  break;
	}
      }

      cout << "applying JEC from the following files:" << endl;
      for (unsigned int ifile = 0; ifile < jetcorr_filenames_pfL1FastJetL2L3.size(); ++ifile) {
	cout << "   " << jetcorr_filenames_pfL1FastJetL2L3.at(ifile) << endl;
      }
      jet_corrector_pfL1FastJetL2L3  = makeJetCorrector(jetcorr_filenames_pfL1FastJetL2L3);
      

      // ad-hoc solution for latter part of 2016F that requires the 2016G corrections
      if (isDataFromFileName && currentFileName.Contains("2016F")) {
	  jetcorr_filenames_pfL1FastJetL2L3_postrun278802.clear();
	  jetcorr_filenames_pfL1FastJetL2L3_postrun278802.push_back  ("jetCorrections/Summer16_07Aug2017GH_V11_DATA_L1FastJet_AK4PFchs.txt"   );
	  jetcorr_filenames_pfL1FastJetL2L3_postrun278802.push_back  ("jetCorrections/Summer16_07Aug2017GH_V11_DATA_L2Relative_AK4PFchs.txt"  );
	  jetcorr_filenames_pfL1FastJetL2L3_postrun278802.push_back  ("jetCorrections/Summer16_07Aug2017GH_V11_DATA_L3Absolute_AK4PFchs.txt"  );
	  jetcorr_filenames_pfL1FastJetL2L3_postrun278802.push_back  ("jetCorrections/Summer16_07Aug2017GH_V11_DATA_L2L3Residual_AK4PFchs.txt");
          cout << "additional run-dependent JEC from the following files:" << endl;
          for (unsigned int ifile = 0; ifile < jetcorr_filenames_pfL1FastJetL2L3_postrun278802.size(); ++ifile) {
              cout << "   " << jetcorr_filenames_pfL1FastJetL2L3_postrun278802.at(ifile) << endl;
          }
          jet_corrector_pfL1FastJetL2L3_postrun278802  = makeJetCorrector(jetcorr_filenames_pfL1FastJetL2L3);
      }

      if (!isDataFromFileName) {
	cout << "applying JEC uncertainties from file: " << endl
	     << "   " << jetcorr_uncertainty_filename << endl;
	jetcorr_uncertainty = new JetCorrectionUncertainty(jetcorr_uncertainty_filename);
      }
      
    } // if applyJECfromFile
    
    // Get File Content
    TFile* f = TFile::Open( currentFile->GetTitle() );
    TTree *tree = (TTree*)f->Get("Events");
    TTreeCache::SetLearnEntries(10);
    tree->SetCacheSize(128*1024*1024);
    cms3.Init(tree);

    // some invalid trees only have eventMaker branches. Check if that is the case here; skip below
    bool hasAllBranches = (tree->GetListOfBranches()->FindObject("float_pfmetMaker_evtpfmet_CMS3.") != NULL);

    // Event Loop
    unsigned int nEventsToLoop = tree->GetEntriesFast();
    if (max_events > 0) nEventsToLoop = (unsigned int) max_events;
    for( unsigned int event = 0; event < nEventsToLoop; ++event) {

      // Get Event Content
      tree->LoadTree(event);
      cms3.GetEntry(event);
      ++nEventsTotal;
      count_hist_->Fill(1);
      
      // Progress
      CMS3::progress( nEventsTotal, nEventsChain );

      InitBabyNtuple();

      isData = cms3.evt_isRealData();
      // sanity check on whether this is data
      if (bool(isData) != isDataFromFileName) {
        cout << "ERROR: file name and content disagree on whether this is real data!! Exiting" << endl;
        return;
      }

      // get CMS3 version number to use later
      TString cms3_version = cms3.evt_CMS3tag().at(0);     
      //TString cms3_version = "CMS4_V09-04-17";
      // convert last two digits of version number to int
      int small_cms3_version = TString(cms3_version(cms3_version.Length()-2,cms3_version.Length())).Atoi();
      bool recent_cms3_version = true;
      if (cms3_version.Contains("V08-00") && small_cms3_version <= 12) recent_cms3_version = false;
      // bool isCMS4 = cms3_version.Contains("CMS4");
      bool isCMS4 = true; // FIXME: hardcode to get around buggy CMS3tag in some files. Shouldn't be running over cms3 anyway
      if (cms3_version.Contains("V10-01-00")) doShortTrackInfo = false;
      if (cms3_version.Contains("V00-00-02")) doShortTrackInfo = false;
      if (cms3_version.Contains("V00-00-03")) doShortTrackInfo = false;

      run  = cms3.evt_run();
      lumi = cms3.evt_lumiBlock();
      evt  = cms3.evt_event();

      isGolden = -1;
      if ( isData && applyJSON ) {
	if ( goodrun(run, lumi) ) isGolden = 1;
	else isGolden = 0;
      }

      if(!isGolden && !hasAllBranches){
          cout << "WARNING: not all branches found in tree, but event isn't in golden json anyway so skipping." << endl;
          FillBabyNtuple();
          continue;
      }

      if (verbose) cout << "before trigger" << endl;

      // for now 2017 fastsim has not trigger info.
      if(config_tag != "mc_94x_fastsim_Fall17"){
          //TRIGGER - check first to enable cuts
          HLT_PFHT1050        = passHLTTriggerPattern("HLT_PFHT1050_v");
          HLT_PFHT900         = passHLTTriggerPattern("HLT_PFHT900_v");
          HLT_PFMET170          = passHLTTriggerPattern("HLT_PFMET170_NoiseCleaned_v") || passHLTTriggerPattern("HLT_PFMET170_JetIdCleaned_v") || passHLTTriggerPattern("HLT_PFMET170_HBHECleaned_v") || passHLTTriggerPattern("HLT_PFMET170_NotCleaned_v"); 
          HLT_PFHT300_PFMET100  = passHLTTriggerPattern("HLT_PFHT300_PFMET100_v"); 
          HLT_PFHT300_PFMET110  = passHLTTriggerPattern("HLT_PFHT300_PFMET110_v"); // 2016
          HLT_PFHT350_PFMET100  = passHLTTriggerPattern("HLT_PFHT350_PFMET100_NoiseCleaned_v") || passHLTTriggerPattern("HLT_PFHT350_PFMET100_JetIdCleaned_v") || passHLTTriggerPattern("HLT_PFHT350_PFMET100_v"); 
          HLT_PFHT350_PFMET120  = passHLTTriggerPattern("HLT_PFHT350_PFMET120_NoiseCleaned_v") || passHLTTriggerPattern("HLT_PFHT350_PFMET120_JetIdCleaned_v"); 
          HLT_PFHT500_PFMET100_PFMHT100 = passHLTTriggerPattern("HLT_PFHT500_PFMET100_PFMHT100_IDTight_v");
          HLT_PFHT700_PFMET85_PFMHT85 = passHLTTriggerPattern("HLT_PFHT700_PFMET85_PFMHT85_IDTight_v");
          HLT_PFHT800_PFMET75_PFMHT75 = passHLTTriggerPattern("HLT_PFHT800_PFMET75_PFMHT75_IDTight_v");
          HLT_PFMETNoMu90_PFMHTNoMu90   = passHLTTriggerPattern("HLT_PFMETNoMu90_JetIdCleaned_PFMHTNoMu90_IDTight_v") || passHLTTriggerPattern("HLT_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight_v") || passHLTTriggerPattern("HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_v");
          HLT_MonoCentralPFJet80_PFMETNoMu90_PFMHTNoMu90   = passHLTTriggerPattern("HLT_MonoCentralPFJet80_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight_v") || passHLTTriggerPattern("HLT_MonoCentralPFJet80_PFMETNoMu90_JetIdCleaned_PFMHTNoMu90_IDTight_v") || passHLTTriggerPattern("HLT_MonoCentralPFJet80_PFMETNoMu90_PFMHTNoMu90_IDTight_v");
          HLT_PFMETNoMu100_PFMHTNoMu100 = passHLTTriggerPattern("HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_v");
          HLT_PFMETNoMu110_PFMHTNoMu110 = passHLTTriggerPattern("HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_v");
          HLT_PFMETNoMu120_PFMHTNoMu120 = passHLTTriggerPattern("HLT_PFMETNoMu120_JetIdCleaned_PFMHTNoMu120_IDTight_v") || passHLTTriggerPattern("HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight_v") || passHLTTriggerPattern("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_v");
          HLT_PFMETNoMu130_PFMHTNoMu130 = passHLTTriggerPattern("HLT_PFMETNoMu130_JetIdCleaned_PFMHTNoMu130_IDTight_v") || passHLTTriggerPattern("HLT_PFMETNoMu130_NoiseCleaned_PFMHTNoMu130_IDTight_v") || passHLTTriggerPattern("HLT_PFMETNoMu130_PFMHTNoMu130_IDTight_v");
          HLT_PFMETNoMu140_PFMHTNoMu140 = passHLTTriggerPattern("HLT_PFMETNoMu140_JetIdCleaned_PFMHTNoMu140_IDTight_v") || passHLTTriggerPattern("HLT_PFMETNoMu140_NoiseCleaned_PFMHTNoMu140_IDTight_v") || passHLTTriggerPattern("HLT_PFMETNoMu140_PFMHTNoMu140_IDTight_v");
          HLT_PFMETNoMu120_PFMHTNoMu120_PFHT60 = passHLTTriggerPattern("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60_v");
          HLT_PFMET90_PFMHT90           = passHLTTriggerPattern("HLT_PFMET90_PFMHT90_IDTight_v") || passHLTTriggerPattern("HLT_PFMET90_PFMHT90_IDLoose_v");
          HLT_PFMET100_PFMHT100         = passHLTTriggerPattern("HLT_PFMET100_PFMHT100_IDTight_v");
          HLT_PFMET110_PFMHT110         = passHLTTriggerPattern("HLT_PFMET110_PFMHT110_IDTight_v");
          HLT_PFMET120_PFMHT120         = passHLTTriggerPattern("HLT_PFMET120_PFMHT120_IDTight_v");
          HLT_PFMET130_PFMHT130         = passHLTTriggerPattern("HLT_PFMET130_PFMHT130_IDTight_v");
          HLT_PFMET140_PFMHT140         = passHLTTriggerPattern("HLT_PFMET140_PFMHT140_IDTight_v");
          HLT_PFMET100_PFMHT100_PFHT60  = passHLTTriggerPattern("HLT_PFMET100_PFMHT100_IDTight_PFHT60_v");
          HLT_PFMET120_PFMHT120_PFHT60  = passHLTTriggerPattern("HLT_PFMET120_PFMHT120_IDTight_PFHT60_v");
          HLT_PFJet450        = passHLTTriggerPattern("HLT_PFJet450_v"); // 2016
          HLT_PFJet500        = passHLTTriggerPattern("HLT_PFJet500_v"); // 2017,18
          HLT_ECALHT800       = passHLTTriggerPattern("HLT_ECALHT800_v");
          HLT_SingleMu     = passHLTTriggerPattern("HLT_IsoMu17_eta2p1_v") ||
              passHLTTriggerPattern("HLT_IsoMu20_v") || passHLTTriggerPattern("HLT_IsoMu20_eta2p1_v") ||
              passHLTTriggerPattern("HLT_IsoTkMu20_v") || passHLTTriggerPattern("HLT_IsoTkMu20_eta2p1_v") ||
              passHLTTriggerPattern("HLT_IsoMu24_v") || passHLTTriggerPattern("HLT_IsoTkMu24_v") || passHLTTriggerPattern("HLT_IsoMu24_eta2p1_v") ||
              passHLTTriggerPattern("HLT_IsoMu27_v") || passHLTTriggerPattern("HLT_IsoTkMu27_v");
          HLT_SingleMu_NonIso     = passHLTTriggerPattern("HLT_Mu50_v") || passHLTTriggerPattern("HLT_TkMu50_v") ||
              passHLTTriggerPattern("HLT_Mu55_v");
          HLT_SingleEl     = 
              passHLTTriggerPattern("HLT_Ele27_eta2p1_WPTight_Gsf_v") || // 2016
              passHLTTriggerPattern("HLT_Ele32_eta2p1_WPTight_Gsf_v") || // 2016
              passHLTTriggerPattern("HLT_Ele27_WPTight_Gsf_v") ||
              passHLTTriggerPattern("HLT_Ele32_WPTight_Gsf_v") ||  // 2017,18
              passHLTTriggerPattern("HLT_Ele35_WPTight_Gsf_v") ||  // 2017,18
              passHLTTriggerPattern("HLT_Ele38_WPTight_Gsf_v") ||  // 2017,18
              passHLTTriggerPattern("HLT_Ele28_eta2p1_WPTight_Gsf_HT150_v"); // 2016
          HLT_SingleEl_NonIso     = passHLTTriggerPattern("HLT_Ele105_CaloIdVT_GsfTrkIdT_v") || //2016
              passHLTTriggerPattern("HLT_Ele115_CaloIdVT_GsfTrkIdT_v");  // 2017,18
          HLT_DoubleEl     = passHLTTriggerPattern("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"); // 2016,17,18
          HLT_DoubleEl33   = passHLTTriggerPattern("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v") || // 2016
              passHLTTriggerPattern("HLT_DoubleEle33_CaloIdL_MW_v");  // 2016,17,18
          HLT_MuX_Ele12 = passHLTTriggerPattern("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v") || // 2016,17,18
              passHLTTriggerPattern("HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v") ||  // 2016
              passHLTTriggerPattern("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"); // 2016,17,18
          HLT_Mu8_EleX = passHLTTriggerPattern("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_v") || // 2016,17,18
              passHLTTriggerPattern("HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL_v") || // 2016
              passHLTTriggerPattern("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v");  // 2016
          HLT_Mu12_EleX = passHLTTriggerPattern("HLT_Mu12_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL_v") ||
              passHLTTriggerPattern("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v"); // 2017,18
          HLT_Mu30_Ele30_NonIso = passHLTTriggerPattern("HLT_Mu30_Ele30_CaloIdL_GsfTrkIdVL_v"); // 2016
          HLT_Mu33_Ele33_NonIso = passHLTTriggerPattern("HLT_Mu33_Ele33_CaloIdL_GsfTrkIdVL_v"); // 2016
          HLT_Mu37_Ele27_NonIso = passHLTTriggerPattern("HLT_Mu37_Ele27_CaloIdL_MW_v"); // 2017,18
          HLT_Mu27_Ele37_NonIso = passHLTTriggerPattern("HLT_Mu27_Ele37_CaloIdL_MW_v"); // 2017,18
          HLT_DoubleMu     = 
              passHLTTriggerPattern("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v") ||    // 2016
              passHLTTriggerPattern("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v") ||  // 2016
              passHLTTriggerPattern("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v") ||  // 2016
              passHLTTriggerPattern("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v") ||  // 2016
              passHLTTriggerPattern("HLT_TkMu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v") ||  // 2016
              passHLTTriggerPattern("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v") || //2017,18
              passHLTTriggerPattern("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_v") ||   //2017,18
              passHLTTriggerPattern("HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_v") ||
              passHLTTriggerPattern("HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_v") ||
              passHLTTriggerPattern("HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8_v") ||
              passHLTTriggerPattern("HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8_v");
          HLT_DoubleMu_NonIso     = 
              passHLTTriggerPattern("HLT_Mu30_TkMu11_v") || // 2016 
              passHLTTriggerPattern("HLT_Mu40_TkMu11_v") || // 2016
              passHLTTriggerPattern("HLT_Mu37_TkMu27_v");   // 2017,18
          HLT_Photon120 = passHLTTriggerPattern("HLT_Photon120_v"); 
          HLT_Photon200 = passHLTTriggerPattern("HLT_Photon200_v"); // 2017,18
          HLT_Photon175_Prescale = passHLTTriggerPattern("HLT_Photon175_v") ? HLT_prescale(triggerName("HLT_Photon175_v"), false) : 0; 
          HLT_Photon165_HE10 = passHLTTriggerPattern("HLT_Photon165_HE10_v"); // 2016
          HLT_Photon250_NoHE = passHLTTriggerPattern("HLT_Photon250_NoHE_v"); 
          HLT_DiCentralPFJet70_PFMET120  = passHLTTriggerPattern("HLT_DiCentralPFJet70_PFMET120_NoiseCleaned_v") || passHLTTriggerPattern("HLT_DiCentralPFJet70_PFMET120_JetIdCleaned_v"); 
          HLT_DiCentralPFJet55_PFMET110  = passHLTTriggerPattern("HLT_DiCentralPFJet55_PFMET110_NoiseCleaned_v") || passHLTTriggerPattern("HLT_DiCentralPFJet55_PFMET110_JetIdCleaned_v"); 
          // 2017,18 prescaled HT triggers
      
          HLT_PFHT180_Prescale  = passHLTTriggerPattern("HLT_PFHT180_v") ? HLT_prescale(triggerName("HLT_PFHT180_v"), true) : 0; 
          HLT_PFHT250_Prescale  = passHLTTriggerPattern("HLT_PFHT250_v") ? HLT_prescale(triggerName("HLT_PFHT250_v"), true) : 0; 
          HLT_PFHT370_Prescale  = passHLTTriggerPattern("HLT_PFHT370_v") ? HLT_prescale(triggerName("HLT_PFHT370_v"), true) : 0; 
          HLT_PFHT430_Prescale  = passHLTTriggerPattern("HLT_PFHT430_v") ? HLT_prescale(triggerName("HLT_PFHT430_v"), true) : 0; 
          HLT_PFHT510_Prescale  = passHLTTriggerPattern("HLT_PFHT510_v") ? HLT_prescale(triggerName("HLT_PFHT510_v"), false) : 0; // PFHT510 and up have broken hlt_l1prescales for Run2017C.
          HLT_PFHT590_Prescale  = passHLTTriggerPattern("HLT_PFHT590_v") ? HLT_prescale(triggerName("HLT_PFHT590_v"), false) : 0; // this will work as long as they have unprescaled L1 
          HLT_PFHT680_Prescale  = passHLTTriggerPattern("HLT_PFHT680_v") ? HLT_prescale(triggerName("HLT_PFHT680_v"), false) : 0; 
          HLT_PFHT780_Prescale  = passHLTTriggerPattern("HLT_PFHT780_v") ? HLT_prescale(triggerName("HLT_PFHT780_v"), false) : 0; 
          HLT_PFHT890_Prescale  = passHLTTriggerPattern("HLT_PFHT890_v") ? HLT_prescale(triggerName("HLT_PFHT890_v"), false) : 0; 
          // 2016 prescaled HT triggers
          HLT_PFHT125_Prescale  = passHLTTriggerPattern("HLT_PFHT125_v") ? HLT_prescale(triggerName("HLT_PFHT125_v")) : 0; 
          HLT_PFHT200_Prescale  = passHLTTriggerPattern("HLT_PFHT200_v") ? HLT_prescale(triggerName("HLT_PFHT200_v")) : 0; 
          HLT_PFHT300_Prescale  = passHLTTriggerPattern("HLT_PFHT300_v") ? HLT_prescale(triggerName("HLT_PFHT300_v")) : 0; 
          HLT_PFHT350_Prescale  = passHLTTriggerPattern("HLT_PFHT350_v") ? HLT_prescale(triggerName("HLT_PFHT350_v")) : 0; 
          HLT_PFHT475_Prescale  = passHLTTriggerPattern("HLT_PFHT475_v") ? HLT_prescale(triggerName("HLT_PFHT475_v")) : 0; 
          HLT_PFHT600_Prescale  = passHLTTriggerPattern("HLT_PFHT600_v") ? HLT_prescale(triggerName("HLT_PFHT600_v")) : 0;

          if (!isData && applyTriggerCuts && !(HLT_PFHT1050 || HLT_PFHT350_PFMET120 || HLT_Photon165_HE10 || HLT_SingleMu 
                                               || HLT_DoubleMu || HLT_DoubleEl || HLT_MuX_Ele12 || HLT_Mu8_EleX)) continue;

      }

      // set jet corrector based on run number for data (late 2016F has special corrections; everything else determined by era)
      if (isData && run >= 278802 && run <= 278808) {
	jet_corrector_pfL1FastJetL2L3_current = jet_corrector_pfL1FastJetL2L3_postrun278802;
      } else {
	jet_corrector_pfL1FastJetL2L3_current = jet_corrector_pfL1FastJetL2L3;
      }
      
      if (!removePostProcVars) {
        if(isCMS4){
          if(!isData){
            const string dataset_name = cms3.evt_dataset().at(0).Data();
	    try {
	      evt_scale1fb = xsec_corr_ * datasetInfoFromFile.getScale1fbFromFile(dataset_name, cms3_version.Data());
	      evt_nEvts    = datasetInfoFromFile.getnEventsTotalFromFile(dataset_name, cms3_version.Data());
	      evt_xsec     = xsec_corr_ * datasetInfoFromFile.getXsecFromFile(dataset_name, cms3_version.Data());
              if(cms3.genps_weight() < 0.0)
                  evt_scale1fb *= -1.0;
	    } catch (const exception& e) {
	      if (event == 0) {
		cout << "WARNING: " << e.what() << endl;
		// cout << "WARNING: Setting evt_nEvts to 0, evt_scale1fb to 1.0, evt_xsec to 0" << endl;
                if(dataset_name.find("SMS-") == string::npos){
                    cout << "ABORTING. This is for safety so we don't get silent scale1fb errors when babymaking. Comment out if you want to proceed." << endl;
                    return;
                }
	      }
	      evt_nEvts = 0;
	      evt_scale1fb = 1.0;
	      evt_xsec = 0.0;
	    }
	    evt_kfactor = 1.0;
	    evt_filter = 1.0;
          }else{
            evt_nEvts = 0;
            evt_scale1fb = 1.0;
            evt_xsec = 0.;
            evt_kfactor = 1.;
            evt_filter = 1.;
          }

        }else{
	  evt_nEvts = cms3.evt_nEvts();
          evt_scale1fb = cms3.evt_scale1fb();
          evt_xsec = cms3.evt_xsec_incl();
          evt_kfactor = cms3.evt_kfactor();
          evt_filter = cms3.evt_filt_eff();
        }
      }
      if (!isData) {
        genWeight = cms3.genps_weight();
        if(config_.year==2016 || config_.year==2017)
            std::tie(weight_L1prefire, weight_L1prefire_UP, weight_L1prefire_DN) = getPrefireInfo(config_.year);
        puWeight = 1.;
        nTrueInt = -1;
	if (cms3.puInfo_trueNumInteractions().size() > 0) nTrueInt = cms3.puInfo_trueNumInteractions().at(0);
      }

      rho = cms3.evt_fixgridfastjet_all_rho(); //this one is used in JECs

      if (verbose) cout << "before vertices" << endl;

      //VERTICES
      nVert = 0;
      for(unsigned int ivtx=0; ivtx < cms3.evt_nvtxs(); ivtx++){
        if(cms3.vtxs_isFake().at(ivtx)) continue;
        if(cms3.vtxs_ndof().at(ivtx) <= 4) continue;
        if(fabs(cms3.vtxs_position().at(ivtx).z()) > 24) continue;
        if(cms3.vtxs_position().at(ivtx).Rho() > 2) continue;

        nVert++;

      }

      //check CMS version to see if we want to use MuEGCleaned MET and turn off JEC corrections for reminiAOD data
      bool useMuEGCleanedMet = false;
      bool skipJECOnMet = false; //we are out of sync with ETH on MET unless we pull directly from reminiAOD
      if (!isCMS4 && small_cms3_version >= 18) {
	useMuEGCleanedMet = true;
	skipJECOnMet = true;
      }
      if (verbose) {
	if (useMuEGCleanedMet) cout << "Using mueg cleaned MET" << endl;
	else cout << "Not using mueg cleaned MET" << endl;
      }
      
      if (applyJECfromFile && recomputeT1MET && !skipJECOnMet) {
      // if (applyJECfromFile && recomputeT1MET) {
	std::pair <float, float> t1metUP;
	std::pair <float, float> t1metDN;
	std::pair <float, float> t1met;
	std::pair <float, float> t1met_old2017;
        int do_2017metfix = config_.year==2017 ? 2 : 0;
	if (!isData) {
	  t1metUP = getT1CHSMET_fromMINIAOD(jet_corrector_pfL1FastJetL2L3_current, jetcorr_uncertainty, 1,doRecomputeRawPFMET_);
	  t1metDN = getT1CHSMET_fromMINIAOD(jet_corrector_pfL1FastJetL2L3_current, jetcorr_uncertainty, 0,doRecomputeRawPFMET_);
	  t1met = getT1CHSMET_fromMINIAOD(jet_corrector_pfL1FastJetL2L3_current, 0, 0,doRecomputeRawPFMET_, do_2017metfix);
	  t1met_old2017 = getT1CHSMET_fromMINIAOD(jet_corrector_pfL1FastJetL2L3_current, 0, 0,doRecomputeRawPFMET_, 0);
	}
	else if (useMuEGCleanedMet) {
	  t1metUP = getT1CHSMET_fromMINIAOD(jet_corrector_pfL1FastJetL2L3_current, NULL, 0, 0, 1); // never apply variations to data
	  t1metDN = getT1CHSMET_fromMINIAOD(jet_corrector_pfL1FastJetL2L3_current, NULL, 0, 0, 1); // never apply variations to data
	  t1met = getT1CHSMET_fromMINIAOD(jet_corrector_pfL1FastJetL2L3_current, NULL, 0, 0, 1); // never apply variations to data
	  t1met_old2017 = getT1CHSMET_fromMINIAOD(jet_corrector_pfL1FastJetL2L3_current, NULL, 0, 0, 1); // never apply variations to data
	}
	else {
	  t1metUP = getT1CHSMET_fromMINIAOD(jet_corrector_pfL1FastJetL2L3_current); // never apply variations to data
	  t1metDN = getT1CHSMET_fromMINIAOD(jet_corrector_pfL1FastJetL2L3_current); // never apply variations to data
	  t1met = getT1CHSMET_fromMINIAOD(jet_corrector_pfL1FastJetL2L3_current, NULL, 0, 0, do_2017metfix); // never apply variations to data
	  t1met_old2017 = getT1CHSMET_fromMINIAOD(jet_corrector_pfL1FastJetL2L3_current, NULL, 0, 0, 0); // never apply variations to data
	}
	met_pt  = t1met.first;
	met_phi = t1met.second;
        met_old2017_pt = t1met_old2017.first;
        met_old2017_phi = t1met_old2017.second;
	met_ptJECup  = t1metUP.first;
	met_phiJECup = t1metUP.second;
	met_ptJECdn  = t1metDN.first;
	met_phiJECdn = t1metDN.second;
      }
      else if (useMuEGCleanedMet){
	met_pt  = cms3.evt_muegclean_pfmet();
	met_phi = cms3.evt_muegclean_pfmetPhi();
	met_old2017_pt  = cms3.evt_muegclean_pfmet();
	met_old2017_phi = cms3.evt_muegclean_pfmetPhi();
      }
      else {
	met_pt  = cms3.evt_pfmet();
	met_phi = cms3.evt_pfmetPhi();
	met_old2017_pt  = cms3.evt_old_pfmet();
	met_old2017_phi = cms3.evt_old_pfmetPhi();
      }
      met_rawPt  = cms3.evt_pfmet_raw();
      met_rawPhi = cms3.evt_pfmetPhi_raw();
      if (!isData) {
        met_genPt  = cms3.gen_met();
        met_genPhi = cms3.gen_metPhi();
      }
      met_caloPt  = cms3.evt_calomet();
      met_caloPhi = cms3.evt_calometPhi();
      // metStruct trkmet = trackerMET(0.1);
      // met_trkPt = trkmet.met;
      // met_trkPhi = trkmet.metphi;
      if (useMuEGCleanedMet){
	met_miniaodPt  = cms3.evt_muegclean_pfmet();
	met_miniaodPhi = cms3.evt_muegclean_pfmetPhi();
      }
      else {	
	met_miniaodPt  = cms3.evt_pfmet();
	met_miniaodPhi = cms3.evt_pfmetPhi();
      }
      
      // MET FILTERS -- not present in fastsim
      if (!isFastsim) {
	Flag_EcalDeadCellTriggerPrimitiveFilter       = cms3.filt_ecalTP();
	Flag_trkPOG_manystripclus53X                  = cms3.filt_trkPOG_manystripclus53X();
	Flag_ecalLaserCorrFilter                      = cms3.filt_ecalLaser();
	Flag_trkPOG_toomanystripclus53X               = cms3.filt_trkPOG_toomanystripclus53X();
	Flag_hcalLaserEventFilter                     = cms3.filt_hcalLaser();
	Flag_trkPOG_logErrorTooManyClusters           = cms3.filt_trkPOG_logErrorTooManyClusters();
	Flag_trkPOGFilters                            = cms3.filt_trkPOGFilters();
	Flag_trackingFailureFilter                    = cms3.filt_trackingFailure();
	Flag_goodVertices                             = cms3.filt_goodVertices();
	Flag_eeBadScFilter                            = cms3.filt_eeBadSc();
	// note: in CMS3, filt_cscBeamHalo and evt_cscTightHaloId are the same
	Flag_CSCTightHaloFilter                       = cms3.filt_cscBeamHalo();
	Flag_CSCTightHalo2015Filter                   = cms3.filt_cscBeamHalo2015();
	// note: in CMS3, filt_hbheNoise and evt_hbheFilter are the same
	Flag_HBHENoiseFilter                          = cms3.filt_hbheNoise();
	Flag_HBHENoiseIsoFilter                       = cms3.filt_hbheNoiseIso();                
        if(config_.filters["ecalBadCalibFilter"])        Flag_ecalBadCalibFilter        = cms3.filt_ecalBadCalibFilter();
        if(config_.filters["ecalBadCalibFilterUpdate"] && !cms3_version.Contains("V09"))  Flag_ecalBadCalibFilterUpdate  = cms3.filt_ecalBadCalibFilterUpdate();
        if(config_.filters["badMuonFilter"])             Flag_badMuonFilter             = cms3.filt_BadPFMuonFilter();
        if(config_.filters["badChargedCandidateFilter"]) Flag_badChargedCandidateFilter = cms3.filt_BadChargedCandidateFilter();
        Flag_globalTightHalo2016Filter                = cms3.filt_globalTightHalo2016();
        Flag_globalSuperTightHalo2016Filter           = cms3.filt_globalSuperTightHalo2016();
        // these were applied manually (post-miniaod) in 2016. Don't think they work in CMS4 anymore
        // since we don't store all PFCands. Should now just be able to use the filters from miniAOD above
        Flag_badMuonFilterV2                        = badMuonFilterV2();         
        Flag_badChargedHadronFilterV2               = badChargedCandidateFilterV2();          
        Flag_METFilters                               = cms3.filt_metfilter();
      }
      
      // gen block -- for MC only
      ngenPart = 0;
      ngenLep = 0;
      ngenStat23 = 0;
      ngenGamma = 0;
      ngenTau = 0;
      ngenTau1Prong = 0;
      ngenTau3Prong = 0;
      ngenLepFromTau = 0;
      ngenLepFromZ = 0;
      ngenNuFromZ = 0;
      if (!isData) {
	
        if (verbose) cout << "before sparm values" << endl;
	
	// assume that first sparm value is parent mass, second is LSP mass
	if (evt_id >= 1000 && evt_id < 1200) {
	  if (sparm_values().size() == 2) {
	    GenSusyMScan1 = sparm_values().at(0);
	    GenSusyMScan2 = sparm_values().at(1);
	    // use sparm values to look up xsec
	    evt_xsec = h_sig_xsec->GetBinContent(h_sig_xsec->FindBin(GenSusyMScan1));
	  }
	  else {
	    std::cout << "WARNING: expected to find 2 sparm values, found instead " << sparm_values().size() << std::endl;
	  }
	}

	// get filter efficiency value for T2cc sample
	if (evt_id == 1130) {
	  evt_filter = cms3.sparm_filterEfficiency();
	}

        if (verbose) cout << "before gen particles" << endl;

        //GEN PARTICLES
        LorentzVector recoil(0,0,0,0);
        int nHardScatter = 0;
	genTop_pt = -1.;
	genTbar_pt = -1.;
	if (cms3.genps_p4().size() != cms3.genps_isLastCopy().size()) {
            std::cout << "WARNING: The gen particle isLastCopy branch does not have the same number of entries as the other gen particle branches. This can happen when you run on cms4 including the packed gen particles, which are appended to a subset of gen particle branches. Add a protection to the isLastCopy access line or modify the NtupleMaker and reproduce cms4 if you wish to avoid the exception that's about to happen.";
	}
        for(unsigned int iGen = 0; iGen < cms3.genps_p4().size(); iGen++){
          if (ngenPart >= max_ngenPart) {
            std::cout << "WARNING: attempted to fill more than " << max_ngenPart << " gen particles" << std::endl;
            break;
          }
          genPart_pt[ngenPart] = cms3.genps_p4().at(iGen).pt();
          genPart_eta[ngenPart] = cms3.genps_p4().at(iGen).eta();
          genPart_phi[ngenPart] = cms3.genps_p4().at(iGen).phi();
          genPart_mass[ngenPart] = cms3.genps_p4().at(iGen).M();
          genPart_pdgId[ngenPart] = cms3.genps_id().at(iGen);
          genPart_status[ngenPart] = cms3.genps_status().at(iGen);
          genPart_charge[ngenPart] = cms3.genps_charge().at(iGen);
          genPart_motherId[ngenPart] = cms3.genps_id_simplemother().at(iGen);
          genPart_grandmotherId[ngenPart] = cms3.genps_id_simplegrandma().at(iGen);
          ngenPart++;

          int pdgId = abs(cms3.genps_id().at(iGen));
          int status = cms3.genps_status().at(iGen);	  
          int isLastCopy = cms3.genps_isLastCopy().at(iGen);

          // find hard scatter products to get recoil pt
          if (evt_id >= 300 && evt_id < 400) {
            // ttbar
            if (isLastCopy == 1 && pdgId == 6) {
              recoil += cms3.genps_p4().at(iGen);
	      if (cms3.genps_id().at(iGen) == 6) genTop_pt = cms3.genps_p4().at(iGen).pt();
	      else if (cms3.genps_id().at(iGen) == -6) genTbar_pt = cms3.genps_p4().at(iGen).pt();
              ++nHardScatter;
            }
            if (nHardScatter > 2) std::cout << "WARNING: found too many tops in ttbar MC!" << std::endl;
          }
          else if (evt_id >= 500 && evt_id < 600) {
            // W+jets
            if (isLastCopy == 1 && pdgId == 24) {
              recoil += cms3.genps_p4().at(iGen);
              ++nHardScatter;
            }
            if (nHardScatter > 1) std::cout << "WARNING: found too many Ws in Wjets MC!" << std::endl;
          }
          else if (evt_id >= 1000 && evt_id < 1100) {
            // SMS T1 and T5 models - gluinos
            if (isLastCopy == 1 && pdgId == 1000021) {
	      genProd_pdgId = pdgId;
              recoil += cms3.genps_p4().at(iGen);
              ++nHardScatter;
            }
            if (nHardScatter > 2) std::cout << "WARNING: found too many gluinos in T1 MC!" << std::endl;
          }
          else if ((evt_id >= 1100 && evt_id < 1110) || (evt_id >= 1140 && evt_id < 1160)) {
            // SMS T2tt, T2bW, T2bt - stops
            if (isLastCopy == 1 && pdgId == 1000006) {
	      genProd_pdgId = pdgId;
              recoil += cms3.genps_p4().at(iGen);
              ++nHardScatter;
            }
            if (nHardScatter > 2) std::cout << "WARNING: found too many stops in T2tt/T2bW/T2bt MC!" << std::endl;
          }
          else if (evt_id >= 1110 && evt_id < 1120) {
            // SMS T2qq - squarks
            if (isLastCopy == 1 && ( (pdgId >= 1000001 && pdgId <= 1000004) || (pdgId >= 2000001 && pdgId <= 2000004) ) ) {
	      genProd_pdgId = pdgId;
              recoil += cms3.genps_p4().at(iGen);
              ++nHardScatter;
            }
            if (nHardScatter > 2) std::cout << "WARNING: found too many squarks in T2qq MC!" << std::endl;
          }
          else if (evt_id >= 1120 && evt_id < 1130) {
            // SMS T2bb - sbottoms
            if (isLastCopy == 1 && pdgId == 1000005) {
	      genProd_pdgId = pdgId;
              recoil += cms3.genps_p4().at(iGen);
              ++nHardScatter;
            }
            if (nHardScatter > 2) std::cout << "WARNING: found too many sbottoms in T2bb MC!" << std::endl;
          }
          else if (evt_id >= 1130 && evt_id < 1140) {
            // SMS T2cc - stops (decaying to charm + LSP)
            if (isLastCopy == 1 && pdgId == 1000006) {
	      genProd_pdgId = pdgId;
              recoil += cms3.genps_p4().at(iGen);
              ++nHardScatter;
            }
            if (nHardScatter > 2) std::cout << "WARNING: found too many stops in T2cc MC!" << std::endl;
          }

          int motherId = abs(cms3.genps_id_simplemother().at(iGen));
          int grandmotherId = abs(cms3.genps_id_simplegrandma().at(iGen));

          // save all status 22 or 23 gen particles
          if (status == 23 || status == 22) {
            genStat23_pt[ngenStat23] = cms3.genps_p4().at(iGen).pt();
            genStat23_eta[ngenStat23] = cms3.genps_p4().at(iGen).eta();
            genStat23_phi[ngenStat23] = cms3.genps_p4().at(iGen).phi();
            genStat23_mass[ngenStat23] = cms3.genps_p4().at(iGen).M();
            genStat23_pdgId[ngenStat23] = cms3.genps_id().at(iGen);
            genStat23_status[ngenStat23] = cms3.genps_status().at(iGen);
            genStat23_charge[ngenStat23] = cms3.genps_charge().at(iGen);
            genStat23_sourceId[ngenStat23] = getSourceId(iGen);
            ++ngenStat23;
          }

          // save gen gammas good for matching
          bool goodGamma = false;
          //gamma: status 1 matched to mother photon or proton
          if (pdgId == 22 && status == 1) { 
            if (motherId <= 22  || motherId == 2212) {
              goodGamma = true;
            } 
          }

          if (goodGamma) {
            genGamma_pt[ngenGamma] = cms3.genps_p4().at(iGen).pt();
            genGamma_eta[ngenGamma] = cms3.genps_p4().at(iGen).eta();
            genGamma_phi[ngenGamma] = cms3.genps_p4().at(iGen).phi();
            genGamma_motherId[ngenGamma] = cms3.genps_id_simplemother().at(iGen);
            ++ngenGamma;
          }

          // save lepton / neutrino info
          if (pdgId < 11 || pdgId > 16) continue;

          // reject leptons with direct parents of quarks or hadrons. 
          //  Allow SUSY parents - not explicitly checking for now though
          if (motherId <= 5 || (motherId > 100 && motherId < 1000000)) continue;

          bool goodLep = false;
          bool goodLepFromZ = false;
          bool goodTau = false;
	  int decayMode = 0;
	  int neutralDaughters = 0;
	  float leadTrackPt = 0.;
          bool goodLepFromTau = false;
          int sourceId = 0;

          // electrons, muons: status 1 or 23 and mother W/Z/H or tau from W/Z/H
          if ( (pdgId == 11 || pdgId == 13) && (status == 1 || status == 23) ) {
            // save leptons pre-FSR: prefer status 23 over status 1
            if (status == 1 && motherId == pdgId && (cms3.genps_status().at(cms3.genps_idx_simplemother().at(iGen)) == 23)) {
              // don't save
            }
            // leptons from taus -- also allow gluon grandparants for ttZ etc
	    //            else if (motherId == 15 && (grandmotherId == 25 || grandmotherId == 24 || grandmotherId == 23 || grandmotherId == 21 || grandmotherId == 15)) {
            else if (motherId == 15 && (grandmotherId == 25 || grandmotherId == 24 || grandmotherId == 23 || grandmotherId == 15)) {
              goodLepFromTau = true;
            } 
            // leptons from W/Z/H -- also allow gluons for ttZ etc
	    //            else if (motherId == 25 || motherId == 24 || motherId == 23 || motherId == 21) {
            else if (motherId == 25 || motherId == 24 || motherId == 23) {
              goodLep = true;
	      // leptons in ttZ can have gluon mothers..
	      //	      if (motherId == 23 || motherId == 21) goodLepFromZ = true;
	      if (motherId == 23) goodLepFromZ = true;
            } 
          } // status 1 e or mu

          // taus: status 2 or 23, from W/Z/H
          else if (pdgId == 15 && (status == 2 || status == 23)) {
            // save leptons pre-FSR: prefer status 23 over status 2
            if (status == 2 && motherId == pdgId && (cms3.genps_status().at(cms3.genps_idx_simplemother().at(iGen)) == 23)) {
              // don't save
            }
            // leptons from W/Z/H -- also allow gluon grandparants for ttZ etc
	    //            else if (motherId == 25 || motherId == 24 || motherId == 23 || motherId == 21) {
            else if (motherId == 25 || motherId == 24 || motherId == 23) {
              goodTau = true;
	      // leptons in ttZ can have gluon mothers..
	      //	      if (motherId == 23 || motherId == 21) goodLepFromZ = true;
	      if (motherId == 23) goodLepFromZ = true;
            }
	    
	    // find decay mode - only count charged daughters
	    if (goodTau) {
	      int tauIdx = iGen;
	      // for status 23 taus: must find the status 2 taus for decays..
	      if (status == 23) {
		for(unsigned int jGen = 0; jGen < cms3.genps_p4().size(); jGen++) {
		  if (abs(cms3.genps_id().at(jGen)) != 15) continue;
		  if (abs(cms3.genps_id_simplemother().at(jGen)) != 15) continue;
		  if ( (cms3.genps_idx_mother().at(jGen) != int(iGen)) && (cms3.genps_idx_mother().at(jGen) != cms3.genps_idx_mother().at(iGen)) ) continue;
		  if (cms3.genps_status().at(jGen) != 2) continue;
		  tauIdx = jGen;
		  break;
		}
	      }
	      for(unsigned int jGen = 0; jGen < cms3.genps_p4().size(); jGen++) {
		int id2 = abs(cms3.genps_id().at(jGen));
		if (cms3.genps_idx_mother().at(jGen) != int(tauIdx)) continue;
		//if (cms3.genps_charge().at(jGen) == 0) continue;
		if ((cms3.genps_status().at(jGen) != 1) && (cms3.genps_status().at(jGen) != 2)) continue;
		// don't count duplicated taus as daughters
		if (id2 == 15) continue;
		if ( (id2 == 11) || (id2 == 13) ) {
		  decayMode = cms3.genps_id().at(jGen);
		  leadTrackPt = cms3.genps_p4().at(jGen).pt();
		  break;
		}
		// not a lepton: count number of prongs
		else if (abs(cms3.genps_charge().at(jGen)) == 1) {
		  ++decayMode;
		  if (cms3.genps_p4().at(jGen).pt() > leadTrackPt) leadTrackPt = cms3.genps_p4().at(jGen).pt();
		}
		// not charged: count neutral (non-neutrino) daughters
		else if ((cms3.genps_charge().at(jGen) == 0) && (id2 != 12 && id2 != 14 && id2 != 16)) {
		  ++neutralDaughters;
		}
	      }
	    } // if goodTau
	    
          } // status 2 tau

	  // neutrinos -- counting based on decays in NLO ttZ sample
	  if (pdgId == 12 || pdgId == 14 || pdgId == 16) {
	    //	    if ((status == 1 && motherId == 23) || (status == 23 && motherId == 21)) ++ngenNuFromZ;
	    if (status == 1 && motherId == 23) ++ngenNuFromZ;
	  }

          if (goodLep || goodTau || goodLepFromTau) {
            sourceId = getSourceId(iGen);
          }

	  if (goodLepFromZ) ++ngenLepFromZ;

          // save gen leptons (e/mu) directly from W/Z/H
          if (goodLep) {
            genLep_pt[ngenLep] = cms3.genps_p4().at(iGen).pt();
            genLep_eta[ngenLep] = cms3.genps_p4().at(iGen).eta();
            genLep_phi[ngenLep] = cms3.genps_p4().at(iGen).phi();
            genLep_mass[ngenLep] = cms3.genps_p4().at(iGen).M();
            genLep_pdgId[ngenLep] = cms3.genps_id().at(iGen);
            genLep_status[ngenLep] = cms3.genps_status().at(iGen);
            genLep_charge[ngenLep] = cms3.genps_charge().at(iGen);
            genLep_sourceId[ngenLep] = sourceId;
            ++ngenLep;
          }

          // save gen taus from W/Z/H
          if (goodTau) {
            genTau_pt[ngenTau] = cms3.genps_p4().at(iGen).pt();
            genTau_eta[ngenTau] = cms3.genps_p4().at(iGen).eta();
            genTau_phi[ngenTau] = cms3.genps_p4().at(iGen).phi();
            genTau_mass[ngenTau] = cms3.genps_p4().at(iGen).M();
            genTau_pdgId[ngenTau] = cms3.genps_id().at(iGen);
            genTau_status[ngenTau] = cms3.genps_status().at(iGen);
            genTau_charge[ngenTau] = cms3.genps_charge().at(iGen);
            genTau_sourceId[ngenTau] = sourceId;
            genTau_decayMode[ngenTau] = decayMode;
            genTau_leadTrackPt[ngenTau] = leadTrackPt;
            genTau_neutralDaughters[ngenTau] = neutralDaughters;
            ++ngenTau;
	    if (decayMode == 1) ++ngenTau1Prong;
	    else if (decayMode == 3) ++ngenTau3Prong;
          }

          // save gen e/mu from taus (which are from W/Z/H)
          if (goodLepFromTau) {
            genLepFromTau_pt[ngenLepFromTau] = cms3.genps_p4().at(iGen).pt();
            genLepFromTau_eta[ngenLepFromTau] = cms3.genps_p4().at(iGen).eta();
            genLepFromTau_phi[ngenLepFromTau] = cms3.genps_p4().at(iGen).phi();
            genLepFromTau_mass[ngenLepFromTau] = cms3.genps_p4().at(iGen).M();
            genLepFromTau_pdgId[ngenLepFromTau] = cms3.genps_id().at(iGen);
            genLepFromTau_status[ngenLepFromTau] = cms3.genps_status().at(iGen);
            genLepFromTau_charge[ngenLepFromTau] = cms3.genps_charge().at(iGen);
            genLepFromTau_sourceId[ngenLepFromTau] = sourceId;
            ++ngenLepFromTau;
          }
  
        } // loop over genPart


        //calculate events weights for left/right polarization in stop->top+LSP decays
        //adapted from https://github.com/cmstas/SingleLepton2012/blob/master/looper/singleLeptonLooper.cc#L46
        weight_pol_L = 1.0;
        weight_pol_R = 1.0;
        if( (baby_name.find("T2tt") != std::string::npos) && (GenSusyMScan1 - GenSusyMScan2) >= 175.0){ //on shell stop decays
          for(unsigned int iGen = 0; iGen < cms3.genps_p4().size(); iGen++){

            if(abs(cms3.genps_id().at(iGen)) > 20) continue; // expect quarks or leptons from W decay
            float status = cms3.genps_status().at(iGen);
            if(status != 23 && status != 22 && status != 1) continue;

            // Navigate upwards in the stop->top->W->fermion decay chain
            int fermion_mother_idx = cms3.genps_idx_simplemother().at(iGen);
            if(abs(cms3.genps_id().at(fermion_mother_idx))!=24) continue;
            int w_mother_idx = cms3.genps_idx_simplemother().at(fermion_mother_idx);
            while(abs(cms3.genps_id().at(w_mother_idx))==24){//go back far enough that the w is not its own mother
              w_mother_idx = cms3.genps_idx_simplemother().at(w_mother_idx);
            }
            if(abs(cms3.genps_id().at(w_mother_idx))!=6) continue;

            // We only care about the down-type fermion
            if (cms3.genps_id().at(w_mother_idx)*cms3.genps_id().at(iGen)>0) continue;

            // We also need a stop
            int top_mother_idx = cms3.genps_idx_simplemother().at(w_mother_idx);
            while(abs(cms3.genps_id().at(top_mother_idx))==6){//go back far enough that the top is not its own mother
              top_mother_idx = cms3.genps_idx_simplemother().at(top_mother_idx);
            }
            if(abs(cms3.genps_id().at(top_mother_idx))!=1000006) continue;

            // Move top and fermion to the stop center-of-mass frame
            TLorentzVector stop4;
            stop4.SetPtEtaPhiE(cms3.genps_p4().at(top_mother_idx).pt(), cms3.genps_p4().at(top_mother_idx).eta(), cms3.genps_p4().at(top_mother_idx).phi(), cms3.genps_p4().at(top_mother_idx).E());
            TVector3 betaV(-stop4.Px()/stop4.Energy(),-stop4.Py()/stop4.Energy(),-stop4.Pz()/stop4.Energy());

            TLorentzVector top4;
            top4.SetPtEtaPhiE(cms3.genps_p4().at(w_mother_idx).pt(), cms3.genps_p4().at(w_mother_idx).eta(), cms3.genps_p4().at(w_mother_idx).phi(), cms3.genps_p4().at(w_mother_idx).E());
            top4.Boost(betaV);

            TLorentzVector ferm4;
            ferm4.SetPtEtaPhiE(cms3.genps_p4().at(iGen).pt(), cms3.genps_p4().at(iGen).eta(), cms3.genps_p4().at(iGen).phi(), cms3.genps_p4().at(iGen).E());
            ferm4.Boost(betaV);


            // Do not reweight if by any reason top/fermion directions are undefined
            // This should be pathological if things are fine
            if (top4.P()<=0 || ferm4.P()<=0) {
              printf("Warning: particles at rest, no weight applied: ptop: %.3e, pf: %.3e\n", top4.P(), ferm4.P());
              continue; 
            }

            double costh = (top4.Px()*ferm4.Px()+top4.Py()*ferm4.Py()+top4.Pz()*ferm4.Pz())/top4.P()/ferm4.P();

            double weight_L = (top4.Energy()+top4.P())*(1-costh);
            double weight_R = (top4.Energy()-top4.P())*(1+costh);
            weight_pol_L *= 2*weight_L/(weight_R+weight_L);
            weight_pol_R *= 2*weight_R/(weight_R+weight_L);
          }
        }
	
	// top pt weight
	if (evt_id >= 300 && evt_id < 400) {
	  weight_toppt = topPtWeight_cutoff(genTop_pt,genTbar_pt);
	}

        // recoil pt - no longer used for "ISR" weight
	genRecoil_pt = recoil.pt();

	// store LHE weight variations
	if (saveLHEweights || saveLHEweightsScaleOnly) {
	  nLHEweight = 0;
	  for (unsigned int iwgt = 0; iwgt < cms3.genweights().size(); ++iwgt) {
	    if (saveLHEweightsScaleOnly && iwgt > 8) break;
	    if (iwgt >= max_nLHEweight) {
	      std::cout << "WARNING: attempted to fill more than " << max_nLHEweight << " LHEweights" << std::endl;
	      break;
	    }
	    LHEweight_wgt[nLHEweight] = cms3.genweights().at(iwgt);
	    ++nLHEweight;
	  }
	}

      } // !isData

      //LEPTONS
      std::vector<std::pair<int, float> > lep_pt_ordering;
      vector<float>vec_lep_pt;
      vector<float>vec_lep_eta;
      vector<float>vec_lep_phi;
      vector<float>vec_lep_mass;
      vector<float>vec_lep_charge;
      vector<int>  vec_lep_pdgId;
      vector<float>vec_lep_dxy;
      vector<float>vec_lep_dz;
      vector<int>  vec_lep_tightId;
      vector<int>  vec_lep_heepId;
      vector<float>vec_lep_highPtFit_pt;
      vector<float>vec_lep_highPtFit_eta;
      vector<float>vec_lep_highPtFit_phi;
      vector<float>vec_lep_relIso03;
      vector<float>vec_lep_relIso04;
      vector<float>vec_lep_miniRelIso;
      // vector<float>vec_lep_relIsoAn04;
      vector<int>  vec_lep_mcMatchId;
      vector<int>  vec_lep_lostHits;
      vector<int>  vec_lep_convVeto;
      vector<int>  vec_lep_tightCharge;

      vector<LorentzVector> p4sUniqueLeptons;

      vector<LorentzVector> p4sForHems;
      vector<LorentzVector> p4sForHemsUP;
      vector<LorentzVector> p4sForHemsDN;
      vector<LorentzVector> p4sForHemsGamma;
      vector<LorentzVector> p4sForHemsZll;
      vector<LorentzVector> p4sForHemsZllUP;
      vector<LorentzVector> p4sForHemsZllDN;
      vector<LorentzVector> p4sForHemsZllMT;
      vector<LorentzVector> p4sForHemsRl;

      vector<LorentzVector> p4sForDphi;
      vector<LorentzVector> p4sForDphiUP;
      vector<LorentzVector> p4sForDphiDN;
      vector<LorentzVector> p4sForDphiGamma;
      vector<LorentzVector> p4sForDphiZll;
      vector<LorentzVector> p4sForDphiZllUP;
      vector<LorentzVector> p4sForDphiZllDN;
      vector<LorentzVector> p4sForDphiZllMT;
      vector<LorentzVector> p4sForDphiRl;

      if (verbose) cout << "before electrons" << endl;

      //ELECTRONS
      nlep = 0;
      nElectrons10 = 0;
      for(unsigned int iEl = 0; iEl < cms3.els_p4().size(); iEl++){
        if(cms3.els_p4().at(iEl).pt() < 10.0) continue;
        if(fabs(cms3.els_p4().at(iEl).eta()) > 2.4) continue;
        // first check ID then iso
        if(!electronID(iEl,id_level_t::HAD_veto_noiso_v5)) continue;
        bool pass_iso = electronID(iEl,id_level_t::HAD_veto_v5);
        if(applyLeptonIso && !pass_iso) continue;
        lep_pt_ordering.push_back( std::pair<int,float>(nlep,cms3.els_p4().at(iEl).pt()) );
        vec_lep_pt.push_back ( cms3.els_p4().at(iEl).pt());
        vec_lep_eta.push_back ( cms3.els_p4().at(iEl).eta()); //save eta, even though we use SCeta for ID
        vec_lep_phi.push_back ( cms3.els_p4().at(iEl).phi());
        vec_lep_mass.push_back ( cms3.els_p4().at(iEl).M());
        vec_lep_charge.push_back ( cms3.els_charge().at(iEl));
        vec_lep_pdgId.push_back ( (-11)*cms3.els_charge().at(iEl));
        vec_lep_dxy.push_back ( cms3.els_dxyPV().at(iEl));
        vec_lep_dz.push_back ( cms3.els_dzPV().at(iEl));
        vec_lep_tightId.push_back ( eleTightID(iEl,analysis_t::HAD,5) );
        vec_lep_heepId.push_back ( isHEEPV60(iEl) );
        vec_lep_highPtFit_pt.push_back ( -1. );
        vec_lep_highPtFit_eta.push_back ( -1. );
        vec_lep_highPtFit_phi.push_back ( -1. );
        vec_lep_relIso03.push_back (  eleRelIso03(iEl,analysis_t::HAD));
        vec_lep_relIso04.push_back ( 0);
        vec_lep_miniRelIso.push_back ( elMiniRelIsoCMS3_EA(iEl, config_.ea_version) );
        // vec_lep_relIsoAn04.push_back ( elRelIsoAn04(iEl) );
        if (!isData && cms3.els_mc3dr().size() > 0 && cms3.els_mc3dr().at(iEl) < 0.2 && cms3.els_mc3idx().at(iEl) != -9999 && abs(cms3.els_mc3_id().at(iEl)) == 11) { // matched to a prunedGenParticle electron?
          int momid =  abs(genPart_motherId[cms3.els_mc3idx().at(iEl)]);
          vec_lep_mcMatchId.push_back ( momid != 11 ? momid : genPart_grandmotherId[cms3.els_mc3idx().at(iEl)]); // if mother is different store mother, otherwise store grandmother
        }
        else vec_lep_mcMatchId.push_back (0);

        vec_lep_lostHits.push_back ( cms3.els_exp_innerlayers().at(iEl)); //cms3.els_lost_pixelhits().at(iEl);
        vec_lep_convVeto.push_back ( !cms3.els_conv_vtx_flag().at(iEl));
        vec_lep_tightCharge.push_back ( tightChargeEle(iEl));

        nlep++;

        // only use isolated leptons for counters, overlaps, etc
        if (pass_iso) {
          nElectrons10++;
          p4sUniqueLeptons.push_back(cms3.els_p4().at(iEl));

          // for mt2 and mht in lepton control region
          if (doJetLepOverlapRemoval) {
            p4sForHems.push_back(cms3.els_p4().at(iEl));
            p4sForHemsUP.push_back(cms3.els_p4().at(iEl));
            p4sForHemsDN.push_back(cms3.els_p4().at(iEl));
            p4sForDphi.push_back(cms3.els_p4().at(iEl));
            p4sForDphiUP.push_back(cms3.els_p4().at(iEl));
            p4sForDphiDN.push_back(cms3.els_p4().at(iEl));
          }

	  if (!isData && applyLeptonSFs) {
	    weightStruct weights = getLepSFFromFile(cms3.els_p4().at(iEl).pt(), cms3.els_p4().at(iEl).eta(), 11);
	    weight_lepsf *= weights.cent;
	    weight_lepsf_UP *= weights.up;
	    weight_lepsf_DN *= weights.dn;
	    if (isFastsim) {
	      weightStruct weights_fastsim = getLepSFFromFile_fastsim(cms3.els_p4().at(iEl).pt(), cms3.els_p4().at(iEl).eta(), 11);
	      weight_lepsf *= weights_fastsim.cent;
	      weight_lepsf_UP *= weights_fastsim.up;
	      weight_lepsf_DN *= weights_fastsim.dn;
	    }
	  }

        } // if (pass_iso)

      }

      if (verbose) cout << "before muons" << endl;

      //MUONS
      nMuons10 = 0;
      nBadMuons20 = 0;
      for(unsigned int iMu = 0; iMu < cms3.mus_p4().size(); iMu++){
        if(cms3.mus_p4().at(iMu).pt() < 10.0) continue;
	// check for bad muons to count, only in recent CMS3 versions
	if (recent_cms3_version) {
	  if (cms3.mus_p4().at(iMu).pt() > 20.0 && isBadGlobalMuon(iMu)) ++nBadMuons20;
	}
        if(fabs(cms3.mus_p4().at(iMu).eta()) > 2.4) continue;
        // first check ID then iso
        if(!muonID(iMu,id_level_t::HAD_loose_noiso_v4)) continue;
        bool pass_iso = muonID(iMu,id_level_t::HAD_loose_v4);
        if (applyLeptonIso && !pass_iso) continue;
        lep_pt_ordering.push_back( std::pair<int,float>(nlep,cms3.mus_p4().at(iMu).pt()) );
        vec_lep_pt.push_back ( cms3.mus_p4().at(iMu).pt());
        vec_lep_eta.push_back ( cms3.mus_p4().at(iMu).eta());
        vec_lep_phi.push_back ( cms3.mus_p4().at(iMu).phi());
        vec_lep_mass.push_back ( cms3.mus_p4().at(iMu).M());
        vec_lep_charge.push_back ( cms3.mus_charge().at(iMu));
        vec_lep_pdgId.push_back ( (-13)*cms3.mus_charge().at(iMu));
        vec_lep_dxy.push_back ( cms3.mus_dxyPV().at(iMu)); // this uses the silicon track. should we use best track instead?
        vec_lep_dz.push_back ( cms3.mus_dzPV().at(iMu)); // this uses the silicon track. should we use best track instead?
        vec_lep_tightId.push_back ( muTightID(iMu,analysis_t::HAD,4) );
	if (recent_cms3_version) {
	  vec_lep_heepId.push_back ( isHighPtMuonPOG(iMu) );
	  vec_lep_highPtFit_pt.push_back ( cms3.mus_bfit_p4().at(iMu).pt());
	  vec_lep_highPtFit_eta.push_back ( cms3.mus_bfit_p4().at(iMu).eta());
	  vec_lep_highPtFit_phi.push_back ( cms3.mus_bfit_p4().at(iMu).phi());
	} else {
	  vec_lep_heepId.push_back ( -1 );
	  vec_lep_highPtFit_pt.push_back ( -1. );
	  vec_lep_highPtFit_eta.push_back ( -1. );
	  vec_lep_highPtFit_phi.push_back ( -1. );
	}
        vec_lep_relIso03.push_back ( muRelIso03(iMu,analysis_t::HAD) );
        vec_lep_relIso04.push_back ( muRelIso04(iMu,analysis_t::HAD) );
        vec_lep_miniRelIso.push_back ( muMiniRelIsoCMS3_EA(iMu, config_.ea_version) );
        // vec_lep_relIsoAn04.push_back ( muRelIsoAn04(iMu) );
        if (!isData && cms3.mus_mc3dr().size() > 0 && cms3.mus_mc3dr().at(iMu) < 0.2 && cms3.mus_mc3idx().at(iMu) != -9999 && abs(cms3.mus_mc3_id().at(iMu)) == 13) { // matched to a prunedGenParticle electron?
          int momid =  abs(genPart_motherId[cms3.mus_mc3idx().at(iMu)]);
          vec_lep_mcMatchId.push_back ( momid != 13 ? momid : genPart_grandmotherId[cms3.mus_mc3idx().at(iMu)]); // if mother is different store mother, otherwise store grandmother
        }
        else vec_lep_mcMatchId.push_back (0);
        vec_lep_lostHits.push_back ( cms3.mus_exp_innerlayers().at(iMu) ); // use defaults as if "good electron"
        vec_lep_convVeto.push_back ( 1);// use defaults as if "good electron"
        vec_lep_tightCharge.push_back ( tightChargeMuon(iMu) );

        nlep++;

        // only use isolated leptons for counters, overlaps, etc
        if (pass_iso) {
          nMuons10++;
          p4sUniqueLeptons.push_back(cms3.mus_p4().at(iMu));

          // for mt2 and mht in lepton control region
          if (doJetLepOverlapRemoval) {
            p4sForHems.push_back(cms3.mus_p4().at(iMu));
            p4sForHemsUP.push_back(cms3.mus_p4().at(iMu));
            p4sForHemsDN.push_back(cms3.mus_p4().at(iMu));
            p4sForDphi.push_back(cms3.mus_p4().at(iMu));
            p4sForDphiUP.push_back(cms3.mus_p4().at(iMu));
            p4sForDphiDN.push_back(cms3.mus_p4().at(iMu));
          }
	  
	  if (!isData && applyLeptonSFs) {
	    weightStruct weights = getLepSFFromFile(cms3.mus_p4().at(iMu).pt(), cms3.mus_p4().at(iMu).eta(), 13);
	    weight_lepsf *= weights.cent;
	    weight_lepsf_UP *= weights.up;
	    weight_lepsf_DN *= weights.dn;
	    if (isFastsim) {
	      weightStruct weights_fastsim = getLepSFFromFile_fastsim(cms3.mus_p4().at(iMu).pt(), cms3.mus_p4().at(iMu).eta(), 13);
	      weight_lepsf *= weights_fastsim.cent;
	      weight_lepsf_UP *= weights_fastsim.up;
	      weight_lepsf_DN *= weights_fastsim.dn;
	    }
	  }

        } // if (pass_iso)

      }

      // Implement pT ordering for leptons (it's irrelevant but easier for us to add than for ETH to remove)
      //now fill arrays from vectors, isotracks with largest pt first
      int i = 0;
      std::sort(lep_pt_ordering.begin(), lep_pt_ordering.end(), sortByValue);
      for(std::vector<std::pair<int, float> >::iterator it = lep_pt_ordering.begin(); it!= lep_pt_ordering.end(); ++it){

        if (i >= max_nlep) {
          std::cout << "WARNING: attempted to fill more than " << max_nlep << " leptons" << std::endl;
          break;
        }

        lep_pt[i]     = vec_lep_pt.at(it->first);
        lep_eta[i]    = vec_lep_eta.at(it->first);
        lep_phi[i]    = vec_lep_phi.at(it->first);
        lep_mass[i]   = vec_lep_mass.at(it->first);
        lep_charge[i] = vec_lep_charge.at(it->first);
        lep_pdgId[i]  = vec_lep_pdgId.at(it->first);
        lep_dz[i]     = vec_lep_dz.at(it->first);
        lep_dxy[i]    = vec_lep_dxy.at(it->first);
        lep_tightId[i]     = vec_lep_tightId.at(it->first);
        lep_heepId[i]      = vec_lep_heepId.at(it->first);
        lep_highPtFit_pt[i]     = vec_lep_highPtFit_pt.at(it->first);
        lep_highPtFit_eta[i]    = vec_lep_highPtFit_eta.at(it->first);
        lep_highPtFit_phi[i]    = vec_lep_highPtFit_phi.at(it->first);
        lep_relIso03[i]    = vec_lep_relIso03.at(it->first);
        lep_relIso04[i]    = vec_lep_relIso04.at(it->first);
        lep_miniRelIso[i]  = vec_lep_miniRelIso.at(it->first);
        // lep_relIsoAn04[i]  = vec_lep_relIsoAn04.at(it->first);
        lep_mcMatchId[i]   = vec_lep_mcMatchId.at(it->first);
        lep_lostHits[i]    = vec_lep_lostHits.at(it->first);
        lep_convVeto[i]    = vec_lep_convVeto.at(it->first);
        lep_tightCharge[i] = vec_lep_tightCharge.at(it->first);
        i++;
      }

      // for Z-->ll control regions 
      // (use two leading leptons regardless of flavor, charge, ID)
      if (nlep == 2) {
        float zll_met_px  = met_pt * cos(met_phi);
        float zll_met_py  = met_pt * sin(met_phi);	
        float zll_met_pxUP  = met_ptJECup * cos(met_phiJECup);
        float zll_met_pyUP  = met_ptJECup * sin(met_phiJECup);	
        float zll_met_pxDN  = met_ptJECdn * cos(met_phiJECdn);
        float zll_met_pyDN  = met_ptJECdn * sin(met_phiJECdn);	
        zll_met_px += lep_pt[0] * cos(lep_phi[0]);
        zll_met_px += lep_pt[1] * cos(lep_phi[1]);
        zll_met_py += lep_pt[0] * sin(lep_phi[0]);
        zll_met_py += lep_pt[1] * sin(lep_phi[1]);
        // recalculated MET with leptons added
        TVector2 zll_met_vec(zll_met_px, zll_met_py);
        zll_met_pt = zll_met_vec.Mod();
        zll_met_phi = TVector2::Phi_mpi_pi(zll_met_vec.Phi()); 

        zll_met_pxUP += lep_pt[0] * cos(lep_phi[0]);
        zll_met_pxUP += lep_pt[1] * cos(lep_phi[1]);
        zll_met_pyUP += lep_pt[0] * sin(lep_phi[0]);
        zll_met_pyUP += lep_pt[1] * sin(lep_phi[1]);
        // recalculated MET with leptons
        TVector2 zll_met_vecUP(zll_met_pxUP, zll_met_pyUP);
        zll_met_ptJECup = zll_met_vecUP.Mod();
        zll_met_phiJECup = TVector2::Phi_mpi_pi(zll_met_vecUP.Phi()); 

        zll_met_pxDN += lep_pt[0] * cos(lep_phi[0]);
        zll_met_pxDN += lep_pt[1] * cos(lep_phi[1]);
        zll_met_pyDN += lep_pt[0] * sin(lep_phi[0]);
        zll_met_pyDN += lep_pt[1] * sin(lep_phi[1]);
        // recalculated MET with leptons added
        TVector2 zll_met_vecDN(zll_met_pxDN, zll_met_pyDN);
        zll_met_ptJECdn = zll_met_vecDN.Mod();
        zll_met_phiJECdn = TVector2::Phi_mpi_pi(zll_met_vecDN.Phi()); 
     
        TLorentzVector l0(0,0,0,0);
        TLorentzVector l1(0,0,0,0);
        l0.SetPtEtaPhiM(lep_pt[0], lep_eta[0], lep_phi[0], lep_mass[0]);
        l1.SetPtEtaPhiM(lep_pt[1], lep_eta[1], lep_phi[1], lep_mass[1]);
        TLorentzVector ll = l0+l1;
        zll_mass = ll.M();
        zll_pt = ll.Pt();
        zll_eta = ll.Eta();
        zll_phi = ll.Phi();

        // alternate set of vars transforming Zll to look like Wlnu. 
        //  "Randomly" turn one lepton into MET and recalc vars
        int kill_lep = evt%2;
        int keep_lep = (kill_lep+1)%2;

        float zllmt_met_px  = met_pt * cos(met_phi);
        float zllmt_met_py  = met_pt * sin(met_phi);	
        zllmt_met_px += lep_pt[kill_lep] * cos(lep_phi[kill_lep]);
        zllmt_met_py += lep_pt[kill_lep] * sin(lep_phi[kill_lep]);
        // recalculated MET with leptons added
        TVector2 zllmt_met_vec(zllmt_met_px, zllmt_met_py);
        zllmt_met_pt = zllmt_met_vec.Mod();
        zllmt_met_phi = TVector2::Phi_mpi_pi(zllmt_met_vec.Phi());      
        zllmt_mt = MT(lep_pt[keep_lep],lep_phi[keep_lep],zllmt_met_pt,zllmt_met_phi);

        LorentzVector keep_lep_vec;
        if (keep_lep == 0) {
          keep_lep_vec.SetPxPyPzE(l0.Px(),l0.Py(),l0.Pz(),l0.E());
        } else {
          keep_lep_vec.SetPxPyPzE(l1.Px(),l1.Py(),l1.Pz(),l1.E());
        }
        p4sForHemsZllMT.push_back(keep_lep_vec);
        p4sForDphiZllMT.push_back(keep_lep_vec);

      }
      //--for removed lepton control regions (w->lnu)
      if (nlep == 1) {
        float rl_met_px  = met_pt * cos(met_phi);
        float rl_met_py  = met_pt * sin(met_phi);	
        rl_met_px += lep_pt[0] * cos(lep_phi[0]);
        rl_met_py += lep_pt[0] * sin(lep_phi[0]);
        // recalculated MET with lepton added
        TVector2 rl_met_vec(rl_met_px, rl_met_py);
        rl_met_pt = rl_met_vec.Mod();
        rl_met_phi = TVector2::Phi_mpi_pi(rl_met_vec.Phi());      
        // TLorentzVector l0(0,0,0,0);
        // TLorentzVector l1(0,0,0,0);
        // l0.SetPtEtaPhiM(lep_pt[0], lep_eta[0], lep_phi[0], lep_mass[0]);
        // TLorentzVector ll = l0;
        rl_mass = lep_mass[0];
        rl_pt  = lep_pt[0];
        rl_eta = lep_eta[0];
        rl_phi = lep_phi[0];
      }

      if (verbose) cout << "before isotracks" << endl;

      //ISOTRACK
      std::vector<std::tuple<LorentzVector, float, int, uint> > isoTrackCandidates; // p4, iso, abs(pdgId), idx

      if(useIsotrackCollectionForVeto){
          for(unsigned int iit = 0; iit < cms3.isotracks_p4().size(); iit++){
              if(!cms3.isotracks_isPFCand().at(iit)) continue;
              if(cms3.isotracks_charge().at(iit) == 0) continue;
              if(fabs(cms3.isotracks_dz().at(iit)) > 0.1) continue;
              if(fabs(cms3.isotracks_dxy().at(iit)) > 0.2) continue;
              if(cms3.isotracks_fromPV().at(iit) <= 1) continue;
                            
              float absiso = cms3.isotracks_pfIso_ch().at(iit);
              int pdgId = abs(cms3.isotracks_particleId().at(iit));

              isoTrackCandidates.push_back(std::make_tuple(cms3.isotracks_p4().at(iit), absiso, pdgId, iit));
          }
      }else{
          for (unsigned int ipf = 0; ipf < cms3.pfcands_p4().size(); ipf++) {              
              if(cms3.pfcands_charge().at(ipf) == 0) continue;
              if(fabs(cms3.pfcands_dz().at(ipf)) > 0.1) continue;
              if(fabs(cms3.pfcands_dxy().at(ipf)) > 0.2) continue;
              if(cms3.pfcands_fromPV().at(ipf) <= 1) continue;
              
              float absiso  = cms3.pfcands_trackIso().at(ipf);              
              int pdgId = abs(cms3.pfcands_particleId().at(ipf));

              isoTrackCandidates.push_back(std::make_tuple(cms3.pfcands_p4().at(ipf), absiso, pdgId, ipf));
          }
      }

      std::vector<std::pair<int, float> > pt_ordering;
      vector<float>vec_isoTrack_pt;
      vector<float>vec_isoTrack_eta;
      vector<float>vec_isoTrack_phi;
      vector<float>vec_isoTrack_mass;
      vector<float>vec_isoTrack_absIso;
      // vector<float>vec_isoTrack_relIsoAn04;
      vector<float>vec_isoTrack_dz;
      vector<int>  vec_isoTrack_pdgId;
      vector<int>  vec_isoTrack_mcMatchId;

      nisoTrack = 0;
      nPFLep5LowMT = 0;
      nPFHad10LowMT = 0;
      nPFCHCand3 = 0;
      for(unsigned int iit = 0; iit < isoTrackCandidates.size(); iit++){
          LorentzVector p4;
          float absiso;
          int pdgId;
          unsigned int idx;
          std::tie(p4, absiso, pdgId, idx) = isoTrackCandidates.at(iit);

          if(fabs(p4.eta()) > 2.4) continue;

          float cand_pt = p4.pt();
          if(cand_pt > 3) ++nPFCHCand3;
          if(cand_pt < 5) continue;

          if(applyLeptonIso && absiso >= min(0.2*cand_pt, 8.0)) continue;

          float mt = MT(cand_pt, p4.phi(), met_pt, met_phi);

          if ((cand_pt > 5.) && (pdgId == 11 || pdgId == 13) && (absiso/cand_pt < 0.2) && (mt < 100.)) {
              ++nPFLep5LowMT;

              // use PF leptons for hemispheres etc same as reco leptons
              //  BUT first do overlap removal with reco leptons to avoid double counting
              bool overlap = false;
              for(unsigned int iLep = 0; iLep < p4sUniqueLeptons.size(); iLep++){
                  float thisDR = DeltaR(p4.eta(), p4sUniqueLeptons.at(iLep).eta(), p4.phi(), p4sUniqueLeptons.at(iLep).phi());
                  // use small DR threshold to ONLY remove objects that are exactly the same (reco/pf leptons)
                  if (thisDR < 0.01) {
                      overlap = true;
                      lep_isPF[iLep] = true;
                      break;
                  }
              } // loop over reco leps
              if (!overlap) {
                  
                  p4sUniqueLeptons.push_back(p4);
                  if (doJetLepOverlapRemoval) {
                      p4sForHems.push_back(p4);
                      p4sForHemsUP.push_back(p4);
                      p4sForHemsDN.push_back(p4);
                      p4sForDphi.push_back(p4);
                      p4sForDphiUP.push_back(p4);
                      p4sForDphiDN.push_back(p4);
                  }
                  
                  // update scale factor and uncertainty.  Assume SFs are 1, based on isolation T&P results.  use only uncertainty.
                  if (!isData && applyLeptonSFs) {
                      weightStruct weights = getLepSFFromFile(p4.pt(), p4.eta(), pdgId);
                      weight_lepsf_UP *= (1.0 + (weights.up - weights.cent));
                      weight_lepsf_DN *= (1.0 - (weights.cent - weights.dn));
                      if (isFastsim) {
                          weightStruct weights_fastsim = getLepSFFromFile_fastsim(p4.pt(), p4.eta(), pdgId);
                          weight_lepsf_UP *= (1.0 + (weights_fastsim.up - weights_fastsim.cent));
                          weight_lepsf_DN *= (1.0 - (weights_fastsim.cent - weights_fastsim.dn));
                      }
                  }
                  
              }
          } // passing pflepton 
          
          if ((cand_pt > 10.) && (pdgId == 211) && (absiso/cand_pt < 0.1) && (mt < 100.)) ++nPFHad10LowMT;
          
          pt_ordering.push_back(std::pair<int,float>(nisoTrack,cand_pt));
          
          vec_isoTrack_pt.push_back    ( cand_pt                          );
          vec_isoTrack_eta.push_back   ( p4.eta()  );
          vec_isoTrack_phi.push_back   ( p4.phi()  );
          vec_isoTrack_absIso.push_back( absiso                           );
          vec_isoTrack_mcMatchId.push_back ( 0 );
          if(useIsotrackCollectionForVeto){
              vec_isoTrack_mass.push_back  ( cms3.isotracks_p4().at(idx).mass() );
              vec_isoTrack_dz.push_back    ( cms3.isotracks_dz().at(idx)        );
              vec_isoTrack_pdgId.push_back ( cms3.isotracks_particleId().at(idx));
          }else{
              vec_isoTrack_mass.push_back  ( cms3.pfcands_mass().at(idx)      );
              vec_isoTrack_dz.push_back    ( cms3.pfcands_dz().at(idx)        );
              vec_isoTrack_pdgId.push_back ( cms3.pfcands_particleId().at(idx));
          }          
          nisoTrack++;
      }

      
      //now fill arrays from vectors, isotracks with largest pt first
      i = 0;
      std::sort(pt_ordering.begin(), pt_ordering.end(), sortByValue);
      for(std::vector<std::pair<int, float> >::iterator it = pt_ordering.begin(); it!= pt_ordering.end(); ++it){

        if (i >= max_nisoTrack) {
          std::cout << "WARNING: attempted to fill more than " << max_nisoTrack << " iso tracks" << std::endl;
          break;
        }

        isoTrack_pt[i]     = vec_isoTrack_pt.at(it->first);
        isoTrack_eta[i]    = vec_isoTrack_eta.at(it->first);
        isoTrack_phi[i]    = vec_isoTrack_phi.at(it->first);
        isoTrack_mass[i]   = vec_isoTrack_mass.at(it->first);
        isoTrack_absIso[i] = vec_isoTrack_absIso.at(it->first);
        // isoTrack_relIsoAn04[i] = vec_isoTrack_relIsoAn04.at(it->first);
        isoTrack_dz[i]     = vec_isoTrack_dz.at(it->first);
        isoTrack_pdgId[i]  = vec_isoTrack_pdgId.at(it->first);
        isoTrack_mcMatchId[i]  = vec_isoTrack_mcMatchId.at(it->first);
        i++;
      }

      //--
      if (verbose) cout << "before highPtPFcands" << endl;

      if (saveHighPtPFcands || savePFphotons) {

	//HIGH-PT PF CANDS
	std::vector<std::pair<int, float> > pf_pt_ordering;
	vector<float>vec_highPtPFcands_pt;
	vector<float>vec_highPtPFcands_eta;
	vector<float>vec_highPtPFcands_phi;
	vector<float>vec_highPtPFcands_mass;
	vector<float>vec_highPtPFcands_absIso;
	// vector<float>vec_highPtPFcands_relIsoAn04;
	vector<float>vec_highPtPFcands_dz;
	vector<int>  vec_highPtPFcands_pdgId;
	vector<int>  vec_highPtPFcands_mcMatchId;

        //PF photons
	std::vector<std::pair<int, float> > pfPhot_pt_ordering;
	vector<float>vec_pfPhoton_pt;
	vector<float>vec_pfPhoton_eta;
	vector<float>vec_pfPhoton_phi;
	
	nhighPtPFcands = 0;
        npfPhoton = 0;
	for (unsigned int ipf = 0; ipf < cms3.pfcands_p4().size(); ipf++) {
	  
	  float cand_pt = cms3.pfcands_p4().at(ipf).pt();
          int cand_pdgId = cms3.pfcands_particleId().at(ipf);

	  if(cand_pt < 5) continue;

          if(cand_pdgId==22){
              pfPhot_pt_ordering.push_back(std::pair<int,float>(npfPhoton,cand_pt));
              npfPhoton++;
              vec_pfPhoton_pt.push_back(cand_pt);
              vec_pfPhoton_eta.push_back(cms3.pfcands_p4().at(ipf).eta());
              vec_pfPhoton_phi.push_back(cms3.pfcands_p4().at(ipf).phi());
          }

	  if(cand_pt < 300 && !(abs(cand_pdgId) == 13) ) continue;
	  
	  float absiso  = cms3.pfcands_trackIso().at(ipf);
	  // float an04 = PFCandRelIsoAn04(ipf);
	  
	  pf_pt_ordering.push_back(std::pair<int,float>(nhighPtPFcands,cand_pt));
	  
	  vec_highPtPFcands_pt.push_back    ( cand_pt                          );
	  vec_highPtPFcands_eta.push_back   ( cms3.pfcands_p4().at(ipf).eta()  );
	  vec_highPtPFcands_phi.push_back   ( cms3.pfcands_p4().at(ipf).phi()  );
	  vec_highPtPFcands_mass.push_back  ( cms3.pfcands_mass().at(ipf)      );
	  vec_highPtPFcands_absIso.push_back( absiso                           );
	  // vec_highPtPFcands_relIsoAn04.push_back( an04                         );
	  vec_highPtPFcands_dz.push_back    ( cms3.pfcands_dz().at(ipf)        );
	  vec_highPtPFcands_pdgId.push_back ( cand_pdgId );
	  vec_highPtPFcands_mcMatchId.push_back ( 0 );
	  
	  nhighPtPFcands++;
	}  
	
	//now fill arrays from vectors, pf cands with largest pt first
        if(saveHighPtPFcands){
            i = 0;
            std::sort(pf_pt_ordering.begin(), pf_pt_ordering.end(), sortByValue);
            for(std::vector<std::pair<int, float> >::iterator it = pf_pt_ordering.begin(); it!= pf_pt_ordering.end(); ++it){
	  
                if (i >= max_nhighPtPFcands) {
                    std::cout << "WARNING: attempted to fill more than " << max_nhighPtPFcands << " iso tracks" << std::endl;
                    break;
                }

                highPtPFcands_pt[i]     = vec_highPtPFcands_pt.at(it->first);
                highPtPFcands_eta[i]    = vec_highPtPFcands_eta.at(it->first);
                highPtPFcands_phi[i]    = vec_highPtPFcands_phi.at(it->first);
                highPtPFcands_mass[i]   = vec_highPtPFcands_mass.at(it->first);
                highPtPFcands_absIso[i] = vec_highPtPFcands_absIso.at(it->first);
                // highPtPFcands_relIsoAn04[i] = vec_highPtPFcands_relIsoAn04.at(it->first);
                highPtPFcands_dz[i]     = vec_highPtPFcands_dz.at(it->first);
                highPtPFcands_pdgId[i]  = vec_highPtPFcands_pdgId.at(it->first);
                highPtPFcands_mcMatchId[i]  = vec_highPtPFcands_mcMatchId.at(it->first);
                i++;
            }
        }else{ nhighPtPFcands = 0; }

        if(savePFphotons){
            i = 0;
            std::sort(pfPhot_pt_ordering.begin(), pfPhot_pt_ordering.end(), sortByValue);
            for(std::vector<std::pair<int, float> >::iterator it = pfPhot_pt_ordering.begin(); it!= pfPhot_pt_ordering.end(); ++it){
	  
                if (i >= max_ngamma) {
                    std::cout << "WARNING: attempted to fill more than " << max_ngamma << " pf photons" << std::endl;
                    break;
                }

                pfPhoton_pt[i]     = vec_pfPhoton_pt.at(it->first);
                pfPhoton_eta[i]    = vec_pfPhoton_eta.at(it->first);
                pfPhoton_phi[i]    = vec_pfPhoton_phi.at(it->first);
                i++;
            }
        }else{ npfPhoton = 0; }
      }//saveHighPtPFcands||savePFphotons
      
      // count number of unique lowMT leptons (e/mu)
      //  same collection as those used for jet/lepton overlap, but require MT < 100 explicitly
      nLepLowMT = 0;
      nLepHighMT = 0;
      for (unsigned int ilep = 0; ilep < p4sUniqueLeptons.size(); ++ilep) {
        float mt = MT(p4sUniqueLeptons.at(ilep).pt(),p4sUniqueLeptons.at(ilep).phi(),met_pt,met_phi);
        if (mt < 100.) ++nLepLowMT;
        else           ++nLepHighMT;
      }

      if (verbose) cout << "before lost gen leptons" << endl;

      weight_lepsf_0l = 1.;
      weight_lepsf_0l_UP = 1.;
      weight_lepsf_0l_DN = 1.;

      // only fill these variables on MC and for events with 0 veto leptons
      if (!isData && (nMuons10+nElectrons10+nPFLep5LowMT+nPFHad10LowMT == 0)) {
	for (int ilep = 0; ilep < ngenLep+ngenLepFromTau; ++ilep) {
	  float pt,eta;
	  int pdgId;
	  if (ilep < ngenLep) {
	    pt = genLep_pt[ilep];
	    eta = genLep_eta[ilep];
	    pdgId = genLep_pdgId[ilep];
	  } else {
	    pt = genLepFromTau_pt[ilep-ngenLep];
	    eta = genLepFromTau_eta[ilep-ngenLep];
	    pdgId = genLepFromTau_pdgId[ilep-ngenLep];
	  }
	  // check acceptance for veto: pt > 5, |eta| < 2.4
	  if (pt < 5.) continue;
	  if (fabs(eta) > 2.4) continue;

	  if (applyLeptonSFs) {
	    if (isFastsim) {
	      // look up SF and vetoeff, by flavor
	      weightStruct sf_struct_fullsim = getLepSFFromFile(pt, eta, pdgId);
	      weightStruct sf_struct_fastsim = getLepSFFromFile_fastsim(pt, eta, pdgId);
	      float sf = sf_struct_fullsim.cent * sf_struct_fastsim.cent;
	      float vetoeff = getLepVetoEffFromFile_fastsim(pt, eta, pdgId);
	      // apply SF to vetoeff, then correction for 0L will be (1 - vetoeff_cor) / (1 - vetoeff) - 1.
	      float vetoeff_cor = vetoeff * sf;
	      float cor_0l = ( (1. - vetoeff_cor) / (1. - vetoeff) ) - 1.;
	      weight_lepsf_0l *= (1. + cor_0l);
	      float unc = (sf_struct_fullsim.up - sf_struct_fullsim.cent) + (sf_struct_fastsim.up - sf_struct_fastsim.cent);
	      float vetoeff_cor_unc_UP = vetoeff_cor * (1. + unc);
	      float unc_UP_0l = ( (1. - vetoeff_cor_unc_UP) / (1. - vetoeff_cor) ) - 1.;
	      weight_lepsf_0l_UP *= (1. + cor_0l + unc_UP_0l);
	      weight_lepsf_0l_DN *= (1. + cor_0l - unc_UP_0l);
	    } // isFastsim

	    else { // fullsim
	      // save correction also for 0L central value for fullsim
	      weightStruct sf_struct = getLepSFFromFile(pt, eta, pdgId);
	      float sf = sf_struct.cent;
	      float vetoeff = getLepVetoEffFromFile_fullsim(pt, eta, pdgId);
	      // apply SF to vetoeff, then correction for 0L will be (1 - vetoeff_cor) / (1 - vetoeff) - 1.
	      float vetoeff_cor = vetoeff * sf;
	      float cor_0l = ( (1. - vetoeff_cor) / (1. - vetoeff) ) - 1.;
	      weight_lepsf_0l *= (1. + cor_0l);
	      float unc = sf_struct.up - sf;
	      float vetoeff_cor_unc_UP = vetoeff_cor * (1. + unc);
	      float unc_UP_0l = ( (1. - vetoeff_cor_unc_UP) / (1. - vetoeff_cor) ) - 1.;
	      weight_lepsf_0l_UP *= (1. + cor_0l + unc_UP_0l);
	      weight_lepsf_0l_DN *= (1. + cor_0l - unc_UP_0l);
	    }
	  } // if applyLeptonSFs
	  
	} // loop over gen leptons
      } // !isData && 0 veto leptons
      


      if (verbose) cout << "before photons" << endl;

      //PHOTONS
      std::vector<std::pair<int, float> > gamma_pt_ordering;
      vector<float> vec_gamma_pt;
      vector<float> vec_gamma_eta;
      vector<float> vec_gamma_phi;
      vector<float> vec_gamma_mass;
      vector<int>   vec_gamma_mcMatchId;
      vector<float> vec_gamma_genIso04;
      vector<float> vec_gamma_drMinParton;
      vector<float> vec_gamma_chHadIso;
      vector<float> vec_gamma_neuHadIso;
      vector<float> vec_gamma_phIso;
      vector<float> vec_gamma_sigmaIetaIeta;
      vector<float> vec_gamma_r9;
      vector<float> vec_gamma_hOverE;
      vector<float> vec_gamma_hOverE015;
      vector<int>   vec_gamma_idCutBased;

      ngamma = 0;
      nGammas20 = 0;
      // to recalculate MET adding photons
      float gamma_met_px  = met_pt * cos(met_phi);
      float gamma_met_py  = met_pt * sin(met_phi);
      for(unsigned int iGamma = 0; iGamma < cms3.photons_p4().size(); iGamma++){
        if(cms3.photons_p4().at(iGamma).pt() < 20.0) continue;
        if(fabs(cms3.photons_p4().at(iGamma).eta()) > 2.5) continue;
        if ( !isLoosePhoton(iGamma,analysis_t::HAD,3) ) continue;

        if (ngamma >= max_ngamma) {
          std::cout << "WARNING: attempted to fill more than " << max_ngamma << " photons" << std::endl;
          break;
        }
	gamma_pt_ordering.push_back( std::pair<int,float>(ngamma,cms3.photons_p4().at(iGamma).pt()) );
        float pt = cms3.photons_p4().at(iGamma).pt();
        float eta = cms3.photons_p4().at(iGamma).eta();
        float phi = cms3.photons_p4().at(iGamma).phi();
        vec_gamma_pt.push_back ( pt );
        vec_gamma_eta.push_back ( eta );
        vec_gamma_phi.push_back ( phi );
        vec_gamma_mass.push_back ( cms3.photons_p4().at(iGamma).M() );
        vec_gamma_sigmaIetaIeta.push_back ( cms3.photons_full5x5_sigmaIEtaIEta().at(iGamma) );
        vec_gamma_chHadIso.push_back ( photons_recoChargedHadronIso().at(iGamma) );
        vec_gamma_neuHadIso.push_back ( photons_recoNeutralHadronIso().at(iGamma) );
        vec_gamma_phIso.push_back ( photons_recoPhotonIso().at(iGamma) );
        vec_gamma_r9.push_back (  photons_full5x5_r9().at(iGamma) );
        vec_gamma_hOverE.push_back (  photons_full5x5_hOverEtowBC().at(iGamma) );
        vec_gamma_hOverE015.push_back (  photons_full5x5_hOverE().at(iGamma) );
        vec_gamma_idCutBased.push_back (  isTightPhoton(iGamma,analysis_t::HAD,3) ? 1 : 0 ); 
        if(pt > 20) nGammas20++;

        // Some work for truth-matching (should be integrated in CMS3 as for the leptons)
        int bestMatch = -1;
        float bestDr = 0.1;
        float bestMatchEta = 999;
        float bestMatchPhi = 999;
        if (!isData) {
          for(unsigned int iGen = 0; iGen < cms3.genps_p4().size(); iGen++){
            if (cms3.genps_id().at(iGen) != 22) continue; 
            if (cms3.genps_status().at(iGen) != 1) continue; 
            if (fabs(cms3.genps_id_simplemother().at(iGen)) > 22  && cms3.genps_id_simplemother().at(iGen)!=2212) continue; // pions etc // but keep photons from the leading proton
            if ( fabs(eta - cms3.genps_p4().at(iGen).eta()) > 0.1) continue;
            if ( pt > 2*cms3.genps_p4().at(iGen).pt() ) continue;
            if ( pt < 0.5*cms3.genps_p4().at(iGen).pt() ) continue;
            float thisDR = DeltaR( cms3.genps_p4().at(iGen).eta(), eta, cms3.genps_p4().at(iGen).phi(), phi);
            if (thisDR < bestDr) {
              bestDr = thisDR;
              bestMatch = iGen;
              bestMatchEta = cms3.genps_p4().at(iGen).eta();
              bestMatchPhi = cms3.genps_p4().at(iGen).phi();
            }
          }
        } // !isData
        if (bestMatch != -1) {
          // 7 is a special code for photons without a mother. this seems to be due to a miniAOD bug where links are broken.
          vec_gamma_mcMatchId.push_back ( cms3.genps_id_simplemother().at(bestMatch) == 0 ? 7 : 22 ); 
          vec_gamma_genIso04.push_back ( cms3.genps_iso().at(bestMatch) );
          // Now want to look at DR between photon and parton
          float minDR = 999.; 
          for(unsigned int iGen = 0; iGen < cms3.genps_p4().size(); iGen++){
            if (cms3.genps_status().at(iGen) != 22 && cms3.genps_status().at(iGen) != 23) continue;
            if (fabs(cms3.genps_id().at(iGen)) > 21) continue;
            float dr = DeltaR( cms3.genps_p4().at(iGen).eta(), bestMatchEta, cms3.genps_p4().at(iGen).phi(), bestMatchPhi);
            if (dr < minDR) minDR = dr;
          }
          vec_gamma_drMinParton.push_back ( minDR );
        }
        else {
          vec_gamma_mcMatchId.push_back ( 0 );
          vec_gamma_genIso04.push_back ( -1 );
          vec_gamma_drMinParton.push_back ( -1 );
        }

        // do not use photon in MT2 or MHT calculations!!
        //p4sForHemsGamma.push_back(cms3.photons_p4().at(iGamma));
        //p4sForDphiGamma.push_back(cms3.photons_p4().at(iGamma));
        ngamma++;
      }

      // Implement pT ordering for photons (to match ETH)
      i = 0;
      std::sort(gamma_pt_ordering.begin(), gamma_pt_ordering.end(), sortByValue);
      for(std::vector<std::pair<int, float> >::iterator it = gamma_pt_ordering.begin(); it!= gamma_pt_ordering.end(); ++it){

        if (i >= max_ngamma) {
          std::cout << "WARNING: attempted to fill more than " << max_ngamma << " gammas" << std::endl;
          break;
        }

        gamma_pt[i]     = vec_gamma_pt.at(it->first);
        gamma_eta[i]    = vec_gamma_eta.at(it->first);
        gamma_phi[i]    = vec_gamma_phi.at(it->first);
        gamma_mass[i]   = vec_gamma_mass.at(it->first);
        gamma_mcMatchId[i]    = vec_gamma_mcMatchId.at(it->first);
        gamma_genIso04[i]     = vec_gamma_genIso04.at(it->first);
        gamma_drMinParton[i]  = vec_gamma_drMinParton.at(it->first);
        gamma_chHadIso[i]     = vec_gamma_chHadIso.at(it->first);
        gamma_neuHadIso[i]    = vec_gamma_neuHadIso.at(it->first);
        gamma_phIso[i]        = vec_gamma_phIso.at(it->first);
        gamma_sigmaIetaIeta[i]  = vec_gamma_sigmaIetaIeta.at(it->first);
        gamma_r9[i]           = vec_gamma_r9.at(it->first);
        gamma_hOverE[i]       = vec_gamma_hOverE.at(it->first);
        gamma_hOverE015[i]    = vec_gamma_hOverE015.at(it->first);
        gamma_idCutBased[i]   = vec_gamma_idCutBased.at(it->first);
        i++;
      }

      // for photon+jets control regions
      if (nGammas20>0) { // Only use the leading Loose photon. Otherwise mt2 will be affected by a bunch of tiny photons
        gamma_met_px += gamma_pt[0] * cos(gamma_phi[0]);
        gamma_met_py += gamma_pt[0] * sin(gamma_phi[0]);
      }

      // recalculated MET with photons added
      TVector2 gamma_met_vec(gamma_met_px, gamma_met_py);
      gamma_met_pt = gamma_met_vec.Mod();
      gamma_met_phi = TVector2::Phi_mpi_pi(gamma_met_vec.Phi());

      if (verbose) cout << "before jets" << endl;

      //JETS
      //before we start, check that no genGet is matched to multiple recoJets
      //vector<float> pTofMatchedGenJets;
      //for(unsigned int iJet = 0; iJet < cms3.pfjets_p4().size(); iJet++){
      //  if(cms3.pfjets_p4().at(iJet).pt() < 10.0) continue;
      //  if(fabs(cms3.pfjets_p4().at(iJet).eta()) > 4.7) continue;
      //	float matchedPt = cms3.pfjets_mc_p4().at(iJet).pt();
      //	if (matchedPt!=0) {
      //	  if ( find( pTofMatchedGenJets.begin(), pTofMatchedGenJets.end(), matchedPt  ) != pTofMatchedGenJets.end() ) {
      //	    cout<<" This genJetPt is identical to one that was already matched to another reco jet of higher pT. Need to find another match"<<endl;
      //	    cout<<"event"<<evt_event()<<" jet pT: "<<cms3.pfjets_p4().at(iJet).pt()<<" genJetPt: "<<matchedPt<<endl;
      //	  }
      //	  else {
      //	    pTofMatchedGenJets.push_back( matchedPt );
      //	  }
      //	}
      //}

      //correct jets and check baseline selections
      vector<LorentzVector> p4sCorrJets; // store corrected p4 for ALL jets, so indices match CMS3 ntuple
      vector<LorentzVector> p4sCorrJetsUP; // store corrected p4 for ALL jets with JEC up variation, so indices match CMS3 ntuple
      vector<LorentzVector> p4sCorrJetsDN; // store corrected p4 for ALL jets with JEC dn variation, so indices match CMS3 ntuple
      vector<std::pair<int,float> > passJets; //index of jets that pass baseline selections with their corrected pt
      for(unsigned int iJet = 0; iJet < cms3.pfjets_p4().size(); iJet++){

        LorentzVector pfjet_p4_cor   = cms3.pfjets_p4().at(iJet);
        LorentzVector pfjet_p4_corUP = cms3.pfjets_p4().at(iJet);
        LorentzVector pfjet_p4_corDN = cms3.pfjets_p4().at(iJet);

        if (applyJECfromFile) {

          // get uncorrected jet p4 to use as input for corrections
          LorentzVector pfjet_p4_uncor = cms3.pfjets_p4().at(iJet) * cms3.pfjets_undoJEC().at(iJet);

          // get L1FastL2L3Residual total correction
          jet_corrector_pfL1FastJetL2L3_current->setRho   ( cms3.evt_fixgridfastjet_all_rho() );
          jet_corrector_pfL1FastJetL2L3_current->setJetA  ( cms3.pfjets_area().at(iJet)       );
          jet_corrector_pfL1FastJetL2L3_current->setJetPt ( pfjet_p4_uncor.pt()               );
          jet_corrector_pfL1FastJetL2L3_current->setJetEta( pfjet_p4_uncor.eta()              );
          double corr = jet_corrector_pfL1FastJetL2L3_current->getCorrection();

          // check for negative correction
          if (corr < 0. && fabs(pfjet_p4_uncor.eta()) < 4.7) {
            std::cout << "ScanChain::Looper: WARNING: negative jet correction: " << corr
              << ", raw jet pt: " << pfjet_p4_uncor.pt() << ", eta: " << pfjet_p4_uncor.eta() << std::endl;
          }

	  // include protections here on jet kinematics to prevent rare warnings/crashes
	  double varUP = 1.;
	  double varDN = 1.;
	  if (!isData && pfjet_p4_uncor.pt()*corr > 0. && fabs(pfjet_p4_uncor.eta()) < 5.4) {
	    jetcorr_uncertainty->setJetEta(pfjet_p4_uncor.eta());
	    jetcorr_uncertainty->setJetPt(pfjet_p4_uncor.pt() * corr); // must use CORRECTED pt
	    double unc = jetcorr_uncertainty->getUncertainty(true);
	    varUP = (1. + unc);
	    varDN = (1. - unc);
	  }

          // apply new JEC to p4
          pfjet_p4_cor   = pfjet_p4_uncor * corr;
          pfjet_p4_corUP = pfjet_p4_uncor * corr * varUP;
          pfjet_p4_corDN = pfjet_p4_uncor * corr * varDN;
        }

        p4sCorrJets.push_back(pfjet_p4_cor);
        p4sCorrJetsUP.push_back(pfjet_p4_corUP);
        p4sCorrJetsDN.push_back(pfjet_p4_corDN);

        if(p4sCorrJets.at(iJet).pt() < 10.0) continue;
        if(fabs(p4sCorrJets.at(iJet).eta()) > 4.7) continue;

        passJets.push_back( std::pair<int,float>(iJet, pfjet_p4_cor.pt()) );

      }

      // sort passing jets by corrected pt
      std::sort(passJets.begin(), passJets.end(), sortByValue);

      if (verbose) cout << "before jet/lepton overlap" << endl;

      //check overlapping with leptons
      //only want to remove the closest jet to a lepton, threshold deltaR < 0.4
      // if a jet is the closest one to 2 leptons, only remove that 1 jet. This loop will find it twice
      vector<int> removedJets; //index of jets to be removed because they overlap with a lepton
      for(unsigned int iLep = 0; iLep < p4sUniqueLeptons.size(); iLep++){

        float minDR = 0.4;
        int minIndex = -1;
        for(unsigned int passIdx = 0; passIdx < passJets.size(); passIdx++){ //loop through jets that passed baseline selections

          int iJet = passJets.at(passIdx).first;

          if(p4sCorrJets.at(iJet).pt() < 20.0) continue;
          if(fabs(p4sCorrJets.at(iJet).eta()) > 4.7) continue;

          float thisDR = DeltaR(p4sCorrJets.at(iJet).eta(), p4sUniqueLeptons.at(iLep).eta(), p4sCorrJets.at(iJet).phi(), p4sUniqueLeptons.at(iLep).phi());
          if(thisDR < minDR){
            minDR = thisDR; 
            minIndex = iJet;
          }
        }
        removedJets.push_back(minIndex);
      }

      if (verbose) cout << "before jet/photon overlap" << endl;

      //check overlapping with photons
      //only want to remove the closest jet to a photon, threshold deltaR < 0.4
      vector<int> removedJetsGamma; //index of jets to be removed because they overlap with a photon
      for(int iGamma = 0; iGamma < ngamma; iGamma++){
        if (iGamma>0) continue; // Only check leading photon. Let the others be

        float minDR = 0.4;
        int minIndex = -1;
        for(unsigned int passIdx = 0; passIdx < passJets.size(); passIdx++){ //loop through jets that passed baseline selections

          int iJet = passJets.at(passIdx).first;

          if(p4sCorrJets.at(iJet).pt() < 20.0) continue;
          if(fabs(p4sCorrJets.at(iJet).eta()) > 4.7) continue;

          bool alreadyRemoved = false;
          for(unsigned int j=0; j<removedJetsGamma.size(); j++){
            if(iJet == removedJetsGamma.at(j)){
              alreadyRemoved = true;
              break;
            }
          }
          if(alreadyRemoved) continue;

          float thisDR = DeltaR(p4sCorrJets.at(iJet).eta(), gamma_eta[iGamma], p4sCorrJets.at(iJet).phi(), gamma_phi[iGamma]);
          if(thisDR < minDR){
            minDR = thisDR; 
            minIndex = iJet;
          }
        }
        removedJetsGamma.push_back(minIndex);
      }

      
      njet = 0;
      nJet30 = 0;
      nJet30JECup = 0;
      nJet30JECdn = 0;
      nJet40 = 0;
      nBJet20 = 0;      // these are counted using cMVAv2 algorithm
      nBJet20JECup = 0;
      nBJet20JECdn = 0;
      nBJet25 = 0;
      nBJet30 = 0;
      nBJet40 = 0;
      nBJet20csv = 0;    // counters for 2 different algorithms
      nBJet30csv = 0;    // counters for 2 different algorithms
      nJet30FailId = 0;
      nJet100FailId = 0;
      nJet20BadFastsim = 0;
      nJet200MuFrac50DphiMet = 0;
      minMTBMet = 999999.;
      jet1_pt = 0.;
      jet2_pt = 0.;
      jet1_ptJECup = 0.;
      jet2_ptJECup = 0.;
      jet1_ptJECdn = 0.;
      jet2_ptJECdn = 0.;
      jet_failFSveto = 0.;

      gamma_nJet30 = 0;
      gamma_nJet40 = 0;
      gamma_nJet30FailId = 0;
      gamma_nJet100FailId = 0;
      gamma_nBJet20 = 0;
      gamma_nBJet20csv = 0;
      gamma_nBJet25 = 0;
      gamma_nBJet30 = 0;
      gamma_nBJet40 = 0;
      gamma_minMTBMet = 999999.;
      gamma_jet1_pt = 0.;
      gamma_jet2_pt = 0.;
      zll_minMTBMet = 999999.;
      rl_minMTBMet = 999999.;

      // for applying btagging SFs, using Method 1a from the twiki below:
      //   https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagSFMethods#1a_Event_reweighting_using_scale
      //   https://twiki.cern.ch/twiki/pub/CMS/BTagSFMethods/Method1aExampleCode_CSVM.cc.txt
      //   - have changed from the above example:
      //    - treat heavy and light flavor separately
      //    - use multiplicative weights for uncertainties instead of additive, to avoid negative uncertainty weights
      float btagprob_data = 1.;
      float btagprob_heavy_UP = 1.;
      float btagprob_heavy_DN = 1.;
      float btagprob_light_UP = 1.;
      float btagprob_light_DN = 1.;
      float btagprob_mc = 1.;

      // store corrected p4 for jets passing: pt > 30, |eta| < 2.4, Loose PF ID, lepton overlap
      // - to use for nisrMatch calculation
      vector<LorentzVector> p4sLeptonCleanedJets; 
      
      if (verbose) cout << "before main jet loop" << endl;

      //now fill variables for jets that pass baseline selections and don't overlap with a lepton
      for(unsigned int passIdx = 0; passIdx < passJets.size(); passIdx++){

        int iJet = passJets.at(passIdx).first;

        //check against list of jets that overlap with a lepton
        bool isOverlapJet = false;
        for(unsigned int j=0; j<removedJets.size(); j++){
          if(iJet == removedJets.at(j)){
            isOverlapJet = true;
            break;
          }
        }
        if(doJetLepOverlapRemoval && isOverlapJet) continue;
        
        bool isOverlapJetGamma = false;
	//check against list of jets that overlap with a photon
	for(unsigned int j=0; j<removedJetsGamma.size(); j++){
	  if(iJet == removedJetsGamma.at(j)){
	    isOverlapJetGamma = true;
	    break;
	  }
	} 
        
        if (njet >= max_njet) {
          std::cout << "WARNING: attempted to fill more than " << max_njet << " jets" << std::endl;
          break;
        }

        // only save jets with pt 20 eta 4.7
        float jetCutoff = 20.0;
        // need lower jet threshold if doing rebalancing
        if(doRebal)
            jetCutoff = 10.0;
        if( (p4sCorrJets.at(iJet).pt() > jetCutoff) && (fabs(p4sCorrJets.at(iJet).eta()) < 4.7) ) {

	  // check for bad fastsim jets
	  if (isFastsim && isBadFastsimJet(iJet)) ++nJet20BadFastsim;

          // first check jet ID - count the number of jets that fail.  Don't apply for fastsim
          bool passJetID = true;
          if(config_.jet_id == "50nsV1")
              passJetID = isLoosePFJet_50nsV1(iJet);
          else if(config_.jet_id == "2017_v1")
              passJetID = isTightPFJet_2017_v1(iJet);
          else
              cout << "WARNING! unknown jet ID '" << config_.jet_id << "'! Assuming all jets pass for now. Fix your config." << endl;
          if(!passJetID && !isFastsim) {
            if (p4sCorrJets.at(iJet).pt() > 30.0) ++nJet30FailId;
            if (p4sCorrJets.at(iJet).pt() > 100.0) ++nJet100FailId;
            if (!isOverlapJetGamma) {
              if (p4sCorrJets.at(iJet).pt() > 30.0) ++gamma_nJet30FailId;
              if (p4sCorrJets.at(iJet).pt() > 100.0) ++gamma_nJet100FailId;
            }
            continue;
          }
          
          jet_pt[njet]   = p4sCorrJets.at(iJet).pt();
          jet_eta[njet]  = p4sCorrJets.at(iJet).eta();
          jet_phi[njet]  = p4sCorrJets.at(iJet).phi();
	  const float jet_ptUP  =  p4sCorrJetsUP.at(iJet).pt();
	  const float jet_etaUP =  p4sCorrJetsUP.at(iJet).eta();
	  const float jet_ptDN  =  p4sCorrJetsDN.at(iJet).pt();
	  const float jet_etaDN =  p4sCorrJetsDN.at(iJet).eta();
          jet_mass[njet] = cms3.pfjets_p4().at(iJet).M();
          jet_btagCSV[njet] = cms3.getbtagvalue("pfCombinedInclusiveSecondaryVertexV2BJetTags",iJet);
          if(config_.cmssw_ver == 80)
              jet_btagDeepCSV[njet] = cms3.getbtagvalue("deepFlavourJetTags:probb",iJet) + cms3.getbtagvalue("deepFlavourJetTags:probbb",iJet);
          else if(config_.cmssw_ver == 94 || config_.cmssw_ver == 102)
              jet_btagDeepCSV[njet] = cms3.getbtagvalue("pfDeepCSVJetTags:probb",iJet) + cms3.getbtagvalue("pfDeepCSVJetTags:probbb",iJet);

          jet_chf[njet] = cms3.pfjets_chargedHadronE().at(iJet) / (cms3.pfjets_undoJEC().at(iJet)*cms3.pfjets_p4()[iJet].energy());
          jet_nhf[njet] = cms3.pfjets_neutralHadronE().at(iJet) / (cms3.pfjets_undoJEC().at(iJet)*cms3.pfjets_p4()[iJet].energy());
          jet_cemf[njet] = cms3.pfjets_chargedEmE().at(iJet) / (cms3.pfjets_undoJEC().at(iJet)*cms3.pfjets_p4()[iJet].energy());
          jet_nemf[njet] = cms3.pfjets_neutralEmE().at(iJet) / (cms3.pfjets_undoJEC().at(iJet)*cms3.pfjets_p4()[iJet].energy());
          jet_muf[njet] = cms3.pfjets_muonE()[iJet] / (cms3.pfjets_undoJEC().at(iJet)*cms3.pfjets_p4()[iJet].energy());

          if(jet_eta[njet] > HEM_region[0] && jet_eta[njet] < HEM_region[1] &&
             jet_phi[njet] > HEM_region[2] && jet_phi[njet] < HEM_region[3]){
              if(jet_pt[njet] > 20.0) nHEMJet20++;
              if(jet_pt[njet] > 30.0) nHEMJet30++;
              if(jet_pt[njet] > 40.0) nHEMJet40++;
          }

          if (!isData) {
	    jet_mcPt[njet] = -1;
            if (cms3.pfjets_mc_p4().size() > 0) jet_mcPt[njet] = cms3.pfjets_mc_p4().at(iJet).pt();
	    jet_mcFlavour[njet] = 0;
            if (cms3.pfjets_partonFlavour().size() > 0) jet_mcFlavour[njet] = cms3.pfjets_partonFlavour().at(iJet);
	    jet_hadronFlavour[njet] = 0;
            if (cms3.pfjets_hadronFlavour().size() > 0) jet_hadronFlavour[njet] = cms3.pfjets_hadronFlavour().at(iJet);
          } else {
            jet_mcPt[njet] = -999.;
            jet_mcFlavour[njet] = -999;
            jet_hadronFlavour[njet] = -999;
          }
          //jet_qgl
          jet_area[njet] = cms3.pfjets_area().at(iJet);
          jet_rawPt[njet] = cms3.pfjets_p4().at(iJet).pt() * cms3.pfjets_undoJEC().at(iJet);

          if(config_.jet_id == "50nsV1"){
              if (isMonoPFJet_Monojet(iJet)) jet_id[njet] = 5;
              else if (isMonoPFJet_MT2(iJet)) jet_id[njet] = 4;
              else if (isTightPFJet_50nsV1(iJet)) jet_id[njet] = 3;
              else if (isLoosePFJet_50nsV1(iJet)) jet_id[njet] = 1;
              else jet_id[njet] = 0;
          }else if(config_.jet_id == "2017_v1"){
              if (isMonoPFJet_Monojet(iJet)) jet_id[njet] = 5;
              else if (isMonoPFJet_MT2(iJet)) jet_id[njet] = 4;
              else if (isTightPFJet_2017_v1(iJet)) jet_id[njet] = 3;
              else jet_id[njet] = 0;
          }              
              
          jet_puId[njet] = pileupJetId(iJet, id_level_t::PUID_loose) ? 1 : 0;

	  // ad-hoc filter:
	  //  check for jets with pt > 200, mufrac > 0.5, dphi(jet,MET) > pi - 0.4
	  if ( (jet_pt[njet] > 200.0) && (jet_muf[njet] > 0.5) && (DeltaPhi(jet_phi[njet],met_phi) > TMath::Pi() - 0.4) ) {
	    ++nJet200MuFrac50DphiMet;
	  }

          // use pt20 for bjet counting, pt30 for everything else
          if( (jet_pt[njet] > 20.0) && (fabs(jet_eta[njet]) < 2.4) ){ 
            if (jet_pt[njet] > 30.0) {
              // store leading/subleading central jet pt.
              //  jets should be pt-ordered before entering this loop
              if (jet1_pt < 0.1) jet1_pt = p4sCorrJets.at(iJet).pt();
              else if (jet2_pt < 0.1) jet2_pt = p4sCorrJets.at(iJet).pt();
              p4sForHems.push_back(p4sCorrJets.at(iJet));
              p4sForDphi.push_back(p4sCorrJets.at(iJet));
              p4sForHemsZll.push_back(p4sCorrJets.at(iJet));
              p4sForDphiZll.push_back(p4sCorrJets.at(iJet));
              p4sForHemsZllMT.push_back(p4sCorrJets.at(iJet));
              p4sForDphiZllMT.push_back(p4sCorrJets.at(iJet));
              p4sForHemsRl.push_back(p4sCorrJets.at(iJet));
              p4sForDphiRl.push_back(p4sCorrJets.at(iJet));
	      p4sLeptonCleanedJets.push_back(p4sCorrJets.at(iJet));
              nJet30++;
              good_jet_idxs[nJet30-1] = njet;
              if (jet_pt[njet] > 40.) 
                  nJet40++;
            } // pt30
            if(jet_btagCSV[njet] >= config_.btag_med_threshold_CSVv2) {
              nBJet20csv++;
	      if (jet_pt[njet] > 30.0) nBJet30csv++;
            }
            //CSVv2IVFM
            if(jet_btagDeepCSV[njet] >= config_.btag_med_threshold_DeepCSV){
              nBJet20++; 
              good_bjet_idxs[nBJet20-1] = njet;
              // btag SF - not final yet
              if (!isData && applyBtagSFs) {
                float eff = getBtagEffFromFile(jet_pt[njet], jet_eta[njet], jet_hadronFlavour[njet], isFastsim);
		BTagEntry::JetFlavor flavor = BTagEntry::FLAV_UDSG;
		if (abs(jet_hadronFlavour[njet]) == 5) flavor = BTagEntry::FLAV_B;
		else if (abs(jet_hadronFlavour[njet]) == 4) flavor = BTagEntry::FLAV_C;
		float weight_cent(1.), weight_UP(1.), weight_DN(1.);
		weight_cent = reader_fullsim->eval_auto_bounds("central",flavor,jet_eta[njet],jet_pt[njet]);
		weight_UP = reader_fullsim->eval_auto_bounds("up",flavor,jet_eta[njet],jet_pt[njet]);
		weight_DN = reader_fullsim->eval_auto_bounds("down",flavor,jet_eta[njet],jet_pt[njet]);
		// extra SF for fastsim
		if (isFastsim) {
		  weight_cent *= reader_fastsim->eval_auto_bounds("central", flavor, jet_eta[njet], jet_pt[njet]);
		  weight_UP *= reader_fastsim->eval_auto_bounds("up", flavor, jet_eta[njet], jet_pt[njet]);
		  weight_DN *= reader_fastsim->eval_auto_bounds("down", flavor, jet_eta[njet], jet_pt[njet]);
		}
                btagprob_data *= weight_cent * eff;
                btagprob_mc *= eff;
		if (flavor == BTagEntry::FLAV_UDSG) {
		  btagprob_light_UP *= weight_UP * eff;
		  btagprob_light_DN *= weight_DN * eff;
		  btagprob_heavy_UP *= weight_cent * eff;
		  btagprob_heavy_DN *= weight_cent * eff;
		} else {
		  btagprob_light_UP *= weight_cent * eff;
		  btagprob_light_DN *= weight_cent * eff;
		  btagprob_heavy_UP *= weight_UP * eff;
		  btagprob_heavy_DN *= weight_DN * eff;
                }
              }
              if (jet_pt[njet] > 25.0) nBJet25++; 
              if (jet_pt[njet] > 30.0) {
                nBJet30++; 
                if (jet_pt[njet] > 40.0) nBJet40++; 
                float mt = MT(jet_pt[njet],jet_phi[njet],met_pt,met_phi);
                if (mt < minMTBMet) minMTBMet = mt;
                if (nlep == 2) {
                  float zllmt = MT(jet_pt[njet],jet_phi[njet],zll_met_pt,zll_met_phi);
                  if (zllmt < zll_minMTBMet) zll_minMTBMet = zllmt;
                }
                if (nlep == 1) {
                  float rlmt = MT(jet_pt[njet],jet_phi[njet],rl_met_pt,rl_met_phi);
                  if (rlmt < rl_minMTBMet) rl_minMTBMet = rlmt;
                }
              } // pt 30
            } // pass med btag
            else if (!isData && applyBtagSFs) { // fail med btag -- needed for SF event weights
              float eff = getBtagEffFromFile(jet_pt[njet], jet_eta[njet], jet_hadronFlavour[njet], isFastsim);
	      BTagEntry::JetFlavor flavor = BTagEntry::FLAV_UDSG;
	      if (abs(jet_hadronFlavour[njet]) == 5) flavor = BTagEntry::FLAV_B;
	      else if (abs(jet_hadronFlavour[njet]) == 4) flavor = BTagEntry::FLAV_C;
	      float weight_cent(1.), weight_UP(1.), weight_DN(1.);
	      weight_cent = reader_fullsim->eval_auto_bounds("central",flavor, jet_eta[njet], jet_pt[njet]);
	      weight_UP = reader_fullsim->eval_auto_bounds("up", flavor, jet_eta[njet], jet_pt[njet]);
	      weight_DN = reader_fullsim->eval_auto_bounds("down", flavor, jet_eta[njet], jet_pt[njet]);
	      // extra SF for fastsim
	      if (isFastsim) {
		weight_cent *= reader_fastsim->eval_auto_bounds("central", flavor, jet_eta[njet], jet_pt[njet]);
		weight_UP *= reader_fastsim->eval_auto_bounds("up", flavor, jet_eta[njet], jet_pt[njet]);
		weight_DN *= reader_fastsim->eval_auto_bounds("down", flavor, jet_eta[njet], jet_pt[njet]);
	      }
              btagprob_data *= (1. - weight_cent * eff);
              btagprob_mc *= (1. - eff);
	      if (flavor == BTagEntry::FLAV_UDSG) {
		btagprob_light_UP *= (1. - weight_UP * eff);
		btagprob_light_DN *= (1. - weight_DN * eff);
		btagprob_heavy_UP *= (1. - weight_cent * eff);
		btagprob_heavy_DN *= (1. - weight_cent * eff);
              } else {
		btagprob_light_UP *= (1. - weight_cent * eff);
		btagprob_light_DN *= (1. - weight_cent * eff);
		btagprob_heavy_UP *= (1. - weight_UP * eff);
		btagprob_heavy_DN *= (1. - weight_DN * eff);
              }
            } // fail med btag
          } // pt 20 eta 2.4
          // accept jets out to eta 4.7 for dphi
          else if ( (jet_pt[njet] > 30.0) && (fabs(jet_eta[njet]) < 4.7) ) {
            p4sForDphi.push_back(p4sCorrJets.at(iJet));
            p4sForDphiZll.push_back(p4sCorrJets.at(iJet));
            p4sForDphiZllMT.push_back(p4sCorrJets.at(iJet));
            p4sForDphiRl.push_back(p4sCorrJets.at(iJet));
          }
	  
	  //repeat for JEC up
	  // use pt20 for bjet counting, pt30 for everything else
          if( (jet_ptUP > 20.0) && (fabs(jet_etaUP) < 2.5) ){ 
            if (jet_ptUP > 30.0) {
              // store leading/subleading central jet pt.
              //  jets should be pt-ordered before entering this loop
              if (jet1_ptJECup < 0.1) jet1_ptJECup = p4sCorrJetsUP.at(iJet).pt();
              else if (jet2_ptJECup < 0.1) jet2_ptJECup = p4sCorrJetsUP.at(iJet).pt();
              p4sForHemsUP.push_back(p4sCorrJetsUP.at(iJet));
              p4sForDphiUP.push_back(p4sCorrJetsUP.at(iJet));
              p4sForHemsZllUP.push_back(p4sCorrJetsUP.at(iJet));
              p4sForDphiZllUP.push_back(p4sCorrJetsUP.at(iJet));
              nJet30JECup++;
            } // pt40
            //CSVv2IVFM
            if(jet_btagDeepCSV[njet] >= config_.btag_med_threshold_DeepCSV) {
              nBJet20JECup++;
            } // pass med btag
          } // pt 20 eta 2.5
	  // accept jets out to eta 4.7 for dphi
          else if ( (jet_ptUP > 30.0) && (fabs(jet_etaUP) < 4.7) ) {
            p4sForDphiUP.push_back(p4sCorrJetsUP.at(iJet));
            p4sForDphiZllUP.push_back(p4sCorrJetsUP.at(iJet));
          }
	  
	  //repeat for JEC dn
	  // use pt20 for bjet counting, pt30 for everything else
          if( (jet_ptDN > 20.0) && (fabs(jet_etaDN) < 2.5) ){ 
            if (jet_ptDN > 30.0) {
              // store leading/subleading central jet pt.
              //  jets should be pt-ordered before entering this loop
              if (jet1_ptJECdn < 0.1) jet1_ptJECdn = p4sCorrJetsDN.at(iJet).pt();
              else if (jet2_ptJECdn < 0.1) jet2_ptJECdn = p4sCorrJetsDN.at(iJet).pt();
              p4sForHemsDN.push_back(p4sCorrJetsDN.at(iJet));
              p4sForDphiDN.push_back(p4sCorrJetsDN.at(iJet));
              p4sForHemsZllDN.push_back(p4sCorrJetsDN.at(iJet));
              p4sForDphiZllDN.push_back(p4sCorrJetsDN.at(iJet));
              nJet30JECdn++;
            } // pt40
            //CSVv2IVFM
            if(jet_btagDeepCSV[njet] >= config_.btag_med_threshold_DeepCSV) {
              nBJet20JECdn++;
            } // pass med btag
          } // pt 20 eta 2.5
	  // accept jets out to eta 4.7 for dphi
          else if ( (jet_ptDN > 30.0) && (fabs(jet_etaDN) < 4.7) ) {
            p4sForDphiDN.push_back(p4sCorrJetsDN.at(iJet));
            p4sForDphiZllDN.push_back(p4sCorrJetsDN.at(iJet));
          }

          // fill gamma_XXX variables before checking for lepton overlap. Why? Let's keep them consistent with the lepton-overlapped jets
          if( ( p4sCorrJets.at(iJet).pt() > 20.0) && (fabs(p4sCorrJets.at(iJet).eta()) < 2.4) ) { 
            if(!isOverlapJetGamma) {
              if (p4sCorrJets.at(iJet).pt() > 30.0) {
                // store leading/subleading central jet pt.
                //  jets should be pt-ordered before entering this loop
                if (gamma_jet1_pt < 0.1) gamma_jet1_pt = p4sCorrJets.at(iJet).pt();
                else if (gamma_jet2_pt < 0.1) gamma_jet2_pt = p4sCorrJets.at(iJet).pt();
                p4sForHemsGamma.push_back(p4sCorrJets.at(iJet));
                p4sForDphiGamma.push_back(p4sCorrJets.at(iJet));
                gamma_nJet30++;
                if (p4sCorrJets.at(iJet).pt() > 40.0) gamma_nJet40++;
              } // pt30
              if(jet_btagCSV[njet] >= config_.btag_med_threshold_CSVv2){
                  gamma_nBJet20csv++;
              }
              if(jet_btagDeepCSV[njet] >= config_.btag_med_threshold_DeepCSV) { 
                gamma_nBJet20++; 
                if (p4sCorrJets.at(iJet).pt() > 25.0) gamma_nBJet25++; 
                if (p4sCorrJets.at(iJet).pt() > 30.0) {
                  gamma_nBJet30++;
                  if (p4sCorrJets.at(iJet).pt() > 40.0) gamma_nBJet40++; 
                  float mt = MT( p4sCorrJets.at(iJet).pt(),p4sCorrJets.at(iJet).phi(),gamma_met_pt,gamma_met_phi);
                  if (mt < gamma_minMTBMet) gamma_minMTBMet = mt;	 
                } // pt30
              } // pass med btag  
            } // not overlap with photon
          } // pt 20 eta 2.4 
          // accept jets out to eta 4.7 for dphi
          else if ( (p4sCorrJets.at(iJet).pt() > 30.0) && (fabs(p4sCorrJets.at(iJet).eta()) < 4.7) ) {
            //check against list of jets that overlap with a photon
            bool isOverlapJetGamma = false;
            for(unsigned int j=0; j<removedJetsGamma.size(); j++){
              if(iJet == removedJetsGamma.at(j)){
                isOverlapJetGamma = true;
                break;
              }
            }

            if(!isOverlapJetGamma) {
              p4sForDphiGamma.push_back(p4sCorrJets.at(iJet));
            }
          }


          njet++;
        } // pt 20 eta 4.7
      }

      // compute event level btag weights
      if (!isData && applyBtagSFs) {
        weight_btagsf = btagprob_data / btagprob_mc;
        weight_btagsf_heavy_UP = btagprob_heavy_UP / btagprob_mc;
        weight_btagsf_heavy_DN = btagprob_heavy_DN / btagprob_mc;
        weight_btagsf_light_UP = btagprob_light_UP / btagprob_mc;
        weight_btagsf_light_DN = btagprob_light_DN / btagprob_mc;
      }

      // get number of reco jets matched to gen ISR jets
      if (!isData) {
	nisrMatch = get_nisrMatch(p4sLeptonCleanedJets);
	weight_isr = get_isrWeight(nisrMatch);
	float unc_isr = get_isrUnc(nisrMatch);
	weight_isr_UP = weight_isr + unc_isr;
	weight_isr_DN = weight_isr - unc_isr;
      }

      if (verbose) cout << "before hemispheres" << endl;

      // sort vectors by pt for hemisphere calculation
      sort(p4sForHems.begin(), p4sForHems.end(), sortByPt);
      sort(p4sForHemsUP.begin(), p4sForHemsUP.end(), sortByPt);
      sort(p4sForHemsDN.begin(), p4sForHemsDN.end(), sortByPt);
      sort(p4sForDphi.begin(), p4sForDphi.end(), sortByPt);
      sort(p4sForDphiUP.begin(), p4sForDphiUP.end(), sortByPt);
      sort(p4sForDphiDN.begin(), p4sForDphiDN.end(), sortByPt);
      sort(p4sForHemsGamma.begin(), p4sForHemsGamma.end(), sortByPt);
      sort(p4sForDphiGamma.begin(), p4sForDphiGamma.end(), sortByPt);
      sort(p4sForHemsZll.begin(), p4sForHemsZll.end(), sortByPt);
      sort(p4sForHemsZllUP.begin(), p4sForHemsZllUP.end(), sortByPt);
      sort(p4sForHemsZllDN.begin(), p4sForHemsZllDN.end(), sortByPt);
      sort(p4sForDphiZll.begin(), p4sForDphiZll.end(), sortByPt);
      sort(p4sForDphiZllUP.begin(), p4sForDphiZllUP.end(), sortByPt);
      sort(p4sForDphiZllDN.begin(), p4sForDphiZllDN.end(), sortByPt);      
      sort(p4sForHemsZllMT.begin(), p4sForHemsZllMT.end(), sortByPt);
      sort(p4sForDphiZllMT.begin(), p4sForDphiZllMT.end(), sortByPt);
      sort(p4sForHemsRl.begin(), p4sForHemsRl.end(), sortByPt);
      sort(p4sForDphiRl.begin(), p4sForDphiRl.end(), sortByPt);

      ht = 0;
      htJECup = 0;
      htJECdn = 0;
      deltaPhiMin = 999;
      deltaPhiMin_genmet = 999;
      deltaPhiMinJECup = 999;
      deltaPhiMinJECdn = 999;
      LorentzVector sumMhtp4 = LorentzVector(0,0,0,0);
      LorentzVector sumMhtp4UP = LorentzVector(0,0,0,0);
      LorentzVector sumMhtp4DN = LorentzVector(0,0,0,0);

      // HT, MT2 and MHT

      // compute HT, MHT using same objects as MT2 inputs
      for (unsigned int ip4 = 0; ip4 < p4sForHems.size(); ++ip4) {
        ht += p4sForHems.at(ip4).pt();
        sumMhtp4 -= p4sForHems.at(ip4);
      }
      for (unsigned int ip4 = 0; ip4 < p4sForHemsUP.size(); ++ip4) {
        htJECup += p4sForHemsUP.at(ip4).pt();
        sumMhtp4UP -= p4sForHemsUP.at(ip4);
      }
      for (unsigned int ip4 = 0; ip4 < p4sForHemsDN.size(); ++ip4) {
        htJECdn += p4sForHemsDN.at(ip4).pt();
        sumMhtp4DN -= p4sForHemsDN.at(ip4);
      }

      // min(dphi) of 4 leading objects
      for (unsigned int ip4 = 0; ip4 < p4sForDphi.size(); ++ip4) {
        if(ip4 < 4) {
	  deltaPhiMin = min(deltaPhiMin, DeltaPhi( met_phi, p4sForDphi.at(ip4).phi() ));
	  if (!isData) deltaPhiMin_genmet = min(deltaPhiMin_genmet, DeltaPhi( met_genPhi, p4sForDphi.at(ip4).phi() ));
	}
      }
      for (unsigned int ip4 = 0; ip4 < p4sForDphiUP.size(); ++ip4) {
        if(ip4 < 4) deltaPhiMinJECup = min(deltaPhiMinJECup, DeltaPhi( met_phiJECup, p4sForDphiUP.at(ip4).phi() ));
      }
      for (unsigned int ip4 = 0; ip4 < p4sForDphiDN.size(); ++ip4) {
        if(ip4 < 4) deltaPhiMinJECdn = min(deltaPhiMinJECdn, DeltaPhi( met_phiJECdn, p4sForDphiDN.at(ip4).phi() ));
      }

      // get closest jet to MET vector for later use in plotting
      jet_closest_met_idx = -1;
      jet_closest_met_dphi = -999;
      float mindphi = 9999;
      for(int i=0; i<nJet30; i++){
          float dphi = DeltaPhi(met_phi, jet_phi[good_jet_idxs[i]]);
          if(dphi < mindphi){
              mindphi = dphi;
              jet_closest_met_idx = good_jet_idxs[i];
              jet_closest_met_dphi = dphi;
          }
      }

      vector<LorentzVector> hemJets;
      if(p4sForHems.size() > 1){
        //Hemispheres used in MT2 calculation
        hemJets = getHemJets(p4sForHems);  

        mt2 = HemMT2(met_pt, met_phi, hemJets.at(0), hemJets.at(1));
        if (!isData) mt2_genmet = HemMT2(met_genPt, met_genPhi, hemJets.at(0), hemJets.at(1));

        // order hemispheres by pt for saving
        int idx_lead = 0;
        int idx_subl = 1;
        if (hemJets.at(1).pt() > hemJets.at(0).pt()) {
          idx_lead = 1;
          idx_subl = 0;
        }

        pseudoJet1_pt   = hemJets.at(idx_lead).pt();
        pseudoJet1_eta  = hemJets.at(idx_lead).eta();
        pseudoJet1_phi  = hemJets.at(idx_lead).phi();
        pseudoJet1_mass = hemJets.at(idx_lead).mass();
        pseudoJet2_pt   = hemJets.at(idx_subl).pt();
        pseudoJet2_eta  = hemJets.at(idx_subl).eta();
        pseudoJet2_phi  = hemJets.at(idx_subl).phi();
        pseudoJet2_mass = hemJets.at(idx_subl).mass();
      }
      vector<LorentzVector> hemJetsUP;
      if(p4sForHemsUP.size() > 1){
        //Hemispheres used in MT2 calculation
        hemJetsUP = getHemJets(p4sForHemsUP);  

        mt2JECup = HemMT2(met_ptJECup, met_phiJECup, hemJetsUP.at(0), hemJetsUP.at(1));
	//if (nlep > 0) sl_mt2 = HemMT2(lep_pt[0], lep_phi[0], hemJets.at(0), hemJets.at(1));

        // order hemispheres by pt for saving
        int idx_lead = 0;
        int idx_subl = 1;
	if (hemJetsUP.at(1).pt() > hemJetsUP.at(0).pt()) {
          idx_lead = 1;
          idx_subl = 0;
	}

	pseudoJet1JECup_pt   = hemJetsUP.at(idx_lead).pt();
	pseudoJet1JECup_eta  = hemJetsUP.at(idx_lead).eta();
	pseudoJet1JECup_phi  = hemJetsUP.at(idx_lead).phi();
	pseudoJet1JECup_mass = hemJetsUP.at(idx_lead).mass();
	pseudoJet2JECup_pt   = hemJetsUP.at(idx_subl).pt();
	pseudoJet2JECup_eta  = hemJetsUP.at(idx_subl).eta();
	pseudoJet2JECup_phi  = hemJetsUP.at(idx_subl).phi();
	pseudoJet2JECup_mass = hemJetsUP.at(idx_subl).mass();
      }
      vector<LorentzVector> hemJetsDN;
      if(p4sForHemsDN.size() > 1){
        //Hemispheres used in MT2 calculation
        hemJetsDN = getHemJets(p4sForHemsDN);  

        mt2JECdn = HemMT2(met_ptJECdn, met_phiJECdn, hemJetsDN.at(0), hemJetsDN.at(1));
	//if (nlep > 0) sl_mt2 = HemMT2(lep_pt[0], lep_phi[0], hemJets.at(0), hemJets.at(1));

        // order hemispheres by pt for saving
        int idx_lead = 0;
        int idx_subl = 1;
	if (hemJetsDN.at(1).pt() > hemJetsDN.at(0).pt()) {
          idx_lead = 1;
          idx_subl = 0;
	}

	pseudoJet1JECdn_pt   = hemJetsDN.at(idx_lead).pt();
	pseudoJet1JECdn_eta  = hemJetsDN.at(idx_lead).eta();
	pseudoJet1JECdn_phi  = hemJetsDN.at(idx_lead).phi();
	pseudoJet1JECdn_mass = hemJetsDN.at(idx_lead).mass();
	pseudoJet2JECdn_pt   = hemJetsDN.at(idx_subl).pt();
	pseudoJet2JECdn_eta  = hemJetsDN.at(idx_subl).eta();
	pseudoJet2JECdn_phi  = hemJetsDN.at(idx_subl).phi();
	pseudoJet2JECdn_mass = hemJetsDN.at(idx_subl).mass();

      }

      mht_pt  = sumMhtp4.pt();
      mht_phi = sumMhtp4.phi();
      mht_ptJECup  = sumMhtp4UP.pt();
      mht_phiJECup = sumMhtp4UP.phi();
      mht_ptJECdn  = sumMhtp4DN.pt();
      mht_phiJECdn = sumMhtp4DN.phi();

      TVector2 mhtVector = TVector2(mht_pt*cos(mht_phi), mht_pt*sin(mht_phi));
      TVector2 metVector = TVector2(met_pt*cos(met_phi), met_pt*sin(met_phi));
      diffMetMht = (mhtVector - metVector).Mod();
      if (!isData) {
	TVector2 genmetVector = TVector2(met_genPt*cos(met_genPhi), met_genPt*sin(met_genPhi));
	diffMetMht_genmet = (mhtVector - genmetVector).Mod();
      }
      TVector2 mhtVectorUP = TVector2(mht_ptJECup*cos(mht_phiJECup), mht_ptJECup*sin(mht_phiJECup));
      TVector2 metVectorUP = TVector2(met_ptJECup*cos(met_phiJECup), met_ptJECup*sin(met_phiJECup));
      diffMetMhtJECup = (mhtVectorUP - metVectorUP).Mod();
      TVector2 mhtVectorDN = TVector2(mht_ptJECdn*cos(mht_phiJECdn), mht_ptJECdn*sin(mht_phiJECdn));
      TVector2 metVectorDN = TVector2(met_ptJECdn*cos(met_phiJECdn), met_ptJECdn*sin(met_phiJECdn));
      diffMetMhtJECdn = (mhtVectorDN - metVectorDN).Mod();

      // HT, MT2 and MHT for photon+jets regions
      //  note that leptons are NOT included in this MT2 calculation
      //  would need to do lepton/photon overlap to include them

      gamma_ht = 0;
      gamma_deltaPhiMin = 999;
      LorentzVector sumMhtp4Gamma = LorentzVector(0,0,0,0);

      // compute HT, MHT using same objects as MT2 inputs
      for (unsigned int ip4 = 0; ip4 < p4sForHemsGamma.size(); ++ip4) {
        gamma_ht += p4sForHemsGamma.at(ip4).pt();
        sumMhtp4Gamma -= p4sForHemsGamma.at(ip4);
      }

      // min(dphi) of 4 leading objects
      for (unsigned int ip4 = 0; ip4 < p4sForDphiGamma.size(); ++ip4) {
        if(ip4 < 4) gamma_deltaPhiMin = min(gamma_deltaPhiMin, DeltaPhi( gamma_met_phi, p4sForDphiGamma.at(ip4).phi() ));
      }

      vector<LorentzVector> hemJetsGamma;
      if(p4sForHemsGamma.size() > 1){
        //Hemispheres used in MT2 calculation
        hemJetsGamma = getHemJets(p4sForHemsGamma);  

        gamma_mt2 = HemMT2(gamma_met_pt, gamma_met_phi, hemJetsGamma.at(0), hemJetsGamma.at(1));

        // order hemispheres by pt for saving
        int idx_lead = 0;
        int idx_subl = 1;
        if (hemJetsGamma.at(1).pt() > hemJetsGamma.at(0).pt()) {
          idx_lead = 1;
          idx_subl = 0;
        }

        gamma_pseudoJet1_pt   = hemJetsGamma.at(idx_lead).pt();
        gamma_pseudoJet1_eta  = hemJetsGamma.at(idx_lead).eta();
        gamma_pseudoJet1_phi  = hemJetsGamma.at(idx_lead).phi();
        gamma_pseudoJet1_mass = hemJetsGamma.at(idx_lead).mass();
        gamma_pseudoJet2_pt   = hemJetsGamma.at(idx_subl).pt();
        gamma_pseudoJet2_eta  = hemJetsGamma.at(idx_subl).eta();
        gamma_pseudoJet2_phi  = hemJetsGamma.at(idx_subl).phi();
        gamma_pseudoJet2_mass = hemJetsGamma.at(idx_subl).mass();

      }

      gamma_mht_pt  = sumMhtp4Gamma.pt();
      gamma_mht_phi = sumMhtp4Gamma.phi();

      TVector2 mhtVectorGamma = TVector2(gamma_mht_pt*cos(gamma_mht_phi), gamma_mht_pt*sin(gamma_mht_phi));
      TVector2 metVectorGamma = TVector2(gamma_met_pt*cos(gamma_met_phi), gamma_met_pt*sin(gamma_met_phi));
      gamma_diffMetMht = (mhtVectorGamma - metVectorGamma).Mod();

      // MT2, MHT for Z-->ll control region
      zll_ht = 0;
      zll_htJECup = 0;
      zll_htJECdn = 0;
      zllmt_ht = 0;
      if (nlep == 2) {
        zll_deltaPhiMin = 999;
        LorentzVector sumMhtp4Zll = LorentzVector(0,0,0,0);
        LorentzVector sumMhtp4ZllUP = LorentzVector(0,0,0,0);
        LorentzVector sumMhtp4ZllDN = LorentzVector(0,0,0,0);

        // compute MHT using same objects as MT2 inputs
        for (unsigned int ip4 = 0; ip4 < p4sForHemsZll.size(); ++ip4) {
          zll_ht += p4sForHemsZll.at(ip4).pt();
          sumMhtp4Zll -= p4sForHemsZll.at(ip4);
        }
        for (unsigned int ip4 = 0; ip4 < p4sForHemsZllUP.size(); ++ip4) {
          zll_htJECup += p4sForHemsZllUP.at(ip4).pt();
          sumMhtp4ZllUP -= p4sForHemsZllUP.at(ip4);
        }
        for (unsigned int ip4 = 0; ip4 < p4sForHemsZllDN.size(); ++ip4) {
          zll_htJECdn += p4sForHemsZllDN.at(ip4).pt();
          sumMhtp4ZllDN -= p4sForHemsZllDN.at(ip4);
        }

        // min(dphi) of 4 leading objects
        for (unsigned int ip4 = 0; ip4 < p4sForDphiZll.size(); ++ip4) {
          if(ip4 < 4) zll_deltaPhiMin = min(zll_deltaPhiMin, DeltaPhi( zll_met_phi, p4sForDphiZll.at(ip4).phi() ));
        }
        // min(dphi) of 4 leading objects
        for (unsigned int ip4 = 0; ip4 < p4sForDphiZllUP.size(); ++ip4) {
          if(ip4 < 4) zll_deltaPhiMinJECup = min(zll_deltaPhiMinJECup, DeltaPhi( zll_met_phiJECup, p4sForDphiZllUP.at(ip4).phi() ));
        }
        // min(dphi) of 4 leading objects
        for (unsigned int ip4 = 0; ip4 < p4sForDphiZllDN.size(); ++ip4) {
          if(ip4 < 4) zll_deltaPhiMinJECdn = min(zll_deltaPhiMinJECdn, DeltaPhi( zll_met_phiJECdn, p4sForDphiZllDN.at(ip4).phi() ));
        }

        vector<LorentzVector> hemJetsZll;
        vector<LorentzVector> hemJetsZllUP;
        vector<LorentzVector> hemJetsZllDN;
        if(p4sForHemsZll.size() > 1){
          //Hemispheres used in MT2 calculation
          hemJetsZll = getHemJets(p4sForHemsZll);  

          zll_mt2 = HemMT2(zll_met_pt, zll_met_phi, hemJetsZll.at(0), hemJetsZll.at(1));

	  // order hemispheres by pt for saving
	  int idx_lead = 0;
	  int idx_subl = 1;
	  if (hemJetsZll.at(1).pt() > hemJetsZll.at(0).pt()) {
	    idx_lead = 1;
	    idx_subl = 0;
	  }
	  
	  zll_pseudoJet1_pt   = hemJetsZll.at(idx_lead).pt();
	  zll_pseudoJet1_eta  = hemJetsZll.at(idx_lead).eta();
	  zll_pseudoJet1_phi  = hemJetsZll.at(idx_lead).phi();
	  zll_pseudoJet1_mass = hemJetsZll.at(idx_lead).mass();
	  zll_pseudoJet2_pt   = hemJetsZll.at(idx_subl).pt();
	  zll_pseudoJet2_eta  = hemJetsZll.at(idx_subl).eta();
	  zll_pseudoJet2_phi  = hemJetsZll.at(idx_subl).phi();
	  zll_pseudoJet2_mass = hemJetsZll.at(idx_subl).mass();
	  
        }
        if(p4sForHemsZllUP.size() > 1){
          hemJetsZllUP = getHemJets(p4sForHemsZllUP);  
          zll_mt2JECup = HemMT2(zll_met_ptJECup, zll_met_phiJECup, hemJetsZllUP.at(0), hemJetsZllUP.at(1));

	  // order hemispheres by pt for saving
	  int idx_lead = 0;
	  int idx_subl = 1;
	  if (hemJetsZllUP.at(1).pt() > hemJetsZllUP.at(0).pt()) {
	    idx_lead = 1;
	    idx_subl = 0;
	  }
	  
	  zll_pseudoJet1JECup_pt   = hemJetsZllUP.at(idx_lead).pt();
	  zll_pseudoJet1JECup_eta  = hemJetsZllUP.at(idx_lead).eta();
	  zll_pseudoJet1JECup_phi  = hemJetsZllUP.at(idx_lead).phi();
	  zll_pseudoJet1JECup_mass = hemJetsZllUP.at(idx_lead).mass();
	  zll_pseudoJet2JECup_pt   = hemJetsZllUP.at(idx_subl).pt();
	  zll_pseudoJet2JECup_eta  = hemJetsZllUP.at(idx_subl).eta();
	  zll_pseudoJet2JECup_phi  = hemJetsZllUP.at(idx_subl).phi();
	  zll_pseudoJet2JECup_mass = hemJetsZllUP.at(idx_subl).mass();

        }
        if(p4sForHemsZllDN.size() > 1){
          hemJetsZllDN = getHemJets(p4sForHemsZllDN);  
          zll_mt2JECdn = HemMT2(zll_met_ptJECdn, zll_met_phiJECdn, hemJetsZllDN.at(0), hemJetsZllDN.at(1));

	  // order hemispheres by pt for saving
	  int idx_lead = 0;
	  int idx_subl = 1;
	  if (hemJetsZllDN.at(1).pt() > hemJetsZllDN.at(0).pt()) {
	    idx_lead = 1;
	    idx_subl = 0;
	  }
	  
	  zll_pseudoJet1JECdn_pt   = hemJetsZllDN.at(idx_lead).pt();
	  zll_pseudoJet1JECdn_eta  = hemJetsZllDN.at(idx_lead).eta();
	  zll_pseudoJet1JECdn_phi  = hemJetsZllDN.at(idx_lead).phi();
	  zll_pseudoJet1JECdn_mass = hemJetsZllDN.at(idx_lead).mass();
	  zll_pseudoJet2JECdn_pt   = hemJetsZllDN.at(idx_subl).pt();
	  zll_pseudoJet2JECdn_eta  = hemJetsZllDN.at(idx_subl).eta();
	  zll_pseudoJet2JECdn_phi  = hemJetsZllDN.at(idx_subl).phi();
	  zll_pseudoJet2JECdn_mass = hemJetsZllDN.at(idx_subl).mass();

        }		  

        zll_mht_pt  = sumMhtp4Zll.pt();
        zll_mht_phi = sumMhtp4Zll.phi();
        zll_mht_ptJECup  = sumMhtp4ZllUP.pt();
        zll_mht_phiJECup = sumMhtp4ZllUP.phi();
        zll_mht_ptJECdn  = sumMhtp4ZllDN.pt();
        zll_mht_phiJECdn = sumMhtp4ZllDN.phi();

        TVector2 mhtVectorZll = TVector2(zll_mht_pt*cos(zll_mht_phi), zll_mht_pt*sin(zll_mht_phi));
        TVector2 metVectorZll = TVector2(zll_met_pt*cos(zll_met_phi), zll_met_pt*sin(zll_met_phi));
        zll_diffMetMht = (mhtVectorZll - metVectorZll).Mod();

        TVector2 mhtVectorZllUP = TVector2(zll_mht_ptJECup*cos(zll_mht_phiJECup), zll_mht_ptJECup*sin(zll_mht_phiJECup));
        TVector2 metVectorZllUP = TVector2(zll_met_ptJECup*cos(zll_met_phiJECup), zll_met_ptJECup*sin(zll_met_phiJECup));
        zll_diffMetMhtJECup = (mhtVectorZllUP - metVectorZllUP).Mod();

        TVector2 mhtVectorZllDN = TVector2(zll_mht_ptJECdn*cos(zll_mht_phiJECdn), zll_mht_ptJECdn*sin(zll_mht_phiJECdn));
        TVector2 metVectorZllDN = TVector2(zll_met_ptJECdn*cos(zll_met_phiJECdn), zll_met_ptJECdn*sin(zll_met_phiJECdn));
        zll_diffMetMhtJECdn = (mhtVectorZllDN - metVectorZllDN).Mod();


        // same for Zll MT region test
        zllmt_deltaPhiMin = 999;
        LorentzVector sumMhtp4ZllMT = LorentzVector(0,0,0,0);

        // compute MHT using same objects as MT2 inputs
        for (unsigned int ip4 = 0; ip4 < p4sForHemsZllMT.size(); ++ip4) {
          zllmt_ht += p4sForHemsZllMT.at(ip4).pt();
          sumMhtp4ZllMT -= p4sForHemsZllMT.at(ip4);
        }

        // min(dphi) of 4 leading objects
        for (unsigned int ip4 = 0; ip4 < p4sForDphiZllMT.size(); ++ip4) {
          if(ip4 < 4) zllmt_deltaPhiMin = min(zllmt_deltaPhiMin, DeltaPhi( zllmt_met_phi, p4sForDphiZllMT.at(ip4).phi() ));
        }

        vector<LorentzVector> hemJetsZllMT;
        if(p4sForHemsZllMT.size() > 1){
          //Hemispheres used in MT2 calculation
          hemJetsZllMT = getHemJets(p4sForHemsZllMT);  

          zllmt_mt2 = HemMT2(zllmt_met_pt, zllmt_met_phi, hemJetsZllMT.at(0), hemJetsZllMT.at(1));

	  // order hemispheres by pt for saving
	  int idx_lead = 0;
	  int idx_subl = 1;
	  if (hemJetsZllMT.at(1).pt() > hemJetsZllMT.at(0).pt()) {
	    idx_lead = 1;
	    idx_subl = 0;
	  }
	  
	  zllmt_pseudoJet1_pt   = hemJetsZllMT.at(idx_lead).pt();
	  zllmt_pseudoJet1_eta  = hemJetsZllMT.at(idx_lead).eta();
	  zllmt_pseudoJet1_phi  = hemJetsZllMT.at(idx_lead).phi();
	  zllmt_pseudoJet1_mass = hemJetsZllMT.at(idx_lead).mass();
	  zllmt_pseudoJet2_pt   = hemJetsZllMT.at(idx_subl).pt();
	  zllmt_pseudoJet2_eta  = hemJetsZllMT.at(idx_subl).eta();
	  zllmt_pseudoJet2_phi  = hemJetsZllMT.at(idx_subl).phi();
	  zllmt_pseudoJet2_mass = hemJetsZllMT.at(idx_subl).mass();

        }	  

        zllmt_mht_pt  = sumMhtp4ZllMT.pt();
        zllmt_mht_phi = sumMhtp4ZllMT.phi();

        TVector2 mhtVectorZllMT = TVector2(zllmt_mht_pt*cos(zllmt_mht_phi), zllmt_mht_pt*sin(zllmt_mht_phi));
        TVector2 metVectorZllMT = TVector2(zllmt_met_pt*cos(zllmt_met_phi), zllmt_met_pt*sin(zllmt_met_phi));
        zllmt_diffMetMht = (mhtVectorZllMT - metVectorZllMT).Mod();


      }

      // MT2, MHT for removed lepton control region
      rl_ht = 0;
      if (nlep == 1) {
        rl_deltaPhiMin = 999;
        LorentzVector sumMhtp4Rl = LorentzVector(0,0,0,0);

        // compute MHT using same objects as MT2 inputs
        for (unsigned int ip4 = 0; ip4 < p4sForHemsRl.size(); ++ip4) {
          rl_ht += p4sForHemsRl.at(ip4).pt();
          sumMhtp4Rl -= p4sForHemsRl.at(ip4);
        }

        // min(dphi) of 4 leading objects
        for (unsigned int ip4 = 0; ip4 < p4sForDphiRl.size(); ++ip4) {
          if(ip4 < 4) rl_deltaPhiMin = min(rl_deltaPhiMin, DeltaPhi( rl_met_phi, p4sForDphiRl.at(ip4).phi() ));
        }

        vector<LorentzVector> hemJetsRl;
        if(p4sForHemsRl.size() > 1){
          //Hemispheres used in MT2 calculation
          hemJetsRl = getHemJets(p4sForHemsRl);  

          rl_mt2 = HemMT2(rl_met_pt, rl_met_phi, hemJetsRl.at(0), hemJetsRl.at(1));

	  // order hemispheres by pt for saving
	  int idx_lead = 0;
	  int idx_subl = 1;
	  if (hemJetsRl.at(1).pt() > hemJetsRl.at(0).pt()) {
	    idx_lead = 1;
	    idx_subl = 0;
	  }
	  
	  rl_pseudoJet1_pt   = hemJetsRl.at(idx_lead).pt();
	  rl_pseudoJet1_eta  = hemJetsRl.at(idx_lead).eta();
	  rl_pseudoJet1_phi  = hemJetsRl.at(idx_lead).phi();
	  rl_pseudoJet1_mass = hemJetsRl.at(idx_lead).mass();
	  rl_pseudoJet2_pt   = hemJetsRl.at(idx_subl).pt();
	  rl_pseudoJet2_eta  = hemJetsRl.at(idx_subl).eta();
	  rl_pseudoJet2_phi  = hemJetsRl.at(idx_subl).phi();
	  rl_pseudoJet2_mass = hemJetsRl.at(idx_subl).mass();

        }	  

        rl_mht_pt  = sumMhtp4Rl.pt();
        rl_mht_phi = sumMhtp4Rl.phi();

        TVector2 mhtVectorRl = TVector2(rl_mht_pt*cos(rl_mht_phi), rl_mht_pt*sin(rl_mht_phi));
        TVector2 metVectorRl = TVector2(rl_met_pt*cos(rl_met_phi), rl_met_pt*sin(rl_met_phi));
        rl_diffMetMht = (mhtVectorRl - metVectorRl).Mod();

      }
      if (!isData) {
        //GEN MT2
        vector<LorentzVector> goodGenJets;
        for(unsigned int iGenJet=0; iGenJet < cms3.genjets_p4NoMuNoNu().size(); iGenJet++){
          if(cms3.genjets_p4NoMuNoNu().at(iGenJet).pt() < 30.0) continue;
          if(fabs(cms3.genjets_p4NoMuNoNu().at(iGenJet).eta()) > 2.4) continue;
          goodGenJets.push_back(cms3.genjets_p4NoMuNoNu().at(iGenJet));
        }
        if(goodGenJets.size() > 1){
          hemJets = getHemJets(goodGenJets);  
          mt2_gen = HemMT2(met_genPt, met_genPhi, hemJets.at(0), hemJets.at(1));
        	  // order hemispheres by pt for saving
	  int idx_lead = 0;
	  int idx_subl = 1;
	  if (hemJets.at(1).pt() > hemJets.at(0).pt()) {
	    idx_lead = 1;
	    idx_subl = 0;
	  }
	  
	  gen_pseudoJet1_pt   = hemJets.at(idx_lead).pt();
	  gen_pseudoJet1_eta  = hemJets.at(idx_lead).eta();
	  gen_pseudoJet1_phi  = hemJets.at(idx_lead).phi();
	  gen_pseudoJet1_mass = hemJets.at(idx_lead).mass();
	  gen_pseudoJet2_pt   = hemJets.at(idx_subl).pt();
	  gen_pseudoJet2_eta  = hemJets.at(idx_subl).eta();
	  gen_pseudoJet2_phi  = hemJets.at(idx_subl).phi();
	  gen_pseudoJet2_mass = hemJets.at(idx_subl).mass();

	}
      } // !isData

      if (verbose) cout << "before taus" << endl;

      //TAUS
      ntau = 0;
      nTaus20 = 0;
      for(unsigned int iTau = 0; iTau < cms3.taus_pf_p4().size(); iTau++){
        if(cms3.taus_pf_p4().at(iTau).pt() < 20.0) continue; 
        if(fabs(cms3.taus_pf_p4().at(iTau).eta()) > 2.3) continue; 
        if (!cms3.passTauID("byLooseCombinedIsolationDeltaBetaCorr3Hits", iTau)) continue; // HPS3 hits taus

        if (ntau >= max_ntau) {
          std::cout << "WARNING: attempted to fill more than " << max_ntau << " taus" << std::endl;
          break;
        }

        tau_pt[ntau]   = cms3.taus_pf_p4().at(iTau).pt();
        tau_eta[ntau]  = cms3.taus_pf_p4().at(iTau).eta();
        tau_phi[ntau]  = cms3.taus_pf_p4().at(iTau).phi();
        tau_mass[ntau] = cms3.taus_pf_p4().at(iTau).M();
        tau_charge[ntau] = cms3.taus_pf_charge().at(iTau);
        tau_dxy[ntau] = 0; // could use the tau->dxy() function instead, but not sure what it does
        tau_dz[ntau] = 0; // not sure how to get this. 
        tau_isoCI3hit[ntau] = cms3.passTauID("byCombinedIsolationDeltaBetaCorrRaw3Hits", iTau);
        int temp = 0;
        if (cms3.passTauID("byLooseCombinedIsolationDeltaBetaCorr3Hits", iTau)) temp = 1;
        if (cms3.passTauID("byMediumCombinedIsolationDeltaBetaCorr3Hits", iTau)) temp = 2;
        if (cms3.passTauID("byTightCombinedIsolationDeltaBetaCorr3Hits", iTau)) temp = 3;
        tau_idCI3hit[ntau] = temp;
        if(tau_pt[ntau] > 20) nTaus20++;
        //tau_mcMatchId[ntau] = ; // Have to do this by hand unless we want to add tau_mc branches in CMS3 through the CandToGenAssMaker

        ntau++;
      }

      if (!isData && applyDummyWeights) {
        // dummy version of generator scales variation
        if (mt2 < 600.) {
          weight_scales_UP = 1.05;
          weight_scales_DN = 0.95;
        }
        else if (mt2 > 600. && mt2 < 1000.) {
          weight_scales_UP = 1.10;
          weight_scales_DN = 0.90;
        }
        else if (mt2 > 1000.) {
          weight_scales_UP = 1.30;
          weight_scales_DN = 0.70;
        }
      }

      if (verbose) cout << "before fill" << endl;

      // do the rebalancing
      if(doRebal){
          nRebalJets = 0;
          if(njet < 2){
              rebal_status = -1;
          }else{
              
              float met_x = (met_pt)*cos(met_phi);
              float met_y = (met_pt)*sin(met_phi);

              float jet_x = 0;
              float jet_y = 0;
              for(int iJet=0; iJet<njet; iJet++){
                  // only consider jets with pt>10
                  if(jet_pt[iJet] < 10.0){
                      rebal_useJet[iJet] = 0;
                      continue;
                  }

                  // don't rebalance jet if it fails pileup-ID and has pt<100
                  if(jet_pt[iJet] < 100.0 && jet_puId[iJet] < 1){
                      rebal_useJet[iJet] = 0;
                      // remove jets that fail pileup-ID from MET calculation
                      met_x += (jet_pt[iJet])*cos(jet_phi[iJet]);
                      met_y += (jet_pt[iJet])*sin(jet_phi[iJet]);
                      continue;
                  }

                  jet_x += (jet_pt[iJet])*cos(jet_phi[iJet]);
                  jet_y += (jet_pt[iJet])*sin(jet_phi[iJet]);
                  rebal_useJet[iJet] = 1;
                  rebal_jetpt[nRebalJets] = jet_pt[iJet];
                  rebal_jeteta[nRebalJets] = jet_eta[iJet];
                  rebal_jetphi[nRebalJets] = jet_phi[iJet];
                  rebal_jetbtagdeepcsv[nRebalJets] = jet_btagDeepCSV[iJet];
                  nRebalJets++;                  
              }

              if(nRebalJets<2){
                  rebal_status = -1;
              }else{

                  // pt-soft should be sum of jet pt from jets with pt<10
                  rebal_pt_soft_x = -met_x - jet_x;
                  rebal_pt_soft_y = -met_y - jet_y;
                  rebal_status = 1;

                  thisBabyMaker = this;
                  TMinuit* minimizer = new TMinuit(nRebalJets);
                  minimizer->SetFCN(minuitFunction);
                  int iflag = 0;
                  Double_t arglist[10];

                  // suppress warnings so the log files aren't multiple GB
                  minimizer->SetPrintLevel(-1);
                  minimizer->mnexcm("SET NOWarnings",0,0,iflag);

                  arglist[0] = 0;
                  minimizer->mnexcm("SET STRATEGY", arglist, 1, iflag);

                  minimizer->SetErrorDef(0.5);

                  for(int i=0; i<nRebalJets; i++){
                      std::string name = Form("c%d", i);
                      minimizer->mnparm(i,name,1.0,0.05,0.1,3,iflag);
                  }

                  arglist[0] = 10000;
                  arglist[1] = 1.0;

                  minimizer->mnexcm("MIGRAD", arglist, 2, iflag);
                  if (verbose) std::cout << "MIGRAD iflag = " << iflag << std::endl;
                  rebal_status = iflag;
      
                  if(iflag !=0){
                      arglist[1] = 10.0;//easier threshold for convergence
                      minimizer->mnexcm("MIGRAD", arglist, 2, iflag);
                      if(verbose) std::cout << "second MIGRAD iflag = " << iflag << std::endl;
                      rebal_status = iflag;
                  }
      
                  arglist[0] = 5000;
                  arglist[1] = 0;
                  arglist[2] = 1;

                  if(iflag !=0){
                      minimizer->mnexcm("MINOS", arglist, 3, iflag);
                      rebal_status = iflag;
                  }
         
                  for(int i=0; i<nRebalJets; i++){
                      double par_value;
                      double par_error;
                      minimizer->GetParameter(i, par_value, par_error);
                      par_value = 1.0/par_value;
                      rebal_jetpt[i] *= par_value;
                      rebal_factors[i] = par_value; 
                  }

                  delete minimizer;

                  jet_x = 0;
                  jet_y = 0;
                  for(int i=0; i<nRebalJets; i++){
                      jet_x += (rebal_jetpt[i])*cos(rebal_jetphi[i]);
                      jet_y += (rebal_jetpt[i])*sin(rebal_jetphi[i]);
                  }
                  // new met is negative sum of original jet pt for jets w/ pt<10 and new rebalanced pt for other jets
                  float new_met_x = -rebal_pt_soft_x - jet_x;
                  float new_met_y = -rebal_pt_soft_y - jet_y;
                  rebal_met_pt = sqrt(new_met_x*new_met_x + new_met_y*new_met_y);
                  rebal_met_phi = atan2(new_met_y, new_met_x);

              }
          }
      } // end rebalancing

      //////////////
      // Short Track
      //////////////

      if (verbose) std::cout << "Before short tracks" << std::endl;

      if (doShortTrackInfo) {	

	unsigned int Ch1_idx = 1000000; unsigned int Ch2_idx = 1000000;
	const int charginoStatus = isFastsim ? 22 : 1;
	if (!isData) {
	  if (verbose) cout << "Before chargino matching" << endl;
	  // find charginos and see if they have decay lengths in the range of interest, 10 to 100 cm
	  for (unsigned int iGen = 0; iGen < cms3.genps_p4().size(); iGen++) {	      
	    if (abs(cms3.genps_id().at(iGen)) != 1000024 || cms3.genps_status().at(iGen) != charginoStatus) continue; // looking for final state charginos
	    // chargino_decayXY[nCharginos] = cms3.genps_decayXY().at(iGen);
	    chargino_eta[nCharginos] = cms3.genps_p4().at(iGen).eta();
	    chargino_phi[nCharginos] = cms3.genps_p4().at(iGen).phi();
	    chargino_pt[nCharginos] = cms3.genps_p4().at(iGen).pt();  
	    // Charginos outside this volume usually don't leave disappearing tracks
	    chargino_isDisappearing[nCharginos] = chargino_decayXY[nCharginos] > 10 && chargino_decayXY[nCharginos] < 100 && fabs(chargino_eta[nCharginos]) < 2.4;
	    float minDR = 10000; int matchIdx = -1;
	    for (unsigned int i_it = 0; i_it < cms3.isotracks_p4().size(); i_it++) {
	      if (!cms3.isotracks_fromPV().at(i_it)) continue; // don't consider pileup
	      if (cms3.isotracks_numberOfLostPixelHitsInner().at(i_it) > 1) continue; // tracks with many MIHs are just noise
	      float dr = DeltaR(cms3.genps_p4().at(iGen).eta(),cms3.isotracks_etatrk().at(i_it),cms3.genps_p4().at(iGen).phi(),cms3.isotracks_phitrk().at(i_it));
	      if (dr < minDR) {
		minDR = dr;
		matchIdx = i_it;
	      }	
	    }
	    // Save DR to nearest isotracks
	    chargino_minDR[nCharginos] = minDR;
	    nCharginos++;
	    // if it's a good match, save which track it was
	    if (minDR < 0.01) {
	      if (nCharginos == 1) Ch1_idx = matchIdx;
	      else if (nCharginos == 2) Ch2_idx = matchIdx;
	      else cout << "Found more than 2 charginos!!!!" << endl;
	    }
	  }
	}

	ntracks = 0; nshorttracks = 0; nshorttrackcandidates = 0;
	nshorttracks_P = 0; nshorttracks_M = 0; nshorttracks_L = 0;
	nshorttrackcandidates_P = 0; nshorttrackcandidates_M = 0; nshorttrackcandidates_L = 0;
	for (unsigned int i_it = 0; i_it < cms3.isotracks_p4().size(); i_it++) {
	  if (!cms3.isotracks_fromPV().at(i_it)) continue;

	  // If not a chargino, need to pass some requirements
	  if (i_it != Ch1_idx && i_it != Ch2_idx) {
	    if (!cms3.isotracks_isLostTrack().at(i_it)) continue;
	    if (cms3.isotracks_pttrk().at(i_it) < 15) continue;
	  }
	  else if (i_it == Ch1_idx) {
	    chargino_matchedTrackIdx[0] = ntracks;
	    track_matchedCharginoIdx[ntracks] = 0;
	  }
	  else {
	    chargino_matchedTrackIdx[1] = ntracks;
	    track_matchedCharginoIdx[ntracks] = 1;
	  }

	  // If this track is to be saved only because a chargino matches to it, set bits indicating why it wouldn't normally be saved.
	  if (!cms3.isotracks_isLostTrack().at(i_it)) {
	    track_isBadCharginoTrack[ntracks] += 1<<0;
	    cout << "Non-lostTrack chargino, track " << i_it << (cms3.isotracks_isPFCand().at(i_it) ? ": PF cand "  : ": General " ) << cms3.isotracks_particleId().at(i_it) << " decayXY " << chargino_decayXY[track_matchedCharginoIdx[ntracks]] << " nLayers " << cms3.isotracks_trackerLayersWithMeasurement().at(i_it) << " genpt " << chargino_pt[track_matchedCharginoIdx[ntracks]] << " recopt " << cms3.isotracks_pttrk().at(i_it) << endl;
	  }
	  if (cms3.isotracks_pttrk().at(i_it) < 15) {
	    track_isBadCharginoTrack[ntracks] += 1<<1;
	    cout << "Chargino with pt < 15 GeV" << endl;
	  }

	  if (verbose) std::cout << "Before short track gen matching" << std::endl;

	  //	  cout << "matched chargino index for track " << i_it << " (cms4) " << ntracks << " (baby): " << track_matchedCharginoIdx[ntracks] << ", ch1 = " << Ch1_idx << ", ch2 = " << Ch2_idx << endl;

	  // Now that we know we're saving this track, do full gen matching.
	  // Empirical fact: when a chargino's nearest track is within 0.01, that track is always also nearest the chargino.
	  // Empirical fact: essentially all charginos that leave a good track are within DR ~ 0.008 of the track.
	  if (!isData) {
	    float minGenDR = 0.01; int genIdx = -1; float minChgDR = 10000;
	    for (unsigned int iGen = 0; iGen < cms3.genps_p4().size(); iGen++) {	      
	      // Only consider charged, status 1 GPs
	      if (fabs(cms3.genps_charge().at(iGen)) < 0.01 || cms3.genps_status().at(iGen) != 1) continue;
	      const float dr = DeltaR(cms3.genps_p4().at(iGen).eta(),cms3.isotracks_etatrk().at(i_it),cms3.genps_p4().at(iGen).phi(),cms3.isotracks_phitrk().at(i_it));
	      if (dr < minGenDR) {
		minGenDR = dr;
		genIdx = iGen;
	      }
	      if (abs(cms3.genps_id().at(iGen)) == 1000024 && dr < minChgDR) {
		minChgDR = dr;
	      }
	    }
	    track_nearestCharginoDR[ntracks] = minChgDR;
	    // If *any* charged, status 1 GP was within DR < 0.01 of the track...
	    if (genIdx >= 0) {
	      track_genMatchDR[ntracks] = minGenDR;
	      track_genPdgId[ntracks] = cms3.genps_id().at(genIdx);
	      // If we matched a chargino ...
	      if (abs(track_genPdgId[ntracks]) == 1000024) {
		// track_decayXY[ntracks] = cms3.genps_decayXY().at(genIdx);
		track_isDisappearingChargino[ntracks] = track_decayXY[ntracks] > 10 && track_decayXY[ntracks] < 100;
	      } 
	      // if a chargino matched this track, but this track did not match a chargino...
	      else if (track_matchedCharginoIdx[ntracks] >= 0 && chargino_decayXY[track_matchedCharginoIdx[ntracks]] >= 10) {
		cout << "A chargino with > 10cm decay length is matched to a track that matches to a non-chargino. " << run << ":" << lumi << ":" << evt << endl;
		cout << "Gen idx: " << genIdx << endl;
		cout << "Matched ID: " << track_genPdgId[ntracks] << endl;
		cout << "Track MIHs: " << cms3.isotracks_numberOfLostPixelHitsInner().at(i_it) << endl;
		cout << "Matched chargino idx: " << track_matchedCharginoIdx[ntracks] << endl;
		cout << "Chargino decayXY: " << chargino_decayXY[track_matchedCharginoIdx[ntracks]] << endl;
		cout << "Chargino DR: " << chargino_minDR[track_matchedCharginoIdx[ntracks]] << " Matched DR: " << minGenDR << " Min Chg DR: " << minChgDR << endl;
		cout << "Track idx (baby): " << ntracks << endl;
		cout << "Track idx (cms4): " << (track_matchedCharginoIdx[ntracks] == 0 ? Ch1_idx : Ch2_idx) << endl;
	      } // special cases for chargino matches
	    } // we found a good gen match	  
	  } // not data

	  // Candidate kinematics
	  //	const LorentzVector& cand_p4 = cms3.isotracks_p4().at(i_it);
	  
	  // Kinematics
	  // The track-based quantities are *almost* always identical to the candidate quantities for lostTracks.
	  track_pt[ntracks] = cms3.isotracks_pttrk().at(i_it);
	  track_ptErr[ntracks] = cms3.isotracks_pterr().at(i_it);
	  track_eta[ntracks] = cms3.isotracks_etatrk().at(i_it);
	  track_phi[ntracks] = cms3.isotracks_phitrk().at(i_it);
	  track_charge[ntracks] = cms3.isotracks_charge().at(i_it);
	  track_dedxStrip[ntracks] = cms3.isotracks_dEdxStrip().at(i_it);
	  track_dedxPixel[ntracks] = cms3.isotracks_dEdxPixel().at(i_it);
	  
	  // Quality
	  track_nChi2[ntracks] = cms3.isotracks_normChi2().at(i_it);
	  track_dxy[ntracks] = cms3.isotracks_dxy().at(i_it);
	  track_dxyErr[ntracks] = cms3.isotracks_dxyError().at(i_it);
	  track_dz[ntracks] = fabs(cms3.isotracks_dz().at(i_it));
	  track_dzErr[ntracks] = cms3.isotracks_dzError().at(i_it);
	  track_ipSigXY[ntracks] = track_dxyErr[ntracks] > 0 ? track_dxy[ntracks] / track_dxyErr[ntracks] : -1;
	  track_isHighPurity[ntracks] = cms3.isotracks_isHighPurityTrack().at(i_it);
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
	  
	  // Hits and layers
	  track_HitSignature[ntracks] = cms3.isotracks_HitSignature().at(i_it);
	  track_nPixelHits[ntracks] = cms3.isotracks_numberOfValidPixelHits().at(i_it);
	  track_nLostOuterHits[ntracks] = cms3.isotracks_numberOfLostHitsOuter().at(i_it);
	  track_nLostInnerPixelHits[ntracks] = cms3.isotracks_numberOfLostPixelHitsInner().at(i_it);
	  track_nLayersWithMeasurement[ntracks] = cms3.isotracks_trackerLayersWithMeasurement().at(i_it);
	  track_nPixelLayersWithMeasurement[ntracks] = cms3.isotracks_pixelLayersWithMeasurement().at(i_it);
	  
	  // Nearest PF
	  track_nearestPF_id[ntracks] = cms3.isotracks_nearestPF_id().at(i_it);
	  track_nearestPF_DR[ntracks] = cms3.isotracks_nearestPF_DR().at(i_it);
	  track_nearestPF_pt[ntracks] = cms3.isotracks_nearestPF_pt().at(i_it);
	  
	  // Nearest jet
	  float minJetDR = 0.5; int jetIdx = -1;
	  for (int iJet = 0; iJet < nJet30; iJet++) {
	    const float jPhi = jet_phi[iJet];
	    const float jEta = jet_eta[iJet];
	    const float dr = DeltaR(jEta,track_eta[ntracks],jPhi,track_phi[ntracks]);
	    if (dr < minJetDR) {
	      minJetDR = dr;
	      jetIdx = iJet;
	    }
	  }
	  if (jetIdx >= 0) {
	    track_jetDR[ntracks] = minJetDR;
	    track_jetPt[ntracks] = jet_pt[jetIdx];
	    track_jetEta[ntracks] = jet_eta[jetIdx];
	    track_jetPhi[ntracks] = jet_phi[jetIdx];
	  }

	  if (verbose) std::cout << "Before short track pileup and isolation" << std::endl;
	  
	  // Pileup
	  track_chHIso0p3[ntracks] = cms3.isotracks_pfIso_ch().at(i_it); // These two isos are used to slim the isotracks collection for cand_p4.pt() < 20 GeV,
	  track_chHminiIso[ntracks] = cms3.isotracks_miniIso_ch().at(i_it); // using cand_p4 for the relative iso pt.
	  track_neuHIso0p3[ntracks] = cms3.isotracks_pfIso_nh().at(i_it);
	  track_neuHminiIso[ntracks] = cms3.isotracks_miniIso_nh().at(i_it);
	  track_phIso0p3[ntracks] = cms3.isotracks_pfIso_em().at(i_it);
	  track_phminiIso[ntracks] = cms3.isotracks_miniIso_em().at(i_it);
	  
	  const float nIso = track_neuHIso0p3[ntracks] + track_phIso0p3[ntracks];
	  track_iso_uncorrected[ntracks] = track_chHIso0p3[ntracks] + nIso;
	  track_reliso_uncorrected[ntracks] = track_iso_uncorrected[ntracks] / track_pt[ntracks];
	  track_iso_correction[ntracks] = 0.5 * cms3.isotracks_pfIso_db().at(i_it);
	  track_reliso_correction[ntracks] = track_iso_correction[ntracks] / track_pt[ntracks];
	  track_iso[ntracks] = max(0.0, (double) track_iso_uncorrected[ntracks]- track_iso_correction[ntracks]);
	  track_reliso[ntracks] = max(0.0, (double) track_reliso_uncorrected[ntracks] - track_reliso_correction[ntracks]);
	  track_isonomin[ntracks] = track_iso_uncorrected[ntracks] - track_iso_correction[ntracks];
	  track_relisonomin[ntracks] = track_reliso_uncorrected[ntracks] - track_reliso_correction[ntracks];
	  
	  const float nMiniIso = track_neuHminiIso[ntracks] + track_phminiIso[ntracks];
	  track_miniiso_uncorrected[ntracks] = track_chHminiIso[ntracks] + nMiniIso;
	  track_minireliso_uncorrected[ntracks] = track_miniiso_uncorrected[ntracks] / track_pt[ntracks];
	  track_miniiso_correction[ntracks] = 0.5 * cms3.isotracks_miniIso_db().at(i_it);
	  track_minireliso_correction[ntracks] = track_miniiso_correction[ntracks] / track_pt[ntracks];
	  track_miniiso[ntracks] = max(0.0, (double) track_miniiso_uncorrected[ntracks] - track_miniiso_correction[ntracks]);
	  track_minireliso[ntracks] = max(0.0, (double) track_minireliso_uncorrected[ntracks] - track_minireliso_correction[ntracks]);
	  track_miniisonomin[ntracks] = track_miniiso_uncorrected[ntracks] - track_miniiso_correction[ntracks];
	  track_minirelisonomin[ntracks] = track_minireliso_uncorrected[ntracks] - track_minireliso_correction[ntracks];
	  
	  track_neuIso0p05[ntracks] = cms3.isotracks_pfNeutralSum().at(i_it);
	  track_neuRelIso0p05[ntracks] = track_neuIso0p05[ntracks] / track_pt[ntracks];
	  
	  track_isLepOverlap[ntracks] = cms3.isotracks_lepOverlap().at(i_it);
	  

	  // Apply ST and STC selections	
	  
	  // Bad chargino tracks aren't ST(C)s
	  if (track_isBadCharginoTrack[ntracks]) {
	    ntracks++;
	    continue;
	  }

	  // Apply reco veto first so we can store the result for all tracks. We can't fully reconsider the short track definition  without remaking babies otherwise.
	  if (verbose) std::cout << "Before short track lepton rejection" << std::endl;
	  
	  // Lepton veto
	  // Veto on ANY reco lepton, not just those passing MT2 IDs
	  const bool nearestPFSel = !(track_nearestPF_DR[ntracks] < 0.1 && (abs(track_nearestPF_id[ntracks]) == 11 || abs(track_nearestPF_id[ntracks]) == 13));
	  float minrecodr = 0.2; float overlap_id = 0;
	  for (unsigned int i_el = 0; i_el < cms3.els_p4().size(); i_el++) {
	    float dr = DeltaR(cms3.els_p4().at(i_el).eta(),track_eta[ntracks],cms3.els_p4().at(i_el).phi(),track_phi[ntracks]);
	    if (dr < minrecodr) {
	      minrecodr = dr;
	      overlap_id = 11;
	    }
	  }
	  for (unsigned int i_mu = 0; i_mu < cms3.mus_p4().size(); i_mu++) {
	    float dr = DeltaR(cms3.mus_p4().at(i_mu).eta(),track_eta[ntracks],cms3.mus_p4().at(i_mu).phi(),track_phi[ntracks]);
	    if (dr < minrecodr) {
	      minrecodr = dr;
	      overlap_id = 13;
	    }
	  }
	  const bool recoVeto = minrecodr < 0.2;
	  if (verbose && recoVeto && !track_isLepOverlap[ntracks]) 
              std::cout << "Rejected a short track due to lepton reco veto (DR = " << minrecodr << ") without corresponding PF lep. " << run << ":" << lumi << ":" << evt << std::endl;
	  // Get rid of anything near or overlapping a lepton
	  const bool PassesRecoVeto = nearestPFSel && !track_isLepOverlap[ntracks] && !recoVeto;
	
	  track_recoveto[ntracks] = overlap_id;

	  if (!PassesRecoVeto) {ntracks++; continue;}

	  // Basic selections
	  const bool DeadCAL = track_DeadECAL[ntracks] || track_DeadHCAL[ntracks];
	  const bool ptSel = track_pt[ntracks] > 15;
	  const bool etaSel = fabs(track_eta[ntracks]) < 2.4;
	  const bool BaseSel = !DeadCAL && etaSel && ptSel;

	  if (!BaseSel) {ntracks++; continue;}
	  
	  // Length and categorization
	  const bool lostOuterHitsSel = track_nLostOuterHits[ntracks] >= 2;
	  const bool isP = track_nLayersWithMeasurement[ntracks] == track_nPixelLayersWithMeasurement[ntracks] && lostOuterHitsSel;
	  const bool isLong = track_nLayersWithMeasurement[ntracks] >= 7;
	  const bool isM = !isP && !isLong && lostOuterHitsSel;
	  const bool isL = isLong && lostOuterHitsSel;
	  const bool isShort = isP || isM || isL;
	  
	  if (!isShort) {ntracks++; continue;}
	  	  
	  // Isolation
	  const bool PassesIsoSelSTC = track_neuIso0p05[ntracks] < 10 * isoSTC && track_neuRelIso0p05[ntracks] < 0.1 * isoSTC && track_iso[ntracks] < 10 * isoSTC && track_reliso[ntracks] < 0.2 * isoSTC;
	  if (!PassesIsoSelSTC) {ntracks++; continue;}
	  const bool PassesIsoSel = track_neuIso0p05[ntracks] < 10 && track_neuRelIso0p05[ntracks] < 0.1 && track_iso[ntracks] < 10 && track_reliso[ntracks] < 0.2;
	  
	  // Quality
	  const bool PixLayersSel = track_nPixelLayersWithMeasurement[ntracks] >= (track_nLayersWithMeasurement[ntracks] > 4 ? 2 : 3);
	  const bool LostInnerPixelHitsSel = track_nLostInnerPixelHits[ntracks] == 0;
	  
	  const bool QualitySelBase = PixLayersSel && LostInnerPixelHitsSel && track_isHighPurity[ntracks];

	  if (!QualitySelBase) {ntracks++; continue;}
	  
	  const bool ptErrOverPt2Sel = track_ptErr[ntracks] / (track_pt[ntracks]*track_pt[ntracks]) < (isP ? 0.2 : (isM ? 0.02 : 0.005));
	  const bool ptErrOverPt2SelSTC = track_ptErr[ntracks] / (track_pt[ntracks]*track_pt[ntracks]) < (isP ? 0.2 : (isM ? 0.02 : 0.005)) * qualSTC;
	  const bool dxySel = fabs(track_dxy[ntracks]) < (isP ? 0.02 : 0.01);
	  const bool dxySelSTC = fabs(track_dxy[ntracks]) < (isP ? 0.02 : 0.01) * qualSTC; 
	  const bool dzSel = fabs(track_dz[ntracks]) < 0.05;
	  const bool dzSelSTC = fabs(track_dz[ntracks]) < 0.05 * qualSTC;
	  
	  const bool QualityST = ptErrOverPt2Sel && dxySel && dzSel;
	  const bool QualitySTC = ptErrOverPt2SelSTC && dxySelSTC && dzSelSTC;
	  
	  // Due to isotrack slimming selection, we only have isotracks with ChH < 5.0 GeV, ChH / cand_pt < 0.2, miniChH / cand_pt < 0.2 for tracks with cand pt < 20 GeV
	  
	  // Does this isotrack pass short track quality and isolation?
	  if (QualityST && PassesIsoSel) {
	    nshorttracks++;
	    track_isshort[ntracks] = 1;
	    if (isP) {nshorttracks_P++; track_ispixelonly[ntracks] = 1;}
	    else if (isM) {nshorttracks_M++; track_ismedium[ntracks] = 1;}
	    else {nshorttracks_L++; track_islong[ntracks] = 1;}	   
	  }
	  // STCs (can't be STs also)
	  // Already checked for IsoSTC above
	  if (QualitySTC && ! track_isshort[ntracks]) {
	    nshorttrackcandidates++;
	    track_iscandidate[ntracks] = 1;
	    if (isP) {nshorttrackcandidates_P++; track_ispixelonlycandidate[ntracks] = 1;}
	    else if (isM) {nshorttrackcandidates_M++; track_ismediumcandidate[ntracks] = 1;}
	    else {nshorttrackcandidates_L++; track_islongcandidate[ntracks] = 1;}	  
	  }
	
	  ntracks++;
	} // End isotracks loop (short tracks)
      } // End short track info

      FillBabyNtuple();
      
    }//end loop on events in a file
    
    delete tree;
    f->Close();
    
    if (applyJECfromFile) {
      delete jet_corrector_pfL1FastJetL2L3;
      if (jet_corrector_pfL1FastJetL2L3_postrun278802) delete jet_corrector_pfL1FastJetL2L3_postrun278802;
      if (!isDataFromFileName) delete jetcorr_uncertainty;  
    }
    
  }//end loop on files
  
  cout << "Processed " << nEventsTotal << " events" << endl;
  if ( nEventsChain != nEventsTotal ) {
    std::cout << "ERROR: number of events from files is not equal to total number of events" << std::endl;
  }
  
  cout << nDuplicates << " duplicate events were skipped." << endl;
  cout << nFailJSON << " events were removed by JSON." << endl;
  
  CloseBabyNtuple();
  
  if (applyBtagSFs) {
    delete calib;
    /*
      delete reader_heavy;
      delete reader_heavy_UP;
      delete reader_heavy_DN;
      delete reader_light;
      delete reader_light_UP;
      delete reader_light_DN;
    */
    delete reader_fullsim;
    if (isFastsim) {
      delete calib_fastsim;
      delete reader_fastsim;
    }
  }
  
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

void babyMaker::MakeBabyNtuple(const char *BabyFilename){

  BabyFile_ = new TFile(Form("%s", BabyFilename), "RECREATE");
  BabyFile_->cd();
  BabyTree_ = new TTree("mt2", "A Baby Ntuple");
  
  BabyTree_->Branch("run", &run );
  BabyTree_->Branch("lumi", &lumi );
  BabyTree_->Branch("evt", &evt, "evt/l" );
  BabyTree_->Branch("isData", &isData );
  BabyTree_->Branch("isGolden", &isGolden );
  BabyTree_->Branch("evt_scale1fb", &evt_scale1fb);
  BabyTree_->Branch("evt_xsec", &evt_xsec );
  BabyTree_->Branch("evt_kfactor", &evt_kfactor );
  BabyTree_->Branch("evt_filter", &evt_filter );
  BabyTree_->Branch("evt_nEvts", &evt_nEvts, "evt_nEvts/l" );
  BabyTree_->Branch("evt_id", &evt_id );
  BabyTree_->Branch("genWeight", &genWeight );
  BabyTree_->Branch("weight_L1prefire", &weight_L1prefire );
  BabyTree_->Branch("weight_L1prefire_UP", &weight_L1prefire_UP );
  BabyTree_->Branch("weight_L1prefire_DN", &weight_L1prefire_DN );
  BabyTree_->Branch("puWeight", &puWeight );
  BabyTree_->Branch("nVert", &nVert );
  BabyTree_->Branch("nTrueInt", &nTrueInt );
  BabyTree_->Branch("rho", &rho );
  BabyTree_->Branch("nJet30", &nJet30 );
  BabyTree_->Branch("nJet30JECup", &nJet30JECup );
  BabyTree_->Branch("nJet30JECdn", &nJet30JECdn );
  BabyTree_->Branch("nJet40", &nJet40 );
  BabyTree_->Branch("nBJet20", &nBJet20 );
  BabyTree_->Branch("nBJet20csv", &nBJet20csv );
  BabyTree_->Branch("nBJet20JECup", &nBJet20JECup );
  BabyTree_->Branch("nBJet20JECdn", &nBJet20JECdn );
  BabyTree_->Branch("nBJet25", &nBJet25 );
  BabyTree_->Branch("nBJet30", &nBJet30 );
  BabyTree_->Branch("nBJet30csv", &nBJet30csv );
  BabyTree_->Branch("nBJet40", &nBJet40 );
  BabyTree_->Branch("nJet30FailId", &nJet30FailId );
  BabyTree_->Branch("nJet100FailId", &nJet100FailId );
  BabyTree_->Branch("nJet20BadFastsim", &nJet20BadFastsim );
  BabyTree_->Branch("nJet200MuFrac50DphiMet", &nJet200MuFrac50DphiMet );
  BabyTree_->Branch("nHEMJet20", &nHEMJet20 );
  BabyTree_->Branch("nHEMJet30", &nHEMJet30 );
  BabyTree_->Branch("nHEMJet40", &nHEMJet40 );
  BabyTree_->Branch("nMuons10", &nMuons10 );
  BabyTree_->Branch("nBadMuons20", &nBadMuons20 );
  BabyTree_->Branch("nElectrons10", &nElectrons10 );
  BabyTree_->Branch("nLepHighMT", &nLepHighMT );
  BabyTree_->Branch("nLepLowMT", &nLepLowMT );
  BabyTree_->Branch("nTaus20", &nTaus20 );
  BabyTree_->Branch("nGammas20", &nGammas20 );
  BabyTree_->Branch("nPFCHCand3", &nPFCHCand3 );
  BabyTree_->Branch("deltaPhiMin", &deltaPhiMin );
  BabyTree_->Branch("deltaPhiMin_genmet", &deltaPhiMin_genmet );
  BabyTree_->Branch("diffMetMht", &diffMetMht );
  BabyTree_->Branch("diffMetMht_genmet", &diffMetMht_genmet );
  BabyTree_->Branch("deltaPhiMinJECup", &deltaPhiMinJECup );
  BabyTree_->Branch("deltaPhiMinJECdn", &deltaPhiMinJECdn );
  BabyTree_->Branch("diffMetMhtJECup", &diffMetMhtJECup );
  BabyTree_->Branch("diffMetMhtJECdn", &diffMetMhtJECdn );
  BabyTree_->Branch("minMTBMet", &minMTBMet );
  BabyTree_->Branch("zll_minMTBMet", &zll_minMTBMet );
  BabyTree_->Branch("gamma_minMTBMet", &gamma_minMTBMet );
  BabyTree_->Branch("ht", &ht );
  BabyTree_->Branch("htJECup", &htJECup );
  BabyTree_->Branch("htJECdn", &htJECdn );
  BabyTree_->Branch("mt2", &mt2 );
  BabyTree_->Branch("mt2JECup", &mt2JECup );
  BabyTree_->Branch("mt2JECdn", &mt2JECdn );
  BabyTree_->Branch("mt2_gen", &mt2_gen );
  BabyTree_->Branch("mt2_genmet", &mt2_genmet );
  BabyTree_->Branch("jet1_pt", &jet1_pt );
  BabyTree_->Branch("jet2_pt", &jet2_pt );
  BabyTree_->Branch("jet1_ptJECup", &jet1_ptJECup );
  BabyTree_->Branch("jet2_ptJECup", &jet2_ptJECup );
  BabyTree_->Branch("jet1_ptJECdn", &jet1_ptJECdn );
  BabyTree_->Branch("jet2_ptJECdn", &jet2_ptJECdn );
  BabyTree_->Branch("jet_failFSveto", &jet_failFSveto );
  BabyTree_->Branch("gamma_jet1_pt", &gamma_jet1_pt );
  BabyTree_->Branch("gamma_jet2_pt", &gamma_jet2_pt );
  BabyTree_->Branch("pseudoJet1_pt", &pseudoJet1_pt );
  BabyTree_->Branch("pseudoJet1_eta", &pseudoJet1_eta );
  BabyTree_->Branch("pseudoJet1_phi", &pseudoJet1_phi );
  BabyTree_->Branch("pseudoJet1_mass", &pseudoJet1_mass );
  BabyTree_->Branch("pseudoJet2_pt", &pseudoJet2_pt );
  BabyTree_->Branch("pseudoJet2_eta", &pseudoJet2_eta );
  BabyTree_->Branch("pseudoJet2_phi", &pseudoJet2_phi );
  BabyTree_->Branch("pseudoJet2_mass", &pseudoJet2_mass );
  BabyTree_->Branch("pseudoJet1JECup_pt", &pseudoJet1JECup_pt );
  BabyTree_->Branch("pseudoJet1JECup_eta", &pseudoJet1JECup_eta );
  BabyTree_->Branch("pseudoJet1JECup_phi", &pseudoJet1JECup_phi );
  BabyTree_->Branch("pseudoJet1JECup_mass", &pseudoJet1JECup_mass );
  BabyTree_->Branch("pseudoJet2JECup_pt", &pseudoJet2JECup_pt );
  BabyTree_->Branch("pseudoJet2JECup_eta", &pseudoJet2JECup_eta );
  BabyTree_->Branch("pseudoJet2JECup_phi", &pseudoJet2JECup_phi );
  BabyTree_->Branch("pseudoJet2JECup_mass", &pseudoJet2JECup_mass );
  BabyTree_->Branch("pseudoJet1JECdn_pt", &pseudoJet1JECdn_pt );
  BabyTree_->Branch("pseudoJet1JECdn_eta", &pseudoJet1JECdn_eta );
  BabyTree_->Branch("pseudoJet1JECdn_phi", &pseudoJet1JECdn_phi );
  BabyTree_->Branch("pseudoJet1JECdn_mass", &pseudoJet1JECdn_mass );
  BabyTree_->Branch("pseudoJet2JECdn_pt", &pseudoJet2JECdn_pt );
  BabyTree_->Branch("pseudoJet2JECdn_eta", &pseudoJet2JECdn_eta );
  BabyTree_->Branch("pseudoJet2JECdn_phi", &pseudoJet2JECdn_phi );
  BabyTree_->Branch("pseudoJet2JECdn_mass", &pseudoJet2JECdn_mass );
  BabyTree_->Branch("zll_pseudoJet1_pt", &zll_pseudoJet1_pt );
  BabyTree_->Branch("zll_pseudoJet1_eta", &zll_pseudoJet1_eta );
  BabyTree_->Branch("zll_pseudoJet1_phi", &zll_pseudoJet1_phi );
  BabyTree_->Branch("zll_pseudoJet1_mass", &zll_pseudoJet1_mass );
  BabyTree_->Branch("zll_pseudoJet2_pt", &zll_pseudoJet2_pt );
  BabyTree_->Branch("zll_pseudoJet2_eta", &zll_pseudoJet2_eta );
  BabyTree_->Branch("zll_pseudoJet2_phi", &zll_pseudoJet2_phi );
  BabyTree_->Branch("zll_pseudoJet2_mass", &zll_pseudoJet2_mass );
  BabyTree_->Branch("zll_pseudoJet1JECup_pt", &zll_pseudoJet1JECup_pt );
  BabyTree_->Branch("zll_pseudoJet1JECup_eta", &zll_pseudoJet1JECup_eta );
  BabyTree_->Branch("zll_pseudoJet1JECup_phi", &zll_pseudoJet1JECup_phi );
  BabyTree_->Branch("zll_pseudoJet1JECup_mass", &zll_pseudoJet1JECup_mass );
  BabyTree_->Branch("zll_pseudoJet2JECup_pt", &zll_pseudoJet2JECup_pt );
  BabyTree_->Branch("zll_pseudoJet2JECup_eta", &zll_pseudoJet2JECup_eta );
  BabyTree_->Branch("zll_pseudoJet2JECup_phi", &zll_pseudoJet2JECup_phi );
  BabyTree_->Branch("zll_pseudoJet2JECup_mass", &zll_pseudoJet2JECup_mass );
  BabyTree_->Branch("zll_pseudoJet1JECdn_pt", &zll_pseudoJet1JECdn_pt );
  BabyTree_->Branch("zll_pseudoJet1JECdn_eta", &zll_pseudoJet1JECdn_eta );
  BabyTree_->Branch("zll_pseudoJet1JECdn_phi", &zll_pseudoJet1JECdn_phi );
  BabyTree_->Branch("zll_pseudoJet1JECdn_mass", &zll_pseudoJet1JECdn_mass );
  BabyTree_->Branch("zll_pseudoJet2JECdn_pt", &zll_pseudoJet2JECdn_pt );
  BabyTree_->Branch("zll_pseudoJet2JECdn_eta", &zll_pseudoJet2JECdn_eta );
  BabyTree_->Branch("zll_pseudoJet2JECdn_phi", &zll_pseudoJet2JECdn_phi );
  BabyTree_->Branch("zll_pseudoJet2JECdn_mass", &zll_pseudoJet2JECdn_mass );
  BabyTree_->Branch("gamma_pseudoJet1_pt", &gamma_pseudoJet1_pt );
  BabyTree_->Branch("gamma_pseudoJet1_eta", &gamma_pseudoJet1_eta );
  BabyTree_->Branch("gamma_pseudoJet1_phi", &gamma_pseudoJet1_phi );
  BabyTree_->Branch("gamma_pseudoJet1_mass", &gamma_pseudoJet1_mass );
  BabyTree_->Branch("gamma_pseudoJet2_pt", &gamma_pseudoJet2_pt );
  BabyTree_->Branch("gamma_pseudoJet2_eta", &gamma_pseudoJet2_eta );
  BabyTree_->Branch("gamma_pseudoJet2_phi", &gamma_pseudoJet2_phi );
  BabyTree_->Branch("gamma_pseudoJet2_mass", &gamma_pseudoJet2_mass );
  BabyTree_->Branch("zllmt_pseudoJet1_pt", &zllmt_pseudoJet1_pt );
  BabyTree_->Branch("zllmt_pseudoJet1_eta", &zllmt_pseudoJet1_eta );
  BabyTree_->Branch("zllmt_pseudoJet1_phi", &zllmt_pseudoJet1_phi );
  BabyTree_->Branch("zllmt_pseudoJet1_mass", &zllmt_pseudoJet1_mass );
  BabyTree_->Branch("zllmt_pseudoJet2_pt", &zllmt_pseudoJet2_pt );
  BabyTree_->Branch("zllmt_pseudoJet2_eta", &zllmt_pseudoJet2_eta );
  BabyTree_->Branch("zllmt_pseudoJet2_phi", &zllmt_pseudoJet2_phi );
  BabyTree_->Branch("zllmt_pseudoJet2_mass", &zllmt_pseudoJet2_mass );
  BabyTree_->Branch("rl_pseudoJet1_pt", &rl_pseudoJet1_pt );
  BabyTree_->Branch("rl_pseudoJet1_eta", &rl_pseudoJet1_eta );
  BabyTree_->Branch("rl_pseudoJet1_phi", &rl_pseudoJet1_phi );
  BabyTree_->Branch("rl_pseudoJet1_mass", &rl_pseudoJet1_mass );
  BabyTree_->Branch("rl_pseudoJet2_pt", &rl_pseudoJet2_pt );
  BabyTree_->Branch("rl_pseudoJet2_eta", &rl_pseudoJet2_eta );
  BabyTree_->Branch("rl_pseudoJet2_phi", &rl_pseudoJet2_phi );
  BabyTree_->Branch("rl_pseudoJet2_mass", &rl_pseudoJet2_mass );
  BabyTree_->Branch("gen_pseudoJet1_pt", &gen_pseudoJet1_pt );
  BabyTree_->Branch("gen_pseudoJet1_eta", &gen_pseudoJet1_eta );
  BabyTree_->Branch("gen_pseudoJet1_phi", &gen_pseudoJet1_phi );
  BabyTree_->Branch("gen_pseudoJet1_mass", &gen_pseudoJet1_mass );
  BabyTree_->Branch("gen_pseudoJet2_pt", &gen_pseudoJet2_pt );
  BabyTree_->Branch("gen_pseudoJet2_eta", &gen_pseudoJet2_eta );
  BabyTree_->Branch("gen_pseudoJet2_phi", &gen_pseudoJet2_phi );
  BabyTree_->Branch("gen_pseudoJet2_mass", &gen_pseudoJet2_mass );

  BabyTree_->Branch("mht_pt", &mht_pt );
  BabyTree_->Branch("mht_phi", &mht_phi );
  BabyTree_->Branch("mht_ptJECup", &mht_ptJECup );
  BabyTree_->Branch("mht_phiJECup", &mht_phiJECup );
  BabyTree_->Branch("mht_ptJECdn", &mht_ptJECdn );
  BabyTree_->Branch("mht_phiJECdn", &mht_phiJECdn );
  BabyTree_->Branch("met_pt", &met_pt );
  BabyTree_->Branch("met_phi", &met_phi );
  BabyTree_->Branch("met_old2017_pt", &met_old2017_pt );
  BabyTree_->Branch("met_old2017_phi", &met_old2017_phi );
  BabyTree_->Branch("met_ptJECup", &met_ptJECup );
  BabyTree_->Branch("met_phiJECup", &met_phiJECup );
  BabyTree_->Branch("met_ptJECdn", &met_ptJECdn );
  BabyTree_->Branch("met_phiJECdn", &met_phiJECdn );
  BabyTree_->Branch("met_rawPt",  &met_rawPt );
  BabyTree_->Branch("met_rawPhi", &met_rawPhi );
  BabyTree_->Branch("met_caloPt",  &met_caloPt );
  BabyTree_->Branch("met_caloPhi", &met_caloPhi );
  BabyTree_->Branch("met_trkPt",  &met_trkPt );
  BabyTree_->Branch("met_trkPhi", &met_trkPhi );
  BabyTree_->Branch("met_genPt",  &met_genPt );
  BabyTree_->Branch("met_genPhi", &met_genPhi );
  BabyTree_->Branch("met_miniaodPt",  &met_miniaodPt );
  BabyTree_->Branch("met_miniaodPhi", &met_miniaodPhi );
  BabyTree_->Branch("Flag_EcalDeadCellTriggerPrimitiveFilter", &Flag_EcalDeadCellTriggerPrimitiveFilter );
  BabyTree_->Branch("Flag_trkPOG_manystripclus53X", &Flag_trkPOG_manystripclus53X );
  BabyTree_->Branch("Flag_ecalLaserCorrFilter", &Flag_ecalLaserCorrFilter );
  BabyTree_->Branch("Flag_trkPOG_toomanystripclus53X", &Flag_trkPOG_toomanystripclus53X );
  BabyTree_->Branch("Flag_hcalLaserEventFilter", &Flag_hcalLaserEventFilter );
  BabyTree_->Branch("Flag_trkPOG_logErrorTooManyClusters", &Flag_trkPOG_logErrorTooManyClusters );
  BabyTree_->Branch("Flag_trkPOGFilters", &Flag_trkPOGFilters );
  BabyTree_->Branch("Flag_trackingFailureFilter", &Flag_trackingFailureFilter );
  BabyTree_->Branch("Flag_CSCTightHalo2015Filter", &Flag_CSCTightHalo2015Filter );
  BabyTree_->Branch("Flag_CSCTightHaloFilter", &Flag_CSCTightHaloFilter );
  BabyTree_->Branch("Flag_globalTightHalo2016Filter", &Flag_globalTightHalo2016Filter );
  BabyTree_->Branch("Flag_globalSuperTightHalo2016Filter", &Flag_globalSuperTightHalo2016Filter );
  BabyTree_->Branch("Flag_HBHENoiseFilter", &Flag_HBHENoiseFilter );
  BabyTree_->Branch("Flag_HBHENoiseIsoFilter", &Flag_HBHENoiseIsoFilter );
  BabyTree_->Branch("Flag_goodVertices", &Flag_goodVertices );
  BabyTree_->Branch("Flag_eeBadScFilter", &Flag_eeBadScFilter );
  BabyTree_->Branch("Flag_ecalBadCalibFilter", &Flag_ecalBadCalibFilter );
  BabyTree_->Branch("Flag_ecalBadCalibFilterUpdate", &Flag_ecalBadCalibFilterUpdate );
  BabyTree_->Branch("Flag_badMuonFilter", &Flag_badMuonFilter );
  BabyTree_->Branch("Flag_badMuonFilterV2", &Flag_badMuonFilterV2 ); 
  BabyTree_->Branch("Flag_badMuons", &Flag_badMuons ); 
  BabyTree_->Branch("Flag_duplicateMuons", &Flag_duplicateMuons );   
  BabyTree_->Branch("Flag_noBadMuons", &Flag_noBadMuons );  
  BabyTree_->Branch("Flag_badChargedCandidateFilter", &Flag_badChargedCandidateFilter );
  BabyTree_->Branch("Flag_badChargedHadronFilterV2", &Flag_badChargedHadronFilterV2 );    
  BabyTree_->Branch("Flag_METFilters", &Flag_METFilters );
  BabyTree_->Branch("HLT_PFHT1050", &HLT_PFHT1050 );
  BabyTree_->Branch("HLT_PFHT900", &HLT_PFHT900 );
  BabyTree_->Branch("HLT_PFHT500_PFMET100_PFMHT100", &HLT_PFHT500_PFMET100_PFMHT100 ); 
  BabyTree_->Branch("HLT_PFHT700_PFMET85_PFMHT85", &HLT_PFHT700_PFMET85_PFMHT85 );
  BabyTree_->Branch("HLT_PFHT800_PFMET75_PFMHT75", &HLT_PFHT800_PFMET75_PFMHT75 );
  BabyTree_->Branch("HLT_PFMET170", &HLT_PFMET170 );
  BabyTree_->Branch("HLT_PFHT300_PFMET100", &HLT_PFHT300_PFMET100 );
  BabyTree_->Branch("HLT_PFHT300_PFMET110", &HLT_PFHT300_PFMET110 );
  BabyTree_->Branch("HLT_PFHT350_PFMET100", &HLT_PFHT350_PFMET100 );
  BabyTree_->Branch("HLT_PFHT350_PFMET120", &HLT_PFHT350_PFMET120 );
  BabyTree_->Branch("HLT_PFMETNoMu90_PFMHTNoMu90", &HLT_PFMETNoMu90_PFMHTNoMu90 );
  BabyTree_->Branch("HLT_MonoCentralPFJet80_PFMETNoMu90_PFMHTNoMu90", &HLT_MonoCentralPFJet80_PFMETNoMu90_PFMHTNoMu90 );
  BabyTree_->Branch("HLT_PFMETNoMu100_PFMHTNoMu100", &HLT_PFMETNoMu100_PFMHTNoMu100 );
  BabyTree_->Branch("HLT_PFMETNoMu110_PFMHTNoMu110", &HLT_PFMETNoMu110_PFMHTNoMu110 );
  BabyTree_->Branch("HLT_PFMETNoMu120_PFMHTNoMu120", &HLT_PFMETNoMu120_PFMHTNoMu120 );
  BabyTree_->Branch("HLT_PFMETNoMu130_PFMHTNoMu130", &HLT_PFMETNoMu130_PFMHTNoMu130 );
  BabyTree_->Branch("HLT_PFMETNoMu140_PFMHTNoMu140", &HLT_PFMETNoMu140_PFMHTNoMu140 );
  BabyTree_->Branch("HLT_PFMETNoMu120_PFMHTNoMu120_PFHT60", &HLT_PFMETNoMu120_PFMHTNoMu120_PFHT60 );
  BabyTree_->Branch("HLT_PFMET90_PFMHT90", &HLT_PFMET90_PFMHT90 );
  BabyTree_->Branch("HLT_PFMET100_PFMHT100", &HLT_PFMET100_PFMHT100 );
  BabyTree_->Branch("HLT_PFMET110_PFMHT110", &HLT_PFMET110_PFMHT110 );
  BabyTree_->Branch("HLT_PFMET120_PFMHT120", &HLT_PFMET120_PFMHT120 );
  BabyTree_->Branch("HLT_PFMET130_PFMHT130", &HLT_PFMET130_PFMHT130 );
  BabyTree_->Branch("HLT_PFMET140_PFMHT140", &HLT_PFMET140_PFMHT140 );
  BabyTree_->Branch("HLT_PFMET100_PFMHT100_PFHT60", &HLT_PFMET100_PFMHT100_PFHT60 );
  BabyTree_->Branch("HLT_PFMET120_PFMHT120_PFHT60", &HLT_PFMET120_PFMHT120_PFHT60 );
  BabyTree_->Branch("HLT_PFJet450", &HLT_PFJet450 );
  BabyTree_->Branch("HLT_PFJet500", &HLT_PFJet500 );
  BabyTree_->Branch("HLT_ECALHT800", &HLT_ECALHT800 );
  BabyTree_->Branch("HLT_SingleMu", &HLT_SingleMu );
  BabyTree_->Branch("HLT_SingleMu_NonIso", &HLT_SingleMu_NonIso );
  BabyTree_->Branch("HLT_SingleEl", &HLT_SingleEl );
  BabyTree_->Branch("HLT_SingleEl_NonIso", &HLT_SingleEl_NonIso );
  BabyTree_->Branch("HLT_DoubleEl", &HLT_DoubleEl );
  BabyTree_->Branch("HLT_DoubleEl33", &HLT_DoubleEl33 );
  BabyTree_->Branch("HLT_MuX_Ele12", &HLT_MuX_Ele12 );
  BabyTree_->Branch("HLT_Mu8_EleX", &HLT_Mu8_EleX );
  BabyTree_->Branch("HLT_Mu12_EleX", &HLT_Mu12_EleX );
  BabyTree_->Branch("HLT_Mu30_Ele30_NonIso", &HLT_Mu30_Ele30_NonIso );
  BabyTree_->Branch("HLT_Mu33_Ele33_NonIso", &HLT_Mu33_Ele33_NonIso );
  BabyTree_->Branch("HLT_Mu37_Ele27_NonIso", &HLT_Mu37_Ele27_NonIso );
  BabyTree_->Branch("HLT_Mu27_Ele37_NonIso", &HLT_Mu27_Ele37_NonIso );
  BabyTree_->Branch("HLT_DoubleMu", &HLT_DoubleMu );
  BabyTree_->Branch("HLT_DoubleMu_NonIso", &HLT_DoubleMu_NonIso );
  BabyTree_->Branch("HLT_Photon120", &HLT_Photon120 );
  BabyTree_->Branch("HLT_Photon200", &HLT_Photon200 );
  BabyTree_->Branch("HLT_Photon175_Prescale", &HLT_Photon175_Prescale );
  BabyTree_->Branch("HLT_Photon165_HE10", &HLT_Photon165_HE10 );
  BabyTree_->Branch("HLT_Photon250_NoHE", &HLT_Photon250_NoHE );
  BabyTree_->Branch("HLT_PFHT180_Prescale", &HLT_PFHT180_Prescale );
  BabyTree_->Branch("HLT_PFHT250_Prescale", &HLT_PFHT250_Prescale );
  BabyTree_->Branch("HLT_PFHT370_Prescale", &HLT_PFHT370_Prescale );
  BabyTree_->Branch("HLT_PFHT430_Prescale", &HLT_PFHT430_Prescale );
  BabyTree_->Branch("HLT_PFHT510_Prescale", &HLT_PFHT510_Prescale );
  BabyTree_->Branch("HLT_PFHT590_Prescale", &HLT_PFHT590_Prescale );
  BabyTree_->Branch("HLT_PFHT680_Prescale", &HLT_PFHT680_Prescale );
  BabyTree_->Branch("HLT_PFHT780_Prescale", &HLT_PFHT780_Prescale );
  BabyTree_->Branch("HLT_PFHT890_Prescale", &HLT_PFHT890_Prescale );
  BabyTree_->Branch("HLT_PFHT125_Prescale", &HLT_PFHT125_Prescale );
  BabyTree_->Branch("HLT_PFHT200_Prescale", &HLT_PFHT200_Prescale );
  BabyTree_->Branch("HLT_PFHT300_Prescale", &HLT_PFHT300_Prescale );
  BabyTree_->Branch("HLT_PFHT350_Prescale", &HLT_PFHT350_Prescale );
  BabyTree_->Branch("HLT_PFHT475_Prescale", &HLT_PFHT475_Prescale );
  BabyTree_->Branch("HLT_PFHT600_Prescale", &HLT_PFHT600_Prescale );
  BabyTree_->Branch("HLT_DiCentralPFJet70_PFMET120", &HLT_DiCentralPFJet70_PFMET120 );
  BabyTree_->Branch("HLT_DiCentralPFJet55_PFMET110", &HLT_DiCentralPFJet55_PFMET110 );
  BabyTree_->Branch("nlep", &nlep, "nlep/I" );
  BabyTree_->Branch("lep_pt", lep_pt, "lep_pt[nlep]/F");
  BabyTree_->Branch("lep_eta", lep_eta, "lep_eta[nlep]/F" );
  BabyTree_->Branch("lep_phi", lep_phi, "lep_phi[nlep]/F" );
  BabyTree_->Branch("lep_mass", lep_mass, "lep_mass[nlep]/F" );
  BabyTree_->Branch("lep_charge", lep_charge, "lep_charge[nlep]/I" );
  BabyTree_->Branch("lep_pdgId", lep_pdgId, "lep_pdgId[nlep]/I" );
  BabyTree_->Branch("lep_dxy", lep_dxy, "lep_dxy[nlep]/F" );
  BabyTree_->Branch("lep_dz", lep_dz, "lep_dz[nlep]/F" );
  BabyTree_->Branch("lep_tightId", lep_tightId, "lep_tightId[nlep]/I" );
  BabyTree_->Branch("lep_heepId", lep_heepId, "lep_heepId[nlep]/I" );
  BabyTree_->Branch("lep_highPtFit_pt", lep_highPtFit_pt, "lep_highPtFit_pt[nlep]/F");
  BabyTree_->Branch("lep_highPtFit_eta", lep_highPtFit_eta, "lep_highPtFit_eta[nlep]/F" );
  BabyTree_->Branch("lep_isPF", lep_isPF, "lep_isPF[nlep]/I" );
  BabyTree_->Branch("HLT_PFHT125_Prescale", &HLT_PFHT125_Prescale );
  BabyTree_->Branch("HLT_PFHT200_Prescale", &HLT_PFHT200_Prescale );
  BabyTree_->Branch("HLT_PFHT300_Prescale", &HLT_PFHT300_Prescale );
  BabyTree_->Branch("HLT_PFHT350_Prescale", &HLT_PFHT350_Prescale );
  BabyTree_->Branch("HLT_PFHT475_Prescale", &HLT_PFHT475_Prescale );
  BabyTree_->Branch("HLT_PFHT600_Prescale", &HLT_PFHT600_Prescale );
  BabyTree_->Branch("HLT_PFHT125_Prescale", &HLT_PFHT125_Prescale );
  BabyTree_->Branch("HLT_PFHT200_Prescale", &HLT_PFHT200_Prescale );
  BabyTree_->Branch("HLT_PFHT300_Prescale", &HLT_PFHT300_Prescale );
  BabyTree_->Branch("HLT_PFHT350_Prescale", &HLT_PFHT350_Prescale );
  BabyTree_->Branch("HLT_PFHT475_Prescale", &HLT_PFHT475_Prescale );
  BabyTree_->Branch("HLT_PFHT600_Prescale", &HLT_PFHT600_Prescale );
  BabyTree_->Branch("lep_highPtFit_phi", lep_highPtFit_phi, "lep_highPtFit_phi[nlep]/F" );
  BabyTree_->Branch("lep_relIso03", lep_relIso03, "lep_relIso03[nlep]/F" );
  BabyTree_->Branch("lep_relIso04", lep_relIso04, "lep_relIso04[nlep]/F" );
  BabyTree_->Branch("lep_miniRelIso", lep_miniRelIso, "lep_miniRelIso[nlep]/F" );
  // BabyTree_->Branch("lep_relIsoAn04", lep_relIsoAn04, "lep_relIsoAn04[nlep]/F" );
  BabyTree_->Branch("lep_mcMatchId", lep_mcMatchId, "lep_mcMatchId[nlep]/I" );
  BabyTree_->Branch("lep_lostHits", lep_lostHits, "lep_lostHits[nlep]/I" );
  BabyTree_->Branch("lep_convVeto", lep_convVeto, "lep_convVeto[nlep]/I" );
  BabyTree_->Branch("lep_tightCharge", lep_tightCharge, "lep_tightCharge[nlep]/I" );
  BabyTree_->Branch("nisoTrack", &nisoTrack, "nisoTrack/I" );
  BabyTree_->Branch("isoTrack_pt", isoTrack_pt, "isoTrack_pt[nisoTrack]/F" );
  BabyTree_->Branch("isoTrack_eta", isoTrack_eta, "isoTrack_eta[nisoTrack]/F" );
  BabyTree_->Branch("isoTrack_phi", isoTrack_phi, "isoTrack_phi[nisoTrack]/F" );
  BabyTree_->Branch("isoTrack_mass", isoTrack_mass, "isoTrack_mass[nisoTrack]/F" );
  BabyTree_->Branch("isoTrack_absIso", isoTrack_absIso, "isoTrack_absIso[nisoTrack]/F" );
  // BabyTree_->Branch("isoTrack_relIsoAn04", isoTrack_relIsoAn04, "isoTrack_relIsoAn04[nisoTrack]/F" );
  BabyTree_->Branch("isoTrack_dz", isoTrack_dz, "isoTrack_dz[nisoTrack]/F" );
  BabyTree_->Branch("isoTrack_pdgId", isoTrack_pdgId, "isoTrack_pdgId[nisoTrack]/I" );
  BabyTree_->Branch("isoTrack_mcMatchId", isoTrack_mcMatchId, "isoTrack_mcMatchId[nisoTrack]/I" );
  BabyTree_->Branch("nhighPtPFcands", &nhighPtPFcands, "nhighPtPFcands/I" );
  BabyTree_->Branch("highPtPFcands_pt", highPtPFcands_pt, "highPtPFcands_pt[nhighPtPFcands]/F" );
  BabyTree_->Branch("highPtPFcands_eta", highPtPFcands_eta, "highPtPFcands_eta[nhighPtPFcands]/F" );
  BabyTree_->Branch("highPtPFcands_phi", highPtPFcands_phi, "highPtPFcands_phi[nhighPtPFcands]/F" );
  BabyTree_->Branch("highPtPFcands_mass", highPtPFcands_mass, "highPtPFcands_mass[nhighPtPFcands]/F" );
  BabyTree_->Branch("highPtPFcands_absIso", highPtPFcands_absIso, "highPtPFcands_absIso[nhighPtPFcands]/F" );
  // BabyTree_->Branch("highPtPFcands_relIsoAn04", highPtPFcands_relIsoAn04, "highPtPFcands_relIsoAn04[nhighPtPFcands]/F" );
  BabyTree_->Branch("highPtPFcands_dz", highPtPFcands_dz, "highPtPFcands_dz[nhighPtPFcands]/F" );
  BabyTree_->Branch("highPtPFcands_pdgId", highPtPFcands_pdgId, "highPtPFcands_pdgId[nhighPtPFcands]/I" );
  BabyTree_->Branch("highPtPFcands_mcMatchId", highPtPFcands_mcMatchId, "highPtPFcands_mcMatchId[nhighPtPFcands]/I" );
  BabyTree_->Branch("nPFLep5LowMT", &nPFLep5LowMT, "nPFLep5LowMT/I" );
  BabyTree_->Branch("nPFHad10LowMT", &nPFHad10LowMT, "nPFHad10LowMT/I" );
  BabyTree_->Branch("ntau", &ntau, "ntau/I" );
  BabyTree_->Branch("tau_pt", tau_pt, "tau_pt[ntau]/F" );
  BabyTree_->Branch("tau_eta", tau_eta, "tau_eta[ntau]/F" );
  BabyTree_->Branch("tau_phi", tau_phi, "tau_phi[ntau]/F" );
  BabyTree_->Branch("tau_mass", tau_mass, "tau_mass[ntau]/F" );
  BabyTree_->Branch("tau_charge", tau_charge, "tau_charge[ntau]/I" );
  BabyTree_->Branch("tau_dxy", tau_dxy, "tau_dxy[ntau]/F" );
  BabyTree_->Branch("tau_dz", tau_dz, "tau_dz[ntau]/F" );
  BabyTree_->Branch("tau_idCI3hit", tau_idCI3hit, "tau_idCI3hit[ntau]/I" );
  BabyTree_->Branch("tau_isoCI3hit", tau_isoCI3hit, "tau_isoCI3hit[ntau]/F" );
  BabyTree_->Branch("tau_mcMatchId", tau_mcMatchId, "tau_mcMatchId[ntau]/I" );
  BabyTree_->Branch("ngamma", &ngamma, "ngamma/I" );
  BabyTree_->Branch("gamma_pt", gamma_pt, "gamma_pt[ngamma]/F" );
  BabyTree_->Branch("gamma_eta", gamma_eta, "gamma_eta[ngamma]/F" );
  BabyTree_->Branch("gamma_phi", gamma_phi, "gamma_phi[ngamma]/F" );
  BabyTree_->Branch("gamma_mass", gamma_mass, "gamma_mass[ngamma]/F" );
  BabyTree_->Branch("gamma_mcMatchId", gamma_mcMatchId, "gamma_mcMatchId[ngamma]/I" );
  BabyTree_->Branch("gamma_genIso04", gamma_genIso04, "gamma_genIso04[ngamma]/F" );
  BabyTree_->Branch("gamma_drMinParton", gamma_drMinParton, "gamma_drMinParton[ngamma]/F" );
  BabyTree_->Branch("gamma_chHadIso", gamma_chHadIso, "gamma_chHadIso[ngamma]/F" );
  BabyTree_->Branch("gamma_neuHadIso", gamma_neuHadIso, "gamma_neuHadIso[ngamma]/F" );
  BabyTree_->Branch("gamma_phIso", gamma_phIso, "gamma_phIso[ngamma]/F" );
  BabyTree_->Branch("gamma_sigmaIetaIeta", gamma_sigmaIetaIeta, "gamma_sigmaIetaIeta[ngamma]/F" );
  BabyTree_->Branch("gamma_r9", gamma_r9, "gamma_r9[ngamma]/F" );
  BabyTree_->Branch("gamma_hOverE", gamma_hOverE, "gamma_hOverE[ngamma]/F" );
  BabyTree_->Branch("gamma_hOverE015", gamma_hOverE015, "gamma_hOverE015[ngamma]/F" );
  BabyTree_->Branch("gamma_idCutBased", gamma_idCutBased, "gamma_idCutBased[ngamma]/I" );
  BabyTree_->Branch("gamma_mt2", &gamma_mt2 );
  BabyTree_->Branch("gamma_nJet30", &gamma_nJet30 );
  BabyTree_->Branch("gamma_nJet40", &gamma_nJet40 );
  BabyTree_->Branch("gamma_nJet30FailId", &gamma_nJet30FailId );
  BabyTree_->Branch("gamma_nJet100FailId", &gamma_nJet100FailId );
  BabyTree_->Branch("gamma_nBJet20", &gamma_nBJet20 );
  BabyTree_->Branch("gamma_nBJet20csv", &gamma_nBJet20csv );
  BabyTree_->Branch("gamma_nBJet25", &gamma_nBJet25 );
  BabyTree_->Branch("gamma_nBJet30", &gamma_nBJet30 );
  BabyTree_->Branch("gamma_nBJet40", &gamma_nBJet40 );
  BabyTree_->Branch("gamma_ht", &gamma_ht );
  BabyTree_->Branch("gamma_deltaPhiMin", &gamma_deltaPhiMin );
  BabyTree_->Branch("gamma_diffMetMht", &gamma_diffMetMht );
  BabyTree_->Branch("gamma_mht_pt", &gamma_mht_pt );
  BabyTree_->Branch("gamma_mht_phi", &gamma_mht_phi );
  BabyTree_->Branch("gamma_met_pt", &gamma_met_pt );
  BabyTree_->Branch("gamma_met_phi", &gamma_met_phi );
  BabyTree_->Branch("npfPhoton", &npfPhoton, "npfPhoton/I" );
  BabyTree_->Branch("pfPhoton_pt", pfPhoton_pt, "pfPhoton_pt[npfPhoton]/F" );
  BabyTree_->Branch("pfPhoton_eta", pfPhoton_eta, "pfPhoton_eta[npfPhoton]/F" );
  BabyTree_->Branch("pfPhoton_phi", pfPhoton_phi, "pfPhoton_phi[npfPhoton]/F" );
  BabyTree_->Branch("zll_mt2", &zll_mt2 );
  BabyTree_->Branch("zll_deltaPhiMin", &zll_deltaPhiMin );
  BabyTree_->Branch("zll_diffMetMht", &zll_diffMetMht );
  BabyTree_->Branch("zll_met_pt", &zll_met_pt );
  BabyTree_->Branch("zll_met_phi", &zll_met_phi );
  BabyTree_->Branch("zll_mht_pt", &zll_mht_pt );
  BabyTree_->Branch("zll_mht_phi", &zll_mht_phi );
  BabyTree_->Branch("zll_mass", &zll_mass );
  BabyTree_->Branch("zll_pt", &zll_pt );
  BabyTree_->Branch("zll_eta", &zll_eta );
  BabyTree_->Branch("zll_phi", &zll_phi );
  BabyTree_->Branch("zll_ht", &zll_ht );
  BabyTree_->Branch("zll_mt2JECup", &zll_mt2JECup );
  BabyTree_->Branch("zll_deltaPhiMinJECup", &zll_deltaPhiMinJECup );
  BabyTree_->Branch("zll_diffMetMhtJECup", &zll_diffMetMhtJECup );
  BabyTree_->Branch("zll_met_ptJECup", &zll_met_ptJECup );
  BabyTree_->Branch("zll_met_phiJECup", &zll_met_phiJECup );
  BabyTree_->Branch("zll_mht_ptJECup", &zll_mht_ptJECup );
  BabyTree_->Branch("zll_mht_phiJECup", &zll_mht_phiJECup );
  BabyTree_->Branch("zll_htJECup", &zll_htJECup );
  BabyTree_->Branch("zll_mt2JECdn", &zll_mt2JECdn );
  BabyTree_->Branch("zll_deltaPhiMinJECdn", &zll_deltaPhiMinJECdn );
  BabyTree_->Branch("zll_diffMetMhtJECdn", &zll_diffMetMhtJECdn );
  BabyTree_->Branch("zll_met_ptJECdn", &zll_met_ptJECdn );
  BabyTree_->Branch("zll_met_phiJECdn", &zll_met_phiJECdn );
  BabyTree_->Branch("zll_mht_ptJECdn", &zll_mht_ptJECdn );
  BabyTree_->Branch("zll_mht_phiJECdn", &zll_mht_phiJECdn );
  BabyTree_->Branch("zll_htJECdn", &zll_htJECdn );
  BabyTree_->Branch("zllmt_mt2", &zllmt_mt2 );
  BabyTree_->Branch("zllmt_deltaPhiMin", &zllmt_deltaPhiMin );
  BabyTree_->Branch("zllmt_diffMetMht", &zllmt_diffMetMht );
  BabyTree_->Branch("zllmt_met_pt", &zllmt_met_pt );
  BabyTree_->Branch("zllmt_met_phi", &zllmt_met_phi );
  BabyTree_->Branch("zllmt_mht_pt", &zllmt_mht_pt );
  BabyTree_->Branch("zllmt_mht_phi", &zllmt_mht_phi );
  BabyTree_->Branch("zllmt_ht", &zllmt_ht );
  BabyTree_->Branch("zllmt_mt", &zllmt_mt );
  BabyTree_->Branch("rl_mt2", &rl_mt2 );
  BabyTree_->Branch("rl_deltaPhiMin", &rl_deltaPhiMin );
  BabyTree_->Branch("rl_diffMetMht", &rl_diffMetMht );
  BabyTree_->Branch("rl_met_pt", &rl_met_pt );
  BabyTree_->Branch("rl_met_phi", &rl_met_phi );
  BabyTree_->Branch("rl_mht_pt", &rl_mht_pt );
  BabyTree_->Branch("rl_mht_phi", &rl_mht_phi );
  BabyTree_->Branch("rl_mass", &rl_mass );
  BabyTree_->Branch("rl_pt", &rl_pt );
  BabyTree_->Branch("rl_eta", &rl_eta );
  BabyTree_->Branch("rl_phi", &rl_phi );
  BabyTree_->Branch("rl_ht", &rl_ht );
  if (!isDataFromFileName) {
    if (saveGenParticles) {
      BabyTree_->Branch("ngenPart", &ngenPart, "ngenPart/I" );
      BabyTree_->Branch("genPart_pt", genPart_pt, "genPart_pt[ngenPart]/F" );
      BabyTree_->Branch("genPart_eta", genPart_eta, "genPart_eta[ngenPart]/F" );
      BabyTree_->Branch("genPart_phi", genPart_phi, "genPart_phi[ngenPart]/F" );
      BabyTree_->Branch("genPart_mass", genPart_mass, "genPart_mass[ngenPart]/F" );
      BabyTree_->Branch("genPart_pdgId", genPart_pdgId, "genPart_pdgId[ngenPart]/I" );
      BabyTree_->Branch("genPart_status", genPart_status, "genPart_status[ngenPart]/I" );
      BabyTree_->Branch("genPart_charge", genPart_charge, "genPart_charge[ngenPart]/F" );
      BabyTree_->Branch("genPart_motherId", genPart_motherId, "genPart_motherId[ngenPart]/I" );
      BabyTree_->Branch("genPart_grandmotherId", genPart_grandmotherId, "genPart_grandmotherId[ngenPart]/I" );
    }
    BabyTree_->Branch("ngenLep", &ngenLep, "ngenLep/I" );
    BabyTree_->Branch("genLep_pt", genLep_pt, "genLep_pt[ngenLep]/F" );
    BabyTree_->Branch("genLep_eta", genLep_eta, "genLep_eta[ngenLep]/F" );
    BabyTree_->Branch("genLep_phi", genLep_phi, "genLep_phi[ngenLep]/F" );
    BabyTree_->Branch("genLep_mass", genLep_mass, "genLep_mass[ngenLep]/F" );
    BabyTree_->Branch("genLep_pdgId", genLep_pdgId, "genLep_pdgId[ngenLep]/I" );
    BabyTree_->Branch("genLep_status", genLep_status, "genLep_status[ngenLep]/I" );
    BabyTree_->Branch("genLep_charge", genLep_charge, "genLep_charge[ngenLep]/F" );
    BabyTree_->Branch("genLep_sourceId", genLep_sourceId, "genLep_sourceId[ngenLep]/I" );
    BabyTree_->Branch("ngenStat23", &ngenStat23, "ngenStat23/I" );
    BabyTree_->Branch("genStat23_pt", genStat23_pt, "genStat23_pt[ngenStat23]/F" );
    BabyTree_->Branch("genStat23_eta", genStat23_eta, "genStat23_eta[ngenStat23]/F" );
    BabyTree_->Branch("genStat23_phi", genStat23_phi, "genStat23_phi[ngenStat23]/F" );
    BabyTree_->Branch("genStat23_mass", genStat23_mass, "genStat23_mass[ngenStat23]/F" );
    BabyTree_->Branch("genStat23_pdgId", genStat23_pdgId, "genStat23_pdgId[ngenStat23]/I" );
    BabyTree_->Branch("genStat23_status", genStat23_status, "genStat23_status[ngenStat23]/I" );
    BabyTree_->Branch("genStat23_charge", genStat23_charge, "genStat23_charge[ngenStat23]/F" );
    BabyTree_->Branch("genStat23_sourceId", genStat23_sourceId, "genStat23_sourceId[ngenStat23]/I" );
    BabyTree_->Branch("ngenGamma", &ngenGamma, "ngenGamma/I" );
    BabyTree_->Branch("genGamma_pt", genGamma_pt, "genGamma_pt[ngenGamma]/F" );
    BabyTree_->Branch("genGamma_eta", genGamma_eta, "genGamma_eta[ngenGamma]/F" );
    BabyTree_->Branch("genGamma_phi", genGamma_phi, "genGamma_phi[ngenGamma]/F" );
    BabyTree_->Branch("genGamma_motherId", genGamma_motherId, "genGamma_motherId[ngenGamma]/I" );
    BabyTree_->Branch("ngenTau", &ngenTau, "ngenTau/I" );
    BabyTree_->Branch("ngenTau1Prong", &ngenTau1Prong, "ngenTau1Prong/I" );
    BabyTree_->Branch("ngenTau3Prong", &ngenTau3Prong, "ngenTau3Prong/I" );
    BabyTree_->Branch("genTau_pt", genTau_pt, "genTau_pt[ngenTau]/F" );
    BabyTree_->Branch("genTau_eta", genTau_eta, "genTau_eta[ngenTau]/F" );
    BabyTree_->Branch("genTau_phi", genTau_phi, "genTau_phi[ngenTau]/F" );
    BabyTree_->Branch("genTau_mass", genTau_mass, "genTau_mass[ngenTau]/F" );
    BabyTree_->Branch("genTau_pdgId", genTau_pdgId, "genTau_pdgId[ngenTau]/I" );
    BabyTree_->Branch("genTau_status", genTau_status, "genTau_status[ngenTau]/I" );
    BabyTree_->Branch("genTau_charge", genTau_charge, "genTau_charge[ngenTau]/F" );
    BabyTree_->Branch("genTau_sourceId", genTau_sourceId, "genTau_sourceId[ngenTau]/I" );
    BabyTree_->Branch("genTau_decayMode", genTau_decayMode, "genTau_decayMode[ngenTau]/I" );
    BabyTree_->Branch("genTau_leadTrackPt", genTau_leadTrackPt, "genTau_leadTrackPt[ngenTau]/F" );
    BabyTree_->Branch("genTau_neutralDaughters", genTau_neutralDaughters, "genTau_neutralDaughters[ngenTau]/I" );
    BabyTree_->Branch("ngenLepFromTau", &ngenLepFromTau, "ngenLepFromTau/I" );
    BabyTree_->Branch("genLepFromTau_pt", genLepFromTau_pt, "genLepFromTau_pt[ngenLepFromTau]/F" );
    BabyTree_->Branch("genLepFromTau_eta", genLepFromTau_eta, "genLepFromTau_eta[ngenLepFromTau]/F" );
    BabyTree_->Branch("genLepFromTau_phi", genLepFromTau_phi, "genLepFromTau_phi[ngenLepFromTau]/F" );
    BabyTree_->Branch("genLepFromTau_mass", genLepFromTau_mass, "genLepFromTau_mass[ngenLepFromTau]/F" );
    BabyTree_->Branch("genLepFromTau_pdgId", genLepFromTau_pdgId, "genLepFromTau_pdgId[ngenLepFromTau]/I" );
    BabyTree_->Branch("genLepFromTau_status", genLepFromTau_status, "genLepFromTau_status[ngenLepFromTau]/I" );
    BabyTree_->Branch("genLepFromTau_charge", genLepFromTau_charge, "genLepFromTau_charge[ngenLepFromTau]/F" );
    BabyTree_->Branch("genLepFromTau_sourceId", genLepFromTau_sourceId, "genLepFromTau_sourceId[ngenLepFromTau]/I" );
    BabyTree_->Branch("ngenLepFromZ", &ngenLepFromZ, "ngenLepFromZ/I" );
    BabyTree_->Branch("ngenNuFromZ", &ngenNuFromZ, "ngenNuFromZ/I" );
    BabyTree_->Branch("GenSusyMScan1", &GenSusyMScan1 );
    BabyTree_->Branch("GenSusyMScan2", &GenSusyMScan2 );
    BabyTree_->Branch("GenSusyMScan3", &GenSusyMScan3 );
    BabyTree_->Branch("GenSusyMScan4", &GenSusyMScan4 );
    if (saveLHEweights || saveLHEweightsScaleOnly) {
      BabyTree_->Branch("nLHEweight", &nLHEweight, "nLHEweight/I" );
      BabyTree_->Branch("LHEweight_wgt", LHEweight_wgt, "LHEweight_wgt[nLHEweight]/F" );
    }
  }
  BabyTree_->Branch("njet", &njet, "njet/I" );
  BabyTree_->Branch("jet_pt", jet_pt, "jet_pt[njet]/F" );
  BabyTree_->Branch("jet_eta", jet_eta, "jet_eta[njet]/F" );
  BabyTree_->Branch("jet_phi", jet_phi, "jet_phi[njet]/F" );
  BabyTree_->Branch("jet_mass", jet_mass, "jet_mass[njet]/F" );
  BabyTree_->Branch("jet_btagCSV", jet_btagCSV, "jet_btagCSV[njet]/F" );
  BabyTree_->Branch("jet_btagDeepCSV", jet_btagDeepCSV, "jet_btagDeepCSV[njet]/F" );
  BabyTree_->Branch("jet_chf", jet_chf, "jet_chf[njet]/F" );
  BabyTree_->Branch("jet_nhf", jet_nhf, "jet_nhf[njet]/F" );
  BabyTree_->Branch("jet_cemf", jet_cemf, "jet_cemf[njet]/F" );
  BabyTree_->Branch("jet_nemf", jet_nemf, "jet_nemf[njet]/F" );
  BabyTree_->Branch("jet_muf", jet_muf, "jet_muf[njet]/F" );
  BabyTree_->Branch("jet_rawPt", jet_rawPt, "jet_rawPt[njet]/F" );
  BabyTree_->Branch("jet_mcPt", jet_mcPt, "jet_mcPt[njet]/F" );
  BabyTree_->Branch("jet_mcFlavour", jet_mcFlavour, "jet_mcFlavour[njet]/I" );
  BabyTree_->Branch("jet_hadronFlavour", jet_hadronFlavour, "jet_hadronFlavour[njet]/I" );
  BabyTree_->Branch("jet_qgl", jet_qgl, "jet_qgl[njet]/F" );
  BabyTree_->Branch("jet_area", jet_area, "jet_area[njet]/F" );
  BabyTree_->Branch("jet_id", jet_id, "jet_id[njet]/I" );
  BabyTree_->Branch("jet_puId", jet_puId, "jet_puId[njet]/I" );
  BabyTree_->Branch("good_jet_idxs", good_jet_idxs, "good_jet_idxs[nJet30]/I" );
  BabyTree_->Branch("good_bjet_idxs", good_bjet_idxs, "good_bjet_idxs[nBJet20]/I" );
  BabyTree_->Branch("jet_closest_met_idx", &jet_closest_met_idx, "jet_closest_met_idx/I" );
  BabyTree_->Branch("jet_closest_met_dphi", &jet_closest_met_dphi, "jet_closest_met_dphi/F" );
  BabyTree_->Branch("weight_lepsf", &weight_lepsf );
  BabyTree_->Branch("weight_lepsf_UP", &weight_lepsf_UP );
  BabyTree_->Branch("weight_lepsf_DN", &weight_lepsf_DN );
  BabyTree_->Branch("weight_lepsf_0l", &weight_lepsf_0l );
  BabyTree_->Branch("weight_lepsf_0l_UP", &weight_lepsf_0l_UP );
  BabyTree_->Branch("weight_lepsf_0l_DN", &weight_lepsf_0l_DN );
  BabyTree_->Branch("weight_btagsf", &weight_btagsf );
  BabyTree_->Branch("weight_btagsf_heavy_UP", &weight_btagsf_heavy_UP );
  BabyTree_->Branch("weight_btagsf_light_UP", &weight_btagsf_light_UP );
  BabyTree_->Branch("weight_btagsf_heavy_DN", &weight_btagsf_heavy_DN );
  BabyTree_->Branch("weight_btagsf_light_DN", &weight_btagsf_light_DN );
  BabyTree_->Branch("weight_sigtrigsf", &weight_sigtrigsf );
  BabyTree_->Branch("weight_dileptrigsf", &weight_dileptrigsf );
  BabyTree_->Branch("weight_phottrigsf", &weight_phottrigsf );
  BabyTree_->Branch("weight_pu", &weight_pu );
  BabyTree_->Branch("weight_isr", &weight_isr );
  BabyTree_->Branch("weight_isr_UP", &weight_isr_UP );
  BabyTree_->Branch("weight_isr_DN", &weight_isr_DN );
  BabyTree_->Branch("weight_scales_UP", &weight_scales_UP );
  BabyTree_->Branch("weight_scales_DN", &weight_scales_DN );
  BabyTree_->Branch("weight_pdfs_UP", &weight_pdfs_UP );
  BabyTree_->Branch("weight_pdfs_DN", &weight_pdfs_DN );
  BabyTree_->Branch("weight_toppt", &weight_toppt );
  BabyTree_->Branch("genRecoil_pt", &genRecoil_pt );
  BabyTree_->Branch("genTop_pt", &genTop_pt );
  BabyTree_->Branch("genTbar_pt", &genTbar_pt );
  BabyTree_->Branch("genProd_pdgId", &genProd_pdgId );
  BabyTree_->Branch("weight_pol_L", &weight_pol_L );
  BabyTree_->Branch("weight_pol_R", &weight_pol_R );
  BabyTree_->Branch("nisrMatch", &nisrMatch );
    
  if(doRebal){
    BabyTree_->Branch("rebal_status", &rebal_status);
    BabyTree_->Branch("nRebalJets", &nRebalJets);
    BabyTree_->Branch("rebal_useJet", rebal_useJet, "rebal_useJet[njet]/I");
    BabyTree_->Branch("rebal_jetpt", rebal_jetpt, "rebal_jetpt[nRebalJets]/F");
    BabyTree_->Branch("rebal_jeteta", rebal_jeteta, "rebal_jeteta[nRebalJets]/F");
    BabyTree_->Branch("rebal_jetphi", rebal_jetphi, "rebal_jetphi[nRebalJets]/F");
    BabyTree_->Branch("rebal_jetbtagdeepcsv", rebal_jetbtagdeepcsv, "rebal_jetbtagdeepcsv[nRebalJets]/F");
    BabyTree_->Branch("rebal_factors", rebal_factors, "rebal_factors[nRebalJets]/F");
    BabyTree_->Branch("rebal_met_pt", &rebal_met_pt);
    BabyTree_->Branch("rebal_met_phi", &rebal_met_phi);
  }

  // Short Track

  BabyTree_->Branch("ntracks", &ntracks);
  BabyTree_->Branch("nshorttracks", &nshorttracks);
  BabyTree_->Branch("nshorttracks_P", &nshorttracks_P);
  BabyTree_->Branch("nshorttracks_M", &nshorttracks_M);
  BabyTree_->Branch("nshorttracks_L", &nshorttracks_L);
  BabyTree_->Branch("nshorttrackcandidates", &nshorttrackcandidates);
  BabyTree_->Branch("nshorttrackcandidates_P", &nshorttrackcandidates_P);
  BabyTree_->Branch("nshorttrackcandidates_M", &nshorttrackcandidates_M);
  BabyTree_->Branch("nshorttrackcandidates_L", &nshorttrackcandidates_L);
  BabyTree_->Branch("track_pt", track_pt, "track_pt[ntracks]/F");
  BabyTree_->Branch("track_ptErr", track_ptErr, "track_ptErr[ntracks]/F");
  BabyTree_->Branch("track_eta", track_eta, "track_eta[ntracks]/F");
  BabyTree_->Branch("track_phi", track_phi, "track_phi[ntracks]/F");
  BabyTree_->Branch("track_charge", track_charge, "track_charge[ntracks]/I");
  BabyTree_->Branch("track_dedxStrip", track_dedxStrip, "track_dedxStrip[ntracks]/F");
  BabyTree_->Branch("track_dedxPixel", track_dedxPixel, "track_dedxPixel[ntracks]/F");
  BabyTree_->Branch("track_nChi2", track_nChi2, "track_nChi2[ntracks]/F");
  BabyTree_->Branch("track_ipSigXY", track_ipSigXY, "track_ipSigXY[ntracks]/F");
  BabyTree_->Branch("track_dxy", track_dxy, "track_dxy[ntracks]/F");
  BabyTree_->Branch("track_dxyErr", track_dxyErr, "track_dxyErr[ntracks]/F");
  BabyTree_->Branch("track_dz", track_dz, "track_dz[ntracks]/F");
  BabyTree_->Branch("track_dzErr", track_dzErr, "track_dzErr[ntracks]/F");
  BabyTree_->Branch("track_isHighPurity", track_isHighPurity, "track_isHighPurity[ntracks]/I");
  BabyTree_->Branch("track_recoveto", track_recoveto, "track_recoveto[ntracks]/I");
  BabyTree_->Branch("track_DeadECAL", track_DeadECAL, "track_DeadECAL[ntracks]/I");
  BabyTree_->Branch("track_DeadHCAL", track_DeadHCAL, "track_DeadHCAL[ntracks]/I");
  BabyTree_->Branch("track_isshort", track_isshort, "track_isshort[ntracks]/I");
  BabyTree_->Branch("track_iscandidate", track_iscandidate, "track_iscandidate[ntracks]/I");
  BabyTree_->Branch("track_ispixelonly", track_ispixelonly, "track_ispixelonly[ntracks]/I");
  BabyTree_->Branch("track_ismedium", track_ismedium, "track_ismedium[ntracks]/I");
  BabyTree_->Branch("track_islong", track_islong, "track_islong[ntracks]/I");
  BabyTree_->Branch("track_ispixelonlycandidate", track_ispixelonlycandidate, "track_ispixelonlycandidate[ntracks]/I");
  BabyTree_->Branch("track_ismediumcandidate", track_ismediumcandidate, "track_ismediumcandidate[ntracks]/I");
  BabyTree_->Branch("track_islongcandidate", track_islongcandidate, "track_islongcandidate[ntracks]/I");
  BabyTree_->Branch("track_HitSignature", track_HitSignature, "track_HitSignature[ntracks]/I");
  BabyTree_->Branch("track_nPixelHits", track_nPixelHits, "track_nPixelHits[ntracks]/I");
  BabyTree_->Branch("track_nLostOuterHits", track_nLostOuterHits, "track_nLostOuterHits[ntracks]/I");
  BabyTree_->Branch("track_nLostInnerPixelHits", track_nLostInnerPixelHits, "track_nLostInnerPixelHits[ntracks]/I");
  BabyTree_->Branch("track_nLayersWithMeasurement", track_nLayersWithMeasurement, "track_nLayersWithMeasurement[ntracks]/I");
  BabyTree_->Branch("track_nPixelLayersWithMeasurement", track_nPixelLayersWithMeasurement, "track_nPixelLayersWithMeasurement[ntracks]/I");
  BabyTree_->Branch("track_nearestPF_id", track_nearestPF_id, "track_nearestPF_id[ntracks]/I");
  BabyTree_->Branch("track_nearestPF_DR", track_nearestPF_DR, "track_nearestPF_DR[ntracks]/F");
  BabyTree_->Branch("track_nearestPF_pt", track_nearestPF_pt, "track_nearestPF_pt[ntracks]/F");
  BabyTree_->Branch("track_jetDR", track_jetDR, "track_jetDR[ntracks]/F");
  BabyTree_->Branch("track_jetPt", track_jetPt, "track_jetPt[ntracks]/F");
  BabyTree_->Branch("track_jetEta", track_jetEta, "track_jetEta[ntracks]/F");
  BabyTree_->Branch("track_jetPhi", track_jetPhi, "track_jetPhi[ntracks]/F");
  BabyTree_->Branch("track_chHIso0p3", track_chHIso0p3, "track_chHIso0p3[ntracks]/F");
  BabyTree_->Branch("track_chHminiIso", track_chHminiIso, "track_chHminiIso[ntracks]/F");
  BabyTree_->Branch("track_neuHIso0p3", track_neuHIso0p3, "track_neuHIso0p3[ntracks]/F");
  BabyTree_->Branch("track_neuHminiIso", track_neuHminiIso, "track_neuHminiIso[ntracks]/F");
  BabyTree_->Branch("track_phIso0p3", track_phIso0p3, "track_phIso0p3[ntracks]/F");
  BabyTree_->Branch("track_phminiIso0p3", track_phminiIso, "track_phminiIso[ntracks]/F");
  BabyTree_->Branch("track_isLepOverlap", track_isLepOverlap, "track_isLepOverlap[ntracks]/I");
  BabyTree_->Branch("track_neuIso0p05", track_neuIso0p05, "track_neuIso0p05[ntracks]/F");
  BabyTree_->Branch("track_neuRelIso0p05", track_neuRelIso0p05, "track_neuRelIso0p05[ntracks]/F");
  BabyTree_->Branch("track_iso", track_iso, "track_iso[ntracks]/F");
  BabyTree_->Branch("track_reliso", track_reliso, "track_reliso[ntracks]/F");
  BabyTree_->Branch("track_isonomin", track_isonomin, "track_isonomin[ntracks]/F");
  BabyTree_->Branch("track_relisonomin", track_relisonomin, "track_relisonomin[ntracks]/F");
  BabyTree_->Branch("track_iso_uncorrected", track_iso_uncorrected, "track_iso_uncorrected[ntracks]/F");
  BabyTree_->Branch("track_reliso_uncorrected", track_reliso_uncorrected, "track_reliso_uncorrected[ntracks]/F");
  BabyTree_->Branch("track_iso_correction", track_iso_correction, "track_iso_correction[ntracks]/F");
  BabyTree_->Branch("track_reliso_correction", track_reliso_correction, "track_reliso_correction[ntracks]/F");
  BabyTree_->Branch("track_miniiso", track_miniiso, "track_miniiso[ntracks]/F");
  BabyTree_->Branch("track_minireliso", track_minireliso, "track_minireliso[ntracks]/F");
  BabyTree_->Branch("track_miniisonomin", track_miniisonomin, "track_miniisonomin[ntracks]/F");
  BabyTree_->Branch("track_minirelisonomin", track_minirelisonomin, "track_minirelisonomin[ntracks]/F");
  BabyTree_->Branch("track_miniiso_uncorrected", track_miniiso_uncorrected, "track_miniiso_uncorrected[ntracks]/F");
  BabyTree_->Branch("track_minireliso_uncorrected", track_minireliso_uncorrected, "track_minireliso_uncorrected[ntracks]/F");
  BabyTree_->Branch("track_miniiso_correction", track_miniiso_correction, "track_miniiso_correction[ntracks]/F");
  BabyTree_->Branch("track_minireliso_correction", track_minireliso_correction, "track_minireliso_correction[ntracks]/F");
  BabyTree_->Branch("track_isChargino", track_isChargino, "track_isChargino[ntracks]/I");
  BabyTree_->Branch("track_isDisappearingChargino", track_isDisappearingChargino, "track_isDisappearingChargino[ntracks]/I");
  BabyTree_->Branch("track_isBadCharginoTrack", track_isBadCharginoTrack, "track_isBadCharginoTrack[ntracks]/I");
  BabyTree_->Branch("track_matchedCharginoIdx", track_matchedCharginoIdx, "track_matchedCharginoIdx[ntracks]/I");
  BabyTree_->Branch("track_genPdgId", track_genPdgId, "track_genPdgId[ntracks]/I");
  BabyTree_->Branch("track_genMatchDR", track_genMatchDR, "track_genMatchDR[ntracks]/F");
  BabyTree_->Branch("track_nearestCharginoDR", track_nearestCharginoDR, "track_nearestCharginoDR[ntracks]/F");
  BabyTree_->Branch("track_decayXY", track_decayXY, "track_decayXY[ntracks]/F");
  BabyTree_->Branch("nCharginos", &nCharginos);
  BabyTree_->Branch("chargino_minDR", chargino_minDR, "chargino_minDR[nCharginos]/F");
  BabyTree_->Branch("chargino_isDisappearing", chargino_isDisappearing, "chargino_isDisappearing[nCharginos]/I");
  BabyTree_->Branch("chargino_matchedTrackIdx", chargino_matchedTrackIdx, "chargino_matchedTrackIdx[nCharginos]/I");
  BabyTree_->Branch("chargino_decayXY", chargino_decayXY, "chargino_decayXY[nCharginos]/F");
  BabyTree_->Branch("chargino_eta", chargino_eta, "chargino_eta[nCharginos]/F");
  BabyTree_->Branch("chargino_phi", chargino_phi, "chargino_phi[nCharginos]/F");
  BabyTree_->Branch("chargino_pt", chargino_pt, "chargino_pt[nCharginos]/F");

  // also make counter histogram
  count_hist_ = new TH1D("Count","Count",1,0,2);

  return;
}

void babyMaker::InitBabyNtuple () {

  run    = -999;
  lumi   = -999;
  evt    = -999;
  isData = -999;
  isGolden = -1;
  evt_scale1fb = 0;
  evt_xsec = -999.0;
  evt_kfactor = -999.0;
  evt_filter = -999.0;
  evt_nEvts = 0;
  //evt_id = -1; // don't reset every event
  genWeight = 0;
  weight_L1prefire = 1;
  weight_L1prefire_UP = 1;
  weight_L1prefire_DN = 1;
  puWeight = -999.0;
  nVert = -999;
  nTrueInt = -999;
  rho = -999.0;
  nJet30 = -999;
  nJet30JECup = -999;
  nJet30JECdn = -999;
  nJet40 = -999;
  nBJet20 = -999;
  nBJet20csv = -999;
  nBJet20JECup = -999;
  nBJet20JECdn = -999;
  nBJet25 = -999;
  nBJet30 = -999;
  nBJet30csv = -999;
  nBJet40 = -999;
  nJet30FailId = -999;
  nJet100FailId = -999;
  nJet20BadFastsim = -999;
  nJet200MuFrac50DphiMet = -999;
  nHEMJet20 = -999;
  nHEMJet30 = -999;
  nHEMJet40 = -999;
  nMuons10 = -999;
  nBadMuons20 = -999;
  nElectrons10 = -999;
  nLepLowMT = -999;
  nLepHighMT = -999;
  nTaus20 = -999;
  nGammas20 = -999;
  nPFCHCand3 = -999;
  deltaPhiMin = -999.0;
  deltaPhiMin_genmet = -999.0;
  diffMetMht = -999.0;
  diffMetMht_genmet = -999.0;
  deltaPhiMinJECup = -999.0;
  deltaPhiMinJECdn = -999.0;
  diffMetMhtJECup = -999.0;
  diffMetMhtJECdn = -999.0;
  minMTBMet = -999.0;
  zll_minMTBMet = -999.0;
  rl_minMTBMet = -999.0;
  gamma_minMTBMet = -999.0;
  ht = -999.0;
  htJECup = -999.0;
  htJECdn = -999.0;
  mt2 = -999.0;
  mt2JECup = -999.0;
  mt2JECdn = -999.0;
  mt2_gen = -999.0;
  mt2_genmet = -999.0;
  jet1_pt = 0.0;
  jet2_pt = 0.0;
  jet1_ptJECup = 0.0;
  jet2_ptJECup = 0.0;
  jet1_ptJECdn = 0.0;
  jet2_ptJECdn = 0.0;
  jet_failFSveto = 0.0;
  gamma_jet1_pt = 0.0;
  gamma_jet2_pt = 0.0;
  pseudoJet1_pt = 0.0;
  pseudoJet1_eta = 0.0;
  pseudoJet1_phi = 0.0;
  pseudoJet1_mass = 0.0;
  pseudoJet2_pt = 0.0;
  pseudoJet2_eta = 0.0;
  pseudoJet2_phi = 0.0;
  pseudoJet2_mass = 0.0;
  pseudoJet1JECup_pt = 0.0;
  pseudoJet1JECup_eta = 0.0;
  pseudoJet1JECup_phi = 0.0;
  pseudoJet1JECup_mass = 0.0;
  pseudoJet2JECup_pt = 0.0;
  pseudoJet2JECup_eta = 0.0;
  pseudoJet2JECup_phi = 0.0;
  pseudoJet2JECup_mass = 0.0;
  pseudoJet1JECdn_pt = 0.0;
  pseudoJet1JECdn_eta = 0.0;
  pseudoJet1JECdn_phi = 0.0;
  pseudoJet1JECdn_mass = 0.0;
  pseudoJet2JECdn_pt = 0.0;
  pseudoJet2JECdn_eta = 0.0;
  pseudoJet2JECdn_phi = 0.0;
  pseudoJet2JECdn_mass = 0.0;
  zll_pseudoJet1_pt = 0.0;
  zll_pseudoJet1_eta = 0.0;
  zll_pseudoJet1_phi = 0.0;
  zll_pseudoJet1_mass = 0.0;
  zll_pseudoJet2_pt = 0.0;
  zll_pseudoJet2_eta = 0.0;
  zll_pseudoJet2_phi = 0.0;
  zll_pseudoJet2_mass = 0.0;
  zll_pseudoJet1JECup_pt = 0.0;
  zll_pseudoJet1JECup_eta = 0.0;
  zll_pseudoJet1JECup_phi = 0.0;
  zll_pseudoJet1JECup_mass = 0.0;
  zll_pseudoJet2JECup_pt = 0.0;
  zll_pseudoJet2JECup_eta = 0.0;
  zll_pseudoJet2JECup_phi = 0.0;
  zll_pseudoJet2JECup_mass = 0.0;
  zll_pseudoJet1JECdn_pt = 0.0;
  zll_pseudoJet1JECdn_eta = 0.0;
  zll_pseudoJet1JECdn_phi = 0.0;
  zll_pseudoJet1JECdn_mass = 0.0;
  zll_pseudoJet2JECdn_pt = 0.0;
  zll_pseudoJet2JECdn_eta = 0.0;
  zll_pseudoJet2JECdn_phi = 0.0;
  zll_pseudoJet2JECdn_mass = 0.0;
  gamma_pseudoJet1_pt = 0.0;
  gamma_pseudoJet1_eta = 0.0;
  gamma_pseudoJet1_phi = 0.0;
  gamma_pseudoJet1_mass = 0.0;
  gamma_pseudoJet2_pt = 0.0;
  gamma_pseudoJet2_eta = 0.0;
  gamma_pseudoJet2_phi = 0.0;
  gamma_pseudoJet2_mass = 0.0;
  zllmt_pseudoJet1_pt = 0.0;
  zllmt_pseudoJet1_eta = 0.0;
  zllmt_pseudoJet1_phi = 0.0;
  zllmt_pseudoJet1_mass = 0.0;
  zllmt_pseudoJet2_pt = 0.0;
  zllmt_pseudoJet2_eta = 0.0;
  zllmt_pseudoJet2_phi = 0.0;
  zllmt_pseudoJet2_mass = 0.0;
  rl_pseudoJet1_pt = 0.0;
  rl_pseudoJet1_eta = 0.0;
  rl_pseudoJet1_phi = 0.0;
  rl_pseudoJet1_mass = 0.0;
  rl_pseudoJet2_pt = 0.0;
  rl_pseudoJet2_eta = 0.0;
  rl_pseudoJet2_phi = 0.0;
  rl_pseudoJet2_mass = 0.0;
  gen_pseudoJet1_pt = 0.0;
  gen_pseudoJet1_eta = 0.0;
  gen_pseudoJet1_phi = 0.0;
  gen_pseudoJet1_mass = 0.0;
  gen_pseudoJet2_pt = 0.0;
  gen_pseudoJet2_eta = 0.0;
  gen_pseudoJet2_phi = 0.0;
  gen_pseudoJet2_mass = 0.0;

  mht_pt = -999.0;
  mht_phi = -999.0;
  mht_ptJECup = -999.0;
  mht_phiJECup = -999.0;
  mht_ptJECdn = -999.0;
  mht_phiJECdn = -999.0;
  met_pt = -999.0;
  met_phi = -999.0;
  met_old2017_pt = -999.0;
  met_old2017_phi = -999.0;
  met_ptJECup = -999.0;
  met_phiJECup = -999.0;
  met_ptJECdn = -999.0;
  met_phiJECdn = -999.0;
  met_rawPt = -999.0;
  met_rawPhi = -999.0;
  met_caloPt = -999.0;
  met_caloPhi = -999.0;
  met_trkPt = -999.0;
  met_trkPhi = -999.0;
  met_genPt = -999.0;
  met_genPhi = -999.0;
  met_miniaodPt = -999.0;
  met_miniaodPhi = -999.0;
  Flag_EcalDeadCellTriggerPrimitiveFilter = -999;
  Flag_trkPOG_manystripclus53X = -999;
  Flag_ecalLaserCorrFilter = -999;
  Flag_trkPOG_toomanystripclus53X = -999;
  Flag_hcalLaserEventFilter = -999;
  Flag_trkPOG_logErrorTooManyClusters = -999;
  Flag_trkPOGFilters = -999;
  Flag_trackingFailureFilter = -999;
  Flag_CSCTightHaloFilter = -999;
  Flag_CSCTightHalo2015Filter = -999;
  Flag_globalTightHalo2016Filter = -999;
  Flag_globalSuperTightHalo2016Filter = -999;
  Flag_HBHENoiseFilter = -999;
  Flag_HBHENoiseIsoFilter = -999;
  Flag_goodVertices = -999;
  Flag_eeBadScFilter = -999;
  Flag_ecalBadCalibFilter = -999;
  Flag_ecalBadCalibFilterUpdate = -999;
  Flag_badMuonFilter = -999;
  Flag_badMuonFilterV2 = -999;    
  Flag_badMuons = -999;    
  Flag_duplicateMuons = -999;    
  Flag_noBadMuons = -999;    
  Flag_badChargedCandidateFilter = -999;
  Flag_badChargedHadronFilterV2 = -999;    
  Flag_METFilters = -999;
  HLT_PFHT1050 = -999;
  HLT_PFHT900 = -999;
  HLT_PFHT500_PFMET100_PFMHT100 = -999;
  HLT_PFHT700_PFMET85_PFMHT85 = -999;
  HLT_PFHT800_PFMET75_PFMHT75 = -999;
  HLT_PFMET170 = -999;
  HLT_PFHT300_PFMET100 = -999;
  HLT_PFHT300_PFMET110 = -999;
  HLT_PFHT350_PFMET100 = -999;
  HLT_PFHT350_PFMET120 = -999;
  HLT_PFMETNoMu90_PFMHTNoMu90 = -999;
  HLT_MonoCentralPFJet80_PFMETNoMu90_PFMHTNoMu90 = -999;
  HLT_PFMETNoMu100_PFMHTNoMu100 = -999;
  HLT_PFMETNoMu110_PFMHTNoMu110 = -999;
  HLT_PFMETNoMu120_PFMHTNoMu120 = -999;
  HLT_PFMETNoMu130_PFMHTNoMu130 = -999;
  HLT_PFMETNoMu140_PFMHTNoMu140 = -999;
  HLT_PFMETNoMu120_PFMHTNoMu120_PFHT60 = -999;
  HLT_PFMET90_PFMHT90 = -999;
  HLT_PFMET100_PFMHT100 = -999;
  HLT_PFMET110_PFMHT110 = -999;
  HLT_PFMET120_PFMHT120 = -999;
  HLT_PFMET130_PFMHT130 = -999;
  HLT_PFMET140_PFMHT140 = -999;
  HLT_PFMET100_PFMHT100_PFHT60 = -999;
  HLT_PFMET120_PFMHT120_PFHT60 = -999;
  HLT_PFJet450 = -999;
  HLT_PFJet500 = -999;
  HLT_ECALHT800 = -999;
  HLT_SingleMu = -999;   
  HLT_SingleMu_NonIso = -999;   
  HLT_SingleEl = -999;   
  HLT_SingleEl_NonIso = -999;   
  HLT_DoubleEl = -999;   
  HLT_DoubleEl33 = -999;   
  HLT_MuX_Ele12 = -999;   
  HLT_Mu8_EleX = -999;   
  HLT_Mu12_EleX = -999;   
  HLT_Mu30_Ele30_NonIso = -999;   
  HLT_Mu33_Ele33_NonIso = -999;   
  HLT_Mu37_Ele27_NonIso = -999;   
  HLT_Mu27_Ele37_NonIso = -999;   
  HLT_DoubleMu = -999;   
  HLT_DoubleMu_NonIso = -999;   
  HLT_Photon120 = -999;   
  HLT_Photon200 = -999;   
  HLT_Photon175_Prescale = -999;   
  HLT_Photon165_HE10 = -999;   
  HLT_Photon250_NoHE = -999;   
  HLT_PFHT180_Prescale = -999;
  HLT_PFHT250_Prescale = -999;
  HLT_PFHT370_Prescale = -999;
  HLT_PFHT430_Prescale = -999;
  HLT_PFHT510_Prescale = -999;
  HLT_PFHT590_Prescale = -999;
  HLT_PFHT680_Prescale = -999;
  HLT_PFHT780_Prescale = -999;
  HLT_PFHT890_Prescale = -999;
  HLT_PFHT125_Prescale = -999;
  HLT_PFHT200_Prescale = -999;
  HLT_PFHT300_Prescale = -999;
  HLT_PFHT350_Prescale = -999;
  HLT_PFHT475_Prescale = -999;
  HLT_PFHT600_Prescale = -999;
  HLT_DiCentralPFJet70_PFMET120 = -999;
  HLT_DiCentralPFJet55_PFMET110 = -999;
  nlep = -999;
  nisoTrack = -999;
  nhighPtPFcands = -999;
  nPFLep5LowMT = -999;
  nPFHad10LowMT = -999;
  ntau = -999;
  npfPhoton = -999;
  ngamma = -999;
  ngenPart = -999;
  ngenLep = -999;
  ngenStat23 = -999;
  ngenGamma = -999;
  ngenTau = -999;
  ngenTau1Prong = -999;
  ngenTau3Prong = -999;
  ngenLepFromTau = -999;
  ngenLepFromZ = -999;
  ngenNuFromZ = -999;
  njet = -999;
  gamma_mt2 = -999.0;
  gamma_nJet30 = -999;
  gamma_nJet40 = -999;
  gamma_nJet30FailId = -999;
  gamma_nJet100FailId = -999;
  gamma_nBJet20 = -999;
  gamma_nBJet20csv = -999;
  gamma_nBJet25 = -999;
  gamma_nBJet30 = -999;
  gamma_nBJet40 = -999;
  gamma_ht = -999.0;
  gamma_deltaPhiMin = -999.0;
  gamma_diffMetMht = -999.0;
  gamma_mht_pt = -999.0;
  gamma_mht_phi = -999.0;
  gamma_met_pt = -999.0;
  gamma_met_phi = -999.0;
  zll_mt2 = -999.0;
  zll_deltaPhiMin = -999.0;
  zll_diffMetMht = -999.0;
  zll_met_pt = -999.0;
  zll_met_phi = -999.0;
  zll_mht_pt = -999.0;
  zll_mht_phi = -999.0;
  zll_mass = -999.0;
  zll_pt = -999.0;
  zll_eta = -999.0;
  zll_phi = -999.0;
  zll_ht = -999.0;
  zll_mt2JECup = -999.0;
  zll_deltaPhiMinJECup = -999.0;
  zll_diffMetMhtJECup = -999.0;
  zll_met_ptJECup = -999.0;
  zll_met_phiJECup = -999.0;
  zll_mht_ptJECup = -999.0;
  zll_mht_phiJECup = -999.0;
  zll_htJECup = -999.0;
  zll_mt2JECdn = -999.0;
  zll_deltaPhiMinJECdn = -999.0;
  zll_diffMetMhtJECdn = -999.0;
  zll_met_ptJECdn = -999.0;
  zll_met_phiJECdn = -999.0;
  zll_mht_ptJECdn = -999.0;
  zll_mht_phiJECdn = -999.0;
  zll_htJECdn = -999.0;
  zllmt_mt2 = -999.0;
  zllmt_deltaPhiMin = -999.0;
  zllmt_diffMetMht = -999.0;
  zllmt_met_pt = -999.0;
  zllmt_met_phi = -999.0;
  zllmt_mht_pt = -999.0;
  zllmt_mht_phi = -999.0;
  zllmt_ht = -999.0;
  zllmt_mt = -999.0;
  rl_mt2 = -999.0;
  rl_deltaPhiMin = -999.0;
  rl_diffMetMht = -999.0;
  rl_met_pt = -999.0;
  rl_met_phi = -999.0;
  rl_mht_pt = -999.0;
  rl_mht_phi = -999.0;
  rl_mass = -999.0;
  rl_pt = -999.0;
  rl_eta = -999.0;
  rl_phi = -999.0;
  rl_ht = -999.0;
  GenSusyMScan1 = 0;
  GenSusyMScan2 = 0;
  GenSusyMScan3 = 0;
  GenSusyMScan4 = 0;
  weight_lepsf = 1.;
  weight_lepsf_UP = 1.;
  weight_lepsf_DN = 1.;
  weight_lepsf_0l = 1.;
  weight_lepsf_0l_UP = 1.;
  weight_lepsf_0l_DN = 1.;
  weight_btagsf = 1.;
  weight_btagsf_heavy_UP = 1.;
  weight_btagsf_light_UP = 1.;
  weight_btagsf_heavy_DN = 1.;
  weight_btagsf_light_DN = 1.;
  weight_sigtrigsf = 1.;
  weight_dileptrigsf = 1.;
  weight_phottrigsf = 1.;
  weight_pu = 1.;
  weight_isr = 1.;
  weight_isr_UP = 1.;
  weight_isr_DN = 1.;
  weight_scales_UP = 1.;
  weight_scales_DN = 1.;
  weight_pdfs_UP = 1.;
  weight_pdfs_DN = 1.;
  weight_toppt = 1.;
  genRecoil_pt = -999.;
  genTop_pt = -999.;
  genTbar_pt = -999.;
  genProd_pdgId = -999;
  nLHEweight = -999;
  weight_pol_L = -999;
  weight_pol_R = -999;
  nisrMatch = -999;

  for(int i=0; i < max_nlep; i++){
    lep_pt[i] = -999;
    lep_eta[i] = -999;
    lep_phi[i] = -999;
    lep_mass[i] = -999;
    lep_charge[i] = -999;
    lep_pdgId[i] = -999;
    lep_dxy[i] = -999;
    lep_dz[i] = -999;
    lep_tightId[i] = -999;
    lep_heepId[i] = -999;
    lep_highPtFit_pt[i] = -999;
    lep_highPtFit_eta[i] = -999;
    lep_highPtFit_phi[i] = -999;
    lep_relIso03[i] = -999;
    lep_relIso04[i] = -999;
    lep_miniRelIso[i] = -999;
    // lep_relIsoAn04[i] = -999;
    lep_mcMatchId[i] = -999;
    lep_lostHits[i] = -999;
    lep_convVeto[i] = -999;
    lep_tightCharge[i] = -999;
    lep_isPF[i] = 0;
  }

  for(int i=0; i < max_nisoTrack; i++){
    isoTrack_pt[i] = -999;
    isoTrack_eta[i] = -999;
    isoTrack_phi[i] = -999;
    isoTrack_mass[i] = -999;
    isoTrack_absIso[i] = -999;
    // isoTrack_relIsoAn04[i] = -999;
    isoTrack_dz[i] = -999;
    isoTrack_pdgId[i] = -999;
    isoTrack_mcMatchId[i] = -999;
  }

  for(int i=0; i < max_nhighPtPFcands; i++){
    highPtPFcands_pt[i] = -999;
    highPtPFcands_eta[i] = -999;
    highPtPFcands_phi[i] = -999;
    highPtPFcands_mass[i] = -999;
    highPtPFcands_absIso[i] = -999;
    // highPtPFcands_relIsoAn04[i] = -999;
    highPtPFcands_dz[i] = -999;
    highPtPFcands_pdgId[i] = -999;
    highPtPFcands_mcMatchId[i] = -999;
  }

  for(int i=0; i < max_ntau; i++){
    tau_pt[i] = -999;
    tau_eta[i] = -999;
    tau_phi[i] = -999;
    tau_mass[i] = -999;
    tau_charge[i] = -999;
    tau_dxy[i] = -999;
    tau_dz[i] = -999;
    tau_idCI3hit[i] = -999;
    tau_isoCI3hit[i] = -999;
    tau_mcMatchId[i] = -999;
  }

  for(int i=0; i < max_ngamma; i++){
    gamma_pt[i] = -999;
    gamma_eta[i] = -999;
    gamma_phi[i] = -999;
    gamma_mass[i] = -999;
    gamma_mcMatchId[i] = -999;
    gamma_genIso04[i] = -999;
    gamma_drMinParton[i] = -999;
    gamma_chHadIso[i] = -999;
    gamma_neuHadIso[i] = -999;
    gamma_phIso[i] = -999;
    gamma_sigmaIetaIeta[i] = -999;
    gamma_r9[i] = -999;
    gamma_hOverE[i] = -999;
    gamma_hOverE015[i] = -999;
    gamma_idCutBased[i] = -999;
    pfPhoton_pt[i] = -999;
    pfPhoton_eta[i] = -999;
    pfPhoton_phi[i] = -999;
  }

  if (saveGenParticles) {
    for(int i=0; i < max_ngenPart; i++){
      genPart_pt[i] = -999;
      genPart_eta[i] = -999;
      genPart_phi[i] = -999;
      genPart_mass[i] = -999;
      genPart_pdgId[i] = -999;
      genPart_status[i] = -999;
      genPart_charge[i] = -999;
      genPart_motherId[i] = -999;
      genPart_grandmotherId[i] = -999;
    }
  }

  for(int i=0; i < max_ngenLep; i++){
    genLep_pt[i] = -999;
    genLep_eta[i] = -999;
    genLep_phi[i] = -999;
    genLep_mass[i] = -999;
    genLep_pdgId[i] = -999;
    genLep_status[i] = -999;
    genLep_charge[i] = -999;
    genLep_sourceId[i] = -999;
  }

  for(int i=0; i < max_ngenStat23; i++){
    genStat23_pt[i] = -999;
    genStat23_eta[i] = -999;
    genStat23_phi[i] = -999;
    genStat23_mass[i] = -999;
    genStat23_pdgId[i] = -999;
    genStat23_status[i] = -999;
    genStat23_charge[i] = -999;
    genStat23_sourceId[i] = -999;
  }

  for(int i=0; i < max_ngenGamma; i++){
    genGamma_pt[i] = -999;
    genGamma_eta[i] = -999;
    genGamma_phi[i] = -999;
    genGamma_motherId[i] = -999;
  }

  for(int i=0; i < max_ngenTau; i++){
    genTau_pt[i] = -999;
    genTau_eta[i] = -999;
    genTau_phi[i] = -999;
    genTau_mass[i] = -999;
    genTau_pdgId[i] = -999;
    genTau_status[i] = -999;
    genTau_charge[i] = -999;
    genTau_sourceId[i] = -999;
    genTau_decayMode[i] = -999;
    genTau_leadTrackPt[i] = -999;
    genTau_neutralDaughters[i] = -999;
  }

  for(int i=0; i < max_ngenLepFromTau; i++){
    genLepFromTau_pt[i] = -999;
    genLepFromTau_eta[i] = -999;
    genLepFromTau_phi[i] = -999;
    genLepFromTau_mass[i] = -999;
    genLepFromTau_pdgId[i] = -999;
    genLepFromTau_status[i] = -999;
    genLepFromTau_charge[i] = -999;
    genLepFromTau_sourceId[i] = -999;
  }

  for(int i=0; i < max_njet; i++){
    jet_pt[i] = -999;
    jet_eta[i] = -999;
    jet_phi[i] = -999;
    jet_mass[i] = -999;
    jet_btagCSV[i] = -999;
    jet_btagDeepCSV[i] = -999;
    jet_chf[i] = -999;
    jet_nhf[i] = -999;
    jet_cemf[i] = -999;
    jet_nemf[i] = -999;
    jet_muf[i] = -999;
    jet_rawPt[i] = -999;
    jet_mcPt[i] = -999;
    jet_mcFlavour[i] = -999;
    jet_hadronFlavour[i] = -999;
    jet_qgl[i] = -999;
    jet_area[i] = -999;
    jet_id[i] = -999;
    jet_puId[i] = -999;
    good_jet_idxs[i] = -999;
    good_bjet_idxs[i] = -999;
  }

  rebal_status = -999;
  nRebalJets = -999;
  rebal_met_pt = -999;
  rebal_met_phi = -999;
  for(int i=0; i < max_njet; i++){
    rebal_useJet[i] = -999;
    rebal_jetpt[i] = -999;
    rebal_jeteta[i] = -999;
    rebal_jetphi[i] = -999;
    rebal_jetbtagdeepcsv[i] = -999;
    rebal_factors[i] = -999;
  }

  if (saveLHEweights || saveLHEweightsScaleOnly) {
    for(int i=0; i < max_nLHEweight; i++){
      LHEweight_wgt[i] = -999;
    }
  }
    
  ntracks = -999;
  nshorttracks = -999;
  nshorttracks_P = -999;
  nshorttracks_M = -999;
  nshorttracks_L = -999;
  nshorttrackcandidates = -999;
  nshorttrackcandidates_P = -999;
  nshorttrackcandidates_M = -999;
  nshorttrackcandidates_L = -999;
    
  for (int i=0; i < maxntracks; i++){
    track_pt[i] = -999;
    track_ptErr[i] = -999;
    track_eta[i] = -999;
    track_phi[i] = -999;
    track_charge[i] = -999;
    track_dedxStrip[i] = -999;
    track_dedxPixel[i] = -999;
    track_nChi2[i] = -999;
    track_ipSigXY[i] = -999;
    track_dxy[i] = -999;
    track_dxyErr[i] = -999;
    track_dz[i] = -999;
    track_dzErr[i] = -999;
    track_isHighPurity[i] = -999;
    track_recoveto[i] = -999;
    track_DeadECAL[i] = 0;
    track_DeadHCAL[i] = 0;
    track_isshort[i] = 0;
    track_iscandidate[i] = 0;
    track_ispixelonly[i] = 0;
    track_ismedium[i] = 0;
    track_islong[i] = 0;
    track_ispixelonlycandidate[i] = 0;
    track_ismediumcandidate[i] = 0;
    track_islongcandidate[i] = 0;
    track_HitSignature[i] = -999;
    track_nPixelHits[i] = -999;
    track_nLostOuterHits[i] = -999;
    track_nLostInnerPixelHits[i] = -999;
    track_nLayersWithMeasurement[i] = -999;
    track_nPixelLayersWithMeasurement[i] = -999;
    track_nearestPF_id[i] = -999;
    track_nearestPF_pt[i] = -999;
    track_nearestPF_DR[i] = -999;
    track_jetDR[i] = -999;
    track_jetPt[i] = -999;
    track_jetEta[i] = -999;
    track_jetPhi[i] = -999;
    track_chHIso0p3[i] = -999;
    track_chHminiIso[i] = -999;
    track_neuHIso0p3[i] = -999;
    track_neuHminiIso[i] = -999;
    track_phIso0p3[i] = -999;
    track_phminiIso[i] = -999;
    track_isLepOverlap[i] = -999;
    track_neuIso0p05[i] = -999;
    track_neuRelIso0p05[i] = -999;
    track_iso[i] = -999;
    track_reliso[i] = -999;
    track_isonomin[i] = -999;
    track_relisonomin[i] = -999;
    track_iso_uncorrected[i] = -999;
    track_reliso_uncorrected[i] = -999;
    track_iso_correction[i] = -999;
    track_reliso_correction[i] = -999;      
    track_miniiso[i] = -999;
    track_minireliso[i] = -999;
    track_miniisonomin[i] = -999;
    track_minirelisonomin[i] = -999;
    track_miniiso_uncorrected[i] = -999;
    track_minireliso_uncorrected[i] = -999;
    track_miniiso_correction[i] = -999;
    track_minireliso_correction[i] = -999;      
    track_isChargino[i] = 0;
    track_isDisappearingChargino[i] = 0;
    track_matchedCharginoIdx[i] = -999;
    track_isBadCharginoTrack[i] = 0;
    track_nearestCharginoDR[i] = -999;
    track_genPdgId[i] = -999;
    track_genMatchDR[i] = -999;
    track_decayXY[i] = -999;
  }
  for (int i = 0; i < max_nchargino; i++) {
    chargino_minDR[i] = -999;
    chargino_decayXY[i] = -999;
    chargino_eta[i] = -999;
    chargino_phi[i] = -999;
    chargino_pt[i] = -999;
    chargino_isDisappearing[i] = -999;
    chargino_matchedTrackIdx[i] = -999;
  }
  nCharginos = 0;

  return;
}

void babyMaker::FillBabyNtuple(){
  BabyTree_->Fill();
  return;
}

void babyMaker::CloseBabyNtuple(){
  BabyFile_->cd();
  BabyTree_->Write("mt2",TObject::kWriteDelete);
  count_hist_->Write();
  BabyFile_->Close();
  return;
}

float babyMaker::getBtagEffFromFile(float pt, float eta, int mcFlavour, bool isFastsim){
  if(!h_btag_eff_b || !h_btag_eff_c || !h_btag_eff_udsg) {
    std::cout << "babyMaker::getBtagEffFromFile: ERROR: missing input hists" << std::endl;
    return 1.;
  }

  if(isFastsim && (!h_btag_eff_b_fastsim || !h_btag_eff_c_fastsim || !h_btag_eff_udsg_fastsim)) {
    std::cout << "babyMaker::getBtagEffFromFile: ERROR: missing input hists" << std::endl;
    return 1.;
  }

  // only use pt bins up to 400 GeV for charm and udsg
  float pt_cutoff = std::max(20.,std::min(399.,double(pt)));
  TH2D* h(0);
  if (abs(mcFlavour) == 5) {
    h = isFastsim ? h_btag_eff_b_fastsim : h_btag_eff_b;
    // use pt bins up to 600 GeV for b
    pt_cutoff = std::max(20.,std::min(599.,double(pt)));
  }
  else if (abs(mcFlavour) == 4) {
    h = isFastsim ? h_btag_eff_c_fastsim : h_btag_eff_c;
  }
  else {
    h = isFastsim ? h_btag_eff_udsg_fastsim : h_btag_eff_udsg;
  }
    
  int binx = h->GetXaxis()->FindBin(pt_cutoff);
  int biny = h->GetYaxis()->FindBin(fabs(eta));
  return h->GetBinContent(binx,biny);
}

void babyMaker::minuitFunction(int& nDim, double* gout, double& result, double par[], int flg) {
  babyMaker *t = thisBabyMaker;
  float likelihood = 0;
  float pt_constrained_x = 0.0;
  float pt_constrained_y = 0.0;
  float min_prob = 1E-20;
  for(int i=0; i < t->nRebalJets; i++){
    bool isBjet = (t->rebal_jetbtagdeepcsv[i] > config_.btag_med_threshold_DeepCSV);        
    float prob = t->rebal_reader.GetValue(t->rebal_jetpt[i]/par[i], fabs(t->rebal_jeteta[i]), isBjet, par[i]);
    prob = max(prob, min_prob);
    likelihood += log(prob);
    pt_constrained_x -= (t->rebal_jetpt[i])*cos(t->rebal_jetphi[i])/par[i];
    pt_constrained_y -= (t->rebal_jetpt[i])*sin(t->rebal_jetphi[i])/par[i];
  }
  //float x1 = (pt_soft_x - pt_constrained_x)/sigma_soft;
  //float x2 = (pt_soft_y - pt_constrained_y)/sigma_soft;
  float x1 = (t->rebal_pt_soft_x - pt_constrained_x)/rebal_sigma_soft_;
  float x2 = (t->rebal_pt_soft_y - pt_constrained_y)/rebal_sigma_soft_;
  likelihood += -x1*x1/2;
  likelihood += -x2*x2/2;

  result = -likelihood;

}
