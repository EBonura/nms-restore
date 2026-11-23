
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE ROUTINES           *
**************************************/

/**************************************
*          DECLARE VARIABLES          *
**************************************/

typedef struct{
	double	x;
	double	y;
	double	vx;
	double	vy;
	double	mass;
	}BALL;

BALL	ball[2]={ 256,256  , 0, 0, 100,
				160,125.7, 0, 0, 50};

LONG	craft;
LONG	sprtemp;

SHORT	sprx;
SHORT	spry;

SPRITE	*spr;
UBYTE	sprframe[MAX_SPRITES];		/* RESTORE: THE FRAME EACH SPRITE IS DRAWN WITH (SEE Print_Sprites) */
SPRITE	*sprtable[MAX_SPRITES];		/* RESTORE: NEW WORK (owner's decision): TABLE EACH SPRITE IS DRAWN WITH, FOR HITS */

	flag		pod_attached;
	char		*source;
	double 	andy_angle;
	int		boost;
	int		i,iy;
	int		craftn;
	int		linex1[2],liney1[2];
	int		linex2[2],liney2[2];
	int		physical_screen;
	int		angle;
	int		old_angle,old_craftn;	/* RESTORE: THE HEADING AND FRAME BEFORE THIS CYCLE'S TURN (SEE THE SHAPE TEST IN Move_Craft) */
	int		rand_factor;
	int		ball_x,ball_y;
	double	tdx,tdy;
	double	adx,ady;
	double	x,y,dx,dy;
	double 	old_x,old_y;
	double 	temp;
	int		firen;
	int		rotated;
	int		bank_factor;
	int		bank_factor2;
	double	fx,fy,fdx,fdy;
	int		fire_delay;
	flag		craft_flashing;
	int		craft_flash_count;
	int		last_scroll_x,last_scroll_y;
	long		pod_no;
	int		ships_gravity;
	long		thrust_factor;
	double	friction_factor;

USHORT	fuel;
SHORT	craftmapw;			/* RESTORE: THE MAP'S SIZE AS THE CRAFT KNOWS IT; 0 UNTIL Init_Ball (SEE Move_Craft) */
SHORT	craftmaph;
SHORT	fuel_deplete;		/* RESTORE: THRUST FRAMES TO THE NEXT GAUGE STEP (PC ships_fuel_deplete) */
SHORT	survivor_message;	/* RESTORE: GAME CYCLES LEFT TO SHOW "RESCUE NOT AVAILABLE" (Move_Craft, MAIN.C) */
USHORT	shields;
FLAG	craftdestroyed;
USHORT	deadtimer;

/**************************************
*          PRINT THE SPRITES          *
**************************************/

void 
Print_Sprites(void)
{

	for (sprtemp=0;sprtemp<256;sprtemp++)
	{
//		sprtemp=spritenumber;

		spr=NULL;


		if (sprites[sprtemp].name==NAME_MISSILE) spr=spr_GUIDED;
		if (sprites[sprtemp].name==NAME_CSHIP)  spr=spr_S_SHIP;
		if (sprites[sprtemp].name==NAME_EBALL) spr=spr_EBALL;
		if (sprites[sprtemp].name==NAME_BUCK01) spr=spr_BUCK01;
		if (sprites[sprtemp].name==NAME_BUCK02) spr=spr_BUCK02;
		if (sprites[sprtemp].name==NAME_AGMINE) spr=spr_MINE;

		/* RESTORE: THE SATURN PORT'S SPRITES (LOADDATA.C), THE SAME FOR EVERY MISSION */
		if (sprites[sprtemp].name==NAME_EXPLOSION) spr=spr_S_EXP1;
		if (sprites[sprtemp].name==NAME_exp4) spr=spr_S_EXP4;
		if (sprites[sprtemp].name==NAME_exp6) spr=spr_S_EXP6;
		if (sprites[sprtemp].name==NAME_exp7) spr=spr_S_EXP7;
		if (sprites[sprtemp].name==NAME_laserfire) spr=spr_S_SPFIRE;
		if (sprites[sprtemp].name==NAME_CHAINB) spr=spr_CHLINK;	/* RESTORE: THE CHAIN WEAPON'S SHOT (FIRE.C, LOADDATA.C) */

		if (level!=0)				/* RESTORE: MISSIONS 2..9, LOADDATA.C levelsprites */
		{
			if ((ULONG)sprites[sprtemp].name<256)
				if (levelspr[sprites[sprtemp].name]!=NULL) spr=levelspr[sprites[sprtemp].name];
		}
		else
		{

//#if 0

		if (sprites[sprtemp].name==0)  spr=spr_S_TUBEL1;
		if (sprites[sprtemp].name==3)  spr=spr_FLOPL1;
		if (sprites[sprtemp].name==4)	 spr=spr_S_BOUNC3;
		if (sprites[sprtemp].name==17) spr=spr_S_MRSPIT;
		if (sprites[sprtemp].name==20) spr=spr_S_FLOAT;
		if (sprites[sprtemp].name==22) spr=spr_S_STAR;
		if (sprites[sprtemp].name==23) spr=spr_S_STARFS;
		if (sprites[sprtemp].name==24) spr=spr_S_TURRET;
		if (sprites[sprtemp].name==25) spr=spr_S_ACIDSP;
//		if (sprites[sprtemp].name==26) spr=spr_S_CHAPIE;
		if (sprites[sprtemp].name==32) spr=spr_PLUNGL1;
		if (sprites[sprtemp].name==29) spr=spr_S_SURVS;
		if (sprites[sprtemp].name==39) spr=spr_S_FLOAT;
		if (sprites[sprtemp].name==41) spr=spr_S_BOUNC1;
		if (sprites[sprtemp].name==42) spr=spr_S_BOUNC2;
		if (sprites[sprtemp].name==47) spr=spr_S_FLOAT;
		if (sprites[sprtemp].name==52) spr=spr_S_RCK01;
		if (sprites[sprtemp].name==53) spr=spr_S_RCK02;
		if (sprites[sprtemp].name==54) spr=spr_S_RCK03;
		if (sprites[sprtemp].name==56) spr=spr_S_STAR;
		if (sprites[sprtemp].name==57) spr=spr_S_TURRET;
		if (sprites[sprtemp].name==59) spr=spr_S_TENTA1;
		if (sprites[sprtemp].name==60) spr=spr_S_TENTA2;

		if (sprites[sprtemp].name==63) spr=spr_S_RCKMPH;
//		if (sprites[sprtemp].name==64) spr=spr_S_CHAPIE;
		if (sprites[sprtemp].name==65) spr=spr_S_STARFS;
		if (sprites[sprtemp].name==66) spr=spr_S_ACIDSP;
//		if (sprites[sprtemp].name==91) spr=spr_S_CHAPIE;
		if (sprites[sprtemp].name==92) spr=spr_S_VIL1+0;
		if (sprites[sprtemp].name==93) spr=spr_S_VIL1+1;
		if (sprites[sprtemp].name==94) spr=spr_S_VIL1+2;
		if (sprites[sprtemp].name==95) spr=spr_S_VIL1+3;
		if (sprites[sprtemp].name==96) spr=spr_S_VIL1+4;
		if (sprites[sprtemp].name==97) spr=spr_S_VIL1+5;
		if (sprites[sprtemp].name==103) spr=spr_S_TNSL1;

		if (sprites[sprtemp].name==104) spr=spr_L1C1;
		if (sprites[sprtemp].name==105) spr=spr_L1C2;
		if (sprites[sprtemp].name==106) spr=spr_L1C3;
		if (sprites[sprtemp].name==107) spr=spr_L1C4;
		if (sprites[sprtemp].name==108) spr=spr_L1C5;
		if (sprites[sprtemp].name==110) spr=spr_S_MSSPIT;
		if (sprites[sprtemp].name==111) spr=spr_S_LRTAD;

		if (sprites[sprtemp].name==62) spr=spr_S_MUSL1;
		if (sprites[sprtemp].name==91) spr=spr_S_CHAPIE;

			if ((ULONG)sprites[sprtemp].name<256)	/* RESTORE: MISSION 1'S SATURN ROWS */
				if (levelspr[sprites[sprtemp].name]!=NULL) spr=levelspr[sprites[sprtemp].name];

		}						/* RESTORE: END OF MISSION 1 */

//#endif

//		spr=spr_FLOPL1;

		sprtable[sprtemp]=spr;			/* RESTORE: NEW WORK (owner's decision) */

		/* RESTORE: A FRAME PAST THE END OF A LOADED TABLE IS DRAWN AS FRAME 0,
		   AS THE SATURN PORT'S DecodeSCS DOES (VDP/VDP1_3.C:106: "if(frame >=
		   NumFrames) frame=0"). THE MAPS GIVE windtun 0..44 FOR THE 45-FRAME
		   S_WINDL2/L4.FLC; THE ART HERE HAS 22 FRAMES. */
		sprframe[sprtemp]=sprites[sprtemp].frame;
		if ((ULONG)sprites[sprtemp].name<256)
			if ((levelspr[sprites[sprtemp].name]==spr) && (sprites[sprtemp].frame>=levelcount[sprites[sprtemp].name]))
				sprframe[sprtemp]=0;

		if ((sprites[sprtemp].visible==TRUE) &&
			(sprites[sprtemp].on==TRUE))		/* RESTORE: a switched-off sprite is not drawn (bug fix) */
		{
			if (spr!=NULL)
			{
				sprx=sprites[sprtemp].x;
				spry=sprites[sprtemp].y;

				if ((sprx>(screenx-XEDGECLIP)) &&
					(sprx<(screenx+320+XEDGECLIP)) &&
						(spry>(screeny-YEDGECLIP)) &&
							(spry<(screeny+240+YEDGECLIP)))
				{
//					sprites[sprtemp].frame=0;

					Print_Sprite(spr+sprframe[sprtemp],
						(sprites[sprtemp].x)-screenx,
							(sprites[sprtemp].y)-screeny+((spr->h)/2)+2,230);
				}
			}
		}
	}
}

