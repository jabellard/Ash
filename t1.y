%{
#include <stdio.h>
int yylex(void);
void yyerror(char *);
static int i = 0;
//extern YY_BUFFER_STATE;
%}

%union
{

char * s;
int i;

}
//%token <i> INTEGER
%token <s> WORD
%token PIPE NEWLINE

%start item_list3

%%

item:
WORD {

printf("an item: %s\n", $1);
}
;

item_list:
 item 
{

printf("1A\n");
}
|
item ','
{
printf("1B\n");
}

;

item_list2:
item_list2 item_list
{
printf("2A\n");
}
|
item_list
{

printf("2B\n");
}
;

item_list3:
item_list2 NEWLINE
{
printf("start\n");

}
|
;
%%

void yyerror(char *s) {
fprintf(stderr, "%s\n", s);
}

int main(void) {
// read input from a string
//YY_BUFFER_STATE *bp;
struct yy_buffer_state *bp;

char *input = "aa , bb , cc, dd\n\0";

//  connect input buffer to specified string
bp = yy_scan_string(input);

// read from the buffer
yy_switch_to_buffer(bp);


// parse 
yyparse();

// delete the buffer
yy_delete_buffer(bp);

// delete the string (or not)


return 0;
}
