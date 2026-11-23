
/**************************************
*     CARD LOAD/SAVE FOR SONY PSX     *
**************************************/

/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE ROUTINES           *
**************************************/

LONG	_clear_event(void);
LONG	_clear_event_x(void);
LONG	_card_event(void);
LONG	_card_event_x(void);			/* RESTORE: was _card_eventx, a name nothing defines */
LONG	card_status(void);
LONG	scan_cards(void);
void	Try_LoadGame(void);			/* RESTORE: NMS's load and save, under #if 0 below, are built now */
void	Try_SaveGame(void);
extern	UBYTE	inmenu;				/* RESTORE: OPTIONS.C's menu state, for the load and save */
extern	UBYTE	*loading1[3],*loading2[3],*loading3[3],*saver1[3],*saver2[3],*saver3[3];
extern	UBYTE	*unform_1[3],*unform_2[3],*unform_3[3];
extern	UBYTE	*formating1[3],*formating2[3],*formating3[3],*formating4[3];

/**************************************
*          DECLARE DEFINES            *
**************************************/

/**************************************
*          DECLARE VARIABLES          *
**************************************/

_CARD	hed;						/* HEADER STRUCTURE FOR CARD */

ULONG	ev0,ev1,ev2,ev3;			/* EVENTS FOR MEMCARD */
ULONG	ev10,ev11,ev12,ev13;		/* EVENTS FOR MEMCARD */

volatile	LONG	ret,ret1,ret2;			/* RETURN DATA FROM CARD SLOT */

BYTE	SAVE_BUF[1*512];			/* DATA IS PUT IN HERE TO SAVE */
BYTE	LOAD_BUF[1*512];			/* DATA IS READ INTO HERE */

LONG	currentcard;				/* CURRENT CARD SLOT USING */

USHORT	stardelay;

/**************************************
*  MESSAGES FOR LOADING AND SAVING    *
**************************************/

UBYTE	*chek_mem1[3]={"CHECKING FOR MEMORY CARDS",
					"VERIFICATION DES CARTES MEMOIRE",
					"SUCHE NACH MEMORY CARDS"};

UBYTE	*please_w[3]={	"PLEASE WAIT",
					"MERCI DE PATIENTER",
					"BITTE WARTEN"};

UBYTE	*no_mem1[3]={	"NO MEMORY CARDS FOUND",
					"CARTE M‚MOIRE NON D‚TECT‚E",
					"KEINE MEMORY CARD GEFUNDEN"};

UBYTE	*no_mem2[3]={	"PLEASE INSERT MEMORY CARD",
					"MERCI D'INS‚RER UNE CARTE M‚MOIRE",
					"MEMORY CARD EINSETZEN"};

UBYTE	*no_mem3[3]={	"OR PRESS ANY BUTTON",
					"OU D'APPUYER SUR N'IMPORTE QUEL BOUTON",
					"ODER EINE TASTE DRUCKEN"};

UBYTE	*cards2_1[3]={	"2 OR MORE MEMORY CARDS FOUND",
					"2 CARTES M‚MOIRE OU PLUS D‚TECT‚ES",
					"2 ODER MEHR MEMORY CARDS GEFUNDEN"};

UBYTE	*cards2_2[3]={	"PRESS X FOR MEMORY CARD 1",
					"APPUYEZ SUR X POUR LA CARTE M‚MOIRE 1",
					"X DRUCKEN UM MEMORY CARD 1 ZU BENUTZEN"};

UBYTE	*cards2_3[3]={	"OR PRESS O FOR MEMORY CARD 2",
					"OU SUR O POUR LA CARTE M‚MOIRE 2",
					"O DRUCKEN UM MEMORY CARD 2 ZU BENUTZEN"};

/**************************************
*         GRAPHICS FOR ICON           *
**************************************/

