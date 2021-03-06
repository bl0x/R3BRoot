r3bevtvis()
{
  FairRunAna *fRun= new FairRunAna();
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open("r3bpar.root");
  rtdb->setFirstInput(parIo1);
  rtdb->print();
  
  fRun->SetInputFile("r3bsim.root");
  fRun->SetOutputFile("test.root");
  
  R3BEventManager *fMan= new R3BEventManager();
  R3BMCTracks *Track =  new R3BMCTracks ("Monte-Carlo Tracks");

  R3BCaloEventDisplay *CaloEvtVis = new R3BCaloEventDisplay("R3BCaloEventDisplay");
  R3BCaloHitEventDisplay *CaloHitEvtVis = new R3BCaloHitEventDisplay("R3BCaloHitEventDisplay");

  //Select the Geometry Version
  CaloEvtVis->SelectGeometryVersion(10);

  fMan->AddTask(CaloEvtVis);
  fMan->AddTask(CaloHitEvtVis);  //Make sure that you have CaloHit branch in the file

  fMan->AddTask(Track);
  
  fMan->Init();
}
