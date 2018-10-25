#include "sigSelections_RebalSmear.h"

namespace mt2 {


std::vector<SRRS> getSignalRegionsICHEP_RS(){

  std::vector<SRRS> temp_SR_vec;
  std::vector<SRRS> SRVec;
  SRRS sr;
  SRRS baseSR;

  //first set binning in njet-nbjet plane
    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    sr.SetVarCRRSInvertDPhi("njets", 2, 4);
    sr.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    sr.SetVarCRRSMT2SideBand("njets", 2, 4);
    sr.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    sr.SetVarCRRSDPhiMT2("njets", 2, 4);
    sr.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    sr.SetVarCRRSInvertDPhi("njets", 2, 4);
    sr.SetVarCRRSInvertDPhi("nbjets", 1, 2);
    sr.SetVarCRRSMT2SideBand("njets", 2, 4);
    sr.SetVarCRRSMT2SideBand("nbjets", 1, 2);
    sr.SetVarCRRSDPhiMT2("njets", 2, 4);
    sr.SetVarCRRSDPhiMT2("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    sr.SetVarCRRSInvertDPhi("njets", 2, 4);
    sr.SetVarCRRSInvertDPhi("nbjets", 2, 3);
    sr.SetVarCRRSMT2SideBand("njets", 2, 4);
    sr.SetVarCRRSMT2SideBand("nbjets", 2, 3);
    sr.SetVarCRRSDPhiMT2("njets", 2, 4);
    sr.SetVarCRRSDPhiMT2("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    sr.SetVarCRRSInvertDPhi("njets", 4, 7);
    sr.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    sr.SetVarCRRSMT2SideBand("njets", 4, 7);
    sr.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    sr.SetVarCRRSDPhiMT2("njets", 4, 7);
    sr.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    sr.SetVarCRRSInvertDPhi("njets", 4, 7);
    sr.SetVarCRRSInvertDPhi("nbjets", 1, 2);
    sr.SetVarCRRSMT2SideBand("njets", 4, 7);
    sr.SetVarCRRSMT2SideBand("nbjets", 1, 2);
    sr.SetVarCRRSDPhiMT2("njets", 4, 7);
    sr.SetVarCRRSDPhiMT2("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    sr.SetVarCRRSInvertDPhi("njets", 4, 7);
    sr.SetVarCRRSInvertDPhi("nbjets", 2, 3);
    sr.SetVarCRRSMT2SideBand("njets", 4, 7);
    sr.SetVarCRRSMT2SideBand("nbjets", 2, 3);
    sr.SetVarCRRSDPhiMT2("njets", 4, 7);
    sr.SetVarCRRSDPhiMT2("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    sr.SetVarCRRSInvertDPhi("njets", 7, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    sr.SetVarCRRSMT2SideBand("njets", 7, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    sr.SetVarCRRSDPhiMT2("njets", 7, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("8");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, 3);
    sr.SetVarCRRSInvertDPhi("njets", 7, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 1, 2); // DY: NB>0
    sr.SetVarCRRSMT2SideBand("njets", 7, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 1, 2); // DY: NB>0
    sr.SetVarCRRSDPhiMT2("njets", 7, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 1, 2); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("9");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, 3);
    sr.SetVarCRRSInvertDPhi("njets", 7, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 2, 3); // DY: NB>0
    sr.SetVarCRRSMT2SideBand("njets", 7, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 2, 3); // DY: NB>0
    sr.SetVarCRRSDPhiMT2("njets", 7, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 2, 3); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    sr.SetVarCRRSInvertDPhi("njets", 2, 7);
    sr.SetVarCRRSInvertDPhi("nbjets", 3, -1);
    sr.SetVarCRRSMT2SideBand("njets", 2, 7);
    sr.SetVarCRRSMT2SideBand("nbjets", 3, -1);
    sr.SetVarCRRSDPhiMT2("njets", 2, 7);
    sr.SetVarCRRSDPhiMT2("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("11");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, 3);
    sr.SetVarCRRSInvertDPhi("njets", 7, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 3, -1); // DY: NB>0
    sr.SetVarCRRSMT2SideBand("njets", 7, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 3, -1); // DY: NB>0
    sr.SetVarCRRSDPhiMT2("njets", 7, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 3, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("12");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    sr.SetVarCRRSInvertDPhi("njets", 4, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    sr.SetVarCRRSMT2SideBand("njets", 4, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    sr.SetVarCRRSDPhiMT2("njets", 4, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("13");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 1, 2);
    sr.SetVarCRRSInvertDPhi("njets", 4, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 1, 2);
    sr.SetVarCRRSMT2SideBand("njets", 4, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 1, 2);
    sr.SetVarCRRSDPhiMT2("njets", 4, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("14");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 2, 3);
    sr.SetVarCRRSInvertDPhi("njets", 4, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 2, 3);
    sr.SetVarCRRSMT2SideBand("njets", 4, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 2, 3);
    sr.SetVarCRRSDPhiMT2("njets", 4, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("15");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 3, -1);
    sr.SetVarCRRSInvertDPhi("njets", 2, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 3, -1);
    sr.SetVarCRRSMT2SideBand("njets", 2, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 3, -1);
    sr.SetVarCRRSDPhiMT2("njets", 2, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();


  //add HT and MET requirements
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    if (iSR >= 3 && iSR <= 10) continue;
    SRRS fullSR = temp_SR_vec.at(iSR); 
    fullSR.SetName(fullSR.GetName() + "VL");
    fullSR.SetVar("ht", 250, 450);
    fullSR.SetVar("met", 250, -1);
    fullSR.SetVarCRSL("ht", 250, 450);
    fullSR.SetVarCRSL("met", 250, -1);
    fullSR.SetVarCRDY("ht", 250, 450);
    fullSR.SetVarCRDY("met", 250, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 250, 450);
    fullSR.SetVarCRRSInvertDPhi("met", 250, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 250, 450);
    fullSR.SetVarCRRSMT2SideBand("met", 250, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 250, 450);
    fullSR.SetVarCRRSDPhiMT2("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SRRS fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "L");
    fullSR.SetVar("ht", 450, 575);
    fullSR.SetVar("met", 250, -1);
    fullSR.SetVarCRSL("ht", 450, 575);
    fullSR.SetVarCRSL("met", 250, -1);
    fullSR.SetVarCRDY("ht", 450, 575);
    fullSR.SetVarCRDY("met", 250, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 450, 575);
    fullSR.SetVarCRRSInvertDPhi("met", 250, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 450, 575);
    fullSR.SetVarCRRSMT2SideBand("met", 250, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 450, 575);
    fullSR.SetVarCRRSDPhiMT2("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, 1500};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1500};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, 1500};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, 1500};      fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SRRS fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "M");
    fullSR.SetVar("ht", 575, 1000);
    fullSR.SetVar("met", 250, -1);
    fullSR.SetVarCRSL("ht", 575, 1000);
    fullSR.SetVarCRSL("met", 250, -1);
    fullSR.SetVarCRDY("ht", 575, 1000);
    fullSR.SetVarCRDY("met", 250, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 575, 1000);
    fullSR.SetVarCRRSInvertDPhi("met", 250, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 575, 1000);
    fullSR.SetVarCRRSMT2SideBand("met", 250, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 575, 1000);
    fullSR.SetVarCRRSDPhiMT2("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 600, 1500};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, 1500};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 600, 1500};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 600, 1500};      fullSR.SetMT2Bins(4, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SRRS fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "H");
    fullSR.SetVar("ht", 1000, 1500);
    fullSR.SetVar("met", 30, -1);
    fullSR.SetVarCRSL("ht", 1000, 1500);
    fullSR.SetVarCRSL("met", 30, -1);
    fullSR.SetVarCRDY("ht", 1000, 1500);
    fullSR.SetVarCRDY("met", 30, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 1000, 1500);
    fullSR.SetVarCRRSInvertDPhi("met", 30, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 1000, 1500);
    fullSR.SetVarCRRSMT2SideBand("met", 30, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 1000, 1500);
    fullSR.SetVarCRRSDPhiMT2("met", 30, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, 1500}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, 1500}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[6] = {200, 400, 600, 800, 1000, 1500};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, 1500}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, 1500}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[6] = {200, 400, 600, 800, 1000, 1500};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1500};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, 1500};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 400, 600, 800, 1500};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[4] = {200, 400, 600, 1500};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 400, 600, 1500};                  fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SRRS fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "UH");
    fullSR.SetVar("ht", 1500, -1);
    fullSR.SetVar("met", 30, -1);
    fullSR.SetVarCRSL("ht", 1500, -1);
    fullSR.SetVarCRSL("met", 30, -1);
    fullSR.SetVarCRDY("ht", 1500, -1);
    fullSR.SetVarCRDY("met", 30, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 1500, -1);
    fullSR.SetVarCRRSInvertDPhi("met", 30, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 1500, -1);
    fullSR.SetVarCRRSMT2SideBand("met", 30, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 1500, -1);
    fullSR.SetVarCRRSDPhiMT2("met", 30, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1400, 1800}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1800};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1400, 1800}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[7] = {200, 400, 600, 800, 1000, 1400, 1800}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 400, 600, 800, 1800};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, 1800};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 400, 600, 800, 1800};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[4] = {200, 400, 600, 1800};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1800};                       fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
  }

  //define baseline selections commmon to all signal regions 
  baseSR.SetVar("mt2", 200, -1);
  baseSR.SetVar("j1pt", 30, -1);
  baseSR.SetVar("j2pt", 30, -1);
  baseSR.SetVar("deltaPhiMin", 0.3, -1);
  baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVar("nlep", 0, 1);
  //baseSR.SetVar("passesHtMet", 1, 1);

  // common selections for CRSL
  baseSR.SetVarCRSL("mt2", 200, -1);
  baseSR.SetVarCRSL("j1pt", 30, -1);
  baseSR.SetVarCRSL("j2pt", 30, -1);
  baseSR.SetVarCRSL("deltaPhiMin", 0.3, -1);
  baseSR.SetVarCRSL("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRSL("nlep", 1, 2);

  // common selections for CRDY
  baseSR.SetVarCRDY("mt2", 200, -1);
  baseSR.SetVarCRDY("j1pt", 30, -1);
  baseSR.SetVarCRDY("j2pt", 30, -1);
  baseSR.SetVarCRDY("deltaPhiMin", 0.3, -1);
  baseSR.SetVarCRDY("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRDY("nlep", 2, 2);

  baseSR.SetVarCRRSInvertDPhi("mt2", 200, -1);
  baseSR.SetVarCRRSInvertDPhi("j1pt", 30, -1);
  baseSR.SetVarCRRSInvertDPhi("j2pt", 30, -1);
  baseSR.SetVarCRRSInvertDPhi("deltaPhiMin", 0.0, 0.3);
  baseSR.SetVarCRRSInvertDPhi("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRRSInvertDPhi("nlep", 0, 1);

  baseSR.SetVarCRRSMT2SideBand("mt2", 100, 200);
  baseSR.SetVarCRRSMT2SideBand("j1pt", 30, -1);
  baseSR.SetVarCRRSMT2SideBand("j2pt", 30, -1);
  baseSR.SetVarCRRSMT2SideBand("deltaPhiMin", 0.3, -1);
  baseSR.SetVarCRRSMT2SideBand("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRRSMT2SideBand("nlep", 0, 1);

  baseSR.SetVarCRRSDPhiMT2("mt2", 100, 200);
  baseSR.SetVarCRRSDPhiMT2("j1pt", 30, -1);
  baseSR.SetVarCRRSDPhiMT2("j2pt", 30, -1);
  baseSR.SetVarCRRSDPhiMT2("deltaPhiMin", 0.0, 0.3);
  baseSR.SetVarCRRSDPhiMT2("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRRSDPhiMT2("nlep", 0, 1);

  //add baseline selections to all signal regions 
  std::vector<std::string> vars = baseSR.GetListOfVariables();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < vars.size(); j++){
      SRVec.at(i).SetVar(vars.at(j), baseSR.GetLowerBound(vars.at(j)), baseSR.GetUpperBound(vars.at(j)));
    }
  }

  //add baseline selections to all CRSL regions 
  std::vector<std::string> varsCRSL = baseSR.GetListOfVariablesCRSL();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRSL.size(); j++){
      SRVec.at(i).SetVarCRSL(varsCRSL.at(j), baseSR.GetLowerBoundCRSL(varsCRSL.at(j)), baseSR.GetUpperBoundCRSL(varsCRSL.at(j)));
    }
  }

  //add baseline selections to all CRDY regions 
  std::vector<std::string> varsCRDY = baseSR.GetListOfVariablesCRDY();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRDY.size(); j++){
      SRVec.at(i).SetVarCRDY(varsCRDY.at(j), baseSR.GetLowerBoundCRDY(varsCRDY.at(j)), baseSR.GetUpperBoundCRDY(varsCRDY.at(j)));
    }
  }

  std::vector<std::string> varsCRRSInvertDPhi = baseSR.GetListOfVariablesCRRSInvertDPhi();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRRSInvertDPhi.size(); j++){
      SRVec.at(i).SetVarCRRSInvertDPhi(varsCRRSInvertDPhi.at(j), baseSR.GetLowerBoundCRRSInvertDPhi(varsCRSL.at(j)), baseSR.GetUpperBoundCRRSInvertDPhi(varsCRSL.at(j)));
    }
  }

  std::vector<std::string> varsCRRSMT2SideBand = baseSR.GetListOfVariablesCRRSMT2SideBand();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRRSMT2SideBand.size(); j++){
      SRVec.at(i).SetVarCRRSMT2SideBand(varsCRRSMT2SideBand.at(j), baseSR.GetLowerBoundCRRSMT2SideBand(varsCRSL.at(j)), baseSR.GetUpperBoundCRRSMT2SideBand(varsCRSL.at(j)));
    }
  }

  std::vector<std::string> varsCRRSDPhiMT2 = baseSR.GetListOfVariablesCRRSDPhiMT2();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRRSDPhiMT2.size(); j++){
      SRVec.at(i).SetVarCRRSDPhiMT2(varsCRRSDPhiMT2.at(j), baseSR.GetLowerBoundCRRSDPhiMT2(varsCRSL.at(j)), baseSR.GetUpperBoundCRRSDPhiMT2(varsCRSL.at(j)));
    }
  }

  return SRVec;

}


