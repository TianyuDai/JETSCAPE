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

int main(int argc, char** argv)
{
  double scale_list[1] = {1.}; 
  double alpha_list[1] = {0.3}; 
  double omegacut_list[1] = {2.}; 

  double jetpTMin = 20., jetRadius = 0.4, partonpTMin = 20.; 

  fjcore::JetDefinition jetDef(fjcore::antikt_algorithm, jetRadius); 
  vector <fjcore::PseudoJet> fjInputs; 
  fjcore::Selector select_pt = fjcore::SelectorPtMin(partonpTMin);

  for (int ii = 0; ii < 1; ii++)
  {
  	  
	  double scale = scale_list[ii]; 
	  for (int jj = 0; jj < 2; jj++)
	  {
	  	double alpha_s = alpha_list[jj]; 
		for (int kk = 0; kk < 1; kk++)
		{
		  double omegacut = omegacut_list[kk]; 
		  auto reader=make_shared<JetScapeReaderAscii>("test_out.dat"); 
		  std::ofstream jet_output (("200GeV_gluon_muscale"+std::to_string(scale)+"alpha"+std::to_string(alpha_s)+"omega"+std::to_string(omegacut)+"_qgpbrick_elas_diffusion.txt").c_str()); 

		  while (!reader->Finished())
			{ 
			  reader->Next();
			  fjInputs.resize(0);          
		  	  vector <fjcore::PseudoJet> inclusiveJets, sortedJets; 
		  	  fjcore::ClusterSequence clustSeq(reader->GetPartonsForFastJet(), jetDef); 
   		  	  inclusiveJets = clustSeq.inclusive_jets(jetpTMin); 
			  vector <fjcore::PseudoJet> selected_jets = select_pt(inclusiveJets); 
		  	  sortedJets = fjcore::sorted_by_pt(selected_jets);
			  vector <shared_ptr <Parton> > partons; 
			  partons = reader->GetPartons(); 
			  double energy = 0.; 
			  for (unsigned int i = 0; i < partons.size(); i++)
			  // 	if  (sortedJets[i].e() > energy) energy = sortedJets[i].e(); 
			   	if  (partons[i]->e() > energy && partons[i]->pid() == 21) energy = partons[i]->e(); 
			  jet_output << energy << "\n"; 
			 }
		  reader->Close(); 
		}
	}
  }
}
