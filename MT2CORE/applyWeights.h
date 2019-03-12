#ifndef APPLYWEIGHTS_H
#define APPLYWEIGHTS_H
#include <iostream>
#include "TString.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2.h"

struct weightStruct {
  float cent; // central
  float up;
  float dn; // down
};

weightStruct getLepSF(float pt, float eta, int pdgId);
weightStruct getBtagSF(float pt, float eta, int pdgId);
float getBtagEff(float pt, float eta, int pdgId);


TH2D* h_elSF = 0;
TH2D* h_elSF_trk = 0;
TH2D* h_elSF_trk_lowPt = 0;
TH2D* h_muSF = 0;
TH1D* h_muSF_trk_ptlt10 = 0;
TH1D* h_muSF_trk_ptgt10 = 0;
bool setElSFfile(TString filenameIDISO, TString filenameTRK, TString filenameTRKlowPt, TString histName, TString isoHistName);
bool setMuSFfile(TString filenameID, TString filenameISO, TString filenameIP, TString filenameTRK, TString idHistName, TString isoHistName, TString ipHistName, TString trklt10HistName, TString trkgt10HistName);
bool setMuSFfileNoTrk(TString filenameID, TString filenameISO, TString filenameIP);
weightStruct getLepSFFromFile(float pt, float eta, int pdgId);

TH2D* h_elSF_fastsim = 0;
TH2D* h_muSF_fastsim = 0;
bool setElSFfile_fastsim(TString filenameIDISO, TString histnameID, TString histnameISO);
bool setMuSFfile_fastsim(TString filenameID, TString filenameISO, TString filenameIP,  TString histnameID, TString histnameISO, TString histnameIP);
weightStruct getLepSFFromFile_fastsim(float pt, float eta, int pdgId);

TH2D* h_elVetoEff_fullsim = 0;
TH2D* h_muVetoEff_fullsim = 0;
bool setVetoEffFile_fullsim(TString filename);
float getLepVetoEffFromFile_fullsim(float pt, float eta, int pdgId);

TH2D* h_elVetoEff_fastsim = 0;
TH2D* h_muVetoEff_fastsim = 0;
bool setVetoEffFile_fastsim(TString filename);
float getLepVetoEffFromFile_fastsim(float pt, float eta, int pdgId);

float getPhotonTriggerWeight(float eta, float pt);

TH2D* h_dilep_trigeff_ee = 0;
TH2D* h_dilep_trigeff_mm = 0;
TH2D* h_dilep_trigeff_em = 0;
bool setDilepTrigEffFile(TString filename);
float getDileptonTriggerWeight(float pt1, int pdgId1, float pt2, int pdgId2, int unc = 0);

#endif
