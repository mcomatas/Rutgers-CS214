/* Authors:
 * Thomas Hanna
 * Michael Comatas
 */

#ifndef _MULTITEST_H
#define _MULTITEST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>

extern const char *mode;
int* init_arr(int n);
int* scramble(int n, int* array);
void* swap(int* array, int size, int index);
float timedif_msec(struct timeval start, struct timeval end);
int indexFind(int num, int* array, int size, int groupSize);

#define find(x,y,z,a) indexFind(x,y,z,a)

#endif
