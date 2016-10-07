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
  SRBase.SetVarAll("njets", 4, -1);
  SRBase.SetVarAll("nbjets", 2, -1);
  SRBase.SetVarAll("passesHtMet", 1, 2);
  SRBase.SetVarAll("ht", 200, -1);
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
  float SRBase_mt2bins[4] = {200, 300, 500, 1000}; 
  SRBase.SetMT2Bins(3, SRBase_mt2bins);

  SRBase.SetName("Incl2h");
  SRBase.SetVarAll("nhcand", 2, -1);
  SRBase.SetVarAll("mbbmax", 0, -1);
  SRBase.SetVarAll("nZcand", 0, -1);
  SRVec.push_back(SRBase);

  SRBase.SetName("hbase");
  SRBase.SetVarAll("njets", 2, -1);
  SRBase.SetVarAll("nhcand", 1, -1);
  SRVec.insert(SRVec.begin(), SRBase);

  SRBase.SetName("Zbase");
  SRBase.SetVarAll("nhcand", 0, -1);
  SRBase.SetVarAll("nZcand", 1, -1);
  SRVec.insert(SRVec.begin()+1, SRBase);

  SRBase.SetName("Hbase");
  SRBase.SetVarAll("nZcand", 0, -1);
  SRBase.SetVarAll("mbbmax", 300, -1);
  SRVec.insert(SRVec.begin()+2, SRBase);

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
  // sr.SetVarCRSL("njets", 4, -1);
  // sr.SetVarCRSL("nbjets", 2, 3);
  temp_SR_VL_vec.push_back(sr);
  sr.Clear();

  // sr.SetName("h2");
  // sr.SetVar("njets", 4, -1);
  // sr.SetVar("nbjets", 3, -1);
  // sr.SetVarCRSL("njets", 4, -1);
  // sr.SetVarCRSL("nbjets", 3, -1);
  // temp_SR_VL_vec.push_back(sr);
  // sr.Clear();

  // SR for M and H regions, separate for >= 7 jets
  sr.SetName("h1");
  sr.SetVarAll("njets", 4, 7);
  sr.SetVarAll("nbjets", 2, -1);
  // sr.SetVarCRSL("njets", 4, 7);
  // sr.SetVarCRSL("nbjets", 2, 3);
  temp_SR_MH_vec.push_back(sr);
  sr.Clear();

  // sr.SetName("h2");
  // sr.SetVar("njets", 4, 7);
  // sr.SetVar("nbjets", 3, -1);
  // sr.SetVarCRSL("njets", 4, 7);
  // sr.SetVarCRSL("nbjets", 3, -1);
  // temp_SR_MH_vec.push_back(sr);
  // sr.Clear();

  sr.SetName("h3");
  sr.SetVarAll("njets", 7, -1);
  sr.SetVarAll("nbjets", 2, -1);
  // sr.SetVarCRSL("njets", 7, -1);
  // sr.SetVarCRSL("nbjets", 2, -1);
  temp_SR_MH_vec.push_back(sr);
  sr.Clear();

  // sr.SetName("h4");
  // sr.SetVar("njets", 7, -1);
  // sr.SetVar("nbjets", 3, -1);
  // sr.SetVarCRSL("njets", 7, -1);
  // sr.SetVarCRSL("nbjets", 1, 3);
  // temp_SR_vec.push_back(sr);
  // sr.Clear();

  //add HT and MET requirements
  for(unsigned int iSR = 0; iSR < temp_SR_VL_vec.size(); iSR++){
    SR fullSR = temp_SR_VL_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "VL";
    fullSR.SetVar("ht", 200, 450);
    fullSR.SetVar("met", 200, -1);
    fullSR.SetVarCRSL("ht", 200, 450);
    fullSR.SetVarCRSL("met", 200, -1);
    fullSR.SetVarCRQCD("ht", 200, 450);
    fullSR.SetVarCRQCD("met", 200, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[3] = {300, 400, 1500}; fullSR.SetMT2Bins(2, mt2bins);}
    // else if (njets_lo == 4 && nbjets_lo == 3) {float mt2bins[3] = {200, 350, 1500}; fullSR.SetMT2Bins(2, mt2bins);}
    fullSR.SetName(fullSRname + "H");
    fullSR.SetVar("minMTbmet", 200, -1);
    fullSR.SetVarCRSL("minMTbmet", 200, -1);
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "L");
    fullSR.SetVar("minMTbmet", 0, 200);
    fullSR.SetVarCRSL("minMTbmet", 0, 200);
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_VL_vec.size(); iSR++){
    SR fullSR = temp_SR_VL_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "L";
    fullSR.SetVar("ht", 450, 575);
    fullSR.SetVar("met", 200, -1);
    fullSR.SetVarCRSL("ht", 450, 575);
    fullSR.SetVarCRSL("met", 200, -1);
    fullSR.SetVarCRQCD("ht", 450, 575);
    fullSR.SetVarCRQCD("met", 200, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {300, 400, 500, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    // else if (njets_lo == 4 && nbjets_lo == 3) {float mt2bins[3] = {200, 350, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    fullSR.SetName(fullSRname + "H");
    fullSR.SetVar("minMTbmet", 200, -1);
    fullSR.SetVarCRSL("minMTbmet", 200, -1);
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "L");
    fullSR.SetVar("minMTbmet", 0, 200);
    fullSR.SetVarCRSL("minMTbmet", 0, 200);
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_MH_vec.size(); iSR++){
    SR fullSR = temp_SR_MH_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "M";
    fullSR.SetVar("ht", 575, 1000);
    fullSR.SetVar("met", 200, -1);
    fullSR.SetVarCRSL("ht", 575, 1000);
    fullSR.SetVarCRSL("met", 200, -1);
    fullSR.SetVarCRQCD("ht", 575, 1000);
    fullSR.SetVarCRQCD("met", 200, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 450, 550, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if (njets_lo == 4 && nbjets_lo == 3) {float mt2bins[3] = {200, 450, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    else if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[3] = {200, 350, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    fullSR.SetName(fullSRname + "H");
    fullSR.SetVar("minMTbmet", 200, -1);
    fullSR.SetVarCRSL("minMTbmet", 200, -1);
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "L");
    fullSR.SetVar("minMTbmet", 0, 200);
    fullSR.SetVarCRSL("minMTbmet", 0, 200);
    if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[4] = {200, 350, 450, 1500};      fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_MH_vec.size(); iSR++){
    SR fullSR = temp_SR_MH_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "H";
    fullSR.SetVar("ht", 1000, -1);
    fullSR.SetVar("met", 30, -1);
    fullSR.SetVarCRSL("ht", 1000, -1);
    fullSR.SetVarCRSL("met", 30, -1);
    fullSR.SetVarCRQCD("ht", 1000, -1);
    fullSR.SetVarCRQCD("met", 30, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[3] = {200, 450, 1500}; fullSR.SetMT2Bins(2, mt2bins);}
    else if (njets_lo == 4 && nbjets_lo == 3) {float mt2bins[2] = {200, 1500};      fullSR.SetMT2Bins(1, mt2bins);}
    else if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[3] = {200, 350, 1500}; fullSR.SetMT2Bins(2, mt2bins);}
    fullSR.SetName(fullSRname + "L");
    fullSR.SetVar("minMTbmet", 200, -1);
    fullSR.SetVarCRSL("minMTbmet", 200, -1);
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "H");
    fullSR.SetVar("minMTbmet", 0, 200);
    fullSR.SetVarCRSL("minMTbmet", 0, 200);
    if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[4] = {200, 350, 450, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
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
  baseSR.SetVarCRSL("passesHtMet", 1, 2);
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
  baseSR.SetVarCRQCD("passesHtMet", 1, 2);
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
  // sr.SetVarCRSL("njets", 4, -1);
  // sr.SetVarCRSL("nbjets", 2, -1);
  // sr.SetVarCRSL("mbbmax", 300, -1);
  temp_SR_VL_vec.push_back(sr);
  sr.Clear();

  // sr.SetName("H2");
  // sr.SetVar("njets", 4, -1);
  // sr.SetVar("nbjets", 3, -1);
  // sr.SetVarCRSL("njets", 4, -1);
  // sr.SetVarCRSL("nbjets", 3, -1);
  // temp_SR_VL_vec.push_back(sr);
  // sr.Clear();

  // SR for M and H regions, separate for >= 7 jets
  sr.SetName("H1");
  sr.SetVarAll("njets", 4, 7);
  sr.SetVarAll("nbjets", 2, -1);
  sr.SetVarAll("mbbmax", 300, -1);
  // sr.SetVarCRSL("njets", 4, 7);
  // sr.SetVarCRSL("nbjets", 2, -1);
  // sr.SetVarCRSL("mbbmax", 300, -1);
  temp_SR_MH_vec.push_back(sr);
  sr.Clear();

  // sr.SetName("H2");
  // sr.SetVar("njets", 4, 7);
  // sr.SetVar("nbjets", 3, -1);
  // sr.SetVarCRSL("njets", 4, 7);
  // sr.SetVarCRSL("nbjets", 3, -1);
  // temp_SR_MH_vec.push_back(sr);
  // sr.Clear();

  sr.SetName("H3");
  sr.SetVarAll("njets", 7, -1);
  sr.SetVarAll("nbjets", 2, -1);
  sr.SetVarAll("mbbmax", 300, -1);
  // sr.SetVarCRSL("njets", 7, -1);
  // sr.SetVarCRSL("nbjets", 2, -1);
  // sr.SetVarCRSL("mbbmax", 300, -1);
  temp_SR_MH_vec.push_back(sr);
  sr.Clear();

  //add HT and MET requirements
  // for(unsigned int iSR = 0; iSR < temp_SR_VL_vec.size(); iSR++){
  //   SR fullSR = temp_SR_VL_vec.at(iSR);
  //   std::string fullSRname = fullSR.GetName() + "VL";
  //   fullSR.SetName(fullSRname);
  //   fullSR.SetVar("ht", 200, 450);
  //   fullSR.SetVar("met", 200, -1);
  //   fullSR.SetVar("minMTbmet", 0, -1);
  //   fullSR.SetVarCRSL("ht", 200, 450);
  //   fullSR.SetVarCRSL("met", 200, -1);
  //   fullSR.SetVarCRSL("minMTbmet", 0, -1);
  //   fullSR.SetVarCRQCD("ht", 200, 450);
  //   fullSR.SetVarCRQCD("met", 200, -1);
  //   int njets_lo = fullSR.GetLowerBound("njets");
  //   int nbjets_lo = fullSR.GetLowerBound("nbjets");
  //   if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[3] = {200, 350, 1500}; fullSR.SetMT2Bins(2, mt2bins);}
  //   else if (njets_lo == 4 && nbjets_lo == 3) {float mt2bins[3] = {200, 300, 1500}; fullSR.SetMT2Bins(2, mt2bins);}
  //   SRVec.push_back(fullSR);
  // }
  for(unsigned int iSR = 0; iSR < temp_SR_VL_vec.size(); iSR++){
    SR fullSR = temp_SR_VL_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "L";
    fullSR.SetName(fullSRname);
    fullSR.SetVar("ht", 200, 575);
    fullSR.SetVar("met", 200, -1);
    fullSR.SetVar("minMTbmet", 0, -1);
    fullSR.SetVarCRSL("ht", 200, 575);
    fullSR.SetVarCRSL("met", 200, -1);
    fullSR.SetVarCRSL("minMTbmet", 0, -1);
    fullSR.SetVarCRQCD("ht", 200, 575);
    fullSR.SetVarCRQCD("met", 200, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 350, 450, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if (njets_lo == 4 && nbjets_lo == 3) {float mt2bins[3] = {200, 350, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_MH_vec.size(); iSR++){
    SR fullSR = temp_SR_MH_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "M";
    fullSR.SetName(fullSRname + "U");
    fullSR.SetVar("ht", 575, 1000);
    fullSR.SetVar("met", 200, -1);
    fullSR.SetVar("mbbmax", 500, -1);
    fullSR.SetVar("minMTbmet", 0, -1);
    fullSR.SetVarCRSL("ht", 575, 1000);
    fullSR.SetVarCRSL("met", 200, -1);
    fullSR.SetVarCRSL("mbbmax", 500, -1);
    fullSR.SetVarCRSL("minMTbmet", 0, -1);
    fullSR.SetVarCRQCD("ht", 575, 1000);
    fullSR.SetVarCRQCD("met", 200, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 400, 600, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if (njets_lo == 4 && nbjets_lo == 3) {float mt2bins[3] = {200, 500, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    else if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[4] = {200, 350, 500, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "M");
    fullSR.SetVar("mbbmax", 300, 500);
    fullSR.SetVarCRSL("mbbmax", 300, 500);
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_MH_vec.size(); iSR++){
    SR fullSR = temp_SR_MH_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "H";
    fullSR.SetName(fullSRname + "U" + "H");
    fullSR.SetVar("ht", 1000, -1);
    fullSR.SetVar("met", 30, -1);
    fullSR.SetVar("mbbmax", 500, -1);
    fullSR.SetVar("minMTbmet", 200, -1);
    fullSR.SetVarCRSL("ht", 1000, -1);
    fullSR.SetVarCRSL("met", 30, -1);
    fullSR.SetVarCRSL("mbbmax", 500, -1);
    fullSR.SetVarCRSL("minMTbmet", 200, -1);
    fullSR.SetVarCRQCD("ht", 1000, -1);
    fullSR.SetVarCRQCD("met", 30, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 400, 700, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if (njets_lo == 4 && nbjets_lo == 3) {float mt2bins[3] = {200, 500, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    else if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[4] = {200, 400, 600, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "M" + "H");
    fullSR.SetVar("mbbmax", 300, 500);
    fullSR.SetVarCRSL("mbbmax", 300, 500);
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "M" + "L");
    fullSR.SetVar("minMTbmet", 0, 200);
    fullSR.SetVarCRSL("minMTbmet", 0, 200);
    SRVec.push_back(fullSR);
    fullSR.SetName(fullSRname + "U" + "L");
    fullSR.SetVar("mbbmax", 500, -1);
    fullSR.SetVarCRSL("mbbmax", 500, -1);
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
  // sr.SetVarCRSL("njets", 4, -1);
  // sr.SetVarCRSL("nbjets", 2, 3);
  temp_SR_VL_vec.push_back(sr);
  sr.Clear();

  // sr.SetName("Z2");
  // sr.SetVar("njets", 4, -1);
  // sr.SetVar("nbjets", 3, -1);
  // sr.SetVarCRSL("njets", 4, -1);
  // sr.SetVarCRSL("nbjets", 3, -1);
  // temp_SR_VL_vec.push_back(sr);
  // sr.Clear();

  // SR for M and H regions, separate for >= 7 jets
  sr.SetName("Z1");
  sr.SetVarAll("njets", 4, 7);
  sr.SetVarAll("nbjets", 2, -1);
  // sr.SetVarCRSL("njets", 4, 7);
  // sr.SetVarCRSL("nbjets", 2, 3);
  temp_SR_MH_vec.push_back(sr);
  sr.Clear();

  // sr.SetName("Z2");
  // sr.SetVar("njets", 4, 7);
  // sr.SetVar("nbjets", 3, -1);
  // sr.SetVarCRSL("njets", 4, 7);
  // sr.SetVarCRSL("nbjets", 3, -1);
  // temp_SR_MH_vec.push_back(sr);
  // sr.Clear();

  sr.SetName("Z3");
  sr.SetVarAll("njets", 7, -1);
  sr.SetVarAll("nbjets", 2, -1);
  // sr.SetVarCRSL("njets", 7, -1);
  // sr.SetVarCRSL("nbjets", 2, -1);
  temp_SR_MH_vec.push_back(sr);
  sr.Clear();

  // sr.SetName("Z4");
  // sr.SetVar("njets", 7, -1);
  // sr.SetVar("nbjets", 3, -1);
  // sr.SetVarCRSL("njets", 7, -1);
  // sr.SetVarCRSL("nbjets", 1, 3);
  // temp_SR_vec.push_back(sr);
  // sr.Clear();

  //add HT and MET requirements
  for(unsigned int iSR = 0; iSR < temp_SR_VL_vec.size(); iSR++){
    SR fullSR = temp_SR_VL_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "VL";
    fullSR.SetName(fullSRname);
    fullSR.SetVar("ht", 200, 450);
    fullSR.SetVar("met", 200, -1);
    fullSR.SetVarCRSL("ht", 200, 450);
    fullSR.SetVarCRSL("met", 200, -1);
    fullSR.SetVarCRQCD("ht", 200, 450);
    fullSR.SetVarCRQCD("met", 200, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[3] = {300, 400, 1500}; fullSR.SetMT2Bins(2, mt2bins);}
    // else if (njets_lo == 4 && nbjets_lo == 3) {float mt2bins[3] = {200, 350, 1500}; fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_VL_vec.size(); iSR++){
    SR fullSR = temp_SR_VL_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "L";
    fullSR.SetName(fullSRname);
    fullSR.SetVar("ht", 450, 575);
    fullSR.SetVar("met", 200, -1);
    fullSR.SetVarCRSL("ht", 450, 575);
    fullSR.SetVarCRSL("met", 200, -1);
    fullSR.SetVarCRQCD("ht", 450, 575);
    fullSR.SetVarCRQCD("met", 200, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {300, 400, 500, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    // else if (njets_lo == 4 && nbjets_lo == 3) {float mt2bins[3] = {200, 350, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_MH_vec.size(); iSR++){
    SR fullSR = temp_SR_MH_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "M";
    fullSR.SetName(fullSRname);
    fullSR.SetVar("ht", 575, 1000);
    fullSR.SetVar("met", 200, -1);
    fullSR.SetVarCRSL("ht", 575, 1000);
    fullSR.SetVarCRSL("met", 200, -1);
    fullSR.SetVarCRQCD("ht", 575, 1000);
    fullSR.SetVarCRQCD("met", 200, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[4] = {200, 450, 550, 1500}; fullSR.SetMT2Bins(3, mt2bins);}
    else if (njets_lo == 4 && nbjets_lo == 3) {float mt2bins[3] = {200, 450, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    else if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[3] = {200, 350, 1500};      fullSR.SetMT2Bins(2, mt2bins);}
    SRVec.push_back(fullSR);
  }
  for(unsigned int iSR = 0; iSR < temp_SR_MH_vec.size(); iSR++){
    SR fullSR = temp_SR_MH_vec.at(iSR);
    std::string fullSRname = fullSR.GetName() + "H";
    fullSR.SetName(fullSRname);
    fullSR.SetVar("ht", 1000, -1);
    fullSR.SetVar("met", 30, -1);
    fullSR.SetVarCRSL("ht", 1000, -1);
    fullSR.SetVarCRSL("met", 30, -1);
    fullSR.SetVarCRQCD("ht", 1000, -1);
    fullSR.SetVarCRQCD("met", 30, -1);
    int njets_lo = fullSR.GetLowerBound("njets");
    int nbjets_lo = fullSR.GetLowerBound("nbjets");
    if      (njets_lo == 4 && nbjets_lo == 2) {float mt2bins[3] = {200, 450, 1500}; fullSR.SetMT2Bins(2, mt2bins);}
    else if (njets_lo == 4 && nbjets_lo == 3) {float mt2bins[2] = {200, 1500};      fullSR.SetMT2Bins(1, mt2bins);}
    else if (njets_lo == 7 && nbjets_lo == 2) {float mt2bins[3] = {200, 350, 1500}; fullSR.SetMT2Bins(2, mt2bins);}
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

} // namespace mt2
