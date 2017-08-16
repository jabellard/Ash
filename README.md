# Ash

Ash -- version 2.0
I. Shell Features

This version of the shell has the following features
(1) Command line editing
	(1a) auto-completion of shell builtin commands
	(1b) custom command line editing features implemented via the readline library, and
	(1c) command line history features
(2) Pipelining
	(2a) pipelining of the form c1 | c2 | ... | cn

(3) Redirection
	(3a) standard input redirection
	(3b) standard output redirection, and 
	(3c) standard error redirection
	
(4) Pipeline list
	(4a) pipeline listing of the form p1; p2; ... ; pn
	
(5) Builtin commands
	(5a) the following builtins are currently implemented
	       	(5a.1) help : display help for the shell.
       		(5a.2) cd <path-name> : change the current working directory to <path-name>.
       		(5a.3) exit : exit from the shell.
(6) Documentation
	(6a) man-page-style documentation

II. Instalation guide

To install on Linux, run the following commands:
(1) make
(2) sudo make install

III. Coming Soon

The following will be implemented in future versions of the shell:
(1) job control sub-system
(2) program configuration sub-system
(3) globbing, substitution, and expansion
(4) error handling sub-system
(5) signal handling sub system