/* RESTORE: CREATURE CONTACT (owner's decision). NMS'S collision_with (MAKEBACK/SCRATCH.C:651, ALSO IN
   SCRATCH.C HERE) TESTS THE CRAFT'S BOX AGAINST EVERY ON AND SHOOTABLE SPRITE WITH THE SAME OVERLAP TESTS.
   IT READ THE BOXES FROM seqdata (PC SPRITE DATA THE PS1 NEVER LOADS); THIS COPY TAKES THEM FROM THE SPRITE
   TABLES Print_Sprites DRAWS WITH (sprtable), AS process_collisions DOES. A CREATURE WITH NO ART HAS NO BOX. */
#ifdef X360_TEST_TOUCH	/* RESTORE: TEST BUILDS ONLY: WHICH CREATURE THE CRAFT LAST TOUCHED (name, x, y, game cycle, its box x y w h) */
LONG testtouch[8];
#endif
static flag Craft_Touches_Creature(flag survivors)	/* RESTORE: survivors=YES TESTS THE SURVIVORS INSTEAD (THE RESCUE MESSAGE, Move_Craft) */
{
	SPRITE	*fspr;
	int	i,x,y,width,height;
	int	its_x,its_y,its_width,its_height;
	flag	x_is_in,y_is_in;

	fspr=sprtable[craft];
	if (fspr==NULL) return FALSE;
	its_y=(int) crafty+(fspr->h/2)+2;
	fspr+=craftn;
	its_x=(int) craftx+fspr->u;
	its_y+=fspr->v;
	its_width=fspr->w;
	if (its_width==0) its_width=256;
	its_height=fspr->h;
	if (its_height==0) its_height=256;

	for(i=0;i<MAX_SPRITES;i++)
	{
		/* RESTORE: bug fix (2026-11-21 audit): A SURVIVOR IS SHOOTABLE (THE JUNE LIST), SO TOUCHING ONE BOTH SHOWED "RESCUE NOT AVAILABLE" AND TOOK SHIELDS; IT NO LONGER HURTS */
		if ((i!=craft) && (sprites[i].on==TRUE) && ((survivors==YES)?(sprites[i].name==NAME_survrs):((sprites[i].shootable==YES) && (sprites[i].name!=NAME_survrs))) && (sprtable[i]!=NULL))
		{
			fspr=sprtable[i];
			x=sprites[i].x;
			y=sprites[i].y+(fspr->h/2)+2;
			fspr+=sprframe[i];
			x+=fspr->u;
			y+=fspr->v;
			width=fspr->w;
			if (width==0) width=256;
			height=fspr->h;
			if (height==0) height=256;

			x_is_in=FALSE;
			if( its_x<=x && (its_x+its_width)>(x+width) ) x_is_in=TRUE;
			if( its_x>=x && its_x<(x+width) ) x_is_in=TRUE;
			if( (its_x+its_width)>=x && (its_x+its_width)<(x+width) ) x_is_in=TRUE;

			y_is_in=FALSE;
			if( its_y<=y && (its_y+its_height)>(y+height) ) y_is_in=TRUE;
			if( its_y>=y && its_y<(y+height) ) y_is_in=TRUE;
			if( (its_y+its_height)>=y && (its_y+its_height)<(y+height) ) y_is_in=TRUE;

#ifdef X360_TEST_TOUCH
			if((x_is_in==TRUE) && (y_is_in==TRUE) && (survivors==NO)) { testtouch[0]=sprites[i].name; testtouch[1]=sprites[i].x; testtouch[2]=sprites[i].y; testtouch[3]=gamecycle; testtouch[4]=x; testtouch[5]=y; testtouch[6]=width; testtouch[7]=height; }
#endif
			if(x_is_in==TRUE && y_is_in==TRUE) return TRUE;
		}
	}
	return FALSE;
}

/**************************************
*        MOVE THE CRAFT ABOUT         *
**************************************/

