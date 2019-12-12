#include "multitest.h"

void test(int groupSize, int size, int* array, int count, int num);

int main( int argc, char** argv )
{
	int* array; 
	int size,i,count,groupSize;
	int num = 42;
	printf("Mode: %s\n", mode);

	//Test 0: 							
	//Array size 10000 
	//# Threads/Proc = 50	

	printf("-----------------------\n");
	printf("Test Plan 0: \n");
	groupSize = 10000/50;
	size = 10000;
	count = 50;
	test(groupSize,size,array,count,num);
	printf("-----------------------\n");

	//Test 1:
	//Array size = 1000 
	//# Threads/Proc = 5	

	printf("-----------------------\n");
	printf("Test Plan 1: \n");
	groupSize = 1000/5;
	size = 1000;
	count = 50;
	test(groupSize,size,array,count,num);
	printf("-----------------------\n");

	//Test 2:
	//Array size = 100
	//# Threads/Proc = 10	

	printf("-----------------------\n");
	printf("Test Plan 2: \n");
	groupSize = 100/10;
	size = 100;
	count = 50;
	test(groupSize,size,array,count,num);
	printf("-----------------------\n");

	//Test 3:
	//Array size = 500 
	//# Threads/Proc = 15	

	printf("-----------------------\n");
	printf("Test Plan 3: \n");
	groupSize = 500/15;
	size = 500;
	count = 50;
	test(groupSize,size,array,count,num);
	printf("-----------------------\n");

	//Test 4: 							
	//Array size = 1000 
	//# Threads/Proc = 20	

	printf("-----------------------\n");
	printf("Test Plan 4: \n");
	groupSize = 1000/20;
	size = 1000;
	count = 50;
	test(groupSize,size,array,count,num);
	printf("-----------------------\n");

	return 0;
}

void test(int groupSize, int size, int* array, int count, int num)
{
	int multi_mode, i;
	struct timeval start;
	struct timeval end;
	double test_time = 0;
	double time_min = 0;
	double time_max = 0;
	double time_avg = 0;
	double temp = 0;
	double std = 0;
	double* times;

	//Initialize and Scramble Array
	array = init_arr(size);
	array = scramble(size,array);
	
	//Used for Storing Time of Each Function
	times = malloc(sizeof(double) * count);

	for(i = 0; i < count; i++)
	{
		printf("Iteration: %d \n", i);

		//Get Time to Run find()
		gettimeofday( &start, 0 );
		multi_mode = find(num,array,size,groupSize);	
		gettimeofday( &end, 0 );

		times[i] = timedif_msec(start,end);

		//Update Minimum
		if(i == 0 || time_min > timedif_msec(start,end))
			time_min = timedif_msec(start,end);

		//Update Maximum
		if(timedif_msec(start,end) > time_max)
			time_max = timedif_msec(start,end);

		//Swap if Index is Found
		if(multi_mode != -1)
			swap(array, size, multi_mode);

		//Update Total Time
		test_time += timedif_msec(start,end);
	}

	//Get Average
	time_avg = test_time / i;

	//Get Sum of Product of each Time - Mean
	for(i = 0; i < count; i++)
	{
		temp = (times[i] - time_avg) * (times[i] - time_avg);
		std += temp;
	}	

	printf("\n");
	printf("Size of array: %d\n", size);
	if(size % groupSize == 0 )
		printf("Number of %s: %d\n", mode, (size/groupSize));
	else
		printf("Number of %s: %d\n", mode, (size/groupSize)+1);
	printf("Value of Number: %d\n", num);
	printf("Time(ms) tot: %f\n", test_time);
	printf("Time(ms) min: %f Time(ms) max: %f\n", time_min, time_max);
	printf("Time(ms) avg: %f Time(ms) std dev: %f\n", time_avg, std/count);
	
	free(array);
	free(times);

	return;
}










