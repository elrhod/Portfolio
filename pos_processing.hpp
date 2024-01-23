 /**
  * @file  	pos_processing.hpp
  * @date 	Aug 29, 2019
  * @author Eduardo Luis Rhod 
 */
 
 #ifndef POSPROCESSINGHPP
    #define POSPROCESSINGHPP
	
	#include "types.hpp"
	
/**
  * @brief                      Update the report.
  * @param  G                   Pointer to a MyGraphType which describes the Graph structure in 
  *                                 the form of a adjacency_list.
  * @param  outPorts            Output ports or nets that will be shown in the report.
  * @param  error_source        The error source of the simulation.
  * @param  cycle             	Cycle of the report.
  * @param  report             	Structure with the metadata of the report.
  *
  */
	void update_report (MyGraphType G, vector<int> outPorts, int error_source, int cycle, map<int,vector<errorProbabilityType>>& report);

/**
  * @brief                      Generate the output file with the report.
  * @param  error_source        The error source of the simulation.
  * @param  output             	Name of the report output file
  * @param  report             	Structure with the metadata of the report.
  *
  */
	void generate_report (string error_source, string output, map< int, vector<errorProbabilityType> > report);

#endif