USHORT	saveclut[]={	0x0000,0x7C00,0x03E0,0x7FE0,
					0x0014,0x7C14,0x03F4,0x7FF4,
					0x001f,0x7C1F,0x03FF,0x7FFF,
					0x0000,0x0000,0x0000,0x0000};

ULONG	saveimage[]={	0x88888888,0x88888888,0xA8888888,0x8888888A,
					0xAA888888,0x888888AA,0xAA888888,0x888888AA,
					0xAA888888,0x888888AA,0xAA888888,0x888888AA,
					0xAA888888,0x888888AA,0xA8888888,0x8888888A,
					0xA8888888,0x8888888A,0x88888888,0x88888888,
					0xA8888888,0x8888888A,0xA8888888,0x8888888A,
					0x88888888,0x88888888,0x88888888,0x88888888,
					0x88888888,0x88888888,0x88888888,0x88888888};

/**************************************
*           LOAD HIGH SCORES          *
**************************************/

void
Load_High_Scores(void)
{

USHORT	delay;

	while (stardelay!=0)
	{
		Start_Loop();

		Print_Sprite(spr_BACKDROP,320,120,32);
		Plot_Stars(48);

		End_Loop();
		stardelay--;
	}

	Start_Loop();

	Print_Sprite(spr_BACKDROP,320,120,252);

	Print_At(192,80);				/* SAY CHECKING FOR CARDS */
	Print_Messl(chek_mem1[language]);

	Print_At(256,104);
	Print_Messl(please_w[language]);

	End_Loop();
	Update_Screen();

	scan_cards();					/* SCAN THE MEMORY CARDS */
	for (delay=0;delay!=120;delay++)
	{
		VSync(0);
	}

	while ((ret1!=0) && (ret1!=3) && (ret2!=0) && (ret2!=3))
	{

		Start_Loop();

		Print_Sprite(spr_BACKDROP,320,120,252);

		Print_At(128,80);			/* NO CARDS FOUND SO TELL THEM */
		Print_Messl(no_mem1[language]);
		Print_At(128,104);
		Print_Messl(no_mem2[language]);
		Print_At(128,128);
		Print_Messl(no_mem3[language]);

		End_Loop();
		Update_Screen();

		scan_cards();

		if ((Debounce1)!=0)			/* RESTORE: the screen says "OR PRESS ANY BUTTON"; as in Try_SaveGame */
		{
			Update_Screen();
			inmenu=0;
			selectno=0;
			return;
		}
	}

	if ((ret2!=0 & ret2!=3) && (ret1==0 | ret1==3) && (currentcard==CARD2))
	currentcard=CARD1;

	if ((ret1!=0 & ret1!=3) && (ret2==0|ret2==3) && (currentcard==CARD1))
	currentcard=CARD2;

	while (((ret1==0) || (ret1==3)) && ((ret2==0) || (ret2==3)))
	{
		Start_Loop();

		Print_Sprite(spr_BACKDROP,320,120,252);	/* RESTORE: was 240-8 */

		Print_At(128,80);			/* 2 CARDS FOUND, SELECT WHICH? */
		Print_Messl(cards2_1[language]);

		Print_At(128,104);
		Print_Messl(cards2_2[language]);

		Print_At(128,128);
		Print_Messl(cards2_3[language]);

		End_Loop();
		Update_Screen();

		if (((Pad1 & PaDRdown)!=0) && (ret1==0 || ret1==3))
		{
			currentcard=CARD1;
			break;
		}

		if (((Pad1 & PaDRright)!=0) && (ret2==0 || ret2==3))
		{
			currentcard=CARD2;
			break;
		}
	}

	Try_LoadGame();				/* RESTORE: was card_status(); CRASH; Try_LoadGame is the rest of the load */

}

























/* RESTORE: was #if 0; Load_High_Scores and Save_High_Scores call the load and save below */









/**************************************
*         TRY AND LOAD GAME           *
**************************************/

