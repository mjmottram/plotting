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

extern int kDriveBins = 100;
extern double kDriveLow = -1000;
extern double kDriveHigh = 1000;

void plotDrive(string fileName, string posFitName, string dirFitName, string plotName="", bool clear=true);
void plotDrive(string fileName, vector<string> posFitNames, vector<string> dirFitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotDrive(vector<string> fileNames, vector<string> posFitNames, vector<string> dirFitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotDrive(RAT::DU::DSReader& dsReader, vector<string> posFitNames, vector<string> dirFitNames, vector<string> plotNames, bool clear=true);


void plotDrive(string fileName, string posFitName, string dirFitName, string plotName, bool clear)
{
  RAT::DU::DSReader dsReader(fileName.c_str());
  vector<string> posFitNames;
  vector<string> dirFitNames;
  vector<string> plotNames;
  posFitNames.push_back(posFitName);
  dirFitNames.push_back(dirFitName);
  if(plotName!="")
    plotNames.push_back(plotName);
  plotDrive(dsReader, posFitNames, dirFitNames, plotNames, clear);
}


void plotDrive(string fileName, vector<string> posFitNames, vector<string> dirFitNames, const vector<string> plotNames, bool clear)
{
  RAT::DU::DSReader dsReader(fileName.c_str());
  plotDrive(dsReader, posFitNames, dirFitNames, plotNames, clear);
}


void plotDrive(vector<string> fileNames, vector<string> posFitNames, vector<string> dirFitNames, const vector<string> plotNames, bool clear)
{
  RAT::DU::DSReader dsReader(fileNames[0].c_str());
  for(unsigned int i=1;i<fileNames.size();i++)
      dsReader.Add(fileNames[i].c_str());
  plotDrive(dsReader, posFitNames, dirFitNames, plotNames, clear);
}


// Plots errors on x, y, z and time.

void plotDrive(RAT::DU::DSReader& dsReader, vector<string> posFitNames, vector<string> dirFitNames, vector<string> plotNames, bool clear)
{

  vector<TH1F*> histsDriveMC;
  vector<TH1F*> histsDriveMCFrac;
  vector<TH1F*> histsDriveFit;
  vector<TH1F*> histsDriveFitFrac;

  TCanvas* canDriveMC = CreateCan("canDriveMC", clear);
  TCanvas* canDriveMCFrac = CreateCan("canDriveMCFrac", clear);
  TCanvas* canDriveFit = CreateCan("canDriveFit", clear);
  TCanvas* canDriveFitFrac = CreateCan("canDriveFitFrac", clear);

  string driveMCTitle = " (#vec{r}_{Fit} - #vec{r}_{MC}).#hat{d}_{MC}";
  string driveMCFracTitle = " (#vec{r}_{Fit} - #vec{r}_{MC}).#hat{d}_{MC} / #cbar #vec{r}_{Fit} - #vec{r}_{MC} #cbar";
  string driveFitTitle = " (#vec{r}_{Fit} - #vec{r}_{MC}).#hat{d}_{Fit}";
  string driveFitFracTitle = " (#vec{r}_{Fit} - #vec{r}_{MC}).#hat{d}_{Fit} / #cbar #vec{r}_{Fit} - #vec{r}_{MC} #cbar";

  if(plotNames.size()==0)
    plotNames = posFitNames;

  for(unsigned int i=0; i<posFitNames.size(); i++)
    {
      // first, check and delete any current hists
      histsDriveMC.push_back( CreateHist( "hDriveMC"+plotNames[i], ";"+driveMCTitle, kDriveBins, kDriveLow, kDriveHigh) );
      histsDriveMCFrac.push_back( CreateHist( "hDriveMCFrac"+plotNames[i], ";"+driveMCFracTitle, 200, -1, 1) );
      histsDriveFit.push_back( CreateHist( "hDriveFit"+plotNames[i], ";"+driveFitTitle, kDriveBins, kDriveLow, kDriveHigh) );
      histsDriveFitFrac.push_back( CreateHist( "hDriveFitFrac"+plotNames[i], ";"+driveFitFracTitle, 200, -1, 1) );
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
      TVector3 mcDirection = mcp.GetMomentum().Unit();

      // Now get the different fit positions and compare
      
      for(unsigned int j=0; j<posFitNames.size(); j++)
        {

          const RAT::DS::FitVertex& posFitVertex = ev.GetFitResult(posFitNames[j]).GetVertex(0);
          const RAT::DS::FitVertex& dirFitVertex = ev.GetFitResult(dirFitNames[j]).GetVertex(0);

          // For some fits may also need to consider whether the positions seed was valid
          
          if( dirFitVertex.ContainsDirection() && (dirFitVertex.ValidDirection() || kIncludeInvalidFits) &&
              posFitVertex.ContainsPosition() && (posFitVertex.ValidPosition() || kIncludeInvalidFits))
            {
              TVector3 fitDirection = dirFitVertex.GetDirection().Unit();
              TVector3 fitPosition = posFitVertex.GetPosition();
              TVector3 positionError = fitPosition - mcPosition;
              double driveMC = positionError.Dot(mcDirection);
              double driveFit = positionError.Dot(fitDirection);
              histsDriveMC[j]->Fill( driveMC );
              histsDriveMCFrac[j]->Fill( driveMC / positionError.Mag() );

              histsDriveFit[j]->Fill( driveFit );
              histsDriveFitFrac[j]->Fill( driveFit / positionError.Mag() );
            }
        }
      
    }

  cerr << endl;

  string draw = "";
  static int firstDraw = 0;
  if(clear)
    firstDraw = 0;

  for(unsigned int i=0; i<posFitNames.size(); i++)
    {
      
      if(firstDraw>0)
        draw = "SAMES";

      TVirtualPad* cPadDriveMC = canDriveMC->cd();
      histsDriveMC[i]->SetLineColor(GetColor(firstDraw));
      histsDriveMC[i]->Draw(draw.c_str());
      canDriveMC->Update();
      ArrangeStatBox(histsDriveMC[i], GetColor(firstDraw), cPadDriveMC, plotNames[i]);

      TVirtualPad* cPadDriveMCFrac = canDriveMCFrac->cd();
      histsDriveMCFrac[i]->SetLineColor(GetColor(firstDraw));
      histsDriveMCFrac[i]->Draw(draw.c_str());
      canDriveMCFrac->Update();
      ArrangeStatBox(histsDriveMCFrac[i], GetColor(firstDraw), cPadDriveMCFrac, plotNames[i]);

      TVirtualPad* cPadDriveFit = canDriveFit->cd();
      histsDriveFit[i]->SetLineColor(GetColor(firstDraw));
      histsDriveFit[i]->Draw(draw.c_str());
      canDriveFit->Update();
      ArrangeStatBox(histsDriveFit[i], GetColor(firstDraw), cPadDriveFit, plotNames[i]);

      TVirtualPad* cPadDriveFitFrac = canDriveFitFrac->cd();
      histsDriveFitFrac[i]->SetLineColor(GetColor(firstDraw));
      histsDriveFitFrac[i]->Draw(draw.c_str());
      canDriveFitFrac->Update();
      ArrangeStatBox(histsDriveFitFrac[i], GetColor(firstDraw), cPadDriveFitFrac, plotNames[i]);

      firstDraw++;

    }
  
  ScaleHists(canDriveMC, true);
  ScaleHists(canDriveMCFrac, true);
  ScaleHists(canDriveFit, true);
  ScaleHists(canDriveFitFrac, true);

}