void 
Move_Craft(void)
{

	if (craftdestroyed==YES) return;

	if (Collision_At(craftx,crafty)==TRUE)
	{
//		Print_At(32,32);
//		Print_Messm("OUCH!!!!!!");
	}

	x=craftx;
	y=crafty;

	Fire_Weapons();

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Rotate Left & Right */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

	old_angle=angle;
	old_craftn=craftn;
	rotated=0;
	bank_factor=0;
	if(bank_factor2>0) 
	{
		bank_factor2-=64;
		if(bank_factor2==0) bank_factor=0;
	}

	if ((Pad1 & PaDLleft)!=0)
	{
		rotated--;
		angle--;
		bank_factor=64;
		bank_factor2+=64;
	}
	if ((Pad1 & PaDLright)!=0)
	{
		rotated++;
		angle++;
		bank_factor=64*4;
		bank_factor2+=64;
	}
	if(rotated!=0)
	{
		angle&=63;
		craftn=((angle+16)&63); //+1+bank_factor+bank_factor2;
//		andy_angle=(angle)*((PI*2)/64);
	}

	if(bank_factor2>0 && rotated==0) 
	{
		bank_factor2-=64;
		if(bank_factor2==0) bank_factor=0;
	}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Thrust */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

	/* RESTORE: BUG FIX. NMS'S PC VERSION COMMENTS OUT THIS TOGGLE (MAKEBACK/MAIN.C:531-546) AND LEAVES
	   ATTACH/DETACH TO ship_pickup_pod. HERE IT SET pod_attached WITH NO POD, AND THE ROPE (update_all)
	   DRAGGED THE CRAFT ABOUT 175 PIXELS AND TORE THE MAP. */
	#if 0
	if ((Pad1 & PAD_GRAB)!=0)
	{
		if (pod_attached==TRUE)
		{
			pod_attached=FALSE;
			sprites[pod_no].action=ACTION_FALL;
//			sprites[pod_no].x8=sprites[pod_no].x<<16;
//			sprites[pod_no].y8=sprites[pod_no].y<<16;
			sprites[pod_no].dx=0; //(16384-rand())*8;
			sprites[pod_no].dy=16384;
		}
		else
		{
			pod_attached=TRUE;
			sprites[pod_no].visible=TRUE;
		}
	}
	#endif

	old_x=x;
	old_y=y;

	boost=0;

	/* RESTORE: THRUST DOES NOT DEPEND ON THE FUEL, AS IN NMS'S PC AND SATURN VERSIONS (PC MAKEBACK/MAIN.C:553-571,
	   SATURN LOGIC/CONTROL.C:220-240: THE THRUST AND ITS PARTICLES RUN WHATEVER THE GAUGE SAYS; ONLY THE GAUGE
	   FALLS, BELOW). THE JULY PS1 CODE CUT THE THRUST AT fuel==0 (WAS: && (fuel!=0)); NO NMS VERSION DOES. */
	if ((Pad1 & PAD_THRUST)!=0) boost=4;

	/* RESTORE: completion, rebuilt from the Saturn version (owner's decision 2026-11-13, "every gameplay feature completed in other ports";
	   found by the 2026-11-21 audit): THE SATURN'S Z BUTTON STOPS THE CRAFT DEAD (LOGIC/CONTROL.C:154-160, "Brake!!": BOTH SPEEDS 0), AS
	   THE PC'S DOWN ARROW DOES (MAKEBACK/MAIN.C:520). HERE R2, WHICH NOTHING READS IN FLIGHT (NOT DOWN: THE ANALOG STICK SETS THE D-PAD BITS,
	   SO A TURN WITH THE STICK A LITTLE LOW WOULD BRAKE). */
	if ((Pad1 & PaDrd)!=0)
	{
		ball[0].vx=0;
		ball[0].vy=0;
	}

//	if(keys[kRIGHTSHIFT]==128 || keys[kUARROW]==128) boost=1;
//	if(keys[kB]==128) boost=4;

	if(boost>0)
	{
		/* RESTORE: THE GAUGE FALLS AS IN THE PC AND SATURN VERSIONS (PC MAKEBACK/MAIN.C:573-581, SATURN
		   LOGIC/CONTROL.C:242-253): ONE STEP EVERY 17TH THRUST FRAME (fuel_deplete COUNTS 16 DOWN), TAKING boost OFF
		   THE PC'S 32*8=256 BAR (MAIN.C:699). THE PS1 TANK IS 12*FUELMULT=192 (LEVEL.C:791, PANEL.C:45 DRAWS
		   fuel/FUELMULT SEGMENTS): THE SAME FRACTION OF 4/256 IS 3/192, SO A STEP TAKES 3. WAS fuel-- EVERY THRUST
		   FRAME (A TANK OF 192 FRAMES; NOW 1088). NOTHING HAPPENS AT 0 IN ANY VERSION. */
		if (fuel>0)
		{
			if (fuel_deplete==0)
			{
				fuel_deplete=16;
				fuel=(fuel>(boost*3/4))?fuel-(boost*3/4):0;
			}
			else fuel_deplete--;
		}
//		play_sample(1,samples_start[FX_THRUSTE],samples_len[FX_THRUSTE]);

		andy_angle=((angle+16)&63)*((PI*2)/64);
		andy_thrust(0,andy_angle,thrust_factor*boost, TIME_STEP);
		rand_factor=(rand()*6)>>15;
		temp=((angle+29+rand_factor)&63)*((pi*2)/64);
		tdx=cos(temp)*1.5;
		tdy=sin(temp)*1.5;
		temp=((angle+16)&63)*((pi*2)/64);
		adx=cos(temp)*7;
		ady=sin(temp)*7;
		Add_Particle((int ) craftx+adx,(int ) crafty+ady,(int ) (tdx*65536),(int ) (tdy*65536));
		Add_Particle((int ) craftx-adx,(int ) crafty-ady,(int ) (tdx*65536),(int ) (tdy*65536));

		temp=(angle)*((PI*2)/64);
	}

	old_x=x; old_y=y;

	update_all(pod_attached);
	Pickups_Tick();		/* RESTORE: THE PC'S FUEL, SHIELD AND GRAVITY PICK-UPS (BELOW) */
#ifdef X360_TEST_TELE
	if (gamecycle==100) { extern void Test_Add_Tele(void); Test_Add_Tele(); }	/* RESTORE: TEST BUILDS ONLY (PROCESS.C) */
#endif

	/* RESTORE: completion, rebuilt from NMS's PC version (owner's decision 2026-11-13, "every gameplay feature completed in other
	   ports"). THE PC TAKES AND DROPS THE POD ON A KEY PRESS (MAKEBACK/MAIN.C:589-593, keys_jp[kG]), NOT WHILE IT IS HELD: THE PS1 CALLED
	   ship_pickup_pod EVERY CYCLE THE BUTTON WAS DOWN, WHICH WITH A POD WOULD TAKE AND DROP IT EVERY CYCLE. Debounce1 IS THE PRESS. */
	if ((Debounce1 & PAD_GRAB)!=0)
	{
		ship_pickup_pod(&pod_attached);
		if (pod_attached==TRUE) sprites[pod_no].visible=TRUE;
	}

#ifdef X360_TEST_WARP
#ifndef X360_TEST_WARP_DX
#define X360_TEST_WARP_DX	0		/* RESTORE: TEST BUILDS ONLY: THE OFFSET FROM THE PLACEMENT (WAS 0, 40) */
#define X360_TEST_WARP_DY	40
#endif
	/* RESTORE: NEW WORK, TEST BUILDS ONLY (X360_CFLAGS='-DX360_TEST_WARP=100 -DX360_TEST_WARP_EVERY=300
	   -DX360_TEST_WARP_HOLD=60 -DX360_TEST_WARP_NAMES=109,112,81'): FOR SCREENSHOTS. FROM GAME CYCLE 100, 400, 700..
	   FOR HOLD CYCLES THE CRAFT IS PUT 40 PIXELS BELOW THE FIRST PLACEMENT OF THE NEXT NAME IN THE LIST (NUMBERS IN LEVELn.BMF) THE MISSION HAS,
	   STILL AND WITH ITS SHIELD FULL; THE VIEW FOLLOWS IT */
	{
		static const int warpnames[]={X360_TEST_WARP_NAMES,-1};
		int	wi;
		int	ws;
		if ((gamecycle>=X360_TEST_WARP) && (((gamecycle-X360_TEST_WARP) % X360_TEST_WARP_EVERY)<X360_TEST_WARP_HOLD))
		{
			wi=(gamecycle-X360_TEST_WARP)/X360_TEST_WARP_EVERY;
			for (ws=0;ws<MAX_SPRITES;ws++)
				if ((warpnames[wi]>=0) && (sprites[ws].on==TRUE) && (sprites[ws].name==warpnames[wi]))
				{
					ball[0].x=sprites[ws].x+X360_TEST_WARP_DX;	ball[0].y=sprites[ws].y+X360_TEST_WARP_DY;
					ball[1].x=ball[0].x;		ball[1].y=ball[0].y+8;
					ball[0].vx=ball[0].vy=ball[1].vx=ball[1].vy=0;
					shields=FULLSHIELDS;
					break;
				}
		}
	}
#endif
#ifdef X360_TEST_AT_X
	/* RESTORE: TEST BUILDS ONLY: -DX360_TEST_AT_X=n -DX360_TEST_AT_Y=n -DX360_TEST_AT=c HOLD THE CRAFT STILL AT THAT PLACE FROM GAME CYCLE c ON (NO CREATURE NEEDED) */
	if (gamecycle>=X360_TEST_AT)
	{
		ball[0].x=X360_TEST_AT_X;	ball[0].y=X360_TEST_AT_Y;
		ball[1].x=ball[0].x;		ball[1].y=ball[0].y+8;
		ball[0].vx=ball[0].vy=ball[1].vx=ball[1].vy=0;
		shields=FULLSHIELDS;
	}
#endif
	/* RESTORE: THE CRAFT'S LOWEST POINT IS THE MAP'S OWN HEIGHT. NMS'S 960 IS MISSION 1'S HEIGHT (ALSO MISSIONS 2, 4, 5, 6: 960 HIGH);
	   MISSIONS 3, 7, 8 AND 9 ARE 720 HIGH, WHERE THE CRAFT SANK 240 PIXELS BELOW THE MAP (LEVELn.MAP HEADER, mapheight, SCROLLER.C:124). */
	if (craftmaph!=0)
	{
	if (ball[0].y>craftmaph) ball[0].y=craftmaph;

	/* RESTORE: completion, rebuilt from the Saturn port (owner's decision 2026-11-13). THE SATURN HOLDS THE CRAFT ON THE MAP: WITHIN 24
	   PIXELS OF ANY EDGE, OR BEYOND IT (THE COMPARISON IS UNSIGNED, SO NEGATIVE POSITIONS COUNT), ITS SPEED IS ZEROED EVERY FRAME
	   (VDP/VDP2_1.C:143-147, IN HandleScrollParameters, FROM sprites[craft].x AND .y; MapWidth AND MapHeight ARE mapwidth AND
	   mapheight HERE, TAKEN BY Init_Ball ONCE THE MAP IS SORTED: MAIN.C CALLS Move_Craft ONCE BEFORE THAT, WHEN THEY ARE STILL THE LAST MISSION'S,
   SO Reset_Craft CLEARS THEM AND THE TWO TESTS WAIT FOR Init_Ball). THE PC AND THE JULY PS1 HAVE NOTHING OF THE KIND. THE CRAFT CAN STILL CREEP: THRUST ADDS SPEED AGAIN AFTER THE
	   ZEROING (ABOUT 0.06 PIXEL A FRAME AT TIME_STEP 0.0017), AS IN THE SATURN. */
	{
		ULONG	bx=(ULONG)((LONG)ball[0].x);
		ULONG	by=(ULONG)((LONG)ball[0].y);
		if ((bx>(ULONG)(craftmapw-24)) || (by>(ULONG)(craftmaph-24)) || (bx<24) || (by<24))
		{
			ball[0].vx=0;
			ball[0].vy=0;
		}
	}
	}

	x=ball[0].x;
	y=ball[0].y;

	/* RESTORE: completion, rebuilt from the PC version (owner's decision 2026-11-13). THE PC BOUNCES THE CRAFT OFF THE TERRAIN
	   (MAKEBACK/MAIN.C:598-610): IF THE CRAFT'S NEW POSITION IS ON THE BACKGROUND IT GOES BACK TO WHERE IT WAS AND ITS SPEED IS
	   REVERSED AND DAMPED (-v+v/3). THE PC TESTS THE CRAFT'S WHOLE SHAPE (sprite_collision, MAKEBACK/SPRITE.C:341); SO DOES THIS
	   (Craft_Mask_Collision, SPRITE2.C), WITH THE SHAPE MADE FROM THE PS1'S OWN S_SHIP FRAMES. THIS REPLACES THE ONE POINT THE
	   PS1'S Sprite_Collision TESTED (THE BLOCK WAS ONLY #if 0 BECAUSE THE FIXED START WAS SOLID). THE TEST WAITS FOR THE MAP (craftmaph
	   IS 0 UNTIL Init_Ball: MAIN.C'S FIRST Move_Craft RUNS BEFORE Sort_Map). THE CRASH SAMPLE THE PC PLAYS, FX_SIDCSH1, IS NOT IN SOUND.H: NO SOUND. */
	if((craftmaph!=0) && (Craft_Mask_Collision((int ) x,(int ) y,craftn)==TRUE))
	{
		x=old_x;
		y=old_y;
		ball[0].x=x;
		ball[0].y=y;
		ball[0].vx=-ball[0].vx+(ball[0].vx/3);
		ball[0].vy=-ball[0].vy+(ball[0].vy/3);
		/* RESTORE: NEW WORK (owner's decision, 2026-11-14: THE TURN-UNDO GUARD STAYS). THE PC TESTS AFTER THE TURN TOO (MAKEBACK/MAIN.C:472-477, 598), BUT ONLY GIVES THE POSITION
		   BACK: A TURN THAT PUTS THE SHAPE ON A WALL KEEPS THE NEW FRAME, AND EVERY LATER TEST FAILS IN IT (THE CRAFT STICKS UNTIL THE PLAYER
		   TURNS BACK). HERE THE TURN IS GIVEN BACK WITH THE POSITION, SO THE SHAPE IS NEVER ON A WALL (tests/gameplay/x360-craft.sh). */
		angle=old_angle;
		craftn=old_craftn;
	}
#ifdef X360_TEST_MASKCOST
	{	/* RESTORE: TEST BUILDS ONLY (X360_CFLAGS=-DX360_TEST_MASKCOST=N): N EXTRA SHAPE TESTS A CYCLE, TO MEASURE THEIR COST */
		int		mc;
		volatile FLAG	sink;
		for (mc=0;mc<X360_TEST_MASKCOST;mc++) sink=Craft_Mask_Collision((int ) x,(int ) y,craftn);
	}
#endif

	/* RESTORE: CREATURE-CONTACT DAMAGE, LIVE IN NMS'S PC VERSION (MAKEBACK/MAIN.C:614-623), UNDER #if 0 HERE
	   (owner's decision). THE PC TAKES ships_shield_add=-4 OFF A BAR THAT STARTS FULL AT 32*8=256 (MAIN.C:701-702),
	   SO 4/256; THE PS1'S shields RUNS 0 TO FULLSHIELDS (127): THE SAME FRACTION IS 2. THE CRAFT FLASHES FOR 16
	   FRAMES AND NO NEW TOUCH COUNTS WHILE IT FLASHES. shields REACHING 0 IS Print_Panel'S NORMAL DEATH. THE
	   EXPLOSION SPRITE NMS COMMENTED OUT STAYS OUT. */
	/* RESTORE: owner's standing rule (2026-11-13, "let's provide some feedback if a feature was never implemented, even a quick message
	   on screen will do"). NO NMS VERSION RESCUES A SURVIVOR: THE PC'S AND THE SATURN'S ai_new_man ONLY WALK (MAKEBACK/AI.S:37-72,
	   20SEPT96.ZIP INCLUDES/AI.DAT:388-670) AND NOTHING WRITES rescuedsurvivors (LEVEL.C). TOUCHING ONE SHOWS A SHORT MESSAGE (MAIN.C). */
	if(Craft_Touches_Creature(YES)==TRUE) survivor_message=50;

	if(Craft_Touches_Creature(NO)==TRUE)
	{
		if(craft_flashing==NO)
		{
			if(shields>0) shields=(shields>2)?shields-2:0;
			SsUtKeyOn(vab,SFX_OUCH1,0,FXPITCH,0,sfxvolume,sfxvolume);	/* RESTORE: NEW WORK (owner's decision 2026-11-21): SOUND.H's SFX_OUCH1, NAMED BY NMS AND PLAYED BY NOTHING, ON A CREATURE'S TOUCH */
			craft_flashing=YES;
			craft_flash_count=16;
		}
	}

	craftx=ball[0].x;
	crafty=ball[0].y;
	ball_x=ball[1].x;
	ball_y=ball[1].y;

	/* RESTORE: completion, rebuilt from the PC version (MAKEBACK/MAIN.C:902-947, process_pod_and_craft AND update_pod_position): A POD IN
	   HAND IS WHERE ITS BALL IS, AND ITS FRAME IS THE ROPE'S ANGLE (64 FRAMES). */
	if (pod_attached==TRUE)
	{
		sprites[pod_no].x=ball_x;
		sprites[pod_no].y=ball_y;
		sprites[pod_no].obj[obj_x]=ball_x;
		sprites[pod_no].obj[obj_y]=ball_y;
		sprites[pod_no].frame=((int ) ((atan2(ball[0].x-ball[1].x,ball[1].y-ball[0].y)*64)/(PI*2)))&63;
	}

#if defined(X360_TEST_POD_X) || defined(X360_TEST_POD_WATCH)
	{	/* RESTORE: TEST BUILDS ONLY: THE POD'S STATE FOR tests/gameplay/x360-pod.sh */
		extern LONG testpodn,testpodx,testpody,testpodatt,testballx,testbally;
		if (testpodn!=-1) { testpodx=sprites[testpodn].x; testpody=sprites[testpodn].y; }
		testpodatt=pod_attached;
		testballx=ball[1].x; testbally=ball[1].y;
	}
#endif

	sprites[craft].x=craftx;
	sprites[craft].y=crafty;
	sprites[craft].frame=craftn;
	sprites[craft].visible=TRUE;
	if(craft_flashing==YES)
	{
		craft_flash_count--;
		if(craft_flash_count==0) craft_flashing=NO;
		if( (craft_flash_count&3)==0) sprites[craft].visible=FALSE;
	}
}

