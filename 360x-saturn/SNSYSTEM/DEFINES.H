//--------------------------------------------------------------------------------------------------------------------------------
// Saturn stuff
//--------------------------------------------------------------------------------------------------------------------------------

// I must start commenting this file !!

//--------------------------------------------------------------------------------------------------------------------------------

#define	myPAL		COL_16

//--------------------------------------------------------------------------------------------------------------------------------

#define	MENUMODE		-1
#define	RESUME		0
#define	GAMMA			1
#define	CD_VOLUME	2
#define	SFX_VOLUME	3

//--------------------------------------------------------------------------------------------------------------------------------

#define	BlockSize		(16*16)

#define	NORM				0
#define	ROTATE			1

#define	MAX_ACTIVE		1024

//--------------------------------------------------------------------------------------------------------------------------------

#define	DOFADEUP	r=-240;g=-240;b=-240;slColOffsetA(r,g,b);FadeFlag	=	FADEUP

#define	DOFADEDOWN r=0;g=0;b=0;slColOffsetA(r,g,b);FadeFlag	=	FADEDOWN

#define	DOFADEDOWNCC r=-120;g=-120;b=-120;slColOffsetA(r,g,b);FadeFlag	=	FADEDOWN

#define	WAITFADE	while(FadeFlag!=NOFADE) \
			{											\
				slSynch();							\
			}

//--------------------------------------------------------------------------------------------------------------------------------

#define	PAUSED			1


#define	TITLE				0
#define	SELECT			1
#define	OPTION			2
#define	HANDLEOPTION	3
#define	CONTROL			4
#define	HISCORE			5
#define	CREDITSCREEN	6
#define	ENTERNAME		7
#define	BRIEF				8
#define	PLAYGAME			9

//--------------------------------------------------------------------------------------------------------------------------------

#define	FADEUP			0
#define	FADEDOWN 		1
#define	NOFADE			2

//--------------------------------------------------------------------------------------------------------------------------------

#define	LONG	long
#define	SHORT	short

//--------------------------------------------------------------------------------------------------------------------------------

#define	PCLP_DISABLE	0x0800
#define	HSS_ENABLE		0x1000

//--------------------------------------------------------------------------------------------------------------------------------

//dimensions of the scroll plane
#define	SCR_PLANEX		1024
#define	SCR_PLANEY		256

//--------------------------------------------------------------------------------------------------------------------------------

//Area size to load sprites
#define	LoadSize			500*1024

//--------------------------------------------------------------------------------------------------------------------------------

//Some addresses that are used
#define	SpriteStart		(CGADDRESS)
#define	RamLow			0x00200000
#define	LowRam			(void *)0x00200000
#define	MapArea			(void *)0x002e1000
#define	MapData			(void *)(0x002e1000+4)

//--------------------------------------------------------------------------------------------------------------------------------

//scroll increment
#define	STEPY				4
#define	STEPX				4

//--------------------------------------------------------------------------------------------------------------------------------

//ship increment
#define	SHIPXINC			2
#define	SHIPYINC			2	

//--------------------------------------------------------------------------------------------------------------------------------

#define	SCREEN_WIDTH	320
#define	SCREEN_HEIGHT	240

#define	MINX				(0)		
#define	MAXX				(MapWidth-SCREEN_WIDTH)
#define	MINY				(0)
#define	MAXY				(MapHeight-SCREEN_HEIGHT)

//--------------------------------------------------------------------------------------------------------------------------------

//flags for scroll update
#define	UPDATETOP		1
#define	UPDATEBOTTOM	2

#define	UPDATELEFT		1
#define	UPDATERIGHT		2

//--------------------------------------------------------------------------------------------------------------------------------

//Screen boundaries
#define	MINSCRX			24
#define	MAXSCRY			240-24

#define	MINSCRY			24
#define	MAXSCRX			320-24

//--------------------------------------------------------------------------------------------------------------------------------

//debug macros
#define	DbCol(r,g,b) 			slBack1ColSet((Uint16 *)VDP2_VRAM_B1+0x1fffe,C_RGB(r,g,b))

//--------------------------------------------------------------------------------------------------------------------------------

//Stuff
#define	px	COL_256

#define olTEXDEF(h,v,presize)		{h,v,(cgaddress+(((presize)*4)>>(px)))/8,(((h)&0x1f8)<<5 | (v))}

#define myTEXDEF(h,v,presize)    {h,v,(cgaddress+(((presize)*4)>>(COL_256)))/8,(((h)&0x1f8)<<5 | (v))}

#define TEXDEF16(h,v,presize)    {h,v,(cgaddress+(((presize)*4)>>(COL_256)))/8,(((h)&0x1f8)<<5 | (v))}

//--------------------------------------------------------------------------------------------------------------------------------

/*

Description of macros:-

#define	Numnxn			-	number of slots allocated in VRAM for this size of sprite
#define	Spritenxn		-	first number of sprite in the size for display i.e. texno
#define	Startnxn			-	address of first slot for this sprite. Add to buffer to get offset
#define	MemSizenxn		-	how much space the slots take up in VRAM
#define	Endnxn			-	end address of slots for this sprite size

-	First sprite number (ie. texno ) starts at ZERO!!

*/

