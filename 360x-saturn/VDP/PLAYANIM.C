/******************************************************************/
//
// PLAYANIM.C
//
//	uses flic.s for CD animation playback
//
/******************************************************************/

#include <sgl.h>
#include <libsn.h>
#include "defines.h"
#include	"types.h"
#include "globals.h"
#include <stdio.h>
#include <sega_cdc.h>
#include <sega_gfs.h>
#include <sega_sys.h>
#include <sega_snd.h>

/* Misc ***********************************************************/

#define	buffer_size		(256*1024)

/* Prototypes *****************************************************/

void			InitAnimationPlaybackScreen(void);
void 			play_anim(int dest, char *fname);
void 			CD_fread(char *dest,long length);
long			read_long_from_disk( int fp );

/* Globals ********************************************************/

Uint8		cd_error	=	0;
Uint32	blip_num=0;

TEXTURE	anim_screen[]=
{
	TEXDEF(320,200,0),
};

Uint8		*proc_bp=LowRam;		// This is where a compressed frame is stored 64000 is very safe 32k is probably OK
char		buffer[buffer_size];	// This is the load buffer (which wraps around etc. in play with cache (this should be as big as possible)
char		screen_buffer[64000];

char		filename[40];

Uint32	VBLcount=0;

Uint8		DONESPRITES=0;

Sint32	sct,nsct,lsize,sctsize;
Sint32	fsize,sect_size;
CdcStat	cdstat;

long		lastsize,total_sect;

void		*filep;

/* Externals ******************************************************/

extern	void		InitPalette(Uint16 *palette,Uint8 bankno);
extern	void		ReadFile(Uint8 *name,Uint8 *target);
extern	int		code_webdecomp(void *,void *);
extern	int		reverse_long(void *,int);

extern	int		memset(void *,char,int);
extern	Uint16	palette[];

extern	GfsHn 	gfs1,gfs2,gfs;
extern	Sint32 	fid,fid2;
extern	Sint32 	ret;
extern	Sint32	dir;
extern	Sint32	ndir;

/******************************************************************/

void	InitAnimationPlaybackScreen(void)
{
	int	i;

	slInitSystem(TV_320x240,anim_screen,1);

	slTVOff();

	//Clear out VRAM
	for(i=0;i<0xfa00;i++)
	{
		*((Uint8 *)(SpriteVRAM+CGADDRESS+i))	=	0;
	}

	slPrioritySpr0(7);

	slColRAMMode(CRM16_1024);

	slColRAMOffsetSpr(0);

	slTVOn();

}

/******************************************************************/

