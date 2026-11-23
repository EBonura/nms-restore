
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*           DECLARE DEFINES           *
**************************************/

#define	MAXPLANETS		6		/* MAXIMUM OBJECTS */

#define	TELETYPEPLANET1	7
#define	TELETYPEPLANET2	7
#define	TELETYPEPLANET3	7
#define	TELETYPEPLANET4	7
#define	TELETYPEPLANET5	7
#define	TELETYPEPLANET6	7
#define	TELETYPEPLANET7	7
#define	TELETYPEPLANET8	7
#define	TELETYPEPLANET9	7

// BOX 1 COORDINATES

#define	BOX1POINT1X	456-(320)
#define	BOX1POINT1Y	172-(120)

#define	BOX1POINT2X	584-(320)
#define	BOX1POINT2Y	172-(120)

#define	BOX1POINT3X	584-(320)
#define	BOX1POINT3Y	212-(120)

#define	BOX1POINT4X	456-(320)
#define	BOX1POINT4Y	212-(120)

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Init_View(void);
void	Init_Light(void);

/**************************************
*          DECLARE STRUCTURES         *
**************************************/

GsDOBJ2		object[MAXPLANETS];		/* ALLOCATE OBJECTS */
GsCOORDINATE2	DWorld,DBall0,DBall1,DBall2,DBall3,DBall4,DBall5;
GsFOGPARAM	FOG1;
int			ambient;

GsRVIEW2		view;
GsF_LIGHT	pslt[3];
RECT		psdisp;

SVECTOR		PWorld;

MATRIX		GsIDMATRIX,GsWSMATRIX;
VECTOR		ModelScale[MAXPLANETS];

UBYTE	*texture1;
UBYTE	*texture2;
UBYTE	*texture3;
UBYTE	*texture4;
UBYTE	*texture5;
UBYTE	*texture6;

UBYTE	*tmd1;
UBYTE	*tmd2;
UBYTE	*tmd3;
UBYTE	*tmd4;
UBYTE	*tmd5;
UBYTE	*tmd6;

/**************************************
*          PLANET BRIEFINGS           *
**************************************/

TEXT planettext1[]={22, 448, 3*7+3,-1,1,"PLANET : ALIEUS",0,
				22, 448, 7*7+3,-1,1,"MASS: 2.5 GLBS",0,
				22, 448, 9*7+3,-1,1,"DIAMETER : 1.0 CLKS",0,
				22, 448,11*7+3,-1,1,"FORM: MORPHUS",0,
				22, 448,13*7+3,-1,1,"LIFEFORMS: BASIC",0,
				22, 448,16*7+3,-1,1,"GRAVITY: 4.3 G",0,
				22, 448,19*7+3,-1,1,"OXYGEN: NEGATIVE",0,
				};

TEXT planettext2[]={22, 448, 3*7+3,-1,1,"PLANET : LEVEL 2",0,
				22, 448, 7*7+3,-1,1,"MASS: 2.5 GLBS",0,
				22, 448, 9*7+3,-1,1,"DIAMETER : 1.0 CLKS",0,
				22, 448,11*7+3,-1,1,"FORM: MORPHUS",0,
				22, 448,13*7+3,-1,1,"LIFEFORMS: BASIC",0,
				22, 448,16*7+3,-1,1,"GRAVITY: 4.3 G",0,
				22, 448,19*7+3,-1,1,"OXYGEN: NEGATIVE",0,
				};

TEXT planettext3[]={22, 448, 3*7+3,-1,1,"PLANET : LEVEL 3",0,
				22, 448, 7*7+3,-1,1,"MASS: 2.5 GLBS",0,
				22, 448, 9*7+3,-1,1,"DIAMETER : 1.0 CLKS",0,
				22, 448,11*7+3,-1,1,"FORM: MORPHUS",0,
				22, 448,13*7+3,-1,1,"LIFEFORMS: BASIC",0,
				22, 448,16*7+3,-1,1,"GRAVITY: 4.3 G",0,
				22, 448,19*7+3,-1,1,"OXYGEN: NEGATIVE",0,
				};

