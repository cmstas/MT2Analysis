// This file is modified from CORE/Tools/hemJet.cc
#include "hemjetasgn.h"

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > LorentzVector;

using namespace std;


std::vector<int> getHemJetsAssignment(std::vector<LorentzVector>& jets){

  if(jets.size() < 2) throw std::runtime_error("less than 2 jets used for hemisphere reconstruction");

  LorentzVector axis1;
  LorentzVector axis2;
  LorentzVector tempAxis1;
  LorentzVector tempAxis2;
  unsigned int axis1Index = 0;
  unsigned int axis2Index = 0;

  std::vector<int> hemAssoc (jets.size(), 0);

  //seed the axes with the pair of jets that form the highest invariant mass
  float maxMass = -1.0;
  for(unsigned int i=0; i<jets.size(); i++) {
    for(unsigned int j=i+1; j<jets.size(); j++) {

      if( (jets.at(i) + jets.at(j)).M() > maxMass ){
        maxMass = (jets.at(i) + jets.at(j)).M();
        axis1Index = i;
        axis2Index = j;
      }

    }
  }

  //set initial axes
  axis1 = jets.at(axis1Index);
  axis2 = jets.at(axis2Index);

  // debug
  // cout << "axis1Index = " << axis1Index << endl;
  // cout << "axis2Index = " << axis2Index << endl;

  //store hemisphere associations for jets that serve as axis seeds
  hemAssoc.at(axis1Index) = 1;
  hemAssoc.at(axis2Index) = 2;

  int counter = 0;
  bool move = true;

  while (move && counter<100) {

    move = false;
    ++counter;
    // if (counter==200) cout << "hemisphere reconstruction not converging" << endl;

    tempAxis1 = LorentzVector(0,0,0,0);
    tempAxis2 = LorentzVector(0,0,0,0);

    //assign each jet to a hemisphere, max of one can be moved per iteration
    for(unsigned int jet=0; jet<jets.size(); jet++){
      if (jet==axis1Index) {
        tempAxis1 += jets.at(jet);
      }
      else if (jet==axis2Index) {
        tempAxis2 += jets.at(jet);
      }
      else {
        if(!move) {
          LorentzVector newAxis1 = hemAssoc.at(jet)==1 ? axis1 - jets.at(jet) : axis1;
          LorentzVector newAxis2 = hemAssoc.at(jet)==2 ? axis2 - jets.at(jet) : axis2;
          if (LundDist(newAxis1, jets.at(jet)) <= LundDist(newAxis2, jets.at(jet))) {
            if (hemAssoc.at(jet) != 1 && hemAssoc.at(jet) != 0) move = true;
            tempAxis1 += jets.at(jet);
            hemAssoc.at(jet) = 1;
          }
          else {
            if (hemAssoc.at(jet) != 2 && hemAssoc.at(jet) != 0) move = true;
            tempAxis2 += jets.at(jet);
            hemAssoc.at(jet) = 2;
          }
        }
        else {
          if (hemAssoc.at(jet)==1) tempAxis1 += jets.at(jet);
          else if (hemAssoc.at(jet)==2) tempAxis2 += jets.at(jet);
        }
      }
    }

    //new axes for each hemisphere composed of the momenta of the jets in that hemisphere
    axis1 = tempAxis1;
    axis2 = tempAxis2;

    if (counter <= 1) move = true;
  }

  // std::vector<LorentzVector> result;

  return hemAssoc;

}

bool AreJetsInSameHems(std::vector<LorentzVector>& jets, LorentzVector bjet1, LorentzVector bjet2){

  int asgn1 = -1;
  int asgn2 = -1;
  std::vector<int> hemAssoc = getHemJetsAssignment(jets);
  for(unsigned int i=0; i < jets.size(); i++){
    if(DeltaR(jets.at(i).eta(), bjet1.eta(), jets.at(i).phi(), bjet1.phi()) < 0.01) asgn1 = i;
    else if(DeltaR(jets.at(i).eta(), bjet2.eta(), jets.at(i).phi(), bjet2.phi()) < 0.01) asgn2 = i;
  }
  if(asgn1 < 0 || asgn2 < 0) std::cout << "Error: bjet not found in jets vector!" << std::endl;
  
  return (asgn1 == asgn2);
}
