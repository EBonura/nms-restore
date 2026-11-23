
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE ROUTINES           *
**************************************/

/**************************************
*           DECLARE DEFINES           *
**************************************/

/**************************************
*          DECLARE SPRITES            *
**************************************/

UBYTE	*particle;
SPRITE*	spr_PARTICLE;

UBYTE	*guidwire;
SPRITE*	spr_GUIDWIRE;

UBYTE	*laswire;
SPRITE*	spr_LASWIRE;

UBYTE	*bckwire;
SPRITE*	spr_BCKWIRE;

UBYTE	*ebwire;
SPRITE*	spr_EBWIRE;

UBYTE	*agmwire;
SPRITE*	spr_AGMWIRE;

UBYTE	*min1wire;
SPRITE*	spr_MIN1WIRE;

UBYTE	*guided;
SPRITE*	spr_GUIDED;

UBYTE	*buck01;
SPRITE*	spr_BUCK01;

UBYTE	*buck02;
SPRITE*	spr_BUCK02;

UBYTE	*eball;
SPRITE*	spr_EBALL;

UBYTE	*mine;
SPRITE*	spr_MINE;


UBYTE	*flopl1;
SPRITE*	spr_FLOPL1;

UBYTE	*l1c1;
SPRITE*	spr_L1C1;

UBYTE	*l1c2;
SPRITE*	spr_L1C2;

UBYTE	*l1c3;
SPRITE*	spr_L1C3;

UBYTE	*l1c4;
SPRITE*	spr_L1C4;

UBYTE	*l1c5;
SPRITE*	spr_L1C5;

UBYTE	*plungl1;
SPRITE*	spr_PLUNGL1;

UBYTE	*s_acidsp;
SPRITE*	spr_S_ACIDSP;

UBYTE	*s_rck01;
SPRITE*	spr_S_RCK01;

UBYTE	*s_rck02;
SPRITE*	spr_S_RCK02;

UBYTE	*s_rck03;
SPRITE*	spr_S_RCK03;

UBYTE	*s_float;
SPRITE*	spr_S_FLOAT;

UBYTE	*s_lrtad;
SPRITE*	spr_S_LRTAD;

UBYTE	*s_mrspit;
SPRITE*	spr_S_MRSPIT;

UBYTE	*s_msspit;
SPRITE*	spr_S_MSSPIT;

UBYTE	*s_tubel1;
SPRITE*	spr_S_TUBEL1;

UBYTE	*s_rckmph;
SPRITE*	spr_S_RCKMPH;

UBYTE	*s_star;
SPRITE*	spr_S_STAR;

UBYTE	*s_starfs;
SPRITE*	spr_S_STARFS;

UBYTE	*s_survs;
SPRITE*	spr_S_SURVS;

UBYTE	*s_turret;
SPRITE*	spr_S_TURRET;

UBYTE	*s_chapie;
SPRITE*	spr_S_CHAPIE;

UBYTE	*s_musl1;
SPRITE*	spr_S_MUSL1;

UBYTE	*s_tenta1;
SPRITE*	spr_S_TENTA1;

UBYTE	*s_tenta2;
SPRITE*	spr_S_TENTA2;

UBYTE	*s_tnsl1;
SPRITE*	spr_S_TNSL1;

UBYTE	*s_vil1;
SPRITE*	spr_S_VIL1;

UBYTE	*s_bounc1;
SPRITE*	spr_S_BOUNC1;

UBYTE	*s_bounc2;
SPRITE*	spr_S_BOUNC2;

UBYTE	*s_bounc3;
SPRITE*	spr_S_BOUNC3;

UBYTE	*s_ship;
SPRITE*	spr_S_SHIP;

UBYTE	*border1;
SPRITE*	spr_BORDER1;

UBYTE	*barsl;
SPRITE*	spr_BARSL;

UBYTE	*barsr;
SPRITE*	spr_BARSR;

UBYTE	*shipwire;
SPRITE*	spr_SHIPWIRE;

/**************************************
*    RESTORE: MISSIONS 2 TO 9 SPRITES *
**************************************/