void andy_thrust(int ball_num, double angle, double force, double time)
{
	double acc_x, acc_y;

	acc_x=force*sin(angle)/ball[ball_num].mass;
	acc_y=force*cos(angle)/ball[ball_num].mass;

	ball[ball_num].vx+=acc_x*time;
	ball[ball_num].vy-=acc_y*time;

	return;
}

void 
update_all(flag pod_attached)
{
	double delta_x,delta_y;
	double hyp,error;

	if(ships_gravity==0)
	{
		gravity(0,TIME_STEP);
		gravity(1,TIME_STEP);
	}
	else
	{
		gravity(0,-TIME_STEP);
		gravity(1,-TIME_STEP);
		ships_gravity--;
	}
	friction(0,TIME_STEP,friction_factor);
	friction(1,TIME_STEP,friction_factor);

	delta_x=ball[0].x-ball[1].x;
	delta_y=ball[0].y-ball[1].y;

	if(pod_attached==TRUE)
	{
		/* RESTORE: completion, rebuilt from the PC version (MAKEBACK/MAIN.C:189-196): A TAUT ROPE MAKES THE CRAFT AND THE POD SHARE THEIR SPEED
		   ALONG IT (ball_collision2, BELOW). THE PC'S rope_min TEST IS UNDER #ifdef STIFF THERE, AND STAYS OUT. */
		if (sqrt((delta_x*delta_x)+(delta_y*delta_y))>rope_max)
			ball_collision2(0,1);

		move(1,TIME_STEP);
	}
	move(0,TIME_STEP);

	// Fudge... (should share the error a bit more angley...)

	if(pod_attached==TRUE)
	{
	delta_x=ball[0].x-ball[1].x;
	delta_y=ball[0].y-ball[1].y;

	hyp=sqrt((delta_x*delta_x)+(delta_y*delta_y));
  
	if(hyp>rope_max)
	{
		error=(hyp-rope_max)/4;
		if( (ball[0].x) > (ball[1].x) ) 
		{
			ball[0].x-=error;
			ball[1].x+=error;
		}
		else
		{
			ball[0].x+=error;
			ball[1].x-=error;
		}
		if( (ball[0].y) > (ball[1].y) ) 
		{
			ball[0].y-=error;
			ball[1].y+=error;
		}
		else
		{
			ball[0].y+=error;
			ball[1].y-=error;
		}
	}
	}
	return;
}

