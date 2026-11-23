 ##### Macros

CC = ccpsx
ASM = asmpsx
LINK = psylink

CFLAGS = -O0 -Wall -comments-c++ -w -c -g -DPSX
##CFLAGS = -Wall -comments-c++ -w -c -g -O3 -DPSX
LFLAGS = /c /m /g /i
AFLAGS = /l /zd /oC+

INCS =  globals.h types.h protos.h psx.h common.h

OBJS =	pool.obj gfx.obj file_io.obj game.obj my3ds.obj gametype.obj \
	ball.obj rules.obj stv3d.obj ai.obj scrdrws.obj menu.obj \
	code.obj ram.obj models.obj psx.obj common.obj engine.obj \
	pad.obj sounds.obj link.obj resource.obj escher.obj sgl.obj

##### Build main.cpe
				  
pool: $(OBJS)
	$(LINK) $(LFLAGS) @pool.lnk,pool.cpe,pool.sym,pool.map

##### Build objects

pool.obj:	pool.c $(INCS)
gfx.obj:	gfx.c $(INCS)
psx.obj:	psx.c $(INCS)
file_io.obj:    file_io.c $(INCS)
game.obj:	game.c $(INCS)
my3ds.obj:	my3ds.c $(INCS)
models.obj:	models.c $(INCS)
gametype.obj:	gametype.c leveldat.c $(INCS)
ball.obj:	ball.c $(INCS)
rules.obj:	rules.c $(INCS)
stv3d.obj:	stv3d.c $(INCS)
ai.obj:		ai.c $(INCS)
scrdrws.obj:	scrdrws.c $(INCS)
engine.obj:	engine.c $(INCS)
menu.obj:	menu.c $(INCS)
ram.obj:	ram.c $(INCS)
pad.obj:	pad.c $(INCS)
common.obj:	common.c $(INCS)
sounds.obj:	sounds.c $(INCS)
link.obj:	link.c $(INCS)
resource.obj:	resource.c $(INCS)
sgl.obj:	sgl.s
code.obj:	code.s
escher.obj:	escher.s

##### Implicit rules

.c.obj:
	$(CC) $(CFLAGS) $*.c -c -o $*.obj

.s.obj:
	$(ASM) $(AFLAGS) $&.s,$&.obj,$&.sym

##### Clean

clean:
	del *.cpe
	del *.obj
	del *.obd
	del *.sym
	del *.map
	del *.err
