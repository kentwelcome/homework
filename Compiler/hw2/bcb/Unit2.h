//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Dialogs.hpp>
#include <Menus.hpp>

//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published:	// IDE-managed Components
	TTreeView *TreeView1;
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *Loadgrammarfile1;
	TOpenDialog *OpenDialog1;	
	TButton *Button1;
	TMenuItem *Loadsourcecode1;
	TOpenDialog *OpenDialog2;
	TLabel *Label1;

	void __fastcall Loadgrammarfile1Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Loadsourcecode1Click(TObject *Sender);

private:	// User declarations
public:		// User declarations

	__fastcall TForm2(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
//---------------------------------------------------------------------------

typedef struct grammar{
        
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
	TTreeNode *parent;
    struct stack *next;
} STACK;

typedef struct parser {
	int rule;
	int special_tag;
} PARSER;

#define TRUE   1
#define FALSE  0

// variable
GRAMMAR *G_start , *G_end;
TOKEN *terminal_start , *terminal_end;
TOKEN *nonterminal_start , *nonterminal_end;
TOKEN *first_set_start , *first_set_end;
TOKEN *follow_set_start , *follow_set_end;
TOKEN *predict_set_start , *predict_set_end;
PARSER parser_table[200][200];
STACK *stack_top;
SET *scan_start , *scan_end;
char lambda[3];
int is_lambda;
int num_of_nonterm , num_of_term;
int iterm_index;
FILE *scan;
FILE *input ,*out;
//FILE *gen_code;

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
void lldiver( TTreeView *TTreeView1 , TLabel *Label1  );
int view_stack();
int push_stack( char *string , TTreeNode *parent );
int pop_stack( int pop_num );
int is_empty();
GRAMMAR *idtogram( int symbol );
char *idtostr( int symbol );
int delete_set( SET **set_point , char *string );
int make_id( char *string );
int is_nonterm( int symbol );
int is_term( int symbol );
int stack_empty();



#endif
