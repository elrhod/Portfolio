 /**
  * @file   scape.cpp
  * @date   Jun 10, 2019
  * @author Eduardo Luis Rhod 
 */

#include <string>
// #include <fstream>

#include <unistd.h>// #include <filesystem>
#include <sstream>
#include <iomanip>
#include <bits/stdc++.h>

#include <boost/graph/properties.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/copy.hpp>
#include <stack>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <pthread.h>
#include "scape.hpp"

#include "types.hpp"
#include "debug.hpp"
#include "tools.hpp"
#include "pos_processing.hpp"
#include "pre_processing.hpp"
#include "scape_utils.hpp"
#define ARGS_NUM_MAX 400

#define DEBUG ON
 
#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

// Global Map for fast conversion
int luts_in_thread_group;
int last_lut;
map <int, string> id2node;
string dbg_lut = "coda2[2]_i_1";
int dbgflag= 0;

void set_out_edge_error_probability_values (MyGraphType& G, int vertex, double p0wrong, double p1wrong, double p0correct, 
    double p1correct, string st , map<string,errorProbabilityType> omap, vector<pair<double,errorValuesModel>> correlated_value) {
    oEdge_it out_i, out_end;
    if (correlated_value.size()>0){
        DEBUG_MSG("correlated_value[" << id2node[vertex] << "]");
        for(auto item : correlated_value) {
            cout << " " << item.first << ":" << item.second;
        }
        cout << endl;
    }
    if(dbgflag==1)
        cout << "writing output values for: " << vertex << ":" << id2node[vertex] << endl;
    if (omap.size()==0){ // only one output (most cases)
        if (st=="NET") {
            for (tie(out_i, out_end) = boost::out_edges(vertex, G); out_i != out_end; ++out_i) {
                G[*out_i].NETP0CORRECT=p0correct;
                G[*out_i].NETP1CORRECT=p1correct;
                G[*out_i].NETP0WRONG=p0wrong;
                G[*out_i].NETP1WRONG=p1wrong;
                G[*out_i].correlated_value = correlated_value;
                if(dbgflag==1)
                    print_out_edge (G[*out_i].NODEOUTEDGE, p0wrong, p1wrong, p0correct, p1correct);
            }
        } else {
            if (st=="NEXT") {
                for (tie(out_i, out_end) = boost::out_edges(vertex, G); out_i != out_end; ++out_i) {
                    G[*out_i].next_net_failure_probability.P0CORRECT = p0correct;
                    G[*out_i].next_net_failure_probability.P1CORRECT = p1correct;
                    G[*out_i].next_net_failure_probability.P0WRONG = p0wrong;
                    G[*out_i].next_net_failure_probability.P1WRONG = p1wrong;
                    G[*out_i].correlated_value = correlated_value;
                    if(dbgflag==1)
                        print_out_edge (G[*out_i].NODEOUTEDGE+"NEXT ", p0wrong, p1wrong, p0correct, p1correct);
                }
            } else {
                cout << "Error, single net specification " << st << " not treated!" << endl;
                exit(1);
            }
        }
    } else { // When you have more than 1 output (carry8 and carry4 components)
        if (st=="NET") {
            for (tie(out_i, out_end) = boost::out_edges(vertex, G); out_i != out_end; ++out_i) {
                string name = G[*out_i].out_pin_name;
                if (name.size()>0){
                    if (name.find("CO[") != -1){
                        string oport = "CO["+std::to_string(name.at(3))+"]";
                        G[*out_i].NETP0CORRECT==omap[oport].P0CORRECT;
                        G[*out_i].NETP1CORRECT=omap[oport].P1CORRECT;
                        G[*out_i].NETP0WRONG=omap[oport].P0WRONG;
                        G[*out_i].NETP1WRONG=omap[oport].P1WRONG;
                        G[*out_i].correlated_value = correlated_value;
                    } else {
                        if (name.find("O[") != -1){
                            string oport = "O["+std::to_string(name.at(2))+"]";
                            G[*out_i].NETP0CORRECT==omap[oport].P0CORRECT;
                            G[*out_i].NETP1CORRECT=omap[oport].P1CORRECT;
                            G[*out_i].NETP0WRONG=omap[oport].P0WRONG;
                            G[*out_i].NETP1WRONG=omap[oport].P1WRONG;
                            G[*out_i].correlated_value = correlated_value;
                        } else {
                            cout << "Error, multiple outputs net specification " << st << " name=" << name << " not treated!" << endl;
                            exit(1);
                        }
                    }
                }
            }
        }
    }
}

vector<float> calc_lut_filter_probability (unsigned long long int func, int lut_size) {
    vector<int> b(pow(2,lut_size));
    b = decToBinary(func,pow(2,lut_size));
    int inp = lut_size;
    float nbits = pow(2,lut_size);
    vector<float> pnf(inp, 0.0); // all elements = 0.0
    int ctrl=0;
    int dev,rep,jmp;

    for (int z=0; z<inp; z++){
        dev= pow(2,z);
        rep= pow(2,z);
        jmp= 2*(pow(2,z));
        int cicle =0;
        float prob =0.0;
        for (int y=0; y<nbits/2;y++){

            if(b[(cicle*jmp+ctrl)]==b[(cicle*jmp+ctrl+dev)]){
                prob = prob+1;
            }
            if (ctrl<(rep-1)){
                ctrl=ctrl+1;
            } else {
                ctrl=0;
                cicle=cicle+1;
            }
        }

        pnf[z]=1-(prob/(nbits/2));
    }
    return pnf;
}

template <typename anyType>
void set_all_edges_attribute (MyGraphType& G, anyType value, enum EdgeAtribTypeEnum attribute_name){

    switch (attribute_name) {
        case NET_FAILURE_PROBABILITY:
            for(pair<edgeIt,edgeIt> it = boost::edges(G); it.first != it.second; ++it.first) {
                G[*it.first].NETP0CORRECT=value;
                G[*it.first].NETP1CORRECT=value;
                G[*it.first].NETP0WRONG=value;
                G[*it.first].NETP1WRONG=value;
            }
            break;
        case NEXT_NET_FAILURE_PROBABILITY:
            for(pair<edgeIt,edgeIt> it = boost::edges(G); it.first != it.second; ++it.first) {
                G[*it.first].next_net_failure_probability.P0CORRECT=value;
                G[*it.first].next_net_failure_probability.P1CORRECT=value;
                G[*it.first].next_net_failure_probability.P0WRONG=value;
                G[*it.first].next_net_failure_probability.P1WRONG=value;
            }
            break;
        case PREVIOUS_NET_FAILURE_PROBABILITY:
            for(pair<edgeIt,edgeIt> it = boost::edges(G); it.first != it.second; ++it.first) {
                G[*it.first].previous_net_failure_probability.P0CORRECT=value;
                G[*it.first].previous_net_failure_probability.P1CORRECT=value;
                G[*it.first].previous_net_failure_probability.P0WRONG=value;
                G[*it.first].previous_net_failure_probability.P1WRONG=value;
            }
            break;
        default:
            cout << "Error in set_all_edges_attribute!!!" << endl;
            exit(1);
    }
}

