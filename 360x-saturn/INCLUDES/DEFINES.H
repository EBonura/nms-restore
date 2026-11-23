//--------------------------------------------------------------------------------------------------------------------------------
// Saturn stuff
//--------------------------------------------------------------------------------------------------------------------------------

// I must start commenting this file !!

//--------------------------------------------------------------------------------------------------------------------------------

#define	nUSECD
#define	nMAKECD

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
#define	BlockW			16
#define	BlockH			16

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
#define	UPDATEVERTICAL		1
#define	UPDATEHORIZONTAL	2

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
//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

#define	BIGFONTNUM		64
#define	BIGFONTSTART	(End128x128)
#define	BIGFONTSPRITE	(Sprite128x128+Num128x128)
#define	BIGFONTSIZE		((BIGFONTNUM*(16*16))/2)
#define	BIGFONTEND		(End128x128+BIGFONTSIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	MEDFONTNUM		64
#define	MEDFONTSTART	(BIGFONTEND)
#define	MEDFONTSPRITE	(BIGFONTSPRITE+BIGFONTNUM)
#define	MEDFONTSIZE		((MEDFONTNUM*(16*16))/2)
#define	MEDFONTEND		(BIGFONTEND+MEDFONTSIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	SMLFONTNUM		64
#define	SMLFONTSTART	(MEDFONTEND)
#define	SMLFONTSPRITE	(MEDFONTSPRITE+MEDFONTNUM)
#define	SMLFONTSIZE		((SMLFONTNUM*(16*16))/2)
#define	SMLFONTEND		(MEDFONTEND+SMLFONTSIZE)

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

#define	BORDERSTART		(SMLFONTEND)
#define	BORDERSPRITE	(SMLFONTSPRITE+SMLFONTNUM)
#define	BORDERSIZE		((56*68)/2)
#define	BORDEREND		(SMLFONTEND+BORDERSIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	SHIPWIRESTART	(BORDEREND)
#define	SHIPWIRESPRITE	(BORDERSPRITE+1)
#define	SHIPWIRESIZE	((24*16)/2)
#define	SHIPWIREEND		(BORDEREND+SHIPWIRESIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	BKWIRESTART		(SHIPWIREEND)
#define	BKWIRESPRITE	(SHIPWIRESPRITE+1)
#define	BKWIRESIZE		((16*14)/2)
#define	BKWIREEND		(SHIPWIREEND+BKWIRESIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	CMNWIRESTART	(BKWIREEND)
#define	CMNWIRESPRITE	(BKWIRESPRITE+1)
#define	CMNWIRESIZE		((32*15)/2)
#define	CMNWIREEND		(BKWIREEND+CMNWIRESIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	EBWIRESTART		(CMNWIREEND)
#define	EBWIRESPRITE	(CMNWIRESPRITE+1)
#define	EBWIRESIZE		((16*13)/2)
#define	EBWIREEND		(CMNWIREEND+EBWIRESIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	LZWIRESTART		(EBWIREEND)
#define	LZWIRESPRITE	(EBWIRESPRITE+1)
#define	LZWIRESIZE		((24*12)/2)
#define	LZWIREEND		(EBWIREEND+LZWIRESIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	MISSWIRESTART	(LZWIREEND)
#define	MISSWIRESPRITE	(LZWIRESPRITE+1)
#define	MISSWIRESIZE	((24*14)/2)
#define	MISSWIREEND		(LZWIREEND+MISSWIRESIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	MNWIRESTART		(MISSWIREEND)
#define	MNWIRESPRITE	(MISSWIRESPRITE+1)
#define	MNWIRESIZE		((16*14)/2)
#define	MNWIREEND		(MISSWIREEND+MNWIRESIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	BARSLSTART		(MNWIREEND)
#define	BARSLSPRITE		(MNWIRESPRITE+1)
#define	BARSLSIZE		((16*27)/2)
#define	BARSLEND			(MNWIREEND+BARSLSIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	BARSRSTART		(BARSLEND)
#define	BARSRSPRITE		(BARSLSPRITE+1)
#define	BARSRSIZE		((16*27)/2)
#define	BARSREND			(BARSLEND+BARSLSIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL1START		(BARSREND)
#define	VIL1SPRITE		(BARSRSPRITE+1)
#define	VIL1SIZE			((56*91)/2)
#define	VIL1END			(BARSREND+VIL1SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL2START		(VIL1END)
#define	VIL2SPRITE		(VIL1SPRITE+1)
#define	VIL2SIZE			((32*86)/2)
#define	VIL2END			(VIL1END+VIL2SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL3START		(VIL2END)
#define	VIL3SPRITE		(VIL2SPRITE+1)
#define	VIL3SIZE			((32*50)/2)
#define	VIL3END			(VIL2END+VIL3SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL4START		(VIL3END)
#define	VIL4SPRITE		(VIL3SPRITE+1)
#define	VIL4SIZE			((32*38)/2)
#define	VIL4END			(VIL3END+VIL4SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL5START		(VIL4END)
#define	VIL5SPRITE		(VIL4SPRITE+1)
#define	VIL5SIZE			((16*36)/2)
#define	VIL5END			(VIL4END+VIL5SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL6START		(VIL5END)
#define	VIL6SPRITE		(VIL5SPRITE+1)
#define	VIL6SIZE			((24*101)/2)
#define	VIL6END			(VIL5END+VIL6SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL7START		(VIL6END)
#define	VIL7SPRITE		(VIL6SPRITE+1)
#define	VIL7SIZE			((16*27)/2)
#define	VIL7END			(VIL6END+VIL7SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL8START		(VIL7END)
#define	VIL8SPRITE		(VIL7SPRITE+1)
#define	VIL8SIZE			((32*58)/2)
#define	VIL8END			(VIL7END+VIL8SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL9START		(VIL8END)
#define	VIL9SPRITE		(VIL8SPRITE+1)
#define	VIL9SIZE			((24*36)/2)
#define	VIL9END			(VIL8END+VIL9SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL10START		(VIL9END)
#define	VIL10SPRITE		(VIL9SPRITE+1)
#define	VIL10SIZE		((24*66)/2)
#define	VIL10END			(VIL9END+VIL10SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL11START		(VIL10END)
#define	VIL11SPRITE		(VIL10SPRITE+1)
#define	VIL11SIZE		((24*28)/2)
#define	VIL11END			(VIL10END+VIL11SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL12START		(VIL11END)
#define	VIL12SPRITE		(VIL11SPRITE+1)
#define	VIL12SIZE		((32*44)/2)
#define	VIL12END			(VIL11END+VIL12SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	VIL13START		(VIL12END)
#define	VIL13SPRITE		(VIL12SPRITE+1)
#define	VIL13SIZE		((32*44)/2)
#define	VIL13END			(VIL12END+VIL13SIZE)