TEXT planettext4[]={22, 448, 3*7+3,-1,1,"PLANET : LEVEL 4",0,
				22, 448, 7*7+3,-1,1,"MASS: 2.5 GLBS",0,
				22, 448, 9*7+3,-1,1,"DIAMETER : 1.0 CLKS",0,
				22, 448,11*7+3,-1,1,"FORM: MORPHUS",0,
				22, 448,13*7+3,-1,1,"LIFEFORMS: BASIC",0,
				22, 448,16*7+3,-1,1,"GRAVITY: 4.3 G",0,
				22, 448,19*7+3,-1,1,"OXYGEN: NEGATIVE",0,
				};

TEXT planettext5[]={22, 448, 3*7+3,-1,1,"PLANET : LEVEL 5",0,
				22, 448, 7*7+3,-1,1,"MASS: 2.5 GLBS",0,
				22, 448, 9*7+3,-1,1,"DIAMETER : 1.0 CLKS",0,
				22, 448,11*7+3,-1,1,"FORM: MORPHUS",0,
				22, 448,13*7+3,-1,1,"LIFEFORMS: BASIC",0,
				22, 448,16*7+3,-1,1,"GRAVITY: 4.3 G",0,
				22, 448,19*7+3,-1,1,"OXYGEN: NEGATIVE",0,
				};

TEXT planettext6[]={22, 448, 3*7+3,-1,1,"PLANET : LEVEL 6",0,
				22, 448, 7*7+3,-1,1,"MASS: 2.5 GLBS",0,
				22, 448, 9*7+3,-1,1,"DIAMETER : 1.0 CLKS",0,
				22, 448,11*7+3,-1,1,"FORM: MORPHUS",0,
				22, 448,13*7+3,-1,1,"LIFEFORMS: BASIC",0,
				22, 448,16*7+3,-1,1,"GRAVITY: 4.3 G",0,
				22, 448,19*7+3,-1,1,"OXYGEN: NEGATIVE",0,
				};

TEXT planettext7[]={22, 448, 3*7+3,-1,1,"PLANET : LEVEL 7",0,
				22, 448, 7*7+3,-1,1,"MASS: 2.5 GLBS",0,
				22, 448, 9*7+3,-1,1,"DIAMETER : 1.0 CLKS",0,
				22, 448,11*7+3,-1,1,"FORM: MORPHUS",0,
				22, 448,13*7+3,-1,1,"LIFEFORMS: BASIC",0,
				22, 448,16*7+3,-1,1,"GRAVITY: 4.3 G",0,
				22, 448,19*7+3,-1,1,"OXYGEN: NEGATIVE",0,
				};

TEXT planettext8[]={22, 448, 3*7+3,-1,1,"PLANET : LEVEL 8",0,
				22, 448, 7*7+3,-1,1,"MASS: 2.5 GLBS",0,
				22, 448, 9*7+3,-1,1,"DIAMETER : 1.0 CLKS",0,
				22, 448,11*7+3,-1,1,"FORM: MORPHUS",0,
				22, 448,13*7+3,-1,1,"LIFEFORMS: BASIC",0,
				22, 448,16*7+3,-1,1,"GRAVITY: 4.3 G",0,
				22, 448,19*7+3,-1,1,"OXYGEN: NEGATIVE",0,
				};

TEXT planettext9[]={22, 448, 3*7+3,-1,1,"PLANET : LEVEL 9",0,
				22, 448, 7*7+3,-1,1,"MASS: 2.5 GLBS",0,
				22, 448, 9*7+3,-1,1,"DIAMETER : 1.0 CLKS",0,
				22, 448,11*7+3,-1,1,"FORM: MORPHUS",0,
				22, 448,13*7+3,-1,1,"LIFEFORMS: BASIC",0,
				22, 448,16*7+3,-1,1,"GRAVITY: 4.3 G",0,
				22, 448,19*7+3,-1,1,"OXYGEN: NEGATIVE",0,
				};

/**************************************
*        SHOW CURRENT PLANETS         *
**************************************/

