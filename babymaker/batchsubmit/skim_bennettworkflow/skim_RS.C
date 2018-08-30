void skim_RS(const char* indir, const char* outdir, const char* sample){

    // long long max_tree_size = 5000000000LL;
    // TTree::SetMaxTreeSize(max_tree_size);
    
    TChain *c = new TChain("mt2");
    c->Add(Form("%s/%s*.root",indir,sample));
    
    TFile fout(Form("%s/%s.root",outdir,sample), "RECREATE");
    TTree *t = c->CopyTree("(ht > 250. && met_pt>30 && ((nJet30 >= 2 && mt2 > 100.) || (nJet30>=1 && nJet30<=2 && met_pt>250))) || (zll_ht > 200. && nJet30 >= 2 && zll_mt2 > 100. && zll_met_pt > 30.)");

    t->Write();
    fout.Close();

}
