#include <rootPlotUtil.hh>

#include <TH1F.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TVirtualPad.h>
#include <TPaveStats.h>
#include <TString.h>
#include <TLatex.h>

#include <RAT/DSReader.hh>
#include <RAT/DS/Root.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/MC.hh>
#include <RAT/DS/MCParticle.hh>
#include <RAT/DS/FitResult.hh>
#include <RAT/DS/FitVertex.hh>

#include <iostream>
#include <sstream>
#include <iomanip> 

extern double kPositionErrorRange = 1000;
extern double kPositionErrorBins = 100;

void printPositionErrorCanvases(string filePrefix);
void plotPositionError(string fileName, string fitName, string plotName="", bool clear=true);
void plotPositionError(string fileName, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotPositionError(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotPositionError(RAT::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear=true);


void printPositionErrorCanvases(string filePrefix)
{
  vector<TCanvas*> canvases;
  vector<string> names;
  canvases.push_back((TCanvas*)gROOT->FindObject( "canErrX" ));
  canvases.push_back((TCanvas*)gROOT->FindObject( "canErrY" ));
  canvases.push_back((TCanvas*)gROOT->FindObject( "canErrZ" ));
  canvases.push_back((TCanvas*)gROOT->FindObject( "canErrR" ));
  names.push_back( "ErrX" );
  names.push_back( "ErrY" );
  names.push_back( "ErrZ" );
  names.push_back( "ErrR" );
  PrintCanvases(canvases, filePrefix, names);

}

void plotPositionError(string fileName, string fitName, string plotName, bool clear)
{
  RAT::DSReader dsReader(fileName.c_str());
  vector<string> fitNames;
  vector<string> plotNames;
  fitNames.push_back(fitName);
  if(plotName!="")
    plotNames.push_back(plotName);
  plotPositionError(dsReader, fitNames, plotNames, clear);
}


void plotPositionError(string fileName, vector<string> fitNames, const vector<string> plotNames, bool clear)
{
  RAT::DSReader dsReader(fileName.c_str());
  plotPositionError(dsReader, fitNames, plotNames, clear);
}


void plotPositionError(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames, bool clear)
{
  RAT::DSReader dsReader(fileNames[0].c_str());
  for(unsigned int i=1;i<fileNames.size();i++)
    dsReader.Add(fileNames[i].c_str());
  plotPositionError(dsReader, fitNames, plotNames, clear);
}


// Plots errors on x, y, z and time.

void plotPositionError(RAT::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear)
{
  
  vector<TH1F*> histsX;
  vector<TH1F*> histsY;
  vector<TH1F*> histsZ;
  vector<TH1F*> histsR;

  TCanvas* canX = CreateCan("canErrX", clear);
  TCanvas* canY = CreateCan("canErrY", clear);
  TCanvas* canZ = CreateCan("canErrZ", clear);
  TCanvas* canR = CreateCan("canErrR", clear);

  string posTitle = " fit - mc (mm)";
  string rTitle = " |fit - mc| (mm)";

  if(plotNames.size()==0)
    plotNames = fitNames;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      // first, check and delete any current hists
      histsX.push_back( CreateHist( "hXErr"+plotNames[i], ";X"+posTitle, kPositionErrorBins, -kPositionErrorRange, kPositionErrorRange ) );
      histsY.push_back( CreateHist( "hYErr"+plotNames[i], ";Y"+posTitle, kPositionErrorBins, -kPositionErrorRange, kPositionErrorRange ) );
      histsZ.push_back( CreateHist( "hZErr"+plotNames[i], ";Z"+posTitle, kPositionErrorBins, -kPositionErrorRange, kPositionErrorRange ) );
      histsR.push_back( CreateHist( "hRErr"+plotNames[i], ";R"+rTitle, kPositionErrorBins, 0, kPositionErrorRange) );
    }
  
  // cout << "Total entries: " << dsReader.GetTotal() << endl;

  vector<int> nFailed(fitNames.size(), 0);

  for(int i=0; i<dsReader.GetTotal(); i++)
    {

      if(dsReader.GetTotal() > 100)
        if(i % (dsReader.GetTotal() / 20) == 0)
          cerr << "*";

      RAT::DS::Root* rds = dsReader.GetEvent(i);

      if(rds->GetEVCount()==0)
        continue;

      RAT::DS::EV* ev = rds->GetEV( 0 );
      RAT::DS::MC* mc = rds->GetMC();
      RAT::DS::MCParticle* mcp = mc->GetMCParticle( 0 );
      TVector3 mcPosition = mcp->GetPos();

      // Now get the different fit positions and compare
      
      for(unsigned int j=0; j<fitNames.size(); j++)
        {
          RAT::DS::FitVertex fitVertex = ev->GetFitResult(fitNames[j]).GetVertex(0);
          TVector3 fitPosition = fitVertex.GetPosition();
          
          if(fitVertex.ContainsPosition() && fitVertex.ValidPosition())
            // will also test time
            {
              histsX[j]->Fill(fitPosition.X() - mcPosition.X());
              histsY[j]->Fill(fitPosition.Y() - mcPosition.Y());
              histsZ[j]->Fill(fitPosition.Z() - mcPosition.Z());
              histsR[j]->Fill((fitPosition - mcPosition).Mag());
            }
          else
            nFailed[j]++;

        }
      
    }

  cerr << endl;
  // for(unsigned int i=0; i<fitNames.size(); i++)
  //   cout << fitNames[i] << "\tfailed: " << nFailed[i] << endl;

  string draw = "";
  static int firstDraw = 0;
  if(clear)
    firstDraw = 0;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      
      if(firstDraw>0)
        draw = "SAMES";

      TVirtualPad* cPadX = canX->cd();
      histsX[i]->SetLineColor(GetColor(firstDraw));      
      histsX[i]->Draw(draw.c_str());
      canX->Update();
      ArrangeStatBox(histsX[i], GetColor(firstDraw), cPadX, plotNames[i]);

      TVirtualPad* cPadY = canY->cd();
      histsY[i]->SetLineColor(GetColor(firstDraw));      
      histsY[i]->Draw(draw.c_str());
      canY->Update();
      ArrangeStatBox(histsY[i], GetColor(firstDraw), cPadY, plotNames[i]);

      TVirtualPad* cPadZ = canZ->cd();
      histsZ[i]->SetLineColor(GetColor(firstDraw));      
      histsZ[i]->Draw(draw.c_str());
      canZ->Update();
      ArrangeStatBox(histsZ[i], GetColor(firstDraw), cPadZ, plotNames[i]);

      TVirtualPad* cPadR = canR->cd();
      histsR[i]->SetLineColor(GetColor(firstDraw));      
      histsR[i]->Draw(draw.c_str());
      canR->Update();
      ArrangeStatBox(histsR[i], GetColor(firstDraw), cPadR, plotNames[i]);

      firstDraw++;

    }
  
  ScaleHists(canX, true);
  ScaleHists(canY, true);
  ScaleHists(canZ, true);
  ScaleHists(canR, true);

}
