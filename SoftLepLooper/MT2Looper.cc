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
#include "Math/Vector4D.h"
#include "TVector2.h"
#include "TBenchmark.h"
#include "TMath.h"
#include "TLorentzVector.h"

// Tools
#include "../CORE/Tools/utils.h"
#include "../CORE/Tools/goodrun.h"
#include "../CORE/Tools/dorky/dorky.h"
#include "../CORE/Tools/badEventFilter.h"
#include "../CORE/Tools/hemJet.h" 
#include "../CORE/Tools/MT2/MT2.h"
#include "../CORE/MCSelections.h"

// header
#include "MT2Looper.h"

//MT2
#include "../MT2CORE/Plotting/PlotUtilities.h"
#include "../MT2CORE/applyWeights.h"

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
//   assuming here: 25 GeV binning, m1 from 0-2000, m2 from 0-2000
//   in Loop, also account for 10 GeV binning from 0-1000 in T2-4bd scan
const int n_m1bins = 81;
float m1bins[n_m1bins+1];
const int n_m2bins = 81;
float m2bins[n_m2bins+1];

const int n_htbins = 5;
const float htbins[n_htbins+1] = {200, 450., 575., 1000., 1500., 3000.};
const int n_htbins2 = 7;
const float htbins2[n_htbins2+1] = {200, 250., 350., 450., 575., 700., 1000., 3000.};
const int n_njbins = 4;
const float njbins[n_njbins+1] = {1, 2, 4, 7, 12};
const int n_nbjbins = 4;
const float nbjbins[n_nbjbins+1] = {0, 1, 2, 3, 6};
const int n_ptVbins = 19;
float logstep(int i) {
  return TMath::Power(10, 2+4.5e-02*i);
}
const float ptVbins[n_ptVbins+1] = {100, logstep(1), logstep(2), logstep(3), logstep(4), logstep(5), logstep(6), logstep(7), logstep(8), logstep(9), 
				    logstep(10), logstep(11), logstep(12), logstep(13), logstep(14), logstep(15), logstep(16), logstep(17), 800, 1200};
const int n_ptsoftbins = 3;
const float ptsoftbins[n_ptsoftbins+1] = {5., 10., 15., 20.};

// turn on to apply weights to central value
bool applyWeights = false;
// turn on to apply btag sf to central value
bool applyBtagSF = true;
// turn on to apply lepton sf to central value
bool applyLeptonSF = true;
// turn on to apply reweighting to ttbar based on top pt
bool applyTopPtReweight = true;
// turn on to apply lepton sf to central value for 0L sample in fastsim
bool applyLeptonSFfastsim = true;
// turn on to enable plots of MT2 with systematic variations applied. will only do variations for applied weights
bool doSystVariationPlots = true;
// turn on to apply Nvtx reweighting to MC
bool doNvtxReweight = false;
// turn on to apply json file to data
bool applyJSON = false;
// veto on jets with pt > 30, |eta| > 3.0
bool doHFJetVeto = false;
// get signal scan nevents from file
bool doScanWeights = true;
// doesn't plot data for MT2 > 200 in signal regions
bool doBlindData = true;
// make variation histograms for tau efficiency
bool doGenTauVars = true;
// make variation histograms for e+mu efficiency
bool doLepEffVars = true;
// make only minimal hists needed for results
bool doMinimalPlots = true;
// if true, only make plots with both el,mu
bool doCombineElMu = true;
// make fake-rate hists
bool calculateFakeRates = false;
bool applyFakeRates = true;
// do pre-scale reweighting (for fake rates)
bool doPrescaleWeight = false;

// This is meant to be passed as the third argument, the predicate, of the standard library sort algorithm
inline bool sortByPt(const LorentzVector &vec1, const LorentzVector &vec2 ) {
  return vec1.pt() > vec2.pt();
}

MT2Looper::MT2Looper(){

}
MT2Looper::~MT2Looper(){

};

void MT2Looper::SetSignalRegions(){

  //SRVec =  getSignalRegionsZurich_jetpt30(); //same as getSignalRegionsZurich(), but with j1pt and j2pt cuts changed to 30 GeV
  SRVec =  getSignalRegionsJamboree(); //adds HT 200-450 regions
  SRVecLep =  getSignalRegionsLep4(); 
  SRVecMonojet = getSignalRegionsMonojet(); // first pass of monojet regions

  //store histograms with cut values for all variables
  storeHistWithCutValues(SRVecLep, "srLep");
  storeHistWithCutValues(SRVecLep, "cr1L");
  storeHistWithCutValues(SRVecLep, "cr1Lmu");
  storeHistWithCutValues(SRVecLep, "cr1Lel");
  storeHistWithCutValues(SRVecLep, "cr2L");

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


  //setup inclusive regions
  SR InclusiveHT200to450 = SRBase;
  InclusiveHT200to450.SetName("srbaseVL");
  InclusiveHT200to450.SetVar("ht", 200, 450);
  InclusiveHT200to450.SetVarCRSL("ht", 200, 450);
  InclusiveRegions.push_back(InclusiveHT200to450);

  SR InclusiveHT450to575 = SRBase;
  InclusiveHT450to575.SetName("srbaseL");
  InclusiveHT450to575.SetVar("ht", 450, 575);
  InclusiveHT450to575.SetVarCRSL("ht", 450, 575);
  InclusiveRegions.push_back(InclusiveHT450to575);

  SR InclusiveHT575to1000 = SRBase;
  InclusiveHT575to1000.SetName("srbaseM");
  InclusiveHT575to1000.SetVar("ht", 575, 1000);
  InclusiveHT575to1000.SetVarCRSL("ht", 575, 1000);
  InclusiveRegions.push_back(InclusiveHT575to1000);

  SR InclusiveHT1000to1500 = SRBase;
  InclusiveHT1000to1500.SetName("srbaseH");
  InclusiveHT1000to1500.SetVar("ht", 1000, 1500);
  InclusiveHT1000to1500.SetVarCRSL("ht", 1000, 1500);
  InclusiveRegions.push_back(InclusiveHT1000to1500);

  SR InclusiveHT1500toInf = SRBase;
  InclusiveHT1500toInf.SetName("srbaseUH");
  InclusiveHT1500toInf.SetVar("ht", 1500, -1);
  InclusiveHT1500toInf.SetVarCRSL("ht", 1500, -1);
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

  // ----- monojet base regions

  SRBaseMonojet.SetName("srbaseJ");
  SRBaseMonojet.SetVar("j1pt", 200, -1);
  SRBaseMonojet.SetVar("njets", 1, 2);
  SRBaseMonojet.SetVar("nlep", 0, 1);
  SRBaseMonojet.SetVar("met", 200, -1);
  SRBaseMonojet.SetVar("deltaPhiMin", 0.3, -1);
  SRBaseMonojet.SetVar("diffMetMhtOverMet", 0, 0.5);
  SRBaseMonojet.SetVarCRSL("j1pt", 200, -1);
  SRBaseMonojet.SetVarCRSL("njets", 1, 2);
  SRBaseMonojet.SetVarCRSL("nlep", 1, 2);
  SRBaseMonojet.SetVarCRSL("met", 200, -1);
  SRBaseMonojet.SetVarCRSL("deltaPhiMin", 0.3, -1);
  SRBaseMonojet.SetVarCRSL("diffMetMhtOverMet", 0, 0.5);
  float SRBaseMonojet_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500};
  SRBaseMonojet.SetMT2Bins(7, SRBaseMonojet_mt2bins);

  SRBaseIncl.SetName("srbaseIncl");
  float SRBaseIncl_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500};
  SRBaseIncl.SetMT2Bins(7, SRBaseIncl_mt2bins);
}


void MT2Looper::storeHistWithCutValues(std::vector<SR> & srvector, TString SR) {
  
  for(unsigned int i = 0; i < srvector.size(); i++){
    std::vector<std::string> vars = srvector.at(i).GetListOfVariables();
    TString dirname = SR+srvector.at(i).GetName();
    TDirectory * dir = (TDirectory*)outfile_->Get(dirname);
    if (dir == 0) {
      dir = outfile_->mkdir(dirname);
    } 
    dir->cd();
    for(unsigned int j = 0; j < vars.size(); j++){
      if (SR=="cr1L") {
	plot1D("h_"+vars.at(j)+"_"+"LOW",  1, srvector.at(i).GetLowerBound(vars.at(j)), srvector.at(i).cr1LHistMap, "", 1, 0, 2);
	plot1D("h_"+vars.at(j)+"_"+"HI",   1, srvector.at(i).GetUpperBound(vars.at(j)), srvector.at(i).cr1LHistMap, "", 1, 0, 2);
      }
      if (SR=="cr2L") {
	plot1D("h_"+vars.at(j)+"_"+"LOW",  1, srvector.at(i).GetLowerBound(vars.at(j)), srvector.at(i).cr2LHistMap, "", 1, 0, 2);
	plot1D("h_"+vars.at(j)+"_"+"HI",   1, srvector.at(i).GetUpperBound(vars.at(j)), srvector.at(i).cr2LHistMap, "", 1, 0, 2);
      }
      if (SR=="srLep") {
	plot1D("h_"+vars.at(j)+"_"+"LOW",  1, srvector.at(i).GetLowerBound(vars.at(j)), srvector.at(i).srHistMap, "", 1, 0, 2);
	plot1D("h_"+vars.at(j)+"_"+"HI",   1, srvector.at(i).GetUpperBound(vars.at(j)), srvector.at(i).srHistMap, "", 1, 0, 2);
      }
    }
    if (SR=="cr1L")  plot1D("h_n_mtbins",  1, srvector.at(i).GetNumberOfMT2Bins(), srvector.at(i).cr1LHistMap, "", 1, 0, 2);
    else if (SR=="cr2L")  plot1D("h_n_mtbins",  1, srvector.at(i).GetNumberOfMT2Bins(), srvector.at(i).cr2LHistMap, "", 1, 0, 2);
    else if (SR=="srLep") plot1D("h_n_mtbins",  1, srvector.at(i).GetNumberOfMT2Bins(), srvector.at(i).srHistMap, "", 1, 0, 2);
    
  }
  outfile_->cd();
  
}


