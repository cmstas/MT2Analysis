#include "sigSelections.h"

namespace mt2 {

  std::vector<SR> getSignalRegions2012(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 3);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("mt2", 200, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 3);
    sr.SetVar("nbjets", 1, 3);
    sr.SetVar("mt2", 200, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 3, 6);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("mt2", 200, 400);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 3, 6);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("mt2", 200, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 3, 6);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("mt2", 200, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 6, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("mt2", 200, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 6, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("mt2", 200, 400);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("8");
    sr.SetVar("njets", 6, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("mt2", 200, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("9");
    sr.SetVar("njets", 3, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("mt2", 200, 400);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    //add HT and MET requirements
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 750);
      fullSR.SetVar("met", 200, -1);
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 750, 1200);
      fullSR.SetVar("met", 30, -1);
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1200, -1);
      fullSR.SetVar("met", 30, -1);
      SRVec.push_back(fullSR);
    }

    //define baseline selections commmon to all signal regions 
    //baseSR.SetVar("mt2", 200, -1);
    baseSR.SetVar("j1pt", 100, -1);
    baseSR.SetVar("j2pt", 100, -1);
    baseSR.SetVar("deltaPhiMin", 0.3, -1);
    baseSR.SetVar("diffMetMht", 0, 70);
    baseSR.SetVar("nlep", 0, 1);
    //baseSR.SetVar("passesHtMet", 1, 1);

    //add baseline selections to all signal regions 
    std::vector<std::string> vars = baseSR.GetListOfVariables();
    for(unsigned int i = 0; i < SRVec.size(); i++){
      for(unsigned int j = 0; j < vars.size(); j++){
        SRVec.at(i).SetVar(vars.at(j), baseSR.GetLowerBound(vars.at(j)), baseSR.GetUpperBound(vars.at(j)));
      }
    }

    return SRVec;

  }

  std::vector<SR> getSignalRegions2015LowLumi(){//used for AN-15-009

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("8");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("9");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    //add HT and MET requirements
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 200, -1);
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1000);
      fullSR.SetVar("met", 200, -1);
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1000, -1);
      fullSR.SetVar("met", 30, -1);
      SRVec.push_back(fullSR);
    }

    //define baseline selections commmon to all signal regions 
    baseSR.SetVar("mt2", 200, -1);
    baseSR.SetVar("j1pt", 100, -1);
    baseSR.SetVar("j2pt", 100, -1);
    baseSR.SetVar("deltaPhiMin", 0.3, -1);
    baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
    baseSR.SetVar("nlep", 0, 1);
    //baseSR.SetVar("passesHtMet", 1, 1);

    //add baseline selections to all signal regions 
    std::vector<std::string> vars = baseSR.GetListOfVariables();
    for(unsigned int i = 0; i < SRVec.size(); i++){
      for(unsigned int j = 0; j < vars.size(); j++){
        SRVec.at(i).SetVar(vars.at(j), baseSR.GetLowerBound(vars.at(j)), baseSR.GetUpperBound(vars.at(j)));
      }
    }

    return SRVec;

  }


  std::vector<SR> getSignalRegions2015ExtendedNJets(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("8");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("9");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("11");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("12");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("13");
    sr.SetVar("njets", 9, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("14");
    sr.SetVar("njets", 9, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("15");
    sr.SetVar("njets", 9, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("16");
    sr.SetVar("njets", 9, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("17");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("18");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("19");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("20");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();


    //add HT and MET requirements
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 200, -1);
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1000);
      fullSR.SetVar("met", 200, -1);
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1000, -1);
      fullSR.SetVar("met", 30, -1);
      SRVec.push_back(fullSR);
    }

    //define baseline selections commmon to all signal regions 
    baseSR.SetVar("mt2", 200, -1);
    baseSR.SetVar("j1pt", 100, -1);
    baseSR.SetVar("j2pt", 100, -1);
    baseSR.SetVar("deltaPhiMin", 0.3, -1);
    baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
    baseSR.SetVar("nlep", 0, 1);
    //baseSR.SetVar("passesHtMet", 1, 1);

    //add baseline selections to all signal regions 
    std::vector<std::string> vars = baseSR.GetListOfVariables();
    for(unsigned int i = 0; i < SRVec.size(); i++){
      for(unsigned int j = 0; j < vars.size(); j++){
        SRVec.at(i).SetVar(vars.at(j), baseSR.GetLowerBound(vars.at(j)), baseSR.GetUpperBound(vars.at(j)));
      }
    }

    return SRVec;

  }

  std::vector<SR> getSignalRegions2015ExtendedNJets_UltraHighHT(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("8");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("9");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("11");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("12");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("13");
    sr.SetVar("njets", 9, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("14");
    sr.SetVar("njets", 9, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("15");
    sr.SetVar("njets", 9, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("16");
    sr.SetVar("njets", 9, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("17");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("18");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("19");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("20");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();


    //add HT and MET requirements
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 200, -1);
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1000);
      fullSR.SetVar("met", 200, -1);
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1000, 1500);
      fullSR.SetVar("met", 30, -1);
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "UH");
      fullSR.SetVar("ht", 1500, -1);
      fullSR.SetVar("met", 30, -1);
      SRVec.push_back(fullSR);
    }

    //define baseline selections commmon to all signal regions 
    baseSR.SetVar("mt2", 200, -1);
    baseSR.SetVar("j1pt", 100, -1);
    baseSR.SetVar("j2pt", 100, -1);
    baseSR.SetVar("deltaPhiMin", 0.3, -1);
    baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
    baseSR.SetVar("nlep", 0, 1);
    //baseSR.SetVar("passesHtMet", 1, 1);

    //add baseline selections to all signal regions 
    std::vector<std::string> vars = baseSR.GetListOfVariables();
    for(unsigned int i = 0; i < SRVec.size(); i++){
      for(unsigned int j = 0; j < vars.size(); j++){
        SRVec.at(i).SetVar(vars.at(j), baseSR.GetLowerBound(vars.at(j)), baseSR.GetUpperBound(vars.at(j)));
      }
    }

    return SRVec;

  }


  std::vector<SR> getSignalRegions2015SevenJets_UltraHighHT(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("8");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("9");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVar("lowMT", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("11");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("12");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("13");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("14");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("15");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("lowMT", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("16");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("lowMT", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();


    //add HT and MET requirements
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 200, -1);
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1000);
      fullSR.SetVar("met", 200, -1);
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1000, 1500);
      fullSR.SetVar("met", 30, -1);
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "UH");
      fullSR.SetVar("ht", 1500, -1);
      fullSR.SetVar("met", 30, -1);
      SRVec.push_back(fullSR);
    }

    //define baseline selections commmon to all signal regions 
    baseSR.SetVar("mt2", 200, -1);
    baseSR.SetVar("j1pt", 100, -1);
    baseSR.SetVar("j2pt", 100, -1);
    baseSR.SetVar("deltaPhiMin", 0.3, -1);
    baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
    baseSR.SetVar("nlep", 0, 1);
    //baseSR.SetVar("passesHtMet", 1, 1);

    //add baseline selections to all signal regions 
    std::vector<std::string> vars = baseSR.GetListOfVariables();
    for(unsigned int i = 0; i < SRVec.size(); i++){
      for(unsigned int j = 0; j < vars.size(); j++){
        SRVec.at(i).SetVar(vars.at(j), baseSR.GetLowerBound(vars.at(j)), baseSR.GetUpperBound(vars.at(j)));
      }
    }

    return SRVec;

  }

  std::vector<SR> getSignalRegionsZurich(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("8");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("9");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("11");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();


    //add HT and MET requirements
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 200, -1);
      fullSR.SetVarCRSL("ht", 450, 575);
      fullSR.SetVarCRSL("met", 200, -1);
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
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[2] = {200, 1500};                      fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[2] = {200, 1500};                      fullSR.SetMT2Bins(1, mt2bins);}
      SRVec.push_back(fullSR);
    }

    //define baseline selections commmon to all signal regions 
    baseSR.SetVar("mt2", 200, -1);
    baseSR.SetVar("j1pt", 100, -1);
    baseSR.SetVar("j2pt", 100, -1);
    baseSR.SetVar("deltaPhiMin", 0.3, -1);
    baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
    baseSR.SetVar("nlep", 0, 1);
    //baseSR.SetVar("passesHtMet", 1, 1);

    // common selections for CRSL
    baseSR.SetVarCRSL("mt2", 200, -1);
    baseSR.SetVarCRSL("j1pt", 100, -1);
    baseSR.SetVarCRSL("j2pt", 100, -1);
    baseSR.SetVarCRSL("deltaPhiMin", 0.3, -1);
    baseSR.SetVarCRSL("diffMetMhtOverMet", 0, 0.5);
    baseSR.SetVarCRSL("nlep", 1, 2);

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

    return SRVec;

  }

  std::vector<SR> getSignalRegionsZurich_jetpt40(){

    std::vector<SR> SRVec = getSignalRegionsZurich();

    //change j1pt and j2pt cuts to 40 GeV
    for(unsigned int i = 0; i < SRVec.size(); i++){
      SRVec.at(i).RemoveVar("j1pt");
      SRVec.at(i).RemoveVar("j2pt");
      SRVec.at(i).SetVar("j1pt", 40, -1);
      SRVec.at(i).SetVar("j2pt", 40, -1);

      SRVec.at(i).RemoveVarCRSL("j1pt");
      SRVec.at(i).RemoveVarCRSL("j2pt");
      SRVec.at(i).SetVarCRSL("j1pt", 40, -1);
      SRVec.at(i).SetVarCRSL("j2pt", 40, -1);
    }

    return SRVec;

  }

  std::vector<SR> getSignalRegionsZurich_jetpt30(){

    std::vector<SR> SRVec = getSignalRegionsZurich();

    //change j1pt and j2pt cuts to 30 GeV
    for(unsigned int i = 0; i < SRVec.size(); i++){
      SRVec.at(i).RemoveVar("j1pt");
      SRVec.at(i).RemoveVar("j2pt");
      SRVec.at(i).SetVar("j1pt", 30, -1);
      SRVec.at(i).SetVar("j2pt", 30, -1);

      SRVec.at(i).RemoveVarCRSL("j1pt");
      SRVec.at(i).RemoveVarCRSL("j2pt");
      SRVec.at(i).SetVarCRSL("j1pt", 30, -1);
      SRVec.at(i).SetVarCRSL("j2pt", 30, -1);
    }

    return SRVec;

  }

  std::vector<SR> getSignalRegionsJamboree(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("8");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("9");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("11");
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
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[2] = {200, 1500};                      fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[2] = {200, 1500};                      fullSR.SetMT2Bins(1, mt2bins);}
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
  

  std::vector<SR> getSignalRegionsMonojet(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR baseSR;

    // define baseline selections commmon to all monojet regions
    //  ETH doesn't explictly cut on jet1_pt for SR or CRSL, just ht, so we won't either for this iteration
    float mt2bins_monojet[2] = {0, 1500};
    //baseSR.SetVar("j1pt", 200, -1);
    baseSR.SetVar("nlep", 0, 1);
    baseSR.SetVar("njets", 1, 2);
    baseSR.SetVar("met", 200, -1);
    baseSR.SetVar("deltaPhiMin", 0.3, -1);
    baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
    //baseSR.SetVarCRSL("j1pt", 200, -1);
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
    const unsigned int nbins_monojet_0b = 7;
    float htbins_0b[nbins_monojet_0b+1] = {200, 250, 350, 450, 575, 700, 1000, -1};
    float htbins_0b_forplot[nbins_monojet_0b+1] = {200, 250, 350, 450, 575, 700, 1000, 1500};
    const unsigned int nbins_monojet_1b = 5;
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


  std::vector<SR> getSignalRegionsICHEP(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("8");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("9");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("11");
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
  
  std::vector<SR> getSignalRegionsMonojet2016(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR baseSR;

    // define baseline selections commmon to all monojet regions
    //  ETH doesn't explictly cut on jet1_pt for SR or CRSL, just ht, so we won't either for this iteration
    float mt2bins_monojet[2] = {0, 1500};
    //baseSR.SetVar("j1pt", 200, -1);
    baseSR.SetVar("nlep", 0, 1);
    baseSR.SetVar("njets", 1, 2);
    baseSR.SetVar("met", 250, -1);
    baseSR.SetVar("deltaPhiMin", 0.3, -1);
    baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
    //baseSR.SetVarCRSL("j1pt", 200, -1);
    baseSR.SetVarCRSL("nlep", 1, 2);
    baseSR.SetVarCRSL("njets", 1, 2);
    baseSR.SetVarCRSL("met", 250, -1);
    baseSR.SetVarCRSL("deltaPhiMin", 0.3, -1);
    baseSR.SetVarCRSL("diffMetMhtOverMet", 0, 0.5);
    baseSR.SetVarCRDY("nlep", 0, 1);
    baseSR.SetVarCRDY("njets", 1, 2);
    baseSR.SetVarCRDY("met", 250, -1);
    baseSR.SetVarCRDY("deltaPhiMin", 0.3, -1);
    baseSR.SetVarCRDY("diffMetMhtOverMet", 0, 0.5);
    // QCD region: 2 jets, low deltaPhiMin, pt subleading between 30 and 60 GeV
    baseSR.SetVarCRQCD("j1pt", 250, -1);
    baseSR.SetVarCRQCD("j2pt", 30, 60);
    baseSR.SetVarCRQCD("nlep", 0, 1);
    baseSR.SetVarCRQCD("njets", 2, 3);
    baseSR.SetVarCRQCD("met", 250, -1);
    baseSR.SetVarCRQCD("deltaPhiMin", 0., 0.3);
    baseSR.SetVarCRQCD("diffMetMhtOverMet", 0, 0.5);
    baseSR.SetMT2Bins(1, mt2bins_monojet);

    // fine binning in HT
    const unsigned int nbins_monojet_0b = 7;
    float htbins_0b[nbins_monojet_0b+1] = {250, 350, 450, 575, 700, 1000, 1200, -1};
    float htbins_0b_forplot[nbins_monojet_0b+1] = {250, 350, 450, 575, 700, 1000, 1200, 1500};
    const unsigned int nbins_monojet_1b = 5;
    float htbins_1b[nbins_monojet_1b+1] = {250, 350, 450, 575, 700, -1};
    float htbins_1b_forplot[nbins_monojet_1b+1] = {250, 350, 450, 575, 700, 1500};

    temp_SR_vec.clear();
    for(unsigned int iSR = 0; iSR < nbins_monojet_0b; iSR++){
      SR fullSR0b = baseSR;  
      fullSR0b.SetName(std::to_string(iSR+1) + "J");
      fullSR0b.SetVar("ht", htbins_0b[iSR], htbins_0b[iSR+1]);
      fullSR0b.SetVar("nbjets", 0, 1);
      fullSR0b.SetVarCRSL("ht", htbins_0b[iSR], htbins_0b[iSR+1]);
      fullSR0b.SetVarCRSL("nbjets", 0, 1);
      fullSR0b.SetVarCRDY("ht", htbins_0b[iSR], htbins_0b[iSR+1]);
      fullSR0b.SetVarCRDY("nbjets", 0, 1);
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
      fullSR1b.SetVarCRDY("ht", htbins_1b[iSR], htbins_1b[iSR+1]);
      fullSR1b.SetVarCRDY("nbjets", 1, -1);
      fullSR1b.SetVarCRQCD("ht", htbins_1b[iSR], htbins_1b[iSR+1]);
      fullSR1b.SetVarCRQCD("nbjets", 1, -1);
      SRVec.push_back(fullSR1b);
    }


    // Also put in some inclusive regions: 0b, 1b
    SR fullSR0b = baseSR;  
    fullSR0b.SetName("baseJ0B");
    fullSR0b.SetVar("nbjets", 0, 1);
    fullSR0b.SetVarCRSL("nbjets", 0, 1);
    fullSR0b.SetVarCRDY("nbjets", 0, 1);
    fullSR0b.SetVarCRQCD("nbjets", 0, 1);
    fullSR0b.SetVar("ht", 250, -1);
    fullSR0b.SetVarCRSL("ht", 250, -1);
    fullSR0b.SetVarCRDY("ht", 250, -1);
    fullSR0b.SetVarCRQCD("ht", 250, -1);
    fullSR0b.SetMT2Bins(nbins_monojet_0b, htbins_0b_forplot);
    SRVec.push_back(fullSR0b);

    SR fullSR1b = baseSR;  
    fullSR1b.SetName("baseJ1B");
    fullSR1b.SetVar("nbjets", 1, -1);
    fullSR1b.SetVarCRSL("nbjets", 1, -1);
    fullSR1b.SetVarCRDY("nbjets", 1, -1);
    fullSR1b.SetVarCRQCD("nbjets", 1, -1);
    fullSR1b.SetVar("ht", 250, -1);
    fullSR1b.SetVarCRSL("ht", 250, -1);
    fullSR1b.SetVarCRDY("ht", 250, -1);
    fullSR1b.SetVarCRQCD("ht", 250, -1);
    fullSR1b.SetMT2Bins(nbins_monojet_1b, htbins_1b_forplot);
    SRVec.push_back(fullSR1b);

    return SRVec;

  }  

  std::vector<SR> getSignalRegions2016(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("8");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("9");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("11");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("12");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("13");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("14");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("15");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("16");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("17");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("18");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("19");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();
    
    //add HT and MET requirements
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      if (iSR >= 3 && iSR <= 10) continue;
      if (iSR >= 15) continue;
      SR fullSR = temp_SR_vec.at(iSR); 
      fullSR.SetName(fullSR.GetName() + "VL");
      fullSR.SetVar("ht", 250, 450);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 250, 450);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 250, 450);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 250, 450);
      fullSR.SetVarCRQCD("met", 250, -1);
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
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);      
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 450, 575);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 450, 575);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 450, 575);
      fullSR.SetVarCRQCD("met", 250, -1);
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
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1000);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 575, 1000);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 575, 1000);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 575, 1000);
      fullSR.SetVarCRQCD("met", 250, -1);
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
      if (iSR >= 11) continue;      
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1000, 1500);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", 1000, 1500);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", 1000, 1500);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", 1000, 1500);
      fullSR.SetVarCRQCD("met", 30, -1);
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
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "UH");
      fullSR.SetVar("ht", 1500, -1);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", 1500, -1);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", 1500, -1);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", 1500, -1);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      int njets_hi = fullSR.GetUpperBound("njets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1400, 1800}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){
        float mt2bins[3] = {200, 400, 1800};
        float mt2bins_ssr[4] = {200, 400, 600, 1800};
        if (njets_hi == -1)
          fullSR.SetMT2Bins(3, mt2bins_ssr);
        else
          fullSR.SetMT2Bins(2, mt2bins);          
      }      
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

    // --- super signal regions, numbered 20 and up
    //     inclusive in HT, NJ, NB
    //     MT2 binning matches the most granular TR that would be included
    //     MT2 cut will be applied later by summing appropriate datacards

    // 2j loose
    sr.SetName("20");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr20[] = {200, 400, 600, 800, 1000, 1200, 1800};
    sr.SetMT2Bins(6, mt2bins_sr20);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 2j tight
    sr.SetName("21");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr21[] = {200, 400, 600, 800, 1000, 1400, 1800};
    sr.SetMT2Bins(6, mt2bins_sr21);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 4j loose
    sr.SetName("22");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr22[] = {200, 400, 600, 800, 1000, 1200, 1800};
    sr.SetMT2Bins(6, mt2bins_sr22);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 4j tight
    sr.SetName("23");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr23[] = {200, 400, 600, 800, 1000, 1400, 1800};
    sr.SetMT2Bins(6, mt2bins_sr23);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j loose
    sr.SetName("24");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr24[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr24);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j tight
    sr.SetName("25");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr25[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr25);
    SRVec.push_back(sr);
    sr.Clear();
    

    // 2b loose
    sr.SetName("26");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr26[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr26);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 2b tight
    sr.SetName("27");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr27[] = {200, 400, 600, 800, 1800};
    sr.SetMT2Bins(4, mt2bins_sr27);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 3b loose
    sr.SetName("28");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr28[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr28);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 3b tight
    sr.SetName("29");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr29[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr29);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j3b loose
    sr.SetName("30");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr30[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr30);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j3b tight
    sr.SetName("31");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr31[] = {200, 400, 1800};
    sr.SetMT2Bins(2, mt2bins_sr31);
    SRVec.push_back(sr);
    sr.Clear();
    
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

    //add baseline selections to all CRDY regions 
    std::vector<std::string> varsCRDY = baseSR.GetListOfVariablesCRDY();
    for(unsigned int i = 0; i < SRVec.size(); i++){
      for(unsigned int j = 0; j < varsCRDY.size(); j++){
        SRVec.at(i).SetVarCRDY(varsCRDY.at(j), baseSR.GetLowerBoundCRDY(varsCRDY.at(j)), baseSR.GetUpperBoundCRDY(varsCRDY.at(j)));
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

  
  std::vector<SR> getSignalRegionsMonojet2017(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR baseSR;

    // define baseline selections commmon to all monojet regions
    //  ETH doesn't explictly cut on jet1_pt for SR or CRSL, just ht, so we won't either for this iteration
    float mt2bins_monojet[2] = {0, 1500};
    //baseSR.SetVar("j1pt", 200, -1);
    baseSR.SetVar("nlep", 0, 1);
    baseSR.SetVar("njets", 1, 2);
    baseSR.SetVar("met", 250, -1);
    baseSR.SetVar("deltaPhiMin", 0.3, -1);
    baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
    //baseSR.SetVarCRSL("j1pt", 200, -1);
    baseSR.SetVarCRSL("nlep", 1, 2);
    baseSR.SetVarCRSL("njets", 1, 2);
    baseSR.SetVarCRSL("met", 250, -1);
    baseSR.SetVarCRSL("deltaPhiMin", 0.3, -1);
    baseSR.SetVarCRSL("diffMetMhtOverMet", 0, 0.5);
    baseSR.SetVarCRDY("nlep", 0, 1);
    baseSR.SetVarCRDY("njets", 1, 2);
    baseSR.SetVarCRDY("met", 250, -1);
    baseSR.SetVarCRDY("deltaPhiMin", 0.3, -1);
    baseSR.SetVarCRDY("diffMetMhtOverMet", 0, 0.5);
    // QCD region: 2 jets, low deltaPhiMin, pt subleading between 30 and 60 GeV
    baseSR.SetVarCRQCD("j1pt", 250, -1);
    baseSR.SetVarCRQCD("j2pt", 30, 60);
    baseSR.SetVarCRQCD("nlep", 0, 1);
    baseSR.SetVarCRQCD("njets", 2, 3);
    baseSR.SetVarCRQCD("met", 250, -1);
    baseSR.SetVarCRQCD("deltaPhiMin", 0., 0.3);
    baseSR.SetVarCRQCD("diffMetMhtOverMet", 0, 0.5);
    baseSR.SetMT2Bins(1, mt2bins_monojet);

    // fine binning in HT
    const unsigned int nbins_monojet_0b = 7;
    float htbins_0b[nbins_monojet_0b+1] = {250, 350, 450, 575, 700, 1000, 1200, -1};
    float htbins_0b_forplot[nbins_monojet_0b+1] = {250, 350, 450, 575, 700, 1000, 1200, 1500};
    const unsigned int nbins_monojet_1b = 5;
    float htbins_1b[nbins_monojet_1b+1] = {250, 350, 450, 575, 700, -1};
    float htbins_1b_forplot[nbins_monojet_1b+1] = {250, 350, 450, 575, 700, 1500};

    temp_SR_vec.clear();
    for(unsigned int iSR = 0; iSR < nbins_monojet_0b; iSR++){
      SR fullSR0b = baseSR;  
      fullSR0b.SetName(std::to_string(iSR+1) + "J");
      fullSR0b.SetVar("ht", htbins_0b[iSR], htbins_0b[iSR+1]);
      fullSR0b.SetVar("nbjets", 0, 1);
      fullSR0b.SetVarCRSL("ht", htbins_0b[iSR], htbins_0b[iSR+1]);
      fullSR0b.SetVarCRSL("nbjets", 0, 1);
      fullSR0b.SetVarCRDY("ht", htbins_0b[iSR], htbins_0b[iSR+1]);
      fullSR0b.SetVarCRDY("nbjets", 0, 1);
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
      fullSR1b.SetVarCRDY("ht", htbins_1b[iSR], htbins_1b[iSR+1]);
      fullSR1b.SetVarCRDY("nbjets", 1, -1);
      fullSR1b.SetVarCRQCD("ht", htbins_1b[iSR], htbins_1b[iSR+1]);
      fullSR1b.SetVarCRQCD("nbjets", 1, -1);
      SRVec.push_back(fullSR1b);
    }


    // Also put in some inclusive regions: 0b, 1b
    SR fullSR0b = baseSR;  
    fullSR0b.SetName("baseJ0B");
    fullSR0b.SetVar("nbjets", 0, 1);
    fullSR0b.SetVarCRSL("nbjets", 0, 1);
    fullSR0b.SetVarCRDY("nbjets", 0, 1);
    fullSR0b.SetVarCRQCD("nbjets", 0, 1);
    fullSR0b.SetVar("ht", 250, -1);
    fullSR0b.SetVarCRSL("ht", 250, -1);
    fullSR0b.SetVarCRDY("ht", 250, -1);
    fullSR0b.SetVarCRQCD("ht", 250, -1);
    fullSR0b.SetMT2Bins(nbins_monojet_0b, htbins_0b_forplot);
    SRVec.push_back(fullSR0b);

    SR fullSR1b = baseSR;  
    fullSR1b.SetName("baseJ1B");
    fullSR1b.SetVar("nbjets", 1, -1);
    fullSR1b.SetVarCRSL("nbjets", 1, -1);
    fullSR1b.SetVarCRDY("nbjets", 1, -1);
    fullSR1b.SetVarCRQCD("nbjets", 1, -1);
    fullSR1b.SetVar("ht", 250, -1);
    fullSR1b.SetVarCRSL("ht", 250, -1);
    fullSR1b.SetVarCRDY("ht", 250, -1);
    fullSR1b.SetVarCRQCD("ht", 250, -1);
    fullSR1b.SetMT2Bins(nbins_monojet_1b, htbins_1b_forplot);
    SRVec.push_back(fullSR1b);

    return SRVec;

  }  


  std::vector<SR> getSignalRegions2017(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("8");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("9");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("11");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("12");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("13");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("14");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("15");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("16");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("17");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("18");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("19");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();
    
    //add HT and MET requirements
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      if (iSR >= 3 && iSR <= 10) continue;
      if (iSR >= 15) continue;
      SR fullSR = temp_SR_vec.at(iSR); 
      fullSR.SetName(fullSR.GetName() + "VL");
      fullSR.SetVar("ht", 250, 450);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 250, 450);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 250, 450);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 250, 450);
      fullSR.SetVarCRQCD("met", 250, -1);
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
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);      
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 450, 575);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 450, 575);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 450, 575);
      fullSR.SetVarCRQCD("met", 250, -1);
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
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1200);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 575, 1200);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 575, 1200);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 575, 1200);
      fullSR.SetVarCRQCD("met", 250, -1);
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
      if (iSR >= 11) continue;      
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1200, 1500);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", 1200, 1500);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", 1200, 1500);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", 1200, 1500);
      fullSR.SetVarCRQCD("met", 30, -1);
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
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "UH");
      fullSR.SetVar("ht", 1500, -1);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", 1500, -1);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", 1500, -1);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", 1500, -1);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      int njets_hi = fullSR.GetUpperBound("njets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1400, 1800}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){
        float mt2bins[3] = {200, 400, 1800};
        float mt2bins_ssr[4] = {200, 400, 600, 1800};
        if (njets_hi == -1)
          fullSR.SetMT2Bins(3, mt2bins_ssr);
        else
          fullSR.SetMT2Bins(2, mt2bins);          
      }      
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

    // --- super signal regions, numbered 20 and up
    //     inclusive in HT, NJ, NB
    //     MT2 binning matches the most granular TR that would be included
    //     MT2 cut will be applied later by summing appropriate datacards

    // 2j loose
    sr.SetName("20");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr20[] = {200, 400, 600, 800, 1000, 1200, 1800};
    sr.SetMT2Bins(6, mt2bins_sr20);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 2j tight
    sr.SetName("21");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr21[] = {200, 400, 600, 800, 1000, 1400, 1800};
    sr.SetMT2Bins(6, mt2bins_sr21);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 4j loose
    sr.SetName("22");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr22[] = {200, 400, 600, 800, 1000, 1200, 1800};
    sr.SetMT2Bins(6, mt2bins_sr22);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 4j tight
    sr.SetName("23");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr23[] = {200, 400, 600, 800, 1000, 1400, 1800};
    sr.SetMT2Bins(6, mt2bins_sr23);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j loose
    sr.SetName("24");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr24[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr24);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j tight
    sr.SetName("25");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr25[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr25);
    SRVec.push_back(sr);
    sr.Clear();
    

    // 2b loose
    sr.SetName("26");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr26[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr26);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 2b tight
    sr.SetName("27");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr27[] = {200, 400, 600, 800, 1800};
    sr.SetMT2Bins(4, mt2bins_sr27);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 3b loose
    sr.SetName("28");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr28[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr28);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 3b tight
    sr.SetName("29");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr29[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr29);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j3b loose
    sr.SetName("30");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr30[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr30);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j3b tight
    sr.SetName("31");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    float mt2bins_sr31[] = {200, 400, 1800};
    sr.SetMT2Bins(2, mt2bins_sr31);
    SRVec.push_back(sr);
    sr.Clear();
    
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

    //add baseline selections to all CRDY regions 
    std::vector<std::string> varsCRDY = baseSR.GetListOfVariablesCRDY();
    for(unsigned int i = 0; i < SRVec.size(); i++){
      for(unsigned int j = 0; j < varsCRDY.size(); j++){
        SRVec.at(i).SetVarCRDY(varsCRDY.at(j), baseSR.GetLowerBoundCRDY(varsCRDY.at(j)), baseSR.GetUpperBoundCRDY(varsCRDY.at(j)));
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

  std::vector<SR> getSignalRegionsPJ(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("8");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("9");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("11");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("12");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("13");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("14");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("15");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("16");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("17");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("18");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("19");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();
    
    //add HT and MET requirements
    for(unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++){
      if (iSR >= 3 && iSR <= 10) continue;
      if (iSR >= 15) continue;
      SR fullSR = temp_SR_vec.at(iSR); 
      fullSR.SetName(fullSR.GetName() + "VL");
      fullSR.SetVar("ht", 250, 450);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 250, 450);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 250, 450);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 250, 450);
      fullSR.SetVarCRQCD("met", 250, -1);
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
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);      
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 450, 575);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 450, 575);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 450, 575);
      fullSR.SetVarCRQCD("met", 250, -1);
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
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1200);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 575, 1200);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 575, 1200);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 575, 1200);
      fullSR.SetVarCRQCD("met", 250, -1);
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
      if (iSR >= 11) continue;      
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1200, 1500);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", 1200, 1500);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", 1200, 1500);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", 1200, 1500);
      fullSR.SetVarCRQCD("met", 30, -1);
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
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "UH");
      fullSR.SetVar("ht", 1500, -1);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", 1500, -1);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", 1500, -1);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", 1500, -1);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      int njets_hi = fullSR.GetUpperBound("njets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1400, 1800}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){
        float mt2bins[3] = {200, 400, 1800};
        float mt2bins_ssr[4] = {200, 400, 600, 1800};
        if (njets_hi == -1)
          fullSR.SetMT2Bins(3, mt2bins_ssr);
        else
          fullSR.SetMT2Bins(2, mt2bins);          
      }      
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

    // Split every bin into low and high eta
    std::vector<SR> SRVec_2;
    for (unsigned int iSR = 0; iSR < SRVec.size(); iSR++) {
      SR low = SRVec.at(iSR);
      low.SetVar("PJ1eta",0,1.4);
      low.SetVarCRSL("PJ1eta",0,1.4);
      low.SetVarCRDY("PJ1eta",0,1.4);
      low.SetVarCRQCD("PJ1eta",0,1.4);
      SRVec_2.push_back(low);
      SR high = SRVec.at(iSR);
      high.SetVar("PJ1eta",1.4,-1);
      high.SetVarCRSL("PJ1eta",1.4,-1);
      high.SetVarCRDY("PJ1eta",1.4,-1);
      high.SetVarCRQCD("PJ1eta",1.4,-1);
      std::string name = high.GetName();
      char last = name.back();
      int remove = 1;
      switch (last) {
      case 'L':
	if (name[name.length()-2] == 'V') {
	  remove = 2;
	}
	break;
      case 'M':
	break;
      case 'H':
	if (name[name.length()-2] == 'U') {
	  remove = 2;
	}
	break;
      }
      std::string ht_str = name.substr(0,name.length()-remove);
      int num = atoi(ht_str.c_str());
      num += 19;
      TString newname = Form("%d%s", num, name.substr(name.length()-remove,remove).c_str());
      high.SetName(newname.Data());
      SRVec_2.push_back(high);
    }

    // --- super signal regions, numbered 40 and up
    //     inclusive in HT, NJ, NB
    //     MT2 binning matches the most granular TR that would be included
    //     MT2 cut will be applied later by summing appropriate datacards

    // 2j loose
    sr.SetName("40");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta",0,-1);
    sr.SetVarCRSL("PJ1eta",0,-1);
    sr.SetVarCRDY("PJ1eta",0,-1);
    sr.SetVarCRQCD("PJ1eta",0,-1);
    float mt2bins_sr40[] = {200, 400, 600, 800, 1000, 1200, 1800};
    sr.SetMT2Bins(6, mt2bins_sr40);
    SRVec_2.push_back(sr);
    sr.Clear();
    
    // 2j tight
    sr.SetName("41");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta",0,-1);
    sr.SetVarCRSL("PJ1eta",0,-1);
    sr.SetVarCRDY("PJ1eta",0,-1);
    sr.SetVarCRQCD("PJ1eta",0,-1);
    float mt2bins_sr41[] = {200, 400, 600, 800, 1000, 1400, 1800};
    sr.SetMT2Bins(6, mt2bins_sr41);
    SRVec_2.push_back(sr);
    sr.Clear();
    
    // 4j loose
    sr.SetName("42");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta",0,-1);
    sr.SetVarCRSL("PJ1eta",0,-1);
    sr.SetVarCRDY("PJ1eta",0,-1);
    sr.SetVarCRQCD("PJ1eta",0,-1);
    float mt2bins_sr42[] = {200, 400, 600, 800, 1000, 1200, 1800};
    sr.SetMT2Bins(6, mt2bins_sr42);
    SRVec_2.push_back(sr);
    sr.Clear();
    
    // 4j tight
    sr.SetName("43");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta",0,-1);
    sr.SetVarCRSL("PJ1eta",0,-1);
    sr.SetVarCRDY("PJ1eta",0,-1);
    sr.SetVarCRQCD("PJ1eta",0,-1);
    float mt2bins_sr43[] = {200, 400, 600, 800, 1000, 1400, 1800};
    sr.SetMT2Bins(6, mt2bins_sr43);
    SRVec_2.push_back(sr);
    sr.Clear();
    
    // 7j loose
    sr.SetName("44");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta",0,-1);
    sr.SetVarCRSL("PJ1eta",0,-1);
    sr.SetVarCRDY("PJ1eta",0,-1);
    sr.SetVarCRQCD("PJ1eta",0,-1);
    float mt2bins_sr44[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr44);
    SRVec_2.push_back(sr);
    sr.Clear();
    
    // 7j tight
    sr.SetName("45");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta",0,-1);
    sr.SetVarCRSL("PJ1eta",0,-1);
    sr.SetVarCRDY("PJ1eta",0,-1);
    sr.SetVarCRQCD("PJ1eta",0,-1);
    float mt2bins_sr45[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr45);
    SRVec_2.push_back(sr);
    sr.Clear();
    

    // 2b loose
    sr.SetName("46");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta",0,-1);
    sr.SetVarCRSL("PJ1eta",0,-1);
    sr.SetVarCRDY("PJ1eta",0,-1);
    sr.SetVarCRQCD("PJ1eta",0,-1);
    float mt2bins_sr46[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr46);
    SRVec_2.push_back(sr);
    sr.Clear();
    
    // 2b tight
    sr.SetName("47");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta",0,-1);
    sr.SetVarCRSL("PJ1eta",0,-1);
    sr.SetVarCRDY("PJ1eta",0,-1);
    sr.SetVarCRQCD("PJ1eta",0,-1);
    float mt2bins_sr47[] = {200, 400, 600, 800, 1800};
    sr.SetMT2Bins(4, mt2bins_sr47);
    SRVec_2.push_back(sr);
    sr.Clear();
    
    // 3b loose
    sr.SetName("48");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta",0,-1);
    sr.SetVarCRSL("PJ1eta",0,-1);
    sr.SetVarCRDY("PJ1eta",0,-1);
    sr.SetVarCRQCD("PJ1eta",0,-1);
    float mt2bins_sr48[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr48);
    SRVec_2.push_back(sr);
    sr.Clear();
    
    // 3b tight
    sr.SetName("49");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta",0,-1);
    sr.SetVarCRSL("PJ1eta",0,-1);
    sr.SetVarCRDY("PJ1eta",0,-1);
    sr.SetVarCRQCD("PJ1eta",0,-1);
    float mt2bins_sr49[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr49);
    SRVec_2.push_back(sr);
    sr.Clear();
    
    // 7j3b loose
    sr.SetName("50");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta",0,-1);
    sr.SetVarCRSL("PJ1eta",0,-1);
    sr.SetVarCRDY("PJ1eta",0,-1);
    sr.SetVarCRQCD("PJ1eta",0,-1);
    float mt2bins_sr50[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr50);
    SRVec_2.push_back(sr);
    sr.Clear();
    
    // 7j3b tight
    sr.SetName("51");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1500, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1500, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1500, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1500, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta",0,-1);
    sr.SetVarCRSL("PJ1eta",0,-1);
    sr.SetVarCRDY("PJ1eta",0,-1);
    sr.SetVarCRQCD("PJ1eta",0,-1);
    float mt2bins_sr51[] = {200, 400, 1800};
    sr.SetMT2Bins(2, mt2bins_sr51);
    SRVec_2.push_back(sr);
    sr.Clear();
    
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

    // common selections for QCD
    baseSR.SetVarCRQCD("mt2", 200, -1);
    baseSR.SetVarCRQCD("j1pt", 30, -1);
    baseSR.SetVarCRQCD("j2pt", 30, -1);
    baseSR.SetVarCRQCD("deltaPhiMin", 0., 0.3);
    baseSR.SetVarCRQCD("diffMetMhtOverMet", 0, 0.5);
    baseSR.SetVarCRQCD("nlep", 0, 1);

    //add baseline selections to all signal regions 
    std::vector<std::string> vars = baseSR.GetListOfVariables();
    for(unsigned int i = 0; i < SRVec_2.size(); i++){
      for(unsigned int j = 0; j < vars.size(); j++){
        SRVec_2.at(i).SetVar(vars.at(j), baseSR.GetLowerBound(vars.at(j)), baseSR.GetUpperBound(vars.at(j)));
      }
    }

    //add baseline selections to all CRSL regions 
    std::vector<std::string> varsCRSL = baseSR.GetListOfVariablesCRSL();
    for(unsigned int i = 0; i < SRVec_2.size(); i++){
      for(unsigned int j = 0; j < varsCRSL.size(); j++){
        SRVec_2.at(i).SetVarCRSL(varsCRSL.at(j), baseSR.GetLowerBoundCRSL(varsCRSL.at(j)), baseSR.GetUpperBoundCRSL(varsCRSL.at(j)));
      }
    }

    //add baseline selections to all CRDY regions 
    std::vector<std::string> varsCRDY = baseSR.GetListOfVariablesCRDY();
    for(unsigned int i = 0; i < SRVec_2.size(); i++){
      for(unsigned int j = 0; j < varsCRDY.size(); j++){
        SRVec_2.at(i).SetVarCRDY(varsCRDY.at(j), baseSR.GetLowerBoundCRDY(varsCRDY.at(j)), baseSR.GetUpperBoundCRDY(varsCRDY.at(j)));
      }
    }

    //add baseline selections to all QCD regions 
    std::vector<std::string> varsCRQCD = baseSR.GetListOfVariablesCRQCD();
    for(unsigned int i = 0; i < SRVec_2.size(); i++){
      for(unsigned int j = 0; j < varsCRQCD.size(); j++){
        SRVec_2.at(i).SetVarCRQCD(varsCRQCD.at(j), baseSR.GetLowerBoundCRQCD(varsCRQCD.at(j)), baseSR.GetUpperBoundCRQCD(varsCRQCD.at(j)));
      }
    }

    return SRVec_2;

  }

  std::vector<SR> getSignalRegions2018(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    // High PJ eta regions
    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("8");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("9");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("11");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("12");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("13");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("14");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("15");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("16");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("17");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("18");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("19");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // Low PJeta regions (20+)

    // first 15 regions (up to index 14) are high eta, then there are some more inclusive regions from indices 15 to 18
    // then rest are low eta
    const unsigned int FirstInclusiveEtaRegionIndex = 15;
    const unsigned int FirstLowEtaRegionIndex = 19;

    // 0 b, 2 and 3 jets
    sr.SetName("20");
    sr.SetVar("njets", 2, 3);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 3);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 3);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("21");
    sr.SetVar("njets", 3, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 3, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 3, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 1 b, 2 and 3 jets
    sr.SetName("22");
    sr.SetVar("njets", 2, 3);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 3);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 3);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("23");
    sr.SetVar("njets", 3, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 3, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 3, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 2 b, 2 and 3 jets
    sr.SetName("24");
    sr.SetVar("njets", 2, 3);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 3);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 3);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("25");
    sr.SetVar("njets", 3, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 3, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 3, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();
    
    // 0 b, 4 and 5-6 jets
    sr.SetName("26");
    sr.SetVar("njets", 4, 5);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 5);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 5);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("27");
    sr.SetVar("njets", 5, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 5, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 5, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 1 b-jet, 4 and 5-6 jets
    sr.SetName("28");
    sr.SetVar("njets", 4, 5);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 5);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 5);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("29");
    sr.SetVar("njets", 5, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 5, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 5, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 2 b-jet, 4 and 5-6 jets
    sr.SetName("30");
    sr.SetVar("njets", 4, 5);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 5);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 5);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("31");
    sr.SetVar("njets", 5, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 5, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 5, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7+ jet region for VL, L
    sr.SetName("32");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("33");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("34");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7-9 jet regions for M, H, UH
    sr.SetName("35");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("36");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("37");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 10+ jet regions for M, H, UH
    sr.SetName("38");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 10, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 10, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("39");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 1, -1);
    sr.SetVarCRSL("njets", 10, -1);
    sr.SetVarCRSL("nbjets", 1, -1);
    sr.SetVarCRDY("njets", 10, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 3+ b-jet regions
    // 2-6 jets
    sr.SetName("40");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7+ jets, for VL, L
    sr.SetName("41");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7-9 jets, for M, H, UH
    sr.SetName("42");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // Merged 2-3j, 2b region for H, UH
    sr.SetName("43");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();


    // Add PJeta requirements
    for (unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++) {
      // Bounds for inclusive region
      float low = 0;
      float high = -1;
      // Bounds for high eta regions
      if (iSR < FirstInclusiveEtaRegionIndex) {
	low = 1.4;
      }
      // Bounds for low eta regions (starting from index 19, region 20)
      else if (iSR >= FirstLowEtaRegionIndex) {
	low = 1.4;
	high = -1;
      }
      SR& srEta = temp_SR_vec.at(iSR);
      srEta.SetVar("PJ1eta", low, high);
      srEta.SetVarCRSL("PJ1eta", low, high);
      srEta.SetVarCRDY("PJ1eta", low, high);
      srEta.SetVarCRQCD("PJ1eta", low, high);
    }

    const float H_UH_division = 1500.0;

    //add HT and MET requirements
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 3 && iSR <= 10) continue;
      if (iSR >= 15) continue;
      SR fullSR = temp_SR_vec.at(iSR); 
      fullSR.SetName(fullSR.GetName() + "VL");
      fullSR.SetVar("ht", 250, 450);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 250, 450);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 250, 450);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 250, 450);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);      
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 450, 575);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 450, 575);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 450, 575);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};           fullSR.SetMT2Bins(2, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1200);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 575, 1200);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 575, 1200);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 575, 1200);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      // Relative to 2016, need one extra bin here in M to adjust for HT trigger shift from 1000 to 1200 GeV (now have 800-1000 and 1000+ bins)
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[7] = {200, 300, 400, 500, 700, 900,  H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[6] = {200, 300, 400, 500, 700, H_UH_division};             fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[6] = {200, 300, 400, 500, 700, H_UH_division};             fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 500, 700, H_UH_division};      fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};           fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};           fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, H_UH_division};                fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 11) continue;      
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1200, H_UH_division);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", 1200, H_UH_division);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", 1200, H_UH_division);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", 1200, H_UH_division);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[2] = {200, H_UH_division};                            fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[6] = {200, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "UH");
      fullSR.SetVar("ht", H_UH_division, -1);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", H_UH_division, -1);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", H_UH_division, -1);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", H_UH_division, -1);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      int njets_hi = fullSR.GetUpperBound("njets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){
        float mt2bins[2] = {200, 1800};
        float mt2bins_ssr[4] = {200, 400, 600, 1800};
        if (njets_hi == -1)
          fullSR.SetMT2Bins(3, mt2bins_ssr);
        else
          fullSR.SetMT2Bins(1, mt2bins);          
      }      
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, 1800};       fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, 1800};       fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      SRVec.push_back(fullSR);
    }

    //add HT and MET requirements for low PJ eta bins
    // we need to modify these bins to account for the 2, 3, 4, 5-6, 7-9, and 10+ jet rebinning
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 35-39 and 42 are M, H, UH only, 43 is H,UH only
      if (iSR >= 34 && iSR <= 38) continue;
      if (iSR >= 41) continue;
      SR fullSR = temp_SR_vec.at(iSR); 
      fullSR.SetName(fullSR.GetName() + "VL");
      fullSR.SetVar("ht", 250, 450);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 250, 450);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 250, 450);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 250, 450);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 2){float mt2bins[3] = {200, 300, H_UH_division};      fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};      fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[3] = {200, 300, H_UH_division};      fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[3] = {200, 300, H_UH_division};      fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[2] = {200, H_UH_division};           fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[2] = {200, H_UH_division};           fullSR.SetMT2Bins(1, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 35-39 and 42 are M, H, UH only, 43 is H,UH only
      if (iSR >= 34 && iSR <= 38) continue;
      if (iSR >= 41) continue;
      SR fullSR = temp_SR_vec.at(iSR);      
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 450, 575);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 450, 575);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 450, 575);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      // Same as VL, except an extra 400-500 MT2 bin
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};           fullSR.SetMT2Bins(2, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 32-34 and 41 are VL, L only, 43 is H,UH only
      if (iSR >= 31 && iSR <= 33) continue;
      if (iSR == 40) continue;
      if (iSR == 42) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1200);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 575, 1200);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 575, 1200);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 575, 1200);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, H_UH_division};                 fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 600, 800, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 600, 800, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division};            fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 600, 800, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};            fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 600, 800, H_UH_division}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 500, 600, 700, H_UH_division};  fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, H_UH_division};           fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[3]= {200, 300, H_UH_division};                fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4]= {200, 300, 400, H_UH_division};           fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 32-34 and 41 are VL, L only
      if (iSR >= 31 && iSR <= 33) continue;
      if (iSR == 23 || iSR == 24) continue; // regions 24 and 25 (2j2b and 3j2b) are merged for H
      if (iSR == 40) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1200, H_UH_division);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", 1200, H_UH_division);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", 1200, H_UH_division);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", 1200, H_UH_division);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[2] = {200, H_UH_division};                            fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 1){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      //      else if(njets_lo == 3 && nbjets_lo == 2){float mt2bins[2] = {200, H_UH_division};                            fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[3]= {200, 300, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4]= {200, 300, 400, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 32-34 and 41 are VL, L only
      if (iSR >= 31 && iSR <= 33) continue;
      if (iSR == 23 || iSR == 24) continue; // regions 24 and 25 (2j2b and 3j2b) are merged for UH
      if (iSR == 40) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "UH");
      fullSR.SetVar("ht", H_UH_division, -1);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", H_UH_division, -1);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", H_UH_division, -1);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", H_UH_division, -1);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      int njets_hi = fullSR.GetUpperBound("njets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, 1800}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[3] = {200, 400, 1800};           fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){
        float mt2bins[2] = {200, 1800};
        float mt2bins_ssr[4] = {200, 400, 600, 1800};
        if (njets_hi == -1)
          fullSR.SetMT2Bins(3, mt2bins_ssr);
        else
          fullSR.SetMT2Bins(1, mt2bins);          
      }      
      else if(njets_lo == 3 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, 1800};       fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 1){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      //      else if(njets_lo == 3 && nbjets_lo == 2){float mt2bins[2] = {200, 1800};                      fullSR.SetMT2Bins(1, mt2bins);}      
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[3] = {200, 300, 1800};                fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1800};           fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }

    // --- super signal regions, numbered 50 and up
    //     inclusive in HT, NJ, NB
    //     MT2 binning matches the most granular TR that would be included
    //     MT2 cut will be applied later by summing appropriate datacards

    // 2j loose
    sr.SetName("50");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr50[] = {200, 400, 600, 800, 1000, 1200, 1800};
    sr.SetMT2Bins(6, mt2bins_sr50);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 2j tight
    sr.SetName("51");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr51[] = {200, 400, 600, 800, 1000, 1400, 1800};
    sr.SetMT2Bins(6, mt2bins_sr51);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 4j loose
    sr.SetName("52");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr52[] = {200, 400, 600, 800, 1000, 1200, 1800};
    sr.SetMT2Bins(6, mt2bins_sr52);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 4j tight
    sr.SetName("53");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr53[] = {200, 400, 600, 800, 1000, 1400, 1800};
    sr.SetMT2Bins(6, mt2bins_sr53);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j loose
    sr.SetName("54");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr54[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr54);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j tight
    sr.SetName("55");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr55[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr55);
    SRVec.push_back(sr);
    sr.Clear();
    

    // 2b loose
    sr.SetName("56");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr56[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr56);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 2b tight
    sr.SetName("57");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr57[] = {200, 400, 600, 800, 1800};
    sr.SetMT2Bins(4, mt2bins_sr57);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 3b loose
    sr.SetName("58");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr58[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr58);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 3b tight
    sr.SetName("59");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr59[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr59);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j3b loose
    sr.SetName("60");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr60[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr60);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j3b tight
    sr.SetName("61");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr61[] = {200, 400, 1800};
    sr.SetMT2Bins(2, mt2bins_sr61);
    SRVec.push_back(sr);
    sr.Clear();
    
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

    //add baseline selections to all CRDY regions 
    std::vector<std::string> varsCRDY = baseSR.GetListOfVariablesCRDY();
    for(unsigned int i = 0; i < SRVec.size(); i++){
      for(unsigned int j = 0; j < varsCRDY.size(); j++){
        SRVec.at(i).SetVarCRDY(varsCRDY.at(j), baseSR.GetLowerBoundCRDY(varsCRDY.at(j)), baseSR.GetUpperBoundCRDY(varsCRDY.at(j)));
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

  std::vector<SR> getSignalRegions2018HUH(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    // High PJ eta regions
    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("8");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("9");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("11");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("12");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("13");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("14");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("15");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("16");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("17");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("18");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("19");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // Low PJeta regions (20+)

    // first 15 regions (up to index 14) are high eta, then there are some more inclusive regions from indices 15 to 18
    // then rest are low eta
    const unsigned int FirstInclusiveEtaRegionIndex = 15;
    const unsigned int FirstLowEtaRegionIndex = 19;

    // 0 b, 2 and 3 jets
    sr.SetName("20");
    sr.SetVar("njets", 2, 3);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 3);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 3);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("21");
    sr.SetVar("njets", 3, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 3, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 3, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 1 b, 2 and 3 jets
    sr.SetName("22");
    sr.SetVar("njets", 2, 3);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 3);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 3);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("23");
    sr.SetVar("njets", 3, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 3, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 3, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 2 b, 2 and 3 jets
    sr.SetName("24");
    sr.SetVar("njets", 2, 3);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 3);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 3);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("25");
    sr.SetVar("njets", 3, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 3, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 3, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();
    
    // 0 b, 4 and 5-6 jets
    sr.SetName("26");
    sr.SetVar("njets", 4, 5);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 5);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 5);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("27");
    sr.SetVar("njets", 5, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 5, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 5, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 1 b-jet, 4 and 5-6 jets
    sr.SetName("28");
    sr.SetVar("njets", 4, 5);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 5);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 5);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("29");
    sr.SetVar("njets", 5, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 5, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 5, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 2 b-jet, 4 and 5-6 jets
    sr.SetName("30");
    sr.SetVar("njets", 4, 5);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 5);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 5);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("31");
    sr.SetVar("njets", 5, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 5, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 5, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7+ jet region for VL, L
    sr.SetName("32");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("33");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("34");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7-9 jet regions for M, H, UH
    sr.SetName("35");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("36");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("37");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 10+ jet regions for M, H, UH
    sr.SetName("38");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 10, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 10, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("39");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 1, -1);
    sr.SetVarCRSL("njets", 10, -1);
    sr.SetVarCRSL("nbjets", 1, -1);
    sr.SetVarCRDY("njets", 10, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 3+ b-jet regions
    // 2-6 jets
    sr.SetName("40");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7+ jets, for VL, L
    sr.SetName("41");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7-9 jets, for M, H, UH
    sr.SetName("42");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // Merged 2-3j, 2b region for H, UH
    sr.SetName("43");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // Merged 2-3j, 1b region for UH
    sr.SetName("44");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // Merged 2-3j, 0b region for UH
    sr.SetName("45");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // Merged 4-6j, 2b region for UH
    sr.SetName("46");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();


    // Add PJeta requirements
    for (unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++) {
      // Bounds for inclusive region
      float low = 0;
      float high = -1;
      // Bounds for high eta regions 
      if (iSR < FirstInclusiveEtaRegionIndex) {
	low = 1.4;
      }
      // Bounds for low eta regions (starting from index 19, region 20)
      else if (iSR >= FirstLowEtaRegionIndex) {
	low = 1.4;
	high = -1;
      }
      SR& srEta = temp_SR_vec.at(iSR);
      srEta.SetVar("PJ1eta", low, high);
      srEta.SetVarCRSL("PJ1eta", low, high);
      srEta.SetVarCRDY("PJ1eta", low, high);
      srEta.SetVarCRQCD("PJ1eta", low, high);
    }

    const float H_UH_division = 1600.0;

    //add HT and MET requirements
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 3 && iSR <= 10) continue;
      if (iSR >= 15) continue;
      SR fullSR = temp_SR_vec.at(iSR); 
      fullSR.SetName(fullSR.GetName() + "VL");
      fullSR.SetVar("ht", 250, 450);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 250, 450);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 250, 450);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 250, 450);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);      
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 450, 575);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 450, 575);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 450, 575);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};           fullSR.SetMT2Bins(2, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1200);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 575, 1200);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 575, 1200);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 575, 1200);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      // Relative to 2016, need one extra bin here in M to adjust for HT trigger shift from 1000 to 1200 GeV (now have 800-1000 and 1000+ bins)
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[7] = {200, 300, 400, 500, 700, 900,  H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[6] = {200, 300, 400, 500, 700, H_UH_division};             fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[6] = {200, 300, 400, 500, 700, H_UH_division};             fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 500, 700, H_UH_division};      fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};           fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};           fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, H_UH_division};                fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 11) continue;      
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1200, H_UH_division);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", 1200, H_UH_division);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", 1200, H_UH_division);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", 1200, H_UH_division);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[2] = {200, H_UH_division};                            fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[6] = {200, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "UH");
      fullSR.SetVar("ht", H_UH_division, -1);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", H_UH_division, -1);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", H_UH_division, -1);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", H_UH_division, -1);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      int njets_hi = fullSR.GetUpperBound("njets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, 1800}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){
        float mt2bins[2] = {200, 1800};
        float mt2bins_ssr[4] = {200, 400, 600, 1800};
        if (njets_hi == -1)
          fullSR.SetMT2Bins(3, mt2bins_ssr);
        else
          fullSR.SetMT2Bins(1, mt2bins);          
      }      
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, 1800};       fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, 1800};       fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[2] = {200, 1800};                 fullSR.SetMT2Bins(1, mt2bins);}
      SRVec.push_back(fullSR);
    }

    //add HT and MET requirements for low PJ eta bins
    // we need to modify these bins to account for the 2, 3, 4, 5-6, 7-9, and 10+ jet rebinning
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 35-39 and 42 are M, H, UH only, 43 is H,UH only
      if (iSR >= 34 && iSR <= 38) continue;
      if (iSR >= 41) continue;
      SR fullSR = temp_SR_vec.at(iSR); 
      fullSR.SetName(fullSR.GetName() + "VL");
      fullSR.SetVar("ht", 250, 450);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 250, 450);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 250, 450);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 250, 450);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 2){float mt2bins[3] = {200, 300, H_UH_division};      fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};      fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[3] = {200, 300, H_UH_division};      fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[3] = {200, 300, H_UH_division};      fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[2] = {200, H_UH_division};           fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[2] = {200, H_UH_division};           fullSR.SetMT2Bins(1, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 35-39 and 42 are M, H, UH only, 43 is H,UH only
      if (iSR >= 34 && iSR <= 38) continue;
      if (iSR >= 41) continue;
      SR fullSR = temp_SR_vec.at(iSR);      
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 450, 575);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 450, 575);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 450, 575);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      // Same as VL, except an extra 400-500 MT2 bin
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};           fullSR.SetMT2Bins(2, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 32-34 and 41 are VL, L only, 43 is H,UH only
      if (iSR >= 31 && iSR <= 33) continue;
      if (iSR == 40) continue;
      if (iSR >= 42) continue; // regions 43+ are H/UH only
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1200);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 575, 1200);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 575, 1200);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 575, 1200);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, H_UH_division};                 fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 600, 800, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 600, 800, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division};            fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 600, 800, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};            fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 600, 800, H_UH_division}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 500, 600, 700, H_UH_division};  fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, H_UH_division};           fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[3]= {200, 300, H_UH_division};                fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4]= {200, 300, 400, H_UH_division};           fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 32-34 and 41 are VL, L only
      if (iSR >= 31 && iSR <= 33) continue;
      if (iSR == 23 || iSR == 24) continue; // regions 24 and 25 (2j2b and 3j2b) are merged for H
      if (iSR == 40) continue;
      if (iSR >= 43) continue; // regions 44+ are UH only
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1200, H_UH_division);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", 1200, H_UH_division);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", 1200, H_UH_division);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", 1200, H_UH_division);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[2] = {200, H_UH_division};                            fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 3 && nbjets_lo == 1){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      //      else if(njets_lo == 3 && nbjets_lo == 2){float mt2bins[2] = {200, H_UH_division};                            fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[3]= {200, 300, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4]= {200, 300, 400, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 32-34 and 41 are VL, L only
      if (iSR >= 31 && iSR <= 33) continue;
      if (iSR == 23 || iSR == 24) continue; // regions 24 and 25 (2j2b and 3j2b) are merged for UH
      if (iSR == 19 || iSR == 20) continue; // regions 20 and 21 (2j0b and 3j0b) are merged for UH
      if (iSR == 21 || iSR == 22) continue; // regions 22 and 23 (2j1b and 3j1b) are merged for UH
      if (iSR == 25 || iSR == 26) continue; // regions 26 and 27 (4j0b and 5-6j0b) are merged for UH
      if (iSR == 29 || iSR == 30) continue; // regions 30 and 31 (4j2b and 5-6j2b) are merged for UH
      if (iSR == 40) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "UH");
      fullSR.SetVar("ht", H_UH_division, -1);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", H_UH_division, -1);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", H_UH_division, -1);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", H_UH_division, -1);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      int njets_hi = fullSR.GetUpperBound("njets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, 1800}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[3] = {200, 400, 1800};           fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){
        float mt2bins[2] = {200, 1800};
        float mt2bins_ssr[4] = {200, 400, 600, 1800};
        if (njets_hi == -1)
          fullSR.SetMT2Bins(3, mt2bins_ssr);
        else
          fullSR.SetMT2Bins(1, mt2bins);          
      }      
      //      else if(njets_lo == 3 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, 1800};       fullSR.SetMT2Bins(4, mt2bins);}
      //else if(njets_lo == 3 && nbjets_lo == 1){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      //      else if(njets_lo == 3 && nbjets_lo == 2){float mt2bins[2] = {200, 1800};                      fullSR.SetMT2Bins(1, mt2bins);}      
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      //else if(njets_lo == 5 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 5 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      //else if(njets_lo == 5 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1800};            fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[2] = {200, 1800};                 fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[3] = {200, 300, 1800};                fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1800};           fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }

    // --- super signal regions, numbered 50 and up
    //     inclusive in HT, NJ, NB
    //     MT2 binning matches the most granular TR that would be included
    //     MT2 cut will be applied later by summing appropriate datacards

    // 2j loose
    sr.SetName("50");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr50[] = {200, 400, 600, 800, 1000, 1200, 1800};
    sr.SetMT2Bins(6, mt2bins_sr50);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 2j tight
    sr.SetName("51");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr51[] = {200, 400, 600, 800, 1000, 1400, 1800};
    sr.SetMT2Bins(6, mt2bins_sr51);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 4j loose
    sr.SetName("52");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr52[] = {200, 400, 600, 800, 1000, 1200, 1800};
    sr.SetMT2Bins(6, mt2bins_sr52);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 4j tight
    sr.SetName("53");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr53[] = {200, 400, 600, 800, 1000, 1400, 1800};
    sr.SetMT2Bins(6, mt2bins_sr53);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j loose
    sr.SetName("54");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr54[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr54);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j tight
    sr.SetName("55");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr55[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr55);
    SRVec.push_back(sr);
    sr.Clear();
    

    // 2b loose
    sr.SetName("56");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr56[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr56);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 2b tight
    sr.SetName("57");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr57[] = {200, 400, 600, 800, 1800};
    sr.SetMT2Bins(4, mt2bins_sr57);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 3b loose
    sr.SetName("58");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr58[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr58);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 3b tight
    sr.SetName("59");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr59[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr59);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j3b loose
    sr.SetName("60");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr60[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr60);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j3b tight
    sr.SetName("61");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr61[] = {200, 400, 1800};
    sr.SetMT2Bins(2, mt2bins_sr61);
    SRVec.push_back(sr);
    sr.Clear();
    
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

    //add baseline selections to all CRDY regions 
    std::vector<std::string> varsCRDY = baseSR.GetListOfVariablesCRDY();
    for(unsigned int i = 0; i < SRVec.size(); i++){
      for(unsigned int j = 0; j < varsCRDY.size(); j++){
        SRVec.at(i).SetVarCRDY(varsCRDY.at(j), baseSR.GetLowerBoundCRDY(varsCRDY.at(j)), baseSR.GetUpperBoundCRDY(varsCRDY.at(j)));
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

  std::vector<SR> getSignalRegions2018lessNJ(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    // High PJ eta regions
    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("8");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("9");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("11");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("12");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("13");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("14");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("15");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("16");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("17");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("18");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("19");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // Low PJeta regions (20+)

    // first 15 regions (up to index 14) are high eta, then there are some more inclusive regions from indices 15 to 18
    // then rest are low eta
    const unsigned int FirstInclusiveEtaRegionIndex = 15;
    const unsigned int FirstLowEtaRegionIndex = 19;

    // 0 b, 2 and 3 jets
    sr.SetName("20");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 1 b, 2 and 3 jets
    sr.SetName("21");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 2 b, 2 and 3 jets
    sr.SetName("22");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();
    
    // 0 b, 4 and 5-6 jets
    sr.SetName("23");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 1 b-jet, 4 and 5-6 jets
    sr.SetName("24");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 2 b-jet, 4 and 5-6 jets
    sr.SetName("25");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7+ jet region for VL, L
    sr.SetName("26");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("27");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("28");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7-9 jet regions for M, H, UH
    sr.SetName("29");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("30");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("31");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 10+ jet regions for M, H, UH
    sr.SetName("32");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 10, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 10, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("33");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 1, -1);
    sr.SetVarCRSL("njets", 10, -1);
    sr.SetVarCRSL("nbjets", 1, -1);
    sr.SetVarCRDY("njets", 10, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 3+ b-jet regions
    // 2-6 jets
    sr.SetName("34");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7+ jets, for VL, L
    sr.SetName("35");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7-9 jets, for M, H, UH
    sr.SetName("36");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // Add PJeta requirements
    for (unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++) {
      // Bounds for inclusive region
      float low = 0;
      float high = -1;
      // Bounds for high eta regions 
      if (iSR < FirstInclusiveEtaRegionIndex) {
	low = 1.4;
      }
      // Bounds for low eta regions (starting from index 19, region 20)
      else if (iSR >= FirstLowEtaRegionIndex) {
	low = 1.4;
	high = -1;
      }
      SR& srEta = temp_SR_vec.at(iSR);
      srEta.SetVar("PJ1eta", low, high);
      srEta.SetVarCRSL("PJ1eta", low, high);
      srEta.SetVarCRDY("PJ1eta", low, high);
      srEta.SetVarCRQCD("PJ1eta", low, high);
    }

    const float H_UH_division = 1600.0;

    //add HT and MET requirements
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 3 && iSR <= 10) continue;
      if (iSR >= 15) continue;
      SR fullSR = temp_SR_vec.at(iSR); 
      fullSR.SetName(fullSR.GetName() + "VL");
      fullSR.SetVar("ht", 250, 450);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 250, 450);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 250, 450);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 250, 450);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);      
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 450, 575);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 450, 575);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 450, 575);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};           fullSR.SetMT2Bins(2, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1200);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 575, 1200);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 575, 1200);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 575, 1200);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      // Relative to 2016, need one extra bin here in M to adjust for HT trigger shift from 1000 to 1200 GeV (now have 800-1000 and 1000+ bins)
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[7] = {200, 300, 400, 500, 700, 900,  H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[6] = {200, 300, 400, 500, 700, H_UH_division};             fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[6] = {200, 300, 400, 500, 700, H_UH_division};             fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 500, 700, H_UH_division};      fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};           fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};           fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, H_UH_division};                fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 11) continue;      
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1200, H_UH_division);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", 1200, H_UH_division);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", 1200, H_UH_division);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", 1200, H_UH_division);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[2] = {200, H_UH_division};                            fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[6] = {200, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "UH");
      fullSR.SetVar("ht", H_UH_division, -1);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", H_UH_division, -1);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", H_UH_division, -1);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", H_UH_division, -1);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      int njets_hi = fullSR.GetUpperBound("njets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, 1800}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){
        float mt2bins[2] = {200, 1800};
        float mt2bins_ssr[4] = {200, 400, 600, 1800};
        if (njets_hi == -1)
          fullSR.SetMT2Bins(3, mt2bins_ssr);
        else
          fullSR.SetMT2Bins(1, mt2bins);          
      }      
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, 1800};       fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, 1800};       fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[2] = {200, 1800};                 fullSR.SetMT2Bins(1, mt2bins);}
      SRVec.push_back(fullSR);
    }

    //add HT and MET requirements for low PJ eta bins
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 29-33 and 36 are M, H, UH only 
      if (iSR >= 28 && iSR <= 32) continue;
      if (iSR >= 35) continue;
      SR fullSR = temp_SR_vec.at(iSR); 
      fullSR.SetName(fullSR.GetName() + "VL");
      fullSR.SetVar("ht", 250, 450);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 250, 450);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 250, 450);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 250, 450);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};      fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[3] = {200, 300, H_UH_division};      fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[3] = {200, 300, H_UH_division};      fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[2] = {200, H_UH_division};           fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[2] = {200, H_UH_division};           fullSR.SetMT2Bins(1, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 29-33 and 36 are M, H, UH only 
      if (iSR >= 28 && iSR <= 32) continue;
      if (iSR >= 35) continue;
      SR fullSR = temp_SR_vec.at(iSR);      
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 450, 575);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 450, 575);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 450, 575);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      // Same as VL, except an extra 400-500 MT2 bin
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};           fullSR.SetMT2Bins(2, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 26-28 and 35 are VL, L only
      if (iSR >= 25 && iSR <= 27) continue;
      if (iSR == 34) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1200);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 575, 1200);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 575, 1200);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 575, 1200);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, H_UH_division};                 fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 600, 800, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};            fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 500, 600, 700, H_UH_division};  fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, H_UH_division};           fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[3]= {200, 300, H_UH_division};                fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4]= {200, 300, 400, H_UH_division};           fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 26-28 and 35 are VL, L only
      if (iSR >= 25 && iSR <= 27) continue;
      if (iSR == 34) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1200, H_UH_division);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", 1200, H_UH_division);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", 1200, H_UH_division);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", 1200, H_UH_division);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[2] = {200, H_UH_division};                            fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[3]= {200, 300, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4]= {200, 300, 400, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      // regions 26-28 and 35 are VL, L only
      if (iSR >= 25 && iSR <= 27) continue;
      if (iSR == 34) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "UH");
      fullSR.SetVar("ht", H_UH_division, -1);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", H_UH_division, -1);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", H_UH_division, -1);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", H_UH_division, -1);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      int njets_hi = fullSR.GetUpperBound("njets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, 1800}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[3] = {200, 400, 1800};           fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){
        float mt2bins[2] = {200, 1800};
        float mt2bins_ssr[4] = {200, 400, 600, 1800};
        if (njets_hi == -1)
          fullSR.SetMT2Bins(3, mt2bins_ssr);
        else
          fullSR.SetMT2Bins(1, mt2bins);          
      }      
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1800};            fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[2] = {200, 1800};                 fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[3] = {200, 300, 1800};                fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1800};           fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }

    // --- super signal regions, numbered 50 and up
    //     inclusive in HT, NJ, NB
    //     MT2 binning matches the most granular TR that would be included
    //     MT2 cut will be applied later by summing appropriate datacards

    // 2j loose
    sr.SetName("50");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr50[] = {200, 400, 600, 800, 1000, 1200, 1800};
    sr.SetMT2Bins(6, mt2bins_sr50);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 2j tight
    sr.SetName("51");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr51[] = {200, 400, 600, 800, 1000, 1400, 1800};
    sr.SetMT2Bins(6, mt2bins_sr51);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 4j loose
    sr.SetName("52");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr52[] = {200, 400, 600, 800, 1000, 1200, 1800};
    sr.SetMT2Bins(6, mt2bins_sr52);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 4j tight
    sr.SetName("53");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr53[] = {200, 400, 600, 800, 1000, 1400, 1800};
    sr.SetMT2Bins(6, mt2bins_sr53);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j loose
    sr.SetName("54");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr54[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr54);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j tight
    sr.SetName("55");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr55[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr55);
    SRVec.push_back(sr);
    sr.Clear();
    

    // 2b loose
    sr.SetName("56");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr56[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr56);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 2b tight
    sr.SetName("57");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr57[] = {200, 400, 600, 800, 1800};
    sr.SetMT2Bins(4, mt2bins_sr57);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 3b loose
    sr.SetName("58");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr58[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr58);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 3b tight
    sr.SetName("59");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr59[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr59);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j3b loose
    sr.SetName("60");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr60[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr60);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j3b tight
    sr.SetName("61");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr61[] = {200, 400, 1800};
    sr.SetMT2Bins(2, mt2bins_sr61);
    SRVec.push_back(sr);
    sr.Clear();
    
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

    //add baseline selections to all CRDY regions 
    std::vector<std::string> varsCRDY = baseSR.GetListOfVariablesCRDY();
    for(unsigned int i = 0; i < SRVec.size(); i++){
      for(unsigned int j = 0; j < varsCRDY.size(); j++){
        SRVec.at(i).SetVarCRDY(varsCRDY.at(j), baseSR.GetLowerBoundCRDY(varsCRDY.at(j)), baseSR.GetUpperBoundCRDY(varsCRDY.at(j)));
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

  std::vector<SR> getSignalRegions2018hiHTnoPJ(){

    std::vector<SR> temp_SR_vec;
    std::vector<SR> SRVec;
    SR sr;
    SR baseSR;

    // High PJ eta regions
    //first set binning in njet-nbjet plane
    sr.SetName("1");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("2");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("3");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("4");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("5");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("6");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("7");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("8");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("9");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("10");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // shared CR: 7j1-2b
    sr.SetName("11");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("12");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("13");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("14");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("15");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("16");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("17");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("18");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("19");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // Low PJeta regions (20+)

    // first 15 regions (up to index 14) are high eta, then there are some more inclusive regions from indices 15 to 18
    // then rest are low eta
    const unsigned int FirstInclusiveEtaRegionIndex = 15;
    const unsigned int FirstLowEtaRegionIndex = 19;
    const unsigned int FirstHUHRegionIndex = 36;

    // 0 b, 2 and 3 jets
    sr.SetName("20");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 1 b, 2 and 3 jets
    sr.SetName("21");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 2 b, 2 and 3 jets
    sr.SetName("22");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();
    
    // 0 b, 4 and 5-6 jets
    sr.SetName("23");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 1 b-jet, 4 and 5-6 jets
    sr.SetName("24");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 2 b-jet, 4 and 5-6 jets
    sr.SetName("25");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7+ jet region for VL, L
    sr.SetName("26");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("27");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("28");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7-9 jet regions for M
    sr.SetName("29");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("30");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("31");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 10+ jet regions for M
    sr.SetName("32");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 10, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 10, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("33");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 1, -1);
    sr.SetVarCRSL("njets", 10, -1);
    sr.SetVarCRSL("nbjets", 1, -1);
    sr.SetVarCRDY("njets", 10, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 3+ b-jet regions
    // 2-6 jets
    sr.SetName("34");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7+ jets, for VL, L
    sr.SetName("35");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7-9 jets, for M
    sr.SetName("36");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // Regions without pseudojet binning for H, UH

    // 0 b, 2 and 3 jets
    sr.SetName("37");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 1 b, 2 and 3 jets
    sr.SetName("38");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 2 b, 2 and 3 jets
    sr.SetName("39");
    sr.SetVar("njets", 2, 4);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 2, 4);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 2, 4);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();
    
    // 0 b, 4 and 5-6 jets
    sr.SetName("40");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 1 b-jet, 4 and 5-6 jets
    sr.SetName("41");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 2 b-jet, 4 and 5-6 jets
    sr.SetName("42");
    sr.SetVar("njets", 4, 7);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 4, 7);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 4, 7);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7-9 jet regions
    sr.SetName("43");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("44");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 1, 2);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 1, 2);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 1, 2);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("45");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 2, 3);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 2, 3);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 2, 3);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 10+ jet regions
    sr.SetName("46");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 0, 1);
    sr.SetVarCRSL("njets", 10, -1);
    sr.SetVarCRSL("nbjets", 0, 1);
    sr.SetVarCRDY("njets", 10, -1);
    sr.SetVarCRDY("nbjets", 0, 1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    sr.SetName("47");
    sr.SetVar("njets", 10, -1);
    sr.SetVar("nbjets", 1, -1);
    sr.SetVarCRSL("njets", 10, -1);
    sr.SetVarCRSL("nbjets", 1, -1);
    sr.SetVarCRDY("njets", 10, -1);
    sr.SetVarCRDY("nbjets", 1, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 3+ b-jet regions
    // 2-6 jets
    sr.SetName("48");
    sr.SetVar("njets", 2, 7);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 2, 7);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 2, 7);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // 7-9 jets
    sr.SetName("49");
    sr.SetVar("njets", 7, 10);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVarCRSL("njets", 7, 10);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRDY("njets", 7, 10);
    sr.SetVarCRDY("nbjets", 3, -1);
    temp_SR_vec.push_back(sr);
    sr.Clear();

    // Add PJeta requirements
    for (unsigned int iSR = 0; iSR < temp_SR_vec.size(); iSR++) {
      // Bounds for inclusive region
      float low = 0;
      float high = -1;
      // Bounds for high eta regions 
      if (iSR < FirstInclusiveEtaRegionIndex) {
	low = 1.4;
      }
      // Bounds for low eta regions (starting from index 19, region 20)
      else if (iSR >= FirstLowEtaRegionIndex && iSR < FirstHUHRegionIndex) {
	low = 1.4;
	high = -1;
      }
      SR& srEta = temp_SR_vec.at(iSR);
      srEta.SetVar("PJ1eta", low, high);
      srEta.SetVarCRSL("PJ1eta", low, high);
      srEta.SetVarCRDY("PJ1eta", low, high);
      srEta.SetVarCRQCD("PJ1eta", low, high);
    }

    const float H_UH_division = 1600.0;

    //add HT and MET requirements
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 3 && iSR <= 10) continue;
      if (iSR >= 15) continue;
      SR fullSR = temp_SR_vec.at(iSR); 
      fullSR.SetName(fullSR.GetName() + "VL");
      fullSR.SetVar("ht", 250, 450);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 250, 450);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 250, 450);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 250, 450);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);      
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 450, 575);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 450, 575);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 450, 575);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};           fullSR.SetMT2Bins(2, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = 0; iSR < FirstLowEtaRegionIndex; iSR++){
      if (iSR >= 11) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1200);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 575, 1200);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 575, 1200);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 575, 1200);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      // Relative to 2016, need one extra bin here in M to adjust for HT trigger shift from 1000 to 1200 GeV (now have 800-1000 and 1000+ bins)
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[7] = {200, 300, 400, 500, 700, 900,  H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};                  fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[6] = {200, 300, 400, 500, 700, H_UH_division};             fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[6] = {200, 300, 400, 500, 700, H_UH_division};             fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 500, 700, H_UH_division};      fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};           fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};           fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, H_UH_division};                fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    // No eta-binned regions for H, UH

    //add HT and MET requirements for low PJ eta bins
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < FirstHUHRegionIndex; iSR++){
      // regions 29-33 and 36 are M only
      if (iSR >= 28 && iSR <= 32) continue;
      if (iSR >= 35) continue;
      SR fullSR = temp_SR_vec.at(iSR); 
      fullSR.SetName(fullSR.GetName() + "VL");
      fullSR.SetVar("ht", 250, 450);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 250, 450);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 250, 450);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 250, 450);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division}; fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};      fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[3] = {200, 300, H_UH_division};      fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[3] = {200, 300, H_UH_division};      fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[2] = {200, H_UH_division};           fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[2] = {200, H_UH_division};           fullSR.SetMT2Bins(1, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < FirstHUHRegionIndex; iSR++){
      // regions 29-33 and 36 are M 
      if (iSR >= 28 && iSR <= 32) continue;
      if (iSR >= 35) continue;
      SR fullSR = temp_SR_vec.at(iSR);      
      fullSR.SetName(fullSR.GetName() + "L");
      fullSR.SetVar("ht", 450, 575);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 450, 575);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 450, 575);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 450, 575);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      // Same as VL, except an extra 400-500 MT2 bin
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 500, H_UH_division}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[4] = {200, 300, 400, H_UH_division};      fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};           fullSR.SetMT2Bins(2, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < FirstHUHRegionIndex; iSR++){
      // regions 26-28 and 35 are VL, L only
      if (iSR >= 25 && iSR <= 27) continue;
      if (iSR == 34) continue;
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "M");
      fullSR.SetVar("ht", 575, 1200);
      fullSR.SetVar("met", 250, -1);
      fullSR.SetVarCRSL("ht", 575, 1200);
      fullSR.SetVarCRSL("met", 250, -1);
      fullSR.SetVarCRDY("ht", 575, 1200);
      fullSR.SetVarCRDY("met", 250, -1);
      fullSR.SetVarCRQCD("ht", 575, 1200);
      fullSR.SetVarCRQCD("met", 250, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[4] = {200, 400, 600, H_UH_division};                 fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 600, 800, 1000, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[6] = {200, 300, 400, 600, 800, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};            fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[7] = {200, 300, 400, 500, 600, 700, H_UH_division};  fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[5] = {200, 300, 400, 600, H_UH_division};      fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[4] = {200, 300, 400, H_UH_division};           fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[3]= {200, 300, H_UH_division};                fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4]= {200, 300, 400, H_UH_division};           fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }

    // H, UH have no eta binning

    for(unsigned int iSR = FirstHUHRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "H");
      fullSR.SetVar("ht", 1200, H_UH_division);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", 1200, H_UH_division);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", 1200, H_UH_division);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", 1200, H_UH_division);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[7] = {200, 400, 600, 800, 1000, 1200, H_UH_division}; fullSR.SetMT2Bins(6, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){float mt2bins[2] = {200, H_UH_division};                            fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, H_UH_division};       fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[5] = {200, 400, 600, 800, H_UH_division};             fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[3] = {200, 400, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[3] = {200, 300, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[3]= {200, 300, H_UH_division};                       fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4]= {200, 300, 400, H_UH_division};                  fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }
    for(unsigned int iSR = FirstLowEtaRegionIndex; iSR < temp_SR_vec.size(); iSR++){
      SR fullSR = temp_SR_vec.at(iSR);  
      fullSR.SetName(fullSR.GetName() + "UH");
      fullSR.SetVar("ht", H_UH_division, -1);
      fullSR.SetVar("met", 30, -1);
      fullSR.SetVarCRSL("ht", H_UH_division, -1);
      fullSR.SetVarCRSL("met", 30, -1);
      fullSR.SetVarCRDY("ht", H_UH_division, -1);
      fullSR.SetVarCRDY("met", 30, -1);
      fullSR.SetVarCRQCD("ht", H_UH_division, -1);
      fullSR.SetVarCRQCD("met", 30, -1);
      int njets_lo = fullSR.GetLowerBound("njets");
      int nbjets_lo = fullSR.GetLowerBound("nbjets");
      int njets_hi = fullSR.GetUpperBound("njets");
      if     (njets_lo == 2 && nbjets_lo == 0){float mt2bins[5] = {200, 400, 600, 800, 1800}; fullSR.SetMT2Bins(4, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 1){float mt2bins[3] = {200, 400, 1800};           fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 2){
        float mt2bins[2] = {200, 1800};
        float mt2bins_ssr[4] = {200, 400, 600, 1800};
        if (njets_hi == -1)
          fullSR.SetMT2Bins(3, mt2bins_ssr);
        else
          fullSR.SetMT2Bins(1, mt2bins);          
      }      
      else if(njets_lo == 4 && nbjets_lo == 0){float mt2bins[6] = {200, 400, 600, 800, 1000, 1800}; fullSR.SetMT2Bins(5, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 4 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 0){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 1){float mt2bins[4] = {200, 400, 600, 1800};            fullSR.SetMT2Bins(3, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 2){float mt2bins[3] = {200, 400, 1800};            fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 2 && nbjets_lo == 3){float mt2bins[3] = {200, 400, 1800};                 fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 7 && nbjets_lo == 3){float mt2bins[2] = {200, 1800};                 fullSR.SetMT2Bins(1, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 0){float mt2bins[3] = {200, 300, 1800};                fullSR.SetMT2Bins(2, mt2bins);}
      else if(njets_lo == 10 && nbjets_lo == 1){float mt2bins[4] = {200, 300, 400, 1800};           fullSR.SetMT2Bins(3, mt2bins);}
      SRVec.push_back(fullSR);
    }

    // --- super signal regions, numbered 50 and up
    //     inclusive in HT, NJ, NB
    //     MT2 binning matches the most granular TR that would be included
    //     MT2 cut will be applied later by summing appropriate datacards

    // 2j loose
    sr.SetName("50");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr50[] = {200, 400, 600, 800, 1000, 1200, 1800};
    sr.SetMT2Bins(6, mt2bins_sr50);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 2j tight
    sr.SetName("51");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr51[] = {200, 400, 600, 800, 1000, 1400, 1800};
    sr.SetMT2Bins(6, mt2bins_sr51);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 4j loose
    sr.SetName("52");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr52[] = {200, 400, 600, 800, 1000, 1200, 1800};
    sr.SetMT2Bins(6, mt2bins_sr52);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 4j tight
    sr.SetName("53");
    sr.SetVar("njets", 4, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 4, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 4, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr53[] = {200, 400, 600, 800, 1000, 1400, 1800};
    sr.SetMT2Bins(6, mt2bins_sr53);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j loose
    sr.SetName("54");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr54[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr54);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j tight
    sr.SetName("55");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 0, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 0, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 0, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr55[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr55);
    SRVec.push_back(sr);
    sr.Clear();
    

    // 2b loose
    sr.SetName("56");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr56[] = {200, 400, 600, 800, 1000, 1800};
    sr.SetMT2Bins(5, mt2bins_sr56);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 2b tight
    sr.SetName("57");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 2, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 2, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 2, -1);
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr57[] = {200, 400, 600, 800, 1800};
    sr.SetMT2Bins(4, mt2bins_sr57);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 3b loose
    sr.SetName("58");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr58[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr58);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 3b tight
    sr.SetName("59");
    sr.SetVar("njets", 2, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 2, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 2, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr59[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr59);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j3b loose
    sr.SetName("60");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", 1000, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", 1000, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", 1000, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", 1000, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr60[] = {200, 400, 600, 1800};
    sr.SetMT2Bins(3, mt2bins_sr60);
    SRVec.push_back(sr);
    sr.Clear();
    
    // 7j3b tight
    sr.SetName("61");
    sr.SetVar("njets", 7, -1);
    sr.SetVar("nbjets", 3, -1);
    sr.SetVar("ht", H_UH_division, -1);
    sr.SetVar("met", 30, -1);
    sr.SetVarCRSL("njets", 7, -1);
    sr.SetVarCRSL("nbjets", 3, -1);
    sr.SetVarCRSL("ht", H_UH_division, -1);
    sr.SetVarCRSL("met", 30, -1);
    sr.SetVarCRDY("njets", 7, -1);
    sr.SetVarCRDY("nbjets", 1, -1); // DY: NB>0
    sr.SetVarCRDY("ht", H_UH_division, -1);
    sr.SetVarCRDY("met", 30, -1);
    sr.SetVarCRQCD("ht", H_UH_division, -1);
    sr.SetVarCRQCD("met", 30, -1);
    sr.SetVar("PJ1eta", 0, -1);
    sr.SetVarCRSL("PJ1eta", 0, -1);
    sr.SetVarCRDY("PJ1eta", 0, -1);
    sr.SetVarCRQCD("PJ1eta", 0, -1);
    float mt2bins_sr61[] = {200, 400, 1800};
    sr.SetMT2Bins(2, mt2bins_sr61);
    SRVec.push_back(sr);
    sr.Clear();
    
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

    //add baseline selections to all CRDY regions 
    std::vector<std::string> varsCRDY = baseSR.GetListOfVariablesCRDY();
    for(unsigned int i = 0; i < SRVec.size(); i++){
      for(unsigned int j = 0; j < varsCRDY.size(); j++){
        SRVec.at(i).SetVarCRDY(varsCRDY.at(j), baseSR.GetLowerBoundCRDY(varsCRDY.at(j)), baseSR.GetUpperBoundCRDY(varsCRDY.at(j)));
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
