root -b -q plotZinvYields.C
root -b -q compareYieldZinv.C
root -b -q 'plotZinvYieldsBinned.C("J")'
root -b -q 'plotZinvYieldsBinned.C("VL")'
root -b -q 'plotZinvYieldsBinned.C("L")'
root -b -q 'plotZinvYieldsBinned.C("M")'
root -b -q 'plotZinvYieldsBinned.C("H")'
root -b -q 'plotZinvYieldsBinned.C("UH")'
root -b -q 'compareYieldZinvBinned.C("J")'
root -b -q 'compareYieldZinvBinned.C("J", true)'
root -b -q 'compareYieldZinvBinned.C("VL")'
root -b -q 'compareYieldZinvBinned.C("L")'
root -b -q 'compareYieldZinvBinned.C("M")'
root -b -q 'compareYieldZinvBinned.C("M", true)'
root -b -q 'compareYieldZinvBinned.C("H")'
root -b -q 'compareYieldZinvBinned.C("UH")'
