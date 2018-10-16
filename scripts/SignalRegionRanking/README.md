First edit combineByTopoRegion.sh for point to your set of cards and the model (e.g. T1qqqq_1400_1000) you wish to run on. This will produce a set of cards merged across MT2 regions in scratch/ranking_YourModel. combine will then be run on these cards, with limits for each merged region output in that same directory to log_YourModel_ThatRegion.txt

Second, edit readLimitsRanking.sh to run on your desired Model. It will output a list of all the limits from the merged regions to Ranking_YourModel_withBG.txt.

Then, rankRegions.py will print a list of the regions ranked by expected sensitivity to Ranking_Model_ranked.txt, and will also print to stdout the fraction of signal falling into each region (with xsec * lumi as the denominator).
