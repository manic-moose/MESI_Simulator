#include "System.h"
#include "Instruction.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <unistd.h>

#define NUM_PROCESSORS 8
#define MEMORY_LATENCY 100

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
    char filename[NUM_PROCESSORS][256];
    bool file_flag = false;
    FILE *myfiles[NUM_PROCESSORS];
    unsigned long long int nextaddress;
    char nextop;

    // Get the file prefix. Should be the file name without the ".1", ".2", etc.
    do {
      opt = getopt( argc, argv, "hf:" );

      switch( opt ) {
	
      case 'h':
	printf( "Options:\n\t-h: Displays help and exits.\n\t-f: Gives file prefix to use.\n" );
	return -1;
	break;
	
      case 'f':
	strcpy( fileprefix, optarg );
	strcat( fileprefix, "." );
	file_flag = true;
	break;
	
      default:
	break;
	
      }

    } while( opt != -1 );

    if( file_flag == false ) {
      printf( "File prefix not supplied. Please see help with \"-h\" for options.\n" );
      return -1;
    }

    // Open the files for reading
    for( int i = 0; i < NUM_PROCESSORS; i++ ) {

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
    System* s = new System(NUM_PROCESSORS, MESI_CONTROLLER_TYPE, MEMORY_LATENCY);
    
    while(1) {

        // Make sure we have not reached EOF on any of our files
        for( int i = 0; i < NUM_PROCESSORS; i++ ) {
	    if( feof( myfiles[i] ) ) {
  	        break; // Exit out of while loop if end-of-file reached on any file
	    }
	}
    
        // Loop over each processor and give it an instruction.
        for (int pNum = NUM_PROCESSORS - 1; pNum >= 0; pNum--) {
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
    
    // Just some extra cycles at the end to flush anything else out. 
    for (int i = 0; i < 5; i++) {
        s->Tick();   
    }
    
    // Prints out statistics for the system.
    s->reportMemoryOpStatistics();
    
    
    return 0;   
}
