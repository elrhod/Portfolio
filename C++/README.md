# SCAPE Tool (Space Cycle-Accurate Probability of Error Tool)

The SCAPE tool is a cycle-accurate probability of error calculator that maps a digital circuit to a Graph data structure and performs the evaluation of the probability of failure components and how the fault will impact the circuit performance.
The tool was developed in C++ using the Boost Graph Library (GPL) and can be divided into three stages:
- Pre-processing: in the pre-processing stage data is extracted from the digital circuit using a TCL script and pre-processed;
- Cycle-by-cycle error probability calculation: in this stage, the error probability is calculated cycle-by-cycle;
- Post-processing: The error results are packed and stored in files;

## Pre-Processing
Files [pre_processing.cpp](pre_processing.cpp) and [pre_processing.hpp](pre_processing.hpp) have the functions that do the circuit extraction and mapping to a graph structure. The circuit information is extracted and pre-processed to gather important circuit characteristics that will accelerate the circuit cycle-by-cycle simulation in the next steps like, for instance, get the combinational cones of the circuit.

