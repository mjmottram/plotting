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
#include <algorithm>


void plotPositionUncertainties(string fileName, string fitName, string plotName="", bool clear=true);
void plotPositionUncertainties(string fileName, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotPositionUncertainties(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotPositionUncertainties(RAT::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear=true);


void plotPositionUncertainties(string fileName, string fitName, string plotName, bool clear)
{
  RAT::DSReader dsReader(fileName.c_str());
  vector<string> fitNames;
  vector<string> plotNames;
  fitNames.push_back(fitName);
  if(plotName!="")
    plotNames.push_back(plotName);
  plotPositionUncertainties(dsReader, fitNames, plotNames, clear);
}


void plotPositionUncertainties(string fileName, vector<string> fitNames, const vector<string> plotNames, bool clear)
{
  RAT::DSReader dsReader(fileName.c_str());
  plotPositionUncertainties(dsReader, fitNames, plotNames, clear);
}


void plotPositionUncertainties(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames, bool clear)
{
  RAT::DSReader dsReader(fileNames[0].c_str());
  for(unsigned int i=1;i<fileNames.size();i++)
    dsReader.Add(fileNames[i].c_str());
  plotPositionUncertainties(dsReader, fitNames, plotNames, clear);
}


// Plots errors on x, y, z and time.

void plotPositionUncertainties(RAT::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear)
{
  
  vector<TH1F*> histsX;
  vector<TH1F*> histsY;
  vector<TH1F*> histsZ;
  vector<TH1F*> histsR;

  string posTitle = " reported uncertainty (mm)";

  if(plotNames.size()==0)
    plotNames = fitNames;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      // first, check and delete any current hists
      histsX.push_back( CreateHist( "hXUnc"+plotNames[i], ";X"+posTitle, 100, 0, 500) );
      histsY.push_back( CreateHist( "hYUnc"+plotNames[i], ";Y"+posTitle, 100, 0, 500) );
      histsZ.push_back( CreateHist( "hZUnc"+plotNames[i], ";Z"+posTitle, 100, 0, 500) );
      histsR.push_back( CreateHist( "hRUnc"+plotNames[i], ";R"+posTitle, 100, 0, 500) );
    }
  
  int nNoErr = 0;
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

      // Now get the different fit positions and compare
      
      for(unsigned int j=0; j<fitNames.size(); j++)
        {
          RAT::DS::FitVertex fitVertex = ev->GetFitResult(fitNames[j]).GetVertex(0);
          TVector3 posPosErr = fitVertex.GetPosPositionError();
          
          if(fitVertex.ContainsPosition() && fitVertex.ValidPosition())
            {
              histsX[j]->Fill(posPosErr.X());
              histsY[j]->Fill(posPosErr.Y());
              histsZ[j]->Fill(posPosErr.Z());
              histsR[j]->Fill(posPosErr.Mag());
            }
          else
            nFailed[j]++;

        }
      
    }

  cerr << endl;
  for(unsigned int i=0; i<fitNames.size(); i++)
    cout << fitNames[i] << "\tfailed: " << nFailed[i] << endl;

  TCanvas* canX = CreateCan("canXUnc", clear);
  TCanvas* canY = CreateCan("canYUnc", clear);
  TCanvas* canZ = CreateCan("canZUnc", clear);
  TCanvas* canR = CreateCan("canRUnc", clear);

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
