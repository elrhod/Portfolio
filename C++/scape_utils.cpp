 /**
  * @file  	scape_utils.cpp
  * @date 	Aug 14, 2019
  * @author Eduardo Luis Rhod 
 */

#include "types.hpp"
#include "scape_utils.hpp"
#include "exprtk.hpp"

#define DEBUG ON

#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

double eval_function (MyGraphType& G, int elem, string func, int lutsize) {
    map <string, errorProbabilityType > pf_imap;
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double>     expression_t;
    typedef exprtk::parser<double>             parser_t;
    double I5P0CORRECT, I5P0WRONG,I5P1CORRECT,I5P1WRONG,I4P0CORRECT,I4P0WRONG,I4P1CORRECT,I4P1WRONG,I3P0CORRECT,I3P0WRONG,I3P1CORRECT,I3P1WRONG;
    double I2P0CORRECT,I2P0WRONG,I2P1CORRECT,I2P1WRONG,I1P0CORRECT,I1P0WRONG,I1P1CORRECT,I1P1WRONG,I0P0CORRECT,I0P0WRONG,I0P1CORRECT,I0P1WRONG;
    const string expression_string = func;
    map<string,vector<pair<double,errorValuesModel>>> correlated_value_map;

    get_input_fp_map(G, elem, pf_imap, 0, correlated_value_map);

    symbol_table_t symbol_table;

    if (lutsize>=6){
        I5P0CORRECT = pf_imap["I5"].P0CORRECT;
        I5P0WRONG = pf_imap["I5"].P0WRONG;
        I5P1CORRECT = pf_imap["I5"].P1CORRECT;
        I5P1WRONG = pf_imap["I5"].P1WRONG;
        symbol_table.add_variable("I5P0CORRECT",I5P0CORRECT);
        symbol_table.add_variable("I5P0WRONG",I5P0WRONG);
        symbol_table.add_variable("I5P1CORRECT",I5P1CORRECT);
        symbol_table.add_variable("I5P1WRONG",I5P1WRONG);
    }
    if (lutsize>=5){
        I4P0CORRECT = pf_imap["I4"].P0CORRECT;
        I4P0WRONG = pf_imap["I4"].P0WRONG;
        I4P1CORRECT = pf_imap["I4"].P1CORRECT;
        I4P1WRONG = pf_imap["I4"].P1WRONG;
        symbol_table.add_variable("I4P0CORRECT",I4P0CORRECT);
        symbol_table.add_variable("I4P0WRONG",I4P0WRONG);
        symbol_table.add_variable("I4P1CORRECT",I4P1CORRECT);
        symbol_table.add_variable("I4P1WRONG",I4P1WRONG);
    }
    if (lutsize>=4){
        I3P0CORRECT = pf_imap["I3"].P0CORRECT;
        I3P0WRONG = pf_imap["I3"].P0WRONG;
        I3P1CORRECT = pf_imap["I3"].P1CORRECT;
        I3P1WRONG = pf_imap["I3"].P1WRONG;
        symbol_table.add_variable("I3P0CORRECT",I3P0CORRECT);
        symbol_table.add_variable("I3P0WRONG",I3P0WRONG);
        symbol_table.add_variable("I3P1CORRECT",I3P1CORRECT);
        symbol_table.add_variable("I3P1WRONG",I3P1WRONG);
    }
    if (lutsize>=3){
        I2P0CORRECT = pf_imap["I2"].P0CORRECT;
        I2P0WRONG = pf_imap["I2"].P0WRONG;
        I2P1CORRECT = pf_imap["I2"].P1CORRECT;
        I2P1WRONG = pf_imap["I2"].P1WRONG;
        symbol_table.add_variable("I2P0CORRECT",I2P0CORRECT);
        symbol_table.add_variable("I2P0WRONG",I2P0WRONG);
        symbol_table.add_variable("I2P1CORRECT",I2P1CORRECT);
        symbol_table.add_variable("I2P1WRONG",I2P1WRONG);
    }
    if (lutsize>=2){
        I1P0CORRECT = pf_imap["I1"].P0CORRECT;
        I1P0WRONG = pf_imap["I1"].P0WRONG;
        I1P1CORRECT = pf_imap["I1"].P1CORRECT;
        I1P1WRONG = pf_imap["I1"].P1WRONG;
        symbol_table.add_variable("I1P0CORRECT",I1P0CORRECT);
        symbol_table.add_variable("I1P0WRONG",I1P0WRONG);
        symbol_table.add_variable("I1P1CORRECT",I1P1CORRECT);
        symbol_table.add_variable("I1P1WRONG",I1P1WRONG);
    }
    if (lutsize>=1){
        I0P0CORRECT = pf_imap["I0"].P0CORRECT;
        I0P0WRONG = pf_imap["I0"].P0WRONG;
        I0P1CORRECT = pf_imap["I0"].P1CORRECT;
        I0P1WRONG = pf_imap["I0"].P1WRONG;
        symbol_table.add_variable("I0P0CORRECT",I0P0CORRECT);
        symbol_table.add_variable("I0P0WRONG",I0P0WRONG);
        symbol_table.add_variable("I0P1CORRECT",I0P1CORRECT);
        symbol_table.add_variable("I0P1WRONG",I0P1WRONG);
    }
    symbol_table.add_constants();

    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;
    parser.compile(expression_string,expression);

    const double res = expression.value();
    // cout << "res="<< res << endl;
    // getchar();
    return res;
}

