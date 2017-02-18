/*
>> Stephen Weston
>> 01/25/2017
>> ECE 588
>> Winter 2017
>> A. Alameldeen
*/

/* Homework 3 Header File */

#ifndef _WESTON_HOMEWORK3_H
#define _WESTON_HOMEWORK3_H

// Defines
#define _GNU_SOURCE
#define TRUE true
#define FALSE false
#define MAX_THREADS (16)
#define GRID_SIZE (1000)
#define TIME_STEPS (6000)
#define PRINT_INTERVAL (200)
#define NUM_POINTS (6)
#define CENTER_REGION_X_MIN (200)
#define CENTER_REGION_X_MAX (800)
#define CENTER_REGION_Y_MIN (200)
#define CENTER_REGION_Y_MAX (800)
#define CENTER_REGION_TEMP (500.0)
#define OUTER_REGION_TEMP (0.0)
#define C_X (0.12)
#define C_Y (0.1)

// Includes
#include <assert.h>
#include <float.h>
#include <locale.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

// Typedefs
// Thread Arguments Struct
typedef struct threadArg {
	
	uint32_t arrayminX;	 // Min X-coordinate
	uint32_t arraymaxX;	 // Max X-coordinate
	uint32_t arrayminY;	 // Min Y-coordinate
	uint32_t arraymaxY;	 // Max Y-coordinate
	uint16_t numThreads; // Number of threads
	uint32_t timeSteps;  // Number of time to refresh heat map
	
} threadArg_t;

typedef struct heatMapElement {
	
	double temperature;	// temperature of element
	bool POI;			// flag to tell if this cell is a point of interest to print out
	uint32_t history_i;	// history index used to print out results later
	
} heatMapElement_t;

// Function Prototypes
int main( int argc, char *argv[] );
void *threadTask( void *threadArgsLocal );
void syncThreads( uint16_t numThreads, uint32_t timeSteps );
void calcHeatCell( uint32_t x, uint32_t y );

// Global Variables
pthread_t *threads;			 		// Threads
threadArg_t *threadArgs;	 		// Thread arguments
pthread_mutex_t threadLock;	 		// Mutex
pthread_mutex_t syncLock; 			// Mutex
pthread_cond_t syncCV;    			// Condition variable
uint16_t syncCount;					// Synchronize counter
heatMapElement_t **heatMap;  		// 2-D array for the heatmap
heatMapElement_t **heatMap2; 		// Other array that threads will alternate to
heatMapElement_t **currentHeatMap;  // Currently used heat map (for reading)
heatMapElement_t **targetHeatMap;   // Target heat map (for writing)
double **history;					// Tracks track of history of POI points at the print intervals
FILE *output_file;					// Output of the program

#endif
