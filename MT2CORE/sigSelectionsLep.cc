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
    fullSR.SetVar("ht", 600, 1000);
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

  SR srHT;
  srHT.SetName("HT");
  srHT.SetVar("njets", 2, -1);
  srHT.SetVar("nbjets", 0, 3);
  srHT.SetVar("ht", 1000, -1);
  srHT.SetMT2Bins(3, mtbins);
  SRVec.push_back(srHT);

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
  baseSR.SetVar("met", 200, 500);
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


  SR srMET;
  srMET.SetName("MET");
  srMET.SetVar("mt2", 100, -1);
  srMET.SetVar("met", 500, -1);
  srMET.SetVar("deltaPhiMin", 0.3, -1);
  srMET.SetVar("diffMetMhtOverMet", 0, 0.5);
  srMET.SetVar("nlep", 1, 2);
  srMET.SetVar("mt", 20, -1);
  srMET.SetVar("njets", 2, -1);
  srMET.SetVar("nbjets", 0, 3);
  srMET.SetVar("ht", 200, -1);
  srMET.SetMT2Bins(3, mtbins);
  SRVec.push_back(srMET);

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

std::vector<SR> getSignalRegionsLep3(){

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
    fullSR.SetVar("met", 200, 300);
    fullSR.SetVar("ht", 200, 1000);
    float mtbins[4] = {20, 90, 120, 300};      fullSR.SetMT2Bins(3, mtbins);
    SRVec.push_back(fullSR);
  }

  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "M");
    fullSR.SetVar("met", 300, 500);
    fullSR.SetVar("ht", 200, 1000);
    float mtbins[4] = {20, 90, 120, 300};      fullSR.SetMT2Bins(3, mtbins);
    SRVec.push_back(fullSR);
  }
//  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
//    SR fullSR = temp_SR_vec.at(iSR);  
//    fullSR.SetName(fullSR.GetName() + "H");
//    fullSR.SetVar("ht", 600, 1000);
//    float mtbins[4] = {20, 90, 120, 300};      fullSR.SetMT2Bins(3, mtbins);
//    SRVec.push_back(fullSR);
//  }

  SR sr3B;
  sr3B.SetName("B3");
  sr3B.SetVar("njets", 2, -1);
  sr3B.SetVar("nbjets", 3, -1);
  sr3B.SetVar("met", 200, -1);
  sr3B.SetVar("ht", 200, -1);
  float mtbins[4] = {20, 90, 120, 300};      sr3B.SetMT2Bins(3, mtbins);
  SRVec.push_back(sr3B);

  SR srMET;
  srMET.SetName("MET");
  srMET.SetVar("njets", 2, -1);
  srMET.SetVar("nbjets", 0, 3);
  srMET.SetVar("met", 500, -1);
  srMET.SetVar("ht", 200, -1);
  srMET.SetMT2Bins(3, mtbins);
  SRVec.push_back(srMET);

  SR srHT;
  srHT.SetName("HT");
  srHT.SetVar("njets", 2, -1);
  srHT.SetVar("nbjets", 0, 3);
  srHT.SetVar("met", 200, 500);
  srHT.SetVar("ht", 1000, -1);
  srHT.SetMT2Bins(3, mtbins);
  SRVec.push_back(srHT);

  SR srJ1L;
  srJ1L.SetName("J1L");
  srJ1L.SetVar("njets", 1, 2);
  srJ1L.SetVar("nbjets", 0, 1);
  srJ1L.SetVar("ht", 200, 300);
  srJ1L.SetVar("met", 200, -1);
  srJ1L.SetMT2Bins(3, mtbins);
  SRVec.push_back(srJ1L);
  SR srJ1M;
  srJ1M.SetName("J1M");
  srJ1M.SetVar("njets", 1, 2);
  srJ1M.SetVar("nbjets", 0, 1);
  srJ1M.SetVar("ht", 300, -1);
  srJ1M.SetVar("met", 200, -1);
  srJ1M.SetMT2Bins(3, mtbins);
  SRVec.push_back(srJ1M);
  SR srJ2L;
  srJ2L.SetName("J2L");
  srJ2L.SetVar("njets", 1, 2);
  srJ2L.SetVar("nbjets", 1, -1);
  srJ2L.SetVar("ht", 200, -1);
  srJ2L.SetVar("met", 200, -1);
  srJ2L.SetMT2Bins(3, mtbins);
  SRVec.push_back(srJ2L);
  

  //define baseline selections commmon to all signal regions 
  baseSR.SetVar("mt2", 100, -1);
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
  inclusivebase.SetVar("met", 200, -1);
  inclusivebase.SetVar("mt", 20, -1);
  float mtbinsBase[4] = {20, 90, 120, 300};      
  inclusivebase.SetMT2Bins(3, mtbinsBase);
  SRVec.push_back(inclusivebase);

  SR inclusivebaseJ1 = baseSR;  
  inclusivebaseJ1.SetName("baseJ1");
  inclusivebaseJ1.SetVar("njets", 1, 2);
  inclusivebaseJ1.SetVar("nbjets", 0, 1);
  inclusivebaseJ1.SetVar("ht", 200, -1);
  inclusivebaseJ1.SetVar("met", 200, -1);
  inclusivebaseJ1.SetVar("mt", 20, -1);
  inclusivebaseJ1.SetMT2Bins(3, mtbinsBase);
  SRVec.push_back(inclusivebaseJ1);

  SR inclusivebaseJ2 = baseSR;  
  inclusivebaseJ2.SetName("baseJ2");
  inclusivebaseJ2.SetVar("njets", 1, 2);
  inclusivebaseJ2.SetVar("nbjets", 1, -1);
  inclusivebaseJ2.SetVar("ht", 200, -1);
  inclusivebaseJ2.SetVar("met", 200, -1);
  inclusivebaseJ2.SetVar("mt", 20, -1);
  inclusivebaseJ2.SetMT2Bins(3, mtbinsBase);
  SRVec.push_back(inclusivebaseJ2);


  return SRVec;

}
  
