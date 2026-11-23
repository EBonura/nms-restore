
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*    VARIABLES FOR SPRITE ROUTINE     *
**************************************/

/*
SIZE    	VAR     DEFAULT DESCRIPTION
SHORT	sprctrl.scalex;	4096- 1 smallest -> largest   scales X around origin	
SHORT	sprctrl.scaley;	4096- 1 smallest -> largest   scales Y around origin
LONG	sprctrl.rotate;	0   - 4096=1 degree    rotates 	  
BYTE	sprctrl.semitrans; 	0   - set 1-4 for different transparent modes
BYTE	sprctrl.red;		128 - 0 is black -> 255 fully bright
BYTE	sprctrl.green;		128 - 0 is black -> 255 fully bright
BYTE	sprctrl.blue;		128 - 0 is black -> 255 fully bright
BYTE	sprctrl.center;	0   - if set to 1 scales and rotates about center of sprite
BYTE	sprctrl.flipx	   	0   - if set to 1 flips sprite horizontally
BYTE	sprctrl.flipy	   	0   - if set to 1 flips sprite vertically
BYTE	sprctrl.forceZ	 	128 - if not 128, it is the z plane, 0=BKGRND 255=FORE
ULONG    sprctrl.attribute   0   - ORs the full sprite attribute
USHORT   sprctrl.doitlive	0   - set to TEMPVRAM for temporarly downloaded sprites
*/

/**************************************
*          DECLARE VARIABLES          *
**************************************/

SPRCTRL	sprctrl;					/* SPRITE CONTROLLER STRUCTURE TYPE */

TEMPSPRITE	tempsprites[MAXTEMPSPRITES];	/* TEMP DOWNLOADED SPRITE ARRAYS */
SPRITEIMAGE	*spritelive;			/* LIVE DOWNLOAD SPRITE DATA */

ULONG	*imagedata;				/* POINTER TO LIVE DOWNLOADED DATA */
ULONG	*sprheap;					/* POINTER TO SPR HEAP */

USHORT	tpage[22];				/* TEXTURE PAGES ID */
ULONG	respagesloaded[22];			/* WHICH TEXTURE PAGES ARE IN USE */

ULONG 	vram_slot_table[22][32];		/* VRAM SLOT TABLE USAGE */
UBYTE	vram_slot_count[22][32];		/* COUNTER FOR VRAM SLOT TABLE */

UBYTE	respagecnt;				/* RESIDENT PAGE COUNTER */

UBYTE	gamefirstpass;				/* LIVE DOWNLOADING FLAG */
UBYTE	tempspritenumber;			/* LIVE DOWNLOADING CURRENT SPRITE NUMBER */
UBYTE	vramtopback;				/* RESIDENT PAGE VARIABLE */

UBYTE	cluts16[320];				/* FLAGS FOR CURRENT 16 COLOUR PALETTES */
UBYTE	cluts256[16];				/* FLAGS FOR CURRENT 256 COLOUR PALETTES */

USHORT	last16slot;				/* LAST 16 COLOUR SLOT USED */
USHORT	last16cols[16];			/* LAST 16 COLOURS USED */

USHORT	last256slot;				/* LAST 256 COLOUR SLOT USED */
USHORT	last256cols[256];			/* LAST 256 COLOURS USED */

ULONG	maxpolycount;				/* MAXIMUM POLYGONS ALLOWED PER FRAME */

/**************************************
*        VRAM BIT USAGE TABLE         *
**************************************/

const	ULONG	widthmasks[33]={
						0x00000000,
						0x80000000,
					   	0xC0000000,
					   	0xE0000000,
					   	0xF0000000,
						0xF8000000,
					   	0xFC000000,
					   	0xFE000000,
					   	0xFF000000,
					   	0xFF800000,
					   	0xFFC00000,
					   	0xFFE00000,
					   	0xFFF00000,
						0xFFF80000,
					   	0xFFFC0000,
					   	0xFFFE0000,
					   	0xFFFF0000,
					   	0xFFFF8000,
					   	0xFFFFC000,
					   	0xFFFFE000,
					   	0xFFFFF000,
						0xFFFFF800,
					   	0xFFFFFC00,
					   	0xFFFFFE00,
					   	0xFFFFFF00,
						0xFFFFFF80,
					   	0xFFFFFFC0,
					   	0xFFFFFFE0,
					   	0xFFFFFFF0,
						0xFFFFFFF8,
					   	0xFFFFFFFC,
					   	0xFFFFFFFE,
					   	0xFFFFFFFF,  
						};

/**************************************
*     PRINT A COMPLICATED SPRITE      *
**************************************/