void
Try_LoadGame(void)
{

USHORT	delay;

	card_status();

	Start_Loop();

	Print_Sprite(spr_BACKDROP,320,120,252);	/* RESTORE: was 240-8 */

	Print_At(128,80);				/* PRINT LOADING FROM CARD */
	Print_Messl(loading1[language]);
	if (currentcard==CARD1) Print_Messl("1");
	else Print_Messl("2");

	End_Loop();
	Update_Screen();

	for (delay=0;delay!=120;delay++)
	{
		VSync(0);
	}

	if (Load_Game()!=FAIL)
	{
		Start_Loop();

		Print_Sprite(spr_BACKDROP,320,120,252);	/* RESTORE: was 240-8 */

		Print_At(128,80);				/* PRINT LOAD SUCCESSFUL */
		Print_Messl(loading2[language]);

		End_Loop();
		Update_Screen();
	}
	else
	{
		Start_Loop();

		Print_Sprite(spr_BACKDROP,320,120,252);	/* RESTORE: was 240-8 */

		Print_At(128,80);				/* PRINT LOAD FAILED */
		Print_Messl(loading3[language]);

		End_Loop();
		Update_Screen();
	}

	scan_cards();

	Update_Screen();
	inmenu=0;			/* RESTORE: was 1; 360X's option menu is 0 */
	selectno=0;

}















/**************************************
*         TRY AND SAVE GAME           *
**************************************/