double eval_correlated_function (MyGraphType& G, int elem, string func, int lutsize) {
    map <string, errorProbabilityType> pf_imap;
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double>     expression_t;
    typedef exprtk::parser<double>             parser_t;
    double I5P0C,I5P0W,I5P1C,I5P1W,I4P0C,I4P0W,I4P1C,I4P1W,I3P0C,I3P0W,I3P1C,I3P1W;
    double I2P0C,I2P0W,I2P1C,I2P1W,I1P0C,I1P0W,I1P1C,I1P1W,I0P0C,I0P0W,I0P1C,I0P1W;
    const string expression_string = func;
    map<string,vector<pair<double,errorValuesModel>>> correlated_value_map;


    get_input_fp_map(G, elem, pf_imap, 0, correlated_value_map);

    symbol_table_t symbol_table;

    if (lutsize>=6){
        I5P0C = pf_imap["I5"].P0CORRECT;
        I5P0W = pf_imap["I5"].P0WRONG;
        I5P1C = pf_imap["I5"].P1CORRECT;
        I5P1W = pf_imap["I5"].P1WRONG;
        symbol_table.add_variable("I5P0C",I5P0C);
        symbol_table.add_variable("I5P0W",I5P0W);
        symbol_table.add_variable("I5P1C",I5P1C);
        symbol_table.add_variable("I5P1W",I5P1W);
    }
    if (lutsize>=5){
        I4P0C = pf_imap["I4"].P0CORRECT;
        I4P0W = pf_imap["I4"].P0WRONG;
        I4P1C = pf_imap["I4"].P1CORRECT;
        I4P1W = pf_imap["I4"].P1WRONG;
        symbol_table.add_variable("I4P0C",I4P0C);
        symbol_table.add_variable("I4P0W",I4P0W);
        symbol_table.add_variable("I4P1C",I4P1C);
        symbol_table.add_variable("I4P1W",I4P1W);
    }
    if (lutsize>=4){
        I3P0C = pf_imap["I3"].P0CORRECT;
        I3P0W = pf_imap["I3"].P0WRONG;
        I3P1C = pf_imap["I3"].P1CORRECT;
        I3P1W = pf_imap["I3"].P1WRONG;
        symbol_table.add_variable("I3P0C",I3P0C);
        symbol_table.add_variable("I3P0W",I3P0W);
        symbol_table.add_variable("I3P1C",I3P1C);
        symbol_table.add_variable("I3P1W",I3P1W);
    }
    if (lutsize>=3){
        I2P0C = pf_imap["I2"].P0CORRECT;
        I2P0W = pf_imap["I2"].P0WRONG;
        I2P1C = pf_imap["I2"].P1CORRECT;
        I2P1W = pf_imap["I2"].P1WRONG;
        symbol_table.add_variable("I2P0C",I2P0C);
        symbol_table.add_variable("I2P0W",I2P0W);
        symbol_table.add_variable("I2P1C",I2P1C);
        symbol_table.add_variable("I2P1W",I2P1W);
    }
    if (lutsize>=2){
        I1P0C = pf_imap["I1"].P0CORRECT;
        I1P0W = pf_imap["I1"].P0WRONG;
        I1P1C = pf_imap["I1"].P1CORRECT;
        I1P1W = pf_imap["I1"].P1WRONG;
        symbol_table.add_variable("I1P0C",I1P0C);
        symbol_table.add_variable("I1P0W",I1P0W);
        symbol_table.add_variable("I1P1C",I1P1C);
        symbol_table.add_variable("I1P1W",I1P1W);
    }
    if (lutsize>=1){
        I0P0C = pf_imap["I0"].P0CORRECT;
        I0P0W = pf_imap["I0"].P0WRONG;
        I0P1C = pf_imap["I0"].P1CORRECT;
        I0P1W = pf_imap["I0"].P1WRONG;
        symbol_table.add_variable("I0P0C",I0P0C);
        symbol_table.add_variable("I0P0W",I0P0W);
        symbol_table.add_variable("I0P1C",I0P1C);
        symbol_table.add_variable("I0P1W",I0P1W);
    }
    symbol_table.add_constants();

    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;
    parser.compile(expression_string,expression);

    const double res = expression.value();
    // cout << "res="<< res << endl;
    //

    return res;
}

