#ifndef R3BPTOFREADER_H
#define R3BPTOFREADER_H

#include "R3BReader.h"

struct EXT_STR_h101_PTOF_t;
typedef struct EXT_STR_h101_PTOF_t EXT_STR_h101_PTOF;
typedef struct EXT_STR_h101_PTOF_onion_t EXT_STR_h101_PTOF_onion;
class FairLogger;

class R3BPtofReader : public R3BReader
{
	public:
		R3BPtofReader(EXT_STR_h101_PTOF *, UInt_t);
		~R3BPtofReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_PTOF* fData;
		/* Data offset */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger*	fLogger;
		/* the structs of type R3BPtofxMappedItem */
		TClonesArray* fArray; /**< Output array. */

		Bool_t ReadLeadingEdges(EXT_STR_h101_PTOF_onion *, int, int);
		Bool_t ReadTrailingEdges(EXT_STR_h101_PTOF_onion *, int, int);
		Bool_t ReadLeadingEdgeChannel(EXT_STR_h101_PTOF_onion *, int,
		    int, uint32_t, int);
		Bool_t ReadTrailingEdgeChannel(EXT_STR_h101_PTOF_onion *, int,
		    int, uint32_t, int);

	public:
		ClassDef(R3BPtofReader, 0);
};

#if 0
/* Structure layout from ext_h101_ptof.h */
struct {
	struct {
		uint32_t TFLM;
		uint32_t TFLMI[4 /* TFLM */];
		uint32_t TFLME[4 /* TFLM */];
		uint32_t TFL;
		uint32_t TFLv[40 /* TFL */];
		uint32_t TFTM;
		uint32_t TFTMI[4 /* TFTM */];
		uint32_t TFTME[4 /* TFTM */];
		uint32_t TFT;
		uint32_t TFTv[40 /* TFT */];
		uint32_t TCLM;
		uint32_t TCLMI[4 /* TCLM */];
		uint32_t TCLME[4 /* TCLM */];
		uint32_t TCL;
		uint32_t TCLv[40 /* TCL */];
		uint32_t TCTM;
		uint32_t TCTMI[4 /* TCTM */];
		uint32_t TCTME[4 /* TCTM */];
		uint32_t TCT;
		uint32_t TCTv[40 /* TCT */];
	} T[2];
} PTOF_P[1];


#endif

#endif

