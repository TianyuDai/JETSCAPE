#include <iostream>
#include <fstream>
#include <memory>
#include <chrono>
#include <thread>

#include "gzstream.h"
#include "PartonShower.h"
#include "JetScapeLogger.h"
#include "JetScapePartonReader.h"
#include "JetScapeBanner.h"
#include "fjcore.hh"

#include <GTL/dfs.h>

using namespace std;
using namespace fjcore;

using namespace Jetscape;

// You could overload here and then simply use ofstream << p;
// ostream & operator<<(ostream & ostr, const fjcore::PseudoJet & jet);


// -------------------------------------

char* getCmdOptions(char ** begin, char ** end, const std::string & option)
{
	char ** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
		return *itr; 
	return 0; 
}

int main(int argc, char** argv)
{

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
  for (int i_task = 0; i_task < 100; i_task++)
  { 
  	double omega = i_task*0.1+0.1; 

  double jetpTMin = 20., jetRadius = 0.4, partonpTMin = 20.; 

  fjcore::JetDefinition jetDef(fjcore::antikt_algorithm, jetRadius); 
  vector <fjcore::PseudoJet> fjInputs; 
  fjcore::Selector select_pt = fjcore::SelectorPtMin(partonpTMin);

  //auto reader=make_shared<JetScapeReaderAscii>("omega"+std::to_string(omega)+".dat"); 
  auto reader=make_shared<JetScapeReaderAscii>("omega"+std::to_string(omega)+"-"+std::to_string(i_task)+".dat"); 
  //std::ofstream jet_output ("/global/cscratch1/sd/td115/output/"+job_name_str+"-"+job_id_str+"/omega"+std::to_string(omega)+".txt"); 
  std::ofstream jet_output ("/global/cscratch1/sd/td115/output/cutoff/omega"+std::to_string(omega)+".txt"); 
  while (!reader->Finished())
  { 
  	reader->Next();
  	fjInputs.resize(0);          
	vector <shared_ptr <Parton> > partons; 
	partons = reader->GetPartons(); 
	std::cout << "the number of partons is " << partons.size() << "\n"; 
	double energy = 0.; 
	for (unsigned int i = 0; i < partons.size(); i++)
	{
		if  (partons[i]->e() > energy && partons[i]->pid() == 21) energy = partons[i]->e(); 
	  	jet_output << energy << "\n"; 
	}
	reader->Close(); 
  }
}
}
