#include "sigSelectionsLep.h"

namespace mt2 {

std::vector<SR> getSignalRegionsLep(){

  std::vector<SR> temp_SR_vec;
  std::vector<SR> SRVec;
  SR sr;
  SR baseSR;
  
  //first set binning in njet-nbjet plane
  sr.SetName("1");
  sr.SetVar("njets", 2, 4);
  sr.SetVar("nbjets", 0, 1);
  temp_SR_vec.push_back(sr);
  sr.Clear();
  
  sr.SetName("2");
  sr.SetVar("njets", 2, 4);
  sr.SetVar("nbjets", 1, 2);
  temp_SR_vec.push_back(sr);
  sr.Clear();
  
  sr.SetName("3");
  sr.SetVar("njets", 2, 4);
  sr.SetVar("nbjets", 2, 3);
  temp_SR_vec.push_back(sr);
  sr.Clear();
  
  sr.SetName("4");
  sr.SetVar("njets", 4, 7);
  sr.SetVar("nbjets", 0, 1);
  temp_SR_vec.push_back(sr);
  sr.Clear();
  
  sr.SetName("5");
  sr.SetVar("njets", 4, 7);
  sr.SetVar("nbjets", 1, 2);
  temp_SR_vec.push_back(sr);
  sr.Clear();

  sr.SetName("6");
  sr.SetVar("njets", 4, 7);
  sr.SetVar("nbjets", 2, 3);
  temp_SR_vec.push_back(sr);
  sr.Clear();

  sr.SetName("7");
  sr.SetVar("njets", 7, -1);
  sr.SetVar("nbjets", 0, 1);
  temp_SR_vec.push_back(sr);
  sr.Clear();

  sr.SetName("8");
  sr.SetVar("njets", 7, -1);
  sr.SetVar("nbjets", 1, 2);
  temp_SR_vec.push_back(sr);
  sr.Clear();

  sr.SetName("9");
  sr.SetVar("njets", 7, -1);
  sr.SetVar("nbjets", 2, 3);
  temp_SR_vec.push_back(sr);
  sr.Clear();

  sr.SetName("10");
  sr.SetVar("njets", 2, 7);
  sr.SetVar("nbjets", 3, -1);
  temp_SR_vec.push_back(sr);
  sr.Clear();

  sr.SetName("11");
  sr.SetVar("njets", 7, -1);
  sr.SetVar("nbjets", 3, -1);
  temp_SR_vec.push_back(sr);
  sr.Clear();
  
  
  //add HT and MET requirements
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "VL");
    fullSR.SetVar("ht", 200, 450);
    float mtbins[4] = {0, 90, 120, 300};      fullSR.SetMT2Bins(3, mtbins);
    SRVec.push_back(fullSR);
  }

  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "L");
    fullSR.SetVar("ht", 450, 575);
    float mtbins[4] = {0, 90, 120, 300};      fullSR.SetMT2Bins(3, mtbins);
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "M");
    fullSR.SetVar("ht", 575, 1000);
    float mtbins[4] = {0, 90, 120, 300};      fullSR.SetMT2Bins(3, mtbins);
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "H");
    fullSR.SetVar("ht", 1000, 1500);
    float mtbins[4] = {0, 90, 120, 300};      fullSR.SetMT2Bins(3, mtbins);
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "UH");
    fullSR.SetVar("ht", 1500, -1);
    float mtbins[4] = {0, 90, 120, 300};      fullSR.SetMT2Bins(3, mtbins);
    SRVec.push_back(fullSR);
  }

  //define baseline selections commmon to all signal regions 
  baseSR.SetVar("mt2", 100, -1);
  baseSR.SetVar("met", 200, -1);
  baseSR.SetVar("deltaPhiMin", 0.3, -1);
  baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVar("nlep", 1, 2);

  //add baseline selections to all signal regions 
  std::vector<std::string> vars = baseSR.GetListOfVariables();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < vars.size(); j++){
      SRVec.at(i).SetVar(vars.at(j), baseSR.GetLowerBound(vars.at(j)), baseSR.GetUpperBound(vars.at(j)));
    }
  }

  // Also put in some inclusive regions: 0b, 1b
  SR inclusivebase = baseSR;  
  inclusivebase.SetName("base");
  inclusivebase.SetVar("njets", 2, -1);
  inclusivebase.SetVar("nbjets", 0, -1);
  inclusivebase.SetVar("ht", 200, -1);
  float mtbins[4] = {0, 90, 120, 300};      
  inclusivebase.SetMT2Bins(3, mtbins);
  SRVec.push_back(inclusivebase);


  return SRVec;

}


