//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TForm2::Button1Click(TObject *Sender)
{
GRAMMAR *G_ptr;

	TOKEN *T_ptr , *T_tmp;

	char grammar_rule[1024] , file_name[1024];
	char *ptr , *ptr_string;
	char *ptr_lhs , *ptr_rhs;
	SET *S_ptr;
	int i , j;
	int rule_num;

	//lambda[0] = '£f';
	strncpy( lambda , "£f" , strlen("£f")+1 );
	G_start = NULL;
	G_end = NULL;
	terminal_start = NULL;
	terminal_end = NULL;
	nonterminal_start = NULL;
	nonterminal_end = NULL;
	first_set_start = NULL;
	first_set_end = NULL;
	follow_set_start = NULL;
	follow_set_end = NULL;
	predict_set_start = NULL;
	predict_set_end = NULL;
	stack_top = NULL;


	strncpy( file_name , (OpenDialog1->FileName).c_str() , (OpenDialog1->FileName).Length()+1 );
	input = fopen(file_name,"r");

	if ( input == NULL ) {
		Application->MessageBoxA("grammar file error","error",0);
		Label1->Caption = OpenDialog1->FileName;
		return;
	}

	strncpy( file_name , OpenDialog2->FileName.c_str() , OpenDialog2->FileName.Length()+1 );
	scan = fopen(file_name , "r");

	if ( scan == NULL ) {
		Application->MessageBoxA("scan file error","error",0);
		Label1->Caption = OpenDialog1->FileName;
		return;
	}

    out = fopen("table.csv","w");
    //gen_code = fopen("parser.c","w");
     
    for ( i = 0 ; fgets( grammar_rule , 1024 , input ) ;  )
    {
		ptr = grammar_rule;
        // remove \n
        ptr = strstr(grammar_rule , "\n");
        *ptr = '\0';
        // remove rule num
        ptr = strstr( grammar_rule , "." );
        // get left handside
        ptr_lhs = ptr + 2;
		ptr = strstr( grammar_rule , " " );
        *ptr = '\0';
        insert_token( &nonterminal_start , &nonterminal_end , ptr_lhs );    // make nonterminal table
        insert_token( &first_set_start , &first_set_end , ptr_lhs );        // make first set table with nonterminal 
        insert_token( &follow_set_start , &follow_set_end , ptr_lhs );      // make follow set table
        insert_token( &predict_set_start , &predict_set_end , ptr_lhs );    // make predict set table
		// get right handside
		ptr = strstr( ++ptr , "¡÷" );
        do
        {
            ptr = strstr( ++ptr , " " );
            while(isspace(*ptr))
                ptr++;
            ptr_rhs = ptr;
			ptr = strstr( ptr_rhs , "|" );
            if ( ptr != NULL &&  *(ptr+1) != '|' )
            {
                for ( j = 1 ; isspace(*(ptr-j)) ; j++ )
                *(ptr-j) = '\0';
            }
            
            insert_grammar( ++i , ptr_lhs , ptr_rhs );
            
        } while ( ptr != NULL &&  *(ptr+1) != '|' ); 
    }
    
    
    // remove nonterminal in termainal table
	T_ptr = nonterminal_start;
    i = 0;
    while ( T_ptr != NULL )
    {
        T_tmp = search_token( terminal_start , T_ptr->string );
        delete_token( &terminal_start , T_tmp );
        T_ptr = T_ptr->next;
        i++;
	}
	num_of_nonterm = i;
	// make first set table with terminal
	for ( T_ptr = terminal_start , i =0 ; T_ptr != NULL ; T_ptr = T_ptr->next , i++ )
	{
        insert_token( &first_set_start , &first_set_end , T_ptr->string );        // make first set table
	}
	num_of_term = ++i;



    fill_frist_set();
    fill_follow_set();
	fill_predict_set();

	for( i = 0 ; i < num_of_nonterm ; i++ )
	{
		//parser_table[i] = (PARSER*) malloc( sizeof(PARSER) * num_of_term );
		for( j = 0 ; j < num_of_term ; j++ )
		{
			parser_table[i][j].rule = 0;
			parser_table[i][j].special_tag = 0;
        }
    }

    output_perdict_table();
    
	for( i = 0 ; i < num_of_nonterm ; i++ )
	{
        for( j = 0 ; j < num_of_term ; j++ )
		{
			 fprintf(out,"%02d,",parser_table[i][j].rule);
		}
		fprintf(out,"\n");
	}
	fclose(out);
	// start gen
	lldiver(TreeView1,Label1);



	free_token(&terminal_start);
	free_token(&nonterminal_start);
	free_token(&first_set_start);
	free_token(&follow_set_start);
	free_grammar();
	TreeView1->FullExpand();
}
//---------------------------------------------------------------------------
int insert_grammar( int rule , char *lhs , char *rhs )
{
	GRAMMAR *ptr , *create;
	TOKEN *token_start , *token_end;
	char string[64] , *c_ptr;
	token_start = NULL;
	token_end = NULL;

	create = (GRAMMAR*) malloc( sizeof(GRAMMAR) );
    ptr = G_end;

	create->rule = rule;
	create->set = NULL;
	create->next = NULL;
	strncpy( create->lhs , lhs , strlen(lhs)+1 );
	strncpy( create->rhs , rhs , strlen(rhs)+1 );

    

	c_ptr = rhs;
    
    while ( c_ptr != NULL ){

		sscanf( c_ptr , "%s" , string );
		insert_token( &token_start , &token_end , string );

        if ( search_token( terminal_start , string ) == NULL && strcmp( string , "£f" ) != 0 ) // £f is not terminal
        {
			insert_token( &terminal_start , &terminal_end , string );    // make terminal table
        }
        
		c_ptr = strstr( c_ptr , " " );
        if ( c_ptr == NULL )
            break;
		c_ptr++;
    }
    
	create->rhs_string = token_start;


    if( G_start == NULL )
    {
        G_start = create;
		G_end = create;
    }
    else
    {
        ptr->next = create;
        G_end = create;
	}
}

