
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*           DECLARE DEFINES           *
**************************************/

#define	FOG_R	20
#define	FOG_G	20
#define	FOG_B	20

#define	SX		0
#define	SY		0
#define	SZ		0

#define	MULT		300

#define	TIM_HEADER	0x00000010

#define	NCUBE	45
#define	OBJMAX	NCUBE

/**************************************
*         DECLARE VARIABLES           *
**************************************/

GsRVIEW		view;
GsF_LIGHT	pslt[3];
GsFOGPARAM	dq;

GsDOBJ2		object[OBJMAX];
GsCOORDINATE2	objcoord[OBJMAX];
SVECTOR		Rot[OBJMAX];
SVECTOR		RotV[OBJMAX];
VECTOR		Trns[OBJMAX];
VECTOR		TrnsV[OBJMAX];

int			nobj;

int			dqf;
int			Bakuhatu;

UBYTE		*tex1;
UBYTE		*tmd1;

ULONG		*TmdBase;
int			CurrentTmd;

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Init_View_Title(void);
void	Init_Light_Title(int c_mode,int factor);
void	Init_Texture_Title(UBYTE *texaddr);
void	Init_Model_Title(UBYTE *tmdp);
void	Allocate_Cube(int n);
void	Draw_360X(void);
void	Move_Cubes(void);
void	changeFog(void);
void	changeTmd(void);

/**************************************
*       DATA FOR CUBE POSITIONS       *
**************************************/

int	cube_def_pos[132] ={

	(SX+0)*MULT,		(SY+0)*MULT,	(SZ+0)*MULT,	/* 3 */
	(SX+1)*MULT,		(SY+0)*MULT,	(SZ+0)*MULT,
	(SX+2)*MULT,		(SY+0)*MULT,	(SZ+0)*MULT,

	(SX+2)*MULT,		(SY+1)*MULT,	(SZ+0)*MULT,

	(SX+0)*MULT,		(SY+2)*MULT,	(SZ+0)*MULT,
	(SX+1)*MULT,		(SY+2)*MULT,	(SZ+0)*MULT,
	(SX+2)*MULT,		(SY+2)*MULT,	(SZ+0)*MULT,

	(SX+2)*MULT,		(SY+3)*MULT,	(SZ+0)*MULT,

	(SX+0)*MULT,		(SY+4)*MULT,	(SZ+0)*MULT,
	(SX+1)*MULT,		(SY+4)*MULT,	(SZ+0)*MULT,
	(SX+2)*MULT,		(SY+4)*MULT,	(SZ+0)*MULT,


	(SX+4)*MULT,		(SY+0)*MULT,	(SZ+0)*MULT,	/* 6 */
	(SX+5)*MULT,		(SY+0)*MULT,	(SZ+0)*MULT,
	(SX+6)*MULT,		(SY+0)*MULT,	(SZ+0)*MULT,

	(SX+4)*MULT,		(SY+1)*MULT,	(SZ+0)*MULT,

	(SX+4)*MULT,		(SY+2)*MULT,	(SZ+0)*MULT,
	(SX+5)*MULT,		(SY+2)*MULT,	(SZ+0)*MULT,
	(SX+6)*MULT,		(SY+2)*MULT,	(SZ+0)*MULT,

	(SX+4)*MULT,		(SY+3)*MULT,	(SZ+0)*MULT,
	(SX+6)*MULT,		(SY+3)*MULT,	(SZ+0)*MULT,

	(SX+4)*MULT,		(SY+4)*MULT,	(SZ+0)*MULT,
	(SX+5)*MULT,		(SY+4)*MULT,	(SZ+0)*MULT,
	(SX+6)*MULT,		(SY+4)*MULT,	(SZ+0)*MULT,


	(SX+8)*MULT,		(SY+0)*MULT,	(SZ+0)*MULT,	/* 0 */
	(SX+9)*MULT,		(SY+0)*MULT,	(SZ+0)*MULT,
	(SX+10)*MULT,		(SY+0)*MULT,	(SZ+0)*MULT,

	(SX+8)*MULT,		(SY+1)*MULT,	(SZ+0)*MULT,
	(SX+10)*MULT,		(SY+1)*MULT,	(SZ+0)*MULT,

	(SX+8)*MULT,		(SY+2)*MULT,	(SZ+0)*MULT,
	(SX+10)*MULT,		(SY+2)*MULT,	(SZ+0)*MULT,

	(SX+8)*MULT,		(SY+3)*MULT,	(SZ+0)*MULT,
	(SX+10)*MULT,		(SY+3)*MULT,	(SZ+0)*MULT,

	(SX+8)*MULT,		(SY+4)*MULT,	(SZ+0)*MULT,
	(SX+9)*MULT,		(SY+4)*MULT,	(SZ+0)*MULT,
	(SX+10)*MULT,		(SY+4)*MULT,	(SZ+0)*MULT,


	(SX+12)*MULT,		(SY+0)*MULT,	(SZ+0)*MULT,
	(SX+14)*MULT,		(SY+0)*MULT,	(SZ+0)*MULT,

	(SX+12)*MULT,		(SY+1)*MULT,	(SZ+0)*MULT,
	(SX+14)*MULT,		(SY+1)*MULT,	(SZ+0)*MULT,

	(SX+13)*MULT,		(SY+2)*MULT,	(SZ+0)*MULT,

	(SX+12)*MULT,		(SY+3)*MULT,	(SZ+0)*MULT,
	(SX+14)*MULT,		(SY+3)*MULT,	(SZ+0)*MULT,

	(SX+12)*MULT,		(SY+4)*MULT,	(SZ+0)*MULT,
	(SX+14)*MULT,		(SY+4)*MULT,	(SZ+0)*MULT,
};