/* RESTORE: completion, rebuilt from the PC version (MAKEBACK/MAIN.C:239-280). Press with a pod in hand: DROP IT; the pod's own routine (ai_pod,
   AI.DAT) then makes it fall: obj_temp7..10 ARE ITS pods_dx, pods_dy, pods_ddx, pods_ddy (THE PC'S VALUES). Press with none: TAKE THE FIRST POD THAT IS
   FLASHING (ai_pod sets obj_temp1 to 1 WITHIN 128 PIXELS OF THE CRAFT; THE CRAFT MUST ALSO BE THE ONE THE POD IS IN RANGE OF) AND MARK IT TAKEN
   (obj_temp1=2). THE PS1'S OLD VERSION LOOKED FOR THE PC'S JUNE ACTION ACTION_PICKUP_POD, WHICH NOTHING SETS HERE. THE POD IS THE CREATURE NUMBER 171
   (podtemp, Saturn art S_POD: THE PC'S NAME_pod4 HAS NO PLACEMENT OR ART ON THE PS1). */
#define POD_NUMBER	171

void ship_pickup_pod(flag *pod_attached)
{
	int i;

	if(*pod_attached==YES)
	{
		sprites[pod_no].obj[obj_temp1]=0;
		*pod_attached=NO;
		sprites[pod_no].obj[obj_x]=sprites[pod_no].x;
		sprites[pod_no].obj[obj_y]=sprites[pod_no].y;
		sprites[pod_no].obj[obj_temp7]=0;
		sprites[pod_no].obj[obj_temp8]=32768+16384;
		sprites[pod_no].obj[obj_temp9]=0;
		sprites[pod_no].obj[obj_temp10]=1024+512;
		return;
	}

	*pod_attached=NO;

	for(i=0;i<MAX_SPRITES;i++)			/* SCAN FOR PODS TO PICK UP */
	{
		if( (sprites[i].on==TRUE) && (sprites[i].name==POD_NUMBER) )
		{
			if(sprites[i].obj[obj_temp1]!=0) break;
		}
	}

	if(i==MAX_SPRITES) return;
	sprites[i].obj[obj_temp1]=2;

	*pod_attached=YES;
	ball[1].x=sprites[i].x;
	ball[1].y=sprites[i].y;
	ball[1].vx=0;
	ball[1].vy=0;

	pod_no=i;
}