void 
Print_Sprite(SPRITE *spr,SHORT x,SHORT y,UBYTE zbuf)
{

UBYTE	fastspr;
GsSPRITE sprite;
USHORT 	w;
USHORT 	h;
SHORT 	u;
SHORT 	v;

	if (maxpolycount==0) return;		/* CHECK FOR TOO MANY SPRITES THIS FRAME */
	maxpolycount--;				/* OTHERWISE DECREMENT IT AND CARRY ON */

	w=spr->w;						/* GET WIDTH FROM SPRITE */
	if (w==0) w=256;				/* 0 MEANS 256 WIDE */
	sprite.w=w;					/* PUT INTO TEMP SPRITE */
	h=spr->h;						/* GET HEIGHT FROM SPRITE */
	if (h==0) h=256;				/* 0 MEANS 256 HIGH */
	sprite.h=h;					/* PUT INTO TEMP SPRITE */

	if (spr->clut==0)				/* CHECK FOR LIVE DOWNLOAD SPRITE */
	{
		Init_Spr_One_Live(spr,0);		/* INITIALISE A LIVE SPRITE */
		if (spr->clut==0) return;		/* NOIMAGE IN SPRITE SO QUIT */
	}

	fastspr=0;					/* RESET FLAG FOR FAST SPRITE CHECK */
	sprite.attribute=sprctrl.attribute;	/* PASS OVER ATTRIBUTE FROM SPRITE */
	sprite.x=x;					/* PASS OVER X COORDINATE FROM SPRITE */
	u=spr->u;						/* PASS OVER U OFFSET FROM SPRITE */
	sprite.y=y;					/* PASS OVER Y COORDINATE FROM SPRIE */
	v=spr->v;						/* PASS OVER V OFFSET FROM SPRITE */

	sprite.tpage=tpage[spr->tpage];		/* TELL IT WHERE THE TEXTURE PAGE IS */
	sprite.u=spr->x;				/* GET WIDTH */
	sprite.v=spr->y;				/* GET HEIGHT */
	sprite.cx=(spr->clut & 255)<<3;		/* GET COLOUR X POSITION */
	sprite.cy=256+(spr->clut>>8); 		/* GET COLOUR Y POSITION */

	sprite.r=sprctrl.red;			/* GET RED COLOUR VALUE */
	sprite.g=sprctrl.green;			/* GET GREEN COLOUR VALUE */
	sprite.b=sprctrl.blue;			/* GET BLUE COLOUR VALUE */

	sprite.mx=sprite.my=0;
	sprite.scalex=sprctrl.scalex;		/* GET X SCALE VALUE */
	sprite.scaley=sprctrl.scaley;		/* GET Y SCALE VALUE */
	sprite.rotate=sprctrl.rotate;		/* GET ROTATE VALUE */

/**************************************
*     IS THE SPRITE BEING FADED       *
**************************************/

	if ((fade!=128) && (fade!=0) && (sprctrl.red!=0))
		sprite.r=sprite.g=sprite.b=(sprite.r*fade)/sprctrl.red;

/**************************************
*    SPRITE IS 256 COLOURS (8BIT)     *
**************************************/

	if ((spr->flags & COL256)!=0)
	{
		sprite.attribute=sprite.attribute BOR 0x01000000;
		if (screenheight==240) sprite.cy=sprite.cy & 255;
	}

/**************************************
*    SPRITE IS TRUE COLOUR (16BIT)    *
**************************************/

	if ((spr->flags & COL16BIT)!=0)sprite.attribute=sprite.attribute BOR 0x02000000;

/**************************************
*     SPRITE IS SEMI TRANSPARENT      *
**************************************/

	if (sprctrl.semitrans!=0)
	{
		sprite.attribute|=(4+(sprctrl.semitrans-1))<<28;
		fastspr++;
	}

/**************************************
*      PRINT SPRITE ABOUT CENTRE      *
**************************************/

	if (sprctrl.center!=0)
	{
		u=-w/2;
		v=-h/2;
	}

/**************************************
*    SPRITE IS FLIPPED HORIZONTAL     *
**************************************/

	if ((spr->flags & FLIP) XOR (sprctrl.flipx)!=0)
		sprite.scalex=-sprite.scalex;

/**************************************
*     SPRITE IS FLIPPED VERTICAL      *
**************************************/

	if (sprctrl.flipy!=0)
		sprite.scaley=-sprite.scaley;

/**************************************
*     SPRITE IS ROTATED OR SCALED     *
**************************************/

	if ((sprite.rotate!=0) LOR (sprite.scalex!=4096) LOR (sprite.scaley!=4096))
	{ 
		if (sprctrl.center!=0)
		{
			sprite.mx=w/2;
			sprite.my=h/2;
			fastspr++;
		}
		else
		{
			sprite.mx=-u;
			sprite.my=-v;	
		}
		fastspr++;
		u=v=0;
	}

/**************************************
*      PRINT SPRITE WITH OFFSET       *
**************************************/

	if (screenwidth==640) sprite.x-=320-u;
	else sprite.x-=160-u;

	if (screenheight==480) sprite.y-=240-v;
	else sprite.y-=120-v;

	if (sprctrl.forceZ!=128) zbuf=sprctrl.forceZ;

	if (fastspr==0)	GsSortFastSprite(&sprite,&Wot[screentoggle],255-zbuf);
	else				GsSortSprite(&sprite,&Wot[screentoggle],255-zbuf);

	if ((spr->flags & SPLIT)!=0) Print_Sprite(spr+1,x,y,zbuf);

}

/**************************************
*  FIND SPRITE OFFSET WITH NO SPLIT   *
**************************************/

SPRITE* 
Find_Spr(SPRITE* spr,USHORT num)
{

	while (num!=0)					/* CHECK FOR CORRECT SPRITE */
	{
		if ((spr->flags & SPLIT)==0) num--;	/* IGNORE ANY SPLIT SPRITES */
		spr++;					/* MOVE ONTO NEXT SPRITE */
	}

	return spr;

}

