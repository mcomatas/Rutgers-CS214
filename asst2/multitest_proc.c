/* Authors:
 * Thomas Hanna
 * Michael Comatas
 */

#include "multitest.h"

const char *mode = "Process";

//Returns the Difference in Time in ms
float timedif_msec(struct timeval start, struct timeval end)
{
	return (end.tv_sec - start.tv_sec) * 1000.0f + (end.tv_usec - start.tv_usec) / 1000.0f;
}

//Swaps an Index with a Different Random Index
void* swap(int* array, int size, int index)
{
	int rando, temp, i;
	
	i = index;

	rando = rand() % size;
	temp = array[i];
	array[i] = array[rando];
	array[rando] = temp;
}

//Initializes an Array of Size N with Sorted Order 
int* init_arr(int n)
{
	int* array = malloc(n * sizeof(int));
	int i = 0;

	for(i = 0; i < n; i++)
		array[i] = i;

	return array;
}

//Randomizes an Array by Swapping 3/4 of The Numbers Randomly
int* scramble(int n, int* array)
{
	int i = 0;
	int temp = 0;
	int rando = 0;
	srand(time(NULL));

	for(i = 0; i < n; i+=4)
	{
		rando = rand() % (n - 1);
		temp = array[i];
		array[i] = array[rando];
		array[rando] = temp;
	}

	for(i = 1; i < n; i+=4)
	{
		rando = rand() % n;
		temp = array[i];
		array[i] = array[rando];
		array[rando] = temp;
	}

	for(i = 2; i < n; i+=4)
	{
		rando = rand() % n;
		temp = array[i];
		array[i] = array[rando];
		array[rando] = temp;
	}

	return array;
}

//Returns Index if Num is in Array, -1 if not Found
int search(int num, int start, int end, int* array)
{
	int i;

	for(i = start; i < end; i++)
	{
		if(array[i] == num)
			return i - start;
	} 
	
	return -1;	
}

//Uses Child Processes to Find the Index of a Number in an Array
int indexFind(int num, int* array, int size, int groupSize)
{
	int start,end,i;
	int status;
	int index = -1;
	int forkNum = (size / groupSize) + 1;
	pid_t id[forkNum];

	if(size % groupSize == 0)
		forkNum = (size / groupSize);

	//We Don't make a Child Process if the Group Size > Size of Array
	if(size <= groupSize)
	{
		index = search(num,0,size,array);

		printf("Process %d found number at index: %d\n", getpid(), index);

		return index;
	}

	if(250 < groupSize || groupSize < 1)
	{
		index = search(num,0,size,array);

		printf("Group Size Error. Searching with 1 Process.\n");
		printf("Process %d found number at index: %d\n", getpid(), index);

		return index;
	}

	//Child Processes find the Index
	for(i = 0; i < forkNum; i++)
	{
	
		if(id[i] = fork() == 0)
		{
			//inside the child process
			start = i * groupSize;

			if(end >= size)
				end = size;
			else
				end = start + groupSize;

			index = search(num,start,end,array);
			exit(index);
		}
	}

	//Wait for Child Process and Recieve Index
	for(i = 0; i < forkNum; i++)
	{
		//inside parent
		pid_t cpid = waitpid(id[i], &status, 0);
		unsigned char exit_code = WEXITSTATUS(status);

		if(exit_code != 255)
		{
			index = exit_code + i * groupSize;
			printf("Child %d found number %d at index: %d\n", cpid, array[index], index);
		}
	}

	return index;
}







