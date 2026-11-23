
#ifndef	__H_SPRITE2
#define	__H_SPRITE2

/**************************************
*          DECLARE ROUTINES           *
**************************************/

extern	FLAG		Sprite_Collision(sprite_sequence seq,int sprite_no,int x,int y);
extern	FLAG		Collision_At(LONG x,LONG y);
extern	void		Build_Craft_Mask(SPRITE *spr);	/* RESTORE: THE CRAFT'S SHAPE, FROM ITS S_SHIP FRAMES (SPRITE2.C) */
extern	FLAG		Craft_Mask_Collision(int x,int y,int frame);

/* RESTORE: THE SHOTS' SHAPES (SPRITE2.C Shot_Mask_Collision) */
typedef struct
{
	USHORT	nframes;		/* FRAMES MADE (0: NOT LOADED YET) */
	USHORT	frames;			/* FRAMES THE TABLES HOLD */
	USHORT	rows;			/* ROWS A FRAME THE TABLES HOLD */
	SHORT	yoff;			/* h/2 + 2 OF THE FIRST FRAME, AS Print_Sprites */
	UBYTE	*rw;			/* A BYTE A LINE: dx IN THE HIGH NIBBLE, lx-1 IN THE LOW, 0xFF EMPTY */
	signed char	*u;
	signed char	*v;
	UBYTE	*h;
} SHOTMASK;
extern	SHOTMASK	shotguided_mask,shotbuck01_mask,shotbuck02_mask,shoteball_mask,shotmine_mask,shotspfire_mask,shotchlink_mask;
extern	void		Build_Shot_Mask(SHOTMASK *m,SPRITE *spr);
extern	FLAG		Shot_Mask_Collision(SHOTMASK *m,int x,int y,int frame);

#endif