/* RESTORE: NMS's July code loads only LEVEL1/ (above) and Print_Sprites
   (CONTROL.C) maps only Mission 1's sprite numbers, so in missions 2 to 9
   only the creatures Mission 1 also has are drawn. This table gives each
   of those missions its own set, loaded the way Load_Data loads LEVEL1/.
   The evidence, from NMS's files:

   Numbers. A sprite's number in LEVELn.BMF is its name's index in
   fnames_ian[] (GRAPHICS/MAPS/GLOBALS.H:773), the table MAP2BMF uses
   (GRAPHICS/MAPS/MAP2BMF.C:119,379) to convert the editor's text maps
   (GRAPHICS/MAPS/MAPS/LEVELn.MAP, "SPRITE_INFO=<name> x,y,..."); a name the
   table lacks becomes 0 (MAP2BMF.C:391-392). All 1220 placements of the
   nine BMF files match their .MAP names this way, and Print_Sprites'
   Mission 1 numbers are the same indices (91 chapl1, 92-97 vinel11..vine16,
   103 tonsl1, 104-108 l1c1..l1c5, 110 msspit, 111 lrtad).

   Files. Each creature's art is GRAPHICS/LEVELn/X.PSX with its frame table
   X.TXT; the .SPR the game loads is the .TXT compiled (GRAPHICS/TXT2SPR.BAT,
   TXT2SPR.C). tools/x360_txt2spr.py does that step; for Mission 1 it gives
   28 of LEVEL1/'s 30 .SPR files byte for byte (S_SURVS has no .TXT, S_VIL1
   was made from six images). The files below are built into LEVEL2/ to
   LEVEL9/ (and LEVEL1/S_TAD) by 360x/disc.sh.

   A name is connected to a file only where the files tie them together,
   in three ways (every frame range the map gives fits the file's table):
   N  the file is named for the map's name, as l1c1 is L1C1 and plunger
      PLUNGL1 in Mission 1: lNcK -> LNCK or S_LNCK, plunglN -> S_LNPLUN,
      chaplN -> S_LNCHAP (the Saturn port calls chapl1's art S_L1CHAP,
      20SEPT96 GAME.C:424), lNrock1 -> LNRCK01, mussl3 -> S_L3MUSL,
      chisler -> S_CHISLR, bloba -> S_BLOBA, l2tentr -> S_L2TENR, and tad
      -> GRAPHICS/LEVEL1/S_TAD (Saturn GAME.C:441, NAME_tad = "S_TAD.SCS")
   V  a Mission 1 creature (number and art from Print_Sprites) whose art
      the mission's own folder has redrawn: S_LNSTAR has S_STAR's frame
      table (62 of 63 entries the same, the same 11512-byte size; Mission
      3's copy is named S_L3TAR), S_L3TUBE and S_L6TUBE have S_TUBEL1's
      (all 25), S_LNACD is the acid spitter (S_L2ACD shares 30 of
      S_ACIDSP's 31), S_L4TENL/S_L4TENR are the tentacles (sizes 24x25 and
      24x26 as S_TENTA1 and S_TENTA2; l2tentl/l2tentr name the pair in
      Mission 2), S_L4MUSL the mussel
   1  a Mission 1 creature the mission's folder has no art for keeps
      Mission 1's art, as in the July build
   Number 0 is drawn as the tube, as NMS's code has it (LEVEL.C:320
   "FRIG FOR TUBEL1", CONTROL.C:95), only where LEVELn.MAP names a tube
   for that placement (owner's decision): tubel3 and tubel6, with the
   mission's own tube. Missions 2, 4, 5, 7, 8 and 9 place no tube, so they
   have no number 0 entry here; the 0 placements of missions 3 and 6 that
   came from other names are hidden by LEVEL.C (notube[]).

   Left undrawn, as in the July build, for want of a file: vtestlN,
   lNKbt, s_lNcK (unmapped in Mission 1 too), fflop,
   flop2, g2, fuel2, spiral3, nuray, tubes, l2tentl (its .TXT is
   empty), and every name MAP2BMF did not know (number 0: tonsl3, tonsl5,
   tonsl6, l4c2, s_l7c1.. and l8c2..), whose art (S_L3TNS, S_L5TNSL,
   S_L6TONS, L4C2...) no number reaches.

   RESTORE (owner's decision, 2026-11-10): the Saturn port's sprites the PS1
   tree lacks. Its SPRITES/*.SCS files are converted at disc build time by
   tools/x360_scs2spr.py into the same two files ("../SATURN/X.PSX" and
   X.SPR); the Saturn tree is NMS's own source and data, the converted files
   are not in the repository. Wired where NMS's code places or spawns them:
   S  (this file's rows marked S) a creature the maps place, the file named
      for it by NMS's Saturn port: vines (vinelNK -> S_VN_K, as the Saturn
      DrawSpecialSprites draws NAME_vinel11 with the first of Mission 1's
      six vine images), the pod (podtemp, 64 frames for the map's 0..63),
      the laser creature (lazerl1: GAME.C:426 gives NAME_lazerl1
      S_LASER.SCS; 41 frames, the map's ranges 0..20)
   W  the wind tunnel, in the PS1 tree's own S_WINDY (GRAPHICS/LEVEL1: the
      Saturn S_WINDY.SCS is the same 22 frames, pixel for pixel, same
      palette; x360_scs2spr.py --compare); the maps give windtun the range
      0..44, which the 45-frame S_WINDL2.FLC and S_WINDL4.FLC (no .PSX or
      .TXT) fit and these 22 frames do not. The Saturn's DecodeSCS
      (VDP/VDP1_3.C:106) draws frame 0 for a frame past the file's last, and
      so does Print_Sprites here (levelcount[])
   The explosions and the laser shot are loaded by Load_Data for every
   mission (names NAME_EXPLOSION and NAME_exp4.. in GLOBALS.H). */

typedef struct
{
	UBYTE	level;				/* 1..8 = MISSIONS 2..9, 255 = END */
	UBYTE	live;				/* KEPT IN RAM LIKE S_CHAPIE/S_MUSL1 */
	SHORT	name;				/* NUMBER IN LEVELn.BMF */
	BYTE	*psx;				/* THE .SPR HAS THE SAME NAME (RESTORE: ONE STRING A ROW, FOR RAM) */
} LEVELSPRITE;

