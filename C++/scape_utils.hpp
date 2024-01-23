 /**
  * @file  	scape_utils.hpp
  * @date 	Aug 14, 2019
  * @author Eduardo Luis Rhod 
 */

#ifndef SCAPEUTILSHPP
#define SCAPEUTILSHPP

#include "types.hpp"
#include "debug.hpp"
	
/**
  * @brief          Evaluates look-up table function.
  * @param  G       Pointer to a MyGraphType which describes the Graph structure in 
  *                    	the form of a adjacency_list.
  * @param  elem	Element number.
  * @param  func	Look-up table function.
  * @param  lutsize	Look-up table size.
  * 
  * @return 		The function evaluation result.
  *
  */
double eval_function (MyGraphType& G, int elem, string func, int lutsize); 

/**
  * @brief          Evaluates the correlated function.
  * @param  G       Pointer to a MyGraphType which describes the Graph structure in 
  *                    	the form of a adjacency_list.
  * @param  elem	Element number.
  * @param  func	Look-up table function.
  * @param  lutsize	Look-up table size.
  * 
  * @return 		The function evaluation result.
  *
  */
double eval_correlated_function (MyGraphType& G, int elem, string func, int lutsize);

/**
  * @brief          	Find Look-up tables that have the same input.
  * @param  G       	Pointer to a MyGraphType which describes the Graph structure in 
  *                 	   	the form of a adjacency_list.
  * @param  lutsize_map	Map with all luts and the sucessor components.
  * 
  * @return 			Vector with the look-up table numbers that have the same inputs.
  *
  */
vector<pair<int, int>> find_same_input_luts(MyGraphType& G, map<int,vector<int>> lutsize_map);

/**
  * @brief          			Get the probability map and checks if they are all valid.
  * @param  G       			Pointer to a MyGraphType which describes the Graph structure in 
  *                 			   	the form of a adjacency_list.
  * @param  vertex				Vertex number.
  * @param  imap				Map of all components and its error probability value.
  * @param  out_flag			Indicates if it is a memory element.
  * @param  correlated_value	Correlated value of the element.
  * 
  * @return 		True if all probabilities are valid and false if not.
  *
  */
bool get_input_fp_map (MyGraphType& G, int vertex, map <string, errorProbabilityType>& imap, int out_flag, map <string,vector<pair<double,errorValuesModel>>>& correlated_value);

/**
  * @brief          	Converts a primitive string to its enumeration correspondent.
  * @param  st       	String that is being converted.
  * 
  * @return				Returns the enumeration correspondent to the primitive.
  * 
  */  
enum PrimEnum convert_to_enum (string st);

/**
  * @brief          	Calculates the Multiplexer error probability.
  * @param  i0       	Input0 error probality.
  * @param  i1       	Input1 error probality.
  * @param  sel       	Selection error probality.
  * @param  out       	Resultant error probability.
  * 
  */  
void calc_mux_error_probability (errorProbabilityType i0, errorProbabilityType i1, errorProbabilityType sel, errorProbabilityType& out );

/**
  * @brief          	Calculates the Exclusive Or error probability.
  * @param  i0       	Input0 error probality.
  * @param  i1       	Input1 error probality.
  * @param  out       	Resultant error probability.
  * 
  */
void calc_xor_error_probability (errorProbabilityType i0, errorProbabilityType i1, errorProbabilityType& out );

/**
  * @brief          		Try to find a path between source and target.
  * @param  error_source    Source element.
  * @param  target       	Target element.
  * 
  * @return         	Resultant true if there is a path and false if there is not.
  * 
  */	
bool there_is_path(MyGraphType& G, int error_source, int target);

/**
  * @brief          			Merge two correlated values.
  * @param  correlated_value1   Correlated value 1.
  * @param  correlated_value2   Correlated value 2.
  * @param  merged		       	Merged correlated value.
  * 
  */
void merge_correlated_inputs(vector<pair<double,errorValuesModel>> correlated_value1, vector<pair<double,errorValuesModel>> correlated_value2, vector<double>& merged);

/**
  * @brief          				Calculates correlated probability.
  * @param  G       				Pointer to a MyGraphType which describes the Graph structure in 
  *                 				   	the form of a adjacency_list.
  * @param  elem					Element or component that is being calculated.
  * @param  p0w						Probability of a wrong zero.
  * @param  p1w						Probability of a wrong one.
  * @param  p0c						Probability of a correct one.
  * @param  p1c						Probability of a correct one.
  * @param  merged_correlation		Merged correlated vector.
  * @param  convergent_destination	Map with convergent destination.
  * 
  */
void calculate_correlated_probability(MyGraphType& G, int elem, double& p0w, double& p1w, double& p0c, double& p1c, vector<double> merged_correlation,map<int,vector<string>> convergent_destination);

/**
  * @brief          	Generates the correlated look-up table not minimized probability function.
  * @param  G       	Pointer to a MyGraphType which describes the Graph structure in 
  *                 	   	the form of a adjacency_list.
  * @param  func		Look-up table function.
  * @param  lutsize		Look-up table size.
  * 
  * @return 			Vector with correlated LUT probability function not minimized.
  */
vector<string> gen_correlated_lut_prob_func_not_minimized (long long int func, int lutsize);

/**
  * @brief          	Returns the vector with the first lut in the path
  * @param  G       	Pointer to a MyGraphType which describes the Graph structure in 
  *                 	   	the form of a adjacency_list.
  * @param  path		Vector with path to search.
  * 
  * @return 			Returned path that found a LUT.
  */
vertexDescType get_first_lut_in_vector(MyGraphType G, vector<vertexDescType> path);

/**
  * @brief          							Sets the cycle that there is activation of the correlation
  * @param  G       							Pointer to a MyGraphType which describes the Graph structure in 
  *                 							   	the form of a adjacency_list.
  * @param  path								Vector with path to search.
  * @param  cycle_of_correlation_activation		Map with all correlation activations.
  * @param  dest								Destination element.
  * 
  */
void set_cycle_of_correlation_activation(MyGraphType G, vector<vertexDescType> path, map<vertexDescType,int>& cycle_of_correlation_activation, vertexDescType dest); 

#endif
