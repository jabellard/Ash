
# Ash (Alpha shell) -- version 2.0
## I. Shell Features

### This version of the shell has the following features:
#### 1. Command Line Editing
    1. auto-completion of shell builtin commands
    2. custom command line editing features implemented via the readline library, and
    3. command line history features
#### 2. Pipelining
    1. pipelining of the form c1 | c2 | ... | cn

#### 3. Redirection
    1. standard input redirection
    2. standard output redirection, and 
    3. standard error redirection
	
#### 4. Pipeline Listing
    1. pipeline listing of the form p1; p2; ... ; pn
	
#### 5. Builtin Commands
    The following builtins are currently implemented:
    1. help : display help for the shell.
    2. cd <path-name> : change the current working directory to <path-name>.
    3. exit : exit from the shell.
#### 6. Documentation
    1. man-page-style documentation

## II. Instalation Guide

### To install on Linux, run the following commands:
``` 
	make
	sudo make install
```

## III. Coming Soon

### The following will be implemented in future versions of the shell:
 - [ ] Job Control Sub-system
 - [ ] Program Configuration Sub-system
 - [ ] Globbing, Substitution, and Expansion
 - [x] Error Handling Sub-system
 - [ ] Signal Handling Sub-system


