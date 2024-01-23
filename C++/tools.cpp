/**
  * @file  	tools.cpp
  * @date 	Aug 15, 2019
  * @author Eduardo Luis Rhod 
 */

#include "types.hpp"
#include "tools.hpp"

int countFreq (string pattern, string str) {
    int cnt = 0;
    int pos;
    do{
        pos = str.find(pattern);
        if (pos != -1) {
            cnt++;
            str = str.substr(pos + pattern.length());
        }
    }while(pos != -1);

    return cnt;
}

vector<int> decToBinary (unsigned long long int n, int bits){
    // array to store binary number
    vector<int> inverted(bits),binaryNum(bits);

    // counter for binary array
    int i = 0;
    for (int z=0; z<bits; z++) {
        inverted[i] = n % 2;
        n = n / 2;
        i++;
    }

    // printing binary array in reverse order
    for (int z=0; z<bits; z++) {
        binaryNum[bits-1-z]=inverted[z];
    }
    return binaryNum;
}

vector<string> split_string (string line) {
    vector<string> result;
    std::istringstream iss(line);
    for(string s; iss >> s; )
        result.push_back(s);

    return result;
}

void call_delay(){
	long long s=0;
        for (long int i=0; i<10000; i++){
        for (long int j=0; j<100000; j++){
        s++;
        }}
}

void remove_if_in_list (vector<int>& list, vector<int> remove_if_in_here) {
    size_t mid, left = 0;
    sort(list.begin(),list.end());
    for(auto elem : remove_if_in_here) {
        size_t right = list.size(); // one position passed the right end
        while (left < right) {
            mid = left + (right - left)/2;
            if (elem > list[mid]){
                left = mid+1;
            }
            else if (elem < list[mid]){
                right = mid;
            }
            else {
                list.erase(std::remove(list.begin(), list.end(), elem), list.end());
            }
        }
    }
}

vector<int> union_of_vectors (vector<int> A, vector<int> B) {
    map<int,int> m1;
    vector<int> C;

    for (auto elem : A)
        m1[elem]=elem;
    for (auto elem : B)
        m1[elem]=elem;

    for (auto elem : m1)
        C.push_back(elem.first);

    return C;
}

void remove_file (string input) {
    string node, prj_name, file_path;
    char path[256] = "";
    prj_name = input.substr(input.find("/"), input.find(".txt")-6);
    
    if (getcwd(path, sizeof(path)) == NULL) // gets current directory
        perror("getcwd() error");
    else {
        file_path += path;
        file_path += "/preprocessing";
        file_path += prj_name;
        file_path += "_comb_cones_from_sequential_elements_num.txt";
    }

    std::remove(file_path.c_str());
}