std::vector<SR> getSignalRegionsLep4(){

  std::vector<SR> temp_SR_vec;
  std::vector<SR> SRVec;
  SR sr;
  SR baseSR;
  
  //first set binning in njet-nbjet plane
  sr.SetName("1");
  sr.SetVar("njets", 2, 4);
  sr.SetVar("nbjets", 0, 1);
  sr.SetVar("nbjetshard", 0, 1);
  temp_SR_vec.push_back(sr);
  sr.Clear();
  
  sr.SetName("2");
  sr.SetVar("njets", 2, 4);
  sr.SetVar("nbjets", 1, 3);
  sr.SetVar("nbjetshard", 0, 1);
  temp_SR_vec.push_back(sr);
  sr.Clear();
  
  sr.SetName("3");
  sr.SetVar("njets", 2, 4);
  sr.SetVar("nbjets", 1, 3);
  sr.SetVar("nbjetshard", 1, 3);
  temp_SR_vec.push_back(sr);
  sr.Clear();
  
  sr.SetName("4");
  sr.SetVar("njets", 4, 6);
  sr.SetVar("nbjets", 0, 1);
  sr.SetVar("nbjetshard", 0, 1);
  temp_SR_vec.push_back(sr);
  sr.Clear();
  
  sr.SetName("5");
  sr.SetVar("njets", 4, -1);
  sr.SetVar("nbjets", 1, 3);
  sr.SetVar("nbjetshard", 0, 1);
  temp_SR_vec.push_back(sr);
  sr.Clear();

  sr.SetName("6");
  sr.SetVar("njets", 4, -1);
  sr.SetVar("nbjets", 1, 3);
  sr.SetVar("nbjetshard", 1, 3);
  temp_SR_vec.push_back(sr);
  sr.Clear();

  sr.SetName("7");
  sr.SetVar("njets", 6, -1);
  sr.SetVar("nbjets", 0, 1);
  sr.SetVar("nbjetshard", 0, 1);
  temp_SR_vec.push_back(sr);
  sr.Clear();
 
  //add HT and MET requirements
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "L");
    fullSR.SetVar("met", 200, 300);
    fullSR.SetVar("ht", 200, 1000);
    float mtbins[4] = {20, 90, 120, 300};      fullSR.SetMT2Bins(3, mtbins);
    SRVec.push_back(fullSR);
  }

  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR fullSR = temp_SR_vec.at(iSR);  
    fullSR.SetName(fullSR.GetName() + "M");
    fullSR.SetVar("met", 300, 500);
    fullSR.SetVar("ht", 200, 1000);
    float mtbins[4] = {20, 90, 120, 300};      fullSR.SetMT2Bins(3, mtbins);
    SRVec.push_back(fullSR);
  }
