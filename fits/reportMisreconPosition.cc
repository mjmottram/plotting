#include <rootPlotUtil.hh>

#include <TH1F.h>

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


void reportMisreconPosition(string fileName, string fitName, double err=3.0, bool sigma=true);
void reportMisreconPosition(string fileName, vector<string> fitNames, double err=3.0, bool sigma=true);
void reportMisreconPosition(vector<string> fileNames, vector<string> fitNames, double err=3.0, bool sigma=true);
void reportMisreconPosition(RAT::DSReader& dsReader, vector<string> fitNames, double err=3.0, bool sigma=true);


void reportMisreconPosition(string fileName, string fitName, double err, bool sigma)
{
  RAT::DSReader dsReader(fileName.c_str());
  vector<string> fitNames;
  fitNames.push_back(fitName);
  reportMisreconPosition(dsReader, fitNames, err, sigma);
}


void reportMisreconPosition(string fileName, vector<string> fitNames, double err, bool sigma)
{
  RAT::DSReader dsReader(fileName.c_str());
  reportMisreconPosition(dsReader, fitNames, err, sigma);
}


void reportMisreconPosition(vector<string> fileNames, vector<string> fitNames, double err, bool sigma)
{
  RAT::DSReader dsReader(fileNames[0].c_str());
  for(unsigned int i=1;i<fileNames.size();i++)
    dsReader.Add(fileNames[i].c_str());
  reportMisreconPosition(dsReader, fitNames, err, sigma);
}


void reportMisreconPosition(RAT::DSReader& dsReader, vector<string> fitNames, double err, bool sigma)
{
  
  vector<TH1F*> histsX;
  vector<TH1F*> histsY;
  vector<TH1F*> histsZ;
  vector<TH1F*> histsR;

  double xBound = 10000.0;
  double rBound = 10000.0;

  vector<int> nX(fitNames.size(), 0);
  vector<int> nY(fitNames.size(), 0);
  vector<int> nZ(fitNames.size(), 0);
  vector<int> nR(fitNames.size(), 0);

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      stringstream pss;
      pss << i;
      // first, check and delete any current hists
      histsX.push_back( CreateHist( "hXTempErr"+pss.str(), " ", 1000, -xBound, xBound) );
      histsY.push_back( CreateHist( "hYTempErr"+pss.str(), " ", 1000, -xBound, xBound) );
      histsZ.push_back( CreateHist( "hZTempErr"+pss.str(), " ", 1000, -xBound, xBound) );
      histsR.push_back( CreateHist( "hRTempErr"+pss.str(), " ", 1000, 0, rBound) );
    }
  
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
              double errX = fitPosition.X() - mcPosition.X();
              double errY = fitPosition.Y() - mcPosition.Y();
              double errZ = fitPosition.Z() - mcPosition.Z();
              double errR = (fitPosition - mcPosition).Mag();
              histsX[j]->Fill(errX);
              histsY[j]->Fill(errY);
              histsZ[j]->Fill(errZ);
              histsR[j]->Fill(errR);
              
              if(!sigma)
                {
                  if( fabs(errX) > err ) nX[j]++;
                  if( fabs(errY) > err ) nY[j]++;
                  if( fabs(errZ) > err ) nZ[j]++;
                  if( fabs(errR) > err ) nR[j]++;
                }

            }          

        }
      
    }

  cerr << endl;

  if(sigma)
    {
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
                  double errX = fitPosition.X() - mcPosition.X();
                  double errY = fitPosition.Y() - mcPosition.Y();
                  double errZ = fitPosition.Z() - mcPosition.Z();
                  double errR = (fitPosition - mcPosition).Mag();

                  if( errX > ( histsX[j]->GetMean() + histsX[j]->GetRMS() * err ) ||
                      errX < ( histsX[j]->GetMean() - histsX[j]->GetRMS() * err) )
                    nX[j]++;
                  if( errY > ( histsY[j]->GetMean() + histsY[j]->GetRMS() * err ) ||
                      errY < ( histsY[j]->GetMean() - histsY[j]->GetRMS() * err) )
                    nY[j]++;
                  if( errZ > ( histsZ[j]->GetMean() + histsZ[j]->GetRMS() * err ) ||
                      errZ < ( histsZ[j]->GetMean() - histsZ[j]->GetRMS() * err) )
                    nZ[j]++;
                  
                  // for R the issue is one sided
                  if( errR > ( histsR[j]->GetMean() + histsR[j]->GetRMS() * err ) ||
                      errR < ( histsR[j]->GetMean() - histsR[j]->GetRMS() * err) )
                    nR[j]++;
                }
              
            }

        }
      
    }

  cerr << endl;

  for(unsigned int i=0; i<fitNames.size(); i++)
    {
      cout << fitNames[i] << "\tX: " << nX[i] << "\tof " << histsX[i]->GetEntries() << "\t: " << nX[i] / histsX[i]->GetEntries()*100 << "%" << endl;
      cout << fitNames[i] << "\tY: " << nY[i] << "\tof " << histsY[i]->GetEntries() << "\t: " << nY[i] / histsY[i]->GetEntries()*100 << "%" << endl;
      cout << fitNames[i] << "\tZ: " << nZ[i] << "\tof " << histsZ[i]->GetEntries() << "\t: " << nZ[i] / histsZ[i]->GetEntries()*100 << "%" << endl;
      cout << fitNames[i] << "\tR: " << nR[i] << "\tof " << histsR[i]->GetEntries() << "\t: " << nR[i] / histsR[i]->GetEntries()*100 << "%" << endl;

      delete histsX[i];
      delete histsY[i];
      delete histsZ[i];
      delete histsR[i];

    }

}
