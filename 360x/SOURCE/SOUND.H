
#ifndef	__H_SOUND
#define	__H_SOUND

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	InitialiseSound(void);		/* INITIALISE SOUND SUBSYSTEM */
void	UpdateTheSound(void);		/* UPDATE THE SOUND ON THE VBL */
void	Start_Menu_Sound(void);		/* RESTORE: owner's decision, THE MENU MUSIC */

/**************************************
*           DECLARE DEFINES           *
**************************************/

#define	SFX_CLICK		(102)
#define	SFX_EXPLOSION1	(103)
#define	SFX_GOTHOT	(104)		/* SOUND FOR HOTSPOT HIT */
#define	SFX_CANNON	(105)
#define	SFX_FIRENOISE	(106)
#define	SFX_EXPLOSION2	(107)
#define	SFX_FIREGUN	(108)		/* SOUND FOR FIRING GUN */
#define	SFX_FIRELASER	(109)
#define	SFX_OUCH1		(110)
#define	SFX_OUCH2		(111)

#define	MVOL		127				/* MASTER VOLUME */
#define	FXPITCH	60				/* SOUNDFX PITCH */

/**************************************
*          DECLARE VARIABLES          *
**************************************/

extern	UBYTE	soundfx;			/* YES=THEN SOUNDFX ARE IN */
extern	UBYTE	music;			/* YES=THEN MUSIC IS IN */

extern	UBYTE	*VAB_HADDR;		/* VAB HEADER POINTER */
extern	UBYTE	*VAB_BADDR;		/* VAB BINARY POINTER */
extern	UBYTE	*SEQ_ADDR;		/* SEQUENCE 1 POINTER */
extern	BYTE	*VAB_BNAME;		/* RESTORE: platform, SOUND.C */
extern	UBYTE	menumusic;		/* RESTORE: owner's decision, SOUND.C */

extern	SHORT	vab;				/* VAB DATA ID */
extern	SHORT	sequence;			/* SEQ DATA ID */

#endif

