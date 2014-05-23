{
  gSystem->AddIncludePath(" -I${ROOTPLOTTING}/db");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/db/plotPDFs.cc+");
}
