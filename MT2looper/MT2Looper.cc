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

const int n_htbins = 5;
const float htbins[n_htbins+1] = {200, 450., 575., 1000., 1500., 3000.};
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
// get signal scan nevents from file
bool doScanWeights = false;
// doesn't plot data for MT2 > 200 in signal regions
bool doBlindData = true;
// make variation histograms for tau efficiency
bool doGenTauVars = false;

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

  //SRVec =  getSignalRegionsZurich_jetpt30(); //same as getSignalRegionsZurich(), but with j1pt and j2pt cuts changed to 30 GeV
  SRVec =  getSignalRegionsJamboree(); //adds HT 200-450 regions
  SRVecMonojet = getSignalRegionsMonojet(); // first pass of monojet regions

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

    dir = (TDirectory*)outfile_->Get(("crrl"+SRVec.at(i).GetName()).c_str());
    if (dir == 0) {
      dir = outfile_->mkdir(("crrl"+SRVec.at(i).GetName()).c_str());
    } 
    dir->cd();
    for(unsigned int j = 0; j < vars.size(); j++){
      plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRVec.at(i).GetLowerBound(vars.at(j)), SRVec.at(i).crrlHistMap, "", 1, 0, 2);
      plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRVec.at(i).GetUpperBound(vars.at(j)), SRVec.at(i).crrlHistMap, "", 1, 0, 2);
    }
    plot1D("h_n_mt2bins",  1, SRVec.at(i).GetNumberOfMT2Bins(), SRVec.at(i).crrlHistMap, "", 1, 0, 2);

    outfile_->cd();
  }

  //monojet regions: store histograms with cut values for all variables
  for(unsigned int i = 0; i < SRVecMonojet.size(); i++){
    std::vector<std::string> vars = SRVecMonojet.at(i).GetListOfVariables();
    TDirectory * dir = (TDirectory*)outfile_->Get(("sr"+SRVecMonojet.at(i).GetName()).c_str());
    if (dir == 0) {
      dir = outfile_->mkdir(("sr"+SRVecMonojet.at(i).GetName()).c_str());
    } 
    dir->cd();
    for(unsigned int j = 0; j < vars.size(); j++){
      plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRVecMonojet.at(i).GetLowerBound(vars.at(j)), SRVecMonojet.at(i).srHistMap, "", 1, 0, 2);
      plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRVecMonojet.at(i).GetUpperBound(vars.at(j)), SRVecMonojet.at(i).srHistMap, "", 1, 0, 2);
    }
    plot1D("h_n_mt2bins",  1, SRVecMonojet.at(i).GetNumberOfMT2Bins(), SRVecMonojet.at(i).srHistMap, "", 1, 0, 2);

    dir = (TDirectory*)outfile_->Get(("crsl"+SRVecMonojet.at(i).GetName()).c_str());
    if (dir == 0) {
      dir = outfile_->mkdir(("crsl"+SRVecMonojet.at(i).GetName()).c_str());
    } 
    dir->cd();
    std::vector<std::string> varsCRSL = SRVecMonojet.at(i).GetListOfVariablesCRSL();
    for(unsigned int j = 0; j < varsCRSL.size(); j++){
      plot1D("h_"+varsCRSL.at(j)+"_"+"LOW",  1, SRVecMonojet.at(i).GetLowerBoundCRSL(varsCRSL.at(j)), SRVecMonojet.at(i).crslHistMap, "", 1, 0, 2);
      plot1D("h_"+varsCRSL.at(j)+"_"+"HI",   1, SRVecMonojet.at(i).GetUpperBoundCRSL(varsCRSL.at(j)), SRVecMonojet.at(i).crslHistMap, "", 1, 0, 2);
    }
    plot1D("h_n_mt2bins",  1, SRVecMonojet.at(i).GetNumberOfMT2Bins(), SRVecMonojet.at(i).crslHistMap, "", 1, 0, 2);

    dir = (TDirectory*)outfile_->Get(("crgj"+SRVecMonojet.at(i).GetName()).c_str());
    if (dir == 0) {
      dir = outfile_->mkdir(("crgj"+SRVecMonojet.at(i).GetName()).c_str());
    } 
    dir->cd();
    for(unsigned int j = 0; j < vars.size(); j++){
      plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRVecMonojet.at(i).GetLowerBound(vars.at(j)), SRVecMonojet.at(i).crgjHistMap, "", 1, 0, 2);
      plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRVecMonojet.at(i).GetUpperBound(vars.at(j)), SRVecMonojet.at(i).crgjHistMap, "", 1, 0, 2);
    }
    plot1D("h_n_mt2bins",  1, SRVecMonojet.at(i).GetNumberOfMT2Bins(), SRVecMonojet.at(i).crgjHistMap, "", 1, 0, 2);
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
  plot1D("h_n_mt2bins",  1, SRBase.GetNumberOfMT2Bins(), SRBase.crslHistMap, "", 1, 0, 2);
  outfile_->cd();

  std::vector<std::string> varsCRRL = SRBase.GetListOfVariables();
  TDirectory * dirCRRL = (TDirectory*)outfile_->Get("crrlbase");
  if (dirCRRL == 0) {
    dirCRRL = outfile_->mkdir("crrlbase");
  } 
  dirCRRL->cd();
  for(unsigned int j = 0; j < varsCRRL.size(); j++){
    plot1D("h_"+varsCRRL.at(j)+"_"+"LOW",  1, SRBase.GetLowerBound(varsCRRL.at(j)), SRBase.crrlHistMap, "", 1, 0, 2);
    plot1D("h_"+varsCRRL.at(j)+"_"+"HI",   1, SRBase.GetUpperBound(varsCRRL.at(j)), SRBase.crrlHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBase.GetNumberOfMT2Bins(), SRBase.crrlHistMap, "", 1, 0, 2);
  outfile_->cd();

  //setup inclusive regions
  SR InclusiveHT200to450 = SRBase;
  InclusiveHT200to450.SetName("srbaseVL");
  InclusiveHT200to450.SetVar("ht", 200, 450);
  InclusiveRegions.push_back(InclusiveHT200to450);

  SR InclusiveHT450to575 = SRBase;
  InclusiveHT450to575.SetName("srbaseL");
  InclusiveHT450to575.SetVar("ht", 450, 575);
  InclusiveRegions.push_back(InclusiveHT450to575);

  SR InclusiveHT575to1000 = SRBase;
  InclusiveHT575to1000.SetName("srbaseM");
  InclusiveHT575to1000.SetVar("ht", 575, 1000);
  InclusiveRegions.push_back(InclusiveHT575to1000);

  SR InclusiveHT1000to1500 = SRBase;
  InclusiveHT1000to1500.SetName("srbaseH");
  InclusiveHT1000to1500.SetVar("ht", 1000, 1500);
  InclusiveRegions.push_back(InclusiveHT1000to1500);

  SR InclusiveHT1500toInf = SRBase;
  InclusiveHT1500toInf.SetName("srbaseUH");
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

  vars = SRBaseMonojet.GetListOfVariables();
  dir = (TDirectory*)outfile_->Get((SRBaseMonojet.GetName()).c_str());
  if (dir == 0) {
    dir = outfile_->mkdir((SRBaseMonojet.GetName()).c_str());
  } 
  dir->cd();
  for(unsigned int j = 0; j < vars.size(); j++){
    plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRBaseMonojet.GetLowerBound(vars.at(j)), SRBaseMonojet.srHistMap, "", 1, 0, 2);
    plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRBaseMonojet.GetUpperBound(vars.at(j)), SRBaseMonojet.srHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.srHistMap, "", 1, 0, 2);
  outfile_->cd();

  varsCRSL = SRBaseMonojet.GetListOfVariablesCRSL();
  dirCRSL = (TDirectory*)outfile_->Get("crslbaseJ");
  if (dirCRSL == 0) {
    dirCRSL = outfile_->mkdir("crslbaseJ");
  } 
  dirCRSL->cd();
  for(unsigned int j = 0; j < varsCRSL.size(); j++){
    plot1D("h_"+varsCRSL.at(j)+"_"+"LOW",  1, SRBaseMonojet.GetLowerBoundCRSL(varsCRSL.at(j)), SRBaseMonojet.crslHistMap, "", 1, 0, 2);
    plot1D("h_"+varsCRSL.at(j)+"_"+"HI",   1, SRBaseMonojet.GetUpperBoundCRSL(varsCRSL.at(j)), SRBaseMonojet.crslHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.crslHistMap, "", 1, 0, 2);
  outfile_->cd();

  dir = (TDirectory*)outfile_->Get("crgjbaseJ");
  if (dir == 0) {
    dir = outfile_->mkdir("crgjbaseJ");
  } 
  dir->cd();
  for(unsigned int j = 0; j < vars.size(); j++){
    plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRBaseMonojet.GetLowerBound(vars.at(j)), SRBaseMonojet.crgjHistMap, "", 1, 0, 2);
    plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRBaseMonojet.GetUpperBound(vars.at(j)), SRBaseMonojet.crgjHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.crgjHistMap, "", 1, 0, 2);
  outfile_->cd();

  dir = (TDirectory*)outfile_->Get("crdybaseJ");
  if (dir == 0) {
    dir = outfile_->mkdir("crdybaseJ");
  } 
  dir->cd();
  for(unsigned int j = 0; j < vars.size(); j++){
    plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRBaseMonojet.GetLowerBound(vars.at(j)), SRBaseMonojet.crdyHistMap, "", 1, 0, 2);
    plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRBaseMonojet.GetUpperBound(vars.at(j)), SRBaseMonojet.crdyHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.crdyHistMap, "", 1, 0, 2);
  outfile_->cd();

  dir = (TDirectory*)outfile_->Get("crrlbaseJ");
  if (dir == 0) {
    dir = outfile_->mkdir("crrlbaseJ");
  } 
  dir->cd();
  for(unsigned int j = 0; j < vars.size(); j++){
    plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRBaseMonojet.GetLowerBound(vars.at(j)), SRBaseMonojet.crrlHistMap, "", 1, 0, 2);
    plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRBaseMonojet.GetUpperBound(vars.at(j)), SRBaseMonojet.crrlHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.crrlHistMap, "", 1, 0, 2);
  outfile_->cd();


  SRBaseMonojet0B.SetName("srbaseJ0B");
  SRBaseMonojet0B.SetVar("j1pt", 200, -1);
  SRBaseMonojet0B.SetVar("njets", 1, 2);
  SRBaseMonojet0B.SetVar("nbjets", 0, 1);
  SRBaseMonojet0B.SetVar("nlep", 0, 1);
  SRBaseMonojet0B.SetVar("met", 200, -1);
  SRBaseMonojet0B.SetVar("deltaPhiMin", 0.3, -1);
  SRBaseMonojet0B.SetVar("diffMetMhtOverMet", 0, 0.5);
  SRBaseMonojet0B.SetVarCRSL("j1pt", 200, -1);
  SRBaseMonojet0B.SetVarCRSL("njets", 1, 2);
  SRBaseMonojet0B.SetVarCRSL("nbjets", 0, 1);
  SRBaseMonojet0B.SetVarCRSL("nlep", 1, 2);
  SRBaseMonojet0B.SetVarCRSL("met", 200, -1);
  SRBaseMonojet0B.SetVarCRSL("deltaPhiMin", 0.3, -1);
  SRBaseMonojet0B.SetVarCRSL("diffMetMhtOverMet", 0, 0.5);
  float SRBaseMonojet0B_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500};
  SRBaseMonojet0B.SetMT2Bins(7, SRBaseMonojet0B_mt2bins);

  vars = SRBaseMonojet0B.GetListOfVariables();
  dir = (TDirectory*)outfile_->Get((SRBaseMonojet0B.GetName()).c_str());
  if (dir == 0) {
    dir = outfile_->mkdir((SRBaseMonojet0B.GetName()).c_str());
  } 
  dir->cd();
  for(unsigned int j = 0; j < vars.size(); j++){
    plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRBaseMonojet0B.GetLowerBound(vars.at(j)), SRBaseMonojet0B.srHistMap, "", 1, 0, 2);
    plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRBaseMonojet0B.GetUpperBound(vars.at(j)), SRBaseMonojet0B.srHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet0B.GetNumberOfMT2Bins(), SRBaseMonojet0B.srHistMap, "", 1, 0, 2);
  outfile_->cd();

  varsCRSL = SRBaseMonojet0B.GetListOfVariablesCRSL();
  dirCRSL = (TDirectory*)outfile_->Get("crslbaseJ0B");
  if (dirCRSL == 0) {
    dirCRSL = outfile_->mkdir("crslbaseJ0B");
  } 
  dirCRSL->cd();
  for(unsigned int j = 0; j < varsCRSL.size(); j++){
    plot1D("h_"+varsCRSL.at(j)+"_"+"LOW",  1, SRBaseMonojet0B.GetLowerBoundCRSL(varsCRSL.at(j)), SRBaseMonojet0B.crslHistMap, "", 1, 0, 2);
    plot1D("h_"+varsCRSL.at(j)+"_"+"HI",   1, SRBaseMonojet0B.GetUpperBoundCRSL(varsCRSL.at(j)), SRBaseMonojet0B.crslHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet0B.GetNumberOfMT2Bins(), SRBaseMonojet0B.crslHistMap, "", 1, 0, 2);
  outfile_->cd();

  dir = (TDirectory*)outfile_->Get("crgjbaseJ0B");
  if (dir == 0) {
    dir = outfile_->mkdir("crgjbaseJ0B");
  } 
  dir->cd();
  for(unsigned int j = 0; j < vars.size(); j++){
    plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRBaseMonojet0B.GetLowerBound(vars.at(j)), SRBaseMonojet0B.crgjHistMap, "", 1, 0, 2);
    plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRBaseMonojet0B.GetUpperBound(vars.at(j)), SRBaseMonojet0B.crgjHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet0B.GetNumberOfMT2Bins(), SRBaseMonojet0B.crgjHistMap, "", 1, 0, 2);
  outfile_->cd();

  dir = (TDirectory*)outfile_->Get("crdybaseJ0B");
  if (dir == 0) {
    dir = outfile_->mkdir("crdybaseJ0B");
  } 
  dir->cd();
  for(unsigned int j = 0; j < vars.size(); j++){
    plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRBaseMonojet0B.GetLowerBound(vars.at(j)), SRBaseMonojet0B.crdyHistMap, "", 1, 0, 2);
    plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRBaseMonojet0B.GetUpperBound(vars.at(j)), SRBaseMonojet0B.crdyHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet0B.GetNumberOfMT2Bins(), SRBaseMonojet0B.crdyHistMap, "", 1, 0, 2);
  outfile_->cd();

  dir = (TDirectory*)outfile_->Get("crrlbaseJ0B");
  if (dir == 0) {
    dir = outfile_->mkdir("crrlbaseJ0B");
  } 
  dir->cd();
  for(unsigned int j = 0; j < vars.size(); j++){
    plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRBaseMonojet0B.GetLowerBound(vars.at(j)), SRBaseMonojet0B.crrlHistMap, "", 1, 0, 2);
    plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRBaseMonojet0B.GetUpperBound(vars.at(j)), SRBaseMonojet0B.crrlHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet0B.GetNumberOfMT2Bins(), SRBaseMonojet0B.crrlHistMap, "", 1, 0, 2);
  outfile_->cd();


  SRBaseMonojet1B.SetName("srbaseJ1B");
  SRBaseMonojet1B.SetVar("j1pt", 200, -1);
  SRBaseMonojet1B.SetVar("njets", 1, 2);
  SRBaseMonojet1B.SetVar("nbjets", 1, -1);
  SRBaseMonojet1B.SetVar("nlep", 0, 1);
  SRBaseMonojet1B.SetVar("met", 200, -1);
  SRBaseMonojet1B.SetVar("deltaPhiMin", 0.3, -1);
  SRBaseMonojet1B.SetVar("diffMetMhtOverMet", 0, 0.5);
  SRBaseMonojet1B.SetVarCRSL("j1pt", 200, -1);
  SRBaseMonojet1B.SetVarCRSL("njets", 1, 2);
  SRBaseMonojet1B.SetVarCRSL("nbjets", 1, -1);
  SRBaseMonojet1B.SetVarCRSL("nlep", 1, 2);
  SRBaseMonojet1B.SetVarCRSL("met", 200, -1);
  SRBaseMonojet1B.SetVarCRSL("deltaPhiMin", 0.3, -1);
  SRBaseMonojet1B.SetVarCRSL("diffMetMhtOverMet", 0, 0.5);
  float SRBaseMonojet1B_mt2bins[7] = {200, 300, 400, 500, 600, 800, 1500};
  SRBaseMonojet1B.SetMT2Bins(6, SRBaseMonojet1B_mt2bins);

  vars = SRBaseMonojet1B.GetListOfVariables();
  dir = (TDirectory*)outfile_->Get((SRBaseMonojet1B.GetName()).c_str());
  if (dir == 0) {
    dir = outfile_->mkdir((SRBaseMonojet1B.GetName()).c_str());
  } 
  dir->cd();
  for(unsigned int j = 0; j < vars.size(); j++){
    plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRBaseMonojet1B.GetLowerBound(vars.at(j)), SRBaseMonojet1B.srHistMap, "", 1, 0, 2);
    plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRBaseMonojet1B.GetUpperBound(vars.at(j)), SRBaseMonojet1B.srHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet1B.GetNumberOfMT2Bins(), SRBaseMonojet1B.srHistMap, "", 1, 0, 2);
  outfile_->cd();

  varsCRSL = SRBaseMonojet1B.GetListOfVariablesCRSL();
  dirCRSL = (TDirectory*)outfile_->Get("crslbaseJ1B");
  if (dirCRSL == 0) {
    dirCRSL = outfile_->mkdir("crslbaseJ1B");
  } 
  dirCRSL->cd();
  for(unsigned int j = 0; j < varsCRSL.size(); j++){
    plot1D("h_"+varsCRSL.at(j)+"_"+"LOW",  1, SRBaseMonojet1B.GetLowerBoundCRSL(varsCRSL.at(j)), SRBaseMonojet1B.crslHistMap, "", 1, 0, 2);
    plot1D("h_"+varsCRSL.at(j)+"_"+"HI",   1, SRBaseMonojet1B.GetUpperBoundCRSL(varsCRSL.at(j)), SRBaseMonojet1B.crslHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet1B.GetNumberOfMT2Bins(), SRBaseMonojet1B.crslHistMap, "", 1, 0, 2);
  outfile_->cd();

  dir = (TDirectory*)outfile_->Get("crgjbaseJ1B");
  if (dir == 0) {
    dir = outfile_->mkdir("crgjbaseJ1B");
  } 
  dir->cd();
  for(unsigned int j = 0; j < vars.size(); j++){
    plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRBaseMonojet1B.GetLowerBound(vars.at(j)), SRBaseMonojet1B.crgjHistMap, "", 1, 0, 2);
    plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRBaseMonojet1B.GetUpperBound(vars.at(j)), SRBaseMonojet1B.crgjHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet1B.GetNumberOfMT2Bins(), SRBaseMonojet1B.crgjHistMap, "", 1, 0, 2);
  outfile_->cd();

  dir = (TDirectory*)outfile_->Get("crdybaseJ1B");
  if (dir == 0) {
    dir = outfile_->mkdir("crdybaseJ1B");
  } 
  dir->cd();
  for(unsigned int j = 0; j < vars.size(); j++){
    plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRBaseMonojet1B.GetLowerBound(vars.at(j)), SRBaseMonojet1B.crdyHistMap, "", 1, 0, 2);
    plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRBaseMonojet1B.GetUpperBound(vars.at(j)), SRBaseMonojet1B.crdyHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet1B.GetNumberOfMT2Bins(), SRBaseMonojet1B.crdyHistMap, "", 1, 0, 2);
  outfile_->cd();

  dir = (TDirectory*)outfile_->Get("crrlbaseJ1B");
  if (dir == 0) {
    dir = outfile_->mkdir("crrlbaseJ1B");
  } 
  dir->cd();
  for(unsigned int j = 0; j < vars.size(); j++){
    plot1D("h_"+vars.at(j)+"_"+"LOW",  1, SRBaseMonojet1B.GetLowerBound(vars.at(j)), SRBaseMonojet1B.crrlHistMap, "", 1, 0, 2);
    plot1D("h_"+vars.at(j)+"_"+"HI",   1, SRBaseMonojet1B.GetUpperBound(vars.at(j)), SRBaseMonojet1B.crrlHistMap, "", 1, 0, 2);
  }
  plot1D("h_n_mt2bins",  1, SRBaseMonojet1B.GetNumberOfMT2Bins(), SRBaseMonojet1B.crrlHistMap, "", 1, 0, 2);
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

  h_nvtx_weights_ = 0;
  if (doNvtxReweight) {
    TFile* f_weights = new TFile("../babymaker/data/hists_reweight_zjets_Run2015B.root");
    TH1D* h_nvtx_weights_temp = (TH1D*) f_weights->Get("h_nVert_ratio");
    outfile_->cd();
    h_nvtx_weights_ = (TH1D*) h_nvtx_weights_temp->Clone("h_nvtx_weights");
    f_weights->Close();
  }

  h_sig_nevents_ = 0;
  if (doScanWeights && ((sample.find("T1") != std::string::npos) || (sample.find("T2") != std::string::npos))) {
    TFile* f_nsig = new TFile(Form("../babymaker/data/nsig_%s.root",sample.c_str()));
    TH2D* h_sig_nevents_temp = (TH2D*) f_nsig->Get("h_nsig");
    outfile_->cd();
    h_sig_nevents_ = (TH2D*) h_sig_nevents_temp->Clone("h_sig_nevents");
    f_nsig->Close();
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
      //      const float lumi = 3;
      //      const float lumi = 0.042;
      //      const float lumi = 0.209;
      //      const float lumi = 0.579;
      const float lumi = 1.264;

      evtweight_ = 1.;

      // apply relevant weights to MC
      if (!t.isData) {
	if (isSignal_ && doScanWeights) {
	  int binx = h_sig_nevents_->GetXaxis()->FindBin(t.GenSusyMScan1);
	  int biny = h_sig_nevents_->GetYaxis()->FindBin(t.GenSusyMScan2);
	  double nevents = h_sig_nevents_->GetBinContent(binx,biny);
	  evtweight_ = lumi * t.evt_xsec*1000./nevents; // assumes xsec is already filled correctly
	} else {
	  evtweight_ = t.evt_scale1fb * lumi;
	}
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

      // check jet id for monojet
      passMonojetId_ = false;
      if (t.nJet30 == 1) {
	// loop to find central monojet
	for (int ijet = 0; ijet < t.njet; ++ijet) {
	  if (fabs(t.jet_eta[ijet]) > 2.5) continue;
	  if (t.jet_pt[ijet] < 200.) continue;
	  if (t.jet_id[ijet] >= 4) passMonojetId_ = true;
	  break;
	}
      }

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
      if (t.ngamma > 0) {
	if (t.isData) {
	  doGJplots = true;
	}
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
	else {
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

      // Variables for Zll (DY) control region
      bool doDYplots = false;
      if (t.nlep == 2 && !isSignal_) {
	if ( (t.lep_charge[0] * t.lep_charge[1] == -1)
	     && (abs(t.lep_pdgId[0]) == abs(t.lep_pdgId[1]) )
	     && (fabs(t.zll_mass - 90) < 10 ) 
	     && t.lep_pt[0] > 25 && t.lep_pt[1] > 10
	     && (t.HLT_DoubleEl || t.HLT_DoubleMu || t.HLT_Photon165_HE10)
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
      
      if ( !(t.isData && doBlindData && t.mt2 > 200) ) {
	fillHistos(SRNoCut.srHistMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), "");

	fillHistosSignalRegion("sr");

	fillHistosSRBase();
	fillHistosInclusive();
      }

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
  if (passMonojetId_ && (!t.isData || t.HLT_PFMETNoMu90_PFMHTNoMu90)) {
    std::map<std::string, float> values_monojet;
    values_monojet["deltaPhiMin"] = t.deltaPhiMin;
    values_monojet["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
    values_monojet["nlep"]        = nlepveto_;
    values_monojet["j1pt"]        = t.jet1_pt;
    values_monojet["njets"]       = t.nJet30;
    values_monojet["met"]         = t.met_pt;
    std::map<std::string, float> values_monojetB;
    values_monojetB["deltaPhiMin"] = t.deltaPhiMin;
    values_monojetB["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
    values_monojetB["nlep"]        = nlepveto_;
    values_monojetB["j1pt"]        = t.jet1_pt;
    values_monojetB["njets"]       = t.nJet30;
    values_monojetB["nbjets"]      = t.nBJet20;
    values_monojetB["met"]         = t.met_pt;

    if(SRBaseMonojet.PassesSelection(values_monojet)) fillHistos(SRBaseMonojet.srHistMap, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.GetMT2Bins(), SRBaseMonojet.GetName(), "");
    if(SRBaseMonojet0B.PassesSelection(values_monojetB)) fillHistos(SRBaseMonojet0B.srHistMap, SRBaseMonojet0B.GetNumberOfMT2Bins(), SRBaseMonojet0B.GetMT2Bins(), SRBaseMonojet0B.GetName(), "");
    if(SRBaseMonojet1B.PassesSelection(values_monojetB)) fillHistos(SRBaseMonojet1B.srHistMap, SRBaseMonojet1B.GetNumberOfMT2Bins(), SRBaseMonojet1B.GetMT2Bins(), SRBaseMonojet1B.GetName(), "");
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
  if (passMonojetId_ && (!t.isData || t.HLT_PFMETNoMu90_PFMHTNoMu90)) {
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
	break;//signal regions are orthogonal, event cannot be in more than one
      }
    }
  } // monojet regions

  return;
}

// hists for single lepton control region
void MT2Looper::fillHistosCRSL(const std::string& prefix, const std::string& suffix) {

  // trigger requirement on data
  if (t.isData && !(t.HLT_PFHT800 || t.HLT_PFHT350_PFMET100 || t.HLT_PFMETNoMu90_PFMHTNoMu90)) return;
  
  // first fill base region
  std::map<std::string, float> valuesBase;
  valuesBase["deltaPhiMin"] = t.deltaPhiMin;
  valuesBase["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
  valuesBase["nlep"]        = t.nLepLowMT;
  valuesBase["j1pt"]        = t.jet1_pt;
  valuesBase["j2pt"]        = t.jet2_pt;
  valuesBase["mt2"]         = t.mt2;
  valuesBase["passesHtMet"] = ( (t.ht > 200. && t.met_pt > 200.) || (t.ht > 1000. && t.met_pt > 30.) );

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
    valuesInc["passesHtMet"] = ( (t.ht > 200. && t.met_pt > 200.) || (t.ht > 1000. && t.met_pt > 30.) );
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

  // do monojet SRs
  if (passMonojetId_ && (!t.isData || t.HLT_PFMETNoMu90_PFMHTNoMu90)) {

    // first fill base region
    std::map<std::string, float> valuesBase_monojet;
    valuesBase_monojet["deltaPhiMin"] = t.deltaPhiMin;
    valuesBase_monojet["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
    valuesBase_monojet["nlep"]        = t.nLepLowMT;
    valuesBase_monojet["j1pt"]        = t.jet1_pt;
    valuesBase_monojet["njets"]       = t.nJet30;
    valuesBase_monojet["met"]         = t.met_pt;

    if (SRBaseMonojet.PassesSelectionCRSL(valuesBase_monojet)) {
      if(prefix=="crsl") fillHistosSingleLepton(SRBaseMonojet.crslHistMap, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.GetMT2Bins(), "crslbaseJ", suffix);
      else if(prefix=="crslmu") fillHistosSingleLepton(SRBaseMonojet.crslmuHistMap, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.GetMT2Bins(), "crslmubaseJ", suffix);
      else if(prefix=="crslel") fillHistosSingleLepton(SRBaseMonojet.crslelHistMap, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.GetMT2Bins(), "crslelbaseJ", suffix);
    }

    
    std::map<std::string, float> values_monojet;
    values_monojet["deltaPhiMin"] = t.deltaPhiMin;
    values_monojet["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
    values_monojet["nlep"]        = t.nLepLowMT;
    values_monojet["j1pt"]        = t.jet1_pt;
    values_monojet["njets"]       = t.nJet30;
    values_monojet["nbjets"]      = t.nBJet20;
    values_monojet["ht"]          = t.ht;
    values_monojet["met"]         = t.met_pt;

    for(unsigned int srN = 0; srN < SRVecMonojet.size(); srN++){
      if(SRVecMonojet.at(srN).PassesSelectionCRSL(values_monojet)){
	if(prefix=="crsl")    fillHistosSingleLepton(SRVecMonojet.at(srN).crslHistMap,    SRVecMonojet.at(srN).GetNumberOfMT2Bins(), SRVecMonojet.at(srN).GetMT2Bins(), prefix+SRVecMonojet.at(srN).GetName(), suffix);
	else if(prefix=="crslmu")  fillHistosSingleLepton(SRVecMonojet.at(srN).crslmuHistMap,  SRVecMonojet.at(srN).GetNumberOfMT2Bins(), SRVecMonojet.at(srN).GetMT2Bins(), prefix+SRVecMonojet.at(srN).GetName(), suffix);
	else if(prefix=="crslel")  fillHistosSingleLepton(SRVecMonojet.at(srN).crslelHistMap,  SRVecMonojet.at(srN).GetNumberOfMT2Bins(), SRVecMonojet.at(srN).GetMT2Bins(), prefix+SRVecMonojet.at(srN).GetName(), suffix);
	//      break;//control regions are not necessarily orthogonal
      }
    }
  } // monojet regions
  
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
  valuesBase["passesHtMet"] = ( (t.gamma_ht > 200. && t.gamma_met_pt > 200.) || (t.gamma_ht > 1000. && t.gamma_met_pt > 30.) );
  bool passBase = SRBase.PassesSelection(valuesBase);

  std::map<std::string, float> valuesBase_monojet;
  valuesBase_monojet["deltaPhiMin"] = t.gamma_deltaPhiMin;
  valuesBase_monojet["diffMetMhtOverMet"]  = t.gamma_diffMetMht/t.gamma_met_pt;
  valuesBase_monojet["nlep"]        = nlepveto_;
  valuesBase_monojet["j1pt"]        = t.gamma_jet1_pt;
  valuesBase_monojet["njets"]       = t.gamma_nJet30;
  valuesBase_monojet["met"]         = t.gamma_met_pt;

  std::map<std::string, float> valuesBase_monojetB;
  valuesBase_monojetB["deltaPhiMin"] = t.gamma_deltaPhiMin;
  valuesBase_monojetB["diffMetMhtOverMet"]  = t.gamma_diffMetMht/t.gamma_met_pt;
  valuesBase_monojetB["nlep"]        = nlepveto_;
  valuesBase_monojetB["j1pt"]        = t.gamma_jet1_pt;
  valuesBase_monojetB["njets"]       = t.gamma_nJet30;
  valuesBase_monojetB["nbjets"]      = t.gamma_nBJet20;
  valuesBase_monojetB["met"]         = t.gamma_met_pt;

  bool passBaseJ = SRBaseMonojet.PassesSelection(valuesBase_monojet);
  bool passBaseJ0B = SRBaseMonojet0B.PassesSelection(valuesBase_monojetB);
  bool passBaseJ1B = SRBaseMonojet1B.PassesSelection(valuesBase_monojetB);

  std::map<std::string, float> values_monojet;
  values_monojet["deltaPhiMin"] = t.gamma_deltaPhiMin;
  values_monojet["diffMetMhtOverMet"]  = t.gamma_diffMetMht/t.gamma_met_pt;
  values_monojet["nlep"]        = nlepveto_;
  values_monojet["j1pt"]        = t.gamma_jet1_pt;
  values_monojet["njets"]       = t.gamma_nJet30;
  values_monojet["nbjets"]      = t.gamma_nBJet20;
  values_monojet["ht"]          = t.gamma_ht;
  values_monojet["met"]         = t.gamma_met_pt;
  

  //float iso = t.gamma_chHadIso[0] + t.gamma_phIso[0];
  float iso = t.gamma_chHadIso[0];
  float isoCutTight = 2.5;
  float isoCutLoose = 20.;
  if (t.gamma_ht > 200) fillHistosGammaJets(SRNoCut.crgjHistMap, SRNoCut.crgjRooDataSetMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), prefix+SRNoCut.GetName(), suffix+add+"All");

  if (iso>isoCutTight && iso < isoCutLoose) add += "LooseNotTight";
  if (iso>isoCutLoose) add += "NotLoose";
  if (!passSieie) add += "SieieSB"; // Keep Sigma IEta IEta sideband
  if (t.gamma_ht > 200) fillHistosGammaJets(SRNoCut.crgjHistMap, SRNoCut.crgjRooDataSetMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), prefix+SRNoCut.GetName(), suffix+add);
  
  if ( passBaseJ && t.gamma_pt[0] > 180. ) {
    fillHistosGammaJets(SRBaseMonojet.crgjHistMap, SRBaseMonojet.crgjRooDataSetMap, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.GetMT2Bins(), "crgjbaseJ", suffix+add);
    if (passBaseJ0B) fillHistosGammaJets(SRBaseMonojet0B.crgjHistMap, SRBaseMonojet0B.crgjRooDataSetMap, SRBaseMonojet0B.GetNumberOfMT2Bins(), SRBaseMonojet0B.GetMT2Bins(), "crgjbaseJ0B", suffix+add);
    if (passBaseJ1B) fillHistosGammaJets(SRBaseMonojet1B.crgjHistMap, SRBaseMonojet1B.crgjRooDataSetMap, SRBaseMonojet1B.GetNumberOfMT2Bins(), SRBaseMonojet1B.GetMT2Bins(), "crgjbaseJ1B", suffix+add);
    for(unsigned int srN = 0; srN < SRVecMonojet.size(); srN++){
      if(SRVecMonojet.at(srN).PassesSelection(values_monojet)){
	fillHistosGammaJets(SRVecMonojet.at(srN).crgjHistMap, SRVecMonojet.at(srN).crgjRooDataSetMap,   SRVecMonojet.at(srN).GetNumberOfMT2Bins(), SRVecMonojet.at(srN).GetMT2Bins(), prefix+SRVecMonojet.at(srN).GetName(), suffix+add);
      }
    }
  }

  if(passBase && passPtMT2) {
    //if (t.nJet30FailId > 0) cout<<"Event "<<t.evt<<" in run "<<t.run<<" fails jet ID"<<endl;
    //if (iso<isoCutTight && passSieie) {
    //  cout<<" "<<endl;
    //  cout<<"Event "<<t.evt<<" in run "<<t.run<<" passes baseline selection"<<endl;
    //}
    fillHistosGammaJets(SRBase.crgjHistMap, SRBase.crgjRooDataSetMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crgjbase", suffix+add);
    if(passBase && t.gamma_ht > 200.  && t.gamma_ht < 450.)  fillHistosGammaJets(InclusiveRegions.at(0).crgjHistMap,   InclusiveRegions.at(0).crgjRooDataSetMap,   SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crgjbaseVL", suffix+add);
    if(passBase && t.gamma_ht > 450.  && t.gamma_ht < 575.)  fillHistosGammaJets(InclusiveRegions.at(1).crgjHistMap,   InclusiveRegions.at(1).crgjRooDataSetMap,   SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crgjbaseL", suffix+add);
    if(passBase && t.gamma_ht > 575.  && t.gamma_ht < 1000.) fillHistosGammaJets(InclusiveRegions.at(2).crgjHistMap,  InclusiveRegions.at(2).crgjRooDataSetMap,  SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crgjbaseM", suffix+add);
    if(passBase && t.gamma_ht > 1000. && t.gamma_ht < 1500.) fillHistosGammaJets(InclusiveRegions.at(3).crgjHistMap, InclusiveRegions.at(3).crgjRooDataSetMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crgjbaseH", suffix+add);
    if(passBase && t.gamma_ht > 1500.                  ) fillHistosGammaJets(InclusiveRegions.at(4).crgjHistMap,  InclusiveRegions.at(4).crgjRooDataSetMap,  SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crgjbaseUH", suffix+add);
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
    if ( passBaseJ && t.gamma_pt[0] > 180.) {
      fillHistosGammaJets(SRBaseMonojet.crgjHistMap, SRBaseMonojet.crgjRooDataSetMap, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.GetMT2Bins(), "crgjbaseJ", suffix+add);
      if (passBaseJ0B) fillHistosGammaJets(SRBaseMonojet0B.crgjHistMap, SRBaseMonojet0B.crgjRooDataSetMap, SRBaseMonojet0B.GetNumberOfMT2Bins(), SRBaseMonojet0B.GetMT2Bins(), "crgjbaseJ0B", suffix+add);
      if (passBaseJ1B) fillHistosGammaJets(SRBaseMonojet1B.crgjHistMap, SRBaseMonojet1B.crgjRooDataSetMap, SRBaseMonojet1B.GetNumberOfMT2Bins(), SRBaseMonojet1B.GetMT2Bins(), "crgjbaseJ1B", suffix+add);
      for(unsigned int srN = 0; srN < SRVecMonojet.size(); srN++){
	if(SRVecMonojet.at(srN).PassesSelection(values_monojet)){
	  fillHistosGammaJets(SRVecMonojet.at(srN).crgjHistMap, SRVecMonojet.at(srN).crgjRooDataSetMap,   SRVecMonojet.at(srN).GetNumberOfMT2Bins(), SRVecMonojet.at(srN).GetMT2Bins(), prefix+SRVecMonojet.at(srN).GetName(), suffix+add);
	}
      }
    }
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
    if ( passBaseJ && t.gamma_pt[0] > 180.) {
      fillHistosGammaJets(SRBaseMonojet.crgjHistMap, SRBaseMonojet.crgjRooDataSetMap, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.GetMT2Bins(), "crgjbaseJ", suffix+add);
      if (passBaseJ0B) fillHistosGammaJets(SRBaseMonojet0B.crgjHistMap, SRBaseMonojet0B.crgjRooDataSetMap, SRBaseMonojet0B.GetNumberOfMT2Bins(), SRBaseMonojet0B.GetMT2Bins(), "crgjbaseJ0B", suffix+add);
      if (passBaseJ1B) fillHistosGammaJets(SRBaseMonojet1B.crgjHistMap, SRBaseMonojet1B.crgjRooDataSetMap, SRBaseMonojet1B.GetNumberOfMT2Bins(), SRBaseMonojet1B.GetMT2Bins(), "crgjbaseJ1B", suffix+add);
      for(unsigned int srN = 0; srN < SRVecMonojet.size(); srN++){
	if(SRVecMonojet.at(srN).PassesSelection(values_monojet)){
	  fillHistosGammaJets(SRVecMonojet.at(srN).crgjHistMap, SRVecMonojet.at(srN).crgjRooDataSetMap,   SRVecMonojet.at(srN).GetNumberOfMT2Bins(), SRVecMonojet.at(srN).GetMT2Bins(), prefix+SRVecMonojet.at(srN).GetName(), suffix+add);
	}
      }
    }
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
  valuesBase["passesHtMet"] = ( (t.zll_ht > 200. && t.zll_met_pt > 200.) || (t.zll_ht > 1000. && t.zll_met_pt > 30.) );
  bool passBase = SRBase.PassesSelection(valuesBase);

  std::map<std::string, float> valuesBase_monojet;
  valuesBase_monojet["deltaPhiMin"] = t.zll_deltaPhiMin;
  valuesBase_monojet["diffMetMhtOverMet"]  = t.zll_diffMetMht/t.zll_met_pt;
  valuesBase_monojet["nlep"]        = 0;
  valuesBase_monojet["j1pt"]        = t.jet1_pt;
  valuesBase_monojet["njets"]       = t.nJet30;
  valuesBase_monojet["met"]         = t.zll_met_pt;
  std::map<std::string, float> valuesBase_monojetB;
  valuesBase_monojetB["deltaPhiMin"] = t.zll_deltaPhiMin;
  valuesBase_monojetB["diffMetMhtOverMet"]  = t.zll_diffMetMht/t.zll_met_pt;
  valuesBase_monojetB["nlep"]        = 0;
  valuesBase_monojetB["j1pt"]        = t.jet1_pt;
  valuesBase_monojetB["njets"]       = t.nJet30;
  valuesBase_monojetB["nbjets"]      = t.nBJet20;
  valuesBase_monojetB["met"]         = t.zll_met_pt;
  bool passBaseJ = SRBaseMonojet.PassesSelection(valuesBase_monojet);
  bool passBaseJ0B = SRBaseMonojet0B.PassesSelection(valuesBase_monojetB);
  bool passBaseJ1B = SRBaseMonojet1B.PassesSelection(valuesBase_monojetB);
  
  if (t.zll_ht > 200) fillHistosDY(SRNoCut.crdyHistMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), prefix+SRNoCut.GetName(), suffix);
  if(passBase) fillHistosDY(SRBase.crdyHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crdybase", suffix);
  if(passBaseJ) fillHistosDY(SRBaseMonojet.crdyHistMap, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.GetMT2Bins(), "crdybaseJ", suffix);
  if(passBaseJ0B) fillHistosDY(SRBaseMonojet0B.crdyHistMap, SRBaseMonojet0B.GetNumberOfMT2Bins(), SRBaseMonojet0B.GetMT2Bins(), "crdybaseJ0B", suffix);
  if(passBaseJ1B) fillHistosDY(SRBaseMonojet1B.crdyHistMap, SRBaseMonojet1B.GetNumberOfMT2Bins(), SRBaseMonojet1B.GetMT2Bins(), "crdybaseJ1B", suffix);


  if(passBase && t.zll_ht > 200.  && t.zll_ht < 450.)  fillHistosDY(InclusiveRegions.at(0).crdyHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crdybaseVL", suffix);
  if(passBase && t.zll_ht > 450.  && t.zll_ht < 575.)  fillHistosDY(InclusiveRegions.at(1).crdyHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crdybaseL", suffix);
  if(passBase && t.zll_ht > 575.  && t.zll_ht < 1000.) fillHistosDY(InclusiveRegions.at(2).crdyHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crdybaseM", suffix);
  if(passBase && t.zll_ht > 1000. && t.zll_ht < 1500.) fillHistosDY(InclusiveRegions.at(3).crdyHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crdybaseH", suffix);
  if(passBase && t.zll_ht > 1500.                  ) fillHistosDY(InclusiveRegions.at(4).crdyHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crdybaseUH", suffix);

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
  valuesBase["passesHtMet"] = ( (t.rl_ht > 200. && t.rl_met_pt > 200.) || (t.rl_ht > 1000. && t.rl_met_pt > 30.) );
  bool passBase = SRBase.PassesSelection(valuesBase);

  std::map<std::string, float> valuesBase_monojet;
  valuesBase_monojet["deltaPhiMin"] = t.rl_deltaPhiMin;
  valuesBase_monojet["diffMetMhtOverMet"]  = t.rl_diffMetMht/t.rl_met_pt;
  valuesBase_monojet["nlep"]        = 0;
  valuesBase_monojet["j1pt"]        = t.jet1_pt;
  valuesBase_monojet["njets"]       = t.nJet30;
  valuesBase_monojet["met"]         = t.rl_met_pt;
  std::map<std::string, float> valuesBase_monojetB;
  valuesBase_monojetB["deltaPhiMin"] = t.rl_deltaPhiMin;
  valuesBase_monojetB["diffMetMhtOverMet"]  = t.rl_diffMetMht/t.rl_met_pt;
  valuesBase_monojetB["nlep"]        = 0;
  valuesBase_monojetB["j1pt"]        = t.jet1_pt;
  valuesBase_monojetB["njets"]       = t.nJet30;
  valuesBase_monojetB["nbjets"]      = t.nBJet20;
  valuesBase_monojetB["met"]         = t.rl_met_pt;
  bool passBaseJ = SRBaseMonojet.PassesSelection(valuesBase_monojet);
  bool passBaseJ0B = SRBaseMonojet0B.PassesSelection(valuesBase_monojetB);
  bool passBaseJ1B = SRBaseMonojet1B.PassesSelection(valuesBase_monojetB);
  
  if (t.rl_ht > 200) {
    if(prefix=="crrl")        fillHistosRemovedLepton(SRNoCut.crrlHistMap,   SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), prefix+SRNoCut.GetName(), suffix);
    else if(prefix=="crrlmu") fillHistosRemovedLepton(SRNoCut.crrlmuHistMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), prefix+SRNoCut.GetName(), suffix);
    else if(prefix=="crrlel") fillHistosRemovedLepton(SRNoCut.crrlelHistMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), prefix+SRNoCut.GetName(), suffix);
  }

  if(passBase) {
    if(prefix=="crrl")        fillHistosRemovedLepton(SRBase.crrlHistMap,   SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlbase",   suffix);
    else if(prefix=="crrlmu") fillHistosRemovedLepton(SRBase.crrlmuHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlmubase", suffix);
    else if(prefix=="crrlel") fillHistosRemovedLepton(SRBase.crrlelHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlelbase", suffix);
  }
  if(passBaseJ) {
    if(prefix=="crrl")        fillHistosRemovedLepton(SRBaseMonojet.crrlHistMap,   SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.GetMT2Bins(), "crrlbaseJ",   suffix);
    else if(prefix=="crrlmu") fillHistosRemovedLepton(SRBaseMonojet.crrlmuHistMap, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.GetMT2Bins(), "crrlmubaseJ", suffix);
    else if(prefix=="crrlel") fillHistosRemovedLepton(SRBaseMonojet.crrlelHistMap, SRBaseMonojet.GetNumberOfMT2Bins(), SRBaseMonojet.GetMT2Bins(), "crrlelbaseJ", suffix);
  }
  if(passBaseJ0B) {
    if(prefix=="crrl")        fillHistosRemovedLepton(SRBaseMonojet0B.crrlHistMap,   SRBaseMonojet0B.GetNumberOfMT2Bins(), SRBaseMonojet0B.GetMT2Bins(), "crrlbaseJ0B",   suffix);
  }
  if(passBaseJ1B) {
    if(prefix=="crrl")        fillHistosRemovedLepton(SRBaseMonojet1B.crrlHistMap,   SRBaseMonojet1B.GetNumberOfMT2Bins(), SRBaseMonojet1B.GetMT2Bins(), "crrlbaseJ1B",   suffix);
  }

  if(passBase && t.rl_ht > 200.  && t.rl_ht < 450.)  fillHistosRemovedLepton(InclusiveRegions.at(0).crrlHistMap,   SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlbaseVL", suffix);
  if(passBase && t.rl_ht > 450.  && t.rl_ht < 575.)  fillHistosRemovedLepton(InclusiveRegions.at(1).crrlHistMap,   SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlbaseL", suffix);
  if(passBase && t.rl_ht > 575.  && t.rl_ht < 1000.) fillHistosRemovedLepton(InclusiveRegions.at(2).crrlHistMap,  SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlbaseM", suffix);
  if(passBase && t.rl_ht > 1000. && t.rl_ht < 1500.) fillHistosRemovedLepton(InclusiveRegions.at(3).crrlHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlbaseH", suffix);
  if(passBase && t.rl_ht > 1500.                 ) fillHistosRemovedLepton(InclusiveRegions.at(4).crrlHistMap,  SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crrlbaseUH", suffix);

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

  // workaround for monojet bins
  float mt2_temp = t.mt2;
  if (t.nJet30 == 1) mt2_temp = t.ht;

  plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
  plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
  plot1D("h_mt2"+s,       mt2_temp,   evtweight_, h_1d, "; M_{T2} [GeV]", 150, 0, 1500);
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
  plot1D("h_mt2bins"+s,       mt2_temp,   evtweight_, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
  plot1D("h_htbins"+s,       t.ht,   evtweight_, h_1d, ";H_{T} [GeV]", n_htbins, htbins);
  plot1D("h_njbins"+s,       t.nJet30,   evtweight_, h_1d, ";N(jets)", n_njbins, njbins);
  plot1D("h_nbjbins"+s,       t.nBJet20,   evtweight_, h_1d, ";N(bjets)", n_nbjbins, nbjbins);

  TString directoryname(dirname);
  if (directoryname.Contains("nocut") && !t.isData) {
    
    float genHT = 0;
    int lep1idx = -1;
    int lep1id = -1;
    int lep2idx = -1;
    int Zidx = -1;
    int Gidx = -1;
    float minDR = 999;
    for ( int i = 0; i < t.ngenStat23; i++) {
      int ID = fabs(t.genStat23_pdgId[i]);
      if (ID==22) Gidx = i;
      if (ID==23 || ID==24) Zidx = i;
    }

    for ( int i = 0; i < t.ngenStat23; i++) {
      int ID = fabs(t.genStat23_pdgId[i]);
      if (ID<6 ||ID==21) {
	genHT += t.genStat23_pt[i];
	if (Gidx!=-1) {
	  float thisDR = DeltaR(t.genStat23_eta[Gidx],t.genStat23_eta[i],t.genStat23_phi[Gidx],t.genStat23_phi[i]);
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
    if (minDR < 0.4) Gidx = -1; // don't use low-DR photons
    if ((Gidx != -1 || Zidx != -1) && lep2idx==-1) {
      if (Zidx == -1) Zidx = Gidx;
      V.SetPtEtaPhiM(t.genStat23_pt[Zidx], t.genStat23_eta[Zidx], t.genStat23_phi[Zidx], t.genStat23_mass[Zidx]);
    }
    if (Zidx != -1 || Gidx != -1 || lep2idx!=-1) {
      float Vrapidity = V.Rapidity();
      plot1D("h_VpT"+s,       V.Pt(),   evtweight_, h_1d, "; p_{T}^{V} [GeV]", n_ptVbins, ptVbins);
      if (fabs(Vrapidity) < 1.4 ) plot1D("h_VpTcentral"+s,      V.Pt(),   evtweight_, h_1d, "; p_{T}^{V} [GeV]", n_ptVbins, ptVbins);
      if (genHT>200) plot1D("h_VpT_genHT200"+s,      V.Pt(),   evtweight_, h_1d, "; p_{T}^{V} [GeV]", n_ptVbins, ptVbins);
      if (fabs(Vrapidity) < 1.4 && genHT>200) plot1D("h_VpTcentral_genHT200"+s,       V.Pt(),   evtweight_, h_1d, "; p_{T}^{V} [GeV]", n_ptVbins, ptVbins);
      if (t.ht>200) plot1D("h_VpT_HT200"+s,      V.Pt(),   evtweight_, h_1d, "; p_{T}^{V} [GeV]", 300, 0, 1500);

    }

  } // nocut region only

  if (isSignal_) {
    plot3D("h_mt2bins_sigscan"+s, t.GenSusyMScan1, t.GenSusyMScan2, t.mt2, evtweight_, h_1d, "mass1 [GeV];mass2 [GeV];M_{T2} [GeV]", n_m1bins, m1bins, n_m2bins, m2bins, n_mt2bins, mt2bins);
  }

  if (!t.isData && applyWeights && doSystVariationPlots) {
    // assume weights are already applied to central value: lepsf, btagsf, isr 
    plot1D("h_mt2bins_lepsf_UP"+s,       mt2_temp,   evtweight_ / t.weight_lepsf * t.weight_lepsf_UP, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_lepsf_DN"+s,       mt2_temp,   evtweight_ / t.weight_lepsf * t.weight_lepsf_DN, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_btagsf_UP"+s,       mt2_temp,   evtweight_ / t.weight_btagsf * t.weight_btagsf_UP, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_btagsf_DN"+s,       mt2_temp,   evtweight_ / t.weight_btagsf * t.weight_btagsf_DN, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_isr_UP"+s,       mt2_temp,   evtweight_ / t.weight_isr, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_isr_DN"+s,       mt2_temp,   evtweight_ * t.weight_isr, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    // WARNING: will have to make sure we get the correct normalization for these
    //  to avoid including xsec effects..
    plot1D("h_mt2bins_scales_UP"+s,       mt2_temp,   evtweight_ * t.weight_scales_UP, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_scales_DN"+s,       mt2_temp,   evtweight_ * t.weight_scales_DN, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_pdfs_UP"+s,       mt2_temp,   evtweight_  * t.weight_pdfs_UP, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_pdfs_DN"+s,       mt2_temp,   evtweight_  * t.weight_pdfs_DN, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
  }

  if (!t.isData && doGenTauVars) {
    float unc_tau1p = 0.;
    float unc_tau3p = 0.;
    if (t.ngenTau1Prong > 0 || t.ngenTau3Prong > 0) {
      // loop on gen taus
      for (int itau = 0; itau < t.ngenTau; ++itau) {
	// check acceptance for veto: pt > 10
       if (t.genTau_leadTrackPt[itau] < 10.) continue;
       if (t.genTau_decayMode[itau] == 1) unc_tau1p += 0.14; // 14% relative uncertainty for missing a 1-prong tau
       else if (t.genTau_decayMode[itau] == 3) unc_tau3p += 0.06; // 6% relative uncertainty for missing a 3-prong tau
      }
    }
    
    plot1D("h_mt2bins_tau1p_UP"+s,       mt2_temp,   evtweight_ * (1. + unc_tau1p), h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_tau1p_DN"+s,       mt2_temp,   evtweight_ * (1. - unc_tau1p), h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_tau3p_UP"+s,       mt2_temp,   evtweight_ * (1. + unc_tau3p), h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    plot1D("h_mt2bins_tau3p_DN"+s,       mt2_temp,   evtweight_ * (1. - unc_tau3p), h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
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
  //if( (t.evt_id>110 && t.evt_id<120) || t.isData){ //only use qcd samples with pt>=470 to compute FR
  if(t.gamma_idCutBased[0]){ //passSieie
    plot2D("h2d_gammaht_gammapt"+s, t.gamma_ht, t.gamma_pt[0], evtweight_, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",n_FRhtbins,FRhtbins,n_FRptbins,FRptbins);
    plot2D("h2d_gammaht_gammaptW1"+s, t.gamma_ht, t.gamma_pt[0], 1, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",n_FRhtbins,FRhtbins,n_FRptbins,FRptbins);
  }
  if(!t.gamma_idCutBased[0]){ //!passSieie
    plot2D("h2d_gammaht_gammaptSingleBin"+s, t.gamma_ht, t.gamma_pt[0], evtweight_, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",1,0,3000,1,0,1500);
    plot2D("h2d_gammaht_gammaptSingleBinW1"+s, t.gamma_ht, t.gamma_pt[0], 1, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",1,0,3000,1,0,1500);
  }
  //  }
    float gamma_mt2_temp = t.gamma_mt2;
  // workaround for monojet bins
  if (t.gamma_nJet30 == 1) gamma_mt2_temp = t.gamma_ht;

  for (int i = 0; i < SRBase.GetNumberOfMT2Bins(); i++) {
    if ( gamma_mt2_temp > SRBase.GetMT2Bins()[i] &&  gamma_mt2_temp < SRBase.GetMT2Bins()[i+1]) {
      plotRooDataSet("rds_chIso_"+toString(SRBase.GetMT2Bins()[i])+s, x_, w_, evtweight_, datasets, "");
      plot1D("h_chiso_mt2bin"+toString(SRBase.GetMT2Bins()[i])+s,  iso,  evtweight_, h_1d, "; iso", 100, 0, 50);
      plot2D("h2d_gammaht_gammapt"+toString(SRBase.GetMT2Bins()[i])+s, t.gamma_ht, t.gamma_pt[0], evtweight_, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",n_FRhtbins,FRhtbins,n_FRptbins,FRptbins);
      //plot2D("h2d_gammaht_gammaptW1"+toString(SRBase.GetMT2Bins()[i])+s, t.gamma_ht, t.gamma_pt[0], evtweight_, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",n_FRhtbins,FRhtbins,n_FRptbins,FRptbins);	
      plot2D("h2d_gammaht_gammaptSingleBin"+toString(SRBase.GetMT2Bins()[i])+s, t.gamma_ht, t.gamma_pt[0], evtweight_, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",1,0,3000,1,0,1500);
      //plot2D("h2d_gammaht_gammaptSingleBinW1"+toString(SRBase.GetMT2Bins()[i])+s, t.gamma_ht, t.gamma_pt[0], evtweight_, h_1d, ";H_{T} [GeV];gamma p_{T} [GeV]",1,0,3000,1,0,1500);
    }
  }

  //cout<<"Event "<<t.evt<<" with weight "<< evtweight_ <<" is in sr "<<dirname<<endl;
  plot1D("h_mt2bins"+s,       gamma_mt2_temp,   evtweight_, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
  plot1D("h_htbins"+s,       t.gamma_ht,   evtweight_, h_1d, ";H_{T} [GeV]", n_htbins, htbins);
  plot1D("h_njbins"+s,       t.gamma_nJet30,   evtweight_, h_1d, ";N(jets)", n_njbins, njbins);
  plot1D("h_nbjbins"+s,       t.gamma_nBJet20,   evtweight_, h_1d, ";N(bjets)", n_nbjbins, nbjbins);

  if ( (dirname=="crgjnocut" || TString(dirname).Contains("crgjbase") || dirname=="crgjL" || dirname=="crgjM" || dirname=="crgjH") 
       && (s=="" || s=="Fake" || s=="FragGJ" || s=="AllIso" || s=="LooseNotTight") )// Don't make these for Loose, NotLoose. SieieSB
    {
    plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
    plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
    plot1D("h_mt2"+s,       gamma_mt2_temp,   evtweight_, h_1d, "; M_{T2} [GeV]", 150, 0, 1500);
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

    plot1D("h_drMinParton"+s,   t.gamma_drMinParton[0],   evtweight_, h_1d, ";DRmin(photon, parton)", 100, 0, 5);
    TString drName = "h_drMinPartonSample";
    drName += TString::Itoa(t.evt_id, 10);
    drName += s;
    plot1D(drName.Data(),   t.gamma_drMinParton[0],   evtweight_, h_1d, ";DRmin(photon, parton)", 100, 0, 5);
    if (t.gamma_ht > 200) 
      {
	plot1D("h_bosonptbins"+s,      t.gamma_pt[0],   evtweight_, h_1d, ";p_{T}^{V} [GeV]", n_ptVbins, ptVbins);
	plot1D("h_bosonpt"+s,      t.gamma_pt[0],   evtweight_, h_1d, ";p_{T}^{V} [GeV]",300, 0, 1500);
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

  // workaround for monojet bins
  float zll_mt2_temp = t.zll_mt2;
  if (t.nJet30 == 1) zll_mt2_temp = t.zll_ht;

  plot1D("h_mt2bins"+s,       zll_mt2_temp,   evtweight_, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
  
  if (dirname=="crdynocut" || TString(dirname).Contains("crdybase") || dirname=="crdyL" || dirname=="crdyM" || dirname=="crdyH") {
    plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
    plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
    plot1D("h_mt2"+s,       zll_mt2_temp,   evtweight_, h_1d, "; M_{T2} [GeV]", 150, 0, 1500);
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
    if (t.zll_ht > 200) 
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

  // workaround for monojet bins
  float rl_mt2_temp = t.rl_mt2;
  if (t.nJet30 == 1) rl_mt2_temp = t.rl_ht;

  plot1D("h_mt2bins"+s,       rl_mt2_temp,   evtweight_, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);

  if (dirname=="crrlnocut" || TString(dirname).Contains("crrlbase") || dirname=="crrlL" || dirname=="crrlM" || dirname=="crrlH" ||
      dirname=="crrlelnocut" || dirname=="crrlelbase" || dirname=="crrlelL" || dirname=="crrlelM" || dirname=="crrlelH" ||
      dirname=="crrlmunocut" || dirname=="crrlmubase" || dirname=="crrlmuL" || dirname=="crrlmuM" || dirname=="crrlmuH" ) { 

    float mt = MT(t.lep_pt[0],t.lep_phi[0],t.met_pt,t.met_phi);
    plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
    plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
    plot1D("h_mt2"+s,       rl_mt2_temp,   evtweight_, h_1d, "; M_{T2} [GeV]", 150, 0, 1500);
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