void
Try_SaveGame(void)
{

USHORT	delay;

	Start_Loop();

	Print_Sprite(spr_BACKDROP,320,120,252);	/* RESTORE: was 240-8 */

	Print_At(192,80);				/* SAY CHECKING FOR CARDS */
	Print_Messl(chek_mem1[language]);

	Print_At(256,104);
	Print_Messl(please_w[language]);

	End_Loop();
	Update_Screen();

	scan_cards();

	for (delay=0;delay!=120;delay++)
	{
		VSync(0);
	}

	while ((ret1!=0) && (ret1!=3) && (ret2!=0) && (ret2!=3))
	{

		Start_Loop();

		Print_Sprite(spr_BACKDROP,320,120,252);	/* RESTORE: was 240-8 */

		Print_At(128,80);			/* NO CARDS FOUND SO TELL THEM */
		Print_Messl(no_mem1[language]);

		Print_At(128,104);
		Print_Messl(no_mem2[language]);

		Print_At(128,128);
		Print_Messl(no_mem3[language]);

		End_Loop();

		scan_cards();

		if ((Debounce1)!=0)
		{
			Update_Screen();
			inmenu=0;			/* RESTORE: was 1; 360X's option menu is 0 */
			selectno=0;
			return;
		}
	}

	if ((ret2!=0 & ret2!=3) && (ret1==0 | ret1==3) && (currentcard==CARD2))
	currentcard=CARD1;

	if ((ret1!=0 & ret1!=3) && (ret2==0|ret2==3) && (currentcard==CARD1))
	currentcard=CARD2;

	while (((ret1==0) || (ret1==3)) && ((ret2==0) || (ret2==3)))
	{
		Start_Loop();

		Print_Sprite(spr_BACKDROP,320,120,252);	/* RESTORE: was 240-8 */

		Print_At(128,80);			/* 2 CARDS FOUND, SELECT WHICH? */
		Print_Messl(cards2_1[language]);

		Print_At(128,104);
		Print_Messl(cards2_2[language]);

		Print_At(128,128);
		Print_Messl(cards2_3[language]);

		End_Loop();

		if (((Pad1 & PaDRdown)!=0) && (ret1==0 || ret1==3))
		{
			currentcard=CARD1;
			break;
		}

		if (((Pad1 & PaDRright)!=0) && (ret2==0 || ret2==3))
		{
			currentcard=CARD2;
			break;
		}
	}

	card_status();

	if (Test_For_Format()==FALSE)		/* SEE IF CARD IS FORMATTED */
	{

		for (;;)
		{
			Start_Loop();

			Print_Sprite(spr_BACKDROP,320,120,252);	/* RESTORE: was 240-8 */

			Print_At(128,80);			/* PRINT UNFORMATTED */
			Print_Messl(unform_1[language]);

			Print_At(128,104);
			Print_Messl(unform_2[language]);

			Print_At(128,128);
			Print_Messl(unform_3[language]);

			End_Loop();

			if ((Pad1 & PaDRdown)!=0)
			{
				Start_Loop();

				Print_Sprite(spr_BACKDROP,320,120,252);	/* RESTORE: was 240-8 */

				Print_At(128,80);			/* PRINT FORMATTING */
				Print_Messl(formating1[language]);

				if (currentcard==CARD1) Print_Messl("1");
				else Print_Messl("2");

				End_Loop();
				Update_Screen();

				_card_info(currentcard);
				ret = _card_event();
				if(!(ret==1 || ret==2))
				{
					if(ret==3)
					{
						_clear_event_x();
						_card_clear(currentcard);
						ret = _card_event_x();
					}

					_clear_event();
					_card_load(currentcard);
					ret = _card_event();

					if(ret==3)
					{
						if (currentcard==CARD1) ret = format("bu00:");
						if (currentcard==CARD2) ret = format("bu10:");

						if (ret==1)
						{
							Start_Loop();

							Print_Sprite(spr_BACKDROP,320,120,252);	/* RESTORE: was 240-8 */

							Print_At(128,80);			/* PRINT FORMAT COMPLETE */
							Print_Messl(formating2[language]);
							if (currentcard==CARD1) Print_Messl("1");
							else Print_Messl("2");

							End_Loop();
							Update_Screen();
							break;
						}
						else
						{
							Start_Loop();

							Print_Sprite(spr_BACKDROP,320,120,252);	/* RESTORE: was 240-8 */

							Print_At(128,80);			/* PRINT FORMAT ERROR */
							Print_Messl(formating3[language]);

							Print_At(128,104);
							Print_Messl(formating4[language]);

							End_Loop();

							Update_Screen();
							inmenu=0;			/* RESTORE: was 1; 360X's option menu is 0 */
							selectno=0;
							return;
						}
					}
				}
			}

			if ((Pad1 & PaDRright)!=0)
			{
				Update_Screen();
				inmenu=0;			/* RESTORE: was 1; 360X's option menu is 0 */
				selectno=0;
				return;
			}
		}
	}

	Start_Loop();

	Print_Sprite(spr_BACKDROP,320,120,252);	/* RESTORE: was 240-8 */

	Print_At(128,80);				/* PRINT SAVING TO CARD */
	Print_Messl(saver1[language]);
	if (currentcard==CARD1) Print_Messl("1");
	else Print_Messl("2");

	End_Loop();
	Update_Screen();

	for (delay=0;delay!=120;delay++)
	{
		VSync(0);
	}

	if (Save_Game()!=FAIL)
	{
		Start_Loop();

		Print_Sprite(spr_BACKDROP,320,120,252);	/* RESTORE: was 240-8 */

		Print_At(128,80);				/* PRINT LOAD SUCCESSFUL */
		Print_Messl(saver3[language]);

		End_Loop();
		Update_Screen();
	}
	else
	{
		Start_Loop();

		Print_Sprite(spr_BACKDROP,320,120,252);	/* RESTORE: was 240-8 */

		Print_At(128,80);				/* PRINT LOAD FAILED */
		Print_Messl(saver2[language]);

		End_Loop();
		Update_Screen();
	}

	scan_cards();

	Update_Screen();
	inmenu=0;			/* RESTORE: was 1; 360X's option menu is 0 */
	selectno=0;

}



UBYTE	*loading1[3]={	"LOADING FROM MEMORY CARD ",
					"CHARGEMENT DE LA CARTE M‚MOIRE ",
					"LADE VON MEMORY CARD "};

