#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct node
{
	char* data;
	struct node* next;
}node;

node* newNode( char* exp, node* head )//function to create a new node at the head of my linked list
{
	struct node* newNode = malloc(sizeof(node));
	newNode->data = exp;
	newNode->next = head;
	return newNode;
}

static void reverse( struct node** head )//function to reverse the order of the linked list
{
	struct node* prev = NULL;
	struct node* current = *head;
	struct node* next = NULL;
	while( current != NULL )
	{
		next = current->next;
		current->next = prev;
		prev = current;
		current = next;
	}
	*head = prev;
}

int stringLength( char* s )//function to get the length of a string(char array)
{
	int i = 0;
	while( s[i] != '\0' )
	{
		i++;	
	}
	return i;
}

char* subString( char* s, int a, int b )//function to get the sub string of a given string from a up to, but not including b
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

int checkArithmeticOp( char* s )//function to see if the token is an arithmetic operator
{
	int i = 0;
	int num = 0;
	while( s[i] != '\0' )
	{
		if( (s[i] ==  '-' || s[i] == '+' || s[i] == '*' || s[i] == '/') && stringLength( s ) == 1 )
		{
			return 1;//return 1 to state there this token is an arithmetic operator	
		}
		i++;
	}
	return 0;
}

int checkLogicalOp( char* s )//function to see if the token is a logical operator
{
	int i = 0;
	int num = 0;
	while( s[i] != '\0' )
	{
		if( ( s[i] == 'A' && s[i+1] == 'N' && s[i+2] == 'D' && stringLength( s ) == 3 ) || ( s[i] == 'O' && s[i+1] == 'R' && stringLength( s ) == 2 ) )
		{
			return 1;//return 1 to state that the token is a logical operator, more specifically AND or OR
		}
		if( s[i] == 'N' && s[i+1] == 'O' && s[i+2] == 'T' && stringLength( s ) == 3 )
		{
			return 2;//special case for the NOT operator since it is the only unary operator
		}
		i++;
	}
	return 0;
}

int checkArithmeticOperand( char* s )//function to see if the token is an arithmetic operand
{
	int i = 0;
	int num = 0;
	while( s[i] != '\0' )
	{
		if( (s[i] == '0' || s[i] == '1' || s[i] == '2' || s[i] == '3' || s[i] == '4' || s[i] == '5' || s[i] == '6' || s[i] == '7' || s[i] == '8' || s[i] == '9') && stringLength( s ) == 1 )
		{
			return 1;//return 1 if the token is 0-9
		}
		i++;
	}
	return 0;
}

int checkLogicalOperand( char* s )//function to see if the token is a logical operand
{
	int i = 0;
	int num = 0;
	while( s[i] != '\0' )
	{
		if( (s[i] == 't' && s[i+1] == 'r' && s[i+2] == 'u' && s[i+3] == 'e' && stringLength( s ) == 4 ) || (s[i] == 'f' && s[i+1] == 'a' && s[i+2] == 'l' && s[i+3] == 's' && s[i+4] == 'e' && stringLength( s ) == 5 ) )
		{
			return 1;//return 1 if it is true or false
		}
		i++;
	}
	return 0;
}

void freeLL( node* head )//function to free the linked lists I have created
{
	struct node* ptr = head;
	while( ptr != NULL )
	{
		free( ptr );
		ptr = ptr->next;
	}
}