std::vector<SRRS> getSignalRegions2017_RS(){

  std::vector<SRRS> temp_SR_vec;
  std::vector<SRRS> SRVec;
  SRRS sr;
  SRRS baseSR;

  //first set binning in njet-nbjet plane
    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    sr.SetVarCRRSInvertDPhi("njets", 2, 4);
    sr.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    sr.SetVarCRRSMT2SideBand("njets", 2, 4);
    sr.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    sr.SetVarCRRSDPhiMT2("njets", 2, 4);
    sr.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    sr.SetVarCRRSInvertDPhi("njets", 2, 4);
    sr.SetVarCRRSInvertDPhi("nbjets", 1, 2);
    sr.SetVarCRRSMT2SideBand("njets", 2, 4);
    sr.SetVarCRRSMT2SideBand("nbjets", 1, 2);
    sr.SetVarCRRSDPhiMT2("njets", 2, 4);
    sr.SetVarCRRSDPhiMT2("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    sr.SetVarCRRSInvertDPhi("njets", 2, 4);
    sr.SetVarCRRSInvertDPhi("nbjets", 2, 3);
    sr.SetVarCRRSMT2SideBand("njets", 2, 4);
    sr.SetVarCRRSMT2SideBand("nbjets", 2, 3);
    sr.SetVarCRRSDPhiMT2("njets", 2, 4);
    sr.SetVarCRRSDPhiMT2("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    sr.SetVarCRRSInvertDPhi("njets", 4, 7);
    sr.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    sr.SetVarCRRSMT2SideBand("njets", 4, 7);
    sr.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    sr.SetVarCRRSDPhiMT2("njets", 4, 7);
    sr.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    sr.SetVarCRRSInvertDPhi("njets", 4, 7);
    sr.SetVarCRRSInvertDPhi("nbjets", 1, 2);
    sr.SetVarCRRSMT2SideBand("njets", 4, 7);
    sr.SetVarCRRSMT2SideBand("nbjets", 1, 2);
    sr.SetVarCRRSDPhiMT2("njets", 4, 7);
    sr.SetVarCRRSDPhiMT2("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    sr.SetVarCRRSInvertDPhi("njets", 4, 7);
    sr.SetVarCRRSInvertDPhi("nbjets", 2, 3);
    sr.SetVarCRRSMT2SideBand("njets", 4, 7);
    sr.SetVarCRRSMT2SideBand("nbjets", 2, 3);
    sr.SetVarCRRSDPhiMT2("njets", 4, 7);
    sr.SetVarCRRSDPhiMT2("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    sr.SetVarCRRSInvertDPhi("njets", 7, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    sr.SetVarCRRSMT2SideBand("njets", 7, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    sr.SetVarCRRSDPhiMT2("njets", 7, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("8");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, 3);
    sr.SetVarCRRSInvertDPhi("njets", 7, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 1, 2); // DY: NB>0
    sr.SetVarCRRSMT2SideBand("njets", 7, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 1, 2); // DY: NB>0
    sr.SetVarCRRSDPhiMT2("njets", 7, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 1, 2); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("9");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, 3);
    sr.SetVarCRRSInvertDPhi("njets", 7, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 2, 3); // DY: NB>0
    sr.SetVarCRRSMT2SideBand("njets", 7, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 2, 3); // DY: NB>0
    sr.SetVarCRRSDPhiMT2("njets", 7, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 2, 3); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    sr.SetVarCRRSInvertDPhi("njets", 2, 7);
    sr.SetVarCRRSInvertDPhi("nbjets", 3, -1);
    sr.SetVarCRRSMT2SideBand("njets", 2, 7);
    sr.SetVarCRRSMT2SideBand("nbjets", 3, -1);
    sr.SetVarCRRSDPhiMT2("njets", 2, 7);
    sr.SetVarCRRSDPhiMT2("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("11");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, 3);
    sr.SetVarCRRSInvertDPhi("njets", 7, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 3, -1); // DY: NB>0
    sr.SetVarCRRSMT2SideBand("njets", 7, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 3, -1); // DY: NB>0
    sr.SetVarCRRSDPhiMT2("njets", 7, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 3, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("12");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    sr.SetVarCRRSInvertDPhi("njets", 4, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    sr.SetVarCRRSMT2SideBand("njets", 4, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    sr.SetVarCRRSDPhiMT2("njets", 4, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("13");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 1, 2);
    sr.SetVarCRRSInvertDPhi("njets", 4, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 1, 2);
    sr.SetVarCRRSMT2SideBand("njets", 4, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 1, 2);
    sr.SetVarCRRSDPhiMT2("njets", 4, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("14");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 2, 3);
    sr.SetVarCRRSInvertDPhi("njets", 4, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 2, 3);
    sr.SetVarCRRSMT2SideBand("njets", 4, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 2, 3);
    sr.SetVarCRRSDPhiMT2("njets", 4, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("15");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 3, -1);
    sr.SetVarCRRSInvertDPhi("njets", 2, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 3, -1);
    sr.SetVarCRRSMT2SideBand("njets", 2, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 3, -1);
    sr.SetVarCRRSDPhiMT2("njets", 2, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();


  //add HT and MET requirements
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    if (iSR >= 3 && iSR <= 10) continue;
    SRRS fullSR = temp_SR_vec.at(iSR); 
    fullSR.SetName(fullSR.GetName() + "VL");
    fullSR.SetVar("ht", 250, 450);
    fullSR.SetVar("met", 250, -1);
    fullSR.SetVarCRSL("ht", 250, 450);
    fullSR.SetVarCRSL("met", 250, -1);
    fullSR.SetVarCRDY("ht", 250, 450);
    fullSR.SetVarCRDY("met", 250, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 250, 450);
    fullSR.SetVarCRRSInvertDPhi("met", 250, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 250, 450);
    fullSR.SetVarCRRSMT2SideBand("met", 250, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 250, 450);
    fullSR.SetVarCRRSDPhiMT2("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      if(iSR >= 11) continue;
    SRRS fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "L");
    fullSR.SetVar("ht", 450, 575);
    fullSR.SetVar("met", 250, -1);
    fullSR.SetVarCRSL("ht", 450, 575);
    fullSR.SetVarCRSL("met", 250, -1);
    fullSR.SetVarCRDY("ht", 450, 575);
    fullSR.SetVarCRDY("met", 250, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 450, 575);
    fullSR.SetVarCRRSInvertDPhi("met", 250, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 450, 575);
    fullSR.SetVarCRRSMT2SideBand("met", 250, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 450, 575);
    fullSR.SetVarCRRSDPhiMT2("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, 1500};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1500};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, 1500};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, 1500};      fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      if(iSR >= 11) continue;
    SRRS fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "M");
    fullSR.SetVar("ht", 575, 1200);
    fullSR.SetVar("met", 250, -1);
    fullSR.SetVarCRSL("ht", 575, 1200);
    fullSR.SetVarCRSL("met", 250, -1);
    fullSR.SetVarCRDY("ht", 575, 1200);
    fullSR.SetVarCRDY("met", 250, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 575, 1200);
    fullSR.SetVarCRRSInvertDPhi("met", 250, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 575, 1200);
    fullSR.SetVarCRRSMT2SideBand("met", 250, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 575, 1200);
    fullSR.SetVarCRRSDPhiMT2("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 600, 1500};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, 1500};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 600, 1500};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 600, 1500};      fullSR.SetMT2Bins(4, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      if(iSR >= 11) continue;
    SRRS fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "H");
    fullSR.SetVar("ht", 1200, 1500);
    fullSR.SetVar("met", 30, -1);
    fullSR.SetVarCRSL("ht", 1200, 1500);
    fullSR.SetVarCRSL("met", 30, -1);
    fullSR.SetVarCRDY("ht", 1200, 1500);
    fullSR.SetVarCRDY("met", 30, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 1200, 1500);
    fullSR.SetVarCRRSInvertDPhi("met", 30, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 1200, 1500);
    fullSR.SetVarCRRSMT2SideBand("met", 30, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 1200, 1500);
    fullSR.SetVarCRRSDPhiMT2("met", 30, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, 1500}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, 1500}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[6] = {200, 400, 600, 800, 1000, 1500};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, 1500}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, 1500}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[6] = {200, 400, 600, 800, 1000, 1500};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1500};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, 1500};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 400, 600, 800, 1500};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[4] = {200, 400, 600, 1500};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 400, 600, 1500};                  fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      if(iSR >= 11) continue;
    SRRS fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "UH");
    fullSR.SetVar("ht", 1500, -1);
    fullSR.SetVar("met", 30, -1);
    fullSR.SetVarCRSL("ht", 1500, -1);
    fullSR.SetVarCRSL("met", 30, -1);
    fullSR.SetVarCRDY("ht", 1500, -1);
    fullSR.SetVarCRDY("met", 30, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 1500, -1);
    fullSR.SetVarCRRSInvertDPhi("met", 30, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 1500, -1);
    fullSR.SetVarCRRSMT2SideBand("met", 30, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 1500, -1);
    fullSR.SetVarCRRSDPhiMT2("met", 30, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1400, 1800}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1800};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1400, 1800}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[7] = {200, 400, 600, 800, 1000, 1400, 1800}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 400, 600, 800, 1800};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, 1800};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 400, 600, 800, 1800};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[4] = {200, 400, 600, 1800};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1800};                       fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
  }

  //define baseline selections commmon to all signal regions 
  baseSR.SetVar("mt2", 200, -1);
  baseSR.SetVar("j1pt", 30, -1);
  baseSR.SetVar("j2pt", 30, -1);
  baseSR.SetVar("deltaPhiMin", 0.3, -1);
  baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVar("nlep", 0, 1);
  //baseSR.SetVar("passesHtMet", 1, 1);

  // common selections for CRSL
  baseSR.SetVarCRSL("mt2", 200, -1);
  baseSR.SetVarCRSL("j1pt", 30, -1);
  baseSR.SetVarCRSL("j2pt", 30, -1);
  baseSR.SetVarCRSL("deltaPhiMin", 0.3, -1);
  baseSR.SetVarCRSL("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRSL("nlep", 1, 2);

  // common selections for CRDY
  baseSR.SetVarCRDY("mt2", 200, -1);
  baseSR.SetVarCRDY("j1pt", 30, -1);
  baseSR.SetVarCRDY("j2pt", 30, -1);
  baseSR.SetVarCRDY("deltaPhiMin", 0.3, -1);
  baseSR.SetVarCRDY("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRDY("nlep", 0, 1);

  baseSR.SetVarCRRSInvertDPhi("mt2", 200, -1);
  baseSR.SetVarCRRSInvertDPhi("j1pt", 30, -1);
  baseSR.SetVarCRRSInvertDPhi("j2pt", 30, -1);
  baseSR.SetVarCRRSInvertDPhi("deltaPhiMin", 0.0, 0.3);
  baseSR.SetVarCRRSInvertDPhi("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRRSInvertDPhi("nlep", 0, 1);

  baseSR.SetVarCRRSMT2SideBand("mt2", 100, 200);
  baseSR.SetVarCRRSMT2SideBand("j1pt", 30, -1);
  baseSR.SetVarCRRSMT2SideBand("j2pt", 30, -1);
  baseSR.SetVarCRRSMT2SideBand("deltaPhiMin", 0.3, -1);
  baseSR.SetVarCRRSMT2SideBand("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRRSMT2SideBand("nlep", 0, 1);

  baseSR.SetVarCRRSDPhiMT2("mt2", 100, 200);
  baseSR.SetVarCRRSDPhiMT2("j1pt", 30, -1);
  baseSR.SetVarCRRSDPhiMT2("j2pt", 30, -1);
  baseSR.SetVarCRRSDPhiMT2("deltaPhiMin", 0.0, 0.3);
  baseSR.SetVarCRRSDPhiMT2("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRRSDPhiMT2("nlep", 0, 1);

  //add baseline selections to all signal regions 
  std::vector<std::string> vars = baseSR.GetListOfVariables();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < vars.size(); j++){
      SRVec.at(i).SetVar(vars.at(j), baseSR.GetLowerBound(vars.at(j)), baseSR.GetUpperBound(vars.at(j)));
    }
  }

  //add baseline selections to all CRSL regions 
  std::vector<std::string> varsCRSL = baseSR.GetListOfVariablesCRSL();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRSL.size(); j++){
      SRVec.at(i).SetVarCRSL(varsCRSL.at(j), baseSR.GetLowerBoundCRSL(varsCRSL.at(j)), baseSR.GetUpperBoundCRSL(varsCRSL.at(j)));
    }
  }

  //add baseline selections to all CRDY regions 
  std::vector<std::string> varsCRDY = baseSR.GetListOfVariablesCRDY();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRDY.size(); j++){
      SRVec.at(i).SetVarCRDY(varsCRDY.at(j), baseSR.GetLowerBoundCRDY(varsCRDY.at(j)), baseSR.GetUpperBoundCRDY(varsCRDY.at(j)));
    }
  }

  std::vector<std::string> varsCRRSInvertDPhi = baseSR.GetListOfVariablesCRRSInvertDPhi();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRRSInvertDPhi.size(); j++){
      SRVec.at(i).SetVarCRRSInvertDPhi(varsCRRSInvertDPhi.at(j), baseSR.GetLowerBoundCRRSInvertDPhi(varsCRSL.at(j)), baseSR.GetUpperBoundCRRSInvertDPhi(varsCRSL.at(j)));
    }
  }

  std::vector<std::string> varsCRRSMT2SideBand = baseSR.GetListOfVariablesCRRSMT2SideBand();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRRSMT2SideBand.size(); j++){
      SRVec.at(i).SetVarCRRSMT2SideBand(varsCRRSMT2SideBand.at(j), baseSR.GetLowerBoundCRRSMT2SideBand(varsCRSL.at(j)), baseSR.GetUpperBoundCRRSMT2SideBand(varsCRSL.at(j)));
    }
  }

  std::vector<std::string> varsCRRSDPhiMT2 = baseSR.GetListOfVariablesCRRSDPhiMT2();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRRSDPhiMT2.size(); j++){
      SRVec.at(i).SetVarCRRSDPhiMT2(varsCRRSDPhiMT2.at(j), baseSR.GetLowerBoundCRRSDPhiMT2(varsCRSL.at(j)), baseSR.GetUpperBoundCRRSDPhiMT2(varsCRSL.at(j)));
    }
  }
  
  return SRVec;
  
}