UBYTE	*loading2[3]={	"SCORES LOADED OK",
					"SCORES CHARG‚S",
					"HIGH SCORES GELADEN"};

UBYTE	*loading3[3]={	"ERROR HIGH SCORES NOT FOUND",
					"ERROR : MEILLEURS SCORES NON TROUV‚S",
					"FEHLER! HIGH SCORES NICHT GEFUNDEN"};

UBYTE	*saver1[3]={	"SAVING TO MEMORY CARD ",
					"SAUVEGARDE SUR LA CARTE M‚MOIRE ",
					"SPEICHERE AUF MEMORY CARD "};

UBYTE	*saver2[3]={	"ERROR WRITING FILE",
					"ERREUR D'‚CRITURE DU FICHIER",
					"FEHLER BEIM SCHREIBEN DER DATEN"};

UBYTE	*saver3[3]={	"SCORES SAVED OK",
					"SCORES SAUVEGARD‚S",
					"HIGH SCORES GESPEICHERT"};

UBYTE	*saver4[3]={	"ERROR HIGH SCORES NOT SAVED",
					"ERREUR: MEILLEURS SCORES NON SAUVEGARD‚S",
					"FEHLER! HIGH SCORES NICHT GESPEICHERT"};

UBYTE	*delete1[3]={	"ERROR TRYING TO DELETE FILE",
					"IMPOSSIBLE D'EFFACER CE FICHIER",
					"FEHLER BEIM LOSCHEN DER DATEN"};

UBYTE	*create1[3]={	"ERROR TRYING TO CREATE FILE",
					"IMPOSSIBLE DE CR‚ER CE FICHIER",
					"FEHLER BEI DER ERSTELLUNG DER DATEI"};

UBYTE	*unform_1[3]={	"THIS MEMORY CARD IS UNFORMATTED",
					"CETTE CARTE M‚MOIRE N'EST PAS FORMAT‚E",
					"DIE MEMORY CARD IST NICHT FORMATIERT"};

UBYTE	*unform_2[3]={	"PRESS X TO FORMAT IT",
					"APPUYEZ SUR X POUR LA FORMATER",
					"X DRUCKEN UM DIE KARTE ZU FORMATTEREN"};

UBYTE	*unform_3[3]={	"OR PRESS O TO CANCEL",
					"OU APPUYEZ SUR O POUR ANNULER",
					"ODER O DRUCKEN UM ABZUBRECHEN"};

UBYTE	*pre_sav1[3]={	"THIS MEMORY CARD CONTAINS",
					"CETTE CARTE M‚MOIRE CONTIENT",
					"DIESE MEMORY CARD ENTHALT"};

UBYTE	*pre_sav2[3]={	"A PREVIOUSLY SAVED GAME",
					"UN JEU PR‚ALABLEMENT SAUVEGARD‚",
					"EINEN BEREITS GESPEICHERTEN SPIELSTAND"};

UBYTE	*pre_sav3[3]={	"PRESS X TO DELETE IT",
					"APPUYEZ SUR X POUR L'EFFACER",
					"DRUCKEN SIE X UM IHN ZU LOSCHEN"};

UBYTE	*formating1[3]={"FORMATTING MEMORY CARD ",
					"CARTE M‚MOIRE  EN COURS DE FORMATAGE",
					"FORMATIERE MEMORY CARD "};

UBYTE	*formating2[3]={"FORMAT COMPLETE",
					"FORMATAGE TERMIN‚",
					"FORMATIERUNG KOMPLETT"};

UBYTE	*formating3[3]={"ERROR DURING FORMAT",
					"UNE ERREUR S'EST PRODUITE AU COURS"
					"FORMATIERUNGSFEHLER"};

UBYTE	*formating4[3]={"",
					"DU FORMATAGE",
					""};




