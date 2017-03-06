#include "System.h"
#include "Instruction.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <unistd.h>

using namespace std;

int main( int argc, char **argv ) {

    // Get the input for the file name
    if( argc == 1 ) {
      cout << "ERROR: No Arguments supplied. Please use -h to see options." << endl;
      return -1;
    }

    int opt;
    extern char *optarg;
    char fileprefix[256];
    char filesuffix[256];
    bool file_flag = false;
    unsigned long long int nextaddress;
    char nextop;
    
    // Default Parameters
    unsigned int max_ticks      = 100000;  // Maximum number of simulation cycle
    unsigned int num_procs      = 4;       // Number of processors to use
    unsigned int memory_latency = 50;      // Memory read latency
    unsigned int burst_length   = 8;       // Number of cycles a memory bus transfer takes
    unsigned int controllerType = MESI_CONTROLLER_TYPE;
    
    // Get the file prefix. Should be the file name without the ".1", ".2", etc.
    do {
      opt = getopt( argc, argv, "hf:m:p:l:b:c:" );

      switch( opt ) {
        case 'h':
            cout << "Options:" << endl;
            cout << "  -h  Displays help and exits." << endl;
            cout << "  -m <value>  Set the max number of simulation cycles" << endl;
            cout << "  -l <value>  Set the memory read latency" << endl;
            cout << "  -b <value>  Set the burst length (number of cycles for bus data transfer)" << endl;
            cout << "  -p <value>  Set the number of processors to simulate. Must have equal number of trace files as inputs." << endl;
            cout << "  -f <prefix> Gives file prefix to use" << endl;
            cout << "  -c <type>   Define controller type. Default is MESI. Use 0 for MESI, 1 for MSI, or 2 for MI" << endl;
            return -1;
            break;    
        case 'f':
            strcpy( fileprefix, optarg );
            strcat( fileprefix, "." );
            file_flag = true;
            break;  
        case 'm':
            max_ticks = atoi(optarg);
            if (max_ticks < 1) {
                cout << "Maximum simulation cycles must be at least 1." << endl;
                return -1;
            }
            break;
        case 'p':
            num_procs = atoi(optarg);
            if (num_procs < 1) {
                cout << "Number of processors must be at least 1." << endl;
                return -1;
            }
            break;
        case 'l':
            memory_latency = atoi(optarg);
            if (memory_latency < 1) {
                cout << "Memory latency must be at least 1." << endl;
                return -1;
            }
            break;
        case 'b':
            burst_length = atoi(optarg);
            if (burst_length < 1) {
                cout << "Burst Length must be at least 1." << endl;
                return -1;
            }
            break;
       case 'c':
            if (atoi(optarg) == 0) {
                controllerType = MESI_CONTROLLER_TYPE;
            } else if (atoi(optarg) == 1) {
                controllerType = MSI_CONTROLLER_TYPE;
            } else if (atoi(optarg) == 2) {
                controllerType = MI_CONTROLLER_TYPE;
            } else {
                cout << "Illegal controller type. Use 0 for MESI (default), 1 for MSI, or 2 for MI." << endl;
                return -1;
            }
            break;
        default:
        break;
      }
    } while( opt != -1 );
    
    // Dynamically allocate file arrays
    char** filename = new char*[num_procs];
    for (int i = 0; i < 256; i++) {
        filename[i] = new char[256];   
    }
    FILE** myfiles = new FILE*[num_procs];

    bool* eofChecker = new bool[num_procs];
    for (int i = 0; i < num_procs; i++) {
        eofChecker[i] = false;   
    }
    
    if( file_flag == false ) {
      printf( "File prefix not supplied. Please see help with \"-h\" for options.\n" );
      return -1;
    }

    // Open the files for reading
    for( int i = 0; i < num_procs; i++ ) {
      sprintf( filesuffix, "%d", i );
      strcpy( filename[i], strcat( fileprefix, filesuffix ) );
      myfiles[i] = fopen( filename[i], "r" );
      if( myfiles[i] == NULL ) {
          printf( "ERROR: File %s failed to open. Exiting...\n", filename[i] );
        return -1;
      }
      fileprefix[strlen(fileprefix)-1] = '\0'; // Delete end character
    }
    
    cout << "Starting simulation with the following system parameters:" << endl;
    cout << "  PROCESSOR COUNT : " << num_procs << endl;
    if (controllerType == MESI_CONTROLLER_TYPE) {
        cout << "  CONTROLLER TYPE : MESI" << endl;
    } else if (controllerType == MSI_CONTROLLER_TYPE) {
        cout << "  CONTROLLER TYPE : MSI" << endl;
    } else if (controllerType == MI_CONTROLLER_TYPE) {
        cout << "  CONTROLLER TYPE : MI" << endl;
    }
    cout << "  MEMORY LATENCY  : " << memory_latency << endl;
    cout << "  BURST LENGTH    : " << burst_length << endl;
    cout << "  MAX CYCLES      : " << max_ticks << endl;
    cout << "#######################################################################################################" << endl << endl;
    
    // Instantiate the system with system parameters parameters
    System* s = new System(num_procs, controllerType, memory_latency, burst_length);
    unsigned long long int tickCount = 0;
    while(tickCount++ <= max_ticks) {
        // Make sure we have not reached EOF on any of our files
        for( int i = 0; i < num_procs; i++ ) {
            if( feof( myfiles[i] ) ) {
              //break; // Exit out of while loop if end-of-file reached on any file
                eofChecker[i] = true;
            }
        }
        // Loop over each processor and give it an instruction.
        for (int pNum = num_procs - 1; pNum >= 0; pNum--) {
            // This "hasPendingInstructions" check isn't strictly necessary,
            // but can be used as flow control. Basically this is just checking if the processor
            // is handling an instruction currently, and if it is, there is not need to give it a new one.
            // However, the processor is capable of receieving new instructions even if there are still
            // instructions pending, and they will just get queued up and execute once the previous
            // instruction finishes.
            if (!(s->hasPendingInstructions(pNum))) {
                // Read in new instruction from file
                if (eofChecker[pNum]) {
                    nextop = 'n';
                    nextaddress = 0;
                } else {
                    fscanf( myfiles[pNum], "%llx %c\n", &nextaddress, &nextop );
                }
                Instruction* inst = new Instruction;
                if (nextop == 'r' || nextop == 'w' ) {
                    inst->OPCODE  = (nextop == 'r') ? LOAD_CMD : STORE_CMD;
                } else {
                    inst->OPCODE = NOP_CMD;
                }
                inst->ADDRESS = nextaddress;
                cout << "Instruction Insert - " << "P" << pNum << " OPCODE: " << inst->OPCODE << "  Address: " << inst->ADDRESS << endl;
                // Queues up the instruction for the processor
                s->insertInstruction(inst,pNum);
            }
        }
        // System simulation tick. This will be distributed to each component
        // to advance the simulation one cycle
        s->Tick();
    }
    
    cout << "#######################################################################################################" << endl << endl;
    cout << "Simulation completed with " << tickCount << " simulation cycles." << endl;
    
    // Prints out statistics for the system.
    s->reportMemoryOpStatistics();
    
    
    return 0;   
}
