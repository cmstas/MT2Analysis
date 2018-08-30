void skim_qcd(const char* indir, const char* outdir, const char* sample){

    // long long max_tree_size = 5000000000LL;
    // TTree::SetMaxTreeSize(max_tree_size);
    
    TChain *c = new TChain("mt2");
    c->Add(Form("%s/%s*.root",indir,sample));
    
    TFile fout(Form("%s/%s.root",outdir,sample), "RECREATE");
    // TTree *t = c->CopyTree("mt2>50 && ht>250 && nlep==0 && diffMetMht/met_pt<0.5");
    TTree *t = c->CopyTree("nlep==0 && ht>250 && ((nJet30>=2 && mt2>50) || (nJet30>=1 && nJet30<=2 && met_pt>250))");

    t->Write();
    fout.Close();

}