int insert_token( TOKEN **start_point , TOKEN **add_point , char *string )
{
    TOKEN *ptr , *create;    



	create = (TOKEN *) malloc( sizeof(TOKEN) );
    
    ptr = *add_point;
	create->set = NULL;
    create->next = NULL;
    strncpy( (char*) create->string , string , strlen(string)+1 );
    
    if ( *start_point == NULL )
    {
		*start_point = create;
        *add_point = create;
    }
    else
    {
        ptr->next = create;
		*add_point = create;
    }
    
}

int delete_token( TOKEN **start_point , TOKEN *delete_point )
{
    TOKEN *ptr;
    ptr = *start_point;
    
    if( delete_point == NULL )
        return 0;

    if ( *start_point == delete_point )
    {
       *start_point = ptr->next;
       //ptr = delete_point->next;
       free(delete_point);
	   return 0;
    }
    
    while ( ptr->next != delete_point )
        ptr = ptr->next;
    
	ptr->next = delete_point->next;
    free(delete_point);
}

TOKEN *search_token( TOKEN *search_point , char *string )
{
	TOKEN *ptr;
    
    ptr = search_point;
    
    while ( ptr != NULL )
    {
		if( strcmp( string , ptr->string ) == 0 )
        {
            return ptr;
        }
        ptr = ptr->next;
    }
	return NULL;
}

int insert_set( TOKEN *symbel , char *term )
{
    SET *set_ptr , *create;

    for ( set_ptr = symbel->set ; set_ptr != NULL ; set_ptr = set_ptr->next )
    {
        if ( strcmp( term , set_ptr->string ) == 0 )   
            return FALSE;
    }

    set_ptr = symbel->set;
    
	create = (SET*) malloc(sizeof(SET));
    create->next = NULL;
    strncpy( create->string , term , strlen(term)+1 );

    if( set_ptr == NULL )
    {
        symbel->set = create;
    }
    else
	{
        create->next = symbel->set;
        symbel->set = create;
    }
    
    return TRUE;
}

void view_token( TOKEN *start_point )
{
    TOKEN *T_ptr;
    SET *S_ptr;
	T_ptr = start_point;
    while ( T_ptr != NULL )
    {
        printf("%s :",T_ptr->string);
        S_ptr = T_ptr->set;
        while(S_ptr != NULL)
		{
            printf(" %s",S_ptr->string );
            S_ptr = S_ptr->next;       
        }
        printf("\n");
        T_ptr = T_ptr->next;
	}
}

