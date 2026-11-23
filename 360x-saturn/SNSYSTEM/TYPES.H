//--------------------------------------------------------------------------------------------------------------------------------
// Type definitions for 360x
//--------------------------------------------------------------------------------------------------------------------------------

typedef unsigned long	ulong;	 // unsigned 32-bit quantity
typedef short				word;	    // signed 16-bit quantity
typedef unsigned short	uword;	 // unsigned 16-bit quantity
typedef signed char		byte;	    // signed 8-bit quantity
typedef unsigned char	ubyte;	 // unsigned 8-bit quantity
typedef char				text;	    // unsigned 8-bit quantity
typedef unsigned char	flag;	    // unsigned 8-bit quantity


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Global Structures */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

struct	SpGrabInfo
{
	Sint16	charno;		/* The sprite number */
	Uint8		OnScreen;	/* Is the slot being used ? */
};
typedef	struct SpGrabInfo	SpType;

//--------------------------------------------------------------------------------------------------------------------------------

struct	Slots
{
	SpType	S8x8[Num8x8+1];
	SpType	S16x16[Num16x16+1];
	SpType	S24x24[Num24x24+1];
	SpType	S32x32[Num32x32+1];
	SpType	S48x48[Num48x48+1];
	SpType	S64x64[Num64x64+1];
	SpType	S80x80[Num80x80+1];
	SpType	S96x96[Num96x96+1];
	SpType	S128x128[Num128x128+1];
};
typedef	struct Slots	SlotType;

//--------------------------------------------------------------------------------------------------------------------------------

struct	spritedata
{
	Uint16	texno;
	Sint16	offx;
	Sint16	offy;
	Uint8		cx;
	Uint8		cy;
	Uint8		cry;
};
typedef	struct	spritedata	SpriteType;

//--------------------------------------------------------------------------------------------------------------------------------

struct	swapstruct
{
	Uint32	offset;
	Uint16	ClearArea;
	Uint16	Delta;
	Uint16	texno;
};
typedef	struct	swapstruct	SwapType;

//--------------------------------------------------------------------------------------------------------------------------------

struct	transstruct
{
	Uint32	soffset;
	Uint32	toffset;
	Uint16	size;
};
typedef	struct transstruct	TransType;

//--------------------------------------------------------------------------------------------------------------------------------

struct	retstruct
{
	Sint16	Return;
	Uint8		cx,cy;
	Uint8		clx;
};
typedef	struct retstruct		RetType;

//--------------------------------------------------------------------------------------------------------------------------------

struct	starstruct
{
	Sint16	x,y,z;
	Sint16	radius;
  	ANGLE		angle;
};

typedef struct starstruct	StarType;

//--------------------------------------------------------------------------------------------------------------------------------

typedef struct	{
				Uint8		fonttype;
				Uint16	x;
				Uint16	y;
				Uint8		*datastuff;
				}CREDITS;

//--------------------------------------------------------------------------------------------------------------------------------

typedef struct	{
				Uint16	control1;
				Uint16	control2;
				Uint16	control3;
				Uint16	control4;
				Uint8		fonttype;
				Uint8		*datastuff;
				Uint8		nullstring;
				}TEXT;

//--------------------------------------------------------------------------------------------------------------------------------

typedef struct	{
				Uint32	highscore;
				Uint8		highname[12];
				Uint8		nullstring;
				}HIGH;

//--------------------------------------------------------------------------------------------------------------------------------

struct particles_st
{
	FIXED		x;
	FIXED		y;
	FIXED		vx;
	FIXED		vy;
	FIXED		mass;
};

typedef	struct particles_st gravitype;

//--------------------------------------------------------------------------------------------------------------------------------

struct fibres
{
	FIXED		x;
	FIXED		y;
	FIXED		vx;
	FIXED		vy;
};

typedef	struct fibres Fibre;

//--------------------------------------------------------------------------------------------------------------------------------

struct levelstruct
{
	Uint8  	*mapname;
	Uint8		*palettename;
	Uint8		*blockname;
};

typedef	struct levelstruct LevelType;

//--------------------------------------------------------------------------------------------------------------------------------

typedef struct
{
	FIXED		x;
	FIXED		y;
	FIXED		vx;
	FIXED		vy;
	FIXED		mass;
}BALL;

//--------------------------------------------------------------------------------------------------------------------------------

typedef struct
{
	long	ai;
	flag	defined;
	flag	shootable;
	flag	bouncy;
} attribstruc;

//--------------------------------------------------------------------------------------------------------------------------------

typedef struct
{
	long name;
	long action;
} actionstruc;

//--------------------------------------------------------------------------------------------------------------------------------

struct	AI_DATASTRUCT
{
	double	dtemp;
	long 		logic_index;
	flag 		end_slice;
	long 		*logic;
	long 		command;
	long 		temp1,temp2,temp3,temp4,temp5,temp6;
	long		spr_i;
};

//--------------------------------------------------------------------------------------------------------------------------------

typedef	struct	AI_DATASTRUCT	AI_DATATYPE;

//--------------------------------------------------------------------------------------------------------------------------------

typedef struct {
	int	*map;
	char	*data;
	long	frames;
	short	offset;
} sprite_sequence ;

//--------------------------------------------------------------------------------------------------------------------------------

typedef struct
{
	char 	*name;
	long	logic;
} names_and_indexs;


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Pointers to function types */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

typedef	void		(*SlotFunc)(SlotType *SlotData,Uint16 Sprite,SwapType	*SwapInfo);

//--------------------------------------------------------------------------------------------------------------------------------

typedef	void		(*CXS)(Sint16 Frame,SlotType *SlotData,RetType *RetInfo);

//--------------------------------------------------------------------------------------------------------------------------------

typedef	void		(*UPDATE)(Uint32 mapindex,Uint32 vramindex);

//--------------------------------------------------------------------------------------------------------------------------------

typedef	void		(*Multi)(char *,...);

//--------------------------------------------------------------------------------------------------------------------------------

typedef	void		(*Command)(void);

//--------------------------------------------------------------------------------------------------------------------------------

typedef	void		(*SlaveFunc)(void);

//--------------------------------------------------------------------------------------------------------------------------------

typedef	void		(*CommandType)(AI_DATATYPE *data);

//--------------------------------------------------------------------------------------------------------------------------------
