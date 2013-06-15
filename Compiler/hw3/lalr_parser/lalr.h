//---------------------------------------------------------------------------

#ifndef lalrH
#define lalrH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TForm3 : public TForm
{
__published:	// IDE-managed Components
	TButton *Button1;
	TTreeView *TreeView1;
	TMainMenu *MainMenu1;
	TMenuItem *file1;
	TMenuItem *sourcecode1;
	TOpenDialog *OpenDialog1;
	TMenuItem *grammarfile1;
	TOpenDialog *OpenDialog2;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall sourcecode1Click(TObject *Sender);
	void __fastcall grammarfile1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm3(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm3 *Form3;
//---------------------------------------------------------------------------
#endif
/* parser define */
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

typedef struct configuration_set {
    GRAMMAR *rule;
    TOKEN *dot;
    TOKEN *lookahead;
    struct configuration_set *next;
    struct configuration_set *set_next;
} Config_set;

typedef struct state {

    struct configuration_set *config_set;
    int statenum;
    struct state *next;
    
} STATE;

typedef struct parser {
        
    int go_to;
    int action;
    
} PARSER;

typedef struct stack {
    //int symbol;
	int index;
	TTreeNode *TreeView;
    struct stack *next;
} STACK;


#define TRUE   1
#define FALSE  0
#define SHIFT  1
#define REDUCE 2
#define ACCEPT 3
#define ERROR  0

// variable
GRAMMAR *G_start , *G_end;
TOKEN *terminal_start , *terminal_end;
TOKEN *nonterminal_start , *nonterminal_end;
TOKEN *first_set_start , *first_set_end;
TOKEN *follow_set_start , *follow_set_end;
TOKEN *predict_set_start , *predict_set_end;
PARSER **parser_table;
SET *scan_start , *scan_end;
char lambda[3];
int is_lambda;
int num_of_nonterm , num_of_term;
int iterm_index;
FILE *scan , *view;
Config_set *config_start , *config_end;
STATE *state_start , *state_end;
int state_num;
STACK *stack_top;
// scanner
int     COMMENT_MARK = FALSE;
char	*Reserved_word[] = { "void" , "int" , "float" , "if" , "else" , "while" , "for" , "return" ,"break" };
int Check_Reserved_Word(char *string );
int scanner( char *file_name );


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
GRAMMAR *search_grammar( GRAMMAR *search_point , char *string );
int add_set( TOKEN *dst , TOKEN *src );
int free_grammar();
int free_token( TOKEN **free_start );
int free_set( SET **free_start );
int free_state( STATE **free_start );
int free_closure( Config_set **free_start );
char *idtostr( int symbol );
GRAMMAR *idtogram( int symbol );
int is_term( int symbol );
int is_nonterm( int symbol );
Config_set *go_to0( Config_set *Set , TOKEN *X );
int delete_set( SET **set_point , char *string );
int make_id( char *string );
Config_set *create_config_set( GRAMMAR *rule , TOKEN *dot );
void insert_config( Config_set *config , GRAMMAR *rule );
void create_new_state( GRAMMAR *rule , TOKEN *dot );
void build_CFSM();
void view_state( int statenum );
void view_config_set( Config_set *set );
int search_state( Config_set *search_point );
int propagate( int state , Config_set *A , Config_set *B );
int propagate_link( int state , Config_set *A , Config_set *B );
int go_to_state( int from_state , int symbol_id );
int find_state( Config_set *search_point );
void build_LALR_lookahead();
void shift_reduce_driver();
int pop_stack( int pop_num , TTreeNode *parent );
int push_stack( int index , TTreeNode *child );
int count_rhs_num( int rule );
void view_stack();
void shift_reduce_driver( TTreeView *TreeView1 );
int run_parser( TTreeView *TreeView1 , char *file_name );