/**************************************
*     RESET ALL SPRITE PARAMETERS     *
**************************************/

void
Reset_Spr_Paras(void)
{

	sprctrl.scalex=4096;
	sprctrl.scaley=4096;
	sprctrl.semitrans=0;
	sprctrl.rotate=0;
	sprctrl.red=128;
	sprctrl.green=128;
	sprctrl.blue=128;
	sprctrl.center=0;
	sprctrl.flipx=0;
	sprctrl.flipy=0;
	sprctrl.forceZ=128;
	sprctrl.doitlive=0;
	sprctrl.attribute=0;

}

/**************************************
*         TURN SPRITES OFF            *
**************************************/

void 
Sprite_Off(SPRITE *spr)
{

	while ((spr->flags & LASTSPR)==0)		/* SCAN THROUGH TO END OF THIS LIST */
	{
		spr->clut=0;				/* RESET THIS SPRITE */
		spr++;					/* AND GOTO NEXT ONE */
	}
	spr->clut=0;					/* RESET LAST SPRITE TOO */

}

/**************************************
*      INITIALISE TEXTURE PAGES       *
**************************************/

void
Init_TPages()
{

USHORT	x;
USHORT	y;
USHORT	j;
UBYTE	page;

	gamefirstpass=YES;				/* SET TO FIRST GAME CYCLE */
	respagecnt=22;					/* POINT RESIDENT PAGES TO LAST PAGE */
	vramtopback=0;
	page=0;
	y=0;
	x=SCREENWIDTH;
	for (page=0;page!=22;page++)
	{
		tpage[page]=LoadTPage((ULONG *)1,0, 0,x,y,256,256);
		x=x+64;
		if (x>1023)
		{
			x=SCREENWIDTH;
			y=256;
		}
		respagesloaded[page]=0;
	}

	for (j=0;j<MAXTEMPSPRITES;j++)
	{
		tempsprites[j].spr=0;
		tempsprites[j].pal.w=0;
	}
}

/**************************************
*      ADD RESIDENT TEXTURE PAGE      *
**************************************/

UBYTE
Add_Res_Page(UBYTE *mem,UBYTE type)		/* TYPE 0=16 COL, 1=256 COL, 2=TRUE */
{

USHORT	x;
USHORT	y;
UBYTE	page;

	for (page=21;page!=0;page--)		/* FIRST CHECK TO SEE IF PAGE LOADED */
	{
		if (respagesloaded[page]==(ULONG)mem) return page;
	}

	if (type==1) respagecnt--;
	if (type==2) respagecnt-=3;
	respagecnt--;

	if (respagecnt<16)				/* CHECK FOR TOO MANY RESIDENT PAGES */
	{
		printf("Error, Too many resident pages!");
		CRASH;
		Show_VRAM();
	}

	x=SCREENWIDTH+64*respagecnt;
	y=0;
	if (x>1023)
	{
		x=x-(1024-SCREENWIDTH);
		y=256;
	}
	LoadTPage((ULONG *)mem,type,0,x,y,256,256);
	respagesloaded[respagecnt]=(ULONG)mem;
	return respagecnt;

}

/**************************************
*     RESET COLOUR LOOK UP TABLES     *
**************************************/

void
Reset_Cluts()
{

USHORT	j;
USHORT	k;
ULONG	*blankvram;
RECT	rect={0,0,1024,1};

	last16slot=0;					/* RESET LAST 16 COLOURS SLOT */
	last256slot=0;					/* RESET LAST 256 COLOURS SLOT */
	for (j=0;j!=320;j++) cluts16[j]=0;		/* RESET 16 COLOUR SLOTS */
	for (j=0;j!=16;j++) cluts256[j]=0;		/* RESET 256 COLOUR SLOTS */
	for (j=0;j!=16;j++) last16cols[j]=0;	/* RESET LAST 16 COLOURS */
	for (j=0;j!=256;j++) last256cols[j]=0; 	/* RESET LAST 256 COLOURS */

	for (j=0;j<22;j++)				/* RESET THE VIDRAM ALLOCATOR SLOTS */
	{
		for (k=0;k<32;k++)
		{
			vram_slot_table[j][k]=0;
			vram_slot_count[j][k]=32;
		}
	}

	blankvram=(ULONG*)MyMalloc(1024*8);	/* RESTORE: was a cast as lvalue, (UBYTE*)blankvram=... */

	if (screenwidth==320)			/* DIFFERENT BLANK AMOUNT FOR 640X480 */
	{
		for (j=0;j<160;j++) *(blankvram+j)=0;	/* BLANK SCREEN TO BLACK */
		for (j=160;j<512;j++) *(blankvram+j)=-1;	/* BLANK REST OF VIDRAM TO WHITE */
	}
	else
	{
		for (j=0;j<320;j++) *(blankvram+j)=0;	/* BLANK SCREEN TO BLACK */
		for (j=320;j<512;j++) *(blankvram+j)=-1;	/* BLANK REST OF VIDRAM TO WHITE */
	}

	for (rect.y=0;rect.y<512;rect.y++)		/* BLANK VRAM LINE BY LINE */
	{
		DrawSync(0);
		LoadImage(&rect,(ULONG*)blankvram);
	}
	MyFree((UBYTE*)blankvram);

}