void
Show_Planets(void)
{

UBYTE	used_vram;

UBYTE	*backdrop;
SPRITE*	spr_BACKDROP;

UBYTE	*sun;
SPRITE*	spr_SUN;

ULONG	i;
GsDOBJ2	*op;
MATRIX	tempmatrix;
VECTOR	vector;

	Jalloc_Init(&JALLOC,sonymalloc,(maxmalloc*1024));	/* INITIALISE NEW MALLOC ROUTINE */

	Init_TPages();
	Reset_Cluts();
	Resident_Tims();
	Loading_Message();			/* RESTORE: owner's decision, PLEASE WAIT while this screen loads (GENERAL.C) */

	Find_Hole_In_VRAM(32,32);
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
	Xor_VRAM_Hole(((5+11)<<16)+(used_vram & 0x00ff0000),32,28);

	backdrop=LoadFile("../INTRO/NEB2.PSX");
	spr_BACKDROP = (SPRITE*)LoadFile("../INTRO/NEB2.SPR");
	Init_Spr_List(spr_BACKDROP,(ULONG)backdrop);
	MyFree(backdrop);

	sun=LoadFile("../INTRO/SUN.PSX");
	spr_SUN = (SPRITE*)LoadFile("../INTRO/SUN.SPR");
	Add_Spr_Loadoffset(spr_SUN,(ULONG)sun);

	texture1=LoadFile("../INTRO/MAP1.TIM");
	texture2=LoadFile("../INTRO/MAP2.TIM");
	texture3=LoadFile("../INTRO/MAP3.TIM");
	texture4=LoadFile("../INTRO/MAP4.TIM");
	texture5=LoadFile("../INTRO/MAP5.TIM");
	texture6=LoadFile("../INTRO/MAP6.TIM");

	tmd1=LoadFile("../INTRO/PLANET.TMD");
	tmd2=LoadFile("../INTRO/PLANET.TMD");
	tmd3=LoadFile("../INTRO/PLANET.TMD");
	tmd4=LoadFile("../INTRO/PLANET.TMD");
	tmd5=LoadFile("../INTRO/PLANET.TMD");
	tmd6=LoadFile("../INTRO/PLANET.TMD");

	Change_PageTMD(640   ,511,640    ,256,tmd1);
	Change_PageTMD(640+16,511,640+64 ,256,tmd2);
	Change_PageTMD(640+32,511,640+128,256,tmd3);
	Change_PageTMD(640+48,511,640+192,256,tmd4);
	Change_PageTMD(640+64,511,640    ,0  ,tmd5);
	Change_PageTMD(640+16,511,640+64 ,256,tmd6);

	Init_Model();
	Read_Model_Data(tmd1,0);
	Read_Model_Data(tmd2,1);
	Read_Model_Data(tmd3,2);
	Read_Model_Data(tmd4,3);
	Read_Model_Data(tmd5,4);
	Read_Model_Data(tmd6,5);
	Initialise_World();

	level=0;

	Cls();

	while (fade!=0)
	{

		if (((Debounce1 & PaDenter)!=0) && (fade==128))	/* RESTORE: owner's decision, Start or Cross (PaDenter, 360X.H) */
		{
			Start_Fade_Down(-16);
		}

		Start_Loop();

		Print_Sprite(spr_BACKDROP,320,120,3);

		spr=Find_Spr(spr_SUN,(gamecycle MOD 32));
		Print_Sprite(spr,320,136,253);

		if ((gamecycle MOD 16)<8)
		{
			Print_At(256,216);
			Print_Messm("PRESS START BUTTON");
		}

		PWorld.vx=1280;
		PWorld.vy=256;
		PWorld.vz-=8;

		Change_PageTMD(640   ,511,640    ,256,tmd1);
		Change_PageTMD(640+16,511,640+64 ,256,tmd2);
		Change_PageTMD(640+32,511,640+128,256,tmd3);
		Change_PageTMD(640+48,511,640+192,256,tmd4);
		Change_PageTMD(640+64,511,640    ,0  ,tmd5);
		Change_PageTMD(640+16,511,640+64 ,256,tmd6);

		if (level==0)
		{
			if ((gamecycle MOD 16)<8) Change_PageTMD(640   ,510,640    ,256,tmd1);

			sprctrl.red=sprctrl.blue=0;
			Print_At(32,16);
			Print_Messm("PLANET: ALEUIS");
			sprctrl.red=sprctrl.blue=128;
		}

		if (level==1)
		{
			if ((gamecycle MOD 16)<8) Change_PageTMD(640+16,510,640+64 ,256,tmd2);

			sprctrl.red=sprctrl.blue=0;
			Print_At(32,16);
			Print_Messm("PLANET: NIXA");
			sprctrl.red=sprctrl.blue=128;
		}

		if (level==2)
		{
			if ((gamecycle MOD 16)<8) Change_PageTMD(640+32,510,640+128,256,tmd3);

			sprctrl.red=sprctrl.blue=0;
			Print_At(32,16);
			Print_Messm("PLANET: PROLI");
			sprctrl.red=sprctrl.blue=128;
		}

		if (level==3)
		{
			if ((gamecycle MOD 16)<8) Change_PageTMD(640+48,510,640+192,256,tmd4);

			sprctrl.red=sprctrl.blue=0;
			Print_At(32,16);
			Print_Messm("PLANET: BOZAI");
			sprctrl.red=sprctrl.blue=128;
		}

		if (level==4)
		{
			if ((gamecycle MOD 16)<8) Change_PageTMD(640+64,510,640    ,0  ,tmd5);

			sprctrl.red=sprctrl.blue=0;
			Print_At(32,16);
			Print_Messm("PLANET: CUNAI");
			sprctrl.red=sprctrl.blue=128;
		}

		if (level==5)
		{
			if ((gamecycle MOD 16)<8) Change_PageTMD(640+16,510,640+64 ,256,tmd6);

			sprctrl.red=sprctrl.blue=0;
			Print_At(32,16);
			Print_Messm("PLANET: SELIUS");
			sprctrl.red=sprctrl.blue=128;
		}

		if (level==6)
		{
			sprctrl.red=sprctrl.blue=0;
			Print_At(32,16);
			Print_Messm("PLANET: SPECIAL 1");
			sprctrl.red=sprctrl.blue=128;
		}

		if (level==7)
		{
			sprctrl.red=sprctrl.blue=0;
			Print_At(32,16);
			Print_Messm("PLANET: SPECIAL 2");
			sprctrl.red=sprctrl.blue=128;
		}

		if (level==8)
		{
			sprctrl.red=sprctrl.blue=0;
			Print_At(32,16);
			Print_Messm("PLANET: SPECIAL 3");
			sprctrl.red=sprctrl.blue=128;
		}

		sprctrl.red=sprctrl.blue=0;
		Print_At(32,32);
		Print_Messm("MISSION: ");
		Print_Decm(level+1);
		sprctrl.red=sprctrl.blue=128;

		Set_Coordinates(&PWorld,&DWorld);

		for(i=0,op=object;i<6;i++,op++)
		{
			GsGetLw(op->coord2,&tempmatrix);
			vector = ModelScale[i];
			ScaleMatrix(&tempmatrix,&vector);
			GsSetLightMatrix(&tempmatrix);
			GsGetLs(op->coord2,&tempmatrix);
			GsSetLsMatrix(&tempmatrix);
			GsSortObject4(op,&Wot[screentoggle],16-OTLENGTH,(ULONG *)0x1f800000);
		}

		if (((Debounce1 & PaDLleft)!=0) && (level!=0))
		{
			level--;
		}

		if (((Debounce1 & PaDLright)!=0) && (level!=8))
		{
			level++;
		}

		End_Loop();
	}

	VSync(0);
	SetDispMask(OFF);

}