/* RESTORE: completion, rebuilt from the PC version (MAKEBACK/MAIN.C:86-125), unchanged: TWO BALLS ON A ROPE SHARE THE SPEED ALONG THE LINE BETWEEN THEM
   (MOMENTUM KEPT), AND KEEP THEIR SPEED ACROSS IT. */
void ball_collision2(short ball1, short ball2)
{
	double collision_ang;

	double vel_ball1, vel_ball2;
	double perp_vel_ball1, perp_vel_ball2;
	double new_vel_ball1, new_vel_ball2;
	double momentum_ball1, momentum_ball2;
	double total_initial_momentum;

	collision_ang=atan2(ball[ball2].x-ball[ball1].x,
							  ball[ball2].y-ball[ball1].y);

	vel_ball1=ball[ball1].vx*sin(collision_ang)+ball[ball1].vy*cos(collision_ang);
	vel_ball2=ball[ball2].vx*sin(collision_ang)+ball[ball2].vy*cos(collision_ang);

	perp_vel_ball1=-ball[ball1].vy*sin(collision_ang)+ball[ball1].vx*cos(collision_ang);
	perp_vel_ball2=-ball[ball2].vy*sin(collision_ang)+ball[ball2].vx*cos(collision_ang);

	momentum_ball1=vel_ball1*ball[ball1].mass;
	momentum_ball2=vel_ball2*ball[ball2].mass;
	total_initial_momentum=momentum_ball1+momentum_ball2;

	new_vel_ball1=total_initial_momentum/(ball[ball1].mass+ball[ball2].mass);
	new_vel_ball2=new_vel_ball1;

	ball[ball1].vx=new_vel_ball1*sin(collision_ang)+perp_vel_ball1*cos(collision_ang);
	ball[ball1].vy=new_vel_ball1*cos(collision_ang)-perp_vel_ball1*sin(collision_ang);

	ball[ball2].vx=new_vel_ball2*sin(collision_ang)+perp_vel_ball2*cos(collision_ang);
	ball[ball2].vy=new_vel_ball2*cos(collision_ang)-perp_vel_ball2*sin(collision_ang);
}

void gravity(int ball_num, double time)
{
	ball[ball_num].vy+=gravity_factor*time;
}

void friction(int ball_num, double time, double fric)
{
	ball[ball_num].vy-=ball[ball_num].vy*fric*time;
	ball[ball_num].vx-=ball[ball_num].vx*fric*time;
}

void move(int ball_num, double time)
{
	ball[ball_num].y+=ball[ball_num].vy*time;
	ball[ball_num].x+=ball[ball_num].vx*time;
}

/* RESTORE: bug fix. NMS'S PC AND SATURN VERSIONS SET THE CRAFT'S POSITION AND STOP IT WHEN A LEVEL STARTS
   (PC MAKEBACK/LEVEL.C:76-79: ball[0].x=craftx, ball[0].y=crafty, ball[0].vx=0, ball[0].vy=0; SATURN LOGIC/LEVEL.C:158-161,
   the same in 16.16). THE PS1 JULY FILE LACKS IT: ball[0] IS SET ONCE BY ITS INITIALISER (256,256), SO A NEW LIFE OR MISSION STARTED
   WHERE THE LAST ONE ENDED, WITH ITS SPEED, AND MAIN.C BUILT THE START VIEW FROM THAT (STRAY TILES, CRAFT OFF SCREEN). THE PS1
   START IS THE INITIALISER'S 256,256 (docs/DECISIONS.md: THE PS1 SPAWN); CALLED BEFORE THE CRAFT IS ADDED, AS THE PC DOES IT BEFORE ITS START VIEW. */

/* RESTORE: completion, rebuilt from the Saturn port (owner's decision 2026-11-13); positions revised for the craft's shape. THE PC AND
   SATURN PLACE THE CRAFT AT THE LAST NAME_CSHIP (BMF NUMBER 1) PLACEMENT OF THE LEVEL'S BMF (PC MAKEBACK/LEVEL.C:55-70, SATURN
   LOGIC/LEVEL.C:125-140, THE LOOP KEEPS THE LAST); THE PS1 BMFs HAVE NONE. THE SATURN TREE'S LEVELS/LEVEL1.BMF TO LEVEL6.BMF (20SEPT96;
   140-BYTE ENTRIES, BIG-ENDIAN) HOLD TWO OR THREE; MISSIONS 7 TO 9 HAVE NONE, AND THE PC'S OWN DEFAULT IS 250,431.
   THE MARKERS FIT THE CRAFT'S SHAPE WHERE THE PC DRAWS IT: ITS SPRITE AT x+u, y+v. THE PS1 DRAWS IT 8 PIXELS LOWER (Print_Sprites,
   y + h/2 + 2), SO A START IS THE MARKER'S y MINUS 8 (THE CRAFT THEN COVERS THE SAME PIXELS AS IN THE REFERENCE).
   RULE: THE LAST MARKER WHOSE SHAPE, IN ALL 64 FRAMES, IS ON NO SOLID PIXEL (Craft_Mask_Collision). MISSION 1: THE LAST (767,932) FITS
   4 FRAMES, THE SECOND (350,833) ALL: 350,825. MISSIONS 2, 3, 4: THE LAST MARKER FITS: 410,847; 606,132; 196,687. MISSION 7: THE PC'S DEFAULT
   FITS: 250,423. NEW WORK (OWNER'S DECISION, 2026-11-14: THE STARTS OF MISSIONS 5, 8 AND 9 STAY): MISSIONS 5 AND 6 (NO MARKER FITS ALL FRAMES: THE NEAREST POINT THAT DOES TO THE LAST
   MARKER, 678,677 AND 676,434) AND MISSIONS 8 AND 9 (NO NMS POINT FITS: THE NEAREST TO THE SATURN DEFAULT 125,70 THAT FITS AND LIES IN OPEN SPACE OF AT LEAST
   30000 PIXELS, 125,78 AND 125,94; THE NEAREST FIT, 125,50, IS A POCKET OF 2000 TO 4000 PIXELS). docs/DECISIONS.md. */
/* RESTORE: NEW WORK (owner's decision, 2026-11-14: Mission 6 starts at candidate B). MISSION 6 STARTS AT 820,434, NOT 676,434: THE OLD START IS 16 TO 18 PIXELS FROM TWO
   msspit SPITTERS (692,430 AND 690,438) WHOSE BOXES THE CRAFT OVERLAPPED, SO THE CONTACT RULE (Craft_Touches_Creature, THE PC'S MAKEBACK/MAIN.C:614-623) TOOK 2 SHIELD
   POINTS EVERY 16 CYCLES AND THE CRAFT STANDING STILL WAS DESTROYED AT GAME CYCLE 1009. 820,434 FITS THE CRAFT'S SHAPE IN ALL 64 FRAMES AND NO CREATURE TOUCHES IT. docs/DECISIONS.md. */
