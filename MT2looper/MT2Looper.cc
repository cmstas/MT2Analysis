// C++
#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <sstream>
#include <stdexcept>

// ROOT
#include "TDirectory.h"
#include "TTreeCache.h"
#include "Math/VectorUtil.h"
#include "TVector2.h"
#include "TBenchmark.h"
#include "TMath.h"
#include "TLorentzVector.h"

#include "RooRealVar.h"
#include "RooDataSet.h"

// Tools
#include "../CORE/Tools/utils.h"
#include "../CORE/Tools/goodrun.h"
#include "../CORE/Tools/dorky/dorky.h"

// header
#include "MT2Looper.h"

//MT2
#include "../MT2CORE/Plotting/PlotUtilities.h"

using namespace std;
using namespace mt2;
using namespace duplicate_removal;
using namespace RooFit;

class mt2tree;
class SR;

std::string toString(float in){
  std::stringstream ss;
  ss << in;
  return ss.str();
}

// generic binning for signal scans - need arrays since mt2 dimension will be variable
//   assuming here: 25 GeV binning, m1 from 400-2000, m2 from 0-1600
const int n_m1bins = 65;
float m1bins[n_m1bins+1];
const int n_m2bins = 65;
float m2bins[n_m2bins+1];

const int n_htbins = 4;
const float htbins[n_htbins+1] = {450., 575., 1000., 1500., 3000.};
const int n_njbins = 3;
const float njbins[n_njbins+1] = {2, 4, 7, 12};
const int n_nbjbins = 4;
const float nbjbins[n_nbjbins+1] = {0, 1, 2, 3, 6};
const int n_ptVbins = 19;
float logstep(int i) {
  return TMath::Power(10, 2+4.5e-02*i);
}
const float ptVbins[n_ptVbins+1] = {100, logstep(1), logstep(2), logstep(3), logstep(4), logstep(5), logstep(6), logstep(7), logstep(8), logstep(9), 
				    logstep(10), logstep(11), logstep(12), logstep(13), logstep(14), logstep(15), logstep(16), logstep(17), 800, 1200};

//RooRealVars for unbinned data hist
RooRealVar* x_ = new RooRealVar( "x", "", 0., 50.);
RooRealVar* w_ = new RooRealVar( "w", "", 0., 1000.);

bool useDRforGammaQCDMixing = true; // requires GenParticles
// turn on to apply weights to central value
bool applyWeights = false;
// turn on to enable plots of MT2 with systematic variations applied. applyWeights should be true
bool doSystVariationPlots = false;
// turn on to apply Nvtx reweighting to MC
bool doNvtxReweight = false;
// turn on to apply json file to data
bool applyJSON = false;
// veto on jets with pt > 30, |eta| > 3.0
bool doHFJetVeto = false;

MT2Looper::MT2Looper(){

  // set up signal binning
  for (int i = 0; i <= n_m1bins; ++i) {
    m1bins[i] = 387.5 + i*25.;
  }
  for (int i = 0; i <= n_m2bins; ++i) {
    m2bins[i] = -12.5 + i*25.;
  }

}
MT2Looper::~MT2Looper(){

};

void MT2Looper::SetSignalRegions(){

  //SRVec = getSignalRegions2015LowLumi(); //Phys14 AN selection
  //SRVec =  getSignalRegions2015SevenJets_UltraHighHT(); //new selection with additional njet boundary at 7 jets, 4th ht bin
  //SRVec =  getSignalRegionsZurich(); //same as getSignalRegions2015SevenJets_UltraHighHT(), but with minMT binning removed
  //SRVec =  getSignalRegionsZurich_jetpt40(); //same as getSignalRegionsZurich(), but with j1pt and j2pt cuts changed to 40 GeV
  SRVec =  getSignalRegionsZurich_jetpt30(); //same as getSignalRegionsZurich(), but with j1pt and j2pt cuts changed to 30 GeV

  //store histograms with cut values for all variables
  for(unsigned int i = 0; i < SRVec.size(); i++){
    std::vector<std::string> vars = SRVec.at(i).GetListOfVariables();
    TDirectory * dir = (TDirectory*)outfile_->Get(("sr"+SRVec.at(i).GetName()).c_str());
    if (dir == 0) {
      dir = outfile_->mkdir(("sr"+SRVec.at(i).GetName()).c_str());
    } 
    dir->cd();
    for(unsigned int j = 0; j < vars.size(); j++){
      plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRVec.at(i).GetLowerBound(vars.at(j)), SRVec.at(i).srHistMap, "", 1, 0, 2);
      plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRVec.at(i).GetUpperBound(vars.at(j)), SRVec.at(i).srHistMap, "", 1, 0, 2);
    }
    plot1D("h_n_mt2bins",  1, SRVec.at(i).GetNumberOfMT2Bins(), SRVec.at(i).srHistMap, "", 1, 0, 2);

    dir = (TDirectory*)outfile_->Get(("crsl"+SRVec.at(i).GetName()).c_str());
    if (dir == 0) {
      dir = outfile_->mkdir(("crsl"+SRVec.at(i).GetName()).c_str());
    } 
    dir->cd();
    std::vector<std::string> varsCRSL = SRVec.at(i).GetListOfVariablesCRSL();
    for(unsigned int j = 0; j < varsCRSL.size(); j++){
      plot1D("h_"+varsCRSL.at(j)+"_"+"LOW",  1, SRVec.at(i).GetLowerBoundCRSL(varsCRSL.at(j)), SRVec.at(i).crslHistMap, "", 1, 0, 2);
      plot1D("h_"+varsCRSL.at(j)+"_"+"HI",   1, SRVec.at(i).GetUpperBoundCRSL(varsCRSL.at(j)), SRVec.at(i).crslHistMap, "", 1, 0, 2);
    }
    plot1D("h_n_mt2bins",  1, SRVec.at(i).GetNumberOfMT2Bins(), SRVec.at(i).crslHistMap, "", 1, 0, 2);

    dir = (TDirectory*)outfile_->Get(("crgj"+SRVec.at(i).GetName()).c_str());
    if (dir == 0) {
      dir = outfile_->mkdir(("crgj"+SRVec.at(i).GetName()).c_str());
    } 
    dir->cd();
    for(unsigned int j = 0; j < vars.size(); j++){
      plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRVec.at(i).GetLowerBound(vars.at(j)), SRVec.at(i).crgjHistMap, "", 1, 0, 2);
      plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRVec.at(i).GetUpperBound(vars.at(j)), SRVec.at(i).crgjHistMap, "", 1, 0, 2);
    }
    plot1D("h_n_mt2bins",  1, SRVec.at(i).GetNumberOfMT2Bins(), SRVec.at(i).crgjHistMap, "", 1, 0, 2);
    outfile_->cd();
  }

  SRBase.SetName("srbase");
  SRBase.SetVar("mt2", 200, -1);
  SRBase.SetVar("j1pt", 30, -1);
  SRBase.SetVar("j2pt", 30, -1);
  SRBase.SetVar("deltaPhiMin", 0.3, -1);
  SRBase.SetVar("diffMetMhtOverMet", 0, 0.5);
  SRBase.SetVar("nlep", 0, 1);
  SRBase.SetVar("passesHtMet", 1, 2);
  SRBase.SetVarCRSL("mt2", 200, -1);
  SRBase.SetVarCRSL("j1pt", 30, -1);
  SRBase.SetVarCRSL("j2pt", 30, -1);
  SRBase.SetVarCRSL("deltaPhiMin", 0.3, -1);
  SRBase.SetVarCRSL("diffMetMhtOverMet", 0, 0.5);
  SRBase.SetVarCRSL("nlep", 1, 2);
  SRBase.SetVarCRSL("passesHtMet", 1, 2);
  float SRBase_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500}; 
  SRBase.SetMT2Bins(7, SRBase_mt2bins);

  std::vector<std::string> vars = SRBase.GetListOfVariables();
  TDirectory * dir = (TDirectory*)outfile_->Get((SRBase.GetName()).c_str());
  if (dir == 0) {
    dir = outfile_->mkdir((SRBase.GetName()).c_str());
  } 
  dir->cd();
  for(unsigned int j = 0; j < vars.size(); j++){
    plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRBase.GetLowerBound(vars.at(j)), SRBase.srHistMap, "", 1, 0, 2);
    plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRBase.GetUpperBound(vars.at(j)), SRBase.srHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBase.GetNumberOfMT2Bins(), SRBase.srHistMap, "", 1, 0, 2);
  outfile_->cd();

  std::vector<std::string> varsCRSL = SRBase.GetListOfVariablesCRSL();
  TDirectory * dirCRSL = (TDirectory*)outfile_->Get("crslbase");
  if (dirCRSL == 0) {
    dirCRSL = outfile_->mkdir("crslbase");
  } 
  dirCRSL->cd();
  for(unsigned int j = 0; j < varsCRSL.size(); j++){
    plot1D("h_"+varsCRSL.at(j)+"_"+"LOW",  1, SRBase.GetLowerBoundCRSL(varsCRSL.at(j)), SRBase.crslHistMap, "", 1, 0, 2);
    plot1D("h_"+varsCRSL.at(j)+"_"+"HI",   1, SRBase.GetUpperBoundCRSL(varsCRSL.at(j)), SRBase.crslHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBase.GetNumberOfMT2Bins(), SRBase.srHistMap, "", 1, 0, 2);
  outfile_->cd();

  //setup inclusive regions
  SR InclusiveHT450to575 = SRBase;
  InclusiveHT450to575.SetName("InclusiveHT450to575");
  InclusiveHT450to575.SetVar("ht", 450, 575);
  InclusiveRegions.push_back(InclusiveHT450to575);

  SR InclusiveHT575to1000 = SRBase;
  InclusiveHT575to1000.SetName("InclusiveHT575to1000");
  InclusiveHT575to1000.SetVar("ht", 575, 1000);
  InclusiveRegions.push_back(InclusiveHT575to1000);

  SR InclusiveHT1000to1500 = SRBase;
  InclusiveHT1000to1500.SetName("InclusiveHT1000to1500");
  InclusiveHT1000to1500.SetVar("ht", 1000, 1500);
  InclusiveRegions.push_back(InclusiveHT1000to1500);

  SR InclusiveHT1500toInf = SRBase;
  InclusiveHT1500toInf.SetName("InclusiveHT1500toInf");
  InclusiveHT1500toInf.SetVar("ht", 1500, -1);
  InclusiveRegions.push_back(InclusiveHT1500toInf);

  SR InclusiveNJets2to3 = SRBase;
  InclusiveNJets2to3.SetName("InclusiveNJets2to3");
  InclusiveNJets2to3.SetVar("njets", 2, 4);
  InclusiveRegions.push_back(InclusiveNJets2to3);

  SR InclusiveNJets4to6 = SRBase;
  InclusiveNJets4to6.SetName("InclusiveNJets4to6");
  InclusiveNJets4to6.SetVar("njets", 4, 7);
  InclusiveRegions.push_back(InclusiveNJets4to6);

  SR InclusiveNJets7toInf = SRBase;
  InclusiveNJets7toInf.SetName("InclusiveNJets7toInf");
  InclusiveNJets7toInf.SetVar("njets", 7, -1);
  InclusiveRegions.push_back(InclusiveNJets7toInf);

  SR InclusiveNBJets0 = SRBase;
  InclusiveNBJets0.SetName("InclusiveNBJets0");
  InclusiveNBJets0.SetVar("nbjets", 0, 1);
  InclusiveRegions.push_back(InclusiveNBJets0);

  SR InclusiveNBJets1 = SRBase;
  InclusiveNBJets1.SetName("InclusiveNBJets1");
  InclusiveNBJets1.SetVar("nbjets", 1, 2);
  InclusiveRegions.push_back(InclusiveNBJets1);

  SR InclusiveNBJets2 = SRBase;
  InclusiveNBJets2.SetName("InclusiveNBJets2");
  InclusiveNBJets2.SetVar("nbjets", 2, 3);
  InclusiveRegions.push_back(InclusiveNBJets2);

  SR InclusiveNBJets3toInf = SRBase;
  InclusiveNBJets3toInf.SetName("InclusiveNBJets3toInf");
  InclusiveNBJets3toInf.SetVar("nbjets", 3, -1);
  InclusiveRegions.push_back(InclusiveNBJets3toInf);

  for(unsigned int i=0; i<InclusiveRegions.size(); i++){
    TDirectory * dir = (TDirectory*)outfile_->Get((InclusiveRegions.at(i).GetName()).c_str());
    if (dir == 0) {
      dir = outfile_->mkdir((InclusiveRegions.at(i).GetName()).c_str());
    } 
  }

  // CRSL inclusive regions to isolate W+jets and ttbar
  CRSL_WJets = SRBase;
  CRSL_WJets.SetName("crslwjets");
  CRSL_WJets.SetVarCRSL("nbjets", 0, 1);
  CRSL_WJets.crslHistMap.clear();

  CRSL_TTbar = SRBase;
  CRSL_TTbar.SetName("crslttbar");
  CRSL_TTbar.SetVarCRSL("nbjets", 2, -1);
  CRSL_TTbar.crslHistMap.clear();

}


