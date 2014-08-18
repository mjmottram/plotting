#include <rootPlotUtil.hh>

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

void summariseError(string fileName, string fitName);
void summariseError(string fileName, vector<string> fitNames);
void summariseError(vector<string> fileNames, vector<string> fitNames);
void summariseError(RAT::DU::DSReader& dsReader, vector<string> fitNames);


void summariseError(string fileName, string fitName)
{
  RAT::DU::DSReader dsReader(fileName.c_str());
  vector<string> fitNames;
  fitNames.push_back(fitName);
  summariseError(dsReader, fitNames);
}


void summariseError(string fileName, vector<string> fitNames)
{
  RAT::DU::DSReader dsReader(fileName.c_str());
  summariseError(dsReader, fitNames);
}


void summariseError(vector<string> fileNames, vector<string> fitNames)
{
  RAT::DU::DSReader dsReader(fileNames[0].c_str());
  for(unsigned int i=1;i<fileNames.size();i++)
    dsReader.Add(fileNames[i].c_str());
  summariseError(dsReader, fitNames);
}


// Plots errors on x, y, z and time.

void summariseError(RAT::DU::DSReader& dsReader, vector<string> fitNames)
{
  
  cout << "Total entries: " << dsReader.GetEntryCount() << endl;

  int nTotal = dsReader.GetEntryCount();

  vector<int>nFailed (fitNames.size(), 0);
  vector<int>nFailedEnergy (fitNames.size(), 0);
  vector<int>nFailedPosition (fitNames.size(), 0);
  vector<int>nFailedDirection (fitNames.size(), 0);
  vector<int>nFailedTime (fitNames.size(), 0);

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

          const RAT::DS::FitVertex& fitVertex = ev.GetFitResult(fitNames[j]).GetVertex(0);

          if(!fitVertex.GetValid())
            nFailed[j]++;
          if(fitVertex.ContainsDirection() && !fitVertex.ValidDirection())
            nFailedDirection[j]++;
          if(fitVertex.ContainsPosition() && !fitVertex.ValidPosition())
            nFailedPosition[j]++;
          if(fitVertex.ContainsTime() && !fitVertex.ValidTime())
            nFailedTime[j]++;
          if(fitVertex.ContainsEnergy() && !fitVertex.ValidEnergy())
            nFailedEnergy[j]++;

        }

    }

  cerr << endl;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      
      cout << "FIT: " << fitNames[i] << endl;
      cout << "Failed: " << nFailed[i] << "\t" << static_cast<double>(nFailed[i]) / nTotal << endl;
      cout << "Failed Position: " << nFailedPosition[i] << "\t" << static_cast<double>(nFailedPosition[i]) / nTotal << endl;
      cout << "Failed Time: " << nFailedTime[i] << "\t" << static_cast<double>(nFailedTime[i]) / nTotal << endl;
      cout << "Failed Energy: " << nFailedEnergy[i] << "\t" << static_cast<double>(nFailedEnergy[i]) / nTotal << endl;
      cout << "Failed Direction: " << nFailedDirection[i] << "\t" << static_cast<double>(nFailedDirection[i]) / nTotal << endl;
      cout << endl;

    }
  
}
