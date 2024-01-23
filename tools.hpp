/**
  * @file  	tools.hpp
  * @date 	Aug 15, 2019
  * @author Eduardo Luis Rhod 
 */

#ifndef TOOLSHPP
    #define TOOLSHPP

	#include <string>
	#include <vector>

/**
  * @brief          Counts the frequency of a given pattern.
  * @param  str		String to be search.
  * @param  pattern	String pattern to be search.
  * 
  * @return 		Total number of times the pattern was found in str.
  *
  */
	int countFreq (string pattern, string str);

/**
  * @brief          Converts a decimal number to a binary number
  * @param  n		Decimal to be converted.
  * @param  bits	Number of bits to use in the convertion.
  * 
  * @return 		Resulting converted bits.
  *
  */	
	vector<int> decToBinary (unsigned long long int n, int bits);

/**
  * @brief          Splits a line into a vector of strings
  * @param  line	Line to be converted.
  * 
  * @return 		Resulting vector of strings.
  *
  */	
	vector<string> split_string (string line); 

/**
  * @brief          A fixed time delay implementation
  *
  */
	void call_delay();

/**
  * @brief          			Remove elements from a list present at another list.
  * @param  list				Vector list of elements to check.
  * @param  remove_if_in_here	Remove if elements are present here.
  *
  */	
	void remove_if_in_list (vector<int>& list, vector<int> remove_if_in_here);

/**
  * @brief      Makes a union of two vectors, A and B.
  * @param  A	Vector A.
  * @param  B	Vector B.
  * 
  * @result		Resulting union.
  * 
  */
	vector<int> union_of_vectors (vector<int> A, vector<int> B); 

/**
  * @brief      	Remove (delete) file.
  * @param  input	input file name to be deleted.
  * 
  */
	void remove_file (string input);

#endif