/**************************************
*    GET LOCATION FROM THE SPRITE     *
**************************************/

ULONG
Get_location_from_spr(SPRITE *spr)
{

UBYTE	x;

	x=spr->x;
	if ((spr->flags & COL256)!=0) x=x*2;
	if ((spr->flags & COL16BIT)!=0) x=x*4;
	return (spr->tpage<<16)+(((spr->y+7)/8)<<8)+(x+7)/8;	

}

/**************************************
*     DEPACK BITSTREAM TO TEMPRAM     *
**************************************/

void 
Bit2raw(UBYTE *source,UBYTE *dest)
{

UBYTE	*src_cols; 
LONG	destlen;
UBYTE	mask;

	destlen=*(ULONG *)source;		/* FIRST LONG=LENGTH */
	source+=4;					/* MOVE ONTO DATA */
	src_cols=source+((destlen-1)>>3)+1;	/* FIND LIST OF COLOURS */
	while(destlen>0)				/* CHECK FOR END OF DATA */
	{
 		for (mask =0x80;mask!=0;mask>>=1)
		{
 			if ((*source & mask)!=0) *dest++=*src_cols++;
	   	 	else *dest++=0;
		}
		source++;
		destlen-=8;  
	}
}

/**************************************
*    DEPACK RUN LENGTH TO TEMPRAM     *
**************************************/

void
Rle2raw(UBYTE *source,UBYTE *dest)
{

UBYTE	temp;
UBYTE	col;
ULONG	src_len;

	src_len=*(ULONG *)source;		/* FIRST LONG=LENGTH */
	source+=4;					/* MOVE ONTO DATA */
	while(src_len!=0)				/* CHECK FOR END OF DATA */
	{
		temp=*source++;
		if (temp>0x80)
		{
			src_len -=2;
			col   =*source++;
			for (temp=1-temp;temp!=0;temp--)*dest++ = col;
		}
 		else
		{
			temp++;
			src_len-=temp+1;
			for (;temp!=0;temp--)*dest++=*source++;
		}
	}
}
/**************************************
*      FIND SPARE HOLE IN VIDRAM      *
**************************************/

ULONG
Find_Hole_In_VRAM(UBYTE w,UBYTE h)
{

ULONG	location;
UBYTE	page;

UBYTE	x;
UBYTE	y;

UBYTE	u;
UBYTE	v;	
ULONG	tempmask;
UBYTE	movetonextpage;
UBYTE	pagelimit;

UBYTE	notevenclose;
UBYTE	nogood;

	movetonextpage=1-(vramtopback<<1);
	pagelimit=respagecnt*(1 XOR vramtopback);
	for (page=(respagecnt-1)*vramtopback;page!=pagelimit;page+=movetonextpage)
	{
		for (y=0;y<33-h;y++)
		{
			for (v=0;v<h;v++)
			{
				notevenclose=FALSE;
				if(vram_slot_count[page][v+y]<w) notevenclose=TRUE;

				if (notevenclose==FALSE)
				{
					for (x=0;x<33-w;x++)
					{

						nogood=FALSE;
						tempmask=widthmasks[w]>>x;
						for (v=0;v<h;v++)
						{
							if ((vram_slot_table[page][v+y] & tempmask)!=0) nogood=TRUE;
						}

						if (nogood==FALSE)
						{
							location=(page<<16)+(y<<8)+x;
							Xor_VRAM_Hole(location,w,h);
							return location;
						}
					}
				}
			}
 		}
	}

	printf("Error, Ran out of VRAM!");
	CRASH;
	Show_VRAM();
	return;

}

/**************************************
*          XOR A VRAM HOLE            *
**************************************/

void
Xor_VRAM_Hole(ULONG location,UBYTE w,UBYTE h)
{

UBYTE	page=(location>>16) & 255;
UBYTE	y=(location>>8) & 255;
UBYTE	x=location & 255;
UBYTE	v;
UBYTE	j;
ULONG	cnt;
ULONG	cntmax;
ULONG	tablemask;
ULONG	tempmask;

	tempmask=widthmasks[w]>>x;
	for (v=0;v<h;v++)
	{
		vram_slot_table[page][y+v]=vram_slot_table[page][y+v] XOR tempmask;
		vram_slot_count[page][y+v]=0;
		tablemask=vram_slot_table[page][y+v];
		cnt=0;
		cntmax=0;
		for (j=0;j<32;j++)
		{
			if ((tablemask & 1)!=0)
			{
				cnt=0;
			}
			else
			{
				cnt++;
				if (cnt>cntmax) cntmax=cnt;
			}
			tablemask=tablemask>>1;
		}
		vram_slot_count[page][y+v]=cntmax;
	}
}

/**************************************
*        INITIALISE SPRITE LIST       *
**************************************/

void
Init_Spr_List(SPRITE *spr,ULONG loadoffset)
{

	Add_Spr_Loadoffset(spr,loadoffset);

	while ((spr->flags & LASTSPR)==0)
	{
		Init_Spr_One(spr,loadoffset);
		spr++;
	}

	Init_Spr_One(spr,loadoffset);

}

/**************************************
*     INITIALISE A SINGLE SPRITE      *
**************************************/

