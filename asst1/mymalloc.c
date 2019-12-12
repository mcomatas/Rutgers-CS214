/* Authors:
 * Thomas Hanna
 * Michael Comatas
 */
#include "mymalloc.h"

void* mymalloc(size_t size, const char *file, int line)
{
	metadata* head = (void*)heap;
	
	/*Returns NULL if user tries to insert size <= 0 */
	if(size <= 0)
	{
		fprintf(stderr,"Error: User attempting to "
			"allocate zero or negative space. Returning NULL\n"
			" File: '%s', Line: '%d'\n", file, line);
		return NULL;
	}
	
	/*Returns NULL if user data is too large for the heap*/
	if((sizeof(metadata) + size) > (BLOCKSIZE - sizeof(metadata)))
	{
		fprintf(stderr, "Error: User attempting to "
			"allocate too much space. Returning NULL\n "
			"File: '%s', Line: '%d'\n", file , line);
		return NULL;
	}

	/*takes magic number from array and stores as int*/
	int* magicNum_node = (int*)heap;
	int i;
	int magicNum_original = 2;
	metadata* current;
	
	/*Check if heap hasnt been allocated yet*/
	/*Creates first node if true            */
	if(*magicNum_node == magicNum_original)
	{	
		head->inUse = '0';
		head->size = BLOCKSIZE - sizeof(metadata);
		head->next = NULL;
	}	

	current = head;
	void* retPtr;

	/*iterate trough memory to find the next free node*/
	while(current != NULL)
	{	
		if(current->inUse == '0')
		{
			/*if we find a block with exactly the correct size*/
			if(current->size == size)
			{
				current->inUse = '1';
				retPtr = (void*)(current);
				return retPtr;
			}
			/*else if the given memory size is less than the current nodes size*/
			else if(current->size > size + sizeof(metadata))	
			{
				allocateNode(current, size);
				retPtr = (void*)(current);
				return retPtr;
			}
		}
		current = current->next;
	}
	/*If we get through the while loop and dont */
	/*find any available memory we print this and ret NULL*/
	fprintf(stderr, "Error: User attempting to"
		" allocate too much space. Returning"
		" NULL\n File: '%s', Line: '%d'\n", file, line);

	return NULL;
}

/*Splits current node into two, one with size of users memory*/
/*The other one with the remaining size of the node*/
void allocateNode(metadata* node, size_t size)
{
	//creates new node at new offset then stores new data
	metadata* node2 = (void*)((void*)node + size + sizeof(metadata));

	node2->size = (node->size) - (size + sizeof(metadata));
	node->size = size;
	node2->inUse = '0';
	node->inUse = '1';

	node2->next = node->next;
	node->next = node2;	
}

/*Function for freeing the given pointer*/
void myfree(void* ptr, const char *file, int line)
{
	//creates variable for the head node
	metadata* head = (void*)heap;
	
	//stores the value of the heap into an int
	int* magicNum_node = (int*)heap;	
	int magicNum_original = 2;

	//checks if the pointer given is NULL
	if(ptr == NULL)
	{
		fprintf(stderr,"Error: User attempting to"
			" free NULL pointer.\n File:"
			" '%s', Line: '%d'\n", file, line);
		return;
	}

	//checks if the heap has been touched before
	if(*magicNum_node == magicNum_original)
	{
		fprintf(stderr,"Error: User attempting to"
			" free without prior allocation.\n File:"
			" '%s', Line: '%d'\n", file , line );
		return;
	}
	
	//clean heap function runs before freeing a pointer
	cleanHeap();
	
	//Iterate through the nodes and look to see if the ptr given is inside it
	metadata* current = head;
	int hasPtr = 0;	
	while(current != NULL)
	{
		if(current == ptr)
		{
			if(current->inUse == '0')
			{
				fprintf(stderr,"Error: User attempting to "
					"free previously freed pointer.\n"
					" File: '%s', Line: '%d'\n", file, line);
				return;
			}
			else
			{
				current->inUse = '0';
				hasPtr = 1;
			}
		}
		
		current = current->next;
	}
	if(hasPtr == 0)
	{
		fprintf(stderr,"Error: The pointer you want to"
			" free has not been allocated before.\n"
			" File: '%s', Line: '%d'\n", file, line);
	}

	return;
}

/*Function that finds two unused nodes and merges them together*/
void cleanHeap()
{
	metadata* head = (void*)heap;
	metadata* current = head;
	metadata* nextNode = current->next;
	unsigned short int nextSize;

	/*iterates through all nodes from the start until it finds 2 unused ones*/
	/*After it finds the 2 then starts over from the head until theres no more*/	
	current = head;
	while(current != NULL)
	{
		if(current->inUse == '0' && nextNode->inUse == '0')
		{
			nextSize = nextNode->size;
			nextNode->size = 0;
			current->next = nextNode->next;
			nextNode->next = NULL;
			nextNode = (void*)((void*)current + nextSize + sizeof(metadata));
			current->size = current->size + nextSize + sizeof(metadata);
			current = head;
			nextNode = current->next;
		}
		else
		{
			if(nextNode->next == NULL) return;
			current = current->next;
			nextNode = current->next;
		}
		if(nextNode->next == NULL) return;
	}
	
	return;
}

