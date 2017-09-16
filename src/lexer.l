%{ 
 
#include <string.h> 
#include <parser.h>
 
%} 

%%


\n      { 
                return NEWLINE; 
 
        } 
 
[ \t]   { 
                /* Discard spaces and tabs */ 
        } 
 
">"     { 
                return GREAT; 
        } 
 
"<"     { 
                return LESS; 
        } 
 

 
"|"    { 
                return PIPE; 
        } 
 
"&"    { 
                return AMPERSAND; 
        } 
        
";"	{
		return SEMICOLON;

	}
 

 
[-_/.a-zA-Z0-9][-_/.a-zA-Z0-9]*  { 
                /*
                [a-zA-Z][a-zA-Z0-9]*  { 
                
                yylval.str = strdup(yytext); 
                return WORD; 
        } 
        */
                yylval.str = strdup(yytext); 
                return ID; 
        } 
 
.       { 
                /* Invalid character in input */ 
                return BAD_TOKEN;
         }

%%
int yywrap(void) {
return 1;
}


