#include <stdio.h>
 #include <stdio.h>
  #include <readline/readline.h>
 #include <readline/history.h>
 #include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
 
 
 

 
 /* A static variable for holding the line. */
static char *line_read = (char *)NULL;
/* Read a string, and return a pointer to it.
Returns NULL on EOF. */
char *
rl_gets ()
{
/* If the buffer has already been allocated,
return the memory to the free pool. */
if (line_read)
{
free(line_read);

line_read = (char *)NULL;
}
/* Get a line from the user. */
line_read = readline("Enter line: ");
/* If the line has any text in it,
save it on the history. */
if (line_read && *line_read)
{
	add_history(line_read);
}

return (line_read);
}



/* Invert the case of the COUNT following characters. */
int
invert_case_line (count, key)
int count, key;
{
int direction;
register int start, end, i;
//use of readline variable in custom function
start = rl_point;
if (rl_point >= rl_end)
return (0);
if (count < 0)
{
direction = -1;
count = -count;
}
else
direction = 1;
/* Find the end of the range to modify. */
end = start + (count * direction);

/* Force it to be within range. */
if (end > rl_end)
end = rl_end;
else if (end < 0)
end = 0;
if (start == end)
return (0);
if (start > end)
{
int temp = start;
start = end;
end = temp;
}
/* Tell readline that we are modifying the line,
so it will save the undo information. */
rl_modifying (start, end);
for (i = start; i != end; i++)
{
if (_rl_uppercase_p (rl_line_buffer[i]))
rl_line_buffer[i] = _rl_to_lower (rl_line_buffer[i]);
else if (_rl_lowercase_p (rl_line_buffer[i]))
rl_line_buffer[i] = _rl_to_upper (rl_line_buffer[i]);
}
/* Move point to on top of the last character changed. */
rl_point = (direction == 1) ? end - 1 : start;
return (0);
}
 
 int main()
 {
 	//key binding
 	int result = rl_bind_keyseq("\M-c", &invert_case_line);
 	
 	if (result)
 	{
 		printf("could not bind key\n");
 	}
 	
 	char *line = (char *) NULL;
 
 	int i = 0;
 	
 	for (i; i < 4; i++)
 	{
 		line = rl_gets();
 		printf("recieved: %s", line);
 	} // end for
 	return 0;
 
 } // end main
