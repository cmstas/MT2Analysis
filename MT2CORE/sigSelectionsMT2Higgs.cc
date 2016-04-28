#include "sigSelectionsMT2Higgs.h"

namespace mt2 {

  std::vector<SR> getSignalRegionsMT2Higgs(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;
  
    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 3, -1);
    sr.SetVar("nbjets", 2, -1);
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

    return SRVec;

  }
  
  
} // namespace mt2
