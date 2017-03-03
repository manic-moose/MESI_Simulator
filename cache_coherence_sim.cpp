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
    
    // Get the file prefix. Should be the file name without the ".1", ".2", etc.
    do {
      opt = getopt( argc, argv, "hf:m:p:l:b:" );

      switch( opt ) {
        case 'h':
            cout << "Options:" << endl;
            cout << "  -h  Displays help and exits." << endl;
            cout << "  -m <value>  Set the max number of simulation cycles" << endl;
            cout << "  -l <value>  Set the memory read latency" << endl;
            cout << "  -b <value>  Set the burst length (number of cycles for bus data transfer)" << endl;
            cout << "  -f <prefix> Gives file prefix to use" << endl;
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

    if( file_flag == false ) {
      printf( "File prefix not supplied. Please see help with \"-h\" for options.\n" );
      return -1;
    }

    // Open the files for reading
    for( int i = 0; i < num_procs; i++ ) {
      sprintf( filesuffix, "%d", (i+1) );
      strcpy( filename[i], strcat( fileprefix, filesuffix ) );
      myfiles[i] = fopen( filename[i], "r" );
      if( myfiles[i] == NULL ) {
          printf( "ERROR: File %s failed to open. Exiting...\n", filename[i] );
        return -1;
      }
      fileprefix[strlen(fileprefix)-1] = '\0'; // Delete end character
    }
    
    // Instantiate the system with system parameters parameters
    System* s = new System(num_procs, MESI_CONTROLLER_TYPE, memory_latency, burst_length);
    unsigned long long int tickCount = 0;
    while(tickCount++ <= max_ticks) {
        // Make sure we have not reached EOF on any of our files
        for( int i = 0; i < num_procs; i++ ) {
            if( feof( myfiles[i] ) ) {
              break; // Exit out of while loop if end-of-file reached on any file
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
                 fscanf( myfiles[pNum], "%llx %c\n", &nextaddress, &nextop );
                assert( nextop == 'r' || nextop == 'w' );
          
                Instruction* inst = new Instruction;
                inst->OPCODE  = (nextop == 'r') ? 0 : 1;
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
