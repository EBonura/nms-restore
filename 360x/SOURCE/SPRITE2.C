
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE VARIABLES          *
**************************************/

UBYTE	right_byte_masks[9]={
						0x00,		//0
						0x80,		//1
						0xc0,		//2
						0xe0,		//3
						0xf0,		//4
						0xf8,		//5
						0xfc,		//6
						0xfe,		//7
						0xff		//8
						};

/**************************************
*     CHECK FOR SPRITE COLLISION      *
**************************************/

FLAG
Sprite_Collision(sprite_sequence seq,int sprite_no,int x,int y)
{

//SPRITE*	spr;

//UBYTE	*source;
//SHORT	*sourcew;

//UBYTE	mask;
//UBYTE	bit;



LONG	newx,newy;


	int i;
	int ix,iy;
	int dx,lx;
	int disp;
	int width,height;









//	spr=spr_S_SURVS;

//	x+=spr->x;
//	y+=spr->y;
	width=32;		//spr->w;
	height=32;		//spr->h;

	newx=x;
	newy=y;

//	i=seq.map[sprite_no];
//	source=seq.data;
//	sourcew=(short *) (source+i);
//	x+=(int ) sourcew[0];
//	y+=(int ) sourcew[1];
//	width =sourcew[2];
//	height=sourcew[3];

//	i+=8;

//	for (iy=0;iy<height;iy++)
//	{
//		for (ix=0;ix<width;ix++)
//		{
			if (Collision_At(newx,newy)==TRUE) return TRUE;
//		}
//	}
	return FALSE;







#if 0



//		dx=(unsigned int) source[i++];	// x distance to first pixel
//		lx=(unsigned int) source[i++];	// length of line
//		i++;
//		i+=lx;

		if( ((y+iy)>0) && ((y+iy)<mapheight) )
		{
			disp=x+dx+mapwidth*(y+iy);
			bit=1<<(7-(disp&7));
			disp=disp>>3;
			mask=levelcol[disp++];
								// left edge (slow bit)
			while(lx>0 && bit>0)
			{
				if( (mask & bit) !=0 ) return TRUE;
				bit=bit>>1;
				lx--;
			}
								// middle (could be faster) (could be rep scasb)
			while(lx>8)
			{
				mask=levelcol[disp++];
				if(mask!=0) return TRUE;
				lx-=8;
			}
								// end edge
			if(lx>0)
			{	
				mask=levelcol[disp++];
				bit=1<<7;
				if( (mask&right_byte_masks[lx])!=0 ) return TRUE;
			}
		}
	}
	return FALSE;

#endif
}

/**************************************
*     CHECK FOR COLLISION AT X,Y      *
**************************************/

#ifdef X360_TEST_COLBOUNDS
LONG	colmaxread=-1;				/* RESTORE: TEST BUILDS ONLY */
LONG	colbeyond=0;
#endif

FLAG
Collision_At(LONG x,LONG y)
{

UBYTE	mask;
UBYTE	bit;
LONG	pos;

	/* RESTORE: bug fix. Missions 7 to 9's collision maps (LEVELnC.BIN,
	   88200 bytes) are shorter than their maps (992 x 720 / 8 = 89280), and
	   Move_Fire tests a point before checking it is on the map, so a test
	   could read past the loaded file. A position outside the file is empty;
	   inside it the result is NMS's. */
	if ((x<0) || (x>=colwidth)) return TRUE;			/* RESTORE: bug fix, LEFT OF OR PAST THE COLLISION MAP'S EDGES (SEE colwidth, SCROLLER.C) IS ROCK, NOT THE NEXT ROW */
	pos=x+(y*colwidth);				/* RESTORE: bug fix, ROWS ARE colwidth APART */
	if ((pos<0) || ((pos>>3)>=levelcolsize))
	{
#ifdef X360_TEST_COLBOUNDS
		colbeyond++;				/* TEST BUILDS: TESTS OUTSIDE THE FILE */
#endif
		return FALSE;
	}
#ifdef X360_TEST_COLBOUNDS
	if ((pos>>3)>colmaxread) colmaxread=pos>>3;	/* TEST BUILDS: HIGHEST BYTE READ */
#endif

	mask=levelcol[(pos>>3)];
	bit=1<<(7-(pos&7));				/* RESTORE: bug fix, THE BIT OF pos: A ROW NEED NOT START ON A BYTE (980 PIXELS) */
	if((mask&bit)!=0) return TRUE;
	return FALSE;

}



