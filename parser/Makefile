SRCS = query.c
HDRS = Bbogus.h Bexecinfo.h Bextent.h Bprojlist.h Bparsetree.h By.tab.h \
       Bopcodes.h
OBJS = query.o
CLEANABLES = $(OBJS)

default : query.o

include ../Config.make
include ../Makefile.common

YACC = bison --yacc
YFLAGS = -d --no-lines
LEX = flex -L

Bscanner.c : Bscanner.l
Bparser.c : Bparser.y 
	$(YACC) $(YFLAGS) Bparser.y
	mv y.tab.c $@
	mv y.tab.h By.tab.h

include ./$(MAKE_DEPEND_FILE)