vector<pair<int, int>> find_same_input_luts(MyGraphType& G, map<int,vector<int>> lutsize_map){
    iEdge_it in_i, in_end;
    vector<pair<int, int>> correlated_luts_v;

    for (auto elem : lutsize_map){
        for (size_t first_lut = 0; first_lut < elem.second.size(); first_lut++) {
            for (size_t second_lut = first_lut+1; second_lut < elem.second.size(); second_lut++) {
                vector<int> first_lut_i, second_lut_i;
                for (tie(in_i, in_end) = boost::in_edges(elem.second.at(first_lut),G); in_i != in_end; ++in_i) {
                    first_lut_i.push_back(boost::source(*in_i,G));
                }
                std::sort(first_lut_i.begin(), first_lut_i.end());

                for (tie(in_i, in_end) = boost::in_edges(elem.second.at(second_lut),G); in_i != in_end; ++in_i) {
                    second_lut_i.push_back(boost::source(*in_i,G));
                }
                std::sort(second_lut_i.begin(), second_lut_i.end());

                if(first_lut_i==second_lut_i && second_lut_i.size()>0){
                    correlated_luts_v.push_back(std::make_pair(elem.second.at(first_lut),elem.second.at(second_lut)));
                }
            }
        }
    }



    return correlated_luts_v;
}

bool get_input_fp_map (MyGraphType& G, int vertex, map <string, errorProbabilityType>& imap, int out_flag, map <string,vector<pair<double,errorValuesModel>>>& correlated_value) {
    iEdge_it in_i, in_end;
    oEdge_it out_i, out_end;
    imap.clear();

    if (out_flag==1) { //memory element
        /// gets all input values
        for (tie(in_i, in_end) = boost::in_edges(vertex,G); in_i != in_end; ++in_i) {
            imap[G[*in_i].PININEDGE] = std::make_pair(
                std::make_pair(G[*in_i].previous_net_failure_probability.P0CORRECT,G[*in_i].previous_net_failure_probability.P1CORRECT) ,
                std::make_pair(G[*in_i].previous_net_failure_probability.P0WRONG,G[*in_i].previous_net_failure_probability.P1WRONG));
        /// get correlated values
            correlated_value[G[*in_i].PININEDGE] = G[*in_i].correlated_value;
        }
        /// gets all output values
        for (tie(out_i, out_end) = boost::out_edges(vertex, G); out_i != out_end; ++out_i) {
            imap[G[*out_i].PINOUTEDGE]= std::make_pair( std::make_pair(G[*out_i].NETP0CORRECT,G[*out_i].NETP1CORRECT) , std::make_pair(G[*out_i].NETP0WRONG,G[*out_i].NETP1WRONG));
            break;
        }
    } else {
        /// gets all input values
        for (tie(in_i, in_end) = boost::in_edges(vertex,G); in_i != in_end; ++in_i) {
            imap[G[*in_i].PININEDGE] = std::make_pair( std::make_pair(G[*in_i].NETP0CORRECT,G[*in_i].NETP1CORRECT) , std::make_pair(G[*in_i].NETP0WRONG,G[*in_i].NETP1WRONG));
        /// get correlated values
            correlated_value[G[*in_i].PININEDGE] = G[*in_i].correlated_value;
        }
    }
    bool ret = true;
    for(auto item : imap) {
        if(item.second.P0CORRECT==-1)
            ret=false;
    }

    return ret; // returns true if all probabilities have a valid value and false if not.
}

