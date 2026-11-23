
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libsn.h>

#define	SX	0
#define	SY	0
#define	SZ	0

#define	MULT	300

int cube_def_pos[132] ={

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

#define TIM_ADDR 0x80020000
#define TIM_HEADER 0x00000010
#define TMD_ADDR 0x80010000

u_long *TmdBase;
int CurrentTmd;

#define OT_LENGTH  7
GsOT WorldOT[2];
GsOT_TAG OTTags[2][1<<OT_LENGTH];

#define PACKETMAX 1000
PACKET GpuPacketArea[2][PACKETMAX*64];

#define NCUBE 45
#define OBJMAX NCUBE
int nobj;
GsDOBJ2 object[OBJMAX];
GsCOORDINATE2 objcoord[OBJMAX];
SVECTOR Rot[OBJMAX];
SVECTOR RotV[OBJMAX];
VECTOR Trns[OBJMAX];
VECTOR TrnsV[OBJMAX];

GsRVIEW  View;
int ViewAngleXZ;
int ViewRadial;
#define DISTANCE 600

GsF_LIGHT pslt[3];

int Bakuhatu;
u_long PadData;
u_long oldpad;
GsFOGPARAM dq;
int dqf;
int back_r, back_g, back_b;

#define FOG_R 20
#define FOG_G 20
#define FOG_B 20

void drawCubes();
int moveCubes();
void initModelingData();
void allocCube();
void initSystem();
void initAll();
void initTexture();
void initView();
void initLight();
void changeFog();
void changeTmd();

main()
{
	initSystem();

	Bakuhatu = 0;
	PadData = 0;
	CurrentTmd = 0;
	dqf = 0;
	back_r = back_g = back_b = 0;
	initView();
	initLight(0, 0xc0);
	initModelingData(TMD_ADDR);
	initTexture(TIM_ADDR);
	allocCube(NCUBE);

	while(1)
	{
		moveCubes();
		GsSetRefView(&View);
		drawCubes();
		pollhost();
	}
}

void drawCubes()
{
	int i;
	GsDOBJ2 *objp;
	int activeBuff;
	MATRIX LsMtx;

	activeBuff = GsGetActiveBuff();
	GsSetWorkBase((PACKET*)GpuPacketArea[activeBuff]);
	GsClearOt(0, 0, &WorldOT[activeBuff]);

	objp = object;
	for(i = 0; i < nobj; i++)
	{

		RotMatrix(Rot+i, &(objp->coord2->coord));
		TransMatrix(&(objp->coord2->coord), &Trns[i]);
		objp->coord2->flg = 0;
		GsGetLs(objp->coord2, &LsMtx);
		GsSetLsMatrix(&LsMtx);
		GsSetLightMatrix(&LsMtx);
		GsSortObject2(objp, &WorldOT[activeBuff], 14-OT_LENGTH);

		objp++;
	}

	oldpad = PadData;
	PadData = PadRead(1);
	VSync(0);
	ResetGraph(1);
	GsSwapDispBuff();
	GsSortClear(back_r, back_g, back_b, &WorldOT[activeBuff]);
	GsDrawOt(&WorldOT[activeBuff]);
}

int moveCubes()
{
	int i;
	GsDOBJ2   *objp;

	if((PadData & PADLleft)>0)
	{
		View.vpy-=512;

		ViewAngleXZ++;
		if(ViewAngleXZ >= 72)
		{
			ViewAngleXZ = 0;
		}
	}
	if((PadData & PADLright)>0)
	{
		View.vpy+=512;

		ViewAngleXZ--;
		if(ViewAngleXZ < 0)
		{
			ViewAngleXZ = 71;
		}
	}
	if((PadData & PADLup)>0) View.vpz += 100;
	if((PadData & PADLdown)>0) View.vpz -= 100;
	if((PadData & PADRdown)>0)
	{
		ViewRadial-=3;
		if(ViewRadial < 8)
		{
			ViewRadial = 8;
		}
	}
	if((PadData & PADRright)>0)
	{
		ViewRadial+=3;
		if(ViewRadial > 450)
		{
			ViewRadial = 450;
		}
	}

	if((PadData & PADk)>0) return(-1);
	if(((PadData & PADRleft)>0)&&((oldpad&PADRleft) == 0)) changeFog();
	if(((PadData & PADRup)>0)&&((oldpad&PADRup) == 0)) changeTmd();
	if(((PadData & PADn)>0)&&((oldpad&PADn) == 0)) Bakuhatu = 1;
	if(((PadData & PADl)>0)&&((oldpad&PADl) == 0)) allocCube(NCUBE);

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

	return(0);
}

void allocCube(n)
int n;
{       
	int x, y, z;
	int i;
	int *posp;
	GsDOBJ2 *objp;
	GsCOORDINATE2 *coordp;

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
	Bakuhatu = 0;
}

void initSystem()
{
	int i;

	PadInit(0);

	SetVideoMode(MODE_PAL);

	GsInitGraph(640, 480, 2, 0, 0);
	GsDefDispBuff(0, 0, 0, 0);

	for(i = 0; i < 2; i++) {        
		WorldOT[i].length = OT_LENGTH;
		WorldOT[i].org = OTTags[i];
	}       

	GsInit3D();
}

void initModelingData(tmdp)
u_long *tmdp;
{
	u_long size;
	int i;
	int tmdobjnum;

	tmdp++;

	GsMapModelingData(tmdp);

	tmdp++;
	tmdobjnum = *tmdp;
	tmdp++;
	TmdBase = tmdp;
}

void initTexture(tex_addr)
u_long *tex_addr;
{
	RECT rect1;
	GsIMAGE tim1;
	int i;
	
	while(1)
	{
		if(*tex_addr != TIM_HEADER)
		{
			break;
		}
		tex_addr++;
		GsGetTimInfo(tex_addr, &tim1);
		tex_addr += tim1.pw*tim1.ph/2+3+1;
		rect1.x=tim1.px;
		rect1.y=tim1.py;
		rect1.w=tim1.pw;
		rect1.h=tim1.ph;
		LoadImage(&rect1,tim1.pixel);
		if((tim1.pmode>>3)&0x01)
		{
			rect1.x=tim1.cx;
			rect1.y=tim1.cy;
			rect1.w=tim1.cw;
			rect1.h=tim1.ch;
			LoadImage(&rect1,tim1.clut);
			tex_addr += tim1.cw*tim1.ch/2+3;
		}
	}
}

void initView()
{
	ViewAngleXZ = 54;
	ViewRadial = 75;
	View.vpx = 0;
	View.vpy = 0;
	View.vpz = -8192;
	View.vrx = 2048;
	View.vry = 0;
	View.vrz = 0;
	View.rz=0;

	View.super = WORLD;
	GsSetRefView(&View);
	GsSetProjection(1000);
	GsSetNearClip(100);
	GsSetLightMode(0);
}

void initLight(c_mode, factor)
int c_mode;
int factor;
{
	if(c_mode == 0)
	{
		pslt[0].vx = 200; pslt[0].vy= 200; pslt[0].vz= 300;
		pslt[0].r = factor; pslt[0].g = factor; pslt[0].b = factor;
		GsSetFlatLight(0,&pslt[0]);
		
		pslt[1].vx = -50; pslt[1].vy= -1000; pslt[1].vz= 0;
		pslt[1].r=0x20; pslt[1].g=0x20; pslt[1].b=0x20;
		GsSetFlatLight(1,&pslt[1]);
		
		pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
		pslt[2].r=0x0; pslt[2].g=0x0; pslt[2].b=0x0;
		GsSetFlatLight(2,&pslt[2]);
	}
	else
	{
		pslt[0].vx = 200; pslt[0].vy= 100; pslt[0].vz= 0;
		pslt[0].r = factor; pslt[0].g = 0; pslt[0].b = 0;
		GsSetFlatLight(0,&pslt[0]);
		
		pslt[1].vx = -200; pslt[1].vy= 100; pslt[1].vz= 0;
		pslt[1].r=0; pslt[1].g=0; pslt[1].b=factor;
		GsSetFlatLight(1,&pslt[1]);
		
		pslt[2].vx = 0; pslt[2].vy= -200; pslt[2].vz= 0;
		pslt[2].r=0; pslt[2].g=factor; pslt[2].b=0;
		GsSetFlatLight(2,&pslt[2]);
	}       

	GsSetAmbient(ONE/2,ONE/2,ONE/2);
}

void changeFog()
{
	if(dqf)
	{
		GsSetLightMode(0);
		dqf = 0;
		back_r = 0;
		back_g = 0;
		back_b = 0;
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
		back_r = FOG_R;
		back_g = FOG_G;
		back_b = FOG_B;
	}
}

void changeTmd()
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
		initLight(0, 0xc0);
		break;
	    case 1:			/* FLAT */
		initLight(0, 0xc0);
		break;
	    case 2:			/* GORAUD */
		initLight(1, 0xff);
		break;
	    case 3:			/* PHONG */
		initLight(0, 0xff);
		break;
	}
}

