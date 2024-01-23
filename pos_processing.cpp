 /**
  * @file  	pos_processing.cpp
  * @date 	Aug 29, 2019
  * @author Eduardo Luis Rhod 
 */

#include <fstream>
#include "types.hpp"
#include "pos_processing.hpp"

/* External strutures */
extern map <int, string> id2node;

void update_report (MyGraphType G, vector<int> outPorts, int error_source, int cycle, map<int,vector<errorProbabilityType>>& report){
    errorProbabilityType nfp; // net failure prob. read
    vector <errorProbabilityType> nfp_v; // net failure probability vector. Holds the error probability for each output at each cycle.
    iEdge_it in_i, in_end;

    for (auto target : outPorts) {

        for (tie(in_i, in_end) = boost::in_edges(target,G); in_i != in_end; ++in_i) {
            nfp = G[*in_i].net_failure_probability;
            nfp_v.clear();
            if (nfp.P0WRONG !=-1) {
                if (report.find(target) == report.end()) { //if not in the dictionary...add it.
                    nfp_v.push_back(nfp);
                    report[target] = nfp_v;
                } else { // if there are values from previous cycles, read the existent values and add the new one
                    nfp_v = report[target];
                    nfp_v.push_back(nfp);
                    report[target] = nfp_v;
                }

            }
        }
    }

}

void generate_report (string error_source, string output, map< int, vector<errorProbabilityType> > report){
    char path[256];
    static bool first_time = true;
    static bool first_time2 = true;
    string file_path;
    std::ofstream outfile;
    int loop_limit, cycle;
    // cout << "r1" << endl;

    if (getcwd(path, sizeof(path)) == NULL) // gets current directory
        perror("getcwd() error");
    else {
        file_path += path;
        file_path += "/";
        file_path += output;
    }

    if (first_time){
        outfile.open(file_path);
        first_time = false;
    }else
        outfile.open(file_path, std::ios_base::app);

        for (auto elem : report) {
            bool is_first = true;
            loop_limit = elem.second.size();

            for (size_t i = 0; i < loop_limit; i++) {
                cycle = i+1;
                if (is_first){
                    if (error_source!="NO_ERROR_DUMMY_COMPONENT") {
                        outfile << error_source << "," << id2node[elem.first] << "," << cycle << ",(" << elem.second.at(i).P0WRONG << ";" << elem.second.at(i).P1WRONG << ")";
                    } else {
                        outfile << error_source << "," << id2node[elem.first] << "," << cycle << ",(" << elem.second.at(i).P0CORRECT << ";" << elem.second.at(i).P1CORRECT << ")";
                    }
                    is_first = false;
                } else
                    if (error_source!="NO_ERROR_DUMMY_COMPONENT") {
                        outfile << ",(" << elem.second.at(i).P0WRONG << ";" << elem.second.at(i).P1WRONG << ")";
                    } else {
                        outfile << ",(" << elem.second.at(i).P0CORRECT << ";" << elem.second.at(i).P1CORRECT << ")";
                    }

                if (i == loop_limit-1)
                    outfile  << endl;
            }
        }

    outfile.close();
    file_path="";
    if (getcwd(path, sizeof(path)) == NULL) // gets current directory
        perror("getcwd() error");
    else {
        file_path += path;
        file_path += "/";
        file_path += output;
        file_path.replace(file_path.find("/outputs/"),9,"/outputs/acc_",13);
    }

    if (first_time2){
        outfile.open(file_path);
        first_time2 = false;
    }else
        outfile.open(file_path, std::ios_base::app);

    double accumulated_probability=1, this_cycle_probability;
    for (auto elem : report) {
        accumulated_probability=1;
        loop_limit = elem.second.size();
        for (size_t i = 0; i < loop_limit; i++) {
            cycle = i+1;
            this_cycle_probability = elem.second.at(i).P0WRONG + elem.second.at(i).P1WRONG;
            accumulated_probability *= (1-this_cycle_probability);
        }
        accumulated_probability = 1-accumulated_probability;
        outfile << error_source << "," << id2node[elem.first] << "," << accumulated_probability << endl;
    }

    outfile.close();
}
