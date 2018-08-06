#include "ShortTrackUtils.h"

using namespace std;

string HitSigStr (const int hit_signature, const int nLayers) {
  if (nLayers < 1) return "";
  ostringstream bits;
  bits << bitset<16>(hit_signature);
  string bs = bits.str();
  reverse(bs.begin(),bs.end());
  return bs.substr(0,nLayers);
}

// for testing
int main () {
  cout << HitSigStr(31, 3) << endl;
  cout << HitSigStr(31, 5) << endl;
  return 0;
}
