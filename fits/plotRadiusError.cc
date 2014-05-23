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


void plotRadiusError(string fileName, string fitName, string plotName="", bool clear=true);
void plotRadiusError(string fileName, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotRadiusError(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotRadiusError(RAT::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear=true);


void plotRadiusError(string fileName, string fitName, string plotName, bool clear)
{
  RAT::DSReader dsReader(fileName.c_str());
  vector<string> fitNames;
  vector<string> plotNames;
  fitNames.push_back(fitName);
  if(plotName!="")
    plotNames.push_back(plotName);
  plotRadiusError(dsReader, fitNames, plotNames, clear);
}


void plotRadiusError(string fileName, vector<string> fitNames, const vector<string> plotNames, bool clear)
{
  RAT::DSReader dsReader(fileName.c_str());
  plotRadiusError(dsReader, fitNames, plotNames, clear);
}


void plotRadiusError(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames, bool clear)
{
  RAT::DSReader dsReader(fileNames[0].c_str());
  for(unsigned int i=1;i<fileNames.size();i++)
    dsReader.Add(fileNames[i].c_str());
  plotRadiusError(dsReader, fitNames, plotNames, clear);
}


// Plots errors on x, y, z and time.

void plotRadiusError(RAT::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear)
{
  
  vector<TH1F*> histsRad;

  string rTitle = " fit - mc (mm)";

  if(plotNames.size()==0)
    plotNames = fitNames;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      // first, check and delete any current hists
      histsRad.push_back( CreateHist( "hRadErr"+plotNames[i], ";Radius "+rTitle, 100, -1000, 1000) );
    }
  
  cout << "Total entries: " << dsReader.GetTotal() << endl;

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
              histsRad[j]->Fill(fitPosition.Mag() - mcPosition.Mag());
            }

        }
      
    }

  cerr << endl;

  TCanvas* canRad = CreateCan("canErrRad", clear);

  string draw = "";
  static int firstDraw = 0;
  if(clear)
    firstDraw = 0;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      
      if(firstDraw>0)
        draw = "SAMES";

      TVirtualPad* cPadRad = canRad->cd();
      histsRad[i]->SetLineColor(GetColor(firstDraw));      
      histsRad[i]->Draw(draw.c_str());
      canRad->Update();
      ArrangeStatBox(histsRad[i], GetColor(firstDraw), cPadRad, plotNames[i]);

      firstDraw++;

    }
  
  ScaleHists(canRad, true);

}
