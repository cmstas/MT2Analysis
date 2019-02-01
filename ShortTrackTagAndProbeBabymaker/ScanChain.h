#ifndef STTAP_h
#define STTAP_h

// C++ includes
//#include <string>
//#include <vector>

// ROOT includes
#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "Math/LorentzVector.h"
#include "Math/GenVector/LorentzVector.h"

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > LorentzVector;


class ShortTrackTagAndProbeBabymaker {

 public:

  ShortTrackTagAndProbeBabymaker() {};
  ~ShortTrackTagAndProbeBabymaker() {
    delete BabyFile_;
    delete BabyTree_;
  };

  void ScanChain(TChain*, std::string sample, const std::string config_tag, int max_events = -1);

  void MakeBabyNtuple(const char *);
  void InitBabyNtuple();
  void FillBabyNtuple();
  void CloseBabyNtuple();

 private:

  TFile *BabyFile_;
  TTree *BabyTree_;

  bool isDataFromFileName;

  //baby ntuple variables

  Int_t           run;
  Int_t           lumi;
  ULong64_t       evt;
  Int_t           isData;

  Float_t         evt_scale1fb;
  Float_t         evt_xsec;
  Float_t         evt_kfactor;
  Float_t         evt_filter;

  Int_t           ntracks;
  Float_t         track_pt[20];
  Float_t         track_eta[20];
  Float_t         track_phi[20];
  Float_t         track_mass[20];
  Int_t           track_charge[20];
  Int_t           track_isMuon[20];
  Int_t           track_isElectron[20];
  Int_t           track_isPFCand[20];
  
  Float_t         track_dz[20];
  Float_t         track_dxy[20];
  Float_t         track_PtErrOverPt2[20];
  Float_t         track_iso[20];
  Float_t         track_pfNeutralSum[20];
  
  Int_t           track_iscand[20];

  Int_t           track_DeadECAL[20];
  Int_t           track_DeadHCAL[20];

  Int_t           track_missingOuterHits[20];
  Int_t           track_nLayers[20];

  Int_t           nsams;
  Float_t         sam_pt[10];
  Float_t         sam_eta[10];
  Float_t         sam_phi[10];
  Float_t         sam_mass[10];
  Int_t           sam_charge[10];

};

#endif

