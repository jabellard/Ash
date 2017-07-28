
%{
#include "file-lexer.h"
%}


%%

"mode"			return MODE;
"keymap"		return KEYMAP;
"history_size"		return HISTORY_SIZE;
:			return COLON;

[a-zA-Z][_a-zA-Z0-9]*	return STRINGG;
[1-9][0-9]*		return INTT;
[ \n\t]			;
.			printf("unexpted input\n");
%% 
// define this function so that the lexer can be used in another program
int yywrap(void)
{
	return 1;
}
