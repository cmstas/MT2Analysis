#include <stdexcept>
#include <iostream>
#include <string>
#include <cmath>

#include "SR.h"

SR::SR(){
  Clear();
}

SR::~SR(){
}

void SR::SetName(std::string sr_name){
  srName_ = sr_name;
}

void SR::SetVar(std::string var_name, float lower_bound, float upper_bound){
  bins_[var_name] = std::pair<float, float>(lower_bound, upper_bound);
}

void SR::SetVarCRSL(std::string var_name, float lower_bound, float upper_bound){
  binsCRSL_[var_name] = std::pair<float, float>(lower_bound, upper_bound);
}

void SR::SetVarCRRSInvertDPhi(std::string var_name, float lower_bound, float upper_bound){
  binsCRRSInvertDPhi_[var_name] = std::pair<float, float>(lower_bound, upper_bound);
}

void SR::SetVarCRRSMT2SideBand(std::string var_name, float lower_bound, float upper_bound){
  binsCRRSMT2SideBand_[var_name] = std::pair<float, float>(lower_bound, upper_bound);
}

void SR::SetVarCRRSDPhiMT2(std::string var_name, float lower_bound, float upper_bound){
  binsCRRSDPhiMT2_[var_name] = std::pair<float, float>(lower_bound, upper_bound);
}

void SR::SetMT2Bins(int nbins, float* bins){
  n_mt2bins_ = nbins;
  mt2bins_ = new float [n_mt2bins_+1];
  for(int i=0; i<=nbins; i++){
    mt2bins_[i] = bins[i];
  }
}

std::string SR::GetName(){
  return srName_;
}

float SR::GetLowerBound(std::string var_name){
  return (bins_.at(var_name)).first;
}

float SR::GetUpperBound(std::string var_name){
  return (bins_.at(var_name)).second;
}

unsigned int SR::GetNumberOfVariables(){
  return (bins_.size());
}

std::vector<std::string> SR::GetListOfVariables(){
  std::vector<std::string> vars;
  for(std::map<std::string, std::pair<float, float> >::const_iterator it = bins_.begin(); it != bins_.end(); ++it) {
    vars.push_back(it->first);
  }
  return (vars);
}

float SR::GetLowerBoundCRSL(std::string var_name){
  return (binsCRSL_.at(var_name)).first;
}

float SR::GetUpperBoundCRSL(std::string var_name){
  return (binsCRSL_.at(var_name)).second;
}

unsigned int SR::GetNumberOfVariablesCRSL(){
  return (binsCRSL_.size());
}

std::vector<std::string> SR::GetListOfVariablesCRSL(){
  std::vector<std::string> vars;
  for(std::map<std::string, std::pair<float, float> >::const_iterator it = binsCRSL_.begin(); it != binsCRSL_.end(); ++it) {
    vars.push_back(it->first);
  }
  return (vars);
}

float SR::GetLowerBoundCRRSInvertDPhi(std::string var_name){
  return (binsCRRSInvertDPhi_.at(var_name)).first;
}

float SR::GetUpperBoundCRRSInvertDPhi(std::string var_name){
  return (binsCRRSInvertDPhi_.at(var_name)).second;
}

unsigned int SR::GetNumberOfVariablesCRRSInvertDPhi(){
  return (binsCRRSInvertDPhi_.size());
}

std::vector<std::string> SR::GetListOfVariablesCRRSInvertDPhi(){
  std::vector<std::string> vars;
  for(std::map<std::string, std::pair<float, float> >::const_iterator it = binsCRRSInvertDPhi_.begin(); it != binsCRRSInvertDPhi_.end(); ++it) {
    vars.push_back(it->first);
  }
  return (vars);
}

float SR::GetLowerBoundCRRSMT2SideBand(std::string var_name){
  return (binsCRRSMT2SideBand_.at(var_name)).first;
}

float SR::GetUpperBoundCRRSMT2SideBand(std::string var_name){
  return (binsCRRSMT2SideBand_.at(var_name)).second;
}

unsigned int SR::GetNumberOfVariablesCRRSMT2SideBand(){
  return (binsCRRSMT2SideBand_.size());
}

