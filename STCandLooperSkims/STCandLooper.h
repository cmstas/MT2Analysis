#ifndef STCANDLOOPER_H
#define STCANDLOOPER_H

// C++
#include <iostream>
#include <cmath>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <map>

// ROOT
#include "TMath.h"
#include "TLorentzVector.h"
#include "TH1.h"
#include "TH2.h"
#include "TChain.h"

#include "../CORE/Tools/utils.h"

#include "../MT2CORE/mt2tree.h"
#include "../MT2CORE/sttree.h"

class STCandLooper {
 public:
  STCandLooper();
  ~STCandLooper();
  int InEtaPhiVetoRegion(float eta, float phi);
  int loop(char* indir, char* sample, int selectionMode, int vetoMode);

 private:
  // For short track
  const float pt_min_st = 15;
  const float dxy_max_st = 0.02;
  const float dz_max_st = 0.05;
  const float iso_max_st = 10;
  const float reliso_max_st = 0.2;
  const float niso_max_st = 10;
  const float chiso_max_st = 10;
  const float nreliso_max_st = 0.1;
  const float chreliso_max_st = 0.1;
  const float pterr_max_st = 0.2;
  const float pterr_max_st_s = 0.02;
  const float pterr_max_st_l = 0.005;
  const float genlepDR_min_st = 0.1;
  const float recolepDR_min_st = 0.1;
};

#endif
