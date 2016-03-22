#ifndef MT2LOOPER_h
#define MT2LOOPER_h

// C++ includes
//#include <string>
//#include <vector>

// ROOT includes
#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "Math/LorentzVector.h"


//MT2
#include "../MT2CORE/mt2tree.h"
#include "../MT2CORE/sigSelections.h"
#include "../MT2CORE/sigSelectionsLep.h"
#include "../MT2CORE/SR.h"

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > LorentzVector;
using namespace mt2;
class MT2Looper {

 public:

  struct lepcand {
    float pt;
    float eta;
    float phi;
    int pdgId;
    float mt;
    bool isPFCand;
  };

  MT2Looper();
  ~MT2Looper();

  void SetSignalRegions();
  void storeHistWithCutValues(std::vector<SR> & srvector, TString SR);
  void loop(TChain* chain, std::string sample, std::string output_dir);
  void fillHistosSRBase();
  void fillHistosInclusive();
  void fillHistosSignalRegion(const std::string& prefix = "", const std::string& suffix = "");
  void fillHistosLepSignalRegions(const std::string& prefix = "", const std::string& suffix = "");
  void fillHistosCR1L(const std::string& prefix = "", const std::string& suffix = "");
  void fillHistosDoubleL(const std::string& prefix = "", const std::string& suffix = "");
  void fillHistosZll(const std::string& prefix = "", const std::string& suffix = "");
  void fillHistos(std::map<std::string, TH1*>& h_1d, int n_mt2bins, float* mt2bins,
		  const std::string& dir = "", const std::string& suffix = "");
  void fillHistosSingleSoftLepton(std::map<std::string, TH1*>& h_1d, int n_mt2bins, float* mt2bins,
			      const std::string& dir = "", const std::string& suffix = ""); 
  void fillHistosDoubleLepton(std::map<std::string, TH1*>& h_1d, int n_mt2bins, float* mt2bins,
			      const std::string& dir = "", const std::string& suffix = ""); 
  void fillLepUncSR();
  void fillLepCorSRfastsim();
  void fillMissLepSF(int igenlep, bool isFromTau, bool isFastsim, float & lostSF, float & lostSFup, float & lostSFdn);

  
 private:

  TFile * outfile_;
  mt2tree t;
  float evtweight_;
  float evtweight_lepEffUp_;
  float evtweight_lepEffDn_;
  int nlepveto_;
    
  //soft lep variables
  float softleppt_;
  float softlepeta_;
  float softlepphi_;
  float softlepM_;
  int softlepId_;
  float softlepmt_;
  int softlepIdx_;
  int softlepCharge_;

  float hardleppt_;
  float hardlepeta_;
  float hardlepphi_;
  float hardlepM_;
  int hardlepId_;
  float hardlepmt_;
  int hardlepIdx_;
  int hardlepCharge_;

  float hardlep2pt_;
  float hardlep2eta_;
  float hardlep2phi_;
  float hardlep2M_;
  int hardlep2Id_;
  float hardlep2mt_;
  int hardlep2Idx_;
  int hardlep2Charge_;

  int removedzllLep_;
  float zllmt_;
  float zllmet_;
  
  bool foundMissingTau_;
  int missIdx_;
  float missPt_;
  float missEta_;
  float bjetPt_;
  int nSoftB_;
  int nHardB_;
  int categoryB_;

  bool onlyMakeFRplots_;

  //2-lep softLep CR variables
  float dilepmll_;
  
  float dilepmllTrack_;
  
  int nJet30Eta3_;
  bool isSignal_;
  bool passMonojetId_;
  std::map<std::string, TH1*> h_1d_global;
  std::vector<SR> SRVec;
  std::vector<SR> SRVecMonojet;
  std::vector<SR> InclusiveRegions;
  std::vector<SR> InclusiveRegionsMonojet;
  std::vector<SR> SRVecLep;
  SR SRBase;
  SR SRBaseIncl;
  SR SRBaseMonojet;
  SR SRBaseMonojet0B;
  SR SRBaseMonojet1B;
  SR SRNoCut;
  SR CRSL_WJets;
  SR CRSL_TTbar;
  TH1D* h_nvtx_weights_;
  TH2D* h_sig_nevents_;
  TH2D* h_sig_avgweight_btagsf_;
  TH2D* h_sig_avgweight_btagsf_heavy_UP_;
  TH2D* h_sig_avgweight_btagsf_light_UP_;
  TH2D* h_sig_avgweight_btagsf_heavy_DN_;
  TH2D* h_sig_avgweight_btagsf_light_DN_;
  TH2D* h_sig_avgweight_isr_;
  float cor_lepeff_sr_;
  float unc_lepeff_sr_;
  
};

#endif

