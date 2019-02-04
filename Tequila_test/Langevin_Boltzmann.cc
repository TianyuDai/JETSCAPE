#include <iostream>
#include <time.h>

// JetScape Framework includes ...
#include "JetScape.h"
#include "JetEnergyLoss.h"
#include "JetEnergyLossManager.h"
#include "JetScapeWriterStream.h"
#ifdef USE_HEPMC
#include "JetScapeWriterHepMC.h"
#endif


// User modules derived from jetscape framework clasess
#include "TrentoInitial.h"
#include "Tequila.h"
#include "Langevin.h"
#include "Martini.h"
#include "Brick.h"
#include "PGun.h"
#include "PythiaGun.h"
#include "PartonPrinter.h"
#include "HydroFromFile.h"

#include "tinyxml2.h"
#include <chrono>
#include <thread>

#define hbarc 0.197327053

using namespace std;

using namespace Jetscape;

// Forward declaration
void Show();

// -------------------------------------

void RunEvents(double scale, double alpha_s, double omegacut, int N)
{
	//modify the init.xml file
  	  // JetScapeXML::Instance()->OpenXMLFile("./Langevin_Boltzmann.xml");
	  JetScapeXML::Instance()->OpenXMLFile("./Langevin_Boltzmann.xml");
  	  tinyxml2::XMLElement *scalexml=JetScapeXML::Instance()->GetXMLRoot()->FirstChildElement("Eloss" )->FirstChildElement("Tequila" )->FirstChildElement("mu_scale"); 
  	  scalexml->SetText(scale);
  	  tinyxml2::XMLElement *alphaxml= JetScapeXML::Instance()->GetXMLRoot()->FirstChildElement("Eloss" )->FirstChildElement("Tequila" )->FirstChildElement("alpha_s");
  	  alphaxml->SetText(alpha_s);
  	  tinyxml2::XMLElement *omegaxml= JetScapeXML::Instance()->GetXMLRoot()->FirstChildElement("Eloss" )->FirstChildElement("Tequila" )->FirstChildElement("omega_over_T_cutoff");
  	  omegaxml->SetText(omegacut); 

  	  double t = 0.3*0.3/alpha_s/alpha_s; 
  	  tinyxml2::XMLElement *lenthxml= JetScapeXML::Instance()->GetXMLRoot()->FirstChildElement("Eloss" )->FirstChildElement("maxT" );
  	  lenthxml->SetText(t);
	  // JetScapeXML::Instance()->CloseXMLFile();
	  
	  double deltaT = 0.01; 
	  tinyxml2::XMLElement *dtxml= JetScapeXML::Instance()->GetXMLRoot()->FirstChildElement("Eloss" )->FirstChildElement("deltaT" );
  	  dtxml->SetText(deltaT);
  	  
	  // auto jetscape = make_shared<JetScape>("./Langevin_Boltzmann.xml",N);
	  auto jetscape = make_shared<JetScape>("./Langevin_Boltzmann.xml",N);
	  jetscape->SetReuseHydro(true); 
	  jetscape->SetNReuseHydro(1000000); 
	  // jetscape->SetId("primary");

	  // auto trento = make_shared<TrentoInitial>();
	  // auto pythiaGun= make_shared<PythiaGun> ();
	  auto pGun= make_shared<PGun> ();
	  auto hydro = make_shared<Brick> (); 
	  // auto hydro = make_shared<HydroFromFile> ();
	  jetscape->Add(pGun); 
	  jetscape->Add(hydro); 


	  // Energy loss
	  auto jlossmanager = make_shared<JetEnergyLossManager> ();
	  auto jloss = make_shared<JetEnergyLoss> ();

	  auto tequila = make_shared<Tequila> ();
	  jloss->Add(tequila);
	  jlossmanager->Add(jloss);  
	  jetscape->Add(jlossmanager);

	  auto printer = make_shared<PartonPrinter> ();
	  jetscape->Add(printer);
	  
	  // Output
	  auto writer= make_shared<JetScapeWriterAscii> ("test_out.dat");
	  jetscape->Add(writer); 

	  time_t start, init; 

	  jetscape->Init();

	  jetscape->Exec();

	  jetscape->Finish();
	  
	  writer->Close(); 
}

int main(int argc, char** argv)
{
  clock_t t; t = clock();
  time_t start, end; time(&start);

  JetScapeLogger::Instance()->SetInfo(true);
  JetScapeLogger::Instance()->SetDebug(false);
  JetScapeLogger::Instance()->SetRemark(false);
  JetScapeLogger::Instance()->SetVerboseLevel(0);
  
  Show();

  cout<<endl;
    
  double scale_list[1] = {2.}; 
  double alpha_list[1] = {0.32}; 
  double omegacut_list[1] = {1.}; 
  for (int i = 0; i < 1; i++)
  {
  	  
	  double scale = scale_list[i]; 
	  for (int j = 0; j < 1; j++)
	  {
	  	double alpha_s = alpha_list[j]; 
	  	for (int k = 0; k < 1; k++)
	  	{
	  		double omegacut = omegacut_list[k]; 
	  		RunEvents(scale, alpha_s, omegacut, 2); 
	  	}
	  }
  }
  
  INFO_NICE<<"Finished!"; 
  cout<<endl;

  t = clock() - t;
  time(&end);
  printf ("CPU time: %f seconds.\n",((float)t)/CLOCKS_PER_SEC);
  printf ("Real time: %f seconds.\n",difftime(end,start));

  return 0;
}

// -------------------------------------

void Show()
{
  INFO_NICE<<"------------------------------------";
  INFO_NICE<<"| Brick Test JetScape Framework ... |";
  INFO_NICE<<"------------------------------------";
  INFO_NICE;
}