//--------------------------------------------------------------------------------------------------------------------------------

#define	Num8x8			20
#define	Sprite8x8 		0
#define	Start8x8			(SpriteStart)
#define	MemSize8x8 		((Num8x8*(8*8))/2)
#define	End8x8			(Start8x8+MemSize8x8)				

//--------------------------------------------------------------------------------------------------------------------------------

#define	Num16x16			20
#define	Sprite16x16 	(Sprite8x8+Num8x8)
#define	Start16x16		(End8x8)
#define	MemSize16x16 	((Num16x16*(16*16))/2)
#define	End16x16			(Start16x16+MemSize16x16)				

//--------------------------------------------------------------------------------------------------------------------------------

#define	Num24x24			30
#define	Sprite24x24 	(Sprite16x16+Num16x16)
#define	Start24x24		(End16x16)
#define	MemSize24x24 	((Num24x24*(24*24))/2)
#define	End24x24			(Start24x24+MemSize24x24)				

//--------------------------------------------------------------------------------------------------------------------------------

#define	Num32x32			20
#define	Sprite32x32		(Sprite24x24+Num24x24)
#define	Start32x32		(End24x24)
#define	MemSize32x32 	((Num32x32*(32*32))/2)
#define	End32x32			(End24x24+MemSize32x32)				

//--------------------------------------------------------------------------------------------------------------------------------

#define	Num48x48			20
#define	Sprite48x48		(Sprite32x32+Num32x32)
#define	Start48x48		(End32x32)
#define	MemSize48x48	((Num48x48*(48*48))/2)
#define	End48x48			(End32x32+MemSize48x48)				

//--------------------------------------------------------------------------------------------------------------------------------

#define	Num64x64			20
#define	Sprite64x64		(Sprite48x48+Num48x48)
#define	Start64x64		(End48x48)
#define	MemSize64x64	((Num64x64*(64*64))/2)
#define	End64x64			(End48x48+MemSize64x64)				

//--------------------------------------------------------------------------------------------------------------------------------

#define	Num80x80			20
#define	Sprite80x80		(Sprite64x64+Num64x64)
#define	Start80x80		(End64x64)
#define	MemSize80x80	((Num80x80*(80*80))/2)
#define	End80x80			(End64x64+MemSize80x80)				

//--------------------------------------------------------------------------------------------------------------------------------

#define	Num96x96			10
#define	Sprite96x96		(Sprite80x80+Num80x80)
#define	Start96x96		(End80x80)
#define	MemSize96x96	((Num96x96*(96*96))/2)
#define	End96x96			(End80x80+MemSize96x96)

//--------------------------------------------------------------------------------------------------------------------------------

#define	Num128x128		10
#define	Sprite128x128	(Sprite96x96+Num96x96)
#define	Start128x128	(End96x96)
#define	MemSize128x128	((Num128x128*(128*128))/2)
#define	End128x128		(End96x96+MemSize128x128)

//--------------------------------------------------------------------------------------------------------------------------------

#define	PARTICLESTART	(End128x128)
#define	PARTICLESPRITE	(Sprite128x128+Num128x128)
#define	PARTICLESIZE	((8*8)/2)
#define	PARTICLEEND		(End128x128+PARTICLESIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	BORDERSTART		(PARTICLEEND)
#define	BORDERSPRITE	(PARTICLESPRITE+1)
#define	BORDERSIZE		((56*68)/2)
#define	BORDEREND		(PARTICLEEND+BORDERSIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	SHIPWIRESTART	(BORDEREND)
#define	SHIPWIRESPRITE	(BORDERSPRITE+1)
#define	SHIPWIRESIZE	((24*16)/2)
#define	SHIPWIREEND		(BORDEREND+SHIPWIRESIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	MISSWIRESTART	(SHIPWIREEND)
#define	MISSWIRESPRITE	(SHIPWIRESPRITE+1)
#define	MISSWIRESIZE	((24*14)/2)
#define	MISSWIREEND		(SHIPWIREEND+MISSWIRESIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	BARSLSTART		(MISSWIREEND)
#define	BARSLSPRITE		(MISSWIRESPRITE+1)
#define	BARSLSIZE		((16*27)/2)
#define	BARSLEND			(MISSWIREEND+BARSLSIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	BARSRSTART		(BARSLEND)
#define	BARSRSPRITE		(BARSLSPRITE+1)
#define	BARSRSIZE		((16*27)/2)
#define	BARSREND			(BARSLEND+BARSLSIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL1START		(BARSREND)
#define	VIL1SPRITE		(BARSRSPRITE+1)
#define	VIL1SIZE			((72*92)/2)
#define	VIL1END			(BARSREND+VIL1SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL2START		(VIL1END)
#define	VIL2SPRITE		(VIL1SPRITE+1)
#define	VIL2SIZE			((64*46)/2)
#define	VIL2END			(VIL1END+VIL2SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL3START		(VIL2END)
#define	VIL3SPRITE		(VIL2SPRITE+1)
#define	VIL3SIZE			((56*101)/2)
#define	VIL3END			(VIL2END+VIL3SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL4START		(VIL3END)
#define	VIL4SPRITE		(VIL3SPRITE+1)
#define	VIL4SIZE			((56*63)/2)
#define	VIL4END			(VIL3END+VIL4SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL5START		(VIL4END)
#define	VIL5SPRITE		(VIL4SPRITE+1)
#define	VIL5SIZE			((48*66)/2)
#define	VIL5END			(VIL4END+VIL5SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL6START		(VIL5END)
#define	VIL6SPRITE		(VIL5SPRITE+1)
#define	VIL6SIZE			((40*26)/2)
#define	VIL6END			(VIL5END+VIL6SIZE)

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

