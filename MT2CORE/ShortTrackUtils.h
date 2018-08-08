#ifndef SHORTTRACKUTILS_H
#define SHORTTRACKUTILS_H

#include <bitset>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>

// Converts track hit signatures into a human-readable binary string for the first nLayers
std::string HitSigStr (const int hit_signature, const int nLayers = 16);

#endif
