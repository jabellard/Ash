%{
#include <stdio.h>
#include <stdlib.h>
void yyerror (char *);
%}
// yacc definitions
%union{int num; char *id;}
%start configuration
%token <num> number
%token <id> identifier
%type <id> configuration setting parameter


%%
configuration	: setting '=' parameter ';' {printf("%s is set to %s\n", $1, $3);}
;

setting		: identifier		{$$ = $1;}
;
parameter	: identifier		{$$ = $1;}
		| number		{$$ = $1;}

;

%%

void yyerror(char *s)
{
	printf("%s\n", s);
} // end yyerror

int main()
{

	return yyparse();
} // end main
