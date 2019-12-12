/* Authors:
 * Thomas Hanna
 * Michael Comatas
 */

#include <unistd.h> 
#include <string.h>
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <pthread.h>
#include <sys/time.h>
#include <ctype.h>
#include <signal.h>
#include <semaphore.h>


//Globals
pthread_t tids[256];
int sockets[256];
int* connections;
int* killFlag;
sem_t db_lock;

//Should Handle Kill ctr-c
void handle_sigINT(int signum){
	*killFlag = 1;
}

//Structs for Network Params
typedef struct _acceptParams{
	int sfd;
	struct sockaddr* addr;
	socklen_t* slen;
} acceptParams;

typedef struct _sfdPill{
	int sfd;
	
} sfdPill;

//Structs for Mailbox/Message Data Structures
typedef struct _queue{
	char* message;
	char* origMessage;
	struct _queue* next;
} queue;

typedef struct _mBox{
	char* name;
	struct _queue* messages;
	int inUse;
	struct _mBox* next;
} mBox;

typedef struct _user{
	int bOpen;
	struct _mBox* openBox;
} user;

mBox* head;
user* curUser;

//Two Functions for Showing Client Functions to Server
void eventOut(char* sfd, char* event)
{
	time_t now;
	time(&now);
	printf("%s %s %s", sfd, event, ctime(&now));
}

void errorOut(char* sfd, char* error)
{
	time_t now;
	time(&now);
	printf("%s %s %s", sfd, error, ctime(&now));

}

void HELLO(char* arg, int sfd)
{
	char* message = "HELLO DUMBv0 ready!";
	send(sfd , message , strlen(message) , 0 ); 
	eventOut("client", "HELLO");
	return;
}

void GDBYE(char* arg, int sfd)
{
	eventOut("client", "disconnected");

	return;
}

//Mailbox function to retrieve mailbox with name (name)
mBox* getBox(char* name){
	//sem_wait(&db_lock);
	if(head == NULL){
		return NULL;
	}
	if(name == NULL){
		return NULL;
	}
	mBox* ptr = head;
	while(ptr != NULL){
		if((strcmp(ptr->name, name)) == 0){
			return ptr;
		} else{
			ptr = ptr->next;
		}
	}
	//sem_post(&db_lock);
	return NULL;
}

//Used to determine if a mailbox has the name (name)
int exists(char* name)
{
	//sem_wait(&db_lock);
	if(head == NULL){
		return 0;
	}
	if(name == NULL){
		return 0;
	}
	mBox* ptr = head;
	while(ptr != NULL){
		if((strcmp(ptr->name, name)) == 0){
			return 1;
		} else{
			ptr = ptr->next;
		}
	}
	//sem_post(&db_lock);
	return 0;
}

//Appends a mailbox onto the LL
void append(mBox* new){
	//sem_wait(&db_lock);
	if(new == NULL){
		return;
	} else if(head == NULL){
		head = new;
		return;
	}

	mBox* ptr = head;
	while(ptr->next != NULL){
		ptr = ptr->next;
	}
	ptr->next = new;
	//sem_post(&db_lock);
	return;
}

//Create Mailbox
void CREAT(char* arg, int sfd)
{
	//sem_wait(&db_lock);
	//if message box is not in correct format
	if(strlen(arg) > 25 || strlen(arg) < 5 || !isalpha(arg[0]) )
	{
		send(sfd , "ER:WHAT?" , strlen("ER:WHAT?") , 0 );
		errorOut("client", "ER:WHAT?");

		return;
	}
	int exist = exists(arg);
	//if box alredy exists
	if(exist)
	{
		errorOut("client", "ER:EXIST");
		send(sfd , "ER:EXIST" , strlen("ER:EXIST") , 0 );
		return;
	}
	
	mBox* new = (mBox*)malloc(sizeof(mBox));
	new->name = (char*)malloc(sizeof(char)*strlen(arg));
	strcpy(new->name, arg);
	new->messages = NULL;
	new->inUse = 0;
	new->next = NULL;
	append(new);

	send(sfd , "OK!" , strlen("OK!") , 0 );

	eventOut("client", "CREAT");
	
	//sem_post(&db_lock);

	return;
}

