// ----------------------------------------------------------------
// -----            R3BPtofMapped2TCalPar (TAMEX)             -----
// -----                 Folder: R3BRoot/tof                  -----
// ----------------------------------------------------------------


#include "R3BPtofMapped2CalPar.h"
#include "R3BPaddleTamexMappedData.h"
#include "R3BEventHeader.h"
#include "R3BTCalPar.h"
#include "R3BTCalEngine.h"

#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunIdGenerator.h"
#include "FairRtdbRun.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TF1.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

/* ToDo:
 * 
 * We need to establish a new parameter container for the Ptof:
 * Open file R3BRoot/tcal/R3BTCalContFact.cxx
 * See how and where the containers are created
 * Add a container for the Ptof
 * 
 */

R3BPtofMapped2CalPar::R3BPtofMapped2CalPar()
    : FairTask("R3BPtofMapped2CalPar", 1)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fCal_Par(NULL)
{
}

R3BPtofMapped2CalPar::R3BPtofMapped2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fCal_Par(NULL)
{
}

R3BPtofMapped2CalPar::~R3BPtofMapped2CalPar()
{
// ToDo: Free the memory of the container and the engine
}
 
InitStatus R3BPtofMapped2CalPar::Init()
{
	// get access to the FairRootManager
	auto fairRootManager = FairRootManager::Instance();
	
	// get access to Mapped data
	auto fMappedItems = fairRootManager->GetObject("PtofMapped");
	if (!fMappedItems) {
		FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN,
		    "Branch PtofMapped not found");
		return kFATAL;
	}

	// get access to the parameter container
	fCal_Par = (R3BTCalPar *)FairRuntimeDb::instance()->
	    getContainer("PtofTCalPar");
	
	// tag the container as "changed" (to force writing to disk?)
	fCal_Par->setChanged();

	// create an instance of the TCalEngine
	fEngine = new R3BTCalEngine(fCal_Par, fMinStats);

	return kSUCCESS;
}

void R3BPtofMapped2CalPar::Exec(Option_t* option)
{
/* ToDo: Loop over all mapped items and fill the fine times into the
 * TCalEngine. That's all :-)
 * We do not need to care about the writing of the container to disk.
 * This is done in the macro.
 * 
 * 
 * Convert the following pseudo code into real C++:
 */


	// loop over all mapped items:
	auto entries = fMapped->GetEntries();
	for (int i = 0; i < entries; ++i)
	{
		// get pointers to the data we work with:
		auto mapped = (R3BPaddleTamexMappedData *)fMapped->At(i);

		// fetch plane and bar from mapped item for later usage:
		auto iPlane = mapped->GetPlaneId();
		auto iBar = mapped->GetBarId();

		// feel free to check plane and bar for reasonable values
		// ...

		// fill all four edges into the TcalEngine
		fEngine->Fill(iPlane, iBar, 0, mapped->GetFineTime(0, 0));
		fEngine->Fill(iPlane, iBar, 0, mapped->GetFineTime(1, 0));
		fEngine->Fill(iPlane, iBar, 1, mapped->GetFineTime(0, 1));
		fEngine->Fill(iPlane, iBar, 1, mapped->GetFineTime(1, 1));
	}
}

void R3BPtofMapped2CalPar::FinishEvent()
{
}

void R3BPtofMapped2CalPar::FinishTask()
{
	fEngine->CalculateParamVFTX();
	fCal_Par->printParams();
}

ClassImp(R3BPtofMapped2CalPar)

// ToDo: update CMakeLists.txt and TofLinkDef.h in the current folder
// (R3BRoot/tof)