enum PrimEnum convert_to_enum (string st) {
    if (st.find("LUT1")!=-1){ return LUT1; } else {
    if (st.find("LUT2")!=-1){ return LUT2; } else {
    if (st.find("LUT3")!=-1){ return LUT3; } else {
    if (st.find("LUT4")!=-1){ return LUT4; } else {
    if (st.find("LUT5")!=-1){ return LUT5; } else {
    if (st.find("LUT6")!=-1){ return LUT6; } else {
    if (st.find("CARRY8")!=-1){ return CARRY8; } else {
    if (st.find("CARRY4")!=-1){ return CARRY4; } else {
    if (st.find("OUTPORT")!=-1){ return OUTPORT; } else {
    if (st.find("INPORT")!=-1){ return INPORT; } else {
    if (st.find("IBUF")!=-1){ return IBUF; } else {
    if (st.find("OBUF")!=-1){ return OBUF; } else {
    if (st.find("BUFGCE")!=-1){ return BUFGCE; } else {
    if (st.find("FDRE")!=-1){ return FDRE; } else {
    if (st.find("FDPE")!=-1){ return FDPE; } else {
    if (st.find("FDCE")!=-1){ return FDCE; } else {
    if (st.find("BUFG")!=-1){ return BUFG; } else {
    if (st.find("MUXF7")!=-1){ return MUXF7; } else {
    if (st.find("VCC")!=-1){ return VCC; } else {
    if (st.find("GND")!=-1){ return GND; } else {
        cout << "Error in 'convert_to_enum' while converting: " << st << endl;
        exit(1);
    }}}}}}}}}}}}}}}}}}}}
}

void calc_mux_error_probability (errorProbabilityType i0, errorProbabilityType i1, errorProbabilityType sel, errorProbabilityType& out ) {
    out.P0CORRECT = sel.P0CORRECT*i0.P0CORRECT + sel.P1CORRECT*i1.P0CORRECT + i0.P0CORRECT*i1.P0CORRECT*(sel.P0WRONG+sel.P1WRONG);
    out.P1CORRECT = sel.P0CORRECT*i0.P1CORRECT + sel.P1CORRECT*i1.P1CORRECT + i0.P1CORRECT*i1.P1CORRECT*(sel.P0WRONG+sel.P1WRONG);
    out.P0WRONG = sel.P0CORRECT*i0.P0WRONG + sel.P1CORRECT*i1.P0WRONG + sel.P0WRONG*(i1.P0CORRECT*i0.P0WRONG + i1.P1CORRECT*i0.P0CORRECT + i1.P1CORRECT*i0.P0WRONG + i1.P0WRONG*i0.P0CORRECT +
      i1.P0WRONG*i0.P0WRONG + i1.P1WRONG*i0.P0CORRECT + i1.P1WRONG*i0.P0WRONG) + sel.P1WRONG*i1.P0CORRECT*(i0.P1CORRECT + i0.P0WRONG + i0.P1WRONG) + sel.P1WRONG*i1.P0WRONG;
    out.P1WRONG = sel.P0CORRECT*i0.P1WRONG + sel.P1CORRECT*i1.P1WRONG + sel.P0WRONG*i1.P0CORRECT*(i0.P1CORRECT + i0.P1WRONG) + sel.P0WRONG*i1.P1CORRECT*i0.P1WRONG + sel.P0WRONG*i1.P0WRONG*(i0.P1CORRECT + i0.P1WRONG)
       + sel.P0WRONG*i1.P1WRONG*(i0.P1CORRECT + i0.P1WRONG) + sel.P1WRONG*i1.P1CORRECT*(i0.P0CORRECT + i0.P0WRONG + i0.P1WRONG) + sel.P1WRONG*i1.P1WRONG;
}

