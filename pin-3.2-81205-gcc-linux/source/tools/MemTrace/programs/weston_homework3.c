/*
>> Stephen Weston
>> 01/25/2017
>> ECE 588
>> Winter 2017
>> A. Alameldeen
*/

/* 
>> Homework 3: Use pthreads and the POSIX library simulate a heat map for a 1000x1000 grid for 6000 time steps.
>> 				Certain points' temperatures will be printed out every 200 steps.
>> 
>> Algorithm Description:
>> A grid of size 1000x1000 has center region (200,200) to (800,800) of 500 degrees initially and
>>  all other grid spaces are 0 degrees. Each grid point's next temperature is calculated by:
>>  T(x,y)(t) = T(x,y)(t-1) + Cx * (T(x+1,y)(t-1) + T(x-1,y)(t-1) – 2 T(x,y)(t-1))
>>  + Cy * (T(x,y+1) (t-1) + T(x,y-1) (t-1) – 2 T(x,y) (t-1)), where Cx = 0.12 and Cy = 0.1
>> 
>> Sources Used:
>> https://computing.llnl.gov/tutorials/parallel_comp/#ExamplesHeat
>> 
*/

#include "weston_homework3.h"

// Main function
int main( int argc, char *argv[] ) {
	
	setlocale( LC_NUMERIC, "" );	// Lets decimal numbers be printed with commas in printf
	
	// Read arguments
	if( argc == 1 ) { // Arguments are required - error & exit if none are provided
		printf( "ERROR: No arguments supplied. Run program with \"-h\" option for help.\n" );
		return -1;
	}
	
	// Parsing variables
	uint32_t retVal; 	 // Return value
	extern char *optarg; // Supplied by library
	uint32_t int_optarg; // int conversion of arguments
	uint32_t opt;		 // Return value of getopt()
	bool tFlag = FALSE;	 // Flag to make sure user enters number of threads
	uint16_t numThreads; // Number of threads to use
	
	do {  // Parse any arguments
	
		opt = getopt( argc, argv, "ht:" );
		switch(opt) {
			
			case 'h':	// "-h" shows help screen options
			
				printf( "Options:\n\t-h: Displays help\n\t-t: Number of threads\n" );
				return 0;
				break;
				
			case 't':	// "-t" allows user to enter number of threads (1-16)
			
				int_optarg = atol( optarg );
				
				if( int_optarg >= 1 && int_optarg <= MAX_THREADS ) {
					
					tFlag = TRUE;
					numThreads = int_optarg;
					
				}
				else {
					
					printf( "Invalid number of threads received. Enter a number between 1-16.\nExiting...\n" );
					return -1;
					
				}
				
				break;
				
			default:
			
				break;
			
		}
		
	} while( opt != -1 );
	
	if( tFlag == FALSE ) {
		
		printf( "ERROR: Not all arguments were supplied. Please enter number of threads.\nUse \"-h\" to see all options.\n" );
		return -1;
		
	}
	else {
		printf( "Running heat map calculation with %d threads.\n", numThreads );
	}
	
	// Misc. Variables
	uint32_t i, j, k;			// Looping indices
	uint32_t temp1, temp2;		// temp
	struct timespec tick, tock;	// Stopwatch
	uint64_t time_elapsed_ns;	// Time elapsed in ns
	double time_elapsed;		// Time elapsed in program
	pthread_attr_t attr;  		// Thread attributes
	
	// Allocate space for the threads, thread arguments, and heat maps
	threads = malloc( numThreads * sizeof( *threads ) );
	threadArgs = malloc( numThreads * sizeof( *threadArgs ) );
	
	heatMap = malloc( (GRID_SIZE+2) * sizeof( *heatMap ) );
	for( i = 0; i < (GRID_SIZE+2); i++ ) {
		heatMap[i] = malloc( (GRID_SIZE+2) * sizeof( *heatMap[i] ) );
	}
	
	heatMap2 = malloc( (GRID_SIZE+2) * sizeof( *heatMap2 ) );
	for( i = 0; i < (GRID_SIZE+2); i++ ) {
		heatMap2[i] = malloc( (GRID_SIZE+2) * sizeof( *heatMap2[i] ) );
	}
	
	// Make sure threads are joinable
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
	pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
	
	// Initialize mutexes and conditional variables
	retVal = pthread_mutex_init( &threadLock, NULL );
	assert( retVal == 0 );
	retVal = pthread_mutex_init( &syncLock, NULL );
	assert( retVal == 0 );
	retVal = pthread_cond_init( &syncCV, NULL );
	assert( retVal == 0 );
	syncCount = 0;
	
	// Initialize the heat maps
	k = 0;
	for( i = 0; i < ( GRID_SIZE + 2 ); i++ ) {
		for( j = 0; j < ( GRID_SIZE + 2 ); j++ ) {
			
			if( ( i == 1 && j == 1 ) || ( i == 150 && j == 150 ) || ( i == 400 && j == 400 ) || \
				( i == 500 && j == 500 ) || ( i == 750 && j == 750 ) || ( i == 900 && j == 900 ) ) {
			
				heatMap[i][j].POI = TRUE;
				heatMap2[i][j].POI = TRUE;
				heatMap[i][j].history_i = k;
				heatMap2[i][j].history_i = k;
				k += 1;
				assert( k <= NUM_POINTS );
				
			}
			
			else {
				heatMap[i][j].POI = FALSE;
				heatMap2[i][j].POI = FALSE;
				heatMap[i][j].history_i = 0;
				heatMap2[i][j].history_i = 0;
			}
			
			if( i >= CENTER_REGION_X_MIN && i <= CENTER_REGION_X_MAX && j >= CENTER_REGION_Y_MIN && j <= CENTER_REGION_Y_MAX ) {
				heatMap[i][j].temperature = CENTER_REGION_TEMP;
				heatMap2[i][j].temperature = CENTER_REGION_TEMP;
			}
			
			else {
				heatMap[i][j].temperature = OUTER_REGION_TEMP;
				heatMap2[i][j].temperature = OUTER_REGION_TEMP;
			}
			
		}
	}
	
	// Initialize the history array
	history = malloc( NUM_POINTS * sizeof( *history ) );
	for( i = 0; i < NUM_POINTS; i++ ) {
		history[i] = malloc( ( TIME_STEPS / PRINT_INTERVAL + 1 ) * sizeof( *history[i] ) );
	}
	
	// Will be switching between the two structures to avoid contention over writing
	currentHeatMap = heatMap;
	targetHeatMap = heatMap2;
	
	// Open the file to be written (this file has no extension)
	output_file = fopen( "hw3_output.txt", "w" );
	if( output_file == NULL ) {
		printf( "ERROR: Could not open output file. Exiting...\n" );
		return -1;
	}
	
	// Get everything ready to start threads...
	temp1 = 0;
	temp2 = GRID_SIZE;
	for( i = 0; i < numThreads; i++ ){
		
		threadArgs[i].numThreads = numThreads;
		threadArgs[i].timeSteps = 0;
		
		// Set number of columns for each thread, taking into account the number of columns may not divide evenly.
		if( temp2 % numThreads == 0 ) {
			threadArgs[i].arrayminX = i * ( temp2 / numThreads ) + temp1 + 1;
			threadArgs[i].arraymaxX = ( i + 1 ) * ( temp2 / numThreads ) + temp1;
			threadArgs[i].arrayminY = 1;
			threadArgs[i].arraymaxY = GRID_SIZE;
		}
		else {
			threadArgs[i].arrayminX = i * ( temp2 / numThreads ) + temp1 + 1;
			temp1 += 1;
			threadArgs[i].arraymaxX = ( i + 1 ) * ( temp2 / numThreads ) + temp1;
			threadArgs[i].arrayminY = 1;
			threadArgs[i].arraymaxY = GRID_SIZE;
			temp2 -= 1;
		}
		
		//printf( "Thread %d: x min: %d, x max: %d, y min: %d, y max: %d\n", i, threadArgs[i].arrayminX, threadArgs[i].arraymaxX, threadArgs[i].arrayminY, threadArgs[i].arraymaxY );
		//char debug[127];
		//gets(debug);
		
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
	
	// Stop stopwatch and print time elapsed
	clock_gettime( CLOCK_MONOTONIC, &tock );
	time_elapsed_ns = (1000000000 * tock.tv_sec + tock.tv_nsec) - (1000000000 * tick.tv_sec + tick.tv_nsec);
	time_elapsed = (double)time_elapsed_ns / 1000000000.0;
	printf( "Time = %lldns (%.9f sec)\n", time_elapsed_ns, time_elapsed );
	
	// Print out the results
	for( k = 0; k < ( TIME_STEPS / PRINT_INTERVAL + 1 ); k++ ) {
		
		fprintf( output_file, "Time Step %ld:\n----------------------------------------\n", ( k * PRINT_INTERVAL ) );
		
		for( i = 0; i < ( GRID_SIZE + 2 ); i++ ) {
			
			for( j = 0; j < ( GRID_SIZE + 2 ); j++ ) {
				
				if( currentHeatMap[i][j].POI == TRUE ) {
					
					fprintf( output_file, "Point (%ld, %ld): %.6f\n", i, j, history[currentHeatMap[i][j].history_i][k] );
						
				}
					
			}
		}
		
		fprintf( output_file, "\n" );
		
	}
	
	// Clean up...
	pthread_attr_destroy( &attr );
	pthread_mutex_destroy( &threadLock );
	pthread_mutex_destroy( &syncLock );
	pthread_cond_destroy( &syncCV );
	pthread_exit( NULL );
	
	// EXIT
	return 0;
}

// All threads enter this function
void *threadTask( void *threadArgsLocal ) {
	
	threadArg_t *threadData = (threadArg_t *) threadArgsLocal;	// Thread Data is local pointer (converting back from void*)
	uint32_t i, j;	// Loop indices
	
	while( threadData->timeSteps < TIME_STEPS ) {
		
		// Calculate new values for heat map
		for( i = threadData->arrayminX; i <= threadData->arraymaxX; i++ ) {
			for( j = threadData->arrayminY; j <= threadData->arraymaxY; j++ ) {
				
				calcHeatCell( i, j );
				
				// Print out data for points of interest at certain intervals
				if( ( threadData->timeSteps == 0 ) && ( currentHeatMap[i][j].POI == TRUE ) ) {
					
					history[currentHeatMap[i][j].history_i][0] = currentHeatMap[i][j].temperature;
					//printf( "Point (%ld, %ld) is %.6f degrees at time step %d.\n", i, j, currentHeatMap[i][j].temperature, ( threadData->timeSteps ) );
					
				}
				
				else if( ( ( threadData->timeSteps + 1 ) % PRINT_INTERVAL == 0 ) && ( targetHeatMap[i][j].POI == TRUE ) ) {
					
					history[targetHeatMap[i][j].history_i][( threadData->timeSteps + 1 ) / PRINT_INTERVAL] = targetHeatMap[i][j].temperature;
					//printf( "Point (%ld, %ld) is %.6f degrees at time step %d.\n", i, j, targetHeatMap[i][j].temperature, ( threadData->timeSteps + 1 ) );
					
				}
				
			}
		}
		
		threadData->timeSteps += 1;
		syncThreads( threadData->numThreads, threadData->timeSteps );
	}
	
	// EXIT thread
	pthread_exit( NULL );
	
}

void syncThreads( uint16_t numThreads, uint32_t timeSteps ) {	// Function to make sure all of the threads synchronize

  uint32_t retVal;	// Return value
  uint32_t i, j;	// Loop indices
  
  pthread_mutex_lock( &syncLock );
  syncCount++;
  
  if( syncCount == numThreads ) {
	
	// Switch active heat map
	if( timeSteps % 2 == 0 ) {
		currentHeatMap = heatMap;
		targetHeatMap = heatMap2;
	}
	else {
		currentHeatMap = heatMap2;
		targetHeatMap = heatMap;
	}
	
	syncCount = 0;	// Reset sync counter	
    retVal = pthread_cond_broadcast( &syncCV );	// Get all threads going again
    assert( retVal == 0 );
	
  } 
  else {
	  
    retVal = pthread_cond_wait( &syncCV, &syncLock ); 
    assert( retVal == 0 );
	
  }
  
  pthread_mutex_unlock( &syncLock );
  
}

void calcHeatCell( uint32_t x, uint32_t y ) {
	
	assert( x >= 0 && y >= 0 && x <= GRID_SIZE + 1 && y <= GRID_SIZE + 1 );
	
	targetHeatMap[x][y].temperature = currentHeatMap[x][y].temperature + \
		( C_X * ( currentHeatMap[x+1][y].temperature + currentHeatMap[x-1][y].temperature - 2.0 * currentHeatMap[x][y].temperature )) + \
		( C_Y * ( currentHeatMap[x][y+1].temperature + currentHeatMap[x][y-1].temperature - 2.0 * currentHeatMap[x][y].temperature ));
	
}

