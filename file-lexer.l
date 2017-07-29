%{
#include "y.tab.h"
%}

%%
"mode"			{yylval.id = yytext; return identifier;}
"keymap"		{yylval.id = yytext; return identifier;}
"history_size"		{yylval.id = yytext; return identifier;}
[0-9]+			{yylval.num = atoi(yytext); return number;}		
[a-zA-Z][_a-zA-Z0-9]*	{yylval.id = yytext; return identifier;}
[ \t\n]			;
[=;]			{return yytext[0];}
.			{ECHO, yyerror("unexpected input");}
%%

int yywrap()
{

	return 1;
}
