/* Authors:
 * Thomas Hanna
 * Michael Comatas
 */
#ifndef _MYMALLOC_H
#define _MYMALLOC_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define BLOCKSIZE 4096

static char heap[BLOCKSIZE] = {2};

#pragma pack(1)
typedef struct _metadata {
	char inUse;
	unsigned int size; //size of data
	struct _metadata* next;
}metadata;	

void* mymalloc(size_t size, const char *file, int line);
void myfree(void* ptr, const char *file, int line);
void allocateNode(metadata* node, size_t size);
void cleanHeap();

#define malloc(x) mymalloc(x,__FILE__,__LINE__)
#define free(x) myfree(x,__FILE__,__LINE__)

enum _bool {FALSE,TRUE};
typedef enum _bool bool;
#endif
