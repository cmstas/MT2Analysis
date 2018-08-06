#ifndef FSL_H
#define FSL_H

// C++
#include <iostream>
#include <cmath>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <map>
#include <bitset>

// ROOT
#include "TMath.h"
#include "TLorentzVector.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TChain.h"

#include "../CORE/Tools/utils.h"

#include "../MT2CORE/mt2tree.h"

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

class FshortLooper {
 public:
  FshortLooper();
  ~FshortLooper();
  bool FillHists(const std::vector<TH2F*> hist, const double weight, const int fill_type, const int len_index=1);
  int loop(TChain* ch_st, char* infile);
};

#endif
