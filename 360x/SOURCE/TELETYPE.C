
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE DEFINES            *
**************************************/

/**************************************
*          DECLARE VARIABLES          *
**************************************/

USHORT	teletypex;
USHORT	teletypey;
USHORT	teletypenumber;
USHORT	teletypepoint;
USHORT	teletypedelay;

/**************************************
*       INITIALISE THE TELETYPE       *
**************************************/

void
Init_Teletype(void)
{

	teletypenumber=0;
	teletypepoint=0;
	teletypedelay=0;

}

/**************************************
*     PRINT THE CREDITS ON SCREEN     *
**************************************/

void 
Print_Teletype(TEXT *text,SHORT lines)
{

USHORT	j;
UBYTE	letter=1;		/* RESTORE: was uninitialised; read before set at the start of each line, nonzero in the 1996 build (a callee-saved register), so the line is typed out */
UBYTE	*mess;

	if (teletypenumber!=0)
	{
		for (j=0;j<teletypenumber;j++)
		{
			if ((text[j].control1)==22) Print_At(text[j].control2,text[j].control3);

			if (text[j].fonttype==0)
				Print_Messs(text[j].textstuff);
	
			if (text[j].fonttype==1)
				Print_Messm(text[j].textstuff);
	
			if (text[j].fonttype==2)
				Print_Messl(text[j].textstuff);
		}
	}

	if (teletypenumber!=lines)
	{
		mess=text[teletypenumber].textstuff;

		if ((text[teletypenumber].control1)==22)
			Print_At(text[teletypenumber].control2,text[teletypenumber].control3);

		for (j=0;j<teletypepoint;j++)
		{
			letter=*(mess++);

			if (text[teletypenumber].fonttype==0)
			{
				if (letter>=32) Print_Sprite(&spr_FONTSMALL[letter-32],messx,messy,255);
				messx+=spr_FONTSMALL[letter-32].w;
			}

			if (text[teletypenumber].fonttype==1)
			{
				if (letter>=32) Print_Sprite(&spr_FONTMEDIUM[letter-32],messx,messy,255);
				messx+=spr_FONTMEDIUM[letter-32].w;
			}

			if (text[teletypenumber].fonttype==2)
			{
				if (letter>=32) Print_Sprite(&spr_FONTLARGE[letter-32],messx,messy,255);
				messx+=spr_FONTLARGE[letter-32].w;
			}
		}

		if (text[teletypenumber].fonttype==0)
			Print_Sprite(&spr_FONTSMALL[62],messx,messy,255);
		if (text[teletypenumber].fonttype==1)
			Print_Sprite(&spr_FONTMEDIUM[62],messx,messy,255);
		if (text[teletypenumber].fonttype==2)
			Print_Sprite(&spr_FONTLARGE[62],messx,messy,255);

		if (letter==0)
		{
			teletypepoint=0;
			if (teletypenumber!=lines) teletypenumber++;
		}
		else
		{
			if ((gamecycle MOD 1)==0) teletypepoint++;
		}
	}
}