void MT2Looper::loop(TChain* chain, std::string output_name){

  // Benchmark
  TBenchmark *bmark = new TBenchmark();
  bmark->Start("benchmark");

  gROOT->cd();
  cout << "[MT2Looper::loop] creating output file: " << output_name << endl;

  outfile_ = new TFile(output_name.c_str(),"RECREATE") ; 

  const char* json_file = "../babymaker/jsons/Cert_246908-251883_13TeV_PromptReco_Collisions15_JSON_v2_snt.txt";
  if (applyJSON) {
    cout << "Loading json file: " << json_file << endl;
    set_goodrun_file(json_file);
  }

  h_nvtx_weights_ = 0;
  if (doNvtxReweight) {
    TFile* f_weights = new TFile("../babymaker/data/hists_reweight_zjets_Run2015B.root");
    TH1D* h_nvtx_weights_temp = (TH1D*) f_weights->Get("h_nVert_ratio");
    outfile_->cd();
    h_nvtx_weights_ = (TH1D*) h_nvtx_weights_temp->Clone("h_nvtx_weights");
    f_weights->Close();
  }
  
  cout << "[MT2Looper::loop] setting up histos" << endl;

  SetSignalRegions();

  SRNoCut.SetName("nocut");
  float SRNoCut_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500}; 
  SRNoCut.SetMT2Bins(7, SRNoCut_mt2bins);

  // These will be set to true if any SL GJ or DY control region plots are produced
  bool saveGJplots = false;
  bool saveDYplots = false;
  bool saveRLplots = false;
  bool saveRLELplots = false;
  bool saveRLMUplots = false;
  bool saveSLplots = false;
  bool saveSLMUplots = false;
  bool saveSLELplots = false;

  // File Loop
  int nDuplicates = 0;
  int nEvents = chain->GetEntries();
  unsigned int nEventsChain = nEvents;
  cout << "[MT2Looper::loop] running on " << nEventsChain << " events" << endl;
  unsigned int nEventsTotal = 0;
  TObjArray *listOfFiles = chain->GetListOfFiles();
  TIter fileIter(listOfFiles);
  TFile *currentFile = 0;
  while ( (currentFile = (TFile*)fileIter.Next()) ) {
    cout << "[MT2Looper::loop] running on file: " << currentFile->GetTitle() << endl;

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
    //for( unsigned int event = 0; event < 100; ++event) {
      
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
      
      if (t.nVert == 0) continue;

      // MET filters (data and MC)
      if (!t.Flag_goodVertices) continue;
      if (!t.Flag_CSCTightHaloFilter) continue;
      if (!t.Flag_eeBadScFilter) continue;
      if (!t.Flag_HBHENoiseFilter) continue;
      bool passJetID = true;
      //if (t.nJet30FailId > 0) continue;
      if (t.nJet30FailId > 0) passJetID = false;

      // remove low pt QCD samples 
      if (t.evt_id >= 100 && t.evt_id < 109) continue;
      // remove low HT QCD samples 
      if (t.evt_id >= 120 && t.evt_id < 123) continue;
      if (t.evt_id >= 151 && t.evt_id < 154) continue;

      // flag signal samples
      if (t.evt_id >= 1000) isSignal_ = true;
      else isSignal_ = false; 

      //---------------------
      // set weights and start making plots
      //---------------------
      outfile_->cd();
      const float lumi = 3.;
      //      const float lumi = 0.042;
      //      const float lumi = 0.107;
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

      plot1D("h_nvtx",       t.nVert,       evtweight_, h_1d_global, ";N(vtx)", 80, 0, 80);
      plot1D("h_mt2",       t.mt2,       evtweight_, h_1d_global, ";M_{T2} [GeV]", 80, 0, 800);

      // note: this will double count some leptons, since reco leptons can appear as PFcands
      nlepveto_ = t.nMuons10 + t.nElectrons10 + t.nPFLep5LowMT + t.nPFHad10LowMT;

      // variables for single lep control region
      bool doSLplots = false;
      bool doSLMUplots = false;
      bool doSLELplots = false;
      leppt_ = -1.;
      mt_ = -1.;

      // count number of forward jets
      nJet30Eta3_ = 0;
      for (int ijet = 0; ijet < t.njet; ++ijet) {
	if (t.jet_pt[ijet] > 30. && fabs(t.jet_eta[ijet]) > 3.0) ++nJet30Eta3_;
      }

      // veto on forward jets
      if (doHFJetVeto && nJet30Eta3_ > 0) continue;

      // simple counter to check for 1L CR
      if (t.nLepLowMT == 1) {
	doSLplots = true;

	// find unique lepton to plot pt,MT and get flavor
	bool foundlep = false;
	int cand_pdgId = 0;

	// if reco leps, check those
	if (t.nlep > 0) {
      	  for (int ilep = 0; ilep < t.nlep; ++ilep) {
      	    float mt = sqrt( 2 * t.met_pt * t.lep_pt[ilep] * ( 1 - cos( t.met_phi - t.lep_phi[ilep]) ) );
	    if (mt > 100.) continue;

	    // good candidate: save
	    leppt_ = t.lep_pt[ilep];
	    mt_ = mt;
	    cand_pdgId = t.lep_pdgId[ilep];
	    foundlep = true;
	    break;
	  }
	} // t.nlep > 0

	// otherwise check PF leps that don't overlap with a reco lepton
	if (!foundlep && t.nPFLep5LowMT > 0) {
      	  for (int itrk = 0; itrk < t.nisoTrack; ++itrk) {
	    float pt = t.isoTrack_pt[itrk];
	    if (pt < 5.) continue;
      	    int pdgId = t.isoTrack_pdgId[itrk];
	    if ((abs(pdgId) != 11) && (abs(pdgId) != 13)) continue;
      	    if (t.isoTrack_absIso[itrk]/pt > 0.2) continue;
      	    float mt = sqrt( 2 * t.met_pt * pt * ( 1 - cos( t.met_phi - t.isoTrack_phi[itrk]) ) );
	    if (mt > 100.) continue;

	    // check overlap with reco leptons
	    bool overlap = false;
	    for(int ilep = 0; ilep < t.nlep; ilep++){
	      float thisDR = DeltaR(t.isoTrack_eta[itrk], t.lep_eta[ilep], t.isoTrack_phi[itrk], t.lep_phi[ilep]);
	      if (thisDR < 0.01) {
		overlap = true;
		break;
	      }
	    } // loop over reco leps
	    if (overlap) continue;

	    // good candidate: save
	    leppt_ = pt;
	    mt_ = mt;
	    cand_pdgId = pdgId;
	    foundlep = true;
	    break;
	  } // loop on isotracks
	}

	if (!foundlep) {
	  std::cout << "MT2Looper::Loop: WARNING! didn't find a lowMT candidate when expected: evt: " << t.evt
		    << ", nMuons10: " << t.nMuons10 << ", nElectrons10: " << t.nElectrons10 
		    << ", nPFLep5LowMT: " << t.nPFLep5LowMT << ", nLepLowMT: " << t.nLepLowMT << std::endl;
	}

	if (abs(cand_pdgId) == 11) doSLELplots = true;
	else if (abs(cand_pdgId) == 13) doSLMUplots = true;

      } // for 1L control region

      // Variables for gamma+jets control region
      bool doGJplots = false;
      if (t.evt_id < 300 && t.evt_id!=120) { // remove lowest qcd_ht sample
      	if (t.ngamma > 0) {
//	  if (	(t.evt_id ==105 && t.gamma_pt[0]>50      )   //reject photons in qcd_ptAAAtoBBB samples with pt>BBB
//		|| (t.evt_id ==106 && t.gamma_pt[0]>80   )   //reject photons in qcd_ptAAAtoBBB samples with pt>BBB
//		|| (t.evt_id ==107 && t.gamma_pt[0]>120  )   //reject photons in qcd_ptAAAtoBBB samples with pt>BBB
//		|| (t.evt_id ==108 && t.gamma_pt[0]>170  )   //reject photons in qcd_ptAAAtoBBB samples with pt>BBB
//		|| (t.evt_id ==109 && t.gamma_pt[0]>300  )   //reject photons in qcd_ptAAAtoBBB samples with pt>BBB
//		|| (t.evt_id ==110 && t.gamma_pt[0]>470  )   //reject photons in qcd_ptAAAtoBBB samples with pt>BBB
//		|| (t.evt_id ==111 && t.gamma_pt[0]>600  )   //reject photons in qcd_ptAAAtoBBB samples with pt>BBB
//		|| (t.evt_id ==112 && t.gamma_pt[0]>800  )   //reject photons in qcd_ptAAAtoBBB samples with pt>BBB
//		|| (t.evt_id ==113 && t.gamma_pt[0]>1000 )   //reject photons in qcd_ptAAAtoBBB samples with pt>BBB
//		|| (t.evt_id ==114 && t.gamma_pt[0]>1400 )   //reject photons in qcd_ptAAAtoBBB samples with pt>BBB
//		|| (t.evt_id ==115 && t.gamma_pt[0]>1800 )   //reject photons in qcd_ptAAAtoBBB samples with pt>BBB
//		|| (t.evt_id ==116 && t.gamma_pt[0]>2400 )   //reject photons in qcd_ptAAAtoBBB samples with pt>BBB
//		|| (t.evt_id ==117 && t.gamma_pt[0]>3200 )   //reject photons in qcd_ptAAAtoBBB samples with pt>BBB
//		) 
	  if (t.evt_id < 109 && t.evt_id != -1) // Simply start at pt170to300, ignore earlier samples
	    { doGJplots = false; }
	  else if (!useDRforGammaQCDMixing) {
	    if ( (t.evt_id < 200 && t.gamma_mcMatchId[0]>0  && t.gamma_genIso04[0]<5)    // Reject true photons from QCD (iso is always 0 for now)
		|| (t.evt_id >=200 && t.gamma_mcMatchId[0]==0 )                           // Reject unmatched photons from Gamma+Jets
		|| (t.evt_id >=200 && t.gamma_mcMatchId[0] >0 && t.gamma_genIso04[0]>5)    // Reject non-iso photons from Gamma+Jets
		 )
	      { doGJplots = false; }
	    else {
	      doGJplots = true;
	    }	    
	  }
	  else {//useDRforGammaQCDMixing
	    //float minDR = 3;
	    //float minDRtrue = 3;
	    //float photonEta=0;
	    //float photonPhi=0;
	    //for (int i = 0; i < t.ngenPart; i++) {
	    //  if (t.genPart_status[i]!=22 &&  t.genPart_status[i]!=23) continue;
	    //  if (fabs(t.genPart_pdgId[i])!=22) continue;
	    //  photonEta = t.genPart_eta[i];
	    //  photonPhi = t.genPart_phi[i];
	    //}
	    //for (int i = 0; i < t.ngenPart; i++) {
	    //  if (t.genPart_status[i]!=22 &&  t.genPart_status[i]!=23) continue;
	    //  if (fabs(t.genPart_pdgId[i])>21) continue;
	    //  float dr = DeltaR(t.genPart_eta[i],  t.gamma_eta[0], t.genPart_phi[i],  t.gamma_phi[0]);
	    //  if (dr < minDR) minDR = dr;
	    //  if (photonEta!=0 && photonPhi!=0) {
	    //	float drtrue = DeltaR(t.genPart_eta[i], photonEta, t.genPart_phi[i], photonPhi);
	    //	if (drtrue < minDRtrue) minDRtrue = drtrue;
	    //  }
	    //}
	    if ( (t.evt_id < 200 && t.gamma_mcMatchId[0]>0 && t.gamma_drMinParton[0] > 0.4)   // reject DR>0.4 photons from QCD
		 || (t.evt_id >=200 && t.gamma_mcMatchId[0]==0 )   // Reject unmatched photons from Gamma+Jets (should be small)    
		 || (t.evt_id >=200 && t.gamma_drMinParton[0]<0.4 )   // Reject fragmentation photons from Gamma+Jets (only there in samples with DR>0.05)
		 )
	      { doGJplots = false; }
	    else {
	      doGJplots = true;
	    }
	  } //useDRforGammaQCDMixing
      	} // ngamma > 0
      }// evt_id < 300

      // Variables for Zll (DY) control region
      bool doDYplots = false;
      if (t.nlep == 2 && !isSignal_) {
	if ( (t.lep_charge[0] * t.lep_charge[1] == -1)
	     && (abs(t.lep_pdgId[0]) == abs(t.lep_pdgId[1]) )
	     && (fabs(t.zll_mass - 90) < 10 ) 
	     && t.lep_pt[0] > 25 && t.lep_pt[1] > 20
	     && (t.HLT_DoubleEl || t.HLT_DoubleMu)
	     ) {
	  // no additional explicit lepton veto
	  // i.e. implicitly allow 3rd PF lepton or hadron
	  //nlepveto_ = 0; 
	  doDYplots = true;
	}
      } // nlep == 2
      
      // Variables for Removed single lepton (RL) region
      //muon only
      bool doRLplots = false;
      bool doRLMUplots = false;
      bool doRLELplots = false;
      if ( t.nlep == 1 && !isSignal_) {
	if ( t.lep_pt[0] > 30 && fabs(t.lep_eta[0])<2.5 && t.nBJet20 == 0) { // raise threshold to avoid Ele23 in MC
	  if (abs(t.lep_pdgId[0])==13) { // muons
	    if (t.HLT_SingleMu)  doRLMUplots = true;
	  }
	  if (abs(t.lep_pdgId[0])==11) { // electrons
	    if ( (t.HLT_SingleEl )   // Ele23 trigger not present in MC. Need to keep lepton threshold high
		 && t.lep_relIso03[0]<0.1 // tighter selection for electrons
		 && t.lep_relIso03[0]*t.lep_pt[0]<5 // tighter selection for electrons
		 && t.lep_tightId[0]>2
		 ) 
	      doRLELplots = true;
	  }
	  // no additional explicit lepton veto
	  // i.e. implicitly allow 2nd PF lepton or hadron
	  //nlepveto_ = 0; 
	  if (doRLMUplots || doRLELplots )  doRLplots = true;
	}
      } // nlep == 1
      

      ////////////////////////////////////
      /// done with overall selection  /// 
      ////////////////////////////////////
      ///   time to fill histograms    /// 
      ////////////////////////////////////

      if (doGJplots) {
        saveGJplots = true;
	if (t.gamma_nJet30FailId == 0) {

	  //// To test Madgraph fragmentation (need to remove drMinParton requirement from above) //
	  //if ( t.gamma_mcMatchId[0] > 0 ) {
	  //  if (t.evt_id < 200 || t.gamma_drMinParton[0]>0.4) fillHistosCRGJ("crgj"); // Prompt photon
	  //  if (t.evt_id >=200 && t.gamma_drMinParton[0]<0.4)  fillHistosCRGJ("crgj", "FragGJ");
	  //  if (t.evt_id < 200 && t.gamma_drMinParton[0]<0.05) fillHistosCRGJ("crgj", "FragGJ");
	  //}
	  //// End of Madgraph fragmentation tests //
	  
	  if ( t.gamma_mcMatchId[0] > 0 || t.isData) fillHistosCRGJ("crgj"); // Prompt photon 
	  else fillHistosCRGJ("crgj", "Fake");
	}
      }
      
      if (!passJetID) continue;

      fillHistos(SRNoCut.srHistMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), "");

      fillHistosSignalRegion("sr");

      fillHistosSRBase();
      fillHistosInclusive();

      if (doDYplots) {
        saveDYplots = true;
        fillHistosCRDY("crdy");
      }
      if (doRLplots) {
        saveRLplots = true;
        fillHistosCRRL("crrl");
      }
      if (doRLELplots) {
        saveRLELplots = true;
        fillHistosCRRL("crrlel");
      }
      if (doRLMUplots) {
        saveRLMUplots = true;
        fillHistosCRRL("crrlmu");
      }
      if (doSLplots) {
        saveSLplots = true;
        fillHistosCRSL("crsl");
      }
      if (doSLMUplots) {
        fillHistosCRSL("crslmu");
      }
      if (doSLELplots) {
        fillHistosCRSL("crslel");
      }


   }//end loop on events in a file
  
    delete tree;
    f.Close();
  }//end loop on files
  
  cout << "[MT2Looper::loop] processed " << nEventsTotal << " events" << endl;
  if ( nEventsChain != nEventsTotal ) {
    std::cout << "ERROR: number of events from files is not equal to total number of events" << std::endl;
  }

  cout << nDuplicates << " duplicate events were skipped." << endl;

  //---------------------
  // Save Plots
  //---------------------

  outfile_->cd();
  savePlotsDir(h_1d_global,outfile_,"");
  savePlotsDir(SRNoCut.srHistMap,outfile_,SRNoCut.GetName().c_str());
  savePlotsDir(SRBase.srHistMap,outfile_,SRBase.GetName().c_str());
  savePlotsDir(SRBase.crslHistMap,outfile_,"crslbase");
  savePlotsDir(SRBase.crrlHistMap,outfile_,"crrlbase");
  savePlotsDir(CRSL_WJets.crslHistMap,outfile_,CRSL_WJets.GetName().c_str());
  savePlotsDir(CRSL_TTbar.crslHistMap,outfile_,CRSL_TTbar.GetName().c_str());
  savePlotsDir(SRNoCut.crgjHistMap,outfile_,"crgjnocut");
  savePlotsDir(SRBase.crgjHistMap,outfile_,"crgjbase");

  for(unsigned int srN = 0; srN < SRVec.size(); srN++){
    if(!SRVec.at(srN).srHistMap.empty()){
      savePlotsDir(SRVec.at(srN).srHistMap, outfile_, ("sr"+SRVec.at(srN).GetName()).c_str());
    }
  }
  if (saveGJplots) {
    for(unsigned int srN = 0; srN < SRVec.size(); srN++){
      if(!SRVec.at(srN).crgjHistMap.empty()){
        savePlotsDir(SRVec.at(srN).crgjHistMap, outfile_, ("crgj"+SRVec.at(srN).GetName()).c_str());
	saveRooDataSetsDir(SRVec.at(srN).crgjRooDataSetMap, outfile_,  ("crgj"+SRVec.at(srN).GetName()).c_str());
      }
    }
  }
  if (saveDYplots) {
    for(unsigned int srN = 0; srN < SRVec.size(); srN++){
      if(!SRVec.at(srN).crdyHistMap.empty()){
        savePlotsDir(SRVec.at(srN).crdyHistMap, outfile_, ("crdy"+SRVec.at(srN).GetName()).c_str());
      }
    }
  }
  if (saveRLplots) {
    for(unsigned int srN = 0; srN < SRVec.size(); srN++){
      if(!SRVec.at(srN).crrlHistMap.empty()){
        savePlotsDir(SRVec.at(srN).crrlHistMap, outfile_, ("crrl"+SRVec.at(srN).GetName()).c_str());
      }
    }
  }
  if (saveRLELplots) {
    for(unsigned int srN = 0; srN < SRVec.size(); srN++){
      if(!SRVec.at(srN).crrlelHistMap.empty()){
        savePlotsDir(SRVec.at(srN).crrlelHistMap, outfile_, ("crrlel"+SRVec.at(srN).GetName()).c_str());
      }
    }
  }
  if (saveRLMUplots) {
    for(unsigned int srN = 0; srN < SRVec.size(); srN++){
      if(!SRVec.at(srN).crrlmuHistMap.empty()){
        savePlotsDir(SRVec.at(srN).crrlmuHistMap, outfile_, ("crrlmu"+SRVec.at(srN).GetName()).c_str());
      }
    }
  }
  if (saveSLplots) {
    for(unsigned int srN = 0; srN < SRVec.size(); srN++){
      if(!SRVec.at(srN).crslHistMap.empty()){
        savePlotsDir(SRVec.at(srN).crslHistMap, outfile_, ("crsl"+SRVec.at(srN).GetName()).c_str());
      }
    }
  }
  if (saveSLMUplots) {
    for(unsigned int srN = 0; srN < SRVec.size(); srN++){
      if(!SRVec.at(srN).crslmuHistMap.empty()){
        savePlotsDir(SRVec.at(srN).crslmuHistMap, outfile_, ("crslmu"+SRVec.at(srN).GetName()).c_str());
      }
    }
  }
  if (saveSLELplots) {
    for(unsigned int srN = 0; srN < SRVec.size(); srN++){
      if(!SRVec.at(srN).crslelHistMap.empty()){
        savePlotsDir(SRVec.at(srN).crslelHistMap, outfile_, ("crslel"+SRVec.at(srN).GetName()).c_str());
      }
    }
  }


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

