 /**
  * @file   types.hpp
  * @date   Jun 10, 2019
  * @author Eduardo Luis Rhod 
 */

#ifndef TYPESHPP
#define TYPESHPP

#include <iostream>
#include <vector>
#include <map>
#include <bitset>
#include <boost/program_options.hpp>
#include <boost/graph/adjacency_list.hpp>
    
// Defines that help making names simpler
#define P0CORRECT first.first
#define P1CORRECT first.second
#define P0WRONG second.first
#define P1WRONG second.second
#define NETP0CORRECT net_failure_probability.first.first
#define NETP1CORRECT net_failure_probability.first.second
#define NETP0WRONG net_failure_probability.second.first
#define NETP1WRONG net_failure_probability.second.second
#define PININEDGE name.second.second
#define PINOUTEDGE name.first.second
#define NODEOUTEDGE name.first.first
#define NODEINEDGE name.second.first

// Defines bit codes
#define OUT_0C "1000"
#define OUT_1C "0100"
#define OUT_0W "0010"
#define OUT_1W "0001"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;
using std::pair;
using std::tie;
using std::bitset;
using std::ifstream;

// typedef MyGraphType::vertex_descriptor vertexDescType;
typedef boost::adjacency_list<>::vertex_descriptor vertexDescType;

// Program Option Struct
struct programOptions {
    string input;
    string output;
    string lut;
    int run_mode;
    int print;
    int watchlist_flag;
    int cycles_to_run; 
    string output_watch;
    vector<int> multiproc;
    int propagation_time;
    int error_flag;
};
typedef struct programOptions programOptions;
    
// Input and output Edge type.
typedef vector<pair<pair<string,string>,string> > ioEdgeType;
// Error Probability Type.
typedef pair <pair<double,double> , pair<double,double>> errorProbabilityType;
//      map<SOURCE,         map<DEST,           vector<vector<VERTEX> => maps all paths from sources to destination in vectors of vectors

// Enumeration with Model of Error Values
enum errorValuesModel {ERROR_VAL_NOT_DEFINED, C0 , C1 , W0 , W1 };

// Enumeration with component type
enum ComponentTypeEnum { NOT_DEFINED, Combinational, Sequential };

// Enumeration with component primitives
//                      0           1   2      3        4      5    6      7       8    9     10    11      12     13     14                                                                  15    16    17    18    19    20
enum PrimEnum { PRIM_NOT_DEFINED, VCC, GND, OUTPORT, INPORT, IBUF, OBUF, BUFGCE, FDRE, FDPE, FDCE, BUFG, CARRY8, CARRY4, MUXF7, /*NEW Primitives MUST ALWAYS be inserted here, before LUTS*/ LUT1, LUT2, LUT3, LUT4, LUT5, LUT6 };
    
// Enumeration with component definition type
enum PrimTypeEnum { PRIMTYPE_NOT_DEFINED, PORT, CELL };

// Enumeration with edge atributes
enum EdgeAtribTypeEnum { ID, NAME, NET_FAILURE_PROBABILITY, PREVIOUS_NET_FAILURE_PROBABILITY, NEXT_NET_FAILURE_PROBABILITY, P0P1_ATRIBUTE };
    
// Struct with node properties
struct nodeProperties {   // vertex are the same as nodes in a graph
    vertexDescType id;
    enum PrimTypeEnum prim_type = PRIMTYPE_NOT_DEFINED;
    enum PrimEnum prim = PRIM_NOT_DEFINED;
    long long int logic_func;
    pair<pair<string,string>,pair<string,string>> prob_funcs;  // < <p0c_s,p1c_s> , <p0w_s,p1w_s> >
    enum ComponentTypeEnum component_type = NOT_DEFINED;
    ioEdgeType out_edges; // out_edges pair of (out_pin-in_pin, next_node)
    ioEdgeType in_edges; // in_edges pair of (in_pin, previous_node)
    vector<float> logic_prob_not_filtering;
    int combinational_path_position=0;
    vector<string> correlated_value_expression;
    vector<errorValuesModel> correlated_value_matrix;       
 };
typedef struct nodeProperties nodeProperties;
   
// Struct with edge properties
struct edgeProperties {
    int id;
    pair< pair<string,string> , pair<string,string> > name;  // a pair of string pairs <<out_node,out_pin> , <in_node,in_pin>>
    errorProbabilityType net_failure_probability;
    errorProbabilityType next_net_failure_probability;  // used to control that all memory elements have its value altered only at the "end of the clock cycle"
    errorProbabilityType previous_net_failure_probability;
    string in_pin_name;
    string out_pin_name;
    vector<pair<double,errorValuesModel>> correlated_value;
};
typedef struct edgeProperties edgeProperties;

// Program Option namespace
namespace po = boost::program_options;

// MY Graph type definition
typedef boost::adjacency_list<boost::vecS,
                             boost::vecS,
                             boost::bidirectionalS,
                             nodeProperties,
                             edgeProperties
                             > MyGraphType;
// Node map definition
typedef map<string, nodeProperties> nodeMapType;

// Edge map definition
typedef map<int, edgeProperties> edgeMapType;

// Edge descriptor type definition
typedef MyGraphType::edge_descriptor edgeDescType;

// Iterators definition
typedef boost::graph_traits <MyGraphType>::edge_iterator edgeIt;
typedef boost::graph_traits <MyGraphType>::vertex_iterator vertexIt;
typedef boost::graph_traits <MyGraphType>::out_edge_iterator oEdge_it;
typedef boost::graph_traits <MyGraphType>::in_edge_iterator iEdge_it;

// Reconvergence and correlated source type definitions
typedef map<vertexDescType, map<vertexDescType, vector<vector<vertexDescType>>>> reconvergencePathMapType;
typedef map<vertexDescType, map<vertexDescType, map <int, vector<vector<vertexDescType>>>>> reconvergencePathCycleMapType;
typedef vector<vector<pair<int,string>>> correlatedSourcesType;

// Circuit information structure
struct circuitInfo {
    map<int,vector<int>> combinational_cones;
    edgeMapType edgeMap;
    vector<int> outPorts;
    vector<int> inPorts;
    vector<int> sequential_list;
    vector<int> combinational_list;
    vector<int> lutList;
    int global_combinational_size=0;
 };
typedef struct circuitInfo circuitInfo;

// Correlation information structure
struct correlationInfo {
    reconvergencePathMapType reconvergence_path_map;
    correlatedSourcesType correlated_sources;
};
typedef struct correlationInfo correlationInfo;

// Thread argument structure
struct thread_args {
    MyGraphType* G;
    int error_source;
    nodeMapType* nodeMap;
    map<int,vector <errorProbabilityType>> report;
    circuitInfo cif;
    int cycles_to_run;
    correlationInfo correlation_Info;
 };

// Extern functions and variables
extern string dbg_lut;
extern map <int, string> id2node;
extern int dbgflag;

#endif