//Opens box
void OPNBX(char* arg, int sfd)
{
	//if message box is not in correct format
	if(strlen(arg) > 25 || strlen(arg) < 5 || !isalpha(arg[0]) )
	{
		errorOut("client", "ER:WHAT?");
		send(sfd , "ER:WHAT?" ,strlen("ER:WHAT?") , 0 );
		return;
	}

	int exist = exists(arg);
	//if box alredy exists
	if(!exist)
	{
		send(sfd , "ER:NEXST" , strlen("ER:NEXST") , 0 );
		errorOut("client", "ER:NEXST");
		return;
	}
	
	mBox* box = getBox(arg);
	//If box is in use return
	if(box->inUse)
	{
		errorOut("client", "ER:OPEND");
		send(sfd , "ER:OPEND" , strlen("ER:OPEND") , 0 ); 
		return;
	}

	curUser = (user*)malloc(sizeof(user));
	curUser->openBox = box;
	curUser->bOpen = 1;
	box->inUse = 1;
	send(sfd , "OK!" , strlen("OK!") , 0 );

	eventOut("client", "OPNBX");

	return;
}

//Gets next messag ein queue then deletes it
void NXTMG(char* arg, int sfd)
{
	//If the current user has no box open
	if(curUser == NULL || curUser->bOpen != 1)
	{
		errorOut("client", "ER:NOOPN");
		send(sfd , "ER:NOOPN" , strlen("ER:NOOPN") , 0 );
		return;
	}
	
	mBox* cur = curUser->openBox;

	//if the current user has an empty box
	if(cur->messages == NULL)
	{
		errorOut("client", "ER:EMPTY");
		send(sfd , "ER:EMPTY" , strlen("ER:EMPTY" ) , 0 );
		return;
	}

	
	queue* temp = cur->messages; 
  	
	if(temp->next == NULL)
	{
		
		send(sfd , temp->message, strlen(temp->message) , 0 );
		
		
    		free(temp); 
		cur->messages = NULL;

		eventOut("client", "NXTMG");

		return;
	}

	queue* prev;

    	while (temp->next != NULL) 
    	{ 
        	prev = temp;
        	temp = temp->next;
    	} 
  
    	prev->next = NULL; 
  
	send(sfd , temp->message, strlen(temp->message) , 0 );

    	free(temp); 

	eventOut("client", "NXTMG");

	return;
}

//Put message in open box
void PUTMG(char* arg, int sfd)
{	
	//if the user has no box open
	if(curUser == NULL || curUser->bOpen != 1)
	{
		errorOut("client", "ER:NOOPN");
		send(sfd , "ER:NOOPN" , strlen("ER:NOOPN") , 0 );
		return;
	}
	
	char sizeC[10];
	int j = 0;
	int i = 0;
	while(arg[i] != '!')
	{	
		sizeC[i] = arg[i];
		i++;
	}
	sizeC[i] = '\0';
	int size = atoi(sizeC);
	char* mess = malloc(size * sizeof(char) + 1);
	for(j = 0;j < size; j++)
	{
		mess[j] = arg[i];
		i++;
	}
	mess[size] = '\0';
	mBox* cur = curUser->openBox;

	char reply[4 + strlen(sizeC) + 2];
	snprintf(reply, 4 + strlen(sizeC) + 1, "%s%s", "OK!", sizeC);
	
	if(cur->messages == NULL)
	{
		cur->messages = (queue*)malloc(sizeof(queue));
		cur->messages->message = malloc( sizeof(char) * strlen(mess) * strlen(reply) + 1);
		snprintf(cur->messages->message, strlen(mess) + strlen(reply) + 1, "%s%s", reply, mess);

		cur->messages->next = NULL;
		cur->messages->origMessage = NULL;
		send(sfd , reply , strlen(reply) , 0 );

		return;
	}
	
	queue* temp = (queue*)malloc(sizeof(queue));
	temp->message = malloc( sizeof(char) * strlen(mess) * strlen(reply) + 1);
	snprintf(temp->message, strlen(mess) + strlen(reply) + 1, "%s%s", reply, mess);
	temp->next = cur->messages;
	cur->messages = temp;
	
	send(sfd , reply , strlen(reply) , 0 );

	eventOut("client", "PUTMG");
	
	return;
}

