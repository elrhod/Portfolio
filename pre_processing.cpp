 /**
  * @file  	pos_processing.cpp
  * @date 	Aug 14, 2019
  * @author Eduardo Luis Rhod 
 */

#include "types.hpp"
#include "scape_utils.hpp"
#include "pre_processing.hpp"

#define DEBUG ON
#define DEBUG2 ON
#define DEBUG3 ON
 
#ifdef DEBUG
#define DEBUG_MSG_PRE(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG_PRE(str) do { } while ( false )
#endif

#ifdef DEBUG2
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

#ifdef DEBUG3
#define DBGMSG(str) do { std::cout << str; } while( false )
#else
#define DBGMSG(str) do { } while ( false )
#endif

int processLine (string line, nodeMapType& nodeMap, edgeMapType& edgeMap, vector<int>& outPorts, vector<int>& inPorts) {
    // #  Line format:
    // # Examples:
    // # INPUT PORT TYPE   # INPORT::[inport_name]=>[next_cell]@[input_pin]
    // # OUTPUT PORT TYPE  # OUTPORT::[outport_name]<=[next_cell]@[output_pin]
    // # LUT CELL TYPE     # LUT4 16'hxxxx::[lut_name]<=[input_pin_name0]<-[source_cell]@[source_output_pin_name] [input_pin_name1]<-[source_cell]@[source_output_pin_name] ...
    // # GENERAL CELL TYPE # [cell_type]::[cell_name]<=[input_pin_name0]<-[source_cell]@[source_output_pin_name] [input_pin_name1]<-[source_cell]@[source_output_pin_name] ...

    nodeProperties nprop1,nprop2, vcc_node, gnd_node;
    std::fill(nprop1.logic_prob_not_filtering.begin(), nprop1.logic_prob_not_filtering.end(), 0.0); // initializing vector elements with 0.0
    edgeProperties eprop1;
    vector<string> line_spl;
    string st, sti, sto;
    string hx;
    static int counter=0;
    static int edge_id=0;
    int cnt = countFreq("<-",line);

    vcc_node.prim_type = CELL;
    vcc_node.prim = VCC;
    vcc_node.component_type = NOT_DEFINED;
    gnd_node.prim_type = CELL;
    gnd_node.prim = GND;
    gnd_node.component_type = NOT_DEFINED;

    if (line.length()>0)
    if (cnt == 0) { // INPORT or OUTPORT types
        ///////////////////// INPORTS
        if (line.find("INPORT")!=string::npos){
            string inport_name = line.substr(line.find("::")+2,(line.find("=> ") - (line.find("::")+2)));
            string next_cell = line.substr(line.find("=> ")+3,(line.find("@") - (line.find("=> ")+3)));
            string input_pin = line.substr(line.find("@")+1);

            if (nodeMap.find(inport_name) == nodeMap.end() )  { // if node does not exist yet, create it...
                nprop1.prim_type = PORT;
                nprop1.prim = INPORT;
                nprop1.logic_func = -1;
                nprop1.component_type = NOT_DEFINED;
                nprop1.id = counter++;
                nodeMap[inport_name] = nprop1;
                inPorts.push_back(nprop1.id);
            } else {
                cout << "Component " << line << " , Instatiated 2 times!!!" << endl;
                exit(1);
            }

        } else {
            ///////////////////// OUTPORTS
            if (line.find("OUTPORT")!=string::npos) {
                string outport_name = line.substr(line.find("::")+2,(line.find("<= ")-(line.find("::")+2)));
                string prev_cell = line.substr(line.find("<= ")+3,(line.find("@")-(line.find("<= ")+3)));
                string output_pin = line.substr(line.find("@")+1);

                if (nodeMap.find(outport_name) == nodeMap.end() )  { // if node does not exist yet, create it...
                    nprop1.prim_type = PORT;
                    nprop1.prim = OUTPORT;
                    nprop1.logic_func = -1;
                    nprop1.component_type = NOT_DEFINED;
                    nprop1.in_edges.push_back( make_pair(make_pair("OUTPORT", output_pin ), prev_cell) );
                    nprop1.id = counter++;
                    nodeMap[outport_name] = nprop1;
                    eprop1.name = std::make_pair(std::make_pair(prev_cell,output_pin),std::make_pair(outport_name,"."));
                    edgeMap[edge_id++]= eprop1;
                    outPorts.push_back(nprop1.id);
                } else {
                    cout << "Component " << line << " , Instantiated 2 times!!!" << endl;
                    exit(1);
                }

            }
        }
    } else {
        if (cnt > 0){ // Other types of CELLS
            //////////////////////////////////////// LUT
            string sub = line.substr(0,3);
            
            if (line.substr(0,3).find("LUT")!=-1){ // It is a LUT

                string this_cell = line.substr(line.find("::")+2,(line.find("<= ")-(line.find("::")+2)));
                string line_after_first_space =  line.substr(line.find("<= ")+3);
                vector<string> line_spl = split_string(line_after_first_space);
                nprop1.prim_type = CELL;
                nprop1.prim = convert_to_enum(line.substr(0 , line.find(" ")));
                nprop1.component_type = Combinational;

                string hx = line.substr(line.find("'h")+2,(line.find("::")-(line.find("'h")+2)));
                std::istringstream ishx(hx);
                unsigned long long int func;
                ishx >> std::hex >> func;
                int lut_size = line.at(3) - '0';


                nprop1.prob_funcs =  gen_lut_prob_func_not_minimized (func,  lut_size);
                nprop1.correlated_value_expression =  gen_correlated_lut_prob_func_not_minimized (func,  lut_size);
                nprop1.correlated_value_matrix = gen_lut_prob_func_matrix_not_minimized (func,  lut_size);

                nprop1.id = counter++;
                nprop1.logic_func = func;
                nprop1.component_type = Combinational;

                if (nodeMap.find(this_cell) == nodeMap.end() )  { // if node does not exist yet, create it...
                    for (size_t i = 0; i < cnt; i++) {
                        string input_pin = line_spl.at(i).substr(0 , line_spl.at(i).find("<-"));
                        string output_pin = line_spl.at(i).substr(line_spl.at(i).find("@")+1);
                        string prev_cell = line_spl.at(i).substr(line_spl.at(i).find("<-")+2 , (line_spl.at(i).find("@")-(line_spl.at(i).find("<-")+2)));
                        if (prev_cell.find("/IBUFCTRL_INST")!=-1){
                            prev_cell = prev_cell.substr(0,prev_cell.find("/IBUFCTRL_INST"));
                        } else {
                            if (prev_cell.find("VCC")!=-1 && prev_cell.size()<=5){ // it is a VCC type of source_cell
                                nodeMap[prev_cell] = vcc_node;
                            } else {
                                if (prev_cell.find("GND")!=-1 && prev_cell.size()<=5){ // it is a VCC type of source_cell
                                    nodeMap[prev_cell] = gnd_node;
                                }
                            }
                        }

                        nprop1.in_edges.push_back( make_pair ( make_pair(output_pin , input_pin) ,prev_cell) ); // in_edges pair of ((out_pin,in_pin), prev_node)
                        eprop1.name = std::make_pair(std::make_pair(prev_cell,output_pin),std::make_pair(this_cell,input_pin));
                        edgeMap[edge_id++]=eprop1;
                    }
                    nodeMap[this_cell] = nprop1;
                } else {
                    cout << "Component " << line << " , Instantiated 2 times!!!" << endl;
                    exit(1);
                }

            } else { // It is another type of cell
            //////////////////////////////////////////// ALL OTHER CELLS

                string line_after_first_space =  line.substr(line.find("<= ")+3);
                vector<string> line_spl = split_string(line_after_first_space);

                nprop1.prim_type = CELL;
                nprop1.id = counter++;
                nprop1.prim = convert_to_enum(line.substr(0 , line.find("::")));
                string this_cell = line.substr(line.find("::")+2,(line.find("<= ")-(line.find("::")+2)));
                if (nodeMap.find(this_cell) == nodeMap.end() )  { // if node does not exist yet, create it...
                    string line_after_first_space =  line.substr(line.find("<= ")+3);
                    vector<string> line_spl = split_string(line_after_first_space);
                    for (size_t i = 0; i < cnt; i++) {
                        string input_pin = line_spl.at(i).substr(0 , line_spl.at(i).find("<-"));
                        string output_pin = line_spl.at(i).substr(line_spl.at(i).find("@")+1);
                        string prev_cell = line_spl.at(i).substr(line_spl.at(i).find("<-")+2 , (line_spl.at(i).find("@")-(line_spl.at(i).find("<-")+2)));
                        if (prev_cell.find("/IBUFCTRL_INST")!=-1){
                            prev_cell = prev_cell.substr(0,prev_cell.find("/IBUFCTRL_INST"));
                        } else {
                            if (prev_cell.find("VCC")!=-1 && prev_cell.size()<=5){ // it is a VCC type of source_cell
                                if (nodeMap.find(prev_cell) == nodeMap.end() )  {  // if node does not exist yet, create it...
                                    vcc_node.id = counter++;
                                    nodeMap[prev_cell] = vcc_node;
                                }

                            } else {
                                if (prev_cell.find("GND")!=-1 && prev_cell.size()<=5){ // it is a VCC type of source_cell
                                    if (nodeMap.find(prev_cell) == nodeMap.end() )  {  // if node does not exist yet, create it...
                                        gnd_node.id = counter++;
                                        nodeMap[prev_cell] = gnd_node;
                                    }
                                }
                            }
                        }
                        nprop1.in_edges.push_back( make_pair ( make_pair(output_pin , input_pin) ,prev_cell) ); // in_edges pair of ((out_pin,in_pin), prev_node)
                        eprop1.name = std::make_pair(std::make_pair(prev_cell,output_pin),std::make_pair(this_cell,input_pin));
                        edgeMap[edge_id++]=eprop1;
                    }
                    nodeMap[this_cell] = nprop1;
                } else {
                    cout << "Component " << line << " , Instatiated 2 times!!!" << endl;
                    exit(1);
                }
            }
        }
    }

    return counter;
}

