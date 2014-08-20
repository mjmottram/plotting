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

void plotPull(string fileName, string fitName, string plotName="", bool clear=true);
void plotPull(string fileName, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotPull(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotPull(RAT::DU::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear=true);


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

  vector<TH1F*> histsPullMC;
  vector<TH1F*> histsPullMCFrac;
  vector<TH1F*> histsPullFit;
  vector<TH1F*> histsPullFitFrac;

  TCanvas* canPullMC = CreateCan("canPullMC", clear);
  TCanvas* canPullMCFrac = CreateCan("canPullMCFrac", clear);
  TCanvas* canPullFit = CreateCan("canPullFit", clear);
  TCanvas* canPullFitFrac = CreateCan("canPullFitFrac", clear);

  string pullMCTitle = " (#vec{r}_{Fit} - #vec{r}_{MC}).#hat{d}_{MC}";
  string pullMCFracTitle = " (#vec{r}_{Fit} - #vec{r}_{MC}).#hat{d}_{MC} / #cbar #vec{r}_{Fit} - #vec{r}_{MC} #cbar";
  string pullFitTitle = " (#vec{r}_{Fit} - #vec{r}_{MC}).#hat{d}_{Fit}";
  string pullFitFracTitle = " (#vec{r}_{Fit} - #vec{r}_{MC}).#hat{d}_{Fit} / #cbar #vec{r}_{Fit} - #vec{r}_{MC} #cbar";

  if(plotNames.size()==0)
    plotNames = fitNames;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      // first, check and delete any current hists
      histsPullMC.push_back( CreateHist( "hPullMC"+plotNames[i], ";"+pullMCTitle, kPullBins, kPullLow, kPullHigh) );
      histsPullMCFrac.push_back( CreateHist( "hPullMCFrac"+plotNames[i], ";"+pullMCFracTitle, 200, -1, 1) );
      histsPullFit.push_back( CreateHist( "hPullFit"+plotNames[i], ";"+pullFitTitle, kPullBins, kPullLow, kPullHigh) );
      histsPullFitFrac.push_back( CreateHist( "hPullFitFrac"+plotNames[i], ";"+pullFitFracTitle, 200, -1, 1) );
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
          
          if( fitVertex.ContainsPosition() && (fitVertex.ValidPosition() || kIncludeInvalidFits))
            {
              TVector3 fitPosition = fitVertex.GetPosition();
              TVector3 positionError = fitPosition - mcPosition;

              double pullMC = positionError.Dot(mcPosition);
              double pullFit = positionError.Dot(fitPosition);

              histsPullMC[j]->Fill( pullMC );
              histsPullMCFrac[j]->Fill( pullMC / positionError.Mag() );

              histsPullFit[j]->Fill( pullFit );
              histsPullFitFrac[j]->Fill( pullFit / positionError.Mag() );
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

      TVirtualPad* cPadPullMC = canPullMC->cd();
      histsPullMC[i]->SetLineColor(GetColor(firstDraw));
      histsPullMC[i]->Draw(draw.c_str());
      canPullMC->Update();
      ArrangeStatBox(histsPullMC[i], GetColor(firstDraw), cPadPullMC, plotNames[i]);

      TVirtualPad* cPadPullMCFrac = canPullMCFrac->cd();
      histsPullMCFrac[i]->SetLineColor(GetColor(firstDraw));
      histsPullMCFrac[i]->Draw(draw.c_str());
      canPullMCFrac->Update();
      ArrangeStatBox(histsPullMCFrac[i], GetColor(firstDraw), cPadPullMCFrac, plotNames[i]);

      TVirtualPad* cPadPullFit = canPullFit->cd();
      histsPullFit[i]->SetLineColor(GetColor(firstDraw));
      histsPullFit[i]->Draw(draw.c_str());
      canPullFit->Update();
      ArrangeStatBox(histsPullFit[i], GetColor(firstDraw), cPadPullFit, plotNames[i]);

      TVirtualPad* cPadPullFitFrac = canPullFitFrac->cd();
      histsPullFitFrac[i]->SetLineColor(GetColor(firstDraw));
      histsPullFitFrac[i]->Draw(draw.c_str());
      canPullFitFrac->Update();
      ArrangeStatBox(histsPullFitFrac[i], GetColor(firstDraw), cPadPullFitFrac, plotNames[i]);

      firstDraw++;

    }
  
  ScaleHists(canPullMC, true);
  ScaleHists(canPullMCFrac, true);
  ScaleHists(canPullFit, true);
  ScaleHists(canPullFitFrac, true);

}