//  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
//    SR fullSR = temp_SR_vec.at(iSR);  
//    fullSR.SetName(fullSR.GetName() + "H");
//    fullSR.SetVar("ht", 600, 1000);
//    float mtbins[4] = {20, 90, 120, 300};      fullSR.SetMT2Bins(3, mtbins);
//    SRVec.push_back(fullSR);
//  }

  SR sr3B;
  sr3B.SetName("B3");
  sr3B.SetVar("njets", 2, -1);
  sr3B.SetVar("nbjets", 3, -1);
  sr3B.SetVar("nbjetshard", 0, -1);
  sr3B.SetVar("met", 200, -1);
  sr3B.SetVar("ht", 200, -1);
  float mtbins[4] = {20, 90, 120, 300};      sr3B.SetMT2Bins(3, mtbins);
  SRVec.push_back(sr3B);

  SR srMET;
  srMET.SetName("MET");
  srMET.SetVar("njets", 2, -1);
  srMET.SetVar("nbjets", 0, 3);
  srMET.SetVar("nbjetshard", 0, 3);
  srMET.SetVar("met", 500, -1);
  srMET.SetVar("ht", 200, -1);
  srMET.SetMT2Bins(3, mtbins);
  SRVec.push_back(srMET);

  SR srHT;
  srHT.SetName("HT");
  srHT.SetVar("njets", 2, -1);
  srHT.SetVar("nbjets", 0, 3);
  srHT.SetVar("nbjetshard", 0, 3);
  srHT.SetVar("met", 200, 500);
  srHT.SetVar("ht", 1000, -1);
  srHT.SetMT2Bins(3, mtbins);
  SRVec.push_back(srHT);

  SR srJ1L;
  srJ1L.SetName("J1L");
  srJ1L.SetVar("njets", 1, 2);
  srJ1L.SetVar("nbjets", 0, 1);
  srJ1L.SetVar("nbjetshard", 0, 1);
  srJ1L.SetVar("ht", 200, 300);
  srJ1L.SetVar("met", 200, -1);
  srJ1L.SetMT2Bins(3, mtbins);
  SRVec.push_back(srJ1L);
  SR srJ1M;
  srJ1M.SetName("J1M");
  srJ1M.SetVar("njets", 1, 2);
  srJ1M.SetVar("nbjets", 0, 1);
  srJ1M.SetVar("nbjetshard", 0, 1);
  srJ1M.SetVar("ht", 300, -1);
  srJ1M.SetVar("met", 200, -1);
  srJ1M.SetMT2Bins(3, mtbins);
  SRVec.push_back(srJ1M);
  SR srJ2L;
  srJ2L.SetName("J2L");
  srJ2L.SetVar("njets", 1, 2);
  srJ2L.SetVar("nbjets", 1, -1);
  srJ2L.SetVar("nbjetshard", 0, 1);
  srJ2L.SetVar("ht", 200, -1);
  srJ2L.SetVar("met", 200, -1);
  srJ2L.SetMT2Bins(3, mtbins);
  SRVec.push_back(srJ2L);
  SR srJ3L;
  srJ3L.SetName("J3L");
  srJ3L.SetVar("njets", 1, 2);
  srJ3L.SetVar("nbjets", 1, -1);
  srJ3L.SetVar("nbjetshard", 1, -1);
  srJ3L.SetVar("ht", 200, -1);
  srJ3L.SetVar("met", 200, -1);
  srJ3L.SetMT2Bins(3, mtbins);
  SRVec.push_back(srJ3L);
  

  //define baseline selections commmon to all signal regions 
  baseSR.SetVar("mt2", 0, -1);
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

  //inclusive regions for njet/nbjet topological bins
  for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
    SR topologicalSR = temp_SR_vec.at(iSR);  
    topologicalSR.SetName("base"+topologicalSR.GetName());
    topologicalSR.SetVar("mt2", 0, -1);
    topologicalSR.SetVar("deltaPhiMin", 0.3, -1);
    topologicalSR.SetVar("diffMetMhtOverMet", 0, 0.5);
    topologicalSR.SetVar("nlep", 1, 2);
    topologicalSR.SetVar("ht", 200, -1);
    topologicalSR.SetVar("met", 200, -1);
    topologicalSR.SetVar("mt", 20, -1);
    float mtbins[4] = {20, 90, 120, 300};      topologicalSR.SetMT2Bins(3, mtbins);
    SRVec.push_back(topologicalSR);
  }

  // Also put in the inclusive region2
  SR inclusivebase = baseSR;  
  inclusivebase.SetName("base");
  inclusivebase.SetVar("njets", 2, -1);
  inclusivebase.SetVar("nbjets", 0, -1);
  inclusivebase.SetVar("nbjetshard", 0, -1);
  inclusivebase.SetVar("ht", 200, -1);
  inclusivebase.SetVar("met", 200, -1);
  inclusivebase.SetVar("mt", 20, -1);
  float mtbinsBase[4] = {20, 90, 120, 300};      
  inclusivebase.SetMT2Bins(3, mtbinsBase);
  SRVec.push_back(inclusivebase);

  SR inclusivebaseHighHT = baseSR;  
  inclusivebaseHighHT.SetName("baseHighHT");
  inclusivebaseHighHT.SetVar("njets", 2, -1);
  inclusivebaseHighHT.SetVar("nbjets", 0, -1);
  inclusivebaseHighHT.SetVar("nbjetshard", 0, -1);
  inclusivebaseHighHT.SetVar("ht", 500, -1);
  inclusivebaseHighHT.SetVar("met", 200, -1);
  inclusivebaseHighHT.SetVar("mt", 20, -1);  
  inclusivebaseHighHT.SetMT2Bins(3, mtbinsBase);
  SRVec.push_back(inclusivebaseHighHT);

  SR inclusivebaseMedHT = baseSR;  
  inclusivebaseMedHT.SetName("baseMedHT");
  inclusivebaseMedHT.SetVar("njets", 2, -1);
  inclusivebaseMedHT.SetVar("nbjets", 0, -1);
  inclusivebaseMedHT.SetVar("nbjetshard", 0, -1);
  inclusivebaseMedHT.SetVar("ht", 300, -1);
  inclusivebaseMedHT.SetVar("met", 200, -1);
  inclusivebaseMedHT.SetVar("mt", 20, -1);  
  inclusivebaseMedHT.SetMT2Bins(3, mtbinsBase);
  SRVec.push_back(inclusivebaseMedHT);

  SR inclusivebaseZeroB = baseSR;  
  inclusivebaseZeroB.SetName("baseZeroB");
  inclusivebaseZeroB.SetVar("njets", 2, -1);
  inclusivebaseZeroB.SetVar("nbjets", 0, 1);
  inclusivebaseZeroB.SetVar("nbjetshard", 0, 1);
  inclusivebaseZeroB.SetVar("ht", 200, -1);
  inclusivebaseZeroB.SetVar("met", 200, -1);
  inclusivebaseZeroB.SetVar("mt", 20, -1);
  inclusivebaseZeroB.SetMT2Bins(3, mtbinsBase);
  SRVec.push_back(inclusivebaseZeroB);

  SR inclusivebaseWithB = baseSR;  
  inclusivebaseWithB.SetName("baseWithB");
  inclusivebaseWithB.SetVar("njets", 2, -1);
  inclusivebaseWithB.SetVar("nbjets", 1, -1);
  inclusivebaseWithB.SetVar("nbjetshard", 0, -1);
  inclusivebaseWithB.SetVar("ht", 200, -1);
  inclusivebaseWithB.SetVar("met", 200, -1);
  inclusivebaseWithB.SetVar("mt", 20, -1);
  inclusivebaseWithB.SetMT2Bins(3, mtbinsBase);
  SRVec.push_back(inclusivebaseWithB);
  
  SR inclusivebaseAll = baseSR;  
  inclusivebaseAll.SetName("baseAll");
  inclusivebaseAll.SetVar("njets", 1, -1);
  inclusivebaseAll.SetVar("nbjets", 0, -1);
  inclusivebaseAll.SetVar("nbjetshard", 0, -1);
  inclusivebaseAll.SetVar("ht", 200, -1);
  inclusivebaseAll.SetVar("met", 200, -1);
  inclusivebaseAll.SetVar("mt", 20, -1);
  inclusivebaseAll.SetMT2Bins(3, mtbinsBase);
  SRVec.push_back(inclusivebaseAll);
  
  SR inclusivebaseJ = baseSR;  
  inclusivebaseJ.SetName("baseJ");
  inclusivebaseJ.SetVar("njets", 1, 2);
  inclusivebaseJ.SetVar("nbjets", 0, -1);
  inclusivebaseJ.SetVar("nbjetshard", 0, -1);
  inclusivebaseJ.SetVar("ht", 200, -1);
  inclusivebaseJ.SetVar("met", 200, -1);
  inclusivebaseJ.SetVar("mt", 20, -1);
  inclusivebaseJ.SetMT2Bins(3, mtbinsBase);
  SRVec.push_back(inclusivebaseJ);

  SR inclusivebaseJ1 = baseSR;  
  inclusivebaseJ1.SetName("baseJ1");
  inclusivebaseJ1.SetVar("njets", 1, 2);
  inclusivebaseJ1.SetVar("nbjets", 0, 1);
  inclusivebaseJ1.SetVar("nbjetshard", 0, 1);
  inclusivebaseJ1.SetVar("ht", 200, -1);
  inclusivebaseJ1.SetVar("met", 200, -1);
  inclusivebaseJ1.SetVar("mt", 20, -1);
  inclusivebaseJ1.SetMT2Bins(3, mtbinsBase);
  SRVec.push_back(inclusivebaseJ1);

  return SRVec;

}
  


  
} // namespace mt2
