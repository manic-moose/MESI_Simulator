#ifndef BUSPROTOCOL_H
#define BUSPROTOCOL_H

// Address to send message to all nodes on bus
#define BROADCAST_ADX 0

// List of possible bus commands from a processor
// for its local memory operations
#define BUSREAD     0
#define BUSREADX    1
#define BUSWRITE    2

// Response type commands
// Data for a bus read returned from memory
#define DATA_RETURN_MEMORY    3
// Data for a bus read returned from a processor
#define DATA_RETURN_PROCESSOR 4

#define INVALIDATE 5

// This is just use as a dummy output to 
// simulate bursting data across several
// cycles.
#define NULL_BURST 6

#define IS_DATA_RETURN(a) ((a == DATA_RETURN_MEMORY) || (a == DATA_RETURN_PROCESSOR))


#endif //BUSPROTOCOL