/**************************************
*   THE CRAFT'S SHAPE AGAINST THE MAP  *
**************************************/

/* RESTORE: completion, rebuilt from NMS's PC version (owner's decision 2026-11-13: "NMS's PC version tests the
   whole sprite shape: let's restore that as well"). THE PC'S sprite_collision (MAKEBACK/SPRITE.C:341) TESTS EVERY
   LINE OF THE CRAFT'S FRAME AGAINST THE COLLISION MAP: A LINE IS dx (DISTANCE TO ITS FIRST PIXEL) AND lx (ITS
   LENGTH), FROM THE SPRITE DATA'S LINE HEADERS (MAKEBACK/SPRITE.C:360-366). THE PS1 NEVER LOADS THAT DATA (THE PC'S
   FORMAT IS NOT ON THE DISC; THE PS1'S COPY OF THE ROUTINE, Sprite_Collision ABOVE, IS #if 0 AND ENDS IN A POINT).
   THE LINES ARE MADE HERE, AT LOAD, FROM THE PS1'S OWN S_SHIP IMAGES (CRAFT_MASK, LOADDATA.C): FOR EACH LINE OF
   A FRAME, dx IS ITS FIRST PIXEL THAT IS NOT CLEAR (INDEX 0) AND lx RUNS TO ITS LAST ONE. THE PC'S ENCODER IS NOT
   IN THE ARCHIVES, SO WHETHER lx ENDS AT THE FIRST RUN OF A LINE WITH A GAP OR AT ITS LAST PIXEL IS NOT KNOWN; THIS IS
   THE SECOND (A GAP IS TESTED AS PART OF THE LINE), WHICH IS NEVER SHORTER THAN THE SHAPE. THE ROUTINE BELOW IS THE
   PC'S, WITH THE PS1'S BOUNDS (levelcolsize) AND ITS DRAWING POSITION (Print_Sprites: x + u, y + h/2 + 2 + v). */

#define	CRAFT_MASK_FRAMES	64
#define	CRAFT_MASK_ROWS		14

static UBYTE	craftmask_dx[CRAFT_MASK_FRAMES][CRAFT_MASK_ROWS];	/* FIRST PIXEL OF EACH LINE */
static UBYTE	craftmask_lx[CRAFT_MASK_FRAMES][CRAFT_MASK_ROWS];	/* LENGTH OF EACH LINE, 0 = EMPTY */
static SHORT	craftmask_u[CRAFT_MASK_FRAMES];
static SHORT	craftmask_v[CRAFT_MASK_FRAMES];
static UBYTE	craftmask_h[CRAFT_MASK_FRAMES];
static SHORT	craftmask_yoff;						/* h/2 + 2 OF THE FIRST FRAME, AS Print_Sprites */
static UBYTE	craftmask_raw[512];					/* ONE DECODED FRAME */
static FLAG	craftmask_ready=FALSE;

void
Build_Craft_Mask(SPRITE *spr)
{
	int	n,r,x,w,h,stride,first,last;
	UBYTE	*raw;

	craftmask_yoff=(spr->h/2)+2;
	for (n=0;n<CRAFT_MASK_FRAMES;n++)
	{
		w=spr->w;
		h=spr->h;
		if (h>CRAFT_MASK_ROWS) h=CRAFT_MASK_ROWS;
		stride=(((w-1)/4)+1)*2;				/* BYTES A LINE: 4 BITS A PIXEL, WHOLE 16 BIT WORDS */

		raw=(UBYTE *)spr->image;
		if ((spr->flags & BITSTREAM)!=0)		/* AS Add_Sprite_Frame (SPRITE.C) DECODES IT */
		{
			Bit2raw(raw,craftmask_raw);
			raw=craftmask_raw;
		}
		if ((spr->flags & RUNLENGTH)!=0)
		{
			Rle2raw(raw,craftmask_raw);
			raw=craftmask_raw;
		}

		for (r=0;r<h;r++)
		{
			first=-1;
			last=-1;
			for (x=0;x<w;x++)
			{
				if (((raw[r*stride+(x>>1)]>>((x&1)*4)) & 15)!=0)	/* THE LEFT PIXEL IS THE LOW NIBBLE */
				{
					if (first<0) first=x;
					last=x;
				}
			}
			craftmask_dx[n][r]=(first<0)?0:first;
			craftmask_lx[n][r]=(first<0)?0:(last-first+1);
		}
		craftmask_h[n]=h;
		craftmask_u[n]=spr->u;
		craftmask_v[n]=spr->v;

		if ((spr->flags & LASTSPR)!=0) break;
		spr++;
	}
	craftmask_ready=TRUE;
}

