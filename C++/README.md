# SCAPE Tool (Space Cycle-Accurate Probability of Error Tool)

The SCAPE tool is a cycle-accurate probability of error calculator that maps a digital circuit to a Graph data structure. It then evaluates the probability of failure considering different failing components and how the fault will impact the circuit performance.
The tool was developed in C++ using the Boost Graph Library (GPL) and Program Options and can be divided into three stages:
- Pre-processing: in the pre-processing stage data is extracted from the digital circuit using a TCL script and pre-processed;
- Cycle-by-cycle error probability calculation: in this stage, the error probability is calculated cycle-by-cycle;
- Post-processing: The error results are packed and stored in files;
The user can also choose to run the tool using multiple processing cores. This is implemented using the C++ class std::threads.


## Pre-Processing
Files [pre_processing.cpp](pre_processing.cpp) and [pre_processing.hpp](pre_processing.hpp) have the functions that do the circuit extraction and mapping to a graph structure. The circuit information is extracted and pre-processed to gather important circuit characteristics that will accelerate the circuit cycle-by-cycle simulation in the next steps like, for instance, getting the combinational cones of the circuit.

## Cycle-by-Cycle Error Probability Calculation
The cycle-by-cycle calculation is done at the [scape.cpp](scape.cpp) and header [scape.hpp](scape.hpp) files. They perform the probability of error calculation and propagation by separating the sequential and combinational parts of the circuit and evaluating each part according to the behavior of each component. Auxiliary utility functions are called from [scape_utils.cpp](scape_utils.cpp) and header [scape_utils.hpp](scape_utils.hpp) like specific evaluation or special components like multiplexers or exclusive or gates. In addition, some tooling functions like counting the frequency of a string or a string splitting operation can be found at the [tools.cpp](tools.cpp) and the header [tools.hpp](tools.hpp).

## Post-Processing
The post-processing steps are executed by functions that are in the [pos_processing.cpp](pos_processing.cpp) and header [pos_processing.hpp](pos_processing.hpp). These files hold the functions that perform report data update and generation.

## Debugging and Types
The [debug.cpp](debug.cpp) and [debug.hpp](debug.hpp) files have functions that allow the observability of the error propagation cycle-by-cycle by printing the signals and its error probability in an ordered and visual way. Finally, the [types.hpp](types.hpp) have all the types and data structure definitions that are used throughout the code.


