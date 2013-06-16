//
//  scanner.h
//  scanner
//
//  Created by Kent Huang on 13/6/16.
//
//

#ifndef scanner_scanner_h
#define scanner_scanner_h

#define             TRUE       1
#define             FAULT      0

#define RESERVED_WORDS  {\
    "void",\
    "int",\
    "float",\
    "if",\
    "else",\
    "while",\
    "for",\
    "return",\
    "break",\
}

void usageDisplay(char *projectName);

int Find_Comments(char **string );  // fine /*  ...  */
int Find_Comments2(char **string ); // fine //
int Check_Reserved_Word(char *string );
char *strupr( char *s );

#endif
