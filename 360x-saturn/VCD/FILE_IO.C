/*****************************************************************/
//
//	File System Code
//
/*****************************************************************/

#include <sgl.h>
#include <libsn.h>
#include <sega_cdc.h>
#include <sega_gfs.h>
#include <stdio.h>
#include	"defines.h"
#include "types.h"

/*****************************************************************/
/* CD Stuff ******************************************************/
/*****************************************************************/

#define 		OPEN_MAX 	2
#define 		MAX_DIR  	80
#define 		SECT_SIZE 	2048

Uint32 		work[GFS_WORK_SIZE(OPEN_MAX) / 4]; /* library working area */
GfsDirTbl 	dirtbl1;									  //Directory tables
GfsDirTbl 	dirtbl2;									  //Directory tables
GfsDirName 	dirid[MAX_DIR];
GfsDirName 	dirsub[MAX_DIR];
GfsHn 		gfs1,gfs2,gfs;								  //File handles

Sint32 		fid,fid2;								  //file id
Sint32 		ret;
Sint32		dir;
Sint32		ndir;

static		CdcPly 		ply;
static		CdcPos 		pos;


/* Globals ********************************************************/

/* ProtoTypes *****************************************************/

void	InitFileSystem(void);
void	ReadFile(char *name,char *target);
void	SaveData(Uint8 *data,Uint32 len,Uint8 *fname);
void	LoadBackground(void);
void	LoadBackgroundData(char *levtext);

void	PlayCDAudio(Uint16	trackno);
void	PauseCDAudio(void);
void	ResumeCDAudio(void);
void	StopCDAudio(void);

/* Externals ******************************************************/

extern	void		InitPalette(Uint16 *palette,Uint8 bankno);

extern	Uint16	MapWidth;
extern	Uint16	MapHeight;
extern	Uint16	MapCellWidth;
extern	Uint16	MapCellHeight;
extern	long		fsize;
extern	Uint8		*collmap;

extern	Uint8		levelname[8][22];
extern	Uint8		Level;
extern	void 		msg(char *ptr);
extern	void 		clearmsg(void);

extern	char		stat_str[];
extern	Uint16	palette[];

/******************************************************************/

void	InitFileSystem(void)
{
	#ifdef	USECD

	Sint32      ndir=0;
	Sint32      bdir;

	#ifndef	CUTCD
	PCinit();

	while(1)
	{
		bdir	=	PSYQcdinit();

		if(bdir==0)
		{
			msg("PSYQcdinit complete \n");
			break;
		}
	}
	#endif

	GFS_DIRTBL_TYPE(&dirtbl1) 		= GFS_DIR_NAME;	//Main directory
	GFS_DIRTBL_NDIR(&dirtbl1) 		= MAX_DIR;
	GFS_DIRTBL_DIRNAME(&dirtbl1) 	= dirid;

	GFS_DIRTBL_TYPE(&dirtbl2) 		= GFS_DIR_NAME;	//Subdirectories
	GFS_DIRTBL_NDIR(&dirtbl2) 		= MAX_DIR;
	GFS_DIRTBL_DIRNAME(&dirtbl2) 	= dirsub;

	ret = GFS_Init(OPEN_MAX,work,&dirtbl1);

	//Enable DMA Channel 0
	INT_ChgMsk(INT_MSK_DMA0,INT_MSK_NULL);	

	//for cdda
	bdir	=	CDC_CdInit((127-96),0xffff,5,0xff);

	//Get current directory information
	ndir	=	GFS_LoadDir(dir,&dirtbl1);
	ndir	=	GFS_SetDir(&dirtbl1);

	#else

	Sint32      bdir;

	bdir	=	CDC_CdInit((127-96),0xffff,5,0xff);	//for cdda
   
	PCinit();

	#endif

}

/******************************************************************/

#ifndef	USECD

void	ReadFile(char *name,char *target)
{
	Uint32	len;
	Sint16	handle;

	handle	=	PCopen(name,0,0);

	msg("Loading:");
	msg(name);

	if(handle!=-1)
	{
		len = PClseek(handle,0,2);

		fsize	=	len;

		PClseek(handle,0,0);

		PCread(handle,target,len);

	}
	else
	{
		
		sprintf(stat_str,"CAN'T OPEN FILE:%s",name);
		msg(stat_str);
		
		slTVOn();

		while(1)
		{
			pollhost();
			slSynch();
		}
	}

	while((fsize % 4) !=0)
	{
		fsize++;
	}

	sprintf(stat_str," - %d bytes\n",(int)fsize);

	msg(stat_str);

	PCclose(handle);

}