void error_prob_init (MyGraphType& G, int error_source) {

    oEdge_it out_i, out_end;

    // ####################################################################
	// ### Initialization
	// # All nets start with -1 to indicate they are blank
    set_all_edges_attribute(G, -1, NET_FAILURE_PROBABILITY);
    set_all_edges_attribute(G, -1, PREVIOUS_NET_FAILURE_PROBABILITY);
    set_all_edges_attribute(G, -1, NEXT_NET_FAILURE_PROBABILITY);
    // get_sequential_fps(G, previous_sequential_fps);

    // ### Initialize the error source
    for (tie(out_i, out_end) = boost::out_edges(error_source, G); out_i != out_end; ++out_i) {
        G[*out_i].NETP0CORRECT=0;
        G[*out_i].NETP1CORRECT=0;
        G[*out_i].NETP0WRONG=0.5;
        G[*out_i].NETP1WRONG=0.5;
    }
}

void calculate_output_probabilities(MyGraphType& G, int error_source, vector<int> faulty_list, correlationInfo correlation_Info, int current_cycle) {
    map <string, errorProbabilityType > pf_imap, omap; // inputs probability of failure map
    iEdge_it in_i, in_end;
    oEdge_it out_i, out_end;
    double pf_i=0, pf_o=0, cyinit=0, ci=0, ci_top=0, p0c_i, p1c_i, p0w_i, p1w_i;
    vector<double> pf_co{0,0,0,0,0,0,0,0};
    vector<double> pf_o_v{0,0,0,0,0,0,0,0};
    vector<double> pf_di{0,0,0,0,0,0,0,0};
    vector<double> pf_s{0,0,0,0,0,0,0,0};
    map <string, vector<pair<double,errorValuesModel>>> correlated_value_map;
    static bool first_time=true;
    static map<int,int> correlated_sources_map;
    static map<int,vector<string>> convergent_destination;
    static map<vertexDescType, int> cycle_of_correlation_activation;
    cout.precision(17);

    if (first_time){
#ifdef DEBUG
        for (auto m1 : correlation_Info.reconvergence_path_map) {
            cout << "Source: " <<  id2node[m1.first];
            for (auto m2 : m1.second) {
                cout << "\n\t Dest: " <<  id2node[m2.first];
                for (auto path : m2.second){
                    cout << " {";
                    for (auto pa : path) {
                        cout << " " << id2node[pa];
                    }
                    cout << " }";
                }
            }
            cout << endl;
        }
#endif
        for (auto m1 : correlation_Info.reconvergence_path_map) {
            for (auto m2 : m1.second) {
                for (auto path : m2.second){
                    set_cycle_of_correlation_activation(G, path, cycle_of_correlation_activation, m2.first); 
                }
            }
        }

        first_time=false;
        for (auto m1 : correlation_Info.reconvergence_path_map){
            for (auto m2 : m1.second){
                vector<string> in_pins;
                vertexDescType target = m2.first;
                for (size_t i = 0; i < m2.second.size(); i++) {
                    vertexDescType first_lut_in_path = get_first_lut_in_vector(G, m2.second.at(i));
                    vertexDescType last_vertex_in_path = m2.second.at(i).at(m2.second.at(i).size()-1);
                    pair<edgeDescType, bool> my_edge = boost::edge(last_vertex_in_path, target, G);
                    in_pins.push_back(G[my_edge.first].PININEDGE);
                    correlated_sources_map[first_lut_in_path]=first_lut_in_path;
                }
                convergent_destination[target]=in_pins;
            }
        }

#ifdef DEBUG
        cout << "cycle_of_correlation_activation: ";
        for (auto m1 : cycle_of_correlation_activation) {
            cout << id2node[m1.first] << ":" << m1.second << endl;
        }

        cout << "correlated_sources_map: ";
        for (auto m1 : correlated_sources_map) {
            cout << " " <<  id2node[m1.first];
        }
        cout << endl;
        cout << "convergent_destination: ";
        for (auto m1 : convergent_destination) {
            cout  <<  id2node[m1.first] << "{ ";
            for (auto m2 : m1.second){
                cout  <<  m2 << " ";
            }
            cout << " }" << endl;
        }
#endif

    }

    for(auto const& elem : faulty_list) {

        vector<pair<double,errorValuesModel>> correlated_value;

        DEBUG_MSG("- " << elem);
        if (id2node[elem]==dbg_lut){
            dbgflag =1;
        }

        if ((G[elem].prim == IBUF) || (G[elem].prim == OBUF)) {
            if(get_input_fp_map(G, elem, pf_imap, 0, correlated_value_map)){
                // # calculate the pf
    			// ###################### PROBABILITY FUNCTION #################
                // output = input
    			// #############################################################
    			// # update output nets
                omap.clear();
                // CORRELATED VALUE IS ONLY BEING FORWARDED FROM 'I' TO 'O'
                set_out_edge_error_probability_values (G, elem, pf_imap["I"].P0WRONG, pf_imap["I"].P1WRONG, pf_imap["I"].P0CORRECT, pf_imap["I"].P1CORRECT, "NET", omap, correlated_value_map["I"] );
            }
        } else {
            if ((G[elem].prim == BUFGCE)) {
                // OBS.: ATTRIBUTES ARE NOT BEING TARGET IN THIS VERSION
                // Get input PF (Probability of Failure)
                if(get_input_fp_map(G, elem, pf_imap, 0, correlated_value_map)) {
                    // print_map(pf_imap);
                    // P0CORRECT
                    p0c_i = pf_imap["CE"].P0CORRECT + (pf_imap["CE"].P1CORRECT*pf_imap["I"].P0CORRECT);
                    // P1CORRECT
                    p1c_i = pf_imap["CE"].P1CORRECT*pf_imap["I"].P1CORRECT;
                    // P0WRONG
                    p0w_i = pf_imap["CE"].P0WRONG + pf_imap["CE"].P1WRONG*(pf_imap["I"].P0CORRECT + pf_imap["I"].P0WRONG) + pf_imap["CE"].P1CORRECT*pf_imap["I"].P0WRONG;
                    // P1WRONG
                    p1w_i = pf_imap["I"].P1WRONG*(pf_imap["CE"].P1CORRECT + pf_imap["CE"].P1WRONG);
                    // #############################################################
                    // # update output nets
                    omap.clear();
                    // CORRELATED VALUE IS ONLY BEING FORWARDED FROM 'I' TO 'O'
                    set_out_edge_error_probability_values (G, elem, p0w_i, p1w_i, p0c_i, p1c_i, "NET", omap, correlated_value_map["I"]);
                }
            } else {
                if (G[elem].prim == FDRE) {
                    // OBS.: ATTRIBUTES ARE NOT BEING TARGET IN THIS VERSION
                    // Get input PF (Probability of Failure)
                    if(get_input_fp_map(G, elem, pf_imap, 1, correlated_value_map)) {
                        // print_map(pf_imap);
                        double PR0CE0 = pf_imap["R"].P0CORRECT*pf_imap["CE"].P0CORRECT;
                        double PR0CE0W = pf_imap["R"].P0CORRECT*pf_imap["CE"].P0WRONG;
                        double PQ0_Q0W = pf_imap["Q"].P0CORRECT + pf_imap["Q"].P0WRONG;
                        double PQ1_Q1W = pf_imap["Q"].P1CORRECT + pf_imap["Q"].P1WRONG;
                        double PR0WCE0 = pf_imap["R"].P0WRONG*pf_imap["CE"].P0CORRECT;
                        double PR0WCE0W = pf_imap["R"].P0WRONG*pf_imap["CE"].P0WRONG;
                        double PR0CE1 = pf_imap["R"].P0CORRECT*pf_imap["CE"].P1CORRECT;
                        double PR0CE1W = pf_imap["R"].P0CORRECT*pf_imap["CE"].P1WRONG;
                        double PD0_D0W = pf_imap["D"].P0CORRECT + pf_imap["D"].P0WRONG;
                        double PD1_D1W = pf_imap["D"].P1CORRECT + pf_imap["D"].P1WRONG;
                        double PR0WCE1 = pf_imap["R"].P0WRONG*pf_imap["CE"].P1CORRECT;
                        double PR0WCE1W = pf_imap["R"].P0WRONG*pf_imap["CE"].P1WRONG;
                        // P0CORRECT
                        p0c_i = pf_imap["R"].P1CORRECT + PR0CE0*pf_imap["Q"].P0CORRECT + PR0CE1*pf_imap["D"].P0CORRECT;
                        // P1CORRECT
                        p1c_i = PR0CE0*pf_imap["Q"].P1CORRECT + PR0CE1*pf_imap["D"].P1CORRECT;
                        // P0WRONG
                        p0w_i = pf_imap["R"].P1WRONG + PR0CE0*pf_imap["Q"].P0WRONG +
                            PQ0_Q0W*(PR0CE0W + PR0WCE0 + PR0WCE0W) +
                            PR0CE1*pf_imap["D"].P0WRONG + PD0_D0W*(PR0CE1W + PR0WCE1 + PR0WCE1W);
                        // P1WRONG
                        p1w_i = PR0CE0*pf_imap["Q"].P1WRONG + PQ1_Q1W*(PR0CE0W + PR0WCE0 + PR0WCE0W) +
                            PR0CE1*pf_imap["D"].P1WRONG + PD1_D1W*(PR0CE1W + PR0WCE1 + PR0WCE1W);
                        // #############################################################
                        // # update output nets
                        omap.clear();
                        // CORRELATED VALUE IS ONLY BEING FORWARDED FROM 'D' TO 'O'
                        set_out_edge_error_probability_values (G, elem, p0w_i, p1w_i, p0c_i, p1c_i, "NEXT", omap, correlated_value_map["D"]);
                    }
                } else {
                    if ((G[elem].prim == FDPE))  {
                        if(get_input_fp_map(G, elem, pf_imap, 1, correlated_value_map)) {
                            // print_map(pf_imap);
                            double PPRE1CE0 = pf_imap["PRE"].P0CORRECT*pf_imap["CE"].P0CORRECT;
                            double PPRE0CE0W = pf_imap["PRE"].P0CORRECT*pf_imap["CE"].P0WRONG;
                            double PQ0_Q0W = pf_imap["Q"].P0CORRECT + pf_imap["Q"].P0WRONG;
                            double PQ1_Q1W = pf_imap["Q"].P1CORRECT + pf_imap["Q"].P1WRONG;
                            double PPRE0WCE0 = pf_imap["PRE"].P0WRONG*pf_imap["CE"].P0CORRECT;
                            double PPRE0WCE0W = pf_imap["PRE"].P0WRONG*pf_imap["CE"].P0WRONG;
                            double PPRE0CE1 = pf_imap["PRE"].P0CORRECT*pf_imap["CE"].P1CORRECT;
                            double PPRE0CE1W = pf_imap["PRE"].P0CORRECT*pf_imap["CE"].P1WRONG;
                            double PD0_D0W = pf_imap["D"].P0CORRECT + pf_imap["D"].P0WRONG;
                            double PD1_D1W = pf_imap["D"].P1CORRECT + pf_imap["D"].P1WRONG;
                            double PPRE0WCE1 = pf_imap["PRE"].P0WRONG*pf_imap["CE"].P1CORRECT;
                            double PPRE0WCE1W = pf_imap["PRE"].P0WRONG*pf_imap["CE"].P1WRONG;
                            // P0CORRECT
                            p0c_i = PPRE1CE0*pf_imap["Q"].P0CORRECT +
                                PPRE0CE1*pf_imap["D"].P0CORRECT;
                            // P1CORRECT
                            p1c_i = pf_imap["PRE"].P1CORRECT + PPRE1CE0*pf_imap["Q"].P1CORRECT +
                                PPRE0CE1*pf_imap["D"].P1CORRECT;
                            // P0WRONG
                            p0w_i = PPRE1CE0*pf_imap["Q"].P0WRONG + PQ0_Q0W*(PPRE0CE0W + PPRE0WCE0 + PPRE0WCE0W) +
                                PPRE0CE1*pf_imap["D"].P0WRONG + PD0_D0W*(PPRE0CE1W + PPRE0WCE1 + PPRE0WCE1W);
                            // P1WRONG
                            p1w_i = pf_imap["PRE"].P1WRONG + PPRE1CE0*pf_imap["Q"].P1WRONG +
                                PQ1_Q1W*(PPRE0CE0W + PPRE0WCE0 + PPRE0WCE0W) +
                                PPRE0CE1*pf_imap["D"].P1WRONG + PD1_D1W*(PPRE0CE1W+PPRE0WCE1+PPRE0WCE1W);
                            // #############################################################
                            // # update output nets
                            omap.clear();
                            // CORRELATED VALUE IS ONLY BEING FORWARDED FROM 'D' TO 'O'
                            set_out_edge_error_probability_values (G, elem, p0w_i, p1w_i, p0c_i, p1c_i, "NEXT", omap, correlated_value_map["D"]);
                        }
                    } else {
                        if(G[elem].prim == FDCE){
                            // OBS.: ATTRIBUTES ARE NOT BEING TARGET IN THIS VERSION
                            // Get input PF (Probability of Failure)
                            if(get_input_fp_map(G, elem, pf_imap, 1, correlated_value_map)) {
                                if (dbgflag==1){
                                    cout << "elem->" << id2node[elem] << "   ";
                                    print_map(pf_imap);
                                }
                                double PCLR0CE0 = pf_imap["CLR"].P0CORRECT*pf_imap["CE"].P0CORRECT;
                                double PCLR1CE0 = pf_imap["CLR"].P0CORRECT*pf_imap["CE"].P0CORRECT;
                                double PCLR0CE0W = pf_imap["CLR"].P0CORRECT*pf_imap["CE"].P0WRONG;
                                double PQ0_Q0W = pf_imap["Q"].P0CORRECT + pf_imap["Q"].P0WRONG;
                                double PQ1_Q1W = pf_imap["Q"].P1CORRECT + pf_imap["Q"].P1WRONG;
                                double PCLR0WCE0 = pf_imap["CLR"].P0WRONG*pf_imap["CE"].P0CORRECT;
                                double PCLR0WCE0W = pf_imap["CLR"].P0WRONG*pf_imap["CE"].P0WRONG;
                                double PCLR0CE1 = pf_imap["CLR"].P0CORRECT*pf_imap["CE"].P1CORRECT;
                                double PCLR0CE1W = pf_imap["CLR"].P0CORRECT*pf_imap["CE"].P1WRONG;
                                double PD0_D0W = pf_imap["D"].P0CORRECT + pf_imap["D"].P0WRONG;
                                double PD1_D1W = pf_imap["D"].P1CORRECT + pf_imap["D"].P1WRONG;
                                double PCLR0WCE1 = pf_imap["CLR"].P0WRONG*pf_imap["CE"].P1CORRECT;
                                double PCLR0WCE1W = pf_imap["CLR"].P0WRONG*pf_imap["CE"].P1WRONG;
                                // P0CORRECT
                                p0c_i = pf_imap["CLR"].P1CORRECT + PCLR0CE0*pf_imap["Q"].P0CORRECT + pf_imap["CLR"].P0CORRECT*pf_imap["CE"].P1CORRECT*pf_imap["D"].P0CORRECT;
                                // P1CORRECT
                                p1c_i = PCLR0CE0*pf_imap["Q"].P1CORRECT + pf_imap["CLR"].P0CORRECT*pf_imap["CE"].P1CORRECT*pf_imap["D"].P1CORRECT;
                                // P0WRONG
                                p0w_i = pf_imap["CLR"].P1WRONG + PCLR0CE0*pf_imap["Q"].P0WRONG +
                                    PQ0_Q0W*(PCLR0CE0W + PCLR0WCE0 + PCLR0WCE0W) +
                                    PCLR0CE1*pf_imap["D"].P0WRONG + PD0_D0W*(PCLR0CE1W + PCLR0WCE1 + PCLR0WCE1W);
                                // P1WRONG
                                p1w_i = PCLR0CE0*pf_imap["Q"].P1WRONG + PQ1_Q1W*(PCLR0CE0W + PCLR0WCE0 + PCLR0WCE0W) +
                                    PCLR0CE1*pf_imap["D"].P1WRONG + PD1_D1W*(PCLR0CE1W + PCLR0WCE1 + PCLR0WCE1W);

                                omap.clear();
                                // CORRELATED VALUE IS ONLY BEING FORWARDED FROM 'D' TO 'O'
                                set_out_edge_error_probability_values (G, elem, p0w_i, p1w_i, p0c_i, p1c_i, "NEXT", omap, correlated_value_map["D"]);
                            }
                        } else {
                            if(G[elem].prim == BUFG) {
                                if(get_input_fp_map(G, elem, pf_imap, 0, correlated_value_map)) {
                                    // print_map(pf_imap);
                                    // # calculate the pf
                                    // ###################### PROBABILITY FUNCTION #################
                                    // output = input
                                    // #############################################################
                                    // # update output nets
                                    omap.clear();
                                    // CORRELATED VALUE IS ONLY BEING FORWARDED FROM 'I' TO 'O'
                                    set_out_edge_error_probability_values (G, elem, pf_imap["I"].P0WRONG, pf_imap["I"].P1WRONG, pf_imap["I"].P0CORRECT, pf_imap["I"].P1CORRECT, "NET", omap, correlated_value_map["I"]);
                                }
                            } else {
                                if (G[elem].prim>=LUT1) {
                                    vector<double> merged_correlation(16,0);
                                    if(get_input_fp_map(G, elem, pf_imap, 0, correlated_value_map)) {
                                        if (dbgflag==1){
                                            cout << "   elem->" << id2node[elem] << "   ";
                                            print_map(pf_imap);
                                            cout << G[elem].prob_funcs.P0CORRECT<< endl;
                                            cout << G[elem].prob_funcs.P1CORRECT<< endl;
                                            cout << G[elem].prob_funcs.P0WRONG<< endl;
                                            cout << G[elem].prob_funcs.P1WRONG<< endl;
                                        }

                                        int lutsize = G[elem].prim - LUT1 +1;
                                        // P0CORRECT
                                        // getchar();
                                        p0c_i = eval_function (G, elem, G[elem].prob_funcs.P0CORRECT, lutsize);
                                        // P1CORRECT
                                        p1c_i = eval_function (G, elem, G[elem].prob_funcs.P1CORRECT, lutsize);
                                        // P0WRONG
                                        p0w_i = eval_function (G, elem, G[elem].prob_funcs.P0WRONG, lutsize);
                                        // P0WRONG
                                        p1w_i = eval_function (G, elem, G[elem].prob_funcs.P1WRONG,lutsize);
                                        // #############################################################
                                        // # update output nets
                                        omap.clear();
                                        // if element is a correlated source
                                        if (correlated_sources_map.find(elem)!=correlated_sources_map.end() && cycle_of_correlation_activation[elem] <= current_cycle) { 
                                            errorValuesModel corr_val_i = ERROR_VAL_NOT_DEFINED;
                                            DEBUG_MSG("calculating correlated values for " << id2node[elem]);

                                            for (auto item : G[elem].correlated_value_expression){

                                                string corr_val=item.substr(0,2);
                                                if(corr_val=="C0"){
                                                    corr_val_i = C0;
                                                } else {
                                                    if(corr_val=="C1"){
                                                        corr_val_i = C1;
                                                    } else {
                                                        if(corr_val=="W0"){
                                                            corr_val_i = W0;
                                                        } else {
                                                            if(corr_val=="W1"){
                                                                corr_val_i = W1;
                                                            } else {
                                                                cout << "Unexpected correlated Value "<< corr_val << "!!!"  << endl;
                                                                exit(1);
                                                            }
                                                        }
                                                    }
                                                }
                                                string expr = item.substr(3);
                                                double res = eval_correlated_function(G, elem, expr, lutsize);
                                                if (res>0){
                                                    DEBUG_MSG("corr_val_i=" << corr_val_i <<  "Res=" << res);
                                                    correlated_value.push_back(std::make_pair(res,corr_val_i));

                                                }
                                            }
                                        }

                                        /// if it is a convergent LUT than calculate the correlated output probability
                                        if (convergent_destination.find(elem)!=convergent_destination.end() && cycle_of_correlation_activation[elem] <= current_cycle) {
                                            DEBUG_MSG("Checking merged_correlation for " << id2node[elem]);
                                            string correlated_pin1 = convergent_destination[elem].at(0);
                                            string correlated_pin2 = convergent_destination[elem].at(1);

                                            cout << "correlated_value_map[correlated_pin1]: ";
                                            for( auto item : correlated_value_map[correlated_pin1]){
                                                cout << " "<< item.second << ":" << item.first;
                                            }
                                            cout << endl;
                                            cout << "correlated_value_map[correlated_pin2]: ";
                                            for( auto item : correlated_value_map[correlated_pin2]){
                                                cout << " "<< item.second << ":" << item.first;
                                            }
                                            cout << endl;

                                            merge_correlated_inputs (correlated_value_map[correlated_pin1], correlated_value_map[correlated_pin2], merged_correlation);

                                            double zeros = std::all_of(merged_correlation.begin(), merged_correlation.end(), [](double i) { return i==0.0; });

                                            if(!zeros){
                                                DEBUG_MSG("calculating converged correlated value for " << elem);
                                                calculate_correlated_probability(G, elem, p0w_i, p1w_i, p0c_i, p1c_i, merged_correlation, convergent_destination);
                                                DEBUG_MSG("Correlated: "<< p0c_i << "|"<< p1c_i <<"|"<< p0w_i <<"|"<< p1w_i <<  "|");
                                            }
                                        }

                                        if (elem==error_source){
                                            double newp0c_i, newp1c_i, newp0w_i, newp1w_i;
                                            newp0c_i = p1w_i;
                                            newp1c_i = p0w_i;
                                            newp0w_i = p1c_i;
                                            newp1w_i = p0c_i;
                                            set_out_edge_error_probability_values (G, elem, newp0w_i, newp1w_i, newp0c_i, newp1c_i, "NET", omap, correlated_value);
                                        } else {
                                            set_out_edge_error_probability_values (G, elem, p0w_i, p1w_i, p0c_i, p1c_i, "NET", omap, correlated_value);
                                        }

                                    }
                                } else {
                                    if((G[elem].prim == CARRY8) || (G[elem].prim == CARRY4)) {
                                        errorProbabilityType i0_pair, i1_pair, sel_pair;

                                        if(get_input_fp_map(G, elem, pf_imap, 0, correlated_value_map)){
                                            // # calculate the pf
                                            // ###################### PROBABILITY FUNCTION #################
                                            i0_pair = std::make_pair(std::make_pair(pf_imap["CIN"].P0CORRECT,pf_imap["CIN"].P1CORRECT),std::make_pair(pf_imap["CIN"].P0WRONG,pf_imap["CIN"].P1WRONG));
                                            i1_pair = std::make_pair(std::make_pair(pf_imap["DI[0]"].P0CORRECT,pf_imap["DI[0]"].P1CORRECT),std::make_pair(pf_imap["DI[0]"].P0WRONG,pf_imap["DI[0]"].P1WRONG));
                                            sel_pair = std::make_pair(std::make_pair(pf_imap["S[0]"].P0CORRECT,pf_imap["S[0]"].P1CORRECT),std::make_pair(pf_imap["S[0]"].P0WRONG,pf_imap["S[0]"].P1WRONG));
                                            omap["CO[0]"] = std::make_pair(std::make_pair(0,0),std::make_pair(0,0));
                                            calc_mux_error_probability (i0_pair, i1_pair, sel_pair, omap["CO[0]"]);
                                            omap["O[0]"] = std::make_pair(std::make_pair(0,0),std::make_pair(0,0));
                                            calc_xor_error_probability (i0_pair, sel_pair, omap["O[0]"]);

                                            int loop_limit = 8;
                                            if (G[elem].prim == CARRY4)
                                                loop_limit = 4;

                                            for (size_t i = 1; i < loop_limit; i++) {
                                                string st = "CO["+std::to_string(i-1)+"]";
                                                i0_pair = std::make_pair(std::make_pair(omap[st].P0CORRECT,omap[st].P1CORRECT),std::make_pair(omap[st].P0WRONG,omap[st].P1WRONG));
                                                st = "DI["+std::to_string(i)+"]";
                                                i1_pair = std::make_pair(std::make_pair(pf_imap[st].P0CORRECT,pf_imap[st].P1CORRECT),std::make_pair(pf_imap[st].P0WRONG,pf_imap[st].P1WRONG));
                                                st = "S["+std::to_string(i)+"]";
                                                sel_pair = std::make_pair(std::make_pair(pf_imap[st].P0CORRECT,pf_imap[st].P1CORRECT),std::make_pair(pf_imap[st].P0WRONG,pf_imap[st].P1WRONG));
                                                st = "CO["+std::to_string(i)+"]";
                                                omap[st] = std::make_pair(std::make_pair(0,0),std::make_pair(0,0));
                                                calc_mux_error_probability (i0_pair, i1_pair, sel_pair, omap[st]);
                                                st = "O["+std::to_string(i)+"]";
                                                omap[st] = std::make_pair(std::make_pair(0,0),std::make_pair(0,0));
                                                calc_xor_error_probability (i0_pair, sel_pair, omap[st]);
                                            }
                                            // #############################################################
                                            // # update output nets
                                            cout << "CORRELATED VALUE IS NOT BEING CALCULETED YET" << endl;
                                            set_out_edge_error_probability_values (G, elem, 0, 0, 0, 0, "NET", omap, correlated_value);
                                        }
                                    } else {
                                        if(G[elem].prim == MUXF7) {
                                            errorProbabilityType i0_pair, i1_pair, sel_pair, out;

                                            if(get_input_fp_map(G, elem, pf_imap, 0, correlated_value_map)) {

                                                // # calculate the pf
                                                // ###################### PROBABILITY FUNCTION #################
                                                i0_pair = std::make_pair(std::make_pair(pf_imap["I0"].P0CORRECT,pf_imap["I0"].P1CORRECT),std::make_pair(pf_imap["I0"].P0WRONG,pf_imap["I0"].P1WRONG));
                                                i1_pair = std::make_pair(std::make_pair(pf_imap["I1"].P0CORRECT,pf_imap["I1"].P1CORRECT),std::make_pair(pf_imap["I1"].P0WRONG,pf_imap["I1"].P1WRONG));
                                                sel_pair = std::make_pair(std::make_pair(pf_imap["S[0]"].P0CORRECT,pf_imap["S[0]"].P1CORRECT),std::make_pair(pf_imap["S[0]"].P0WRONG,pf_imap["S[0]"].P1WRONG));
                                                calc_mux_error_probability (i0_pair, i1_pair, sel_pair, out);

                                                // #############################################################
                                                // # update output nets
                                                omap.clear();
                                                cout << "CORRELATED VALUE IS NOT BEING CALCULETED YET" << endl;
                                                set_out_edge_error_probability_values (G, elem, out.P0WRONG, out.P1WRONG, out.P0CORRECT, out.P1CORRECT, "NET", omap, correlated_value);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        dbgflag=0;
    }
}

void run_combinational_propagation(MyGraphType& G, int error_source, circuitInfo cif, bool is_first, vector<int> current_sequential_fp_sources, correlationInfo correlation_Info, int current_cycle) {

    map <string, errorProbabilityType> imap;
    vector<int> out_of_order_combinational_faulty_list;
    map<int,vector<int>> combinational_path_order;
    vector<int> combinational_faulty_list;

    if (is_first == true){
	 out_of_order_combinational_faulty_list = cif.combinational_list;
    } else {
        for(auto const& elem : current_sequential_fp_sources){
            out_of_order_combinational_faulty_list = union_of_vectors(out_of_order_combinational_faulty_list, cif.combinational_cones[elem]);
        }
    }

    remove_if_in_list (out_of_order_combinational_faulty_list, cif.outPorts);
    std::sort(out_of_order_combinational_faulty_list.begin(), out_of_order_combinational_faulty_list.end());
    out_of_order_combinational_faulty_list.erase( unique( out_of_order_combinational_faulty_list.begin(), out_of_order_combinational_faulty_list.end() ), out_of_order_combinational_faulty_list.end() );

    // ## Get the order and add to the right position in list of lists
    for(auto const& elem : out_of_order_combinational_faulty_list) {
        int val = G[elem].combinational_path_position-1;
        combinational_path_order[val].push_back(elem);
    }

    for(auto const& elem : combinational_path_order) {
        for(auto const& el : elem.second) {
            combinational_faulty_list.push_back(el);
        }
    }

    calculate_output_probabilities(G, error_source, combinational_faulty_list, correlation_Info, current_cycle);
}

vector<int> run_sequential_propagation(MyGraphType& G, int error_source, vector<int> sequential_list, correlationInfo correlation_Info){
    iEdge_it in_i, in_end;
    oEdge_it out_i, out_end;
    vector<int> current_sequential_fp_sources;

    for (auto elem : sequential_list) {
        for (tie(in_i, in_end) = boost::in_edges(elem,G); in_i != in_end; ++in_i){

            if (G[*in_i].NETP0CORRECT!= -1){
 
                if ((G[*in_i].NETP0CORRECT!= G[*in_i].previous_net_failure_probability.P0CORRECT) ||
                    (G[*in_i].NETP1CORRECT!= G[*in_i].previous_net_failure_probability.P1CORRECT) ||
                    (G[*in_i].NETP0WRONG!= G[*in_i].previous_net_failure_probability.P0WRONG) ||
                    (G[*in_i].NETP1WRONG!= G[*in_i].previous_net_failure_probability.P1WRONG)) {

                        current_sequential_fp_sources.push_back(elem);
                        break;
                }
            }
        }
    }

    for (auto elem : sequential_list){
        for (tie(in_i, in_end) = boost::in_edges(elem,G); in_i != in_end; ++in_i) {
            G[*in_i].previous_net_failure_probability.P0CORRECT = G[*in_i].NETP0CORRECT;
            G[*in_i].previous_net_failure_probability.P1CORRECT = G[*in_i].NETP1CORRECT;
            G[*in_i].previous_net_failure_probability.P0WRONG   = G[*in_i].NETP0WRONG;
            G[*in_i].previous_net_failure_probability.P1WRONG   = G[*in_i].NETP1WRONG;
        }
    }

    calculate_output_probabilities(G, error_source, current_sequential_fp_sources, correlation_Info, 0);

    for (auto elem : sequential_list){
        for (tie(out_i, out_end) = boost::out_edges(elem, G); out_i != out_end; ++out_i){
            if ((G[*out_i].next_net_failure_probability.P0CORRECT!=-1) && (G[*out_i].next_net_failure_probability.P1CORRECT!=-1) && (G[*out_i].next_net_failure_probability.P0WRONG!=-1) && (G[*out_i].next_net_failure_probability.P1WRONG!=-1)) {
                G[*out_i].NETP0CORRECT = G[*out_i].next_net_failure_probability.P0CORRECT;// updating current npf with next
                G[*out_i].NETP1CORRECT = G[*out_i].next_net_failure_probability.P1CORRECT;// updating current npf with next
                G[*out_i].NETP0WRONG = G[*out_i].next_net_failure_probability.P0WRONG;// updating current npf with next
                G[*out_i].NETP1WRONG = G[*out_i].next_net_failure_probability.P1WRONG;// updating current npf with next
            }
        }
    }

    return current_sequential_fp_sources;
}

void run_error_propagation(MyGraphType& G, int error_source, circuitInfo cif, map<int, vector<errorProbabilityType>>& report, 
    vector<int> watch_list, programOptions ins, correlationInfo correlation_Info){
    bool is_first = true, finished=false;
    vector<int> combinational_1st_faulty_list;
    vector<int> current_sequential_fp_sources;
    vector<bool> thereIsPath;
    int cycles_cnt = 0;

    error_prob_init(G, error_source);

    constant_probability_init(G, cif.inPorts, cif.sequential_list);

    for (unsigned int i=0; i<cif.outPorts.size(); i++)
        thereIsPath.push_back(false);

    for (unsigned int i=0; i<cif.outPorts.size(); i++)
        thereIsPath.at(i) = there_is_path(G, error_source, cif.outPorts.at(i));

    bool at_least_one_path = false;
    for (auto elem : thereIsPath)
        if (elem == true)
            at_least_one_path = true;

    if (id2node[error_source]!="NO_ERROR_DUMMY_COMPONENT"){
        if (!at_least_one_path){
            cout << " Error, lut " << id2node[error_source] << " drives no output!!!!" << endl;
            return;
        }
    }


    run_combinational_propagation(G, error_source, cif, is_first, current_sequential_fp_sources, correlation_Info, 0);
    if (ins.print== 1){         run_print(G, cif, 0, watch_list); }

    int cycle = 0;

    while(cycle!=(ins.cycles_to_run)){

        current_sequential_fp_sources = run_sequential_propagation(G, error_source, cif.sequential_list, correlation_Info);
        cycle++;

        if (ins.print==1)  run_print(G, cif, cycle, watch_list);

        run_combinational_propagation(G, error_source, cif, false, current_sequential_fp_sources, correlation_Info, cycle);

        if (ins.print==1)  run_print(G, cif, cycle, watch_list);

        update_report (G, cif.outPorts, error_source, cycle, report);
    }
}

void constant_probability_init(MyGraphType& G, vector<int> inPorts, vector<int> sequential_list){
    oEdge_it out_i, out_end;
    iEdge_it in_i, in_end;

    for (auto elem : inPorts){
        for (tie(out_i, out_end) = boost::out_edges(elem, G); out_i != out_end; ++out_i) {
            if ((id2node[elem]=="reset") ||(id2node[elem]=="rst") || (id2node[elem]=="RESET") ||  (id2node[elem]=="RST")) {
                G[*out_i].NETP0CORRECT=1;
                G[*out_i].NETP1CORRECT=0;
                G[*out_i].NETP0WRONG=0;
                G[*out_i].NETP1WRONG=0;
            } else {
                G[*out_i].NETP0CORRECT=0.5;
                G[*out_i].NETP1CORRECT=0.5;
                G[*out_i].NETP0WRONG=0;
                G[*out_i].NETP1WRONG=0;
            }
        }
    }

    auto es = boost::edges(G);
    for (auto eit = es.first; eit != es.second; ++eit) {
        if(G[*eit].NODEOUTEDGE.find("VCC")!=-1 && G[*eit].NODEOUTEDGE.size()<=5){
            G[*eit].NETP0CORRECT=0;
            G[*eit].NETP1CORRECT=1;
            G[*eit].NETP0WRONG=0;
            G[*eit].NETP1WRONG=0;
        } else {
            if(G[*eit].NODEOUTEDGE.find("GND")!=-1 && G[*eit].NODEOUTEDGE.size()<=5){
                G[*eit].NETP0CORRECT=1;
                G[*eit].NETP1CORRECT=0;
                G[*eit].NETP0WRONG=0;
                G[*eit].NETP1WRONG=0;
            }
        }
    }

    // initializing all ff elements with P0C=1.0 considering that the circuit starts reseted
    for (auto elem : sequential_list){
        for (tie(out_i, out_end) = boost::out_edges(elem, G); out_i != out_end; ++out_i) {
            G[*out_i].NETP0CORRECT=1;
            G[*out_i].NETP1CORRECT=0;
            G[*out_i].NETP0WRONG=0;
            G[*out_i].NETP1WRONG=0;
            G[*out_i].previous_net_failure_probability.P0CORRECT=1;
            G[*out_i].previous_net_failure_probability.P1CORRECT=0;
            G[*out_i].previous_net_failure_probability.P0WRONG=0;
            G[*out_i].previous_net_failure_probability.P1WRONG=0;
        }
    }

}

void* run_error_propagation_multithread(void* arg) {

    bool is_first = true, finished=false;
    vector<int> combinational_1st_faulty_list;
    vector<int> current_sequential_fp_sources;
    vector<bool> thereIsPath;
    struct thread_args *ptargs=(struct thread_args*) arg;
    int first = ptargs->error_source;
    MyGraphType G;

    for (size_t i = 0; i < luts_in_thread_group; i++) {
        boost::copy_graph(*ptargs->G, G);

        constant_probability_init(G, ptargs->cif.inPorts, ptargs->cif.sequential_list);
	
        error_prob_init(G, ptargs->error_source);

        for (unsigned int i=0; i<(ptargs->cif.outPorts).size(); i++)
            thereIsPath.push_back(false);

        for (unsigned int i=0; i<(ptargs->cif.outPorts).size(); i++)
            thereIsPath.at(i) = there_is_path(G, ptargs->error_source, (ptargs->cif.outPorts).at(i));

        bool at_least_one_path = false;
        for (auto elem : thereIsPath) {
            if (elem == true)
                at_least_one_path = true;
        }

        if (!at_least_one_path){
            cout << " Error, lut " << id2node[ptargs->error_source] << " drives no output!!!!" << endl;
            pthread_exit(0);
        }

        run_combinational_propagation(G, ptargs->error_source, ptargs->cif, is_first, current_sequential_fp_sources, ptargs->correlation_Info, 0);

	int cycle=0;
     while(cycle!=(ptargs->cycles_to_run)){
 
         current_sequential_fp_sources = run_sequential_propagation(G, ptargs->error_source, ptargs->cif.sequential_list, ptargs->correlation_Info);
 
         cycle++;
 
         run_combinational_propagation(G, ptargs->error_source, ptargs->cif, false, current_sequential_fp_sources, ptargs->correlation_Info, cycle);
 
         update_report (G, ptargs->cif.outPorts, ptargs->error_source, cycle, ptargs->report);
     }

	if (ptargs->error_source==last_lut)
            break;
        else
            ptargs++;

        G.clear();
    }
    
    pthread_exit(0);
}

void run_multiproc (MyGraphType G, nodeMapType nodeMap, programOptions ins, circuitInfo cif) {
    pthread_t v_tids[ARGS_NUM_MAX];
    struct thread_args v_targs[ARGS_NUM_MAX];
    last_lut = cif.lutList.back();
    int lutListSize = cif.lutList.size();
    luts_in_thread_group = ins.multiproc.at(1);
    int num_of_groups = ARGS_NUM_MAX/luts_in_thread_group; // q = (x + y - 1)/y    for ceiling division
    int loops = (lutListSize + ARGS_NUM_MAX -1)/ARGS_NUM_MAX; // q = (x + y - 1)/y    for ceiling division
    
    if (num_of_groups*luts_in_thread_group > ARGS_NUM_MAX) {
        cout << "Error! num_of_groups cannot be greater than ARGS_NUM_MAX!\n Sugestion: increase the second parameter to at least:" << lutListSize/ARGS_NUM_MAX + 1 << ",or increase the ARGS_NUM_MAX according to your available memory." << endl;
        exit(1);
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    enum {FINISHED, LAST_THREAD, NOT_FINISHED} done = NOT_FINISHED;
    cout << "loops="<<loops << " num_of_groups=" << num_of_groups << " luts_in_thread_group=" << luts_in_thread_group << " lutListSize="<<lutListSize<< endl;
    for (size_t i = 0; i < loops; i++) {
        int thread_created =0;
        for (size_t thread_count = 0; thread_count < num_of_groups; thread_count++) {
            for (size_t j = 0; j < luts_in_thread_group; j++) {
                int x = thread_count*luts_in_thread_group+j;
                if((((i*num_of_groups+thread_count)*luts_in_thread_group)+j)<lutListSize){

                    v_targs[x].error_source = cif.lutList.at(((i*num_of_groups+thread_count)*luts_in_thread_group)+j);

                    v_targs[x].G = &G;
                    v_targs[x].nodeMap = &nodeMap;
                    v_targs[x].cif.global_combinational_size = cif.global_combinational_size;
                    v_targs[x].cif.outPorts = cif.outPorts;
                    v_targs[x].cif.inPorts = cif.inPorts;
                    v_targs[x].cif.combinational_list = cif.combinational_list;
                    v_targs[x].cif.sequential_list = cif.sequential_list;
                    v_targs[x].cif.combinational_cones = cif.combinational_cones;
                    v_targs[x].report = {};
                    v_targs[x].cycles_to_run = ins.cycles_to_run;
                } else {
                    if (done == NOT_FINISHED && (lutListSize%luts_in_thread_group)!=0) {
                        done = LAST_THREAD;
                        break;
                    }else {
                        done = FINISHED;
                        break;
                    }
                }
            }

            if(done != FINISHED) {
                pthread_create(&v_tids[thread_count], &attr, &run_error_propagation_multithread,(void*) &v_targs[thread_count*luts_in_thread_group]);
                thread_created++;
                cout << (((i*num_of_groups+thread_count)*luts_in_thread_group)*100) /lutListSize << "\%  loop =" << i << " thread_count =" << thread_count << " "<<((i*num_of_groups+thread_count)*luts_in_thread_group)<< endl;
            } else {
                break;
            }
        }
        for (size_t thread_count = 0; thread_count < thread_created; thread_count++) {
            pthread_join(v_tids[thread_count], NULL);
        }

        for (size_t thread_count = 0; thread_count < num_of_groups; thread_count++) {
            for (size_t j = 0; j < luts_in_thread_group; j++) {
                if((((i*num_of_groups+thread_count)*luts_in_thread_group)+j)<lutListSize){
                    generate_report(id2node[v_targs[thread_count*luts_in_thread_group+j].error_source], ins.output, v_targs[thread_count*luts_in_thread_group+j].report);
                } else if (done == FINISHED) {
                    i = loops;
                    break;
                }
            }
        }
    }
    pthread_attr_destroy(&attr);
}

int main(int argc, char** argv) {

    string file_path, prj_name;
    int  error_source;
    nodeMapType nodeMap;
    edgeMapType edgeMap;
    MyGraphType G;
    vector<int>  watchlist;
    map< int, vector<errorProbabilityType> > report;
    programOptions program_Options;
    circuitInfo circuit_Info;
    correlationInfo correlation_Info;


    auto start = std::chrono::steady_clock::now();

    bool result = process_command_line(argc, argv, program_Options);

    if (!result)
        return 1;

    G = file_to_graph(program_Options.input, nodeMap, edgeMap, id2node, circuit_Info.outPorts, circuit_Info.inPorts);

    run_preprocessing(G, program_Options, nodeMap, id2node, circuit_Info, correlation_Info);

    if (program_Options.error_flag==0){
        error_source = nodeMap["NO_ERROR_DUMMY_COMPONENT"].id;
        run_error_propagation(G, error_source, circuit_Info, report, watchlist, program_Options, correlation_Info);
        generate_report(id2node[error_source], program_Options.output, report);
    } else {
        if (program_Options.multiproc.at(0)>1) {
           run_multiproc(G, nodeMap, program_Options, circuit_Info);
        } else {
            if (program_Options.watchlist_flag==1) {
                watchlist = generate_watchlist(G, nodeMap[program_Options.lut].id, nodeMap[program_Options.output_watch].id , nodeMap);

                exit(0);
            } else
                watchlist = {};

            if (program_Options.lut.size()>0){
                DEBUG_MSG("********************** SOURCE = " << program_Options.lut);
                report.clear();
                run_error_propagation(G, nodeMap[program_Options.lut].id, circuit_Info, report, watchlist, program_Options, correlation_Info);
                generate_report(id2node[error_source], program_Options.output, report);
            } else {
                for (auto lut : circuit_Info.lutList) {
                    error_source = lut;
                    DEBUG_MSG("********************** SOURCE = " << error_source);
                    report.clear();

                    run_error_propagation(G, error_source, circuit_Info, report, watchlist, program_Options, correlation_Info);

                    generate_report(id2node[error_source], program_Options.output, report);
                }
            }
        }
    }
    cout << "100!!! " << endl;
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    cout << std::chrono::duration <double, std::milli> (diff).count() << " ms" << endl;

    return 0;
}
