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
#include <algorithm> 
#include <string>

#define hbarc 0.197327053

using namespace std;

using namespace Jetscape;

// Forward declaration
void Show();

// -------------------------------------

char* getCmdOptions(char ** begin, char ** end, const std::string & option)
{
	char ** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
		return *itr; 
	return 0; 
}

void RunEvents(double scale, double alpha_s, double omega, int N, std::string task_str)
{
	//modify the init.xml file
  	  // JetScapeXML::Instance()->OpenXMLFile("./Langevin_Boltzmann.xml");
	  JetScapeXML::Instance()->OpenXMLFile("./Langevin_Boltzmann.xml");
  	  tinyxml2::XMLElement *scalexml=JetScapeXML::Instance()->GetXMLRoot()->FirstChildElement("Eloss" )->FirstChildElement("Tequila" )->FirstChildElement("mu_scale"); 
  	  scalexml->SetText(scale);
  	  tinyxml2::XMLElement *alphaxml= JetScapeXML::Instance()->GetXMLRoot()->FirstChildElement("Eloss" )->FirstChildElement("Tequila" )->FirstChildElement("alpha_s");
  	  alphaxml->SetText(alpha_s);
  	  tinyxml2::XMLElement *omegaxml= JetScapeXML::Instance()->GetXMLRoot()->FirstChildElement("Eloss" )->FirstChildElement("Tequila" )->FirstChildElement("omega_over_T_cutoff");
  	  omegaxml->SetText(omega); 

  	  double t = 0.3*0.3/alpha_s/alpha_s; 
  	  tinyxml2::XMLElement *lenthxml= JetScapeXML::Instance()->GetXMLRoot()->FirstChildElement("Eloss" )->FirstChildElement("maxT" );
  	  lenthxml->SetText(t);
	  // JetScapeXML::Instance()->CloseXMLFile();
	  
	  double deltaT = 0.01; 
	  tinyxml2::XMLElement *dtxml= JetScapeXML::Instance()->GetXMLRoot()->FirstChildElement("Eloss" )->FirstChildElement("deltaT" );
  	  dtxml->SetText(deltaT);
  	  
	  // auto jetscape = make_shared<JetScape>("./Langevin_Boltzmann.xml",N);
	  auto jetscape = make_shared<JetScape>("./cutoff_test.xml",N);
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
	  auto writer= make_shared<JetScapeWriterAscii> ("omega"+std::to_string(omega)+".dat");
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
    
  char * task = getCmdOptions(argv, argv+argc, "--task"); 
  std::string task_str = ""; 
  int i_task = 0; 
  if (task != NULL) {task_str = task; i_task = std::stoi(task_str);} 

  char * job_name = getCmdOptions(argv, argv+argc, "--job-name"); 
  std::string job_name_str = ""; 
  if (job_name != NULL) job_name_str = job_name; 

  char * job_id = getCmdOptions(argv, argv+argc, "--job-id"); 
  std::string job_id_str = ""; 
  if (job_id != NULL) job_id_str = job_id; 

  double scale = 1.; 
  double alpha = 0.3; 
  double omega = i_task*0.1+0.1;

  RunEvents(scale, alpha, omega, 100000, task_str); 
  
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
