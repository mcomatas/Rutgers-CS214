#include <stdio.h>
#include <stdlib.h>

int stringLength( char* s )
{
	int i = 0;
	while( s[i] != '\0' )
	{
		i++;
	}
	return i;
}

char* subString( char* s, int a, int b )
{
	int length = b - a;
	char* dest = (char*)malloc(sizeof(char) * (length+1));
	int i;
	for( i = a; i < b && (*s != '\0'); i++ )
	{
		*dest = *(s + i);
		dest++;
	}
	*dest = '\0';
	return dest - length;
}

int main( int argc, char** argv )
{
	//char* t = ";";
	//char* s = subString( t, 0, 0 );

	//int length = stringLength( t );
	//
	
	int i;

	for( i = 0; i < 50; i++ )
	{
		int random = (int)(rand() / (double)(RAND_MAX) * (2));	

		printf( "%d\n", random );
	}	

	return 0;
}
