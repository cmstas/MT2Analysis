#!/bin/bash

INDIR=cards_T2-4bd/
SAMPLE=T2-4bd_275_235


OUTDIR=${INDIR}/ordered/
mkdir -p ${OUTDIR}

#reorder datacards to correspond to following binning, plus mt bins in ascending order:
  # regions.push_back("srLep1L");
  # regions.push_back("srLep2L");
  # regions.push_back("srLep3L");
  # regions.push_back("srLep4L");
  # regions.push_back("srLep5L");
  # regions.push_back("srLep6L");
  # regions.push_back("srLep7L");

  # regions.push_back("srLep1M");
  # regions.push_back("srLep2M");
  # regions.push_back("srLep3M");
  # regions.push_back("srLep4M");
  # regions.push_back("srLep5M");
  # regions.push_back("srLep6M");
  # regions.push_back("srLep7M");

  # regions.push_back("srLepJ1L");
  # regions.push_back("srLepJ1M");
  # regions.push_back("srLepJ2L");
  # regions.push_back("srLepJ3L");

  # regions.push_back("srLepB3");
  # regions.push_back("srLepMET");
  # regions.push_back("srLepHT");

#low met regions
cp ${INDIR}datacard_HT200to1000_MET200to300_j2to3_b0_Hb0_m20to90_${SAMPLE}.txt             ${OUTDIR}datacard0001.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j2to3_b0_Hb0_m90to120_${SAMPLE}.txt            ${OUTDIR}datacard0002.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j2to3_b0_Hb0_m120toInf_${SAMPLE}.txt           ${OUTDIR}datacard0003.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j2to3_b1to2_Hb0_m20to90_${SAMPLE}.txt          ${OUTDIR}datacard0004.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j2to3_b1to2_Hb0_m90to120_${SAMPLE}.txt         ${OUTDIR}datacard0005.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j2to3_b1to2_Hb0_m120toInf_${SAMPLE}.txt        ${OUTDIR}datacard0006.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j2to3_b1to2_Hb1to2_m20to90_${SAMPLE}.txt       ${OUTDIR}datacard0007.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j2to3_b1to2_Hb1to2_m90to120_${SAMPLE}.txt      ${OUTDIR}datacard0008.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j2to3_b1to2_Hb1to2_m120toInf_${SAMPLE}.txt     ${OUTDIR}datacard0009.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j4to5_b0_Hb0_m20to90_${SAMPLE}.txt             ${OUTDIR}datacard0010.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j4to5_b0_Hb0_m90to120_${SAMPLE}.txt            ${OUTDIR}datacard0011.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j4to5_b0_Hb0_m120toInf_${SAMPLE}.txt           ${OUTDIR}datacard0012.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j4toInf_b1to2_Hb0_m20to90_${SAMPLE}.txt        ${OUTDIR}datacard0013.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j4toInf_b1to2_Hb0_m90to120_${SAMPLE}.txt       ${OUTDIR}datacard0014.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j4toInf_b1to2_Hb0_m120toInf_${SAMPLE}.txt      ${OUTDIR}datacard0015.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j4toInf_b1to2_Hb1to2_m20to90_${SAMPLE}.txt     ${OUTDIR}datacard0016.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j4toInf_b1to2_Hb1to2_m90to120_${SAMPLE}.txt    ${OUTDIR}datacard0017.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j4toInf_b1to2_Hb1to2_m120toInf_${SAMPLE}.txt   ${OUTDIR}datacard0018.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j6toInf_b0_Hb0_m20to90_${SAMPLE}.txt           ${OUTDIR}datacard0019.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j6toInf_b0_Hb0_m90to120_${SAMPLE}.txt          ${OUTDIR}datacard0020.txt
cp ${INDIR}datacard_HT200to1000_MET200to300_j6toInf_b0_Hb0_m120toInf_${SAMPLE}.txt         ${OUTDIR}datacard0021.txt

#high met regions
cp ${INDIR}datacard_HT200to1000_MET300to500_j2to3_b0_Hb0_m20to90_${SAMPLE}.txt             ${OUTDIR}datacard0022.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j2to3_b0_Hb0_m90to120_${SAMPLE}.txt            ${OUTDIR}datacard0023.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j2to3_b0_Hb0_m120toInf_${SAMPLE}.txt           ${OUTDIR}datacard0024.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j2to3_b1to2_Hb0_m20to90_${SAMPLE}.txt          ${OUTDIR}datacard0025.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j2to3_b1to2_Hb0_m90to120_${SAMPLE}.txt         ${OUTDIR}datacard0026.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j2to3_b1to2_Hb0_m120toInf_${SAMPLE}.txt        ${OUTDIR}datacard0027.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j2to3_b1to2_Hb1to2_m20to90_${SAMPLE}.txt       ${OUTDIR}datacard0028.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j2to3_b1to2_Hb1to2_m90to120_${SAMPLE}.txt      ${OUTDIR}datacard0029.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j2to3_b1to2_Hb1to2_m120toInf_${SAMPLE}.txt     ${OUTDIR}datacard0030.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j4to5_b0_Hb0_m20to90_${SAMPLE}.txt             ${OUTDIR}datacard0031.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j4to5_b0_Hb0_m90to120_${SAMPLE}.txt            ${OUTDIR}datacard0032.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j4to5_b0_Hb0_m120toInf_${SAMPLE}.txt           ${OUTDIR}datacard0033.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j4toInf_b1to2_Hb0_m20to90_${SAMPLE}.txt        ${OUTDIR}datacard0034.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j4toInf_b1to2_Hb0_m90to120_${SAMPLE}.txt       ${OUTDIR}datacard0035.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j4toInf_b1to2_Hb0_m120toInf_${SAMPLE}.txt      ${OUTDIR}datacard0036.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j4toInf_b1to2_Hb1to2_m20to90_${SAMPLE}.txt     ${OUTDIR}datacard0037.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j4toInf_b1to2_Hb1to2_m90to120_${SAMPLE}.txt    ${OUTDIR}datacard0038.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j4toInf_b1to2_Hb1to2_m120toInf_${SAMPLE}.txt   ${OUTDIR}datacard0039.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j6toInf_b0_Hb0_m20to90_${SAMPLE}.txt           ${OUTDIR}datacard0040.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j6toInf_b0_Hb0_m90to120_${SAMPLE}.txt          ${OUTDIR}datacard0041.txt
cp ${INDIR}datacard_HT200to1000_MET300to500_j6toInf_b0_Hb0_m120toInf_${SAMPLE}.txt         ${OUTDIR}datacard0042.txt

