import time
import os
import sys

import mt2datasets as mt2

from metis.Sample import SNTSample
from metis.LocalMergeTask import LocalMergeTask
from metis.CondorTask import CondorTask
from metis.StatsParser import StatsParser

if __name__ == "__main__":

    # samples you want to process
    sample_map = {}
    sample_map.update(mt2.getMC2017())
    sample_map.update(mt2.getData2017())

    # samples you want to merge directly onto nfs; typically, data is too large to do this before skimming
    merge_map = {}
    merge_map.update(mt2.getMC2017())

    mt2home = os.path.dirname(os.path.realpath(__file__))

    # submission tag
    tag = "V00-10-05_2016fullYear_17July2018"
    merged_dir = "/nfs-6/userdata/mt2/{}/".format(tag)
    tasks = {}
    merge_tasks = {}
    for dsname,shortname in sample_map.items():
        task = CondorTask(
            sample = SNTSample(
                dataset=dsname,
                # tag="CMS4_V09-04-13", # uncomment this and set it to the desired tag, otherwise Metis will run on the most recent version
                ),
            files_per_output = 1,
            output_name = "output.root",
            tag = tag,
            condor_submit_params = {"use_xrootd":True},
            cmssw_version = "CMSSW_9_4_9",
            input_executable = mt2home+"/metis_executable.sh", # your condor executable here
            tarfile = mt2home+"/job_input/input.tar.xz", # your tarfile with assorted goodies here
            special_dir = "mt2babies/{}".format(tag), # output files into /hadoop/cms/store/<user>/<special_dir>
            )
        tasks[dsname] = task
    # When babymaking task finishes, fire off a task that takes outputs and merges them locally (hadd)
    # into a file that ends up on nfs (specified by `merged_dir` above)
    for dsname,shortname in merge_map.items():
        merge_task = LocalMergeTask(
            input_filenames=task.get_outputs(),
            output_filename="{}/{}.root".format(merged_dir,shortname)
            )
        merge_tasks[dsname] = merge_task
    first_iteration = True
    done = False    
    while not done:
#        summary_info = {}
        done = True # Assume all tasks are complete until we see otherwise
        for dsname,shortname in sample_map.items():
            task = tasks[dsname]
            # Straightforward logic
            if not task.complete():
                task.process()
                done = False
        for dsname,shortname in sample_map_merge.items():
            task = tasks[dsname]
            merge_task = merge_tasks[dsname]            
            if task.complete() and not merge_task.complete():
                merge_task.process()
                done = False
#            summary_info[shortname+"_babymaking"] = task.get_task_summary()
#        StatsParser(data=summary_info, webdir="~/public_html/dump/metis/", make_plots=False).do()
        # sleep for 10 min unless it's the very first run of the loop. In that case sleep for 1 min, to permit a quick check that things are working
        if first_iteration:
            time.sleep(60)
            first_iteration = False
        else:
            time.sleep(1.*600)
