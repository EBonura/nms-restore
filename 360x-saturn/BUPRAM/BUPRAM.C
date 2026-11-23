/******************************************************************/
/* 																					*/
/* BUPRAM.C																			*/
/* 																					*/
/* BACKUP Ram Code																*/
/* 																					*/
/******************************************************************/

#include <sgl.h>
#include <libsn.h>
#include	<sega_bup.h>
#include <string.h>
#include "defines.h"
#include "types.h"

/* Globals ********************************************************/

Uint32	BackUpRamWork[2048];
Uint8		BackUpRamArea[(20*1024)];
Uint32	backupdevice;
BupStat	sttb;
BupDir	brdir;

/* ProtoTypes *****************************************************/

void		BackUpInit(BupConfig *cntb);
Sint32	BackUpWrite(Uint32 device, BupDir *wdir, Uint8 *data, Uint8 sw);
Sint32	BackUpDelete(Uint32 device,Uint8 *filename);
Sint32	BackUpFormat(Uint32 device);
void 		UpdateBackupRam(void);
Sint32	ReadHiscores(void);
Sint32	WriteHiscores(void);

/* Externals ******************************************************/

extern	void		PrintLARGE(Uint16 x,Uint16 y,Uint8 *string);
extern	void		PrintMEDIUM(Uint16 x,Uint16 y,Uint8 *string);
extern	void		PrintSMALL(Uint16 x,Uint16 y,Uint8 *string);

extern	HIGH		highest[];

/* Misc ***********************************************************/

#define	BUP_START_ADDR	BackUpRamArea

#define	FILE_NAME		"360X"
#define	COMMENT			"HighScores"
#define	BACKUP_DEVICE	(Uint32)1

#define	HEX2DEC(x)		( ( 0x0f&(x) ) + ( (x) >> 4 )*10 )

#define	TABLESIZE		(sizeof(HIGH) * 10)

/******************************************************************/
/******************************************************************/
/******************************************************************/

void	BackUpInit(BupConfig cntb[3] )
{
	slResetDisable();
	BUP_Init((Uint32 *)BUP_START_ADDR,BackUpRamWork,cntb);
	slResetEnable();
}

/******************************************************************/

Sint32	BackUpWrite(Uint32 device, BupDir *wdir, Uint8 *data, Uint8 sw)
{

	Sint32			ret;
	SmpcDateTime	*time;
	BupDate			date;
	
	if(!wdir->date)
	{
		time = &(Smpc_Status->rtc);
		date.year  = (Uint8 )( slDec2Hex((Uint32)time->year)-1980 );
		date.month = (Uint8 )( time->month & 0x0f);
		date.week  = (Uint8 )( time->month >> 4  );
		date.day   = (Uint8 )( slDec2Hex((Uint32)time->date));
		date.time  = (Uint8 )( slDec2Hex((Uint32)time->hour));
		date.min   = (Uint8 )( slDec2Hex((Uint32)time->minute));
		wdir->date = BUP_SetDate(&date);
	}
	slResetDisable();
	ret = BUP_Write(device,wdir,data,sw);
	slResetEnable();

	return(ret);

}

/******************************************************************/

Sint32	BackUpDelete(Uint32 device,Uint8 *filename)
{
	Sint32	ret;

	slResetDisable();
	ret = BUP_Delete(device,filename);
	slResetEnable();

	return(ret);
}

/******************************************************************/

Sint32	BackUpFormat(Uint32 device)
{
	Sint32	ret;

	slResetDisable();
	ret = BUP_Format(device);
	slResetEnable();

	return(ret);
}

/******************************************************************/

void InitBackupRam(void)
{

	//BupDate		datetb, date;
	//Uint8			*time;

	BupConfig	conf[3];
	Sint32		status;
	//Uint8			buf[256];
	int			i;
	flag			savedata;


	slGetStatus(); 

	//Wait for function to complete
	for(i=0;i<100;i++)
	{
		slSynch() ;
	}

	BackUpInit(conf);
	
	status = BUP_Stat( backupdevice, TABLESIZE, &sttb ) ;

   //Check if backup ram has been formatted...if not,format it now                                         
	if( status == BUP_UNFORMAT)
	{
	 	status = BackUpFormat(backupdevice);
		//Re-read status
		BUP_Stat( backupdevice, TABLESIZE, &sttb );
		savedata=FALSE; //No hiscores will be in the bupram!
	}
	else
	{
		status = BUP_Read( backupdevice, FILE_NAME, (Uint8 *)highest);

		if(status==BUP_NOT_FOUND)
		{
			savedata=FALSE;	//No hiscore data is in bupram
		}
		else
		{
			ReadHiscores();
			savedata=TRUE;
		}
	}


	//If no data has previosly been written to backup ram write it now
	if(savedata==FALSE)
	{
		if( sttb.freeblock >= TABLESIZE )
		{
			WriteHiscores();
		}
		else
		{
			//Print message to show not enough ram is left
		}
	}

}

/******************************************************************/

Sint32	ReadHiscores(void)
{
   Sint32	status;

	slResetDisable();
	status = BUP_Read( backupdevice, FILE_NAME, (Uint8 *)highest);
	slResetEnable();
	
	return status;

}

/******************************************************************/

Sint32	WriteHiscores(void)
{
   Sint32	status;

	strncpy( brdir.filename, FILE_NAME  , 4 );
	strncpy( brdir.comment , COMMENT, 9 );
	brdir.language = BUP_ENGLISH;
	brdir.datasize = TABLESIZE;
	brdir.date     = 0;

	status = BackUpWrite(backupdevice, &brdir, (Uint8 *)&highest,OFF);

	return(status);
}

/******************************************************************/
