import subprocess
import glob

indirs = glob.glob("/hadoop/cms/store/user/bemarsh/mt2babies")
# indirs = glob.glob("/hadoop/cms/store/group/snt/run2_data2017/*")

badfiles = []
for indir in indirs:
    # print "Checking dir",indir
    p = subprocess.Popen("hdfs fsck {0}".format(indir.replace("/hadoop","")).split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    lines = out.split("\n")

    for line in lines:
        if "CORRUPT blockpool" in line:
            file = line.split(":")[0]
            badfiles.append("/hadoop"+file)
            print "    BAD FILE:",badfiles[-1]
            # subprocess.call("hdfs dfs -rm "+file, shell=True)