std::vector<SR> getSignalRegionsLep2(){

  std::vector<SR> temp_SR_vec;
  std::vector<SR> SRVec;
  SR sr;
  SR baseSR;
  
  //first set binning in njet-nbjet plane
  sr.SetName("1");
  sr.SetVar("njets", 2, 4);
  sr.SetVar("nbjets", 0, 1);
  temp_SR_vec.push_back(sr);
  sr.Clear();
  
  sr.SetName("2");
  sr.SetVar("njets", 2, 4);
  sr.SetVar("nbjets", 1, 2);
  temp_SR_vec.push_back(sr);
  sr.Clear();
  
  sr.SetName("3");
  sr.SetVar("njets", 2, 4);
  sr.SetVar("nbjets", 2, 3);
  temp_SR_vec.push_back(sr);
  sr.Clear();
  
  sr.SetName("4");
  sr.SetVar("njets", 4, -1);
  sr.SetVar("nbjets", 0, 1);
  temp_SR_vec.push_back(sr);
  sr.Clear();
  
  sr.SetName("5");
  sr.SetVar("njets", 4, -1);
  sr.SetVar("nbjets", 1, 2);
  temp_SR_vec.push_back(sr);
  sr.Clear();

  sr.SetName("6");
  sr.SetVar("njets", 4, -1);
  sr.SetVar("nbjets", 2, 3);
  temp_SR_vec.push_back(sr);
  sr.Clear();
  
 
  //add HT and MET requirements
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "L");
    fullSR.SetVar("ht", 200, 300);
    float mtbins[4] = {20, 90, 120, 300};      fullSR.SetMT2Bins(3, mtbins);
    SRVec.push_back(fullSR);
  }

  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "M");
    fullSR.SetVar("ht", 300, 600);
    float mtbins[4] = {20, 90, 120, 300};      fullSR.SetMT2Bins(3, mtbins);
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "H");
    fullSR.SetVar("ht", 600, -1);
    float mtbins[4] = {20, 90, 120, 300};      fullSR.SetMT2Bins(3, mtbins);
    SRVec.push_back(fullSR);
  }

  SR sr3B;
  sr3B.SetName("B3");
  sr3B.SetVar("njets", 2, -1);
  sr3B.SetVar("nbjets", 3, -1);
  sr3B.SetVar("ht", 200, -1);
  float mtbins[4] = {20, 90, 120, 300};      sr3B.SetMT2Bins(3, mtbins);
  SRVec.push_back(sr3B);

  SR srJ1L;
  srJ1L.SetName("J1L");
  srJ1L.SetVar("njets", 1, 2);
  srJ1L.SetVar("nbjets", 0, 1);
  srJ1L.SetVar("ht", 200, 300);
  srJ1L.SetMT2Bins(3, mtbins);
  SRVec.push_back(srJ1L);
  SR srJ1M;
  srJ1M.SetName("J1M");
  srJ1M.SetVar("njets", 1, 2);
  srJ1M.SetVar("nbjets", 0, 1);
  srJ1M.SetVar("ht", 300, -1);
  srJ1M.SetMT2Bins(3, mtbins);
  SRVec.push_back(srJ1M);
  SR srJ2L;
  srJ2L.SetName("J2L");
  srJ2L.SetVar("njets", 1, 2);
  srJ2L.SetVar("nbjets", 1, -1);
  srJ2L.SetVar("ht", 200, -1);
  srJ2L.SetMT2Bins(3, mtbins);
  SRVec.push_back(srJ2L);
  

  //define baseline selections commmon to all signal regions 
  baseSR.SetVar("mt2", 100, -1);
  baseSR.SetVar("met", 200, -1);
  baseSR.SetVar("deltaPhiMin", 0.3, -1);
  baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVar("nlep", 1, 2);
  baseSR.SetVar("mt", 20, -1);

  //add baseline selections to all signal regions 
  std::vector<std::string> vars = baseSR.GetListOfVariables();
  for(unsigned int i = 0; i < SRVec.size(); i++){
    for(unsigned int j = 0; j < vars.size(); j++){
      SRVec.at(i).SetVar(vars.at(j), baseSR.GetLowerBound(vars.at(j)), baseSR.GetUpperBound(vars.at(j)));
    }
  }

  // Also put in the inclusive region2
  SR inclusivebase = baseSR;  
  inclusivebase.SetName("base");
  inclusivebase.SetVar("njets", 2, -1);
  inclusivebase.SetVar("nbjets", 0, -1);
  inclusivebase.SetVar("ht", 200, -1);
  inclusivebase.SetVar("mt", 20, -1);
  float mtbinsBase[4] = {20, 90, 120, 300};      
  inclusivebase.SetMT2Bins(3, mtbinsBase);
  SRVec.push_back(inclusivebase);

  SR inclusivebaseJ1 = baseSR;  
  inclusivebaseJ1.SetName("baseJ1");
  inclusivebaseJ1.SetVar("njets", 1, 2);
  inclusivebaseJ1.SetVar("nbjets", 0, 1);
  inclusivebaseJ1.SetVar("ht", 200, -1);
  inclusivebaseJ1.SetVar("mt", 20, -1);
  inclusivebaseJ1.SetMT2Bins(3, mtbinsBase);
  SRVec.push_back(inclusivebaseJ1);

  SR inclusivebaseJ2 = baseSR;  
  inclusivebaseJ2.SetName("baseJ2");
  inclusivebaseJ2.SetVar("njets", 1, 2);
  inclusivebaseJ2.SetVar("nbjets", 1, -1);
  inclusivebaseJ2.SetVar("ht", 200, -1);
  inclusivebaseJ2.SetVar("mt", 20, -1);
  inclusivebaseJ2.SetMT2Bins(3, mtbinsBase);
  SRVec.push_back(inclusivebaseJ2);


  return SRVec;

}
  