/**************************************
*      TEST FOR A FORMATTED CARD      *
**************************************/

SHORT
Test_For_Format(void)
{

	ret = et_format(currentcard);

	if (ret==TRUE) return TRUE;
	else return FALSE;

}


/**************************************
*   TEST TO SEE IF CARD IS FORMATTED  *
**************************************/

SHORT
et_format(LONG chan)
{

BYTE	buf[128];

	bzero(&buf[0],128);
	_clear_event_x();				/* CLEAR EVENTS */
	_new_card();					/* CLEARS CARD FLAGS */
	_card_read(chan,0,&buf[0]);		/* DO A SAMPLE READ */

	if(_card_event_x()!=0)			/* IF FAILED THEN NOT FORMATTED */
		return FAIL;

	if(buf[0]=='M' && buf[1]=='C')		/* OTHERWISE IT IS FORMATTED */
		return TRUE;

	return FAIL;
}

/**************************************
*     LOAD THE SAVE FROM THE SLOT     *
**************************************/

SHORT
Load_Game(void)
{

long	i,fd;
int		errors=0;

	if (currentcard==CARD1)
	{
		if((fd=open(game_save_name1,O_RDONLY))>=0)
		{     
			i = read(fd,&LOAD_BUF[0],sizeof(_CARD));
			i = read(fd,&LOAD_BUF[0],sizeof(LOAD_BUF));	/* RESTORE: was 4096, past the end of LOAD_BUF */
			close(fd);			/* RESTORE: was closed only after a short read */
			if (i!=sizeof(LOAD_BUF)) return FAIL;
		}
		else
		{
			return FAIL;
		}
	}

	if (currentcard==CARD2)
	{
		if((fd=open(game_save_name2,O_RDONLY))>=0)
		{     
			i = read(fd,&LOAD_BUF[0],sizeof(_CARD));
			i = read(fd,&LOAD_BUF[0],sizeof(LOAD_BUF));	/* RESTORE: was 4096, past the end of LOAD_BUF */
			close(fd);			/* RESTORE: was closed only after a short read */
			if (i!=sizeof(LOAD_BUF)) return FAIL;
		}
		else
		{
			return FAIL;
		}
	}
	memcpy(highest,LOAD_BUF,sizeof(highest));	/* RESTORE: the save holds the high score table, as the Saturn's BUPRAM.C */
	return PASS;

}

/**************************************
*       SAVE THE GAME TO A SLOT       *
**************************************/