void calc_xor_error_probability (errorProbabilityType i0, errorProbabilityType i1, errorProbabilityType& out ) {
    out.P0CORRECT = i0.P0CORRECT*i1.P0CORRECT + i0.P1CORRECT*i1.P1CORRECT + i0.P0WRONG*i1.P0WRONG + i0.P1WRONG*i1.P1WRONG;
    out.P1CORRECT = i0.P0CORRECT*i1.P1CORRECT + i0.P1CORRECT*i1.P0CORRECT + i0.P1WRONG*i1.P0WRONG + i0.P0WRONG*i1.P1WRONG;
    out.P0WRONG = i0.P0WRONG*i1.P0CORRECT + i0.P1WRONG*i1.P1CORRECT + i0.P0CORRECT*i1.P0WRONG + i0.P1CORRECT*i1.P1WRONG;
    out.P1WRONG = i0.P1WRONG*i1.P0CORRECT + i0.P1WRONG*i1.P1CORRECT + i0.P1CORRECT*i1.P0WRONG + i0.P0CORRECT*i1.P1WRONG;
}

bool there_is_path(MyGraphType& G, int error_source, int target) {
    oEdge_it out_i, out_end;
    vector<int> todo_list;
    map<int,int> nodes_dic;
    int node;
    // int print =0;
    // if (error_source==8203){
    //     print =1;
    // }
    // /*1*/ cout << "Target: "<< id2node[target] << " Source: "<< id2node[error_source] << endl;

    for (tie(out_i, out_end) = boost::out_edges(error_source, G); out_i != out_end; ++out_i){
        // node = print_list_port.push_back(make_pair(elem,G[*in_i].net_failure_probability));
        node = boost::target(*out_i,G);
        // /*1*/ cout << "pushing: "<< id2node[node] << endl;
        todo_list.push_back(node);
        nodes_dic[node] = node;
    }
    // /*1*/ getchar();
    while (!todo_list.empty()) {
        int next_node = todo_list[todo_list.size()-1]; todo_list.pop_back();
        // /*1*/ cout << "poping: "<< id2node[next_node] << endl;
        nodes_dic[next_node] = next_node;
        // get next_node predecessors and add to list and dictionary
        if (next_node == target){
            return true;
        }
        for (tie(out_i, out_end) = boost::out_edges(next_node, G); out_i != out_end; ++out_i){
            node = boost::target(*out_i,G);
            // check if node is not in dictionary
            if (nodes_dic.find(node) == nodes_dic.end()){ //not in the dictionary...
                //  not processed... then then put node in the list0
                todo_list.push_back(node);
                // /*1*/ cout << "pushing: "<< id2node[node] << endl;
            }
        }
        // if (print) /*1*/ getchar();
    }
    // /*1*/ cout << "no path between " << error_source << " : " << id2node[error_source] << " and " << target << " : " << id2node[target]<< endl;
    // /*1*/ getchar();
    return false;
}

void merge_correlated_inputs(vector<pair<double,errorValuesModel>> correlated_value1, vector<pair<double,errorValuesModel>> correlated_value2, vector<double>& merged){
    cout << "M1" << endl;

    if (correlated_value1.size() ==correlated_value2.size()) {
        cout << "M2 " <<correlated_value1.size()<< endl;
        for (size_t i = 0; i < correlated_value1.size(); i++) {
            cout << "M3"<<i << endl;

            if(correlated_value1.at(i).first != correlated_value2.at(i).first){
                cout << "Error, value of correlated inputs at same vector position ("<< correlated_value1.at(i).first<<","<<correlated_value2.at(i).first <<") should be equal!" << endl;
                exit(1);
            } else {
                cout << "MM3"<<i << endl;
                merged.at((correlated_value2.at(i).second-1)*4+(correlated_value1.at(i).second-1))+=correlated_value1.at(i).first;
                cout << "MMM3"<<i << endl;
            }
        }
    } else {
        cout << "Error, size of correlated vectors ("<< correlated_value1.size()<<","<<correlated_value2.size() <<") should match!" << endl;
        exit(1);
    }

    for (size_t i = 0; i < merged.size(); i++) {
        DEBUG_MSG("Merged["<<i<<"]="<< merged.at(i));
    }
}

