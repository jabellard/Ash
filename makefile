TARGET_OBJECT := Ash
rebuildables = ${TARGET_OBJECT}

# compilation driver variables
CCD := gcc
CCD_FLAGS := -g -Iinclude -Isrc
LIBS := -lreadline

SOURCE_TREE := src
INCLUDE_TREE := include

vpath %.c src
vpath %.y src
vpath %.l src
vpath %.h include src

# list of source files
sources = $(wildcard ${SOURCE_TREE}/*.c)
yacc_source = $(subst .y,.c,$(wildcard ${SOURCE_TREE}/*.y))
sources += ${yacc_source}
#yacc_source += $(subst .y,.h,$(wildcard ${SOURCE_TREE}/*.y))
rebuildables += ${yacc_source}
lex_source = $(subst .l,.c,$(wildcard ${SOURCE_TREE}/*.l))
rebuildables += ${lex_source}
sources += ${lex_source}


# list of object files
objects = $(subst .c,.o,${sources})
rebuildables += ${objects}

# list of external programs
ECHO := echo
CP := cp
GZIP := gzip
RM := rm
YACC := yacc
LEX := lex
SED := sed
MAKE := make
AWK := awk
SORT := sort
PR := pr

.PHONY: all
all: ${TARGET_OBJECT}

${TARGET_OBJECT}: ${objects} ${LIBS}
	@${ECHO} "Linking executable object \"${TARGET_OBJECT}\"..."
	${CCD} ${CCD_FLAGS}  -o $@ $^
	@${ECHO} "Done linking \"${TARGET_OBJECT}\"."	
%.o: %.cc
	@${ECHO} "Building \"$@\" object..."
	${CCD} -c ${CCD_FLAGS}  -o $@ $<
	@${ECHO} "Done building \"$@\"."
	
%.c: %.y
	@${ECHO} "Generating Yacc files \"$*.c\" & \"$*.h\"..."
	${YACC} -d -o $@ $<	
	@${ECHO} "Done generating \"$*.c\" & \"$*.h\"."
%.c: %.l
	@${ECHO} "Generating Lex file \"$*.c\"..."
	${LEX} -o$@ $<
	@${ECHO} "Done generating generating \"$*.c\"."

# automatically manage include dependencies for the source files
include_dependency_files = $(subst .c,.d,${sources})
include ${include_dependency_files}
rebuildables += ${include_dependency_files}

%.d: %.c
	@${ECHO} "Generating include dependency file \"$@\"..."
	$(CCD) -M  ${CCD_FLAGS} $< > $@
	@${ECHO} "Done generating \"$@\"."
.PHONY: install
install:
	@${ECHO} "Copying configuration files to home directory..."
	${CP} -r .Ash ~/
	@${ECHO} "Done copying configuration files to home directory."
	
	@${ECHO} "Compressing manual page..."		
	${GZIP} -k -f doc/Ash.1
	@${ECHO} "Done compressing manual page."
	
	@${ECHO} "Copying compressed man page to a directory searched by \"man\"..."	
	${CP} doc/Ash.1.gz "/usr/local/man/man1/Ash.1.gz"
	@${ECHO} "Done copying compressed man page to a directory searched by \"man\"."	
	
	@${ECHO} "Copying binary executable to a PATH directory..."	
	${CP} ${TARGET_OBJECT} /usr/local/bin
	@${ECHO} "Done copying binary executable to a PATH directory."	

.PHONY: clean
clean:
	@${ECHO} "Removing rebuildable files..."	
	${RM} -f ${rebuildables}
	@${ECHO} "Done removing rebuildable files."	

define display-help
${MAKE} --print-data-base --question | \
${AWK} '/^[^.%][-A-Za-z0-9_]*:/ \
{ print substr($$1, 1, length($$1)-1) }' | \
${SORT} | \
${PR} --omit-pagination --width=80 --columns=4
endef

.PHONY: help
help:
	@${ECHO} "List of targets:\n"	
	@${display-help}

