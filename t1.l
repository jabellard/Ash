%{ 
 
#include <string.h> 
#include "y.tab.h" 
 
%} 

%%


\n      { 
                return NEWLINE; 
 
        } 
 
[ \t]   { 
                /* Discard spaces and tabs */ 
        } 
 

“|”    { 
                return PIPE; 
        } 
 

 
[a-zA-Z][a-zA-Z0-9]*  { 
                /* Assume that file names have only alpha chars */ 
                yylval.s = strdup(yytext); 
                return WORD; 
        } 
 

 
.       { 
                /* Invalid character in input */ 
                //return BAD_TOKEN;
         }

%%
int yywrap(void) {
return 1;
}


