 /**
  * @file  	pre_processing.hpp
  * @date 	Aug 14, 2019
  * @author Eduardo Luis Rhod 
 */

#ifndef PREPROCESSINGHPP
    #define PREPROCESSINGHPP
	
	#include "types.hpp"
	#include "tools.hpp"
	#include <fstream>

/**
  * @brief                      Process one line of the file with the extracted circuit.
  * @param  line                The line that is being processed.
  * @param  nodeMap             Node map that is created that hold all instances of components using strings.
  * @param  edgeMap        		Edge map that is created that hold all instances of components using integers (for performance).
  * @param  outPorts            Vector with all mapped output ports of the circuit.
  * @param  inPorts             Vector with all mapped input ports of the circuit.
  *
  */
	int processLine (string line, nodeMapType& nodeMap, edgeMapType& edgeMap, vector<int>& outPorts, vector<int>& inPorts);

/**
  * @brief              Process the user imput arguments.
  * @param  argc        Number of paramenters pointed by argv.
  * @param  argv        Pointer to the parameters passed by the user.
  * @param  ins       	Input parameters struct constructed by the argument processed.
  * 
  * @return 			Returns the success status of the program options parsing.
  *
  */
	bool process_command_line (int argc, char** argv, programOptions& ins);

/**
  * @brief              Reads and process the file and generates the correspondent circuit in a graph format
  * @param  input       Input file name.
  * @param  nodeMap     Node map that is created that hold all instances of components using strings.
  * @param  edgeMap     Edge map that is created that hold all instances of components using integers (for performance).
  * @param  id2node     Map with id to node correspondence.
  * @param  outPorts    Vector with all mapped output ports of the circuit.
  * @param  inPorts     Vector with all mapped input ports of the circuit.
  * 
  * @return 			The generated graph.
  *
  */
 	MyGraphType file_to_graph(string input, nodeMapType& nodeMap, edgeMapType& edgeMap, map <int, string>& id2node, 
    	vector<int>& outPorts, vector<int>& inPorts);

/**
  * @brief              	Reads and process the file and generates the correspondent circuit in a graph format
  * @param  G           	Pointer to a MyGraphType which describes the Graph structure in 
  *                         	the form of a adjacency_list.
  * @param  starting_node	Starting node the discover the combinational cone.
  * 
  * @return 				The generated combinational cone.
  *
  */
	vector<int> get_combinational_cone (MyGraphType G, int starting_node);

/**
  * @brief              				Reads and process the file and generates the correspondent circuit in a graph format
  * @param  G           				Pointer to a MyGraphType which describes the Graph structure in 
  *                         				the form of a adjacency_list.
  * @param  starting_node				Starting node the discover the combinational cone.
  * @param  global_combinational_size	Size of the biggest combinational path.
  * @param  nodeMap     				Node map that is created that hold all instances of components using strings.
  * @param  input						Starting node the discover the combinational cone.
  *
  */
	void generate_combinational_path_order (MyGraphType G, string starting_node, int& global_combinational_size, nodeMapType& nodeMap, string input); 

/**
  * @brief		Reverse a bitset.
  * @param  b	The bitset that will be reversed.
  *
  */
	void reverse(std::bitset<N> &b)

/**
  * @brief			Generates the non-minimized look-up table probability of error function.
  * @param  func	The source lut function.
  * @param  lutsize	The size of the lut.
  *
  * @return			The probability of error look-up table function.
  */
	pair<pair<string,string>,pair<string,string>> gen_lut_prob_func_not_minimized (long long int func, int lutsize); 

/**
  * @brief			Generates the look-up table probability of error functions for all the graph.
  * @param  G       Pointer to a MyGraphType which describes the Graph structure in 
  *                 	the form of a adjacency_list.
  * @param  nodeMap Node map that is created that hold all instances of components using strings.
  * 
  */
	void generate_LUT_probability_of_error_functions (MyGraphType& G, nodeMapType& nodeMap); 

