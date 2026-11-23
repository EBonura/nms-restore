
#ifndef	__H_LEVEL
#define	__H_LEVEL

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define	TELETYPELEVEL1	(10)
#define	TELETYPELEVEL2	(10)
#define	TELETYPELEVEL3	(10)
#define	TELETYPELEVEL4	(10)
#define	TELETYPELEVEL5	(10)
#define	TELETYPELEVEL6	(10)
#define	TELETYPELEVEL7	(10)
#define	TELETYPELEVEL8	(10)
#define	TELETYPELEVEL9	(10)

#define	TELETYPESTATUS	(6)

/**************************************
*          DECLARE STRUCTURES         *
**************************************/

/**************************************
*          DECLARE VARIABLES          *
**************************************/

extern	USHORT	craftremaining;
extern	FLAG		levelcompleted;
extern	FLAG		changelevel;
extern	LONG		newlevel;

extern	USHORT	totalsurvivors;
extern	USHORT	rescuedsurvivors;
extern	USHORT	killedsurvivors;

extern	USHORT	shotsfired;
extern	USHORT	shotsontarget;
extern	USHORT	shotsaccuracy;

extern	USHORT	alienskilled;

extern	USHORT	weaponbonus;

extern	USHORT	timebonus;

/**************************************
*          DECLARE ROUTINES           *
**************************************/

FLAG	Process_Script(BYTE *fname);
FLAG	Process_Linedata(ULONG *data);
void	Level_Description(BYTE *mappsx,BYTE *mapspr,TEXT *levelinfo,SHORT nolines);
void	Load_Level(void);
void	Exit_Level(void);
void	Init_Newgame(void);
void	Init_Level(void);
void	Check_Level_Completed(void);		/* RESTORE: NEW WORK */

#endif

