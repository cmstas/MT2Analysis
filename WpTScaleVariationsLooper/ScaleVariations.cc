// C++
#include <iostream>
#include <vector>
#include <set>
#include <cmath>

// ROOT
#include "TDirectory.h"
#include "TTreeCache.h"
#include "Math/VectorUtil.h"
#include "TVector2.h"
#include "TBenchmark.h"

// Tools
#include "../CORE/Tools/utils.h"
#include "../CORE/Tools/goodrun.h"
#include "../CORE/Tools/dorky/dorky.h"
#include "../CORE/Tools/badEventFilter.h"

// header
#include "ScaleVariations.h"

//MT2
#include "../MT2CORE/Plotting/PlotUtilities.h"


using namespace std;
using namespace mt2;
using namespace duplicate_removal;

class mt2tree;
class SR;

// turn on to apply weights to central value
bool applyWeights = false;
// turn on to apply Nvtx reweighting to MC
bool doNvtxReweight = true;
// turn on to apply json file to data
bool applyJSON = false;

//_______________________________________
ScaleVariations::ScaleVariations(){
}
//_______________________________________
ScaleVariations::~ScaleVariations(){

};

//_______________________________________
void ScaleVariations::SetSignalRegions(){

  // CRScaleVar base: just good photon
  CRScaleVarBase.SetName("crgjetbase");

  //FIXME: define additonal SRs here

}


