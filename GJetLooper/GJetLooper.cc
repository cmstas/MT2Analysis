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
#include "GJetLooper.h"

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
GJetLooper::GJetLooper(){
}
//_______________________________________
GJetLooper::~GJetLooper(){

};

//_______________________________________
void GJetLooper::SetSignalRegions(){

  // CRGJet base: just good photon
  CRGJetBase.SetName("crgjetbase");

  //FIXME: define additonal SRs here

}


//_______________________________________
void GJetLooper::loop(TChain* chain, std::string sample, std::string output_dir){

  // Benchmark
  TBenchmark *bmark = new TBenchmark();
  bmark->Start("benchmark");

  gROOT->cd();
  TString output_name = Form("%s/%s.root",output_dir.c_str(),sample.c_str());
  cout << "[GJetLooper::loop] creating output file: " << output_name << endl;

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

  cout << "[GJetLooper::loop] setting up histos" << endl;

  SetSignalRegions();

  // File Loop
  int nDuplicates = 0;
  int nEvents = chain->GetEntries();
  unsigned int nEventsChain = nEvents;
  cout << "[GJetLooper::loop] running on " << nEventsChain << " events" << endl;
  unsigned int nEventsTotal = 0;
  TObjArray *listOfFiles = chain->GetListOfFiles();
  TIter fileIter(listOfFiles);
  TFile *currentFile = 0;
  while ( (currentFile = (TFile*)fileIter.Next()) ) {
    cout << "[GJetLooper::loop] running on file: " << currentFile->GetTitle() << endl;

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

      // basic photon selection, triggers, etc.
      if (t.nVert == 0) continue;
      if (t.ngamma == 0) continue;

      if (!t.HLT_Photon165_HE10) continue;
      
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


      // require good photon and baseline selection
      bool pass_photon = (t.gamma_pt[0] > 200 && t.gamma_idCutBased[0] && t.gamma_chHadIso[0] < 2.5);
      bool pass_selection = (t.nlep == 0 && t.nJet30FailId == 0 && t.ht > 200 && t.nJet30 > 1);
      if (!pass_photon || !pass_selection) continue;

      
      //calculate additional MET variables
      float metX = t.met_pt * cos(t.met_phi);
      float metY = t.met_pt * sin(t.met_phi);
      TVector2* recoMet = new TVector2;
      recoMet->SetX(metX); recoMet->SetY(metY);
      float mhtX = t.mht_pt * cos(t.mht_phi);
      float mhtY = t.mht_pt * sin(t.mht_phi);
      TVector2* recoMht = new TVector2;
      recoMht->SetX(mhtX); recoMht->SetY(mhtY);
      //add photon to MET
      TVector2* gammaMet = new TVector2;
      float gammaX = t.gamma_pt[0] * cos(t.gamma_phi[0]);
      float gammaY = t.gamma_pt[0] * sin(t.gamma_phi[0]);
      gammaMet->SetX(metX+gammaX); gammaMet->SetY(metY+gammaY);
      //add to mht
      TVector2* gammaMht = new TVector2;
      gammaMht->SetX(mhtX-gammaX); gammaMht->SetY(mhtY-gammaY);
      //met cleaning
      if (fabs(gammaMht->Mod()-gammaMet->Mod())/gammaMet->Mod() > 0.5) continue;

      
  
      fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase");
      if (fabs(t.gamma_eta[0]) < 1.479) fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase","EB");
      else if (fabs(t.gamma_eta[0]) > 1.479) fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase","EE");

      if (t.gamma_pt[0] > 200 && t.gamma_pt[0] < 350) {
	fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase","PT200");
	if (fabs(t.gamma_eta[0]) < 1.479) fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase","PT200EB");
	else if (fabs(t.gamma_eta[0]) > 1.479) fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase","PT200EE");
      }
      if (t.gamma_pt[0] > 300 && t.gamma_pt[0] < 500) {
	fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase","PT300");
	if (fabs(t.gamma_eta[0]) < 1.479) fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase","PT300EB");
	else if (fabs(t.gamma_eta[0]) > 1.479) fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase","PT300EE");
      }
       if (t.gamma_pt[0] > 500) {
	fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase","PT500");
	if (fabs(t.gamma_eta[0]) < 1.479) fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase","PT500EB");
	else if (fabs(t.gamma_eta[0]) > 1.479) fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase","PT500EE");
      }

      // if (gammaMet->Mod() > 200) {
      // 	fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase","MinMET");
      // 	if (fabs(t.gamma_eta[0]) < 1.479) fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase","MinMETEB");
      // 	else if (fabs(t.gamma_eta[0]) > 1.479) fillHistosGJet(CRGJetBase.crslHistMap,"crgjetbase","MinMETEE");
      // }
      
    }//end loop on events in a file
  
    delete tree;
    f.Close();
  }//end loop on files
  
  cout << "[GJetLooper::loop] processed " << nEventsTotal << " events" << endl;
  if ( nEventsChain != nEventsTotal ) {
    std::cout << "ERROR: number of events from files is not equal to total number of events" << std::endl;
  }

  cout << nDuplicates << " duplicate events were skipped." << endl;

  //---------------------
  // Save Plots
  //---------------------

  outfile_->cd();
  savePlotsDir(h_1d_global,outfile_,"");
  savePlotsDir(CRGJetBase.crslHistMap,outfile_,"crgjetbase");

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
void GJetLooper::fillHistosGJet(std::map<std::string, TH1*>& h_1d, const std::string& dirname, const std::string& s) {

  if (dirname.size()) {
    TDirectory * dir = (TDirectory*)outfile_->Get(dirname.c_str());
    if (dir == 0) {
      dir = outfile_->mkdir(dirname.c_str());
    } 
    dir->cd();
  } else {
    outfile_->cd();
  }
      
  //calculate additional MET variables
  float metX = t.met_pt * cos(t.met_phi);
  float metY = t.met_pt * sin(t.met_phi);
  TVector2* recoMet = new TVector2;
  recoMet->SetX(metX); recoMet->SetY(metY);

  //add photon to MET
  TVector2* gammaMet = new TVector2;
  TVector2* gammaXY = new TVector2;
  float gammaX = t.gamma_pt[0] * cos(t.gamma_phi[0]);
  float gammaY = t.gamma_pt[0] * sin(t.gamma_phi[0]);
  gammaMet->SetX(metX+gammaX); gammaMet->SetY(metY+gammaY);
  gammaXY->SetX(gammaX); gammaXY->SetY(gammaY);
  

  //smear MET "mismeasurement" by 2
  TVector2* genMet = new TVector2;
  TVector2* smearMet = new TVector2;
  //add photon to smearMET
  TVector2* gammaSmearMet = new TVector2;
  if (!t.isData) {
    float genmetX = t.met_genPt * cos(t.met_genPhi);
    float genmetY = t.met_genPt * sin(t.met_genPhi);
    genMet->SetX(genmetX); genMet->SetY(genmetY);
    float smearmetX = genmetX + 2*(metX - genmetX);
    float smearmetY = genmetY + 2*(metY - genmetY);
    smearMet->SetX(smearmetX); smearMet->SetY(smearmetY);
    gammaSmearMet->SetX(smearmetX+gammaX); gammaSmearMet->SetY(smearmetY+gammaY);
  }
  else { genMet = 0; smearMet->SetX(metX); smearMet->SetY(metY); gammaSmearMet->SetX(metX+gammaX); gammaSmearMet->SetY(metY+gammaY); }


  //parallel & perp components
  TVector2* gamma_par = new TVector2;
  gamma_par->Set(recoMet->Proj(*gammaXY));
  TVector2* gamma_perp = new TVector2;
  gamma_perp->Set(recoMet->Norm(*gammaXY));
  TVector2* gammaMet_par = new TVector2;
  TVector2* gammaSmearMet_par = new TVector2;
  gammaMet_par->Set(gammaMet->Proj(*gammaXY));
  gammaSmearMet_par->Set(gammaSmearMet->Proj(*smearMet));
  TVector2* gammaMet_perp = new TVector2;
  TVector2* gammaSmearMet_perp = new TVector2;
  gammaMet_perp->Set(gammaMet->Norm(*gammaXY));
  gammaSmearMet_perp->Set(gammaSmearMet->Norm(*smearMet));
  // TVector2* met_par = new TVector2;
  // TVector2* met_perp = new TVector2;
  // met_par->Set(gammaXY->Proj(*recoMet));
  // met_perp->Set(gammaXY->Norm(*recoMet));
  
  
  plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
  plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
  plot1D("h_nVert"+s,          t.nVert,   evtweight_, h_1d, ";N(vertices)",50,0,50);
  plot1D("h_nJet30"+s,     t.nJet30,   evtweight_, h_1d, ";N(jets)", 15, 0, 15);
  plot1D("h_nBJet20"+s,    t.nBJet20,   evtweight_, h_1d, ";N(bjets)", 6, 0, 6);

  //photon specific plots
  plot1D("h_gammaEta"+s,       t.gamma_eta[0],   evtweight_, h_1d, ";gamma #eta [GeV]", 50, -2.5, 2.5);
  plot1D("h_iso"+s,      t.gamma_phIso[0]+t.gamma_chHadIso[0],   evtweight_, h_1d, ";iso [GeV]", 100, 0, 50);
  plot1D("h_chiso"+s,      t.gamma_chHadIso[0],   evtweight_, h_1d, ";iso [GeV]", 100, 0, 50);
  plot1D("h_phiso"+s,      t.gamma_phIso[0],   evtweight_, h_1d, ";iso [GeV]", 100, 0, 50);
  plot1D("h_sieie"+s,        t.gamma_sigmaIetaIeta[0],   evtweight_, h_1d, ";gamma #sigma_{i#etai#eta}", 100, 0, 0.025);
  if (fabs(t.gamma_eta[0]) < 1.479) plot1D("h_EBsieie"+s,        t.gamma_sigmaIetaIeta[0],   evtweight_, h_1d, ";gamma #sigma_{i#etai#eta}", 100, 0, 0.015);
  else if (fabs(t.gamma_eta[0]) > 1.479) plot1D("h_EEsieie"+s,        t.gamma_sigmaIetaIeta[0],   evtweight_, h_1d, ";gamma #sigma_{i#etai#eta}", 100, 0, 0.025);
  
  plot1D("h_gammaPt"+s,   t.gamma_pt[0] ,   evtweight_, h_1d, ";#gamma p_{T} [GeV]", 100, 0, 500);
  plot1D("h_deltaPhiGammaMet"+s,  t.gamma_phi[0]-gammaMet->Phi() ,   evtweight_, h_1d, ";deltaPhiGammaMet", 100, -1, 1);
  plot1D("h_gammaMet"+s,       gammaMet->Mod(),   evtweight_, h_1d, ";gamma+MET", 100, 0, 1000);
  plot1D("h_gamma_par"+s,       gamma_par->Mod(),   evtweight_, h_1d, ";gamma+MET", 100, 0, 1000);
  plot1D("h_gamma_perp"+s,       gamma_perp->Mod(),   evtweight_, h_1d, ";gamma+MET", 300, 0, 300);
  plot1D("h_gammaMet_par"+s,       gammaMet_par->Mod(),   evtweight_, h_1d, ";gamma+MET", 100, 0, 3);
  plot1D("h_gammaMet_perp"+s,       gammaMet_perp->Mod(),   evtweight_, h_1d, ";gamma+MET", 300, 0, 300);
  plot1D("h_met"+s,       t.met_pt,   evtweight_, h_1d, ";MET", 100, 0, 1000);
  plot1D("h_met2"+s,       recoMet->Mod(),   evtweight_, h_1d, ";MET", 100, 0, 1000);
  // plot1D("h_met_par"+s,       met_par->Mod(),   evtweight_, h_1d, ";MET", 100, 0, 1000);
  // plot1D("h_met_perp"+s,       met_par->Mod(),   evtweight_, h_1d, ";MET", 100, 0, 1000);
  plot1D("h_smearMet"+s,       smearMet->Mod(),   evtweight_, h_1d, "; smearedMET", 100, 0, 1000);
  plot1D("h_gammaSmearMet"+s,       gammaSmearMet->Mod(),   evtweight_, h_1d, "; gammaSmearedMET", 100, 0, 1000);
  
  plot1D("h_gammaMetOverGamma"+s,       gammaMet->Mod()/t.gamma_pt[0],   evtweight_, h_1d, ";gamma+MET", 50, 0, 2);
  plot1D("h_gammaSmearMetOverGamma"+s,       gammaSmearMet->Mod()/t.gamma_pt[0],   evtweight_, h_1d, ";gamma+smearMET", 50, 0, 2);
  plot1D("h_gammaMetParOverGamma"+s,       gammaMet_par->Mod()/t.gamma_pt[0],   evtweight_, h_1d, ";gamma+MET", 50, 0, 2);
  plot1D("h_gammaSmearMetParOverGamma"+s,       gammaSmearMet_par->Mod()/t.gamma_pt[0],   evtweight_, h_1d, ";gamma+smearMET", 50, 0, 2);
  plot1D("h_gammaMetPerpOverGamma"+s,       gammaMet_perp->Mod()/t.gamma_pt[0],   evtweight_, h_1d, ";gamma+MET", 50, 0, 2);
  plot1D("h_gammaSmearMetPerpOverGamma"+s,       gammaSmearMet_perp->Mod()/t.gamma_pt[0],   evtweight_, h_1d, ";gamma+smearMET", 50, 0, 2);

  plot1D("h_gammaMetOverGamma2"+s,       gammaMet->Mod()/gammaXY->Mod(),   evtweight_, h_1d, ";gamma+MET", 50, 0, 2);
  plot1D("h_gammaSmearMetOverGamma2"+s,       gammaSmearMet->Mod()/gammaXY->Mod(),   evtweight_, h_1d, ";gamma+smearMET", 50, 0, 2);
  plot1D("h_gammaMetParOverGamma2"+s,       gammaMet_par->Mod()/gammaXY->Mod(),   evtweight_, h_1d, ";gamma+MET", 50, 0, 2);
  plot1D("h_gammaSmearMetParOverGamma2"+s,       gammaSmearMet_par->Mod()/gammaXY->Mod(),   evtweight_, h_1d, ";gamma+smearMET", 50, 0, 2);
  plot1D("h_gammaMetPerpOverGamma2"+s,       gammaMet_perp->Mod()/gammaXY->Mod(),   evtweight_, h_1d, ";gamma+MET", 50, 0, 2);
  plot1D("h_gammaSmearMetPerpOverGamma2"+s,       gammaSmearMet_perp->Mod()/gammaXY->Mod(),   evtweight_, h_1d, ";gamma+smearMET", 50, 0, 2);
  
  plot1D("h_gammaMetParOverGammaPar"+s,       gammaMet_par->Mod()/gamma_par->Mod(),   evtweight_, h_1d, ";gamma+MET", 50, 0, 2);
  plot1D("h_gammaSmearMetParOverGammaPar"+s,       gammaSmearMet_par->Mod()/gamma_par->Mod(),   evtweight_, h_1d, ";gamma+smearMET", 50, 0, 2);
  plot1D("h_gammaMetPerpOverGammaPerp"+s,       gammaMet_perp->Mod()/gamma_perp->Mod(),   evtweight_, h_1d, ";gamma+MET", 50, 0, 2);
  plot1D("h_gammaSmearMetPerpOverGammaPerp"+s,       gammaSmearMet_perp->Mod()/gamma_perp->Mod(),   evtweight_, h_1d, ";gamma+smearMET", 50, 0, 2);
  if (!t.isData) {
    plot1D("h_diffRecometGenmetOverGenmet"+s,       (recoMet->Mod()-genMet->Mod())/genMet->Mod(),   evtweight_, h_1d, ";diffRecometGenmetOverGenmet", 500, -50, 50);
    plot1D("h_diffRecometGenmetOverGenmet2"+s,       (t.met_pt - t.met_genPt)/t.met_pt,   evtweight_, h_1d, ";diffRecometGenmetOverGenmet", 500, -50, 50);
  }
  
  outfile_->cd();
  return;
}
