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
 
">>"     { 
                return GREATGREAT; 
        } 
 
">&"     { 
                return GREATAMPERSAND; 
        } 
 
“|”    { 
                return PIPE; 
        } 
 
“&”    { 
                return AMPERSAND; 
        } 
 
 
[^ \t\n][^ \t\n]*  { 
                /* Assume that file names have only alpha chars */ 
                yylval.string_val = strdup(yytext); 
                return WORD; 
        } 
 
/* Add more tokens here */ 
 
.       { 
                /* Invalid character in input */ 
                return NOTOKEN;
         }

%%