//_______________________________________
void ScaleVariations::loop(TChain* chain, std::string sample, std::string output_dir){

  // Benchmark
  TBenchmark *bmark = new TBenchmark();
  bmark->Start("benchmark");

  gROOT->cd();
  TString output_name = Form("%s/%s.root",output_dir.c_str(),sample.c_str());
  cout << "[ScaleVariations::loop] creating output file: " << output_name << endl;

  outfile_ = new TFile(output_name.Data(),"RECREATE") ; 

  const char* json_file = "../babymaker/jsons/Cert_246908-251883_13TeV_PromptReco_Collisions15_JSON_v2_snt.txt";
  if (applyJSON) {
    cout << "Loading json file: " << json_file << endl;
    set_goodrun_file(json_file);
  }

  h_nvtx_weights_ = 0;
  if (doNvtxReweight) {
    TFile* f_weights = new TFile("../babymaker/data/hists_reweight_zjets_Run2015B.root");
    TH1D* h_nvtx_weights_temp = (TH1D*) f_weights->Get("h_nVert_ratio");
    // TFile* f_weights = new TFile("nvtx_ratio.root");
    // TH1D* h_nvtx_weights_temp = (TH1D*) f_weights->Get("h_vtx_ratio");
    outfile_->cd();
    h_nvtx_weights_ = (TH1D*) h_nvtx_weights_temp->Clone("h_nvtx_weights");
    f_weights->Close();
  }
  
  eventFilter metFilterTxt;
  TString stringsample = sample;
  if (stringsample.Contains("data")) {
    cout<<"Loading bad event files ..."<<endl;
    metFilterTxt.loadBadEventList("/nfs-6/userdata/mt2utils/eventlist_DoubleEG_csc2015.txt");
    metFilterTxt.loadBadEventList("/nfs-6/userdata/mt2utils/eventlist_DoubleMuon_csc2015.txt");
    metFilterTxt.loadBadEventList("/nfs-6/userdata/mt2utils/eventlist_HTMHT_csc2015.txt");
    metFilterTxt.loadBadEventList("/nfs-6/userdata/mt2utils/eventlist_JetHT_csc2015.txt");
    metFilterTxt.loadBadEventList("/nfs-6/userdata/mt2utils/eventlist_MET_csc2015.txt");
    metFilterTxt.loadBadEventList("/nfs-6/userdata/mt2utils/eventlist_SingleElectron_csc2015.txt");
    metFilterTxt.loadBadEventList("/nfs-6/userdata/mt2utils/eventlist_SingleMuon_csc2015.txt");
    metFilterTxt.loadBadEventList("/nfs-6/userdata/mt2utils/eventlist_SinglePhoton_csc2015.txt");
    cout<<" ... finished!"<<endl;
  }

  cout << "[ScaleVariations::loop] setting up histos" << endl;

  SetSignalRegions();

  // File Loop
  int nDuplicates = 0;
  int nEvents = chain->GetEntries();
  unsigned int nEventsChain = nEvents;
  cout << "[ScaleVariations::loop] running on " << nEventsChain << " events" << endl;
  unsigned int nEventsTotal = 0;
  TObjArray *listOfFiles = chain->GetListOfFiles();
  TIter fileIter(listOfFiles);
  TFile *currentFile = 0;
  while ( (currentFile = (TFile*)fileIter.Next()) ) {
    cout << "[ScaleVariations::loop] running on file: " << currentFile->GetTitle() << endl;

    // Get File Content
    TFile f( currentFile->GetTitle() );
    TTree *tree = (TTree*)f.Get("mt2");
    TTreeCache::SetLearnEntries(10);
    tree->SetCacheSize(128*1024*1024);
    //mt2tree t(tree);
    
    // Use this to speed things up when not looking at genParticles
    //tree->SetBranchStatus("genPart_*", 0); 

    t.Init(tree);

    // Event Loop
    unsigned int nEventsTree = tree->GetEntriesFast();
    for( unsigned int event = 0; event < nEventsTree; ++event) {
      
      t.GetEntry(event);

      //---------------------
      // bookkeeping and progress report
      //---------------------
      ++nEventsTotal;
      if (nEventsTotal%10000==0) {
	ULong64_t i_permille = (int)floor(1000 * nEventsTotal / float(nEventsChain));
	if (isatty(1)) {
	  printf("\015\033[32m ---> \033[1m\033[31m%4.1f%%"
		 "\033[0m\033[32m <---\033[0m\015", i_permille/10.);
	  fflush(stdout);
	}
	else {
	  cout<<i_permille/10.<<" ";
	  if (nEventsTotal%100000==0) cout<<endl;
	}
      }

//      if (!t.HLT_MuX_Ele12 && !t.HLT_Mu8_EleX && !t.HLT_DoubleEl && !t.HLT_DoubleMu ) continue; // HLT. Do this first for max speed
//      if (t.met_pt < 100) continue;
//      if (t.nlepIso < 2) continue;

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

      // MET filters (data and MC)
      if (!t.Flag_goodVertices) continue;
      //if (!t.Flag_CSCTightHaloFilter) continue; // use txt files instead
      if (!t.Flag_eeBadScFilter) continue;
      if (!t.Flag_HBHENoiseFilter) continue;
      if (!t.Flag_HBHEIsoNoiseFilter) continue;
      // txt MET filters (data only)
      if (t.isData && metFilterTxt.eventFails(t.run, t.lumi, t.evt)) {
	//cout<<"Found bad event in data: "<<t.run<<", "<<t.lumi<<", "<<t.evt<<endl;
	continue;
      }

      //bool passJetID = true;
      if (t.nJet30FailId > 0) continue;

      if (t.nVert == 0) continue;
      
      // remove low pt QCD samples 
      if (t.evt_id >= 100 && t.evt_id < 108) continue;

      //---------------------
      // set weights and start making plots
      //---------------------
      outfile_->cd();
      //      const float lumi = 4.;
      //      const float lumi = 0.042;
      const float lumi = 2.26;
      evtweight_ = 1.;
      
      // apply relevant weights to MC
      if (!t.isData) {
	evtweight_ = t.evt_scale1fb * lumi;
	if (applyWeights) evtweight_ *= t.weight_lepsf * t.weight_btagsf * t.weight_isr * t.weight_pu;
	// get pu weight from hist, restrict range to nvtx 4-31
	if (doNvtxReweight) {
	  int nvtx_input = t.nVert;
	  if (t.nVert > 31) nvtx_input = 31;
	  if (t.nVert < 4) nvtx_input = 4;
	  float puWeight = h_nvtx_weights_->GetBinContent(h_nvtx_weights_->FindBin(nvtx_input));
	  evtweight_ *= puWeight;
	}
      } // !isData

      // select hard lepton
      int foundEl = 0;
      int foundMu = 0;
      int charge = 1;
      for (int ilep = 0; ilep < t.nlep; ++ilep) {
	if ( t.lep_pt[ilep] < 25 ) continue;
	if ( abs(t.lep_eta[ilep])>2.5 ) continue; 
	//iso/id requirements
	if (abs(t.lep_pdgId[ilep]) == 13 && (t.lep_miniRelIso[ilep]<0.1 && t.lep_relIso03[ilep]<0.2 && abs(t.lep_dxy[ilep])< 0.02 && abs(t.lep_dz[ilep]) < 0.02)) 
	  { foundMu++; charge *= t.lep_charge[ilep];}
	if (abs(t.lep_pdgId[ilep]) == 11 && (t.lep_miniRelIso[ilep]<0.1 && t.lep_relIso03[ilep]<0.1 && t.lep_tightIdNoIso[ilep] >= 2 && t.lep_relIso03[ilep]*t.lep_pt[ilep]<5)) 
	  { foundEl++; charge *= t.lep_charge[ilep];}
      }

      bool pass_charge = (charge < 0);
      bool pass_emu = (foundMu==1) && (foundEl==1);
      bool pass_ee  = (foundEl==2) && (t.zll_mass < 75 || t.zll_mass > 105);
      bool pass_mm  = (foundMu==2) && (t.zll_mass < 75 || t.zll_mass > 105);
      bool pass_met = (t.met_pt > 100);

      if (!pass_charge || !pass_met || t.nBJet20<1) continue;

      
  
      fillHistosScaleVar(CRScaleVarBase.crslHistMap,"crttdlbase");
      if ( pass_emu) fillHistosScaleVar(CRScaleVarBase.crslHistMap,"crttdlbase","EM");
      else if ( pass_ee ) fillHistosScaleVar(CRScaleVarBase.crslHistMap,"crttdlbase","EE");
      else if ( pass_mm ) fillHistosScaleVar(CRScaleVarBase.crslHistMap,"crttdlbase","MM");
      
    }//end loop on events in a file
  
    delete tree;
    f.Close();
  }//end loop on files
  
  cout << "[ScaleVariations::loop] processed " << nEventsTotal << " events" << endl;
  if ( nEventsChain != nEventsTotal ) {
    std::cout << "ERROR: number of events from files is not equal to total number of events" << std::endl;
  }

  cout << nDuplicates << " duplicate events were skipped." << endl;

  //---------------------
  // Save Plots
  //---------------------

  outfile_->cd();
  savePlotsDir(h_1d_global,outfile_,"");
  savePlotsDir(CRScaleVarBase.crslHistMap,outfile_,"crttdlbase");

  //---------------------
  // Write and Close file
  //---------------------
  outfile_->Write();
  outfile_->Close();
  delete outfile_;

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

//_______________________________________
void ScaleVariations::fillHistosScaleVar(std::map<std::string, TH1*>& h_1d, const std::string& dirname, const std::string& s) {

  if (dirname.size()) {
    TDirectory * dir = (TDirectory*)outfile_->Get(dirname.c_str());
    if (dir == 0) {
      dir = outfile_->mkdir(dirname.c_str());
    } 
    dir->cd();
  } else {
    outfile_->cd();
  }
      
  
  plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
  plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
  plot1D("h_nVert"+s,          t.nVert,   evtweight_, h_1d, ";N(vertices)",50,0,50);
  plot1D("h_nJet30"+s,     t.nJet30,   evtweight_, h_1d, ";N(jets)", 15, 0, 15);
  plot1D("h_nBJet20"+s,    t.nBJet20,   evtweight_, h_1d, ";N(bjets)", 6, 0, 6);
  plot1D("h_met"+s,       t.met_pt,   evtweight_, h_1d, ";MET", 100, 0, 1000);
  
  outfile_->cd();
  return;
}