LEVELSPRITE levelsprites[]=
{
	/* MISSION 1 (RESTORE: THE SATURN'S SPRITES ONLY) */
	{0,0,109,"../SATURN/S_LASER.PSX"},	/* S lazerl1 */

	/* MISSION 2 */
	{1,0,  4,"../LEVEL1/S_BOUNC3.PSX"},	/* 1 bouncer3 */
	{1,0,  8,"../LEVEL2/S_CHISLR.PSX"},	/* N chisler */
	{1,0, 17,"../LEVEL1/S_MRSPIT.PSX"},	/* 1 mrspit */
	{1,0, 22,"../LEVEL2/S_L2STAR.PSX"},	/* V star */
	{1,0, 25,"../LEVEL2/S_L2ACD.PSX"},	/* V acidspt */
	{1,0, 29,"../LEVEL1/S_SURVS.PSX"},	/* 1 survrs */
	{1,0, 41,"../LEVEL1/S_BOUNC1.PSX"},	/* 1 bouncer1 */
	{1,0, 42,"../LEVEL1/S_BOUNC2.PSX"},	/* 1 bouncer2 */
	{1,0, 72,"../LEVEL2/L2C1.PSX"},		/* N l2c1 */
	{1,0, 73,"../LEVEL2/L2C2.PSX"},		/* N l2c2 */
	{1,0, 74,"../LEVEL2/L2C3.PSX"},		/* N l2c3 */
	{1,0, 75,"../LEVEL2/L2C4.PSX"},		/* N l2c4 */
	{1,0, 76,"../LEVEL2/L2C5.PSX"},		/* N l2c5 */
	{1,0, 82,"../LEVEL2/S_BLOBA.PSX"},	/* N bloba */
	{1,0, 83,"../LEVEL1/S_TAD.PSX"},		/* N tad */
	{1,0, 85,"../LEVEL2/L2RCK01.PSX"},	/* N l2rock1 */
	{1,0,110,"../LEVEL1/S_MSSPIT.PSX"},	/* 1 msspit */
	{1,0,111,"../LEVEL1/S_LRTAD.PSX"},	/* 1 lrtad */
	{1,0,123,"../LEVEL2/S_L2TENR.PSX"},	/* N l2tentr */
	{1,0,124,"../LEVEL2/S_L2PLUN.PSX"},	/* N plungl2 */
	{1,1,125,"../LEVEL2/S_L2CHAP.PSX"},	/* N chapl2 */

	{1,0,112,"../SATURN/S_V2_1.PSX"},	/* S vinel21 */
	{1,0,113,"../SATURN/S_V2_2.PSX"},	/* S vinel22 */
	{1,0,114,"../SATURN/S_V2_3.PSX"},	/* S vinel23 */
	{1,0,115,"../SATURN/S_V2_4.PSX"},	/* S vinel24 */
	{1,0,116,"../SATURN/S_V2_6.PSX"},	/* S vinel26 */
	{1,0, 81,"../LEVEL1/S_WINDY.PSX"},	/* W windtun */
	{1,0,126,"../SATURN/S_LASER.PSX"},	/* S lazerl2 */

	/* MISSION 3 */
	{2,0,  0,"../LEVEL3/S_L3TUBE.PSX"},	/* V tube (tubel3) */
	{2,0,  3,"../LEVEL1/FLOPL1.PSX"},		/* 1 floppy */
	{2,0,  4,"../LEVEL1/S_BOUNC3.PSX"},	/* 1 bouncer3 */
	{2,0, 17,"../LEVEL1/S_MRSPIT.PSX"},	/* 1 mrspit */
	{2,0, 20,"../LEVEL1/S_FLOAT.PSX"},	/* 1 float */
	{2,0, 22,"../LEVEL3/S_L3TAR.PSX"},	/* V star */
	{2,0, 23,"../LEVEL1/S_STARFS.PSX"},	/* 1 starfsh */
	{2,0, 25,"../LEVEL3/S_L3ACD.PSX"},	/* V acidspt */
	{2,0, 29,"../LEVEL1/S_SURVS.PSX"},	/* 1 survrs */
	{2,0, 83,"../LEVEL1/S_TAD.PSX"},		/* N tad */
	{2,0,110,"../LEVEL1/S_MSSPIT.PSX"},	/* 1 msspit */
	{2,0,111,"../LEVEL1/S_LRTAD.PSX"},	/* 1 lrtad */
	{2,0,207,"../LEVEL3/L3RCK01.PSX"},	/* N l3rock1 */
	{2,0,208,"../LEVEL3/S_L3PLUN.PSX"},	/* N plungl3 */
	{2,1,209,"../LEVEL3/S_L3MUSL.PSX"},	/* N mussl3 */
	{2,1,210,"../LEVEL3/S_L3CHAP.PSX"},	/* N chapl3 */
	{2,0,211,"../LEVEL3/L3C1.PSX"},		/* N l3c1 */
	{2,0,212,"../LEVEL3/L3C2.PSX"},		/* N l3c2 */
	{2,0,214,"../LEVEL3/L3C4.PSX"},		/* N l3c4 */
	{2,0,215,"../LEVEL3/L3C5.PSX"},		/* N l3c5 */

	{2,0,198,"../SATURN/S_V3_1.PSX"},	/* S vinel31 */
	{2,0,199,"../SATURN/S_V3_3.PSX"},	/* S vinel33 */
	{2,0,200,"../SATURN/S_V3_4.PSX"},	/* S vinel34 */
	{2,0,201,"../SATURN/S_V3_6.PSX"},	/* S vinel36 */
	{2,0, 81,"../LEVEL1/S_WINDY.PSX"},	/* W windtun */
	{2,0,218,"../SATURN/S_LASER.PSX"},	/* S lazer3 */

	/* MISSION 4 */
	{3,0,  3,"../LEVEL1/FLOPL1.PSX"},		/* 1 floppy */
	{3,0,  4,"../LEVEL1/S_BOUNC3.PSX"},	/* 1 bouncer3 */
	{3,0, 17,"../LEVEL1/S_MRSPIT.PSX"},	/* 1 mrspit */
	{3,0, 20,"../LEVEL1/S_FLOAT.PSX"},	/* 1 float */
	{3,0, 22,"../LEVEL4/S_L4STAR.PSX"},	/* V star */
	{3,0, 23,"../LEVEL1/S_STARFS.PSX"},	/* 1 starfsh */
	{3,0, 25,"../LEVEL1/S_ACIDSP.PSX"},	/* 1 acidspt */
	{3,0, 29,"../LEVEL1/S_SURVS.PSX"},	/* 1 survrs */
	{3,0, 42,"../LEVEL1/S_BOUNC2.PSX"},	/* 1 bouncer2 */
	{3,0, 59,"../LEVEL4/S_L4TENL.PSX"},	/* V tentac1 */
	{3,0, 60,"../LEVEL4/S_L4TENR.PSX"},	/* V tentac2 */
	{3,1, 62,"../LEVEL4/S_L4MUSL.PSX"},	/* V mussel */
	{3,0, 83,"../LEVEL1/S_TAD.PSX"},		/* N tad */
	{3,0,110,"../LEVEL1/S_MSSPIT.PSX"},	/* 1 msspit */
	{3,0,111,"../LEVEL1/S_LRTAD.PSX"},	/* 1 lrtad */
	{3,0,140,"../LEVEL4/L4RCK01.PSX"},	/* N l4rock1 */
	{3,0,141,"../LEVEL4/L4C1.PSX"},		/* N l4c1 */
	{3,0,142,"../LEVEL4/L4C3.PSX"},		/* N l4c3 */
	{3,0,143,"../LEVEL4/L4C4.PSX"},		/* N l4c4 */
	{3,0,144,"../LEVEL4/L4C5.PSX"},		/* N l4c5 */
	{3,0,145,"../LEVEL4/S_L4PLUN.PSX"},	/* N plungl4 */

	{3,0,129,"../SATURN/S_V4_1.PSX"},	/* S vinel41 */
	{3,0,130,"../SATURN/S_V4_2.PSX"},	/* S vinel42 */
	{3,0,131,"../SATURN/S_V4_3.PSX"},	/* S vinel43 */
	{3,0,132,"../SATURN/S_V4_4.PSX"},	/* S vinel44 */
	{3,0,133,"../SATURN/S_V4_6.PSX"},	/* S vinel46 */
	{3,0, 81,"../LEVEL1/S_WINDY.PSX"},	/* W windtun */

	/* MISSION 5 */
	{4,0,  3,"../LEVEL1/FLOPL1.PSX"},		/* 1 floppy */
	{4,0,  4,"../LEVEL1/S_BOUNC3.PSX"},	/* 1 bouncer3 */
	{4,0, 17,"../LEVEL1/S_MRSPIT.PSX"},	/* 1 mrspit */
	{4,0, 20,"../LEVEL1/S_FLOAT.PSX"},	/* 1 float */
	{4,0, 22,"../LEVEL5/S_L5STAR.PSX"},	/* V star */
	{4,0, 23,"../LEVEL1/S_STARFS.PSX"},	/* 1 starfsh */
	{4,0, 25,"../LEVEL5/S_L5ACD.PSX"},	/* V acidspt */
	{4,0, 29,"../LEVEL1/S_SURVS.PSX"},	/* 1 survrs */
	{4,0, 42,"../LEVEL1/S_BOUNC2.PSX"},	/* 1 bouncer2 */
	{4,0, 83,"../LEVEL1/S_TAD.PSX"},		/* N tad */
	{4,0,110,"../LEVEL1/S_MSSPIT.PSX"},	/* 1 msspit */
	{4,0,111,"../LEVEL1/S_LRTAD.PSX"},	/* 1 lrtad */
	{4,0,184,"../LEVEL5/L5RCK01.PSX"},	/* N l5rock1 */
	{4,0,186,"../LEVEL5/L5C1.PSX"},		/* N l5c1 */
	{4,0,187,"../LEVEL5/L5C2.PSX"},		/* N l5c2 */
	{4,0,188,"../LEVEL5/L5C3.PSX"},		/* N l5c3 */
	{4,0,189,"../LEVEL5/L5C4.PSX"},		/* N l5c4 */
	{4,0,190,"../LEVEL5/L5C5.PSX"},		/* N l5c5 */
	{4,1,192,"../LEVEL5/S_L5CHAP.PSX"},	/* N chapl5 */
	{4,0,193,"../LEVEL5/S_L5PLUN.PSX"},	/* N plungl5 */

	{4,0,174,"../SATURN/S_V5_1.PSX"},	/* S vinel51 */
	{4,0,175,"../SATURN/S_V5_2.PSX"},	/* S vinel52 */
	{4,0,176,"../SATURN/S_V5_3.PSX"},	/* S vinel53 */
	{4,0,177,"../SATURN/S_V5_4.PSX"},	/* S vinel54 */
	{4,0,178,"../SATURN/S_V5_6.PSX"},	/* S vinel56 */
	{4,0, 81,"../LEVEL1/S_WINDY.PSX"},	/* W windtun */
	{4,0,191,"../SATURN/S_LASER.PSX"},	/* S lazerl5 */

	/* MISSION 6 */
	{5,0,  0,"../LEVEL6/S_L6TUBE.PSX"},	/* V tube (tubel6) */
	{5,0,  3,"../LEVEL1/FLOPL1.PSX"},		/* 1 floppy */
	{5,0,  4,"../LEVEL1/S_BOUNC3.PSX"},	/* 1 bouncer3 */
	{5,0, 17,"../LEVEL1/S_MRSPIT.PSX"},	/* 1 mrspit */
	{5,0, 20,"../LEVEL1/S_FLOAT.PSX"},	/* 1 float */
	{5,0, 22,"../LEVEL6/S_L6STAR.PSX"},	/* V star */
	{5,0, 23,"../LEVEL1/S_STARFS.PSX"},	/* 1 starfsh */
	{5,0, 25,"../LEVEL1/S_ACIDSP.PSX"},	/* 1 acidspt */
	{5,0, 29,"../LEVEL1/S_SURVS.PSX"},	/* 1 survrs */
	{5,0, 41,"../LEVEL1/S_BOUNC1.PSX"},	/* 1 bouncer1 */
	{5,0, 42,"../LEVEL1/S_BOUNC2.PSX"},	/* 1 bouncer2 */
	{5,0,110,"../LEVEL1/S_MSSPIT.PSX"},	/* 1 msspit */
	{5,0,158,"../LEVEL6/L6C1.PSX"},		/* N l6c1 */
	{5,0,159,"../LEVEL6/L6C2.PSX"},		/* N l6c2 */
	{5,0,160,"../LEVEL6/L6C3.PSX"},		/* N l6c3 */
	{5,0,161,"../LEVEL6/L6C4.PSX"},		/* N l6c4 */
	{5,0,162,"../LEVEL6/L6C5.PSX"},		/* N l6c5 */
	{5,0,168,"../LEVEL6/L6RCK01.PSX"},	/* N l6rock1 */
	{5,0,169,"../LEVEL6/S_L6PLUN.PSX"},	/* N plungl6 */
	{5,1,172,"../LEVEL6/S_L6CHAP.PSX"},	/* N chapl6 */

	{5,0,147,"../SATURN/S_V6_1.PSX"},	/* S vinel61 */
	{5,0,148,"../SATURN/S_V6_2.PSX"},	/* S vinel62 */
	{5,0,149,"../SATURN/S_V6_3.PSX"},	/* S vinel63 */
	{5,0,150,"../SATURN/S_V6_4.PSX"},	/* S vinel64 */
	{5,0,151,"../SATURN/S_V6_5.PSX"},	/* S vinel65 */
	{5,0,152,"../SATURN/S_V6_6.PSX"},	/* S vinel66 */
	{5,0,170,"../SATURN/S_LASER.PSX"},	/* S lazerl6 */
	{5,0,171,"../SATURN/S_POD.PSX"},	/* S podtemp */

	/* MISSION 7 */
	{6,0, 29,"../LEVEL1/S_SURVS.PSX"},	/* 1 survrs */
	{6,0, 83,"../LEVEL1/S_TAD.PSX"},		/* N tad */
	{6,0,111,"../LEVEL1/S_LRTAD.PSX"},	/* 1 lrtad */
	{6,0,220,"../LEVEL7/S_L7C1.PSX"},		/* N l7c1 */
	{6,0,221,"../LEVEL7/S_L7C2.PSX"},		/* N l7c2 */
	{6,0,222,"../LEVEL7/S_L7C3.PSX"},		/* N l7c3 */
	{6,0,223,"../LEVEL7/S_L7C4.PSX"},		/* N l7c4 */
	{6,0,224,"../LEVEL7/S_L7C5.PSX"},		/* N l7c5 */

	/* MISSION 8 */
	{7,0, 83,"../LEVEL1/S_TAD.PSX"},		/* N tad */
	{7,0,111,"../LEVEL1/S_LRTAD.PSX"},	/* 1 lrtad */
	{7,0,229,"../LEVEL8/S_L8C1.PSX"},		/* N l8c1 */

	/* MISSION 9 */
	{8,0, 83,"../LEVEL1/S_TAD.PSX"},		/* N tad */
	{8,0,111,"../LEVEL1/S_LRTAD.PSX"},	/* 1 lrtad */
	{8,0,234,"../LEVEL9/S_L9C1.PSX"},		/* N l9c1 */

	{255,0,0,0}
};