void fill_frist_set(void)
{
    TOKEN *term , *nonterm , *ptr , *ptr2 , *ptr3 , *ptr_end;
	GRAMMAR *gram , *G_ptr;
    term = terminal_start;
    nonterm = nonterminal_start;
    gram = G_start;
    int change;

	// nonterminal
    
    for ( G_ptr = G_start ; G_ptr != NULL ; G_ptr = G_ptr->next )
    {
        //printf("%s\n",gram->rhs);
        if ( strcmp( G_ptr->rhs_string->string , "£f" ) == 0 )
		{
            ptr = search_token( first_set_start , G_ptr->lhs );
            insert_set( ptr , "£f" ); // add £finto first set
        }
    }
    

    // terminal
    for ( term = terminal_start ; term != NULL ; term = term->next )
    {
        ptr = search_token( first_set_start , term->string );
        insert_set( ptr , term->string );
		for ( gram = G_start ; gram != NULL ; gram = gram->next )
        {
            if( strcmp( gram->rhs_string->string , term->string ) == 0 )
            {
                ptr = search_token( first_set_start , gram->lhs );
                insert_set( ptr , term->string ); // add terminal into first set                
			}
        }
    }

    // change
    do {
		change = FALSE;
        for ( gram = G_start ; gram != NULL ; gram = gram->next )
        {
            if ( comput_first( gram->rhs_string , first_set_start , gram->lhs ) == TRUE )
               change = TRUE;
        }

    } while (change);
     
}

int comput_first( TOKEN *alpha , TOKEN *dst , char *dst_name )
{
    int change = FALSE;
    TOKEN *ptr , *ptr2 , *ptr3 , *ptr_end;
    TOKEN *tmp;
    is_lambda = FALSE;
	tmp = (TOKEN*) malloc(sizeof(TOKEN));
	strncpy( tmp->string , "tmp" , 4 );
    tmp->set = NULL;
    tmp->next = NULL;
    
    if( alpha == NULL )
    {
		is_lambda = TRUE;
        return FALSE;
    }
    
    if ( strcmp( alpha->string , "£f" ) == 0 )
    {
		ptr = search_token( dst , dst_name );
        if ( insert_set( ptr , "£f" ) == TRUE )
            change = TRUE;
    }
    else
    {
		ptr = search_token( dst , dst_name );
        ptr2 = search_token( first_set_start , alpha->string );
        
        add_set( tmp , ptr2 );
		delete_set( &(tmp->set) , "£f" );
        
		if ( add_set( ptr , tmp ) == TRUE )
            change = TRUE;
        free_set( &(tmp->set) );
        
        for( ptr3 = alpha ; ptr3 != NULL  ; ptr3 = ptr3->next )
        {
			ptr2 = search_token( first_set_start , ptr3->string );
            if( search_set( ptr2 , "£f" ) != TRUE )
                break;

            if ( ptr3->next == NULL )
            {
				ptr_end = ptr3;
                ptr3 = search_token( first_set_start , ptr_end->string );
                if( search_set( ptr3 , "£f" ) == TRUE )
                {
                    is_lambda = TRUE;
                    if ( insert_set( ptr , "£f" ) == TRUE )
						change = TRUE;
                }
                break;
            }
                
            ptr2 = search_token( first_set_start , ptr3->next->string );
			add_set( tmp , ptr2 );
            delete_set( &(tmp->set) , "£f" );
            
            
            if ( add_set( ptr , tmp ) == TRUE )
            {
				delete_set( &(ptr->set) , "£f" );
                change = TRUE;
            }
            free_set( &(tmp->set) );
                
        }


    }
    
    free_token(&tmp);
    return change;
}

