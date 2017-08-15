all:

	gcc -o Ash main.c cle.c builtins.c exec.c y.tab.c lex.yy.c -lreadline

install:
	cp Ash /usr/local/bin

clean:

