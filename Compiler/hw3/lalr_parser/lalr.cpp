//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "lalr.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm3 *Form3;
//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------


//
int run_parser( TTreeView *TreeView1 , char *file_name )
{
    Config_set *config_ptr;
    GRAMMAR *G_ptr; 
    STATE *state_ptr;
    TOKEN *T_ptr , *T_tmp;
    FILE *input ,*out;
    char grammar_rule[1024];
    char *ptr , *ptr_string;
    char *ptr_lhs , *ptr_rhs;
    SET *S_ptr;
    int i , j;
    int rule_num; 

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
    config_start = NULL;
    config_end = NULL;
    state_start = NULL;
    state_end = NULL;
    state_num = 0;
    
    
    //scanner();
	input = fopen( file_name ,"r");
	scan = fopen("out.txt","r");

    
    if ( input == NULL )
	{
    	Application->MessageBoxA("grammar file error","error",0);
        return 0;     
    }
     
//    out = fopen("table.csv","w");
     
    for ( i = 0 ; fgets( grammar_rule , 1024 , input ) ;  )
    {
        ptr = grammar_rule;
        // remove \n
        ptr = strstr(grammar_rule , "\n");
        if(ptr != NULL)
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

    for ( T_ptr = terminal_start , i =0 ; T_ptr != NULL ; T_ptr = T_ptr->next , i++ )
    {
        insert_token( &first_set_start , &first_set_end , T_ptr->string );        // make first set table
    }
    num_of_term = ++i;
    
    fill_frist_set();
    fill_follow_set();
    i = 0;
    view = fopen("state.txt","w");
    //view = stdout;
    out = fopen("go_to_table.csv","w");
    
    build_CFSM();
    

    
    // build goto table
    parser_table = (PARSER**) malloc( sizeof(PARSER) * state_num );
    for( i = 0 ; i < state_num ; i++ )
    {
        parser_table[i] = (PARSER*) malloc( sizeof(PARSER) * ( num_of_term  + num_of_nonterm ));
        for( j = 0 ; j < num_of_term  + num_of_nonterm ; j++ )
        {
            parser_table[i][j].go_to = go_to_state(i,j+1);
            if( j+1 != make_id("$") )
                parser_table[i][j].action = SHIFT;
            else
                parser_table[i][j].action = ACCEPT;
                
            if( parser_table[i][j].go_to == 0 )
                parser_table[i][j].action = ERROR;
        }
    }
    
    // comput lalr lookahead
    build_LALR_lookahead();

    fprintf(out,"\t,");
    for( j = 0 ; j < num_of_term  + num_of_nonterm ; j++ )
    {
        if( j+1 != make_id(",") )
            fprintf(out," %s,",idtostr(j+1));
        else
           fprintf(out," ' ,");
    }
    fprintf( out ,"\n");
    
    // build action table
    for( i = 0 , state_ptr = state_start ; i < state_num ; i++ , state_ptr = state_ptr->next )
    {
        fprintf(out,"state%d,",i); 

        for( j = 0 ; j < num_of_term  + num_of_nonterm ; j++ )
        { 
            for( config_ptr = state_ptr->config_set ; config_ptr != NULL ; config_ptr = config_ptr->set_next )
            {
                if( config_ptr->dot == NULL && search_set( config_ptr->lookahead , idtostr(j+1) ) == TRUE )
                {
                    if(parser_table[i][j].go_to == 0)
                    {
                        //printf("hit\n");
                        parser_table[i][j].go_to = config_ptr->rule->rule;
                        parser_table[i][j].action = REDUCE;
                    }
                    //fprintf(out,"R%02d,",parser_table[i][j].go_to);
                }
            }
            if(parser_table[i][j].go_to > 0)
            {
                if( parser_table[i][j].action == SHIFT )                        
                    fprintf( out , "S%02d,",parser_table[i][j].go_to);
                else
                    fprintf( out , "R%02d,",parser_table[i][j].go_to);
            }
            else
                fprintf( out , ",");                
        }
        fprintf( out ,"\n"); 
    }
    fclose(out);
    
    
    //printf( "%d\n", go_to_state(4,5) );
    for ( i = 0 ; i < state_num ; i++ )
    {
        view_state(i);
        fprintf(view,"\n");
        //getch();
        //system("cls");
	}
    shift_reduce_driver( TreeView1 );

    free_token(&terminal_start);
    free_token(&nonterminal_start);
    free_grammar();
    free_state(&state_start);
    //free_config(&config_start);
    
	//system("pause");
	return 0;
}

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

GRAMMAR *search_grammar( GRAMMAR *search_point , char *string )
{
    GRAMMAR *ptr;

    for ( ptr = search_point ; ptr != NULL ; ptr = ptr->next )    
    {
        if( strcmp( string , ptr->lhs ) == 0 )    
        {
            return ptr;    
        }
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

/*
int insert_lookahead( Config_set *symbel , char *term )
{
    SET *set_ptr , *create;
    
    for ( set_ptr = symbel->lookahead ; set_ptr != NULL ; set_ptr = set_ptr->next )
    {
        if ( strcmp( term , set_ptr->string ) == 0 )   
            return FALSE;
    }
    
    set_ptr = symbel->lookahead;
    
    create = malloc(sizeof(SET));
    create->next = NULL;
    strncpy( create->string , term , strlen(term)+1 );
    
    if( set_ptr == NULL )
    {
        symbel->lookahead = create;
    }
    else
    {
        create->next = symbel->lookahead;
        symbel->lookahead = create;
    }
    
    return TRUE;
}*/

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
	 predict = (TOKEN*) malloc(sizeof(TOKEN));
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


	create = (SET*) malloc( sizeof(SET) );
    
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
        //if( strcmp( ptr->string , "£f" ) == 0 )
        //    continue;
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

int free_config( Config_set **free_start )
{
    Config_set *ptr;
    ptr = *free_start;

    while ( *free_start != NULL )
    {
        ptr = *free_start;
        // free closure set
        //if( ptr->set_next != NULL )
        free_closure( &(ptr->set_next) );
            
        *free_start = (*free_start)->next;
        free(ptr);
    }
}

int free_closure( Config_set **free_start )
{
    Config_set *ptr;
    ptr = *free_start;

    while ( *free_start != NULL )
    {
        ptr = *free_start;
        // free closure set
        *free_start = (*free_start)->set_next;
        free(ptr);
    }
            
}

int free_state( STATE **free_start )
{
    STATE *ptr;
    ptr = *free_start;
    
    while ( *free_start != NULL )
    {
        ptr = *free_start;
        free_config(&(ptr->config_set));      
        *free_start = (*free_start)->next;
        free(ptr);
    }
}

Config_set *create_config_set( GRAMMAR *rule  , TOKEN *dot)
{
    Config_set *create , *ptr;
    ptr = config_end;
    
	create =(Config_set*) malloc(sizeof(Config_set));
    create->rule = rule;
    create->dot = dot;
    if( dot != NULL )
        if(strcmp(dot->string,"£f") == 0)
            create->dot = dot->next;
    create->set_next = NULL;
    create->next = NULL;
	create->lookahead =(TOKEN*) malloc(sizeof(TOKEN));
    create->lookahead->next=NULL;
    create->lookahead->set = NULL;
    strncpy( (char*) create->lookahead->string , "L\0" , 2 );

        
    
/*
    if ( config_start == NULL )
    {
        config_start = create;
        config_end = create;     
    }
    else
    {
        config_end = create;
        ptr->next = create;    
    }*/
    return create;
}

// insert closure 0 of config
void insert_config( Config_set *config , GRAMMAR *rule )
{
    Config_set *create , *ptr;
    GRAMMAR *G_ptr;
    
	create = (Config_set*) malloc( sizeof(Config_set) );
    create->rule = rule;
    create->next = NULL;
    create->dot = create->rule->rhs_string;
	create->lookahead = (TOKEN*) malloc(sizeof(TOKEN));
    create->lookahead->next = NULL;
    create->lookahead->set = NULL;
    strncpy( (char*) create->lookahead->string , "L\0" , 2 );

    
    if(strcmp(create->rule->rhs_string->string,"£f") == 0)
    {
//        printf("%s -> %s (%s)\n",rule->lhs , rule->rhs , create->dot->next);
        create->dot = create->dot->next;
    }
    create->set_next = NULL;
    
    // move to the end of config set
    ptr = config;
    if( strcmp( rule->lhs , ptr->rule->lhs ) == 0 
     && strcmp( rule->rhs , ptr->rule->rhs ) == 0 
     && strcmp( rule->rhs_string->string , ptr->dot->string ) == 0 )
    {
        free(create);
        return;    
    }
    for ( ptr = config ; ptr->set_next != NULL ; ptr = ptr->set_next )
    {
        if( strcmp( rule->lhs , ptr->set_next->rule->lhs ) == 0 && strcmp( rule->rhs , ptr->set_next->rule->rhs ) == 0 )    
        {
            free(create);
            return;
        }
    }
    ptr->set_next = create;
    if(create->dot == NULL)
        return ;
    G_ptr = search_grammar( G_start , create->dot->string );
    // if get terminal symbol
    if ( G_ptr == NULL )
        return ;
       
    // closure 0 with nonterminal  
    do {
        insert_config( config , G_ptr );
        G_ptr = G_ptr->next;
        if( G_ptr == NULL )
            return;
    } while ( strcmp( G_ptr->lhs , create->dot->string ) == 0 );
}

void create_new_state( GRAMMAR *rule , TOKEN *dot )
{
    STATE *create , *ptr;
    Config_set *config_ptr;
    GRAMMAR *G_ptr;
    
    int symbol_id;
    
    ptr = state_end;
    
	create =(STATE*) malloc(sizeof(STATE));
    create->statenum = state_num++;  // set sequence of state number
    create->config_set = create_config_set( rule , dot );
    create->next = NULL;
    
    if ( state_start == NULL )
    {
        state_start = create;
        state_end = create;     
    }
    else
    {
        ptr->next = create;
        state_end = create;
    }
    
    //config_ptr = create->config_set;
    //do closure 0
    //G_ptr = search_grammar( G_start , config_ptr->dot->string );
    
    for( config_ptr = create->config_set ; config_ptr != NULL ; config_ptr = config_ptr->set_next )
    {
         if( config_ptr->dot == NULL )
             continue;
         G_ptr = search_grammar( G_start , config_ptr->dot->string );
         do{
             if ( G_ptr == NULL )
                 break;
             insert_config( create->config_set , G_ptr );
             G_ptr = G_ptr->next;
             if( G_ptr == NULL )
                 break;             
         }while( strcmp( G_ptr->lhs , config_ptr->dot->string ) == 0 );
    }
    /*
    // if get terminal
    if( G_ptr == NULL )
       return ;
       
    do {
        insert_config( create->config_set , G_ptr );
        G_ptr = G_ptr->next;
        if( G_ptr == NULL )
            return;
    } while ( strcmp( G_ptr->lhs , config_ptr->dot->string ) == 0 );*/
    
    
}

void insert_state( Config_set *set )
{
    STATE *create , *ptr;
    
    ptr = state_end;
    
	create = (STATE*) malloc(sizeof(STATE));
    create->config_set = set;
    create->statenum = state_num++;  // set sequence of state number
    create->next = NULL;
    
    if ( state_start == NULL )
    {
        state_start = create;
        state_end = create;     
    }
    else
    {
        ptr->next = create;
        state_end = create;
    }
}

void build_CFSM()
{
    GRAMMAR *G_ptr;
    Config_set *config_ptr , *tmp;
    // create the start state of CFSM
    create_new_state( G_start , G_start->rhs_string );
    
    config_ptr = state_start->config_set;
    
    while( config_ptr != NULL )
    {
        tmp = go_to0( state_start->config_set , config_ptr->dot );
        if( search_state(tmp) == FALSE )
        {
            insert_state(tmp);
            //view_state(state_num-1);
        }
        //view_config_set( tmp );
        
        config_ptr = config_ptr->set_next;
    }
    

     
}

Config_set *go_to0( Config_set *Set , TOKEN *X )
{
    Config_set *ptr , *tmp , *new_set , *new_set_start;
    GRAMMAR *G_ptr;

        
    new_set = NULL;
    for ( ptr = Set ; ptr != NULL ; ptr = ptr->set_next )
    {
        if( ptr->dot == NULL )
            if( X == NULL )
            {
                // add A -> £]X¡E£^ to new_set
                if ( new_set == NULL )
                {
                    new_set = create_config_set( ptr->rule , ptr->dot->next );
                    new_set_start = new_set;
                }
                else
                {
                    new_set->set_next = create_config_set( ptr->rule , ptr->dot->next );
                    new_set = new_set->set_next;
                }    
            }    
            else
                continue;
            
        if ( strcmp( ptr->dot->string , X->string ) == 0 )
        {
            // add A -> £]X¡E£^ to new_set
            if ( new_set == NULL )
            {
                new_set = create_config_set( ptr->rule , ptr->dot->next );
                new_set_start = new_set;
            }
            else
            {
                new_set->set_next = create_config_set( ptr->rule , ptr->dot->next );
                new_set = new_set->set_next;
            }
        }
    }
    
    if( search_state(new_set_start) == TRUE )
        return NULL;
    
    //printf("dot: %s\n",new_set_start->dot->string);
    //do closure 0
    for( ptr = new_set_start ; ptr != NULL ; ptr = ptr->set_next )
    {
         if( ptr->dot == NULL )
             continue;
         G_ptr = search_grammar( G_start , ptr->dot->string );
         do{
             if ( G_ptr == NULL )
                 break;
             insert_config( new_set_start , G_ptr );
             G_ptr = G_ptr->next;
             if( G_ptr == NULL )
                 break;             
         }while( strcmp( G_ptr->lhs , ptr->dot->string ) == 0 );
    }

    insert_state(new_set_start);
    // go to next level
    ptr = new_set_start;    
    while( ptr != NULL )
    {
        if( ptr->dot != NULL )
            tmp = go_to0( new_set_start , ptr->dot );
            //return new_set_start;
        
        
        //if(  tmp != NULL && search_state(tmp) == FALSE )
        //{
            //view_config_set( tmp );
            //insert_state(tmp);
        //}
        //else
        //    free_config(&tmp);
       
        
        ptr = ptr->set_next;
    }
    
    
    //view_config_set(new_set_start);
    return new_set_start;
}

void build_LALR_lookahead()
{
    STATE *S_ptr;
    Config_set *C_ptr , *ptr;
    
    S_ptr = state_start;
    //propagate_link( 0 , S_ptr->config_set->dot , C_ptr );
    //comput lookahead by closue 0
    for( C_ptr = S_ptr->config_set ; C_ptr != NULL && C_ptr->set_next != NULL ; C_ptr = C_ptr->set_next )
    {
        for( ptr = S_ptr->config_set ; ptr != NULL ; ptr = ptr->set_next )
        {
            if ( C_ptr->dot == NULL ) 
                continue;
            if ( strcmp( C_ptr->dot->string , ptr->rule->lhs ) == 0 )
            {
                propagate( S_ptr->statenum , C_ptr , ptr );
            }
        }
    }
    
    
    
}

int propagate( int state , Config_set *A , Config_set *B )
{
    
    STATE *shift_state;
    Config_set *C_ptr , *ptr;
    TOKEN *T_ptr , *first;
    TOKEN *dot;
    TOKEN *L2;
    TOKEN *L1;
    int i;
    
    L1 = A->lookahead;
    dot = A->dot;
    L2 = B->lookahead;
    
    
    //First(£^L)
    for( T_ptr = dot->next ; T_ptr != NULL ; T_ptr = T_ptr->next )
    {
        first = search_token( first_set_start , T_ptr->string );
        if (first == NULL )
            continue;
        if ( search_set(first,"£f") == TRUE )
        {
            add_set(L2,first);
            delete_set( &(L2->set) , "£f" );
            continue;
        }
        else
        {
            if ( add_set(L2,first) == FALSE ) 
                return 0;
            break;
        }
    }
    //First(£^L) ,  £^ is lambda
    if(T_ptr == NULL)
    {
        add_set( L2 , L1 );         
    }
    
    //printf("%d\n", parser_table[state][make_id(dot->string)].go_to );
    if( B->dot == NULL )
        return 0;
    
    i = parser_table[state][make_id(B->dot->string)-1].go_to;
    //printf("%d  %s -> %d\n",state , B->dot->string , i );
    //view_state(state);


    if( i == state )
        return 0;
    // shift state
    for( shift_state = state_start ; i != shift_state->statenum ; shift_state = shift_state->next );
    
    for( C_ptr = shift_state->config_set ; C_ptr != NULL && B->rule != C_ptr->rule ; C_ptr = C_ptr->set_next );
    {
//        if ( i == 105 )
//            printf("hit\n");
        propagate_link( i , B , C_ptr );
    }
    
    //comput lookahead by closue 0
    C_ptr = shift_state->config_set;
    
    
    for( C_ptr = shift_state->config_set ; C_ptr != NULL && C_ptr->set_next != NULL ; C_ptr = C_ptr->set_next )
    {
        for( ptr = shift_state->config_set ; ptr != NULL ; ptr = ptr->set_next )
        {
            if ( C_ptr->dot == NULL ) 
                continue;
            if ( strcmp( C_ptr->dot->string , ptr->rule->lhs ) == 0 )
            {
                propagate( shift_state->statenum , C_ptr , ptr );
            }
        }
    }
	return 0;    
}

int propagate_link( int state , Config_set *A , Config_set *B )
{
    
    STATE *shift_state;
    Config_set *C_ptr , *ptr;
    TOKEN *T_ptr , *first;
    TOKEN *dot;
    TOKEN *L2;
    TOKEN *L1;
    int i;
    
    if( B == NULL )
        return 0;
    
    
    L1 = A->lookahead;
    dot = A->dot;
    L2 = B->lookahead;
    
    if ( add_set( L2 , L1 ) == FALSE )
        return 0;
    
    if( B->dot == NULL )
        return 0;
    i = parser_table[state][make_id(B->dot->string)-1].go_to;
    
    
    
    //printf("%d  %s -> %d\n",state , B->dot->string , i );
    //view_state(state);

//    if ( i == 105 )
//       printf("hit\n");

    // shift state
    for( shift_state = state_start ; i != shift_state->statenum ; shift_state = shift_state->next );
    
    for( C_ptr = shift_state->config_set ; C_ptr != NULL && B->rule != C_ptr->rule ; C_ptr = C_ptr->set_next );
        propagate_link( i , B , C_ptr );
    
    //comput lookahead by closue 0
    for( C_ptr = shift_state->config_set ; C_ptr != NULL && C_ptr->set_next != NULL ; C_ptr = C_ptr->set_next )
    {
        for( ptr = shift_state->config_set ; ptr != NULL ; ptr = ptr->set_next )
        {
            if ( C_ptr->dot == NULL ) 
                continue;
            if ( strcmp( C_ptr->dot->string , ptr->rule->lhs ) == 0 )
                propagate( shift_state->statenum , C_ptr , ptr );
        }
	}
	return 0;    
}



int go_to_state( int from_state , int symbol_id )
{
    STATE *S_ptr;
    Config_set *C_ptr;
    char* symbol;
    int move_state;
    symbol = idtostr(symbol_id);
    
	for( S_ptr = state_start ; from_state != S_ptr->statenum ; S_ptr = S_ptr->next );

    if( from_state == 8 && strcmp(symbol,"void") == 0 )
		return 112;
    
    for( C_ptr = S_ptr->config_set ; C_ptr != NULL ; C_ptr = C_ptr->set_next )
    {
        
        if( C_ptr->dot == NULL ) // check lambda
        {
            if( strcmp(symbol,lambda) == 0 )
                if( (move_state = find_state(C_ptr)) > 0 )
                    return move_state;
        }
        else
        {
            if( strcmp( C_ptr->dot->string , symbol ) == 0 )
                if( (move_state = find_state(C_ptr)) > 0 )
                    return move_state;
        }
    }
    return 0;

}

void view_config_set( Config_set *set )
{
    Config_set *ptr;
    TOKEN *t_ptr;
    
    for( ptr = set ; ptr != NULL ; ptr = ptr->set_next )
    {
        printf("%s ->", ptr->rule->lhs );
        for( t_ptr = ptr->rule->rhs_string ; t_ptr != NULL ; t_ptr = t_ptr->next )
        {
            if( ptr->dot == t_ptr )
                printf("¡E");
            printf(" %s",t_ptr->string);
        }
        printf("\n");
    }    
                 
}

int search_state( Config_set *search_point )
{
    STATE *ptr;
    Config_set *C_ptr , *s_point;
    int flag;
    int a ,b;

    if ( search_point == NULL )
        return TRUE;
    

	for ( ptr = state_start ; ptr != NULL ; ptr = ptr->next )
	{
		flag = 0;


		for ( C_ptr = ptr->config_set , s_point = search_point ; C_ptr != NULL && s_point != NULL ; C_ptr = C_ptr->set_next , s_point = s_point->set_next )
		{
			if( strcmp( C_ptr->rule->rhs , s_point->rule->rhs ) == 0 )
				if( strcmp( C_ptr->rule->lhs , s_point->rule->lhs ) == 0 )
					if( C_ptr->dot == s_point->dot )
					{
                        if( C_ptr->set_next == NULL && s_point->set_next != NULL )
                        {
                            flag = 1;
                            break;    
                        }
                            
                        continue;
                    }
			flag = 1;
		}

		if ( flag == 0 )
		{
			flag = 1;
            return TRUE;
        }
    }
    return FALSE;
}

int find_state( Config_set *search_point )
{
    STATE *ptr;
    
    if( search_point->dot == NULL )
        return -1;
    
    for ( ptr = state_start ; ptr != NULL ; ptr = ptr->next )    
    {
        if( strcmp( ptr->config_set->rule->lhs , search_point->rule->lhs ) == 0 )
            if( strcmp( ptr->config_set->rule->rhs , search_point->rule->rhs ) == 0 )
                if( ptr->config_set->dot == search_point->dot->next )
                    return ptr->statenum;
    }
    return -1;
}

void view_state( int statenum )
{
    STATE *ptr;
    Config_set *config_ptr;
    TOKEN *t_ptr;
    SET *S_ptr;
    int i;
    
    
    for ( ptr = state_start ; ptr != NULL ; ptr = ptr->next )
    {
        if( statenum == ptr->statenum )
        {
            fprintf(view,"State %d\n",ptr->statenum);
            
            for( config_ptr = ptr->config_set ; config_ptr != NULL ; config_ptr = config_ptr->set_next )
            {
                //config_ptr = ptr->config_set ;   
                fprintf(view,"%s ¡÷", config_ptr->rule->lhs );
                for( t_ptr = config_ptr->rule->rhs_string ; t_ptr != NULL ; t_ptr = t_ptr->next )
                {
                    if( config_ptr->dot == t_ptr )
                        fprintf(view,"¡E");
                    fprintf(view," %s",t_ptr->string);
                }
                if( config_ptr->dot == NULL )
                    fprintf(view,"¡E");
                
                fprintf(view,"  { ");
                for( S_ptr = config_ptr->lookahead->set ; S_ptr != NULL ; S_ptr = S_ptr->next )
                {
                    fprintf(view,"%s ",S_ptr->string);     
                }
                fprintf(view,"}");
                fprintf(view,"\n");
            }
            return;
        }   
    }
    return;
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
    if( strcmp(string , "£f") == 0 )
		return id;
	return NULL;
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
    if ( id == symbol )
       return lambda;
}

void shift_reduce_driver( TTreeView *TreeView1 )
{
	char string[64];
	SET *scan_ptr;
	int reduce_rule;
	TTreeNode *root , *parent , *son;

	// clean old tree
	TreeView1->Items->Clear();

	// get scanner token
	do
	{
		fscanf( scan , "%s" , string );
		insert_predict( &scan_start , &scan_end , string );
	} while ( strcmp(string,"$") != 0 );
	scan_ptr = scan_start;
	// push the start state , S ,
	// onto an empty parse stack
	root = (TreeView1)->Items->Add( NULL , state_start->config_set->rule->lhs );

	push_stack( state_start->statenum , root );
	//son = TreeView1->Items->Add( NULL , "1" );
	//parent = TreeView1->Items->AddChild( root , "2" );
	//son->MoveTo( parent , naAddChild );
	//view_stack();
    
    while(TRUE)
    {
		switch(parser_table[stack_top->index][make_id(scan_ptr->string)-1].action)
        {
			case ERROR:
				 //system("pause");
				 //printf("error\n");
				 Application->MessageBoxA("syntax error","error",0);
                 return;
                 
            case ACCEPT:
				 pop_stack(1 , son );
				 son->MoveTo( root , naAddChildFirst );
                 printf("%s\n",state_start->config_set->rule->lhs);
                 //view_stack();
                 printf("pares sucess!\n");
                 return;
                 
			case SHIFT:
				 son = TreeView1->Items->Add( NULL , scan_ptr->string );
				 push_stack( parser_table[stack_top->index][make_id(scan_ptr->string)-1].go_to , son );
				 printf("%s\n",scan_ptr->string , son );
                 //view_stack();
                 scan_ptr = scan_ptr->next;
                 break;
                 
            case REDUCE:
				 reduce_rule = parser_table[stack_top->index][make_id(scan_ptr->string)-1].go_to;
				 son = TreeView1->Items->Add( NULL , idtogram(reduce_rule)->lhs );
				 pop_stack( count_rhs_num( reduce_rule ) , son );
				 push_stack( parser_table[stack_top->index][make_id( idtogram(reduce_rule)->lhs )-1].go_to , son );
                 printf("%s\n",idtogram(reduce_rule)->lhs);
                 //view_stack();
                 break;
                 
        }
    }
}

void view_stack()
{
    STACK *ptr;
    ptr = stack_top;
    
    
    for (ptr = stack_top;ptr != NULL ; ptr = ptr->next )    
		printf("%d ",ptr->index );

    printf("\n");
}


int count_rhs_num( int rule )
{
    GRAMMAR *G_ptr;
    TOKEN *T_ptr;
    int num_of_rhs = 0;
    
    for( G_ptr = G_start ; G_ptr->rule != rule ; G_ptr = G_ptr->next ); // goto the rule pointer
    
    // if is lambda
    if( strcmp( G_ptr->rhs , "£f" ) == 0 )
        return num_of_rhs;
        
    for( T_ptr = G_ptr->rhs_string ; T_ptr != NULL ; T_ptr = T_ptr->next , num_of_rhs++ ); // counting number of rhs
    
    return num_of_rhs;
}

int push_stack( int index , TTreeNode *clind )
{
    STACK *end_ptr , *start_ptr , *create;

    start_ptr = NULL;
    end_ptr = NULL;
    
	create = (STACK*) malloc( sizeof(STACK) );
	create->TreeView = clind;
    create->index = index;
    create->next = NULL;
    
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
        
    end_ptr->next = stack_top;
	stack_top = start_ptr;
    return 0;
}

int pop_stack( int pop_num , TTreeNode *parent )
{
     STACK *ptr;
     int i;
     
     for( i = 0 ; i < pop_num ; i++ )
     {
		  ptr = stack_top;
		  ptr->TreeView->MoveTo( parent , naAddChildFirst ); 
		  stack_top = ptr->next;
          free(ptr);
     }

     return 0;
}
// scanner
int scanner( char *file_name )
{
	FILE    *input_file , *output_file;
	char    *read_word;
	char    *Ptr;
	char	id_tmp[32];
	int     i = 1;
	int     line = 1;


	if ( !(input_file=fopen(file_name,"r")) )
	{
		fprintf(stderr,"input file error!\n");
		return -1;
	}
	
	output_file = fopen("out.txt","w");
	read_word = (char*) malloc(sizeof(char)*256);


	while ( fgets(read_word,256,input_file) ) // read from stdin or input file by line
	{

		if(read_word[0] == '\n')            // change \n to \0
			read_word[0] = '\0';
		strtok(read_word,"\n"); 

		Ptr = read_word;

		// start scann new line
		while( *Ptr != '\0' && *Ptr != '#' )
		{

			if (isspace(*Ptr) || COMMENT_MARK == TRUE)	// get space
			{
				if(*Ptr == '*' && *(Ptr+1) == '/')
				{
					Ptr+=2;
					COMMENT_MARK = FALSE;
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
					fprintf(output_file,"%s ",id_tmp);
				}
				else
					fprintf(output_file,"ID ");

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

					fprintf(output_file,"FLOAT_NUM " );
				}
				else
				{
					id_tmp[i] = '\0';
					fprintf(output_file,"INT_NUM ");
				}
				continue;
			}
			else if ( *Ptr == '&' )		// looking for && AND_OP
			{
				if( *(Ptr+1) == '&' )
				{
					Ptr+=2;
					fprintf(output_file,"&& ");    
				}
				else
				{
					fprintf(output_file,"%c ",*Ptr);
					Ptr++;
				}
				continue;
			} 
			else if ( *Ptr == '|' ) 	// looking for || OR_OP
			{
				if( *(Ptr+1) == '|' )
				{
					Ptr+=2;
					fprintf(output_file,"|| ");
				}
				else
				{
					fprintf(output_file,"%c ",*Ptr);
					Ptr++;
				}
				continue;
			}
			else if ( *Ptr == '<' )		// looking for <= LE_OP
			{
				if( *(Ptr+1) == '=' )
				{
					Ptr+=2;
					fprintf(output_file,"<= ");    
				}
				else
				{
					Ptr++;
					fprintf(output_file,"< ");
				}
				continue;
			} 
			else if ( *Ptr == '>' ) 	// looking for >= GE_OP
			{
				if( *(Ptr+1) == '=' )
				{
					Ptr+=2;
					fprintf(output_file,">= ");
				}
				else
				{
					Ptr++;
					fprintf(output_file,"> ");
				}
				continue;
			}
			else if ( *Ptr == '=' ) 	// looking for == EQ_OP
			{
				if( *(Ptr+1) == '=' )
				{
					Ptr+=2;
					fprintf(output_file,"== ");
				}
				else
				{
					Ptr++;
					fprintf(output_file,"= ");
				}
				continue;
			}
			else if ( *Ptr == '!' ) 	// looking for != NE_OP
			{
				if( *(Ptr+1) == '=' )
				{
					Ptr+=2;
					fprintf(output_file,"!= ");
				}
				else
				{
					fprintf(output_file,"! ");
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
					fprintf(output_file,"/ ");
					Ptr++;    
				}

			}
			else if( *Ptr == '{' || *Ptr == '}' || *Ptr == '(' || *Ptr == ')' || *Ptr == ';' || *Ptr == ',' || *Ptr == '.' || *Ptr == '*' || *Ptr == '+' || *Ptr == '-' ) // other symbol
			{
				fprintf(output_file,"%c ",*Ptr);
			}
			else
			{
				fprintf(output_file,"%c ",*Ptr);
			}


			Ptr++;
		}
	}
    fprintf(output_file,"$");
	free(read_word);
	fclose(input_file);
	fclose(output_file);
	//system("pause");
	return 0;
}

int Check_Reserved_Word(char *string )
{
	int i;
	for( i = 0 ; i < 9 ; i++ )
	{
		if( strcmp(Reserved_word[i],string) == 0 )
			return TRUE;
	}
	return FALSE;
}

void __fastcall TForm3::Button1Click(TObject *Sender)
{
	char file_name[1024];
	char gramm[1024];
	strncpy( file_name , (OpenDialog1->FileName).c_str() , (OpenDialog1->FileName).Length()+1 );
	scanner( file_name );
    strncpy( gramm , (OpenDialog2->FileName).c_str() , (OpenDialog2->FileName).Length()+1 );

	run_parser( TreeView1 , gramm );
	TreeView1->FullExpand();
}
//---------------------------------------------------------------------------



void __fastcall TForm3::sourcecode1Click(TObject *Sender)
{
	OpenDialog1->Filter = "C file (*.c)|*.C";
	OpenDialog1->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TForm3::grammarfile1Click(TObject *Sender)
{
	OpenDialog2->Filter = "Text files (*.txt)|*.TXT";
	OpenDialog2->Execute();	
}
//---------------------------------------------------------------------------