#monojet regions
cp ${INDIR}datacard_HT200to300_MET200toInf_j1_b0_Hb0_m20to90_${SAMPLE}.txt                 ${OUTDIR}datacard0043.txt
cp ${INDIR}datacard_HT200to300_MET200toInf_j1_b0_Hb0_m90to120_${SAMPLE}.txt                ${OUTDIR}datacard0044.txt
cp ${INDIR}datacard_HT200to300_MET200toInf_j1_b0_Hb0_m120toInf_${SAMPLE}.txt               ${OUTDIR}datacard0045.txt
cp ${INDIR}datacard_HT300toInf_MET200toInf_j1_b0_Hb0_m20to90_${SAMPLE}.txt                 ${OUTDIR}datacard0046.txt
cp ${INDIR}datacard_HT300toInf_MET200toInf_j1_b0_Hb0_m90to120_${SAMPLE}.txt                ${OUTDIR}datacard0047.txt
cp ${INDIR}datacard_HT300toInf_MET200toInf_j1_b0_Hb0_m120toInf_${SAMPLE}.txt               ${OUTDIR}datacard0048.txt
cp ${INDIR}datacard_HT200toInf_MET200toInf_j1_b1toInf_Hb0_m20to90_${SAMPLE}.txt            ${OUTDIR}datacard0049.txt
cp ${INDIR}datacard_HT200toInf_MET200toInf_j1_b1toInf_Hb0_m90to120_${SAMPLE}.txt           ${OUTDIR}datacard0050.txt
cp ${INDIR}datacard_HT200toInf_MET200toInf_j1_b1toInf_Hb0_m120toInf_${SAMPLE}.txt          ${OUTDIR}datacard0051.txt
cp ${INDIR}datacard_HT200toInf_MET200toInf_j1_b1toInf_Hb1toInf_m20to90_${SAMPLE}.txt       ${OUTDIR}datacard0052.txt
cp ${INDIR}datacard_HT200toInf_MET200toInf_j1_b1toInf_Hb1toInf_m90to120_${SAMPLE}.txt      ${OUTDIR}datacard0053.txt
cp ${INDIR}datacard_HT200toInf_MET200toInf_j1_b1toInf_Hb1toInf_m120toInf_${SAMPLE}.txt     ${OUTDIR}datacard0054.txt

#tail regions
cp ${INDIR}datacard_HT200toInf_MET200toInf_j2toInf_b3toInf_Hb0toInf_m20to90_${SAMPLE}.txt   ${OUTDIR}datacard0055.txt
cp ${INDIR}datacard_HT200toInf_MET200toInf_j2toInf_b3toInf_Hb0toInf_m90to120_${SAMPLE}.txt  ${OUTDIR}datacard0056.txt
cp ${INDIR}datacard_HT200toInf_MET200toInf_j2toInf_b3toInf_Hb0toInf_m120toInf_${SAMPLE}.txt ${OUTDIR}datacard0057.txt
cp ${INDIR}datacard_HT200toInf_MET500toInf_j2toInf_b0to2_Hb0to2_m20to90_${SAMPLE}.txt       ${OUTDIR}datacard0058.txt
cp ${INDIR}datacard_HT200toInf_MET500toInf_j2toInf_b0to2_Hb0to2_m90to120_${SAMPLE}.txt      ${OUTDIR}datacard0059.txt
cp ${INDIR}datacard_HT200toInf_MET500toInf_j2toInf_b0to2_Hb0to2_m120toInf_${SAMPLE}.txt     ${OUTDIR}datacard0060.txt
cp ${INDIR}datacard_HT1000toInf_MET200to500_j2toInf_b0to2_Hb0to2_m20to90_${SAMPLE}.txt      ${OUTDIR}datacard0061.txt
cp ${INDIR}datacard_HT1000toInf_MET200to500_j2toInf_b0to2_Hb0to2_m90to120_${SAMPLE}.txt     ${OUTDIR}datacard0062.txt
cp ${INDIR}datacard_HT1000toInf_MET200to500_j2toInf_b0to2_Hb0to2_m120toInf_${SAMPLE}.txt    ${OUTDIR}datacard0063.txt
