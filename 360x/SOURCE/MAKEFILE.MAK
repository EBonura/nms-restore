
CC=ccpsx
OPT=-O4 -g -Xo$80010000 -Xn999 -comments-c++
OPTOBJ=-c -O4 -g -Xo$80010000 -comments-c++

incbins.obj:	incbins.s
			asmpsx /zd /l /k incbins.s,incbins.obj








fire.obj:	fire.c fire.h 360x.h
			$(CC) $(OPTOBJ) fire.c -ofire.obj

particle.obj:	particle.c particle.h 360x.h
			$(CC) $(OPTOBJ) particle.c -oparticle.obj

interupt.obj:	interupt.c interupt.h 360x.h
			$(CC) $(OPTOBJ) interupt.c -ointerupt.obj

sprite.obj:	sprite.c sprite.h 360x.h
			$(CC) $(OPTOBJ) sprite.c -osprite.obj


loaddata.obj:	loaddata.c loaddata.h 360x.h
			$(CC) $(OPTOBJ) loaddata.c -oloaddata.obj

control.obj:	control.c control.h 360x.h
			$(CC) $(OPTOBJ) control.c -ocontrol.obj

scroller.obj:	scroller.c scroller.h 360x.h
			$(CC) $(OPTOBJ) scroller.c -oscroller.obj

level.obj:	level.c level.h 360x.h
			$(CC) $(OPTOBJ) level.c -olevel.obj

process.obj:	process.c process.h 360x.h
			$(CC) $(OPTOBJ) process.c -oprocess.obj

sprite2.obj:	sprite2.c sprite2.h 360x.h
			$(CC) $(OPTOBJ) sprite2.c -osprite2.obj


panel.obj:	panel.c panel.h 360x.h
			$(CC) $(OPTOBJ) panel.c -opanel.obj

general.obj:	general.c general.h 360x.h
			$(CC) $(OPTOBJ) general.c -ogeneral.obj



files.obj:	files.c files.h 360x.h
			$(CC) $(OPTOBJ) files.c -ofiles.obj


sound.obj:	sound.c sound.h 360x.h
			$(CC) $(OPTOBJ) sound.c -osound.obj

sprlist.obj:	sprlist.c sprlist.h 360x.h
			$(CC) $(OPTOBJ) sprlist.c -osprlist.obj

jalloc.obj:	jalloc.c jalloc.h 360x.h
			$(CC) $(OPTOBJ) jalloc.c -ojalloc.obj







title.obj:	title.c title.h 360x.h
			$(CC) $(OPTOBJ) title.c -otitle.obj

options.obj:	options.c options.h 360x.h
			$(CC) $(OPTOBJ) options.c -ooptions.obj

language.obj:	language.c language.h 360x.h
			$(CC) $(OPTOBJ) language.c -olanguage.obj

shipdemo.obj:	shipdemo.c shipdemo.h 360x.h
			$(CC) $(OPTOBJ) shipdemo.c -oshipdemo.obj

planets.obj:	planets.c planets.h 360x.h
			$(CC) $(OPTOBJ) planets.c -oplanets.obj

high.obj:	high.c high.h 360x.h
			$(CC) $(OPTOBJ) high.c -ohigh.obj

credits.obj:	credits.c credits.h 360x.h
			$(CC) $(OPTOBJ) credits.c -ocredits.obj

teletype.obj:	teletype.c teletype.h 360x.h
			$(CC) $(OPTOBJ) teletype.c -oteletype.obj

loadsave.obj:	loadsave.c loadsave.h 360x.h
			$(CC) $(OPTOBJ) loadsave.c -oloadsave.obj









main.obj:	incbins.obj \



			fire.obj \
			particle.obj \
			interupt.obj \
			sprite.obj \
			control.obj \
			scroller.obj \
			level.obj \
			process.obj \
			sprite2.obj \

			loaddata.obj \
			panel.obj \
			general.obj \
			files.obj \
			sound.obj \
			sprlist.obj \
			jalloc.obj \


			title.obj \
			options.obj \
			language.obj \
			shipdemo.obj \
			planets.obj \
			high.obj \
			credits.obj \
			teletype.obj \
			loadsave.obj \



			main.c

			$(CC) $(OPT) @objlist.cmd main.c -omain.cpe,main.sym

all:
		cls
		psymake main.obj
		-run /w9 main

