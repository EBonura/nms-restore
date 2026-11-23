
/**************************************
*           INCBIN HEADERS            *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE VARIABLES          *
**************************************/

UBYTE	soundfx;					/* YES=THEN SOUNDFX ARE IN */
UBYTE	music;					/* YES=THEN MUSIC IS IN */

UBYTE	*VAB_HADDR;				/* VAB HEADER POINTER */
UBYTE	*VAB_BADDR;				/* VAB BINARY POINTER */
UBYTE	*SEQ_ADDR;				/* SEQUENCE 1 POINTER */
BYTE	*VAB_BNAME;				/* RESTORE: platform: VAB BINARY FILE, SENT IN PIECES WHEN VAB_BADDR IS 0 */
UBYTE	menumusic;				/* RESTORE: owner's decision: YES WHILE THE MENU MUSIC (OPTPA) IS THE SOUND STARTED LAST */

SHORT	vab;						/* VAB DATA ID */
SHORT	sequence;					/* SEQ DATA ID */

BYTE	seq_table [SS_SEQ_TABSIZ*2*2];	/* SEQ TABLE SIZE */

/**************************************
*        INITIALISE THE SOUND         *
**************************************/

void
InitialiseSound(void)
{

	menumusic=NO;					/* RESTORE: owner's decision, see Start_Menu_Sound */
	SsInit();						/* RESET SOUND SYSTEM AND CLEAR MEMORY */
	SsSetTableSize(seq_table,2,2);		/* SET TABLE SIZE */
	SsSetTickMode(SS_NOTICK);		/* SET TICK MODE = NOTICK, USER VBL UPDATE */

	if (music==YES)				/* IS SEQUENCE BUFFER BEING USED */
	{
		sequence=SsSeqOpen((ULONG *)SEQ_ADDR,vab);
	}

	if (soundfx==YES)				/* IS VAB BUFFER BEING USED */
	{
		vab =SsVabOpenHead((UBYTE *)VAB_HADDR,-1);	/* SEARCH FOR EMPTY VAB */
		if (vab<0)
		{
			printf ("SsVabOpenHead : failed!\n");
			CRASH;
		}

		/* RESTORE: platform (2 MB): a bank whose body was not loaded
		   (VAB_BADDR 0, MAIN.C's mission bank) is read from its file and
		   sent in pieces; a loaded one goes as NMS sent it. */
		if (((VAB_BADDR!=0) ? SsVabTransBody((UBYTE *)VAB_BADDR,vab) : Stream_Vab_Body(VAB_BNAME,vab))!=vab)
		{
			printf ("SsVabTransBody : failed!\n");
			CRASH;
		}

	}

	SsStart();					/* START SOUND */
	SsSetMVol(MVOL,MVOL);			/* SET MAIN VOLUME */

	if (music==YES)
	{
		SsSeqSetVol(sequence, musvolume, musvolume);	/* SET SEQ DATA VOLUME */
		SsSeqPlay(sequence,SSPLAY_PLAY,SSPLAY_INFINITY); /* START THE MUSIC */
	}

}

/**************************************
*  RESTORE: START THE MENU MUSIC      *
**************************************/

/* RESTORE: owner's decision. The menus play OPTPA.SEQ on the OPTIONS bank,
   as NMS's March 1996 build did from its start (NMS.zip NMS/360WORK MAIN.C:92-98:
   OPTIONS.VH, OPTIONS.VB, OPTPA.SEQ and OPTPB.SEQ loaded, soundfx and music
   YES, InitialiseSound, OPTPA played for ever). Adapted to July's code:
   InitialiseSound opens one song (SEQ_ADDR), so OPTPB, which March opened
   as a second song and never played, is not loaded; the body is sent in
   pieces as MAIN.C's mission bank is (platform, 2 MB). It does nothing while
   the menu music is still the sound started last, so a caller can ask for
   it on every way into the menus. */

void
Start_Menu_Sound(void)
{

	if (menumusic==YES) return;		/* ALREADY PLAYING */

	VAB_HADDR=LoadFile("../SOUND/OPTIONS.VH");
	VAB_BADDR=0;					/* PLATFORM: SENT IN PIECES (FILES.C) */
	VAB_BNAME="../SOUND/OPTIONS.VB";
	SEQ_ADDR=LoadFile("../SOUND/OPTPA.SEQ");
	soundfx=YES;					/* DO YOU NEED VAB BUFFER? */
	music=YES;					/* DO YOU NEED SEQ BUFFER? */
	InitialiseSound();				/* START THE MUSIC */
	menumusic=YES;

}

/**************************************
*          UPDATE THE SOUND           *
**************************************/

void	
UpdateTheSound(void)
{

	SsSeqCalledTbyT();				/* UPDATE FROM VBL (NOTICK) */

}