void play_anim(int dest, char *fname)
{
	unsigned 	char 	seq;
	int			amount_loaded;
	int			amount_played;
	int			amount_needed;
	int			to_end_of_buffer;
//	int			handle;
	int			frame_pos,load_pos;
	int			s=1,j;
	int			filesize;
//	long			temp,*vbl=&VBLcount;
	flag			playtheflic=YES;
//	Sint32 		i,stat,nbyte,ret;
	int 			first_frame;
//	SndIniDt 	snd_init;
//   SndSeqStat 	status;
	int			pos_error;
//	static		SPR_ATTR		Normal	=	SPR_ATTRIBUTE(0,0x0000,No_Window|No_Gouraud,CL256Bnk|ECdis|MESHoff|CL_Replace,sprNoflip);
//	FIXED			Pos[]={0,0,ORIGINAL*65536,170*65536};

	
	cd_error=0;
	sprintf(filename,"%s.PAL",fname);
	ReadFile(filename,(Uint8 *)palette);
	InitPalette(palette,0);

	first_frame=0;
	
	while(playtheflic==YES)
	{
		load_pos=0;
		frame_pos=4;
		amount_loaded=0;
		amount_played=0;
		seq=0;

		#if 0
		if (which_anim==1)
		{
			sprintf(filename,"XEARTHM.BIN",fname);
			loadfile(0x205d70);
			sprintf(filename,"XEARTH.BIN",fname);
			loadfile(0x205e00);
		}
		else
		{
			sprintf(filename,"X%sM.BIN",fname);
			loadfile(0x205d70);
			sprintf(filename,"X%s.BIN",fname);
			loadfile(0x205e00);
		}
		#endif

		sprintf(filename,"%s.PAT",fname);
		fid= 	GFS_NameToId(filename);
		gfs=	GFS_Open(fid);
		GFS_GetFileSize(gfs,&sctsize,&nsct,&lastsize);
		filesize=sctsize*(nsct-1)+lastsize;

		GFS_SetTransPara(gfs,16384/2048);
//		GFS_SetReadPara(gfs,16384/2048);
  		GFS_NwCdRead(gfs,nsct);

		total_sect=0;

//		buffer=0x6080000;
//		zoom=255;
//		fade_type=3;

		CD_fread(buffer,(buffer_size/2));				// get length of first frame
		if (cd_error!=0) break;
		load_pos+=(buffer_size/2);
		amount_loaded+=(buffer_size/2);
		filep=buffer;

//		sound_init();

//		while(1)
//		{
//			if (fade_type==0) break;
//			slSynch();
//			//WaitforVB();
//		}

		//Clear the buffer
		memset(buffer,0,64000);
		
		memset(screen_buffer,0,64000);
		
		//memcpy(pallettebuf,proc_bp,512); 

		s = read_long_from_disk(s);

		while( (amount_played+2048)<filesize )
		{
			// Read next chunk into buffer
			CDC_GetCurStat(&cdstat);
			if (cdstat.status==CDC_ST_OPEN) SYS_EXECDMP();
		
			amount_needed=2048*8;
			if( (amount_loaded+amount_needed) <= (amount_played+(s+4)) )
			{
				amount_needed=((s+4)/sect_size)+1;
				amount_needed=amount_needed*sect_size;
			}
		
			// Bodge...
		
			if(amount_loaded<filesize)
			{
				to_end_of_buffer=amount_loaded+amount_needed-amount_played;
				if( to_end_of_buffer > (buffer_size-(65536*3)) )
				{
				amount_needed=2048*6;
				}
				if( to_end_of_buffer > (buffer_size-(65536*2)) )
				{
					amount_needed=2048*4;
				}							  
				if( to_end_of_buffer > (buffer_size-(65536+32768)) )
				{
					amount_needed=2048*2;
				}
				if( to_end_of_buffer > (buffer_size-(65536+16384)) )
				{
					amount_needed=2048;
				}
				if( to_end_of_buffer > (buffer_size-65536 ) )
				{
					amount_needed=0;
				}
			}
			else amount_needed=0;
		
			amount_loaded+=amount_needed;
		
			to_end_of_buffer=buffer_size-load_pos;
			if( amount_needed>to_end_of_buffer )
			{
				CD_fread( buffer+load_pos,to_end_of_buffer);
				amount_needed-=to_end_of_buffer;
				load_pos=0;
			}
			CD_fread(buffer+load_pos,amount_needed);
			load_pos+=amount_needed;
		
			// Copy buffer data to SA
		
			amount_needed=s+4;
			amount_played+=amount_needed;
			to_end_of_buffer=buffer_size-frame_pos;
		
			if( amount_needed>to_end_of_buffer )
			{
				memcpy( proc_bp+4,buffer+frame_pos,to_end_of_buffer );
				amount_needed-=to_end_of_buffer;
				frame_pos=0;
			}
			else to_end_of_buffer=0;
			memcpy(proc_bp+4+to_end_of_buffer,buffer+frame_pos,amount_needed);
			frame_pos+=amount_needed;
		

			filep=proc_bp+4+s;
			s=read_long_from_disk(s);
			if( s!= -1 && cd_error==0)		// get length of next frame
			{			
 				DONESPRITES=0;
				pos_error=code_webdecomp( (void *)screen_buffer, proc_bp+4);
				//if (first_frame==2) zoom=0, fade_type=2;
 				if (pos_error==0) DONESPRITES=1;
				else cd_error=1;
				//if (first_frame==seq_ptr[seq])
				//{
					//SND_CtrlDirMidi(0x00, 0x00, 0x01,  0x00, seq_ptr[seq+1], 127);
					seq+=2;
				//]}
				first_frame++;
			}
			else break;
			
			//WaitforVB();
			slSynch();
			
			//if(pad0 & PADABUT) cd_error=1;

			if (blip_num==0 && first_frame==236)
			{
				for (j=0;j<100;j++)
				{
					slSynch();
				}
				//WaitforVB();
			}
  		}

		if (cd_error!=0)
		{
			//stop_all_the_game_sound_samples();
		}

		CDC_GetCurStat(&cdstat);
		if(cdstat.status==CDC_ST_OPEN) SYS_EXECDMP();
		GFS_Close(gfs);
		break;

	}

	blip_num++;
//	return OK;

}

/******************************************************************/

void CD_fread(char *dest,long length)
{
	Sint32 	stat;
	int 		error;

	if (cd_error==0)
	{
		error=0;
		if (total_sect+(length/2048) > nsct	) length=(nsct-total_sect)*2048;

		if(length<=0) return;

		stat=GFS_Fread(gfs,length/2048,dest,length);

		total_sect+=length/2048;

		if (stat<0) cd_error=1;
		//return OK;
	}
} 

/******************************************************************/

void	AnimVblank(void)
{
	
	VBLcount++;

	if(DONESPRITES==1)
	{
		slDMACopy(screen_buffer,(void *)SpriteVRAM+CGADDRESS,64000);
	}

}

/******************************************************************/

long	read_long_from_disk( int fp )
{
	long	retval;

	retval=reverse_long(filep,1);
	filep+=4;
	return retval;
}

/******************************************************************/

#ifdef	TANX

void	InitPalette(Uint16 *palette,Uint8 bankno)
{
	Uint16	x;
	volatile	Uint16	*CRAM	=	(Uint16 *)VDP2_COLRAM;

	if(bankno>3)
	{
		bankno	=	3;
	}

	CRAM	+=	(bankno * 256);	//Number of words to offset to the correct bank

	for(x=0;x<256;x++)
	{
		*(CRAM+x)	=	palette[x];
	}
}

#endif

/******************************************************************/