int comput_first_f( TOKEN *alpha , TOKEN *dst , char *dst_name )
{
    int change = FALSE;
    TOKEN *ptr , *ptr2 , *ptr3 , *ptr_end;
	TOKEN *tmp;
    is_lambda = FALSE;
	tmp = (TOKEN*) malloc(sizeof(TOKEN));
	strncpy( tmp->string , "tmp" , 4 );
    tmp->set = NULL;
    tmp->next = NULL;

    if( alpha == NULL )
    {
        is_lambda = TRUE;
        return FALSE;
    }

    if ( strcmp( alpha->string , "£f" ) == 0 )
    {
        //ptr = search_token( dst , dst_name );
        //if ( insert_set( ptr , "£f" ) == TRUE )
        is_lambda = TRUE;
		change = TRUE;
    }
    else
    {
        ptr = search_token( dst , dst_name );
        ptr2 = search_token( first_set_start , alpha->string );

        add_set( tmp , ptr2 );
        delete_set( &(tmp->set) , "£f" );
        if ( add_set( ptr , tmp ) == TRUE ) 
            change = TRUE;
        free_set( &(tmp->set) );

        for( ptr3 = alpha ; ptr3 != NULL  ; ptr3 = ptr3->next )
        {
            ptr2 = search_token( first_set_start , ptr3->string );
            if( search_set( ptr2 , "£f" ) != TRUE )
                break;

            if ( ptr3->next == NULL )
            {
                ptr_end = ptr3;
                ptr3 = search_token( first_set_start , ptr_end->string );
                if( search_set( ptr3 , "£f" ) == TRUE )
				{
                    if ( insert_set( ptr , "£f" ) == TRUE )
                    {
                        is_lambda = TRUE;
                        change = TRUE;
                    }
				}
                break;
            }

            ptr2 = search_token( first_set_start , ptr3->next->string );
            add_set( tmp , ptr2 );
			delete_set( &(tmp->set) , "£f" );
            if ( add_set( ptr , tmp ) == TRUE )
            {
                delete_set( &(ptr->set) , "£f" );
                change = TRUE;
            }
			free_set( &(tmp->set) );
            
        }

    }
    
	free_token(&tmp);
    return change;
}

void fill_follow_set(void)
{
	TOKEN *term , *nonterm , *ptr , *ptr2 , *ptr3 , *ptr_end;
    TOKEN *test;
    GRAMMAR *gram , *G_ptr;
    int change;
    int lambda;
    
	test = (TOKEN*) malloc(sizeof(TOKEN));

    // set start symbol 
    ptr = follow_set_start;
    insert_set( ptr , "£f" );
    
	do {
        change = FALSE;
        for( G_ptr = G_start ; G_ptr != NULL ; G_ptr = G_ptr->next ) // start with first production rule
        {
            for( nonterm = G_ptr->rhs_string ; nonterm != NULL ; nonterm = nonterm->next )
            {
				if( search_token( nonterminal_start , nonterm->string ) == NULL )
                    continue;
                // follow[B] = follow[B] U (comput_first(£]) - £f)
                if ( comput_first_f( nonterm->next , follow_set_start , nonterm->string ) == TRUE )
                   change = TRUE;
                
				// if ( £f in comput_first(£]) then follow[B] = follow[B] U follow[A])
                if( is_lambda == TRUE )
                {
                    ptr = search_token( follow_set_start , G_ptr->lhs );
                    ptr2 = search_token( follow_set_start , nonterm->string );
                    //printf(" %s %s \n",ptr->string , ptr2->string);
					if ( add_set( ptr2 , ptr) == TRUE )
                       change = TRUE;
                }
                
            }
        }
	} while ( change );
     
}

void fill_predict_set(void)
{
	 GRAMMAR *G_ptr;
     TOKEN   *ptr , *first , *follow , *predict;
     SET     *set_ptr , *set_srtat , *set_end;
	 predict =(TOKEN*) malloc(sizeof(TOKEN));
     predict->set = NULL;
     predict->next = NULL;
	 strncpy( predict->string , "predict" , strlen("predict")+1 );
     
     for ( G_ptr = G_start ; G_ptr != NULL ; G_ptr = G_ptr->next )     
     {
         set_srtat = NULL;
         set_end = NULL;
		 //predict = search_token( predict_set_start , G_ptr->lhs );
         for ( ptr = G_ptr->rhs_string ; ptr != NULL ; ptr = ptr->next )
         {
             first = search_token( first_set_start , ptr->string );

             if ( first == NULL )
			 {
                 continue;
             }
             else if ( search_set( first , "£f" ) == TRUE )
             {
                 add_set( predict , first );
				 delete_set( &(predict->set) , "£f" );
                 continue;
             } 
             else
             {
                 add_set( predict , first );
				 break;
             }
         }
         
         if ( ptr == NULL )
         {
			 follow = search_token( follow_set_start , G_ptr->lhs );
             add_set( predict , follow );
         }
         
         
         for( set_ptr = predict->set ; set_ptr != NULL ; set_ptr = set_ptr->next )
		 {
             insert_predict( &set_srtat , &set_end , set_ptr->string );
             //printf("%s ",set_ptr->string);
         }
         G_ptr->set = set_srtat;
                  
		 //printf("\n");
         //view_token(predict);
         free_set(&(predict->set));
         
     }
}