void MT2Looper::loop(TChain* chain, std::string sample, std::string output_dir){

  // Benchmark
  TBenchmark *bmark = new TBenchmark();
  bmark->Start("benchmark");

  gROOT->cd();
  TString output_name = Form("%s/%s.root",output_dir.c_str(),sample.c_str());
  cout << "[MT2Looper::loop] creating output file: " << output_name << endl;

  outfile_ = new TFile(output_name.Data(),"RECREATE") ; 

  const char* json_file = "../babymaker/jsons/Cert_246908-251883_13TeV_PromptReco_Collisions15_JSON_v2_snt.txt";
  if (applyJSON) {
    cout << "Loading json file: " << json_file << endl;
    set_goodrun_file(json_file);
  }

  eventFilter metFilterTxt;
  TString stringsample = sample;
  if (stringsample.Contains("data")) {
    cout<<"Loading bad event files ..."<<endl;
    // updated lists for full dataset
    metFilterTxt.loadBadEventList("/nfs-6/userdata/mt2utils/csc2015_Dec01.txt");
    metFilterTxt.loadBadEventList("/nfs-6/userdata/mt2utils/ecalscn1043093_Dec01.txt");
    cout<<" ... finished!"<<endl;
  }

  h_nvtx_weights_ = 0;
  if (doNvtxReweight) {
    TFile* f_weights = new TFile("../babymaker/data/hists_reweight_zjets_Run2015B.root");
    TH1D* h_nvtx_weights_temp = (TH1D*) f_weights->Get("h_nVert_ratio");
    outfile_->cd();
    h_nvtx_weights_ = (TH1D*) h_nvtx_weights_temp->Clone("h_nvtx_weights");
    h_nvtx_weights_->SetDirectory(0);
    f_weights->Close();
    delete f_weights;
  }

  h_sig_nevents_ = 0;
  h_sig_avgweight_btagsf_ = 0;
  h_sig_avgweight_btagsf_heavy_UP_ = 0;
  h_sig_avgweight_btagsf_light_UP_ = 0;
  h_sig_avgweight_btagsf_heavy_DN_ = 0;
  h_sig_avgweight_btagsf_light_DN_ = 0;
  h_sig_avgweight_isr_ = 0;
  if ((doScanWeights || applyBtagSF) && 
      ((sample.find("T1") != std::string::npos) || (sample.find("T2") != std::string::npos) || (sample.find("T5") != std::string::npos) || (sample.find("TChiNeu") != std::string::npos) )) {
    std::string scan_name = sample;
    if (sample.find("T1") != std::string::npos) scan_name = sample.substr(0,6);
    else if (sample.find("T2-4bd") != std::string::npos) scan_name = sample.substr(0,6);
    else if (sample.find("T2") != std::string::npos) scan_name = sample.substr(0,4);
    else if (sample.find("T5qqqqWW_modified") != std::string::npos) scan_name = "T5qqqqWW_modified";
    else if (sample.find("T5") != std::string::npos) scan_name = sample.substr(0,8);
    else if (sample.find("TChiNeu") != std::string::npos) scan_name = sample.substr(0,7);
    TFile* f_nsig_weights = new TFile(Form("../babymaker/data/nsig_weights_%s.root",scan_name.c_str()));
    TH2D* h_sig_nevents_temp = (TH2D*) f_nsig_weights->Get("h_nsig");
    TH2D* h_sig_avgweight_btagsf_temp = (TH2D*) f_nsig_weights->Get("h_avg_weight_btagsf");
    TH2D* h_sig_avgweight_btagsf_heavy_UP_temp = (TH2D*) f_nsig_weights->Get("h_avg_weight_btagsf_heavy_UP");
    TH2D* h_sig_avgweight_btagsf_light_UP_temp = (TH2D*) f_nsig_weights->Get("h_avg_weight_btagsf_light_UP");
    TH2D* h_sig_avgweight_btagsf_heavy_DN_temp = (TH2D*) f_nsig_weights->Get("h_avg_weight_btagsf_heavy_DN");
    TH2D* h_sig_avgweight_btagsf_light_DN_temp = (TH2D*) f_nsig_weights->Get("h_avg_weight_btagsf_light_DN");
    TH2D* h_sig_avgweight_isr_temp = (TH2D*) f_nsig_weights->Get("h_avg_weight_isr");
    h_sig_nevents_ = (TH2D*) h_sig_nevents_temp->Clone("h_sig_nevents");
    h_sig_avgweight_btagsf_ = (TH2D*) h_sig_avgweight_btagsf_temp->Clone("h_sig_avgweight_btagsf");
    h_sig_avgweight_btagsf_heavy_UP_ = (TH2D*) h_sig_avgweight_btagsf_heavy_UP_temp->Clone("h_sig_avgweight_btagsf_heavy_UP");
    h_sig_avgweight_btagsf_light_UP_ = (TH2D*) h_sig_avgweight_btagsf_light_UP_temp->Clone("h_sig_avgweight_btagsf_light_UP");
    h_sig_avgweight_btagsf_heavy_DN_ = (TH2D*) h_sig_avgweight_btagsf_heavy_DN_temp->Clone("h_sig_avgweight_btagsf_heavy_DN");
    h_sig_avgweight_btagsf_light_DN_ = (TH2D*) h_sig_avgweight_btagsf_light_DN_temp->Clone("h_sig_avgweight_btagsf_light_DN");
    h_sig_avgweight_isr_ = (TH2D*) h_sig_avgweight_isr_temp->Clone("h_sig_avgweight_isr");
    h_sig_nevents_->SetDirectory(0);
    h_sig_avgweight_btagsf_->SetDirectory(0);
    h_sig_avgweight_btagsf_heavy_UP_->SetDirectory(0);
    h_sig_avgweight_btagsf_light_UP_->SetDirectory(0);
    h_sig_avgweight_btagsf_heavy_DN_->SetDirectory(0);
    h_sig_avgweight_btagsf_light_DN_->SetDirectory(0);
    h_sig_avgweight_isr_->SetDirectory(0);
    f_nsig_weights->Close();
    delete f_nsig_weights;
  }

  if (doLepEffVars) {
    setElSFfile("../babymaker/lepsf/kinematicBinSFele.root");
    setMuSFfile("../babymaker/lepsf/TnP_MuonID_NUM_LooseID_DENOM_generalTracks_VAR_map_pt_eta.root","../babymaker/lepsf/TnP_MuonID_NUM_MiniIsoTight_DENOM_LooseID_VAR_map_pt_eta.root");
    setVetoEffFile_fullsim("../babymaker/lepsf/vetoeff_emu_etapt_lostlep.root");  
  }
  
  if (applyLeptonSFfastsim && ((sample.find("T1") != std::string::npos) || (sample.find("T2") != std::string::npos) || (sample.find("T5") != std::string::npos))) {
    setElSFfile_fastsim("../babymaker/lepsf/sf_el_vetoCB_mini01.root");  
    setMuSFfile_fastsim("../babymaker/lepsf/sf_mu_looseID_mini02.root");  
    setVetoEffFile_fastsim("../babymaker/lepsf/vetoeff_emu_etapt_T1tttt_mGluino-1500to1525.root");  
  }

  // set up signal binning
  for (int i = 0; i <= n_m1bins; ++i) {
    m1bins[i] = i*25.;
  }
  for (int i = 0; i <= n_m2bins; ++i) {
    // 10 GeV binning for T2cc
    if (sample.find("T2-4bd") != std::string::npos) m2bins[i] = i*5.;
    else m2bins[i] = i*25.;
  }
  
  cout << "[MT2Looper::loop] setting up histos" << endl;

  SetSignalRegions();

  SRNoCut.SetName("nocut");
  float SRNoCut_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500}; 
  SRNoCut.SetMT2Bins(7, SRNoCut_mt2bins);

  // These will be set to true if any SoftLep, CR1L, or CR2L plots are produced
  bool saveSoftLplots = false;
  bool saveSoftLMuplots = false;
  bool saveSoftLElplots = false;
  bool save1Lplots = false;
  bool save1Lmuplots = false;
  bool save1Lelplots = false;
  bool save2Lplots = false;
  bool saveZllplots = false;

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
      //for( unsigned int event = 0; event < 10000; ++event) {
      
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
      //if (!t.Flag_CSCTightHaloFilter) continue; // use txt files instead
      if (!t.Flag_eeBadScFilter) continue; // txt files are in addition to this flag
      if (!t.Flag_HBHENoiseFilter) continue;
      if (!t.Flag_HBHEIsoNoiseFilter) continue;
      // txt MET filters (data only)
      if (t.isData && metFilterTxt.eventFails(t.run, t.lumi, t.evt)) {
	//cout<<"Found bad event in data: "<<t.run<<", "<<t.lumi<<", "<<t.evt<<endl;
	continue;
      }

      // Jet ID Veto
      bool passJetID = true;
      //if (t.nJet30FailId > 0) continue;
      if (t.nJet30FailId > 0) passJetID = false;

      // remove low pt QCD samples 
      if (t.evt_id >= 100 && t.evt_id < 109) continue;
      // remove low HT QCD samples 
      if (t.evt_id >= 120 && t.evt_id < 123) continue;
      if (t.evt_id >= 151 && t.evt_id < 154 && !calculateFakeRates) continue; // if calculating FR, drop these later

      // flag signal samples
      if (t.evt_id >= 1000) isSignal_ = true;
      else isSignal_ = false;

      //if (isSignal_ && (t.GenSusyMScan1 != 1600 || t.GenSusyMScan2 != 0)) continue;
      
      // note: this will double count some leptons, since reco leptons can appear as PFcands
      nlepveto_ = t.nMuons10 + t.nElectrons10 + t.nPFLep5LowMT + t.nPFHad10LowMT;

      //---------------------
      // set weights and start making plots
      //---------------------
      outfile_->cd();
      const float lumi = 2.26;
      //const float lumi = 4;
      
      //only keep single mass point in scans
     if (isSignal_ 
	  // && !(t.GenSusyMScan1 == 275 && t.GenSusyMScan2 == 235 && sample  == "T2-4bd_275")
	  && !(t.GenSusyMScan1 == 100 && t.GenSusyMScan2 == 90 && sample  == "TChiNeu_100_90")
	  && !(t.GenSusyMScan1 == 300 && t.GenSusyMScan2 == 285 && sample  == "TChiNeu_300_285")
	  && !(t.GenSusyMScan1 == 375 && t.GenSusyMScan2 == 295 && sample  == "T2-4bd_375_295")
	  && !(t.GenSusyMScan1 == 375 && t.GenSusyMScan2 == 335 && sample  == "T2-4bd_375_335")
	  && !(t.GenSusyMScan1 == 375 && t.GenSusyMScan2 == 355 && sample  == "T2-4bd_375_355")
	  && !(t.GenSusyMScan1 == 375 && t.GenSusyMScan2 == 365 && sample  == "T2-4bd_375_365")
	  && !(t.GenSusyMScan1 == 275 && t.GenSusyMScan2 == 195 && sample  == "T2-4bd_275_195")
	  && !(t.GenSusyMScan1 == 275 && t.GenSusyMScan2 == 235 && sample  == "T2-4bd_275_235")
	  && !(t.GenSusyMScan1 == 275 && t.GenSusyMScan2 == 255 && sample  == "T2-4bd_275_255")
	  && !(t.GenSusyMScan1 == 275 && t.GenSusyMScan2 == 265 && sample  == "T2-4bd_275_265")
	  && !(t.GenSusyMScan1 == 1025 && t.GenSusyMScan2 == 775 && (sample  == "T5qqqqWW_1025_775_custom" || sample  == "1025_775_T5qqqqWW_modified" || sample  == "T5qqqqWW_1025_775_old"))
   	  && !(t.GenSusyMScan1 == 1100 && t.GenSusyMScan2 == 500 && (sample  == "T5qqqqWW_1100_500_custom" || sample  == "1100_500_T5qqqqWW_modified" || sample  == "T5qqqqWW_1100_500_old"))
	  && !(t.GenSusyMScan1 == 1300 && t.GenSusyMScan2 == 600 && sample  == "T5qqqqWW_1300_600")
	  && !(t.GenSusyMScan1 == 1500 && t.GenSusyMScan2 == 100 && sample  == "T5qqqqWW_1500_100")
         ) continue;

      evtweight_ = 1.;
      
      // apply relevant weights to MC
      if (!t.isData) {
	if (isSignal_ && doScanWeights && sample  == "TChiNeu_100_90") {
	  evtweight_ = lumi * 22670.1 / nEvents; // assumes xsec is already filled correctly for mChargino = 100
	}
	else if (isSignal_ && doScanWeights && sample  == "TChiNeu_300_285") {
	  evtweight_ = lumi * 386.936 * 50 / nEvents; // assumes xsec is already filled correctly for mChargino = 300, scales up by 50x
	}
	else if (isSignal_ && doScanWeights) {
	  int binx = h_sig_nevents_->GetXaxis()->FindBin(t.GenSusyMScan1);
	  int biny = h_sig_nevents_->GetYaxis()->FindBin(t.GenSusyMScan2);
	  double nevents = h_sig_nevents_->GetBinContent(binx,biny);
	  evtweight_ = lumi * t.evt_xsec*1000./nevents; // assumes xsec is already filled correctly
	} else {
	  evtweight_ = t.evt_scale1fb * lumi;
	}
	if (applyBtagSF && (sample.find("TChi") == std::string::npos)) {
	  // remove events with 0 btag weight for now..
	  if (fabs(t.weight_btagsf) < 0.001) continue;
	  evtweight_ *= t.weight_btagsf;
	  if (isSignal_) {
	    int binx = h_sig_avgweight_btagsf_->GetXaxis()->FindBin(t.GenSusyMScan1);
	    int biny = h_sig_avgweight_btagsf_->GetYaxis()->FindBin(t.GenSusyMScan2);
	    float avgweight_btagsf = h_sig_avgweight_btagsf_->GetBinContent(binx,biny);
	    evtweight_ /= avgweight_btagsf;
	  }
	}
	// get pu weight from hist, restrict range to nvtx 4-31
	if (doNvtxReweight) {
	  int nvtx_input = t.nVert;
	  if (t.nVert > 31) nvtx_input = 31;
	  if (t.nVert < 4) nvtx_input = 4;
	  float puWeight = h_nvtx_weights_->GetBinContent(h_nvtx_weights_->FindBin(nvtx_input));
	  evtweight_ *= puWeight;
	}
//MT2	if (isSignal_ && applyLeptonSFfastsim && nlepveto_ == 0) {
//MT2	  fillLepCorSRfastsim();
//MT2	  evtweight_ *= (1. + cor_lepeff_sr_);
//MT2	}
//MT2	else if (doLepEffVars && nlepveto_ == 0) fillLepUncSR();
	
	// always apply SF (need to add the Fastsim ones to babies)
	// will have to revert later when using UP ar DN variations 
	if (applyLeptonSF) {
	  evtweight_ *= t.weight_lepsf;
	  evtweight_lepEffUp_ = evtweight_ / t.weight_lepsf * t.weight_lepsf_UP;
	  evtweight_lepEffDn_ = evtweight_ / t.weight_lepsf * t.weight_lepsf_DN;
	  //cout<<"lepSF is "<<t.weight_lepsf<<", "<<t.weight_lepsf_UP<<", "<<t.weight_lepsf_DN<<endl;

	}

	if (applyTopPtReweight && t.evt_id >= 300 && t.evt_id < 400) {
	  evtweight_ *= t.weight_toppt;
	}

      } // !isData

      plot1D("h_nvtx",       t.nVert,       evtweight_, h_1d_global, ";N(vtx)", 80, 0, 80);
      plot1D("h_mt2",       t.mt2,       evtweight_, h_1d_global, ";M_{T2} [GeV]", 80, 0, 800);

      // count number of forward jets
      nJet30Eta3_ = 0;
      for (int ijet = 0; ijet < t.njet; ++ijet) {
	if (t.jet_pt[ijet] > 30. && fabs(t.jet_eta[ijet]) > 3.0) ++nJet30Eta3_;
      }

      // veto on forward jets
      if (doHFJetVeto && nJet30Eta3_ > 0) continue;

      // check jet id for monojet
      passMonojetId_ = false;
      if (t.nJet30 >= 1) {
	// loop to find central (highest pt) monojet
	for (int ijet = 0; ijet < t.njet; ++ijet) {
	  if (fabs(t.jet_eta[ijet]) > 2.5) continue;
	  if (t.jet_pt[ijet] < 200.) continue;
	  if (t.jet_id[ijet] >= 4) passMonojetId_ = true;
	  break;
	}
      }

      //-------------------------//
      //-------Soft Lepton-------//
      //-------------------------//

      //initialize
      bool doSoftLepSRplots = false;
      bool doSoftLepMuSRplots = false;
      bool doSoftLepElSRplots = false;
      bool doSoftLepCRplots = false;
      bool doSoftLepMuCRplots = false;
      bool doSoftLepElCRplots = false;
      bool doDoubleLepCRplots = false;
      bool doHardDoubleLepCRplots = false;
      bool doZllCRplots = false;
      softlepIdx_ = -1;
      softleppt_ = -1;
      softlepeta_ = -1;
      softlepphi_ = -1;
      softlepM_ = -1;
      softlepCharge_ = -1;
      softlepId_ = -1;
      softlepmt_ = -1;
      hardlepIdx_ = -1;
      hardleppt_ = -1;
      hardlepeta_ = -1;
      hardlepphi_ = -1;
      hardlepM_ = -1;
      hardlepCharge_ = -1;
      hardlepId_ = -1;
      hardlepmt_ = -1;
      hardlep2Idx_ = -1;
      hardlep2pt_ = -1;
      hardlep2eta_ = -1;
      hardlep2phi_ = -1;
      hardlep2M_ = -1;
      hardlep2Charge_ = -1;
      hardlep2Id_ = -1;
      hardlep2mt_ = -1;
      bool passIsoId = false;
      bool passHardIsoId = false;
      // some variables relevant for CR2L
      bool isDilepton = false;
      if (t.ngenLep + t.ngenTau >= 2) isDilepton = true;      
      bool isEE = false;
      bool isMuMu = false;
      removedzllLep_ = 0;
      zllmt_ = -1;
      zllmet_ = -1;

      //counter to find highest pT isolated track
      int idxTrk = -1;
      float maxTrkPt = 0;
      for (int itrk = 0; itrk < t.nisoTrack; ++itrk) {
      	float pt = t.isoTrack_pt[itrk];
      	if (pt < 10.) continue;
      	int pdgId = t.isoTrack_pdgId[itrk];
      	if (abs(pdgId) != 211) continue;
      	if (t.isoTrack_absIso[itrk]/pt > 0.1) continue;

      	//overlap removal with reco leps
      	bool overlap = false;
      	for(int ilep = 0; ilep < t.nlep; ilep++){
      	  float thisDR = DeltaR(t.isoTrack_eta[itrk], t.lep_eta[ilep], t.isoTrack_phi[itrk], t.lep_phi[ilep]);
      	  if (thisDR < 0.01) {
      	    overlap = true;
      	    break;
      	  }
      	} // loop over reco leps
      	if (overlap) continue;

      	if (pt > maxTrkPt){
      	  maxTrkPt = pt;
      	  idxTrk = itrk;
      	}
	
      }

      
      //-------------------------------------------//
      //-------Find 1 soft and/or 1 hard lepton-------//
      //-------------------------------------------//

      bool foundsoftlep = false;
      bool foundhardlep = false;
      bool foundhardlep2 = false;

      // select soft lepton
      for (int ilep = 0; ilep < t.nlep; ++ilep) {
	if ( t.lep_pt[ilep] > 20 || t.lep_pt[ilep] < 5 ) continue;
	if ( abs(t.lep_eta[ilep])>1.479 ) continue;
	//iso/id requirements
	if (abs(t.lep_pdgId[ilep]) == 13 && t.lep_miniRelIso[ilep]<0.1 && t.lep_relIso03[ilep]<0.2 && abs(t.lep_dxy[ilep])< 0.02 && abs(t.lep_dz[ilep]) < 0.02) passIsoId = true;
	if (abs(t.lep_pdgId[ilep]) == 11 && t.lep_miniRelIso[ilep]<0.1 && t.lep_relIso03[ilep]<0.2 && t.lep_tightIdNoIso[ilep] > 0) passIsoId = true;
	float mt = sqrt( 2 * t.met_pt * t.lep_pt[ilep] * ( 1 - cos( t.met_phi - t.lep_phi[ilep]) ) );

	softlepIdx_ = ilep;
	softleppt_ = t.lep_pt[ilep];
	softlepeta_ = t.lep_eta[ilep];
	softlepphi_ = t.lep_phi[ilep];
	softlepM_ = t.lep_mass[ilep];
	softlepCharge_ = t.lep_charge[ilep];
	softlepId_ = t.lep_pdgId[ilep];
	softlepmt_ = mt;

	foundsoftlep = true;
	break;
      }

      // select hard lepton
      for (int ilep = 0; ilep < t.nlep; ++ilep) {
	if ( ilep == softlepIdx_) continue;
	if ( t.lep_pt[ilep] < 25 ) continue;
	if ( abs(t.lep_eta[ilep])>1.479 ) continue; 
	//iso/id requirements
	if (abs(t.lep_pdgId[ilep]) == 13 && (t.lep_miniRelIso[ilep]<0.1 && t.lep_relIso03[ilep]<0.2 && abs(t.lep_dxy[ilep])< 0.02 && abs(t.lep_dz[ilep]) < 0.02)) passHardIsoId = true;
	if (abs(t.lep_pdgId[ilep]) == 11 && (t.lep_miniRelIso[ilep]<0.1 && t.lep_relIso03[ilep]<0.1 && t.lep_tightIdNoIso[ilep] >= 2 && t.lep_relIso03[ilep]*t.lep_pt[ilep]<5)) passHardIsoId = true;
	float mt = sqrt( 2 * t.met_pt * t.lep_pt[ilep] * ( 1 - cos( t.met_phi - t.lep_phi[ilep]) ) );
	hardlepIdx_ = ilep;
	hardleppt_ = t.lep_pt[ilep];
	hardlepeta_ = t.lep_eta[ilep];
	hardlepphi_ = t.lep_phi[ilep];
	hardlepM_ = t.lep_mass[ilep];
	hardlepCharge_ = t.lep_charge[ilep];
	hardlepId_ = t.lep_pdgId[ilep];
	hardlepmt_ = mt;
	
	foundhardlep = true;
	break;
      }

      // select second hard lepton
      for (int ilep = 0; ilep < t.nlep; ++ilep) {
	if ( ilep == hardlepIdx_) continue;
	if ( t.lep_pt[ilep] < 25 ) continue;
	if ( abs(t.lep_eta[ilep])>1.479 ) continue; 
	//iso/id requirements
	if (abs(t.lep_pdgId[ilep]) == 13 && (t.lep_miniRelIso[ilep]>0.1 || t.lep_relIso03[ilep]>0.2 || abs(t.lep_dxy[ilep])> 0.02 || abs(t.lep_dz[ilep]) > 0.02)) continue;
	if (abs(t.lep_pdgId[ilep]) == 11 && (t.lep_miniRelIso[ilep]>0.1 || t.lep_relIso03[ilep]>0.1 || t.lep_tightIdNoIso[ilep] < 2 ||  t.lep_relIso03[ilep]*t.lep_pt[ilep]>5)) continue;
	float mt = sqrt( 2 * t.met_pt * t.lep_pt[ilep] * ( 1 - cos( t.met_phi - t.lep_phi[ilep]) ) );
	hardlep2Idx_ = ilep;
	hardlep2pt_ = t.lep_pt[ilep];
	hardlep2eta_ = t.lep_eta[ilep];
	hardlep2phi_ = t.lep_phi[ilep];
	hardlep2M_ = t.lep_mass[ilep];
	hardlep2Charge_ = t.lep_charge[ilep];
	hardlep2Id_ = t.lep_pdgId[ilep];
	hardlep2mt_ = mt;
	
	foundhardlep2 = true;
	break;
      }


      //Can select an additonal hard lepton for a 2-hard lepton CR here

      //-------------------------------------//
      //-------find lost lepton--------------//
      //-------------------------------------//
      bool foundMissingLep = false;
      bool foundMissingLepFromTau = false;
      foundMissingTau_ = false;
      missIdx_ = -1;
      missPt_ = -1;
      missEta_ = 999;
      int decayMode = -1;
      
      if (!t.isData && foundsoftlep && !foundhardlep) { // we only care about this in SR

	for(int ilep = 0; ilep < t.ngenLep; ilep++){
	  float thisDR = DeltaR(t.genLep_eta[ilep], softlepeta_, t.genLep_phi[ilep], softlepphi_);
	  if (thisDR > 0.1) {
	    missIdx_ = ilep;
	    missPt_ = t.genLep_pt[ilep];	
	    missEta_ = t.genLep_eta[ilep];	
	    foundMissingLep = true;
	    //construct dilep mass
	    TLorentzVector misslep_p4(0,0,0,0);
	    TLorentzVector softlep_p4(0,0,0,0);
	    misslep_p4.SetPtEtaPhiM(t.genLep_pt[ilep],t.genLep_eta[ilep],t.genLep_phi[ilep],t.genLep_mass[ilep]);
	    softlep_p4.SetPtEtaPhiM(softleppt_,softlepeta_,softlepphi_,softlepM_);
	    TLorentzVector dilep_p4 = misslep_p4 + softlep_p4;
	    dilepmll_ = dilep_p4.M();
	    break;
	  }
	}//ngenLep

	if (!foundMissingLep) {
	  for(int ilep = 0; ilep < t.ngenLepFromTau; ilep++){
	    float thisDR = DeltaR(t.genLepFromTau_eta[ilep], softlepeta_, t.genLepFromTau_phi[ilep], softlepphi_);
	    if (thisDR > 0.1) {
	      missIdx_ = ilep;
	      missPt_ = t.genLepFromTau_pt[ilep];
	      missEta_ = t.genLepFromTau_eta[ilep];	
	      foundMissingLepFromTau = true;
	      //construct dilep mass
	      TLorentzVector misslep_p4(0,0,0,0);
	      TLorentzVector softlep_p4(0,0,0,0);
	      misslep_p4.SetPtEtaPhiM(t.genLepFromTau_pt[ilep],t.genLepFromTau_eta[ilep],t.genLepFromTau_phi[ilep],t.genLepFromTau_mass[ilep]);
	      softlep_p4.SetPtEtaPhiM(softleppt_,softlepeta_,softlepphi_,softlepM_);
	      TLorentzVector dilep_p4 = misslep_p4 + softlep_p4;
	      dilepmll_ = dilep_p4.M();
	      break;
	    }
	  }//ngenLepFromTau
	}
	
	if (!foundMissingLep && !foundMissingLepFromTau) {
	  for(int ilep = 0; ilep < t.ngenTau; ilep++){
	    float thisDR = DeltaR(t.genTau_eta[ilep], softlepeta_, t.genTau_phi[ilep], softlepphi_);
	    if (thisDR > 0.1) {
	      missIdx_ = ilep;
	      missPt_ = t.genTau_leadTrackPt[ilep];
	      foundMissingTau_ = true;
	      //construct dilep mass
	      TLorentzVector misslep_p4(0,0,0,0);
	      TLorentzVector softlep_p4(0,0,0,0);
	      misslep_p4.SetPtEtaPhiM(t.genTau_pt[ilep],t.genTau_eta[ilep],t.genTau_phi[ilep],t.genTau_mass[ilep]);
	      softlep_p4.SetPtEtaPhiM(softleppt_,softlepeta_,softlepphi_,softlepM_);
	      TLorentzVector dilep_p4 = misslep_p4 + softlep_p4;
	      dilepmll_ = dilep_p4.M();

	      decayMode = t.genTau_decayMode[ilep];
	      
	      break;
	    }
	  }//ngenTau
	}
	
      } // find lost lepton if (!t.isData && foundsoftlep && !foundhardlep)

      // Scale factors (and uncertainties) for SR events with a lost lepton
      if ( !t.isData && (foundMissingLep || foundMissingLepFromTau) && ( applyLeptonSFfastsim || applyLeptonSF ) &&  foundsoftlep && !foundhardlep) {
	  bool fastsim = isSignal_ && applyLeptonSFfastsim;
	  float lostsf = 1.;
	  float lostsf_up = 1.;
	  float lostsf_dn = 1.;
	  
	  if (missIdx_ != -1 && missPt_ > 5 && fabs(missEta_) < 2.4) {
	    fillMissLepSF(missIdx_, foundMissingLepFromTau, fastsim, lostsf, lostsf_up, lostsf_dn);
	    //cout<<"found lost lep SF weights to be "<<lostsf<<", "<<lostsf_up<<", "<<lostsf_dn<<endl;
	    evtweight_ *= lostsf;
	    evtweight_lepEffUp_ *= lostsf_up;
	    evtweight_lepEffDn_ *= lostsf_dn;
	  }
      }

      //---------------------------------------//
      //-------Soft SR/CR1L/CR2L Regions-------//
      //---------------------------------------//



      if ( foundsoftlep && !foundhardlep && t.nlep == 1) { // SR
	doSoftLepSRplots = true;
	if (abs(softlepId_) == 13) doSoftLepMuSRplots = true;
	if (abs(softlepId_) == 11) doSoftLepElSRplots = true;
      }
      else if ( !foundsoftlep && foundhardlep && t.nlep == 1) { // CR1L
	//additional cuts for 1-lep CR  
	if (t.HLT_SingleMu || t.HLT_SingleEl) {

	  // to draw all the usual plots, fill the soft lepton variables with the hardlepton values
	  // this only makes sense in CR1L, where there is NO soft lepton
	  softlepIdx_   = hardlepIdx_    ;
	  softleppt_    = hardleppt_     ;
	  softlepeta_   = hardlepeta_    ;
	  softlepphi_   = hardlepphi_    ;
	  softlepM_     = hardlepM_      ;
	  softlepCharge_= hardlepCharge_ ;
	  softlepId_    = hardlepId_     ;
	  softlepmt_    = hardlepmt_     ;

	  if (abs(softlepId_) == 13) doSoftLepMuCRplots = true;
	  if (abs(softlepId_) == 11) doSoftLepElCRplots = true;

	  //add muon only requirement to CR1L
	  if (abs(softlepId_) == 13) doSoftLepCRplots = true;

	}// additional cuts for CR1L 
      }// CR1L
      else if ( foundsoftlep && foundhardlep && t.nlep == 2) { // CR2L
	//opposite sign
	if ( (softlepCharge_ != hardlepCharge_) ) {
	  if ( abs(softlepId_)==11 && abs(hardlepId_)==11) isEE = true;
	  else if ( abs(softlepId_)==13 && abs(hardlepId_)==13) isMuMu = true;
	  //construct dilep mass
	  TLorentzVector lep1_p4(0,0,0,0);
	  TLorentzVector lep2_p4(0,0,0,0);
	  lep1_p4.SetPtEtaPhiM(softleppt_,softlepeta_,softlepphi_,softlepM_);
	  lep2_p4.SetPtEtaPhiM(hardleppt_,hardlepeta_,hardlepphi_,hardlepM_);
	  TLorentzVector dilep_p4 = lep1_p4 + lep2_p4;
	  dilepmll_ = dilep_p4.M();
	  doDoubleLepCRplots = true;
	}// additional cuts for CR2L
      }// CR2L
      else if ( !foundsoftlep && foundhardlep && foundhardlep2 && t.nlep == 2) { // HardDileptonCR
	
	//opposite sign
	if ( (hardlep2Charge_ != hardlepCharge_) ) {
	  // the easiest thing to do is to use the standard softlep and hardlep variables, so that all the plots can just work
	  softlepIdx_    = hardlepIdx_   ;    	  hardlepIdx_    = hardlep2Idx_   ;
	  softleppt_     = hardleppt_    ;    	  hardleppt_     = hardlep2pt_    ;
	  softlepeta_    = hardlepeta_   ;    	  hardlepeta_    = hardlep2eta_   ;
	  softlepphi_    = hardlepphi_   ;    	  hardlepphi_    = hardlep2phi_   ;
	  softlepM_      = hardlepM_     ;    	  hardlepM_      = hardlep2M_     ;
	  softlepCharge_ = hardlepCharge_;    	  hardlepCharge_ = hardlep2Charge_;
	  softlepId_     = hardlepId_    ;    	  hardlepId_     = hardlep2Id_    ;
	  softlepmt_     = hardlepmt_    ;    	  hardlepmt_     = hardlep2mt_    ;

	  if ( abs(softlepId_)==11 && abs(hardlepId_)==11) isEE = true;
	  else if ( abs(softlepId_)==13 && abs(hardlepId_)==13) isMuMu = true;
	  //construct dilep mass
	  TLorentzVector lep1_p4(0,0,0,0);
	  TLorentzVector lep2_p4(0,0,0,0);
	  lep1_p4.SetPtEtaPhiM(softleppt_,softlepeta_,softlepphi_,softlepM_);
	  lep2_p4.SetPtEtaPhiM(hardleppt_,hardlepeta_,hardlepphi_,hardlepM_);
	  TLorentzVector dilep_p4 = lep1_p4 + lep2_p4;
	  dilepmll_ = dilep_p4.M();
	  doHardDoubleLepCRplots = true;
	}// additional cuts for CR2L
      }// HardDileptonCR

      if ( ((doDoubleLepCRplots && passIsoId) || doHardDoubleLepCRplots) && passHardIsoId ) { //ZllCR, overlapping with CR2Ls
	//OSSF & mll
	if ( dilepmll_ > 75 && dilepmll_ < 105 && (isEE || isMuMu) ) {
	  //initialize MET variables
	  float metX = t.met_pt * cos(t.met_phi);
	  float metY = t.met_pt * sin(t.met_phi);
	  TVector2* newMet = new TVector2;
	  
	  //remove random lepton, depending on event number
	  if (t.evt % 2 == 1) {
	    removedzllLep_ = 1; 
	    float lepX = hardleppt_ * cos(hardlepphi_);
	    float lepY = hardleppt_ * sin(hardlepphi_);
	    newMet->SetX(metX+lepX);
	    newMet->SetY(metY+lepY);
	  }
	  else {
	    removedzllLep_ = 2;
	    float lepX = softleppt_ * cos(softlepphi_);
	    float lepY = softleppt_ * sin(softlepphi_);
	    newMet->SetX(metX+lepX);
	    newMet->SetY(metY+lepY);
	  }
	  
	  zllmet_ = newMet->Mod(); //alternate MET
	  if (removedzllLep_ == 1) zllmt_ = sqrt( 2 * newMet->Mod() * softleppt_ * ( 1 - cos( newMet->Phi() - softlepphi_) ) ); //recalculated MT
	  else if (removedzllLep_ == 2) zllmt_ = sqrt( 2 * newMet->Mod() * hardleppt_ * ( 1 - cos( newMet->Phi() - hardlepphi_) ) ); //recalculated MT
   
	  doZllCRplots = true;
	}// additional cuts for ZllCR
      }// ZllCR
      
      //-------------------------------------//
      //-----dR matching for soft lepton-----//
      //-------------------------------------//
      
      bool softlepMatched = false;
      if (!t.isData && (foundsoftlep || foundhardlep)){
	double minDR = 999;
	for(int ilep = 0; ilep < t.ngenLep; ilep++){
	  if (abs(t.genLep_pdgId[ilep]) != abs(softlepId_)) continue;
	  float thisDR = DeltaR(t.genLep_eta[ilep], softlepeta_, t.genLep_phi[ilep], softlepphi_);
	  if (thisDR < minDR) minDR = thisDR;
	}
	if (minDR < 0.5) softlepMatched = true;

	//if still not matched, check genLepFromTau
	if (!softlepMatched){
	  minDR = 999;
	  for(int ilep = 0; ilep < t.ngenLepFromTau; ilep++){
	    if (abs(t.genLepFromTau_pdgId[ilep]) != abs(softlepId_)) continue;
	    float thisDR = DeltaR(t.genLepFromTau_eta[ilep], softlepeta_, t.genLepFromTau_phi[ilep], softlepphi_);
	    if (thisDR < minDR) minDR = thisDR;
	  }
	  if (minDR < 0.5) softlepMatched = true;
	}

	if(!softlepMatched){
	  if (abs(t.lep_mcMatchId[softlepIdx_]) == 24) softlepMatched = true;
	}
	
      }//if !t.isData && foundsoftlep
      
      // Inclusive plots
      bool passBaseline = (t.met_pt > 200 && t.ht > 200 && t.deltaPhiMin > 0.3 && t.diffMetMht/t.met_pt < 0.5 && t.nlepIso+t.nPFHad10 == 1);
      if (passBaseline && foundsoftlep) {
	plot1D("h_leppt",       softleppt_,       evtweight_, h_1d_global, ";p_{T} [GeV]", 500, 0, 500);
	plot1D("h_mt",    softlepmt_,   evtweight_, h_1d_global, ";M_{T} [GeV]", 250, 0, 250);
      }
      else if (passBaseline && foundhardlep) plot1D("h_leppt",       hardleppt_,       evtweight_, h_1d_global, ";p_{T} [GeV]", 500, 0, 500);
      
      //----------------------------------------//
      //--------Make plots for Fake Rate--------//
      //----------------------------------------//
      bool passFRTrig = false;

      // trigger requirement on data
      if (calculateFakeRates) {
	if (!t.isData) { if (t.HLT_PFHT900 || t.HLT_PFHT800 || t.HLT_PFHT600_Prescale || t.HLT_PFHT475_Prescale || t.HLT_PFHT350_Prescale || t.HLT_PFHT200_Prescale) passFRTrig = true;}
	else if (t.HLT_PFHT800 && t.ht > 1000) {passFRTrig = true;}
	else if (t.HLT_PFHT600_Prescale && t.ht > 700) {if (doPrescaleWeight) evtweight_ *= t.HLT_PFHT600_Prescale;passFRTrig = true;}
	else if (t.HLT_PFHT475_Prescale && t.ht > 575) {if (doPrescaleWeight) evtweight_ *= t.HLT_PFHT475_Prescale;passFRTrig = true;}
	else if (t.HLT_PFHT350_Prescale && t.ht > 450) {if (doPrescaleWeight) evtweight_ *= t.HLT_PFHT350_Prescale;passFRTrig = true;}
	else if (t.HLT_PFHT200_Prescale && t.ht > 300) {if (doPrescaleWeight) evtweight_ *= t.HLT_PFHT200_Prescale;passFRTrig = true;}
      }
      

      if (t.nlep >=1 && calculateFakeRates && passFRTrig && t.ht > 300 && foundsoftlep) {
	
	TString suffix = "";
	TString HTsuffix = "";
	
	if (((t.isData && t.HLT_PFHT800) || (!t.isData && t.HLT_PFHT900))  && t.ht > 1000 ) {HTsuffix += "HT800";}
	else if (t.HLT_PFHT600_Prescale && t.ht > 700) {HTsuffix += "HT600";}
	else if (((t.isData && t.HLT_PFHT475_Prescale) || !t.isData) && t.ht > 575) {HTsuffix += "HT475";}
	else if (((t.isData && t.HLT_PFHT350_Prescale) || !t.isData) && t.ht > 450) {HTsuffix += "HT350";}
	else if (((t.isData && t.HLT_PFHT200_Prescale) || !t.isData) && t.ht > 300) {HTsuffix += "HT200";}
	
	if (abs(softlepId_) == 13) suffix += "Mu";
	if (abs(softlepId_) == 11) suffix += "El";
	
	suffix += "Loose";
	
	if (!t.isData && !softlepMatched) suffix += "Fake";

	string suffixString = suffix.Data();
	string HTsuffixString = HTsuffix.Data();
	
	
	if (t.met_pt < 200) {
	  
	  //fill plots in HT bins
	  fillHistosSingleSoftLepton(SRNoCut.srHistMap , SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), HTsuffixString+suffixString);
	  fillHistosSingleSoftLepton(SRNoCut.srHistMap , SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), suffixString);
	  if (t.met_pt < 60 && softlepmt_ < 30) {
	    fillHistosSingleSoftLepton(SRNoCut.srHistMap , SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), "EWKcuts"+HTsuffixString+suffixString);
	    fillHistosSingleSoftLepton(SRNoCut.srHistMap , SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), "EWKcuts"+suffixString);
	  }
	  if (passIsoId) {
	    suffix.ReplaceAll("Loose","Tight");
	    suffixString = suffix.Data();
	    fillHistosSingleSoftLepton(SRNoCut.srHistMap , SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), HTsuffixString+suffixString);
	    fillHistosSingleSoftLepton(SRNoCut.srHistMap , SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), suffixString);
	    if (t.met_pt < 60 && softlepmt_ < 30) {
	      fillHistosSingleSoftLepton(SRNoCut.srHistMap , SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), "EWKcuts"+HTsuffixString+suffixString);
	      fillHistosSingleSoftLepton(SRNoCut.srHistMap , SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), "EWKcuts"+suffixString);
	    }
	    else if (t.met_pt > 40) { // needed to normalize the EWK contribution
	      fillHistosSingleSoftLepton(SRNoCut.srHistMap , SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), "MET40"+HTsuffixString+suffixString);
	      fillHistosSingleSoftLepton(SRNoCut.srHistMap , SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), "MET40"+suffixString);
	    }
	  }
	  else {
	    suffix.ReplaceAll("Loose","LooseNotTight");
	    suffixString = suffix.Data();
	    fillHistosSingleSoftLepton(SRNoCut.srHistMap , SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), HTsuffixString+suffixString);
	    fillHistosSingleSoftLepton(SRNoCut.srHistMap , SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), suffixString);
	    if (t.met_pt < 60 && softlepmt_ < 30) {
	      fillHistosSingleSoftLepton(SRNoCut.srHistMap , SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), "EWKcuts"+HTsuffixString+suffixString);
	      fillHistosSingleSoftLepton(SRNoCut.srHistMap , SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), "EWKcuts"+suffixString);
	    }
	  }	    
	}//met < 200
      } // Fake rate plots	
      
      // Only needed these to calculate the FR. Drop them for everything else.
      if (t.evt_id >= 151 && t.evt_id < 154 && calculateFakeRates) continue;
      
      //compute invariant mass between softlep and highest pT isolated (veto) track
      dilepmllTrack_ = -1;
      if (doSoftLepSRplots && t.nPFHad10 > 0){
	TLorentzVector track_p4(0,0,0,0);
	TLorentzVector softlep_p4(0,0,0,0);
	track_p4.SetPtEtaPhiM(t.isoTrack_pt[idxTrk],t.isoTrack_eta[idxTrk],t.isoTrack_phi[idxTrk],t.isoTrack_mass[idxTrk]);
	softlep_p4.SetPtEtaPhiM(softleppt_,softlepeta_,softlepphi_,softlepM_);
	TLorentzVector dilep_p4 = track_p4 + softlep_p4;
	dilepmllTrack_ = dilep_p4.M();
      }

      //find the highest-pT b-jet and count soft/hard b-jets
      bjetPt_ = -1;
      nSoftB_ = 0;
      nHardB_ = 0;
      for(int ijet = 0; ijet < t.njet; ijet++){
	if (!(fabs(t.jet_eta[ijet]) < 2.5)) continue;
	if (!(t.jet_btagCSV[ijet] >= 0.89)) continue;

	if (t.jet_pt[ijet] > 50) nHardB_++;
	else nSoftB_++;
	    
	if (t.jet_pt[ijet] > bjetPt_) bjetPt_ = t.jet_pt[ijet];
      }
      // define b-category
      categoryB_ = -1;
      if (nSoftB_ + nHardB_ > 2) categoryB_ = 3.5;
      else if (nHardB_ > 0) categoryB_ = 2.5;
      else if (nSoftB_ > 0) categoryB_ = 1.5;
      else categoryB_ = 0.5;
      
      ////////////////////////////////////
      /// done with overall selection  /// 
      ////////////////////////////////////
      ///   time to fill histograms    /// 
      ////////////////////////////////////
      onlyMakeFRplots_ = false;

