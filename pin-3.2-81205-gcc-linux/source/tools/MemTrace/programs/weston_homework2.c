/*
>> Stephen Weston
>> 01/25/2017
>> ECE 588
>> Winter 2017
>> A. Alameldeen
*/

/* 
>> Homework 1: Use pthreads and the POSIX library to use parallel programming to calculate pi
>> 
>> Algorithm used: random points are generated within a square of size 1 x 1
>>   A circle of radius 0.5 is inscribed within the square, and pi is approximated by
>>   4 times the number of points that are generated within the circle divided by the
>>   total number of points generated.
>>
>> Sources Used:
>>   https://computing.llnl.gov/tutorials/parallel_comp/#ExamplesPI (description of algorithm)
>>   https://computing.llnl.gov/tutorials/mpi/samples/C/mpi_pi_reduce.c (sample code using MPI)
*/

// Defines
#define _GNU_SOURCE
#define RADIUS   (0.5f)
#define CIRCLE_X (0.5f)
#define CIRCLE_Y (0.5f)
#define TRUE 	 true
#define FALSE 	 false
#define MASTER	 (0)

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
typedef struct {
	
	uint32_t numPoints;		// Number of points that the thread must do
	
} threadArg_t;

typedef double DOUBLE_T;

// Function Prototypes
int main( int argc, char *argv[] );
void *threadTask( void *threadArgsLocal );
bool isWithinCircle( DOUBLE_T circRadius, DOUBLE_T circCenterX, DOUBLE_T circCenterY, DOUBLE_T pointX, DOUBLE_T pointY, DOUBLE_T c_squared );

// Global Variables
pthread_t *threads;			// Threads
threadArg_t *threadArgs;	// Thread arguments
pthread_mutex_t threadLock;	// Mutex
uint32_t pointSum;			// Sum of all circle points

// Main function
int main( int argc, char *argv[] ) {
	
	// Characteristic Variables
	uint32_t totalPoints;	// Number of points to generate (total)
	uint32_t numPoints;		// Number of points to generate
	uint16_t numThreads;	// Number of threads to use
	
	// Misc. variables
	uint32_t i;		 			// Looping index
	uint32_t retVal; 			// Return value
	struct timespec tick, tock;	// Stopwatch
	double time_elapsed;		// Time elapsed in program
	
	setlocale( LC_NUMERIC, "" );	// Lets decimal numbers be printed with commas in printf
	srand( (uint32_t)time(NULL) );	// Give seed to random number generator
	pointSum = 0;
	
	// Read arguments
	if( argc == 1 ) { // Arguments are required - error & exit if none are provided
		printf( "ERROR: No arguments supplied. Run program with \"-h\" option for help.\n" );
		return -1;
	}
	
	extern char *optarg; // Supplied by library
	uint32_t int_optarg; // int conversion of arguments
	uint32_t opt;		 // Return value of getopt()
	bool tFlag = FALSE;	 // Flag to make sure user enters number of threads
	bool pFlag = FALSE;  // Flag to make sure user enters number of points
	
	do {  // Parse the arguments
	
		opt = getopt( argc, argv, "ht:p:" );
		switch(opt) {
			
			case 'h':	// "-h" shows help screen options
			
				printf( "Options:\n\t-h: Displays help\n\t-t: Number of threads\n\t-p: Number of points to use in algorithm\n" );
				return 0;
				break;
				
			case 't':	// "-t" allows user to enter number of threads (1-16)
			
				int_optarg = atol( optarg );
				
				if( int_optarg >= 1 && int_optarg <= 16 ) {
					
					tFlag = TRUE;
					numThreads = int_optarg;
					
				}
				else {
					
					printf( "Invalid number of threads received. Enter a number between 1-16.\nExiting...\n" );
					return -1;
					
				}
				
				break;
			
			case 'p':	// "-p" allows user to enter number of points to randomly generate for pi calculation
			
				int_optarg = atol( optarg );
				
				if( int_optarg >= 1000 && int_optarg <= 1000000000 ) {
					
					pFlag = TRUE;
					numPoints = int_optarg;
					totalPoints = int_optarg;
					
				}
				else {
					
					printf( "Invalid number of points received. Enter a decimal number between 1 thousand and 1 billion.\nExiting...\n" );
					return -1;
					
				}
				
				break;
		}
		
	} while( opt != -1 );
	
	if( tFlag == FALSE || pFlag == FALSE ) {
		
		printf( "ERROR: Not all arguments were supplied. Please enter both number of threads and number of points.\nUse \"-h\" to see all options.\n" );
		return -1;
		
	}
	else {
		printf( "Running pi calculation with %d threads and %'d points.\n", numThreads, totalPoints );
	}
	
	// Important Variables
	pthread_t threads_arr[numThreads];		// Threads array
	pthread_attr_t attr;					// Thread attributes
	threadArg_t threadArgs_arr[numThreads];	// Thread arguments array
	threads = threads_arr;					// Link to global pointer
	threadArgs = threadArgs_arr;			// Link to global pointer
	
	// Make sure threads are joinable
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
	pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
	retVal = pthread_mutex_init( &threadLock, NULL );
	assert( retVal == 0 );
	
	// Get everything ready to start threads...
	for( i = 0; i < numThreads; i++ ){
		
		// Set number of points for each thread, taking into account the number may not divide evenly.
		if(numPoints % numThreads == 0)
			threadArgs[i].numPoints = numPoints / numThreads;
		else {
			threadArgs[i].numPoints = numPoints / numThreads + 1;
			numPoints -= 1;
		}
		
	}
	
	// Start stopwatch
	clock_gettime( CLOCK_MONOTONIC, &tick );
	
	// Create all threads
	for( i = 0; i < numThreads; i++ ){
		retVal = pthread_create( &threads[i], &attr, threadTask, (void *) &threadArgs[i] );
		if( retVal != 0 ){
			printf( "Error occurred while creating thread #%d, return code from pthread_create is %d.\n", i, retVal );
			exit(-1);
		}
	}
	
	// Wait for all threads to finish...
	for( i = 0; i < numThreads; i++ )
		pthread_join( threads[i], NULL );
	
	// Calculate PI
	DOUBLE_T PI = 4.0 * (DOUBLE_T)pointSum / (DOUBLE_T)totalPoints;
	printf( "Value of pi calculated is: %1.22f\n", PI );
	
	// Stop stopwatch and print time elapsed
	clock_gettime( CLOCK_MONOTONIC, &tock );
	time_elapsed = (tock.tv_sec - tick.tv_sec) + (tock.tv_nsec - tick.tv_nsec) / 1000000000.0;
	printf( "Time elapsed: %.6f sec\n", time_elapsed );
	
	// Clean up...
	pthread_attr_destroy( &attr );
	pthread_mutex_destroy( &threadLock );
	pthread_exit( NULL );
	
	// EXIT
	return 0;
}

