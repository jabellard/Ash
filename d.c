#include <stdio.h>
#include "exec.h"


int main()
{
	struct simple_command *a = create_simple_command();
	
	insert_arg_in_simple_command(a, "ls");
	insert_arg_in_simple_command(a, "-a");
	insert_arg_in_simple_command(a, "-l");
	//insert_arg_in_simple_command(a, NULL);
	
	struct simple_command *b = create_simple_command();
	
	insert_arg_in_simple_command(b, "awk");
	insert_arg_in_simple_command(b, "{print $1}");
	//insert_arg_in_simple_command(b, NULL);
	//insert_arg_in_simple_command(b, "B4");
	
	
	struct simple_command *c = create_simple_command();
	
	insert_arg_in_simple_command(c, "sort");
	insert_arg_in_simple_command(c, "-r");
	//insert_arg_in_simple_command(c, NULL);
	//insert_arg_in_simple_command(c, "B4");
	//print_simple_command(b);
	
	
	struct shell_pipeline *s = create_shell_pipeline();
	
	insert_simple_command_in_shell_pipeline(s, a);
	//insert_simple_command_in_shell_pipeline(s, b);
	//insert_simple_command_in_shell_pipeline(s, c);
	//insert_simple_command_in_shell_pipeline(s, b);
	
	// test input redirection
	//s->input_file = "~/dev/Ash-exec/in";
	
	s->output_file =  "~/dev/Ash-exec/out";
	
	//s->error_file =  "~/dev/Ash-exec/err";
	
	
	print_shell_pipeline(s);
	
	execute_shell_pipeline(s);
	
				
	

	return 0;
} // end main