/******************************************************************/

#else

void	ReadFile(char *name, char *target)
{

	Sint32	sct,nsct,lsize;
	Uint32	offset	=	0;

	//Convert filename to file id
	fid	=	GFS_NameToId(name);

	if(fid > 0)
	{
		//Get the file size
		gfs1 = GFS_Open(fid);

		GFS_GetFileSize(gfs1,&sct,&nsct,&lsize);

		GFS_Close(gfs1);

		fsize	=	sct * (nsct - 1) + lsize;

		//If the file is bigger than 400k (ie. what can fit in the buffer)
		//then load it in bits

		if( fsize>(400*1024) )
		{
			while(fsize>(offset*2048))
			{
				GFS_Load(fid,offset,(Uint8 *)(target+(offset*SECT_SIZE)),(SECT_SIZE*100));
				offset+=100;
			}
		}
		else
		{
			if((fsize % 2)!=0) fsize++;

			GFS_Load(fid,0,target,GFS_BUFSIZ_INF);
		}

		msg("Loading:");
		msg(name);
		sprintf(stat_str," - %d bytes\n",(int)fsize);
		msg(stat_str);

	}
	else
	{
		sprintf(stat_str,"Bad file ID for %s\n",name);
		msg(stat_str);
	}

}	 	

#endif

/******************************************************************/

void	LoadBackgroundData(char *levtext)
{
	char	filename[40];

	//Load Palette File
	sprintf(filename,"levels/%s.pal",levtext);
	ReadFile(filename,(Uint8 *)palette);
//	InitPalette(palette,1);

	//Load Map File
	sprintf(filename,"levels/%s.map",levtext);
	ReadFile(filename,MapArea);

	//Load Block File
	sprintf(filename,"levels/%s.bin",levtext);
	ReadFile(filename,LowRam);

	//Read in the collision map
	sprintf(filename,"levels/%s.col",levtext);
	ReadFile(filename,collmap);

	//Get dimension of the map
	MapCellWidth	=	*((Uint16 *)(MapArea));
	MapCellHeight	=	*((Uint16 *)(MapArea+2));

	MapWidth		=	MapCellWidth * 16;
	MapHeight	=	MapCellHeight * 16;
	
}

/*****************************************************************/

void	PlayCDAudio(Uint16	trackno)
{

	 CDC_PLY_STYPE(&ply)		=	CDC_PTYPE_TNO;
	 CDC_PLY_STNO(&ply)		=	trackno;
	 CDC_PLY_SIDX(&ply)		=	0;
	 CDC_PLY_ETYPE(&ply)		=	CDC_PTYPE_TNO;
	 CDC_PLY_ETNO(&ply)		=	trackno;
	 CDC_PLY_EIDX(&ply)		=	0;
	 CDC_PLY_PMODE(&ply)		=	0x0f;		//Endless repeats

	 ret	=	CDC_CdPlay(&ply);

}

/*****************************************************************/

void	PauseCDAudio(void)
{
	CDC_POS_PTYPE(&pos)	=	CDC_PTYPE_NOCHG;
	ret	=	CDC_CdSeek(&pos);
}

/*****************************************************************/

void	ResumeCDAudio(void)
{
	CDC_PLY_STYPE(&ply) = CDC_PTYPE_NOCHG;
	CDC_PLY_ETYPE(&ply) = CDC_PTYPE_NOCHG;
	CDC_PLY_PMODE(&ply) = CDC_PM_NOCHG;
	CDC_CdPlay(&ply);

	ret	=	CDC_CdPlay(&ply);
}

/*****************************************************************/

void	StopCDAudio(void)
{
	CDC_POS_PTYPE(&pos)	=	CDC_PTYPE_DFL;
	ret	=	CDC_CdSeek(&pos);
}

/*****************************************************************/

#ifndef	USECD

void	SaveData(Uint8 *data,Uint32 len,Uint8 *fname)
{
	Uint32	length;
	Sint16	handle;

	sprintf(stat_str,"Creating %s\n",fname);
	msg(stat_str);

	handle	=	PCcreat(fname,0);

	if(handle!=-1)
	{
		length=PCwrite(handle,data,len);
		sprintf(stat_str,"Written %d bytes\n",(int)length);
	}
	else
	{
		sprintf(stat_str,"CAN'T OPEN FILE:%s",fname);
	}

	msg(stat_str);

	PCclose(handle);

}

#endif

/******************************************************************/