// The threads enter this function
void *threadTask( void *threadArgsLocal ) {
	
	threadArg_t *threadData = (threadArg_t *) threadArgsLocal;	// Thread Data is local pointer (converting back from void*)
	uint32_t numPoints = threadData->numPoints;
	DOUBLE_T r1, r2;	// Random numbers
	bool inCircle;     	// Boolean value for if the points are in the circle
	uint32_t i;			// Index looping
	uint32_t circCount = 0;
	
	DOUBLE_T c_squared = RADIUS * RADIUS;	// Calculate this early to avoid taking many square roots
	
	// Go through all of the points for this task
	while( numPoints > 0 ) {
		
		// First, generate two random numbers for x-y coordinates (between 0.0 and 1.0)
		r1 = (DOUBLE_T)rand()/(DOUBLE_T)(RAND_MAX);
		r2 = (DOUBLE_T)rand()/(DOUBLE_T)(RAND_MAX);
		
		// See if the point is in the circle
		inCircle = isWithinCircle( RADIUS, CIRCLE_X, CIRCLE_Y, r1, r2, c_squared );
		
		// Increment counter if point is in circle
		if( inCircle == TRUE )
			circCount += 1;
		
		numPoints -= 1;
		
	}
	
	// Add circle points to global sum
	pthread_mutex_lock( &threadLock );
    pointSum += circCount;
    pthread_mutex_unlock( &threadLock );
	
	// EXIT thread
	pthread_exit( NULL );
	
}

// Checks to see if a point is inside the circle
bool isWithinCircle( DOUBLE_T circRadius, DOUBLE_T circCenterX, DOUBLE_T circCenterY, DOUBLE_T pointX, DOUBLE_T pointY, DOUBLE_T c_squared ) {
	
	// Use distance formula to check distance. Uses comparison to c squared to avoid using slow sqrt() function
	//  Also avoids using slow pow() function
	DOUBLE_T distance_squared = (( circCenterX - pointX ) * ( circCenterX - pointX )) + (( circCenterY - pointY ) * ( circCenterY - pointY ));
	if(distance_squared <= c_squared)
		return TRUE;
	else
		return FALSE;
	
}