std::vector<std::string> SR::GetListOfVariablesCRRSMT2SideBand(){
  std::vector<std::string> vars;
  for(std::map<std::string, std::pair<float, float> >::const_iterator it = binsCRRSMT2SideBand_.begin(); it != binsCRRSMT2SideBand_.end(); ++it) {
    vars.push_back(it->first);
  }
  return (vars);
}

float SR::GetLowerBoundCRRSDPhiMT2(std::string var_name){
  return (binsCRRSDPhiMT2_.at(var_name)).first;
}

float SR::GetUpperBoundCRRSDPhiMT2(std::string var_name){
  return (binsCRRSDPhiMT2_.at(var_name)).second;
}

unsigned int SR::GetNumberOfVariablesCRRSDPhiMT2(){
  return (binsCRRSDPhiMT2_.size());
}

std::vector<std::string> SR::GetListOfVariablesCRRSDPhiMT2(){
  std::vector<std::string> vars;
  for(std::map<std::string, std::pair<float, float> >::const_iterator it = binsCRRSDPhiMT2_.begin(); it != binsCRRSDPhiMT2_.end(); ++it) {
    vars.push_back(it->first);
  }
  return (vars);
}

float* SR::GetMT2Bins(){
  return mt2bins_;
}

int SR::GetNumberOfMT2Bins(){
  return n_mt2bins_;
}

bool SR::PassesSelection(std::map<std::string, float> values){
  float ep = 0.000001;
  if(GetNumberOfVariables() != values.size()){
    throw std::invalid_argument("Number of variables to cut on != number of variables in signal region");
  }
  for(std::map<std::string, float>::const_iterator it = values.begin(); it != values.end(); it++){
    if(bins_.find(it->first) != bins_.end()){ //check that we actually have bounds set for this variable
      float value = it->second;
      float cut_lower = GetLowerBound(it->first);
      float cut_upper = GetUpperBound(it->first);
      if(value < cut_lower) return false;
      if(( std::abs(cut_upper + 1.0) > ep ) && (value >= cut_upper)) return false;
    }
    else{
      throw std::invalid_argument("Cut values not set for this variable");
    }
  }
  return true;
}

bool SR::PassesSelectionCRSL(std::map<std::string, float> values){
  float ep = 0.000001;
  if(GetNumberOfVariablesCRSL() != values.size()){
    std::cout << "Number of variables to cut on != number of variables in CRSL region. Passed " << values.size() << ", expected " << GetNumberOfVariablesCRSL() << std::endl;
    throw std::invalid_argument("Number of variables to cut on != number of variables in CRSL region");
  }
  for(std::map<std::string, float>::const_iterator it = values.begin(); it != values.end(); it++){
    if(binsCRSL_.find(it->first) != binsCRSL_.end()){ //check that we actually have bounds set for this variable
      float value = it->second;
      float cut_lower = GetLowerBoundCRSL(it->first);
      float cut_upper = GetUpperBoundCRSL(it->first);
      if(value < cut_lower) return false;
      if(( std::abs(cut_upper + 1.0) > ep ) && (value >= cut_upper)) return false;
    }
    else{
      throw std::invalid_argument("Cut values not set for this variable");
    }
  }
  return true;
}

bool SR::PassesSelectionCRRSInvertDPhi(std::map<std::string, float> values){
  float ep = 0.000001;
  if(GetNumberOfVariablesCRRSInvertDPhi() != values.size()){
    std::cout << "Number of variables to cut on != number of variables in CRRSInvertDPhi region. Passed " << values.size() << ", expected " << GetNumberOfVariablesCRRSInvertDPhi() << std::endl;
    throw std::invalid_argument("Number of variables to cut on != number of variables in CRRSInvertDPhi region");
  }
  for(std::map<std::string, float>::const_iterator it = values.begin(); it != values.end(); it++){
    if(binsCRRSInvertDPhi_.find(it->first) != binsCRRSInvertDPhi_.end()){ //check that we actually have bounds set for this variable
      float value = it->second;
      float cut_lower = GetLowerBoundCRRSInvertDPhi(it->first);
      float cut_upper = GetUpperBoundCRRSInvertDPhi(it->first);
      if(value < cut_lower) return false;
      if(( std::abs(cut_upper + 1.0) > ep ) && (value >= cut_upper)) return false;
    }
    else{
      throw std::invalid_argument("Cut values not set for this variable");
    }
  }
  return true;
}