/* RESTORE: NEW WORK (owner's decision, 2026-11-21: "use the spawn from the PC/Saturn, and adjust them so they're quite far from the
   collision"; THIS REPLACES THE STARTS ABOVE, MISSION 6'S CANDIDATE B AND THE 2026-11-14 DECISION ON 5, 8 AND 9). EACH MISSION STARTS AT THE
   POINT NEAREST THE CRAFT MARKER THE PC AND SATURN CODE USE (THE LAST NAME 1 ENTRY OF THE SATURN LEVELn.BMF: 767,932; 410,855; 606,140;
   196,695; 676,687; 676,446; MISSIONS 7 TO 9 HAVE NONE, SO THE PC CODE'S DEFAULT 250,431) WHERE THE CRAFT HAS AT LEAST 48 PIXELS OF OPEN
   COLLISION MAP ALL ROUND ITS CENTRE (x, y+2), IN THE MAP'S LARGEST OPEN AREA, WITH THE MAP'S EDGE COUNTED AS WALL AND MISSIONS 7 TO 9'S MAPS READ
   980 WIDE (colwidth, SCROLLER.C), AND WHERE THE CRAFT, LEFT ALONE, FALLS TO THE FLOOR BELOW AND LOSES NO SHIELD FOR AT LEAST 1000 ROUTE
   TICKS (A CREATURE NEAR THE FLOOR UNDER THE NEAREST POINTS OF MISSIONS 2, 4, 6 AND 9 TOOK SHIELDS AT ONCE, OR DESTROYED THE CRAFT; THOSE
   STARTS ARE THE NEXT POINTS THAT PASS: MISSION 2 STRAIGHT ABOVE ITS MARKER, MISSION 6 AT 820,446 BESIDE THE 2026-11-14 CANDIDATE B,
   MISSION 4 235 PIXELS EAST OF ITS MARKER). THE MARKERS THEMSELVES REST THE CRAFT ON A LEDGE (0 TO 3 PIXELS CLEAR). docs/DECISIONS.md. */
static const SHORT startx[9]={780,410,574,431,703,820,246,198,268};
static const SHORT starty[9]={874,728, 98,612,599,446,414,422,371};

void
Reset_Craft(void)
{
	ball[0].x=startx[(level<9)?level:0];
	ball[0].y=starty[(level<9)?level:0];
#ifdef X360_TEST_START_X
	ball[0].x=X360_TEST_START_X;	/* RESTORE: TEST BUILDS ONLY: A START TO TRY (docs/DECISIONS.md: the starts) */
	ball[0].y=X360_TEST_START_Y;
#endif
	/* RESTORE: completion, rebuilt from the Saturn port (owner's report 2026-11-18, "the ship can barely move"). EVERY LIFE STARTS
	   POINTING UP AND THRUSTING UP. THE PC AND THE JULY PS1 LEFT angle AND craftn AT 0: FRAME 0 (UP) WAS DRAWN BUT THRUST USES
	   (angle+16)&63 = 16, TO THE RIGHT (PC MAKEBACK/GLOBALS.H:252-253, MAIN.C:475,561), SO IN MISSION 1'S 22-PIXEL FUNNEL THE
	   CRAFT WAS PUSHED INTO THE WALL AND THE FIRST TURN JUMPED THE DRAWN FRAME FROM 0 TO 15. THE SATURN DRAWS AND THRUSTS BY THE
	   SAME INDEX (20SEPT96.ZIP LOGIC/CONTROL.C:107,210,226: craftn=craft_angle, angles[craft_angle]), STARTING AT 0, UP. HERE THAT
	   IS angle 48 ((48+16)&63 = 0) WITH FRAME 0. docs/DECISIONS.md. */
	angle=48;
	craftn=0;
#ifdef X360_TEST_HEADING
	angle=(X360_TEST_HEADING-16)&63;	/* RESTORE: TEST BUILDS ONLY (X360_CFLAGS=-DX360_TEST_HEADING=n): EVERY LIFE STARTS ON FRAME n, DRAWN, THRUSTING AND FIRING THAT WAY (16: RIGHT, AS THE GAMEPLAY TESTS' SCENES WERE LAID OUT) */
	craftn=X360_TEST_HEADING;
#endif
	survivor_message=0;		/* RESTORE: THE RESCUE MESSAGE (MAIN.C) DOES NOT CARRY INTO A NEW LIFE */
	/* RESTORE: bug fix (2026-11-21 audit). NOTHING ELSE ENDS THESE WITH A LIFE: A POD HELD WHEN THE CRAFT DIED, QUIT OR FINISHED STAYED ON
	   THE ROPE IN THE NEXT LIFE (ITS ball[1] WHERE IT WAS, THE ROPE DRAGGING THE CRAFT ACROSS THE MAP, A RELOADED CREATURE IN ITS SLOT
	   MOVED AS THE POD); A FLASH FROM THE LAST TOUCH CARRIED OVER; AND THE FIRST Move_Craft OF A LIFE (MAIN.C, BEFORE Init_Level'S
	   Init_Fire) COULD FIRE INTO LAST LIFE'S SHOT SLOTS, NOW HOLDING NEW CREATURES. fire_delay HOLDS THE FIRE FOR NMS'S 16 FRAMES. */
	pod_attached=NO;
	craft_flashing=NO;
	craft_flash_count=0;
	fire_delay=16;
	ball[0].vx=0;
	ball[0].vy=0;
	craftmapw=0;			/* RESTORE: THE MAP IS NOT SORTED YET, SEE Move_Craft */
	craftmaph=0;
	craftx=ball[0].x;
	crafty=ball[0].y;
}

void
Init_Ball(void)
{
		andy_angle=PI;
		ball[0].mass=100;
		ball[1].mass=50;
		gravity_factor=800;
		thrust_factor=500000;
		friction_factor=0.5;
		rope_length=32;
		rope_max=rope_length;
		rope_min=rope_length-0.3;
		fuel_deplete=0;			/* RESTORE: PC MAKEBACK/MAIN.C:698 */
		ships_fuel_add=0;		/* RESTORE: PC MAKEBACK/MAIN.C:699-703 */
		ships_shield_add=0;
		ships_gravity=0;
		craftmapw=mapwidth;		/* RESTORE: THE MAP IS SORTED BY NOW (MAIN.C: Sort_Map BEFORE Init_Level) */
		craftmaph=mapheight;
}



/* RESTORE: completion, rebuilt from the PC version (owner's decision 2026-11-13, "every gameplay feature completed in other ports"):
   THE FUEL, SHIELD AND GRAVITY PICK-UPS (MAKEBACK/OLD.C:233-287, ACTION_PICKUP_FUEL, _SHIELD, _GRAVITY). THE PC CYCLES THE PICK-UP'S FRAMES (EVERY 4
   TICKS; HERE ai_cycle DOES), AND WHEN THE CRAFT'S CENTRE IS INSIDE THE FRAME'S RECTANGLE (collided_with_craft, MAKEBACK/SCRATCH.C:521) THE PICK-UP IS TAKEN: FUEL ships_fuel_add=64 AND SHIELD ships_shield_add=64 OF THE PC'S 256-UNIT GAUGES, AND
   ships_gravity=2048 CYCLES OF REVERSED GRAVITY (update_all ALREADY RUNS THAT). THE PC'S GAUGES FILL ONE UNIT A CYCLE (MAKEBACK/SPLIT.C:16-45), ONLY
   WHILE NOT FULL: HERE THE SAME FRACTIONS OF THE PS1'S GAUGES, FUEL 48 OF 192 (3 EVERY 4 CYCLES) AND SHIELDS 32 OF 127 (1 EVERY 2 CYCLES), BOTH 64 CYCLES.
   THE PC MAKES THE TAKEN PICK-UP ACTION_DYING; THE PS1 HAS NO DYING ACTION FOR IT, SO IT IS SWITCHED OFF. NOTHING IN ANY VERSION CREATES A PICK-UP
   (THE PC'S OLD.C IS JUNE CODE, NO TABLE ROW OR PLACEMENT NAMES ONE): THIS IS ONLY REACHED BY A SPRITE WHOSE action IS ONE OF THE THREE. NO MISSION HAS ONE. */
