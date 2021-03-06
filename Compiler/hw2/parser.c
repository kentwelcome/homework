#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// struct
typedef struct grammar {
        
    int     rule;
    char    lhs[256];
    char    rhs[256];
    struct set *set;
    struct token *rhs_string;
    struct grammar *next;
        
}GRAMMAR ;

typedef struct token {
        
    char string[64];
    struct set *set;
    struct token *next;
    
} TOKEN;

typedef struct set {
        
    char string[64];
    struct set *next;
    
} SET;

typedef struct stack {
    int symbol;
    int index;
    struct stack *next;
} STACK;

typedef struct parser {
    int rule;
    int special_tag;
} PARSER;

typedef struct tree {
    int index;
    char string[64];
    struct tree *brother;
    struct tree *child;
    struct tree *parent;
} TREE;

#define TRUE   1
#define FALSE  0

// variable
GRAMMAR *G_start , *G_end;
TOKEN *terminal_start , *terminal_end;
TOKEN *nonterminal_start , *nonterminal_end;
TOKEN *first_set_start , *first_set_end;
TOKEN *follow_set_start , *follow_set_end;
TOKEN *predict_set_start , *predict_set_end;
PARSER **parser_table;
STACK *stack_top;
SET *scan_start , *scan_end;
char lambda[2];
int is_lambda;
int num_of_nonterm , num_of_term;
int iterm_index;
FILE *scan;


// function
int insert_grammar( int rule , char *lhs , char *rhs );
int insert_token( TOKEN **start_point , TOKEN **add_point , char *string );
int delete_token( TOKEN **start_point ,  TOKEN *delete_point );
TOKEN *search_token( TOKEN *search_point , char *string );
int insert_set( TOKEN *symbel , char *term );
void view_token( TOKEN *start_point );
void fill_frist_set(void);
int comput_first( TOKEN *alpha , TOKEN *dst , char *dst_name );
void fill_follow_set(void);
void fill_predict_set(void);
int insert_predict( SET **start_point , SET **add_point , char *term );
int search_set( TOKEN *set_point , char *string );
int add_set( TOKEN *dst , TOKEN *src );
int free_grammar();
int free_token( TOKEN **free_start );
int free_set( SET **free_start );
void output_perdict_table();
int is_in_set( SET *set , char *string );
void lldiver();
int view_stack();
int push_stack( char *string , int index );
int pop_stack( int pop_num );
int is_empty();
GRAMMAR *idtogram( int symbol );
char *idtostr( int symbol );
GRAMMAR *idtogram( int symbol );
int is_term( int symbol );
int is_nonterm( int symbol );