void
Init_Spr_One(SPRITE *spr,ULONG loadoffset)
{

SPRITE	*spr1;

	if ((spr->flags & NOIMAGE)!=0) return;	/* NO IMAGE */

	spr->flags&=0xffff-TEMPVRAM;		/* RESET TEMPVRAM FLAG */

	if (spr->clut==0)				/* HAS PALETTE BEEN SET UP */
	{
		spritelive=0;
		if ((spr->flags & RESPAGE)!=0)	/* CHECK FOR RESIDENT PAGE */
		{
			if ((spr->flags & (COL16BIT+COL256))==0)spr->tpage=Add_Res_Page((UBYTE *)spr->image,0);
			if ((spr->flags & COL256)!=0)	spr->tpage=Add_Res_Page((UBYTE *)spr->image,1); 
			if ((spr->flags & COL16BIT)!=0)	spr->tpage=Add_Res_Page((UBYTE *)spr->image,2);
		}
		else
		{
			Add_Sprite_Frame(spr);		/* ADD A SPRITE TO THE LIST */
		}

		if ((spr->flags & COL256)!=0)		spr->clut=Add_Clut256((UBYTE *)spr->pal);
		if ((spr->flags & (COL16BIT+COL256))==0)	spr->clut=Add_Clut16((UBYTE *)spr->pal);
	}

	spr1=spr;						/* MAKE A COPY OF THE SPRITE */

	for (;;)
	{
		if ((spr1->flags & LASTSPR+RESPAGE)!=0) return;	/* CHECK FOR LAST OR RESIDENT PAGE */
		spr1++;
		if (spr1->image==spr->image)
		{
			spr->flags&=0xffff-TEMPVRAM;	/* RESET TEMPVRAM FLAG */
		}

		if (spr1->clut==0)			/* CHECK FOR CLUT ALREADY THERE */
		{
			if (spr1->image==spr->image)	/* ARE BOTH IMAGES THE SAME ADDRESS */
			{
				spr1->tpage=spr->tpage;	/* IF SO THEN COPY DATA OVER */
				spr1->x=spr->x;
				spr1->y=spr->y;
				spr1->clut=spr->clut;
			}
		}
	}
}

/**************************************
*     INITIALISE A SINGLE SPRITE      *
**************************************/

ULONG
Add_Sprite_Frame(SPRITE *spr)
{

ULONG	location;		
USHORT	w;						/* WIDTH IN NO. OF 8*8 BLOCKS */

USHORT	h;						/* WIDTH IN NO. of 8*8 BLOCKS */
USHORT	j;
RECT	rect;

	w=spr->w;						/* GET WIDTH OF SPRITE */
	if (w==0) w=256;				/* CHECK FOR FULL WIDTH */
	w--;
	w=w/2;						/* DIVIDE WITH UP */

	if ((spr->flags & COL256)!=0) w=w/2;
	if ((spr->flags & (COL16BIT+COL256))==0) w=w/4;
	w++;

	h=spr->h;						/* GET HEIGHT OF SPRITE */
	if (h==0) h=256;				/* CHECK FOR FULL HEIGHT */
	h--;
	h=h/8;						/* DIVIDE HEIGHT UP */
	h++;

	location=Find_Hole_In_VRAM(w,h);		/* FIND VIDRAM FOR SPRITE */

	rect.y=0;
	rect.x=SCREENWIDTH+((location>>16) & 255)*64;

	if (rect.x>1023)
	{
		rect.y=256;
		rect.x=rect.x-(1024-SCREENWIDTH);
	}
	rect.x=rect.x+(location & 255)*2;
		
	rect.y=rect.y+((location>>8) & 255)*8;

	rect.w=spr->w;
	if (rect.w==0) rect.w=256;
	rect.w--;
	if ((spr->flags & COL256)!=0) rect.w=rect.w/2;
	if ((spr->flags & (COL16BIT+COL256))==0) rect.w=rect.w/4;
	rect.w++;
	rect.h=spr->h;
	if (rect.h==0) rect.h=256;

	imagedata=(ULONG *)spr->image;
	sprheap=0;

	sprheap=(ULONG*)MyMalloc(rect.h*rect.w*4+32);	/* RESTORE: was a cast as lvalue, (BYTE*)sprheap=... */
	if (sprheap==0) CRASH;

	if ((spr->flags & BITSTREAM)!=0)
	{
		Bit2raw((UBYTE *)imagedata,(UBYTE *)sprheap);
		imagedata=(ULONG *)sprheap;
	}

	if ((spr->flags & RUNLENGTH)!=0)
	{
		Rle2raw((UBYTE *)imagedata,(UBYTE *)sprheap);
		imagedata=(ULONG *)sprheap;
	}

	LoadImage(&rect,(ULONG *)imagedata); 
	MyFree((BYTE*)sprheap);

	spr->tpage=(location>>16) & 255;
	spr->x=(location & 255)<<3;
	if ((spr->flags & COL256)!=0) spr->x=spr->x/2;
	if ((spr->flags & COL16BIT)!=0) spr->x=spr->x/4;
	spr->y=((location>>8) & 255)<<3;
	return location;

}

/**************************************
*         ADD 16 COLOUR CLUT          *
**************************************/