/**************************************
*       INITIALISE MODEL DATA         *
**************************************/

void
Init_Model(void)
{

	GsSetLightMode(1);

	Init_View();
	Init_Light();
	Init_Texture_Bitmap(texture1,640,256);
	Init_Texture_Clut(texture1,640,511);

	Init_Texture_Bitmap(texture2,640+64,256);
	Init_Texture_Clut(texture2,640+16,511);

	Init_Texture_Bitmap(texture3,640+128,256);
	Init_Texture_Clut(texture3,640+32,511);

	Init_Texture_Bitmap(texture4,640+192,256);
	Init_Texture_Clut(texture4,640+48,511);

	Init_Texture_Bitmap(texture5,640,0);
	Init_Texture_Clut(texture5,640+64,511);

}

/**************************************
*         INITIALISE VIEW             *
**************************************/

void
Init_View(void)
{

	GsSetProjection(1000);			/* SET PROJECTION (DEFAULT) */

	view.vpx	=0;					/* SET VIEWPOINT POSITION */
	view.vpy	=-75;
	view.vpz	=-5000;
	view.vrx	=0;
	view.vry	=-75;
	view.vrz	=0;
	view.rz	=0;
	view.super = WORLD;
	GsSetRefView2(&view);

	GsSetNearClip(500);

}