SHORT
Save_Game(void)
{

int		length;
long	totalbytes;
long	totalblock;
long	i,fd;
char	test[129];
int		counter;

unsigned char load_buff[4096];

	hed.Magic[0] = 'S';				/* CONSTRUCT HEADER DATA */
	hed.Magic[1] = 'C';
	hed.Type = 0x11;
	strcpy( hed.Title,"360X Save Game");
	memcpy( hed.Clut,saveclut, 32 );
	memcpy( hed.Icon[0],saveimage, 128);

	length = sizeof(SAVE_BUF) + sizeof(_CARD); /* GET LENGTH OF DATA */
	totalbytes = ((length/8192) * 8192) +  (length%8192 ? 8192:0 ); /* NO OF 8K SLOTS */
	hed.BlockEntry = ( totalbytes/8192  )+( totalbytes%8192 ? 1:0 ); /* NO OF 8K SLOTS */

	if (currentcard==CARD1)			/* DO CARD 1 */
	{
		if((fd=open(game_save_name1,O_WRONLY))>=0)
		{
			close(fd);			/* RESTORE: was printf("ERROR: that file already exists"); return FAIL; */
		}					/* the save now writes over its own file (NMS's prompt to delete it was never written) */

		else if((fd=open(game_save_name1,O_CREAT|(hed.BlockEntry<<16)))==-1)	/* RESTORE: was if */
		{	
			printf("ERROR: could not create file \n");
			return FAIL;
		}
		else
		{
			close(fd);
			printf("\n");
		}

		if((fd=open(game_save_name1,O_WRONLY))>=0)
		{
			memcpy(&load_buff[0],&hed,sizeof(_CARD));
			i = write(fd,&load_buff[0],sizeof(_CARD));
			if(i!=(sizeof(_CARD)))
			{
				close(fd);
				return FAIL;
			}

			i = write(fd,&SAVE_BUF,sizeof(SAVE_BUF));	/* RESTORE: was 4096, past the end of SAVE_BUF */
			if(i!=sizeof(SAVE_BUF)) 
			{
				close(fd);
				return FAIL;
			}
		}
	}

	if (currentcard==CARD2)			/* DO CARD 2 */
	{
		if((fd=open(game_save_name2,O_WRONLY))>=0)
		{
			close(fd);			/* RESTORE: was printf("ERROR: that file already exists"); return FAIL; */
		}					/* the save now writes over its own file (NMS's prompt to delete it was never written) */

		else if((fd=open(game_save_name2,O_CREAT|(hed.BlockEntry<<16)))==-1)	/* RESTORE: was if */
		{	
			printf("ERROR: could not create file \n");
			return FAIL;
		}
		else
		{
			close(fd);
			printf("\n");
		}

		if((fd=open(game_save_name2,O_WRONLY))>=0)
		{
			memcpy(&load_buff[0],&hed,sizeof(_CARD));
			i = write(fd,&load_buff[0],sizeof(_CARD));
			if(i!=(sizeof(_CARD)))
			{
				printf("Error failed whilst writing header\n"); 
				close(fd);
				return FAIL;
			}

			i = write(fd,&SAVE_BUF,sizeof(SAVE_BUF));	/* RESTORE: was 4096, past the end of SAVE_BUF */
			if(i!=sizeof(SAVE_BUF)) 
			{
				printf("Error failed whilst writing body\n"); 
				close(fd);
				return FAIL;
			}
		}
	}

	close(fd);
	return PASS;

}




















/* RESTORE: was #endif */








#if 0

/**************************************
*         GET THE CARD INFO           *
**************************************/

GetCardInfo(void)
{

	_pre_draw();
	_draw();
	exec=0;

	_card_info(currentcard);
	ret = _card_event();

	FntPrint("\nSTATUS OF CARD PORT");
	if (currentcard==CARD1) FntPrint("1\n");
	if (currentcard==CARD2) FntPrint("2\n");

	switch(ret)
	{
		case 0:
			FntPrint("CARD EXISTS\n");
			break;
		case 2:
			FntPrint("NO CARD\n");
			break;
		case 3:
			FntPrint("NEW CARD\n");
			_clear_event_x();
			_card_clear(currentcard);
			ret = _card_event_x();
			break;
		case 1:
		default:
			FntPrint("ERROR\n");
			break;
	}

	_clear_event();
  	_card_load(currentcard);			/* LOAD FILE SYSTEM */
  	ret = _card_event();

	switch(ret)
	{
		case 0:
			FntPrint("FORMATTED\n");
			break;
		case 2:
			FntPrint("NO CARD\n");
			break;
		case 3:
			FntPrint("UNFORMATTED\n");
			break;
		case 1:
		default:
			FntPrint("ERROR\n");
		break;
	}

	FntPrint("\nPRESS A KEY");
	_pre_draw();
	_draw();
	while(GetPad()==0);
	while(GetPad()!=0);

}

/**************************************
*    DELETE A FILE FROM THE CARD      *
**************************************/

DeleteFile(void)
{

	card_status();

	if (Delete_Game()==FAIL) FntPrint("\nDelete Failed...");
	else FntPrint("\nDelete Successful...");

	_pre_draw();
	_draw();
	while(GetPad()==0);
	while(GetPad()!=0);

}

/**************************************
*    DELETE THE SAVE FROM THE SLOT    *
**************************************/

