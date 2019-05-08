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
#include "TRandom3.h"
#include "TList.h"
#include "TCollection.h"
#include "TKey.h"
#include "TLorentzVector.h"

// Tools
#include "../../CORE/Tools/utils.h"
#include "../../CORE/Tools/goodrun.h"
#include "../../CORE/Tools/dorky/dorky.h"
#include "../../CORE/Tools/hemJet.h"
#include "../../CORE/Tools/MT2/MT2.h"

// header
#include "SmearLooper.h"

//MT2
#include "../../MT2CORE/Plotting/PlotUtilities.h"
// #include "../../MT2CORE/Plotting/configurations.h"


using namespace std;
using namespace mt2;
using namespace duplicate_removal;

class mt2tree;
class SRRS;

std::string toString(float in){
    std::stringstream ss;
    ss << in;
    return ss.str();
}

// generic binning for signal scans - need arrays since mt2 dimension will be variable
//   assuming here: 50 GeV binning, m1 from 400-2000, m2 from 0-1600
const int n_m1bins = 33;
float m1bins[n_m1bins+1];
const int n_m2bins = 33;
float m2bins[n_m2bins+1];

const int n_htbins = 5;
const float htbins[n_htbins+1] = {250., 450., 575., 1200., 1500., 3000.};
const int n_njbins = 3;
const float njbins[n_njbins+1] = {2, 4, 7, 12};
const int n_nbjbins = 4;
const float nbjbins[n_nbjbins+1] = {0, 1, 2, 3, 6};

// turn on to apply weights to central value
// bool applyWeights_ = false;
// turn on to enable plots of MT2 with systematic variations applied. applyWeights_ should be true
bool doSystVariationPlots = false;
// turn on to apply json file to data
bool applyJSON = true;
// veto on jets with pt > 30, |eta| > 3.0
bool doHFJetVeto = false;
bool prescalesByEvent = true;
bool doNTrueIntReweight = true;
bool doCustomHEMsmear = false;
bool doHEMregionVeto = true;
float HEM_region[4] = {-4.7, -1.4, -1.6, -0.8}; // etalow, etahigh, philow, phihigh
float HEM_ptCut = 30.0;
int HEM_startRun = 319077; // affects 38.58 out of 58.83 fb-1 in 2018
uint HEM_fracNum = 1286, HEM_fracDen = 1961;
const int numberOfSmears = 100;
const float smearNormalization = 1.0/float(numberOfSmears);
const int MAX_SMEARS = 5000;
// factors to widen the core, magnify the tails, and shift the mean of the response templates
// float coreScale = 1.0;
// float tailScale = 1.0;
// float meanShift = 0.00;
const float EWK_CUTOFF = 100.0;   // cut on rebalanced MET to remove electroweak contamination in data
float prescale_correction = 1.0;  //correct the weight if prescale is too high and we don't smear enough times

std::vector<float> jet_pt;
std::vector<float> jet_eta;
std::vector<float> jet_phi;
std::vector<float> jet_btagDeepCSV;
std::vector<float> PU_passes_id_jet_pt;
std::vector<float> PU_passes_id_jet_eta;
std::vector<float> PU_passes_id_jet_phi;
std::vector<float> PU_passes_id_jet_btagDeepCSV;
std::vector<float> PU_fails_id_jet_pt;
std::vector<float> PU_fails_id_jet_eta;
std::vector<float> PU_fails_id_jet_phi;
float pt_soft_x;
float pt_soft_y;

inline bool sortByPt(const LorentzVector &vec1, const LorentzVector &vec2 ) {
    return vec1.pt() > vec2.pt();
}

SmearLooper::SmearLooper() :
    config_tag_(""),
    applyWeights_(false),
    doRebalanceAndSmear_(false),
    makeSmearBaby_(false),
    useRawHists_(false),
    useBjetResponse_(true),
    isData_(false),
    doNJetReweighting_(false),
    coreScale_(1.),
    tailScale_(1.),
    meanShift_(0.),
    JER_uncert_var_(0),
    CUT_LEVEL_(1) 
{
  
    // set up signal binning
    for (int i = 0; i <= n_m1bins; ++i) {
        m1bins[i] = 375. + i*50.;
    }
    for (int i = 0; i <= n_m2bins; ++i) {
        m2bins[i] = -25. + i*50.;
    }

}
SmearLooper::~SmearLooper(){

};

