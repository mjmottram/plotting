#include <rootPlotUtil.hh>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <RAT/Log.hh>
#include <RAT/DB.hh>
#include <RAT/DBLink.hh>

#include <TH1F.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>

using namespace std;

void initDB();
void plotET1D(string index);
void plotET1D(vector<string> indexes);
void plotGV1D(string index);
void plotGV1D(vector<string> indexes);
void plotFitDir(string index);
void plotFitDir(vector<string> indexes);

//////////////////////////////////////////////////////
// Utility functions
//////////////////////////////////////////////////////

void initDB()
{
  static bool loaded = false;
  if(!loaded)
    {
      RAT::Log::Init("/dev/null");
      RAT::DB *db = RAT::DB::Get();
      db->LoadDefaults();
      loaded = true;
    }
}


//////////////////////////////////////////////////////
// Time PDF plotting
//////////////////////////////////////////////////////
void plotET1D(string index)
{
  vector<string> indexes;
  indexes.push_back(index);
  plotET1D(indexes);
}

void plotET1D(vector<string> indexes)
{

  initDB();

  vector<TGraph*> graphs;

  TCanvas *can = new TCanvas("canET1D", "canET1D");
  can->cd();

  TLegend *legend = new TLegend(0.6, 0.7, 0.9, 0.9);
  legend->SetFillColor(0);

  for(unsigned int i=0; i<indexes.size(); i++)
    {
      RAT::DBLinkPtr ET1D = RAT::DB::Get()->GetLink("ET1D", indexes[i]);
      vector<double> times;
      vector<double> probability;

      times = ET1D->GetDArray( "time" );
      probability = ET1D->GetDArray( "probability" );
  
      TGraph *gr = new TGraph();

      for(int iTime=0;iTime<times.size();iTime++)
        gr->SetPoint(iTime, times[iTime], probability[iTime]);      

      graphs.push_back( gr );

      if(i==0)
          gr->Draw("al");
      else
          gr->Draw("l");
      gr->SetName(("gET1D" + indexes[i]).c_str());
      gr->SetLineColor(GetColor(i));

      legend->AddEntry(gr, indexes[i].c_str(), "l");

    }

  legend->Draw();

}



void plotGV1D(string index)
{
  vector<string> indexes;
  indexes.push_back(index);
  plotGV1D(indexes);
}

void plotGV1D(vector<string> indexes)
{

  initDB();

  vector<TGraph*> graphs;

  TCanvas *can = new TCanvas("canGV1D", "canGV1D");
  can->cd();

  for(unsigned int i=0; i<indexes.size(); i++)
    {
      RAT::DBLinkPtr GV1D = RAT::DB::Get()->GetLink("GV1D", indexes[i]);
      vector<double> times;
      vector<double> probability;

      times = GV1D->GetDArray( "time" );
      probability = GV1D->GetDArray( "probability" );
  
      TGraph *gr = new TGraph();

      for(int iTime=0;iTime<times.size();iTime++)
        gr->SetPoint(iTime, times[iTime], probability[iTime]);      

      graphs.push_back( gr );

      gr->Draw("al");
      gr->SetName(("gGV1D" + indexes[i]).c_str());

      gr->SetTitle( ("GV1D: " + indexes[i]).c_str() );

      gPad->SetLogy();
      gr->GetXaxis()->SetTitle("Time residuals [ns]");
      gr->GetYaxis()->SetTitle("Probability [arb]");

    }


}




void plotFitDir(string index)
{
  vector<string> indexes;
  indexes.push_back(index);
  plotFitDir(indexes);
}

void plotFitDir(vector<string> indexes)
{

  initDB();

  gStyle->SetTitleOffset(1.4, "X");
  gStyle->SetTitleOffset(1.2, "Y");
  vector<TGraph*> graphs;

  TCanvas *can = new TCanvas("canFitDir", "canFitDir");
  can->cd();

  for(unsigned int i=0; i<indexes.size(); i++)
    {
      RAT::DBLinkPtr FitDir = RAT::DB::Get()->GetLink("FIT_DIR", indexes[i]);
      vector<double> times;
      vector<double> probability;

      times = FitDir->GetDArray( "angle" );
      probability = FitDir->GetDArray( "probability" );
  
      TGraph *gr = new TGraph();

      for(int iTime=0;iTime<times.size();iTime++)
        gr->SetPoint(iTime, times[iTime], probability[iTime]);      

      graphs.push_back( gr );

      gr->Draw("al");
      gr->SetName(("gFitDir" + indexes[i]).c_str());

      gr->SetTitle( ("FitDir: " + indexes[i]).c_str() );

      gPad->SetBottomMargin(0.13);
      gPad->SetLeftMargin(0.11);
      gr->GetXaxis()->SetTitle("cos^{-1}(#vec{d_{ev}}.(#vec{PMT} - #vec{r_{ev}})) [rad.]");
      gr->GetYaxis()->SetTitle("Probability [arb]");

    }


}



void plotTwoPar(string type, string index, string xpars, string ypars)
{

  initDB();

  TCanvas* can = new TCanvas("canTwoPars", "canTwoPars");

  RAT::DBLinkPtr dbLink = RAT::DB::Get()->GetLink(type, index);
  vector<double> xvals = dbLink->GetDArray(xpars);
  vector<double> yvals = dbLink->GetDArray(ypars);
  
  TGraph *gr = new TGraph();

  for(int i=0;i<xvals.size();i++)
    gr->SetPoint(i, xvals[i], yvals[i]);      

  gr->GetXaxis()->SetTitle(xpars.c_str());
  gr->GetYaxis()->SetTitle(ypars.c_str());
  gr->SetMarkerStyle(23);
  gr->Draw("ap");

}