/**************************************
*         INITIALISE LIGHTS           *
**************************************/

void
Init_Light(void)
{

	pslt[0].vx=0;					/* SET LIGHT 1 */
	pslt[0].vy=0;
	pslt[0].vz=1;
	pslt[0].r=255;
	pslt[0].g=255;
	pslt[0].b=255;
	GsSetFlatLight(0,&pslt[0]);

	pslt[1].vx=-100;				/* SET LIGHT 2 */
	pslt[1].vy=0;
	pslt[1].vz=1;
	pslt[1].r=255;
	pslt[1].g=255;
	pslt[1].b=255;
	GsSetFlatLight(1,&pslt[1]);

	pslt[2].vx=-100;				/* SET LIGHT 3 */
	pslt[2].vy=0;
	pslt[2].vz=1;
	pslt[2].r=255;
	pslt[2].g=255;
	pslt[2].b=255;
	GsSetFlatLight(2,&pslt[2]);

	ambient = 128;					/* SET AMBIENT LIGHTING */
	GsSetAmbient(ambient,ambient,ambient);

	FOG1.dqa = -10000*ONE/64/1000;		/* SET FOG PARAMETER */
	FOG1.dqb = 4096;
	FOG1.rfc = 0x80;
	FOG1.gfc = 0x80;
	FOG1.bfc = 0x80;
	GsSetFogParam(&FOG1);

}

/**************************************
*     INITIALISE TEXTURE BITMAP       *
**************************************/

void
Init_Texture_Bitmap(UBYTE *currenttim,USHORT xpos,USHORT ypos)
{

RECT	rect1;
GsIMAGE	tim1;

	GsGetTimInfo((ULONG *)(currenttim+4),&tim1);
	rect1.x=xpos;
	rect1.y=ypos;
	rect1.w=tim1.pw;
	rect1.h=tim1.ph-16;
	LoadImage(&rect1,tim1.pixel);
}

/**************************************
*     INITIALISE TEXTURE BITMAP       *
**************************************/

void
Init_Texture_Clut(UBYTE *currenttim,USHORT clutx,USHORT cluty)
{

RECT	rect1;
GsIMAGE	tim1;

	GsGetTimInfo((u_long *)(currenttim+4),&tim1);

	if ((tim1.pmode>>3)&0x01)		/* IF CLUT EXISTS */
	{
		rect1.x=clutx;
		rect1.y=cluty;
		rect1.w=tim1.cw;
		rect1.h=tim1.ch;
		LoadImage(&rect1,tim1.clut);
	}
}

/**************************************
*       READ IN THE MODEL DATA        *
**************************************/

void
Read_Model_Data(UBYTE *modeladdr,ULONG modelno)
{

ULONG	*dop;
GsDOBJ2	*objp;

	dop=(ULONG *)modeladdr;			/* GET ADDRESS OF MODEL */
	dop++;						/* SKIP PAST VERSION NUMBER */
	GsMapModelingData(dop);
	dop++;						/* SKIP TO NUMBER OF MODELS */
	dop++;						/* POINT TO MODEL DATA */

	GsLinkObject4((ULONG)dop,&object[modelno],0);
	objp=object+modelno;
	objp->coord2 = &DWorld;

}

/**************************************
*          INITIALISE WORLD           *
**************************************/

