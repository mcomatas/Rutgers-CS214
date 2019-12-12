/* Authors:
 * Thomas Hanna
 * Michael Comatas
 */
#include "multitest.h"

int currentThread = 0;

const char *mode = "Thread";

//Returns the Difference in Time in ms
float timedif_msec(struct timeval start, struct timeval end)
{
	return (end.tv_sec - start.tv_sec) * 1000.0f + (end.tv_usec - start.tv_usec) / 1000.0f;
}

//a sturct to hold the paramters to pass into dummySearch
typedef struct stuff{
	int* array;
	int target;
	int arraySize;
	int groupSize;
}stuff;

//a function to initialize an array of size n
int* init_arr( int n )
{
	int* array = malloc( n * sizeof( int ) );
	int i;

	for( i = 0; i < n; i++ )
	{
		array[i] = i;
	}

	return array;
}

//swap the values of two elements of an array
void* swap( int* array, int size, int index )
{
	int rando = rand() % size;
	
	int temp = array[index];
	array[index] = array[rando];
	array[rando] = temp;
}

//randomize at least 75% of an array
int* scramble( int n, int* array )
{
	int i;
	int temp = 0;
	int rando = 0;

	srand( time( NULL ) );

	for( i = 0; i < n; i+=4 )
	{
		rando = rand() % n;
		temp = array[i];
		array[i] = array[rando];
		array[rando] = temp;
	}

	for( i = 1; i < n; i += 4 )
	{
		rando = rand() % n;
		temp = array[i];
		array[i] = array[rando];
		array[rando] = temp;
	}

	for( i = 2; i < n; i+=4 )
	{
		rando = rand() % n;
		temp = array[i];
		array[i] = array[rando];
		array[rando] = temp;
	}

	return array;
}

//search funciton that each thread uses
void* dummySearch( void* args )
{
	struct stuff* myData = (struct stuff*)args;
	int i;
	int* b = malloc( sizeof( int ) );
	int n = currentThread++;
	int arrSize = myData->arraySize;
	int group = myData->groupSize;
	int totalThreads;
	if( arrSize % group != 0 )
	{
		totalThreads = ( arrSize / group ) + 1;
	}
	else
	{
		totalThreads = arrSize / group;
	}
	int start = n * ( arrSize / totalThreads );
	
	for( i = start; i < start + group || i < arrSize; i++ )
	{
		if( myData->array[i] == myData->target )
		{
			*b = i;
			//printf( "b is %d\n", *b );
			//printf( "%d found at index %d\n", myData->array[i], i );
			pthread_exit( (void*)b );
		}
	}
	pthread_exit( NULL );
}

//main search function
int indexFind( int num, int* array, int size, int groupSize )
{
	int i;
	int ret = -1;
	int* a;
	struct stuff *data = malloc( sizeof( stuff ) );
	int group;
	if( groupSize > 250 )
	{
		group = 250;
	}
	else
	{
		group = groupSize;
	}
	data->array = array;
	data->target = num;
	data->arraySize = size;
	data->groupSize = group;
	
	int totalThreads;

	if( size <= group )
	{
		totalThreads = 1;
	}
	else
	{
		if( size % group != 0 )
		{
			totalThreads = ( size / group ) + 1;
		}
		else
		{
			totalThreads = size / group;
		}
	}

	pthread_t thread[totalThreads];
	for( i = 0; i < totalThreads; i++ )
	{
		pthread_create( &thread[i], NULL, dummySearch, (void*)data );
	}
	
	for( i = 0; i < totalThreads; i++ )
	{
		pthread_join( thread[i], (void**)&a );
		if( (int*)a != 0 )
		{
			currentThread = 0;
			ret = *(int*)a;
			//free(a);
		}
	}
	free( data );
	printf( "index = %d, array val: %d\n", ret,array[ret] );
	return ret;
}