USHORT 
Add_Clut16(UBYTE *palette)
{

USHORT	clutno;
USHORT	k;
USHORT	clutaddr;
RECT	rect;

	for (k=0;k<16;k++)				/* CHECK FOR LAST PALETTE */
	{
		if (last16cols[k]!=(USHORT )*(palette+k))
		break;
	}

	if (k!=16)					/* CHECK FOR NEW PALETTE */
	{
		clutno=0;
		while (cluts16[clutno]!=0)		/* FIND AN EMPTY SLOT */
		{
			clutno++;
			if (clutno==160)		/* CHECK FOR NOT FOUND EMPTY SLOT */
			{
				printf("Error, CLUTS16 have ran out!");
				CRASH;
				Show_VRAM();
			}
		}
	}
	else
	{
		clutno=last16slot;			/* COPY OF ANOTHER PALETTE */
	}

	clutaddr=(((clutno MOD 20)*32)>>4)+(((clutno/20)+240)<<8);	
	rect.x=(clutaddr & 255)<<3;
	rect.y=256+(clutaddr>>8);
	rect.w=16;
	rect.h=1;

	LoadImage(&rect,(ULONG *)palette);		/* COPY COLS TO VRAM */

	for (k=0;k<16;k++)				/* MAKE THESE COLS LAST 16 USED */
	{
		last16cols[k]=(USHORT )*(palette+k);
	}
	last16slot=clutno;				/* SET THIS TO LAST SLOT USED */
	cluts16[clutno]=255;			/* SET CLUT TO BEING USED */

	return clutaddr;

}

/**************************************
*         ADD 256 COLOUR CLUT         *
**************************************/

USHORT 
Add_Clut256(UBYTE *palette)
{

USHORT	clutno;
USHORT	k;
USHORT	clutaddr;
RECT	rect;

	for (k=0;k<256;k++)				/* CHECK FOR LAST PALETTE */
	{
		if (last256cols[k]!=(USHORT )*(palette+k*2))
		break;
	}

	if (k!=256)					/* CHECK FOR NEW PALETTE */
	{
		clutno=0;
		while (cluts256[clutno]!=0)		/* FIND AN EMPTY SLOT */
		{
			clutno++;
			if (clutno==16)		/* CHECK FOR NOT FOUND EMPTY SLOT */
			{
				printf("Error, CLUTS256 have ran out!");
				CRASH;
				Show_VRAM();
			}
		}
	}
	else
	{
		clutno=last256slot;			/* COPY OF ANOTHER PALETTE */
	}

	clutaddr=(clutno+240)<<8;
	rect.x=(clutaddr & 255)<<3;
	rect.y=clutaddr>>8;
	rect.w=256;
	rect.h=1;
	LoadImage(&rect,(ULONG *)palette);		/* COPY COLOURS TO VRAM */

	for (k=0;k<256;k++)				/* MAKE THESE COLS LAST 16 USED */
	{
		last256cols[k]=(USHORT )*(palette+k*2);
	}
	last256slot=clutno;				/* SET THIS TO LAST SLOT USED */
	cluts256[clutno]=255;			/* SET CLUT TO BEING USED */

	return clutaddr;

}

/**************************************
*    ADD LOAD OFFSET TO SPRITE DATA   *
**************************************/

void 
Add_Spr_Loadoffset(SPRITE *spr,ULONG loadoffset)
{
	while ((spr->flags & LASTSPR)==0)
	{
		if (spr->image<0x800000)
		{
			spr->image+=loadoffset;
			spr->pal+=loadoffset;
		}
		spr++;
	}

	if (spr->image<0x800000)
	{
		spr->image+=loadoffset;
		spr->pal+=loadoffset;
	}
}

/**************************************
*      INITIALISE A LIVE SPRITE       *
**************************************/

void
Init_Spr_One_Live(SPRITE *spr,ULONG loadoffset)
{

SPRITE	*spr1;

	if ((spr->flags & NOIMAGE)!=0) return;	/* NO IMAGE */

	spr->flags&=0xffff-TEMPVRAM;		/* RESET LIVE FLAG */
	spr->flags|=TEMPVRAM;			/* SET TO LIVE DOWNLOADING */

	if (spr->clut==0)				/* HAS PALETTE BEEN SET UP */
	{
		Add_Sprite_Frame_Live(spr);		/* SETUP LIVE SPRITE */

		if ((spr->flags & COL256)!=0)		spr->clut=Add_Clut256_Live((UBYTE *)spr->pal);
		if ((spr->flags & (COL16BIT+COL256))==0)	spr->clut=Add_Clut16_Live((UBYTE *)spr->pal);

		InteruptGPU[currentlive]=(MOVEIMAGE*)spritelive;	/* SLOT IN NEW DATA */
		currentlive++;				/* AND MOVE ONTO NEXT SLOT */
	}
}

/**************************************
*      INITIALISE A LIVE SPRITE       *
**************************************/

