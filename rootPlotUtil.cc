
#include <rootPlotUtil.hh>

#include <TROOT.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TVirtualPad.h>
#include <TPaveStats.h>
#include <TList.h>
#include <TSystem.h>

#include <iostream>
#include <sstream>

using namespace std;
using namespace ROOT;

bool gPlottingLogX = false;
bool gPlottingLogY = false;
bool gPlottingLogZ = false;
bool gStatOverflows = false;
bool kIncludeInvalidFits = false;

//////////////////////////////////////////////////
// Arrange statistics boxes for multiple plots
//////////////////////////////////////////////////

void ArrangeStatBox(TH1F* hHistogram,
		    TCanvas* can)
{
  // Update the canvas to ensure the stat box is available
  can->Update();
  ArrangeStatBox(hHistogram, hHistogram->GetLineColor(), can->cd());
}

void ArrangeStatBox(TH1F* hHistogram,
                    Int_t color,
                    TVirtualPad* pad,
                    string label,
		    double x1, double x2)
{
  // First Get the number of exisiting stat boxes
  int numStats = 0;
  TIter next( pad->GetListOfPrimitives() );
  while( TObject *obj = next() )
    {
      if( obj->GetName() == string( "stats" ) )
        numStats++;
    }
  TPaveStats *sBox = (TPaveStats*)hHistogram->GetListOfFunctions()->FindObject("stats");
  if(!sBox)
    {
      cout << "ArrangeStatBox::no stat box to move! Did you plot with S (or SAMES)" << endl;
      return;
    }
  hHistogram->GetListOfFunctions()->Remove( sBox );
  hHistogram->SetStats( 0 );
  sBox->SetLineColor( color );
  if(x1 > 0 && x2 > 0)
    {
      sBox->SetX1NDC( x1 );
      sBox->SetX2NDC( x2 );
    }
  sBox->SetY1NDC( sBox->GetY1NDC() - numStats * 0.2 );
  sBox->SetY2NDC( sBox->GetY2NDC() - numStats * 0.2 );
  sBox->Draw();
}


//////////////////////////////////////////////////
// Create a new TCanvas for given name (or return existing one)
//////////////////////////////////////////////////

TCanvas* CreateCan(string name, bool clear)
{
  TCanvas* c = (TCanvas*)gROOT->FindObject(name.c_str());
  if(c)
    {
      if( !clear )
        return c;
      delete c;
      c = NULL;
    }
  c = new TCanvas(name.c_str(), name.c_str());
  c->SetLogx( gPlottingLogX );
  c->SetLogy( gPlottingLogY );
  c->SetLogz( gPlottingLogZ );
  return c;
}


//////////////////////////////////////////////////
// Create a new TH1F for given name (clears old)
//////////////////////////////////////////////////

TH1F* CreateHist(string name, string title, int n, double x1, double x2)
{
  TH1F* h = (TH1F*)gROOT->FindObject(name.c_str());
  if(h)
    {
      delete h;
      h = NULL;
    }
  h = new TH1F(name.c_str(), title.c_str(), n, x1, x2);
  h->StatOverflows(gStatOverflows);
  return h;
}


//////////////////////////////////////////////////
// Gets color
//////////////////////////////////////////////////

Int_t GetColor(int i)
{
  const int gNColors = 5;
  Int_t gColors[gNColors] = {kBlack, kRed+1, kBlue+2, kGreen+2, kMagenta+1};
  int col = i;
  if(i>gNColors)
    cerr << "Cannot get color, index of of range" << endl;
  else
    col = gColors[i];
  return col;
}


//////////////////////////////////////////////////
// Normalise histograms
//////////////////////////////////////////////////
void Normalise(TH1F* hist)
{
  double sum = hist->GetSumOfWeights(); // should really do sumofweights
  for(int i=0;i<hist->GetNbinsX();i++)
    {
      double content = hist->GetBinContent(i+1) / sum;
      double error = hist->GetBinError(i+1) / sum;
      hist->SetBinContent(i+1, content );
      hist->SetBinError(i+1, error);
    }
}
void Normalise(TH1D* hist)
{
  double sum = hist->GetSumOfWeights(); // should really do sumofweights
  for(int i=0;i<hist->GetNbinsX();i++)
    {
      double content = hist->GetBinContent(i+1) / sum;
      double error = hist->GetBinError(i+1) / sum;
      hist->SetBinContent(i+1, content );
      hist->SetBinError(i+1, error);
    }
}

//////////////////////////////////////////////////
// Print (save) canvases
//////////////////////////////////////////////////
void PrintCanvases(vector<TCanvas*> canvases, string prefix, vector<string> names)
{
  for( unsigned int iCanvas=0; iCanvas<canvases.size(); iCanvas++)
    {
      stringstream ss;
      ss << prefix << names[iCanvas] << ".pdf";
      canvases[iCanvas]->Print(ss.str().c_str());
    }
}

//////////////////////////////////////////////////
// Scale hists (already drawn) to fit on canvas.
//////////////////////////////////////////////////

void ScaleGraphs(TCanvas* can, bool logScale, double min)
{  
  double maxContent = 0;
  double minContent = 1e10;

  vector<TGraph*> graphs;

  TIter canIter( can->GetListOfPrimitives() );
  while( TObject* obj = canIter() )
    {  
      if( obj->InheritsFrom( "TGraph" ) )
        {
          TGraph* g = (TGraph*)can->GetPrimitive( obj->GetName() );
          for( int i=0; i<g->GetN(); i++)
            {
              if( g->GetY()[i] > maxContent )
                maxContent = g->GetY()[i];
              if( g->GetY()[i] < minContent )
                minContent = g->GetY()[i];
            }
          graphs.push_back(g);
        }
    }

  if(min > -1e9)
    {
      if(logScale)
        minContent = 0.5;
      else
        minContent = min;
    }
  else
    minContent = (minContent - fabs(minContent/10));
  
  for(unsigned int i=0;i<graphs.size();i++)
    graphs[i]->GetYaxis()->SetRangeUser(minContent, maxContent + fabs(maxContent/10));
  can->Update();

}


void ScaleHists(TCanvas* can, bool logScale)
{  
  double maxContent = 0;
  double minContent = 0;
  if(logScale)
    minContent = 0.5;

  vector<TH1*> hists;

  TIter canIter( can->GetListOfPrimitives() );
  while( TObject* obj = canIter() )
    {  
      if( obj->InheritsFrom( "TH1" ) )
        {
          TH1* h = (TH1*)can->GetPrimitive( obj->GetName() );
          if(h->GetBinContent(h->GetMaximumBin()) > maxContent)
            maxContent = h->GetBinContent(h->GetMaximumBin());
          hists.push_back(h);
        }
    }
  
  for(unsigned int i=0;i<hists.size();i++)
    {
      hists[i]->GetYaxis()->SetRangeUser(minContent, maxContent*1.1);
    }
  can->Update();

}
