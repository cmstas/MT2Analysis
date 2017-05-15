#include "sigSelectionsMT2Higgs.h"

namespace mt2 {

std::vector<SR> getSignalRegionsMT2Higgs() {
  std::vector<SR> SRVec = getSignalRegionsHcand();
  std::vector<SR> tempVec = getSignalRegionsMbbMax();
  SRVec.insert(SRVec.end(), tempVec.begin(), tempVec.end());
  tempVec = getSignalRegionsZcand();
  SRVec.insert(SRVec.end(), tempVec.begin(), tempVec.end());

  // -- base regions --
  SR SRBase;
  SRBase.SetVarAll("j1pt", 30, -1);
  SRBase.SetVarAll("j2pt", 30, -1);
  SRBase.SetVarAll("diffMetMhtOverMet", 0, 0.5);
  SRBase.SetVarAll("njets", 2, -1);
  SRBase.SetVarAll("nbjets", 2, -1);
  SRBase.SetVarAll("passesHtMet", 1, 2);
  SRBase.SetVarAll("ht", 250, -1);
  SRBase.SetVarAll("met", 30, -1);
  SRBase.SetVarAll("minMTbmet", 0, -1);

  SRBase.SetVar("nlep", 0, 1);
  SRBase.SetVarCRSL("nlep", 1, 2);
  SRBase.SetVarCRQCD("nlep", 0, 1);
  SRBase.SetVar("mt2", 200, -1);
  SRBase.SetVarCRSL("mt2", 200, -1);
  SRBase.SetVarCRQCD("mt2", 0, 200);
  SRBase.SetVar("deltaPhiMin", 0.3, -1);
  SRBase.SetVarCRSL("deltaPhiMin", 0.3, -1);
  SRBase.SetVarCRQCD("deltaPhiMin", 0, 0.3);
  // SRBase.SetVarCRQCD("nbjets", 0, -1);
  SRBase.SetMT2Bins({200, 300, 400, 500, 700, 1500});

  SRBase.SetName("hbase");
  // SRBase.SetVarAll("njets", 0, -1);
  SRBase.SetVarAll("nhcand", 1, -1);
  SRBase.SetVarAll("mbbmax", 0, -1);
  SRBase.SetVarAll("nZcand", 0, -1);
  SRVec.insert(SRVec.begin(), SRBase);

  SRBase.SetName("hbase2");
  SRBase.SetVarAll("njets", 4, -1);
  SRVec.insert(SRVec.begin()+1, SRBase);

  SRBase.SetName("Zbase");
  SRBase.SetVarAll("njets", 2, -1);
  SRBase.SetVarAll("nhcand", 0, -1);
  SRBase.SetVarAll("nZcand", 1, -1);
  SRVec.insert(SRVec.begin()+2, SRBase);

  SRBase.SetName("Zbase2");
  SRBase.SetVarAll("njets", 4, -1);
  SRVec.insert(SRVec.begin()+3, SRBase);

  SRBase.SetName("Hbase");
  SRBase.SetVarAll("njets", 2, -1);
  SRBase.SetVarAll("nZcand", 0, -1);
  SRBase.SetVarAll("mbbmax", 300, -1);
  SRVec.insert(SRVec.begin()+4, SRBase);

  SRBase.SetName("Hbase2");
  SRBase.SetVarAll("njets", 4, -1);
  SRVec.insert(SRVec.begin()+5, SRBase);

  SRBase.SetName("hIncl2h");
  SRBase.SetVarAll("njets", 4, -1);
  SRBase.SetVarAll("nbjets", 4, -1);
  SRBase.SetVarAll("nhcand", 2, -1);
  SRBase.SetVarAll("mbbmax", 0, -1);
  SRBase.SetVarAll("nZcand", 0, -1);
  float Incl2h_mt2bins[3] = {200, 300, 1500};
  SRBase.SetMT2Bins(2, Incl2h_mt2bins);
  SRVec.push_back(SRBase);


  return SRVec;
}

std::vector<SR> getSignalRegionsHcand() {

  std::vector<SR> temp_SR_VL_vec;
  std::vector<SR> temp_SR_MH_vec;
  std::vector<SR> SRVec;
  SR sr;
  SR baseSR;

  // first set binning in njet-nbjet plane
  // SR for VL and L regions, inclusive for >= 7 jets
  sr.SetName("h1");
  sr.SetVarAll("njets", 4, -1);
  sr.SetVarAll("nbjets", 2, -1);
  temp_SR_VL_vec.push_back(sr);
  sr.Clear();

  // SR for M and H regions, separate for >= 7 jets
  sr.SetName("h1");
  sr.SetVarAll("njets", 4, 7);
  sr.SetVarAll("nbjets", 2, -1);
  temp_SR_MH_vec.push_back(sr);
  sr.Clear();

  sr.SetName("h3");
  sr.SetVarAll("njets", 7, -1);
  sr.SetVarAll("nbjets", 2, -1);
  temp_SR_MH_vec.push_back(sr);
  sr.Clear();

  //add HT and MET requirements
  for(unsigned int iSR = 0; iSR < temp_SR_VL_vec.size(); iSR++){
    SR fullSR = temp_SR_VL_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "VL";
    fullSR.SetVarAll("ht", 250, 450);
    fullSR.SetVarAll("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    fullSR.SetName(fullSRname + "H");
    fullSR.SetVar("minMTbmet", 200, -1);
    fullSR.SetVarCRSL("minMTbmet", 200, -1);
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 300, 350, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "L");
    fullSR.SetVar("minMTbmet", 0, 200);
    fullSR.SetVarCRSL("minMTbmet", 0, 200);
    // if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_VL_vec.size(); iSR++){
    SR fullSR = temp_SR_VL_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "L";
    fullSR.SetVarAll("ht", 450, 575);
    fullSR.SetVarAll("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    fullSR.SetName(fullSRname + "H");
    fullSR.SetVar("minMTbmet", 200, -1);
    fullSR.SetVarCRSL("minMTbmet", 200, -1);
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 350, 450, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "L");
    fullSR.SetVar("minMTbmet", 0, 200);
    fullSR.SetVarCRSL("minMTbmet", 0, 200);
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 350, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_MH_vec.size(); iSR++){
    SR fullSR = temp_SR_MH_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "M";
    fullSR.SetVarAll("ht", 575, 1000);
    fullSR.SetVarAll("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    fullSR.SetName(fullSRname + "H");
    fullSR.SetVar("minMTbmet", 200, -1);
    fullSR.SetVarCRSL("minMTbmet", 200, -1);
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 400, 600, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[3] = {200, 400, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "L");
    fullSR.SetVar("minMTbmet", 0, 200);
    fullSR.SetVarCRSL("minMTbmet", 0, 200);
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 400, 500, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[4] = {200, 350, 450, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_MH_vec.size(); iSR++){
    SR fullSR = temp_SR_MH_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "H";
    fullSR.SetVarAll("ht", 1000, -1);
    fullSR.SetVarAll("met", 30, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    fullSR.SetName(fullSRname + "H");
    fullSR.SetVar("minMTbmet", 200, -1);
    fullSR.SetVarCRSL("minMTbmet", 200, -1);
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[3] = {200, 500, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    else if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[3] = {200, 450, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "L");
    fullSR.SetVar("minMTbmet", 0, 200);
    fullSR.SetVarCRSL("minMTbmet", 0, 200);
    if      (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[4] = {200, 300, 450, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    // if      (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[3] = {200, 500, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
  }

  //define baseline selections commmon to all signal regions
  baseSR.SetVar("mt2", 200, -1);
  baseSR.SetVar("j1pt", 30, -1);
  baseSR.SetVar("j2pt", 30, -1);
  baseSR.SetVar("deltaPhiMin", 0.3, -1);
  baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVar("nlep", 0, 1);
  baseSR.SetVar("passesHtMet", 0, -1);
  baseSR.SetVar("nhcand", 1, 2);
  baseSR.SetVar("nZcand", 0, -1);
  baseSR.SetVar("mbbmax", 0, -1);

  // common selections for CRSL
  baseSR.SetVarCRSL("mt2", 200, -1);
  baseSR.SetVarCRSL("j1pt", 30, -1);
  baseSR.SetVarCRSL("j2pt", 30, -1);
  baseSR.SetVarCRSL("deltaPhiMin", 0.3, -1);
  baseSR.SetVarCRSL("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRSL("nlep", 1, 2);
  baseSR.SetVarCRSL("passesHtMet", 0, -1);
  baseSR.SetVarCRSL("nhcand", 1, 2);
  baseSR.SetVarCRSL("nZcand", 0, -1);
  baseSR.SetVarCRSL("mbbmax", 0, -1);

  // common selections for QCD
  baseSR.SetVarCRQCD("mt2", 0, 200);
  baseSR.SetVarCRQCD("j1pt", 30, -1);
  baseSR.SetVarCRQCD("j2pt", 30, -1);
  baseSR.SetVarCRQCD("deltaPhiMin", 0., 0.3);
  baseSR.SetVarCRQCD("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRQCD("nlep", 0, 1);
  baseSR.SetVarCRQCD("minMTbmet", 0, -1);
  baseSR.SetVarCRQCD("passesHtMet", 0, -1);
  baseSR.SetVarCRQCD("nhcand", 1, 2);
  baseSR.SetVarCRQCD("nZcand", 0, -1);
  baseSR.SetVarCRQCD("mbbmax", 0, -1);

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

std::vector<SR> getSignalRegionsMbbMax() {

  std::vector<SR> temp_SR_VL_vec;
  std::vector<SR> temp_SR_MH_vec;
  std::vector<SR> SRVec;
  SR sr;
  SR baseSR;

  // SR for VL and L regions, inclusive for >= 7 jets
  sr.SetName("H1");
  sr.SetVarAll("njets", 4, -1);
  sr.SetVarAll("nbjets", 2, -1);
  sr.SetVarAll("mbbmax", 300, -1);
  temp_SR_VL_vec.push_back(sr);
  sr.Clear();

  // SR for M and H regions, separate for >= 7 jets
  sr.SetName("H1");
  sr.SetVarAll("njets", 4, 7);
  sr.SetVarAll("nbjets", 2, -1);
  sr.SetVarAll("mbbmax", 300, -1);
  temp_SR_MH_vec.push_back(sr);
  sr.Clear();

  sr.SetName("H3");
  sr.SetVarAll("njets", 7, -1);
  sr.SetVarAll("nbjets", 2, -1);
  sr.SetVarAll("mbbmax", 300, -1);
  temp_SR_MH_vec.push_back(sr);
  sr.Clear();

  //add HT and MET requirements
  for(unsigned int iSR = 0; iSR < temp_SR_VL_vec.size(); iSR++){
    SR fullSR = temp_SR_VL_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "L";
    fullSR.SetName(fullSRname);
    fullSR.SetVarAll("ht", 250, 575);
    fullSR.SetVarAll("met", 250, -1);
    fullSR.SetVar("minMTbmet", 0, -1);
    fullSR.SetVarCRSL("minMTbmet", 0, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_MH_vec.size(); iSR++){
    SR fullSR = temp_SR_MH_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "M";
    fullSR.SetVarAll("ht", 575, 1000);
    fullSR.SetVarAll("met", 250, -1);
    fullSR.SetVar("minMTbmet", 0, -1);
    fullSR.SetVarCRSL("minMTbmet", 0, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    fullSR.SetName(fullSRname + "U");
    fullSR.SetVar("mbbmax", 500, -1);
    fullSR.SetVarCRSL("mbbmax", 500, -1);
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 400, 550, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[3] = {200, 400, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "M");
    fullSR.SetVar("mbbmax", 300, 500);
    fullSR.SetVarCRSL("mbbmax", 300, 500);
    if      (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[4] = {200, 300, 400, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_MH_vec.size(); iSR++){
    SR fullSR = temp_SR_MH_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "H";
    fullSR.SetVarAll("ht", 1000, -1);
    fullSR.SetVarAll("met", 30, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    fullSR.SetName(fullSRname + "U" + "H");
    fullSR.SetVar("mbbmax", 500, -1);
    fullSR.SetVar("minMTbmet", 200, -1);
    fullSR.SetVarCRSL("mbbmax", 500, -1);
    fullSR.SetVarCRSL("minMTbmet", 200, -1);
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 400, 700, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[3] = {200, 400, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "M" + "H");
    fullSR.SetVar("mbbmax", 300, 500);
    fullSR.SetVarCRSL("mbbmax", 300, 500);
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[3] = {200, 700, 1500}; fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "M" + "L");
    fullSR.SetVar("minMTbmet", 0, 200);
    fullSR.SetVarCRSL("minMTbmet", 0, 200);
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 300, 500, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[4] = {200, 300, 500, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "U" + "L");
    fullSR.SetVar("mbbmax", 500, -1);
    fullSR.SetVarCRSL("mbbmax", 500, -1);
    // if      (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[3] = {200, 400, 1500}; fullSR.SetMT2Bins(2, mt2bins);}
    if      (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[4] = {200, 300, 450, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }

  //define baseline selections commmon to all signal regions
  baseSR.SetVar("mt2", 200, -1);
  baseSR.SetVar("j1pt", 30, -1);
  baseSR.SetVar("j2pt", 30, -1);
  baseSR.SetVar("deltaPhiMin", 0.3, -1);
  baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVar("nlep", 0, 1);
  baseSR.SetVar("passesHtMet", 1, 2);
  baseSR.SetVar("nhcand", 0, -1);
  baseSR.SetVar("nZcand", 0, -1);
  // baseSR.SetVar("mbbmax", 0, -1);

  // common selections for CRSL
  baseSR.SetVarCRSL("mt2", 200, -1);
  baseSR.SetVarCRSL("j1pt", 30, -1);
  baseSR.SetVarCRSL("j2pt", 30, -1);
  baseSR.SetVarCRSL("deltaPhiMin", 0.3, -1);
  baseSR.SetVarCRSL("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRSL("nlep", 1, 2);
  baseSR.SetVarCRSL("passesHtMet", 1, 2);
  baseSR.SetVarCRSL("nhcand", 0, -1);
  baseSR.SetVarCRSL("nZcand", 0, -1);
  // baseSR.SetVarCRSL("mbbmax", 0, -1);

  // common selections for QCD
  baseSR.SetVarCRQCD("mt2", 0, 200);
  baseSR.SetVarCRQCD("j1pt", 30, -1);
  baseSR.SetVarCRQCD("j2pt", 30, -1);
  baseSR.SetVarCRQCD("deltaPhiMin", 0., 0.3);
  baseSR.SetVarCRQCD("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRQCD("nlep", 0, 1);
  baseSR.SetVarCRQCD("passesHtMet", 1, 2);
  baseSR.SetVarCRQCD("minMTbmet", 0, -1);
  baseSR.SetVarCRQCD("nhcand", 0, -1);
  baseSR.SetVarCRQCD("nZcand", 0, -1);
  baseSR.SetVarCRQCD("mbbmax", 0, -1);

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

std::vector<SR> getSignalRegionsZcand() {

  std::vector<SR> temp_SR_VL_vec;
  std::vector<SR> temp_SR_MH_vec;
  std::vector<SR> SRVec;
  SR sr;
  SR baseSR;

  // first set binning in njet-nbjet plane
  // SR for VL and L regions, inclusive for >= 7 jets
  sr.SetName("Z1");
  sr.SetVarAll("njets", 4, -1);
  sr.SetVarAll("nbjets", 2, -1);
  temp_SR_VL_vec.push_back(sr);
  sr.Clear();

  // SR for M and H regions, separate for >= 7 jets
  sr.SetName("Z1");
  sr.SetVarAll("njets", 4, 7);
  sr.SetVarAll("nbjets", 2, -1);
  temp_SR_MH_vec.push_back(sr);
  sr.Clear();

  sr.SetName("Z3");
  sr.SetVarAll("njets", 7, -1);
  sr.SetVarAll("nbjets", 2, -1);
  temp_SR_MH_vec.push_back(sr);
  sr.Clear();

  //add HT and MET requirements
  for(unsigned int iSR = 0; iSR < temp_SR_VL_vec.size(); iSR++){
    SR fullSR = temp_SR_VL_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "VL";
    fullSR.SetName(fullSRname);
    fullSR.SetVarAll("ht", 250, 450);
    fullSR.SetVarAll("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    // if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[3] = {300, 375, 1500}; fullSR.SetMT2Bins(2, mt2bins);}
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 300, 375, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_VL_vec.size(); iSR++){
    SR fullSR = temp_SR_VL_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "L";
    fullSR.SetName(fullSRname);
    fullSR.SetVarAll("ht", 450, 575);
    fullSR.SetVarAll("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    // if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[3] = {300, 450, 1500}; fullSR.SetMT2Bins(2, mt2bins);}
    // if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {300, 375, 450, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[5] = {200, 300, 375, 450, 1500}; fullSR.SetMT2Bins(4, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_MH_vec.size(); iSR++){
    SR fullSR = temp_SR_MH_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "M";
    fullSR.SetName(fullSRname);
    fullSR.SetVarAll("ht", 575, 1000);
    fullSR.SetVarAll("met", 250, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 400, 600, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[4] = {200, 350, 500, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_MH_vec.size(); iSR++){
    SR fullSR = temp_SR_MH_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "H";
    fullSR.SetName(fullSRname);
    fullSR.SetVarAll("ht", 1000, -1);
    fullSR.SetVarAll("met", 30, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 350, 500, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[4] = {200, 350, 500, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }

  //define baseline selections commmon to all signal regions
  baseSR.SetVar("mt2", 200, -1);
  baseSR.SetVar("j1pt", 30, -1);
  baseSR.SetVar("j2pt", 30, -1);
  baseSR.SetVar("deltaPhiMin", 0.3, -1);
  baseSR.SetVar("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVar("nlep", 0, 1);
  baseSR.SetVar("passesHtMet", 1, 2);
  baseSR.SetVar("minMTbmet", 0, -1);
  baseSR.SetVar("nhcand", 0, -1);
  baseSR.SetVar("nZcand", 1, -1);
  baseSR.SetVar("mbbmax", 0, -1);

  // common selections for CRSL
  baseSR.SetVarCRSL("mt2", 200, -1);
  baseSR.SetVarCRSL("j1pt", 30, -1);
  baseSR.SetVarCRSL("j2pt", 30, -1);
  baseSR.SetVarCRSL("deltaPhiMin", 0.3, -1);
  baseSR.SetVarCRSL("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRSL("nlep", 1, 2);
  baseSR.SetVarCRSL("passesHtMet", 1, 2);
  baseSR.SetVarCRSL("minMTbmet", 0, -1);
  baseSR.SetVarCRSL("nhcand", 0, -1);
  baseSR.SetVarCRSL("nZcand", 1, -1);
  baseSR.SetVarCRSL("mbbmax", 0, -1);

  // common selections for QCD
  baseSR.SetVarCRQCD("mt2", 0, 200);
  baseSR.SetVarCRQCD("j1pt", 30, -1);
  baseSR.SetVarCRQCD("j2pt", 30, -1);
  baseSR.SetVarCRQCD("deltaPhiMin", 0., 0.3);
  baseSR.SetVarCRQCD("diffMetMhtOverMet", 0, 0.5);
  baseSR.SetVarCRQCD("nlep", 0, 1);
  baseSR.SetVarCRQCD("passesHtMet", 1, 2);
  baseSR.SetVarCRQCD("minMTbmet", 0, -1);
  baseSR.SetVarCRQCD("nhcand", 0, -1);
  baseSR.SetVarCRQCD("nZcand", 1, -1);
  baseSR.SetVarCRQCD("mbbmax", 0, -1);

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


std::vector<SR> getTestRegionsMT2Higgs() {

  std::vector<SR> SRVec;

  SR SRBase;
  SRBase.SetVarAll("j1pt", 30, -1);
  SRBase.SetVarAll("j2pt", 30, -1);
  SRBase.SetVarAll("diffMetMhtOverMet", 0, 0.5);
  SRBase.SetVarAll("njets", 2, -1);
  SRBase.SetVarAll("nbjets", 2, -1);
  SRBase.SetVarAll("passesHtMet", 1, 2);
  SRBase.SetVarAll("ht", 250, -1);
  SRBase.SetVarAll("met", 30, -1);

  SRBase.SetVar("nlep", 0, 1);
  SRBase.SetVarCRSL("nlep", 1, 2);
  SRBase.SetVarCRQCD("nlep", 0, 1);
  SRBase.SetVar("mt2", 200, -1);
  SRBase.SetVarCRSL("mt2", 200, -1);
  SRBase.SetVarCRQCD("mt2", 0, 200);
  SRBase.SetVar("deltaPhiMin", 0.3, -1);
  SRBase.SetVarCRSL("deltaPhiMin", 0.3, -1);
  SRBase.SetVarCRQCD("deltaPhiMin", 0, 0.3);
  SRBase.SetMT2Bins({200, 300, 400, 500, 700, 1500});

  SRBase.SetVarAll("nhcand", 0, -1);
  SRBase.SetVarAll("mbbmax", 0, -1);
  SRBase.SetVarAll("nZcand", 0, -1);
  SRBase.SetVarAll("minMTbmet", 0, -1);

  // -- variable dependency checks --
  std::vector<SR> temp_test_vec;
  SRBase.SetName("hAllIncl2b");
  SRBase.SetVarAll("nbjets", 2, 3);
  SRBase.SetMT2Bins({200, 1500});
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllIncl3b");
  SRBase.SetVarAll("nbjets", 3, 4);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllInclgeq4b");
  SRBase.SetVarAll("nbjets", 4, -1);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);

  for (SR sr : temp_test_vec) {
    sr.SetName(sr.GetName().replace(0, 4, "h"));
    sr.SetVarAll("nhcand", 1, -1);
    SRVec.push_back(sr);
  }
  for (SR sr : temp_test_vec) {
    sr.SetName(sr.GetName().replace(0, 4, "H"));
    sr.SetVarAll("mbbmax", 300, -1);
    SRVec.push_back(sr);
  }
  for (SR sr : temp_test_vec) {
    sr.SetName(sr.GetName().replace(0, 4, "Z"));
    sr.SetVarAll("nZcand", 1, -1);
    SRVec.push_back(sr);
  }

  temp_test_vec.clear();
  SRBase.SetVarAll("nbjets", 2, -1);

  SRBase.SetName("hAllIncl2j");
  SRBase.SetVarAll("njets", 2, 3);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllIncl3j");
  SRBase.SetVarAll("njets", 3, 4);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllIncl4j");
  SRBase.SetVarAll("njets", 4, 5);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllIncl5to6j");
  SRBase.SetVarAll("njets", 5, 7);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllInclgeq7j");
  SRBase.SetVarAll("njets", 7, -1);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);

  for (SR sr : temp_test_vec) {
    sr.SetName(sr.GetName().replace(0, 4, "h"));
    sr.SetVarAll("nhcand", 1, -1);
    SRVec.push_back(sr);
  }
  for (SR sr : temp_test_vec) {
    sr.SetName(sr.GetName().replace(0, 4, "H"));
    sr.SetVarAll("mbbmax", 300, -1);
    SRVec.push_back(sr);
  }
  for (SR sr : temp_test_vec) {
    sr.SetName(sr.GetName().replace(0, 4, "Z"));
    sr.SetVarAll("nZcand", 1, -1);
    SRVec.push_back(sr);
  }

  temp_test_vec.clear();
  SRBase.SetVarAll("njets", 2, -1);
  SRBase.SetVarAll("met", 250, -1);

  SRBase.SetName("hAllInclht");
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllInclhtVL");
  SRBase.SetVarAll("ht", 250, 450);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllInclhtL");
  SRBase.SetVarAll("ht", 450, 575);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllInclhtM");
  SRBase.SetVarAll("ht", 575, 1000);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllInclhtH");
  SRBase.SetVarAll("ht", 1000, 1500);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllInclhtUH");
  SRBase.SetVarAll("ht", 1500, -1);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);

  for (SR sr : temp_test_vec) {
    sr.SetName(sr.GetName().replace(0, 4, "h"));
    sr.SetVarAll("nhcand", 1, -1);
    SRVec.push_back(sr);
  }
  for (SR sr : temp_test_vec) {
    sr.SetName(sr.GetName().replace(0, 4, "H"));
    sr.SetVarAll("mbbmax", 300, -1);
    SRVec.push_back(sr);
  }
  for (SR sr : temp_test_vec) {
    sr.SetName(sr.GetName().replace(0, 4, "Z"));
    sr.SetVarAll("nZcand", 1, -1);
    SRVec.push_back(sr);
  }

  temp_test_vec.clear();
  SRBase.SetVarAll("ht", 250, -1);
  SRBase.SetVarAll("passesHtMet", 0, 2);

  SRBase.SetName("hAllInclmet");
  SRBase.SetVarAll("met", 30, -1);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllInclmetVL");
  SRBase.SetVarAll("met", 30, 250);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllInclmetL");
  SRBase.SetVarAll("met", 250, 300);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllInclmetM");
  SRBase.SetVarAll("met", 300, 400);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);
  SRBase.SetName("hAllInclmetH");
  SRBase.SetVarAll("met", 400, -1);
  SRVec.push_back(SRBase);
  temp_test_vec.push_back(SRBase);

  for (SR sr : temp_test_vec) {
    sr.SetName(sr.GetName().replace(0, 4, "h"));
    sr.SetVarAll("nhcand", 1, -1);
    SRVec.push_back(sr);
  }
  for (SR sr : temp_test_vec) {
    sr.SetName(sr.GetName().replace(0, 4, "H"));
    sr.SetVarAll("mbbmax", 300, -1);
    SRVec.push_back(sr);
  }
  for (SR sr : temp_test_vec) {
    sr.SetName(sr.GetName().replace(0, 4, "Z"));
    sr.SetVarAll("nZcand", 1, -1);
    SRVec.push_back(sr);
  }

  temp_test_vec.clear();
  SRBase.SetVarAll("passesHtMet", 1, 2);
  SRBase.SetVarAll("met", 30, -1);
  SRBase.SetName("hAllInclmt2");
  SRBase.SetMT2Bins({0, 30, 100, 200, 300, 400, 500, 700, 1500});
  SRVec.push_back(SRBase);

  SRBase.SetName("hInclmt2");
  SRBase.SetVarAll("nhcand", 1, -1);
  SRVec.push_back(SRBase);

  SRBase.SetName("HInclmt2");
  SRBase.SetVarAll("nhcand", 0, -1);
  SRBase.SetVarAll("mbbmax", 300, -1);
  SRVec.push_back(SRBase);

  SRBase.SetName("ZInclmt2");
  SRBase.SetVarAll("mbbmax", 0, -1);
  SRBase.SetVarAll("nZcand", 1, -1);
  SRVec.push_back(SRBase);


  return SRVec;
}

} // namespace mt2
