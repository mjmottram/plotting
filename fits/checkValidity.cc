#include <RAT/DU/DSReader.hh>
#include <RAT/DS/Entry.hh>
#include <RAT/DS/EV.hh>

#include <iostream>
#include <sstream>
#include <iomanip> 

void checkValidity(string fileName, string fitName);


void checkValidity(string fileName, string fitName)
{

  RAT::DU::DSReader dsReader(fileName.c_str());

  int totalEvents = 0;
  int fitExists = 0;
  int validDirection = 0;
  int validPosition = 0;
  int validEnergy = 0;
  int validTime = 0;

  for(size_t i=0; i<dsReader.GetEntryCount(); i++)
    {

      if(dsReader.GetEntryCount() > 100)
        if(i % (dsReader.GetEntryCount() / 20) == 0)
          cerr << "*";

      const RAT::DS::Entry& rds = dsReader.GetEntry(i);

      if(rds.GetEVCount()==0)
        continue;

      const RAT::DS::EV& ev = rds.GetEV( 0 );
      totalEvents++;

      if( ev.FitResultExists( fitName ) ) fitExists++;
      else continue;

      if( ev.GetFitResult( fitName ).GetVertex( 0 ).ValidDirection() ) validDirection++;
      if( ev.GetFitResult( fitName ).GetVertex( 0 ).ValidPosition() ) validPosition++;
      if( ev.GetFitResult( fitName ).GetVertex( 0 ).ValidEnergy() ) validEnergy++;
      if( ev.GetFitResult( fitName ).GetVertex( 0 ).ValidTime() ) validTime++;
      
    }

  cerr << endl;

  float percentExists = static_cast<float>(fitExists) / static_cast<float>(totalEvents) * 100;
  float percentDirection = static_cast<float>(validDirection) / static_cast<float>(totalEvents) * 100;
  float percentPosition = static_cast<float>(validPosition) / static_cast<float>(totalEvents) * 100;
  float percentEnergy = static_cast<float>(validEnergy) / static_cast<float>(totalEvents) * 100;
  float percentTime = static_cast<float>(validTime) / static_cast<float>(totalEvents) * 100;

  cout << "Total events: " << totalEvents << endl;
  cout << "Fit exists: " << percentExists << endl;
  cout << "Direction: " << percentDirection << endl;
  cout << "Position: " << percentPosition << endl;
  cout << "Energy: " << percentEnergy << endl;
  cout << "Time: " << percentTime << endl;

}