#define	BIGFONTNUM		64
#define	BIGFONTSTART	(VIL6END)
#define	BIGFONTSPRITE	(VIL6SPRITE+1)
#define	BIGFONTSIZE		(BIGFONTNUM*(16*16))
#define	BIGFONTEND		(VIL6END+BIGFONTSIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	MEDFONTNUM		64
#define	MEDFONTSTART	(BIGFONTEND)
#define	MEDFONTSPRITE	(BIGFONTSPRITE+BIGFONTNUM)
#define	MEDFONTSIZE		(MEDFONTNUM*(16*16))
#define	MEDFONTEND		(BIGFONTEND+MEDFONTSIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	SMLFONTNUM		64
#define	SMLFONTSTART	(MEDFONTEND)
#define	SMLFONTSPRITE	(MEDFONTSPRITE+MEDFONTNUM)
#define	SMLFONTSIZE		(SMLFONTNUM*(16*16))
#define	SMLFONTEND		(MEDFONTEND+SMLFONTSIZE)

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

#define	MAXSPRITENUM	(SMLFONTSPRITE+SMLFONTNUM)

#define	DATAEND			SMLFONTEND

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

//Clever stuff to align the Cluts onto a 32 byte boundary
#define	bound				(DATAEND / 32)
#define	diff				(32 * bound)
#define	diffp 			(DATAEND - diff)
#define	res				(32-diffp)

#define	PARTICLECLUT	(DATAEND+res)
#define	CLUTSTART		(PARTICLECLUT+32)
#define	CLUTEND			(CLUTSTART+(MAXSPRITENUM*32))

#define	FONTCLUT			(CLUTEND-(32*11))

#define	VIL1CLUT			(CLUTEND-(32*10))
#define	VIL2CLUT			(CLUTEND-(32*9))
#define	VIL3CLUT			(CLUTEND-(32*8))
#define	VIL4CLUT			(CLUTEND-(32*7))
#define	VIL5CLUT			(CLUTEND-(32*6))
#define	VIL6CLUT			(CLUTEND-(32*6))

#define	BORDERCLUT		(CLUTEND-(32*5))
#define	SHIPWIRECLUT	(CLUTEND-(32*4))
#define	MISSWIRECLUT	(CLUTEND-(32*3))
#define	BARSLCLUT		(CLUTEND-(32*2))
#define	BARSRCLUT		(CLUTEND-(32*1))

//Maximum number of GFX transfers that can take place in VBlank (ha!)
#define	MAX_TRANS		MAXSPRITENUM

#define	VRAMUSED		  (MemSize8x8) \
							  + (MemSize16x16) \
							  + (MemSize24x24) \
                       + (MemSize32x32) \
                       + (MemSize48x48) \
                       + (MemSize64x64) \
                       + (MemSize80x80) \
                       + (MemSize96x96) \
							  + (MemSize128x128) \
                       + (BORDERSIZE + SHIPWIRESIZE + MISSWIRESIZE + (MAXSPRITENUM*32) + PARTICLESIZE) \
                       + (VIL1SIZE + VIL2SIZE + VIL3SIZE + VIL4SIZE + VIL5SIZE + VIL6SIZE) \
                       + (BIGFONTSIZE + MEDFONTSIZE + SMLFONTSIZE)

#define	Planet0Start		Start128x128
#define	Planet0Sprite		Sprite128x128

#define	Planet1Start		Start80x80
#define	Planet1Sprite		Sprite80x80


//--------------------------------------------------------------------------------------------------------------------------------
//	CONSTANTS
//--------------------------------------------------------------------------------------------------------------------------------

#define 	TRUE			1
#define 	FALSE			0

#define 	OK				1
#define 	FAIL			0

#define 	YES			1
#define	NO				0
#define 	NEITHER		99

#define	RUNLENGTH	0
#define	BITSTREAM	1


//--------------------------------------------------------------------------------------------------------------------------------
//	Color change MACROS
//--------------------------------------------------------------------------------------------------------------------------------

#define	FONTCOL(r,g,b)		*(((Uint16 *)VDP2_COLRAM+(256*3)+1))	=	C_RGB(r,g,b)

#define	SHIPCOL(r,g,b)		*((Uint16 *)(SpriteVRAM+(SHIPWIRECLUT)+2))	=	C_RGB(r,g,b)