//      doSoftLepSRplots = false;
//      doDoubleLepCRplots = false;

      if (!passJetID) continue;
      
      if ( !(t.isData && doBlindData && t.mt2 > 200) ) {
	// fillHistos(SRNoCut.srHistMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), "");

	// fillHistosSignalRegion("sr");

	// fillHistosSRBase();
	// fillHistosInclusive();
      }

      if (doSoftLepSRplots && !(t.isData && doBlindData) ) {
        saveSoftLplots = true;
	// Tight Leptons (no data driven fakes):
	if (passIsoId) {
	  fillHistosLepSignalRegions("srLep"); // inclusive plot for Data and MC
	  // Truth matching for MC
	  if (!t.isData) {
	    if (softlepMatched && isDilepton) fillHistosLepSignalRegions("srLep", "Dilepton");
	    else if (softlepMatched) fillHistosLepSignalRegions("srLep", "Onelep");
	    else fillHistosLepSignalRegions("srLep", "Fake");
	  }
	}
	// Additional plots to apply Fake Rate, divided in e/mu and Tigth/LooseNotTight
	// For the Loose and LooseNotTight plots, there is not much of a point in plotting too many variables, so stick with FR variables only
	if (applyFakeRates) {

	  onlyMakeFRplots_ = true; // we just need the MTvsLepPt plot, in each SR.

	  string s = "";
	  if (doSoftLepElSRplots) s += "El";
	  else if (doSoftLepMuSRplots) s += "Mu";

	  //first fill, for Loose
	  fillHistosLepSignalRegions("srLep", s+"Loose");
	  if (!t.isData) {
	    if (softlepMatched && isDilepton) fillHistosLepSignalRegions("srLep", s+"LooseDilepton");
	    else if (softlepMatched) fillHistosLepSignalRegions("srLep", s+"LooseOnelep");
	    else fillHistosLepSignalRegions("srLep", s+"LooseFake");
	  }

	  //if passed iso and id req, fill for tight
	  if (passIsoId) {
	    fillHistosLepSignalRegions("srLep", s+"Tight"); // inclusive plot for Data and MC
	    // Truth matching for MC
	    if (!t.isData) {
	      if (softlepMatched && isDilepton) fillHistosLepSignalRegions("srLep", s+"TightDilepton");
	      else if (softlepMatched) fillHistosLepSignalRegions("srLep", s+"TightOnelep");
	      else fillHistosLepSignalRegions("srLep", s+"TightFake");
	    }
	  }
	  //else, fill LooseNotTight
	  else {
	    fillHistosLepSignalRegions("srLep", s+"LooseNotTight");
	    if (!t.isData) {
	      if (softlepMatched && isDilepton) fillHistosLepSignalRegions("srLep", s+"LooseNotTightDilepton");
	      else if (softlepMatched) fillHistosLepSignalRegions("srLep", s+"LooseNotTightOnelep");
	      else fillHistosLepSignalRegions("srLep", s+"LooseNotTightFake");
	    }
	  }

	  onlyMakeFRplots_ = false; 
 
	}
	
      }

      if (doSoftLepSRplots && t.isData && doBlindData && applyFakeRates && !passIsoId) { // if blind, only look at LooseNotTight
	saveSoftLplots = true;
	onlyMakeFRplots_ = true; // we just need the MTvsLepPt plot, in each SR.
	string s = "";
	if (doSoftLepElSRplots) s += "El";
	else if (doSoftLepMuSRplots) s += "Mu";
	fillHistosLepSignalRegions("srLep", s+"LooseNotTight");
	onlyMakeFRplots_ = false; 
      }

      if (doSoftLepMuSRplots && !(t.isData && doBlindData) && !doCombineElMu) {
        saveSoftLMuplots = true;
	fillHistosLepSignalRegions("srLepMu"); 
	if (!t.isData) {
	  if (softlepMatched && isDilepton) fillHistosLepSignalRegions("srLepMu", "Dilepton");
	  else if (softlepMatched) fillHistosLepSignalRegions("srLepMu", "Onelep");
	  else fillHistosLepSignalRegions("srLepMu", "Fake");
	}
      }
      if (doSoftLepElSRplots && !(t.isData && doBlindData) && !doCombineElMu) {
        saveSoftLElplots = true;
	fillHistosLepSignalRegions("srLepEl"); 
	if (!t.isData) {
	  if (softlepMatched && isDilepton) fillHistosLepSignalRegions("srLepEl", "Dilepton");
	  else if (softlepMatched) fillHistosLepSignalRegions("srLepEl", "Onelep");
	  else fillHistosLepSignalRegions("srLepEl", "Fake");
	}
      }

      //loose, inclusive CR1L plots
      if (doSoftLepCRplots && !doMinimalPlots) {
      	save1Lplots = true;
	fillHistosCR1L("cr1L","Loose");
	if (!t.isData) {
	  if (softlepMatched && isDilepton) fillHistosCR1L("cr1L", "LooseDilepton");
	  else if (softlepMatched) fillHistosCR1L("cr1L","LooseOnelep");
	  else fillHistosCR1L("cr1L","LooseFake");
	}	
      }
      if (doSoftLepMuCRplots && !doMinimalPlots && !doCombineElMu) {
     	save1Lmuplots = true;
	fillHistosCR1L("cr1Lmu","Loose");
	if (!t.isData) {
	  if (softlepMatched && isDilepton) fillHistosCR1L("cr1Lmu", "LooseDilepton");
	  else if (softlepMatched) fillHistosCR1L("cr1Lmu","LooseOnelep"); 
	  else fillHistosCR1L("cr1Lmu","LooseFake");
	}
      }
      if (doSoftLepElCRplots && !doMinimalPlots && !doCombineElMu) {
     	save1Lelplots = true;
	fillHistosCR1L("cr1Lel","Loose");
	if (!t.isData) {
	  if (softlepMatched && isDilepton) fillHistosCR1L("cr1Lel", "LooseDilepton");
	  else if (softlepMatched) fillHistosCR1L("cr1Lel","LooseOnelep");
	  else fillHistosCR1L("cr1Lel","LooseFake");
	}
      }

      //tight CR1L plots
      if (passHardIsoId) {
	if (doSoftLepCRplots) {
	  save1Lplots = true;
	  fillHistosCR1L("cr1L");
	  if (!t.isData) {
	    if (softlepMatched && isDilepton) fillHistosCR1L("cr1L", "Dilepton");
	    else if (softlepMatched) fillHistosCR1L("cr1L","Onelep");
	    else fillHistosCR1L("cr1L","Fake");
	  }
	}
	if (doSoftLepMuCRplots && !doCombineElMu) {
	  save1Lmuplots = true;
	  fillHistosCR1L("cr1Lmu");
	  if (!t.isData) {
	    if (softlepMatched && isDilepton) fillHistosCR1L("cr1Lmu", "Dilepton");
	    else if (softlepMatched) fillHistosCR1L("cr1Lmu","Onelep"); 
	    else fillHistosCR1L("cr1Lmu","Fake");
	  }
	}
	if (doSoftLepElCRplots && !doCombineElMu) {
	  save1Lelplots = true;
	  fillHistosCR1L("cr1Lel");
	  if (!t.isData) {
	    if (softlepMatched && isDilepton) fillHistosCR1L("cr1Lel", "Dilepton");
	    else if (softlepMatched) fillHistosCR1L("cr1Lel","Onelep");
	    else fillHistosCR1L("cr1Lel","Fake");
	  }
	}
      }//passHardIsoId

      //tight CR2L plots
      if (doDoubleLepCRplots && passHardIsoId) {
        save2Lplots = true;	

	if (passIsoId) {
	  fillHistosDoubleL("cr2L");
	  if (!t.isData) {
	    if (isDilepton) fillHistosDoubleL("cr2L","Dilepton");
	    else fillHistosDoubleL("cr2L","Fake");
	  }
	  
	  if (!doCombineElMu){
	    if (isEE){
	      fillHistosDoubleL("cr2L","EE");
	      if (!t.isData) {
		if (isDilepton) fillHistosDoubleL("cr2L","EEDilepton");
		else fillHistosDoubleL("cr2L","EEFake");
	      }
	    }
	    else if (isMuMu){
	      fillHistosDoubleL("cr2L","MuMu");
	      if (!t.isData) {
		if (isDilepton) fillHistosDoubleL("cr2L","MuMuDilepton");
		else fillHistosDoubleL("cr2L","MuMuFake");
	      }
	    }
	    else{
	      fillHistosDoubleL("cr2L","EMu");
	      if (!t.isData) {
	      if (isDilepton) fillHistosDoubleL("cr2L","EMuDilepton");
	      else fillHistosDoubleL("cr2L","EMuFake");
	      }
	    }
	  }
	}
	
      } //doDoubleLepCRplots
      	
      if (doHardDoubleLepCRplots && passHardIsoId && !doMinimalPlots) {
	fillHistosDoubleL("cr2L","Hard");
	if (!t.isData) {
	  if (isDilepton) fillHistosDoubleL("cr2L","HardDilepton");
	  else fillHistosDoubleL("cr2L","HardFake");
	}
	
	// if (isEE){
	//   fillHistosDoubleL("cr2L","HardEE");
	//   if (isDilepton) fillHistosDoubleL("cr2L","HardEEDilepton");
	//   else fillHistosDoubleL("cr2L","HardEEFake");
	// }
	// else if (isMuMu){
	//   fillHistosDoubleL("cr2L","HardMuMu");
	//   if (isDilepton) fillHistosDoubleL("cr2L","HardMuMuDilepton");
	//   else fillHistosDoubleL("cr2L","HardMuMuFake");
	// }
	// else{
	//   fillHistosDoubleL("cr2L","HardEMu");
	//   if (isDilepton) fillHistosDoubleL("cr2L","HardEMuDilepton");
	//   else fillHistosDoubleL("cr2L","HardEMuFake");
	// }
	
      } //doHardDoubleLepCRplots

        //tight CR2L plots
      if (doZllCRplots) {
        saveZllplots = true;	
	fillHistosZll("crZll");
	if (!t.isData) {
	  if (isDilepton) fillHistosZll("crZll","Dilepton");
	  else fillHistosZll("crZll","Fake");
	}
	
	if (!doCombineElMu){
	  if (isEE){
	    fillHistosZll("crZll","EE");
	    if (!t.isData) {
	      if (isDilepton) fillHistosZll("crZll","EEDilepton");
	      else fillHistosZll("crZll","EEFake");
	    }
	  }
	  else if (isMuMu){
	    fillHistosZll("crZll","MuMu");
	    if (!t.isData) {
	      if (isDilepton) fillHistosZll("crZll","MuMuDilepton");
	      else fillHistosZll("crZll","MuMuFake");
	    }
	  }
	}//!doCombineElMu

	//require soft lepton
	if (softleppt_<20) {
	  fillHistosZll("crZll","Soft");
	  if (!t.isData) {
	    if (isDilepton) fillHistosZll("crZll","SoftDilepton");
	    else fillHistosZll("crZll","SoftFake");
	  }
	  
	  if (!doCombineElMu){
	    if (isEE){
	      fillHistosZll("crZll","SoftEE");
	      if (!t.isData) {
		if (isDilepton) fillHistosZll("crZll","SoftEEDilepton");
		else fillHistosZll("crZll","SoftEEFake");
	      }
	    }
	    else if (isMuMu){
	      fillHistosZll("crZll","SoftMuMu");
	      if (!t.isData) {
		if (isDilepton) fillHistosZll("crZll","SoftMuMuDilepton");
		else fillHistosZll("crZll","SoftMuMuFake");
	      }
	    }
	  }//!doCombineElMu
	}//softleppt < 20
      } //doZllCRplots
      
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

  if (saveSoftLplots) {
    for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
      if(!SRVecLep.at(srN).srHistMap.empty()){
	cout<<"Saving srLep"<< SRVecLep.at(srN).GetName() <<endl;
        savePlotsDir(SRVecLep.at(srN).srHistMap, outfile_, ("srLep"+SRVecLep.at(srN).GetName()).c_str());
      }
    }
  }
  if (saveSoftLMuplots) {
    for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
      if(!SRVecLep.at(srN).srMuHistMap.empty()){
	cout<<"Saving srLepMu"<< SRVecLep.at(srN).GetName() <<endl;
        savePlotsDir(SRVecLep.at(srN).srMuHistMap, outfile_, ("srLepMu"+SRVecLep.at(srN).GetName()).c_str());
      }
    }
  }
  if (saveSoftLElplots) {
    for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
      if(!SRVecLep.at(srN).srElHistMap.empty()){
	cout<<"Saving srLepEl"<< SRVecLep.at(srN).GetName() <<endl;
        savePlotsDir(SRVecLep.at(srN).srElHistMap, outfile_, ("srLepEl"+SRVecLep.at(srN).GetName()).c_str());
      }
    }
  }
  if (save1Lplots) {
    for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
      if(!SRVecLep.at(srN).cr1LHistMap.empty()){
	cout<<"Saving cr1L"<< SRVecLep.at(srN).GetName() <<endl;
        savePlotsDir(SRVecLep.at(srN).cr1LHistMap, outfile_, ("cr1L"+SRVecLep.at(srN).GetName()).c_str());
      }
    }
  }
  if (save1Lmuplots) {
    for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
      if(!SRVecLep.at(srN).cr1LmuHistMap.empty()){
	cout<<"Saving cr1Lmu"<< SRVecLep.at(srN).GetName() <<endl;
        savePlotsDir(SRVecLep.at(srN).cr1LmuHistMap, outfile_, ("cr1Lmu"+SRVecLep.at(srN).GetName()).c_str());
      }
    }
  }
  if (save1Lelplots) {
    for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
      if(!SRVecLep.at(srN).cr1LelHistMap.empty()){
	cout<<"Saving cr1Lel"<< SRVecLep.at(srN).GetName() <<endl;
        savePlotsDir(SRVecLep.at(srN).cr1LelHistMap, outfile_, ("cr1Lel"+SRVecLep.at(srN).GetName()).c_str());
      }
    }
  }
  if (save2Lplots) {
    for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
      if(!SRVecLep.at(srN).cr2LHistMap.empty()){
	cout<<"Saving cr2L"<< SRVecLep.at(srN).GetName() <<endl;
        savePlotsDir(SRVecLep.at(srN).cr2LHistMap, outfile_, ("cr2L"+SRVecLep.at(srN).GetName()).c_str());
      }
    }
  }
  if (saveZllplots) {
    for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
      if(!SRVecLep.at(srN).crZllHistMap.empty()){
	cout<<"Saving crZll"<< SRVecLep.at(srN).GetName() <<endl;
        savePlotsDir(SRVecLep.at(srN).crZllHistMap, outfile_, ("crZll"+SRVecLep.at(srN).GetName()).c_str());
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
  if (t.isData && !(t.HLT_PFHT800 || t.HLT_PFHT350_PFMET100 || t.HLT_PFMETNoMu90_PFMHTNoMu90)) return;

  std::map<std::string, float> values;
  values["deltaPhiMin"] = t.deltaPhiMin;
  values["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
  values["nlep"]        = nlepveto_;
  values["j1pt"]        = t.jet1_pt;
  values["j2pt"]        = t.jet2_pt;
  values["mt2"]         = t.mt2;
  values["passesHtMet"] = ( (t.ht > 200. && t.met_pt > 200.) || (t.ht > 1000. && t.met_pt > 30.) );

  if(SRBase.PassesSelection(values)) fillHistos(SRBase.srHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), SRBase.GetName(), "");

  // do monojet SRs
  bool passMonojet = false;
  if (passMonojetId_ && (!t.isData || t.HLT_PFMETNoMu90_PFMHTNoMu90 || t.HLT_PFHT350_PFMET100)) {
    std::map<std::string, float> values_monojet;
    values_monojet["deltaPhiMin"] = t.deltaPhiMin;
    values_monojet["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
    values_monojet["nlep"]        = nlepveto_;
    values_monojet["j1pt"]        = t.jet1_pt;
    values_monojet["njets"]       = t.nJet30;
    values_monojet["met"]         = t.met_pt;
    
    if(SRBaseMonojet.PassesSelection(values_monojet)) passMonojet = true;
    if (passMonojet) fillHistos(SRBaseMonojet.srHistMap, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.GetMT2Bins(), SRBaseMonojet.GetName(), "");
  }
  if ((SRBase.PassesSelection(values)) || (passMonojet)) {
    fillHistos(SRBaseIncl.srHistMap, SRBaseIncl.GetNumberOfMT2Bins(), SRBaseIncl.GetMT2Bins(), SRBaseIncl.GetName(), "");
  }


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
  values["passesHtMet"] = ( (t.ht > 200. && t.met_pt > 200.) || (t.ht > 1000. && t.met_pt > 30.) );

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
  if (t.isData && !(t.HLT_PFHT800 || t.HLT_PFHT350_PFMET100 || t.HLT_PFMETNoMu90_PFMHTNoMu90)) return;
  
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
  //values["passesHtMet"] = ( (t.ht > 200. && t.met_pt > 200.) || (t.ht > 1000. && t.met_pt > 30.) );


  for(unsigned int srN = 0; srN < SRVec.size(); srN++){
    if(SRVec.at(srN).PassesSelection(values)){
      fillHistos(SRVec.at(srN).srHistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix);
      break;//signal regions are orthogonal, event cannot be in more than one
    }
  }
  
  // do monojet SRs
  if (passMonojetId_ && (!t.isData || t.HLT_PFMETNoMu90_PFMHTNoMu90 || t.HLT_PFHT350_PFMET100)) {
    std::map<std::string, float> values_monojet;
    values_monojet["deltaPhiMin"] = t.deltaPhiMin;
    values_monojet["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
    values_monojet["nlep"]        = nlepveto_;
    values_monojet["j1pt"]        = t.jet1_pt;
    values_monojet["njets"]       = t.nJet30;
    values_monojet["nbjets"]      = t.nBJet20;
    values_monojet["ht"]          = t.ht;
    values_monojet["met"]         = t.met_pt;

    for(unsigned int srN = 0; srN < SRVecMonojet.size(); srN++){
      if(SRVecMonojet.at(srN).PassesSelection(values_monojet)){
	fillHistos(SRVecMonojet.at(srN).srHistMap, SRVecMonojet.at(srN).GetNumberOfMT2Bins(), SRVecMonojet.at(srN).GetMT2Bins(), prefix+SRVecMonojet.at(srN).GetName(), suffix);
	//break;//signal regions are orthogonal, event cannot be in more than one --> not true for Monojet, since we have inclusive regions
      }
    }
  } // monojet regions

  return;
}

//hists for soft lepton regions
void MT2Looper::fillHistosLepSignalRegions(const std::string& prefix, const std::string& suffix) {

  // trigger requirement on data
  if (t.isData && !(t.HLT_PFHT800 || t.HLT_PFHT350_PFMET100 || t.HLT_PFMETNoMu90_PFMHTNoMu90)) return;

  std::map<std::string, float> values;
  values["deltaPhiMin"] = t.deltaPhiMin;
  values["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
  values["nlep"]        = t.nlep+t.nPFHad10; 
  values["njets"]       = t.nJet30;
  values["nbjets"]      = t.nBJet20;
  values["nbjetshard"]  = nHardB_;
  values["mt2"]         = t.nJet30 > 1 ? t.mt2 : t.met_pt; // require large MT2 for multijet events
  values["ht"]          = t.ht;
  values["met"]         = t.met_pt;
  values["mt"]          = softlepmt_;

  for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
    if(SRVecLep.at(srN).PassesSelection(values)){
      if (prefix=="srLep") fillHistosSingleSoftLepton(SRVecLep.at(srN).srHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), suffix);
      else if (prefix=="srLepMu") fillHistosSingleSoftLepton(SRVecLep.at(srN).srMuHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), suffix);
      else if (prefix=="srLepEl") fillHistosSingleSoftLepton(SRVecLep.at(srN).srElHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), suffix);
      //break;//signal regions are orthogonal, event cannot be in more than one
    }
  }

}

