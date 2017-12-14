BASE = /local.tom/optpls
WISH = /s/std/bin/wish


#### You probably don't need to change anything down here ###

A_APG = $(BASE)/search
A_OPT = $(BASE)/sampleopt
A_QUERY = $(BASE)/parser
A_TCL = $(BASE)/tclstuff



CC = g++
MACHINE_FLAG = -DA_SPARC
GPROF_FLAGS = 
CFLAGS = -g $(INCLUDES) $(MACHINE_FLAG) $(GPROF_FLAGS)
MAKE_DEPEND_FILE = Makefile.depend
MAKECMD = $(MAKE) 'GPROF_FLAGS=$(GPROF_FLAGS)'
TAGS = etags --c++ 
AR = ar
ARFLAGS = ruv
RANLIB = ranlib
LIB_QPROC = $(BASE)/libqproc.a

MAKE_DEPEND_COMMAND = $(CC) $(CFLAGS) -M $(SRCS) > $(MAKE_DEPEND_FILE)

INCLUDES = -I$(A_APG) -I$(A_OPT) -I$(A_QUERY)
