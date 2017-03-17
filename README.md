# MESI_Simulator
Simulator to evaluate performance of MI, MSI, and MESI Cache Coherence Protocols

This simulator accepts traces of input files and simulates performance of MESI,MSI, and MI cache coherence protocols
for N processors.

====================================
COMPILING THE CODE
To compile the code, the provided makefile can be used. Simply run "make" in this directory
and the compiler will generate "cache_coherence_sim" as the executable.

The cache models built into the simulator are parameterizable, but only through the use of the file:
CacheParams.h

If you wish to modify the cache parameters, edit this file and recompile the executable.
====================================
RUNNING THE SIMULATOR

There are two methods that can be used to run the simulator. You can simply
run the compiled executable with arguments or use the provided perl script, run_benchmarks.pl.

Running simulator manually:
To get a list of arguments to the simulator, execute the following command:
./cache_coherence_sim -h

All arguments are optional with default values EXCEPT -f, which tells the simulator where to load
the trace files from. The -f input must be a prefix of all the trace files. There must be one
trace file per simulated processor. The tracefile names must follow the following format:
<prefix>.#  Where <prefix> is the value passed to -f and "#" is the thread number that the trace file
belongs to. For example, if the user runs:
./cache_coherence_sim -p 2 -f memtrace
The simulator expects to simulate 2 threads, and requires trace files to exist with these names:
memtrace.0
memtrace.1

Running simulator using run_benchmarks.pl:
run_benchmarks.pl is designed to run large numbers of simulations and collect all the data
at completion into a csv for easy parsing. Use "./run_benchmarks.pl -h" for input argument
and usage requirements. An example set of small trace files has been provided in the tracefiles/dummy directory.
For the context of "run_benchmarks.pl", dummy is the name of the benchmark.

To run the dummy trace files can be used with the simulator by running the following command:
./run_benchmarks.pl -numthreads 4,8,16 -benchmarks dummy
This will load the 4,8, and 16 input trace files and run simulations for MI,MSI, and MESI.

It will output the log files for each trace into the logs/ directory. It will also generate
SUMMARY.csv that combines all data into a single spreadsheet.

Use "./run_benchmark.pl -h" for a list of additional options that can be used to customize the simulation's execution.

====================================
GENERATING TRACE FILES

The included pin-3.2-81205-gcc-linux directory is third party software designed to allow profiling
of software.

For specific instructions how how to generate trace files for the simulator, please see:
pin-3.2-81205-gcc-linux/GROUP-README

For more information about about PinTool, visit:
https://software.intel.com/en-us/articles/pintool/
====================================