void
Initialise_World(void)
{

VECTOR	vector;

	GsInitCoordinate2(WORLD,&DWorld);
	DWorld.coord.t[2] = 1000;

	GsInitCoordinate2(&DWorld,&DBall0);
	GsInitCoordinate2(&DWorld,&DBall1);
	GsInitCoordinate2(&DWorld,&DBall2);
	GsInitCoordinate2(&DWorld,&DBall3);
	GsInitCoordinate2(&DWorld,&DBall4);
	GsInitCoordinate2(&DWorld,&DBall5);

	PWorld.vx=PWorld.vy=PWorld.vz=0;

	object[0].attribute = 0x00000000;		/* BALL 1 */
	object[0].coord2 = &DBall0;
	object[0].coord2->coord.t[0] = 20*8;
	object[0].coord2->coord.t[1] = 225*8;
	vector.vx = ONE/2;
	vector.vy = ONE/2;
	vector.vz = ONE/2;
	ScaleMatrix(&DBall0.coord,&vector);
	ModelScale[0].vx = ModelScale[0].vy = ModelScale[0].vz = ONE;

	object[1].attribute = 0x00000000;		/* BALL 2 */
	object[1].coord2 = &DBall1;
	object[1].coord2->coord.t[0] = 50*8;
	object[1].coord2->coord.t[1] = 150*8;
	vector.vx = ONE/2;
	vector.vy = ONE/2;
	vector.vz = ONE/2;
	ScaleMatrix(&DBall1.coord,&vector);
	ModelScale[1].vx = ModelScale[1].vy = ModelScale[1].vz = ONE;

	object[2].attribute = 0x00000000;		/* BALL 3*/
	object[2].coord2 = &DBall2;
	object[2].coord2->coord.t[0] = 130*8;
	object[2].coord2->coord.t[1] = -130*8;
	vector.vx = ONE/1.5;
	vector.vy = ONE/1.5;
	vector.vz = ONE/1.5;
	ScaleMatrix(&DBall2.coord,&vector);
	ModelScale[2].vx = ModelScale[2].vy = ModelScale[2].vz = ONE;

	object[3].attribute = 0x00000000;		/* BALL 4*/
	object[3].coord2 = &DBall3;
	object[3].coord2->coord.t[0] = 30*8;
	object[3].coord2->coord.t[1] = -30*8;
	vector.vx = ONE/1.8;
	vector.vy = ONE/1.8;
	vector.vz = ONE/1.8;
	ScaleMatrix(&DBall3.coord,&vector);
	ModelScale[3].vx = ModelScale[3].vy = ModelScale[3].vz = ONE;

	object[4].attribute = 0x00000000;		/* BALL 5*/
	object[4].coord2 = &DBall4;
	object[4].coord2->coord.t[0] = -120*8;
	object[4].coord2->coord.t[1] = 0;
	vector.vx = ONE;
	vector.vy = ONE;
	vector.vz = ONE;
	ScaleMatrix(&DBall4.coord,&vector);
	ModelScale[4].vx = ModelScale[4].vy = ModelScale[4].vz = ONE;

	object[5].attribute = 0x00000000;		/* BALL 6*/
	object[5].coord2 = &DBall5;
	object[5].coord2->coord.t[0] = 200*8;
	object[5].coord2->coord.t[1] = 80*8;
	vector.vx = ONE/1.2;
	vector.vy = ONE/1.2;
	vector.vz = ONE/1.2;
	ScaleMatrix(&DBall5.coord,&vector);
	ModelScale[5].vx = ModelScale[5].vy = ModelScale[5].vz = ONE;

}

/**************************************
*     SET COORDINATES FOR WORLD       *
**************************************/

void
Set_Coordinates(SVECTOR *pos,GsCOORDINATE2 *coor)
{

MATRIX	tmp1;
SVECTOR	v1;

	tmp1   = GsIDMATRIX;

	tmp1.t[0] = coor->coord.t[0];
	tmp1.t[1] = coor->coord.t[1];
	tmp1.t[2] = coor->coord.t[2];

	v1 = *pos;

	RotMatrix(&v1,&tmp1);
	coor->coord = tmp1;
	coor->flg = 0;
}

/**************************************
*        CHANGE PALETTE ON TMD        *
**************************************/

void
Change_PageTMD(USHORT x,USHORT y,USHORT px,USHORT py,UBYTE *currentmodel)
{

ULONG	*model;
USHORT	*palettepos;
USHORT	*texturepos;

ULONG	data;
USHORT	j;

	model=(ULONG *)currentmodel;		/* GET ADDRESS OF MODEL */

	palettepos=(USHORT *)model;
	palettepos+=(46/2);				/* MOVE TO CLUT ADDRESS */
	*(palettepos)=((y*64)+(x/16));

	for (j=0;j<3310;j++)
	{
		data=*(model+j);
		if (data==0x24000507)		/* FOUND A POLYGON */
		{
			palettepos=(USHORT *)model;
			palettepos+=(j*2)+3;		/* MOVE TO CLUT ADDRESS */
			*(palettepos)=((y*64)+(x/16));

			texturepos=(USHORT *)model;
			texturepos+=(j*2)+5;		/* MOVE TO TEXTURE ADDRESS */
			*(texturepos)=(px/64)+((py/256)*16);

		}
	}
}

