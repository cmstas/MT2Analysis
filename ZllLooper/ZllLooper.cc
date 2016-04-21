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
#include "TLorentzVector.h"


// Tools
#include "../CORE/Tools/utils.h"
#include "../CORE/Tools/goodrun.h"
#include "../CORE/Tools/dorky/dorky.h"
#include "../CORE/Tools/badEventFilter.h"

// header
#include "ZllLooper.h"

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
ZllLooper::ZllLooper(){
}
//_______________________________________
ZllLooper::~ZllLooper(){

};

//_______________________________________
void ZllLooper::SetSignalRegions(){

  // CRZll base: just good photon
  CRZllBase.SetName("crZllbaseZeroB");

  //FIXME: define additonal SRs here

}


//_______________________________________
void ZllLooper::loop(TChain* chain, std::string sample, std::string output_dir){

  // Benchmark
  TBenchmark *bmark = new TBenchmark();
  bmark->Start("benchmark");

  gROOT->cd();
  TString output_name = Form("%s/%s.root",output_dir.c_str(),sample.c_str());
  cout << "[ZllLooper::loop] creating output file: " << output_name << endl;

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
  
  //load GJets file with reweighting
  TFile* f_weights = new TFile("data/hists_reweight.root");
  TH1D* h_weightMET200 = (TH1D*) f_weights->Get("h_ratioMET200");
  TH1D* h_weightMET300 = (TH1D*) f_weights->Get("h_ratioMET300");
  TH1D* h_weightMET500 = (TH1D*) f_weights->Get("h_ratioMET500");

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

  cout << "[ZllLooper::loop] setting up histos" << endl;

  SetSignalRegions();

  // File Loop
  int nDuplicates = 0;
  int nEvents = chain->GetEntries();
  unsigned int nEventsChain = nEvents;
  cout << "[ZllLooper::loop] running on " << nEventsChain << " events" << endl;
  unsigned int nEventsTotal = 0;
  TObjArray *listOfFiles = chain->GetListOfFiles();
  TIter fileIter(listOfFiles);
  TFile *currentFile = 0;
  while ( (currentFile = (TFile*)fileIter.Next()) ) {
    cout << "[ZllLooper::loop] running on file: " << currentFile->GetTitle() << endl;

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
      }

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

      // basic selection, triggers, etc.
      if (t.nVert == 0) continue;
      if (t.nlepIso != 2) continue;
      if (t.nlep != 2) continue;
      
      // trigger requirement on data
      if (t.isData && !(t.HLT_SingleEl || t.HLT_SingleMu)) continue;
        
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

      //-----------SELECTION BELOW------------
      
      //reinitialize global vars
      removedzllLep_ = -1;
      zllmt_ = -1;
      zllmet_ = -1;
      bosonScale_ = -1;
      metScale_ = -1;
      
      // require baseline selection
      bool pass_selection = (t.ht > 200 && t.nJet30 > 1 && t.nBJet20 == 0);
      if (!pass_selection) continue;   

      bool failIsoId = false;
      //require lepton Iso/ID
      for (int ilep = 0; ilep < t.nlep; ilep++) {
	//barrel
	if (fabs(t.lep_eta[ilep]) > 1.479) failIsoId = true;
	//pt
	if (t.lep_pt[ilep] > 20 && t.lep_pt[ilep] < 25) failIsoId = true;
	//iso and ID
	if (t.lep_pt[ilep] < 20) {
	  if (abs(t.lep_pdgId[ilep]) == 13 && (t.lep_miniRelIso[ilep]>0.1 || t.lep_relIso03[ilep]>0.2 || abs(t.lep_dxy[ilep])> 0.02 || abs(t.lep_dz[ilep]) > 0.02)) failIsoId = true;
	  if (abs(t.lep_pdgId[ilep]) == 11 && (t.lep_miniRelIso[ilep]>0.1 || t.lep_relIso03[ilep]>0.2 || t.lep_tightIdNoIso[ilep] == 0)) failIsoId = true;
	}
	else {
	  if (abs(t.lep_pdgId[ilep]) == 13 && ((t.lep_miniRelIso[ilep]>0.1 || t.lep_relIso03[ilep]>0.2 || abs(t.lep_dxy[ilep])>0.02 || abs(t.lep_dz[ilep]) > 0.02))) failIsoId = true;
	  if (abs(t.lep_pdgId[ilep]) == 11 && ((t.lep_miniRelIso[ilep]>0.1 || t.lep_relIso03[ilep]>0.1 || t.lep_tightIdNoIso[ilep] < 2 || t.lep_relIso03[ilep]*t.lep_pt[ilep]>5))) failIsoId = true;
	}
	
      }
      if (failIsoId) continue;

      //calculate dilepMll
      TLorentzVector lep1_p4(0,0,0,0);
      TLorentzVector lep2_p4(0,0,0,0);
      lep1_p4.SetPtEtaPhiM(t.lep_pt[0],t.lep_eta[0],t.lep_phi[0],t.lep_mass[0]);
      lep2_p4.SetPtEtaPhiM(t.lep_pt[1],t.lep_eta[1],t.lep_phi[1],t.lep_mass[1]);
      TLorentzVector dilep_p4 = lep1_p4 + lep2_p4;
      float dilepmll = dilep_p4.M();

      if (dilepmll < 75 || dilepmll > 105) continue;

      //calculate mt by removing random lepton
      //initialize MET variables
      float metX = t.met_pt * cos(t.met_phi);
      float metY = t.met_pt * sin(t.met_phi);
      TVector2* newMet = new TVector2;
      
      float lep1X = t.lep_pt[0] * cos(t.lep_phi[0]);
      float lep1Y = t.lep_pt[0] * sin(t.lep_phi[0]);
      float lep2X = t.lep_pt[1] * cos(t.lep_phi[1]);
      float lep2Y = t.lep_pt[1] * sin(t.lep_phi[1]);
      
      //remove random lepton, depending on event number
      if (t.evt % 2 == 1) {
	removedzllLep_ = 1; 
	newMet->SetX(metX+lep1X);
	newMet->SetY(metY+lep1Y);
      }
      else {
	removedzllLep_ = 2;
	newMet->SetX(metX+lep2X);
	newMet->SetY(metY+lep2Y);
      }
      
      zllmet_ = newMet->Mod(); //alternate MET
      if (removedzllLep_ == 1) zllmt_ = sqrt( 2 * newMet->Mod() * t.lep_pt[1] * ( 1 - cos( newMet->Phi() - t.lep_phi[1]) ) ); //recalculated MT
      else if (removedzllLep_ == 2) zllmt_ = sqrt( 2 * newMet->Mod() * t.lep_pt[0] * ( 1 - cos( newMet->Phi() - t.lep_phi[0]) ) ); //recalculated MT

      //min MT
      if (zllmt_ < 20) continue;

      //classify
      bool isDilepton = false;
      if (t.ngenLep + t.ngenTau >= 2) isDilepton = true;      

      // ---- re-weighting based on GJets studies ----

      //first get boson+MET bin
      TVector2* ZPlusMet = new TVector2;
      float ZX = dilep_p4.Pt() * cos(dilep_p4.Phi());
      float ZY = dilep_p4.Pt() * sin(dilep_p4.Phi());
      ZPlusMet->SetX(metX+ZX);
      ZPlusMet->SetY(metY+ZY);
      bosonScale_ = ZPlusMet->Mod() / dilep_p4.Pt(); 
      
      //load GJets file with reweighting
      if (!t.isData) {
	if (zllmet_ > 200 && zllmet_ < 300) {
	  metScale_ = h_weightMET200->GetBinContent(h_weightMET200->FindBin(bosonScale_));
	}
	else if (zllmet_ > 300 && zllmet_ < 500) {
	  metScale_ = h_weightMET300->GetBinContent(h_weightMET300->FindBin(bosonScale_));
	}
	else if (zllmet_ > 500) {
	  metScale_ = h_weightMET500->GetBinContent(h_weightMET500->FindBin(bosonScale_));
	}
      }
            
      //plot
      string suf = "";

      if (zllmet_ > 200) fillHistosZll(CRZllBase.crslHistMap,"crZllbaseZeroB", "MET200"+suf);
      if (zllmet_ > 150) fillHistosZll(CRZllBase.crslHistMap,"crZllbaseZeroB", "MET150"+suf);
      if (zllmet_ > 100) fillHistosZll(CRZllBase.crslHistMap,"crZllbaseZeroB", "MET100"+suf);
      if (zllmet_ > 50)  fillHistosZll(CRZllBase.crslHistMap,"crZllbaseZeroB", "MET50"+suf);

      if (isDilepton) suf += "Dilepton";
      else suf += "Fake";
      
      if (zllmet_ > 200) fillHistosZll(CRZllBase.crslHistMap,"crZllbaseZeroB", "MET200"+suf);
      if (zllmet_ > 150) fillHistosZll(CRZllBase.crslHistMap,"crZllbaseZeroB", "MET150"+suf);
      if (zllmet_ > 100) fillHistosZll(CRZllBase.crslHistMap,"crZllbaseZeroB", "MET100"+suf);
      if (zllmet_ > 50)  fillHistosZll(CRZllBase.crslHistMap,"crZllbaseZeroB", "MET50"+suf);
      
    }//end loop on events in a file
  
    delete tree;
    f.Close();
  }//end loop on files
  
  cout << "[ZllLooper::loop] processed " << nEventsTotal << " events" << endl;
  if ( nEventsChain != nEventsTotal ) {
    std::cout << "ERROR: number of events from files is not equal to total number of events" << std::endl;
  }

  cout << nDuplicates << " duplicate events were skipped." << endl;

  //---------------------
  // Save Plots
  //---------------------

  outfile_->cd();
  savePlotsDir(h_1d_global,outfile_,"");
  savePlotsDir(CRZllBase.crslHistMap,outfile_,"crZllbaseZeroB");

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
void ZllLooper::fillHistosZll(std::map<std::string, TH1*>& h_1d, const std::string& dirname, const std::string& s) {

  if (dirname.size()) {
    TDirectory * dir = (TDirectory*)outfile_->Get(dirname.c_str());
    if (dir == 0) {
      dir = outfile_->mkdir(dirname.c_str());
    } 
    dir->cd();
  } else {
    outfile_->cd();
  }

  //mt bins
  float mtbins[4] = {20., 90., 120., 300.};

  //zll mt bins
  float zllmtbins[4] = {20., 100., 130., 300.};
  
  //zllCR specific plots
  plot1D("h_zllmt"+s,            zllmt_,   evtweight_, h_1d, ";M_{T} [GeV]", 250, 0, 250);
  plot1D("h_zllmtbins"+s,            zllmt_,   evtweight_, h_1d, ";M_{T} [GeV]", 3, mtbins);
  plot1D("h_zllAltmtbins"+s,            zllmt_,   evtweight_, h_1d, ";M_{T} [GeV]", 3, zllmtbins);
  plot1D("h_zllmet"+s,       zllmet_,   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 100, 0, 1000);

  if (metScale_ != -1) {
    plot1D("h_zllmtScaled"+s,            zllmt_,   evtweight_*metScale_, h_1d, ";M_{T} [GeV]", 250, 0, 250);
    plot1D("h_zllmtbinsScaled"+s,            zllmt_,   evtweight_*metScale_, h_1d, ";M_{T} [GeV]", 3, mtbins);
    plot1D("h_zllAltmtbinsScaled"+s,            zllmt_,   evtweight_*metScale_, h_1d, ";M_{T} [GeV]", 3, zllmtbins);
    plot1D("h_zllmetScaled"+s,       zllmet_,   evtweight_*metScale_, h_1d, ";E_{T}^{miss} [GeV]", 100, 0, 1000);
  }
  
  plot1D("h_removedZlllep"+s,       removedzllLep_,   evtweight_, h_1d, "removedLep", 3, 0, 3);
  if (removedzllLep_ == 1) plot1D("h_removedZllleppt"+s,      t.lep_pt[0],   evtweight_, h_1d, ";rem p_{T}(lep) [GeV]", 200, 0, 1000);
  else if (removedzllLep_ == 2) plot1D("h_removedZllleppt"+s,      t.lep_pt[1],   evtweight_, h_1d, ";rem p_{T}(lep) [GeV]", 200, 0, 1000);
  
  outfile_->cd();
  return;
}