void calculate_correlated_probability(MyGraphType& G, int elem, double& p0w, double& p1w, double& p0c, double& p1c, vector<double> merged_correlation,
    map<int,vector<string>> convergent_destination)  {
    map<string,errorProbabilityType> pf_imap;
    map<int,int> conversion_table;
    int lutsize = G[elem].prim-LUT1+1;
    int index1=-1, index2=-1;
    vector <enum errorValuesModel> val {ERROR_VAL_NOT_DEFINED, C0 , C1 , W0 , W1 };
    enum errorValuesModel current_inputs[lutsize];
    map <string, vector<pair<double,errorValuesModel>>> correlated_value_map;
    // cout << "X1" << endl;
    if(get_input_fp_map(G, elem, pf_imap, 0, correlated_value_map)) {
        if (convergent_destination[elem].at(0)=="I0"){
            index1=0;
        } else {
            if (convergent_destination[elem].at(0)=="I1"){
                index1=1;
            } else {
                if (convergent_destination[elem].at(0)=="I2"){
                    index1=2;
                } else {
                    if (convergent_destination[elem].at(0)=="I3"){
                        index1=3;
                    } else {
                        if (convergent_destination[elem].at(0)=="I4"){
                            index1=4;
                        } else {
                            if (convergent_destination[elem].at(0)=="I5"){
                                index1=5;
                            } else {
                                cout << "error in geting index from " << convergent_destination[elem].at(0) << endl;
                                exit(1);
                            }
                        }
                    }
                }
            }
        }
        cout << "index1 "<< index1 << endl;
        if (convergent_destination[elem].at(1)=="I0"){
            index2=0;
        } else {
            if (convergent_destination[elem].at(1)=="I1"){
                index2=1;
            } else {
                if (convergent_destination[elem].at(1)=="I2"){
                    index2=2;
                } else {
                    if (convergent_destination[elem].at(1)=="I3"){
                        index2=3;
                    } else {
                        if (convergent_destination[elem].at(1)=="I4"){
                            index2=4;
                        } else {
                            if (convergent_destination[elem].at(1)=="I5"){
                                index2=5;
                            } else {
                                cout << "error in geting index from " << convergent_destination[elem].at(1) << endl;
                                exit(1);
                            }
                        }
                    }
                }
            }
        }
        cout << "index2 "<< index2 << endl;

        for (size_t i = 0; i < pow(4,lutsize); i++) {
            vector<int> decomposition;
            // cout << "Xi "<<i << endl;
            for (size_t j = 0; j < lutsize; j++) {
                // cout << "Xj "<<j << endl;
                if (j==0){
                    decomposition.push_back(i%4);
                } else {
                    decomposition.push_back((int(i/(pow(4,j))))%4);
                }
            }
            // for (auto item : decomposition){
            //     cout << item << " ";
            // }
            // cout << endl;
            conversion_table[i]=decomposition.at(index2)*4+decomposition.at(index1);
            cout << conversion_table[i] << " conversion_table[i]" << endl;
            //

        }
        cout << "conversion_table " << endl;
        for (auto elem : conversion_table){
            cout << elem.first << ":" << elem.second << endl;
        }

        p0c=0;
        p1c=0;
        p0w=0;
        p1w=0;

        for (size_t i = 0; i < pow(4,lutsize); i++) {
            DEBUG_MSG(" i\t"<<i);
            double p = 1;
            if (merged_correlation.at(conversion_table[i])>0){
                for (size_t j = 0; j < lutsize; j++) {
                    DEBUG_MSG(" j\t"<<j);
                    if(j!=index1 && j!=index2) {
                        cout << "X5 " << endl;
                        string pin_str = "I"+std::to_string(j);
                        errorValuesModel this_input;
                        if (j==0){
                            this_input = val.at((i%4)+1); // +1 to skip "ERROR_VAL_NOT_DEFINED"
                        } else {
                            this_input = val.at(((int(i/(pow(4,j))))%4)+1);  // +1 to skip "ERROR_VAL_NOT_DEFINED"
                        }
                        cout << " this_input " << this_input << " pin_str "<< pin_str;

                        switch(this_input) {
                            case C0:
                                p*=pf_imap[pin_str].P0CORRECT;
                                cout << " P0CORRECT\t" << pf_imap[pin_str].P0CORRECT;
                            break;
                            case C1:
                                p*=pf_imap[pin_str].P1CORRECT;
                                cout << " P1CORRECT\t" << pf_imap[pin_str].P1CORRECT;
                            break;
                            case W0:
                                p*=pf_imap[pin_str].P0WRONG;
                                cout << " P0WRONG\t" << pf_imap[pin_str].P0WRONG;
                            break;
                            case W1:
                                p*=pf_imap[pin_str].P1WRONG;
                                cout << " P1WRONG\t" << pf_imap[pin_str].P1WRONG;
                            break;
                        }
                        cout << "X6 " << endl;
                        cout << merged_correlation.size() << endl;
                        cout << "X7 " << endl;
                    }
                    cout << " p " << p << endl;
                }
            }
            cout << "HERE, conversion_table["<<i<<"]=" << conversion_table[i] << endl;

            if (p>0 && merged_correlation.at(conversion_table[i])>0){
                cout << "HERE2, conversion_table["<<i<<"]=" << conversion_table[i] << endl;
                cout << "merged_correlation.size() " <<merged_correlation.size() << endl;
                p=p*merged_correlation.at(conversion_table[i]);
                cout << "merged_correlation.at(conversion_table["<<i<<"]) " << merged_correlation.at(conversion_table[i]) << endl;
                cout << "HERE3, G[elem].correlated_value_matrix.at("<<i<<")=" << G[elem].correlated_value_matrix.at(i) << endl;
                switch (G[elem].correlated_value_matrix.at(i)) {
                    case C0:
                        p0c+=p;
                        // cout << " pC0 " << p0c << endl;
                    break;
                    case C1:
                        p1c+=p;
                        // cout << " pC1 " << p1c << endl;
                    break;
                    case W0:
                        p0w+=p;
                        // cout << " pW0 " << p0w << endl;
                    break;
                    case W1:
                        p1w+=p;
                        // cout << " pW1 " << p1w << endl;
                    break;
                }
            } else {
                cout << endl;
            }
        }
    }
    cout << "p0c " << p0c << endl;
    cout << "p1c " << p1c << endl;
    cout << "p0w " << p0w << endl;
    cout << "p1w " << p1w << endl;
    //
}