/**************************************
*       WARP TO THE PLANETOID         *
**************************************/

void
Warp_To_Planet(void)
{

	if (level==0) Zoom_To_Planet(planettext1,TELETYPEPLANET1);
	if (level==1) Zoom_To_Planet(planettext2,TELETYPEPLANET2);
	if (level==2) Zoom_To_Planet(planettext3,TELETYPEPLANET3);
	if (level==3) Zoom_To_Planet(planettext4,TELETYPEPLANET4);
	if (level==4) Zoom_To_Planet(planettext5,TELETYPEPLANET5);
	if (level==5) Zoom_To_Planet(planettext6,TELETYPEPLANET6);
	if (level==6) Zoom_To_Planet(planettext7,TELETYPEPLANET7);
	if (level==7) Zoom_To_Planet(planettext8,TELETYPEPLANET8);
	if (level==8) Zoom_To_Planet(planettext9,TELETYPEPLANET9);

}

/**************************************
*          SHOW NEW SYSTEM            *
**************************************/

void
Zoom_To_Planet(TEXT *planetinfo,SHORT nolines)
{

UBYTE	used_vram;

USHORT	zoom;

UBYTE	*backdrop;
SPRITE*	spr_BACKDROP;

ULONG	i;
GsDOBJ2	*op;
MATRIX	tempmatrix;
VECTOR	vector;

SHORT	speed;

SHORT	sine;
SHORT	newsine;

USHORT	sinetime;

LONG	sinex;

SHORT	angle1=0, angle2=0, angle3=0;
SHORT	angle1bak=0, angle2bak=0, angle3bak=0;

	Jalloc_Init(&JALLOC,sonymalloc,(maxmalloc*1024)); /* INITIALISE NEW MALLOC ROUTINE */

	Init_TPages();
	Reset_Cluts();
	Resident_Tims();
	Loading_Message();			/* RESTORE: owner's decision, PLEASE WAIT while this screen loads (GENERAL.C) */

	Find_Hole_In_VRAM(32,32);
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

	backdrop=LoadFile("../INTRO/NEB2.PSX");
	spr_BACKDROP = (SPRITE*)LoadFile("../INTRO/NEB2.SPR");
	Init_Spr_List(spr_BACKDROP,(ULONG)backdrop);
	MyFree(backdrop);

	Init_Star_List();

	zoom=32;
	x=0;
	y=0;
	speed=16;

	if (level==0) texture1=LoadFile("../INTRO/MAP1.TIM");
	if (level==1) texture1=LoadFile("../INTRO/MAP2.TIM");
	if (level==2) texture1=LoadFile("../INTRO/MAP3.TIM");
	if (level==3) texture1=LoadFile("../INTRO/MAP4.TIM");
	if (level==4) texture1=LoadFile("../INTRO/MAP5.TIM");
	if (level==5) texture1=LoadFile("../INTRO/MAP6.TIM");

	if (level==6) texture1=LoadFile("../INTRO/MAP7.TIM");
	if (level==7) texture1=LoadFile("../INTRO/MAP8.TIM");
	if (level==8) texture1=LoadFile("../INTRO/MAP9.TIM");

	if (level==0) tmd1=LoadFile("../INTRO/PLANET.TMD");
	if (level==1) tmd1=LoadFile("../INTRO/PLANET.TMD");
	if (level==2) tmd1=LoadFile("../INTRO/PLANET.TMD");
	if (level==3) tmd1=LoadFile("../INTRO/PLANET.TMD");
	if (level==4) tmd1=LoadFile("../INTRO/PLANET.TMD");
	if (level==5) tmd1=LoadFile("../INTRO/PLANET.TMD");

	if (level==6) tmd1=LoadFile("../INTRO/PLANET.TMD");
	if (level==7) tmd1=LoadFile("../INTRO/PLANET.TMD");
	if (level==8) tmd1=LoadFile("../INTRO/PLANET.TMD");

	Change_PageTMD(640   ,511,640    ,256,tmd1);

	Init_Model();
	Read_Model_Data(tmd1,0);
	Initialise_World2();

	Init_Teletype();

	sine=12;
	newsine=12;
	sinetime=128;

	Cls();

	while (fade!=0)
	{

		if (((Debounce1 & PaDstart)!=0) && (fade==128))
		{
			Start_Fade_Down(-16);
		}

		Start_Loop();

		for (sinex=0;sinex<128;sinex++)
		{

			angle2=angle1;

			Draw_Line(sinex-320+456,72+(GetSin((angle2+(sinex*sine)) MOD 1024))/2048,
					sinex-320+456,72+(GetSin((angle2+(sinex*sine)) MOD 1024))/2048,
					128,
					255-sinex,
					0,254);

			angle2+=1;

		}

		Draw_Line(BOX1POINT1X,BOX1POINT1Y,BOX1POINT2X,BOX1POINT2Y,0,128,0,254);
		Draw_Line(BOX1POINT2X,BOX1POINT2Y,BOX1POINT3X,BOX1POINT3Y,0,128,0,254);
		Draw_Line(BOX1POINT3X,BOX1POINT3Y,BOX1POINT4X,BOX1POINT4Y,0,128,0,254);
		Draw_Line(BOX1POINT4X,BOX1POINT4Y,BOX1POINT1X,BOX1POINT1Y,0,128,0,254);

		angle1+=32;
		if (angle1==1024) angle1=0;

		if (newsine==sine)
		{
			if (sinetime!=0) sinetime--;
			if (sinetime==0)
			{
				newsine=rand()&63;
				sinetime=128;
			}
		}

		if (sine>newsine) sine--;
		if (sine<newsine) sine++;

		if ((gamecycle MOD 16)<8)
		{
			Print_At(256,216);
			Print_Messm("PRESS START BUTTON");
		}

		sprctrl.rotate=(zoom*8);
		sprctrl.scalex=sprctrl.scaley=4096+(zoom/2);
		Print_Sprite(spr_BACKDROP,320,120,64);
		sprctrl.scalex=sprctrl.scaley=4096;
		sprctrl.rotate=0;

		if (speed!=0)
		{
			zoom+=16;
		}

		Add_Star();
		Plot_Stars(speed);

		if ((gamecycle MOD 32)==0)
		{
			if (speed!=0) speed--;
		}

		if (DWorld.coord.t[2]!=-(10*430))
		{
			DWorld.coord.t[2]-=10;
		}

		PWorld.vx=0;
		PWorld.vy+=8;
		PWorld.vz=0;

		Set_Coordinates(&PWorld,&DWorld);

		for(i=0,op=object;i<1;i++,op++)
		{
			GsGetLw(op->coord2,&tempmatrix);
			vector = ModelScale[i];
			ScaleMatrix(&tempmatrix,&vector);
			GsSetLightMatrix(&tempmatrix);
			GsGetLs(op->coord2,&tempmatrix);
			GsSetLsMatrix(&tempmatrix);
			GsSortObject4(op,&Wot[screentoggle],16-OTLENGTH,(ULONG *)0x1f800000);
		}

		sprctrl.red=sprctrl.blue=0;
		sprctrl.green=96;
		Print_Teletype(planetinfo,nolines);
		sprctrl.green=128;
		sprctrl.red=sprctrl.blue=128;

		End_Loop();
	}

	VSync(0);
	SetDispMask(OFF);

}

/**************************************
*          INITIALISE WORLD           *
**************************************/

void
Initialise_World2(void)
{

VECTOR	vector;

	GsInitCoordinate2(WORLD,&DWorld);

	DWorld.coord.t[0] = -60;
	DWorld.coord.t[1] = -70;
	DWorld.coord.t[2] = 10*85;

	GsInitCoordinate2(&DWorld,&DBall0);

	PWorld.vx=PWorld.vy=PWorld.vz=0;

	object[0].attribute = 0x00000000;		/* BALL 1 */
	object[0].coord2 = &DBall0;
	object[0].coord2->coord.t[0] = 0;
	object[0].coord2->coord.t[1] = 0;
	vector.vx = ONE/2;
	vector.vy = ONE/2;
	vector.vz = ONE/2;
	ScaleMatrix(&DBall0.coord,&vector);
	ModelScale[0].vx = ModelScale[0].vy = ModelScale[0].vz = ONE;

}

