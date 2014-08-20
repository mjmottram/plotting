#include <rootPlotUtil.hh>

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
#include <algorithm>


void plotTimeUncertainties(string fileName, string fitName, string plotName="", bool clear=true);
void plotTimeUncertainties(string fileName, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotTimeUncertainties(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames = vector<string>(), bool clear=true);
void plotTimeUncertainties(RAT::DU::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear=true);


void plotTimeUncertainties(string fileName, string fitName, string plotName, bool clear)
{
  RAT::DU::DSReader dsReader(fileName.c_str());
  vector<string> fitNames;
  vector<string> plotNames;
  fitNames.push_back(fitName);
  if(plotName!="")
    plotNames.push_back(plotName);
  plotTimeUncertainties(dsReader, fitNames, plotNames, clear);
}


void plotTimeUncertainties(string fileName, vector<string> fitNames, const vector<string> plotNames, bool clear)
{
  RAT::DU::DSReader dsReader(fileName.c_str());
  plotTimeUncertainties(dsReader, fitNames, plotNames, clear);
}


void plotTimeUncertainties(vector<string> fileNames, vector<string> fitNames, const vector<string> plotNames, bool clear)
{
  RAT::DU::DSReader dsReader(fileNames[0].c_str());
  for(unsigned int i=1;i<fileNames.size();i++)
    dsReader.Add(fileNames[i].c_str());
  plotTimeUncertainties(dsReader, fitNames, plotNames, clear);
}


void plotTimeUncertainties(RAT::DU::DSReader& dsReader, vector<string> fitNames, vector<string> plotNames, bool clear)
{
  
  vector<TH1F*> histsT;

  string xTitle = " reported uncertainty (ns)";

  if(plotNames.size()==0)
    plotNames = fitNames;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      // first, check and delete any current hists
      histsT.push_back( CreateHist( "hTUnc"+plotNames[i], ";Time"+xTitle, 100, 0, 10) );
    }
  
  int nNoErr = 0;
  vector<int> nFailed(fitNames.size(), 0);

  for(size_t i=0; i<dsReader.GetEntryCount(); i++)
    {

      if(dsReader.GetEntryCount() > 100)
        if(i % (dsReader.GetEntryCount() / 20) == 0)
          cerr << "*";

      const RAT::DS::Entry& rds = dsReader.GetEntry(i);

      if(rds.GetEVCount()==0)
        continue;

      const RAT::DS::EV& ev = rds.GetEV( 0 );

      // Now get the different fit positions and compare
      
      for(unsigned int j=0; j<fitNames.size(); j++)
        {
          const RAT::DS::FitVertex fitVertex = ev.GetFitResult(fitNames[j]).GetVertex(0);
          double timeErr = fitVertex.GetPositiveTimeError();
          
          if(fitVertex.ContainsTime() && (fitVertex.ValidTime() || kIncludeInvalidFits))
            {
              histsT[j]->Fill(timeErr);
              if(timeErr > 1000)
                nNoErr++;
            }
          else
            nFailed[j]++;

        }
      
    }

  TCanvas* canT = CreateCan("canTUnc", clear);

  string draw = "";
  static int firstDraw = 0;
  if(clear)
    firstDraw = 0;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      
      if(firstDraw>0)
        draw = "SAMES";

      TVirtualPad* cPadT = canT->cd();
      histsT[i]->SetLineColor(GetColor(firstDraw));      
      histsT[i]->Draw(draw.c_str());
      canT->Update();
      ArrangeStatBox(histsT[i], GetColor(firstDraw), cPadT, plotNames[i]);

      firstDraw++;

    }
  
  ScaleHists(canT, true);

}