std::vector<SRRS> getSignalRegions2018_RS(){

  std::vector<SRRS> temp_SR_vec;
  std::vector<SRRS> SRVec;
  SRRS sr;
  SRRS baseSR;

  //first set binning in njet-nbjet plane
    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    sr.SetVarCRRSInvertDPhi("njets", 2, 4);
    sr.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    sr.SetVarCRRSMT2SideBand("njets", 2, 4);
    sr.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    sr.SetVarCRRSDPhiMT2("njets", 2, 4);
    sr.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    sr.SetVarCRRSInvertDPhi("njets", 2, 4);
    sr.SetVarCRRSInvertDPhi("nbjets", 1, 2);
    sr.SetVarCRRSMT2SideBand("njets", 2, 4);
    sr.SetVarCRRSMT2SideBand("nbjets", 1, 2);
    sr.SetVarCRRSDPhiMT2("njets", 2, 4);
    sr.SetVarCRRSDPhiMT2("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    sr.SetVarCRRSInvertDPhi("njets", 2, 4);
    sr.SetVarCRRSInvertDPhi("nbjets", 2, 3);
    sr.SetVarCRRSMT2SideBand("njets", 2, 4);
    sr.SetVarCRRSMT2SideBand("nbjets", 2, 3);
    sr.SetVarCRRSDPhiMT2("njets", 2, 4);
    sr.SetVarCRRSDPhiMT2("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    sr.SetVarCRRSInvertDPhi("njets", 4, 7);
    sr.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    sr.SetVarCRRSMT2SideBand("njets", 4, 7);
    sr.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    sr.SetVarCRRSDPhiMT2("njets", 4, 7);
    sr.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    sr.SetVarCRRSInvertDPhi("njets", 4, 7);
    sr.SetVarCRRSInvertDPhi("nbjets", 1, 2);
    sr.SetVarCRRSMT2SideBand("njets", 4, 7);
    sr.SetVarCRRSMT2SideBand("nbjets", 1, 2);
    sr.SetVarCRRSDPhiMT2("njets", 4, 7);
    sr.SetVarCRRSDPhiMT2("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    sr.SetVarCRRSInvertDPhi("njets", 4, 7);
    sr.SetVarCRRSInvertDPhi("nbjets", 2, 3);
    sr.SetVarCRRSMT2SideBand("njets", 4, 7);
    sr.SetVarCRRSMT2SideBand("nbjets", 2, 3);
    sr.SetVarCRRSDPhiMT2("njets", 4, 7);
    sr.SetVarCRRSDPhiMT2("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    sr.SetVarCRRSInvertDPhi("njets", 7, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    sr.SetVarCRRSMT2SideBand("njets", 7, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    sr.SetVarCRRSDPhiMT2("njets", 7, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("8");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    sr.SetVarCRRSInvertDPhi("njets", 7, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 1, 2); // DY: NB>0
    sr.SetVarCRRSMT2SideBand("njets", 7, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 1, 2); // DY: NB>0
    sr.SetVarCRRSDPhiMT2("njets", 7, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 1, 2); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("9");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    sr.SetVarCRRSInvertDPhi("njets", 7, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 2, 3); // DY: NB>0
    sr.SetVarCRRSMT2SideBand("njets", 7, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 2, 3); // DY: NB>0
    sr.SetVarCRRSDPhiMT2("njets", 7, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 2, 3); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    sr.SetVarCRRSInvertDPhi("njets", 2, 7);
    sr.SetVarCRRSInvertDPhi("nbjets", 3, -1);
    sr.SetVarCRRSMT2SideBand("njets", 2, 7);
    sr.SetVarCRRSMT2SideBand("nbjets", 3, -1);
    sr.SetVarCRRSDPhiMT2("njets", 2, 7);
    sr.SetVarCRRSDPhiMT2("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("11");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    sr.SetVarCRRSInvertDPhi("njets", 7, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 3, -1); // DY: NB>0
    sr.SetVarCRRSMT2SideBand("njets", 7, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 3, -1); // DY: NB>0
    sr.SetVarCRRSDPhiMT2("njets", 7, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 3, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("12");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    sr.SetVarCRRSInvertDPhi("njets", 4, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    sr.SetVarCRRSMT2SideBand("njets", 4, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    sr.SetVarCRRSDPhiMT2("njets", 4, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("13");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 1, 2);
    sr.SetVarCRRSInvertDPhi("njets", 4, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 1, 2);
    sr.SetVarCRRSMT2SideBand("njets", 4, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 1, 2);
    sr.SetVarCRRSDPhiMT2("njets", 4, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("14");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 2, 3);
    sr.SetVarCRRSInvertDPhi("njets", 4, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 2, 3);
    sr.SetVarCRRSMT2SideBand("njets", 4, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 2, 3);
    sr.SetVarCRRSDPhiMT2("njets", 4, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("15");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 3, -1);
    sr.SetVarCRRSInvertDPhi("njets", 2, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 3, -1);
    sr.SetVarCRRSMT2SideBand("njets", 2, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 3, -1);
    sr.SetVarCRRSDPhiMT2("njets", 2, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("20");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    sr.SetVarCRRSInvertDPhi("njets", 7, 10);
    sr.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    sr.SetVarCRRSMT2SideBand("njets", 7, 10);
    sr.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    sr.SetVarCRRSDPhiMT2("njets", 7, 10);
    sr.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("21");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    sr.SetVarCRRSInvertDPhi("njets", 7, 10);
    sr.SetVarCRRSInvertDPhi("nbjets", 1, 2);
    sr.SetVarCRRSMT2SideBand("njets", 7, 10);
    sr.SetVarCRRSMT2SideBand("nbjets", 1, 2);
    sr.SetVarCRRSDPhiMT2("njets", 7, 10);
    sr.SetVarCRRSDPhiMT2("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("22");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    sr.SetVarCRRSInvertDPhi("njets", 7, 10);
    sr.SetVarCRRSInvertDPhi("nbjets", 2, 3);
    sr.SetVarCRRSMT2SideBand("njets", 7, 10);
    sr.SetVarCRRSMT2SideBand("nbjets", 2, 3);
    sr.SetVarCRRSDPhiMT2("njets", 7, 10);
    sr.SetVarCRRSDPhiMT2("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("23");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 3, 4);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    sr.SetVarCRRSInvertDPhi("njets", 7, 10);
    sr.SetVarCRRSInvertDPhi("nbjets", 3, 4);
    sr.SetVarCRRSMT2SideBand("njets", 7, 10);
    sr.SetVarCRRSMT2SideBand("nbjets", 3, 4);
    sr.SetVarCRRSDPhiMT2("njets", 7, 10);
    sr.SetVarCRRSDPhiMT2("nbjets", 3, 4);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("24");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 4, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    sr.SetVarCRRSInvertDPhi("njets", 7, 10);
    sr.SetVarCRRSInvertDPhi("nbjets", 4, -1);
    sr.SetVarCRRSMT2SideBand("njets", 7, 10);
    sr.SetVarCRRSMT2SideBand("nbjets", 4, -1);
    sr.SetVarCRRSDPhiMT2("njets", 7, 10);
    sr.SetVarCRRSDPhiMT2("nbjets", 4, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("25");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    sr.SetVarCRRSInvertDPhi("njets", 10, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    sr.SetVarCRRSMT2SideBand("njets", 10, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 0, 1);
    sr.SetVarCRRSDPhiMT2("njets", 10, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("26");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    sr.SetVarCRRSInvertDPhi("njets", 10, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 1, 2);
    sr.SetVarCRRSMT2SideBand("njets", 10, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 1, 2);
    sr.SetVarCRRSDPhiMT2("njets", 10, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("27");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    sr.SetVarCRRSInvertDPhi("njets", 10, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 2, 3);
    sr.SetVarCRRSMT2SideBand("njets", 10, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 2, 3);
    sr.SetVarCRRSDPhiMT2("njets", 10, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("28");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 3, 4);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    sr.SetVarCRRSInvertDPhi("njets", 10, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 3, 4);
    sr.SetVarCRRSMT2SideBand("njets", 10, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 3, 4);
    sr.SetVarCRRSDPhiMT2("njets", 10, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 3, 4);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("29");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 4, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    sr.SetVarCRRSInvertDPhi("njets", 10, -1);
    sr.SetVarCRRSInvertDPhi("nbjets", 4, -1);
    sr.SetVarCRRSMT2SideBand("njets", 10, -1);
    sr.SetVarCRRSMT2SideBand("nbjets", 4, -1);
    sr.SetVarCRRSDPhiMT2("njets", 10, -1);
    sr.SetVarCRRSDPhiMT2("nbjets", 4, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();


  //add HT and MET requirements
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    if (iSR >= 11) continue;
    SRRS fullSR = temp_SR_vec.at(iSR); 
    fullSR.SetName(fullSR.GetName() + "VL");
    fullSR.SetVar("ht", 250, 450);
    fullSR.SetVar("met", 250, -1);
    fullSR.SetVarCRSL("ht", 250, 450);
    fullSR.SetVarCRSL("met", 250, -1);
    fullSR.SetVarCRDY("ht", 250, 450);
    fullSR.SetVarCRDY("met", 250, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 250, 450);
    fullSR.SetVarCRRSInvertDPhi("met", 250, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 250, 450);
    fullSR.SetVarCRRSMT2SideBand("met", 250, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 250, 450);
    fullSR.SetVarCRRSDPhiMT2("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, 1200}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1200}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, 1200}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, 1200}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1200}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, 1200}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 500, 1200}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[3] = {200, 300, 1200};      fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[3] = {200, 300, 1200};      fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 1200}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 300, 1200};      fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    if(iSR >= 11) continue;
    SRRS fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "L");
    fullSR.SetVar("ht", 450, 575);
    fullSR.SetVar("met", 250, -1);
    fullSR.SetVarCRSL("ht", 450, 575);
    fullSR.SetVarCRSL("met", 250, -1);
    fullSR.SetVarCRDY("ht", 450, 575);
    fullSR.SetVarCRDY("met", 250, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 450, 575);
    fullSR.SetVarCRRSInvertDPhi("met", 250, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 450, 575);
    fullSR.SetVarCRRSMT2SideBand("met", 250, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 450, 575);
    fullSR.SetVarCRRSDPhiMT2("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, 1200}; fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, 1200}; fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, 1200}; fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, 1200}; fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, 1200}; fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, 1200}; fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, 1200};      fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1200};      fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, 1200};      fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 500, 1200}; fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, 1200};      fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    if ((iSR >= 6 && iSR <= 8) || iSR == 10) continue; // The 7+ jet regions are VL, L only
    if (iSR >= 11 && iSR <= 18) continue;
    SRRS fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "M");
    fullSR.SetVar("ht", 575, 1200);
    fullSR.SetVar("met", 250, -1);
    fullSR.SetVarCRSL("ht", 575, 1200);
    fullSR.SetVarCRSL("met", 250, -1);
    fullSR.SetVarCRDY("ht", 575, 1200);
    fullSR.SetVarCRDY("met", 250, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 575, 1200);
    fullSR.SetVarCRRSInvertDPhi("met", 250, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 575, 1200);
    fullSR.SetVarCRRSMT2SideBand("met", 250, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 575, 1200);
    fullSR.SetVarCRRSDPhiMT2("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[11] = {200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200}; fullSR.SetMT2Bins(10, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, 1200}; fullSR.SetMT2Bins(6, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[6] = {200, 300, 400, 600, 800, 1200};       fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[11] = {200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200}; fullSR.SetMT2Bins(10, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, 1200}; fullSR.SetMT2Bins(6, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[6] = {200, 300, 400, 600, 800, 1200};       fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[6] = {200, 300, 400, 600, 800, 1200};       fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 600, 800, 1200};      fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[6] = {200, 300, 400, 600, 800, 1200};      fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[6] = {200, 300, 400, 600, 800, 1200};      fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 600, 1200};      fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 4){float mt2bins[4] = {200, 300, 400, 1200};           fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 500, 1200};          fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 500, 1200};          fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 500, 1200};          fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 3){float mt2bins[3] = {200, 300, 1200};               fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 4){float mt2bins[2] = {200, 1200};                    fullSR.SetMT2Bins(1, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    if ((iSR >= 6 && iSR <= 8) || iSR == 10) continue; // The 7+ jet regions are VL, L only
    if (iSR >= 11 && iSR <= 18) continue;
    SRRS fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "H");
    fullSR.SetVar("ht", 1200, 1700);
    fullSR.SetVar("met", 30, -1);
    fullSR.SetVarCRSL("ht", 1200, 1700);
    fullSR.SetVarCRSL("met", 30, -1);
    fullSR.SetVarCRDY("ht", 1200, 1700);
    fullSR.SetVarCRDY("met", 30, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 1200, 1700);
    fullSR.SetVarCRRSInvertDPhi("met", 30, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 1200, 1700);
    fullSR.SetVarCRRSMT2SideBand("met", 30, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 1200, 1700);
    fullSR.SetVarCRRSDPhiMT2("met", 30, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[8] = {200, 400, 600, 800, 1000, 1200, 1400, 1700}; fullSR.SetMT2Bins(7, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, 1700}; fullSR.SetMT2Bins(6, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[6] = {200, 400, 600, 800, 1000, 1700};       fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[8] = {200, 400, 600, 800, 1000, 1200, 1400, 1700}; fullSR.SetMT2Bins(7, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, 1700}; fullSR.SetMT2Bins(6, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[6] = {200, 400, 600, 800, 1000, 1700};       fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1700};       fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[6] = {200, 400, 600, 800, 1000, 1700};       fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 400, 600, 800, 1700};             fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[4] = {200, 400, 600, 1700};                  fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 400, 600, 1700};                  fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 4){float mt2bins[3] = {200, 400, 1700};                       fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[4] = {200, 400, 600, 1700};                 fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1700};                 fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, 1700};                 fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1700};                      fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 4){float mt2bins[2] = {200, 1700};                           fullSR.SetMT2Bins(1, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    if ((iSR >= 6 && iSR <= 8) || iSR == 10) continue; // The 7+ jet regions are VL, L only
    if (iSR >= 11 && iSR <= 18) continue;
    SRRS fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "UH");
    fullSR.SetVar("ht", 1700, -1);
    fullSR.SetVar("met", 30, -1);
    fullSR.SetVarCRSL("ht", 1700, -1);
    fullSR.SetVarCRSL("met", 30, -1);
    fullSR.SetVarCRDY("ht", 1700, -1);
    fullSR.SetVarCRDY("met", 30, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 1700, -1);
    fullSR.SetVarCRRSInvertDPhi("met", 30, -1);
    fullSR.SetVarCRRSMT2SideBand("ht", 1700, -1);
    fullSR.SetVarCRRSMT2SideBand("met", 30, -1);
    fullSR.SetVarCRRSDPhiMT2("ht", 1700, -1);
    fullSR.SetVarCRRSDPhiMT2("met", 30, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int njets_hi = fullSR.GetUpperBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[9] = {200, 400, 600, 800, 1000, 1200, 1400, 1800, 2400}; fullSR.SetMT2Bins(8, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, 2400};       fullSR.SetMT2Bins(6, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 2){
        float mt2bins[3] = {200, 400, 2400};
        float mt2bins_ssr[4] = {200, 400, 600, 2400};
        if (njets_hi == -1)
            fullSR.SetMT2Bins(3, mt2bins_ssr);
        else
            fullSR.SetMT2Bins(2, mt2bins);          
    }      
    else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[9] = {200, 400, 600, 800, 1000, 1200, 1400, 1600, 2400}; fullSR.SetMT2Bins(8, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[7] = {200, 400, 600, 800, 1000, 1400, 2400}; fullSR.SetMT2Bins(6, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 400, 600, 800, 2400};             fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1400, 2400};       fullSR.SetMT2Bins(6, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, 2400};             fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 400, 600, 800, 2400};             fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[4] = {200, 400, 600, 2400};                  fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 400, 800, 2400};                  fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 4){float mt2bins[3] = {200, 400, 2400};                       fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[4] = {200, 400, 800, 2400};                 fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 800, 2400};                 fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 2400};                      fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 2400};                      fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 10 && nbjets_lo == 4){float mt2bins[3] = {200, 400, 2400};                      fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
  }

  //define baseline selections commmon to all signal regions 
  baseSR.SetVar("mt2", 200, -1);
  baseSR.SetVar("j1pt", 30, -1);
  baseSR.SetVar("j2pt", 30, -1);
  baseSR.SetVar("deltaPhiMin", 0.3, -1);
  baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVar("nlep", 0, 1);
  //baseSR.SetVar("passesHtMet", 1, 1);

  // common selections for CRSL
  baseSR.SetVarCRSL("mt2", 200, -1);
  baseSR.SetVarCRSL("j1pt", 30, -1);
  baseSR.SetVarCRSL("j2pt", 30, -1);
  baseSR.SetVarCRSL("deltaPhiMin", 0.3, -1);
  baseSR.SetVarCRSL("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRSL("nlep", 1, 2);

  // common selections for CRDY
  baseSR.SetVarCRDY("mt2", 200, -1);
  baseSR.SetVarCRDY("j1pt", 30, -1);
  baseSR.SetVarCRDY("j2pt", 30, -1);
  baseSR.SetVarCRDY("deltaPhiMin", 0.3, -1);
  baseSR.SetVarCRDY("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRDY("nlep", 0, 1);

  baseSR.SetVarCRRSInvertDPhi("mt2", 200, -1);
  baseSR.SetVarCRRSInvertDPhi("j1pt", 30, -1);
  baseSR.SetVarCRRSInvertDPhi("j2pt", 30, -1);
  baseSR.SetVarCRRSInvertDPhi("deltaPhiMin", 0.0, 0.3);
  baseSR.SetVarCRRSInvertDPhi("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRRSInvertDPhi("nlep", 0, 1);

  baseSR.SetVarCRRSMT2SideBand("mt2", 100, 200);
  baseSR.SetVarCRRSMT2SideBand("j1pt", 30, -1);
  baseSR.SetVarCRRSMT2SideBand("j2pt", 30, -1);
  baseSR.SetVarCRRSMT2SideBand("deltaPhiMin", 0.3, -1);
  baseSR.SetVarCRRSMT2SideBand("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRRSMT2SideBand("nlep", 0, 1);

  baseSR.SetVarCRRSDPhiMT2("mt2", 100, 200);
  baseSR.SetVarCRRSDPhiMT2("j1pt", 30, -1);
  baseSR.SetVarCRRSDPhiMT2("j2pt", 30, -1);
  baseSR.SetVarCRRSDPhiMT2("deltaPhiMin", 0.0, 0.3);
  baseSR.SetVarCRRSDPhiMT2("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRRSDPhiMT2("nlep", 0, 1);

  //add baseline selections to all signal regions 
  std::vector<std::string> vars = baseSR.GetListOfVariables();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < vars.size(); j++){
      SRVec.at(i).SetVar(vars.at(j), baseSR.GetLowerBound(vars.at(j)), baseSR.GetUpperBound(vars.at(j)));
    }
  }

  //add baseline selections to all CRSL regions 
  std::vector<std::string> varsCRSL = baseSR.GetListOfVariablesCRSL();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRSL.size(); j++){
      SRVec.at(i).SetVarCRSL(varsCRSL.at(j), baseSR.GetLowerBoundCRSL(varsCRSL.at(j)), baseSR.GetUpperBoundCRSL(varsCRSL.at(j)));
    }
  }

  //add baseline selections to all CRDY regions 
  std::vector<std::string> varsCRDY = baseSR.GetListOfVariablesCRDY();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRDY.size(); j++){
      SRVec.at(i).SetVarCRDY(varsCRDY.at(j), baseSR.GetLowerBoundCRDY(varsCRDY.at(j)), baseSR.GetUpperBoundCRDY(varsCRDY.at(j)));
    }
  }

  std::vector<std::string> varsCRRSInvertDPhi = baseSR.GetListOfVariablesCRRSInvertDPhi();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRRSInvertDPhi.size(); j++){
      SRVec.at(i).SetVarCRRSInvertDPhi(varsCRRSInvertDPhi.at(j), baseSR.GetLowerBoundCRRSInvertDPhi(varsCRSL.at(j)), baseSR.GetUpperBoundCRRSInvertDPhi(varsCRSL.at(j)));
    }
  }

  std::vector<std::string> varsCRRSMT2SideBand = baseSR.GetListOfVariablesCRRSMT2SideBand();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRRSMT2SideBand.size(); j++){
      SRVec.at(i).SetVarCRRSMT2SideBand(varsCRRSMT2SideBand.at(j), baseSR.GetLowerBoundCRRSMT2SideBand(varsCRSL.at(j)), baseSR.GetUpperBoundCRRSMT2SideBand(varsCRSL.at(j)));
    }
  }

  std::vector<std::string> varsCRRSDPhiMT2 = baseSR.GetListOfVariablesCRRSDPhiMT2();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRRSDPhiMT2.size(); j++){
      SRVec.at(i).SetVarCRRSDPhiMT2(varsCRRSDPhiMT2.at(j), baseSR.GetLowerBoundCRRSDPhiMT2(varsCRSL.at(j)), baseSR.GetUpperBoundCRRSDPhiMT2(varsCRSL.at(j)));
    }
  }
  
  return SRVec;
  
}
    
std::vector<SRRS> getSignalRegionsMonojet_RS(){

    std::vector<SRRS> temp_SR_vec;
    std::vector<SRRS> SRVec;
    SRRS baseSR;

    baseSR.SetVar("nlep", 0, 1);
    baseSR.SetVar("njets", 1, 2);
    baseSR.SetVar("met", 250, -1);
    baseSR.SetVar("jet1_pt", 250, -1);
    baseSR.SetVar("deltaPhiMin", 0.3, -1);
    baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
    // use this for the monojet CR: invert deltaphi, require ==2jets, and jet1_pt>250
    baseSR.SetVarCRRSInvertDPhi("nlep", 0, 1);
    baseSR.SetVarCRRSInvertDPhi("njets", 2, 3);
    baseSR.SetVarCRRSInvertDPhi("met", 250, -1);
    baseSR.SetVarCRRSInvertDPhi("jet1_pt", 250, -1);
    baseSR.SetVarCRRSInvertDPhi("jet2_pt", 30, -1);
    baseSR.SetVarCRRSInvertDPhi("deltaPhiMin", 0, 0.3);
    baseSR.SetVarCRRSInvertDPhi("diffMetMhtOverMet", 0, 0.5);

    // fine binning in HT
    const unsigned int nbins_monojet_0b = 7;
    float htbins_0b[nbins_monojet_0b+1] = {250, 350, 450, 575, 700, 1000, 1500, -1};
    float htbins_0b_forPlotting[nbins_monojet_0b+1] = {250, 350, 450, 575, 700, 1000, 1500, 2000};
    const unsigned int nbins_monojet_1b = 5;
    float htbins_1b[nbins_monojet_1b+1] = {250, 350, 450, 575, 700, -1};
    float htbins_1b_forPlotting[nbins_monojet_1b+1] = {250, 350, 450, 575, 700, 1500};

    temp_SR_vec.clear();
    for(unsigned int iSR = 0; iSR < nbins_monojet_0b; iSR++){
      SRRS fullSR0b = baseSR;  
      fullSR0b.SetName(std::to_string(iSR+1) + "J");
      fullSR0b.SetVar("ht", htbins_0b[iSR], htbins_0b[iSR+1]);
      fullSR0b.SetVar("nbjets", 0, 1);
      fullSR0b.SetMT2Bins(nbins_monojet_0b, htbins_0b_forPlotting);
      fullSR0b.SetVarCRRSInvertDPhi("ht", htbins_0b[iSR], htbins_0b[iSR+1]);
      fullSR0b.SetVarCRRSInvertDPhi("nbjets", 0, 1);
      SRVec.push_back(fullSR0b);
    }
    for(unsigned int iSR = 0; iSR < nbins_monojet_1b; iSR++){
      SRRS fullSR1b = baseSR;  
      fullSR1b.SetName(std::to_string(iSR+11) + "J");
      fullSR1b.SetVar("ht", htbins_1b[iSR], htbins_1b[iSR+1]);
      fullSR1b.SetVar("nbjets", 1, -1);
      fullSR1b.SetMT2Bins(nbins_monojet_1b, htbins_1b_forPlotting);
      fullSR1b.SetVarCRRSInvertDPhi("ht", htbins_1b[iSR], htbins_1b[iSR+1]);
      fullSR1b.SetVarCRRSInvertDPhi("nbjets", 1, -1);
      SRVec.push_back(fullSR1b);
    }

    // Also put in some inclusive regions: 0b, 1b
    SRRS fullSR0b = baseSR;  
    fullSR0b.SetName("baseJ0B");
    fullSR0b.SetVar("nbjets", 0, 1);
    fullSR0b.SetVarCRRSInvertDPhi("nbjets", 0, 1);
    fullSR0b.SetVar("ht", 250, -1);
    fullSR0b.SetVarCRRSInvertDPhi("ht", 250, -1);
    fullSR0b.SetMT2Bins(nbins_monojet_0b, htbins_0b_forPlotting);
    SRVec.push_back(fullSR0b);

    SRRS fullSR1b = baseSR;  
    fullSR1b.SetName("baseJ1B");
    fullSR1b.SetVar("nbjets", 1, -1);
    fullSR1b.SetVarCRRSInvertDPhi("nbjets", 1, -1);
    fullSR1b.SetVar("ht", 250, -1);
    fullSR1b.SetVarCRRSInvertDPhi("ht", 250, -1);
    fullSR1b.SetMT2Bins(nbins_monojet_1b, htbins_1b_forPlotting);
    SRVec.push_back(fullSR1b);

    SRRS fullSR = baseSR;  
    fullSR.SetName("baseJ");
    fullSR.SetVar("nbjets", 0, -1);
    fullSR.SetVarCRRSInvertDPhi("nbjets", 0, -1);
    fullSR.SetVar("ht", 250, -1);
    fullSR.SetVarCRRSInvertDPhi("ht", 250, -1);
    fullSR.SetMT2Bins(nbins_monojet_0b, htbins_0b_forPlotting);
    SRVec.push_back(fullSR);

    return SRVec;

}

}
