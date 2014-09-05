
#ifndef __rootPlotUtil__
#define __rootPlotUtil__

#include <TH1F.h>
#include <TVirtualPad.h>
#include <TCanvas.h>

#include <string>
#include <vector>

void ArrangeStatBox(TH1F* hHistogram, TCanvas* can);
void ArrangeStatBox(TH1F* hHistogram, Int_t color, TVirtualPad* pad, std::string label="", double x1=-999, double x2=-999 );
TCanvas* CreateCan(std::string name, bool clear=false);
TH1F* CreateHist(std::string name, std::string title, int n, double x1, double x2);
Int_t GetColor(int i);
void Normalise(TH1F* hist);
void Normalise(TH1D* hist);
void PrintCanvases(std::vector<TCanvas*> canvases, std::string prefix, std::vector<std::string> names);
void ScaleGraphs(TCanvas* can, bool logScale=false, double min=-1e10);
void ScaleHists(TCanvas* can, bool logScale=false);

extern bool gPlottingLogX;
extern bool gPlottingLogY;
extern bool gPlottingLogZ;
extern bool gStatOverflows;

extern bool kIncludeInvalidFits;

#endif