//Delets named box
void DELBX(char* arg, int sfd)
{
	//sem_wait(&db_lock);

	if(strlen(arg) > 25 || strlen(arg) < 5 || !isalpha(arg[0]) )
	{
		send(sfd , "WHAT?" , strlen("WHAT?") , 0 );
		return;
	}

	int exist = exists(arg);
	if(!exist)
	{
		errorOut("client", "ER:NEXIST");
		send(sfd , "ER:NEXIST" , strlen( "ER:NEXIST" ), 0 );
		return;
	}
	
	mBox* box = getBox(arg);
	if(box->inUse)
	{
		errorOut("client", "ER:OPEND");
		send(sfd , "ER:OPEND" , strlen("ER:OPEND") , 0 ); 
		return;
	}
	//The box needs to be empty to delete
	if(box->messages != NULL)
	{
		errorOut("client", "ER:NOTMT");
		send(sfd , "ER:NOTMT" , strlen( "ER:NOTMT" ) , 0 );
		return;
	}

	mBox* ptr = head;
	
	if (head->next == NULL && strcmp(head->name, arg))
    	{  
		head = head->next;
		free(ptr);
		send(sfd , "OK!" , 3 , 0 );
		eventOut("client", "DELBX");              
        	return; 
    	} 

	if (head != NULL && strcmp(head->name, arg))
    	{ 
        	head = head->next;   
        	free(ptr);
		send(sfd , "OK!" , 3 , 0 );
		eventOut("client", "DELBX");              
        	return; 
    	} 

	mBox* prev;
	while(ptr->next != NULL){
		prev = ptr;
		ptr = ptr->next;
		if((strcmp(ptr->name, arg)) == 0)
			break;
	}
	
    	prev->next = ptr->next; 
  
    	free(ptr); 

	send(sfd , "OK!" , 3 , 0 );

	eventOut("client", "DELBX");

	//sem_post(&db_lock);

	return;
}

//Closes specified box
void CLSBX(char* arg, int sfd)
{
	if(strlen(arg) > 25 || strlen(arg) < 5 || !isalpha(arg[0]) )
	{
		errorOut("client", "ER:WHAT?");
		send(sfd , "ER:WHAT?" , strlen("ER:WHAT?") , 0 );
		return;
	}

	int exist = exists(arg);
	mBox* box = getBox(arg);
	if(!exist || box->inUse == 0)
	{
		errorOut("client", "ER:NOOPN");
		send(sfd , "ER:NOOPN" , strlen("ER:NOOPN") , 0 );
		return;
	}

	box->inUse = 0;
	curUser->bOpen = 0;
	curUser->openBox = NULL;
	send(sfd , "OK!" , strlen("OK!") , 0 );

	eventOut("client", "CLSBX");	

	return;
}

//Listener function for thread connections
void listener(void* server_fd){
	sfdPill* sfdP = (sfdPill*)server_fd;
	int sfd = sfdP->sfd;	
	int msgLength = 0;
	char buffer[1024] = {0};

	int hello = 0;
	while(*killFlag == 0)
	{
		msgLength = recv(sfd, buffer, 1024, 0);
		buffer[msgLength] = '\0';
		char* error = "To start service say [HELLO]. Closing connection.";
		
		if((strcmp(buffer, "HELLO")) == 0){
			hello = 1;
			HELLO(buffer, sfd);
			
		} else if((strcmp(buffer, "GDBYE")) == 0){
			if(hello != 1)
			{	
				send(sfd , error, strlen(error) , 0 );
			}
			GDBYE(buffer, sfd);
			close( sfd );
			break;
		} else if((strcmp(buffer, "CREAT")) == 0){
			if(hello != 1)
			{	
				send(sfd , error, strlen(error) , 0 );
			}
			msgLength = recv(sfd, buffer, 1024, 0);
			buffer[msgLength] = '\0';
			CREAT(buffer, sfd);
		} else if((strcmp(buffer, "OPNBX")) == 0){
			if(hello != 1)
			{	
				send(sfd , error, strlen(error) , 0 );
			}
			msgLength = recv(sfd, buffer, 1024, 0);
			OPNBX(buffer, sfd);
		} else if((strcmp(buffer, "NXTMG")) == 0){
			if(hello != 1)
			{	
				send(sfd , error, strlen(error) , 0 );
			}
			NXTMG(buffer, sfd);
		} else if((strcmp(buffer, "PUTMG")) == 0){
			if(hello != 1)
			{	
				send(sfd , error, strlen(error) , 0 );
			}
			msgLength = recv(sfd, buffer, 1024, 0);
			buffer[msgLength] = '\0';
			PUTMG(buffer, sfd);	
		} else if((strcmp(buffer, "DELBX")) == 0){
			if(hello != 1)
			{	
				send(sfd , error, strlen(error) , 0 );
			}
			msgLength = recv(sfd, buffer, 1024, 0);
			buffer[msgLength] = '\0';
			DELBX(buffer, sfd);
		} else if((strcmp(buffer, "CLSBX")) == 0){
			if(hello != 1)
			{	
				send(sfd , error, strlen(error) , 0 );
			}
			msgLength = recv(sfd, buffer, 1024, 0);
			buffer[msgLength] = '\0';
			CLSBX(buffer, sfd);
		}
	}
	return;
}

