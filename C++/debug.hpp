 /**
  * @file  debug.hpp
  * @date Jul 24, 2019
  * @author Eduardo Luis Rhod 
 */

#ifndef DEBUGHPP
#define DEBUGHPP
	
#include "types.hpp"

/**
  * @brief  Clears the screen to restart the information printing.
  *  
  */ 
void clrscr();

/**
  * @brief                 Prints the current state of the fault propagation simulation.      
  * @param  G              Pointer to a MyGraphType which describes the Graph structure in 
  *                        		the form of a adjacency_list.    
  * @param  cif            Circuit information structure.
  * @param  cycle          Number of the current running cycle.
  * @param  watch_list     List with the components or nets that will be printed.
  *
  */ 
void run_print(MyGraphType G, circuitInfo cif, int cycle, vector<int> watch_list);

/**
  * @brief                 	Prints the id to node vector.      
  * @param  v              	Vector of Ids.
  * @param  st          	String that will precede the id to node vector value.
  *
  */ 
void print_vector (vector<int> v, string st);

/**
  * @brief                 	Prints the probability of error map     
  * @param  iMap            A map with all paths from sources to destination in vectors of vectors
  *
  */	
void print_map (map<string,errorProbabilityType> iMap);

/**
  * @brief             		Generates the watchlist based on the graph G.
  * @param  G              	Pointer to a MyGraphType which describes the Graph structure in 
  *                        		the form of a adjacency_list.  
  * @param  source          The error source.
  * @param  target          The error propagation target component.
  * 
  * @return 				The watchlist vector.
  * 
  */	
vector<int> generate_watchlist(MyGraphType G, int source, int target);

/**
  * @brief          	   		Prints the error probability of a given edge.
  * @param  st		            The output edge (vertex) that is being printed.
  * @param  p0wrong             Probability of the edge having a zero value that is wrong.
  * @param  p1wrong             Probability of the edge having a one value that is wrong.
  * @param  p0correct           Probability of the edge having a zero value that is correct.
  * @param  p1correct           Probability of the edge having a one value that is correct.
  * 
  */
void print_out_edge (string st ,double p0wrong, double p1wrong, double p0correct, double p1correct );
#endif
