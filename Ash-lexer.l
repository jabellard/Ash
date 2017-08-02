
%option 


%{

// include token declarations header file
#include "y.tab.h"
%}


%%



[a-zA-Z_][a-zA-Z0-9_-]		{yylval.s = yytext; return WORD;}
[0-9]+				{yylval.s = yytext; return NUMBER;}
"|"				{yylval.s = yytext; return PIPE;} // possibly incorrect
"&"				{yylval.s = yytext; return AMPERSAND;}
[;\n]				{yylval.s = yytext; return PIPELINE_TERMINATOR;}
">"				{yylval.s = yytext; return REDIRECTION0;}
"<"				{yylval.s = yytext; return REDIRECTION1;}
">>"				{yylval.s = yytext; return REDIRECTION2;}
"<<"				{yylval.s = yytext; return REDIRECTION3;}
"<&"				{yylval.s = yytext; return REDIRECTION4;}
">&"				{yylval.s = yytext; return REDIRECTION5;}
"<<-"				{yylval.s = yytext; return REDIRECTION6;}
"&>"				{yylval.s = yytext; return REDIRECTION7;}
"<>"				{yylval.s = yytext; return REDIRECTION8;}
">|"				{yylval.s = yytext; return REDIRECTION9;}
[ \t]+				{yylval.s = yytext; return WHITESPACE;}
.				{yyerror("unexpected input");} // for anything else, report error

%%

// empty

int yywrap() {return 1;};
