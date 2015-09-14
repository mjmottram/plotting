#include <rootPlotUtil.hh>

#include <TROOT.h>
#include <TH1F.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TVirtualPad.h>
#include <TPaveStats.h>
#include <TString.h>
#include <TLatex.h>

#include <RAT/DU/DSReader.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/MCParticle.hh>
#include <RAT/DS/FitResult.hh>
#include <RAT/DS/FitVertex.hh>

#include <iostream>
#include <sstream>
#include <iomanip> 

extern int kPullBins = 100;
extern double kPullLow = -1000;
extern double kPullHigh = 1000;

void printPullCanvases(string filePrefix);
void plotPull(string fileName, string fitName, string plotName="", bool clear=true);
void plotPull(string fileName, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotPull(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotPull(RAT::DU::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear=true);

void printPullCanvases(string filePrefix)
{

  vector<TCanvas*> canvases;
  vector<string> names;
  canvases.push_back((TCanvas*)gROOT->FindObject( "canPull" ));
  canvases.push_back((TCanvas*)gROOT->FindObject( "canPullFrac" ));
  names.push_back( "Pull" );
  names.push_back( "PullFrac" );
  PrintCanvases(canvases, filePrefix, names);

}


void plotPull(string fileName, string fitName, string plotName, bool clear)
{
  RAT::DU::DSReader dsReader(fileName.c_str());
  vector<string> fitNames;
  vector<string> plotNames;
  fitNames.push_back(fitName);
  if(plotName!="")
    plotNames.push_back(plotName);
  plotPull(dsReader, fitNames, plotNames, clear);
}


void plotPull(string fileName, vector<string> fitNames, const vector<string> plotNames, bool clear)
{
  RAT::DU::DSReader dsReader(fileName.c_str());
  plotPull(dsReader, fitNames, plotNames, clear);
}


void plotPull(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames, bool clear)
{
  RAT::DU::DSReader dsReader(fileNames[0].c_str());
  for(unsigned int i=1;i<fileNames.size();i++)
      dsReader.Add(fileNames[i].c_str());
  plotPull(dsReader, fitNames, plotNames, clear);
}


// Plot the pull in the direction of the radial -> event vector

void plotPull(RAT::DU::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear)
{

  vector<TH1F*> histsPull;
  vector<TH1F*> histsPullFrac;

  TCanvas* canPull = CreateCan("canPull", clear);
  TCanvas* canPullFrac = CreateCan("canPullFrac", clear);

  string pullTitle = " (#vec{r}_{Fit} - #vec{r}_{MC}).#hat{r}";
  string pullFracTitle = " (#vec{r}_{Fit} - #vec{r}_{MC}).#hat{r} / #cbar #vec{r}_{Fit} - #vec{r}_{MC} #cbar";

  if(plotNames.size()==0)
    plotNames = fitNames;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      // first, check and delete any current hists
      histsPull.push_back( CreateHist( "hPull"+plotNames[i], ";"+pullTitle, kPullBins, kPullLow, kPullHigh) );
      histsPullFrac.push_back( CreateHist( "hPullFrac"+plotNames[i], ";"+pullFracTitle, 200, -1, 1) );
    }

  cout << "Total entries: " << dsReader.GetEntryCount() << endl;

  for(size_t i=0; i<dsReader.GetEntryCount(); i++)
    {

      if(dsReader.GetEntryCount() > 100)
        if(i % (dsReader.GetEntryCount() / 20) == 0)
          cerr << "*";

      const RAT::DS::Entry& rds = dsReader.GetEntry(i);

      if(rds.GetEVCount()==0)
        continue;

      const RAT::DS::EV& ev = rds.GetEV( 0 );
      const RAT::DS::MC& mc = rds.GetMC();
      const RAT::DS::MCParticle& mcp = mc.GetMCParticle( 0 );
      TVector3 mcPosition = mcp.GetPosition();

      // Now get the different fit positions and compare
      
      for(unsigned int j=0; j<fitNames.size(); j++)
        {

          const RAT::DS::FitVertex& fitVertex = ev.GetFitResult(fitNames[j]).GetVertex(0);

          // For some fits may also need to consider whether the positions seed was valid
          if( fitVertex.ContainsPosition() && (fitVertex.ValidPosition() || kIncludeInvalidFits ))
            {
              TVector3 fitPosition = fitVertex.GetPosition();
              TVector3 positionError = fitPosition - mcPosition;
              double pullMC = positionError.Dot(mcPosition.Unit());
              histsPull[j]->Fill( pullMC );
              histsPullFrac[j]->Fill( pullMC / positionError.Mag() );

            }
        }
      
    }

  cerr << endl;

  string draw = "";
  static int firstDraw = 0;
  if(clear)
    firstDraw = 0;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      
      if(firstDraw>0)
        draw = "SAMES";

      TVirtualPad* cPadPull = canPull->cd();
      histsPull[i]->SetLineColor(GetColor(firstDraw));
      histsPull[i]->Draw(draw.c_str());
      canPull->Update();
      ArrangeStatBox(histsPull[i], GetColor(firstDraw), cPadPull, plotNames[i], 0.15, 0.35);

      TVirtualPad* cPadPullFrac = canPullFrac->cd();
      histsPullFrac[i]->SetLineColor(GetColor(firstDraw));
      histsPullFrac[i]->Draw(draw.c_str());
      canPullFrac->Update();
      ArrangeStatBox(histsPullFrac[i], GetColor(firstDraw), cPadPullFrac, plotNames[i], 0.15, 0.35);

      firstDraw++;

    }
  
  ScaleHists(canPull, true);
  ScaleHists(canPullFrac, true);

}