//hists for soft lepton regions
void MT2Looper::fillHistosCR1L(const std::string& prefix, const std::string& suffix) {
 
  std::map<std::string, float> values;
  values["deltaPhiMin"] = t.deltaPhiMin;
  values["diffMetMhtOverMet"]  = 0; //dummy variable, inefficient at low MET
  values["nlep"]        = t.nlep+t.nPFHad10;
  values["njets"]       = t.nJet30;
  values["nbjets"]      = t.nBJet20;
  values["nbjetshard"]  = nHardB_;
  values["mt2"]         = t.nJet30 > 1 ? t.sl_mt2 : softleppt_; // require large MT2 for multijet events //replace met with lepton pT in cr1L
  values["ht"]          = t.ht; //corrected ht in this CR
  values["met"]         = softleppt_; //replace met with lepton pT in cr1L
  values["mt"]          = softlepmt_;

  //second value map for mt validation region
  std::map<std::string, float> values2;
  values2["deltaPhiMin"] = t.deltaPhiMin;
  values2["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
  values2["nlep"]        = t.nlep+t.nPFHad10;
  values2["njets"]       = t.nJet30;
  values2["nbjets"]      = t.nBJet20;
  values2["nbjetshard"]  = nHardB_;
  values2["mt2"]         = t.nJet30 > 1 ? t.sl_mt2 : softleppt_; // require large MT2 for multijet events //replace met with lepton pT in cr1L
  values2["ht"]          = t.ht; //corrected ht in this CR
  values2["met"]         = 201; //dummy variable for validation region
  values2["mt"]          = softlepmt_;

  //validation regions
  for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
    if(SRVecLep.at(srN).PassesSelection(values2)){
      if (SRVecLep.at(srN).GetName().find("base") == std::string::npos) continue; //skip non baseline regions
      if (softleppt_ > 25 && softleppt_ < 40 && t.met_pt > 180) {
	if (prefix=="cr1L") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "Validate"+suffix);
	else if (prefix=="cr1Lmu") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LmuHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "Validate"+suffix);
	else if (prefix=="cr1Lel") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LelHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "Validate"+suffix);
      }
      else if (softleppt_ > 40 && softleppt_ < 55 && t.met_pt > 180) {
	if (prefix=="cr1L") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "Validate2"+suffix);
	else if (prefix=="cr1Lmu") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LmuHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "Validate2"+suffix);
	else if (prefix=="cr1Lel") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LelHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "Validate2"+suffix);
      }

    }
  }
  
  //maximum MET cut for CR1L
  if (t.met_pt < 60) {
    for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
      if(SRVecLep.at(srN).PassesSelection(values)){
	if (prefix=="cr1L") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), suffix);
	else if (prefix=="cr1Lmu") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LmuHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), suffix);
	else if (prefix=="cr1Lel") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LelHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), suffix);
      }
    }
  }

  //extra regions for testing, only if !doMinimalPlots
  if (!doMinimalPlots) {
    for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
      if(SRVecLep.at(srN).PassesSelection(values)){

	//inclusive MET region (no cut required) 
	if (prefix=="cr1L") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "HighMET"+suffix);
	else if (prefix=="cr1Lmu") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LmuHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "HighMET"+suffix);
	else if (prefix=="cr1Lel") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LelHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "HighMET"+suffix);

	//inclusive WJets region for polarization checks
	bool passInclusiveW = softlepmt_ < 100 && t.met_pt > 50 && softleppt_ > 30; 
	if (passInclusiveW) {
	  if (prefix=="cr1L") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "IncW"+suffix);
	  else if (prefix=="cr1Lmu") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LmuHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "IncW"+suffix);
	  else if (prefix=="cr1Lel") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LelHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "IncW"+suffix);
	}
	
	// //mt2 cut region
	// if (t.met_pt < 60 && t.sl_mt2 > 100) {
      	//   if (prefix=="cr1L") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "MinMT2"+suffix);
	//   else if (prefix=="cr1Lmu") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LmuHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "MinMT2"+suffix);
	//   else if (prefix=="cr1Lel") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LelHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "MinMT2"+suffix);
	// }

	//maximum and min MET cut for CR1L
	if (t.met_pt < 60 && t.met_pt > 30) {
	  if (prefix=="cr1L") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "MinMET"+suffix);
	  else if (prefix=="cr1Lmu") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LmuHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "MinMET"+suffix);
	  else if (prefix=="cr1Lel") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LelHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "MinMET"+suffix);
	}

	//alternate MET region
	if (t.met_pt < 100 && t.met_pt > 60) {
  	  if (prefix=="cr1L") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "MaxMET"+suffix);
	  else if (prefix=="cr1Lmu") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LmuHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "MaxMET"+suffix);
	  else if (prefix=="cr1Lel") fillHistosSingleSoftLepton(SRVecLep.at(srN).cr1LelHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "MaxMET"+suffix);
	}
	
      }//passesSelection
    }// SR loop    
  }//!doMinimalPlots

 
  return;
} //soft lepton


