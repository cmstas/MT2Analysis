#ifndef QCDLOOPER_h
#define QCDLOOPER_h

// C++ includes
/* #include <string> */
#include <vector>

// ROOT includes
#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "Math/LorentzVector.h"

//MT2
#include "../MT2CORE/mt2tree.h"
#include "../MT2CORE/sigSelections.h"
#include "../MT2CORE/SR.h"
#include "../MT2CORE/configurations.h"

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > LorentzVector;
using namespace mt2;
class QCDLooper {

public:

    QCDLooper();
    ~QCDLooper();

    void SetSignalRegions();
    void loop(TChain* chain, std::string config_tag, std::string output_name = "test.root");
    void fillHistosRphi(std::map<std::string, TH1*>& h_1d, const std::string& dirname, const std::string& s = "");
    void fillHistosFj(std::map<std::string, TH1*>& h_1d, const std::string& dirname, const std::string& s = "");
    void fillHistosRb(std::map<std::string, TH1*>& h_1d, const std::string& dirname, const std::string& s = "");
    double getTriggerPrescale(std::string dirname="");

private:

    TFile * outfile_;
    MT2Configuration config_;
    mt2tree t;
    float evtweight_;
    float prescale_;
    std::vector<SR> SRVec_rphi;
    std::vector<SR> SRVec_fj;
    std::vector<SR> SRVec_rb;
    std::map<std::string, TH1*> h_1d_global_;

};

#endif

