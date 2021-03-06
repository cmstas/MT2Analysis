#ifndef STP_H
#define STP_H

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
#include "TProfile.h"
#include "TChain.h"

// Tools
#include "../CORE/Tools/utils.h"
#include "../CORE/Tools/goodrun.h"
#include "../CORE/Tools/dorky/dorky.h"
#include "../CORE/Tools/badEventFilter.h"

#include "../MT2CORE/MHtree.h"
#include "../MT2CORE/configurations.h"

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

class ShortTrackTagAndProbeLooper {
 public:
  ShortTrackTagAndProbeLooper();
  ~ShortTrackTagAndProbeLooper();
  int loop(TChain* ch_st, char* infile, char* config_tag);
 private:
  MT2Configuration config_;
};

#endif