void MT2Looper::fillHistosDoubleL(const std::string& prefix, const std::string& suffix) {


  std::map<std::string, float> values;
  values["deltaPhiMin"] = t.deltaPhiMin;
  values["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
  values["nlep"]        = 1; //dummy variable for double lepton CR
  values["njets"]       = t.nJet30;
  values["nbjets"]      = t.nBJet20;
  values["nbjetshard"]  = nHardB_;
  values["mt2"]         = t.nJet30 > 1 ? t.mt2 : t.met_pt; // require large MT2 for multijet events
  values["ht"]          = t.ht;
  values["met"]         = t.met_pt;
  values["mt"]          = softlepmt_;

  //alternate valueMap for lepton-triggered CR
  std::map<std::string, float> values2;
  values2["deltaPhiMin"] = t.deltaPhiMin;
  values2["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
  values2["nlep"]        = 1; //dummy variable for double lepton CR
  values2["njets"]       = t.nJet30;
  values2["nbjets"]      = t.nBJet20;
  values2["nbjetshard"]  = nHardB_;
  values2["mt2"]         = t.nJet30 > 1 ? t.mt2 : t.met_pt; // require large MT2 for multijet events
  values2["ht"]          = t.ht;
  values2["met"]         = 201; //dummy value for alt double lepton CR
  values2["mt"]          = softlepmt_;

  //lepton triggered dilepCR
  bool passAltSelection = (t.HLT_SingleMu || t.HLT_SingleEl) && t.met_pt > 150;
  for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
    if(SRVecLep.at(srN).PassesSelection(values2) && passAltSelection){
      if (prefix=="cr2L") fillHistosDoubleLepton(SRVecLep.at(srN).cr2LHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "Alt"+suffix);
    }
  }

  
  // trigger requirement on data
  if (t.isData && !(t.HLT_PFHT800 || t.HLT_PFHT350_PFMET100 || t.HLT_PFMETNoMu90_PFMHTNoMu90)) return;
  for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
    if(SRVecLep.at(srN).PassesSelection(values)){
      if (prefix=="cr2L") fillHistosDoubleLepton(SRVecLep.at(srN).cr2LHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), suffix);
    }
  }
  
  return;
} //double lep

void MT2Looper::fillHistosZll(const std::string& prefix, const std::string& suffix) {

  std::map<std::string, float> values;
  values["deltaPhiMin"] = 0.5; //dummy variable
  values["diffMetMhtOverMet"]  = 0; //dummy variable, inefficient at low MET
  values["nlep"]        = 1; //dummy variable for double lepton CR
  values["njets"]       = t.nJet30;
  values["nbjets"]      = t.nBJet20;
  values["nbjetshard"]  = nHardB_;
  values["mt2"]         = t.nJet30 > 1 ? t.mt2 : t.met_pt; // require large MT2 for multijet events
  values["ht"]          = t.ht;
  values["met"]         = 201; //dummy value
  values["mt"]          = zllmt_; //use recalculated MT
  
  // trigger requirement on data
  if (t.isData && !(t.HLT_SingleEl || t.HLT_SingleMu)) return;
  
  for(unsigned int srN = 0; srN < SRVecLep.size(); srN++){
    if (SRVecLep.at(srN).GetName().find("base") == std::string::npos) continue; //skip non baseline regions
    if(SRVecLep.at(srN).PassesSelection(values)){
      if (prefix=="crZll" && zllmet_ > 200) fillHistosDoubleLepton(SRVecLep.at(srN).crZllHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "MET200"+suffix);
      if (prefix=="crZll" && zllmet_ > 150) fillHistosDoubleLepton(SRVecLep.at(srN).crZllHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "MET150"+suffix);
      if (prefix=="crZll" && zllmet_ > 100) fillHistosDoubleLepton(SRVecLep.at(srN).crZllHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "MET100"+suffix);
      if (prefix=="crZll" && zllmet_ > 50) fillHistosDoubleLepton(SRVecLep.at(srN).crZllHistMap, SRVecLep.at(srN).GetNumberOfMT2Bins(), SRVecLep.at(srN).GetMT2Bins(), prefix+SRVecLep.at(srN).GetName(), "MET50"+suffix);
    }
  }
  
  return;
} //Zll

