#ifndef SIGNALREGIONSLEP_H
#define SIGNALREGIONSLEP_H

#include <string>
#include "TTree.h"
#include "SR.h"

namespace mt2
{

  std::vector<SR> getSignalRegionsLep();
  std::vector<SR> getSignalRegionsLep2();

} // namespace mt2

#endif // SIGNALREGIONSLEP_H