int main( int argc, char **argv )
{

	int expressions = 1;//total expressions
	int arithmetic = 0;//arithmetic expressions
	int logical = 0;//logical expressions

	struct node* head = NULL;
	struct node* expHead = NULL;
	
	if( argc < 2 || argc > 2 )//checks to see if the arguments is more or less than 2( 1 for executable and 1 for the entire user inputted argument)
	{
		printf( "Error: Invalid amount of arguments\n" );
		return 0;	
	}
	else
	{
		int i = 0;//marker variable
		int j = 0;//marker variable
		while( argv[1][i] != '\0' )//this is a while loop to create a linked list where the nodes contain one expression each ending before the semicolon
		{
			if( argv[1][i] == ';' || argv[1][i+1] == '\0'  )
			{
				char* s;
				if( argv[1][i+1] == '\0' )//this is to check if we are at the last expression of the argument
				{
					if( argv[1][i] == ';' )//if we are at the last expression and there is a semicolon then the following expression is missing
					{
						printf( "Error in expression %d, missing expression\n", expressions );
						s = subString( argv[1], j, i );
					}
					else
					{
						s = subString( argv[1], j, i + 1 );
					}
				}
				else
				{
					if( argv[1][i+1] || i == 0 )
					{
						printf( "Error in expression %d, missing expression\n", expressions );
					}
					s = subString( argv[1], j, i );
				}
				j = i + 2;//this is to skip whitespace assuming proper format which I know is not the best idea. If no space then the next token will be unknown identifier
				head = newNode( s, head );
				if( argv[1][i] == ';' )
				{
					expressions++;
				}
			}
			i++;
		}
	}
	struct node* ptr = head;
	reverse( &ptr );
	int expNum = 0;//expression number, which expression are we on
	while( ptr != NULL )//a while loop to loop through the expression linked list
	{
		int i = 0;//marker variable
		int j = 0;//marker variable
		int isArithmeticOp = 0;//a variable to hold wheter or not it is a arithmeric operation(expression)
		int isLogicalOp = 0;//same thing, but for logical
		char prevToken = ' ';//a variable to hold what the previous token was: a = arithmetic operand, b = arithmetic operator, c = logical operand, d = logical operator, u = unknown
		int totalArithmeticOperands = 0;
		int totalArithmeticOperators = 0;
		int totalLogicalOperands = 0;
		int totalLogicalOperators = 0;
		int index = 0;
		int tokens = 0;//total tokens in the expression
		int isOperatorError = 0;//is there an operator error present already
		int isOperandError = 0;//is there an operand error present already
		expHead = NULL;
		while( ptr->data[i] != '\0' )//this is a while loop to create a linked list where the nodes contain the tokens of the expressions
		{
			if( ptr->data[i] == ' ' || ptr->data[i+1] == '\0' )
			{
				char* t;
				if( ptr->data[i+1] == '\0' )
				{
					t = subString( ptr->data, j, i + 1 );
				}
				else
				{
					t = subString( ptr->data, j, i );
				}
				j = i + 1;
				expHead = newNode( t, expHead );
			}
			i++;
		}
		struct node* expPtr = expHead;
		reverse( &expPtr );
		while( expPtr != NULL )//this is a while loop to loop through the token linked list while we are still inside the expression linked list
		{
			while( expPtr->data[0] == ' ' )
			{
				expPtr = expPtr->next;//here I basically skip over all 'tokens' that are spaces since they are delimeters
			}
			if( checkArithmeticOp( expPtr->data ) == 0 && checkLogicalOp(expPtr->data ) == 0 && checkArithmeticOperand( expPtr->data ) == 0 && checkLogicalOperand( expPtr->data ) == 0 )//if the token is unknown
			{
				if( prevToken == ' ' )//all of these are how to handle an unknown token given what the previous token is
				{	
					printf( "Error in expression %d, unknown identifier: \"%s\"\n", expNum, expPtr->data );
				}
				if( prevToken == 'a' || prevToken == 'c' )
				{
					printf( "Error in expression %d, unknown operator: \"%s\"\n", expNum, expPtr->data );
					isOperatorError = 1;
				}
				if( prevToken == 'b' || prevToken == 'd' )
				{
					printf( "Error in expression %d, unknown operand: \"%s\"\n", expNum, expPtr->data );
				}
				prevToken = 'u';
			}

			if( isArithmeticOp == 0 && isLogicalOp == 0 && checkArithmeticOp( expPtr->data ) == 1 )
			{
				isArithmeticOp = 1;//this sets the operation(expression) to an arithmetic one
			}
			if( isArithmeticOp == 0 && isLogicalOp == 0 && ( checkLogicalOp( expPtr->data ) == 1 || checkLogicalOp( expPtr->data ) == 2 ) )
			{
				isLogicalOp = 1;//this sets the operation(expression) to a logical one
			}
			if( checkArithmeticOperand( expPtr->data ) == 1 )//is the token a number 0-9
			{
				if( prevToken == 'a' )//this handles an arithmetic operand given what the previous token is
				{
					if( tokens < 3 && isOperatorError == 0 )
					{
						printf( "Error in expression %d, unexpected operator: \"%s\"\n", expNum, expPtr->data );
					}
					else
					{
						printf( "Error in expression %d, unexpected operand: \"%s\"\n", expNum, expPtr->data );
					}
				}
				if( prevToken == 'c' )
				{
					printf( "Error in expression %d, unexpected operator: \"%s\"\n", expNum, expPtr->data );
				}
				if( prevToken == 'd' && isOperandError == 0 )
				{
					printf( "Error in expression %d, unexpected operand: \"%s\"\n", expNum, expPtr->data );
				}
				if( prevToken == 'd' && totalArithmeticOperands == 1 )
				{
					printf( "Error in expression %d, operator type mismatch\n", expNum );
				}
				totalArithmeticOperands++;
				prevToken = 'a';//not done here i think there is more to put
			}
			if( checkLogicalOperand( expPtr->data ) == 1 )//is the token true, false
			{
				if( prevToken == 'a' )//this handles a logical operand given what the previous token is
				{
					printf( "Error in expression %d, unexpected operator: \"%s\"\n", expNum, expPtr->data );
				}
				if( prevToken == 'b' )
				{
					printf( "Error in expression %d, unexpected operand: \"%s\"\n", expNum, expPtr->data );
				}
				if( prevToken == 'c' )
				{
					printf( "Error in expression %d, unexpected operator: \"%s\"\n", expNum, expPtr->data );
				}
				totalLogicalOperands++; 
				prevToken = 'c';
			}
			if( checkArithmeticOp( expPtr->data ) == 1 )//is the token + - * /
			{
				if( prevToken == ' ' )//this handles an arithmetic operator given what the previous token is 
				{
					printf( "Error in expression %d, unexpected operator: \"%s\"\n", expNum, expPtr->data );
				}
				if( prevToken == 'b' )
				{
					printf( "Error in expression %d, unexpected operand: \"%s\"\n", expNum, expPtr->data );
				}
				if( prevToken == 'c' )
				{
					printf( "Error in epxression %d, operand type mismatch\n", expNum );
				}
				if( tokens >= 3 || prevToken == 'u' )
				{
					printf( "Error in expression %d, unexpected operator: \"%s\"\n", expNum, expPtr->data );
				}
				totalArithmeticOperators++;
				prevToken = 'b';
			}
			if( checkLogicalOp( expPtr->data ) == 1 || checkLogicalOp( expPtr->data ) == 2 )// is the token AND, OR, NOT
			{
				if( prevToken == ' ' && checkLogicalOp( expPtr->data ) == 1 )//this handles a logical operator given what the previous token is
				{
					printf( "Error in expression %d, unexpected operand: \"%s\"\n", expNum, expPtr->data );
				}
				if( prevToken == 'a' )
				{
					printf( "Error in expression %d, operand type mismatch\n", expNum );
					isOperandError = 1;
				}
				if( prevToken == 'b' )
				{
					printf( "Error in epxression %d, unexpected operand: \"%s\"\n", expNum, expPtr->data );
				}
				if( prevToken == 'd' )
				{
					printf( "Error in expression %d, unexpected operand: \"%s\"\n", expNum, expPtr->data );
				}
				if( checkLogicalOp( expPtr->data ) == 2 )
				{
					totalLogicalOperands++;//so a NOT basically can count for an additional logical operand since it only needs one, it is unary
					tokens++;//or an extra token basically
				}
				totalLogicalOperators++;
				prevToken = 'd';
			}
			tokens++;
			index++;
			expPtr = expPtr->next;
		}

		if( totalLogicalOperators == 0 && totalArithmeticOperators == 0 && ( totalLogicalOperands >= 1 || totalArithmeticOperands >= 1 ) && isOperatorError == 0  )//special case where format is ok up until there is no operator
		{
			printf( "Error in expression %d, missing operator\n", expNum );
		}
		if( totalLogicalOperators == 0 && totalLogicalOperands == 0 && totalArithmeticOperands == 1 && totalArithmeticOperators == 1 && tokens < 3 && prevToken == 'b' )//special case where format is ok until there is no operand
		{
			printf( "Error in expression %d, missing operand\n", expNum );
		}
		if( totalLogicalOperators == 0 && totalArithmeticOperators == 0 && totalLogicalOperands == 0 && totalArithmeticOperands == 0 )//when there is nothing, but not empty string
		{
			printf( "Error in expression %d, incomplete expression\n", expNum );
		}
		if( tokens > 3 )//when the expression goes on too long
		{
			printf( "Error in expression %d, expression not ended\n", expNum );
		}
		arithmetic = arithmetic + isArithmeticOp;//increases total arithmetic operations
		logical = logical + isLogicalOp;//increases total logical operations
		expNum++;
		ptr = ptr->next;
	}

	if( stringLength( argv[1] ) == 0 )//special case to take care of the empty string
	{
		printf( "Error in expression %d, missing expression\n", expNum );
	}

	printf( "Found %d expressions: %d logical and %d arithmetic\n", expressions, logical, arithmetic );//print total expression, logical expressions, arithmetic expressions
	freeLL( head );//free the expression linked list nodes
	freeLL( expHead );//free the token linked list nodes

	return 0;
}