bool SR::PassesSelectionCRRSMT2SideBand(std::map<std::string, float> values){
  float ep = 0.000001;
  if(GetNumberOfVariablesCRRSMT2SideBand() != values.size()){
    std::cout << "Number of variables to cut on != number of variables in CRRSMT2SideBand region. Passed " << values.size() << ", expected " << GetNumberOfVariablesCRRSMT2SideBand() << std::endl;
    throw std::invalid_argument("Number of variables to cut on != number of variables in CRRSMT2SideBand region");
  }
  for(std::map<std::string, float>::const_iterator it = values.begin(); it != values.end(); it++){
    if(binsCRRSMT2SideBand_.find(it->first) != binsCRRSMT2SideBand_.end()){ //check that we actually have bounds set for this variable
      float value = it->second;
      float cut_lower = GetLowerBoundCRRSMT2SideBand(it->first);
      float cut_upper = GetUpperBoundCRRSMT2SideBand(it->first);
      if(value < cut_lower) return false;
      if(( std::abs(cut_upper + 1.0) > ep ) && (value >= cut_upper)) return false;
    }
    else{
      throw std::invalid_argument("Cut values not set for this variable");
    }
  }
  return true;
}

bool SR::PassesSelectionCRRSDPhiMT2(std::map<std::string, float> values){
  float ep = 0.000001;
  if(GetNumberOfVariablesCRRSDPhiMT2() != values.size()){
    std::cout << "Number of variables to cut on != number of variables in CRRSDPhiMT2 region. Passed " << values.size() << ", expected " << GetNumberOfVariablesCRRSDPhiMT2() << std::endl;
    throw std::invalid_argument("Number of variables to cut on != number of variables in CRRSDPhiMT2 region");
  }
  for(std::map<std::string, float>::const_iterator it = values.begin(); it != values.end(); it++){
    if(binsCRRSDPhiMT2_.find(it->first) != binsCRRSDPhiMT2_.end()){ //check that we actually have bounds set for this variable
      float value = it->second;
      float cut_lower = GetLowerBoundCRRSDPhiMT2(it->first);
      float cut_upper = GetUpperBoundCRRSDPhiMT2(it->first);
      if(value < cut_lower) return false;
      if(( std::abs(cut_upper + 1.0) > ep ) && (value >= cut_upper)) return false;
    }
    else{
      throw std::invalid_argument("Cut values not set for this variable");
    }
  }
  return true;
}

void SR::RemoveVar(std::string var_name){
  if(bins_.find(var_name) != bins_.end()) bins_.erase(var_name);
  else throw std::invalid_argument("Variable not added. Cannot remove!");
}

void SR::RemoveVarCRSL(std::string var_name){
  if(binsCRSL_.find(var_name) != binsCRSL_.end()) binsCRSL_.erase(var_name);
  else throw std::invalid_argument("Variable not added. Cannot remove!");
}

void SR::RemoveVarCRRSInvertDPhi(std::string var_name){
  if(binsCRRSInvertDPhi_.find(var_name) != binsCRRSInvertDPhi_.end()) binsCRRSInvertDPhi_.erase(var_name);
  else throw std::invalid_argument("Variable not added. Cannot remove!");
}

void SR::RemoveVarCRRSMT2SideBand(std::string var_name){
  if(binsCRRSMT2SideBand_.find(var_name) != binsCRRSMT2SideBand_.end()) binsCRRSMT2SideBand_.erase(var_name);
  else throw std::invalid_argument("Variable not added. Cannot remove!");
}

void SR::RemoveVarCRRSDPhiMT2(std::string var_name){
  if(binsCRRSDPhiMT2_.find(var_name) != binsCRRSDPhiMT2_.end()) binsCRRSDPhiMT2_.erase(var_name);
  else throw std::invalid_argument("Variable not added. Cannot remove!");
}

void SR::Clear(){
  srName_ = "";
  bins_.clear();
  binsCRSL_.clear();
  binsCRRSInvertDPhi_.clear();
  binsCRRSMT2SideBand_.clear();
  binsCRRSDPhiMT2_.clear();
}
