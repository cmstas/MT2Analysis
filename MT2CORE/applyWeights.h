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
TH2D* h_muSF = 0;
bool setElSFfile(TString filename);
bool setMuSFfile(TString filenameID, TString filenameISO);
weightStruct getLepSFFromFile(float pt, float eta, int pdgId);

TH2D* h_elSF_fastsim = 0;
TH2D* h_muSF_fastsim = 0;
bool setElSFfile_fastsim(TString filename);
bool setMuSFfile_fastsim(TString filename);
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

#endif