bool process_command_line (int argc, char** argv, programOptions& ins) {
    try
    {
        po::options_description desc("Program Usage", 1024, 512);
        desc.add_options()
        ("help,h",     ": Help message describing program options")
        ("input,i",   po::value<string>(&ins.input)->required(), ": path to the input file generated by the TCL script (mandatory)")
        ("output,o",   po::value<string>(&ins.output)->required(), ": path to the output file to generate the report (mandatory)")
        ("lut,l",   po::value<string>(&ins.lut)->default_value(""), ": lut name to inject error (optional)")
        ("print,p",   po::value<int>(&ins.print)->default_value(0), ": if print_flag = 1, will print the net probability of failure values (optional)")
        ("run_mode,r",   po::value<int>(&ins.run_mode)->default_value(0), ": run_mode = 0, normal fault injection; \n\t run_mode = 1, debuging;")
        ("cycles_to_run,c",   po::value<int>(&ins.cycles_to_run)->default_value(-1), ": if cycles_to_run > 0, will generate a report for the given number of cycles (optional)")
        ("propagation_time,g",   po::value<int>(&ins.propagation_time)->default_value(-1), ": Propagation time of each circuit (optional)")
        ("watch,w",   po::value<int>(&ins.watchlist_flag)->default_value(0), ": if watchlist_flag = 1, will generate a watchlist from source to target (optional)")
        ("error,e",   po::value<int>(&ins.error_flag)->default_value(1), ": if error_flag = 0, will run the simulation without fault injection (optional)")
        ("output_watch,x",   po::value<string>(&ins.output_watch)->default_value(""), ": if watchlist_flag = 1, will generate a watchlist from source(lut) to output_watch target (optional)")
        ("multiprocess,m",   po::value<vector<int>>(&ins.multiproc)->multitoken(), ": if first value greater than 1, indicates how many parallel threads will be created. Must be less or equal to the THREADS_NUM. Second integer value indicates how many luts will be used to create the parallel processing package.")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        {
            cout << desc << "\n";
            return false;
        }

        // There must be an easy way to handle the relationship between the
        // option "help" and "host"-"port"-"config"
        // Yes, the magic is putting the po::notify after "help" option check
        po::notify(vm);

        if (ins.multiproc.size()<= 0){
            ins.multiproc = {0,0};
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return false;
    }
    catch(...)
    {
        std::cerr << "Unknown error!" << "\n";
        return false;
    }

    return true;
}

MyGraphType file_to_graph (string input, nodeMapType& nodeMap, edgeMapType& edgeMap, map <int, string>& id2node, 
    vector<int>& outPorts, vector<int>& inPorts) {
    //// Constructs file path based on current location + input path
    oEdge_it out_i, out_end;
    char path[256] = "";
    string file_path = "";
    int cnt;
    nodeProperties dummy;

    if (getcwd(path, sizeof(path)) == NULL) // gets current directory
        perror("getcwd() error");
    else {
        file_path += path;
        file_path += '/';
        file_path += input;
    }

    //// Opens input file
    std::ifstream file(file_path);
    if (!file) {
        cout << "unable to open file:" << file_path << endl;
        exit(0); // TODO: Improve error messages
    }
    string line;
    while (getline(file, line)) {
        cnt = processLine(line, nodeMap, edgeMap, outPorts, inPorts);
    }

    for(auto elem : nodeMap) {
        id2node[elem.second.id]=elem.first;
    }

    for(auto& elem : nodeMap) {
        if (elem.second.component_type == NOT_DEFINED){
            if (elem.second.prim == IBUF
                || elem.second.prim == OBUF
                || elem.second.prim == CARRY8
                || elem.second.prim == CARRY4
                || elem.second.prim == MUXF7
                || elem.second.prim == BUFG
                || elem.second.prim == BUFGCE ){
                elem.second.component_type = Combinational;
            } else {
            if (elem.second.prim == FDPE
                || elem.second.prim == FDCE
                || elem.second.prim == FDRE){
                elem.second.component_type = Sequential;
            }}
        }
    }

    /* DUMMY COMPONENT DECLARATION TO USE AS ERROR SOURCE IN CASE OF FAULTY FREE SIMULATION */
    dummy.id = nodeMap.size();
    dummy.prim_type=PRIMTYPE_NOT_DEFINED;
    dummy.prim = PRIM_NOT_DEFINED;
    dummy.logic_func = 0;
    dummy.component_type = NOT_DEFINED;
    id2node[dummy.id]="NO_ERROR_DUMMY_COMPONENT";
    nodeMap["NO_ERROR_DUMMY_COMPONENT"] = dummy;

    //////////// CREATE GRAPH
    MyGraphType G(nodeMap.size());

    /////////// POPULATE NODES AND PROPERTIES
    for(auto elem : nodeMap) {
        G[elem.second.id] = elem.second;
    }



    //////////// ADD EDGES AND PROPERTIES
    // cout << "edgeMap!" << cnt << endl;
    for(auto ele : edgeMap) {
        edgeDescType e = add_edge(nodeMap[ele.second.NODEOUTEDGE].id, nodeMap[ele.second.NODEINEDGE].id,G).first;
        
        if (ele.second.PININEDGE.size()>0)
                G[e].in_pin_name = ele.second.PININEDGE;
        if (ele.second.PINOUTEDGE.size()>0)
                G[e].out_pin_name = ele.second.PINOUTEDGE;
                nodeMap[ele.second.NODEOUTEDGE].out_edges.push_back( make_pair(make_pair(ele.second.PINOUTEDGE, ele.second.PININEDGE), ele.second.NODEINEDGE) ); // out_edges pair of (out_pin, next_node)
        if (ele.second.PININEDGE.size()>0 || ele.second.PINOUTEDGE.size()>0) {
                G[e] = ele.second;
        }
    }

    return G;
}

vector<int> get_combinational_cone (MyGraphType G, int starting_node) {
    int current_position = 1;
    vector<int> todo_list, cone_list;
    map<int, int> nodes_dic;
    int node;
    oEdge_it out_i, out_end;

    // Populates the list and dictionary with the first successors
    for (tie(out_i, out_end) = boost::out_edges(starting_node, G); out_i != out_end; ++out_i){
        node = boost::target(*out_i,G);
        
        if(G[node].component_type == Combinational){
            todo_list.push_back(node);
            nodes_dic[node] = current_position;
        }
    }

    while (!todo_list.empty()) {
        int next_node = todo_list[todo_list.size()-1]; todo_list.pop_back();
        int node_position = nodes_dic[next_node];
        
        if(G[next_node].component_type == Combinational){
            // get next_node predecessors and add to list and dictionary
            current_position= node_position+1;
            for (tie(out_i, out_end) = boost::out_edges(next_node, G); out_i != out_end; ++out_i){
                node = boost::target(*out_i,G);
                // check if node is combinational
                if(G[node].component_type == Combinational){
                    // check if node is not in the list
                    if ( std::find(todo_list.begin(), todo_list.end(), node) != todo_list.end() ) {
                        // node is in list...it will be processed later. The node is also in a bigger position and the position have to be updated
                        if (nodes_dic[node]<current_position){
                            nodes_dic[node]=current_position;
                        }
                    } else {
                        // check if node was already processed (is already in the dictionary)
                        if (nodes_dic.find(node) == nodes_dic.end()){ //not in the dictionary...
                            //  not processed... then then put node in the list
                            todo_list.push_back(node);
                            nodes_dic[node]=current_position;
                        } else { //it was processed...
                            // check if node's previous position is smaller than current
                            if (nodes_dic[node]<current_position){ // if it is, then process the node and all its predecessors
                                todo_list.push_back(node);
                                nodes_dic[node]=current_position;
                            }
                        }
                    }
                }
            }
        }
    }
    
    for(auto elem : nodes_dic) {
        cone_list.push_back(elem.first);
    }

    return cone_list;
}
 
void generate_combinational_path_order (MyGraphType G, string starting_node, int& global_combinational_size, nodeMapType& nodeMap, string input) {
    int current_position = 1;
    char path[256] = "";
    vector<string> todo_list;
    map<string, int> nodes_dic;
    string prj_name;
    string node;
    std::ofstream outfile;
    string file_path;
    oEdge_it out_i, out_end;

    // Populates the list and dictionary with the first successors
    for(unsigned int i = 0; i < nodeMap[starting_node].out_edges.size(); i++) {
        node = nodeMap[starting_node].out_edges[i].second;

        if(nodeMap[node].component_type == Combinational){
            todo_list.push_back(node);
            nodes_dic[node] = current_position;
        }
    }

    while (!todo_list.empty()) {
        string next_node = todo_list[todo_list.size()-1]; todo_list.pop_back();
        int node_position = nodes_dic[next_node];
        if(nodeMap[next_node].component_type == Combinational){
            // update node comb path position
            if (nodeMap[next_node].combinational_path_position < node_position){
                nodeMap[next_node].combinational_path_position = node_position;
            }
            // update global_combinational_size
            if (global_combinational_size < current_position) {
                global_combinational_size = current_position;
            }
            // get next_node predecessors and add to list and dictionary
            current_position= node_position+1;
            for(unsigned int i = 0; i < nodeMap[next_node].out_edges.size(); i++) {
                node = nodeMap[next_node].out_edges[i].second;
                // check if node is combinational
                if(nodeMap[node].component_type == Combinational){
                    // check if node is not in the list
                    if ( std::find(todo_list.begin(), todo_list.end(), node) != todo_list.end() ) {
                        // node is in list...it will be processed later. The node is also in a bigger position and the position have to be updated
                        if (nodes_dic[node]<current_position){
                            nodes_dic[node]=current_position;
                        }
                    } else {
                        // check if node was already processed (is already in the dictionary)
                        if (nodes_dic.find(node) == nodes_dic.end()){ //not in the dictionary...
                            //  not processed... then then put node in the list
                            todo_list.push_back(node);
							nodes_dic[node]=current_position;
                        } else { //it was processed...
                            // check if node's previous position is smaller than current
                            if (nodes_dic[node]<current_position){ // if it is, then process the node and all its predecessors
                                todo_list.push_back(node);
    							nodes_dic[node]=current_position;
                            }
                        }
                    }
                }
            }
        }
    }

    vector<int>  cone_list;
    for(auto elem : nodes_dic) {
        cone_list.push_back(nodeMap[elem.first].id);
    }

    map<int, vector<int>> cone_dict;
    cone_dict[nodeMap[starting_node].id] = cone_list;

    prj_name = input.substr(input.find("/"), input.find(".txt")-6);
    if (getcwd(path, sizeof(path)) == NULL) // gets current directory
        perror("getcwd() error");
    else {
        file_path += path;
        file_path += "/preprocessing";
        file_path += prj_name;
        file_path += "_comb_cones_from_sequential_elements_num.txt";
        DEBUG_MSG_PRE("Combinational path order filepath is " << file_path);
    }

    outfile.open(file_path, std::ios_base::app);

    for (auto elem : cone_dict) {
        outfile << elem.first << ", [";
        unsigned int ii=0;
        for (const auto &e : elem.second) {
            outfile << e;
            if (ii!=(elem.second.size()-1))
                outfile << ", ";
            ii++;
        }
    }
    outfile << "]" << endl;
}

template<std::size_t N>
void reverse(std::bitset<N> &b) {
    for(std::size_t i = 0; i < N/2; ++i) {
        bool t = b[i];
        b[i] = b[N-i-1];
        b[N-i-1] = t;
    }
}

pair<pair<string,string>,pair<string,string>> gen_lut_prob_func_not_minimized (long long int func, int lutsize) {
    vector <enum errorValuesModel> val {C0 , C1 , W0 , W1};
    enum errorValuesModel current_inputs[lutsize];
    bitset<2> l1func(func);
    bitset<4> l2func(func);
    bitset<8> l3func(func);
    bitset<16> l4func(func);
    bitset<32> l5func(func);
    bitset<64> l6func(func);
    bitset<2> inp1; // initialized with all bits `0`
    bitset<4> inp2; // initialized with all bits `0`
    bitset<6> inp3; // initialized with all bits `0`
    bitset<8> inp4; // initialized with all bits `0`
    bitset<10> inp5; // initialized with all bits `0`
    bitset<12> inp6; // initialized with all bits `0`
    int right_bit=0, wrong_bit=0; // right and wrong result bit
    string p1c_s ="",p0c_s ="",p1w_s ="",p0w_s ="";
    
    if (dbgflag==1){  cout << "l5func:"<<l5func << endl; }

    for (long int i = 0; i < pow(2,(lutsize*2)); i++) {
        for (size_t j = 0; j < lutsize; j++) {
            if (dbgflag==1){ cout << i << "|" << j << " "; }
            if (j==0){
                current_inputs[0] = val.at(i%4);
            } else {
                current_inputs[j] = val.at((int(i/(pow(4,j))))%4);
            }
            if (dbgflag==1){  cout << current_inputs[j] << " "; }
        }

        right_bit=0;
        wrong_bit=0;
        string expr = "";
        for (int j = lutsize-1; j >=0 ; j--) {
            if (dbgflag==1){  cout << "j:"<<j << endl; }
            switch (current_inputs[j]) {
                case W1:
                    if (j!=0) {
                        wrong_bit+=pow(2,j);
                    } else {
                        wrong_bit+=1;
                    }
                    if (dbgflag==1){  cout << "W1 "; }
                    expr+="I"+std::to_string(j)+"P1WRONG*";
                    break;
                case C1:
                    if (j!=0) {
                        right_bit+=pow(2,j);
                        wrong_bit+=pow(2,j);
                    } else {
                        right_bit+=1;
                        wrong_bit+=1;
                    }
                    if (dbgflag==1){  cout << "C1 "; }
                    expr+="I"+std::to_string(j)+"P1CORRECT*";
                    break;
                case W0:
                    if (j!=0) {
                        right_bit+=pow(2,j);
                    } else {
                        right_bit+=1;
                    }
                    if (dbgflag==1){  cout << "W0 "; }
                    expr+="I"+std::to_string(j)+"P0WRONG*";
                    break;
                case C0:
                    if (dbgflag==1){  cout << "C0 "; }
                    expr+="I"+std::to_string(j)+"P0CORRECT*";
                    break;
            }
        }
        expr+="1";
        if (dbgflag==1){
            cout << expr << endl;
            cout << "\n right_bit=" << right_bit << "\n wrong_bit=" << wrong_bit << endl;
        }
        switch (lutsize) {
            case 1:
                if(l1func[right_bit]==l1func[wrong_bit]){
                    if (l1func[right_bit]==1){
                        p1c_s+=expr+"+";
                    } else {
                        p0c_s+=expr+"+";
                    }
                } else {
                    if (l1func[wrong_bit]==1){
                        p1w_s+=expr+"+";
                    } else {
                        p0w_s+=expr+"+";
                    }
                }
            break;
            case 2:
                if(l2func[right_bit]==l2func[wrong_bit]){
                    if (l2func[right_bit]==1){
                        p1c_s+=expr+"+";
                    } else {
                        p0c_s+=expr+"+";
                    }
                } else {
                    if (l2func[wrong_bit]==1){
                        p1w_s+=expr+"+";
                    } else {
                        p0w_s+=expr+"+";
                    }
                }
                break;
            case 3:
            if(l3func[right_bit]==l3func[wrong_bit]){
                if (l3func[right_bit]==1){
                    p1c_s+=expr+"+";
                } else {
                    p0c_s+=expr+"+";
                }
            } else {
                if (l3func[wrong_bit]==1){
                    p1w_s+=expr+"+";
                } else {
                    p0w_s+=expr+"+";
                }
            }
            break;
            case 4:
            if(l4func[right_bit]==l4func[wrong_bit]){
                if (l4func[right_bit]==1){
                    p1c_s+=expr+"+";
                } else {
                    p0c_s+=expr+"+";
                }
            } else {
                if (l4func[wrong_bit]==1){
                    p1w_s+=expr+"+";
                } else {
                    p0w_s+=expr+"+";
                }
            }
            break;
            case 5:
            if(l5func[right_bit]==l5func[wrong_bit]){
                if (l5func[right_bit]==1){
                    if (dbgflag==1){ cout << "p1c" << OUT_1C << endl; }
                    p1c_s+=expr+"+";
                } else {
                    if (dbgflag==1){ cout << "p0c"<< OUT_0C << endl; }
                    p0c_s+=expr+"+";
                }
            } else {
                if (l5func[wrong_bit]==1){
                    if (dbgflag==1){ cout << "p1w"<< OUT_1W << endl; }
                    p1w_s+=expr+"+";
                } else {
                    p0w_s+=expr+"+";
                    if (dbgflag==1){ cout << "p0w"<< OUT_0W << endl; }
                }
            }
            break;
            case 6:
            if(l6func[right_bit]==l6func[wrong_bit]){
                if (l6func[right_bit]==1){
                    p1c_s+=expr+"+";
                } else {
                    p0c_s+=expr+"+";
                }
            } else {
                if (l6func[wrong_bit]==1){
                    p1w_s+=expr+"+";
                } else {
                    p0w_s+=expr+"+";
                }
            }
            break;
        }
    }
    p0c_s+="0;";
    p1c_s+="0;";
    p0w_s+="0;";
    p1w_s+="0;";
    
    if (dbgflag==1){
        cout << "p0c_s="<< p0c_s  << "\np1c_s="<< p1c_s  << "\np0w_s="<< p0w_s  << "\np1w_s="<< p1w_s << endl;
        getchar();
    }
    return (std::make_pair(std::make_pair(p0c_s,p1c_s),std::make_pair(p0w_s,p1w_s)));
}

void generate_LUT_probability_of_error_functions (MyGraphType& G, nodeMapType& nodeMap) {

    char path[256] = "";
    std::ifstream fin;
    pair<pair<string,string>,pair<string,string>> prob_funcs;

    int cnt =0;
    for(pair<vertexIt,vertexIt> it = boost::vertices(G); it.first != it.second; ++it.first) {
        if (G[*it.first].prim>=LUT1){
            if (G[*it.first].id == nodeMap[dbg_lut].id) {
                dbgflag=1;
            } else {
                DEBUG_MSG_PRE(G[*it.first].id<< ":"<< nodeMap[dbg_lut].id);
            }

            int lutsize = G[*it.first].prim-LUT1 +1;
            long long int logic_func = G[*it.first].logic_func;
            G[*it.first].prob_funcs = gen_lut_prob_func_not_minimized(logic_func, lutsize);

            cnt++;
            dbgflag=0;
        }
    }
}

map<vertexDescType, vector<vector<vertexDescType>>>  get_all_paths (MyGraphType G, vertexDescType source, vector<vertexDescType> targets){
    oEdge_it out_i, out_end;
    vertexDescType node;
    map<vertexDescType, vector<vector<vertexDescType>>> map_all_the_paths;
    cout << "\nGetting paths for " <<  source << ":" << id2node[source] << endl;

    auto src_pos = std::find(targets.begin(), targets.end(), source);
    if(src_pos!=targets.end()) targets.erase(src_pos);

    map<vertexDescType, vector<vertexDescType>> visited_path, predecessors;

    for (auto  targ : targets){
        cout << " TARGET= " <<  targ << ":" << id2node[targ] <<  endl;
        vector<vertexDescType> todo_list, current_list, current_path;
        vector<vector<vertexDescType>> all_the_paths, all_the_paths2;
        visited_path.clear();
        predecessors.clear();

        vector<vertexDescType> source_v;
        source_v.push_back(source);
        visited_path[source]=source_v; // adds source as a visited node.

        for (tie(out_i, out_end) = boost::out_edges(source, G); out_i != out_end; ++out_i) {
            node = boost::target(*out_i,G);
            cout << node <<  " ";
            todo_list.push_back(node);
        }
        cout << endl;
        while (!todo_list.empty()) {
            cout << "   Todo List: ";
            for(auto it : todo_list){
                cout << it << " ";
            }
            cout << endl;

            // POP from todo_list
            vertexDescType next_node = todo_list[todo_list.size()-1]; todo_list.pop_back();
            cout << "    Processing:"<< next_node << "(" << id2node[next_node] << ")" << endl;

            if (predecessors.find(next_node) == predecessors.end()) { // if it has predecessors already visited?
                current_path = predecessors[next_node];
            }
            if (next_node==targ){
                cout<< "_TARGET " ;
                all_the_paths.push_back(current_path);
                cout << "     Addded path current_path: ";
                for (auto it : current_path){
                    cout <<" "<<it;
                }
                cout <<endl;
            } else {
                visited_path[next_node]=current_path;
                current_path.push_back(next_node);

                if(out_degree(next_node, G)>0){ // if it is not an outport
                    for (tie(out_i, out_end) = boost::out_edges(next_node, G); out_i != out_end; ++out_i) {
                        node = boost::target(*out_i,G);
                        cout<< "     " << node << "? ";
                        if (node==targ) { // if it is target
                            cout<< "TARGET " ;
                            all_the_paths.push_back(current_path);
                            cout << "     Addded path current_path: ";
                            for (auto it : current_path){
                                cout <<" "<<it;
                            }
                            cout <<endl;
                        } else { // Not the target
                            if (visited_path.find(node) == visited_path.end()) { // Node is not in the dictionary?
                                cout<< "added to todo list!" << endl;
                                todo_list.push_back(node); // add to the list to be visited
                                predecessors[node]=current_path;
                            } else {
                                cout<< " Node "<< node << " already visited (it's in the dictionary)!!!" << endl;
                            }
                        }
                    }
                }
            }
            //
        }
        if (all_the_paths.size()>0)
            map_all_the_paths[targ] = all_the_paths;
    }

    DBGMSG("source:" << id2node[source] << "("<<source<<")");
    for(auto elem : map_all_the_paths) {
        DBGMSG("\n\tDEST: "<<id2node[elem.first]<< " :: ");
        for (auto vector : elem.second){
            DBGMSG(" [");
            for (auto nod : vector){
                DBGMSG(id2node[nod] << " ");

            }
            DBGMSG("]");
        }
    }
    getchar();
    //

    return map_all_the_paths;
}

void run_preprocessing (MyGraphType& G, programOptions prog_opt, nodeMapType& nodeMap, map <int, string> id2node, circuitInfo& cif, 
    correlationInfo& correlation_Info ) {
    string file_path, prj_name;
    char path[256] = "";
    std::ofstream fout;

    int cnt=0;
    int cnt2=0;

    for(pair<vertexIt,vertexIt> it = boost::vertices(G); it.first != it.second; ++it.first) {
          cnt++;
        if (G[*it.first].component_type == Sequential || G[*it.first].prim == INPORT) {
            cnt2++;
            generate_combinational_path_order(G, id2node[*it.first], cif.global_combinational_size, nodeMap, prog_opt.input);
        }
    }

    prj_name = prog_opt.input.substr(prog_opt.input.find("/"), prog_opt.input.find(".txt")-6);
    if (getcwd(path, sizeof(path)) == NULL) // gets current directory
        perror("getcwd() error");
    else {
        file_path += path;
        file_path += "/preprocessing";
        file_path += prj_name;
        file_path += "_comb_path_position_num.txt";
        DEBUG_MSG_PRE ("Current combinational path position filepath is " << file_path);
    }

    fout.open(file_path);
    for (auto elem : nodeMap){
        if (elem.second.component_type == Combinational){
            fout << elem.second.id << ", " << elem.second.combinational_path_position << "\n";
            cif.combinational_list.push_back(elem.second.id);
            if (elem.second.prim >= LUT1)
                cif.lutList.push_back(elem.second.id);
        }
    }
    fout.close();

    for (auto elem : nodeMap)
        if (elem.second.component_type == Sequential)
            cif.sequential_list.push_back(elem.second.id);

    // ################################################################################################################
    // ######## GLOBAL combinational path list of lists generation
    // ### Load Global combinational path positions from file (./preprocessing/combinational_path_position.txt)
    // ### Load combinational cones for each sequential element from file (./preprocessing/comb_cones_from_sequential_elements.txt)
    load_combinational_path_positions(G, cif.global_combinational_size, nodeMap, prog_opt.input);

    cif.combinational_cones = load_combinational_cones(prog_opt.input);


    file_path="";
    if (getcwd(path, sizeof(path)) == NULL) // gets current directory
        perror("getcwd() error");
    else {
        file_path += path;
        file_path += "/preprocessing";
        file_path += prj_name;
        file_path += "_all_paths.txt";
        DEBUG_MSG_PRE ("Current all_paths path filepath is " << file_path);
    }

    correlation_Info.reconvergence_path_map = find_reconvergence_paths(G, file_path, nodeMap);

    for (auto m1 : correlation_Info.reconvergence_path_map) {
        DBGMSG("\nSource: " <<  id2node[m1.first]);
        for (auto m2 : m1.second) {
            DBGMSG(" Dest: " <<  id2node[m2.first]);
            for (auto paths : m2.second){
                DBGMSG(" {");
                for (auto pa : paths) {
                    DBGMSG(" " << id2node[pa]);
                }
                DBGMSG("}\n\t\t");
            }
            DBGMSG("\n\t\t");
        }
    }
    getchar();

    generate_LUT_probability_of_error_functions (G, nodeMap); 
}

void load_combinational_path_positions (MyGraphType& G, int&  global_combinational_size, nodeMapType nodeMap, string input) {
    char path[256] = "";
    string file_path = "";
    vector<string> line_spl;
    string line;
    int vertex;

    if (getcwd(path, sizeof(path)) == NULL) // gets current directory
        perror("getcwd() error");
    else {
        file_path += path;
        file_path += "/preprocessing";
        file_path += input.substr(input.find('/'), (input.find(".txt")-input.find('/')));
        file_path += "_comb_path_position_num.txt";
    }

    //// Opens input file
    std::ifstream file(file_path);
    if (!file) {
        cout << "unable to open file" << file_path << endl;
        exit(0); // TODO: Improve error messages
    }

    while (getline(file, line)) {
        line_spl = split_string(line);
        int spl1i;

        std::istringstream iss (line_spl[1]);
        vertex = std::stoi(line_spl[0].substr(0, line_spl[0].length()-1));
        iss >> spl1i;

        if (global_combinational_size < spl1i)
            global_combinational_size = spl1i;
        G[vertex].combinational_path_position = spl1i;
    }
}

map<int,vector<int>> load_combinational_cones (string input) {

    char path[256] = "";
    string file_path = "", line;
    vector<string> line_spl;
    map<int,vector<int>> comb_cones;

    reconvergencePathMapType reconvergence_path_map;

    if (getcwd(path, sizeof(path)) == NULL) // gets current directory
        perror("getcwd() error");
    else {
        file_path += path;
        file_path += "/preprocessing";
        file_path += input.substr(input.find('/'), input.find(".txt")-6);
        file_path += "_comb_cones_from_sequential_elements_num.txt";
    }

    std::ifstream file(file_path);
    if (!file) {
        exit(0); // TODO: Improve error messages
    }

    while (getline(file, line)) {
        line_spl = split_string(line);
        vector<int> comb_c;

        if (line_spl[1].find("[]") ==-1) // if it is not an empty brackets, convert the number
            comb_c.push_back(std::stoi(line_spl[1].substr(1,line_spl[1].length()-2)));
        if(line_spl.size()>2)
            for(int i=2; i<line_spl.size();i++)
                if (line_spl[i].find("[]") ==-1) // if it is not an empty brackets, convert the number
                    comb_c.push_back(std::stoi(line_spl[i].substr(0, line_spl[i].length()-1)));
        if (line_spl[0].find("[]") ==-1) // if it is not an empty brackets, convert the number
            comb_cones[std::stoi(line_spl[0].substr(0,line_spl[0].length()-1))] = comb_c;
    }

    return comb_cones;
}

reconvergencePathMapType read_all_possible_reconvergent_paths_from_file(string input_file, vector<vertexDescType> list_of_sources, vector<vertexDescType> list_of_targets, nodeMapType nodeMap){
    std::ifstream file(input_file);
    vector<string> spl;
    reconvergencePathMapType complete_map;
    vertexDescType source_id, dest_id;
    
    if (!file) {
        cout << "unable to open file with all paths:" << input_file << endl;
        exit(0); // TODO: Improve error messages
    }
    string line;
    while (getline(file, line)) {
        cout << "line:"+line << endl;
        spl = split_string(line);

        cout << "spl:";
        source_id = nodeMap[spl[0]].id;
        dest_id = nodeMap[spl[spl.size()-1]].id;
        if(std::find(list_of_sources.begin(), list_of_sources.end(),source_id)!=list_of_sources.end()) {
            if(std::find(list_of_targets.begin(), list_of_targets.end(),dest_id)!=list_of_targets.end()) {
                vector<vertexDescType> path;
                for(auto elem : spl) {
                    cout << elem << " ";
                }
                cout << endl;
                for (int i = 1; i < spl.size()-1; ++i)    {
                    path.push_back(nodeMap[spl.at(i)].id);
                }
                complete_map[source_id][dest_id].push_back(path);
            }
        }
    }

    return complete_map;
}

void filter_reconvergent_paths(MyGraphType G, reconvergencePathMapType& rec_path, string input_file, nodeMapType nodeMap) {

    reconvergencePathCycleMapType recon_path_cycle;
    string exclude_list_path = input_file.substr(0,input_file.find("_all_paths.txt"));
    exclude_list_path+="_exclude_from_correlation_list.txt";
    std::ifstream file(exclude_list_path);
    string line;
    vector<string> exclude;

    getline(file, line); // get first line from exclude_list file
    exclude = split_string(line);

    DBGMSG("##        0000000000000000000000000000000      ####\n");
    for (auto m1 : rec_path) {
        DBGMSG("\nSource: " <<  id2node[m1.first]);    
        for (auto m2 : m1.second) {
            DBGMSG(" Dest: " <<  id2node[m2.first] << " ");
            for (auto path : m2.second) {
                DBGMSG(m2.second.size() << " {");
                for (auto pa : path) {
                    DBGMSG(" " << id2node[pa]);
                }
                DBGMSG("}\n\t\t   ");
            }
            DBGMSG("\n\t\t");
        }
    }
    DBGMSG("\n");
    getchar();

    /////////// EXCLUDE IF IN EXCLUDE LIST
    ////////// and REMOVE EMPTY and SINGLE PATHS
    for(auto m1 : rec_path){
        for (auto elem : exclude){
            if (rec_path.find(nodeMap[elem].id)!=rec_path.end()){
                rec_path.erase(nodeMap[elem].id);
            }
        }
        for(auto m2 : m1.second){
            for (auto elem : exclude){
                if (rec_path[m1.first].find(nodeMap[elem].id)!=rec_path[m1.first].end()) {
                    rec_path[m1.first].erase(nodeMap[elem].id);
                }
            }
            for (int i = 0; i < m2.second.size(); ++i) {
                if (m2.second.size()==1) { // if it is single path
                    rec_path[m1.first].erase(m2.first);
                    break;
                } 
                else {                        
                    if(m2.second.at(i).size()==0 && m2.second.size()==2) { // if it is single path and has empty path as well
                        rec_path[m1.first].erase(m2.first);
                        break;
                    }
                }
            }
        }
    }

    DBGMSG("##        11111111111111111111111111111111      ####\n");
    for (auto m1 : rec_path) {
        DBGMSG("\nSource: " <<  id2node[m1.first]);    
        for (auto m2 : m1.second) {
            DBGMSG(" Dest: " <<  id2node[m2.first] << " ");
            for (auto path : m2.second) {
                DBGMSG(m2.second.size() << " {");
                for (auto pa : path) {
                    DBGMSG(" " << id2node[pa]);
                }
                DBGMSG("}\n\t\t   ");
            }
            DBGMSG("\n\t\t");
        }
    }
    DBGMSG("\n");
    getchar();


    ////////// Correct the convergent destination
    // Ex.: For the 2 paths bellow:
    // 1. FSM_reg[0] FSM[1]_i_1 FSM_reg[1] u_i_1 u_reg 
    // 2. FSM_reg[0] FSM[2]_i_1 FSM_reg[2] u_i_1 u_reg 
    // They converge to the same vertex (u_i_1) and have 
    // one unecessary vertex in their path (u_reg).
    // Action: u_i_1 must be set as the new destination
    for(auto & m1 : rec_path){
        vertexDescType source = m1.first;
        for(auto & m2 : m1.second){
            vertexDescType dest = m2.first;
            for (int i = 0; i < m2.second.size()-1; ++i){
                bool decrement_i=false;
                vertexDescType last_vertex_i = m2.second.at(i).back();
                for (int j = i+1; j < m2.second.size()-1; ++j) {
                    vertexDescType last_vertex_j = m2.second.at(j).back();
                    if (m2.second.at(i).size() == m2.second.at(j).size()) {
                        if(last_vertex_i == last_vertex_j){
                            DBGMSG("Found the same last vertex for 2 paths!\n");
                            for (auto v : m2.second.at(i))
                                DBGMSG(v << " ");
                            DBGMSG("\n ");
                            for (auto v : m2.second.at(j))
                                DBGMSG(v << " ");
                            DBGMSG("\n ");

                            vector<vertexDescType> new_vi = m2.second.at(i);
                            DBGMSG("new_vi ");
                            for (auto v : new_vi)
                                DBGMSG(v << " ");
                            DBGMSG("\n ");
                            DBGMSG("new_vj ");
                            vector<vertexDescType> new_vj = m2.second.at(j);
                            for (auto v : new_vj)
                                DBGMSG(v << " ");
                            DBGMSG("\n ");
                            DBGMSG("new_vi poped ");
                            new_vi.pop_back(); // Remove the last vertex form vector ()
                            for (auto v : new_vi)
                                DBGMSG(v << " ");
                            DBGMSG("\n ");
                            DBGMSG("new_vj poped ");
                            new_vj.pop_back(); // Remove the last vertex form vector ()
                            for (auto v : new_vj)
                                DBGMSG(v << " ");
                            DBGMSG("\n ");

                            if(rec_path[source].find(last_vertex_j)!=rec_path[source].end()) { // if last_vertex_was already a destination
                                if (std::find(rec_path[source][last_vertex_j].begin(), rec_path[source][last_vertex_j].end(), new_vi) != rec_path[source][last_vertex_j].end()){ // new vector already exists as a path?
                                    DBGMSG("Vector already in map!\n");
                                } else {
                                    DBGMSG("Vector " << i << "was not in map!\n");
                                    rec_path[source][last_vertex_j].push_back(new_vi);
                                }
                                if (std::find(rec_path[source][last_vertex_j].begin(), rec_path[source][last_vertex_j].end(), new_vj) != rec_path[source][last_vertex_j].end()){ // new vector already exists as a path?
                                    DBGMSG("Vector already in map!\n");
                                } else {
                                    DBGMSG("Vector " << j << "was not in map!\n");
                                    rec_path[source][last_vertex_j].push_back(new_vj);
                                }
                            } else {  // if it was not a destination, include it as a new one
                                DBGMSG("Was not a destination!\n");
                                rec_path[source][last_vertex_j].push_back(new_vi);
                                rec_path[source][last_vertex_j].push_back(new_vj);
                            }        
                            DBGMSG("Before Erase: " <<  m2.second.size() << "\n ");   
                            vector<vector<vertexDescType>>::iterator it = m2.second.begin();
                            for (int x = 0; x < j; ++x){
                                ++it;
                            }                         
                            m2.second.erase(it);
                            DBGMSG("After Erase: " <<  m2.second.size() << "\n ");                            
                            j--;
                            decrement_i=true;
                        }
                    }
                }
                if(decrement_i) {
                    DBGMSG("Before Erase: " <<  m2.second.size() << "\n ");                            
                    vector<vector<vertexDescType>>::iterator it = m2.second.begin();
                    for (int x = 0; x < i; ++x){
                        ++it;
                    }                         
                    m2.second.erase(it);
                    DBGMSG("After Erase: " <<  m2.second.size() << "\n ");                            
                    i--;
                }
            }
        }
    }

    DBGMSG("##        BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB      ####\n");
    for (auto m1 : rec_path) {
        DBGMSG("\nSource: " <<  id2node[m1.first]);    
        for (auto m2 : m1.second) {
            DBGMSG(" Dest: " <<  id2node[m2.first] << " ");
            for (auto path : m2.second) {
                DBGMSG(m2.second.size() << " {");
                for (auto pa : path) {
                    DBGMSG(" " << id2node[pa]);
                }
                DBGMSG("}\n\t\t   ");
            }
            DBGMSG("\n\t\t");
        }
    }
    DBGMSG("\n");
    getchar();


    ////////// ORGANIZE REMAINING PATHS BY CLOCK CYCLES COUNT
    for(auto m1 : rec_path){
        vertexDescType source = m1.first;
        for(auto m2 : m1.second){
            vertexDescType dest = m2.first;
            if (m2.second.size()>0) {
                for(auto vec : m2.second){
                    if (vec.size()>0) {
                        int cycle_cnt=0;
                        for(auto vertex : vec) {
                            if(G[vertex].component_type==Sequential){
                                cycle_cnt++;
                            }
                        }
                        cout << cycle_cnt << " ";
                        recon_path_cycle[source][dest][cycle_cnt].push_back(vec);
                    }
                }
            }
        }
    }

    ////////// print resulting reconvergent paths
    DBGMSG("### 222222222222222222222222222222222222222222222 ###\n");
    for (auto m1 : recon_path_cycle) {
        DBGMSG("\nSource: " <<  id2node[m1.first]);    
        for (auto m2 : m1.second) {
            DBGMSG(" Dest: " <<  id2node[m2.first] << " " << m2.second.size());
            for (auto m3 : m2.second){
                    DBGMSG("\n\t\t" <<  m3.first << ": ");
                    for (auto path : m3.second) {
                        DBGMSG(m3.second.size() << " {");
                        for (auto pa : path) {
                            DBGMSG(" " << id2node[pa]);
                        }
                        DBGMSG("}\n\t\t   ");
                    }
            }
            DBGMSG("\n\t\t");
        }
    }
    DBGMSG("\n");
    getchar();

    ////////// REMOVE SINGLE PATH AFTER ORGANIZED BY CLOCK CYCLES COUNT
    for (auto m1 : recon_path_cycle) {
        for (auto m2 : m1.second) {
            if (m2.second.size()==1) {
                recon_path_cycle[m1.first].erase(m2.first);
            }
            for (auto m3 : m2.second){
                if (m3.second.size()==1) {
                    recon_path_cycle[m1.first][m2.first].erase(m3.first);
                }
            }
        }
    }

    ////////// print resulting reconvergent paths
    DBGMSG("### 333333333333333333333333333333333333333333333 ###\n");
    for (auto m1 : recon_path_cycle) {
        DBGMSG("\nSource: " <<  id2node[m1.first]);    
        for (auto m2 : m1.second) {
            DBGMSG(" Dest: " <<  id2node[m2.first] << " " << m2.second.size());
            for (auto m3 : m2.second){
                    DBGMSG("\n\t\t" <<  m3.first << ": ");
                    for (auto path : m3.second) {
                        DBGMSG(m3.second.size() << " {");
                        for (auto pa : path) {
                            DBGMSG(" " << id2node[pa]);
                        }
                        DBGMSG("}\n\t\t   ");
                    }
            }
            DBGMSG("\n\t\t");
        }
    }
    DBGMSG("\n");
    getchar();

    ////////// REMOVE REMAINING EMPTY source->dest PAIRS
    for (auto m1 : recon_path_cycle) {
        for (auto m2 : m1.second) {
            if (m1.second.size()==1 && m2.second.size()==0){
                recon_path_cycle.erase(m1.first);
            } else {
                if (m2.second.size()==0) {
                    recon_path_cycle[m1.first].erase(m2.first);
                }
            }
        }
    }



    ////////// print resulting reconvergent paths
    DBGMSG("##########################################################\n");
    for (auto m1 : recon_path_cycle) {
        DBGMSG("\nSource: " <<  id2node[m1.first]);    
        for (auto m2 : m1.second) {
            DBGMSG(" Dest: " <<  id2node[m2.first] << " " << m2.second.size());
            for (auto m3 : m2.second){
                    DBGMSG("\n\t\t" <<  m3.first << ": ");
                    for (auto path : m3.second) {
                        DBGMSG(m3.second.size() << " {");
                        for (auto pa : path) {
                            DBGMSG(" " << id2node[pa]);
                        }
                        DBGMSG("}\n\t\t   ");
                    }
            }
            DBGMSG("\n\t\t");
        }
    }
    DBGMSG("\n");
    getchar();
}

reconvergencePathMapType find_reconvergence_paths(MyGraphType G, string input_file, nodeMapType nodeMap) {
    oEdge_it out_i, out_end;
    vector<vertexDescType> list_of_sources;
    vector<vertexDescType> list_of_targets;
    reconvergencePathMapType m, reconvergent_paths;
    for(pair<vertexIt,vertexIt> it = boost::vertices(G); it.first != it.second; ++it.first) {
        if ( (id2node[*it.first]!="clock") && (id2node[*it.first]!="clk") && (id2node[*it.first]!="reset") && (id2node[*it.first]!="rst") && 
           (id2node[*it.first]!="CLOCK") && (id2node[*it.first]!="CLK") && (id2node[*it.first]!="RESET") && (id2node[*it.first]!="RST") && 
           (id2node[*it.first]!="VCC") && (id2node[*it.first]!="VCC2") && (id2node[*it.first]!="GND") ) {
            auto out_deg  = out_degree(*it.first, G);
            DEBUG_MSG(id2node[*it.first] << " out_deg= " << out_deg);
            if(out_degree(*it.first, G)>1){
                if(G[*it.first].prim!=CARRY4 &&  G[*it.first].prim!=CARRY8){
                    list_of_sources.push_back(*it.first);
                    DEBUG_MSG("source added= "<< id2node[*it.first]);
                } else {
                    ;// if it's a CARRY... special treatment is needed
                }
            }
            DEBUG_MSG(id2node[*it.first] << " in_deg= " << in_degree(*it.first, G));
            if(in_degree(*it.first, G)>1){
                list_of_targets.push_back(*it.first);
                DEBUG_MSG("target added= "<< id2node[*it.first]);
            }
        }
    }

    reconvergent_paths = read_all_possible_reconvergent_paths_from_file(input_file, list_of_sources, list_of_targets, nodeMap);

    filter_reconvergent_paths(G, reconvergent_paths, input_file, nodeMap);

    return reconvergent_paths;
}

vector<errorValuesModel> gen_lut_prob_func_matrix_not_minimized (long long int func, int lutsize) {
    vector <errorValuesModel> val {C0 , C1 , W0 , W1};
    errorValuesModel current_inputs[lutsize];
    vector<string> correlated_value_expression;
    bitset<2> l1func(func);
    bitset<4> l2func(func);
    bitset<8> l3func(func);
    bitset<16> l4func(func);
    bitset<32> l5func(func);
    bitset<64> l6func(func);
    errorValuesModel val1[4], val2[16], val3[64], val4[256], val5[1024], val6[4096];
    vector<errorValuesModel> ret;
    int right_bit=0, wrong_bit=0; // right and wrong result bit
    string p1c_s ="",p0c_s ="",p1w_s ="",p0w_s ="";

    for (long int i = 0; i < pow(2,(lutsize*2)); i++) {
        for (size_t j = 0; j < lutsize; j++) {
            if (j==0){
                current_inputs[0] = val.at(i%4);
            } else {
                current_inputs[j] = val.at((int(i/(pow(4,j))))%4);
            }
        }

        right_bit=0;
        wrong_bit=0;
        for (int j = lutsize-1; j >=0 ; j--) {
            switch (current_inputs[j]) {
                case W1:
                    if (j!=0) {
                        wrong_bit+=pow(2,j);
                    } else {
                        wrong_bit+=1;
                    }
                    break;
                case C1:
                    if (j!=0) {
                        right_bit+=pow(2,j);
                        wrong_bit+=pow(2,j);
                    } else {
                        right_bit+=1;
                        wrong_bit+=1;
                    }
                    break;
                case W0:
                    if (j!=0) {
                        right_bit+=pow(2,j);
                    } else {
                        right_bit+=1;
                    }
                    break;
                case C0:
                    break;
            }
        }

        switch (lutsize) {
            case 1:
                if(l1func[right_bit]==l1func[wrong_bit]) {
                    if (l1func[right_bit]==1) {
                        ret.push_back(C1);
                    } else {
                        ret.push_back(C0);
                    }
                } else {
                    if (l1func[wrong_bit]==1){
                        ret.push_back(W1);
                    } else {
                        ret.push_back(W0);
                    }
                }
            break;
            case 2:
                if(l2func[right_bit]==l2func[wrong_bit]){
                    if (l2func[right_bit]==1){
                        ret.push_back(C1);
                    } else {
                        ret.push_back(C0);
                    }
                } else {
                    if (l2func[wrong_bit]==1){
                        ret.push_back(W1);
                    } else {
                        ret.push_back(W0);
                    }
                }
                break;
            case 3:
            if(l3func[right_bit]==l3func[wrong_bit]){
                if (l3func[right_bit]==1){
                    ret.push_back(C1);
                } else {
                    ret.push_back(C0);
                }
            } else {
                if (l3func[wrong_bit]==1){
                    ret.push_back(W1);
                } else {
                    ret.push_back(W0);
                }
            }
            break;
            case 4:
            if(l4func[right_bit]==l4func[wrong_bit]){
                if (l4func[right_bit]==1){
                    ret.push_back(C1);
                } else {
                    ret.push_back(C0);
                }
            } else {
                if (l4func[wrong_bit]==1){
                    ret.push_back(W1);
                } else {
                    ret.push_back(W0);
                }
            }
            break;
            case 5:
            if(l5func[right_bit]==l5func[wrong_bit]){
                if (l5func[right_bit]==1){
                    ret.push_back(C1);
                } else {
                    ret.push_back(C0);
                }
            } else {
                if (l5func[wrong_bit]==1){
                    ret.push_back(W1);
                } else {
                    ret.push_back(W0);
                }
            }
            break;
            case 6:
            if(l6func[right_bit]==l6func[wrong_bit]){
                if (l6func[right_bit]==1){
                    ret.push_back(C1);
                } else {
                    ret.push_back(C0);
                }
            } else {
                if (l6func[wrong_bit]==1){
                    ret.push_back(W1);
                } else {
                    ret.push_back(W0);
                }
            }
            break;
        }
    }

    return ret;
}

