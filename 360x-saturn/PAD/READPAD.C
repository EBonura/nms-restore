/******************************************************************/
//
// READPAD.C
//
// All scroll control stuff 
//
/******************************************************************/

#include	<sgl.h>
#include <libsn.h>
#include	"defines.h"
#include "types.h"
#include "globals.h"
#include <stdio.h>

/* Globals ********************************************************/


/* Externals ******************************************************/

extern	Uint8		weapontype;
extern	Uint16	MapWidth;
extern	Uint16	MapHeight;
extern	Uint16	MapCellWidth;
extern	Uint16	MapCellHeight;

extern	Sint16	ScrollX;
extern	Sint16	ScrollY;
extern	Uint16	LastScrollX;
extern	Uint16	LastScrollY;

extern	void		UpdateBottomEdge(void);
extern	void		UpdateTopEdge(void);

extern	Uint8		GamePauseFlag;
extern	Uint8		toggle;

extern	void		PlayCDAudio(Uint16	trackno);
extern	void		PauseCDAudio(void);
extern	void		ResumeCDAudio(void);
extern	void		StopCDAudio(void);

extern	Uint8		CdTrack;

extern	char		stat_str[];
extern	Sint16	Gamma;
extern	flag		HandlePauseFlag;
extern	Sint8		HandlePauseMenu;
extern	Uint8		PauseOption;

extern	Sint16	CDAudioVolume;
extern	Sint16	SFXVolume;
extern	BALL 		ball[];

extern	Uint8		play_sample(Uint8	channel,Uint8 pitch);
extern	Uint8		play_sample_with_volume(Uint8	voice,Uint8 pitch,Uint8 volume);
extern	Uint8		play_sample_with_volume_and_direction(Uint8	voice,Uint8 pitch,Uint8 volume,Sint8 direction);

extern	void 		HandleScrollParameters(void);

/* Prototypes *****************************************************/

void	ProcessPad(void);
void	ProcessPausePad(Uint16	push,Uint16	data);

/******************************************************************/
//	Routine to control scroller
/******************************************************************/


void	ProcessPad(void)
{

 	Uint16	data;
	Uint16	push;


	//Go immediately into pause mode on pad disconnect
	if(!Per_Connect1)
	{
		GamePauseFlag	= PAUSED;
	}
	else
	{
		//Get the pad data
		data	=	Smpc_Peripheral[0].data;
		push	=	Smpc_Peripheral[0].push;
		
		//Check which pad routine needs to be processed
		if(HandlePauseFlag==TRUE)
		{
			ProcessPausePad(push,data);
		}
		else
		{
			HandleScrollParameters();
		}
		
		//See if pause menu has been summoned
		if( (HandlePauseFlag==FALSE) && (GamePauseFlag==PAUSED) && (push & PER_DGT_TA)==0)
		{
			HandlePauseFlag=TRUE;
			PauseOption=0;
		}
		
		//Change pause states
		if((push & PER_DGT_ST)==0 )
		{
			GamePauseFlag	^= PAUSED;
			
			if(GamePauseFlag!=PAUSED) 
			{
				HandlePauseFlag=FALSE;
			}
			else
			{
				HandlePauseMenu=MENUMODE;
			}
		
			if(toggle==0)
			{
				toggle = 1;
			}
			else if(toggle==2)
			{
				toggle = 3;
			}
		}
		
		#if	0
		//CD Player controls
		if((push & PER_DGT_TL)==0 )
		{
			if(CdTrack>2) CdTrack--;
			PlayCDAudio(CdTrack);
		}
		
		if((push & PER_DGT_TR)==0 )
		{
			if(CdTrack<24) CdTrack++;
			PlayCDAudio(CdTrack);
		}
		#endif
		
		//Change the weapons
		if((push & PER_DGT_TL)==0  && (weapontype!=0)) weapontype--;
		if((push & PER_DGT_TR)==0  && (weapontype!=6)) weapontype++;

		if((push & PER_DGT_TX)==0 ) play_sample(0,36);
		if((push & PER_DGT_TY)==0 ) play_sample(1,36);
		if((push & PER_DGT_TZ)==0 ) play_sample(2,36);

		//if((data & PER_DGT_TX)==0 ) play_sample_with_volume(0,36,1);
		//if((data & PER_DGT_TY)==0 ) play_sample_with_volume(0,36,64);
		//if((data & PER_DGT_TZ)==0 ) play_sample_with_volume(0,36,127);

	}
}

/******************************************************************/

void	ProcessPausePad(Uint16	push,Uint16	data)
{

	//Go back to the PAUSED sign if B is pressed
	if( (GamePauseFlag==PAUSED) && (push & PER_DGT_TB)==0)
	{
		HandlePauseFlag=FALSE;
	}

	//Poll the controls for each type of menu
 	switch(HandlePauseMenu)
	{
		case	MENUMODE:
		{
			if( (push & PER_DGT_KU)==0 && (PauseOption > 0) ) PauseOption--;
			if( (push & PER_DGT_KD)==0 && (PauseOption < 3) ) PauseOption++;
			if( (push & PER_DGT_TA)==0 ) HandlePauseMenu = PauseOption;
			break;
		}

		case	RESUME:
		{
			if( (push & PER_DGT_TA)==0 )
			{
				HandlePauseMenu=RESUME;
			}
			break;
		}
		
		case	GAMMA:
		{
			if( (data & PER_DGT_KL)==0 && (Gamma > -31)) Gamma-=1;
			if( (data & PER_DGT_KR)==0 && (Gamma < 31)) Gamma+=1;
			if( (push & PER_DGT_TA)==0 ) HandlePauseMenu = MENUMODE;
			break;
		}

		case	CD_VOLUME:
		{
			if( (data & PER_DGT_KL)==0 && (CDAudioVolume > 0)) CDAudioVolume-=1;
			if( (data & PER_DGT_KR)==0 && (CDAudioVolume < 127)) CDAudioVolume+=1;
			if( (push & PER_DGT_TA)==0 ) HandlePauseMenu = MENUMODE; 
			break;
		}

		case	SFX_VOLUME:
		{
			if( (data & PER_DGT_KL)==0 && (SFXVolume > 0)) SFXVolume-=1;
			if( (data & PER_DGT_KR)==0 && (SFXVolume < 127)) SFXVolume+=1;
			if( (push & PER_DGT_TA)==0 ) HandlePauseMenu = MENUMODE; 
			break;
		}

		default:
		{
			break;
		}
	}

}

/******************************************************************/