void MT2Looper::fillHistos(std::map<std::string, TH1*>& h_1d, int n_mt2bins, float* mt2bins, const std::string& dirname, const std::string& s) {
  TDirectory * dir = (TDirectory*)outfile_->Get(dirname.c_str());
  if (dir == 0) {
    dir = outfile_->mkdir(dirname.c_str());
  } 
  dir->cd();

  // workaround for monojet bins
  float mt2_temp = t.mt2;
  if (t.nJet30 == 1) mt2_temp = t.jet1_pt;

  plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
  plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
  plot1D("h_mt2bins"+s,       mt2_temp,   evtweight_, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
  plot1D("h_htbins"+s,       t.ht,   evtweight_, h_1d, ";H_{T} [GeV]", n_htbins, htbins);
  plot1D("h_htbins2"+s,       t.ht,   evtweight_, h_1d, ";H_{T} [GeV]", n_htbins2, htbins2);
  plot1D("h_njbins"+s,       t.nJet30,   evtweight_, h_1d, ";N(jets)", n_njbins, njbins);
  plot1D("h_nbjbins"+s,       t.nBJet20,   evtweight_, h_1d, ";N(bjets)", n_nbjbins, nbjbins);
  //if (!doMinimalPlots) {
    plot1D("h_mt2"+s,       mt2_temp,   evtweight_, h_1d, "; M_{T2} [GeV]", 100, 0, 1000);
    plot1D("h_met"+s,       t.met_pt,   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 100, 0, 1000);
    plot1D("h_ht"+s,       t.ht,   evtweight_, h_1d, ";H_{T} [GeV]", 80, 0, 2000);
    plot1D("h_nJet30"+s,       t.nJet30,   evtweight_, h_1d, ";N(jets)", 15, 0, 15);
    plot1D("h_nJet30Eta3"+s,       nJet30Eta3_,   evtweight_, h_1d, ";N(jets, |#eta| > 3.0)", 10, 0, 10);
    plot1D("h_nBJet20"+s,      t.nBJet20,   evtweight_, h_1d, ";N(bjets)", 6, 0, 6);
    plot1D("h_deltaPhiMin"+s,  t.deltaPhiMin,   evtweight_, h_1d, ";#Delta#phi_{min}", 32, 0, 3.2);
    plot1D("h_diffMetMht"+s,   t.diffMetMht,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| [GeV]", 120, 0, 300);
    plot1D("h_diffMetMhtOverMet"+s,   t.diffMetMht/t.met_pt,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| / E_{T}^{miss}", 100, 0, 2.);
    plot1D("h_minMTBMet"+s,   t.minMTBMet,   evtweight_, h_1d, ";min M_{T}(b, E_{T}^{miss}) [GeV]", 150, 0, 1500);
    plot1D("h_nlepveto"+s,     nlepveto_,   evtweight_, h_1d, ";N(leps)", 10, 0, 10);
    plot1D("h_J0pt"+s,       t.jet1_pt,   evtweight_, h_1d, ";p_{T}(jet1) [GeV]", 100, 0, 1000);
    plot1D("h_J1pt"+s,       t.jet2_pt,   evtweight_, h_1d, ";p_{T}(jet2) [GeV]", 100, 0, 1000);
    plot1D("h_BJetpt"+s,       bjetPt_,   evtweight_, h_1d, ";p_{T}(bjet1) [GeV]", 100, 0, 1000);
    plot1D("h_BJetptshort"+s,  bjetPt_,   evtweight_, h_1d, ";p_{T}(bjet1) [GeV]", 100, 0, 100);
    if (t.jet_btagCSV[0]>0.89) plot1D("h_BJetptLead"+s,       bjetPt_,   evtweight_, h_1d, ";p_{T}(bjet1) [GeV]", 100, 0, 1000);
    else plot1D("h_BJetptNotLead"+s,       bjetPt_,   evtweight_, h_1d, ";p_{T}(bjet1) [GeV]", 100, 0, 1000);
    plot1D("h_categoryB"+s,  categoryB_,   evtweight_, h_1d, ";b-category", 4, 0, 4);
    //}

  TString directoryname(dirname);

  if (isSignal_) {
    plot3D("h_mtbins_sigscan"+s, softlepmt_, t.GenSusyMScan1, t.GenSusyMScan2, evtweight_, h_1d, ";M_{T} [GeV];mass1 [GeV];mass2 [GeV]", n_mt2bins, mt2bins, n_m1bins, m1bins, n_m2bins, m2bins);
  }

  if (!t.isData && isSignal_ && doSystVariationPlots) {
    int binx = h_sig_avgweight_isr_->GetXaxis()->FindBin(t.GenSusyMScan1);
    int biny = h_sig_avgweight_isr_->GetYaxis()->FindBin(t.GenSusyMScan2);
    // stored ISR weight is for DN variation
    float weight_isr_DN = t.weight_isr;
    float avgweight_isr_DN = h_sig_avgweight_isr_->GetBinContent(binx,biny);
    float weight_isr_UP = 2. - weight_isr_DN;
    float avgweight_isr_UP = 2. - avgweight_isr_DN;
    plot1D("h_mtbins_isr_UP"+s,       softlepmt_,   evtweight_ * weight_isr_UP / avgweight_isr_UP, h_1d, "; M_{T} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mtbins_isr_DN"+s,       softlepmt_,   evtweight_ * weight_isr_DN / avgweight_isr_DN, h_1d, "; M_{T} [GeV]", n_mt2bins, mt2bins);
    plot3D("h_mtbins_sigscan_isr_UP"+s, softlepmt_, t.GenSusyMScan1, t.GenSusyMScan2, evtweight_ * weight_isr_UP / avgweight_isr_UP, h_1d, ";M_{T} [GeV];mass1 [GeV];mass2 [GeV]", n_mt2bins, mt2bins, n_m1bins, m1bins, n_m2bins, m2bins);
    plot3D("h_mtbins_sigscan_isr_DN"+s, softlepmt_, t.GenSusyMScan1, t.GenSusyMScan2, evtweight_ * weight_isr_DN / avgweight_isr_DN, h_1d, ";M_{T} [GeV];mass1 [GeV];mass2 [GeV]", n_mt2bins, mt2bins, n_m1bins, m1bins, n_m2bins, m2bins);
  }

  if (!t.isData && applyBtagSF && doSystVariationPlots) {

    int binx,biny;
    float avgweight_btagsf = 1.;
    float avgweight_heavy_UP = 1.;
    float avgweight_heavy_DN = 1.;
    float avgweight_light_UP = 1.;
    float avgweight_light_DN = 1.;

    if (isSignal_) {
      binx = h_sig_avgweight_btagsf_heavy_UP_->GetXaxis()->FindBin(t.GenSusyMScan1);
      biny = h_sig_avgweight_btagsf_heavy_UP_->GetYaxis()->FindBin(t.GenSusyMScan2);
      avgweight_btagsf = h_sig_avgweight_btagsf_->GetBinContent(binx,biny);
      avgweight_heavy_UP = h_sig_avgweight_btagsf_heavy_UP_->GetBinContent(binx,biny);
      avgweight_heavy_DN = h_sig_avgweight_btagsf_heavy_DN_->GetBinContent(binx,biny);
      avgweight_light_UP = h_sig_avgweight_btagsf_light_UP_->GetBinContent(binx,biny);
      avgweight_light_DN = h_sig_avgweight_btagsf_light_DN_->GetBinContent(binx,biny);
    }

    // assume weights are already applied to central value
    plot1D("h_mtbins_btagsf_heavy_UP"+s,       softlepmt_,   evtweight_ / t.weight_btagsf * avgweight_btagsf * t.weight_btagsf_heavy_UP / avgweight_heavy_UP, h_1d, "; M_{T} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mtbins_btagsf_light_UP"+s,       softlepmt_,   evtweight_ / t.weight_btagsf * avgweight_btagsf * t.weight_btagsf_light_UP / avgweight_light_UP, h_1d, "; M_{T} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mtbins_btagsf_heavy_DN"+s,       softlepmt_,   evtweight_ / t.weight_btagsf * avgweight_btagsf * t.weight_btagsf_heavy_DN / avgweight_heavy_DN, h_1d, "; M_{T} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mtbins_btagsf_light_DN"+s,       softlepmt_,   evtweight_ / t.weight_btagsf * avgweight_btagsf * t.weight_btagsf_light_DN / avgweight_light_DN, h_1d, "; M_{T} [GeV]", n_mt2bins, mt2bins);
    if (isSignal_) {
      plot3D("h_mtbins_sigscan_btagsf_heavy_UP"+s, softlepmt_, t.GenSusyMScan1, t.GenSusyMScan2, evtweight_ / t.weight_btagsf * avgweight_btagsf * t.weight_btagsf_heavy_UP / avgweight_heavy_UP, h_1d, ";M_{T} [GeV];mass1 [GeV];mass2 [GeV]", n_mt2bins, mt2bins, n_m1bins, m1bins, n_m2bins, m2bins);
      plot3D("h_mtbins_sigscan_btagsf_light_UP"+s, softlepmt_, t.GenSusyMScan1, t.GenSusyMScan2, evtweight_ / t.weight_btagsf * avgweight_btagsf * t.weight_btagsf_light_UP / avgweight_light_UP, h_1d, ";M_{T} [GeV];mass1 [GeV];mass2 [GeV]", n_mt2bins, mt2bins, n_m1bins, m1bins, n_m2bins, m2bins);
      plot3D("h_mtbins_sigscan_btagsf_heavy_DN"+s, softlepmt_, t.GenSusyMScan1, t.GenSusyMScan2, evtweight_ / t.weight_btagsf * avgweight_btagsf * t.weight_btagsf_heavy_DN / avgweight_heavy_DN, h_1d, ";M_{T} [GeV];mass1 [GeV];mass2 [GeV]", n_mt2bins, mt2bins, n_m1bins, m1bins, n_m2bins, m2bins);
      plot3D("h_mtbins_sigscan_btagsf_light_DN"+s, softlepmt_, t.GenSusyMScan1, t.GenSusyMScan2, evtweight_ / t.weight_btagsf * avgweight_btagsf * t.weight_btagsf_light_DN / avgweight_light_DN, h_1d, ";M_{T} [GeV];mass1 [GeV];mass2 [GeV]", n_mt2bins, mt2bins, n_m1bins, m1bins, n_m2bins, m2bins);
    }
  }

  if (!t.isData && doGenTauVars) {
//MT2       float unc_tau1p = 0.;
//MT2       float unc_tau3p = 0.;
//MT2       if (t.ngenTau1Prong > 0 || t.ngenTau3Prong > 0) {
//MT2         // loop on gen taus
//MT2         for (int itau = 0; itau < t.ngenTau; ++itau) {
//MT2   	// check acceptance for veto: pt > 10
//MT2          if (t.genTau_leadTrackPt[itau] < 10.) continue;
//MT2          if (t.genTau_decayMode[itau] == 1) unc_tau1p += 0.14; // 14% relative uncertainty for missing a 1-prong tau
//MT2          else if (t.genTau_decayMode[itau] == 3) unc_tau3p += 0.06; // 6% relative uncertainty for missing a 3-prong tau
//MT2         }
//MT2       }
    float unc_tau1p = 0.;
    float unc_tau3p = 0.;
    if (foundMissingTau_ && missIdx_ != -1) {
      if (t.genTau_leadTrackPt[missIdx_] > 10.) {
	if (t.genTau_decayMode[missIdx_] == 1) unc_tau1p += 0.14; // 14% relative uncertainty for missing a 1-prong tau
	else if (t.genTau_decayMode[missIdx_] == 3) unc_tau3p += 0.06; // 6% relative uncertainty for missing a 3-prong tau
      }
    }
    plot1D("h_mtbins_tau1p_UP"+s,       softlepmt_,   evtweight_ * (1. + unc_tau1p), h_1d, "; M_{T} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mtbins_tau1p_DN"+s,       softlepmt_,   evtweight_ * (1. - unc_tau1p), h_1d, "; M_{T} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mtbins_tau3p_UP"+s,       softlepmt_,   evtweight_ * (1. + unc_tau3p), h_1d, "; M_{T} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mtbins_tau3p_DN"+s,       softlepmt_,   evtweight_ * (1. - unc_tau3p), h_1d, "; M_{T} [GeV]", n_mt2bins, mt2bins);
  }

  // --------------------------------------------------------
  // -------------- old dummy uncertainty code --------------
  // --------------------------------------------------------
  
  // // lepton efficiency variation in signal region: large uncertainty on leptons NOT vetoed
  // if (!t.isData && doLepEffVars && directoryname.Contains("sr")) {
  //   float unc_lepeff = 0.;
  //   if (t.ngenLep > 0 || t.ngenLepFromTau > 0) {
  //     // loop on gen e/mu
  //     for (int ilep = 0; ilep < t.ngenLep; ++ilep) {
  // 	// check acceptance for veto: pt > 5, |eta| < 2.4
  //      if (t.genLep_pt[ilep] < 5.) continue;
  //      if (fabs(t.genLep_eta[ilep]) > 2.4) continue;
  //      unc_lepeff += 0.20; // 12% relative uncertainty for missing lepton
  //     }
  //     for (int ilep = 0; ilep < t.ngenLepFromTau; ++ilep) {
  // 	// check acceptance for veto: pt > 5
  //      if (t.genLepFromTau_pt[ilep] < 5.) continue;
  //      if (fabs(t.genLepFromTau_eta[ilep]) > 2.4) continue;
  //      unc_lepeff += 0.20; // 12% relative uncertainty for missing lepton
  //     }
  //   }

  //   // if lepeff goes up, number of events in SR should go down.
  //   plot1D("h_mt2bins_lepeff_UP"+s,       mt2_temp,   evtweight_ * (1. - unc_lepeff), h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
  //   plot1D("h_mt2bins_lepeff_DN"+s,       mt2_temp,   evtweight_ * (1. + unc_lepeff), h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
  // }
  // --------------------------------------------------------

//MT2  // lepton efficiency variation in signal region for fastsim: large uncertainty on leptons NOT vetoed
//MT2  if (!t.isData && isSignal_ && doLepEffVars && applyLeptonSFfastsim && directoryname.Contains("sr")) {
//MT2
//MT2    // if lepeff goes up, number of events in SR should go down. Already taken into account in unc_lepeff_sr_
//MT2    //  need to first remove lepeff SF
//MT2    plot1D("h_mt2bins_lepeff_UP"+s,       mt2_temp,   evtweight_ / (1. + cor_lepeff_sr_) * (1. + cor_lepeff_sr_ + unc_lepeff_sr_), h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
//MT2    plot1D("h_mt2bins_lepeff_DN"+s,       mt2_temp,   evtweight_ / (1. + cor_lepeff_sr_) * (1. + cor_lepeff_sr_ - unc_lepeff_sr_), h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
//MT2    plot3D("h_mt2bins_sigscan_lepeff_UP"+s, mt2_temp, t.GenSusyMScan1, t.GenSusyMScan2, evtweight_ / (1. + cor_lepeff_sr_) * (1. + cor_lepeff_sr_ + unc_lepeff_sr_), h_1d, ";M_{T2} [GeV];mass1 [GeV];mass2 [GeV]", n_mt2bins, mt2bins, n_m1bins, m1bins, n_m2bins, m2bins);
//MT2    plot3D("h_mt2bins_sigscan_lepeff_DN"+s, mt2_temp, t.GenSusyMScan1, t.GenSusyMScan2, evtweight_ / (1. + cor_lepeff_sr_) * (1. + cor_lepeff_sr_ - unc_lepeff_sr_), h_1d, ";M_{T2} [GeV];mass1 [GeV];mass2 [GeV]", n_mt2bins, mt2bins, n_m1bins, m1bins, n_m2bins, m2bins);
//MT2    
//MT2  }
//MT2
//MT2  else if (!t.isData && doLepEffVars && directoryname.Contains("sr")) {
//MT2    // if lepeff goes up, number of events in SR should go down. Already taken into account in unc_lepeff_sr_
//MT2    plot1D("h_mt2bins_lepeff_UP"+s,       mt2_temp,   evtweight_ * (1. + unc_lepeff_sr_), h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
//MT2    plot1D("h_mt2bins_lepeff_DN"+s,       mt2_temp,   evtweight_ * (1. - unc_lepeff_sr_), h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
//MT2  }

  // lepton efficiency variations for MT2+lepton: just apply the weight, which already contains: *SF for each lepton, and *lostSF for one lost lepton
  if (!t.isData && doLepEffVars) {
    plot1D("h_mtbins_lepeff_UP"+s,       softlepmt_,   evtweight_lepEffUp_, h_1d, "; M_{T} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mtbins_lepeff_DN"+s,       softlepmt_,   evtweight_lepEffDn_, h_1d, "; M_{T} [GeV]", n_mt2bins, mt2bins); 
    if (isSignal_ && applyLeptonSFfastsim && directoryname.Contains("sr")) {
      plot3D("h_mtbins_sigscan_lepeff_UP"+s, softlepmt_, t.GenSusyMScan1, t.GenSusyMScan2, evtweight_lepEffUp_, h_1d, ";M_{T} [GeV];mass1 [GeV];mass2 [GeV]", n_mt2bins, mt2bins, n_m1bins, m1bins, n_m2bins, m2bins);
      plot3D("h_mtbins_sigscan_lepeff_DN"+s, softlepmt_, t.GenSusyMScan1, t.GenSusyMScan2, evtweight_lepEffDn_, h_1d, ";M_{T} [GeV];mass1 [GeV];mass2 [GeV]", n_mt2bins, mt2bins, n_m1bins, m1bins, n_m2bins, m2bins);
    }
  }
  
  outfile_->cd();
  return;
}

void MT2Looper::fillHistosSingleSoftLepton(std::map<std::string, TH1*>& h_1d, int n_mt2bins, float* mt2bins, const std::string& dirname, const std::string& s) {
  TDirectory * dir = (TDirectory*)outfile_->Get(dirname.c_str());
  if (dir == 0) {
    dir = outfile_->mkdir(dirname.c_str());
  } 
  dir->cd();

  // If you are not in srLepBase region, just plot the lepton pT (and whatever else is needed) and exit
  TString directoryname(dirname);
  if ( onlyMakeFRplots_ ) {
    plot1D("h_mtbins"+s,            softlepmt_,   evtweight_, h_1d, ";M_{T} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_lepptshortBins"+s,      softleppt_,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 3, 5, 20);
    plot2D("h_mtbins_leppt"+s, softlepmt_, softleppt_, evtweight_, h_1d, ";M_{T} [GeV];p_{T}(lep) [GeV]", n_mt2bins, mt2bins, n_ptsoftbins, ptsoftbins);

      // We might have to add other plots, for Fake estimates in CR2L as a function of NJ, NB, etc

    //cone-correction for isolation
    float isoCut = -1;
    if (abs(softlepId_) == 13) isoCut = 0.1;
    else if (abs(softlepId_) == 11) isoCut = 0.1;
    Float_t floor = 0; //so compiler doesn't complain about typecasting
    float correction = TMath::Max( floor , t.lep_miniRelIso[softlepIdx_]-isoCut );
    float ptCorr = softleppt_ * (1 + correction);
    plot1D("h_lepptCorr"+s,      ptCorr,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 1000, 0, 1000);
    plot1D("h_lepptCorrshort"+s,      ptCorr,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 30, 0, 30);
    
    //mother origin
    if (!t.isData) {
      int mom = abs(t.lep_mcMatchId[softlepIdx_]);
      int flavorType = -1;
      if (mom == 15) flavorType = 0;
      else if (idIsCharm(mom)) flavorType = 2;
      else if (idIsBeauty(mom)) flavorType = 3;
      else flavorType = 1;    
      plot1D("h_motherFlavor"+s,  flavorType,   evtweight_, h_1d, ";parton mother flavor", 4, 0, 4);    
    }
    
    return;
  }

  if ( calculateFakeRates ) {
    plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
    plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
    plot1D("h_lepptshortBins"+s,      softleppt_,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 3, 5, 20);
    plot1D("h_lepptshortBins_w1"+s,      softleppt_,   1, h_1d, ";p_{T}(lep) [GeV]", 3, 5, 20);
    plot1D("h_mtbins"+s,            softlepmt_,   evtweight_, h_1d, ";M_{T} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_met"+s,       t.met_pt,   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 100, 0, 1000);
    plot1D("h_mt"+s,            softlepmt_,   evtweight_, h_1d, ";M_{T} [GeV]", 250, 0, 250);

    //cone-correction for isolation
    float isoCut = -1;
    if (abs(softlepId_) == 13) isoCut = 0.1;
    else if (abs(softlepId_) == 11) isoCut = 0.1;
    Float_t floor = 0; //so compiler doesn't complain about typecasting
    float correction = TMath::Max( floor , t.lep_miniRelIso[softlepIdx_]-isoCut );
    float ptCorr = softleppt_ * (1 + correction);
    plot1D("h_lepptCorr"+s,      ptCorr,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 1000, 0, 1000);
    plot1D("h_lepptCorrshort"+s,      ptCorr,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 30, 0, 30);
    
    //mother origin
    if (!t.isData) {
      int mom = abs(t.lep_mcMatchId[softlepIdx_]);
      int flavorType = -1;
      if (mom == 15) flavorType = 0;
      else if (idIsCharm(mom)) flavorType = 2;
      else if (idIsBeauty(mom)) flavorType = 3;
      else flavorType = 1;    
      plot1D("h_motherFlavor"+s,  flavorType,   evtweight_, h_1d, ";parton mother flavor", 4, 0, 4);    
    }
    
    return;
  }


  plot1D("h_leppt"+s,      softleppt_,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 1000, 0, 1000);
  plot1D("h_lepptshort"+s,      softleppt_,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 30, 0, 30);
  plot1D("h_lepptshort_w1"+s,      softleppt_,   1, h_1d, ";p_{T}(lep) [GeV]", 30, 0, 30);
  plot1D("h_lepptshortBins"+s,      softleppt_,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 3, 5, 20);
  plot1D("h_lepptshortBins_w1"+s,      softleppt_,   1, h_1d, ";p_{T}(lep) [GeV]", 3, 5, 20);
  plot1D("h_lepphi"+s,      softlepphi_,   evtweight_, h_1d, "phi",  64, -3.2, 3.2);
  plot1D("h_lepeta"+s,      softlepeta_,   evtweight_, h_1d, "eta",  60, -3, 3);
  plot1D("h_mt"+s,            softlepmt_,   evtweight_, h_1d, ";M_{T} [GeV]", 250, 0, 250);
  plot1D("h_mtbins"+s,            softlepmt_,   evtweight_, h_1d, ";M_{T} [GeV]", n_mt2bins, mt2bins);
  plot1D("h_softlepmt2"+s,    t.sl_mt2,   evtweight_, h_1d, "; M_{T2} [GeV]", 1000, 0, 1000);
  plot1D("h_softlepmet"+s,       softleppt_,   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 100, 0, 1000);
  plot1D("h_softlepht"+s,       t.ht,   evtweight_, h_1d, ";H_{T} [GeV]", 80, 0, 2000);
  plot1D("h_lepTightId"+s,       t.lep_tightId[softlepIdx_],   evtweight_, h_1d, ";lep_tightId", 4, 0, 4);

  //isolation  
  plot1D("h_miniRelIso"+s,  t.lep_miniRelIso[softlepIdx_],   evtweight_, h_1d, "miniRelIso [GeV]", 100, 0, 0.5);
  plot1D("h_absIso"+s,  t.lep_relIso03[softlepIdx_]*t.lep_pt[softlepIdx_],   evtweight_, h_1d, "absIso [GeV]", 80, 0, 40);
  plot1D("h_relIso03"+s,  t.lep_relIso03[softlepIdx_],   evtweight_, h_1d, ";relIso03 [GeV]", 100, 0, 2);
  plot1D("h_relIso04"+s,  t.lep_relIso04[softlepIdx_],   evtweight_, h_1d, ";relIso04 [GeV]", 100, 0, 2);
  plot1D("h_relIsoAn04"+s,  t.lep_relIsoAn04[softlepIdx_],   evtweight_, h_1d, ";relIsoAn04 [GeV]", 100, 0, 10);
  
  //missing lep
  plot1D("h_missingleppt"+s,      missPt_,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 200, 0, 1000);
  plot1D("h_missingdilepmll"+s,     dilepmll_,  evtweight_, h_1d, "m_{ll}", 150, 0 , 150);
  plot1D("h_trackdilepmll"+s,     dilepmllTrack_,  evtweight_, h_1d, "m_{ll}", 150, 0 , 150);
  
  //compute stuff for soft lep
  float metX = t.met_pt * cos(t.met_phi);
  float metY = t.met_pt * sin(t.met_phi);
  float lepX = softleppt_ * cos(softlepphi_);
  float lepY = softleppt_ * sin(softlepphi_);
  float wX   = metX + lepX;
  float wY   = metY + lepY;
  float wPt  = sqrt(wX*wX + wY*wY);
  //float wPhi = (wX > 0) ? TMath::ATan(wY/wX) : (TMath::ATan(wY/wX) + TMath::Pi()/2);
  plot1D("h_Wpt"+s,      wPt,   evtweight_, h_1d, ";p_{T}(l,MET) [GeV]", 200, 0, 1000);    
  
    //polarization stuff
  if (!doMinimalPlots) {
    //3vectors in transverse plane
    //set eta to zero for both lepton and MET 3-vectors
    //sum gives "transverse W" vector
    TVector3 Lp(0,0,0);
    TVector3 METp(0,0,0);
    TVector3 Wp(0,0,0);
    Lp.SetPtEtaPhi(softleppt_,0,softlepphi_);
    METp.SetPtEtaPhi(t.met_pt,0,t.met_phi);
    Wp = Lp+METp;

    //4vectors in transverse plane
    //set eta to zero for both lepton and MET 4-vectors
    //sum gives "transverse W" vector
    TLorentzVector Lvec(0,0,0,0);
    TLorentzVector METvec(0,0,0,0);
    TLorentzVector Wvec(0,0,0,0); 
    TVector3 Wrf(0,0,0); //3vector to boost to W rest frame
    TLorentzVector Lrf(0,0,0,0); //4vector of lepton boosted to W rest frame
    Lvec.SetPtEtaPhiM(softleppt_,0,softlepphi_,softlepM_);
    METvec.SetPtEtaPhiM(t.met_pt,0,t.met_phi,0); // m=0 for met vector
    Wvec = Lvec+METvec;
    Wrf = Wvec.BoostVector(); //boost vector to w rest frame
    Lrf = ROOT::Math::VectorUtil::boost(Lvec, Wrf);
    TVector3 Lrf3 = Lrf.Vect(); //3vector piece of boosted lepton 4vector
    
    //polarization stuff
    {
      //3vectors in transverse plane
      //set eta to zero for both lepton and MET 3-vectors
      //sum gives "transverse W" vector
      TVector3 Lp(0,0,0);
      TVector3 METp(0,0,0);
      TVector3 Wp(0,0,0);
      Lp.SetPtEtaPhi(softleppt_,0,softlepphi_);
      METp.SetPtEtaPhi(t.met_pt,0,t.met_phi);
      Wp = Lp+METp;
      
      //4vectors in transverse plane
      //set eta to zero for both lepton and MET 4-vectors
      //sum gives "transverse W" vector
      TLorentzVector Lvec(0,0,0,0);
      TLorentzVector METvec(0,0,0,0);
      TLorentzVector Wvec(0,0,0,0); 
      TVector3 Wrf(0,0,0); //3vector to boost to W rest frame
      TLorentzVector Lrf(0,0,0,0); //4vector of lepton boosted to W rest frame
      Lvec.SetPtEtaPhiM(softleppt_,0,softlepphi_,softlepM_);
      METvec.SetPtEtaPhiM(t.met_pt,0,t.met_phi,0); // m=0 for met vector
      Wvec = Lvec+METvec;
      Wrf = Wvec.BoostVector(); //boost vector to w rest frame
      Lrf = ROOT::Math::VectorUtil::boost(Lvec, Wrf);
      TVector3 Lrf3 = Lrf.Vect(); //3vector piece of boosted lepton 4vector
      
      //polarization variables
      float cmsPol = Lp.Dot(Wp) / (wPt*wPt);
      float atlasPol = Wp.Dot(Lrf3)/(wPt*Lrf3.Mag());
      
      //some test plots to debug difference between cmsPol,cmsPol2
      plot1D("h_wMagDiff"+s,      wPt/Wp.Mag(),   evtweight_, h_1d, "wPt / Wp.Mag", 20, 0, 2);
      plot1D("h_wPhiDiff"+s,      TMath::ATan(wY/wX) - Wp.Phi(),   evtweight_, h_1d, "arctan(wx/wy) - Wp.Phi",  64, -3.2, 3.2);
      plot1D("h_phi1"+s,      TMath::ATan(wY/wX),   evtweight_, h_1d, "arctan(wx/wy)",  64, -3.2, 3.2);
      plot1D("h_phi2"+s,       Wp.Phi(),   evtweight_, h_1d, "Wp.Phi",  64, -3.2, 3.2);
      plot1D("h_eta"+s,       Wp.Eta(),   evtweight_, h_1d, "Wp.Phi",  64, -3.2, 3.2);
      plot1D("h_lepPhi"+s,       softlepphi_,   evtweight_, h_1d, " lep Phi",  64, -3.2, 3.2);
      plot1D("h_metPhi"+s,       t.met_phi,   evtweight_, h_1d, "MET Phi",  64, -3.2, 3.2);
      
      plot1D("h_cmsPol"+s,      cmsPol,   evtweight_, h_1d, "L_{P}", 26, -1.3, 1.3);
      plot1D("h_cmsPol2"+s,      TMath::Cos(TMath::ATan(wY/wX) - softlepphi_) * softleppt_ / wPt,   evtweight_, h_1d, "L_{P}", 26, -1.3, 1.3);
      plot1D("h_atlasPol"+s,      atlasPol,   evtweight_, h_1d, "L_{P}", 20, -1, 1);
      if (softlepId_ > 0) {
	plot1D("h_cmsPolMinus"+s,      cmsPol,   evtweight_, h_1d, "L_{P}", 13, 0, 1.3);
	plot1D("h_cmsPol2Minus"+s,      TMath::Cos(TMath::ATan(wY/wX) - softlepphi_) * softleppt_ / wPt,   evtweight_, h_1d, "L_{P}", 13, 0, 1.3);
	plot1D("h_atlasPolMinus"+s,      atlasPol,   evtweight_, h_1d, "L_{P}", 20, -1, 1);
      }
      if (softlepId_ < 0) {
	plot1D("h_cmsPolPlus"+s,      cmsPol,   evtweight_, h_1d, "L_{P}", 13, 0, 1.3);
	plot1D("h_cmsPol2Plus"+s,      TMath::Cos(TMath::ATan(wY/wX) - softlepphi_) * softleppt_ / wPt,   evtweight_, h_1d, "L_{P}", 13, 0, 1.3);
	plot1D("h_atlasPolPlus"+s,      atlasPol,   evtweight_, h_1d, "L_{P}", 20, -1, 1);
      }
    }
    
    if (t.met_pt < 90 || softleppt_ < 20)  plot1D("h_Wpt90"+s,      wPt,   evtweight_, h_1d, ";p_{T}(l,MET) [GeV]", 200, 0, 1000);
    if (t.met_pt < 80 || softleppt_ < 20)  plot1D("h_Wpt80"+s,      wPt,   evtweight_, h_1d, ";p_{T}(l,MET) [GeV]", 200, 0, 1000);
    if (t.met_pt < 70 || softleppt_ < 20)  plot1D("h_Wpt70"+s,      wPt,   evtweight_, h_1d, ";p_{T}(l,MET) [GeV]", 200, 0, 1000);
    if (t.met_pt < 60 || softleppt_ < 20)  plot1D("h_Wpt60"+s,      wPt,   evtweight_, h_1d, ";p_{T}(l,MET) [GeV]", 200, 0, 1000);
    if (t.met_pt < 50 || softleppt_ < 20)  plot1D("h_Wpt50"+s,      wPt,   evtweight_, h_1d, ";p_{T}(l,MET) [GeV]", 200, 0, 1000);

    plot1D("h_deltaPhiMETminusLep"+s, t.met_phi - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{MET-Lep}", 64, -3.2, 3.2);
    plot1D("h_deltaPhiWminusLep"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);

    //true W-pt for MC only
    if (!t.isData) {
      float trueWpt = -1;
      //loop over gen part to find W
      for (int igen = 0; igen < t.ngenStat23; ++igen) {
	if (abs(t.genStat23_pdgId[igen]) == 24) {trueWpt = t.genStat23_pt[igen]; break;}
      }
      plot1D("h_trueWpt"+s,   trueWpt ,   evtweight_, h_1d, ";W p_{T} [GeV]", 200, 0, 1000);
    }
    
    //dPhi for diff Wpt cuts
    if (wPt < 100 && wPt > 50) plot1D("h_deltaPhiWminusLep_W50t100"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);
    if (wPt < 200 && wPt > 100) plot1D("h_deltaPhiWminusLep_W100t200"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);
    if (wPt < 300 && wPt > 200) plot1D("h_deltaPhiWminusLep_W200t300"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);
    if (wPt < 500 && wPt > 300) plot1D("h_deltaPhiWminusLep_W300t500"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);
    
    if (softlepId_ > 0) {
      plot1D("h_deltaPhiWminusLepMinus"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);
      if (wPt < 100 && wPt > 50) plot1D("h_deltaPhiWminusLepMinus_W50t100"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);
      if (wPt < 200 && wPt > 100) plot1D("h_deltaPhiWminusLepMinus_W100t200"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);
      if (wPt < 300 && wPt > 200) plot1D("h_deltaPhiWminusLepMinus_W200t300"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);
      if (wPt < 500 && wPt > 300) plot1D("h_deltaPhiWminusLepMinus_W300t500"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);
    }
    if (softlepId_ < 0) {
      plot1D("h_deltaPhiWminusLepPlus"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);
      if (wPt < 100 && wPt > 50) plot1D("h_deltaPhiWminusLepPlus_W50t100"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);
      if (wPt < 200 && wPt > 100) plot1D("h_deltaPhiWminusLepPlus_W100t200"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);
      if (wPt < 300 && wPt > 200) plot1D("h_deltaPhiWminusLepPlus_W200t300"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);
      if (wPt < 500 && wPt > 300) plot1D("h_deltaPhiWminusLepPlus_W300t500"+s, TMath::ATan(wY/wX) - softlepphi_ ,   evtweight_, h_1d, ";#Delta#phi_{W-Lep}", 64, -3.2, 3.2);
    }
  }//doMinimal
  
  outfile_->cd();

  fillHistos(h_1d, n_mt2bins, mt2bins, dirname, s);
  return;
}

void MT2Looper::fillHistosDoubleLepton(std::map<std::string, TH1*>& h_1d, int n_mt2bins, float* mt2bins, const std::string& dirname, const std::string& s) {
  TDirectory * dir = (TDirectory*)outfile_->Get(dirname.c_str());
  if (dir == 0) {
    dir = outfile_->mkdir(dirname.c_str());
  } 
  dir->cd();

  plot1D("h_lep1pt"+s,      hardleppt_,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 200, 0, 1000);
  plot1D("h_lep1ptshort"+s,      hardleppt_,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 30, 0, 30);
  plot1D("h_lep1phi"+s,      hardlepphi_,   evtweight_, h_1d, "phi",  64, -3.2, 3.2);
  plot1D("h_lep1eta"+s,      hardlepeta_,   evtweight_, h_1d, "eta",  60, -3, 3);
  plot1D("h_lep2pt"+s,      softleppt_,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 200, 0, 1000);
  plot1D("h_lep2ptshort"+s,      softleppt_,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 30, 0, 30);
  plot1D("h_lep2phi"+s,      softlepphi_,   evtweight_, h_1d, "phi",  64, -3.2, 3.2);
  plot1D("h_lep2eta"+s,      softlepeta_,   evtweight_, h_1d, "eta",  60, -3, 3);
  plot1D("h_softlepht"+s,       t.ht,   evtweight_, h_1d, ";H_{T} [GeV]", 80, 0, 2000);

  plot1D("h_dilepmll"+s,     dilepmll_,  evtweight_, h_1d, "m_{ll}", 150, 0 , 150);
  
  //compute mt with softest lepton for soft lep
  TLorentzVector lowp4;
  TLorentzVector highp4;
  TLorentzVector dilepp4;

  lowp4.SetPtEtaPhiM(softleppt_, softlepeta_, softlepphi_, softlepM_);
  highp4.SetPtEtaPhiM(hardleppt_, hardlepeta_, hardlepphi_, hardlepM_);

  dilepp4 = lowp4 + highp4;
  
  //compute additional mT shapes
  float metX = t.met_pt * cos(t.met_phi);
  float metY = t.met_pt * sin(t.met_phi);
  float lepX = hardleppt_ * cos(hardlepphi_);
  float lepY = hardleppt_ * sin(hardlepphi_);
  TVector2* newMet = new TVector2;
  newMet->SetX(metX+lepX);
  newMet->SetY(metY+lepY);

  float mt = sqrt( 2 * t.met_pt * softleppt_ * ( 1 - cos( t.met_phi - softlepphi_) ) );
  float mt_hard = sqrt( 2 * t.met_pt * hardleppt_ * ( 1 - cos( t.met_phi - hardlepphi_) ) );
  float mt_alt = sqrt( 2 * newMet->Mod() * softleppt_ * ( 1 - cos( newMet->Phi() - softlepphi_) ) );

  float lepDR = DeltaR(softlepeta_, hardlepeta_, softlepphi_, hardlepphi_);

  //plot1D("h_mt"+s,            mt,   evtweight_, h_1d, ";M_{T} [GeV]", 250, 0, 250); //filled in fillHistosSingleSoftLepton
  plot1D("h_mtHard"+s,            mt_hard,   evtweight_, h_1d, ";M_{T} [GeV]", 250, 0, 250);
  plot1D("h_mtAlternate"+s,            mt_alt,   evtweight_, h_1d, ";M_{T} [GeV]", 250, 0, 250);
  plot1D("h_mtbins"+s,            mt,   evtweight_, h_1d, ";M_{T} [GeV]", n_mt2bins, mt2bins);
  plot1D("h_lowleppt"+s,      softleppt_,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 30, 0, 30);
  plot1D("h_highleppt"+s,      hardleppt_,   evtweight_, h_1d, ";p_{T}(lep) [GeV]", 200, 0, 1000);
  plot1D("h_deltaRLep"+s,       lepDR, evtweight_, h_1d, ";deltaR", 1000, 0, 10);
  plot1D("h_deltaEtaLep"+s,       hardlepeta_-softlepeta_, evtweight_, h_1d, ";deltaEta", 80, -4, 4);
  plot1D("h_deltaPhiLep"+s,       hardlepphi_-softlepphi_, evtweight_, h_1d, ";deltaPhi", 140, -7, 7);
  plot1D("h_deltaPtLep"+s,       hardleppt_-softleppt_, evtweight_, h_1d, ";deltaPt", 200, 0, 200);
  plot1D("h_dilepPt"+s,       dilepp4.Pt(), evtweight_, h_1d, ";Pt", 200, 0, 200);

  //save type
  int type = -1;
  if (hardleppt_>20) type = 2;
  else if (hardleppt_<20 && hardleppt_>10) type = 1;
  else if (hardleppt_>5 && hardleppt_<10) type = 0;
  plot1D("h_type"+s,      type,   evtweight_, h_1d, ";type", 3, 0, 3);

  //zll mt bins
  float zllmtbins[4] = {20., 100., 130., 300.};
  
  //zllCR specific plots
  plot1D("h_zllmt"+s,            zllmt_,   evtweight_, h_1d, ";M_{T} [GeV]", 250, 0, 250);
  plot1D("h_zllmtbins"+s,            zllmt_,   evtweight_, h_1d, ";M_{T} [GeV]", n_mt2bins, mt2bins);
  plot1D("h_zllAltmtbins"+s,            zllmt_,   evtweight_, h_1d, ";M_{T} [GeV]", 3, zllmtbins);
  plot1D("h_zllmet"+s,       zllmet_,   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 100, 0, 1000);
  
  plot1D("h_removedZlllep"+s,       removedzllLep_,   evtweight_, h_1d, "removedLep", 3, 0, 3);
  if (removedzllLep_ == 1) plot1D("h_removedZllleppt"+s,      hardleppt_,   evtweight_, h_1d, ";rem p_{T}(lep) [GeV]", 200, 0, 1000);
  else if (removedzllLep_ == 2) plot1D("h_removedZllleppt"+s,      softleppt_,   evtweight_, h_1d, ";rem p_{T}(lep) [GeV]", 200, 0, 1000);
  
  delete newMet;
  
  outfile_->cd();

  fillHistosSingleSoftLepton(h_1d, n_mt2bins, mt2bins, dirname, s);
  return;
}

void MT2Looper::fillLepUncSR() {

  // lepton efficiency variation in signal region for fastsim: large uncertainty on leptons NOT vetoed
  cor_lepeff_sr_ = 0.;
  unc_lepeff_sr_ = 0.;
  if (t.ngenLep > 0 || t.ngenLepFromTau > 0) {
    // loop on gen e/mu
    for (int ilep = 0; ilep < t.ngenLep; ++ilep) {
      // check acceptance for veto: pt > 5, |eta| < 2.4
      if (t.genLep_pt[ilep] < 5.) continue;
      if (fabs(t.genLep_eta[ilep]) > 2.4) continue;
      // look up SF and vetoeff, by flavor
      weightStruct sf_struct = getLepSFFromFile(t.genLep_pt[ilep], t.genLep_eta[ilep], t.genLep_pdgId[ilep]);
      float sf = sf_struct.cent;
      float vetoeff = getLepVetoEffFromFile_fullsim(t.genLep_pt[ilep], t.genLep_eta[ilep], t.genLep_pdgId[ilep]);
      float unc = sf_struct.up - sf;
      float vetoeff_unc_UP = vetoeff * (1. + unc);
      float unc_UP_0l = ( (1. - vetoeff_unc_UP) / (1. - vetoeff) ) - 1.;
      unc_lepeff_sr_ += unc_UP_0l;
    }
    for (int ilep = 0; ilep < t.ngenLepFromTau; ++ilep) {
      // check acceptance for veto: pt > 5
      if (t.genLepFromTau_pt[ilep] < 5.) continue;
      if (fabs(t.genLepFromTau_eta[ilep]) > 2.4) continue;
      weightStruct sf_struct = getLepSFFromFile(t.genLepFromTau_pt[ilep], t.genLepFromTau_eta[ilep], t.genLepFromTau_pdgId[ilep]);
      float sf = sf_struct.cent;
      float vetoeff = getLepVetoEffFromFile_fullsim(t.genLepFromTau_pt[ilep], t.genLepFromTau_eta[ilep], t.genLepFromTau_pdgId[ilep]);
      float unc = sf_struct.up - sf;
      float vetoeff_unc_UP = vetoeff * (1. + unc);
      float unc_UP_0l = ( (1. - vetoeff_unc_UP) / (1. - vetoeff) ) - 1.;
      unc_lepeff_sr_ += unc_UP_0l;
    }
  }

  return;
}

void MT2Looper::fillLepCorSRfastsim() {

  // lepton efficiency variation in signal region for fastsim: large uncertainty on leptons NOT vetoed
  cor_lepeff_sr_ = 0.;
  unc_lepeff_sr_ = 0.;
  if (t.ngenLep > 0 || t.ngenLepFromTau > 0) {
    // loop on gen e/mu
    for (int ilep = 0; ilep < t.ngenLep; ++ilep) {
      // check acceptance for veto: pt > 5, |eta| < 2.4
      if (t.genLep_pt[ilep] < 5.) continue;
      if (fabs(t.genLep_eta[ilep]) > 2.4) continue;
      // look up SF and vetoeff, by flavor
      weightStruct sf_struct = getLepSFFromFile_fastsim(t.genLep_pt[ilep], t.genLep_eta[ilep], t.genLep_pdgId[ilep]);
      float sf = sf_struct.cent;
      float vetoeff = getLepVetoEffFromFile_fastsim(t.genLep_pt[ilep], t.genLep_eta[ilep], t.genLep_pdgId[ilep]);
      // apply SF to vetoeff, then correction for 0L will be (1 - vetoeff_cor) / (1 - vetoeff) - 1.
      float vetoeff_cor = vetoeff * sf;
      float cor_0l = ( (1. - vetoeff_cor) / (1. - vetoeff) ) - 1.;
      cor_lepeff_sr_ += cor_0l;
      float unc = sf_struct.up - sf;
      float vetoeff_cor_unc_UP = vetoeff_cor * (1. + unc);
      float unc_UP_0l = ( (1. - vetoeff_cor_unc_UP) / (1. - vetoeff_cor) ) - 1.;
      unc_lepeff_sr_ += unc_UP_0l;
    }
    for (int ilep = 0; ilep < t.ngenLepFromTau; ++ilep) {
      // check acceptance for veto: pt > 5
      if (t.genLepFromTau_pt[ilep] < 5.) continue;
      if (fabs(t.genLepFromTau_eta[ilep]) > 2.4) continue;
      weightStruct sf_struct = getLepSFFromFile_fastsim(t.genLepFromTau_pt[ilep], t.genLepFromTau_eta[ilep], t.genLepFromTau_pdgId[ilep]);
      float sf = sf_struct.cent;
      float vetoeff = getLepVetoEffFromFile_fastsim(t.genLepFromTau_pt[ilep], t.genLepFromTau_eta[ilep], t.genLepFromTau_pdgId[ilep]);
      // apply SF to vetoeff, then correction for 0L will be 1. - (1 - vetoeff_cor) / (1 - vetoeff)
      float vetoeff_cor = vetoeff * sf;
      float cor_0l = ( (1. - vetoeff_cor) / (1. - vetoeff) ) - 1.;
      cor_lepeff_sr_ += cor_0l;
      float unc = sf_struct.up - sf;
      float vetoeff_cor_unc_UP = vetoeff_cor * (1. + unc);
      float unc_UP_0l = ( (1. - vetoeff_cor_unc_UP) / (1. - vetoeff_cor) ) - 1.;
      unc_lepeff_sr_ += unc_UP_0l;
    }
  }

  return;
}


void MT2Looper::fillMissLepSF(int igenlep, bool isFromTau, bool isFastsim, float & lostSF, float & lostSFup, float & lostSFdn) {
  
  float pt  = 0;
  float eta = 0;
  int   id  = 0;
  if (isFromTau) {
    pt  = t.genLepFromTau_pt[igenlep];	
    eta = t.genLepFromTau_eta[igenlep];	
    id  = t.genLepFromTau_pdgId[igenlep];
  }
  else {
    pt  = t.genLep_pt[igenlep];	
    eta = t.genLep_eta[igenlep];	
    id  = t.genLep_pdgId[igenlep];
  }

  weightStruct sf_struct = getLepSFFromFile(pt, eta, id);
  if (isFastsim) sf_struct = getLepSFFromFile_fastsim(pt, eta, id);
  float sf = sf_struct.cent;
  float sfup = sf_struct.up;
  float sfdn = sf_struct.dn;
  float vetoeff = getLepVetoEffFromFile_fullsim(pt, eta, id);
  if (isFastsim) vetoeff = getLepVetoEffFromFile_fastsim(pt, eta, id);

  if (isnan(sf) || isnan(sfup) || isnan(sfdn)) {
    cerr << "WARNING: some lep scale factors are NAN in evt " << t.evt << ". Setting SF to one..." << endl;
    sf = 1;
    sfup = 1;
    sfdn = 1;
  }
  else if (abs(id) == 13 && fabs(eta) > 2.4 ) {
    cerr << "WARNING: muon outside acceptance in evt " << t.evt << ". Setting SF to one..." << endl;
    sf = 1;
    sfup = 1;
    sfdn = 1;
  }
  
  lostSF = (1 - vetoeff * sf) / (1 - vetoeff);
  lostSFup = (1 - vetoeff * sfup) / (1 - vetoeff);
  lostSFdn = (1 - vetoeff * sfdn) / (1 - vetoeff);

  return;
}

