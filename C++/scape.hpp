 /**
  * @file   scape.cpp
  * @date   Jun 10, 2019
  * @author Eduardo Luis Rhod 
 */

#ifndef SCAPEHPP
#define SCAPEHPP
#include "types.hpp"

/* Function Prototypes */
/**
  * @brief                      Sets the error probability values of an output edge. 
  * @param  G                   Pointer to a MyGraphType which describes the Graph structure in 
  *                                 the form of a adjacency_list.
  * @param  vertex              The vertex that is being set.
  * @param  p0wrong             Probability of the edge having a zero value that is wrong.
  * @param  p1wrong             Probability of the edge having a one value that is wrong.
  * @param  p0correct           Probability of the edge having a zero value that is correct.
  * @param  p1correct           Probability of the edge having a one value that is correct.
  * @param  st                  Indicates the type of egde that is being set.
  * @param  omap                The map that holds the ouput probability
  * @param  correlated_value    Vector of correlation values
  *
  */
void set_out_edge_error_probability_values (MyGraphType& G, int vertex, double p0wrong, double p1wrong, double p0correct, double p1correct, string st , map<string,errorProbabilityType> omap, vector<pair<double,errorValuesModel>> correlated_value);

/**
  * @brief                    Calculates the probability of a look-up table to filter a fault.   
  * @param  func              The look-up table function.
  * @param  lut_size          The look-up table size.
  *
  * @return The calculate probability of the lut to filter faults.
  */
vector<float> calc_lut_filter_probability (unsigned long long int func, int lut_size);

/**
  * @brief                      Used to set the same value for all the graph for a given attribute name. 
  * @param  G                   Pointer to a MyGraphType which describes the Graph structure in 
  *                                 the form of a adjacency_list.
  * @param  value               The value to be set.
  * @param  attribute_name      The attribute name that is being set.
  *
  */ 
void set_all_edges_attribute (MyGraphType& G, anyType value, enum EdgeAtribTypeEnum attribute_name);

/**
  * @brief                      Initialize the error probability. This is used to start the fault injection process
  * @param  G                   Pointer to a MyGraphType which describes the Graph structure in 
  *                                 the form of a adjacency_list.
  * @param  error_source        The error source number that is being initialized.              
  *
  */
void error_prob_init (MyGraphType& G, int error_source);

/**
  * @brief                      Calculates the output probability of error.
  * @param  G                   Pointer to a MyGraphType which describes the Graph structure in 
  *                                 the form of a adjacency_list.
  * @param  error_source        The error source number that is being used in the calculation            
  * @param  faulty_list         list of components that are fault sensitive.
  * @param  correlation_Info    Correlation information for the error source with the reconvergence path and correlated sources.
  * @param  current_cycle       Number of the current running cycle.
  *            
  */
void calculate_output_probabilities(MyGraphType& G, int error_source, vector<int> faulty_list, correlationInfo correlation_Info, int current_cycle);

 /**
  * @brief                      Run the error propagation for the combinational part 
  * @param  G                   Pointer to a MyGraphType which describes the Graph structure in 
  *                                 the form of a adjacency_list.
  * @param  error_source                    The error source number.    
  * @param  cif                             Circuit information structure.
  * @param  is_first                        Indicates if it is the first cycle.
  * @param  current_sequential_fp_sources   The vector that holds the current sequential fault probability sources
  * @param  correlation_Info                Correlation information for the error source with the reconvergence path and correlated sources.
  * @param  current_cycle                   Number of the current running cycle.
  *
  */ 
void run_combinational_propagation(MyGraphType& G, int error_source, circuitInfo cif, bool is_first, vector<int> current_sequential_fp_sources, correlationInfo correlation_Info, int current_cycle);

/**
  * @brief                      Run the error propagation for the sequential part 
  * @param  G                   Pointer to a MyGraphType which describes the Graph structure in 
  *                                 the form of a adjacency_list.
  * @param  error_source                    The error source number.    
  * @param  sequential_list                 List with the sequential components.
  * @param  correlation_Info                Correlation information for the error source with the reconvergence path and correlated sources.
  *
  * @return                     Current probability of faults for sequential components.
  */
vector<int> run_sequential_propagation(MyGraphType& G, int error_source, vector<int> sequential_list, correlationInfo correlation_Info);

/**
  * @brief                      Run the error propagation 
  * @param  G                   Pointer to a MyGraphType which describes the Graph structure in 
  *                                 the form of a adjacency_list.
  * @param  error_source                    The error source number.  
  * @param  cif                             Circuit information structure.
  * @param  report                          report structure that will be updated with the resulting error propagation.   
  * @param  watch_list                      List with the signals that are being printed.
  * @param  ins                             Structure with the program options chosen by the user.
  * @param  correlation_Info                Correlation information for the error source with the reconvergence path and correlated sources.
  *
  */
void run_error_propagation(MyGraphType& G, int error_source, circuitInfo cif, map<int, vector<errorProbabilityType>>& report, vector<int> watch_list, programOptions ins, correlationInfo correlation_Info);

/**
  * @brief                      Initializes the components that have a constant probability of error propagation. 
  * @param  G                   Pointer to a MyGraphType which describes the Graph structure in 
  *                                 the form of a adjacency_list.
  * @param  inPorts             Vector of input ports.     
  * @param  sequential_list     List with the sequential components.
  *
  */
void constant_probability_init(MyGraphType& G, vector<int> inPorts, vector<int> sequential_list);

/**
  * @brief                      Thread version of the error propagation function
  * @param  arg                 Pointer to thread arguments 
  * 
  */
void* run_error_propagation_multithread(void* arg);

/**
  * @brief                      Run the error propagation using multiple processors with threads
  * @param  G                   Pointer to a MyGraphType which describes the Graph structure in 
  *                                 the form of a adjacency_list.
  * @param  nodeMap             The vertices map  
  * @param  ins                 Structure with the program options chosen by the user.
  * @param  cif                 Circuit information structure.
  * 
  */
void run_multiproc (MyGraphType G, nodeMapType nodeMap, programOptions ins, circuitInfo cif);

#endif
 