ULONG
Add_Sprite_Frame_Live(SPRITE *spr)
{

ULONG	location;		
USHORT	w;						/* WIDTH IN NO. OF 8*8 BLOCKS */
USHORT	h;						/* WIDTH IN NO. of 8*8 BLOCKS */
USHORT	j;
RECT	rect;

	w=spr->w;						/* GET WIDTH OF SPRITE */
	if (w==0) w=256;				/* CHECK FOR FULL WIDTH */
	w--;
	w=w/2;						/* DIVIDE WITH UP */

	if ((spr->flags & COL256)!=0) w=w/2;
	if ((spr->flags & (COL16BIT+COL256))==0) w=w/4;
	w++;

	h=spr->h;						/* GET HEIGHT OF SPRITE */
	if (h==0) h=256;				/* CHECK FOR FULL HEIGHT */
	h--;
	h=h/8;						/* DIVIDE HEIGHT UP */
	h++;

	vramtopback=1;
	location=Find_Hole_In_VRAM(w,h);
	vramtopback=0;

	rect.y=0;
	rect.x=SCREENWIDTH+((location>>16) & 255)*64;

	if (rect.x>1023)
	{
		rect.y=256;
		rect.x=rect.x-(1024-SCREENWIDTH);
	}
	rect.x=rect.x+(location & 255)*2;

	rect.y=rect.y+((location>>8) & 255)*8;

	rect.w=spr->w;
	if (rect.w==0) rect.w=256;
	rect.w--;
	if ((spr->flags & COL256)!=0) rect.w=rect.w/2;
	if ((spr->flags & (COL16BIT+COL256))==0) rect.w=rect.w/4;
	rect.w++;
	rect.h=spr->h;
	if (rect.h==0) rect.h=256;

	imagedata=(ULONG *)spr->image;
	sprheap=0;

	sprheap=(ULONG*)MyMalloc(rect.h*rect.w*4+32);	/* RESTORE: was (BYTE*)sprheap=..., a cast as lvalue */ /* FIND MEMORY FOR DECOMPRESSION */
	if (sprheap==0) CRASH;

	if ((spr->flags & BITSTREAM)!=0)		/* CHECK FOR BITSTREAM */
	{
		Bit2raw((UBYTE *)imagedata,(UBYTE *)sprheap);
		imagedata=(ULONG *)sprheap;
	}

	if ((spr->flags & RUNLENGTH)!=0)		/* CHECK FOR RUNLENGTH */
	{
		Rle2raw((UBYTE *)imagedata,(UBYTE *)sprheap);
		imagedata=(ULONG *)sprheap;
	}

	tempspritenumber=0;
	while((tempsprites[tempspritenumber].spr)!=0) tempspritenumber++;
	if (tempspritenumber>=MAXTEMPSPRITES)
	{
		printf("TOO MANY LIVE DOWNLOADED SPRITES!");
		CRASH;
	}

	tempsprites[tempspritenumber].rect.x=rect.x;	/* SETUP A LIVE SPRITE */
	tempsprites[tempspritenumber].rect.y=rect.y;
	tempsprites[tempspritenumber].rect.w=rect.w;
	tempsprites[tempspritenumber].rect.h=rect.h;
	tempsprites[tempspritenumber].spr=spr;
	tempsprites[tempspritenumber].pal.w=0;

	spritelive=(SPRITEIMAGE*)MyMalloc(sizeof(SPRITEIMAGE));	/* RESTORE: was a cast as lvalue, (BYTE*)spritelive=... */
	if (spritelive==0) CRASH;

	spritelive->type=LIVEDOWNLOAD;
	spritelive->rect.x=rect.x;
	spritelive->rect.y=rect.y;
	spritelive->rect.w=rect.w;
	spritelive->rect.h=rect.h;
	spritelive->data=(ULONG*)imagedata;
	spritelive->free=(ULONG*)spritelive;
	spritelive->pal=0; 

	spr->tpage=(location>>16) & 255;
	spr->x=(location & 255)<<3;
	if ((spr->flags & COL256)!=0) spr->x=spr->x/2;
	if ((spr->flags & COL16BIT)!=0) spr->x=spr->x/4;
	spr->y=((location>>8) & 255)<<3;

	return location;

}

/**************************************
*      ADD 16 COLOUR LIVE CLUT        *
**************************************/

USHORT 
Add_Clut16_Live(UBYTE *palette)
{

USHORT	clutno;
USHORT	clutaddr;
RECT	rect;

	clutno=319;
	while (cluts16[clutno]!=0)		/* FIND AN EMPTY SLOT */
	{
		clutno--;
		if (clutno==160)			/* CHECK FOR NOT FOUND EMPTY SLOT */
		{
			printf("Error, Live CLUTS16 have ran out!");
			CRASH;
			Show_VRAM();
		}
	}

	clutaddr=(((clutno MOD 20)*32)>>4)+(((clutno/20)+240)<<8);
	rect.x=(clutaddr & 255)<<3;
	rect.y=256+(clutaddr>>8);
	rect.w=16;
	rect.h=1;

	spritelive->pal=(ULONG *)palette;
	spritelive->rectpal.x=rect.x;
	spritelive->rectpal.y=rect.y;
	spritelive->rectpal.w=rect.w;
	spritelive->rectpal.h=rect.h;

	cluts16[clutno]=255;

	return clutaddr;
}

/**************************************
*      ADD 256 COLOUR LIVE CLUT       *
**************************************/

