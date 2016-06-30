#include "sigSelectionsMT2Higgs.h"

namespace mt2 {

std::vector<SR> getSignalRegionsMT2Higgs(){

  std::vector<SR> temp_SR_vec;
  std::vector<SR> SRVec;
  SR sr;
  SR baseSR;

  //first set binning in njet-nbjet plane

  sr.SetName("1");
  sr.SetVar("njets", 4, 7);
  sr.SetVar("nbjets", 2, 3);
  sr.SetVarCRSL("njets", 4, 7);
  sr.SetVarCRSL("nbjets", 2, 3);
  temp_SR_vec.push_back(sr);
  sr.Clear();

  sr.SetName("2");
  sr.SetVar("njets", 7, -1);
  sr.SetVar("nbjets", 2, 3);
  sr.SetVarCRSL("njets", 7, -1);
  sr.SetVarCRSL("nbjets", 1, 3);
  temp_SR_vec.push_back(sr);
  sr.Clear();

  sr.SetName("3");
  sr.SetVar("njets", 4, 7);
  sr.SetVar("nbjets", 3, -1);
  sr.SetVarCRSL("njets", 2, 7);
  sr.SetVarCRSL("nbjets", 3, -1);
  temp_SR_vec.push_back(sr);
  sr.Clear();

  sr.SetName("4");
  sr.SetVar("njets", 7, -1);
  sr.SetVar("nbjets", 3, -1);
  sr.SetVarCRSL("njets", 7, -1);
  sr.SetVarCRSL("nbjets", 1, 3);
  temp_SR_vec.push_back(sr);
  sr.Clear();

  //add HT and MET requirements
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);
    fullSR.SetName(fullSR.GetName() + "VL");
    fullSR.SetVar("ht", 200, 450);
    fullSR.SetVar("met", 200, -1);
    fullSR.SetVarCRSL("ht", 200, 450);
    fullSR.SetVarCRSL("met", 200, -1);
    fullSR.SetVarCRQCD("ht", 200, 450);
    fullSR.SetVarCRQCD("met", 200, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[2] = {200, 1500};           fullSR.SetMT2Bins(1, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[2] = {200, 1500};           fullSR.SetMT2Bins(1, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[2] = {200, 1500};           fullSR.SetMT2Bins(1, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 300, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[2] = {200, 1500};           fullSR.SetMT2Bins(1, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);
    fullSR.SetName(fullSR.GetName() + "L");
    fullSR.SetVar("ht", 450, 575);
    fullSR.SetVar("met", 200, -1);
    fullSR.SetVarCRSL("ht", 450, 575);
    fullSR.SetVarCRSL("met", 200, -1);
    fullSR.SetVarCRQCD("ht", 450, 575);
    fullSR.SetVarCRQCD("met", 200, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[2] = {200, 1500};                fullSR.SetMT2Bins(1, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[3] = {200, 300, 1500};           fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[2] = {200, 1500};                fullSR.SetMT2Bins(1, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 300, 1500};           fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[2] = {200, 1500};                fullSR.SetMT2Bins(1, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);
    fullSR.SetName(fullSR.GetName() + "M");
    fullSR.SetVar("ht", 575, 1000);
    fullSR.SetVar("met", 200, -1);
    fullSR.SetVarCRSL("ht", 575, 1000);
    fullSR.SetVarCRSL("met", 200, -1);
    fullSR.SetVarCRQCD("ht", 575, 1000);
    fullSR.SetVarCRQCD("met", 200, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, 1500};      fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 300, 400, 600, 800, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 600, 1500};      fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, 1500};      fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, 1500};           fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1500};           fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, 1500};           fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, 1500};           fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, 1500};           fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);
    fullSR.SetName(fullSR.GetName() + "H");
    fullSR.SetVar("ht", 1000, 1500);
    fullSR.SetVar("met", 30, -1);
    fullSR.SetVarCRSL("ht", 1000, 1500);
    fullSR.SetVarCRSL("met", 30, -1);
    fullSR.SetVarCRQCD("ht", 1000, 1500);
    fullSR.SetVarCRQCD("met", 30, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, 1500};       fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1500};                 fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, 1500};       fullSR.SetMT2Bins(4, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, 1500};            fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 400, 600, 1500};            fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1500};            fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1500};                 fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1500};                 fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1500};                 fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);
    fullSR.SetName(fullSR.GetName() + "UH");
    fullSR.SetVar("ht", 1500, -1);
    fullSR.SetVar("met", 30, -1);
    fullSR.SetVarCRSL("ht", 1500, -1);
    fullSR.SetVarCRSL("met", 30, -1);
    fullSR.SetVarCRQCD("ht", 1500, -1);
    fullSR.SetVarCRQCD("met", 30, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1500};            fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[2] = {200, 1500};                      fullSR.SetMT2Bins(1, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1500}; fullSR.SetMT2Bins(5, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1500};            fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, 1500};            fullSR.SetMT2Bins(3, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[3] = {200, 400, 1500};                 fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[3] = {200, 400, 1500};                 fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1500};                 fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1500};                 fullSR.SetMT2Bins(2, mt2bins);}
    else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1500};                 fullSR.SetMT2Bins(2, mt2bins);}
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

  // common selections for QCD
  baseSR.SetVarCRQCD("mt2", 200, -1);
  baseSR.SetVarCRQCD("j1pt", 30, -1);
  baseSR.SetVarCRQCD("j2pt", 30, -1);
  baseSR.SetVarCRQCD("deltaPhiMin", 0., 0.3);
  baseSR.SetVarCRQCD("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRQCD("nlep", 0, 1);

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

  //add baseline selections to all QCD regions
  std::vector<std::string> varsCRQCD = baseSR.GetListOfVariablesCRQCD();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < varsCRQCD.size(); j++){
      SRVec.at(i).SetVarCRQCD(varsCRQCD.at(j), baseSR.GetLowerBoundCRQCD(varsCRQCD.at(j)), baseSR.GetUpperBoundCRQCD(varsCRQCD.at(j)));
    }
  }

  return SRVec;

}


} // namespace mt2
