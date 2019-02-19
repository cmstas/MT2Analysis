#!/bin/bash

tag=$1
destination=$2

echo "copying files with tag ${tag} to destination ${destination}"

mkdir -p $destination

declare -a main=(pt nlayer iso)
declare -a extras=(dz dxy reliso niso nreliso mt mtmt2 mtmet mt2met met ht mt2)

declare -a distribution_dirs=(data_2016_${tag} data_2017_${tag} data_2018_${tag} data_2017and2018_${tag} mc_2016_${tag} mc_2017_${tag} mc_2018_${tag} mc_2017and2018_${tag})
for d in ${distrinbution_dirs[@]}; do
    shortname=${d%_${tag}}

    extradir=$destination/extras/${shortname}
    mkdir -p $extradir
    sourcedir=../FshortLooper/distributions/$d/
    pushd $sourcedir
    for extra in ${extra[@]}; do
	cp -r *_$extra_* $extradir
    done
    popd

    extradir=$destination/extras_nm1/${shortname}
    mkdir -p $extradir
    sourcedir=../FshortLooper/distributionsNM1/$d/
    pushd $sourcedir
    for extra in ${extra[@]}; do
	cp -r *_$extra_* $extradir
    done
    popd

    maindir=$destination/main/${shortname}
    mkdir -p $maindir
    sourcedir=../FshortLooper/distributions/$d/
    pushd $sourcedir
    for main in ${main[@]}; do
	cp -r *_$main_* $maindir
    done
    popd

    maindir=$destination/main_nm1/${shortname}
    mkdir -p $maindir
    sourcedir=../FshortLooper/distributionsNM1/$d/
    pushd $sourcedir
    for main in ${main[@]}; do
	cp -r *_$main_* $maindir
    done
    popd
done

cp -r ../FshortLooper/fshort_variations $destination

cp -r ../ShortTrack/contam_plots $destination

cp -r ../ShortTrack/region_plots/${tag} $destination/region_plots

cp -r ../ShortTrack/region_tables/${tag} $destination/region_tables

cp -r limit_tables/${tag} $destination/limit_tables

cp -r /nfs-6/userdata/dpgilber/ShortTrackEtaPhiSignalEff/ $destination/EtaPhiSignalEff

cp -r /nfs-6/userdata/dpgilber/EtaphiVetoMaps $destination

cp -r /nfs-6/userdata/dpgilber/ShortTrackTagAndProbe $destination/EtaPhiTagAndProbe

cp -r /nfs-6/userdata/dpgilber/CompareT1qqqq $destination

cp -r ../FshortLooper/sigeff_plots $destination

cp -r ../FshortLooper/pdfs_SvsBG $destination/SvsBG_plots

chmod -R 775 $destination
