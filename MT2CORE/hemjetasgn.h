#include <vector>
#include "Math/LorentzVector.h"
#include "Math/Vector3D.h"

#include "../CORE/Tools/hemJet.h"
#include "../CORE/Tools/utils.h"

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > LorentzVector;

std::vector<int> getHemJetsAssignment(std::vector<LorentzVector>& jets);
bool AreJetsInSameHems(std::vector<LorentzVector>& jets, LorentzVector bjet1, LorentzVector bjet2);
