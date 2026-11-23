########################################################################
# PsyMake file for Saturn 360x					       #	
########################################################################

CC = ccsh @ccshrc
AS = asmsh 
LD = psylink
AR = ar

ASFLAGS	=/l /i /zd /x /d /k /g

LDFLAGS	= /c /d /i /m /v

INCS = includes/globals.h includes/defines.h makefile.mak includes/gamedefs.h

########################################################################
########################################################################

360x.cpe:	workarea.obj \
		main.obj \
		bupram/bupram.obj \
		shcode/address.o \
		logic/random.obj \
		vcd/file_io.obj \
		vdp/intro1.obj \
		vdp/intro2.obj \
		vdp/vdp1_1.obj \
		vdp/vdp1_2.obj \
		vdp/vdp1_3.obj \
		vdp/vdp2_1.obj \
		logic/process.obj \
		logic/level.obj  \
		logic/sprite.obj \
		logic/control.obj \
		logic/particle.obj \
		logic/fire.obj \
		pad/readpad.obj \
		shcode/maths.o \
		shcode/bit2raw.o \
		shcode/runlen.o \
		shcode/main.o \
		shcode/memset.o \
		shcode/scroll.o \
		game.obj
		
		psylink $(LDFLAGS) @overlay.lnk,360x.cpe,360x.sym,360x.map

########################################################################
# The main code stub object files come first ###########################
########################################################################

main.obj: main.c $(INCS)
	$(CC) main.c -c -o main.obj

workarea.obj: workarea.c
	$(CC) workarea.c -c -o workarea.obj

shcode/address.o: shcode/address.s
	$(AS) $(ASFLAGS) shcode/address.s,shcode/address.o,shcode/address.sym

bupram/bupram.obj:  bupram/bupram.c $(INCS)
	$(CC) bupram/bupram.c -c -o bupram/bupram.obj

logic/random.obj: logic/random.c $(INCS)
	$(CC) logic/random.c -c -o logic/random.obj

vcd/file_io.obj: vcd/file_io.c $(INCS)
	$(CC) vcd/file_io.c -c -o vcd/file_io.obj

########################################################################
# The intro code #######################################################
########################################################################

vdp/intro1.obj:	vdp/intro1.c $(INCS)
	$(CC) -Wa,sl1 vdp/intro1.c -c -o vdp/intro1.obj
	
vdp/intro2.obj:	vdp/intro2.c $(INCS)
	$(CC) -Wa,sl1 vdp/intro2.c -c -o vdp/intro2.obj

########################################################################
# The game code ########################################################
########################################################################

game.obj: game.c $(INCS)
	$(CC) -Wa,sl2 game.c -c -o game.obj

pad/readpad.obj: pad/readpad.c $(INCS)
	$(CC) -Wa,sl2 pad/readpad.c -c -o pad/readpad.obj
	
vdp/vdp2_1.obj:	vdp/vdp2_1.c $(INCS)
	$(CC) -Wa,sl2 vdp/vdp2_1.c -c -o vdp/vdp2_1.obj
	
vdp/vdp1_1.obj:	vdp/vdp1_1.c $(INCS)
	$(CC) -Wa,sl2 vdp/vdp1_1.c -c -o vdp/vdp1_1.obj
	
vdp/vdp1_2.obj:	vdp/vdp1_2.c $(INCS)
	$(CC) -Wa,sl2 vdp/vdp1_2.c -c -o vdp/vdp1_2.obj
	
vdp/vdp1_3.obj:	vdp/vdp1_3.c $(INCS)
	$(CC) -Wa,sl2 vdp/vdp1_3.c -c -o vdp/vdp1_3.obj
	
logic/process.obj: logic/process.c $(INCS)
	$(CC) -Wa,sl2 logic/process.c -c -o logic/process.obj
	
logic/level.obj: logic/level.c $(INCS)
	$(CC) -Wa,sl2 logic/level.c -c -o logic/level.obj
	
logic/sprite.obj : logic/sprite.c $(INCS)
	$(CC) -Wa,sl2 logic/sprite.c -c -o logic/sprite.obj
	
logic/control.obj: logic/control.c $(INCS)
	$(CC) -Wa,sl2 logic/control.c -c -o logic/control.obj
	
logic/particle.obj: logic/particle.c $(INCS)
	$(CC) -Wa,sl2 logic/particle.c -c -o logic/particle.obj
	
logic/fire.obj:	logic/fire.c $(INCS)
	$(CC) -Wa,sl2 logic/fire.c -c -o logic/fire.obj
	
logic/nfire.obj: logic/nfire.c $(INCS)
	$(CC) -Wa,sl2 logic/nfire.c -c -o logic/nfire.obj


########################################################################
# SH2 Code  ############################################################
########################################################################

shcode/maths.o: shcode/maths.s
	$(AS) $(ASFLAGS) shcode/maths.s,shcode/maths.o,shcode/maths.sym

shcode/bit2raw.o: shcode/bit2raw.s
	$(AS) $(ASFLAGS) shcode/bit2raw.s,shcode/bit2raw.o,shcode/bit2raw.sym

shcode/runlen.o: shcode/runlen.s
	$(AS) $(ASFLAGS) shcode/runlen.s,shcode/runlen.o,shcode/runlen.sym

shcode/main.o: shcode/main.s
	$(AS) $(ASFLAGS) shcode/main.s,shcode/main.o,shcode/main.sym

shcode/memset.o: shcode/memset.s
	$(AS) $(ASFLAGS) shcode/memset.s,shcode/memset.o,shcode/memset.sym

shcode/scroll.o: shcode/scroll.s
	$(AS) $(ASFLAGS) shcode/scroll.s,shcode/scroll.o,shcode/scroll.sym

