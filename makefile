.PHONY: all install clean
CC = gcc 
FLAGS = -g
OBJS = main.o \
	cle.o \
	job.o \
	y.tab.o \
	lex.yy.o \
	err.o
LIBS = -lreadline
TARGET = Ash
REBUILDABLES = ${OBJS}

all: ${TARGET}

${TARGET}: ${OBJS} ${LIBS}
	@echo "Building..."
	${CC} ${FLAGS} -o $@ $^ ${LIBS}

%.o: %.c
	-${CC} ${FLAGS} -c -o $@ $^


install:

	# copy configuration files in the user's home directory
	cp -r ../.Ash ~/
	
	# compress the man page
	gzip -k  ../doc/Ash.1
	
	# copy the man page to one of the directories search by man
	cp ../doc/Ash.1.gz "/usr/local/man/man1/Ash.1.gz"
	
	# copy the built binary to one of the directories in PATH
	cp ${TARGET} /usr/local/bin

clean:
	# clean up
	@echo "Cleaning up..."
	#rm Ash
	rm -f ${REBUILDABLES}
	#rm ../doc/Ash.1.gz
