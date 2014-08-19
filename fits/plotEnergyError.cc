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
#include <RAT/DS/FitResult.hh>
#include <RAT/DS/FitVertex.hh>

#include <iostream>
#include <sstream>
#include <iomanip> 

extern double kEnergyErrorHigh = -1.0;
extern double kEnergyErrorLow = 1.0;
extern int kEnergyErrorBins = 200;


void printEnergyErrorCanvases(string filePrefix);
void plotEnergyError(string fileName, string fitName, string plotName="", bool clear=true);
void plotEnergyError(string fileName, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotEnergyError(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotEnergyError(RAT::DU::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear=true);



void printEnergyErrorCanvases(string filePrefix)
{
  vector<TCanvas*> canvases;
  vector<string> names;
  canvases.push_back((TCanvas*)gROOT->FindObject( "canEnergyError" ));
  names.push_back( "ErrEnergy" );
  PrintCanvases(canvases, filePrefix, names);

}


void plotEnergyError(string fileName, string fitName, string plotName, bool clear)
{
  RAT::DU::DSReader dsReader(fileName.c_str());
  vector<string> fitNames;
  vector<string> plotNames;
  fitNames.push_back(fitName);
  if(plotName!="")
    plotNames.push_back(plotName);
  plotEnergyError(dsReader, fitNames, plotNames, clear);
}


void plotEnergyError(string fileName, vector<string> fitNames, const vector<string> plotNames, bool clear)
{
  RAT::DU::DSReader dsReader(fileName.c_str());
  plotEnergyError(dsReader, fitNames, plotNames, clear);
}


void plotEnergyError(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames, bool clear)
{
  RAT::DU::DSReader dsReader(fileNames[0].c_str());
  for(unsigned int i=1;i<fileNames.size();i++)
      dsReader.Add(fileNames[i].c_str());
  plotEnergyError(dsReader, fitNames, plotNames, clear);
}


// Plots errors on x, y, z and time.

void plotEnergyError(RAT::DU::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear)
{

  vector<TH1F*> histsEnergy;

  string xTitle = "E_{Fit} - E_{MC}";

  if(plotNames.size()==0)
    plotNames = fitNames;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      // first, check and delete any current hists
      histsEnergy.push_back( CreateHist( "hEnergyErr"+plotNames[i], ";"+xTitle, kEnergyErrorBins, kEnergyErrorLow, kEnergyErrorHigh) );
    }
  
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
      double mcEnergy = mc.GetScintEnergyDeposit();

      // Now get the different fit positions and compare
      
      for(unsigned int j=0; j<fitNames.size(); j++)
        {

          RAT::DS::FitVertex fitVertex = ev.GetFitResult(fitNames[j]).GetVertex(0);

          // For some fits may also need to consider whether the positions seed was valid
          
          if(fitVertex.ContainsEnergy() && fitVertex.ValidEnergy())
            {
              double fitEnergy = fitVertex.GetEnergy();
              histsEnergy[j]->Fill( fitEnergy - mcEnergy );
            }
        }

    }

  cerr << endl;

  TCanvas* canEnergyError = CreateCan("canEnergyError", clear);

  string draw = "";
  static int firstDraw = 0;
  if(clear)
    firstDraw = 0;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      
      if(firstDraw>0)
        draw = "SAMES";

      TVirtualPad* cPadDot = canEnergyError->cd();
      histsEnergy[i]->SetLineColor(GetColor(firstDraw));
      histsEnergy[i]->Draw(draw.c_str());
      canEnergyError->Update();
      ArrangeStatBox(histsEnergy[i], GetColor(firstDraw), cPadDot, plotNames[i], 0.15, 0.35);

      firstDraw++;

    }
  
  ScaleHists(canEnergyError, true);

}
