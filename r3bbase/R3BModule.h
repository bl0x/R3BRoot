#ifndef R3BMODULE_H
#define R3BMODULE_H 1


#include "FairModule.h"

#include "TObject.h"
#include "TVector3.h"
#include "TGeoMatrix.h"


class R3BModule : public FairModule
{
 public:

  /** Default constructor **/
  R3BModule();


  /** Constructor with arguments
   **/
  R3BModule(const char* Name, const char* title, Bool_t Active=kFALSE);

  /** Destructor **/
  virtual ~R3BModule();

  /** Transformation **/
  void SetRotAngles(Double_t phi, Double_t theta, Double_t psi);
  void SetTranslation(Double_t tx, Double_t ty, Double_t tz);

  TGeoRotation* GetGlobalRot(){ return fGlobalRot;}
  TGeoTranslation* GetGlobalTrans(){ return fGlobalTrans;}
  TGeoCombiTrans* GetGlobalPosition() {
      return ( new TGeoCombiTrans(*fGlobalTrans,*fGlobalRot));
  }

protected:
    TGeoRotation *fGlobalRot;
    TGeoTranslation *fGlobalTrans;


  ClassDef(R3BModule,1)

};

#endif