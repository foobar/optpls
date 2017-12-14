default: all 

include ./Config.make

DIRS = $(A_APG) $(A_QUERY) $(A_OPT)
OBJS = $(A_APG)/apg.o $(A_OPT)/opt.o $(A_QUERY)/query.o

lib: apg_objs opt_objs query_objs $(LIB_QPROC)
$(LIB_QPROC): $(OBJS)
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $@

all: startup opt install

startup: depend 

opt: showplan
	@(cd $(A_OPT); $(MAKECMD) opt)

install:
	@mv $(A_OPT)/opt bin/sampleopt
	@mv $(A_TCL)/showplan bin/showplan

showplan:
	@(cd $(A_TCL); $(MAKECMD) all)

apg_objs:
	(cd $(A_APG); $(MAKECMD) apg.o)
opt_objs:
	(cd $(A_OPT); $(MAKECMD) opt.o)
query_objs:
	(cd $(A_QUERY); $(MAKECMD) query.o)
	

depend:
	@for i in $(DIRS); \
	do (cd $$i; \
	  echo making depend in $$i; \
          touch $(MAKE_DEPEND_FILE); \
	  make depend;) || exit 1; \
	done;

clean:
	@for i in $(DIRS); \
	do (cd $$i; \
	  echo making clean in $$i; \
	  make clean;) || exit 1; \
	done;
	rm -f $(LIB_QPROC)
	
profile:
	mv opt/opt opt/opt.tmp
	make clean
	make 'GPROF_FLAGS=-pg' all
	mv opt/opt opt/opt-pg
	make clean
	mv opt/opt.tmp opt/opt

profilec:
	make 'GPROF_FLAGS=-pg' all

tags:
	$(TAGS) */*.[ch]


qproc.tar:
	tar -cvf qproc.tar */*.[chly] 
