// R3BsfiDigitizer.cxx 25. 08. 2017.
// Created by: Aron Kripko

#include "R3BsfiDigitizer.h"
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"


// includes for modeling
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"
#include "TGeoShapeAssembly.h"


#include "TVector3.h"
#include "TMath.h"
#include "TRandom.h"
#include "TH1F.h"
#include "TH2F.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>


#include "R3BFi4Point.h"
#include "R3BMCTrack.h"
		
using std::cout;
using std::endl;
		

R3BsfiDigitizer::R3BsfiDigitizer() :
  FairTask("R3B sfi Digitization scheme ") 
  { 
	  
	esigma = 0.001;
	tsigma = 0.01;
	NumOfFibers = 512;
	  
}

R3BsfiDigitizer::R3BsfiDigitizer(Double_t e, Double_t t) :
  FairTask("R3B sfi Digitization scheme ") 
  { 
	  
	esigma = e;
	tsigma = t;
	NumOfFibers = 512;
	  
}


R3BsfiDigitizer::~R3BsfiDigitizer() {
}


void R3BsfiDigitizer::SetEnergyResolution(Double_t e)
{
	esigma = e;
}

void R3BsfiDigitizer::SetTimeResolution(Double_t t)
{
	tsigma = t;
}

InitStatus R3BsfiDigitizer::Init() {

  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fPoints = (TClonesArray*) ioman->GetObject("SFIPoint");
  
  fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
   
  // Register output array DchDigi
  Hits = new TClonesArray("R3BFi4HitItem",1000);
  ioman->Register("sfiHit", "Digital response in sfi", Hits, kTRUE);
  
  //sigmas
  prnd = new TRandom3();
  
  
  return kSUCCESS;

}


void R3BsfiDigitizer::Exec(Option_t* opt)
{
   Reset();

	   Int_t entryNum = fPoints -> GetEntries();
	   
	   if (!entryNum) return;
	   
	   //creating the storage for energy and time for each PMT
	   // currently there is no difference between the two sides (no propagation in this version)
	   
	   std::vector<Double_t> energy_l [NumOfFibers/2];
	   
	   //std::vector<Double_t> energy_r [NumOfFibers/2];
	   
	   std::vector<Double_t> energy_t [NumOfFibers/2];
	   
	   //std::vector<Double_t> energy_b [NumOfFibers/2];
	   
	   std::vector<Double_t> time_l [NumOfFibers/2];
	   
	   //std::vector<Double_t> time_r [NumOfFibers/2];
	   
	   std::vector<Double_t> time_to [NumOfFibers/2];
	   
	   //std::vector<Double_t> time_b [NumOfFibers/2];
	   
	   
	   for(Int_t i = 0; i < NumOfFibers/2; ++i)
	   {
		   time_l[i].push_back(99999);
		   time_to[i].push_back(99999);
		   
		   energy_l[i].push_back(0);
		   energy_t[i].push_back(0);

	   }
	   
	   struct TempHit
	   {
		   Int_t fiberID;
		   Double_t Energy;
		   Double_t Time;
		   
		   TempHit(Int_t id, Double_t e, Double_t t):fiberID(id),Energy(e),Time(t)
		   {
		   }
	   };
	   
	   //ordering the hits in time
	   
	   std::vector<TempHit> TempHits;
	   
	   for (Int_t i = 0; i < entryNum; ++i)
	   {
		   R3BFi4Point * data_element = (R3BFi4Point *) fPoints -> At(i);
		   
		   TempHits.push_back(TempHit(data_element -> GetDetectorID(), data_element -> GetEnergyLoss(), data_element -> GetTime()));
	   }
	   
	   std::sort(TempHits.begin(), TempHits.end(), [](const TempHit& lhs, const TempHit& rhs){ return lhs.Time < rhs.Time; });
	   
	   //registering the hits in the PMTs
	   
	   for (TempHit& Hit : TempHits)
	   {
		   if(Hit.Energy < 0.0015)
		   {
			   continue;
		   }
		   
		   Int_t fiberID = Hit.fiberID;
		   
		   if(fiberID < NumOfFibers/2)
		   {
			   if(Hit.Time - time_l[fiberID].back() < 30)
			   {	   		   
					energy_l[fiberID].back() += Hit.Energy;
					time_l[fiberID].back() = (time_l[fiberID].back() > Hit.Time) ? Hit.Time : time_l[fiberID].back();
			   }
			   else
			   {
				   energy_l[fiberID].push_back(Hit.Energy);
				   time_l[fiberID].push_back(Hit.Time);
			   }
		   }
		   else
		   {
			   if(Hit.Time - time_to[fiberID-NumOfFibers/2].back() < 30)
			   {	   		   
					energy_t[fiberID-NumOfFibers/2].back() += Hit.Energy;
					time_to[fiberID-NumOfFibers/2].back() = (time_to[fiberID-NumOfFibers/2].back() > Hit.Time) ? Hit.Time : time_to[fiberID-NumOfFibers/2].back();
			   }
			   else
			   {
				   energy_t[fiberID-NumOfFibers/2].push_back(Hit.Energy);
				   time_to[fiberID-NumOfFibers/2].push_back(Hit.Time);
			   }
		   }
		   
		}
		
		//creating the final hits

	   for (Int_t i =0; i < NumOfFibers/2; ++i)
	   {
		    for(Double_t& energyl : energy_l[i])
		    {
				if(energyl > 0.0015)
				{
					for (Int_t j =0; j < NumOfFibers/2; ++j)
					{
						for(Double_t& energyt : energy_t[j])
							{
								if(energyt > 0.0015)
								{
									new ((*Hits) [Hits->GetEntries()]) R3BFi4HitItem((UInt_t) i ,(UInt_t) j, (i + .5 - NumOfFibers/4) * 0.02, (j + .5 - NumOfFibers/4) * 0.02, 0, prnd->Gaus(energyl, esigma), prnd->Gaus(energyt, esigma), prnd->Gaus(time_l[i].at(&energyl-energy_l[i].data()), tsigma), prnd->Gaus(time_to[j].at(&energyt-energy_t[j].data()), tsigma));
								}
							}
						}
				}
		}
	}
		
}
// -------------------------------------------------------------------------

void R3BsfiDigitizer::Reset()
{
 if (Hits) Hits->Clear();
}   

void R3BsfiDigitizer::Finish()
{
}

ClassImp(R3BsfiDigitizer)