FLAG
Craft_Mask_Collision(int x,int y,int frame)
{
	int	iy,lx,h;
	LONG	pos;
	LONG	disp;
	UBYTE	mask;
	UBYTE	bit;

	if (craftmask_ready==FALSE) return FALSE;
	frame&=(CRAFT_MASK_FRAMES-1);

	x+=craftmask_u[frame];
	y+=craftmask_yoff+craftmask_v[frame];
	h=craftmask_h[frame];

	for (iy=0;iy<h;iy++)
	{
		lx=craftmask_lx[frame][iy];
		if (lx==0) continue;
		if (((y+iy)>0) && ((y+iy)<mapheight))
		{
			pos=x+craftmask_dx[frame][iy]+(colwidth*(y+iy));	/* RESTORE: bug fix, colwidth */
			if (pos<0) continue;
			bit=1<<(7-(pos&7));
			disp=pos>>3;
			if (disp>=levelcolsize) continue;
			mask=levelcol[disp++];
			while (lx>0 && bit>0)			/* LEFT EDGE (SLOW BIT) */
			{
				if ((mask & bit)!=0) return TRUE;
				bit=bit>>1;
				lx--;
			}
			while (lx>8 && disp<levelcolsize)	/* MIDDLE */
			{
				mask=levelcol[disp++];
				if (mask!=0) return TRUE;
				lx-=8;
			}
			if (lx>0 && disp<levelcolsize)		/* END EDGE */
			{
				mask=levelcol[disp];
				if ((mask & right_byte_masks[lx])!=0) return TRUE;
			}
		}
	}
	return FALSE;
}


/**************************************
*   THE SHOTS' SHAPES AGAINST THE MAP  *
**************************************/

/* RESTORE: completion, rebuilt from NMS's PC version (owner's decision 2026-11-13: the craft's whole-shape test, and "every gameplay feature completed in other
   ports"). THE PC'S move_fire TESTS EACH SHOT'S SPRITE SHAPE AGAINST THE MAP (sprite_collision, MAKEBACK/SPRITE.C:341, ON THE SHOT'S FRAME), WHERE THE PS1
   TESTED THE SHOT'S ONE POINT. THE SHAPE IS MADE EXACTLY AS THE CRAFT'S IS (Build_Craft_Mask ABOVE) FROM EACH SHOT'S OWN IMAGES, AT LOAD, BEFORE THEY ARE
   FREED: FOR EACH LINE OF A FRAME, dx IS ITS FIRST PIXEL THAT IS NOT CLEAR AND lx RUNS TO ITS LAST ONE. Shot_Mask_Collision IS Craft_Mask_Collision ON THESE TABLES.
   SEVEN SHOT SPRITES, THEIR TABLES FIXED SIZE (FRAMES x ROWS: GUIDED 64x6, BUCK01 32x8, BUCK02 16x7, EBALL 64x10, MINE 32x11, S_SPFIRE 40x10, CHLINK 16x3: THE CHAIN SHOT,
   THE PC'S NAME_CHAINB, WHOSE WALL TEST IS THE SAME sprite_collision, MAKEBACK/FIRE.C:522). */

#define	SHOT_ROWS_MAX	12

