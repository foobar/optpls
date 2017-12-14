default : all

include ../Config.make

all :
	echo "#!$(WISH)" > showplan
	echo "set optplsdir $(BASE)" >> showplan
	cat showplan.tmpl >> showplan
	chmod +x showplan