vector<string> gen_correlated_lut_prob_func_not_minimized (long long int func, int lutsize) {
    vector <enum errorValuesModel> val {C0 , C1 , W0 , W1};
    enum errorValuesModel current_inputs[lutsize];
    vector<string> correlated_value_expression;
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

    for (long int i = 0; i < pow(2,(lutsize*2)); i++) {
        for (size_t j = 0; j < lutsize; j++) {
            // cout << i << "|" << j << " ";
            if (j==0){
                current_inputs[0] = val.at(i%4);
                // cout << current_inputs[j] << " ";
            } else {
                current_inputs[j] = val.at((int(i/(pow(4,j))))%4);
                // cout << current_inputs[j] << " ";
            }
        }

        right_bit=0;
        wrong_bit=0;
        string expr = "";
        for (int j = lutsize-1; j >=0 ; j--) {
            // cout << "j:"<<j << endl;
            //
            switch (current_inputs[j]) {
                case W1:
                    if (j!=0) {
                        wrong_bit+=pow(2,j);
                    } else {
                        wrong_bit+=1;
                    }
                    // cout << "W1 ";
                    expr+="I"+std::to_string(j)+"P1W*";
                    break;
                case C1:
                    if (j!=0) {
                        right_bit+=pow(2,j);
                        wrong_bit+=pow(2,j);
                    } else {
                        right_bit+=1;
                        wrong_bit+=1;
                    }
                    // cout << "C1 ";
                    expr+="I"+std::to_string(j)+"P1C*";
                    break;
                case W0:
                    if (j!=0) {
                        right_bit+=pow(2,j);
                    } else {
                        right_bit+=1;
                    }
                    // cout << "W0 ";
                    expr+="I"+std::to_string(j)+"P0W*";
                    break;
                case C0:
                    // cout << "C0 ";
                    expr+="I"+std::to_string(j)+"P0C*";
                    break;
            }
        }
        expr+="1";
        // cout << expr << endl;
        //
        // cout << "\n right_bit=" << right_bit << "\n wrong_bit=" << wrong_bit << endl;

        switch (lutsize) {
            case 1:
                if(l1func[right_bit]==l1func[wrong_bit]) {
                    if (l1func[right_bit]==1) {
                        correlated_value_expression.push_back("C1="+expr);
                    } else {
                        correlated_value_expression.push_back("C0="+expr);
                    }
                } else {
                    if (l1func[wrong_bit]==1){
                        correlated_value_expression.push_back("W1="+expr);
                    } else {
                        correlated_value_expression.push_back("W0="+expr);
                    }
                }
            break;
            case 2:
                if(l2func[right_bit]==l2func[wrong_bit]){
                    if (l2func[right_bit]==1){
                        correlated_value_expression.push_back("C1="+expr);
                    } else {
                        correlated_value_expression.push_back("C0="+expr);
                    }
                } else {
                    if (l2func[wrong_bit]==1){
                        correlated_value_expression.push_back("W1="+expr);
                    } else {
                        correlated_value_expression.push_back("W0="+expr);
                    }
                }
                break;
            case 3:
            if(l3func[right_bit]==l3func[wrong_bit]){
                if (l3func[right_bit]==1){
                    correlated_value_expression.push_back("C1="+expr);
                } else {
                    correlated_value_expression.push_back("C0="+expr);
                }
            } else {
                if (l3func[wrong_bit]==1){
                    correlated_value_expression.push_back("W1="+expr);
                } else {
                    correlated_value_expression.push_back("W0="+expr);
                }
            }
            break;
            case 4:
            if(l4func[right_bit]==l4func[wrong_bit]){
                if (l4func[right_bit]==1){
                    correlated_value_expression.push_back("C1="+expr);
                } else {
                    correlated_value_expression.push_back("C0="+expr);
                }
            } else {
                if (l4func[wrong_bit]==1){
                    correlated_value_expression.push_back("W1="+expr);
                } else {
                    correlated_value_expression.push_back("W0="+expr);
                }
            }
            break;
            case 5:
            if(l5func[right_bit]==l5func[wrong_bit]){
                if (l5func[right_bit]==1){
                    correlated_value_expression.push_back("C1="+expr);
                } else {
                    correlated_value_expression.push_back("C0="+expr);
                }
            } else {
                if (l5func[wrong_bit]==1){
                    correlated_value_expression.push_back("W1="+expr);
                } else {
                    correlated_value_expression.push_back("W0="+expr);
                }
            }
            break;
            case 6:
            if(l6func[right_bit]==l6func[wrong_bit]){
                if (l6func[right_bit]==1){
                    correlated_value_expression.push_back("C1="+expr);
                } else {
                    correlated_value_expression.push_back("C0="+expr);
                }
            } else {
                if (l6func[wrong_bit]==1){
                    correlated_value_expression.push_back("W1="+expr);
                } else {
                    correlated_value_expression.push_back("W0="+expr);
                }
            }
            break;
        }
    }
    return correlated_value_expression;
}

vertexDescType get_first_lut_in_vector(MyGraphType G, vector<vertexDescType> path) {
    for (int i = 0; i < path.size(); ++i){
        if(G[path.at(i)].prim>=LUT1) {
            return path.at(i);
        }
    }

    DEBUG_MSG("ERROR, no LUT found in path:");
    for(auto item : path){
        DEBUG_MSG(id2node[item] << " ");
    }     
    exit(1);
}

void set_cycle_of_correlation_activation(MyGraphType G, vector<vertexDescType> path, map<vertexDescType,int>& cycle_of_correlation_activation, vertexDescType dest) {
    int cycle_counter =0; 

    // Identify the cycle for the first lut in the path
    for (auto vertex : path) {
        if(G[vertex].component_type==Sequential)
            cycle_counter++;
        else
            if (get_first_lut_in_vector(G, path)==vertex)
                cycle_of_correlation_activation[vertex]=cycle_counter;            

    }

    // Identify the cycle for the destination lut 
    cycle_counter =0; 
    for (auto vertex : path)
        if(G[vertex].component_type==Sequential)
            cycle_counter++;

    cycle_of_correlation_activation[dest]=cycle_counter;
}           