//--------------------------------------------------------------------------------------------------------------------------------

#define	PORTDLSTART		(VIL13END)
#define	PORTDLSPRITE	(VIL13SPRITE+1)
#define	PORTDLSIZE		((40*9)/2)
#define	PORTDLEND		(VIL13END+PORTDLSIZE)
#define	PORTDLOX			-20
#define	PORTDLOY			-9

//--------------------------------------------------------------------------------------------------------------------------------

#define	PORTDUSTART		(PORTDLEND)
#define	PORTDUSPRITE	(PORTDLSPRITE+1)
#define	PORTDUSIZE		((48*14)/2)
#define	PORTDUEND		(PORTDLEND+PORTDUSIZE)
#define	PORTDUOX			-24
#define	PORTDUOY			-14

//--------------------------------------------------------------------------------------------------------------------------------

#define	PORTLLSTART		(PORTDUEND)
#define	PORTLLSPRITE	(PORTDUSPRITE+1)
#define	PORTLLSIZE		((16*35)/2)
#define	PORTLLEND		(PORTDUEND+PORTLLSIZE)
#define	PORTLLOX			-8
#define	PORTLLOY			-35

//--------------------------------------------------------------------------------------------------------------------------------

#define	PORTLUSTART		(PORTLLEND)
#define	PORTLUSPRITE	(PORTLLSPRITE+1)
#define	PORTLUSIZE		((24*49)/2)
#define	PORTLUEND		(PORTLLEND+PORTLUSIZE)
#define	PORTLUOX			-12
#define	PORTLUOY			-49

//--------------------------------------------------------------------------------------------------------------------------------

#define	PORTRLSTART		(PORTLUEND)
#define	PORTRLSPRITE	(PORTLUSPRITE+1)
#define	PORTRLSIZE		((8*36)/2)
#define	PORTRLEND		(PORTLUEND+PORTRLSIZE)
#define	PORTRLOX			-4
#define	PORTRLOY			-36