#define	SHOTMASK_STORE(name,frames,rows) \
	static UBYTE	name##_rw[frames][rows]; \
	static signed char	name##_u[frames]; \
	static signed char	name##_v[frames]; \
	static UBYTE	name##_h[frames]; \
	SHOTMASK	name##_mask={0,frames,rows,0,&name##_rw[0][0],name##_u,name##_v,name##_h};

SHOTMASK_STORE(shotguided,64,6)
SHOTMASK_STORE(shotbuck01,32,8)
SHOTMASK_STORE(shotbuck02,16,7)
SHOTMASK_STORE(shoteball,64,10)
SHOTMASK_STORE(shotmine,32,11)
SHOTMASK_STORE(shotspfire,40,10)
SHOTMASK_STORE(shotchlink,16,3)

static UBYTE	shotmask_raw[512];

void
Build_Shot_Mask(SHOTMASK *m,SPRITE *spr)
{
	int	n,r,x,w,h,stride,first,last;
	UBYTE	*raw;

	m->yoff=(spr->h/2)+2;
	for (n=0;n<m->frames;n++)
	{
		w=spr->w;
		h=spr->h;
		if (h>m->rows) h=m->rows;
		stride=(((w-1)/4)+1)*2;				/* BYTES A LINE: 4 BITS A PIXEL, WHOLE 16 BIT WORDS */

		raw=(UBYTE *)spr->image;
		if ((spr->flags & BITSTREAM)!=0)
		{
			Bit2raw(raw,shotmask_raw);
			raw=shotmask_raw;
		}
		if ((spr->flags & RUNLENGTH)!=0)
		{
			Rle2raw(raw,shotmask_raw);
			raw=shotmask_raw;
		}

		for (r=0;r<h;r++)
		{
			first=-1;
			last=-1;
			for (x=0;x<w;x++)
			{
				if (((raw[r*stride+(x>>1)]>>((x&1)*4)) & 15)!=0)
				{
					if (first<0) first=x;
					last=x;
				}
			}
			/* ONE BYTE A LINE (THE ROOM UNDER THE STACK IS A FEW KB): THE HIGH NIBBLE IS dx (0 TO 15), THE LOW ONE lx-1 (1 TO 16 PIXELS), 0xFF AN EMPTY LINE */
			m->rw[n*m->rows+r]=(first<0)?0xFF:((first<<4)|(last-first));
		}
		m->h[n]=h;
		m->u[n]=spr->u;
		m->v[n]=spr->v;
		m->nframes=n+1;

		if ((spr->flags & LASTSPR)!=0) break;
		spr++;
	}
}

FLAG
Shot_Mask_Collision(SHOTMASK *m,int x,int y,int frame)
{
	int	iy,lx,h,dxs;
	LONG	pos;
	LONG	disp;
	UBYTE	mask;
	UBYTE	bit;

	if ((m->nframes==0) || (frame<0) || (frame>=m->nframes)) frame=0;
	if (m->nframes==0) return Collision_At(x,y);		/* NO SHAPE (NOT LOADED): THE POINT */

	x+=m->u[frame];
	y+=m->yoff+m->v[frame];
	h=m->h[frame];

	for (iy=0;iy<h;iy++)
	{
		lx=m->rw[frame*m->rows+iy];
		if (lx==0xFF) continue;
		dxs=lx>>4;
		lx=(lx&15)+1;
		if (((y+iy)>0) && ((y+iy)<mapheight))
		{
			pos=x+dxs+(colwidth*(y+iy));	/* RESTORE: bug fix, colwidth */
			if (pos<0) continue;
			bit=1<<(7-(pos&7));
			disp=pos>>3;
			if (disp>=levelcolsize) continue;
			mask=levelcol[disp++];
			while (lx>0 && bit>0)			/* LEFT EDGE (SLOW BIT) */
			{
				if ((mask & bit)!=0) return TRUE;
				bit=bit>>1;
				lx--;
			}
			while (lx>8 && disp<levelcolsize)	/* MIDDLE */
			{
				mask=levelcol[disp++];
				if (mask!=0) return TRUE;
				lx-=8;
			}
			if (lx>0 && disp<levelcolsize)		/* END EDGE */
			{
				mask=levelcol[disp];
				if ((mask & right_byte_masks[lx])!=0) return TRUE;
			}
		}
	}
	return FALSE;
}
