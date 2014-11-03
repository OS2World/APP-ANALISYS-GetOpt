PROJ	=GETOPT
DEBUG	=1
CC	=qcl
CFLAGS_G	= /AL /W4 /Ze /DANGLAIS 
CFLAGS_D	= /Zi /Zr /Od 
CFLAGS_R	= /O /Ot /DNDEBUG 
CFLAGS	=$(CFLAGS_G) $(CFLAGS_D)
LFLAGS_G	= /CP:0xfff /NOI /SE:0x80 /ST:0x1388 
LFLAGS_D	= /CO /M 
LFLAGS_R	= 
LFLAGS	=$(LFLAGS_G) $(LFLAGS_D)
RUNFLAGS	=/i1 /row0,5,2,1 @@
OBJS_EXT = 	
LIBS_EXT = 	

.asm.obj: ; $(AS) $(AFLAGS) -c $*.asm

all:	$(PROJ).EXE

getopt.obj:	getopt.c $(H)

testopt.obj:	testopt.c $(H)

$(PROJ).EXE:	getopt.obj testopt.obj $(OBJS_EXT)
	echo >NUL @<<$(PROJ).crf
getopt.obj +
testopt.obj +
$(OBJS_EXT)
$(PROJ).EXE

$(LIBS_EXT);
<<
	ilink -a -e "qlink $(LFLAGS) @$(PROJ).crf" $(PROJ)

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)