void SmearLooper::SetSignalRegions(){

    SRVec =  getSignalRegions2018_RS();
    SRVecMonojet =  getSignalRegionsMonojet_RS();

    //store histograms with cut values for all variables
    for(unsigned int i = 0; i < SRVec.size(); i++){
        cout << "SR: " << SRVec.at(i).GetName() << endl;
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

        dir = (TDirectory*)outfile_->Get(("crdy"+SRVec.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir(("crdy"+SRVec.at(i).GetName()).c_str());
        } 
        dir->cd();
        std::vector<std::string> varsCRDY = SRVec.at(i).GetListOfVariablesCRDY();
        for(unsigned int j = 0; j < varsCRDY.size(); j++){
            plot1D("h_"+varsCRDY.at(j)+"_"+"LOW",  1, SRVec.at(i).GetLowerBoundCRDY(varsCRDY.at(j)), SRVec.at(i).crdyHistMap, "", 1, 0, 2);
            plot1D("h_"+varsCRDY.at(j)+"_"+"HI",   1, SRVec.at(i).GetUpperBoundCRDY(varsCRDY.at(j)), SRVec.at(i).crdyHistMap, "", 1, 0, 2);
        }
        plot1D("h_n_mt2bins",  1, SRVec.at(i).GetNumberOfMT2Bins(), SRVec.at(i).crdyHistMap, "", 1, 0, 2);

        dir = (TDirectory*)outfile_->Get(("crRSInvertDPhi"+SRVec.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir(("crRSInvertDPhi"+SRVec.at(i).GetName()).c_str());
        } 
        dir->cd();
        std::vector<std::string> varsCRRSInvertDPhi = SRVec.at(i).GetListOfVariablesCRRSInvertDPhi();
        for(unsigned int j = 0; j < varsCRRSInvertDPhi.size(); j++){
            plot1D("h_"+varsCRRSInvertDPhi.at(j)+"_"+"LOW",  1, SRVec.at(i).GetLowerBoundCRRSInvertDPhi(varsCRRSInvertDPhi.at(j)), SRVec.at(i).crRSInvertDPhiHistMap, "", 1, 0, 2);
            plot1D("h_"+varsCRRSInvertDPhi.at(j)+"_"+"HI",   1, SRVec.at(i).GetUpperBoundCRRSInvertDPhi(varsCRRSInvertDPhi.at(j)), SRVec.at(i).crRSInvertDPhiHistMap, "", 1, 0, 2);
        }
        plot1D("h_n_mt2bins",  1, SRVec.at(i).GetNumberOfMT2Bins(), SRVec.at(i).crRSInvertDPhiHistMap, "", 1, 0, 2);

        dir = (TDirectory*)outfile_->Get(("crRSInvertDPhiSL"+SRVec.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir(("crRSInvertDPhiSL"+SRVec.at(i).GetName()).c_str());
        } 
        dir->cd();
        std::vector<std::string> varsCRRSInvertDPhiSL = SRVec.at(i).GetListOfVariablesCRRSInvertDPhi();
        for(unsigned int j = 0; j < varsCRRSInvertDPhiSL.size(); j++){
            plot1D("h_"+varsCRRSInvertDPhi.at(j)+"_"+"LOW",  1, SRVec.at(i).GetLowerBoundCRRSInvertDPhi(varsCRRSInvertDPhi.at(j)), SRVec.at(i).crRSInvertDPhiHistMap, "", 1, 0, 2);
            plot1D("h_"+varsCRRSInvertDPhi.at(j)+"_"+"HI",   1, SRVec.at(i).GetUpperBoundCRRSInvertDPhi(varsCRRSInvertDPhi.at(j)), SRVec.at(i).crRSInvertDPhiHistMap, "", 1, 0, 2);
        }
        plot1D("h_n_mt2bins",  1, SRVec.at(i).GetNumberOfMT2Bins(), SRVec.at(i).crRSInvertDPhiSLHistMap, "", 1, 0, 2);

        dir = (TDirectory*)outfile_->Get(("crRSInvertDPhiDY"+SRVec.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir(("crRSInvertDPhiDY"+SRVec.at(i).GetName()).c_str());
        } 
        dir->cd();
        std::vector<std::string> varsCRRSInvertDPhiDY = SRVec.at(i).GetListOfVariablesCRRSInvertDPhi();
        for(unsigned int j = 0; j < varsCRRSInvertDPhiDY.size(); j++){
            plot1D("h_"+varsCRRSInvertDPhi.at(j)+"_"+"LOW",  1, SRVec.at(i).GetLowerBoundCRRSInvertDPhi(varsCRRSInvertDPhi.at(j)), SRVec.at(i).crRSInvertDPhiHistMap, "", 1, 0, 2);
            plot1D("h_"+varsCRRSInvertDPhi.at(j)+"_"+"HI",   1, SRVec.at(i).GetUpperBoundCRRSInvertDPhi(varsCRRSInvertDPhi.at(j)), SRVec.at(i).crRSInvertDPhiHistMap, "", 1, 0, 2);
        }
        plot1D("h_n_mt2bins",  1, SRVec.at(i).GetNumberOfMT2Bins(), SRVec.at(i).crRSInvertDPhiDYHistMap, "", 1, 0, 2);

        dir = (TDirectory*)outfile_->Get(("crRSMT2SideBand"+SRVec.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir(("crRSMT2SideBand"+SRVec.at(i).GetName()).c_str());
        } 
        dir->cd();
        std::vector<std::string> varsCRRSMT2SideBand = SRVec.at(i).GetListOfVariablesCRRSMT2SideBand();
        for(unsigned int j = 0; j < varsCRRSMT2SideBand.size(); j++){
            plot1D("h_"+varsCRRSMT2SideBand.at(j)+"_"+"LOW",  1, SRVec.at(i).GetLowerBoundCRRSMT2SideBand(varsCRRSMT2SideBand.at(j)), SRVec.at(i).crRSMT2SideBandHistMap, "", 1, 0, 2);
            plot1D("h_"+varsCRRSMT2SideBand.at(j)+"_"+"HI",   1, SRVec.at(i).GetUpperBoundCRRSMT2SideBand(varsCRRSMT2SideBand.at(j)), SRVec.at(i).crRSMT2SideBandHistMap, "", 1, 0, 2);
        }
        plot1D("h_n_mt2bins",  1, SRVec.at(i).GetNumberOfMT2Bins(), SRVec.at(i).crRSMT2SideBandHistMap, "", 1, 0, 2);

        dir = (TDirectory*)outfile_->Get(("crRSMT2SideBandSL"+SRVec.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir(("crRSMT2SideBandSL"+SRVec.at(i).GetName()).c_str());
        } 
        dir->cd();
        std::vector<std::string> varsCRRSMT2SideBandSL = SRVec.at(i).GetListOfVariablesCRRSMT2SideBand();
        for(unsigned int j = 0; j < varsCRRSMT2SideBandSL.size(); j++){
            plot1D("h_"+varsCRRSMT2SideBand.at(j)+"_"+"LOW",  1, SRVec.at(i).GetLowerBoundCRRSMT2SideBand(varsCRRSMT2SideBand.at(j)), SRVec.at(i).crRSMT2SideBandHistMap, "", 1, 0, 2);
            plot1D("h_"+varsCRRSMT2SideBand.at(j)+"_"+"HI",   1, SRVec.at(i).GetUpperBoundCRRSMT2SideBand(varsCRRSMT2SideBand.at(j)), SRVec.at(i).crRSMT2SideBandHistMap, "", 1, 0, 2);
        }
        plot1D("h_n_mt2bins",  1, SRVec.at(i).GetNumberOfMT2Bins(), SRVec.at(i).crRSMT2SideBandSLHistMap, "", 1, 0, 2);

        dir = (TDirectory*)outfile_->Get(("crRSMT2SideBandDY"+SRVec.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir(("crRSMT2SideBandDY"+SRVec.at(i).GetName()).c_str());
        } 
        dir->cd();
        std::vector<std::string> varsCRRSMT2SideBandDY = SRVec.at(i).GetListOfVariablesCRRSMT2SideBand();
        for(unsigned int j = 0; j < varsCRRSMT2SideBandDY.size(); j++){
            plot1D("h_"+varsCRRSMT2SideBand.at(j)+"_"+"LOW",  1, SRVec.at(i).GetLowerBoundCRRSMT2SideBand(varsCRRSMT2SideBand.at(j)), SRVec.at(i).crRSMT2SideBandHistMap, "", 1, 0, 2);
            plot1D("h_"+varsCRRSMT2SideBand.at(j)+"_"+"HI",   1, SRVec.at(i).GetUpperBoundCRRSMT2SideBand(varsCRRSMT2SideBand.at(j)), SRVec.at(i).crRSMT2SideBandHistMap, "", 1, 0, 2);
        }
        plot1D("h_n_mt2bins",  1, SRVec.at(i).GetNumberOfMT2Bins(), SRVec.at(i).crRSMT2SideBandDYHistMap, "", 1, 0, 2);

        dir = (TDirectory*)outfile_->Get(("crRSDPhiMT2"+SRVec.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir(("crRSDPhiMT2"+SRVec.at(i).GetName()).c_str());
        } 
        dir->cd();
        std::vector<std::string> varsCRRSDPhiMT2 = SRVec.at(i).GetListOfVariablesCRRSDPhiMT2();
        for(unsigned int j = 0; j < varsCRRSDPhiMT2.size(); j++){
            plot1D("h_"+varsCRRSDPhiMT2.at(j)+"_"+"LOW",  1, SRVec.at(i).GetLowerBoundCRRSDPhiMT2(varsCRRSDPhiMT2.at(j)), SRVec.at(i).crRSDPhiMT2HistMap, "", 1, 0, 2);
            plot1D("h_"+varsCRRSDPhiMT2.at(j)+"_"+"HI",   1, SRVec.at(i).GetUpperBoundCRRSDPhiMT2(varsCRRSDPhiMT2.at(j)), SRVec.at(i).crRSDPhiMT2HistMap, "", 1, 0, 2);
        }
        plot1D("h_n_mt2bins",  1, SRVec.at(i).GetNumberOfMT2Bins(), SRVec.at(i).crRSDPhiMT2HistMap, "", 1, 0, 2);

        dir = (TDirectory*)outfile_->Get(("crRSDPhiMT2SL"+SRVec.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir(("crRSDPhiMT2SL"+SRVec.at(i).GetName()).c_str());
        }
        dir->cd();
        std::vector<std::string> varsCRRSDPhiMT2SL = SRVec.at(i).GetListOfVariablesCRRSDPhiMT2();
        for(unsigned int j = 0; j < varsCRRSDPhiMT2SL.size(); j++){
            plot1D("h_"+varsCRRSDPhiMT2.at(j)+"_"+"LOW",  1, SRVec.at(i).GetLowerBoundCRRSDPhiMT2(varsCRRSDPhiMT2.at(j)), SRVec.at(i).crRSDPhiMT2HistMap, "", 1, 0, 2);
            plot1D("h_"+varsCRRSDPhiMT2.at(j)+"_"+"HI",   1, SRVec.at(i).GetUpperBoundCRRSDPhiMT2(varsCRRSDPhiMT2.at(j)), SRVec.at(i).crRSDPhiMT2HistMap, "", 1, 0, 2);
        }
        plot1D("h_n_mt2bins",  1, SRVec.at(i).GetNumberOfMT2Bins(), SRVec.at(i).crRSDPhiMT2SLHistMap, "", 1, 0, 2);

        dir = (TDirectory*)outfile_->Get(("crRSDPhiMT2DY"+SRVec.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir(("crRSDPhiMT2DY"+SRVec.at(i).GetName()).c_str());
        } 
        dir->cd();
        std::vector<std::string> varsCRRSDPhiMT2DY = SRVec.at(i).GetListOfVariablesCRRSDPhiMT2();
        for(unsigned int j = 0; j < varsCRRSDPhiMT2DY.size(); j++){
            plot1D("h_"+varsCRRSDPhiMT2.at(j)+"_"+"LOW",  1, SRVec.at(i).GetLowerBoundCRRSDPhiMT2(varsCRRSDPhiMT2.at(j)), SRVec.at(i).crRSDPhiMT2HistMap, "", 1, 0, 2);
            plot1D("h_"+varsCRRSDPhiMT2.at(j)+"_"+"HI",   1, SRVec.at(i).GetUpperBoundCRRSDPhiMT2(varsCRRSDPhiMT2.at(j)), SRVec.at(i).crRSDPhiMT2HistMap, "", 1, 0, 2);
        }
        plot1D("h_n_mt2bins",  1, SRVec.at(i).GetNumberOfMT2Bins(), SRVec.at(i).crRSDPhiMT2DYHistMap, "", 1, 0, 2);
    }

    // MONOJET
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

        dir = (TDirectory*)outfile_->Get(("crRSInvertDPhi"+SRVecMonojet.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir(("crRSInvertDPhi"+SRVecMonojet.at(i).GetName()).c_str());
        } 
        dir->cd();
        std::vector<std::string> varsCRRSInvertDPhi = SRVecMonojet.at(i).GetListOfVariablesCRRSInvertDPhi();
        for(unsigned int j = 0; j < varsCRRSInvertDPhi.size(); j++){
            plot1D("h_"+varsCRRSInvertDPhi.at(j)+"_"+"LOW",  1, SRVecMonojet.at(i).GetLowerBoundCRRSInvertDPhi(varsCRRSInvertDPhi.at(j)), SRVecMonojet.at(i).crRSInvertDPhiHistMap, "", 1, 0, 2);
            plot1D("h_"+varsCRRSInvertDPhi.at(j)+"_"+"HI",   1, SRVecMonojet.at(i).GetUpperBoundCRRSInvertDPhi(varsCRRSInvertDPhi.at(j)), SRVecMonojet.at(i).crRSInvertDPhiHistMap, "", 1, 0, 2);
        }
        plot1D("h_n_mt2bins",  1, SRVecMonojet.at(i).GetNumberOfMT2Bins(), SRVecMonojet.at(i).crRSInvertDPhiHistMap, "", 1, 0, 2);
    }

    SRBase.SetName("srbase");
    SRBase.SetVar("mt2", 200, -1);//FIXME
    SRBase.SetVar("j1pt", 30, -1);
    SRBase.SetVar("j2pt", 30, -1);
    SRBase.SetVar("deltaPhiMin", 0.3, -1);
    SRBase.SetVar("diffMetMhtOverMet", 0, 0.5);
    SRBase.SetVar("nlep", 0, 1);
    SRBase.SetVar("passesHtMet", 1, 2);
    SRBase.SetVarCRRSInvertDPhi("mt2", 200, -1);
    SRBase.SetVarCRRSInvertDPhi("j1pt", 30, -1);
    SRBase.SetVarCRRSInvertDPhi("j2pt", 30, -1);
    SRBase.SetVarCRRSInvertDPhi("deltaPhiMin", 0.0, 0.3);
    SRBase.SetVarCRRSInvertDPhi("diffMetMhtOverMet", 0, 0.5);
    SRBase.SetVarCRRSInvertDPhi("nlep", 0, 1);
    SRBase.SetVarCRRSInvertDPhi("passesHtMet", 1, 2);
    SRBase.SetVarCRRSMT2SideBand("mt2", 100, 200);
    SRBase.SetVarCRRSMT2SideBand("j1pt", 30, -1);
    SRBase.SetVarCRRSMT2SideBand("j2pt", 30, -1);
    SRBase.SetVarCRRSMT2SideBand("deltaPhiMin", 0.3, -1);
    SRBase.SetVarCRRSMT2SideBand("diffMetMhtOverMet", 0, 0.5);
    SRBase.SetVarCRRSMT2SideBand("nlep", 0, 1);
    SRBase.SetVarCRRSMT2SideBand("passesHtMet", 1, 2);
    SRBase.SetVarCRRSDPhiMT2("mt2", 100, 200);
    SRBase.SetVarCRRSDPhiMT2("j1pt", 30, -1);
    SRBase.SetVarCRRSDPhiMT2("j2pt", 30, -1);
    SRBase.SetVarCRRSDPhiMT2("deltaPhiMin", 0.0, 0.3);
    SRBase.SetVarCRRSDPhiMT2("diffMetMhtOverMet", 0, 0.5);
    SRBase.SetVarCRRSDPhiMT2("nlep", 0, 1);
    SRBase.SetVarCRRSDPhiMT2("passesHtMet", 1, 2);
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

    std::vector<std::string> varsCRRSInvertDPhi = SRBase.GetListOfVariablesCRRSInvertDPhi();
    TDirectory * dirCRRSInvertDPhi = (TDirectory*)outfile_->Get("crRSInvertDPhibase");
    if (dirCRRSInvertDPhi == 0) {
        dirCRRSInvertDPhi = outfile_->mkdir("crRSInvertDPhibase");
    } 
    dirCRRSInvertDPhi->cd();
    for(unsigned int j = 0; j < varsCRRSInvertDPhi.size(); j++){
        plot1D("h_"+varsCRRSInvertDPhi.at(j)+"_"+"LOW",  1, SRBase.GetLowerBoundCRRSInvertDPhi(varsCRRSInvertDPhi.at(j)), SRBase.crRSInvertDPhiHistMap, "", 1, 0, 2);
        plot1D("h_"+varsCRRSInvertDPhi.at(j)+"_"+"HI",   1, SRBase.GetUpperBoundCRRSInvertDPhi(varsCRRSInvertDPhi.at(j)), SRBase.crRSInvertDPhiHistMap, "", 1, 0, 2);
    }
    plot1D("h_n_mt2bins",  1, SRBase.GetNumberOfMT2Bins(), SRBase.crRSInvertDPhiHistMap, "", 1, 0, 2);
    outfile_->cd();

    std::vector<std::string> varsCRRSMT2SideBand = SRBase.GetListOfVariablesCRRSMT2SideBand();
    TDirectory * dirCRRSMT2SideBand = (TDirectory*)outfile_->Get("crRSMT2SideBandbase");
    if (dirCRRSMT2SideBand == 0) {
        dirCRRSMT2SideBand = outfile_->mkdir("crRSMT2SideBandbase");
    } 
    dirCRRSMT2SideBand->cd();
    for(unsigned int j = 0; j < varsCRRSMT2SideBand.size(); j++){
        plot1D("h_"+varsCRRSMT2SideBand.at(j)+"_"+"LOW",  1, SRBase.GetLowerBoundCRRSMT2SideBand(varsCRRSMT2SideBand.at(j)), SRBase.crRSMT2SideBandHistMap, "", 1, 0, 2);
        plot1D("h_"+varsCRRSMT2SideBand.at(j)+"_"+"HI",   1, SRBase.GetUpperBoundCRRSMT2SideBand(varsCRRSMT2SideBand.at(j)), SRBase.crRSMT2SideBandHistMap, "", 1, 0, 2);
    }
    plot1D("h_n_mt2bins",  1, SRBase.GetNumberOfMT2Bins(), SRBase.crRSMT2SideBandHistMap, "", 1, 0, 2);
    outfile_->cd();

    std::vector<std::string> varsCRRSDPhiMT2 = SRBase.GetListOfVariablesCRRSDPhiMT2();
    TDirectory * dirCRRSDPhiMT2 = (TDirectory*)outfile_->Get("crRSDPhiMT2base");
    if (dirCRRSDPhiMT2 == 0) {
        dirCRRSDPhiMT2 = outfile_->mkdir("crRSDPhiMT2base");
    } 
    dirCRRSDPhiMT2->cd();
    for(unsigned int j = 0; j < varsCRRSDPhiMT2.size(); j++){
        plot1D("h_"+varsCRRSDPhiMT2.at(j)+"_"+"LOW",  1, SRBase.GetLowerBoundCRRSDPhiMT2(varsCRRSDPhiMT2.at(j)), SRBase.crRSDPhiMT2HistMap, "", 1, 0, 2);
        plot1D("h_"+varsCRRSDPhiMT2.at(j)+"_"+"HI",   1, SRBase.GetUpperBoundCRRSDPhiMT2(varsCRRSDPhiMT2.at(j)), SRBase.crRSDPhiMT2HistMap, "", 1, 0, 2);
    }
    plot1D("h_n_mt2bins",  1, SRBase.GetNumberOfMT2Bins(), SRBase.crRSDPhiMT2HistMap, "", 1, 0, 2);
    outfile_->cd();

    //setup inclusive regions
    SRRS InclusiveHT250to1200 = SRBase;
    InclusiveHT250to1200.SetName("InclusiveHT250to1200");
    InclusiveHT250to1200.SetVar("ht", 250, 1200);
    InclusiveHT250to1200.SetVarCRRSInvertDPhi("ht", 250, 1200);
    InclusiveHT250to1200.SetVarCRRSMT2SideBand("ht", 250, 1200);
    InclusiveHT250to1200.SetVarCRRSDPhiMT2("ht", 250, 1200);
    InclusiveRegions.push_back(InclusiveHT250to1200);

    SRRS InclusiveHT450to1200 = SRBase;
    InclusiveHT450to1200.SetName("InclusiveHT450to1200");
    InclusiveHT450to1200.SetVar("ht", 450, 1200);
    InclusiveHT450to1200.SetVarCRRSInvertDPhi("ht", 450, 1200);
    InclusiveHT450to1200.SetVarCRRSMT2SideBand("ht", 450, 1200);
    InclusiveHT450to1200.SetVarCRRSDPhiMT2("ht", 450, 1200);
    InclusiveRegions.push_back(InclusiveHT450to1200);

    SRRS InclusiveHT1200toInf = SRBase;
    InclusiveHT1200toInf.SetName("InclusiveHT1200toInf");
    InclusiveHT1200toInf.SetVar("ht", 1200, -1);
    InclusiveHT1200toInf.SetVarCRRSInvertDPhi("ht", 1200, -1);
    InclusiveHT1200toInf.SetVarCRRSMT2SideBand("ht", 1200, -1);
    InclusiveHT1200toInf.SetVarCRRSDPhiMT2("ht", 1200, -1);
    InclusiveRegions.push_back(InclusiveHT1200toInf);

    SRRS InclusiveNJets2to3 = SRBase;
    InclusiveNJets2to3.SetName("InclusiveNJets2to3");
    InclusiveNJets2to3.SetVar("njets", 2, 4);
    InclusiveNJets2to3.SetVarCRRSInvertDPhi("njets", 2, 4);
    InclusiveNJets2to3.SetVarCRRSMT2SideBand("njets", 2, 4);
    InclusiveNJets2to3.SetVarCRRSDPhiMT2("njets", 2, 4);
    InclusiveRegions.push_back(InclusiveNJets2to3);

    SRRS InclusiveNJets4to6 = SRBase;
    InclusiveNJets4to6.SetName("InclusiveNJets4to6");
    InclusiveNJets4to6.SetVar("njets", 4, 7);
    InclusiveNJets4to6.SetVarCRRSInvertDPhi("njets", 4, 7);
    InclusiveNJets4to6.SetVarCRRSMT2SideBand("njets", 4, 7);
    InclusiveNJets4to6.SetVarCRRSDPhiMT2("njets", 4, 7);
    InclusiveRegions.push_back(InclusiveNJets4to6);

    SRRS InclusiveNJets7toInf = SRBase;
    InclusiveNJets7toInf.SetName("InclusiveNJets7toInf");
    InclusiveNJets7toInf.SetVar("njets", 7, -1);
    InclusiveNJets7toInf.SetVarCRRSInvertDPhi("njets", 7, -1);
    InclusiveNJets7toInf.SetVarCRRSMT2SideBand("njets", 7, -1);
    InclusiveNJets7toInf.SetVarCRRSDPhiMT2("njets", 7, -1);
    InclusiveRegions.push_back(InclusiveNJets7toInf);

    SRRS InclusiveNBJets0 = SRBase;
    InclusiveNBJets0.SetName("InclusiveNBJets0");
    InclusiveNBJets0.SetVar("nbjets", 0, 1);
    InclusiveNBJets0.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    InclusiveNBJets0.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    InclusiveNBJets0.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    InclusiveRegions.push_back(InclusiveNBJets0);

    SRRS InclusiveNBJets1 = SRBase;
    InclusiveNBJets1.SetName("InclusiveNBJets1");
    InclusiveNBJets1.SetVar("nbjets", 1, 2);
    InclusiveNBJets1.SetVarCRRSInvertDPhi("nbjets", 1, 2);
    InclusiveNBJets1.SetVarCRRSMT2SideBand("nbjets", 1, 2);
    InclusiveNBJets1.SetVarCRRSDPhiMT2("nbjets", 1, 2);
    InclusiveRegions.push_back(InclusiveNBJets1);

    SRRS InclusiveNBJets2 = SRBase;
    InclusiveNBJets2.SetName("InclusiveNBJets2");
    InclusiveNBJets2.SetVar("nbjets", 2, 3);
    InclusiveNBJets2.SetVarCRRSInvertDPhi("nbjets", 2, 3);
    InclusiveNBJets2.SetVarCRRSMT2SideBand("nbjets", 2, 3);
    InclusiveNBJets2.SetVarCRRSDPhiMT2("nbjets", 2, 3);
    InclusiveRegions.push_back(InclusiveNBJets2);

    SRRS InclusiveNBJets3toInf = SRBase;
    InclusiveNBJets3toInf.SetName("InclusiveNBJets3toInf");
    InclusiveNBJets3toInf.SetVar("nbjets", 3, -1);
    InclusiveNBJets3toInf.SetVarCRRSInvertDPhi("nbjets", 3, -1);
    InclusiveNBJets3toInf.SetVarCRRSMT2SideBand("nbjets", 3, -1);
    InclusiveNBJets3toInf.SetVarCRRSDPhiMT2("nbjets", 3, -1);
    InclusiveRegions.push_back(InclusiveNBJets3toInf);

    for(unsigned int i=0; i<InclusiveRegions.size(); i++){
        InclusiveRegions.at(i).srHistMap.clear();
        InclusiveRegions.at(i).crRSInvertDPhiHistMap.clear();
        InclusiveRegions.at(i).crRSMT2SideBandHistMap.clear();
        InclusiveRegions.at(i).crRSDPhiMT2HistMap.clear();
        TDirectory * dir = (TDirectory*)outfile_->Get((InclusiveRegions.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir((InclusiveRegions.at(i).GetName()).c_str());
        } 
    }

    SRVec_temp = SRVec;
    for(unsigned int srN = 0; srN < SRVec_temp.size(); srN++){
        SRVec_temp.at(srN).SetName("temp_"+SRVec_temp.at(srN).GetName());
        SRVec_temp.at(srN).srHistMap.clear();
        SRVec_temp.at(srN).crRSInvertDPhiHistMap.clear();
        SRVec_temp.at(srN).crRSMT2SideBandHistMap.clear();
        SRVec_temp.at(srN).crRSDPhiMT2HistMap.clear();
    }
    for(unsigned int i=0; i<SRVec_temp.size(); i++){
        TDirectory * dir = (TDirectory*)outfile_->Get((SRVec_temp.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir((SRVec_temp.at(i).GetName()).c_str());
        } 
    }

    SRVecMonojet_temp = SRVecMonojet;
    for(unsigned int srN = 0; srN < SRVecMonojet_temp.size(); srN++){
        SRVecMonojet_temp.at(srN).SetName("temp_"+SRVecMonojet_temp.at(srN).GetName());
        SRVecMonojet_temp.at(srN).srHistMap.clear();
        SRVecMonojet_temp.at(srN).crRSInvertDPhiHistMap.clear();
        SRVecMonojet_temp.at(srN).crRSMT2SideBandHistMap.clear();
        SRVecMonojet_temp.at(srN).crRSDPhiMT2HistMap.clear();
    }
    for(unsigned int i=0; i<SRVecMonojet_temp.size(); i++){
        TDirectory * dir = (TDirectory*)outfile_->Get((SRVecMonojet_temp.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir((SRVecMonojet_temp.at(i).GetName()).c_str());
        } 
    }

    InclusiveRegions_temp = InclusiveRegions;
    for(unsigned int srN = 0; srN < InclusiveRegions_temp.size(); srN++){
        InclusiveRegions_temp.at(srN).SetName("temp_"+InclusiveRegions_temp.at(srN).GetName());
        InclusiveRegions_temp.at(srN).srHistMap.clear();
        InclusiveRegions_temp.at(srN).crRSInvertDPhiHistMap.clear();
        InclusiveRegions_temp.at(srN).crRSMT2SideBandHistMap.clear();
        InclusiveRegions_temp.at(srN).crRSDPhiMT2HistMap.clear();
    }
    for(unsigned int i=0; i<InclusiveRegions_temp.size(); i++){
        TDirectory * dir = (TDirectory*)outfile_->Get((InclusiveRegions_temp.at(i).GetName()).c_str());
        if (dir == 0) {
            dir = outfile_->mkdir((InclusiveRegions_temp.at(i).GetName()).c_str());
        } 
    }

    SRBase_temp = SRBase;
    SRBase_temp.SetName("temp_"+SRBase_temp.GetName());
    SRBase_temp.srHistMap.clear();
    SRBase_temp.crRSInvertDPhiHistMap.clear();
    SRBase_temp.crRSMT2SideBandHistMap.clear();
    SRBase_temp.crRSDPhiMT2HistMap.clear();
    TDirectory * dir_SRBase_temp = (TDirectory*)outfile_->Get((SRBase_temp.GetName()).c_str());
    if (dir_SRBase_temp == 0) {
        dir_SRBase_temp = outfile_->mkdir((SRBase_temp.GetName()).c_str());
    } 

}


void SmearLooper::loop(TChain* chain, std::string output_name, int maxEvents){

    TRandom3 *random = new TRandom3();

    if(config_tag_ == ""){
        cout << "ERROR! need to set the MT2Configuration. use SmearLooper::SetMT2ConfigTag()" << endl;
        return;
    }

    // Load the configuration and output to screen
    config_ = GetMT2Config(config_tag_);
    cout << "[SmearLooper::loop] using configuration tag: " << config_tag_ << endl;
    cout << "                  JSON: " << config_.json << endl;
    cout << "                  lumi: " << config_.lumi << " fb-1" << endl;
    if (config_.pu_weights_file != ""){
        cout << "                  PU weights file: " << config_.pu_weights_file << endl;
    }
    cout << "                  Filters applied:" << endl;
    for(map<string,bool>::iterator it=config_.filters.begin(); it!=config_.filters.end(); it++){
        if(it->second)
            cout << "                      " << it->first << endl;
    }    

    // setup template reader
    reader.SetCoreScale(coreScale_);
    reader.SetTailScale(tailScale_);
    reader.SetMeanShift(meanShift_);
    reader.UseRawHistograms(useRawHists_);
    if(isData_){
        cout << "[SmearLooper::loop] Applying JER correction factors" << endl;
        if(JER_uncert_var_==1)
            cout << "[SmearLooper::loop] Varying JER uncertainty UP" << endl;
        if(JER_uncert_var_==-1)
            cout << "[SmearLooper::loop] Varying JER uncertainty DOWN" << endl;
    }
    reader.Init("JetResponseTemplates.root", isData_, JER_uncert_var_, config_tag_);
    // reader.Init("/home/users/bemarsh/analysis/jet_response_templates/CMSSW_9_4_7/src/JetResponseTemplates/JetResponseTemplates/output/JetResponseTemplates_ptBinned_94x_JetID_PUID_BTagSFs_core2sigma.root",isData_);

    // draw all of the templates for debugging. Uncomment the drawing code in JRTreader.cc:GetRandomResponse
    // for(int ipt=0; ipt<23; ipt++){
    //     for(int ieta=0; ieta<17; ieta++){
    //         reader.GetRandomResponse(JRTreader::pt_bins[ipt]+0.01, JRTreader::eta_bins[ieta]+0.01, false);
    //     }
    // }
    // return;

    if(doNJetReweighting_){
        const uint n_ht_regs = 5;
        string ht_regs[n_ht_regs] = {"ht250to450", "ht450to575", "ht575to1200", "ht1200to1500", "ht1500to2000"};
        TFile *fid_njet_weights_ = new TFile(njet_weights_file_.c_str());
        for(uint iht=0; iht<n_ht_regs; iht++){
            nJetWeights_[ht_regs[iht]] = (TH1D*)fid_njet_weights_->Get((ht_regs[iht] + "/h_weights").c_str());
            nJetWeights_[ht_regs[iht]]->SetDirectory(0);
        }
        fid_njet_weights_->Close();
    }
    
    vector<TH1D*> HEMsmears;
    if(doCustomHEMsmear){
        TFile* fid_hem_smears = new TFile("HEM_smears.root");
        for(int i=0; i<8; i++){
            string hname = "reg0/h_reg_0_pT_"+to_string(i);
            HEMsmears.push_back((TH1D*)fid_hem_smears->Get(hname.c_str()));
            if(!HEMsmears.at(i)){
                cout << "INVALID HEM HIST NAME " << hname <<  endl;
                return;
            }
            HEMsmears.at(i)->SetDirectory(0);
        }
        fid_hem_smears->Close();
    }

    // load PU reweighting weights
    TH1D* h_nTrueInt_weights_ = 0;
    if (doNTrueIntReweight && config_.pu_weights_file != "") {
        cout << "Applying nTrueInt reweighting from weight file " << config_.pu_weights_file << endl;
        TFile* f_weights = new TFile(config_.pu_weights_file.c_str());
        TH1D* h_nTrueInt_weights_temp = (TH1D*) f_weights->Get("pileupWeight");
        h_nTrueInt_weights_ = (TH1D*) h_nTrueInt_weights_temp->Clone("h_pileupWeight");
        h_nTrueInt_weights_->SetDirectory(0);
        f_weights->Close();
        delete f_weights;
    }

    // Benchmark
    TBenchmark *bmark = new TBenchmark();
    bmark->Start("benchmark");

    gROOT->cd();
    cout << "[SmearLooper::loop] creating output file: " << output_name << endl;

    outfile_ = new TFile(output_name.c_str(),"RECREATE") ; 

    if (applyJSON && config_.json != "") {
        cout << "[SmearLooper::loop] Loading json file: " << config_.json << endl;
        set_goodrun_file(("jsons/"+config_.json).c_str());
    }

    cout << "[SmearLooper::loop] setting up histos" << endl;

    SetSignalRegions();

    SRNoCut.SetName("nocut");
    float SRNoCut_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500}; 
    SRNoCut.SetMT2Bins(7, SRNoCut_mt2bins);

    SRNoCut_temp = SRNoCut;
    SRNoCut_temp.SetName("temp_"+SRNoCut_temp.GetName());
    TDirectory * dir_SRNoCut_temp = (TDirectory*)outfile_->Get((SRNoCut_temp.GetName()).c_str());
    if (dir_SRNoCut_temp == 0) {
        dir_SRNoCut_temp = outfile_->mkdir((SRNoCut_temp.GetName()).c_str());
    } 

    SRJustHT1.SetName("ht450to1200");
    float SRJustHT1_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500}; 
    SRJustHT1.SetMT2Bins(7, SRJustHT1_mt2bins);

    SRJustHT1_temp = SRJustHT1;
    SRJustHT1_temp.SetName("temp_"+SRJustHT1_temp.GetName());
    TDirectory * dir_SRJustHT1_temp = (TDirectory*)outfile_->Get((SRJustHT1_temp.GetName()).c_str());
    if (dir_SRJustHT1_temp == 0) {
        dir_SRJustHT1_temp = outfile_->mkdir((SRJustHT1_temp.GetName()).c_str());
    } 

    SRJustHT2.SetName("ht1200toInf");
    float SRJustHT2_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500}; 
    SRJustHT2.SetMT2Bins(7, SRJustHT2_mt2bins);

    SRJustHT2_temp = SRJustHT2;
    SRJustHT2_temp.SetName("temp_"+SRJustHT2_temp.GetName());
    TDirectory * dir_SRJustHT2_temp = (TDirectory*)outfile_->Get((SRJustHT2_temp.GetName()).c_str());
    if (dir_SRJustHT2_temp == 0) {
        dir_SRJustHT2_temp = outfile_->mkdir((SRJustHT2_temp.GetName()).c_str());
    } 

    SRJustHT3.SetName("ht250to1200");
    float SRJustHT3_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500}; 
    SRJustHT3.SetMT2Bins(7, SRJustHT3_mt2bins);

    SRJustHT3_temp = SRJustHT3;
    SRJustHT3_temp.SetName("temp_"+SRJustHT3_temp.GetName());
    TDirectory * dir_SRJustHT3_temp = (TDirectory*)outfile_->Get((SRJustHT3_temp.GetName()).c_str());
    if (dir_SRJustHT3_temp == 0) {
        dir_SRJustHT3_temp = outfile_->mkdir((SRJustHT3_temp.GetName()).c_str());
    } 

    SRJustHT4.SetName("ht250to450");
    float SRJustHT4_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500}; 
    SRJustHT4.SetMT2Bins(7, SRJustHT4_mt2bins);

    SRJustHT4_temp = SRJustHT4;
    SRJustHT4_temp.SetName("temp_"+SRJustHT4_temp.GetName());
    TDirectory * dir_SRJustHT4_temp = (TDirectory*)outfile_->Get((SRJustHT4_temp.GetName()).c_str());
    if (dir_SRJustHT4_temp == 0) {
        dir_SRJustHT4_temp = outfile_->mkdir((SRJustHT4_temp.GetName()).c_str());
    } 

    SRJustHT5.SetName("ht450to575");
    float SRJustHT5_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500}; 
    SRJustHT5.SetMT2Bins(7, SRJustHT5_mt2bins);

    SRJustHT5_temp = SRJustHT5;
    SRJustHT5_temp.SetName("temp_"+SRJustHT5_temp.GetName());
    TDirectory * dir_SRJustHT5_temp = (TDirectory*)outfile_->Get((SRJustHT5_temp.GetName()).c_str());
    if (dir_SRJustHT5_temp == 0) {
        dir_SRJustHT5_temp = outfile_->mkdir((SRJustHT5_temp.GetName()).c_str());
    } 

    SRJustHT6.SetName("ht575to1200");
    float SRJustHT6_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500}; 
    SRJustHT6.SetMT2Bins(7, SRJustHT6_mt2bins);

    SRJustHT6_temp = SRJustHT6;
    SRJustHT6_temp.SetName("temp_"+SRJustHT6_temp.GetName());
    TDirectory * dir_SRJustHT6_temp = (TDirectory*)outfile_->Get((SRJustHT6_temp.GetName()).c_str());
    if (dir_SRJustHT6_temp == 0) {
        dir_SRJustHT6_temp = outfile_->mkdir((SRJustHT6_temp.GetName()).c_str());
    } 

    SRJustHT7.SetName("ht1200to1500");
    float SRJustHT7_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500}; 
    SRJustHT7.SetMT2Bins(7, SRJustHT7_mt2bins);

    SRJustHT7_temp = SRJustHT7;
    SRJustHT7_temp.SetName("temp_"+SRJustHT7_temp.GetName());
    TDirectory * dir_SRJustHT7_temp = (TDirectory*)outfile_->Get((SRJustHT7_temp.GetName()).c_str());
    if (dir_SRJustHT7_temp == 0) {
        dir_SRJustHT7_temp = outfile_->mkdir((SRJustHT7_temp.GetName()).c_str());
    } 

    SRJustHT8.SetName("ht1500to2000");
    float SRJustHT8_mt2bins[8] = {200, 300, 400, 500, 600, 800, 1000, 1500}; 
    SRJustHT8.SetMT2Bins(7, SRJustHT8_mt2bins);

    SRJustHT8_temp = SRJustHT8;
    SRJustHT8_temp.SetName("temp_"+SRJustHT8_temp.GetName());
    TDirectory * dir_SRJustHT8_temp = (TDirectory*)outfile_->Get((SRJustHT8_temp.GetName()).c_str());
    if (dir_SRJustHT8_temp == 0) {
        dir_SRJustHT8_temp = outfile_->mkdir((SRJustHT8_temp.GetName()).c_str());
    } 
  
    if (makeSmearBaby_) {
        std::cout << "[SmearLooper::loop] Making baby ntuple..." << std::endl;    
        std::string baby_name = output_name;
        baby_name.replace(baby_name.end()-5,baby_name.end(),"_smearbaby");
        MakeBabyNtuple(Form("%s.root", baby_name.c_str()));
    }
    
    // File Loop
    int nDuplicates = 0;
    int nEvents = chain->GetEntries();
    unsigned int nEventsChain = (maxEvents > 0) ? maxEvents : nEvents;  
    cout << "[SmearLooper::loop] running on " << nEventsChain << " events" << endl;
    unsigned int nEventsTotal = 0;
    TObjArray *listOfFiles = chain->GetListOfFiles();
    TIter fileIter(listOfFiles);
    TFile *currentFile = 0;
    int nTotalEvents = 0;
    while ( (currentFile = (TFile*)fileIter.Next()) ) {
        cout << "[SmearLooper::loop] running on file: " << currentFile->GetTitle() << endl;

        // Get File Content
        TFile f( currentFile->GetTitle() );
        TTree *tree = (TTree*)f.Get("mt2");
        TTreeCache::SetLearnEntries(10);
        tree->SetCacheSize(128*1024*1024);

        t.Init(tree);

        // Event Loop
        unsigned int nEventsTree = tree->GetEntriesFast();
        // if (maxEvents > 0) nEventsTree = (unsigned int)maxEvents;
        // nEventsTree = 10000;
        for( unsigned int event = 0; event < nEventsTree; ++event) {
            ++nTotalEvents;
            if (maxEvents > 0 && nTotalEvents > maxEvents) break;
          
            t.GetEntry(event);

            int prescale = 1; //FIX THIS

            bool doDYplots = false;
            bool doOFplots = false;
            bool doLowPtSFplots = false;
            bool doLowPtOFplots = false;
            bool doSLplots = false;

            if(t.nLepLowMT == 1){
                doSLplots = true;
            }

            if(t.nlep == 2){
                bool passSFtrig = passesTriggerSF();
                bool passOFtrig = passesTriggerOF();
                if((t.lep_charge[0] * t.lep_charge[1] == -1)
                   && (abs(t.lep_pdgId[0]) == 13 ||  t.lep_tightId[0] > 0 )
                   && (abs(t.lep_pdgId[1]) == 13 ||  t.lep_tightId[1] > 0 )
                   && t.lep_pt[0] > 100 && t.lep_pt[1] > 35 ) {

                    if(abs(t.lep_pdgId[0]) == abs(t.lep_pdgId[1]) && passSFtrig){
                        if     (t.zll_pt > 200 && fabs(t.zll_mass - 91.19) < 20)                    doDYplots = true;
                        else if(t.zll_pt < 200 && fabs(t.zll_mass - 91.19) > 20 && t.zll_mass > 50) doLowPtSFplots = true;
                    }
                    else if(abs(t.lep_pdgId[0]) != abs(t.lep_pdgId[1]) && passOFtrig){
                        if     (t.zll_pt > 200 && fabs(t.zll_mass - 91.19) < 20)                    doOFplots = true;
                        else if(t.zll_pt < 200 && fabs(t.zll_mass - 91.19) > 20 && t.zll_mass > 50) doLowPtOFplots = true;
                    }
                }
            }

            //--------------------
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

            if (makeSmearBaby_) InitBabyNtuple();

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

            if(doRebalanceAndSmear_ && t.rebal_status == -1) continue;
      
            // this is necessary for doing a normalization check. Want to run the same 
            //set of events through R&S and non-R&S.
            if(!doRebalanceAndSmear_ && t.rebal_status == -1 && CUT_LEVEL_==3) continue;

            if( applyJSON && t.isData && !goodrun(t.run, t.lumi) ) continue;
      
            if(isinf(t.met_pt) || isnan(t.met_pt) || isinf(t.ht) || isnan(t.ht) || t.jet_pt[0] > 13000.) continue;

            if (t.nVert == 0) continue;
            if (!doRebalanceAndSmear_ && CUT_LEVEL_<4){
                if(t.nJet30 < 1) continue;
                if(t.ht < 250.0 && (t.nlep!=2 || t.zll_ht < 250)) continue;
                if(t.ht < 1200.0 && t.met_pt < 30.0 && (t.nlep!=2 || (t.zll_ht<1200 && t.zll_met_pt<30))) continue;
                if(t.ht >= 1200.0 && t.met_pt < 30.0 && (t.nlep!=2 || (t.zll_ht>=1200 && t.zll_met_pt<30))) continue;
                if(t.nJet30>=3 && t.mt2 < 50 && (t.nlep!=2 || t.zll_mt2<50)) continue;
                //if((t.diffMetMht/t.met_pt) > 0.5) continue;
                //if(t.deltaPhiMin < 0.3) continue;
            }


            if (config_.filters["eeBadScFilter"] && !t.Flag_eeBadScFilter) continue; 
            if (config_.filters["globalSuperTightHalo2016Filter"] && !t.Flag_globalSuperTightHalo2016Filter) continue; 
            if (config_.filters["globalTightHalo2016Filter"] && !t.Flag_globalTightHalo2016Filter) continue; 
            if (config_.filters["goodVertices"] && !t.Flag_goodVertices) continue;
            if (config_.filters["HBHENoiseFilter"] && !t.Flag_HBHENoiseFilter) continue;
            if (config_.filters["HBHENoiseIsoFilter"] && !t.Flag_HBHENoiseIsoFilter) continue;
            if (config_.filters["EcalDeadCellTriggerPrimitiveFilter"] && !t.Flag_EcalDeadCellTriggerPrimitiveFilter) continue;
            if (config_.filters["ecalBadCalibFilter"] && !t.Flag_ecalBadCalibFilter) continue;
            if (config_.filters["ecalBadCalibFilterUpdate"] && !t.Flag_ecalBadCalibFilterUpdate) continue;
            if (config_.filters["badMuonFilter"] && !t.Flag_badMuonFilter) continue;
            if (config_.filters["badChargedCandidateFilter"] && !t.Flag_badChargedCandidateFilter) continue; 
            if (config_.filters["badMuonFilterV2"] && !t.Flag_badMuonFilterV2) continue;
            if (config_.filters["badChargedHadronFilterV2"] && !t.Flag_badChargedHadronFilterV2) continue; 


            if (!doRebalanceAndSmear_ && CUT_LEVEL_<4) {
                if (t.met_miniaodPt>200 && t.met_miniaodPt / t.met_caloPt > 5.0) continue;
                if (t.nJet200MuFrac50DphiMet > 0) continue;
            }

            // decide if we should apply HEM veto based on run number (data) or event number (MC)
            bool candidateHEMevent = false;
            if(config_.year == 2018 && ((t.isData && t.run >= HEM_startRun) || (!t.isData && t.evt % HEM_fracDen < HEM_fracNum)))
                candidateHEMevent = true;

            // flag signal samples
            if (t.evt_id >= 1000) isSignal_ = true;
            else isSignal_ = false; 

            // remove spiky qcd mc events
            if(t.evt_id >=150 && t.evt_id<153) continue;
            if(t.evt_id == 153 && t.ht > 600) continue;  //ht300to500
            if(t.evt_id == 154 && t.ht > 1000) continue;  //ht500to700
            if(t.evt_id == 155 && t.ht > 1500) continue;  //ht700to1000

            passMonojetId_ = isSignal_ || (t.nJet30>=1 && t.jet_id[0]>=4);

            //---------------------
            // set weights and start making plots
            //---------------------
            outfile_->cd();
            const float lumi = 1.0;
            evtweight_ = 1.;

            // apply relevant weights to MC
            if (!t.isData) {
                evtweight_ = t.evt_scale1fb * lumi;

                if (applyWeights_) {
                    evtweight_ *= t.weight_lepsf * t.weight_btagsf;
                    if(doNTrueIntReweight){
                        if(h_nTrueInt_weights_==0){
                            cout << "[SmearLooper::loop] WARNING! You requested to do nTrueInt reweighting but didn't supply a PU weights file. Turning off." << endl;
                        }else{
                            int nTrueInt_input = t.nTrueInt;
                            float puWeight = h_nTrueInt_weights_->GetBinContent(h_nTrueInt_weights_->FindBin(nTrueInt_input));
                            evtweight_ *= puWeight;
                        }
                    }
                }
            } // !isData

            plot1D("h_nvtx",       t.nVert,       evtweight_, h_1d_global, ";N(vtx)", 80, 0, 80);
            plot1D("h_mt2",       t.mt2,       evtweight_, h_1d_global, ";M_{T2} [GeV]", 80, 0, 800);

            // note: this will double count some leptons, since reco leptons can appear as PFcands
            nlepveto_ = t.nMuons10 + t.nElectrons10 + t.nPFLep5LowMT + t.nPFHad10LowMT;
            if(doRebalanceAndSmear_ && nlepveto_ > 0 && CUT_LEVEL_<4) continue;

            // count number of forward jets
            nJet30Eta3_ = 0;
            for (int ijet = 0; ijet < t.njet; ++ijet) {
                if (t.jet_pt[ijet] > 30. && fabs(t.jet_eta[ijet]) > 3.0) ++nJet30Eta3_;
            }            

            // veto on forward jets
            if (doHFJetVeto && nJet30Eta3_ > 0) continue;

            // veto events with a jet that fails the loose PFJet ID
            // if (t.nJet30FailId && !doRebalanceAndSmear_) continue;
            if (t.nJet30FailId) continue;
      
            ////////////////////////////////////
            /// done with overall selection  /// 
            ////////////////////////////////////
            ///   time to fill histograms    /// 
            ////////////////////////////////////

            // fill variables to store in baby
            if (makeSmearBaby_) {
                run                  = t.run;
                ls                   = t.lumi;
                evt                  = t.evt;
                isData               = t.isData;
                evt_scale1fb         = t.evt_scale1fb;
                evt_id               = t.evt_id;
                HLT_PFHT1050          = t.HLT_PFHT1050;            
                HLT_PFJet500         = t.HLT_PFJet500;           
                HLT_PFHT180_Prescale = t.HLT_PFHT180_Prescale;   
                HLT_PFHT250_Prescale = t.HLT_PFHT250_Prescale;   
                HLT_PFHT370_Prescale = t.HLT_PFHT370_Prescale;   
                HLT_PFHT430_Prescale = t.HLT_PFHT430_Prescale;                 
                HLT_PFHT510_Prescale = t.HLT_PFHT510_Prescale;
                HLT_PFHT590_Prescale = t.HLT_PFHT590_Prescale;
                HLT_PFHT680_Prescale = t.HLT_PFHT680_Prescale;
                HLT_PFHT780_Prescale = t.HLT_PFHT780_Prescale;
                HLT_PFHT890_Prescale = t.HLT_PFHT890_Prescale;
                a_nJet30             = t.nJet30;
                a_nBJet20            = t.nBJet20;
                a_deltaPhiMin        = t.deltaPhiMin;
                a_diffMetMhtOverMet  = t.diffMetMht/t.met_pt;
                a_ht                 = t.ht;
                a_mt2                = t.mt2;
                a_met_pt             = t.met_pt;
                a_met_phi            = t.met_phi;
            }

            // // REMOVE THIS
            // if(t.ht < 700)
            //     continue;
            
            if(doRebalanceAndSmear_){

                if (not passesTrigger()) continue;
        
                prescale = 1.0; //default value for MC       
                prescale = getTriggerPrescale();

                // if we run into the limit of MAX_SMEARS, then the  *effective* prescale for our purposes is higher by the below factor
                prescale_correction = 1.0;
                if(numberOfSmears*prescale > MAX_SMEARS)
                    prescale_correction = (float)numberOfSmears*prescale / MAX_SMEARS;
                
                jet_pt.clear();
                jet_eta.clear();
                jet_phi.clear();
                jet_btagDeepCSV.clear();
                PU_passes_id_jet_pt.clear();
                PU_passes_id_jet_eta.clear();
                PU_passes_id_jet_phi.clear();
                PU_passes_id_jet_btagDeepCSV.clear();
                PU_fails_id_jet_pt.clear();
                PU_fails_id_jet_eta.clear();
                PU_fails_id_jet_phi.clear();

                float new_met_x = t.met_pt*cos(t.met_phi);
                float new_met_y = t.met_pt*sin(t.met_phi);

                float pt_soft_true_x = 0.0;
                float pt_soft_true_y = 0.0;
                float pt_soft_reco_x = - t.met_pt*cos(t.met_phi);
                float pt_soft_reco_y = - t.met_pt*sin(t.met_phi);
                bool dojet = true;
                int jetCounter = -1;                
                float rf = -999;
                r_ht = 0;
                r_jet1_pt=0;
                r_jet2_pt=-999;
                r_jet1_eta=0;
                r_jet2_eta=-999;
                for(int i=0; i<t.njet; i++){

                    new_met_x += t.jet_pt[i]*cos(t.jet_phi[i]);
                    new_met_y += t.jet_pt[i]*sin(t.jet_phi[i]);

                    pt_soft_reco_x -= t.jet_pt[i]*cos(t.jet_phi[i]);
                    pt_soft_reco_y -= t.jet_pt[i]*sin(t.jet_phi[i]);

                    if((t.jet_pt[i] < 100) && (t.jet_puId[i] < 1)){
                        dojet = false;
                        if(t.jet_id[i] > 0){
                            PU_passes_id_jet_pt.push_back(t.jet_pt[i]);
                            PU_passes_id_jet_eta.push_back(t.jet_eta[i]);
                            PU_passes_id_jet_phi.push_back(t.jet_phi[i]);
                            PU_passes_id_jet_btagDeepCSV.push_back(t.jet_btagDeepCSV[i]);
                        } else {
                            PU_fails_id_jet_pt.push_back(t.jet_pt[i]);
                            PU_fails_id_jet_eta.push_back(t.jet_eta[i]);
                            PU_fails_id_jet_phi.push_back(t.jet_phi[i]);
                        }
                    }
                    else dojet = true;

                    if(!dojet) continue;

                    jetCounter++;
                    
                    rf = t.rebal_factors[jetCounter];
                    jet_pt.push_back(t.jet_pt[i]*rf);
                    jet_eta.push_back(t.jet_eta[i]);
                    jet_phi.push_back(t.jet_phi[i]);
                    jet_btagDeepCSV.push_back(t.jet_btagDeepCSV[i]);

                    if (makeSmearBaby_){
                        r_ht += jet_pt.back();
                        if(jet_pt.back() > r_jet1_pt){
                            r_jet2_pt = r_jet1_pt;
                            r_jet2_eta = r_jet1_eta;
                            r_jet1_pt = jet_pt.back();
                            r_jet1_eta = jet_eta.back();
                        }else if(jet_pt.back() > r_jet2_pt){
                            r_jet2_pt = jet_pt.back();
                            r_jet2_eta = jet_eta.back();
                        }
                    }

                    new_met_x -= jet_pt.at(jetCounter)*cos(jet_phi.at(jetCounter));
                    new_met_y -= jet_pt.at(jetCounter)*sin(jet_phi.at(jetCounter));
                    pt_soft_true_x -= jet_pt.at(jetCounter)*cos(jet_phi.at(jetCounter));
                    pt_soft_true_y -= jet_pt.at(jetCounter)*sin(jet_phi.at(jetCounter));

                }

                float reb_met_pt = sqrt(new_met_x*new_met_x + new_met_y*new_met_y);

                int numSmears = min((numberOfSmears*prescale), MAX_SMEARS);

                // rebalanced met cut to removed EWK contamination in data
                if(t.isData && reb_met_pt > EWK_CUTOFF)
                    continue;

                if (makeSmearBaby_) {
                    r_met_pt   = t.rebal_met_pt;
                    r_met_phi  = t.rebal_met_phi;
                    r_nJet     = jetCounter;
                    s_prescale = prescale;
                    s_nsmears  = numSmears;
                }
        
                random->SetSeed();

                float orig_weight = evtweight_;
                for(int iSmear=0; iSmear<numSmears; iSmear++){
                    evtweight_ = 1.0;

                    std::vector<float> jet_pt_smeared = jet_pt;

                    for(unsigned int i=0; i<jet_pt_smeared.size(); i++){
                        float smear = reader.GetRandomResponse(jet_pt[i], jet_eta[i], useBjetResponse_ ? (jet_btagDeepCSV[i]>config_.btag_med_threshold_DeepCSV) : false);
                        plot1D("h_smear",      smear,   evtweight_, h_1d_global, ";smear", 10000, 0, 10);
                        jet_pt_smeared.at(i) *= smear;
                        
                        if(doCustomHEMsmear && candidateHEMevent){
                            if(jet_eta[i] < -1.5 && jet_phi[i] > -1.6 && jet_phi[i] < -0.8){
                                float smearfact = 1.0;
                                float pt = jet_pt_smeared.at(i);
                                // smearfact = HEMsmears.at(0)->GetRandom();
                                if(20 <= pt  && pt < 30)  smearfact = HEMsmears.at(1)->GetRandom();
                                if(30 <= pt  && pt < 50)  smearfact = HEMsmears.at(2)->GetRandom();
                                if(50 <= pt  && pt < 80)  smearfact = HEMsmears.at(3)->GetRandom();
                                if(80 <= pt  && pt < 120) smearfact = HEMsmears.at(4)->GetRandom();
                                if(120 <= pt && pt < 170) smearfact = HEMsmears.at(5)->GetRandom();
                                if(170 <= pt && pt < 300) smearfact = HEMsmears.at(6)->GetRandom();
                                if(300 <= pt)             smearfact = HEMsmears.at(7)->GetRandom();

                                // cout << endl << "HEM smear! " << smearfact << endl;
                                jet_pt_smeared.at(i) *= smearfact;
                            }
                        }
                    }

                    new_met_x = t.met_pt*cos(t.met_phi);
                    new_met_y = t.met_pt*sin(t.met_phi);

                    float met_pt;
                    float met_phi;
                    float mt2;
                    float ht;
                    int nJet30;
                    int nBJet20;
                    float diffMetMht;
                    float deltaPhiMin;
                    float jet1_pt;
                    float jet2_pt;
                    float jet1_eta;
                    float jet2_eta;
                    float jet1_phi;
                    float jet2_phi;

                    float jet_x = 0;
                    float jet_y = 0;
                    nJet30 = 0;
                    nBJet20 = 0;
                    ht = 0;
                    for(int i=0; i<t.njet; i++){
                        new_met_x += t.jet_pt[i]*cos(t.jet_phi[i]);
                        new_met_y += t.jet_pt[i]*sin(t.jet_phi[i]);
                    }
                    for(unsigned int i=0; i<jet_pt_smeared.size(); i++){
                        new_met_x -= jet_pt_smeared.at(i)*cos(jet_phi.at(i));
                        new_met_y -= jet_pt_smeared.at(i)*sin(jet_phi.at(i));
                        jet_x += (jet_pt_smeared.at(i))*cos(jet_phi.at(i));
                        jet_y += (jet_pt_smeared.at(i))*sin(jet_phi.at(i));
                        if( (jet_pt_smeared.at(i) > 30.0) && (fabs(jet_eta.at(i)) < 2.4) ) ht += jet_pt_smeared.at(i);
                        if( (jet_pt_smeared.at(i) > 30.0) && (fabs(jet_eta.at(i)) < 2.4) ) nJet30++;
                        if( (jet_pt_smeared.at(i) > 20.0) && (fabs(jet_eta.at(i)) < 2.4) && (jet_btagDeepCSV.at(i) > config_.btag_med_threshold_DeepCSV) ) nBJet20++;
                    }
                    for(unsigned int i=0; i<PU_passes_id_jet_pt.size(); i++){
                        new_met_x -= PU_passes_id_jet_pt.at(i)*cos(PU_passes_id_jet_phi.at(i));
                        new_met_y -= PU_passes_id_jet_pt.at(i)*sin(PU_passes_id_jet_phi.at(i));
                        jet_x += (PU_passes_id_jet_pt.at(i))*cos(PU_passes_id_jet_phi.at(i));
                        jet_y += (PU_passes_id_jet_pt.at(i))*sin(PU_passes_id_jet_phi.at(i));
                        if( (PU_passes_id_jet_pt.at(i) > 30.0) && (fabs(PU_passes_id_jet_eta.at(i)) < 2.4) ) ht += PU_passes_id_jet_pt.at(i);
                        if( (PU_passes_id_jet_pt.at(i) > 30.0) && (fabs(PU_passes_id_jet_eta.at(i)) < 2.4) ) nJet30++;
                        if( (PU_passes_id_jet_pt.at(i) > 20.0) && (fabs(PU_passes_id_jet_eta.at(i)) < 2.4) && (PU_passes_id_jet_btagDeepCSV.at(i) > config_.btag_med_threshold_DeepCSV) ) nBJet20++;
                    }
                    for(unsigned int i=0; i<PU_fails_id_jet_pt.size(); i++){
                        new_met_x -= PU_fails_id_jet_pt.at(i)*cos(PU_fails_id_jet_phi.at(i));
                        new_met_y -= PU_fails_id_jet_pt.at(i)*sin(PU_fails_id_jet_phi.at(i));
                        jet_x += (PU_fails_id_jet_pt.at(i))*cos(PU_fails_id_jet_phi.at(i));
                        jet_y += (PU_fails_id_jet_pt.at(i))*sin(PU_fails_id_jet_phi.at(i));
                    }

                    met_pt = sqrt(new_met_x*new_met_x + new_met_y*new_met_y);
                    TLorentzVector met_vec;
                    met_vec.SetPxPyPzE(new_met_x, new_met_y, 0, met_pt);
                    met_phi = met_vec.Phi();


                    if(nJet30 < 1 && CUT_LEVEL_<3) continue;
                    if(ht < 250.0 && CUT_LEVEL_<3) continue;
                    if (met_pt < 30. && CUT_LEVEL_<3) continue;
                    if (met_pt<250. && ht<1200 && CUT_LEVEL_<2) continue;

                    s_jet1_pt[iSmear] = -1;
                    s_jet1_eta[iSmear] = -1;
                    s_jet1_smearfact[iSmear] = -1;
                    s_jet2_smearfact[iSmear] = -1;
                    s_jet2_pt[iSmear] = -1;
                    s_jet2_eta[iSmear] = -1;
                    s_max_smearfact[iSmear] = -1;
                    float max_ptdiff = 0;
                    
                    bool foundHEMjet = false;
                    float actual_HEM_ptCut = HEM_ptCut;
                    if(nJet30==1 || ht<575)
                        actual_HEM_ptCut = 20.0;
                    std::vector<LorentzVector> p4sForDphi;
                    for(unsigned int i=0; i<jet_pt_smeared.size(); i++){                        
                        if(doHEMregionVeto){
                            if(jet_eta[i] > HEM_region[0] && jet_eta[i] < HEM_region[1] && 
                               jet_phi[i] > HEM_region[2] && jet_phi[i] < HEM_region[3] && 
                               jet_pt_smeared[i] > actual_HEM_ptCut)
                                foundHEMjet = true;
                        }
                        if(jet_pt_smeared.at(i) < 30.0 || fabs(jet_eta.at(i)) > 4.7) continue;
                        if(jet_pt_smeared.at(i) > s_jet1_pt[iSmear]){
                            s_jet2_pt[iSmear] = s_jet1_pt[iSmear];
                            s_jet2_eta[iSmear] = s_jet1_eta[iSmear];
                            s_jet2_smearfact[iSmear] = s_jet1_smearfact[iSmear];
                            s_jet1_pt[iSmear] = jet_pt_smeared.at(i);
                            s_jet1_eta[iSmear] = jet_eta.at(i);
                            s_jet1_smearfact[iSmear] = jet_pt_smeared.at(i) / jet_pt.at(i);
                        }else if(jet_pt_smeared.at(i) > s_jet2_pt[iSmear]){
                            s_jet2_pt[iSmear] = jet_pt_smeared.at(i);
                            s_jet2_eta[iSmear] = jet_eta.at(i);                            
                            s_jet2_smearfact[iSmear] = jet_pt_smeared.at(i) / jet_pt.at(i);
                        }

                        float pt_diff = fabs(jet_pt.at(i) - jet_pt_smeared.at(i));
                        if(pt_diff > max_ptdiff){
                            max_ptdiff = pt_diff;
                            s_max_smearfact[iSmear] = jet_pt_smeared.at(i) / jet_pt.at(i);
                        }

                        LorentzVector tempVec;
                        tempVec.SetPx((jet_pt_smeared.at(i))*cos(jet_phi.at(i)));
                        tempVec.SetPy((jet_pt_smeared.at(i))*sin(jet_phi.at(i)));
                        tempVec.SetPz((jet_pt_smeared.at(i))*sinh(jet_eta.at(i)));
                        tempVec.SetE(tempVec.P());
                        p4sForDphi.push_back(tempVec);
                    }
                    for(unsigned int i=0; i<PU_passes_id_jet_pt.size(); i++){
                        if(doHEMregionVeto){
                            if(PU_passes_id_jet_eta[i] > HEM_region[0] && PU_passes_id_jet_eta[i] < HEM_region[1] && 
                               PU_passes_id_jet_phi[i] > HEM_region[2] && PU_passes_id_jet_phi[i] < HEM_region[3] && 
                               PU_passes_id_jet_pt[i] > actual_HEM_ptCut)
                                foundHEMjet = true;
                        }
                        if(PU_passes_id_jet_pt.at(i) < 30.0 || fabs(PU_passes_id_jet_eta.at(i)) > 4.7) continue;
                        LorentzVector tempVec;
                        tempVec.SetPx((PU_passes_id_jet_pt.at(i))*cos(PU_passes_id_jet_phi.at(i)));
                        tempVec.SetPy((PU_passes_id_jet_pt.at(i))*sin(PU_passes_id_jet_phi.at(i)));
                        tempVec.SetPz((PU_passes_id_jet_pt.at(i))*sinh(PU_passes_id_jet_eta.at(i)));
                        tempVec.SetE(tempVec.P());
                        p4sForDphi.push_back(tempVec);
                    }

                    if(doHEMregionVeto && candidateHEMevent && foundHEMjet)
                        continue;

                    sort(p4sForDphi.begin(), p4sForDphi.end(), sortByPt);

                    deltaPhiMin = 999;
                    for (unsigned int ip4 = 0; ip4 < p4sForDphi.size(); ++ip4) {
                        if(ip4 < 4) deltaPhiMin = min(deltaPhiMin, DeltaPhi( met_phi, p4sForDphi.at(ip4).phi() ));
                    }

                    // if(deltaPhiMin < 0.3 && CUT_LEVEL_<2) continue;

                    std::vector<LorentzVector> p4sForHems;
                    for(unsigned int i=0; i<jet_pt_smeared.size(); i++){
                        if(jet_pt_smeared.at(i) < 30.0) continue;
                        if(fabs(jet_eta.at(i)) > 2.4) continue;
                        LorentzVector tempVec;
                        tempVec.SetPx((jet_pt_smeared.at(i))*cos(jet_phi.at(i)));
                        tempVec.SetPy((jet_pt_smeared.at(i))*sin(jet_phi.at(i)));
                        tempVec.SetPz((jet_pt_smeared.at(i))*sinh(jet_eta.at(i)));
                        tempVec.SetE(tempVec.P());
                        p4sForHems.push_back(tempVec);
                    }
                    for(unsigned int i=0; i<PU_passes_id_jet_pt.size(); i++){
                        if(PU_passes_id_jet_pt.at(i) < 30.0) continue;
                        if(fabs(PU_passes_id_jet_eta.at(i)) > 2.4) continue;
                        LorentzVector tempVec;
                        tempVec.SetPx((PU_passes_id_jet_pt.at(i))*cos(PU_passes_id_jet_phi.at(i)));
                        tempVec.SetPy((PU_passes_id_jet_pt.at(i))*sin(PU_passes_id_jet_phi.at(i)));
                        tempVec.SetPz((PU_passes_id_jet_pt.at(i))*sinh(PU_passes_id_jet_eta.at(i)));
                        tempVec.SetE(tempVec.P());
                        p4sForHems.push_back(tempVec);
                    }
                    sort(p4sForHems.begin(), p4sForHems.end(), sortByPt);

                    LorentzVector sumMhtp4 = LorentzVector(0,0,0,0);
                    for (unsigned int ip4 = 0; ip4 < p4sForHems.size(); ++ip4) {
                        sumMhtp4 -= p4sForHems.at(ip4);
                    }
                    float mht_pt  = sumMhtp4.pt();
                    float mht_phi = sumMhtp4.phi();
                    TVector2 mhtVector = TVector2(mht_pt*cos(mht_phi), mht_pt*sin(mht_phi));
                    TVector2 metVector = TVector2(met_pt*cos(met_phi), met_pt*sin(met_phi));
                    diffMetMht = (mhtVector - metVector).Mod();

                    if(diffMetMht/met_pt > 0.5 && CUT_LEVEL_<2) continue;
                    
                    vector<LorentzVector> hemJets;
                    if(p4sForHems.size() > 1){
                        hemJets = getHemJets(p4sForHems); 
                        mt2 = HemMT2(met_pt, met_phi, hemJets.at(0), hemJets.at(1));
                        jet1_pt = p4sForHems.at(0).pt();
                        jet2_pt = p4sForHems.at(1).pt();
                        jet1_eta = p4sForHems.at(0).eta();
                        jet2_eta = p4sForHems.at(1).eta();
                        jet1_phi = p4sForHems.at(0).phi();
                        jet2_phi = p4sForHems.at(1).phi();
                    }else{
                        if(p4sForHems.size() == 1){
                            jet1_pt = p4sForHems.at(0).pt();
                            jet1_eta = p4sForHems.at(0).eta();
                            jet1_phi = p4sForHems.at(0).phi();
                        }else{
                            jet1_pt = -1;
                            jet1_eta = -1;
                            jet1_phi = -1;
                        }
                        mt2 = -1;
                        jet2_pt = -1;
                        jet2_eta = -1;
                        jet2_phi = -1;
                    }

                    if(nJet30>=3 && mt2 < 50.0 && CUT_LEVEL_<3) continue;
                    if(nJet30>=3 && mt2 < 100.0 && CUT_LEVEL_<2) continue;

                    RS_vars_["mt2"] = mt2;
                    RS_vars_["reb_met_pt"] = reb_met_pt;
                    RS_vars_["met_pt"] = met_pt;
                    RS_vars_["mht_pt"] = mht_pt;
                    RS_vars_["ht"] = ht;
                    RS_vars_["nJet30"] = nJet30;
                    RS_vars_["nBJet20"] = nBJet20;
                    RS_vars_["deltaPhiMin"] = deltaPhiMin;
                    RS_vars_["diffMetMht"] = diffMetMht;
                    RS_vars_["jet1_pt"] = jet1_pt;
                    RS_vars_["jet2_pt"] = jet2_pt;
                    RS_vars_["jet1_eta"] = jet1_eta;
                    RS_vars_["jet2_eta"] = jet2_eta;
                    RS_vars_["jet1_phi"] = jet1_phi;
                    RS_vars_["jet2_phi"] = jet2_phi;

                    if (makeSmearBaby_) {
                        s_nJet30[iSmear]            = nJet30;
                        s_nBJet20[iSmear]           = nBJet20;
                        s_deltaPhiMin[iSmear]       = deltaPhiMin;
                        s_diffMetMhtOverMet[iSmear] = diffMetMht/met_pt;
                        s_ht[iSmear]                = ht;
                        s_mt2[iSmear]               = mt2;
                        s_met_pt[iSmear]            = met_pt;
                    }
                    
                    // // ELECTROWEAK CORRECTIONS. Hard-coded based on comparisons of QCD/EWK MC.
                    // if(t.isData){
                    //     if(RS_vars_["ht"] >=250  && RS_vars_["ht"]<450)  evtweight_*=1.02;
                    //     if(RS_vars_["ht"] >=450  && RS_vars_["ht"]<575)  evtweight_*=1.02;
                    //     if(RS_vars_["ht"] >=575  && RS_vars_["ht"]<1200) evtweight_*=1.17;
                    //     if(RS_vars_["ht"] >=1200 && RS_vars_["ht"]<1500) evtweight_*=1.11;
                    //     if(RS_vars_["ht"] >=1500)                        evtweight_*=1.07;
                    // }

                    if(doNJetReweighting_){
                        TH1D* weights = 0;
                        if(                         RS_vars_["ht"]<450)  weights = nJetWeights_["ht250to450"];
                        if(RS_vars_["ht"] >=450  && RS_vars_["ht"]<575)  weights = nJetWeights_["ht450to575"];
                        if(RS_vars_["ht"] >=575  && RS_vars_["ht"]<1200) weights = nJetWeights_["ht575to1200"];
                        if(RS_vars_["ht"] >=1200 && RS_vars_["ht"]<1500) weights = nJetWeights_["ht1200to1500"];
                        if(RS_vars_["ht"] >=1500)                        weights = nJetWeights_["ht1500to2000"];
                        evtweight_ *= weights->GetBinContent(weights->FindBin(RS_vars_["nJet30"]));
                    }

                    //fill temp histograms
                    fillHistos(SRNoCut_temp.srHistMap, SRNoCut_temp.GetNumberOfMT2Bins(), SRNoCut_temp.GetMT2Bins(), SRNoCut_temp.GetName(), "");
                    if(RS_vars_["nJet30"]>=2 && RS_vars_["ht"]>=450 && RS_vars_["ht"]<1200 && RS_vars_["met_pt"]>30 && RS_vars_["mt2"]>50) 
                        fillHistos(SRJustHT1_temp.srHistMap, SRJustHT1_temp.GetNumberOfMT2Bins(), SRJustHT1_temp.GetMT2Bins(), SRJustHT1_temp.GetName(), "");
                    if(RS_vars_["nJet30"]>=2 && RS_vars_["ht"]>=1200 && RS_vars_["met_pt"]>30 && RS_vars_["mt2"]>50) 
                    // if(RS_vars_["nJet30"]>=2 && RS_vars_["ht"]>=1200 && RS_vars_["met_pt"]>30 && RS_vars_["mt2"]>100 && RS_vars_["diffMetMht"]/RS_vars_["met_pt"]<0.5) 
                        fillHistos(SRJustHT2_temp.srHistMap, SRJustHT2_temp.GetNumberOfMT2Bins(), SRJustHT2_temp.GetMT2Bins(), SRJustHT2_temp.GetName(), "");
                    if(RS_vars_["nJet30"]>=2 && RS_vars_["ht"]>=250 && RS_vars_["ht"]<1200 && RS_vars_["met_pt"]>30 && RS_vars_["mt2"]>50) 
                        fillHistos(SRJustHT3_temp.srHistMap, SRJustHT3_temp.GetNumberOfMT2Bins(), SRJustHT3_temp.GetMT2Bins(), SRJustHT3_temp.GetName(), "");
                    if(RS_vars_["nJet30"]>=2 && RS_vars_["ht"]>=250 && RS_vars_["ht"]<450 && RS_vars_["met_pt"]>30 && RS_vars_["mt2"]>50) 
                        fillHistos(SRJustHT4_temp.srHistMap, SRJustHT4_temp.GetNumberOfMT2Bins(), SRJustHT4_temp.GetMT2Bins(), SRJustHT4_temp.GetName(), "");
                    if(RS_vars_["nJet30"]>=2 && RS_vars_["ht"]>=450 && RS_vars_["ht"]<575 && RS_vars_["met_pt"]>30 && RS_vars_["mt2"]>50) 
                        fillHistos(SRJustHT5_temp.srHistMap, SRJustHT5_temp.GetNumberOfMT2Bins(), SRJustHT5_temp.GetMT2Bins(), SRJustHT5_temp.GetName(), "");
                    if(RS_vars_["nJet30"]>=2 && RS_vars_["ht"]>=575 && RS_vars_["ht"]<1200 && RS_vars_["met_pt"]>30 && RS_vars_["mt2"]>50) 
                        fillHistos(SRJustHT6_temp.srHistMap, SRJustHT6_temp.GetNumberOfMT2Bins(), SRJustHT6_temp.GetMT2Bins(), SRJustHT6_temp.GetName(), "");
                    if(RS_vars_["nJet30"]>=2 && RS_vars_["ht"]>=1200 && RS_vars_["ht"]<1500 && RS_vars_["met_pt"]>30 && RS_vars_["mt2"]>50) 
                        fillHistos(SRJustHT7_temp.srHistMap, SRJustHT7_temp.GetNumberOfMT2Bins(), SRJustHT7_temp.GetMT2Bins(), SRJustHT7_temp.GetName(), "");
                    if(RS_vars_["nJet30"]>=2 && RS_vars_["ht"]>=1500 && RS_vars_["met_pt"]>30 && RS_vars_["mt2"]>50) 
                        fillHistos(SRJustHT8_temp.srHistMap, SRJustHT8_temp.GetNumberOfMT2Bins(), SRJustHT8_temp.GetMT2Bins(), SRJustHT8_temp.GetName(), "");          
                    fillHistosSignalRegion("sr");
                    fillHistosSRBase();
                    fillHistosInclusive();
                    fillHistosCRRSInvertDPhi();
                    fillHistosCRRSMT2SideBand();
                    fillHistosCRRSDPhiMT2();
                }//numberOfSmears

                if(!t.isData) evtweight_ = orig_weight;
                //now fill histograms once per event with appropriate weights to get correct stat uncertainty
                plotRS(SRNoCut_temp.srHistMap, SRNoCut.srHistMap, SRNoCut.GetName());
                plotRS(SRJustHT1_temp.srHistMap, SRJustHT1.srHistMap, SRJustHT1.GetName());
                plotRS(SRJustHT2_temp.srHistMap, SRJustHT2.srHistMap, SRJustHT2.GetName());
                plotRS(SRJustHT3_temp.srHistMap, SRJustHT3.srHistMap, SRJustHT3.GetName());
                plotRS(SRJustHT4_temp.srHistMap, SRJustHT4.srHistMap, SRJustHT4.GetName());
                plotRS(SRJustHT5_temp.srHistMap, SRJustHT5.srHistMap, SRJustHT5.GetName());
                plotRS(SRJustHT6_temp.srHistMap, SRJustHT6.srHistMap, SRJustHT6.GetName());
                plotRS(SRJustHT7_temp.srHistMap, SRJustHT7.srHistMap, SRJustHT7.GetName());
                plotRS(SRJustHT8_temp.srHistMap, SRJustHT8.srHistMap, SRJustHT8.GetName());        
                plotRS(SRBase_temp.srHistMap, SRBase.srHistMap, SRBase.GetName());
                plotRS(SRBase_temp.crRSInvertDPhiHistMap, SRBase.crRSInvertDPhiHistMap, "crRSInvertDPhi"+SRBase.GetName());
                plotRS(SRBase_temp.crRSMT2SideBandHistMap, SRBase.crRSMT2SideBandHistMap, "crRSMT2SideBand"+SRBase.GetName());
                plotRS(SRBase_temp.crRSDPhiMT2HistMap, SRBase.crRSDPhiMT2HistMap, "crRSDPhiMT2"+SRBase.GetName());
                for(unsigned int srN = 0; srN < SRVec_temp.size(); srN++){
                    plotRS(SRVec_temp.at(srN).srHistMap, SRVec.at(srN).srHistMap, "sr"+SRVec.at(srN).GetName());
                    plotRS(SRVec_temp.at(srN).crRSInvertDPhiHistMap, SRVec.at(srN).crRSInvertDPhiHistMap, "crRSInvertDPhi"+SRVec.at(srN).GetName());
                    plotRS(SRVec_temp.at(srN).crRSMT2SideBandHistMap, SRVec.at(srN).crRSMT2SideBandHistMap, "crRSMT2SideBand"+SRVec.at(srN).GetName());
                    plotRS(SRVec_temp.at(srN).crRSDPhiMT2HistMap, SRVec.at(srN).crRSDPhiMT2HistMap, "crRSDPhiMT2"+SRVec.at(srN).GetName());
                }
                for(unsigned int srN = 0; srN < SRVecMonojet_temp.size(); srN++){
                    plotRS(SRVecMonojet_temp.at(srN).srHistMap, SRVecMonojet.at(srN).srHistMap, "sr"+SRVecMonojet.at(srN).GetName());
                    plotRS(SRVecMonojet_temp.at(srN).crRSInvertDPhiHistMap, SRVecMonojet.at(srN).crRSInvertDPhiHistMap, "crRSInvertDPhi"+SRVecMonojet.at(srN).GetName());
                }
                for(unsigned int srN = 0; srN < InclusiveRegions_temp.size(); srN++){
                    plotRS(InclusiveRegions_temp.at(srN).srHistMap, InclusiveRegions.at(srN).srHistMap, InclusiveRegions.at(srN).GetName());
                    plotRS(InclusiveRegions_temp.at(srN).crRSInvertDPhiHistMap, InclusiveRegions.at(srN).crRSInvertDPhiHistMap, "crRSInvertDPhi"+InclusiveRegions.at(srN).GetName());
                    plotRS(InclusiveRegions_temp.at(srN).crRSMT2SideBandHistMap, InclusiveRegions.at(srN).crRSMT2SideBandHistMap, "crRSMT2SideBand"+InclusiveRegions.at(srN).GetName());
                    plotRS(InclusiveRegions_temp.at(srN).crRSDPhiMT2HistMap, InclusiveRegions.at(srN).crRSDPhiMT2HistMap, "crRSDPhiMT2"+InclusiveRegions.at(srN).GetName());
                }

                //reset all histograms
                resetHistmap(SRNoCut_temp.srHistMap, SRNoCut_temp.GetName());
                resetHistmap(SRJustHT1_temp.srHistMap, SRJustHT1_temp.GetName());
                resetHistmap(SRJustHT2_temp.srHistMap, SRJustHT2_temp.GetName());
                resetHistmap(SRJustHT3_temp.srHistMap, SRJustHT3_temp.GetName());        
                resetHistmap(SRJustHT4_temp.srHistMap, SRJustHT4_temp.GetName());
                resetHistmap(SRJustHT5_temp.srHistMap, SRJustHT5_temp.GetName());
                resetHistmap(SRJustHT6_temp.srHistMap, SRJustHT6_temp.GetName());        
                resetHistmap(SRJustHT7_temp.srHistMap, SRJustHT7_temp.GetName());
                resetHistmap(SRJustHT8_temp.srHistMap, SRJustHT8_temp.GetName());
                resetHistmap(SRBase_temp.srHistMap, SRBase_temp.GetName());
                resetHistmap(SRBase_temp.crRSInvertDPhiHistMap, "crRSInvertDPhi"+SRBase_temp.GetName());
                resetHistmap(SRBase_temp.crRSMT2SideBandHistMap, "crRSMT2SideBand"+SRBase_temp.GetName());
                resetHistmap(SRBase_temp.crRSDPhiMT2HistMap, "crRSDPhiMT2"+SRBase_temp.GetName());
                for(unsigned int srN = 0; srN < SRVec_temp.size(); srN++){
                    resetHistmap(SRVec_temp.at(srN).srHistMap, SRVec_temp.at(srN).GetName());
                    resetHistmap(SRVec_temp.at(srN).crRSInvertDPhiHistMap, "crRSInvertDPhi"+SRVec_temp.at(srN).GetName());
                    resetHistmap(SRVec_temp.at(srN).crRSMT2SideBandHistMap, "crRSMT2SideBand"+SRVec_temp.at(srN).GetName());
                    resetHistmap(SRVec_temp.at(srN).crRSDPhiMT2HistMap, "crRSDPhiMT2"+SRVec_temp.at(srN).GetName());
                }
                for(unsigned int srN = 0; srN < SRVecMonojet_temp.size(); srN++){
                    resetHistmap(SRVecMonojet_temp.at(srN).srHistMap, SRVecMonojet_temp.at(srN).GetName());
                    resetHistmap(SRVecMonojet_temp.at(srN).crRSInvertDPhiHistMap, "crRSInvertDPhi"+SRVecMonojet_temp.at(srN).GetName());
                }
                for(unsigned int srN = 0; srN < InclusiveRegions_temp.size(); srN++){
                    resetHistmap(InclusiveRegions_temp.at(srN).srHistMap, InclusiveRegions_temp.at(srN).GetName());
                    resetHistmap(InclusiveRegions_temp.at(srN).crRSInvertDPhiHistMap, "crRSInvertDPhi"+InclusiveRegions_temp.at(srN).GetName());
                    resetHistmap(InclusiveRegions_temp.at(srN).crRSMT2SideBandHistMap, "crRSMT2SideBand"+InclusiveRegions_temp.at(srN).GetName());
                    resetHistmap(InclusiveRegions_temp.at(srN).crRSDPhiMT2HistMap, "crRSDPhiMT2"+InclusiveRegions_temp.at(srN).GetName());
                }
            }


            else {

                if(doHEMregionVeto && candidateHEMevent){
                    bool foundjet = false;
                    float actual_HEM_ptCut = HEM_ptCut;
                    if(t.nJet30==1 || (t.nlep!=2 && t.ht<575) || (t.nlep==2 && t.zll_ht<575))
                        actual_HEM_ptCut = 20.0;
                    for(int ijet=0; ijet<t.njet; ijet++){
                        if(t.jet_eta[ijet] < HEM_region[0] || t.jet_eta[ijet] > HEM_region[1]) continue;
                        if(t.jet_phi[ijet] < HEM_region[2] || t.jet_phi[ijet] > HEM_region[3]) continue;
                        if(t.jet_pt[ijet] < actual_HEM_ptCut) continue;
                        foundjet = true;
                        break;
                    }
                    if(foundjet) continue;
                }

                int lead_jet_idx = -1, sublead_jet_idx = -1;
                for(int i=0; i<t.njet; i++){
                    if(fabs(t.jet_eta[i]) > 2.4 || t.jet_pt[i] < 30)
                        continue;
                    if(lead_jet_idx == -1){
                        lead_jet_idx = i;
                    }else if(sublead_jet_idx == -1){
                        sublead_jet_idx = i;
                        break;
                    }
                }

                if(lead_jet_idx != -1){
                    jet1_eta_ = t.jet_eta[lead_jet_idx];
                    jet1_phi_ = t.jet_phi[lead_jet_idx];
                }
                if(sublead_jet_idx != -1){
                    jet2_eta_ = t.jet_eta[sublead_jet_idx];
                    jet2_phi_ = t.jet_phi[sublead_jet_idx];
                }
                

                fillHistos(SRNoCut.srHistMap, SRNoCut.GetNumberOfMT2Bins(), SRNoCut.GetMT2Bins(), SRNoCut.GetName(), "");
                if(t.nJet30>=2 && t.ht>=450 && t.ht<1200 && t.met_pt>30 && t.mt2>50)
                    fillHistos(SRJustHT1.srHistMap, SRJustHT1.GetNumberOfMT2Bins(), SRJustHT1.GetMT2Bins(), SRJustHT1.GetName(), "");
                if(t.nJet30>=2 && t.ht>=1200 && t.met_pt>30 && t.mt2>50)
                // if(t.nJet30>=2 && t.ht>=1200 && t.met_pt>30 && t.mt2>100 && t.diffMetMht/t.met_pt<0.5) 
                    fillHistos(SRJustHT2.srHistMap, SRJustHT2.GetNumberOfMT2Bins(), SRJustHT2.GetMT2Bins(), SRJustHT2.GetName(), "");
                if(t.nJet30>=2 && t.ht>=250 && t.ht<1200 && t.met_pt>30 && t.mt2>50)
                    fillHistos(SRJustHT3.srHistMap, SRJustHT3.GetNumberOfMT2Bins(), SRJustHT3.GetMT2Bins(), SRJustHT3.GetName(), "");
                if(t.nJet30>=2 && t.ht>=250 && t.ht<450 && t.met_pt>30 && t.mt2>50) 
                    fillHistos(SRJustHT4.srHistMap, SRJustHT4.GetNumberOfMT2Bins(), SRJustHT4.GetMT2Bins(), SRJustHT4.GetName(), "");
                if(t.nJet30>=2 && t.ht>=450 && t.ht<575 && t.met_pt>30 && t.mt2>50) 
                    fillHistos(SRJustHT5.srHistMap, SRJustHT5.GetNumberOfMT2Bins(), SRJustHT5.GetMT2Bins(), SRJustHT5.GetName(), "");
                if(t.nJet30>=2 && t.ht>=575 && t.ht<1200 && t.met_pt>30 && t.mt2>50) 
                    fillHistos(SRJustHT6.srHistMap, SRJustHT6.GetNumberOfMT2Bins(), SRJustHT6.GetMT2Bins(), SRJustHT6.GetName(), "");
                if(t.nJet30>=2 && t.ht>=1200 && t.ht<1500 && t.met_pt>30 && t.mt2>50) 
                    fillHistos(SRJustHT7.srHistMap, SRJustHT7.GetNumberOfMT2Bins(), SRJustHT7.GetMT2Bins(), SRJustHT7.GetName(), "");
                if(t.nJet30>=2 && t.ht>=1500 && t.met_pt>30 && t.mt2>50) 
                    fillHistos(SRJustHT8.srHistMap, SRJustHT8.GetNumberOfMT2Bins(), SRJustHT8.GetMT2Bins(), SRJustHT8.GetName(), "");                  
                fillHistosSignalRegion("sr");
                fillHistosSRBase();
                fillHistosInclusive();
                fillHistosCRRSInvertDPhi();
                fillHistosCRRSMT2SideBand();
                fillHistosCRRSDPhiMT2();
                if(doSLplots){
                    fillHistosCRSL();
                }
                if(doDYplots){
                    fillHistosCRDY();
                }
                if(doOFplots){
                    fillHistosCRDY("OF");
                    // cout << "FOUNDEVENT: OF1" << endl;                    
                }
                if(doLowPtSFplots) fillHistosCRDY("LowPtSF");
                if(doLowPtOFplots) fillHistosCRDY("LowPtOF");

            }

            if (makeSmearBaby_)
                FillBabyNtuple();


        }//end loop on events in a file

        delete tree;
        f.Close();
    }//end loop on files

    cout << "[SmearLooper::loop] processed " << nEventsTotal << " events" << endl;
    if ( nEventsChain != nEventsTotal ) {
        std::cout << "ERROR: number of events from files is not equal to total number of events" << std::endl;
    }

    cout << nDuplicates << " duplicate events were skipped." << endl;

    //---------------------
    // Save Plots
    //---------------------

    if (makeSmearBaby_)
        CloseBabyNtuple();
    
    outfile_->cd();
    savePlotsDir(h_1d_global,outfile_,"");
    savePlotsDir(SRNoCut.srHistMap,outfile_,SRNoCut.GetName().c_str());
    savePlotsDir(SRJustHT1.srHistMap,outfile_,SRJustHT1.GetName().c_str());
    savePlotsDir(SRJustHT2.srHistMap,outfile_,SRJustHT2.GetName().c_str());
    savePlotsDir(SRJustHT3.srHistMap,outfile_,SRJustHT3.GetName().c_str());  
    savePlotsDir(SRJustHT4.srHistMap,outfile_,SRJustHT4.GetName().c_str());
    savePlotsDir(SRJustHT5.srHistMap,outfile_,SRJustHT5.GetName().c_str());
    savePlotsDir(SRJustHT6.srHistMap,outfile_,SRJustHT6.GetName().c_str());  
    savePlotsDir(SRJustHT7.srHistMap,outfile_,SRJustHT7.GetName().c_str());
    savePlotsDir(SRJustHT8.srHistMap,outfile_,SRJustHT8.GetName().c_str());
    savePlotsDir(SRBase.srHistMap,outfile_,SRBase.GetName().c_str());
    savePlotsDir(SRBase.crRSInvertDPhiHistMap,outfile_,"crRSInvertDPhibase");
    savePlotsDir(SRBase.crRSMT2SideBandHistMap,outfile_,"crRSMT2SideBandbase");
    savePlotsDir(SRBase.crRSDPhiMT2HistMap,outfile_,"crRSDPhiMT2base");

    for(unsigned int srN = 0; srN < InclusiveRegions.size(); srN++){
        if(!InclusiveRegions.at(srN).srHistMap.empty()){
            savePlotsDir(InclusiveRegions.at(srN).srHistMap, outfile_, (InclusiveRegions.at(srN).GetName()).c_str());
        }
        if(!InclusiveRegions.at(srN).crRSInvertDPhiHistMap.empty()){
            savePlotsDir(InclusiveRegions.at(srN).crRSInvertDPhiHistMap, outfile_, ("crRSInvertDPhi"+InclusiveRegions.at(srN).GetName()).c_str());
        }
        if(!InclusiveRegions.at(srN).crRSMT2SideBandHistMap.empty()){
            savePlotsDir(InclusiveRegions.at(srN).crRSMT2SideBandHistMap, outfile_, ("crRSMT2SideBand"+InclusiveRegions.at(srN).GetName()).c_str());
        }
        if(!InclusiveRegions.at(srN).crRSDPhiMT2HistMap.empty()){
            savePlotsDir(InclusiveRegions.at(srN).crRSDPhiMT2HistMap, outfile_, ("crRSDPhiMT2"+InclusiveRegions.at(srN).GetName()).c_str());
        }
    }
    for(unsigned int srN = 0; srN < SRVec.size(); srN++){
        if(!SRVec.at(srN).srHistMap.empty()){
            savePlotsDir(SRVec.at(srN).srHistMap, outfile_, ("sr"+SRVec.at(srN).GetName()).c_str());
        }
        if(!SRVec.at(srN).crslHistMap.empty()){
            savePlotsDir(SRVec.at(srN).crslHistMap, outfile_, ("crsl"+SRVec.at(srN).GetName()).c_str());
        }
        if(!SRVec.at(srN).crdyHistMap.empty()){
            savePlotsDir(SRVec.at(srN).crdyHistMap, outfile_, ("crdy"+SRVec.at(srN).GetName()).c_str());
        }
        if(!SRVec.at(srN).crRSInvertDPhiHistMap.empty()){
            savePlotsDir(SRVec.at(srN).crRSInvertDPhiHistMap, outfile_, ("crRSInvertDPhi"+SRVec.at(srN).GetName()).c_str());
        }
        if(!SRVec.at(srN).crRSInvertDPhiSLHistMap.empty()){
            savePlotsDir(SRVec.at(srN).crRSInvertDPhiSLHistMap, outfile_, ("crRSInvertDPhiSL"+SRVec.at(srN).GetName()).c_str());
        }
        if(!SRVec.at(srN).crRSInvertDPhiDYHistMap.empty()){
            savePlotsDir(SRVec.at(srN).crRSInvertDPhiDYHistMap, outfile_, ("crRSInvertDPhiDY"+SRVec.at(srN).GetName()).c_str());
        }
        if(!SRVec.at(srN).crRSMT2SideBandSLHistMap.empty()){
            savePlotsDir(SRVec.at(srN).crRSMT2SideBandSLHistMap, outfile_, ("crRSMT2SideBandSL"+SRVec.at(srN).GetName()).c_str());
        }
        if(!SRVec.at(srN).crRSMT2SideBandDYHistMap.empty()){
            savePlotsDir(SRVec.at(srN).crRSMT2SideBandDYHistMap, outfile_, ("crRSMT2SideBandDY"+SRVec.at(srN).GetName()).c_str());
        }
        if(!SRVec.at(srN).crRSDPhiMT2HistMap.empty()){
            savePlotsDir(SRVec.at(srN).crRSDPhiMT2HistMap, outfile_, ("crRSDPhiMT2"+SRVec.at(srN).GetName()).c_str());
        }
        if(!SRVec.at(srN).crRSDPhiMT2SLHistMap.empty()){
            savePlotsDir(SRVec.at(srN).crRSDPhiMT2SLHistMap, outfile_, ("crRSDPhiMT2SL"+SRVec.at(srN).GetName()).c_str());
        }
        if(!SRVec.at(srN).crRSDPhiMT2DYHistMap.empty()){
            savePlotsDir(SRVec.at(srN).crRSDPhiMT2DYHistMap, outfile_, ("crRSDPhiMT2DY"+SRVec.at(srN).GetName()).c_str());
        }
    }
    for(unsigned int srN = 0; srN < SRVecMonojet.size(); srN++){
        if(!SRVecMonojet.at(srN).srHistMap.empty()){
            savePlotsDir(SRVecMonojet.at(srN).srHistMap, outfile_, ("sr"+SRVecMonojet.at(srN).GetName()).c_str());
        }
        if(!SRVecMonojet.at(srN).crRSInvertDPhiHistMap.empty()){
            savePlotsDir(SRVecMonojet.at(srN).crRSInvertDPhiHistMap, outfile_, ("crRSInvertDPhi"+SRVecMonojet.at(srN).GetName()).c_str());
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

void SmearLooper::fillHistosSRBase() {

    // trigger requirement on data
    if (!doRebalanceAndSmear_ && !passesTriggerSR()) return;
 
    std::map<std::string, float> values;
    if(doRebalanceAndSmear_){
        values["deltaPhiMin"] = RS_vars_["deltaPhiMin"];
        values["diffMetMhtOverMet"]  = RS_vars_["diffMetMht"]/RS_vars_["met_pt"];
        values["nlep"]        = nlepveto_;
        values["j1pt"]        = RS_vars_["jet1_pt"];
        values["j2pt"]        = RS_vars_["jet2_pt"];
        values["mt2"]         = RS_vars_["mt2"];
        values["passesHtMet"] = ( (RS_vars_["ht"] > 250. && RS_vars_["met_pt"] > 250.) || (RS_vars_["ht"] > 1200. && RS_vars_["met_pt"] > 30.) );//FIXME
        //values["passesHtMet"] = ( (RS_vars_["ht"] > 450. && RS_vars_["met_pt"] > 30.) || (RS_vars_["ht"] > 1200. && RS_vars_["met_pt"] > 30.) );
    }
    else{
        values["deltaPhiMin"] = t.deltaPhiMin;
        values["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
        values["nlep"]        = nlepveto_;
        values["j1pt"]        = t.jet1_pt;
        values["j2pt"]        = t.jet2_pt;
        values["mt2"]         = t.mt2;
        values["passesHtMet"] = ( (t.ht > 250. && t.met_pt > 250.) || (t.ht > 1200. && t.met_pt > 30.) );
    }

    if(!doRebalanceAndSmear_){
        if(SRBase.PassesSelection(values)){
            // cout << t.run << ":" << t.lumi << ":" << t.evt << endl;
            fillHistos(SRBase.srHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), SRBase.GetName(), "");
        }
        if(SRBase.PassesSelectionCRRSInvertDPhi(values)) fillHistos(SRBase.crRSInvertDPhiHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crRSInvertDPhi"+SRBase.GetName(), "");
        if(SRBase.PassesSelectionCRRSMT2SideBand(values)) fillHistos(SRBase.crRSMT2SideBandHistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crRSMT2SideBand"+SRBase.GetName(), "");
        if(SRBase.PassesSelectionCRRSDPhiMT2(values)) fillHistos(SRBase.crRSDPhiMT2HistMap, SRBase.GetNumberOfMT2Bins(), SRBase.GetMT2Bins(), "crRSDPhiMT2"+SRBase.GetName(), "");
    }
    else{
        if(SRBase_temp.PassesSelection(values)) fillHistos(SRBase_temp.srHistMap, SRBase_temp.GetNumberOfMT2Bins(), SRBase_temp.GetMT2Bins(), SRBase_temp.GetName(), "");
        if(SRBase_temp.PassesSelectionCRRSInvertDPhi(values)) fillHistos(SRBase_temp.crRSInvertDPhiHistMap, SRBase_temp.GetNumberOfMT2Bins(), SRBase_temp.GetMT2Bins(), "crRSInvertDPhi"+SRBase_temp.GetName(), "");
        if(SRBase_temp.PassesSelectionCRRSMT2SideBand(values)) fillHistos(SRBase_temp.crRSMT2SideBandHistMap, SRBase_temp.GetNumberOfMT2Bins(), SRBase_temp.GetMT2Bins(), "crRSMT2SideBand"+SRBase_temp.GetName(), "");
        if(SRBase_temp.PassesSelectionCRRSDPhiMT2(values)) fillHistos(SRBase_temp.crRSDPhiMT2HistMap, SRBase_temp.GetNumberOfMT2Bins(), SRBase_temp.GetMT2Bins(), "crRSDPhiMT2"+SRBase_temp.GetName(), "");
    }

    return;
}

void SmearLooper::fillHistosCRSL() {

    // trigger requirement on data
    if (!doRebalanceAndSmear_ && !passesTriggerSR()) return; 

    std::map<std::string, float> values;
    values["deltaPhiMin"] = t.deltaPhiMin;
    values["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
    values["nlep"]        = t.nLepLowMT;
    values["nbjets"]      = t.nBJet20;
    values["njets"]       = t.nJet30;
    values["j1pt"]        = t.jet1_pt;
    values["j2pt"]        = t.jet2_pt;
    values["mt2"]         = t.mt2;
    values["ht"]          = t.ht;
    values["met"]         = t.met_pt;
  
    for(unsigned int srN = 0; srN < SRVec.size(); srN++){
        if(SRVec.at(srN).PassesSelectionCRSL(values)){
            fillHistos(SRVec.at(srN).crslHistMap,    SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), "crsl"+SRVec.at(srN).GetName(), "");
        }
    }  
    values["nlep"] = 0; // the doSLplots flag in the main loop takes care of this.
    for(unsigned int srN = 0; srN < SRVec.size(); srN++){   
        if(SRVec.at(srN).PassesSelectionCRRSInvertDPhi(values)) fillHistos(SRVec.at(srN).crRSInvertDPhiSLHistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), "crRSInvertDPhiSL"+SRVec.at(srN).GetName(), "");
        if(SRVec.at(srN).PassesSelectionCRRSMT2SideBand(values)) fillHistos(SRVec.at(srN).crRSMT2SideBandSLHistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), "crRSMT2SideBandSL"+SRVec.at(srN).GetName(), "");
        if(SRVec.at(srN).PassesSelectionCRRSDPhiMT2(values)) fillHistos(SRVec.at(srN).crRSDPhiMT2SLHistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), "crRSDPhiMT2SL"+SRVec.at(srN).GetName(), "");
    }

    return;

}

void SmearLooper::fillHistosCRDY(const std::string& suffix) {

    std::map<std::string, float> values;
    values["deltaPhiMin"] = t.zll_deltaPhiMin;
    values["diffMetMhtOverMet"]  = t.zll_diffMetMht/t.zll_met_pt;
    values["nlep"]        = 0; //dummy value, taken care of by doDYplots flag
    values["nbjets"]      = t.nBJet20;
    values["njets"]       = t.nJet30;
    values["j1pt"]        = t.jet1_pt;
    values["j2pt"]        = t.jet2_pt;
    values["mt2"]         = t.zll_mt2;
    values["ht"]          = t.zll_ht;
    values["met"]         = t.zll_met_pt;
  
    for(unsigned int srN = 0; srN < SRVec.size(); srN++){
        if(SRVec.at(srN).PassesSelectionCRDY(values)){
            // if(suffix=="" && SRVec.at(srN).GetName()=="4L")
            //     cout << endl << "FOUNDEVENT: " << t.run << ":" << t.lumi << ":" << t.evt << endl;
            fillHistos(SRVec.at(srN).crdyHistMap,    SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), "crdy"+SRVec.at(srN).GetName(), suffix);
        }
        if(SRVec.at(srN).PassesSelectionCRRSInvertDPhi(values)){
            fillHistos(SRVec.at(srN).crRSInvertDPhiDYHistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), "crRSInvertDPhiDY"+SRVec.at(srN).GetName(), suffix);
            // if(suffix=="OF") cout << "FOUNDEVENT: OF2 " << SRVec.at(srN).GetName() << " " << t.nJet30 << " " << t.nBJet20 <<  endl;
        }
        if(SRVec.at(srN).PassesSelectionCRRSMT2SideBand(values)) fillHistos(SRVec.at(srN).crRSMT2SideBandDYHistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), "crRSMT2SideBandDY"+SRVec.at(srN).GetName(), suffix);
        if(SRVec.at(srN).PassesSelectionCRRSDPhiMT2(values)) fillHistos(SRVec.at(srN).crRSDPhiMT2DYHistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), "crRSDPhiMT2DY"+SRVec.at(srN).GetName(), suffix);
    }

    return;

}


void SmearLooper::fillHistosInclusive() {

    // trigger requirement on data
    if (!doRebalanceAndSmear_ && !passesTriggerSR()) return; 

    std::map<std::string, float> values;
    if(doRebalanceAndSmear_){
        values["deltaPhiMin"] = RS_vars_["deltaPhiMin"];
        values["diffMetMhtOverMet"]  = RS_vars_["diffMetMht"]/RS_vars_["met_pt"];
        values["nlep"]        = nlepveto_;
        values["j1pt"]        = RS_vars_["jet1_pt"];
        values["j2pt"]        = RS_vars_["jet2_pt"];
        values["mt2"]         = RS_vars_["mt2"];
        values["passesHtMet"] = ( (RS_vars_["ht"] > 250. && RS_vars_["met_pt"] > 250.) || (RS_vars_["ht"] > 1200. && RS_vars_["met_pt"] > 30.) );//FIXME
        //values["passesHtMet"] = ( (RS_vars_["ht"] > 450. && RS_vars_["met_pt"] > 30.) || (RS_vars_["ht"] > 1200. && RS_vars_["met_pt"] > 30.) );
    }
    else{
        values["deltaPhiMin"] = t.deltaPhiMin;
        values["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
        values["nlep"]        = nlepveto_;
        values["j1pt"]        = t.jet1_pt;
        values["j2pt"]        = t.jet2_pt;
        values["mt2"]         = t.mt2;
        values["passesHtMet"] = ( (t.ht > 250. && t.met_pt > 250.) || (t.ht > 1200. && t.met_pt > 30.) );
    }

    if(!doRebalanceAndSmear_){
        for(unsigned int srN = 0; srN < InclusiveRegions.size(); srN++){
            std::map<std::string, float> values_temp = values;
            std::vector<std::string> vars = InclusiveRegions.at(srN).GetListOfVariables();
            for(unsigned int iVar=0; iVar<vars.size(); iVar++){
                if(doRebalanceAndSmear_){
                    if(vars.at(iVar) == "ht") values_temp["ht"] = RS_vars_["ht"];
                    else if(vars.at(iVar) == "njets") values_temp["njets"] = RS_vars_["nJet30"];
                    else if(vars.at(iVar) == "nbjets") values_temp["nbjets"] = RS_vars_["nBJet20"];
                }
                else{
                    if(vars.at(iVar) == "ht") values_temp["ht"] = t.ht;
                    else if(vars.at(iVar) == "njets") values_temp["njets"] = t.nJet30;
                    else if(vars.at(iVar) == "nbjets") values_temp["nbjets"] = t.nBJet20;
                } 
            }
            if(InclusiveRegions.at(srN).PassesSelection(values_temp)){
                fillHistos(InclusiveRegions.at(srN).srHistMap, InclusiveRegions.at(srN).GetNumberOfMT2Bins(), InclusiveRegions.at(srN).GetMT2Bins(), InclusiveRegions.at(srN).GetName(), "");
            }
            if(InclusiveRegions.at(srN).PassesSelectionCRRSInvertDPhi(values_temp)){
                fillHistos(InclusiveRegions.at(srN).crRSInvertDPhiHistMap, InclusiveRegions.at(srN).GetNumberOfMT2Bins(), InclusiveRegions.at(srN).GetMT2Bins(), "crRSInvertDPhi"+InclusiveRegions.at(srN).GetName(), "");
            }
            if(InclusiveRegions.at(srN).PassesSelectionCRRSMT2SideBand(values_temp)){
                fillHistos(InclusiveRegions.at(srN).crRSMT2SideBandHistMap, InclusiveRegions.at(srN).GetNumberOfMT2Bins(), InclusiveRegions.at(srN).GetMT2Bins(), "crRSMT2SideBand"+InclusiveRegions.at(srN).GetName(), "");
            }
            if(InclusiveRegions.at(srN).PassesSelectionCRRSDPhiMT2(values_temp)){
                fillHistos(InclusiveRegions.at(srN).crRSDPhiMT2HistMap, InclusiveRegions.at(srN).GetNumberOfMT2Bins(), InclusiveRegions.at(srN).GetMT2Bins(), "crRSDPhiMT2"+InclusiveRegions.at(srN).GetName(), "");
            }
        }
    }
    else{
        for(unsigned int srN = 0; srN < InclusiveRegions_temp.size(); srN++){
            std::map<std::string, float> values_temp = values;
            std::vector<std::string> vars = InclusiveRegions_temp.at(srN).GetListOfVariables();
            for(unsigned int iVar=0; iVar<vars.size(); iVar++){
                if(doRebalanceAndSmear_){
                    if(vars.at(iVar) == "ht") values_temp["ht"] = RS_vars_["ht"];
                    else if(vars.at(iVar) == "njets") values_temp["njets"] = RS_vars_["nJet30"];
                    else if(vars.at(iVar) == "nbjets") values_temp["nbjets"] = RS_vars_["nBJet20"];
                }
                else{
                    if(vars.at(iVar) == "ht") values_temp["ht"] = t.ht;
                    else if(vars.at(iVar) == "njets") values_temp["njets"] = t.nJet30;
                    else if(vars.at(iVar) == "nbjets") values_temp["nbjets"] = t.nBJet20;
                }
            }
            if(InclusiveRegions_temp.at(srN).PassesSelection(values_temp)){
                fillHistos(InclusiveRegions_temp.at(srN).srHistMap, InclusiveRegions_temp.at(srN).GetNumberOfMT2Bins(), InclusiveRegions_temp.at(srN).GetMT2Bins(), InclusiveRegions_temp.at(srN).GetName(), "");
            }
            if(InclusiveRegions_temp.at(srN).PassesSelectionCRRSInvertDPhi(values_temp)){
                fillHistos(InclusiveRegions_temp.at(srN).crRSInvertDPhiHistMap, InclusiveRegions_temp.at(srN).GetNumberOfMT2Bins(), InclusiveRegions_temp.at(srN).GetMT2Bins(), "crRSInvertDPhi"+InclusiveRegions_temp.at(srN).GetName(), "");
            }
            if(InclusiveRegions_temp.at(srN).PassesSelectionCRRSMT2SideBand(values_temp)){
                fillHistos(InclusiveRegions_temp.at(srN).crRSMT2SideBandHistMap, InclusiveRegions_temp.at(srN).GetNumberOfMT2Bins(), InclusiveRegions_temp.at(srN).GetMT2Bins(), "crRSMT2SideBand"+InclusiveRegions_temp.at(srN).GetName(), "");
            }
            if(InclusiveRegions_temp.at(srN).PassesSelectionCRRSDPhiMT2(values_temp)){
                fillHistos(InclusiveRegions_temp.at(srN).crRSDPhiMT2HistMap, InclusiveRegions_temp.at(srN).GetNumberOfMT2Bins(), InclusiveRegions_temp.at(srN).GetMT2Bins(), "crRSDPhiMT2"+InclusiveRegions_temp.at(srN).GetName(), "");
            }
        }
    }

    return;
}

void SmearLooper::fillHistosSignalRegion(const std::string& prefix, const std::string& suffix) {

    // trigger requirement on data
    if (!doRebalanceAndSmear_ && !passesTriggerSR()) return; 

    std::map<std::string, float> values;

    if(doRebalanceAndSmear_){
        values["deltaPhiMin"] = RS_vars_["deltaPhiMin"];
        values["diffMetMhtOverMet"]  = RS_vars_["diffMetMht"]/RS_vars_["met_pt"];
        values["nlep"]        = nlepveto_;
        values["j1pt"]        = RS_vars_["jet1_pt"];
        values["j2pt"]        = RS_vars_["jet2_pt"];
        values["njets"]       = RS_vars_["nJet30"];
        values["nbjets"]      = RS_vars_["nBJet20"];
        values["mt2"]         = RS_vars_["mt2"];
        values["ht"]          = RS_vars_["ht"];
        values["met"]         = RS_vars_["met_pt"];
    }
    else{
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
    }


    if(!doRebalanceAndSmear_){
        for(unsigned int srN = 0; srN < SRVec.size(); srN++){
            if(SRVec.at(srN).PassesSelection(values)){
                fillHistos(SRVec.at(srN).srHistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix);
                // break;//signal regions are orthogonal, event cannot be in more than one
            }
        }
    }
    else{
        for(unsigned int srN = 0; srN < SRVec_temp.size(); srN++){
            if(SRVec_temp.at(srN).PassesSelection(values)){
                fillHistos(SRVec_temp.at(srN).srHistMap, SRVec_temp.at(srN).GetNumberOfMT2Bins(), SRVec_temp.at(srN).GetMT2Bins(), prefix+SRVec_temp.at(srN).GetName(), suffix);
                // break;//signal regions are orthogonal, event cannot be in more than one
            }
        }
    }

    if(passMonojetId_){

        std::map<std::string, float> valuesMonojet;

        if(doRebalanceAndSmear_){
            valuesMonojet["deltaPhiMin"] = RS_vars_["deltaPhiMin"];
            valuesMonojet["diffMetMhtOverMet"]  = RS_vars_["diffMetMht"]/RS_vars_["met_pt"];
            valuesMonojet["nlep"]        = nlepveto_;
            valuesMonojet["jet1_pt"]        = RS_vars_["jet1_pt"];
            valuesMonojet["njets"]       = RS_vars_["nJet30"];
            valuesMonojet["nbjets"]      = RS_vars_["nBJet20"];
            valuesMonojet["ht"]          = RS_vars_["ht"];
            valuesMonojet["met"]         = RS_vars_["met_pt"];
        }
        else{
            valuesMonojet["deltaPhiMin"] = t.deltaPhiMin;
            valuesMonojet["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
            valuesMonojet["nlep"]        = nlepveto_;
            valuesMonojet["jet1_pt"]        = t.jet1_pt;
            valuesMonojet["njets"]       = t.nJet30;
            valuesMonojet["nbjets"]      = t.nBJet20;
            valuesMonojet["ht"]          = t.ht;
            valuesMonojet["met"]         = t.met_pt;
        }

        if(!doRebalanceAndSmear_){
            for(unsigned int srN = 0; srN < SRVecMonojet.size(); srN++){
                if(SRVecMonojet.at(srN).PassesSelection(valuesMonojet)){
                    fillHistosMonojet(SRVecMonojet.at(srN).srHistMap, SRVecMonojet.at(srN).GetNumberOfMT2Bins(), SRVecMonojet.at(srN).GetMT2Bins(), prefix+SRVecMonojet.at(srN).GetName(), suffix);
                }
            }
        }
        else{
            for(unsigned int srN = 0; srN < SRVecMonojet_temp.size(); srN++){
                if(SRVecMonojet_temp.at(srN).PassesSelection(valuesMonojet)){
                    fillHistosMonojet(SRVecMonojet_temp.at(srN).srHistMap, SRVecMonojet_temp.at(srN).GetNumberOfMT2Bins(), SRVecMonojet_temp.at(srN).GetMT2Bins(), prefix+SRVecMonojet_temp.at(srN).GetName(), suffix);
                }
            }
        }
    }

    return;
}


// hists for R&S inverted deltaPhiMin region
void SmearLooper::fillHistosCRRSInvertDPhi(const std::string& prefix, const std::string& suffix) {

    // trigger requirement on data
    if (!doRebalanceAndSmear_ && !passesTriggerSR()) return; 
      
    std::map<std::string, float> values;
    if(doRebalanceAndSmear_){
        values["deltaPhiMin"] = RS_vars_["deltaPhiMin"];
        values["diffMetMhtOverMet"]  = RS_vars_["diffMetMht"]/RS_vars_["met_pt"];
        values["nlep"]        = nlepveto_;
        values["j1pt"]        = RS_vars_["jet1_pt"];
        values["j2pt"]        = RS_vars_["jet2_pt"];
        values["njets"]       = RS_vars_["nJet30"];
        values["nbjets"]      = RS_vars_["nBJet20"];
        values["mt2"]         = RS_vars_["mt2"];
        values["ht"]          = RS_vars_["ht"];
        values["met"]         = RS_vars_["met_pt"];
    }
    else{
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
    }

    if(!doRebalanceAndSmear_){
        for(unsigned int srN = 0; srN < SRVec.size(); srN++){
            if(SRVec.at(srN).PassesSelectionCRRSInvertDPhi(values)){
                fillHistos(SRVec.at(srN).crRSInvertDPhiHistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix);
                // break;
            }
        }
    }
    else{
        for(unsigned int srN = 0; srN < SRVec_temp.size(); srN++){
            if(SRVec_temp.at(srN).PassesSelectionCRRSInvertDPhi(values)){
                fillHistos(SRVec_temp.at(srN).crRSInvertDPhiHistMap, SRVec_temp.at(srN).GetNumberOfMT2Bins(), SRVec_temp.at(srN).GetMT2Bins(), prefix+SRVec_temp.at(srN).GetName(), suffix);
                // break;
            }
        }
    }

    if(passMonojetId_){

        std::map<std::string, float> valuesMonojet;

        if(doRebalanceAndSmear_){
            valuesMonojet["deltaPhiMin"] = RS_vars_["deltaPhiMin"];
            valuesMonojet["diffMetMhtOverMet"]  = RS_vars_["diffMetMht"]/RS_vars_["met_pt"];
            valuesMonojet["nlep"]        = nlepveto_;
            valuesMonojet["jet1_pt"]        = RS_vars_["jet1_pt"];
            valuesMonojet["jet2_pt"]        = RS_vars_["jet2_pt"];
            valuesMonojet["njets"]       = RS_vars_["nJet30"];
            valuesMonojet["nbjets"]      = RS_vars_["nBJet20"];
            valuesMonojet["ht"]          = RS_vars_["ht"];
            valuesMonojet["met"]         = RS_vars_["met_pt"];
        }
        else{
            valuesMonojet["deltaPhiMin"] = t.deltaPhiMin;
            valuesMonojet["diffMetMhtOverMet"]  = t.diffMetMht/t.met_pt;
            valuesMonojet["nlep"]        = nlepveto_;
            valuesMonojet["jet1_pt"]        = t.jet1_pt;
            valuesMonojet["jet2_pt"]        = t.jet2_pt;
            valuesMonojet["njets"]       = t.nJet30;
            valuesMonojet["nbjets"]      = t.nBJet20;
            valuesMonojet["ht"]          = t.ht;
            valuesMonojet["met"]         = t.met_pt;
        }

        if(!doRebalanceAndSmear_){
            for(unsigned int srN = 0; srN < SRVecMonojet.size(); srN++){
                if(SRVecMonojet.at(srN).PassesSelectionCRRSInvertDPhi(valuesMonojet)){
                    // if(SRVecMonojet.at(srN).GetName()=="baseJ")
                    //     cout << endl << "FOUNDEVENT: " << t.run << ":" << t.lumi << ":" << t.evt << " " << t.evt_id << endl;
                    fillHistosMonojet(SRVecMonojet.at(srN).crRSInvertDPhiHistMap, SRVecMonojet.at(srN).GetNumberOfMT2Bins(), SRVecMonojet.at(srN).GetMT2Bins(), prefix+SRVecMonojet.at(srN).GetName(), suffix);
                }
            }
        }
        else{
            for(unsigned int srN = 0; srN < SRVecMonojet_temp.size(); srN++){
                if(SRVecMonojet_temp.at(srN).PassesSelectionCRRSInvertDPhi(valuesMonojet)){
                    fillHistosMonojet(SRVecMonojet_temp.at(srN).crRSInvertDPhiHistMap, SRVecMonojet_temp.at(srN).GetNumberOfMT2Bins(), SRVecMonojet_temp.at(srN).GetMT2Bins(), prefix+SRVecMonojet_temp.at(srN).GetName(), suffix);
                }
            }
        }
    }

    return;
}

void SmearLooper::fillHistosCRRSMT2SideBand(const std::string& prefix, const std::string& suffix) {

    // trigger requirement on data
    if (!doRebalanceAndSmear_ && !passesTriggerSR()) return; 

    std::map<std::string, float> values;
    if(doRebalanceAndSmear_){
        values["deltaPhiMin"] = RS_vars_["deltaPhiMin"];
        values["diffMetMhtOverMet"]  = RS_vars_["diffMetMht"]/RS_vars_["met_pt"];
        values["nlep"]        = nlepveto_;
        values["j1pt"]        = RS_vars_["jet1_pt"];
        values["j2pt"]        = RS_vars_["jet2_pt"];
        values["njets"]       = RS_vars_["nJet30"];
        values["nbjets"]      = RS_vars_["nBJet20"];
        values["mt2"]         = RS_vars_["mt2"];
        values["ht"]          = RS_vars_["ht"];
        values["met"]         = RS_vars_["met_pt"];
    }
    else{
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
    }

    if(!doRebalanceAndSmear_){
        for(unsigned int srN = 0; srN < SRVec.size(); srN++){
            if(SRVec.at(srN).PassesSelectionCRRSMT2SideBand(values)){
                fillHistos(SRVec.at(srN).crRSMT2SideBandHistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix);
                // break;
            }
        }
    }
    else{
        for(unsigned int srN = 0; srN < SRVec_temp.size(); srN++){
            if(SRVec_temp.at(srN).PassesSelectionCRRSMT2SideBand(values)){
                fillHistos(SRVec_temp.at(srN).crRSMT2SideBandHistMap, SRVec_temp.at(srN).GetNumberOfMT2Bins(), SRVec_temp.at(srN).GetMT2Bins(), prefix+SRVec_temp.at(srN).GetName(), suffix);
                // break;
            }
        }
    }

    return;
}

void SmearLooper::fillHistosCRRSDPhiMT2(const std::string& prefix, const std::string& suffix) {

    // trigger requirement on data
    if (!doRebalanceAndSmear_ && !passesTriggerSR()) return; 

    std::map<std::string, float> values;
    if(doRebalanceAndSmear_){
        values["deltaPhiMin"] = RS_vars_["deltaPhiMin"];
        values["diffMetMhtOverMet"]  = RS_vars_["diffMetMht"]/RS_vars_["met_pt"];
        values["nlep"]        = nlepveto_;
        values["j1pt"]        = RS_vars_["jet1_pt"];
        values["j2pt"]        = RS_vars_["jet2_pt"];
        values["njets"]       = RS_vars_["nJet30"];
        values["nbjets"]      = RS_vars_["nBJet20"];
        values["mt2"]         = RS_vars_["mt2"];
        values["ht"]          = RS_vars_["ht"];
        values["met"]         = RS_vars_["met_pt"];
    }
    else{
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
    }

    if(!doRebalanceAndSmear_){
        for(unsigned int srN = 0; srN < SRVec.size(); srN++){
            if(SRVec.at(srN).PassesSelectionCRRSDPhiMT2(values)){
                fillHistos(SRVec.at(srN).crRSDPhiMT2HistMap, SRVec.at(srN).GetNumberOfMT2Bins(), SRVec.at(srN).GetMT2Bins(), prefix+SRVec.at(srN).GetName(), suffix);
                // break;
            }
        }
    }
    else{
        for(unsigned int srN = 0; srN < SRVec_temp.size(); srN++){
            if(SRVec_temp.at(srN).PassesSelectionCRRSDPhiMT2(values)){
                fillHistos(SRVec_temp.at(srN).crRSDPhiMT2HistMap, SRVec_temp.at(srN).GetNumberOfMT2Bins(), SRVec_temp.at(srN).GetMT2Bins(), prefix+SRVec_temp.at(srN).GetName(), suffix);
                // break;
            }
        }
    }

    return;
}

void SmearLooper::fillHistos(std::map<std::string, TH1*>& h_1d, int n_mt2bins, float* mt2bins, const std::string& dirname, const std::string& s) {
    TDirectory * dir = (TDirectory*)outfile_->Get(dirname.c_str());
    if (dir == 0) {
        dir = outfile_->mkdir(dirname.c_str());
    } 
    dir->cd();
    
    bool do_zll = false;
    if(dirname.find("DY") != string::npos || dirname.find("crdy") != string::npos)
        do_zll = true;
  
    if(doRebalanceAndSmear_){
        plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
        plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
        plot1D("h_mt2"+s,       RS_vars_["mt2"],   evtweight_, h_1d, "; M_{T2} [GeV]", 150, 0, 1500);
        plot1D("h_met"+s,       RS_vars_["met_pt"],   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 150, 0, 1500);
        plot1D("h_reb_met"+s,       RS_vars_["reb_met_pt"],   evtweight_, h_1d, ";Rebalanced E_{T}^{miss} [GeV]", 50, 0, 500);
        plot1D("h_mht"+s,       RS_vars_["mht_pt"],   evtweight_, h_1d, ";H_{T}^{miss} [GeV]", 150, 0, 1500);
        plot1D("h_ht"+s,       RS_vars_["ht"],   evtweight_, h_1d, ";H_{T} [GeV]", 120, 0, 3000);
        plot1D("h_nJet30"+s,       RS_vars_["nJet30"],   evtweight_, h_1d, ";N(jets)", 15, 0, 15);
        plot1D("h_nBJet20"+s,      RS_vars_["nBJet20"],   evtweight_, h_1d, ";N(bjets)", 6, 0, 6);
        plot1D("h_deltaPhiMin"+s,  RS_vars_["deltaPhiMin"],   evtweight_, h_1d, ";#Delta#phi_{min}", 96, 0, 3.2);
        plot1D("h_diffMetMht"+s,   RS_vars_["diffMetMht"],   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| [GeV]", 120, 0, 300);
        plot1D("h_diffMetMhtOverMet"+s,   RS_vars_["diffMetMht"]/RS_vars_["met_pt"],   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| / E_{T}^{miss}", 100, 0, 5.);
        plot1D("h_nlepveto"+s,     nlepveto_,   evtweight_, h_1d, ";N(leps)", 10, 0, 10);
        plot1D("h_J0pt"+s,       RS_vars_["jet1_pt"],   evtweight_, h_1d, ";p_{T}(jet1) [GeV]", 150, 0, 1500);
        plot1D("h_J1pt"+s,       RS_vars_["jet2_pt"],   evtweight_, h_1d, ";p_{T}(jet2) [GeV]", 150, 0, 1500);
        plot1D("h_J0eta"+s,       RS_vars_["jet1_eta"],   evtweight_, h_1d, ";eta(jet1) [GeV]", 100, -5, 5);
        plot1D("h_J1eta"+s,       RS_vars_["jet2_eta"],   evtweight_, h_1d, ";eta(jet2) [GeV]", 100, -5, 5);
        plot1D("h_J0phi"+s,       RS_vars_["jet1_phi"],   evtweight_, h_1d, ";phi(jet1) [GeV]", 100, -3.2, 3.2);
        plot1D("h_J1phi"+s,       RS_vars_["jet2_phi"],   evtweight_, h_1d, ";phi(jet2) [GeV]", 100, -3.2, 3.2);
        plot1D("h_mt2bins"+s,       RS_vars_["mt2"],   evtweight_, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
    }
    else{
        plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
        plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
        plot1D("h_nJet30"+s,       t.nJet30,   evtweight_, h_1d, ";N(jets)", 15, 0, 15);
        plot1D("h_nBJet20"+s,      t.nBJet20,   evtweight_, h_1d, ";N(bjets)", 6, 0, 6);
        plot1D("h_minMTBMet"+s,   t.minMTBMet,   evtweight_, h_1d, ";min M_{T}(b, E_{T}^{miss}) [GeV]", 150, 0, 1500);
        plot1D("h_nlepveto"+s,     nlepveto_,   evtweight_, h_1d, ";N(leps)", 10, 0, 10);
        plot1D("h_J0pt"+s,       t.jet1_pt,   evtweight_, h_1d, ";p_{T}(jet1) [GeV]", 150, 0, 1500);
        plot1D("h_J1pt"+s,       t.jet2_pt,   evtweight_, h_1d, ";p_{T}(jet2) [GeV]", 150, 0, 1500);
        plot1D("h_J0eta"+s,       jet1_eta_,   evtweight_, h_1d, ";eta(jet1) [GeV]", 100, -5, 5);
        plot1D("h_J1eta"+s,       jet2_eta_,   evtweight_, h_1d, ";eta(jet2) [GeV]", 100, -5, 5);
        plot1D("h_J0phi"+s,       jet1_phi_,   evtweight_, h_1d, ";phi(jet1) [GeV]", 100, -3.2, 3.2);
        plot1D("h_J1phi"+s,       jet2_phi_,   evtweight_, h_1d, ";phi(jet2) [GeV]", 100, -3.2, 3.2);
        if(!do_zll){
            plot1D("h_mt2"+s,       t.mt2,   evtweight_, h_1d, "; M_{T2} [GeV]", 150, 0, 1500);
            plot1D("h_met"+s,       t.met_pt,   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 150, 0, 1500);
            plot1D("h_ht"+s,       t.ht,   evtweight_, h_1d, ";H_{T} [GeV]", 120, 0, 3000);
            plot1D("h_mt2bins"+s,       t.mt2,   evtweight_, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
            plot1D("h_deltaPhiMin"+s,  t.deltaPhiMin,   evtweight_, h_1d, ";#Delta#phi_{min}", 96, 0, 3.2);
            plot1D("h_diffMetMht"+s,   t.diffMetMht,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| [GeV]", 120, 0, 300);
            plot1D("h_diffMetMhtOverMet"+s,   t.diffMetMht/t.met_pt,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| / E_{T}^{miss}", 100, 0, 5.);
        }else{
            plot1D("h_mt2"+s,       t.zll_mt2,   evtweight_, h_1d, "; M_{T2} [GeV]", 150, 0, 1500);
            plot1D("h_met"+s,       t.zll_met_pt,   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 150, 0, 1500);
            plot1D("h_ht"+s,       t.zll_ht,   evtweight_, h_1d, ";H_{T} [GeV]", 120, 0, 3000);
            plot1D("h_mt2bins"+s,       t.zll_mt2,   evtweight_, h_1d, "; M_{T2} [GeV]", n_mt2bins, mt2bins);
            plot1D("h_deltaPhiMin"+s,  t.zll_deltaPhiMin,   evtweight_, h_1d, ";#Delta#phi_{min}", 96, 0, 3.2);
            plot1D("h_diffMetMht"+s,   t.zll_diffMetMht,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| [GeV]", 120, 0, 300);
            plot1D("h_diffMetMhtOverMet"+s,   t.zll_diffMetMht/t.zll_met_pt,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| / E_{T}^{miss}", 100, 0, 5.);
        }


        if (isSignal_) {
            plot3D("h_mt2bins_sigscan"+s, t.GenSusyMScan1, t.GenSusyMScan2, t.mt2, evtweight_, h_1d, "mass1 [GeV];mass2 [GeV];M_{T2} [GeV]", n_m1bins, m1bins, n_m2bins, m2bins, n_mt2bins, mt2bins);
        }

        if (!t.isData && applyWeights_ && doSystVariationPlots) {
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
    } 

    outfile_->cd();
    return;
}

void SmearLooper::fillHistosMonojet(std::map<std::string, TH1*>& h_1d, int n_htbins, float* htbins, const std::string& dirname, const std::string& s) {
    TDirectory * dir = (TDirectory*)outfile_->Get(dirname.c_str());
    if (dir == 0) {
        dir = outfile_->mkdir(dirname.c_str());
    } 
    dir->cd();
  
    if(doRebalanceAndSmear_){
        plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
        plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
        plot1D("h_met"+s,       RS_vars_["met_pt"],   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 150, 0, 1500);
        plot1D("h_reb_met"+s,       RS_vars_["reb_met_pt"],   evtweight_, h_1d, ";Rebalanced E_{T}^{miss} [GeV]", 50, 0, 500);
        plot1D("h_mht"+s,       RS_vars_["mht_pt"],   evtweight_, h_1d, ";H_{T}^{miss} [GeV]", 150, 0, 1500);
        plot1D("h_ht"+s,       RS_vars_["ht"],   evtweight_, h_1d, ";H_{T} [GeV]", 100, 0, 2000);
        plot1D("h_nJet30"+s,       RS_vars_["nJet30"],   evtweight_, h_1d, ";N(jets)", 15, 0, 15);
        plot1D("h_nBJet20"+s,      RS_vars_["nBJet20"],   evtweight_, h_1d, ";N(bjets)", 6, 0, 6);
        plot1D("h_deltaPhiMin"+s,  RS_vars_["deltaPhiMin"],   evtweight_, h_1d, ";#Delta#phi_{min}", 96, 0, 3.2);
        plot1D("h_diffMetMht"+s,   RS_vars_["diffMetMht"],   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| [GeV]", 120, 0, 300);
        plot1D("h_diffMetMhtOverMet"+s,   RS_vars_["diffMetMht"]/RS_vars_["met_pt"],   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| / E_{T}^{miss}", 100, 0, 5.);
        plot1D("h_nlepveto"+s,     nlepveto_,   evtweight_, h_1d, ";N(leps)", 10, 0, 10);
        plot1D("h_J0pt"+s,       RS_vars_["jet1_pt"],   evtweight_, h_1d, ";p_{T}(jet1) [GeV]", 150, 0, 1500);
        plot1D("h_J1pt"+s,       RS_vars_["jet2_pt"],   evtweight_, h_1d, ";p_{T}(jet2) [GeV]", 150, 0, 1500);
        plot1D("h_J0eta"+s,       RS_vars_["jet1_eta"],   evtweight_, h_1d, ";eta(jet1) [GeV]", 100, -5, 5);
        plot1D("h_J1eta"+s,       RS_vars_["jet2_eta"],   evtweight_, h_1d, ";eta(jet2) [GeV]", 100, -5, 5);
        plot1D("h_J0phi"+s,       RS_vars_["jet1_phi"],   evtweight_, h_1d, ";phi(jet1) [GeV]", 100, -3.2, 3.2);
        plot1D("h_J1phi"+s,       RS_vars_["jet2_phi"],   evtweight_, h_1d, ";phi(jet2) [GeV]", 100, -3.2, 3.2);
        plot1D("h_htbins"+s,       RS_vars_["ht"],   evtweight_, h_1d, "; H_{T} [GeV]", n_htbins, htbins);
        if(RS_vars_["nJet30"]==2 && RS_vars_["jet2_pt"]>30 && RS_vars_["jet2_pt"]<60)
            plot1D("h_htbins_jet2pt_30_60"+s,       RS_vars_["ht"],   evtweight_, h_1d, "; H_{T} [GeV]", n_htbins, htbins);
        plot1D("h_J0ptbins"+s,       RS_vars_["jet1_pt"],   evtweight_, h_1d, "; p_{T}(jet1) [GeV]", n_htbins, htbins);
        if(RS_vars_["nJet30"]==2 && RS_vars_["jet2_pt"]>30 && RS_vars_["jet2_pt"]<60)
            plot1D("h_J0ptbins_jet2pt_30_60"+s,       RS_vars_["jet1_pt"],   evtweight_, h_1d, "; p_{T}(jet1) [GeV]", n_htbins, htbins);
    }
    else{
        plot1D("h_Events"+s,  1, 1, h_1d, ";Events, Unweighted", 1, 0, 2);
        plot1D("h_Events_w"+s,  1,   evtweight_, h_1d, ";Events, Weighted", 1, 0, 2);
        plot1D("h_met"+s,       t.met_pt,   evtweight_, h_1d, ";E_{T}^{miss} [GeV]", 150, 0, 1500);
        plot1D("h_ht"+s,       t.ht,   evtweight_, h_1d, ";H_{T} [GeV]", 100, 0, 2000);
        plot1D("h_nJet30"+s,       t.nJet30,   evtweight_, h_1d, ";N(jets)", 15, 0, 15);
        plot1D("h_nBJet20"+s,      t.nBJet20,   evtweight_, h_1d, ";N(bjets)", 6, 0, 6);
        plot1D("h_deltaPhiMin"+s,  t.deltaPhiMin,   evtweight_, h_1d, ";#Delta#phi_{min}", 96, 0, 3.2);
        plot1D("h_diffMetMht"+s,   t.diffMetMht,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| [GeV]", 120, 0, 300);
        plot1D("h_diffMetMhtOverMet"+s,   t.diffMetMht/t.met_pt,   evtweight_, h_1d, ";|E_{T}^{miss} - MHT| / E_{T}^{miss}", 100, 0, 5.);
        plot1D("h_nlepveto"+s,     nlepveto_,   evtweight_, h_1d, ";N(leps)", 10, 0, 10);
        plot1D("h_J0pt"+s,       t.jet1_pt,   evtweight_, h_1d, ";p_{T}(jet1) [GeV]", 150, 0, 1500);
        plot1D("h_J1pt"+s,       t.jet2_pt,   evtweight_, h_1d, ";p_{T}(jet2) [GeV]", 150, 0, 1500);
        plot1D("h_J0eta"+s,       jet1_eta_,   evtweight_, h_1d, ";eta(jet1) [GeV]", 100, -5, 5);
        plot1D("h_J1eta"+s,       jet2_eta_,   evtweight_, h_1d, ";eta(jet2) [GeV]", 100, -5, 5);
        plot1D("h_J0phi"+s,       jet1_phi_,   evtweight_, h_1d, ";phi(jet1) [GeV]", 100, -3.2, 3.2);
        plot1D("h_J1phi"+s,       jet2_phi_,   evtweight_, h_1d, ";phi(jet2) [GeV]", 100, -3.2, 3.2);
        plot1D("h_htbins"+s,       t.ht,   evtweight_, h_1d, "; H_{T} [GeV]", n_htbins, htbins);
        if(t.nJet30==2 && t.jet2_pt>30 && t.jet2_pt<60)
            plot1D("h_htbins_jet2pt_30_60"+s,       t.ht,   evtweight_, h_1d, "; H_{T} [GeV]", n_htbins, htbins);
        plot1D("h_J0ptbins"+s,       t.jet1_pt,   evtweight_, h_1d, "; p_{T}(jet1) [GeV]", n_htbins, htbins);
        if(t.nJet30==2 && t.jet2_pt>30 && t.jet2_pt<60)
            plot1D("h_J0ptbins_jet2pt_30_60"+s,       t.jet1_pt,   evtweight_, h_1d, "; p_{T}(jet1) [GeV]", n_htbins, htbins);

    } 

    outfile_->cd();
    return;
}

void SmearLooper::plotRS(std::map<string, TH1*> &inmap, std::map<string, TH1*> &outmap, std::string outdir){
    TDirectory * dir = (TDirectory*)outfile_->Get(outdir.c_str());
    if (dir == 0) {
        dir = outfile_->mkdir(outdir.c_str());
    } 
    for(std::map<string, TH1*>::iterator in_iter = inmap.begin(); in_iter != inmap.end(); in_iter++){

        std::string name = in_iter->second->GetName(); 
        if(name.find("_LOW") != std::string::npos) continue;
        if(name.find("_HI") != std::string::npos) continue;
        if(name.find("n_mt2bins") != std::string::npos) continue;

        std::map<string, TH1*>::iterator find_iter= outmap.find(name);
        if(find_iter == outmap.end()){
            TH1D *currentHisto = (TH1D*)in_iter->second->Clone(name.c_str());
            currentHisto->Reset();
            outmap.insert(std::pair<string, TH1D*> (name,currentHisto) );
        }

        int numbinsx = in_iter->second->GetNbinsX();
        float xmin = in_iter->second->GetXaxis()->GetXmin();
        float xmax = in_iter->second->GetXaxis()->GetXmax();
        for(int i=0; i<=numbinsx+1; i++){
            if(in_iter->second->GetBinContent(i) == 0) continue;
            float xval = in_iter->second->GetBinCenter(i);
            float weight = (in_iter->second->GetBinContent(i)) * smearNormalization;
            if(!t.isData) weight *= evtweight_;
            if (t.isData) weight *= prescale_correction;
            dir->cd();
            //std::cout << i << std::endl;
            //std::cout << in_iter->second->GetBinContent(i) << std::endl;
            //std::cout << xval << std::endl;
            //std::cout << weight << std::endl;
            plot1D(name, xval, weight, outmap, name, numbinsx, xmin, xmax);
        }
    }
    outfile_->cd();
    return;
}

void SmearLooper::resetHistmap(std::map<string, TH1*> &inmap, std::string outdir){
    TDirectory * dir = (TDirectory*)outfile_->Get(outdir.c_str());
    if (dir == 0) {
        dir = outfile_->mkdir(outdir.c_str());
    } 
    dir->cd();
    for(std::map<string, TH1*>::iterator in_iter = inmap.begin(); in_iter != inmap.end(); in_iter++){
        in_iter->second->Reset();
    }
    outfile_->cd();
    return;
}

void SmearLooper::MakeBabyNtuple(const char *SmearBabyFilename) {
    SmearBabyFile_ = new TFile(Form("%s", SmearBabyFilename), "RECREATE");
    SmearBabyFile_->cd();
    SmearBabyTree_ = new TTree("mt2", "A Smear Baby Ntuple");

    SmearBabyTree_->Branch("run"                  , &run                  );
    SmearBabyTree_->Branch("ls"                   , &ls                   );
    SmearBabyTree_->Branch("evt"                  , &evt                  );
    SmearBabyTree_->Branch("isData"               , &isData               );
    SmearBabyTree_->Branch("evt_scale1fb"         , &evt_scale1fb         );
    SmearBabyTree_->Branch("evt_id"               , &evt_id               );  
    SmearBabyTree_->Branch("HLT_PFHT1050"          , &HLT_PFHT1050          );   
    SmearBabyTree_->Branch("HLT_PFJet500"         , &HLT_PFJet500         );   
    SmearBabyTree_->Branch("HLT_PFHT180_Prescale" , &HLT_PFHT180_Prescale );   
    SmearBabyTree_->Branch("HLT_PFHT250_Prescale" , &HLT_PFHT250_Prescale );   
    SmearBabyTree_->Branch("HLT_PFHT370_Prescale" , &HLT_PFHT370_Prescale );   
    SmearBabyTree_->Branch("HLT_PFHT430_Prescale" , &HLT_PFHT430_Prescale );   
    SmearBabyTree_->Branch("HLT_PFHT510_Prescale" , &HLT_PFHT510_Prescale );   
    SmearBabyTree_->Branch("HLT_PFHT590_Prescale" , &HLT_PFHT590_Prescale );   
    SmearBabyTree_->Branch("HLT_PFHT680_Prescale" , &HLT_PFHT680_Prescale );   
    SmearBabyTree_->Branch("HLT_PFHT780_Prescale" , &HLT_PFHT780_Prescale );   
    SmearBabyTree_->Branch("HLT_PFHT890_Prescale" , &HLT_PFHT890_Prescale );   
    SmearBabyTree_->Branch("a_nJet30"             , &a_nJet30             );
    SmearBabyTree_->Branch("a_nBJet20"            , &a_nBJet20            );
    SmearBabyTree_->Branch("a_deltaPhiMin"        , &a_deltaPhiMin        );
    SmearBabyTree_->Branch("a_diffMetMhtOverMet"  , &a_diffMetMhtOverMet  );
    SmearBabyTree_->Branch("a_ht"                 , &a_ht                 );
    SmearBabyTree_->Branch("a_mt2"                , &a_mt2                );
    SmearBabyTree_->Branch("a_met_pt"             , &a_met_pt             );
    SmearBabyTree_->Branch("a_met_phi"            , &a_met_phi            );  

    if (doRebalanceAndSmear_) {
        SmearBabyTree_->Branch("r_nJet"              , &r_nJet              );
        SmearBabyTree_->Branch("r_ht"                , &r_ht                );
        SmearBabyTree_->Branch("r_met_pt"            , &r_met_pt            );
        SmearBabyTree_->Branch("r_met_phi"           , &r_met_phi           );      
        SmearBabyTree_->Branch("r_jet1_pt"           , &r_jet1_pt           );      
        SmearBabyTree_->Branch("r_jet1_eta"          , &r_jet1_eta          );      
        SmearBabyTree_->Branch("r_jet2_pt"           , &r_jet2_pt           );      
        SmearBabyTree_->Branch("r_jet2_eta"          , &r_jet2_eta          );      
        SmearBabyTree_->Branch("s_prescale"          , &s_prescale          );              
        SmearBabyTree_->Branch("s_nsmears"           , &s_nsmears           , "s_nsmears/I"                      );
        SmearBabyTree_->Branch("s_nJet30"            , &s_nJet30            , "s_nJet30[s_nsmears]/I"            );
        SmearBabyTree_->Branch("s_nBJet20"           , &s_nBJet20           , "s_nBJet20[s_nsmears]/I"           );
        SmearBabyTree_->Branch("s_deltaPhiMin"       , &s_deltaPhiMin       , "s_deltaPhiMin[s_nsmears]/F"       );
        SmearBabyTree_->Branch("s_diffMetMhtOverMet" , &s_diffMetMhtOverMet , "s_diffMetMhtOverMet[s_nsmears]/F" );
        SmearBabyTree_->Branch("s_jet1_pt"           , &s_jet1_pt           , "s_jet1_pt[s_nsmears]/F"                );
        SmearBabyTree_->Branch("s_jet1_eta"          , &s_jet1_eta          , "s_jet1_eta[s_nsmears]/F"                );
        SmearBabyTree_->Branch("s_jet1_smearfact"    , &s_jet1_smearfact    , "s_jet1_smearfact[s_nsmears]/F"                );
        SmearBabyTree_->Branch("s_jet2_smearfact"    , &s_jet2_smearfact    , "s_jet2_smearfact[s_nsmears]/F"                );
        SmearBabyTree_->Branch("s_max_smearfact"     , &s_max_smearfact     , "s_max_smearfact[s_nsmears]/F"                );
        SmearBabyTree_->Branch("s_jet2_pt"           , &s_jet2_pt           , "s_jet2_pt[s_nsmears]/F"                );
        SmearBabyTree_->Branch("s_jet2_eta"          , &s_jet2_eta          , "s_jet2_eta[s_nsmears]/F"                );
        SmearBabyTree_->Branch("s_ht"                , &s_ht                , "s_ht[s_nsmears]/F"                );
        SmearBabyTree_->Branch("s_mt2"               , &s_mt2               , "s_mt2[s_nsmears]/F"               );
        SmearBabyTree_->Branch("s_met_pt"            , &s_met_pt            , "s_met_pt[s_nsmears]/F"            );
    }
    return;
}

void SmearLooper::InitBabyNtuple () {
    evt                  = 0;  
    run                  = -999;
    ls                   = -999;
    isData               = 0;
    evt_id               = -999;  
    HLT_PFHT1050          = 0;   
    HLT_PFJet500         = 0;   
    HLT_PFHT180_Prescale = 0;   
    HLT_PFHT250_Prescale = 0;   
    HLT_PFHT370_Prescale = 0;   
    HLT_PFHT430_Prescale = 0;   
    HLT_PFHT510_Prescale = 0;   
    HLT_PFHT590_Prescale = 0;   
    HLT_PFHT680_Prescale = 0;   
    HLT_PFHT780_Prescale = 0;   
    HLT_PFHT890_Prescale = 0;   
    a_nJet30             = -999;
    a_nBJet20            = -999;  
    evt_scale1fb         = -999;
    a_deltaPhiMin        = -999;
    a_diffMetMhtOverMet  = -999;
    a_ht                 = -999;
    a_mt2                = -999;
    a_met_pt             = -999;
    a_met_phi            = -999;  

    if (doRebalanceAndSmear_) {
        r_nJet     = -999;
        r_ht       = -999;
        r_met_pt   = -999;
        r_met_phi  = -999;  
        r_jet1_pt  = -999;  
        r_jet1_eta  = -999;  
        r_jet2_pt  = -999;  
        r_jet2_eta  = -999;  
        s_nsmears  = -999;
        s_prescale = -999;            
        for (unsigned int i = 0; i < max_nsmear; i++) {
            s_nJet30[i]            = -999;
            s_nBJet20[i]           = -999;
            s_deltaPhiMin[i]       = -999;
            s_diffMetMhtOverMet[i] = -999;
            s_ht[i]                = -999;
            s_jet1_pt[i]           = -999;
            s_jet1_eta[i]          = -999;
            s_jet1_smearfact[i]    = -999;
            s_jet2_smearfact[i]    = -999;
            s_jet2_pt[i]           = -999;
            s_jet2_eta[i]          = -999;
            s_mt2[i]               = -999;
            s_met_pt[i]            = -999;
            s_max_smearfact[i]     = -999;
        }
    }
    return;
}

void SmearLooper::FillBabyNtuple () {
    SmearBabyTree_->Fill();
    return;
}

void SmearLooper::CloseBabyNtuple () {
    SmearBabyFile_->cd();
    SmearBabyTree_->Write();
    SmearBabyFile_->Close();
    return;
}

bool SmearLooper::passesTrigger() {
    //
    // We should only use each trigger in a certain HT range.
    // Otherwise, we will double count the data contributions to the r&s estimate.
    //
    // HLT_PFHT900 for ht >= 1000
    // HLT_PFHT600 for 700 <= ht < 1000
    // HLT_PFHT475 for 575 <= ht < 700
    // HLT_PFHT350 for 450 <= ht < 575
    // HLT_PFHT300 for 400 <= ht < 450
    // HLT_PFHT200 for 300 <= ht < 400
    // HLT_PFHT125 for 250 <= ht < 300

    if (not t.isData) return true;

    // // 2016
    // if(config_tag_.find("2016") != string::npos){
    //     if (t.ht < 300)
    //         return t.HLT_PFHT125_Prescale;
    //     else if (t.ht < 400)
    //         return t.HLT_PFHT200_Prescale;
    //     else if (t.ht < 450)
    //         return t.HLT_PFHT300_Prescale;
    //     else if (t.ht < 575)
    //         return t.HLT_PFHT350_Prescale;
    //     else if (t.ht < 700)
    //         return (t.HLT_PFHT475_Prescale or t.HLT_PFJet450);
    //     else if (t.ht < 1000)
    //         return (t.HLT_PFHT600_Prescale or t.HLT_PFJet450);
    //     else
    //         return (t.HLT_PFHT900 or t.HLT_PFJet450);
    // }else{
    //     // 2017
    //     if(t.ht < 250)
    //         return false;
    //     if(t.ht >= 250 && t.ht < 340)
    //         return t.HLT_PFHT180_Prescale;
    //     if(t.ht >= 340 && t.ht < 480)
    //         return t.HLT_PFHT250_Prescale;
    //     if(t.ht >= 480 && t.ht < 540)
    //         return t.HLT_PFHT370_Prescale;
    //     if(t.ht >= 540 && t.ht < 640)
    //         return t.HLT_PFHT430_Prescale;
    //     if(t.ht >= 640 && t.ht < 720)
    //         return t.HLT_PFHT510_Prescale || t.HLT_PFJet500;
    //     if(t.ht >= 720 && t.ht < 820)
    //         return t.HLT_PFHT590_Prescale || t.HLT_PFJet500;
    //     if(t.ht >= 820 && t.ht < 940)
    //         return t.HLT_PFHT680_Prescale || t.HLT_PFJet500;
    //     if(t.ht >= 940 && t.ht < 1040)
    //         return t.HLT_PFHT780_Prescale || t.HLT_PFJet500;
    //     if(t.ht >= 1040 && t.ht < 1200)
    //         return t.HLT_PFHT890_Prescale || t.HLT_PFJet500;
    //     if(t.ht >= 1200)
    //         return t.HLT_PFHT1050 || t.HLT_PFJet500;
    // }


    // 2016
    if(config_tag_.find("2016") != string::npos){
        if (t.ht < 300)
            return t.HLT_PFHT125_Prescale;
        else if (t.ht < 400)
            return t.HLT_PFHT200_Prescale;
        else if (t.ht < 450)
            return t.HLT_PFHT300_Prescale;
        else if (t.ht < 575)
            return t.HLT_PFHT350_Prescale;
        else if (t.ht < 700)
            return (t.HLT_PFHT475_Prescale or t.HLT_PFJet450);
        else if (t.ht < 1000)
            return (t.HLT_PFHT600_Prescale or t.HLT_PFJet450);
        else
            return (t.HLT_PFHT900 or t.HLT_PFJet450);
    }else{
        // 2017
        if(t.ht < 250)
            return false;
        if(t.ht >= 250 && t.ht < 440)
            return t.HLT_PFHT180_Prescale;
        if(t.ht >= 440 && t.ht < 520)
            return t.HLT_PFHT370_Prescale;
        if(t.ht >= 520 && t.ht < 620)
            return t.HLT_PFHT430_Prescale;
        if(t.ht >= 620 && t.ht < 700)
            return t.HLT_PFHT510_Prescale || t.HLT_PFJet500;
        if(t.ht >= 700 && t.ht < 800)
            return t.HLT_PFHT590_Prescale || t.HLT_PFJet500;
        if(t.ht >= 800 && t.ht < 900)
            return t.HLT_PFHT680_Prescale || t.HLT_PFJet500;
        if(t.ht >= 900 && t.ht < 1000)
            return t.HLT_PFHT780_Prescale || t.HLT_PFJet500;
        if(t.ht >= 1000 && t.ht < 1200)
            return t.HLT_PFHT890_Prescale || t.HLT_PFJet500;
        if(t.ht >= 1200)
            return t.HLT_PFHT1050 || t.HLT_PFJet500;
    }

    return false;
}

// for full year
float SmearLooper::getTriggerPrescale () {

    if (not t.isData) return 1;

    // if(config_tag_.find("2016") != string::npos){
    //     if(!prescalesByEvent){
    //         if (t.ht < 250)
    //             return -1;
    //         else if (t.ht < 300)
    //             return 9421;
    //         else if (t.ht < 400)
    //             return 5410;
    //         else if (t.ht < 450)
    //             return 917;
    //         else if (t.ht < 575)
    //             return 458;
    //         else if(t.HLT_PFJet450)
    //             return 1;
    //         else if (t.ht < 700)
    //             return 115;
    //         else if (t.ht < 1000)
    //             return 29;
    //         else
    //             return 1;
    //     }else{
    //         if (t.ht < 250)
    //             return -1;
    //         else if (t.ht < 300)
    //             return t.HLT_PFHT125_Prescale;
    //         else if (t.ht < 400)
    //             return t.HLT_PFHT200_Prescale;
    //         else if (t.ht < 450)
    //             return t.HLT_PFHT300_Prescale;
    //         else if (t.ht < 575)
    //             return t.HLT_PFHT350_Prescale;
    //         else if(t.HLT_PFJet450)
    //             return 1;
    //         else if (t.ht < 700)
    //             return t.HLT_PFHT475_Prescale;
    //         else if (t.ht < 1000)
    //             return t.HLT_PFHT600_Prescale;
    //         else
    //             return 1;
    //     }
    // }else if(config_tag_.find("2017") != string::npos){
    //     if(!prescalesByEvent){
    //         if(t.ht < 250)
    //             return -1;
    //         else if(t.ht >= 250 && t.ht < 340)
    //             return 3981.;
    //         else if(t.ht >= 340 && t.ht < 480)
    //             return 2760.;
    //         else if(t.ht >= 480 && t.ht < 540)
    //             return 863.3;
    //         else if(t.ht >= 540 && t.ht < 640)
    //             return 309.6;
    //         else if(t.HLT_PFJet500)
    //             return 1;
    //         else if(t.ht >= 640 && t.ht < 720)
    //             return 190.0;
    //         else if(t.ht >= 720 && t.ht < 820)
    //             return 94.8;
    //         else if(t.ht >= 820 && t.ht < 940)
    //             return 53.1;
    //         else if(t.ht >= 940 && t.ht < 1040)
    //             return 28.5;
    //         else if(t.ht >= 1040 && t.ht < 1200)
    //             return 14.8;
    //         else
    //             return 1;
    //     }else{
    //         if(t.ht < 250)
    //             return -1;
    //         else if(t.ht >= 250 && t.ht < 340)
    //             return t.HLT_PFHT180_Prescale;
    //         else if(t.ht >= 340 && t.ht < 480)
    //             return t.HLT_PFHT250_Prescale;
    //         else if(t.ht >= 480 && t.ht < 540)
    //             return t.HLT_PFHT370_Prescale;
    //         else if(t.ht >= 540 && t.ht < 640)
    //             return t.HLT_PFHT430_Prescale; 
    //         else if(t.HLT_PFJet500)
    //             return 1;
    //         else if(t.ht >= 640 && t.ht < 720)
    //             return t.HLT_PFHT510_Prescale;
    //         else if(t.ht >= 720 && t.ht < 820)
    //             return t.HLT_PFHT590_Prescale;
    //         else if(t.ht >= 820 && t.ht < 940)
    //             return t.HLT_PFHT680_Prescale;
    //         else if(t.ht >= 940 && t.ht < 1040)
    //             return t.HLT_PFHT780_Prescale;
    //         else if(t.ht >= 1040 && t.ht < 1200)
    //             return t.HLT_PFHT890_Prescale;
    //         else
    //             return 1;
    //     }
    // }else{
    //     if(!prescalesByEvent){
    //         if(t.ht < 250)
    //             return -1;
    //         else if(t.ht >= 250 && t.ht < 340)
    //             return 11433.;
    //         else if(t.ht >= 340 && t.ht < 480)
    //             return 4120.;
    //         else if(t.ht >= 480 && t.ht < 540)
    //             return 1604.;
    //         else if(t.ht >= 540 && t.ht < 640)
    //             return 502.5;
    //         else if(t.HLT_PFJet500)
    //             return 1;
    //         else if(t.ht >= 640 && t.ht < 720)
    //             return 255.3;
    //         else if(t.ht >= 720 && t.ht < 820)
    //             return 128.7;
    //         else if(t.ht >= 820 && t.ht < 940)
    //             return 64.8;
    //         else if(t.ht >= 940 && t.ht < 1040)
    //             return 32.5;
    //         else if(t.ht >= 1040 && t.ht < 1200)
    //             return 16.3;
    //         else
    //             return 1;
    //     }else{
    //         if(t.ht < 250)
    //             return -1;
    //         else if(t.ht >= 250 && t.ht < 340)
    //             return t.HLT_PFHT180_Prescale;
    //         else if(t.ht >= 340 && t.ht < 480)
    //             return t.HLT_PFHT250_Prescale;
    //         else if(t.ht >= 480 && t.ht < 540)
    //             return t.HLT_PFHT370_Prescale;
    //         else if(t.ht >= 540 && t.ht < 640)
    //             return t.HLT_PFHT430_Prescale; 
    //         else if(t.HLT_PFJet500)
    //             return 1;
    //         else if(t.ht >= 640 && t.ht < 720)
    //             return t.HLT_PFHT510_Prescale;
    //         else if(t.ht >= 720 && t.ht < 820)
    //             return t.HLT_PFHT590_Prescale;
    //         else if(t.ht >= 820 && t.ht < 940)
    //             return t.HLT_PFHT680_Prescale;
    //         else if(t.ht >= 940 && t.ht < 1040)
    //             return t.HLT_PFHT780_Prescale;
    //         else if(t.ht >= 1040 && t.ht < 1200)
    //             return t.HLT_PFHT890_Prescale;
    //         else
    //             return 1;
    //     }
    // }


    if(config_tag_.find("2016") != string::npos){
        if(!prescalesByEvent){
            if (t.ht < 250)
                return -1;
            else if (t.ht < 300)
                return 9421;
            else if (t.ht < 400)
                return 5410;
            else if (t.ht < 450)
                return 917;
            else if (t.ht < 575)
                return 458;
            else if(t.HLT_PFJet450)
                return 1;
            else if (t.ht < 700)
                return 115;
            else if (t.ht < 1000)
                return 29;
            else
                return 1;
        }else{
            if (t.ht < 250)
                return -1;
            else if (t.ht < 300)
                return t.HLT_PFHT125_Prescale;
            else if (t.ht < 400)
                return t.HLT_PFHT200_Prescale;
            else if (t.ht < 450)
                return t.HLT_PFHT300_Prescale;
            else if (t.ht < 575)
                return t.HLT_PFHT350_Prescale;
            else if(t.HLT_PFJet450)
                return 1;
            else if (t.ht < 700)
                return t.HLT_PFHT475_Prescale;
            else if (t.ht < 1000)
                return t.HLT_PFHT600_Prescale;
            else
                return 1;
        }
    }else{
        if(!prescalesByEvent){
            if(t.ht < 250)
                return -1;
            else if(t.ht >= 250 && t.ht < 440)
                return 1316.;
            else if(t.ht >= 440 && t.ht < 520)
                return 707.;
            else if(t.ht >= 520 && t.ht < 620)
                return 307.;
            else if(t.HLT_PFJet500)
                return 1;
            else if(t.ht >= 620 && t.ht < 700)
                return 145.;
            else if(t.ht >= 700 && t.ht < 800)
                return 67.4;
            else if(t.ht >= 800 && t.ht < 900)
                return 47.4;
            else if(t.ht >= 900 && t.ht < 1000)
                return 29.5;
            else if(t.ht >= 1000 && t.ht < 1200)
                return 17.1;
            else
                return 1;
        }else{
            if(t.ht < 250)
                return -1;
            else if(t.ht >= 250 && t.ht < 440)
                return t.HLT_PFHT180_Prescale;
            else if(t.ht >= 440 && t.ht < 520)
                return t.HLT_PFHT370_Prescale;
            else if(t.ht >= 520 && t.ht < 620)
                return t.HLT_PFHT430_Prescale; 
            else if(t.HLT_PFJet500)
                return 1;
            else if(t.ht >= 620 && t.ht < 700)
                return t.HLT_PFHT510_Prescale;
            else if(t.ht >= 700 && t.ht < 800)
                return t.HLT_PFHT590_Prescale;
            else if(t.ht >= 800 && t.ht < 900)
                return t.HLT_PFHT680_Prescale;
            else if(t.ht >= 900 && t.ht < 1000)
                return t.HLT_PFHT780_Prescale;
            else if(t.ht >= 1000 && t.ht < 1200)
                return t.HLT_PFHT890_Prescale;
            else
                return 1;
        }
    }

}

bool SmearLooper::passesTriggerSR(){
    if(not t.isData) return 1;

    if(config_tag_.find("2016") != string::npos){
        return t.HLT_PFHT900 || t.HLT_PFJet450 || t.HLT_PFHT300_PFMET110 || t.HLT_PFMET120_PFMHT120 || t.HLT_PFMETNoMu120_PFMHTNoMu120;
    }else{
        return t.HLT_PFHT1050 || t.HLT_PFJet500 || t.HLT_PFHT800_PFMET75_PFMHT75 || t.HLT_PFHT500_PFMET100_PFMHT100 || t.HLT_PFMET120_PFMHT120 || t.HLT_PFMET120_PFMHT120_PFHT60 || t.HLT_PFMETNoMu120_PFMHTNoMu120;
    }    
}

bool SmearLooper::passesTriggerSF(){
    if(not t.isData) return 1;

    if(config_tag_.find("2016") != string::npos){
        return t.HLT_DoubleEl || t.HLT_DoubleMu || t.HLT_Photon165_HE10 || t.HLT_DoubleMu_NonIso || t.HLT_SingleMu_NonIso || t.HLT_DoubleEl33;
    }else{
        return t.HLT_DoubleEl || t.HLT_DoubleMu || t.HLT_Photon200 || t.HLT_DoubleMu_NonIso || t.HLT_SingleMu_NonIso || t.HLT_DoubleEl33;
    }
}

bool SmearLooper::passesTriggerOF(){
    if(not t.isData) return 1;

    if(config_tag_.find("2016") != string::npos){
        return t.HLT_MuX_Ele12 || t.HLT_Mu8_EleX || t.HLT_Mu30_Ele30_NonIso || t.HLT_Mu33_Ele33_NonIso || t.HLT_Photon165_HE10 || t.HLT_SingleMu_NonIso;
    }else{
        return t.HLT_MuX_Ele12 || t.HLT_Mu8_EleX || t.HLT_Mu12_EleX || t.HLT_Mu37_Ele27_NonIso || t.HLT_Mu27_Ele37_NonIso || t.HLT_Photon200 || t.HLT_SingleMu_NonIso;
    }
}