int insert_predict( SET **start_point , SET **add_point , char *term )
{
    SET *ptr , *create;    


	create =(SET*) malloc( sizeof(SET) );
    
    ptr = *add_point;
    create->next = NULL;
    
    strncpy( (char*) create->string , term , strlen(term)+1 );

    if ( *start_point == NULL )
    {
        *start_point = create;
        *add_point = create;
    }
	else
    {
        ptr->next = create;
        *add_point = create; 
    }
}

int search_set( TOKEN *set_point , char *string )
{
    SET *ptr;

	for ( ptr = set_point->set ; ptr != NULL ; ptr = ptr->next )
	{

		if( strcmp( string , ptr->string ) == 0 )
		{
		   return TRUE;
		}
	}
	return FALSE;
}

int add_set( TOKEN *dst , TOKEN *src )
{
	SET *ptr;
	int change;

	change = FALSE;

	for ( ptr = src->set ; ptr != NULL ; ptr = ptr->next )
	{
		if ( insert_set( dst , ptr->string ) == TRUE )
           change = TRUE;
    }
    
    return change;
}

int delete_set( SET **set_point , char *string )
{
    SET *ptr , *tmp;
    
    ptr = *set_point;
	if (ptr == NULL)
        return 0;
    if( strcmp( ptr->string , string ) == 0 )
    {
        *set_point = ptr->next;
        free(ptr);
		return 0;
    }
    
    for ( ptr = (*set_point) ; ptr->next != NULL ; ptr = ptr->next )
    {
        if( strcmp( string , ptr->next->string ) == 0 )
		{
            tmp = ptr->next;
            ptr->next = tmp->next;
            free(tmp);    
            return 0;
        }
	}
    
}


int free_grammar()
{
    GRAMMAR *ptr;
	//ptr = G_start;
	while( G_start != NULL )
	{
		ptr = G_start;
		G_start = G_start->next;
        free_token( &(ptr->rhs_string) );
        free_set( &(ptr->set) );
        free(ptr);
    }
    
}

int free_token( TOKEN **free_start )
{
    TOKEN *ptr;
    ptr = *free_start;
	while ( *free_start != NULL )
    {
        ptr = *free_start;
        *free_start = (*free_start)->next;
        free_set( &(ptr->set) );
        free(ptr);
	}
}

int free_set( SET **free_start )
{
    SET *ptr;
	ptr = *free_start;
    while ( *free_start != NULL )
    {
        ptr = *free_start;
        *free_start = (*free_start)->next;
        free(ptr);
	}
}

void output_perdict_table()
{
    GRAMMAR *G_ptr;
	TOKEN *term , *nonterm;
    SET *S_ptr;
    int i , j;
    FILE *csv;
    csv = fopen("predict.csv","w");
    
	fprintf( csv , "\t," );

	for( term = terminal_start ; term != NULL ; term = term->next )
		fprintf( csv , "\t%s," , term->string );

	fprintf( csv , "\tf,\n");

	for (i = 0 , nonterm = nonterminal_start ; nonterm != NULL ; nonterm = nonterm->next , i++ )
	{
		fprintf( csv , "%s,",nonterm->string );

		for( j = 0 , term = terminal_start ; term != NULL ; term = term->next , j++ )
		{
            
            fprintf( csv , "\t" );
            for ( G_ptr = G_start ; G_ptr != NULL ; G_ptr = G_ptr->next )            
            {
               
				if( strcmp( G_ptr->lhs , nonterm->string ) != 0 )
                    continue;
        
                if ( is_in_set( G_ptr->set , term->string ) == TRUE )
                {
                     /*if( strcmp(G_ptr->lhs , "T5") == 0 )
					 {
                         printf("%d %s %s\n",G_ptr->rule , G_ptr->set , term->string );
                     }*/
                     fprintf( csv , " %d" , G_ptr->rule );
                     if( parser_table[i][j].rule == 0 )
                         parser_table[i][j].rule = G_ptr->rule;
					 else
                         parser_table[i][j].special_tag = G_ptr->rule;
                }
            }
            fprintf( csv , "," );
        }
		// print lambda set
        fprintf( csv , "\t" );
        for ( G_ptr = G_start ; G_ptr != NULL ; G_ptr = G_ptr->next )            
        {
            if( strcmp( G_ptr->lhs , nonterm->string ) != 0 )
                continue;

            if ( is_in_set( G_ptr->set , "£f" ) == TRUE )
            {
                 fprintf( csv , " %d" , G_ptr->rule );
                 
                 if( parser_table[i][j].rule == 0 )
					 parser_table[i][j].rule = G_ptr->rule;
                 else
                     parser_table[i][j].special_tag = G_ptr->rule;
            }
        }
        fprintf( csv , "," );        

        fprintf( csv ,"\n");
    }
    fclose(csv);
}