//--------------------------------------------------------------------------------------------------------------------------------

#define	PORTRUSTART		(PORTRLEND)
#define	PORTRUSPRITE	(PORTRLSPRITE+1)
#define	PORTRUSIZE		((24*51)/2)
#define	PORTRUEND		(PORTRLEND+PORTRUSIZE)
#define	PORTRUOX			-12
#define	PORTRUOY			-51

//--------------------------------------------------------------------------------------------------------------------------------

#define	PORTULSTART		(PORTRUEND)
#define	PORTULSPRITE	(PORTRUSPRITE+1)
#define	PORTULSIZE		((32*8)/2)
#define	PORTULEND		(PORTRUEND+PORTULSIZE)
#define	PORTULOX			-16
#define	PORTULOY			-8

//--------------------------------------------------------------------------------------------------------------------------------

#define	PORTUUSTART		(PORTULEND)
#define	PORTUUSPRITE	(PORTULSPRITE+1)
#define	PORTUUSIZE		((48*16)/2)
#define	PORTUUEND		(PORTULEND+PORTUUSIZE)
#define	PORTUUOX			-24
#define	PORTUUOY			-16

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

#define	MAXSPRITENUM	(PORTUUSPRITE+1)

#define	DATAEND			PORTUUEND

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

//Stuff to align the CLUTS onto a 32 byte boundary
#define	bound				(DATAEND / 32)
#define	diff				(32 * bound)
#define	diffp 			(DATAEND - diff)
#define	res				(32-diffp)

#define	CLUTSTART		(DATAEND+res)
#define	CLUTEND			(CLUTSTART+(MAXSPRITENUM*32))

#define	CLUTAREASIZE	(CLUTEND-CLUTSTART)
#define	STATICCLUT		(CLUTSTART+(BORDERSPRITE*32))


#define	BARSRCLUT		(STATICCLUT+(32*1))
#define	BARSLCLUT		(STATICCLUT+(32*2))
#define	MISSWIRECLUT	(STATICCLUT+(32*3))
#define	SHIPWIRECLUT	(STATICCLUT+(32*4))
#define	BORDERCLUT		(STATICCLUT+(32*5))
#define	VIL1CLUT			(STATICCLUT+(32*6))
#define	VIL2CLUT			(STATICCLUT+(32*7))
#define	VIL3CLUT			(STATICCLUT+(32*8))
#define	VIL4CLUT			(STATICCLUT+(32*9))
#define	VIL5CLUT			(STATICCLUT+(32*10))
#define	VIL6CLUT			(STATICCLUT+(32*11))
#define	VIL7CLUT			(STATICCLUT+(32*12))
#define	VIL8CLUT			(STATICCLUT+(32*13))
#define	VIL9CLUT			(STATICCLUT+(32*14))
#define	VIL10CLUT		(STATICCLUT+(32*15))
#define	VIL11CLUT		(STATICCLUT+(32*16))
#define	VIL12CLUT		(STATICCLUT+(32*17))
#define	VIL13CLUT		(STATICCLUT+(32*18))
#define	FONTCLUT			(STATICCLUT+(32*19))

#define	PORTUUCLUT		(STATICCLUT+(32*20))
#define	PORTULCLUT		(STATICCLUT+(32*21))
#define	PORTRUCLUT		(STATICCLUT+(32*22))
#define	PORTRLCLUT		(STATICCLUT+(32*23))
#define	PORTLUCLUT		(STATICCLUT+(32*24))
#define	PORTLLCLUT		(STATICCLUT+(32*25))
#define	PORTDUCLUT		(STATICCLUT+(32*26))
#define	PORTDLCLUT		(STATICCLUT+(32*27))





//Maximum number of GFX transfers that can take place in VBlank (ha!)
#define	MAX_TRANS		MAXSPRITENUM

#define	VRAMUSED			((512*1024)-(CLUTEND))

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

//#define	FONTCOL(r,g,b)		*((Uint16 *)(SpriteVRAM+(FONTCLUT)+2))	=	((b<<10) + (g<<5) + (r))

#define	FONTCOL(r,g,b)		*((Uint16 *)(SpriteVRAM+(FONTCLUT)+2))	=	C_RGB(r,g,b)

#define	SHIPCOL(r,g,b)		*((Uint16 *)(SpriteVRAM+(SHIPWIRECLUT)+2))	=	C_RGB(r,g,b)

