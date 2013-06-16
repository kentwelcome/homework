#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"

//variable
FILE    *input_file , *output_file;
int     COMMENT_MARK = FAULT;
char	*Reserved_word[] = RESERVED_WORDS;


void usageDisplay(char *projectName)
{
    printf("Usage: %s [Source File]\n",projectName);
}

//Main function
int main( int argc , char *argv[])
{

	char    *read_word;
	char    *Ptr;
	char	id_tmp[32];
	int     i = 1;
	int     line = 1;


	if ( argc != 2 )
		input_file = stdin;
	else
	{
		if ( !(input_file=fopen(argv[1],"r")) )
		{
			fprintf(stderr,"input file error!\n");
			exit(-1);
		}
	}

	output_file = fopen("out.txt","w");
	read_word = malloc(sizeof(char)*256);


	while ( fgets(read_word,256,input_file) ) // read from stdin or input file by line
	{

		if(read_word[0] == '\n')            // change \n to \0
			read_word[0] = '\0';
		strtok(read_word,"\n"); 

		//Find_Comments(&read_word);
		//Find_Comments2(&read_word);

		fprintf(output_file,"%03d: %s\n",line++,read_word);
		printf("%03d: %s\n",line++,read_word);
		Ptr = read_word;

		// start scann new line
		while( *Ptr != '\0' && *Ptr != '#' )
		{

			if (isspace(*Ptr) || COMMENT_MARK == TRUE)	// get space
			{
				if(*Ptr == '*' && *(Ptr+1) == '/')
				{
					Ptr+=2;
					COMMENT_MARK = FAULT;
					continue;
				}

				Ptr++;
				continue;
			}
			else if (isalpha(*Ptr)) // check ID
			{
				//Ptr++
				for( i = 0 ; isalnum(*(Ptr)) || *(Ptr) == '_' ; Ptr++ , i++ )
					id_tmp[i] = *Ptr;

				id_tmp[i] = '\0';

				if( Check_Reserved_Word(id_tmp) )
				{
					fprintf(output_file,"Match keyword: %s\n",strupr(id_tmp) );
					printf("Match keyword: %s\n",strupr(id_tmp) );
				}
				else
					fprintf(output_file,"Match ID: %s\n",id_tmp);
				printf("Match ID: %s\n",id_tmp);

				continue;

			}
			else if (isdigit(*Ptr)) // check INT_NUM || FLOAT_NUM
			{

				for( i = 0 ; isdigit(*Ptr)  ; Ptr++ , i++ )
					id_tmp[i] = *Ptr;

				if( *Ptr == '.' ) // check FLOAT
				{
					Ptr++;
					id_tmp[i] = '.';
					for( i++ ; isdigit(*Ptr) ; i++ , Ptr++ )
						id_tmp[i] = *Ptr;
					id_tmp[i] = '\0';

					fprintf(output_file,"Match FLOAT_NUM: %s\n",id_tmp);
					printf("Match FLOAT_NUM: %s\n",id_tmp);
				}
				else
				{
					id_tmp[i] = '\0';
					fprintf(output_file,"Match INT_NUM: %d\n",atoi(id_tmp));
					printf("Match INT_NUM: %d\n",atoi(id_tmp));
				}
				continue;
			}
			else if ( *Ptr == '&' )		// looking for && AND_OP
			{
				if( *(Ptr+1) == '&' )
				{
					Ptr+=2;
					fprintf(output_file,"Match symbol: AND_OP\n");    
					printf("Match symbol: AND_OP\n");    
				}
				else
				{
					fprintf(output_file,"Match symbol: %c\n",*Ptr);
					printf("Match symbol: %c\n",*Ptr);
					Ptr++;
				}
				continue;
			} 
			else if ( *Ptr == '|' ) 	// looking for || OR_OP
			{
				if( *(Ptr+1) == '|' )
				{
					Ptr+=2;
					fprintf(output_file,"Match symbol: OR_OP\n");
					printf("Match symbol: OR_OP\n");
				}
				else
				{
					fprintf(output_file,"Match symbol: %c\n",*Ptr);
					printf("Match symbol: %c\n",*Ptr);
					Ptr++;
				}
				continue;
			}
			else if ( *Ptr == '<' )		// looking for <= LE_OP
			{
				if( *(Ptr+1) == '=' )
				{
					Ptr+=2;
					fprintf(output_file,"Match symbol: LE_OP\n");    
					printf("Match symbol: LE_OP\n");    
				}
				else
				{
					Ptr++;
					fprintf(output_file,"Match symbol: <\n");
					printf("Match symbol: <\n");
				}
				continue;
			} 
			else if ( *Ptr == '>' ) 	// looking for >= GE_OP
			{
				if( *(Ptr+1) == '=' )
				{
					Ptr+=2;
					fprintf(output_file,"Match symbol: GE_OP\n");
					printf("Match symbol: GE_OP\n");
				}
				else
				{
					Ptr++;
					fprintf(output_file,"Match symbol: >\n");
					printf("Match symbol: >\n");
				}
				continue;
			}
			else if ( *Ptr == '=' ) 	// looking for == EQ_OP
			{
				if( *(Ptr+1) == '=' )
				{
					Ptr+=2;
					fprintf(output_file,"Match symbol: EQ_OP\n");
					printf("Match symbol: EQ_OP\n");
				}
				else
				{
					Ptr++;
					fprintf(output_file,"Match symbol: =\n");
					printf("Match symbol: =\n");
				}
				continue;
			}
			else if ( *Ptr == '!' ) 	// looking for != NE_OP
			{
				if( *(Ptr+1) == '=' )
				{
					Ptr+=2;
					fprintf(output_file,"Match symbol: NE_OP\n");
					printf("Match symbol: NE_OP\n");
				}
				else
				{
					fprintf(output_file,"Match symbol: !\n");
					printf("Match symbol: !\n");
					Ptr++;
				}
				continue;
			}
			else if ( *Ptr == '/' )		// looking for /* ... */ or //
			{
				if( *(Ptr+1) == '*' )
				{
					Ptr++;
					COMMENT_MARK = TRUE;
					continue;
				}
				else if( *(Ptr+1) == '/' )
					break;
				else
				{
					fprintf(output_file,"Match symbol: /\n");
					printf("Match symbol: /\n");
					Ptr++;    
				}

			}
			else if ( *Ptr == '"' )		// looking for "..." 
			{
				Ptr++;
				fprintf(output_file,"Match string: ");
				printf("Match string: ");
				while( *Ptr != '"' )
				{
					fprintf(output_file,"%c",*Ptr);
					printf("%c",*Ptr);
					Ptr++;
				}
				fprintf(output_file,"\n");
				printf("\n");
				Ptr++;
				continue;
			}
			else if( *Ptr == '{' || *Ptr == '}' || *Ptr == '(' || *Ptr == ')' || *Ptr == ';' || *Ptr == ',' || *Ptr == '.' || *Ptr == '*' || *Ptr == '+' || *Ptr == '-' ) // other symbol
			{
				fprintf(output_file,"Match symbol: %c\n",*Ptr);
				printf("Match symbol: %c\n",*Ptr);
			}
			else
			{
				printf("Unknow: %c\n",*Ptr);
				fprintf(output_file,"Unknow: %c\n",*Ptr);
			}


			Ptr++;
		}
		fprintf(output_file,"\n");
		printf("\n");

	}

	free(read_word);
	fclose(input_file);
	fclose(output_file);
	return 0;
}

