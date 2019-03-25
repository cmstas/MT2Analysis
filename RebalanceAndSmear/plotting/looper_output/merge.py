import sys
import os
from sample_weights import get_weight

def merge (indir, test=False):  
  
  list_of_ext_hists = {}
  list_of_non_ext_hists = {}
  list_of_ext_babies = {}
  list_of_non_ext_babies = {}
  
  name_to_sample_id = {'300': '153', '500': '154', '700': '155', '1000': '156', '1500': '157', '2000': '158'}
  
  for dirname in os.listdir(indir):
    if "ext" in dirname:        
      list_of_ext_hists[dirname.split("_ext")[0]] = (os.path.join("{0}/{1}".format(indir,dirname),"*.root"))
      list_of_ext_babies[dirname.split("_ext")[0]] = (os.path.join("{0}/{1}".format(indir,dirname),"smearbaby/*.root"))        
    else:
      list_of_non_ext_hists[dirname] = (os.path.join("{0}/{1}".format(indir,dirname),"*.root"))
      list_of_non_ext_babies[dirname] = (os.path.join("{0}/{1}".format(indir,dirname),"smearbaby/*.root"))        
      
  # if "qcd" in indir:
  #     print "hadd " + os.path.join(indir,"merged_baby_ext.root") + " " + " ".join(list_of_ext_babies)
  #     print "hadd " + os.path.join(indir,"merged_baby_non_ext.root") + " " + " ".join(list_of_non_ext_babies)
  #     if not test:
  #         os.system("hadd " + os.path.join(indir,"merged_baby_ext.root") + " " + " ".join(list_of_ext_babies))
  #         os.system("hadd " + os.path.join(indir,"merged_baby_non_ext.root") + " " + " ".join(list_of_non_ext_babies))    
  
  # elif "data" in indir:
  #     print "hadd " + os.path.join(indir,"merged_baby.root") + " " + " ".join(list_of_non_ext_babies)
  #     if not test:
  #         os.system("hadd " + os.path.join(indir,"merged_baby.root") + " " + " ".join(list_of_non_ext_babies))    
      
  ##
  ## first we combine the ext and non_ext for each sample
  ##
  list_of_merged_subsamples = []
  if "qcd" in indir:
    for dirname in list_of_non_ext_hists:
      w_ext = 1
      w_non_ext = 1
      if dirname in list_of_ext_hists:
        for sub in dirname.split("_"):
          if "ht" not in sub: continue
          sample_id = name_to_sample_id[sub.strip("ht").split("to")[0]]
          w_ext = get_weight(sample_id, True)
          w_non_ext = get_weight(sample_id, False)
          w_ext_sf = w_non_ext/(w_ext + w_non_ext)
          w_non_ext_sf = w_ext/(w_ext + w_non_ext)        
          outname = "merged_hists_qcd_{0}.root".format(sub.split("/")[0])
          print "haddws {0} {1} {2} {3}".format(list_of_non_ext_hists[dirname], list_of_ext_hists[dirname], w_non_ext_sf, w_ext_sf)
          if not test:
            os.system("haddws {0} {1} {2} {3}".format(list_of_non_ext_hists[dirname], list_of_ext_hists[dirname], w_non_ext_sf, w_ext_sf))
            os.system("mv result.root " + os.path.join(indir, outname))
            list_of_merged_subsamples.append(os.path.join(indir,outname))
      else:
        print "hadd " + os.path.join(indir,"merged_hists_{0}.root".format(dirname)) + " " + " ".join(list_of_non_ext_hists.values())
        if not test:
          os.system("hadd " + os.path.join(indir,"merged_hists_{0}.root".format(dirname)) + " " + " ".join(list_of_non_ext_hists.values()))    
        list_of_merged_subsamples.append(os.path.join(indir, "merged_hists_{0}.root".format(dirname)))

    print "hadd " + os.path.join(indir,"merged_hists.root") + " " + " ".join(list_of_merged_subsamples)
    if not test:
      os.system("hadd " + os.path.join(indir,"merged_hists.root") + " " + " ".join(list_of_merged_subsamples))

  else:
    print "hadd " + os.path.join(indir,"merged_hists.root") + " " + " ".join(list_of_non_ext_hists.values())
    if not test:
      os.system("hadd " + os.path.join(indir,"merged_hists.root") + " " + " ".join(list_of_non_ext_hists.values()))    
                
if __name__ == '__main__':
  test = sys.argv[2] if len(sys.argv) > 2 else False
  merge(sys.argv[1], test)