/**
  * @brief			Get all paths frrom source to target.
  * @param  G       Pointer to a MyGraphType which describes the Graph structure in 
  *                 	the form of a adjacency_list.
  * @param  source  Starting node.
  * @param  targets Target node.
  * 
  * @return  		All the paths between the source and target nodes.
  * 
  */
	map<vertexDescType, vector<vector<vertexDescType>>>  get_all_paths (MyGraphType G, vertexDescType source, vector<vertexDescType> targets);

/**
  * @brief						Run the preprocessing steps.
  * @param  G       			Pointer to a MyGraphType which describes the Graph structure in 
  *                 				the form of a adjacency_list.
  * @param  prog_opt 			User program options.
  * @param  nodeMap 			Node map that is created that hold all instances of components using strings.
  * @param  id2node 			Struct that maps Ids to Nodes.
  * @param  cif                 Circuit information structure.
  * @param  correlation_Info    Correlation information for the error source with the reconvergence path and correlated sources.
  * 
  */
	void run_preprocessing (MyGraphType& G, programOptions prog_opt, nodeMapType& nodeMap, map <int, string> id2node, circuitInfo& cif, 
    correlationInfo& correlation_Info );

/**
  * @brief								Discovers the combinational path position for each combinational component .
  * @param  G       					Pointer to a MyGraphType which describes the Graph structure in 
  *                 						the form of a adjacency_list.
  * @param  global_combinational_size	User program options.
  * @param  nodeMap 					Node map that is created that hold all instances of components using strings.
  * @param  input 						Preprocessing input file.
  * 
  */
	void load_combinational_path_positions (MyGraphType& G, int&  global_combinational_size, nodeMapType nodeMap, string input);

/**
  * @brief			Load combinational cones from input file.
  * @param  input 	Preprocessing input file.
  * 
  * @return 		Map with combinational cones.
  */
	map<int,vector<int>> load_combinational_cones (string input);

/**
  * @brief						Read all possible reconvergent paths from input file.
  * @param  input_file 			Preprocessing input file.
  * @param  list_of_sources 	List of all components that can be sources of error.
  * @param  list_of_targets 	List of all components that can be a target of error.
  * @param  nodeMap 			Node map that is created that hold all instances of components using strings.
  * 
  * @return 					Map with the reconvergent paths.
  */
	reconvergencePathMapType read_all_possible_reconvergent_paths_from_file(string input_file, vector<vertexDescType> list_of_sources, 
		vector<vertexDescType> list_of_targets, nodeMapType nodeMap);
	
/**
  * @brief				Filters the reconvergent paths to reduce the processing scope.
  * @param  G       	Pointer to a MyGraphType which describes the Graph structure in 
  *                 		the form of a adjacency_list.
  * @param  rec_path 	Map with reconvergent paths.
  * @param  input_file	Preprocessing input file.
  * @param  nodeMap 	Node map that is created that hold all instances of components using strings.
  * 
  */
	void filter_reconvergent_paths(MyGraphType G, reconvergencePathMapType& rec_path, string input_file, nodeMapType nodeMap)

/**
  * @brief				Find reconvergent paths.
  * @param  G       	Pointer to a MyGraphType which describes the Graph structure in 
  *                 		the form of a adjacency_list.
  * @param  input_file	Preprocessing input file.
  * @param  nodeMap 	Node map that is created that hold all instances of components using strings.
  * 
  */
	reconvergencePathMapType find_reconvergence_paths(MyGraphType G, string input_file, nodeMapType nodeMap);

/**
  * @brief				Generates look-up table with a matrix of probability of error functions.
  * @param  func       	Look-up table function.
  * @param  lutsize		Look-up table size.
  * 
  */
	vector<errorValuesModel> gen_lut_prob_func_matrix_not_minimized (long long int func, int lutsize)); 


#endif