USHORT 
Add_Clut256_Live(UBYTE *palette)
{

USHORT	clutno;
USHORT	clutaddr;
RECT	rect;

	clutno=15;
	while (cluts256[clutno]!=0)		/* FIND AN EMPTY SLOT */
	{
		clutno--;
		if (clutno==0)				/* CHECK FOR NOT FOUND EMPTY SLOT */
		{
			printf("Error, Live CLUTS256 have ran out!");
			CRASH;
			Show_VRAM();
		}
	}

	clutaddr=(clutno+240)<<8;
	rect.x=(clutaddr & 255)<<3;
	rect.y=clutaddr>>8;
	rect.w=256;
	rect.h=1;

	spritelive->pal=(ULONG *)palette;
	spritelive->rectpal.x=rect.x;
	spritelive->rectpal.y=rect.y;
	spritelive->rectpal.w=rect.w;
	spritelive->rectpal.h=rect.h;

	cluts256[clutno]=255;

	return clutaddr;
}

/**************************************
*     DELETE 16 COLOUR LIVE CLUT      *
**************************************/

void
Del_Clut16_Live(USHORT clutaddr)
{

USHORT	clutno;

	clutno=((clutaddr>>8)-240)*20+(((clutaddr & 255)<<4))/32;
	if (cluts16[clutno]==0)
	{
		printf("Deleting a non existing 16 Clut");
		CRASH;
	}
	cluts16[clutno]=0;

}

/**************************************
*     DELETE 256 COLOUR LIVE CLUT     *
**************************************/

void
Del_Clut256_Live(USHORT clutaddr)
{

USHORT	clutno;

	clutno=(clutaddr>>8)-240;
	if (cluts256[clutno]==0)
	{
		printf("Error, Deleting non-existing 256 CLUT!");
		CRASH;
	}
	cluts256[clutno]=0;

}

/**************************************
*   DELETE LIVE SPRITE FROM VIDRAM    *
**************************************/

void
Delete_Spr_One_Live(SPRITE *spr,UBYTE* address)
{

SPRITE	*spr1;
ULONG	tempaddr;

	spr->flags&=0xffff-TEMPVRAM;		/* RESET TEMPVRAM FLAG */
	if (spr->clut!=0)				/* CHECK TO SEE IF CLUT THERE */
	{
		if ((spr->flags & RESPAGE)!=0)	/* CAN'T DELETE RESIDENT PAGES */
			return;
		if ((spr->flags & COL256)!=0)		/* DELETE 256 COLOUR CLUT */
			Del_Clut256_Live(spr->clut);
		if ((spr->flags & COL256+COL16BIT)==0)	/* DELETE 16 COLOUR CLUT */
			Del_Clut16_Live(spr->clut);
		Del_Sprite_Frame_Live(spr,address); 	/* DELETE THE SPRITE */
	}

	spr1=spr;
	tempaddr=spr->image;

	if (address!=0)
	{
		spr->image-=(ULONG)address;
		spr->pal-=(ULONG)address;
	}

	for (;;)
	{
		if ((spr1->flags & LASTSPR)!=0) return;	/* CHECK ALL FRAMES */
		spr1++;
		if (spr1->image==tempaddr)
		{
			spr1->flags&=0xffff-TEMPVRAM;	/* RESET TEMPVRAM FLAG */
			spr1->clut=0;			/* RESET CLUT */

			if (address!=0)
			{
				spr1->image-=(ULONG)address;
				spr1->pal-=(ULONG)address;
			}
		}
	}
}

/**************************************
*          DELETE A SPRITE            *
**************************************/

void
Del_Sprite_Frame_Live(SPRITE *spr,UBYTE* address)
{

ULONG	location;					/* LOCATION IN VIDRAM OF SPRITE */
UBYTE	w,h;						/* WIDTH AND HEIGHT OF SPRITE */

	w=((spr->w+1)/2);				/* GET REAL WIDTH IN VIDRAM */
	h=((spr->h+7)/8);				/* GET REAL HEIGHT IN VIDRAM */

	if ((spr->flags & COL256)!=0)		/* SPRITE IS 256 COLOUR SO HALF WIDTH */
	{
		w=w/2;
	}

	if ((spr->flags & (COL16BIT+COL256))==0)	/* SPRITE IS 16 COLOUR SO QUARTER WIDTH */
	{
		w=w/4;
	}

	location=Get_location_from_spr(spr);	/* GET LOCATION IN VIDRAM */

	if ((spr->flags & DONTXORVRAM)==0)		/* CHECK FOR TEMPVRAM */
	{
		Xor_VRAM_Hole(location,w,h);
	}
	else
	{
		spr->flags&=(0xffff-DONTXORVRAM); 
	}
	spr->clut=0;					/* RESET THE CLUT */
}

/**************************************
*     DELETE DOWNLOADED SPRITES       *
**************************************/

void
Delete_Last_Downloaded_sprites(void)
{

RECT	rect;

	for (tempspritenumber=0;tempspritenumber<MAXTEMPSPRITES;tempspritenumber++)
	{
		if ((tempsprites[tempspritenumber].spr)!=0)
		{
			if ((tempsprites[tempspritenumber].spr->flags & TEMPVRAM)!=0)
			{
				Delete_Spr_One_Live(tempsprites[tempspritenumber].spr,0);
			}
			tempsprites[tempspritenumber].spr=0;	/* RESET THIS LIVE SPRITE */
			tempsprites[tempspritenumber].pal.w=0;	/* RESET THIS LIVE SPRITE */
		}
	}
}

