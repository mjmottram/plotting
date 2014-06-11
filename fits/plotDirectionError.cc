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

extern double kDirectionErrorHigh = 1.0;
extern double kDirectionErrorLow = -1.0;
extern int kDirectionErrorBins = 200;


void printDirectionErrorCanvases(string filePrefix);
void plotDirectionError(string fileName, string fitName, string plotName="", bool clear=true);
void plotDirectionError(string fileName, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotDirectionError(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotDirectionError(RAT::DU::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear=true);



void printDirectionErrorCanvases(string filePrefix)
{
  vector<TCanvas*> canvases;
  vector<string> names;
  canvases.push_back((TCanvas*)gROOT->FindObject( "canDot" ));
  names.push_back( "ErrDot" );
  PrintCanvases(canvases, filePrefix, names);

}


void plotDirectionError(string fileName, string fitName, string plotName, bool clear)
{
  RAT::DU::DSReader dsReader(fileName.c_str());
  vector<string> fitNames;
  vector<string> plotNames;
  fitNames.push_back(fitName);
  if(plotName!="")
    plotNames.push_back(plotName);
  plotDirectionError(dsReader, fitNames, plotNames, clear);
}


void plotDirectionError(string fileName, vector<string> fitNames, const vector<string> plotNames, bool clear)
{
  RAT::DU::DSReader dsReader(fileName.c_str());
  plotDirectionError(dsReader, fitNames, plotNames, clear);
}


void plotDirectionError(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames, bool clear)
{
  RAT::DU::DSReader dsReader(fileNames[0].c_str());
  for(unsigned int i=1;i<fileNames.size();i++)
      dsReader.Add(fileNames[i].c_str());
  plotDirectionError(dsReader, fitNames, plotNames, clear);
}


// Plots errors on x, y, z and time.

void plotDirectionError(RAT::DU::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear)
{

  vector<TH1F*> histsDot;

  string xTitle = "#hat{d_{fit}}.#hat{d_{MC}}";

  if(plotNames.size()==0)
    plotNames = fitNames;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      // first, check and delete any current hists
      histsDot.push_back( CreateHist( "hDirErr"+plotNames[i], ";"+xTitle, kDirectionErrorBins, kDirectionErrorLow, kDirectionErrorHigh) );
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
      TVector3 mcDirection = mcp.GetMomentum().Unit();

      // Now get the different fit positions and compare
      
      for(unsigned int j=0; j<fitNames.size(); j++)
        {

          const RAT::DS::FitVertex& fitVertex = ev.GetFitResult(fitNames[j]).GetVertex(0);

          // For some fits may also need to consider whether the positions seed was valid
          
          if(fitVertex.ContainsDirection() && fitVertex.ValidDirection())
            {
              TVector3 fitDirection = fitVertex.GetDirection().Unit();
              histsDot[j]->Fill( fitDirection.Dot(mcDirection) );
            }
        }

    }

  cerr << endl;

  TCanvas* canDot = CreateCan("canDot", clear);

  string draw = "";
  static int firstDraw = 0;
  if(clear)
    firstDraw = 0;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      
      if(firstDraw>0)
        draw = "SAMES";

      TVirtualPad* cPadDot = canDot->cd();
      histsDot[i]->SetLineColor(GetColor(firstDraw));
      histsDot[i]->Draw(draw.c_str());
      canDot->Update();
      ArrangeStatBox(histsDot[i], GetColor(firstDraw), cPadDot, plotNames[i], 0.15, 0.35);

      firstDraw++;

    }
  
  ScaleHists(canDot, true);

}
