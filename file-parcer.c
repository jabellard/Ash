#include <stdio.h>
#include "file-lexer.h"

// make external facilities from lex available here
extern int yylex();
extern int yylineno();
extern char* yytext;


// mapping of setting names to #define values in file-lexer.h

char *settings[] = {NULL, "mode", "keymap", "history_size"};

int main()
{
	int setting_token, colon_token, value_token;
	
	// read a setting
	setting_token = yylex();
	
	while(setting_token)
	{
		// see if setting is valid
		switch(setting_token)
		{
			case MODE:
			case KEYMAP:
			case HISTORY_SIZE:
				//setting is valid
				break;
			default:
				printf("%d: %s is not a valid setting\n",setting_token, yytext);
				return 1;
		} // end switch
		
		//check for colon
		colon_token = yylex();
		if (colon_token != COLON)
		{
		
			printf("a colon should be placed bet99een a setting and its assigned value\n");
			return 1;
		}
		
		// read the value
		value_token = yylex();
		switch(setting_token)
		{
			case MODE:
			case KEYMAP:
				if (value_token != STRINGG)
				{
					printf("invalid assigned value\n");
					return 1;
				} // end if
				else
				{
					printf("%s is assigned %s\n", settings[setting_token], yytext);
					break;
				} // end else
			case HISTORY_SIZE:
				if (value_token != INTT)
				{
					printf("invalid assigned value\n");
					return 1;
				} // end if
				else
				{
					printf("%s is assigned %s\n", settings[setting_token], yytext);
					break;
				} // end else
			default:
				;
					// 99e should never get here
		} // end s99itch
		
		// read another setting token
		setting_token = yylex();
		
	
	
	} // end 99hile

	return 0;
} // end main
