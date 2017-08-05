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
 
">"     { 
                return GREAT; 
        } 
 
"<"     { 
                return LESS; 
        } 
 

 
“|”    { 
                return PIPE; 
        } 
 
“&”    { 
                return AMPERSAND; 
        } 
 
 
[^ \t\n][^ \t\n]*  { 
                /* Assume that file names have only alpha chars */ 
                yylval.str = strdup(yytext); 
                return WORD; 
        } 
 

 
.       { 
                /* Invalid character in input */ 
                return BAD_TOKEN;
         }

%%
