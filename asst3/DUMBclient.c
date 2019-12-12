/* Authors:
 * Thomas Hanna
 * Michael Comatas
 */
#include <stdio.h>
#include <stdlib.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 

int main(int argc, char** argv) 
{
	if( argc != 3 )
	{
		printf( "Error: Invalid arguments. Neep IP address and port.\n" );
		return 0;
	}
	 
	int sock = 0, valread; 
	struct sockaddr_in serv_addr;
	char host[256];
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 
   
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons( atoi( argv[2] ) );
       
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
        	return -1; 
    	} 
   
    	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    	{ 
        	printf("\nConnection Failed \n"); 
        	return -1; 
    	}

	//if connection is successful send hello message to server without prompting user
	char* hello = "HELLO";
	char ready[256] = {0};
	send( sock, hello, strlen( hello ), 0 );//send hello message to server
	valread = read( sock, ready, 256 );
	printf("val:%d\n",valread);
	if( strcmp( ready, "HELLO DUMBv0 ready!" ) != 0 )
	{
		printf( "Error: Connection went wrong" );
		return -1;	
	}
	printf( "%s\n", ready );
	
	while(1)
	{
		char command[256] = {0};//a string to hold the command
		char argument[256] = {0};// a string to hold the argument for the command
		char buffer[1024] = {0};//a string to hold to buffer, the message back from server
		int msgLength = 0;
		printf( "> " );//to indicate to the user to enter a command
		fgets( command, 256, stdin );
		command[strcspn( command, "\n" )] = '\0';//removes the trailing newline from fgets
		if( strcmp( command, "quit" ) == 0 )
		{
			char* goodbye = "GDBYE";
			send( sock, goodbye, strlen( goodbye ), 0 );
			valread = read( sock, buffer, 1024 );
			//if the bytes read is greater than 0 there is a response
			//if( strcmp( buffer, "BYE" ) != 0 )//this is if a response comes back, therefore an error
			if( valread > 0 )
			{
				printf( "<error: enexpected response>\n" );
			}
			else//no response came therefore we can quit successfully
			{
				printf( "<socket can not be read from and was closed on server side>\n" );
				close( sock );
				shutdown( sock, SHUT_RDWR );
				return 0;//return 0 to end the connection
				//break;
			}
		}
		else if( strcmp( command, "create" ) == 0 )
		{
			char* create = "CREAT";
			printf( "Okay, what is the name of the message box?\ncreate:> " );
			fgets( argument, 256, stdin );
			argument[strcspn( argument, "\n" )] = '\0';	
			send( sock, create, strlen( create ), 0 );
			send( sock, argument, strlen( argument ), 0 );
			//valread = read( sock, buffer, 1024 );
			msgLength = recv(sock, buffer, 1024, 0);
			buffer[msgLength] = '\0';
			if( strcmp( buffer, "OK!" ) == 0 )
			{
				printf( "Message box created!\n" );
			}
			else if( strcmp( buffer, "ER:EXIST" ) == 0 )
			{
				printf( "This message box name already exists.\n" );
			}
			else if( strcmp( buffer, "ER:WHAT?" ) == 0 )
			{
				printf( "Your message is in some way broken or malformed.\n" );
			}
		}
		else if( strcmp( command, "delete" ) == 0 )
		{
			char* delete = "DELBX";
			printf( "Okay, which box to delete?\ndelete:> " );
			fgets( argument, 256, stdin );
			argument[strcspn(argument, "\n")] = '\0';	
			send( sock, delete, strlen( delete ), 0 );
			send( sock, argument, strlen( argument ), 0 );
			msgLength = recv(sock, buffer, 1024, 0);
			buffer[msgLength] = '\0';
			if( strcmp( buffer, "OK!" ) == 0 )
			{
				printf( "Message box deleted!\n" );
			}
			else if( strcmp( buffer, "ER:OPEND" ) == 0 )
			{
				printf( "This box is already opened.\n" );
			}
			else if( strcmp( buffer, "ER:NOTMT" ) == 0 )
			{
				printf( "This box is not empty yet.\n" );
			}
			else if( strcmp( buffer, "ER:WHAT?" ) == 0 )
			{
				printf( "Your command is in some way broken or malformed.\n" );
			}
		}
		else if( strcmp( command, "open" ) == 0 )
		{
			char* open = "OPNBX";
			printf( "Okay, open which message box?\nopen:> " );
			fgets( argument, 256, stdin );
			argument[strcspn(argument, "\n")] = '\0';
			send( sock, open, strlen( open ), 0 );
			send( sock, argument, strlen( argument ), 0 );
			msgLength = recv(sock, buffer, 1024, 0);
			buffer[msgLength] = '\0';
			if( strcmp( buffer, "OK!" ) == 0 )
			{
				printf( "Message box opened!\n" );
			}
			else if( strcmp( buffer, "ER:NEXST" ) == 0 )
			{
				printf( "This message box does not exist.\n" );
			}
			else if( strcmp( buffer, "ER:OPEND" ) == 0 )
			{
				printf( "This box is opened by another user.\n" );
			}
			else if( strcmp( buffer, "ER:WHAT?" ) == 0 )
			{
				printf( "Your message is in some way broken or malformed.\n" );
			}
		}
		else if( strcmp( command, "close" ) == 0 )
		{
			char* close = "CLSBX";
			printf( "Okay, close which message box?\nclose:> " );
			fgets( argument, 256, stdin );
			argument[strcspn( argument, "\n" )] = '\0';
			send( sock, close, strlen( close ), 0 );
			send( sock, argument, strlen( argument ), 0 );
			msgLength = recv(sock, buffer, 1024, 0);
			buffer[msgLength] = '\0';
			if( strcmp( buffer, "OK!" ) == 0 )
			{
				printf( "Message box closed!\n" );
			}	
			else if( strcmp( buffer, "ER:NOOPN" ) == 0 )
			{
				printf( "This box is not opened.\n" );//or does not exist
			}
			else if( strcmp( buffer, "ER:WHAT?" ) == 0 )
			{
				printf( "Your message is in some way broken or malformed.\n" );
			}
		}
		else if( strcmp( command, "next" ) == 0 )
		{
			char* next = "NXTMG";
			send( sock, next, strlen( next ), 0 );
			msgLength = recv(sock, buffer, 1024, 0);
			buffer[msgLength] = '\0';
			char check[3] = {0};
			check[0] = buffer[0];
			check[1] = buffer[1];
			check[2] = buffer[2];	
			if( strcmp( check, "OK!" ) == 0 )
			{
				char message[256];
				int counter = 0;
				int i;
				for( i = 0; i < strlen( buffer ); i++ )
				{
					if( buffer[i] == '!' )
					{
						counter++;
					}
					if( counter >= 2 )
					{
						message[i] = buffer[i];
					}
				}
				printf( "%s\n", message );
			}
			else if( strcmp( buffer, "ER:EMPTY" ) == 0 )
			{
				printf( "The message box is empty.\n" );
			}
			else if( strcmp( buffer, "ER:NOOPN" ) == 0 )
			{
				printf( "No message box is opened.\n" );
			}
			else if( strcmp( buffer, "ER:WHAT?" ) == 0 )
			{
				printf( "Your message is in some way broken or malformed.\n" );
			}
		}
		else if( strcmp( command, "put" ) == 0 )
		{
			char* put = "PUTMG";
			printf( "Okay, what's the message?\nmessage:> " );
			fgets( argument, 256, stdin );
			argument[strcspn( argument, "\n" )] = '\0';
			int length = strcspn( argument, "\n" ) + 1;
			char* lenStr = malloc(length + 1);
			snprintf( lenStr, length + 1, "%d", length );
			strcat(lenStr, "!");
			strcat(lenStr, argument);
			send( sock, put, strlen( put ), 0 );
			send( sock, lenStr, strlen( lenStr ), 0 );
			free(lenStr);
			msgLength = recv(sock, buffer, 1024, 0);
			buffer[msgLength] = '\0';
			char check[3] = {0};
			check[0] = buffer[0];
			check[1] = buffer[1];
			check[2] = buffer[2];
			if( strcmp( check, "OK!" ) == 0 )
			{
				printf( "Message was successfully put in the box!\n" );
			}
			else if( strcmp( buffer, "ER:NOOPN" ) == 0 )
			{
				printf( "No message box open.\n" );
			}
			else if( strcmp( buffer, "ER:WHAT?" ) == 0 )
			{
				printf( "Your message is in some way broken or malformed.\n" );
			}
		}
		else if( strcmp( command, "help" ) == 0 )
		{
			printf( "Commands:\nquit\ncreate\ndelete\nopen\nclose\nnext\nput\n" );
		}
		else
		{
			printf( "This is not a commnad, for a command list enter \'help\'.\n" );
		}
	}
    	return 0; 
}
