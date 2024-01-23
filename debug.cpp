 /**
  * @file  debug.cpp
  * @date Jul 24, 2019
  * @author Eduardo Luis Rhod 
 */

#include "types.hpp"
#include "debug.hpp"

void clrscr(){
    cout << "\033[2J\033[1;1H";
}

void run_print(MyGraphType G, circuitInfo cif, int cycle, vector<int> watch_list) {
    map<int,errorProbabilityType> print_list_inports, print_list_comb, print_list_seq, print_list_port;
    oEdge_it out_i, out_end;
    iEdge_it in_i, in_end;
    int col =4;

	// clrscr();
    for (auto elem : cif.inPorts){
        for (tie(out_i, out_end) = boost::out_edges(elem, G); out_i != out_end; ++out_i) {
            if (G[*out_i].NETP0CORRECT!=-1){
                if (print_list_inports.find(elem)==print_list_inports.end()){
                    // cout << elem << endl;
                    print_list_inports[elem] = G[*out_i].net_failure_probability;
                }
            }
        }
    }

    cout << "|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|" << endl;
    																																																  
	// ##############################################################################################################
	// ##### OBS.: IMPORTANT TO REMEMBER THAT IN THIS VERSION BUS NETS ARE NOT ENTIRELY PRINTED. ONLY THE FIRST NET WITH ERROR IS PRINTED.
	// ########### This happens specially for CARRY8 instances that have 2 buses as outputs
	// ##############################################################################################################
	cout << "|---  INPUTS                                                                                                                                                                 -----------|" << endl;
    for (size_t i = 0; i < col; i++) {
        cout << "|-------------------------- PC0  PC1  PW0  PW1";
    }
    cout << "|" << endl;
    string st;
    st="|";
    char buff[100], buff2[100];
    std::string buffAsStdStr = buff;
    int i=0;
    for (auto elem : print_list_inports) {
        // cout << id2node[elem.first].c_str() << " " << elem.second.first << " " << elem.second.second << endl;
        snprintf(buff, sizeof(buff), "%2.1f %2.1f %2.1f %2.1f", (100*elem.second.P0CORRECT), (100*elem.second.P1CORRECT), (100*elem.second.P0WRONG), (100*elem.second.P1WRONG));
        snprintf(buff2, sizeof(buff2), "%24.24s %20.20s|", id2node[elem.first].c_str(), buff);

		st+=buff2;
		if ((i+1)%col==0){
			cout << st << endl;
			st="|";
        }else{
			if ((i+1) == print_list_inports.size()){
                cout << st << endl;
            }
        }
        i++;
    }

    cout << "|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|" << endl;
	cout << "|---  COMBINATIONAL                                                                                                                                                          -----------|" << endl;
    for (size_t i = 0; i < col; i++) {
        cout << "|-------------------------- PC0  PC1  PW0  PW1";
    }
    cout << "|" << endl;

    st="|";

    for (auto elem : cif.combinational_list) {
        for (tie(out_i, out_end) = boost::out_edges(elem, G); out_i != out_end; ++out_i) {
            if (G[*out_i].NETP0CORRECT!=-1){
                if (print_list_comb.find(elem)==print_list_comb.end()){
                    print_list_comb[elem] = G[*out_i].net_failure_probability;
                }
            }
        }
    }
    i=0;
    for (auto elem : print_list_comb) {
        snprintf(buff, sizeof(buff), "%2.1f %2.1f %2.1f %2.1f",(100*elem.second.P0CORRECT), (100*elem.second.P1CORRECT), (100*elem.second.P0WRONG), (100*elem.second.P1WRONG));
        snprintf(buff2, sizeof(buff2), "%24.24s %20.20s|", id2node[elem.first].c_str(), buff);
		st+=buff2;
		if ((i+1)%col==0){
			cout << st << endl;
			st="|";
        }else{
			if ((i+1) == print_list_comb.size()){
                cout << st << endl;
            }
        }
        i++;
    }

    cout << "|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|" << endl;
	cout << "|---  SEQUENTIAL                                                                                                                                                     -------------------|" << endl;
    for (size_t i = 0; i < col; i++) {
        cout << "|-------------------------- PC0  PC1  PW0  PW1";
    }
    cout << "|" << endl;
	st="|";

    for (auto elem : cif.sequential_list){
        for (tie(out_i, out_end) = boost::out_edges(elem, G); out_i != out_end; ++out_i) {
            if (G[*out_i].NETP0CORRECT!=-1){
                if (print_list_seq.find(elem)==print_list_seq.end()){
                    print_list_seq[elem] = G[*out_i].net_failure_probability;
                }
            }
        }
    }

    i=0;
    for (auto elem : print_list_seq) {
        snprintf(buff, sizeof(buff), "%2.1f %2.1f %2.1f %2.1f",(100*elem.second.P0CORRECT), (100*elem.second.P1CORRECT), (100*elem.second.P0WRONG), (100*elem.second.P1WRONG));
        snprintf(buff2, sizeof(buff2), "%24.24s %20.20s|", id2node[elem.first].c_str(), buff);
		st+=buff2;
		if ((i+1)%col==0){
			cout << st << endl;
			st="|";
        }else{
			if ((i+1) == print_list_seq.size()){
                cout << st << endl;
            }
        }
        i++;
    }

    cout << "|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|" << endl;
	cout << "|---  PORTS                                                                                                                                                          -------------------|" << endl;
    for (size_t i = 0; i < col; i++) {
        cout << "|-------------------------- PC0  PC1  PW0  PW1";
    }
    cout << "|" << endl;
	st="|";

    for (auto elem : cif.outPorts){
        for (tie(in_i, in_end) = boost::in_edges(elem, G); in_i != in_end; ++in_i) {
            if (G[*in_i].NETP0CORRECT!=-1){
                if (print_list_port.find(elem)==print_list_port.end()){

                    print_list_port[elem] = G[*in_i].net_failure_probability;
                }
            }
        }
    }

    i=0;
    for (auto elem : print_list_port) {
        snprintf(buff, sizeof(buff), "%2.1f %2.1f %2.1f %2.1f", (100*elem.second.P0CORRECT), (100*elem.second.P1CORRECT), (100*elem.second.P0WRONG), (100*elem.second.P1WRONG));
        snprintf(buff2, sizeof(buff2), "%24.24s %20.20s|", id2node[elem.first].c_str(), buff);
		st+=buff2;
		if ((i+1)%col==0){
			cout << st << endl;
			st="|";
        }else{
			if ((i+1) == print_list_port.size()){
                cout << st << endl;
            }
        }
        i++;
    }

	cout << "|---  cycle = "<< cycle << " -----------------------------------------------------------------------------------------------------------------------------------------------------------|" << endl;

    getchar();
}