SPRITE*	levelspr[256];			/* RESTORE: BMF NUMBER -> SPRITES, ALL MISSIONS */
UBYTE	levelcount[256];			/* RESTORE: FRAMES IN EACH levelspr TABLE */

SPRITE*	spr_S_EXP1;				/* RESTORE: THE SATURN PORT'S EXPLOSIONS AND LASER SHOT */
SPRITE*	spr_S_EXP4;
SPRITE*	spr_CHLINK;				/* RESTORE: THE CHAIN WEAPON'S SHOT, MISC/CHLINK (docs/DECISIONS.md) */
SPRITE*	spr_S_EXP6;
SPRITE*	spr_S_EXP7;
SPRITE*	spr_S_SPFIRE;

/* RESTORE: A SPRITE TABLE'S FRAME COUNT (ENTRIES TO THE ONE FLAGGED LASTSPR) */

static USHORT
Spr_Count(SPRITE *spr)
{

USHORT	n;

	n=1;
	while ((spr->flags & LASTSPR)==0)
	{
		spr++;
		n++;
	}
	return n;
}

/* RESTORE: LOAD ONE CONVERTED SATURN SPRITE (tools/x360_scs2spr.py) */

static SPRITE*
Load_Saturn(BYTE *psx,BYTE *spr)
{

UBYTE	*image;
SPRITE	*table;

	image=LoadFile(psx);
	table=(SPRITE*)LoadFile(spr);
	Init_Spr_List(table,(ULONG)image);
	MyFree((UBYTE*)image);
	return table;
}

