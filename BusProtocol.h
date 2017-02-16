#ifndef BUSPROTOCOL_H
#define BUSPROTOCOL_H

// Address to send message to all nodes on bus
#define BROADCAST_ADX 0

// List of possible bus commands from a processor
// for its local memory operations
#define BUSREAD     0
#define BUSWRITE    1

// Response type commands
// Data for a bus read returned from memory
#define DATA_RETURN_MEMORY    2
// Data for a bus read returned from a processor
#define DATA_RETURN_PROCESSOR 3

#define IS_DATA_RETURN(a) ((a == DATA_RETURN_MEMORY) || (a == DATA_RETURN_PROCESSOR))


#endif //BUSPROTOCOL

