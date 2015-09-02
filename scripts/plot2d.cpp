#include "TFile.h"
#include "../../Software/dataMCplotMaker/PlotMaker2D.h"
#include "TH2F.h"
#include "TH1F.h"
#include <iostream>

void plot2d()
{
  TFile* f_ttbar = new TFile("/home/users/sicheng/tas/MT2Analysis/MT2looper/output/raw_elec5/ttall_mg_lo.root");
  //TFile* f_ttbar = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/wiso_mt2g200/T5qqqqWW_315_300.root");
  //TFile* f_zinv  = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/wiso_mt2g200/zinv_ht.root");
  //TFile* f_wjets = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/wiso_mt2g200/wjets_ht.root");
  //TFile* f_qcd   = new TFile("/home/users/sicheng/MT2Analysis/MT2looper/output/wiso_mt2g200/qcd.root");

  TH2F* h1 = (TH2F*) f_ttbar->Get("srsebase/h2d_nlep_frtau_mpt5-10");
  TH2F* h2 = (TH2F*) f_ttbar->Get("srsebase/h2d_nlep_frtau_mpt10-15");
  TH2F* h3 = (TH2F*) f_ttbar->Get("srsebase/h2d_nlep_frtau_mpt15-20");
  //Th2f* h4 = (TH2F*) f_ttbar->Get("srsmbase/h2d_receff_genmupt");

  //h->Draw();
  PlotMaker2D(h1, "--colors --outputName plots/ngenLep_frtau_mpt5-10");
  PlotMaker2D(h2, "--colors --outputName plots/ngenLep_frtau_mpt10-15");
  PlotMaker2D(h3, "--colors --outputName plots/ngenLep_frtau_mpt15-20");

  // PlotMaker2D(h4, "--colors --outputName h2d_receff_genmupt");

}