void
Load_Mission_Sprites(void)
{

USHORT	i;
USHORT	j;
UBYTE	*psx;
SPRITE	*spr;
BYTE	sprname[40];

	for (i=0;i<256;i++) { levelspr[i]=NULL; levelcount[i]=0; }

	for (i=0;levelsprites[i].level!=255;i++)
	{
		if (levelsprites[i].level!=level) continue;

		spr=NULL;					/* SAME FILE AS AN EARLIER NUMBER? */
		for (j=0;j<i;j++)
		{
			if ((levelsprites[j].level==level) &&
				(strcmp(levelsprites[j].psx,levelsprites[i].psx)==0))
					spr=levelspr[levelsprites[j].name];
		}

		if (spr==NULL)
		{
			psx=LoadFile(levelsprites[i].psx);
			strcpy(sprname,levelsprites[i].psx);	/* X.PSX -> X.SPR */
			j=strlen(sprname);
			sprname[j-3]='S';
			sprname[j-2]='P';
			sprname[j-1]='R';
			spr=(SPRITE*)LoadFile(sprname);
			if (levelsprites[i].live!=0)
			{
				Add_Spr_Loadoffset(spr,(ULONG)psx);	/* AS S_CHAPIE AND S_MUSL1 */
			}
			else
			{
				Init_Spr_List(spr,(ULONG)psx);
				MyFree((UBYTE*)psx);
			}
		}
		levelspr[levelsprites[i].name]=spr;
		levelcount[levelsprites[i].name]=Spr_Count(spr);
	}
}

/**************************************
*       LOAD IN GAME GRAPHICS         *
**************************************/

