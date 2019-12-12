/* Authors:
 * Thomas Hanna
 * Michael Comatas
 */
#include "mymalloc.h"

void functionA();
void functionB();
void functionC();
void functionD();
void functionE();
void functionF();

int main(int agrc, char* argv[])
{
	struct timeval start;
	struct timeval end;
	double timeOfA = 0;//total time of functionA
	double timeOfB = 0;//total time of funcitonB
	double timeOfC = 0;//total time of functionC
	double timeOfD = 0;//total time of functionD
	double timeOfE = 0;//total time of funcitonE
	double timeOfF = 0;//total time of functionF
	int i;
	
	for( i = 0; i < 100; i++ )//All workloads one after another 100 times
	{
		gettimeofday( &start, 0 );
		functionA();
		gettimeofday( &end, 0 );
		timeOfA += ((end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec));

		gettimeofday( &start, 0 );
		functionB();
		gettimeofday( &end, 0 );
		timeOfB += ((end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec));

		gettimeofday( &start, 0 );
		functionC();
		gettimeofday( &end, 0 );
		timeOfC += ((end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec));

		gettimeofday( &start, 0 );
		functionD();
		gettimeofday( &end, 0 );
		timeOfD += ((end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec));

		gettimeofday( &start, 0 );
		functionE();
		gettimeofday( &end, 0 );
		timeOfE += ((end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec));

		gettimeofday( &start, 0 );
		functionF();
		gettimeofday( &end, 0 );
		timeOfF += ((end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec));
	}

	printf( "Average time for workload A: %f milliseconds\n", timeOfA/100 );
	printf( "Average time for workload B: %f milliseconds\n", timeOfB/100 );
	printf( "Average time for workload C: %f milliseconds\n", timeOfC/100 );
	printf( "Average time for workload D: %f milliseconds\n", timeOfD/100 );
	printf( "Average time for workload E: %f milliseconds\n", timeOfE/100 );
	printf( "Average time for workload F: %f milliseconds\n", timeOfF/100 );

  return 0;
}


/* malloc() 1 byte and immediately free it - do this 150 time
 */
void functionA()
{
	int i;//malloc and free index
	char* arr[4096];
	for( i = 0; i < 150; i++ )
	{
		arr[i] = (char*)malloc( 1 );
		free( arr[i] );
	}
	return;
}

/* malloc() 1 byte, store the pointer in an array - do this 150 times
 * Once you've malloc()ed 50 byte chuncks, then free() the 50 1 byte pointers one by one
 */
void functionB()
{
	int i, j;//malloc and free indices
	char* arr[4096];

	for( i = 0; i < 150; i++ )
	{
		arr[i] = (char*)malloc(1);
		if( (i + 1) % 50 == 0 )
		{
			for( j = i - 49; j < (i + 1); j++ )
			{
				free( arr[j] );
			}
		}
	}
	return;
}


/*Randomly choose between a 1 byt malloc() or free()ing a 1 byte pointer
 *>do this until you have alloctes 50 times
 *Keep track of each operation so that you eventaully malloc() 50 bytes, in total
 *>if you have already allocated 50 times, disregard the random and just free() on each iteration
 *Keep track of each operation so that you eventaully free() all pointers
 *>don't allow a free() if you have no pointers to free()
 */
void functionC()
{
	int totalMallocs = 0;//total amount of mallocs
	int i = 0;//malloc index
	int j = 0;//free index
	int random;//random number either 0 or 1
	char* arr[4096];
	char check[4096];
	srand(time(NULL));//sets random seed so numbers are random each time the program runs

	while( totalMallocs < 50 && j < BLOCKSIZE)
	{
		random = rand() % 100 + 1;;//get a random interger either 0 or 1
		
		if( random % 2 == 0 )//if random number is 0, malloc
		{
			arr[i] = (char*)malloc(1);
			check[i] = 'a';
			totalMallocs++;
			i++;
		}
		else if( random % 2 != 0 && check[j] == 'a' 
			&& j < totalMallocs)//if random number is 1, free
		{
			free(arr[j]);
			check[j] = 'b';
			j++;
		}
	}

	j = 0;
	while( j < totalMallocs  )
	{
		if( check[j] == 'a' )
		{
			free( arr[j] );
		}
		j++;
	}
	return;
}

/*
 *Randomly choose between a randomly-sized malloc() or free()ing a pointer - do this many times
 *>Keep track of each malloc so that all mallocs do not exceed your total memory capacity
 *>Keep track of each operation so that you eventually malloc() 50 times
 *>Keep track of each operation so that you eventaully free() all pointers
 *>Choose a random allocation size between 1 and 64 bytes
 */
void functionD()
{
	int totalMallocs = 0;//total amount of mallocs
	int totalCapacity = 0;//how much memory has been allocated so far
	char check[4096];
	char* arr[4096];	
	int i = 0;//malloc index
	int j = 0;//free index
	int random;//random number either 0 or 1
	int mallocSize;//random integer between 1 and 64
	srand(time(NULL));//sets random seed so random numbers are different every time

	while( totalMallocs < 50 && totalCapacity < (BLOCKSIZE - sizeof(metadata))
		&& j < BLOCKSIZE)
	{
		random = rand() % 100 + 1;//get a random integer, either 0 or 1
		if( random % 2 == 0 )//if random is 0, malloc
		{
			mallocSize = ( rand() % 63 ) + 1;
			totalCapacity = totalCapacity + mallocSize;
			arr[i] = (char*)malloc( mallocSize );
			check[i] = 'a';
			totalMallocs++;
			i++;
		}
		else if( random % 2 != 0 && check[j] == 'a' 
			&& j < totalMallocs)//if random is 1, free
		{
			free(arr[j]);
			check[j] = 'b';
			j++;
		}
	}
	
	j = 0;
	while( j < totalMallocs )
	{	
		if( check[j] == 'a' )
		{
			free( arr[j] );
		}
		j++;
	}
	return;
}

/* Testcases E and F described in more detail in testplan.txt
 */
void functionE()
{
	int i = 0;//malloc index
	int j = 0;
	int totalMallocs = 0;
	int totalCapacity = 0;
	char* arr[4096];

	//this mallocs 11 times with size 300 blocks to fill the malloc capacity with no possible errors
	for( i = 0; i < 11; i++ )
	{
		arr[i] = (char*)malloc(300);
	}
	for( i = 0; i < 11; i++ )//then frees everything
	{
		free( arr[i] );
	}
	return;
}

void functionF()
{
	int i = 0;//malloc index
	int j = 0;
	int totalMallocs = 0;
	int totalCapacity = 0;
	char* arr[4096];

	//this mallocs 11 times with size 300 blocks like E
	for( i = 0; i < 11; i++ )
	{
		arr[i] = (char*)malloc( 300 );
	}
	for( i = 0; i < 11; i+=2 )//then frees every other block
	{
		free( arr[i] );
	}
	for( i = 0; i < 11; i+=2 )//then re mallocs the freed blocks with size 200
	{
		arr[i] = malloc( 200 );
	}
	for( i = 0; i < 11; i++ )//then frees everything
	{
		free( arr[i] );
	}
	return;
}
