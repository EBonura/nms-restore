
#ifndef	__H_LOADDATA
#define	__H_LOADDATA

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Load_Data(void);
void	Load_Mission_Sprites(void);		/* RESTORE: MISSIONS 2..9 */

/**************************************
*          DECLARE DEFINES            *
**************************************/

/**************************************
*         DECLARE VARIABLES           *
**************************************/

extern	SPRITE*	levelspr[256];		/* RESTORE: BMF NUMBER -> SPRITES */
extern	UBYTE	levelcount[256];		/* RESTORE: FRAMES IN EACH */
extern	SPRITE*	spr_S_EXP1;
extern	SPRITE*	spr_S_EXP4;
extern	SPRITE*	spr_CHLINK;
extern	SPRITE*	spr_S_EXP6;
extern	SPRITE*	spr_S_EXP7;
extern	SPRITE*	spr_S_SPFIRE;

extern	UBYTE	*particle;
extern	SPRITE*	spr_PARTICLE;


extern	UBYTE	*guidwire;
extern	SPRITE*	spr_GUIDWIRE;

extern	UBYTE	*laswire;
extern	SPRITE*	spr_LASWIRE;

extern	UBYTE	*bckwire;
extern	SPRITE*	spr_BCKWIRE;

extern	UBYTE	*ebwire;
extern	SPRITE*	spr_EBWIRE;

extern	UBYTE	*agmwire;
extern	SPRITE*	spr_AGMWIRE;


extern	UBYTE	*min1wire;
extern	SPRITE*	spr_MIN1WIRE;


extern	UBYTE	*guided;
extern	SPRITE*	spr_GUIDED;

extern	UBYTE	*buck01;
extern	SPRITE*	spr_BUCK01;

extern	UBYTE	*buck02;
extern	SPRITE*	spr_BUCK02;

extern	UBYTE	*eball;
extern	SPRITE*	spr_EBALL;

extern	UBYTE	*mine;
extern	SPRITE*	spr_MINE;


extern	UBYTE	*flopl1;
extern	SPRITE*	spr_FLOPL1;

extern	UBYTE	*l1c1;
extern	SPRITE*	spr_L1C1;

extern	UBYTE	*l1c2;
extern	SPRITE*	spr_L1C2;

extern	UBYTE	*l1c3;
extern	SPRITE*	spr_L1C3;

extern	UBYTE	*l1c4;
extern	SPRITE*	spr_L1C4;

extern	UBYTE	*l1c5;
extern	SPRITE*	spr_L1C5;

extern	UBYTE	*plungl1;
extern	SPRITE*	spr_PLUNGL1;

extern	UBYTE	*s_acidsp;
extern	SPRITE*	spr_S_ACIDSP;

extern	UBYTE	*s_rck01;
extern	SPRITE*	spr_S_RCK01;

extern	UBYTE	*s_rck02;
extern	SPRITE*	spr_S_RCK02;

extern	UBYTE	*s_rck03;
extern	SPRITE*	spr_S_RCK03;

extern	UBYTE	*s_float;
extern	SPRITE*	spr_S_FLOAT;

extern	UBYTE	*s_lrtad;
extern	SPRITE*	spr_S_LRTAD;

extern	UBYTE	*s_mrspit;
extern	SPRITE*	spr_S_MRSPIT;

extern	UBYTE	*s_msspit;
extern	SPRITE*	spr_S_MSSPIT;

extern	UBYTE	*s_tubel1;
extern	SPRITE*	spr_S_TUBEL1;

extern	UBYTE	*s_rckmph;
extern	SPRITE*	spr_S_RCKMPH;

extern	UBYTE	*s_star;
extern	SPRITE*	spr_S_STAR;

extern	UBYTE	*s_starfs;
extern	SPRITE*	spr_S_STARFS;

extern	UBYTE	*s_survs;
extern	SPRITE*	spr_S_SURVS;

extern	UBYTE	*s_turret;
extern	SPRITE*	spr_S_TURRET;

extern	UBYTE	*s_chapie;
extern	SPRITE*	spr_S_CHAPIE;

extern	UBYTE	*s_musl1;
extern	SPRITE*	spr_S_MUSL1;

extern	UBYTE	*s_tenta1;
extern	SPRITE*	spr_S_TENTA1;

extern	UBYTE	*s_tenta2;
extern	SPRITE*	spr_S_TENTA2;

extern	UBYTE	*s_tnsl1;
extern	SPRITE*	spr_S_TNSL1;

extern	UBYTE	*s_vil1;
extern	SPRITE*	spr_S_VIL1;

extern	UBYTE	*s_bounc1;
extern	SPRITE*	spr_S_BOUNC1;

extern	UBYTE	*s_bounc2;
extern	SPRITE*	spr_S_BOUNC2;

extern	UBYTE	*s_bounc3;
extern	SPRITE*	spr_S_BOUNC3;

extern	UBYTE	*s_ship;
extern	SPRITE*	spr_S_SHIP;

extern	UBYTE	*border1;
extern	SPRITE*	spr_BORDER1;

extern	UBYTE	*barsl;
extern	SPRITE*	spr_BARSL;

extern	UBYTE	*barsr;
extern	SPRITE*	spr_BARSR;

extern	UBYTE	*shipwire;
extern	SPRITE*	spr_SHIPWIRE;

#endif