void print_vector (vector<int> v, string st){
    cout << endl;
    for (auto i : v){
        cout << st << " " << id2node[i] << " ";
    }
    cout << endl;
}

void print_map (map<string,errorProbabilityType> iMap) {
    for (auto elem : iMap){
        cout << elem.first << ":" << elem.second.P0CORRECT << " " << elem.second.P1CORRECT << " " << elem.second.P0WRONG << " " << elem.second.P1WRONG << endl;
    }
}

vector<int> generate_watchlist(MyGraphType G, int source, int target){
    oEdge_it out_i, out_end;
    iEdge_it in_i, in_end;
    vector<int> todo_list, final_list;
    map<int,int> from_source_nodes_dic, from_target_nodes_dic;
    int node;


    // Creating the cone from source to target
    for (tie(out_i, out_end) = boost::out_edges(source, G); out_i != out_end; ++out_i){
        // node = print_list_port.push_back(make_pair(elem,G[*in_i].net_failure_probability));
        node = boost::target(*out_i,G);
        todo_list.push_back(node);
        from_source_nodes_dic[node] = node;
    }

    while (!todo_list.empty()) {
        int next_node = todo_list[todo_list.size()-1]; todo_list.pop_back();
        from_source_nodes_dic[next_node] = next_node;
        // get next_node predecessors and add to list and dictionary
        if (next_node == target || G[next_node].prim==OUTPORT){
            ;
        } else {
            for (tie(out_i, out_end) = boost::out_edges(next_node, G); out_i != out_end; ++out_i){
                node = boost::target(*out_i,G);
                // check if node is not in dictionary
                if (from_source_nodes_dic.find(node) == from_source_nodes_dic.end()){ //not in the dictionary...
                    //  not processed... then then put node in the list0
                    todo_list.push_back(node);
    }}}}

    // Creating the cone from target to source
    for (tie(in_i, in_end) = boost::in_edges(target, G); in_i != in_end; ++in_i){
        node = boost::source(*in_i,G);
        todo_list.push_back(node);
        from_target_nodes_dic[node] = node;
    }

    while (!todo_list.empty()) {
        int next_node = todo_list[todo_list.size()-1]; todo_list.pop_back();
        from_target_nodes_dic[next_node] = next_node;
        // get next_node predecessors and add to list and dictionary
        if (next_node == source || G[next_node].prim == INPORT){
            ;
        } else {
            for (tie(in_i, in_end) = boost::in_edges(next_node, G); in_i != in_end; ++in_i){
                node = boost::source(*in_i,G);
                // check if node is not in dictionary
                if (from_target_nodes_dic.find(node) == from_target_nodes_dic.end()){ //not in the dictionary...
                    //  not processed... then then put node in the list0
                    todo_list.push_back(node);
    }}}}

    for (auto elem : from_source_nodes_dic){
        if (from_target_nodes_dic.find(elem.first) != from_target_nodes_dic.end())
            final_list.push_back(elem.first);
    }

    return final_list;
}

void print_out_edge (string st ,double p0wrong, double p1wrong, double p0correct, double p1correct ) {
        cout << "OutEdge:" << st << " <PC0,PC1,PW0,Pw1>" << "<"<<p0correct<<","<<p1correct<<","<<p0wrong<<","<<p1wrong<<">" << endl;
}