int Find_Comments(char **string )
{
	char *Ptr;
	// get /*
	Ptr = strstr( *string , "/*" );
	if( Ptr != NULL )
	{
		*Ptr = '\0';
		Ptr+=2;
		Ptr = strstr( Ptr , "*" );
		if(Ptr != NULL)
		{
			if(Ptr[1] == '/')
				*string = Ptr+2;
			return       FAULT;
		}
		COMMENT_MARK = TRUE;
		return       TRUE;
	}
	//  get */
	Ptr = strstr( *string , "*" );
	if(Ptr != NULL)
	{
		if(Ptr[1] == '/')
			*string = Ptr+2;
		COMMENT_MARK = FAULT;
		return       FAULT;
	}



	return FAULT;
}

int Find_Comments2(char **string )
{
	char *Ptr;
	//  get //
	Ptr = strstr( *string , "//" );
	if(Ptr != NULL)
	{
		*Ptr = '\0';
		return     TRUE;   
	}
	return FAULT; 
}

int Check_Reserved_Word(char *string )
{
	int i;
	for( i = 0 ; i < 9 ; i++ )
	{
		if( strcmp(Reserved_word[i],string) == 0 )
			return TRUE;
	}
	return FAULT;
}

// chenk a to A
char *strupr(char *s)
{
	char *t = s;
	while (*s)
	{
		*s = toupper(*s);
		s++;
	}
	return t;
}


