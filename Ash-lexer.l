// options
%option 

// c code block
%{

// include token declarations header file
%}


%%


/*
// tokens
%token WORD
%token WHITESPACE
%token DIGIT
%token PIPE
%token AMPERSAND
%token PIPELINE_TERMINATOR
%token REDIRECTION0 // ">"
%token REDIRECTION1 // "<"
%token REDIRECTION2 // ">>"
%token REDIRECTION3 // "<<"
%token REDIRECTION4 // "<&"
%token REDIRECTION5 // ">&"
%token REDIRECTION6 // "<<-"
%token REDIRECTION7 // "&>"
%token REDIRECTION8 // "<>"
%token REDIRECTION9 // ">|"
*/

// patterns and actions
[a-zA-Z_][a-zA-Z0-9_-]		{return WORD;}
[0-9]+				{return NUMBER;}
"|"				{return PIPE;} // possibly incorrect
"&"				{return AMPERSAND;}
";"|"\n"			{return PIPELINE_TERMINATOR;}
">"				{return REDIRECTION0;}
"<"				{return REDIRECTION1;}
">>"				{return REDIRECTION2;}
"<<"				{return REDIRECTION3;}
"<&"				{return REDIRECTION4;}
">&"				{return REDIRECTION5;}
"<<-"				{return REDIRECTION6;}
"&>"				{return REDIRECTION7;}
"<>"				{return REDIRECTION8;}
">|"				{return REDIRECTION9;}
[ \t]+				{return WHITESPACE;}
.				; // for anything else

%%

// empty
