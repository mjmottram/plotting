#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <RAT/DS/Run.hh>
#include <RAT/DS/Root.hh>
#include <RAT/DB.hh>
#include <RAT/DBLink.hh>

#include <TH1F.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TLegend.h>

using namespace std;

void initDB();
void plotET1D(string index);
void plotET1D(vector<string> indexes);
void plotGV1D(string index);
void plotGV1D(vector<string> indexes);

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

  for(unsigned int i=0; i<indexes.size(); i++)
    {
      RAT::DBLinkPtr ET1D = RAT::DB::Get()->GetLink("ET1D", indexes[i]);
      vector<double> times;
      vector<double> probability;

      times = ET1D->GetDArray( "time" );
      probability = ET1D->GetDArray( "probability" );
  
      TGraph *gr = new TGraph();

      for(int i=0;i<times.size();i++)
        gr->SetPoint(i, times[i], probability[i]);      

      graphs.push_back( gr );

      gr->Draw("al");
      gr->SetName(("gET1D" + indexes[i]).c_str());

    }

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

      for(int i=0;i<times.size();i++)
        gr->SetPoint(i, times[i], probability[i]);      

      graphs.push_back( gr );

      gr->Draw("al");
      gr->SetName(("gGV1D" + indexes[i]).c_str());

    }


}