void MT2Looper::fillHistosSRBase() {

  // trigger requirement on data
  if (t.isData && !(t.HLT_PFHT800 || t.HLT_PFHT350_PFMET100)) return;
  
  std::map<std::string, float> values;
  values["deltaPhiMin"] = t.deltaPhiMin;
  values["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
  values["nlep"]        = nlepveto_;
  values["j1pt"]        = t.jet1_pt;
  values["j2pt"]        = t.jet2_pt;
  values["mt2"]         = t.mt2;
  values["passesHtMet"] = ( (t.ht > 450. && t.met_pt > 200.) || (t.ht > 1000. && t.met_pt > 30.) );

  if(SRBase.PassesSelection(values)) fillHistos(SRBase.srHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), SRBase.GetName(), "");

  return;
}

void MT2Looper::fillHistosInclusive() {

  // trigger requirement on data
  if (t.isData && !(t.HLT_PFHT800 || t.HLT_PFHT350_PFMET100)) return;
  
  std::map<std::string, float> values;
  values["deltaPhiMin"] = t.deltaPhiMin;
  values["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
  values["nlep"]        = nlepveto_;
  values["j1pt"]        = t.jet1_pt;
  values["j2pt"]        = t.jet2_pt;
  values["mt2"]         = t.mt2;
  values["passesHtMet"] = ( (t.ht > 450. && t.met_pt > 200.) || (t.ht > 1000. && t.met_pt > 30.) );

  for(unsigned int srN = 0; srN < InclusiveRegions.size(); srN++){
    std::map<std::string, float> values_temp = values;
    std::vector<std::string> vars = InclusiveRegions.at(srN).GetListOfVariables();
    for(unsigned int iVar=0; iVar<vars.size(); iVar++){
      if(vars.at(iVar) == "ht") values_temp["ht"] = t.ht;
      else if(vars.at(iVar) == "njets") values_temp["njets"] = t.nJet30;
      else if(vars.at(iVar) == "nbjets") values_temp["nbjets"] = t.nBJet20;
    }
    if(InclusiveRegions.at(srN).PassesSelection(values_temp)){
      fillHistos(InclusiveRegions.at(srN).srHistMap, InclusiveRegions.at(srN).GetNumberOfMT2Bins(), InclusiveRegions.at(srN).GetMT2Bins(), InclusiveRegions.at(srN).GetName(), "");
    }
  }

  return;
}

void MT2Looper::fillHistosSignalRegion(const std::string& prefix, const std::string& suffix) {

  // trigger requirement on data
  if (t.isData && !(t.HLT_PFHT800 || t.HLT_PFHT350_PFMET100)) return;
  
  std::map<std::string, float> values;
  values["deltaPhiMin"] = t.deltaPhiMin;
  values["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
  values["nlep"]        = nlepveto_;
  values["j1pt"]        = t.jet1_pt;
  values["j2pt"]        = t.jet2_pt;
  values["njets"]       = t.nJet30;
  values["nbjets"]      = t.nBJet20;
  values["mt2"]         = t.mt2;
  values["ht"]          = t.ht;
  values["met"]         = t.met_pt;
  //values["passesHtMet"] = ( (t.ht > 450. && t.met_pt > 200.) || (t.ht > 1000. && t.met_pt > 30.) );


  for(unsigned int srN = 0; srN < SRVec.size(); srN++){
    if(SRVec.at(srN).PassesSelection(values)){
      fillHistos(SRVec.at(srN).srHistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix);
      break;//signal regions are orthogonal, event cannot be in more than one
    }
  }

  //plot1D("h_SignalRegion",  sr_jets+sr_htmet,   evtweight_, h_1d_global, ";Signal Region", 100, 0, 100);

  return;
}

// hists for single lepton control region
void MT2Looper::fillHistosCRSL(const std::string& prefix, const std::string& suffix) {

  // trigger requirement on data
  if (t.isData && !(t.HLT_PFHT800 || t.HLT_PFHT350_PFMET100)) return;
  
  // first fill base region
  std::map<std::string, float> valuesBase;
  valuesBase["deltaPhiMin"] = t.deltaPhiMin;
  valuesBase["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
  valuesBase["nlep"]        = t.nLepLowMT;
  valuesBase["j1pt"]        = t.jet1_pt;
  valuesBase["j2pt"]        = t.jet2_pt;
  valuesBase["mt2"]         = t.mt2;
  valuesBase["passesHtMet"] = ( (t.ht > 450. && t.met_pt > 200.) || (t.ht > 1000. && t.met_pt > 30.) );

  if (SRBase.PassesSelectionCRSL(valuesBase)) {
    if(prefix=="crsl") fillHistosSingleLepton(SRBase.crslHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crslbase", suffix);
    else if(prefix=="crslmu") fillHistosSingleLepton(SRBase.crslmuHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crslmubase", suffix);
    else if(prefix=="crslel") fillHistosSingleLepton(SRBase.crslelHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crslelbase", suffix);
  }

  // only fill wjets/ttbar histograms for inclusive lepton case
  if(prefix=="crsl") {

    // inclusive regions with btag cuts for wjets/ttbar
    std::map<std::string, float> valuesInc;
    valuesInc["deltaPhiMin"] = t.deltaPhiMin;
    valuesInc["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
    valuesInc["nlep"]        = t.nLepLowMT;
    valuesInc["j1pt"]        = t.jet1_pt;
    valuesInc["j2pt"]        = t.jet2_pt;
    valuesInc["mt2"]         = t.mt2;
    valuesInc["passesHtMet"] = ( (t.ht > 450. && t.met_pt > 200.) || (t.ht > 1000. && t.met_pt > 30.) );
    valuesInc["nbjets"]         = t.nBJet20;
    if (CRSL_WJets.PassesSelectionCRSL(valuesInc)) {
      fillHistosSingleLepton(CRSL_WJets.crslHistMap, CRSL_WJets.GetNumberOfMT2Bins(), CRSL_WJets.GetMT2Bins(), CRSL_WJets.GetName().c_str(), suffix);
    }
    if (CRSL_TTbar.PassesSelectionCRSL(valuesInc)) {
      fillHistosSingleLepton(CRSL_TTbar.crslHistMap, CRSL_TTbar.GetNumberOfMT2Bins(), CRSL_TTbar.GetMT2Bins(), CRSL_TTbar.GetName().c_str(), suffix);
    }
  }

  // topological regions
  std::map<std::string, float> values;
  values["deltaPhiMin"] = t.deltaPhiMin;
  values["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
  values["nlep"]        = t.nLepLowMT;
  values["j1pt"]        = t.jet1_pt;
  values["j2pt"]        = t.jet2_pt;
  values["njets"]       = t.nJet30;
  values["nbjets"]      = t.nBJet20;
  values["mt2"]         = t.mt2;
  values["ht"]          = t.ht;
  values["met"]         = t.met_pt;

  for(unsigned int srN = 0; srN < SRVec.size(); srN++){
    if(SRVec.at(srN).PassesSelectionCRSL(values)){
      if(prefix=="crsl")    fillHistosSingleLepton(SRVec.at(srN).crslHistMap,    SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix);
      else if(prefix=="crslmu")  fillHistosSingleLepton(SRVec.at(srN).crslmuHistMap,  SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix);
      else if(prefix=="crslel")  fillHistosSingleLepton(SRVec.at(srN).crslelHistMap,  SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix);
      //      break;//control regions are not necessarily orthogonal
    }
  }

  return;
}

// hists for Gamma+Jets control region
void MT2Looper::fillHistosCRGJ(const std::string& prefix, const std::string& suffix) {

  if (t.ngamma==0) return;

  // trigger requirement
  if (!t.HLT_Photon165_HE10) return;
  
  bool passSieie = t.gamma_idCutBased[0] ? true : false; // just deal with the standard case now. Worry later about sideband in sieie

  // fill hists
  std::string add="";

  float passPtMT2 = false;
  if (t.mt2 < 200 && t.gamma_pt[0]>180.) passPtMT2 = true;

  std::map<std::string, float> values;
  values["deltaPhiMin"] = t.gamma_deltaPhiMin;
  values["diffMetMhtOverMet"]  = t.gamma_diffMetMht/t.gamma_met_pt;
  values["nlep"]        = nlepveto_;
  values["j1pt"]        = t.gamma_jet1_pt;
  values["j2pt"]        = t.gamma_jet2_pt;
  values["njets"]       = t.gamma_nJet30;
  values["nbjets"]      = t.gamma_nBJet20;
  values["mt2"]         = t.gamma_mt2;
  values["ht"]          = t.gamma_ht;
  values["met"]         = t.gamma_met_pt;
  
  // Separate list for SRBASE
  std::map<std::string, float> valuesBase;
  valuesBase["deltaPhiMin"] = t.gamma_deltaPhiMin;
  valuesBase["diffMetMhtOverMet"]  = t.gamma_diffMetMht/t.gamma_met_pt;
  valuesBase["nlep"]        = nlepveto_;
  valuesBase["j1pt"]        = t.gamma_jet1_pt;
  valuesBase["j2pt"]        = t.gamma_jet2_pt;
  valuesBase["mt2"]         = t.gamma_mt2;
  valuesBase["passesHtMet"] = ( (t.gamma_ht > 450. && t.gamma_met_pt > 200.) || (t.gamma_ht > 1000. && t.gamma_met_pt > 30.) );
  bool passBase = SRBase.PassesSelection(valuesBase);

  //float iso = t.gamma_chHadIso[0] + t.gamma_phIso[0];
  float iso = t.gamma_chHadIso[0];
  float isoCutTight = 2.5;
  float isoCutLoose = 20.;
  if (t.gamma_ht > 200) fillHistosGammaJets(SRNoCut.crgjHistMap, SRNoCut.crgjRooDataSetMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), prefix+SRNoCut.GetName(), suffix+add+"All");

  if (iso>isoCutTight && iso < isoCutLoose) add += "LooseNotTight";
  if (iso>isoCutLoose) add += "NotLoose";
  if (!passSieie) add += "SieieSB"; // Keep Sigma IEta IEta sideband
  if (t.gamma_ht > 200) fillHistosGammaJets(SRNoCut.crgjHistMap, SRNoCut.crgjRooDataSetMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), prefix+SRNoCut.GetName(), suffix+add);
  if(passBase && passPtMT2) {
    fillHistosGammaJets(SRBase.crgjHistMap, SRBase.crgjRooDataSetMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crgjbase", suffix+add);
    for(unsigned int srN = 0; srN < SRVec.size(); srN++){
      if(SRVec.at(srN).PassesSelection(values)){
	fillHistosGammaJets(SRVec.at(srN).crgjHistMap, SRVec.at(srN).crgjRooDataSetMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix+add);
	break;//control regions are orthogonal, event cannot be in more than one
      }
    }//SRloop
  }
  // Remake everything again to get "Loose"
  if (iso<isoCutLoose) {
    add = "Loose";
    if (!passSieie) add += "SieieSB"; // Keep Sigma IEta IEta sideband
    fillHistosGammaJets(SRNoCut.crgjHistMap, SRNoCut.crgjRooDataSetMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), prefix+SRNoCut.GetName(), suffix+add);
    if(passBase && passPtMT2) {
      fillHistosGammaJets(SRBase.crgjHistMap, SRBase.crgjRooDataSetMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crgjbase", suffix+add);
      for(unsigned int srN = 0; srN < SRVec.size(); srN++){
	if(SRVec.at(srN).PassesSelection(values)){
	  fillHistosGammaJets(SRVec.at(srN).crgjHistMap, SRVec.at(srN).crgjRooDataSetMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix+add);
	  break;//control regions are orthogonal, event cannot be in more than one
	}
      }//SRloop
    }
  }
  if (passSieie) {
    add = "AllIso";
    fillHistosGammaJets(SRNoCut.crgjHistMap, SRNoCut.crgjRooDataSetMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), prefix+SRNoCut.GetName(), suffix+add);
    if(passBase && passPtMT2) {
      fillHistosGammaJets(SRBase.crgjHistMap, SRBase.crgjRooDataSetMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crgjbase", suffix+add);
    }
  }
      

  return;
}

// hists for Zll control region
void MT2Looper::fillHistosCRDY(const std::string& prefix, const std::string& suffix) {

  if (t.nlep!=2) return;

  // trigger requirement on data and MC already implemented when defining doDYplots
  
  std::map<std::string, float> values;
  values["deltaPhiMin"] = t.zll_deltaPhiMin;
  values["diffMetMhtOverMet"]  = t.zll_diffMetMht/t.zll_met_pt;
  values["nlep"]        = 0; // dummy value
  values["j1pt"]        = t.jet1_pt;
  values["j2pt"]        = t.jet2_pt;
  values["njets"]       = t.nJet30;
  values["nbjets"]      = t.nBJet20;
  values["mt2"]         = t.zll_mt2;
  values["ht"]          = t.zll_ht;
  values["met"]         = t.zll_met_pt;

 // Separate list for SRBASE
  std::map<std::string, float> valuesBase;
  valuesBase["deltaPhiMin"] = t.zll_deltaPhiMin;
  valuesBase["diffMetMhtOverMet"]  = t.zll_diffMetMht/t.zll_met_pt;
  valuesBase["nlep"]        = 0; // dummy value
  valuesBase["j1pt"]        = t.jet1_pt;
  valuesBase["j2pt"]        = t.jet2_pt;
  valuesBase["mt2"]         = t.zll_mt2;
  valuesBase["passesHtMet"] = ( (t.zll_ht > 450. && t.zll_met_pt > 200.) || (t.zll_ht > 1000. && t.zll_met_pt > 30.) );
  bool passBase = SRBase.PassesSelection(valuesBase);
  
  if (t.zll_ht > 200) fillHistosDY(SRNoCut.crdyHistMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), prefix+SRNoCut.GetName(), suffix);
  if(passBase) fillHistosDY(SRBase.crdyHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crdybase", suffix);
  for(unsigned int srN = 0; srN < SRVec.size(); srN++){
    if(SRVec.at(srN).PassesSelection(values)){
      fillHistosDY(SRVec.at(srN).crdyHistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix);
      break;//control regions are orthogonal, event cannot be in more than one
    }
  }

  return;
}

// hists for removed single lepton control region
void MT2Looper::fillHistosCRRL(const std::string& prefix, const std::string& suffix) {

  if (t.nlep!=1) return;

  // trigger requirement on data and MC already included in RL selection
  
  std::map<std::string, float> values;
  values["deltaPhiMin"] = t.rl_deltaPhiMin;
  values["diffMetMhtOverMet"]  = t.rl_diffMetMht/t.rl_met_pt;
  values["nlep"]        = 0; // dummy value
  values["j1pt"]        = t.jet1_pt;
  values["j2pt"]        = t.jet2_pt;
  values["njets"]       = t.nJet30;
  values["nbjets"]      = t.nBJet20;
  values["mt2"]         = t.rl_mt2;
  values["ht"]          = t.rl_ht;
  values["met"]         = t.rl_met_pt;

 // Separate list for SRBASE
  std::map<std::string, float> valuesBase;
  valuesBase["deltaPhiMin"] = t.rl_deltaPhiMin;
  valuesBase["diffMetMhtOverMet"]  = t.rl_diffMetMht/t.rl_met_pt;
  valuesBase["nlep"]        = 0; // dummy value
  valuesBase["j1pt"]        = t.jet1_pt;
  valuesBase["j2pt"]        = t.jet2_pt;
  valuesBase["mt2"]         = t.rl_mt2;
  valuesBase["passesHtMet"] = ( (t.rl_ht > 450. && t.rl_met_pt > 200.) || (t.rl_ht > 1000. && t.rl_met_pt > 30.) );
  bool passBase = SRBase.PassesSelection(valuesBase);
  
  if(prefix=="crrl")        fillHistosRemovedLepton(SRNoCut.crrlHistMap,   SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), prefix+SRNoCut.GetName(), suffix);
  else if(prefix=="crrlmu") fillHistosRemovedLepton(SRNoCut.crrlmuHistMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), prefix+SRNoCut.GetName(), suffix);
  else if(prefix=="crrlel") fillHistosRemovedLepton(SRNoCut.crrlelHistMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), prefix+SRNoCut.GetName(), suffix);
    
  if(passBase) {
    if(prefix=="crrl")        fillHistosRemovedLepton(SRBase.crrlHistMap,   SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlbase",   suffix);
    else if(prefix=="crrlmu") fillHistosRemovedLepton(SRBase.crrlmuHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlmubase", suffix);
    else if(prefix=="crrlel") fillHistosRemovedLepton(SRBase.crrlelHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlelbase", suffix);
  }
  // if(passBase && t.evt_id == 400 ) fillHistosRemovedLepton(SRBase.crrlHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlbase", "TsChan");
  // if(passBase && t.evt_id == 401 ) fillHistosRemovedLepton(SRBase.crrlHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlbase", "TtChan");
  // if(passBase && t.evt_id == 402 ) fillHistosRemovedLepton(SRBase.crrlHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlbase", "TtWChan");
  // if(passBase && t.evt_id == 403 ) fillHistosRemovedLepton(SRBase.crrlHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlbase", "TbarsChan");
  // if(passBase && t.evt_id == 404 ) fillHistosRemovedLepton(SRBase.crrlHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlbase", "TbartChan");
  // if(passBase && t.evt_id == 405 ) fillHistosRemovedLepton(SRBase.crrlHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlbase", "TbartwChan");

  for(unsigned int srN = 0; srN < SRVec.size(); srN++){
    if(SRVec.at(srN).PassesSelection(values)){
      if(prefix=="crrl")        fillHistosRemovedLepton(SRVec.at(srN).crrlHistMap,   SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix);
      else if(prefix=="crrlmu") fillHistosRemovedLepton(SRVec.at(srN).crrlmuHistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix);
      else if(prefix=="crrlel") fillHistosRemovedLepton(SRVec.at(srN).crrlelHistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix);
      break;//control regions are orthogonal, event cannot be in more than one
    }
  }

  return;
}

void MT2Looper::fillHistos(std::map<std::string, TH1*>& h_1d, int n_mt2bins, float* mt2bins, const std::string& dirname, const std::string& s) {
  TDirectory * dir = (TDirectory*)outfile_->Get(dirname.c_str());
  if (dir == 0) {
    dir = outfile_->mkdir(dirname.c_str());
  } 
  dir->cd();

  plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
  plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
  plot1D("h_mt2"+s,       t.mt2,   evtweight_, h_1d, "; M_{T2} [GeV]", 150, 0, 1500);
  plot1D("h_met"+s,       t.met_pt,   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 150, 0, 1500);
  plot1D("h_ht"+s,       t.ht,   evtweight_, h_1d, ";H_{T} [GeV]", 120, 0, 3000);
  plot1D("h_nJet30"+s,       t.nJet30,   evtweight_, h_1d, ";N(jets)", 15, 0, 15);
  plot1D("h_nJet30Eta3"+s,       nJet30Eta3_,   evtweight_, h_1d, ";N(jets, |#eta| > 3.0)", 10, 0, 10);
  plot1D("h_nBJet20"+s,      t.nBJet20,   evtweight_, h_1d, ";N(bjets)", 6, 0, 6);
  plot1D("h_deltaPhiMin"+s,  t.deltaPhiMin,   evtweight_, h_1d, ";#Delta#phi_{min}", 32, 0, 3.2);
  plot1D("h_diffMetMht"+s,   t.diffMetMht,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| [GeV]", 120, 0, 300);
  plot1D("h_diffMetMhtOverMet"+s,   t.diffMetMht/t.met_pt,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| / E_{T}^{miss}", 100, 0, 2.);
  plot1D("h_minMTBMet"+s,   t.minMTBMet,   evtweight_, h_1d, ";min M_{T}(b, E_{T}^{miss}) [GeV]", 150, 0, 1500);
  plot1D("h_nlepveto"+s,     nlepveto_,   evtweight_, h_1d, ";N(leps)", 10, 0, 10);
  plot1D("h_J0pt"+s,       t.jet1_pt,   evtweight_, h_1d, ";p_{T}(jet1) [GeV]", 150, 0, 1500);
  plot1D("h_J1pt"+s,       t.jet2_pt,   evtweight_, h_1d, ";p_{T}(jet2) [GeV]", 150, 0, 1500);
  plot1D("h_mt2bins"+s,       t.mt2,   evtweight_, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);

  TString directoryname(dirname);
  if (directoryname.Contains("nocut")) {
    
    float genHT = 0;
    int lep1idx = -1;
    int lep1id = -1;
    int lep2idx = -1;
    int Zidx = -1;
    float minDR = 999;
    for ( int i = 0; i < t.ngenStat23; i++) {
      int ID = fabs(t.genStat23_pdgId[i]);
      if (ID==22) Zidx = i;
    }

    for ( int i = 0; i < t.ngenStat23; i++) {
      int ID = fabs(t.genStat23_pdgId[i]);
      if (ID<6 ||ID==21) {
	genHT += t.genStat23_pt[i];
	if (Zidx!=-1) {
	  float thisDR = DeltaR(t.genStat23_eta[Zidx],t.genStat23_eta[i],t.genStat23_phi[Zidx],t.genStat23_phi[i]);
	  if ( thisDR < minDR ) minDR = thisDR;
	}
      }
      if ( (ID==11 || ID==13) &&  t.genStat23_pt[i]>20 && fabs(t.genStat23_eta[i])<2.5 ) { 
	if (lep1idx==-1) {
	  lep1idx = i;
	  lep1id = ID;
	}
	else if ( ID == lep1id ) lep2idx = i;
      }
    }
    
    TLorentzVector V(0,0,0,0);
    if (lep2idx!=-1) {
	TLorentzVector lep1(0,0,0,0);
	lep1.SetPtEtaPhiM(t.genStat23_pt[lep1idx], t.genStat23_eta[lep1idx], t.genStat23_phi[lep1idx], t.genStat23_mass[lep1idx]);
	TLorentzVector lep2(0,0,0,0);
	lep2.SetPtEtaPhiM(t.genStat23_pt[lep2idx], t.genStat23_eta[lep2idx], t.genStat23_phi[lep2idx], t.genStat23_mass[lep2idx]);
	V = lep1 + lep2;
    }
    if (minDR < 0.4) Zidx = -1; // don't use low-DR photons
    if (Zidx != -1) {
      V.SetPtEtaPhiM(t.genStat23_pt[Zidx], t.genStat23_eta[Zidx], t.genStat23_phi[Zidx], t.genStat23_mass[Zidx]);
    }
    if (Zidx != -1 || lep2idx!=-1) {
      float Vrapidity = V.Rapidity();
      plot1D("h_VpT"+s,       V.Pt(),   evtweight_, h_1d, "; p_{T}^{V} [GeV]", n_ptVbins, ptVbins);
      if (fabs(Vrapidity) < 1.4 ) plot1D("h_VpTcentral"+s,      V.Pt(),   evtweight_, h_1d, "; p_{T}^{V} [GeV]", n_ptVbins, ptVbins);
      if (genHT>300) plot1D("h_VpT_HT300"+s,      V.Pt(),   evtweight_, h_1d, "; p_{T}^{V} [GeV]", n_ptVbins, ptVbins);
      if (fabs(Vrapidity) < 1.4 && genHT>300) plot1D("h_VpTcentral_HT300"+s,       V.Pt(),   evtweight_, h_1d, "; p_{T}^{V} [GeV]", n_ptVbins, ptVbins);
    }

  } // nocut region only

  if (isSignal_) {
    plot3D("h_mt2bins_sigscan"+s, t.GenSusyMScan1, t.GenSusyMScan2, t.mt2, evtweight_, h_1d, "mass1 [GeV];mass2 [GeV];M_{T2} [GeV]", n_m1bins, m1bins, n_m2bins, m2bins, n_mt2bins, mt2bins);
  }

  if (!t.isData && applyWeights && doSystVariationPlots) {
    // assume weights are already applied to central value: lepsf, btagsf, isr 
    plot1D("h_mt2bins_lepsf_UP"+s,       t.mt2,   evtweight_ / t.weight_lepsf * t.weight_lepsf_UP, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_lepsf_DN"+s,       t.mt2,   evtweight_ / t.weight_lepsf * t.weight_lepsf_DN, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_btagsf_UP"+s,       t.mt2,   evtweight_ / t.weight_btagsf * t.weight_btagsf_UP, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_btagsf_DN"+s,       t.mt2,   evtweight_ / t.weight_btagsf * t.weight_btagsf_DN, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_isr_UP"+s,       t.mt2,   evtweight_ / t.weight_isr, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_isr_DN"+s,       t.mt2,   evtweight_ * t.weight_isr, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    // WARNING: will have to make sure we get the correct normalization for these
    //  to avoid including xsec effects..
    plot1D("h_mt2bins_scales_UP"+s,       t.mt2,   evtweight_ * t.weight_scales_UP, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_scales_DN"+s,       t.mt2,   evtweight_ * t.weight_scales_DN, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_pdfs_UP"+s,       t.mt2,   evtweight_  * t.weight_pdfs_UP, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_pdfs_DN"+s,       t.mt2,   evtweight_  * t.weight_pdfs_DN, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
  }

  outfile_->cd();
  return;
}

void MT2Looper::fillHistosSingleLepton(std::map<std::string, TH1*>& h_1d, int n_mt2bins, float* mt2bins, const std::string& dirname, const std::string& s) {
  TDirectory * dir = (TDirectory*)outfile_->Get(dirname.c_str());
  if (dir == 0) {
    dir = outfile_->mkdir(dirname.c_str());
  } 
  dir->cd();

  plot1D("h_leppt"+s,      leppt_,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 200, 0, 1000);
  plot1D("h_mt"+s,            mt_,   evtweight_, h_1d, ";M_{T} [GeV]", 200, 0, 1000);

  outfile_->cd();

  fillHistos(h_1d, n_mt2bins, mt2bins, dirname, s);
  return;
}


void MT2Looper::fillHistosGammaJets(std::map<std::string, TH1*>& h_1d, std::map<std::string, RooDataSet*>& datasets, int n_mt2bins, float* mt2bins, const std::string& dirname, const std::string& s) {
  TDirectory * dir = (TDirectory*)outfile_->Get(dirname.c_str());
  if (dir == 0) {
    dir = outfile_->mkdir(dirname.c_str());
  } 
  dir->cd();

  //bins for FR
  const int n_FRhtbins = 6;
  const int n_FRptbins = 5;
  const float FRhtbins[n_FRhtbins+1] = {0,200,450,1000,1500,2000,3000};
  const float FRptbins[n_FRptbins+1] = {0,150,300,450,600,1500};

  float iso = t.gamma_chHadIso[0] + t.gamma_phIso[0];
  float chiso = t.gamma_chHadIso[0];

  //RooRealVars for unbinned data hist
  x_->setVal(chiso);
  w_->setVal(evtweight_);
 
  plot1D("h_iso"+s,      iso,   evtweight_, h_1d, ";iso [GeV]", 100, 0, 50);
  plot1D("h_chiso"+s,      chiso,   evtweight_, h_1d, ";iso [GeV]", 100, 0, 50);
  if (fabs(t.gamma_eta[0]) < 1.479) plot1D("h_chisoEB"+s,      chiso,   evtweight_, h_1d, ";iso [GeV]", 100, 0, 50);
  else plot1D("h_chisoEE"+s,      chiso,   evtweight_, h_1d, ";iso [GeV]", 100, 0, 50);
  plot1D("h_isoW1"+s,      iso,   1, h_1d, ";iso [GeV]", 100, 0, 50);
  plot1D("h_chisoW1"+s,      chiso,   1, h_1d, ";ch iso [GeV]", 100, 0, 50);

  plotRooDataSet("rds_chIso_"+s, x_, w_, evtweight_, datasets, "");

  //for FR calculation
  if( (t.evt_id>110 && t.evt_id<120) || t.evt_id==-1){ //only use qcd samples with pt>=470 to compute FR
    if(t.gamma_idCutBased[0]){ //passSieie
      plot2D("h2d_gammaht_gammapt"+s, t.gamma_ht, t.gamma_pt[0], evtweight_, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",n_FRhtbins,FRhtbins,n_FRptbins,FRptbins);
      plot2D("h2d_gammaht_gammaptW1"+s, t.gamma_ht, t.gamma_pt[0], 1, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",n_FRhtbins,FRhtbins,n_FRptbins,FRptbins);
    }
    if(!t.gamma_idCutBased[0]){ //!passSieie
      plot2D("h2d_gammaht_gammaptSingleBin"+s, t.gamma_ht, t.gamma_pt[0], evtweight_, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",1,0,3000,1,0,1500);
      plot2D("h2d_gammaht_gammaptSingleBinW1"+s, t.gamma_ht, t.gamma_pt[0], 1, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",1,0,3000,1,0,1500);
    }
  }
  
  for (int i = 0; i < SRBase.GetNumberOfMT2Bins(); i++) {
    if ( t.gamma_mt2 > SRBase.GetMT2Bins()[i] &&  t.gamma_mt2 < SRBase.GetMT2Bins()[i+1]) {
      plotRooDataSet("rds_chIso_"+toString(SRBase.GetMT2Bins()[i])+s, x_, w_, evtweight_, datasets, "");
      plot1D("h_chiso_mt2bin"+toString(SRBase.GetMT2Bins()[i])+s,  iso,  evtweight_, h_1d, "; iso", 100, 0, 50);
      plot2D("h2d_gammaht_gammapt"+toString(SRBase.GetMT2Bins()[i])+s, t.gamma_ht, t.gamma_pt[0], evtweight_, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",n_FRhtbins,FRhtbins,n_FRptbins,FRptbins);
      //plot2D("h2d_gammaht_gammaptW1"+toString(SRBase.GetMT2Bins()[i])+s, t.gamma_ht, t.gamma_pt[0], evtweight_, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",n_FRhtbins,FRhtbins,n_FRptbins,FRptbins);	
      plot2D("h2d_gammaht_gammaptSingleBin"+toString(SRBase.GetMT2Bins()[i])+s, t.gamma_ht, t.gamma_pt[0], evtweight_, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",1,0,3000,1,0,1500);
      //plot2D("h2d_gammaht_gammaptSingleBinW1"+toString(SRBase.GetMT2Bins()[i])+s, t.gamma_ht, t.gamma_pt[0], evtweight_, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",1,0,3000,1,0,1500);
    }
  }

  //cout<<"Event "<<t.evt<<" with weight "<< evtweight_ <<" is in sr "<<dirname<<endl;
  plot1D("h_mt2bins"+s,       t.gamma_mt2,   evtweight_, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
  if ( (dirname=="crgjnocut" || dirname=="crgjbase" || dirname=="crgjL" || dirname=="crgjM" || dirname=="crgjH") 
       && (s=="" || s=="Fake" || s=="FragGJ" || s=="AllIso" || s=="LooseNotTight") )// Don't make these for Loose, NotLoose. SieieSB
    {
    plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
    plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
    plot1D("h_mt2"+s,       t.gamma_mt2,   evtweight_, h_1d, "; M_{T2} [GeV]", 150, 0, 1500);
    plot1D("h_met"+s,       t.gamma_met_pt,   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 150, 0, 1500);
    plot1D("h_simplemet"+s,       t.met_pt,   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 150, 0, 1500);
    plot1D("h_gammaPt"+s,       t.gamma_pt[0],   evtweight_, h_1d, ";gamma p_{T} [GeV]", 264, 180, 1500);
    if (fabs(t.gamma_eta[0]) < 1.479) plot1D("h_gammaPtEB"+s,       t.gamma_pt[0],   evtweight_, h_1d, ";gamma p_{T} [GeV]", 264, 180, 1500);
    else plot1D("h_gammaPtEE"+s,       t.gamma_pt[0],   evtweight_, h_1d, ";gamma p_{T} [GeV]", 264, 180, 1500);
    plot1D("h_gammaEta"+s,       t.gamma_eta[0],   evtweight_, h_1d, ";gamma #eta [GeV]", 50, -2.5, 2.5);
    if (t.HLT_Photons) plot1D("h_gammaPt_HLT"+s,       t.gamma_pt[0],   evtweight_, h_1d, ";gamma p_{T} [GeV]", 300, 0, 1500);
    plot1D("h_ht"+s,       t.gamma_ht,   evtweight_, h_1d, ";H_{T} [GeV]", 120, 0, 3000);
    plot1D("h_nJet30"+s,       t.gamma_nJet30,   evtweight_, h_1d, ";N(jets)", 15, 0, 15);
    plot1D("h_nBJet20"+s,      t.gamma_nBJet20,   evtweight_, h_1d, ";N(bjets)", 6, 0, 6);
    plot1D("h_deltaPhiMin"+s,  t.gamma_deltaPhiMin,   evtweight_, h_1d, ";#Delta#phi_{min}", 32, 0, 3.2);
    plot1D("h_diffMetMht"+s,   t.gamma_diffMetMht,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| [GeV]", 120, 0, 300);
    plot1D("h_diffMetMhtOverMet"+s,   t.gamma_diffMetMht/t.gamma_met_pt,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| / E_{T}^{miss}", 100, 0, 2.);
    plot1D("h_minMTBMet"+s,   t.gamma_minMTBMet,   evtweight_, h_1d, ";min M_{T}(b, E_{T}^{miss}) [GeV]", 150, 0, 1500);
    plot1D("h_nlepveto"+s,     nlepveto_,   evtweight_, h_1d, ";N(leps)", 10, 0, 10);
    plot1D("h_htbins"+s,       t.gamma_ht,   evtweight_, h_1d, ";H_{T} [GeV]", n_htbins, htbins);
    plot1D("h_njbins"+s,       t.gamma_nJet30,   evtweight_, h_1d, ";N(jets)", n_njbins, njbins);
    plot1D("h_nbjbins"+s,       t.gamma_nBJet20,   evtweight_, h_1d, ";N(bjets)", n_nbjbins, nbjbins);
    plot1D("h_drMinParton"+s,   t.gamma_drMinParton[0],   evtweight_, h_1d, ";DRmin(photon, parton)", 100, 0, 5);
    TString drName = "h_drMinPartonSample";
    drName += TString::Itoa(t.evt_id, 10);
    drName += s;
    plot1D(drName.Data(),   t.gamma_drMinParton[0],   evtweight_, h_1d, ";DRmin(photon, parton)", 100, 0, 5);
    if (t.gamma_ht > 300) 
      {
	plot1D("h_bosonptbins"+s,      t.gamma_pt[0],   evtweight_, h_1d, ";p_{T}^{V} [GeV]", n_ptVbins, ptVbins);
	if ( fabs(t.gamma_eta[0])<1.4 ) plot1D("h_bosonptbinsCentral"+s,      t.gamma_pt[0],   evtweight_, h_1d, ";p_{T}^{V} [GeV]", n_ptVbins, ptVbins);
      }
    }

  outfile_->cd();
  return;
}

void MT2Looper::fillHistosDY(std::map<std::string, TH1*>& h_1d, int n_mt2bins, float* mt2bins, const std::string& dirname, const std::string& s) {
  TDirectory * dir = (TDirectory*)outfile_->Get(dirname.c_str());
  if (dir == 0) {
    dir = outfile_->mkdir(dirname.c_str());
  } 
  dir->cd();

  plot1D("h_mt2bins"+s,       t.zll_mt2,   evtweight_, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
  
  if (dirname=="crdynocut" || dirname=="crdybase" || dirname=="crdyL" || dirname=="crdyM" || dirname=="crdyH") {
    plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
    plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
    plot1D("h_mt2"+s,       t.zll_mt2,   evtweight_, h_1d, "; M_{T2} [GeV]", 150, 0, 1500);
    plot1D("h_met"+s,       t.zll_met_pt,   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 150, 0, 1500);
    plot1D("h_ht"+s,       t.zll_ht,   evtweight_, h_1d, ";H_{T} [GeV]", 120, 0, 3000);
    plot1D("h_nJet30"+s,       t.nJet30,   evtweight_, h_1d, ";N(jets)", 15, 0, 15);
    plot1D("h_nBJet20"+s,      t.nBJet20,   evtweight_, h_1d, ";N(bjets)", 6, 0, 6);
    plot1D("h_deltaPhiMin"+s,  t.zll_deltaPhiMin,   evtweight_, h_1d, ";#Delta#phi_{min}", 32, 0, 3.2);
    plot1D("h_diffMetMht"+s,   t.zll_diffMetMht,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| [GeV]", 120, 0, 300);
    plot1D("h_diffMetMhtOverMet"+s,   t.zll_diffMetMht/t.zll_met_pt,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| / E_{T}^{miss}", 100, 0, 2.);
    plot1D("h_minMTBMet"+s,   t.zll_minMTBMet,   evtweight_, h_1d, ";min M_{T}(b, E_{T}^{miss}) [GeV]", 150, 0, 1500);
    plot1D("h_nlepveto"+s,     t.nLepLowMT,   evtweight_, h_1d, ";N(leps)", 10, 0, 10);
    plot1D("h_htbins"+s,       t.zll_ht,   evtweight_, h_1d, ";H_{T} [GeV]", n_htbins, htbins);
    plot1D("h_njbins"+s,       t.nJet30,   evtweight_, h_1d, ";N(jets)", n_njbins, njbins);
    plot1D("h_nbjbins"+s,       t.nBJet20,   evtweight_, h_1d, ";N(bjets)", n_nbjbins, nbjbins);
    plot1D("h_leppt1"+s,      t.lep_pt[0],   evtweight_, h_1d, ";p_{T}(lep1) [GeV]", 50, 0, 200);
    plot1D("h_leppt2"+s,      t.lep_pt[1],   evtweight_, h_1d, ";p_{T}(lep2) [GeV]", 50, 0, 200);
    plot1D("h_zllmass"+s,      t.zll_mass,   evtweight_, h_1d, ";m_{ll} [GeV]", 60, 0, 120);
    if (abs(t.lep_pdgId[0])==11) plot1D("h_zllmassEle"+s,      t.zll_mass,   evtweight_, h_1d, ";m_{ll} [GeV]", 60, 0, 120);
    if (abs(t.lep_pdgId[0])==13) plot1D("h_zllmassMu"+s,      t.zll_mass,   evtweight_, h_1d, ";m_{ll} [GeV]", 60, 0, 120);
    if (t.zll_ht > 300) 
      {
	plot1D("h_bosonptbins"+s,      t.zll_pt,   evtweight_, h_1d, ";p_{T}^{V} [GeV]", n_ptVbins, ptVbins);
	TLorentzVector Zll(0,0,0,0);
	Zll.SetPtEtaPhiM(t.zll_pt, t.zll_eta, t.zll_phi, t.zll_mass);
	float Zrapidity = Zll.Rapidity();
	if ( fabs(Zrapidity)<1.4 ) plot1D("h_bosonptbinsCentral"+s,      t.zll_pt,   evtweight_, h_1d, ";p_{T}^{V} [GeV]", n_ptVbins, ptVbins);
      }
  }

  outfile_->cd();
  return;
}

void MT2Looper::fillHistosRemovedLepton(std::map<std::string, TH1*>& h_1d, int n_mt2bins, float* mt2bins, const std::string& dirname, const std::string& s) {
  TDirectory * dir = (TDirectory*)outfile_->Get(dirname.c_str());
  if (dir == 0) {
    dir = outfile_->mkdir(dirname.c_str());
  } 
  dir->cd();

  plot1D("h_mt2bins"+s,       t.rl_mt2,   evtweight_, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);

  if (dirname=="crrlnocut" || dirname=="crrlbase" || dirname=="crrlL" || dirname=="crrlM" || dirname=="crrlH" ||
      dirname=="crrlelnocut" || dirname=="crrlelbase" || dirname=="crrlelL" || dirname=="crrlelM" || dirname=="crrlelH" ||
      dirname=="crrlmunocut" || dirname=="crrlmubase" || dirname=="crrlmuL" || dirname=="crrlmuM" || dirname=="crrlmuH" ) { 

    float mt = MT(t.lep_pt[0],t.lep_phi[0],t.met_pt,t.met_phi);
    plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
    plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
    plot1D("h_mt2"+s,       t.rl_mt2,   evtweight_, h_1d, "; M_{T2} [GeV]", 150, 0, 1500);
    plot1D("h_met"+s,       t.rl_met_pt,   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 150, 0, 1500);
    plot1D("h_ht"+s,       t.rl_ht,   evtweight_, h_1d, ";H_{T} [GeV]", 120, 0, 3000);
    plot1D("h_nJet30"+s,       t.nJet30,   evtweight_, h_1d, ";N(jets)", 15, 0, 15);
    plot1D("h_nBJet20"+s,      t.nBJet20,   evtweight_, h_1d, ";N(bjets)", 6, 0, 6);
    plot1D("h_deltaPhiMin"+s,  t.rl_deltaPhiMin,   evtweight_, h_1d, ";#Delta#phi_{min}", 32, 0, 3.2);
    plot1D("h_diffMetMht"+s,   t.rl_diffMetMht,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| [GeV]", 120, 0, 300);
    plot1D("h_diffMetMhtOverMet"+s,   t.rl_diffMetMht/t.rl_met_pt,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| / E_{T}^{miss}", 100, 0, 2.);
    plot1D("h_minMTBMet"+s,   t.rl_minMTBMet,   evtweight_, h_1d, ";min M_{T}(b, E_{T}^{miss}) [GeV]", 150, 0, 1500);
    plot1D("h_nlepveto"+s,     t.nLepLowMT,   evtweight_, h_1d, ";N(leps)", 10, 0, 10);
    plot1D("h_htbins"+s,       t.rl_ht,   evtweight_, h_1d, ";H_{T} [GeV]", n_htbins, htbins);
    plot1D("h_njbins"+s,       t.nJet30,   evtweight_, h_1d, ";N(jets)", n_njbins, njbins);
    plot1D("h_nbjbins"+s,       t.nBJet20,   evtweight_, h_1d, ";N(bjets)", n_nbjbins, nbjbins);
    plot1D("h_leppt"+s,      t.lep_pt[0],   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 200, 0, 1000);
    plot1D("h_lepeta"+s,      t.lep_eta[0],   evtweight_, h_1d, "#eta(lep) [GeV]", 50, -2.5, 2.5);
    plot1D("h_mt"+s,      mt,   evtweight_, h_1d, ";m_{T}(lep, MET) [GeV]", 200, 0, 400);
    plot1D("h_lepreliso"+s,      t.lep_relIso03[0],   evtweight_, h_1d, ";RelIso03", 50, 0, 1);
    plot1D("h_lepabsiso"+s,      t.lep_relIso03[0]*t.lep_pt[0],   evtweight_, h_1d, ";AbsIso03 [GeV]", 50, 0, 50);
    plot1D("h_simplemet"+s,      t.met_pt,   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 200, 0, 1000);
  }//dirname

  outfile_->cd();
  return;
}
