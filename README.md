
# Ash (Alpha shell) -- version 2.0
## I. Shell Features

### This version of the shell supports the following features:
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
    1. cd - Change the shell's current working directory.
    2. exit - Exit from the shell.
    3. jobs - List the jobs in the shell's active job list.
    4. fg - Continue an active job in the foreground.
    5. bg - Continue one or more active job(s) in the background.
    6. kill - Sends a signal to one or more active job(s).
    7. killall - Sends a signal to all active jobs of the shell.
#### 6. Job Control
    1. Job control features similar to that of bash.

## II. Instalation Guide

### To install on Linux, run the following commands:
``` 
	cd src
	make
	sudo make install
	[OPTIONAL] make clean
```

## III. Documentation
### 1. Man-page Documentation
    After installation, a man page for the shell is avialable.
### 2. Source Code Documentation
    The source of the shell is fully documented with doxygen. The documentation 
    is avialable at the following link: 