/**************************************
*        SHOW THE TITLE SCREEN        *
**************************************/

void
Title_Screen(void)
{

UBYTE	*title;
SPRITE*	spr_TITLE;

SHORT	dropout;

UBYTE	used_vram;

	Jalloc_Init(&JALLOC,sonymalloc,(maxmalloc*1024)); /* INITIALISE NEW MALLOC ROUTINE */

	Init_TPages();
	Reset_Cluts();
	Resident_Tims();
	Loading_Message();			/* RESTORE: owner's decision, PLEASE WAIT while this screen loads (GENERAL.C) */

	used_vram=Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);
	Find_Hole_In_VRAM(32,32);

	Xor_VRAM_Hole(((0+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((1+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((2+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((3+11)<<16)+(used_vram & 0x00ff0000),32,28);
	Xor_VRAM_Hole(((4+11)<<16)+(used_vram & 0x00ff0000),32,28);

	title=LoadFile("../INTRO/360X.PSX");
	spr_TITLE = (SPRITE*)LoadFile("../INTRO/360X.SPR");
	Init_Spr_List(spr_TITLE,(ULONG)title);

	tex1=LoadFile("../INTRO/INTRO.TIM");
	tmd1=LoadFile("../INTRO/INTRO.TMD");

	Init_Texture_Title(tex1);
	Init_Model_Title(tmd1);
	Allocate_Cube(44);

	Init_View_Title();
	Init_Light_Title(0, 0xc0);

	selectno=0;

	dropout=60*10;

	Cls();

	while (fade!=0)
	{
		if (((Debounce1 & PaDenter)!=0) && (fade==128))	/* RESTORE: owner's decision, Start or Cross (PaDenter, 360X.H) */
		{
			Start_Fade_Down(-16);
			SsUtKeyOn(vab,SFX_EXPLOSION2,0,FXPITCH,0,sfxvolume,sfxvolume);
			selectno=INTOGAME;
		}

		if ((dropout==0) && (fade==128))
		{
			Start_Fade_Down(-16);
			selectno=INTOSHIP;
		}
		else dropout--;

		Start_Loop();

//		GsSetRefView(&view);
//		Draw_360X();
//		Move_Cubes();

		Print_At(232,208);
		if ((gamecycle MOD 16)<8) Print_Messl("PRESS START BUTTON");

		Print_Sprite(spr_TITLE,320,240,0);

		End_Loop();
	}

	VSync(0);
	SetDispMask(OFF);

}

/**************************************
*       INITIALISE SCREEN VIEW        *
**************************************/

void
Init_View_Title(void)
{

	view.vpx = 0;
	view.vpy = 0;
	view.vpz = -8192;
	view.vrx = 2048;
	view.vry = 0;
	view.vrz = 0;
	view.rz=0;

	view.super = WORLD;
	GsSetRefView(&view);
	GsSetProjection(1000);
	GsSetNearClip(100);
	GsSetLightMode(0);
}

/**************************************
*         INITIALISE LIGHTS           *
**************************************/

void
Init_Light_Title(c_mode, factor)
{
	if(c_mode == 0)
	{
		pslt[0].vx = 200;
		pslt[0].vy= 200;
		pslt[0].vz= 300;
		pslt[0].r = factor;
		pslt[0].g = factor;
		pslt[0].b = factor;
		GsSetFlatLight(0,&pslt[0]);
		
		pslt[1].vx = -50;
		pslt[1].vy= -1000;
		pslt[1].vz= 0;
		pslt[1].r=0x20;
		pslt[1].g=0x20;
		pslt[1].b=0x20;
		GsSetFlatLight(1,&pslt[1]);
		
		pslt[2].vx = -20;
		pslt[2].vy= 20;
		pslt[2].vz= 100;
		pslt[2].r=0x0;
		pslt[2].g=0x0;
		pslt[2].b=0x0;
		GsSetFlatLight(2,&pslt[2]);
	}
	else
	{
		pslt[0].vx = 200;
		pslt[0].vy= 100;
		pslt[0].vz= 0;
		pslt[0].r = factor;
		pslt[0].g = 0;
		pslt[0].b = 0;
		GsSetFlatLight(0,&pslt[0]);
		
		pslt[1].vx = -200;
		pslt[1].vy= 100;
		pslt[1].vz= 0;
		pslt[1].r=0;
		pslt[1].g=0;
		pslt[1].b=factor;
		GsSetFlatLight(1,&pslt[1]);
		
		pslt[2].vx = 0;
		pslt[2].vy= -200;
		pslt[2].vz= 0;
		pslt[2].r=0;
		pslt[2].g=factor;
		pslt[2].b=0;
		GsSetFlatLight(2,&pslt[2]);
	}       

	GsSetAmbient(ONE/2,ONE/2,ONE/2);
}

/**************************************
*       INITIALISE THE TEXTURE        *
**************************************/

void
Init_Texture_Title(UBYTE *texaddr)
{

RECT	rect1;
GsIMAGE	tim1;
int		i;

ULONG	*current;

	current=(ULONG *)texaddr;

	GsGetTimInfo((ULONG *)(texaddr+4),&tim1);
	rect1.x=tim1.px;
	rect1.y=tim1.py;
	rect1.w=tim1.pw;
	rect1.h=tim1.ph-16;
	LoadImage(&rect1,tim1.pixel);
	DrawSync(0);

	if((tim1.pmode>>3)&0x01)
	{
		rect1.x=tim1.cx;
		rect1.y=tim1.cy;
		rect1.w=tim1.cw;
		rect1.h=tim1.ch;
		LoadImage(&rect1,tim1.clut);
	}

}

/**************************************
*       INITIALISE THE MODELS         *
**************************************/

void
Init_Model_Title(UBYTE *address)
{

ULONG	*tmdp;

ULONG	size;
int		i;
int		tmdobjnum;

	tmdp=(ULONG *)address;

	tmdp++;
	GsMapModelingData(tmdp);
	tmdp++;
	tmdobjnum = *tmdp;
	tmdp++;
	TmdBase = tmdp;
}

/**************************************
*       INITIALISE THE MODELS         *
**************************************/

void
Allocate_Cube(int n)
{

int				x, y, z;
int				i;
int				*posp;
GsDOBJ2			*objp;
GsCOORDINATE2		*coordp;

	posp = cube_def_pos;
	objp = object;
	coordp = objcoord;
	nobj = 0;
	for(i = 0; i < NCUBE; i++)
	{

		GsLinkObject2((u_long)TmdBase, objp, CurrentTmd);
		GsInitCoordinate2(WORLD, coordp);
		objp->coord2 = coordp;
		objp->attribute = 0;
		coordp++;
		objp++;

		Trns[i].vx = *posp++;
		Trns[i].vy = *posp++;
		Trns[i].vz = *posp++;
		Rot[i].vx = 0;
		Rot[i].vy = 0;
		Rot[i].vz = 1024;

		TrnsV[i].vx = 0;
		TrnsV[i].vy = 0;
		TrnsV[i].vz = 0;
		RotV[i].vx = rand()/300;
		RotV[i].vy = rand()/300;
		RotV[i].vz = rand()/300;

		nobj++;
	}
}

/**************************************
*         DRAW THE OBJECTS            *
**************************************/

void
Draw_360X(void)
{

int		i;
GsDOBJ2	*objp;
MATRIX	LsMtx;

	nobj=44;

	objp = object;
	for(i = 0; i < nobj; i++)
	{

		RotMatrix(Rot+i, &(objp->coord2->coord));
		TransMatrix(&(objp->coord2->coord), &Trns[i]);
		objp->coord2->flg = 0;
		GsGetLs(objp->coord2, &LsMtx);
		GsSetLsMatrix(&LsMtx);
		GsSetLightMatrix(&LsMtx);
		GsSortObject2(objp,&Wot[screentoggle],17-OTLENGTH);

		objp++;
	}
}

/**************************************
*        MOVE THE CUBES ABOUT         *
**************************************/

void
Move_Cubes(void)
{
	int i;
	GsDOBJ2   *objp;

	if((Pad1 & PADLleft)>0)
	{
		view.vpy-=512;
	}
	if((Pad1 & PADLright)>0)
	{
		view.vpy+=512;
	}
	if((Pad1 & PADLup)>0) view.vpz += 100;
	if((Pad1 & PADLdown)>0) view.vpz -= 100;

	if((Pad1 & PADk)>0) return;
	if((Debounce1 & PADRleft)>0) changeFog();
	if((Debounce1 & PADRup)>0) changeTmd();
	if((Debounce1 & PADn)>0) Bakuhatu = 1;
	if((Debounce1 & PADl)>0) Allocate_Cube(NCUBE);

	objp = object;
	for(i = 0; i < nobj; i++)
	{

		if(Bakuhatu == 1)
		{

			RotV[i].vx *= 3;
			RotV[i].vy *= 3;
			RotV[i].vz *= 3;

			TrnsV[i].vx = objp->coord2->coord.t[0]/4+
						(rand()-16384)/200;     
			TrnsV[i].vy = objp->coord2->coord.t[1]/6+
						(rand()-16384)/200-200;
			TrnsV[i].vz = objp->coord2->coord.t[2]/4+
						(rand()-16384)/200;
		}
		else if(Bakuhatu > 1)
		{
			if(Trns[i].vy > 3000)
			{
				Trns[i].vy = 3000-(Trns[i].vy-3000)/2;
				TrnsV[i].vy = -TrnsV[i].vy*6/10;
			}
			else
			{
				TrnsV[i].vy += 20*2;
			}

			if((TrnsV[i].vy < 70)&&(TrnsV[i].vy > -70)&&
			   (Trns[i].vy > 2800)) {
				Trns[i].vy = 3000;
				TrnsV[i].vy = 0;

				RotV[i].vx *= 95/100;
				RotV[i].vy *= 95/100;
				RotV[i].vz *= 95/100;
			}


			TrnsV[i].vx = TrnsV[i].vx*97/100;
			TrnsV[i].vz = TrnsV[i].vz*97/100;
		}
/*
		Rot[i].vx += RotV[i].vx;
		Rot[i].vy += RotV[i].vy;
		Rot[i].vz += RotV[i].vz;
*/
		Trns[i].vx += TrnsV[i].vx;
		Trns[i].vy += TrnsV[i].vy;
		Trns[i].vz += TrnsV[i].vz;

		objp++;
	}

	if(Bakuhatu == 1)
		Bakuhatu++;

}




void changeFog(void)
{
	if(dqf)
	{
		GsSetLightMode(0);
		dqf = 0;
	}
	else
	{
		dq.dqa = -600;
		dq.dqb = 4096*4096;
		dq.rfc = FOG_R;
		dq.gfc = FOG_G;
		dq.bfc = FOG_B;
		GsSetFogParam(&dq);
		GsSetLightMode(1);
		dqf = 1;
	}
}

void changeTmd(void)
{
	u_long *tmdp;
	GsDOBJ2 *objp;
	int i;

	CurrentTmd++;
	if(CurrentTmd == 4)
	{
		CurrentTmd = 0;
	}
	objp = object;
	for(i = 0; i < nobj; i++)
	{
		GsLinkObject2((u_long)TmdBase, objp, CurrentTmd);
		objp++;
	}

	switch(CurrentTmd)
	{
	    case 0:			/* FLAT */
		Init_Light_Title(0, 0xc0);
		break;
	    case 1:			/* FLAT */
		Init_Light_Title(0, 0xc0);
		break;
	    case 2:			/* GORAUD */
		Init_Light_Title(1, 0xff);
		break;
	    case 3:			/* PHONG */
		Init_Light_Title(0, 0xff);
		break;
	}
}






#if 0

#define TIM_ADDR 0x80020000
#define TMD_ADDR 0x80010000

u_long *TmdBase;



#define DISTANCE 600


int Bakuhatu;
int dqf;

void drawCubes();
int moveCubes();
void initModelingData();
void allocCube();
void initAll();
void initTexture();
void initLight();
void changeFog();
void changeTmd();

main()
{

	Bakuhatu = 0;
	PadData = 0;
	CurrentTmd = 0;
	dqf = 0;
	initModelingData(TMD_ADDR);
	initTexture(TIM_ADDR);
	allocCube(NCUBE);

	while(1)
	{
		moveCubes();
		GsSetRefView(&view);
		drawCubes();
		pollhost();
	}
}

#endif

