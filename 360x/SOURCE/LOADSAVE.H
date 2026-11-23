
#ifndef	__H_LOADSAVE
#define	__H_LOADSAVE

/**************************************
*          DECLARE DEFINES            *
**************************************/

#define	CARD1	0x00				/* CARD SLOT 1 */
#define	CARD2	0x10				/* CARD SLOT 2 */

#define	game_save_name1 "bu00:360X*SAVE"	/* SAVE GAME SLOT 1 */
#define	game_save_name2 "bu10:360X*SAVE"	/* SAVE GAME SLOT 2 */

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Load_High_Scores(void);
void	Save_High_Scores(void);





void	Update_Screen(void);










extern	SHORT	Test_For_Format(void);
extern	SHORT	et_format(LONG chan);
extern	SHORT	Save_Game(void);		/* SAVE A GAME TO MEMCARD */
extern	SHORT	Load_Game(void);		/* LOAD A GAME FROM MEMCARD */
extern	SHORT	Delete_Game(void);		/* DELETE A GAME FROM MEMCARD */

/**************************************
*          DECLARE VARIABLES          *
**************************************/

extern	ULONG	ev0,ev1,ev2,ev3;		/* EVENTS FOR MEMCARD */
extern	ULONG	ev10,ev11,ev12,ev13;	/* EVENTS FOR MEMCARD */

extern	volatile	LONG		ret,ret1,ret2;	/* RETURN DATA FROM CARD SLOT */

extern	BYTE		SAVE_BUF[1*512];		/* DATA IS PUT IN HERE TO SAVE */
extern	BYTE		LOAD_BUF[1*512];		/* DATA IS READ INTO HERE */

extern	LONG		currentcard;		/* CURRENT CARD SLOT USING */

extern	USHORT	stardelay;

#endif