/*
std::vector<SR> getSignalRegionsMonojet(){

  std::vector<SR> temp_SR_vec;
  std::vector<SR> SRVec;
  SR baseSR;

  // define baseline selections commmon to all monojet regions
  float mt2bins_monojet[2] = {0, 1500};
  baseSR.SetVar("j1pt", 200, -1);
  baseSR.SetVar("nlep", 0, 1);
  baseSR.SetVar("njets", 1, 2);
  baseSR.SetVar("met", 200, -1);
  baseSR.SetVar("deltaPhiMin", 0.3, -1);
  baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRSL("j1pt", 200, -1);
  baseSR.SetVarCRSL("nlep", 1, 2);
  baseSR.SetVarCRSL("njets", 1, 2);
  baseSR.SetVarCRSL("met", 200, -1);
  baseSR.SetVarCRSL("deltaPhiMin", 0.3, -1);
  baseSR.SetVarCRSL("diffMetMhtOverMet", 0, 0.5);
  // QCD region: 2 jets, low deltaPhiMin, pt subleading between 30 and 60 GeV
  baseSR.SetVarCRQCD("j1pt", 200, -1);
  baseSR.SetVarCRQCD("j2pt", 30, 60);
  baseSR.SetVarCRQCD("nlep", 0, 1);
  baseSR.SetVarCRQCD("njets", 2, 3);
  baseSR.SetVarCRQCD("met", 200, -1);
  baseSR.SetVarCRQCD("deltaPhiMin", 0., 0.3);
  baseSR.SetVarCRQCD("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetMT2Bins(1, mt2bins_monojet);

  // fine binning in HT
  const int nbins_monojet_0b = 7;
  float htbins_0b[nbins_monojet_0b+1] = {200, 250, 350, 450, 575, 700, 1000, -1};
  float htbins_0b_forplot[nbins_monojet_0b+1] = {200, 250, 350, 450, 575, 700, 1000, 1500};
  const int nbins_monojet_1b = 5;
  float htbins_1b[nbins_monojet_1b+1] = {200, 250, 350, 450, 575, -1};
  float htbins_1b_forplot[nbins_monojet_1b+1] = {200, 250, 350, 450, 575, 1500};

  temp_SR_vec.clear();
  for(unsigned int iSR = 0; iSR < nbins_monojet_0b; iSR++){
    SR fullSR0b = baseSR;  
    fullSR0b.SetName(std::to_string(iSR+1) + "J");
    fullSR0b.SetVar("ht", htbins_0b[iSR], htbins_0b[iSR+1]);
    fullSR0b.SetVar("nbjets", 0, 1);
    fullSR0b.SetVarCRSL("ht", htbins_0b[iSR], htbins_0b[iSR+1]);
    fullSR0b.SetVarCRSL("nbjets", 0, 1);
    fullSR0b.SetVarCRQCD("ht", htbins_0b[iSR], htbins_0b[iSR+1]);
    fullSR0b.SetVarCRQCD("nbjets", 0, 1);
    SRVec.push_back(fullSR0b);
  }
  for(unsigned int iSR = 0; iSR < nbins_monojet_1b; iSR++){
    SR fullSR1b = baseSR;  
    fullSR1b.SetName(std::to_string(iSR+11) + "J");
    fullSR1b.SetVar("ht", htbins_1b[iSR], htbins_1b[iSR+1]);
    fullSR1b.SetVar("nbjets", 1, -1);
    fullSR1b.SetVarCRSL("ht", htbins_1b[iSR], htbins_1b[iSR+1]);
    fullSR1b.SetVarCRSL("nbjets", 1, -1);
    fullSR1b.SetVarCRQCD("ht", htbins_1b[iSR], htbins_1b[iSR+1]);
    fullSR1b.SetVarCRQCD("nbjets", 1, -1);
    SRVec.push_back(fullSR1b);
  }


  // Also put in some inclusive regions: 0b, 1b
  SR fullSR0b = baseSR;  
  fullSR0b.SetName("baseJ0B");
  fullSR0b.SetVar("nbjets", 0, 1);
  fullSR0b.SetVarCRSL("nbjets", 0, 1);
  fullSR0b.SetVarCRQCD("nbjets", 0, 1);
  fullSR0b.SetVar("ht", 200, -1);
  fullSR0b.SetVarCRSL("ht", 200, -1);
  fullSR0b.SetVarCRQCD("ht", 200, -1);
  fullSR0b.SetMT2Bins(nbins_monojet_0b, htbins_0b_forplot);
  SRVec.push_back(fullSR0b);

  SR fullSR1b = baseSR;  
  fullSR1b.SetName("baseJ1B");
  fullSR1b.SetVar("nbjets", 1, -1);
  fullSR1b.SetVarCRSL("nbjets", 1, -1);
  fullSR1b.SetVarCRQCD("nbjets", 1, -1);
  fullSR1b.SetVar("ht", 200, -1);
  fullSR1b.SetVarCRSL("ht", 200, -1);
  fullSR1b.SetVarCRQCD("ht", 200, -1);
  fullSR1b.SetMT2Bins(nbins_monojet_1b, htbins_1b_forplot);
  SRVec.push_back(fullSR1b);

  return SRVec;

}
*/

  
} // namespace mt2