int is_in_set( SET *set , char *string )
{
    SET *ptr;
    
    for( ptr = set ; ptr != NULL ; ptr = ptr->next )
    {
		if( strcmp( ptr->string , string ) == 0 )
            return TRUE;
    }
    
    return FALSE;
}

char *strtoupper(char *str)
{
        register char *c;

        if (!str || !*str)
				return (NULL);
        for (c = str; *c; c++)
                *c = toupper(*c);
        return (str);
}


void lldiver( TTreeView *TTreeView1 , TLabel *Label1 )
{
    GRAMMAR *G_ptr;
    G_ptr = G_start;
	
	TOKEN *S_ptr;
	SET *scan_ptr;
	int a , i , j;
	char string[64] , *str;
	TTreeNode *TREENODE , *root , *parent , *son;


	scan_start = NULL;
	scan_end = NULL;


	do
	{
		fscanf( scan , "%s" , string );
		insert_predict( &scan_start , &scan_end , string );
	} while ( strcmp(string,"$") != 0 );

	scan_ptr = scan_start;
	iterm_index = 0;
	push_stack( G_ptr->lhs , NULL );
	root = (TTreeView1)->Items->Add( NULL , idtostr(stack_top->symbol) );
	parent = root;


	a = make_id(scan_ptr->string) - num_of_nonterm;

	while( !stack_empty() )
	{
		if( strcmp( scan_ptr->string , "$" ) == 0 )
		{
			pop_stack(1);
			son = TTreeView1->Items->AddChild( parent , "£f" );
			parent = son;
		}
		else if( is_nonterm( stack_top->symbol ) == TRUE && parser_table[stack_top->symbol-1][a-1].rule != 0 ) // nonterminal
		{
			if ( parser_table[stack_top->symbol-1][a-1].special_tag != 0 )
			{
				str = idtogram(parser_table[stack_top->symbol-1][a-1].rule)->rhs_string->next->string;

				if( str != NULL && is_term(make_id(str)) == TRUE )
				{
					if( strcmp( scan_ptr->next->string , str ) == 0 )
						G_ptr = idtogram( parser_table[stack_top->symbol-1][a-1].rule );
					else
						G_ptr = idtogram( parser_table[stack_top->symbol-1][a-1].special_tag );
				}
				else
				{
					G_ptr = idtogram( parser_table[stack_top->symbol-1][a-1].rule );
				}
			}
			else
				G_ptr = idtogram( parser_table[stack_top->symbol-1][a-1].rule );

			pop_stack(1);


			push_stack( G_ptr->rhs , parent );

			parent = stack_top->parent;
			son = TTreeView1->Items->AddChild( parent , idtostr( stack_top->symbol ) );

			if( is_term( make_id( idtostr( stack_top->symbol ) ) ) == FALSE )
				parent = son;
			else
				parent = parent;


		}
		else if ( is_term( stack_top->symbol ) == TRUE && stack_top->symbol - num_of_nonterm == a ) // terminal
		{
			pop_stack(1);
            if ( stack_top == NULL  )
			   return ;
			parent = stack_top->parent;
			son = TTreeView1->Items->AddChild( parent , idtostr(stack_top->symbol) );
			parent = son;

			scan_ptr = scan_ptr->next;
			if( scan_ptr->string[0] != '$' )
				a = make_id(scan_ptr->string) - num_of_nonterm;

		}
		else if ( stack_top->symbol == make_id("£f") )
		{
			pop_stack(1);
            if ( stack_top == NULL  )
			   return ;

			parent = stack_top->parent;
			son = TTreeView1->Items->AddChild( parent , idtostr(stack_top->symbol) );
			parent = son;
		}
		else
		{
			Application->MessageBoxA("syntax error","error",0);
			sprintf( string , "error:%d. %s -> %s {%s}\n",G_ptr->rule , G_ptr->lhs , G_ptr->rhs , scan_ptr->string);
			Label1->Caption = AnsiString(string);
			return;
		}

	}
    Label1->Caption = "Parsing Success!!!\n";
}

