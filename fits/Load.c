{
  gSystem->AddIncludePath(" -I${ROOTPLOTTING}");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/rootPlotUtil.cc+");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/fits/plotDirectionError.cc+");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/fits/plotDrive.cc+");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/fits/plotPull.cc+");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/fits/plotEnergyError.cc+");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/fits/plotPositionError.cc+");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/fits/plotPositionUncertainties.cc+");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/fits/plotRadiusError.cc+");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/fits/plotTimeError.cc+");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/fits/plotEnergyError.cc+");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/fits/plotTimeUncertanties.cc+");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/fits/reportMisreconPosition.cc+");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/fits/summariseError.cc+"); 
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/fits/checkValidity.cc+");
  gROOT->ProcessLine(".L ${ROOTPLOTTING}/fits/plotPosition.cc+");
}