//acceptor function for threads connections
void acceptor(void* args)
{
	//create and init ctrl-c handler
	struct sigaction killer;
	memset(&killer, 0, sizeof(killer));
	killer.sa_handler = &handle_sigINT;
	sigaction(SIGINT, &killer, NULL);
	acceptParams* acceptArgs = (acceptParams*)args;

	int new_client = 0;
	while(*killFlag == 0){
		if((new_client = accept(acceptArgs->sfd, acceptArgs->addr, acceptArgs->slen)) < 0){
			perror("Accept");
			exit(EXIT_FAILURE);
		}
		sockets[*connections-1] = new_client;
		
		struct sockaddr_in* pV4Addr = (struct sockaddr_in*)acceptArgs->addr;
		struct in_addr ipAddr = pV4Addr->sin_addr;
		char str[INET_ADDRSTRLEN];
		inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );

		time_t now;
		time(&now);
		printf("%s connected %s", str, ctime(&now));

		//init the args for listener
		sfdPill* sfdP = (sfdPill*)malloc(sizeof(sfdPill));
		sfdP->sfd = new_client;
	
		//set up communication between server and accepted client
		int pthread_err = 0;
		if((pthread_err = pthread_create(&tids[*connections], NULL, (void*)listener, (void*)sfdP)) != 0){
			perror("Listener");
			exit(EXIT_FAILURE);
		}
		*connections = (*connections)+1;
	}
	return;
}

int main( int argc, char** argv )
{
	//Arguement formats
	if(argc != 2){
		printf("Error: Enter Only The Port Number!\n");
		return -1;
	}

	if(atoi(argv[1]) <= 4000)
	{
		printf("Error: Port Number Has to be Strictly Greater than 4000.\n");
		return -1;
	}

	connections = (int*)malloc(sizeof(int));
	*connections = 0;
	killFlag = (int*)malloc(sizeof(int));
	*killFlag = 0;

	sem_init(&db_lock, 0, 1);
	int i = 0;
	for(i = 0; i < 255; i++){
		sockets[i] = -2;
	}

	int server_fd;
	struct sockaddr_in address; 
 	int opt = 1; 
    	int addrlen = sizeof(address); 
    	char buffer[1024] = {0}; 
	
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("Socket failed");
		exit(EXIT_FAILURE);
	}
	
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    	{ 
       		perror("setsockopt"); 
        	exit(EXIT_FAILURE); 
    	} 
    	address.sin_family = AF_INET; 
    	address.sin_addr.s_addr = INADDR_ANY; 
    	address.sin_port = htons( atoi(argv[1]) ); 

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
    	{ 
        	perror("bind failed"); 
        	exit(EXIT_FAILURE); 
    	} 

    	if (listen(server_fd, 5) < 0) 
    	{ 
        	perror("Listen"); 
        	exit(EXIT_FAILURE); 
    	} 

	//spawn acceptor thread
	acceptParams* acceptArgs = (acceptParams*)malloc(sizeof(acceptParams));
	acceptArgs->sfd = server_fd;
	acceptArgs->addr = (struct sockaddr*)&address;
	acceptArgs->slen = (socklen_t*)&addrlen;
	pthread_t connector_id = NULL;
	
	int new_accept = 0;
	if((new_accept = pthread_create(&tids[*connections], NULL, (void*)acceptor, (void*) acceptArgs)) != 0){
		perror("Acceptor thread creation");
		exit(EXIT_FAILURE);
	} else{
		*connections = (*connections)+1;
	}
	
	while(*killFlag == 0);

	//close all connections
	char* msg = "-1";
	for(i = 0; i < 255; i++){
		if(sockets[i] == -2){
			break;		
		}
		send(sockets[i], msg, strlen(msg), 0);
		close(sockets[i]);
	}
	//join all threads
	int j = 0;
	for(j = 0; j < *connections; j++){
		pthread_detach(tids[j]);
	}

	return 0;
}