int view_stack()
{
    STACK *ptr;
    ptr = stack_top;

    for (;ptr != NULL ; ptr = ptr->next )    
    {
        printf("%s ",idtostr(ptr->symbol) );    
    }
    printf("\n");
}


int push_stack( char *data , TTreeNode *parent )
{
	STACK *end_ptr , *start_ptr , *create;
	char *c_ptr , string[64];
	start_ptr = NULL;
	end_ptr = NULL;
	c_ptr = data;

	while ( c_ptr != NULL )
	{

		sscanf( c_ptr , "%s" , string );
		create = (STACK*) malloc( sizeof(STACK) );
		create->next = NULL;
		create->symbol = make_id( string );
		create->parent = parent;

        if( start_ptr == NULL)
		{
            start_ptr = create;
            end_ptr = create;
		}
        else
        {
			end_ptr->next = create;
            end_ptr = create;    
        }
        
		c_ptr = strstr( c_ptr , " " );
        if ( c_ptr == NULL )
            break;
        c_ptr++;
    }

	end_ptr->next = stack_top;
    stack_top = start_ptr;
}

int pop_stack( int pop_num )
{
	 STACK *ptr;
     int i;
     
     for( i = 0 ; i < pop_num ; i++ )
	 {
		  //iterm_index--;
          ptr = stack_top;
		  stack_top = ptr->next;
          free(ptr);
     }
}

int make_id( char *string )
{
    TOKEN *S_ptr;
    int id;
    
    for( S_ptr = nonterminal_start , id = 1 ; S_ptr != NULL ; S_ptr = S_ptr->next , id++ )
    {
		 if( strcmp( S_ptr->string , string ) == 0 )
             return id;
    }
    
    for( S_ptr = terminal_start ; S_ptr != NULL ; S_ptr = S_ptr->next , id++ )
    {
		 if( strcmp( S_ptr->string , string ) == 0 )
             return id;
    }
    if( strcmp( string , "£f" ) == 0 )
        return id;
}

int stack_empty()
{
    if ( stack_top == NULL )
        return TRUE;
    else
		return FALSE;
        
}

int is_nonterm( int symbol )
{
	if( symbol <= num_of_nonterm && symbol != 0 )
        return TRUE;
        
    return FALSE;
}

int is_term( int symbol )
{
    if( symbol > num_of_nonterm )
        return TRUE;
        
    return FALSE;    
}

GRAMMAR *idtogram( int symbol )
{
    GRAMMAR *G_ptr;
    
	for( G_ptr = G_start ; G_ptr != NULL ; G_ptr = G_ptr->next )
    {
        if( symbol == G_ptr->rule )
            return G_ptr;
    }

}

char *idtostr( int symbol )
{
	TOKEN *S_ptr;
	int id;

	for( S_ptr = nonterminal_start , id = 1 ; S_ptr != NULL ; S_ptr = S_ptr->next , id++ )
	{
		 if( id == symbol )
			 return S_ptr->string;
	}

	for( S_ptr = terminal_start ; S_ptr != NULL ; S_ptr = S_ptr->next , id++ )
	{
		 if( id == symbol )
			 return S_ptr->string;
	}
	if( id == symbol )
	{
        return lambda;
	}
}






void __fastcall TForm2::Loadgrammarfile1Click(TObject *Sender)
{
  char file_name[1024];
  OpenDialog1->Filter = "Text files (*.txt)|*.TXT";

//  input = fopen("grammar.txt","r");

  OpenDialog1->Execute();
}
//---------------------------------------------------------------------------







void __fastcall TForm2::Loadsourcecode1Click(TObject *Sender)
{
	OpenDialog2->Filter = "Text files (*.txt)|*.TXT";
	OpenDialog2->Execute();
}
//---------------------------------------------------------------------------