void
Load_Data(void)
{

	particle=LoadFile("../MISC/PARTICLE.PSX");
	spr_PARTICLE = (SPRITE*)LoadFile("../MISC/PARTICLE.SPR");
	Init_Spr_List(spr_PARTICLE,(ULONG)particle);
	MyFree((UBYTE*)particle);

	guidwire=LoadFile("../MISC/GUIDWIR.PSX");
	spr_GUIDWIRE = (SPRITE*)LoadFile("../MISC/GUIDWIR.SPR");
	Add_Spr_Loadoffset(spr_GUIDWIRE,(ULONG)guidwire);
//	MyFree((UBYTE*)guidwire);

	laswire=LoadFile("../MISC/LASWIRE.PSX");
	spr_LASWIRE = (SPRITE*)LoadFile("../MISC/LASWIRE.SPR");
	Add_Spr_Loadoffset(spr_LASWIRE,(ULONG)laswire);
//	MyFree((UBYTE*)laswire);

	bckwire=LoadFile("../MISC/BCKWIRE.PSX");
	spr_BCKWIRE = (SPRITE*)LoadFile("../MISC/BCKWIRE.SPR");
	Add_Spr_Loadoffset(spr_BCKWIRE,(ULONG)bckwire);
//	MyFree((UBYTE*)bckwire);

	ebwire=LoadFile("../MISC/EBWIRE.PSX");
	spr_EBWIRE = (SPRITE*)LoadFile("../MISC/EBWIRE.SPR");
	Add_Spr_Loadoffset(spr_EBWIRE,(ULONG)ebwire);
//	MyFree((UBYTE*)ebwire);

	agmwire=LoadFile("../MISC/AGMWIRE.PSX");
	spr_AGMWIRE = (SPRITE*)LoadFile("../MISC/AGMWIRE.SPR");
	Add_Spr_Loadoffset(spr_AGMWIRE,(ULONG)agmwire);
//	MyFree((UBYTE*)agmwire);

	min1wire=LoadFile("../MISC/MIN1WIRE.PSX");
	spr_MIN1WIRE = (SPRITE*)LoadFile("../MISC/MIN1WIRE.SPR");
	Add_Spr_Loadoffset(spr_MIN1WIRE,(ULONG)min1wire);
//	MyFree((UBYTE*)min1wire);


	s_ship=LoadFile("../MISC/S_SHIP.PSX");
	spr_S_SHIP = (SPRITE*)LoadFile("../MISC/S_SHIP.SPR");
	Init_Spr_List(spr_S_SHIP,(ULONG)s_ship);
	Build_Craft_Mask(spr_S_SHIP);		/* RESTORE: THE CRAFT'S SHAPE FOR Move_Craft, MADE FROM ITS IMAGES BEFORE THEY ARE FREED (SPRITE2.C) */
	MyFree((UBYTE*)s_ship);

	guided=LoadFile("../MISC/GUIDED.PSX");
	spr_GUIDED = (SPRITE*)LoadFile("../MISC/GUIDED.SPR");
	Init_Spr_List(spr_GUIDED,(ULONG)guided);
	Build_Shot_Mask(&shotguided_mask,spr_GUIDED);	/* RESTORE: THE SHOT'S SHAPE, BEFORE ITS IMAGE IS FREED (SPRITE2.C) */
	MyFree((UBYTE*)guided);

	buck01=LoadFile("../MISC/BUCK01.PSX");
	spr_BUCK01 = (SPRITE*)LoadFile("../MISC/BUCK01.SPR");
	Init_Spr_List(spr_BUCK01,(ULONG)buck01);
	Build_Shot_Mask(&shotbuck01_mask,spr_BUCK01);	/* RESTORE: THE SHOT'S SHAPE, BEFORE ITS IMAGE IS FREED (SPRITE2.C) */
	MyFree((UBYTE*)buck01);

	buck02=LoadFile("../MISC/BUCK02.PSX");
	spr_BUCK02 = (SPRITE*)LoadFile("../MISC/BUCK02.SPR");
	Init_Spr_List(spr_BUCK02,(ULONG)buck02);
	Build_Shot_Mask(&shotbuck02_mask,spr_BUCK02);	/* RESTORE: THE SHOT'S SHAPE, BEFORE ITS IMAGE IS FREED (SPRITE2.C) */
	MyFree((UBYTE*)buck02);

	eball=LoadFile("../MISC/EBALL.PSX");
	spr_EBALL = (SPRITE*)LoadFile("../MISC/EBALL.SPR");
	Init_Spr_List(spr_EBALL,(ULONG)eball);
	Build_Shot_Mask(&shoteball_mask,spr_EBALL);	/* RESTORE: THE SHOT'S SHAPE, BEFORE ITS IMAGE IS FREED (SPRITE2.C) */
	MyFree((UBYTE*)eball);

	mine=LoadFile("../MISC/MINE.PSX");
	spr_MINE = (SPRITE*)LoadFile("../MISC/MINE.SPR");
	Init_Spr_List(spr_MINE,(ULONG)mine);
	Build_Shot_Mask(&shotmine_mask,spr_MINE);	/* RESTORE: THE SHOT'S SHAPE, BEFORE ITS IMAGE IS FREED (SPRITE2.C) */
	MyFree((UBYTE*)mine);


	border1=LoadFile("../MISC/BORDER1.PSX");
	spr_BORDER1 = (SPRITE*)LoadFile("../MISC/BORDER1.SPR");
	Init_Spr_List(spr_BORDER1,(ULONG)border1);
	MyFree((UBYTE*)border1);

	barsl=LoadFile("../MISC/BARSL.PSX");
	spr_BARSL = (SPRITE*)LoadFile("../MISC/BARSL.SPR");
	Init_Spr_List(spr_BARSL,(ULONG)barsl);
	MyFree((UBYTE*)barsl);

	barsr=LoadFile("../MISC/BARSR.PSX");
	spr_BARSR = (SPRITE*)LoadFile("../MISC/BARSR.SPR");
	Init_Spr_List(spr_BARSR,(ULONG)barsr);
	MyFree((UBYTE*)barsr);

	shipwire=LoadFile("../MISC/SHIPWIRE.PSX");
	spr_SHIPWIRE = (SPRITE*)LoadFile("../MISC/SHIPWIRE.SPR");
	Init_Spr_List(spr_SHIPWIRE,(ULONG)shipwire);
	MyFree((UBYTE*)shipwire);

	/* RESTORE: THE SATURN PORT'S EXPLOSIONS (NAME_EXPLOSION IS ITS S_EXP1,
	   GAME.C:421; LOGIC/FIRECODE.C:387 STARTS exp4, exp7 AND exp6 WHERE A
	   MISSILE HITS A WALL) AND THE LASER SHOT (PS1 FIRE.C's NAME_SPFIRE)
	   FOR EVERY MISSION. add_sprite PLAYS FRAMES 0 TO seqdata[].frames-2. */
	spr_S_EXP1=Load_Saturn("../SATURN/S_EXP1.PSX","../SATURN/S_EXP1.SPR");
	spr_S_EXP4=Load_Saturn("../SATURN/S_EXP4.PSX","../SATURN/S_EXP4.SPR");
	spr_S_EXP6=Load_Saturn("../SATURN/S_EXP6.PSX","../SATURN/S_EXP6.SPR");
	spr_S_EXP7=Load_Saturn("../SATURN/S_EXP7.PSX","../SATURN/S_EXP7.SPR");
	{	/* RESTORE: AS Load_Saturn, WITH THE LASER SHOT'S SHAPE MADE BEFORE ITS IMAGE IS FREED (SPRITE2.C) */
		UBYTE	*spfimage;
		spfimage=LoadFile("../SATURN/S_SPFIRE.PSX");
		spr_S_SPFIRE=(SPRITE*)LoadFile("../SATURN/S_SPFIRE.SPR");
		Init_Spr_List(spr_S_SPFIRE,(ULONG)spfimage);
		Build_Shot_Mask(&shotspfire_mask,spr_S_SPFIRE);
		MyFree((UBYTE*)spfimage);
	}
	seqdata[NAME_EXPLOSION].frames=Spr_Count(spr_S_EXP1)+2;
	seqdata[NAME_exp4].frames=Spr_Count(spr_S_EXP4)+2;
	seqdata[NAME_exp6].frames=Spr_Count(spr_S_EXP6)+2;
	seqdata[NAME_exp7].frames=Spr_Count(spr_S_EXP7)+2;
	seqdata[NAME_laserfire].frames=Spr_Count(spr_S_SPFIRE)+2;
	/* RESTORE: THE CHAIN WEAPON'S SHOT WAS DRAWN AS THE CRAFT (FIRE.C: "REPLACE WITH PROPER SPRITES"; THE PC NAMES IT NAME_CHAINB, COMMENTED OUT HERE). THE DISC TREE HAS
	   ITS ART, MISC/CHLINK (THE SATURN'S NAME_CHAIN3, GAME.C:400): SIXTEEN CHAIN-LINK FRAMES, 8 PIXELS WIDE AND 1 OR 3 HIGH, WHICH NOTHING LOADED. ITS SHAPE IS MADE
	   BEFORE THE IMAGE IS FREED, AS THE PC TESTS THE SHOT'S SHAPE AGAINST THE MAP (SPRITE2.C) */
	{
		UBYTE	*chimage;
		chimage=LoadFile("../MISC/CHLINK.PSX");
		spr_CHLINK=(SPRITE*)LoadFile("../MISC/CHLINK.SPR");
		Init_Spr_List(spr_CHLINK,(ULONG)chimage);
		Build_Shot_Mask(&shotchlink_mask,spr_CHLINK);
		MyFree((UBYTE*)chimage);
	}
	seqdata[NAME_CHAINB].frames=Spr_Count(spr_CHLINK)+2;

	if (level!=0)					/* RESTORE: MISSIONS 2..9 LOAD THEIR OWN SET */
	{
		Load_Mission_Sprites();
		return;
	}

//#if 0

	flopl1=LoadFile("../LEVEL1/FLOPL1.PSX");
	spr_FLOPL1 = (SPRITE*)LoadFile("../LEVEL1/FLOPL1.SPR");
	Init_Spr_List(spr_FLOPL1,(ULONG)flopl1);
	MyFree((UBYTE*)flopl1);

	l1c1=LoadFile("../LEVEL1/L1C1.PSX");
	spr_L1C1 = (SPRITE*)LoadFile("../LEVEL1/L1C1.SPR");
	Init_Spr_List(spr_L1C1,(ULONG)l1c1);
	MyFree((UBYTE*)l1c1);

	l1c2=LoadFile("../LEVEL1/L1C2.PSX");
	spr_L1C2 = (SPRITE*)LoadFile("../LEVEL1/L1C2.SPR");
	Init_Spr_List(spr_L1C2,(ULONG)l1c2);
	MyFree((UBYTE*)l1c2);

	l1c3=LoadFile("../LEVEL1/L1C3.PSX");
	spr_L1C3 = (SPRITE*)LoadFile("../LEVEL1/L1C3.SPR");
	Init_Spr_List(spr_L1C3,(ULONG)l1c3);
	MyFree((UBYTE*)l1c3);

	l1c4=LoadFile("../LEVEL1/L1C4.PSX");
	spr_L1C4 = (SPRITE*)LoadFile("../LEVEL1/L1C4.SPR");
	Init_Spr_List(spr_L1C4,(ULONG)l1c4);
	MyFree((UBYTE*)l1c4);

	l1c5=LoadFile("../LEVEL1/L1C5.PSX");
	spr_L1C5 = (SPRITE*)LoadFile("../LEVEL1/L1C5.SPR");
	Init_Spr_List(spr_L1C5,(ULONG)l1c5);
	MyFree((UBYTE*)l1c5);

	plungl1=LoadFile("../LEVEL1/PLUNGL1.PSX");
	spr_PLUNGL1 = (SPRITE*)LoadFile("../LEVEL1/PLUNGL1.SPR");
	Init_Spr_List(spr_PLUNGL1,(ULONG)plungl1);
	MyFree((UBYTE*)plungl1);

	s_acidsp=LoadFile("../LEVEL1/S_ACIDSP.PSX");
	spr_S_ACIDSP = (SPRITE*)LoadFile("../LEVEL1/S_ACIDSP.SPR");
	Init_Spr_List(spr_S_ACIDSP,(ULONG)s_acidsp);
	MyFree((UBYTE*)s_acidsp);

	s_rck01=LoadFile("../LEVEL1/S_RCK01.PSX");
	spr_S_RCK01 = (SPRITE*)LoadFile("../LEVEL1/S_RCK01.SPR");
	Init_Spr_List(spr_S_RCK01,(ULONG)s_rck01);
	MyFree((UBYTE*)s_rck01);

	s_rck02=LoadFile("../LEVEL1/S_RCK02.PSX");
	spr_S_RCK02 = (SPRITE*)LoadFile("../LEVEL1/S_RCK02.SPR");
	Init_Spr_List(spr_S_RCK02,(ULONG)s_rck02);
	MyFree((UBYTE*)s_rck02);

	s_rck03=LoadFile("../LEVEL1/S_RCK03.PSX");
	spr_S_RCK03 = (SPRITE*)LoadFile("../LEVEL1/S_RCK03.SPR");
	Init_Spr_List(spr_S_RCK03,(ULONG)s_rck03);
	MyFree((UBYTE*)s_rck03);

	s_float=LoadFile("../LEVEL1/S_FLOAT.PSX");
	spr_S_FLOAT = (SPRITE*)LoadFile("../LEVEL1/S_FLOAT.SPR");
	Init_Spr_List(spr_S_FLOAT,(ULONG)s_float);
	MyFree((UBYTE*)s_float);

	s_lrtad=LoadFile("../LEVEL1/S_LRTAD.PSX");
	spr_S_LRTAD = (SPRITE*)LoadFile("../LEVEL1/S_LRTAD.SPR");
	Init_Spr_List(spr_S_LRTAD,(ULONG)s_lrtad);
	MyFree((UBYTE*)s_lrtad);

	s_mrspit=LoadFile("../LEVEL1/S_MRSPIT.PSX");
	spr_S_MRSPIT = (SPRITE*)LoadFile("../LEVEL1/S_MRSPIT.SPR");
	Init_Spr_List(spr_S_MRSPIT,(ULONG)s_mrspit);
	MyFree((UBYTE*)s_mrspit);

	s_msspit=LoadFile("../LEVEL1/S_MSSPIT.PSX");
	spr_S_MSSPIT = (SPRITE*)LoadFile("../LEVEL1/S_MSSPIT.SPR");
	Init_Spr_List(spr_S_MSSPIT,(ULONG)s_msspit);
	MyFree((UBYTE*)s_msspit);

	s_tubel1=LoadFile("../LEVEL1/S_TUBEL1.PSX");
	spr_S_TUBEL1 = (SPRITE*)LoadFile("../LEVEL1/S_TUBEL1.SPR");
	Init_Spr_List(spr_S_TUBEL1,(ULONG)s_tubel1);
	MyFree((UBYTE*)s_tubel1);

	s_rckmph=LoadFile("../LEVEL1/S_RCKMPH.PSX");
	spr_S_RCKMPH = (SPRITE*)LoadFile("../LEVEL1/S_RCKMPH.SPR");
	Init_Spr_List(spr_S_RCKMPH,(ULONG)s_rckmph);
	MyFree((UBYTE*)s_rckmph);

	s_star=LoadFile("../LEVEL1/S_STAR.PSX");
	spr_S_STAR = (SPRITE*)LoadFile("../LEVEL1/S_STAR.SPR");
	Init_Spr_List(spr_S_STAR,(ULONG)s_star);
	MyFree((UBYTE*)s_star);

	s_starfs=LoadFile("../LEVEL1/S_STARFS.PSX");
	spr_S_STARFS = (SPRITE*)LoadFile("../LEVEL1/S_STARFS.SPR");
	Init_Spr_List(spr_S_STARFS,(ULONG)s_starfs);
	MyFree((UBYTE*)s_starfs);

	s_survs=LoadFile("../LEVEL1/S_SURVS.PSX");
	spr_S_SURVS = (SPRITE*)LoadFile("../LEVEL1/S_SURVS.SPR");
	Init_Spr_List(spr_S_SURVS,(ULONG)s_survs);
	MyFree((UBYTE*)s_survs);

	s_turret=LoadFile("../LEVEL1/S_TURRET.PSX");
	spr_S_TURRET = (SPRITE*)LoadFile("../LEVEL1/S_TURRET.SPR");
	Init_Spr_List(spr_S_TURRET,(ULONG)s_turret);
	MyFree((UBYTE*)s_turret);

	s_tenta1=LoadFile("../LEVEL1/S_TENTA1.PSX");
	spr_S_TENTA1 = (SPRITE*)LoadFile("../LEVEL1/S_TENTA1.SPR");
	Init_Spr_List(spr_S_TENTA1,(ULONG)s_tenta1);
	MyFree((UBYTE*)s_tenta1);

	s_tenta2=LoadFile("../LEVEL1/S_TENTA2.PSX");
	spr_S_TENTA2 = (SPRITE*)LoadFile("../LEVEL1/S_TENTA2.SPR");
	Init_Spr_List(spr_S_TENTA2,(ULONG)s_tenta2);
	MyFree((UBYTE*)s_tenta2);

	s_tnsl1=LoadFile("../LEVEL1/S_TNSL1.PSX");
	spr_S_TNSL1 = (SPRITE*)LoadFile("../LEVEL1/S_TNSL1.SPR");
	Init_Spr_List(spr_S_TNSL1,(ULONG)s_tnsl1);
	MyFree((UBYTE*)s_tnsl1);

	s_vil1=LoadFile("../LEVEL1/S_VIL1.PSX");
	spr_S_VIL1 = (SPRITE*)LoadFile("../LEVEL1/S_VIL1.SPR");
	Init_Spr_List(spr_S_VIL1,(ULONG)s_vil1);
	MyFree((UBYTE*)s_vil1);

	s_bounc1=LoadFile("../LEVEL1/S_BOUNC1.PSX");
	spr_S_BOUNC1 = (SPRITE*)LoadFile("../LEVEL1/S_BOUNC1.SPR");
	Init_Spr_List(spr_S_BOUNC1,(ULONG)s_bounc1);
	MyFree((UBYTE*)s_bounc1);

	s_bounc2=LoadFile("../LEVEL1/S_BOUNC2.PSX");
	spr_S_BOUNC2 = (SPRITE*)LoadFile("../LEVEL1/S_BOUNC2.SPR");
	Init_Spr_List(spr_S_BOUNC2,(ULONG)s_bounc2);
	MyFree((UBYTE*)s_bounc2);

	s_bounc3=LoadFile("../LEVEL1/S_BOUNC3.PSX");
	spr_S_BOUNC3 = (SPRITE*)LoadFile("../LEVEL1/S_BOUNC3.SPR");
	Init_Spr_List(spr_S_BOUNC3,(ULONG)s_bounc3);
	MyFree((UBYTE*)s_bounc3);

	s_chapie=LoadFile("../LEVEL1/S_CHAPIE.PSX");
	spr_S_CHAPIE = (SPRITE*)LoadFile("../LEVEL1/S_CHAPIE.SPR");
	Add_Spr_Loadoffset(spr_S_CHAPIE,(ULONG)s_chapie);
//	MyFree((UBYTE*)s_chapie);

	s_musl1=LoadFile("../LEVEL1/S_MUSL1.PSX");
	spr_S_MUSL1 = (SPRITE*)LoadFile("../LEVEL1/S_MUSL1.SPR");
	Add_Spr_Loadoffset(spr_S_MUSL1,(ULONG)s_musl1);
//	MyFree((UBYTE*)s_musl1);

//#endif

	Load_Mission_Sprites();			/* RESTORE: MISSION 1'S SATURN ROWS */

}