int Delete_Game()
{

	if (currentcard==CARD1)
	{
		if (delete(game_save_name1)) return PASS;
		else return FAIL;
	}

	if (currentcard==CARD2)
	{
		if (delete(game_save_name2)) return PASS;
		else return FAIL;
	}
}

#endif


/**************************************
*           SAVE HIGH SCORES          *
**************************************/

void
Save_High_Scores(void)
{

USHORT	delay;

	for (delay=0;delay<64;delay++)
	{
		Start_Loop();

		Print_Sprite(spr_BACKDROP,320,120,32);
		Plot_Stars(32);

		End_Loop();
	}

	memcpy(SAVE_BUF,highest,sizeof(highest));	/* RESTORE: Save_High_Scores ended here; the save holds the */
	Try_SaveGame();					/* high score table, as the Saturn's BUPRAM.C writes highest */
}










































/**************************************
*         UPDATE THE SCREEN           *
**************************************/

void
Update_Screen(void)
{

	Start_Loop();
	Print_Sprite(spr_BACKDROP,320,120,252);
	End_Loop();

}










/**************************************
*          CLEAR CARD EVENTS          *
**************************************/

LONG
_clear_event(void)
{
	TestEvent(ev0);
	TestEvent(ev1);
	TestEvent(ev2);
	TestEvent(ev3);
}

/**************************************
*          CLEAR CARD EVENTS          *
**************************************/

LONG
_clear_event_x(void)
{
	TestEvent(ev10);
	TestEvent(ev11);
	TestEvent(ev12);
	TestEvent(ev13);
}

/**************************************
*     A CARD EVENT HAS HAPPENED       *
**************************************/

LONG
_card_event(void)
{
	while(1)
	{
		if(TestEvent(ev0)==1) return 0;	/* IOE */
		if(TestEvent(ev1)==1) return 1;	/* ERROR */
		if(TestEvent(ev2)==1) return 2;	/* TIMEOUT */
		if(TestEvent(ev3)==1) return 3;	/* NEW CARD */
	}
}

/**************************************
*     A CARD EVENT HAS HAPPENED       *
**************************************/

LONG
_card_event_x(void)
{
	while(1)
	{
		if(TestEvent(ev10)==1) return 0;	/* IOE */
		if(TestEvent(ev11)==1) return 1;	/* ERROR */
		if(TestEvent(ev12)==1) return 2;	/* TIMEOUT */
		if(TestEvent(ev13)==1) return 3;	/* NEW CARD */
	}
}

/**************************************
*         GET THE CARD STATUS         *
**************************************/

LONG
card_status(void)
{

LONG	oldret,ret;

	_card_info(currentcard);
	ret=_card_event();
	{
 		if(ret==0) oldret=ret;		/* CARD READY */
		if(ret==1) oldret=ret;		/* CARD FAILURE */
		if(ret==2) oldret=ret;		/* NO CARD PRESENT */
		if(ret==3) oldret=ret;		/* NEW CARD INSERTED */
	}
}

/**************************************
*        SCAN BOTH CARD SLOTS         *
**************************************/

LONG
scan_cards(void)
{
	_card_info(CARD1);				/* GET INFO FROM CARD */
	ret1 =_card_event();			/* WAIT FOR EVENT */
	_clear_event();
	_card_load(CARD1);				/* LOAD FILE SYSTEM */
	ret1 = _card_event();

	_card_info(CARD2);				/* GET INFO FROM CARD */
	ret2 =_card_event();			/* WAIT FOR EVENT */
	_clear_event();
	_card_load(CARD2);				/* LOAD FILE SYSTEM */
	ret2 = _card_event();

	if (ret1==3)
	{
		_clear_event_x();
		_card_clear(CARD1);
		_card_event_x();
	}

	if (ret2==3)
	{
		_clear_event_x();
		_card_clear(CARD2);
		_card_event_x();
	}
}

