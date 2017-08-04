#include <stdio.h>
#include "exec.h"


int main()
{
	struct simple_command *a = create_simple_command();
	insert_arg_in_simple_command(a, "arg1");
	insert_arg_in_simple_command(a, "arg2");
	insert_arg_in_simple_command(a, "arg3");
	insert_arg_in_simple_command(a, "arg4");
	
	struct simple_command *b = create_simple_command();
	insert_arg_in_simple_command(b, "B1");
	insert_arg_in_simple_command(b, "B2");
	insert_arg_in_simple_command(b, "B3");
	insert_arg_in_simple_command(b, "B4");
	
	//print_simple_command(b);
	
	struct shell_pipeline *s = create_shell_pipeline();
	
	insert_simple_command_in_shell_pipeline(s, a);
		insert_simple_command_in_shell_pipeline(s, a);
				insert_simple_command_in_shell_pipeline(s, b);
								insert_simple_command_in_shell_pipeline(s, b);
								insert_simple_command_in_shell_pipeline(s, b);
								insert_simple_command_in_shell_pipeline(s, b);
								insert_simple_command_in_shell_pipeline(s, b);
								
	// destroy
	destroy_shell_pipeline(s);
	
	print_shell_pipeline(s);
	
				
	

	return 0;
} // end main
