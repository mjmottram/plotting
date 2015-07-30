{
  gSystem->AddIncludePath(" -I${ROOTPLOTTING}");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/rootPlotUtil.cc+");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/db/plotPDFs.cc+");
}
