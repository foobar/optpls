SRCS = opt.c main.c
HDRS = Aattr.h Adbrel.h Alogprop.h Aphyprop.h \
       Abitmap.h Afunc.h Amiscops.h Apred.h \
       Acat.h Aglob_vars.h Aopdefs.h Aptree.h \
       Aconsts.h Ajoin.h Aopinc.h Aquery.h \
       Acost.h Alist.h Aoptions.h Arel.h \
       Aseldesc.h Aselect.h Aset.h Atimer.h

OBJS = $(SRCS:.c=.o)
CLEANABLES = $(OBJS) opt

default : opt

include ../Config.make
include ../Makefile.common

lib :	
	@(echo making lib; cd ..; $(MAKECMD) lib)
	
opt : lib $(LIB_QPROC) main.o
	$(CC) $(CFLAGS) -o $@ main.o $(LIB_QPROC) -lm

opt.pure : lib $(LIB_QPROC) main.o
	$(PURIFY) $(CC) $(CFLAGS) -o $@ main.o $(LIB_QPROC) -lm

profile :
	@(cd ..; make profile)

profilec :
	@(cd ..; make profilec)

profopt : 
	gprof opt-pg > /tmp/profout
	/usr/psup/gcc/bin/c++filt < /tmp/profout > /tmp/gprofout
	

include ./$(MAKE_DEPEND_FILE)