int main( int argc , char *argv[] )
{
    
    GRAMMAR *G_ptr; 

    TOKEN *T_ptr , *T_tmp;
    FILE *input ,*out;
    char grammar_rule[1024];
    char *ptr , *ptr_string;
    char *ptr_lhs , *ptr_rhs;
    SET *S_ptr;
    int i , j;
    int rule_num; 

    strcpy(lambda,"λ");
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
    
    if( argc != 3 )
    {
        input = fopen("grammar.txt","r");
        scan = fopen("out.txt","r");
    }
    else
    {
        input = fopen(argv[1],"r");
        scan = fopen(argv[2],"r");
    }
    
    if ( input == NULL || scan == NULL )
    {
        printf("input file error!\n");
        return 0;     
    }
    
    out = fopen("table.csv","w");
     
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

    // create parsing table
    parser_table = malloc( sizeof(PARSER) * num_of_nonterm );
    for( i = 0 ; i < num_of_nonterm ; i++ )
    {
        parser_table[i] = malloc( sizeof(PARSER) * num_of_term );
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
    lldiver();
    
    
    
    free_token(&terminal_start);
    free_token(&nonterminal_start);
    free_token(&first_set_start);
    free_token(&follow_set_start);
    free_grammar();
    
}

int insert_grammar( int rule , char *lhs , char *rhs )
{
    GRAMMAR *ptr , *create;
    TOKEN *token_start , *token_end;
    char string[64] , *c_ptr;
    token_start = NULL;
    token_end = NULL;
    
    create = malloc( sizeof(GRAMMAR) );
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
        
        if ( search_token( terminal_start , string ) == NULL && strcmp( string , "λ" ) != 0 ) // λ is not terminal
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



    create = malloc( sizeof(TOKEN) );
    
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

int insert_set( TOKEN *symbel , char *term )
{
    SET *set_ptr , *create;
    
    for ( set_ptr = symbel->set ; set_ptr != NULL ; set_ptr = set_ptr->next )
    {
        if ( strcmp( term , set_ptr->string ) == 0 )   
            return FALSE;
    }
    
    set_ptr = symbel->set;
    
    create = malloc(sizeof(SET));
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
        if ( strcmp( G_ptr->rhs_string->string , "λ" ) == 0 )
        {
            ptr = search_token( first_set_start , G_ptr->lhs );
            insert_set( ptr , "λ" ); // add λ into first set
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
    tmp = malloc(sizeof(TOKEN));
    strncpy( tmp->string , "tmp" , 4 );
    tmp->set = NULL;
    tmp->next = NULL;
    
    if( alpha == NULL )
    {
        is_lambda = TRUE;
        return FALSE;
    }
    
    if ( strcmp( alpha->string , "λ" ) == 0 )
    {
        ptr = search_token( dst , dst_name );
        if ( insert_set( ptr , "λ" ) == TRUE )
            change = TRUE;
    }
    else
    {
        ptr = search_token( dst , dst_name );
        ptr2 = search_token( first_set_start , alpha->string );
        
        add_set( tmp , ptr2 );
        delete_set( &(tmp->set) , "λ" );
        
        if ( add_set( ptr , tmp ) == TRUE ) 
            change = TRUE;
        free_set( &(tmp->set) );
        
        for( ptr3 = alpha ; ptr3 != NULL  ; ptr3 = ptr3->next )
        {
            ptr2 = search_token( first_set_start , ptr3->string );
            if( search_set( ptr2 , "λ" ) != TRUE )
                break;

            if ( ptr3->next == NULL )
            {
                ptr_end = ptr3;
                ptr3 = search_token( first_set_start , ptr_end->string );
                if( search_set( ptr3 , "λ" ) == TRUE )
                {
                    is_lambda = TRUE;
                    if ( insert_set( ptr , "λ" ) == TRUE )
                        change = TRUE;
                }
                break;
            }
                
            ptr2 = search_token( first_set_start , ptr3->next->string );
            add_set( tmp , ptr2 );
            delete_set( &(tmp->set) , "λ" );
            
            
            if ( add_set( ptr , tmp ) == TRUE )
            {
                delete_set( &(ptr->set) , "λ" );
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
    tmp = malloc(sizeof(TOKEN));
    strncpy( tmp->string , "tmp" , 4 );
    tmp->set = NULL;
    tmp->next = NULL;

    if( alpha == NULL )
    {
        is_lambda = TRUE;
        return FALSE;
    }
    
    if ( strcmp( alpha->string , "λ" ) == 0 )
    {
        //ptr = search_token( dst , dst_name );
        //if ( insert_set( ptr , "λ" ) == TRUE )
        is_lambda = TRUE;
        change = TRUE;
    }
    else
    {
        ptr = search_token( dst , dst_name );
        ptr2 = search_token( first_set_start , alpha->string );
        
        add_set( tmp , ptr2 );
        delete_set( &(tmp->set) , "λ" );
        if ( add_set( ptr , tmp ) == TRUE ) 
            change = TRUE;
        free_set( &(tmp->set) );
        
        for( ptr3 = alpha ; ptr3 != NULL  ; ptr3 = ptr3->next )
        {
            ptr2 = search_token( first_set_start , ptr3->string );
            if( search_set( ptr2 , "λ" ) != TRUE )
                break;
            
            if ( ptr3->next == NULL )
            {
                ptr_end = ptr3;
                ptr3 = search_token( first_set_start , ptr_end->string );
                if( search_set( ptr3 , "λ" ) == TRUE )
                {
                    if ( insert_set( ptr , "λ" ) == TRUE )
                    {
                        is_lambda = TRUE;
                        change = TRUE;
                    }
                }
                break;
            }

            ptr2 = search_token( first_set_start , ptr3->next->string );
            add_set( tmp , ptr2 );
            delete_set( &(tmp->set) , "λ" );
            if ( add_set( ptr , tmp ) == TRUE )
            {
                delete_set( &(ptr->set) , "λ" );
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
    
    test = malloc(sizeof(TOKEN));

    // set start symbol 
    ptr = follow_set_start;
    insert_set( ptr , "λ" );
    
    do {
        change = FALSE;
        for( G_ptr = G_start ; G_ptr != NULL ; G_ptr = G_ptr->next ) // start with first production rule
        {
            for( nonterm = G_ptr->rhs_string ; nonterm != NULL ; nonterm = nonterm->next )
            {
                if( search_token( nonterminal_start , nonterm->string ) == NULL )
                    continue;
                // follow[B] = follow[B] U (comput_first(β) - λ)
                if ( comput_first_f( nonterm->next , follow_set_start , nonterm->string ) == TRUE )
                   change = TRUE;
                
                // if ( λ in comput_first(β) then follow[B] = follow[B] U follow[A])
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
     predict = malloc(sizeof(TOKEN));
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
             else if ( search_set( first , "λ" ) == TRUE )
             {
                 add_set( predict , first );
                 delete_set( &(predict->set) , "λ" );
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


    create = malloc( sizeof(SET) );
    
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
//    if( set_point->set->next = NULL )
//        printf("%s\n", set_point->set->string );
        
    for ( ptr = set_point->set ; ptr != NULL ; ptr = ptr->next )
    {
        //printf( "%s %s\n" , set_point->string , ptr->string ); 
        
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
        //if( strcmp( ptr->string , "λ" ) == 0 )
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
    
            if ( is_in_set( G_ptr->set , "λ" ) == TRUE )
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


void lldiver()
{
    GRAMMAR *G_ptr;
    G_ptr = G_start;

    SET *scan_ptr;
    int a , i;
    char string[64] , *str;
    iterm_index = 0;
    int treenode;
    
    scan_start = NULL;
    scan_end = NULL;
    
    do
    {
        fscanf( scan , "%s" , string );  
        insert_predict( &scan_start , &scan_end , string );
    } while ( strcmp(string,"$") != 0 );
    scan_ptr = scan_start;
    
    treenode = 0;
    push_stack( G_ptr->lhs , treenode );
    printf("Match grammar %s\n" , idtostr(stack_top->symbol) );
    treenode = 1;
    
    a = make_id(scan_ptr->string) - num_of_nonterm;
    
    while( !stack_empty() )
    {
        if( strcmp( scan_ptr->string , "$" ) == 0 )
        {
            pop_stack(1);
            for( i=0 ; i<treenode ; printf(" ") , i++ );
            printf("%d Match lambda f\n",treenode);
        }
        else if( is_nonterm( stack_top->symbol ) == TRUE && parser_table[stack_top->symbol-1][a-1].rule != 0 ) // nonterminal
        {
            // solve hard conflicts
            if ( parser_table[stack_top->symbol-1][a-1].special_tag != 0 )
            {
                // use LL(2) to chouse which rule
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
            else  // ignore conflict rule
                G_ptr = idtogram( parser_table[stack_top->symbol-1][a-1].rule );

            
            pop_stack(1);
            push_stack( G_ptr->rhs , treenode );
            treenode = stack_top->index;
            for( i=0 ; i<treenode ; printf(" ") , i++ );
            if( is_nonterm( stack_top->symbol ) == TRUE )
            {
                printf("Match grammar %s\n" , idtostr(stack_top->symbol) );
                treenode++;            
            }
            else
                printf("Match token %s\n" , idtostr(stack_top->symbol) );

        }
        else if ( is_term( stack_top->symbol ) == TRUE && stack_top->symbol - num_of_nonterm == a ) // terminal
        {
            pop_stack(1);
			if(stack_top == NULL)
				return;
            treenode = stack_top->index;
            for( i=0 ; i<treenode ; printf(" ") , i++ );
            if( is_nonterm( stack_top->symbol ) == TRUE )
            {
                printf("Match grammar %s\n", idtostr(stack_top->symbol) );
                treenode++;
            }
            else
                printf("Match token %s\n" , idtostr(stack_top->symbol) );
            
            if( stack_top->symbol != make_id("λ") )
                scan_ptr = scan_ptr->next;
                
            if( scan_ptr->string[0] != '$' )
                a = make_id(scan_ptr->string) - num_of_nonterm;
            
            
        }
        else if( stack_top->symbol == make_id("λ") ) // eat lambda
        {
            pop_stack(1);
            treenode = stack_top->index;
            for( i=0 ; i<treenode ; printf(" ") , i++ );
            printf("Match grammar %s\n", idtostr(stack_top->symbol) );
            treenode++;
        }
        else // syntax error
        {
            printf("syntax error: %d. %s -> %s {%s} %d %d\n",G_ptr->rule , G_ptr->lhs , G_ptr->rhs , scan_ptr->string , stack_top->symbol , a);
            return;
        }
        
    }
}



int view_stack()
{
    STACK *ptr;
    ptr = stack_top;
    
    
    for (ptr = stack_top;ptr != NULL ; ptr = ptr->next )    
        printf("%s ",idtostr(ptr->symbol) );    

    printf("\n");
}


int push_stack( char *data , int index )
{
    STACK *end_ptr , *start_ptr , *create;
    char *c_ptr , string[64];
    
    start_ptr = NULL;
    end_ptr = NULL;
    c_ptr = data;
    
    while ( c_ptr != NULL )
    {

        sscanf( c_ptr , "%s" , string );
        create = malloc( sizeof(STACK) );
        create->index = index;
        create->next = NULL;
        create->symbol = make_id( string );
        
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
    if( strcmp(string , "λ") == 0 )
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
    if ( id == symbol )
       return lambda;
}