int	ships_fuel_add;
int	ships_shield_add;

/* THE RECTANGLE IS THE ONE THE PS1 DRAWS THE FRAME IN (sprtable, sprframe, AS Craft_Touches_Creature: y + h/2 + 2 + v), AND THE PC'S POINT, THE CRAFT'S
   CENTRE, IS THE CENTRE OF THE CRAFT'S DRAWN FRAME, SO BOTH ARE IN THE SAME SPACE. */
static int
Pickup_Hit(int i)
{
	SPRITE	*fspr;
	int	x,y,width,height;
	int	cx,cy;

	if ((sprtable[i]==NULL) || (sprtable[craft]==NULL)) return FALSE;	/* NO ART, NO BOX */
	fspr=sprtable[craft];
	cy=(int) crafty+(fspr->h/2)+2;
	fspr+=craftn;
	cx=(int) craftx+fspr->u+(fspr->w/2);
	cy+=fspr->v+(fspr->h/2);
	fspr=sprtable[i];
	y=sprites[i].y+(fspr->h/2)+2;
	fspr+=sprframe[i];
	x=sprites[i].x+fspr->u;
	y+=fspr->v;
	width=fspr->w;
	height=fspr->h;
	if ((cx>=x) && (cx<(x+width)) && (cy>=y) && (cy<(y+height))) return TRUE;
	return FALSE;
}

void
Pickups_Tick(void)
{
	int	i;

#ifdef X360_TEST_PICKUP
	/* RESTORE: NEW WORK, TEST BUILDS ONLY (X360_CFLAGS=-DX360_TEST_PICKUP=0|1|2: FUEL, SHIELD, GRAVITY; ART AND MISSION 6 ONLY): AT GAME CYCLE 100 THE
	   GAUGES ARE SET TO A HALF-EMPTY BASELINE (fuel 96, shields 64) AND A PICK-UP OF THAT KIND IS PUT AT THE CRAFT (MISSION 6's CRYSTAL l6c1, BMF 158, STANDS IN FOR THE
	   ART THAT DOES NOT EXIST). FOR tests/gameplay/x360-pickup.sh */
	if (gamecycle==100)
	{
		fuel=96;
		shields=64;
		i=add_sprite(158,ACTION_PICKUP_FUEL+X360_TEST_PICKUP,craftx,crafty);
		if (i!=-1)
		{
			sprites[i].move_ptr=0;
			sprites[i].frame=0;
			sprites[i].startframe=0;
			sprites[i].endframe=0;
			sprites[i].priority=5;
			sprites[i].clipped=NO;
			sprites[i].shootable=NO;
		}
	}
#endif

	for (i=0;i<MAX_SPRITES;i++)
	{
		if ((sprites[i].on!=TRUE) || (sprites[i].action<ACTION_PICKUP_FUEL) || (sprites[i].action>ACTION_PICKUP_GRAVITY)) continue;
		if (Pickup_Hit(i)==FALSE) continue;
		if (sprites[i].action==ACTION_PICKUP_FUEL) ships_fuel_add=48;
		else if (sprites[i].action==ACTION_PICKUP_SHIELD) ships_shield_add=32;
		else ships_gravity=2048;
		sprites[i].on=FALSE;
		sprites[i].visible=FALSE;
	}
	if ((ships_fuel_add>0) && (fuel<(12*FUELMULT)) && ((gamecycle & 3)==0))
	{
		fuel+=3;
		ships_fuel_add-=3;
	}
	if ((ships_shield_add>0) && (shields<FULLSHIELDS) && ((gamecycle & 1)==0))
	{
		shields++;
		ships_shield_add--;
	}
}

#if defined(X360_TEST_POD_X) || defined(X360_TEST_POD_WATCH)
/* RESTORE: TEST BUILDS ONLY: THE POD'S STATE FOR tests/gameplay/x360-pod.sh */
LONG	testpodn=-1;
LONG	testpodx=0;
LONG	testpody=0;
LONG	testpodatt=0;
LONG	testballx=0;
LONG	testbally=0;
#endif

/* RESTORE: NEW WORK (owner's decision, 2026-11-14: one pod in Mission 6, at candidate A of the pod proposals, 585,441). NO NMS MAP GIVES A POD A POSITION: MISSION 6'S MAP HAS ONE podtemp LINE AT 0,0,
   THE MAP EDITOR'S DEFAULT. add_sprite (PROCESS.C) PUTS THAT PLACEMENT AT 585,441, 30 PIXELS RIGHT OF THE SURVIVORS AT 548,441 AND 555,444, WHERE ITS 64 FRAMES AND THE CRAFT'S SHAPE ARE ON NO
   SOLID PIXEL (docs/DECISIONS.md). THIS FINDS IT AFTER THE MAP IS LOADED AND SETS IT AS A STILL POD: FRAME 0, NOT SHOOTABLE, NOT CLIPPED, PRIORITY 5 (THE FIELDS THE POD GATE'S TEST POD HAS). THE POD'S ART IS
   LOADED IN MISSION 6 ONLY (LOADDATA.C), SO NO OTHER MISSION GETS ONE. THE ROPE STAYS THE ONE-PIXEL WHITE LINE. */
void
Add_Level_Pod(void)
{
	LONG	i;

	if (level!=5) return;
	for (i=0;i<MAX_SPRITES;i++)
		if ((sprites[i].on==TRUE) && (sprites[i].name==POD_NUMBER) && (sprites[i].x==585) && (sprites[i].y==441))
		{
			sprites[i].frame=0;
			sprites[i].startframe=0;
			sprites[i].endframe=0;
			sprites[i].priority=5;
			sprites[i].clipped=NO;
			sprites[i].shootable=NO;
#ifdef X360_TEST_POD_WATCH
			testpodn=i;	/* RESTORE: TEST BUILDS ONLY: x360-pod.sh WATCHES THE POD THAT WAS PLACED */
#endif
			break;
		}
}

#ifdef X360_TEST_POD_X
/* RESTORE: NEW WORK, TEST BUILDS ONLY (X360_CFLAGS='-DX360_TEST_POD_X=650 -DX360_TEST_POD_Y=430'): ONE POD AT THAT PLACE OF THE MISSION (THE POD'S ART IS
   LOADED IN MISSION 6 ONLY), AS A MAP WOULD PLACE IT; testpodx AND testpody ARE ITS POSITION, FOR tests/gameplay/x360-pod.sh */

void
Test_Add_Pod(void)
{
	LONG	n,i;

	/* THE MAP'S OWN PLACEHOLDER (0,0) IS SWITCHED OFF, SO THE TEST POD IS THE ONLY ONE (A WARP TO NAME 171 FINDS IT) */
	for (i=0;i<MAX_SPRITES;i++)
		if ((sprites[i].on==TRUE) && (sprites[i].name==POD_NUMBER)) sprites[i].on=FALSE;

	n=add_sprite(POD_NUMBER,ACTION_CYCLE,X360_TEST_POD_X,X360_TEST_POD_Y);
	if (n==-1) return;
	sprites[n].frame=0;
	sprites[n].startframe=0;
	sprites[n].endframe=0;
	sprites[n].priority=5;
	sprites[n].clipped=NO;
	sprites[n].shootable=NO;
	testpodn=n;
}
#endif
