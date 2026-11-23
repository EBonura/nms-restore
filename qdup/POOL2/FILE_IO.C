#include	"globals.h"

/******************************************************************/
// RESTORE: QD Up loaded everything over the dev kit's PC link.  A disc build
// (built with CDMASTER=1) reads the same files from the CD through libcd,
// the way NMS's 360X does it (FILES.C: cdmaster, CdSearchFile, CdControlB
// CdlSeekL, CdRead, CdReadSync); a dev kit build keeps the PC link.
#ifndef	CDMASTER
#define	CDMASTER	NO
#endif
flag	cdmaster = CDMASTER;			// NO = PC link, YES = CD

static	u_char	cd_result[8];
static	long	cd_last_length;

// The disc keeps the game's files in one file, QDUP.DAT, read through an
// index, as 360X's data file does (FILES.C FileInfo, tools/x360_dat.py; QD
// Up's is made by tools/qdup_dat.py, whose header has the format).  One file
// keeps to the disc's root directory, which is one ISO sector (about forty
// names), and the names carry the last folder because the Crypt's BRIGHT.TAB,
// GRRR.PAL and POOLPAL.TAB share their names with the Potters folder's
// different files: the PC-link name ("gfx/front.tga",
// "tables\\potters\\potterus.esh") becomes "GFX\FRONT.TGA",
// "POTTERS\POTTERUS.ESH"; a name without a folder stays as it is.  (A name the
// DAT does not hold under its folder is also looked up by its file name.)
static	u_long	dat_index[1024];		// the index: sectors 0 and 1
static	long	dat_first = -1;			// the DAT's first sector, -1 until read

static	void	dat_key(text *name, text *key)
{
	text	*base = name, *dir = name, c;
	int	n = 0;

	while((c = *name++) != 0) if((c == '/') || (c == '\\'))
		{
		dir = base;
		base = name;
		}
	if(base != dir)
		{
		for(name = dir; (name < base-1) && (n < 60); name++)
			{
			c = *name;
			if((c >= 'a') && (c <= 'z')) c -= 'a'-'A';
			key[n++] = c;
			}
		if(n > 0) key[n++] = '\\';
		}
	while(((c = *base++) != 0) && (n < 120))
		{
		if((c >= 'a') && (c <= 'z')) c -= 'a'-'A';
		key[n++] = c;
		}
	key[n] = 0;
}

// the first sector (from the DAT's start) and size of a file, 0 when absent
static	int	dat_lookup(text *name, long *sector, long *size)
{
	text	key[128], *ent, *s;
	long	count, i;
	CdlFILE	fp;

	// RESTORE: BUG FIX. A failed read of the index was a missing file and every load after it failed in silence; the disc always
	// holds QDUP.DAT, so not finding it or not reading its index is a drive failure, and the game stops on a screen that says so
	// (the layer tried each read five times first).
	if(dat_first < 0)
		{
		if(CdSearchFile(&fp, "\\QDUP.DAT;1") == 0) PsyqLoadFailed("QDUP.DAT", "LOOKUP");
		if(!CdControlB(CdlSeekL, (u_char *)&fp.pos, 0)) PsyqLoadFailed("QDUP.DAT", "SEEK");
		if(!CdRead(2, dat_index, CdlModeSpeed) || (CdReadSync(0, cd_result) < 0)) PsyqLoadFailed("QDUP.DAT", "INDEX");
		dat_first = CdPosToInt(&fp.pos);
		}
	dat_key(name, key);
	count = *(long *)dat_index;
	ent = (text *)dat_index + 4;
	for(s = key; *s != 0; s++) if(*s == '\\') break;
	for(i=0;i<count;i++, ent+=32)
		{
		if(strcmp(ent, key) == 0) break;
		}
	if(i == count)
		{
		text	*base = (*s == '\\') ? s+1 : key;

		ent = (text *)dat_index + 4;
		for(i=0;i<count;i++, ent+=32)
			{
			for(s = ent; *s != 0; s++) if(*s == '\\') break;
			if(strcmp((*s == '\\') ? s+1 : ent, base) == 0) break;
			}
		if(i == count) return(0);
		}
	*sector = *(long *)(ent + 24);
	*size = *(long *)(ent + 28);
	return(1);
}

void	InitFileSystem(void)
{
	if(cdmaster == YES)				// RESTORE
		{
		CdInit();
		return;
		}
	PCinit();
}

/******************************************************************/

void	ReadFile(char *name,char *target)
{
	Uint32	len;
	Sint16	handle;
	char		stat_str[80];
	long		fsize;

	handle	=	PCopen(name,0,0);

	if(handle!=-1)
	{
		len = PClseek(handle,0,2);

		fsize	=	len;

		PClseek(handle,0,0);

		PCread(handle,target,len);

	}

	PCclose(handle);

	while( (fsize % 4)!=0)
	{
		fsize++;
	}

}

/******************************************************************/

ubyte	*LoadFile(text *name)
{
	long	handle;
	long	length, sector;
	ubyte	*addr;
	CdlLOC	pos;

// RESTORE: from the disc, as 360X's LoadFile: whole sectors into a buffer
// rounded up to them
	if(cdmaster == YES)
		{
		if(dat_lookup(name, &sector, &length) == 0) return FAIL;	// not on the disc (the game asks for some files it may lack)
		// RESTORE: BUG FIX. A failed read returned FAIL, which the callers took as a buffer or ignored, and CdReadSync's answer
		// was not read: the game carried on with missing data. A file that is on the disc and cannot be read, or has no room,
		// stops the game on a screen naming it (the layer tried the read five times, re-syncing the drive between tries).
		CdIntToPos(dat_first + sector, &pos);
		if(!CdControlB(CdlSeekL, (u_char *)&pos, 0)) PsyqLoadFailed(name, "SEEK");
		if((addr = Get_Ram_Ptr(((length>>11)+1)<<11)) == NULL) PsyqLoadFailed(name, "MEMORY");
		if(!CdRead((length>>11)+1, (u_long *)addr, CdlModeSpeed) || (CdReadSync(0, cd_result) < 0)) PsyqLoadFailed(name, "READ");
		*(addr+length)=0;
		cd_last_length = length;
		return(addr);
		}

	if((handle = PCopen(name,0,0)) ==-1) return FAIL;
	length = PClseek(handle,0,2);
	PClseek(handle,0,0);
	if((addr = Get_Ram_Ptr(length)) == NULL) return FAIL;
	PCread(handle,addr,length);
	PCclose(handle);
	*(addr+length)=0;
	return(addr);
}

/******************************************************************/
// RESTORE: Escher's loader (ESCHER.S OpenFile, ReadFile, CloseFile) called
// PCopen, PCread and PCclose; it calls these, which read the disc in a disc
// build: the file is loaded whole and read from memory.

// One file at a time, as the game opens them; handle 0, or -1 when the file
// is not there (as PCopen answers).

static	ubyte	*cd_file = NULL;
static	long	cd_file_pos;

long	FileOpen(text *name, long mode, long attr)
{
	if(cdmaster == NO) return(PCopen(name, mode, attr));
	if((cd_file = LoadFile(name)) == FAIL) return(-1);
	cd_file_pos = 0;
	return(0);
}

long	FileRead(long handle, ubyte *dest, long length)
{
	if(cdmaster == NO) return(PCread(handle, dest, length));
	if((handle < 0) || (cd_file == NULL)) return(-1);
	if(length > cd_last_length - cd_file_pos) length = cd_last_length - cd_file_pos;
	memcpy(dest, cd_file + cd_file_pos, length);
	cd_file_pos += length;
	return(length);
}

long	FileClose(long handle)
{
	if(cdmaster == NO) return(PCclose(handle));
	if((handle < 0) || (cd_file == NULL)) return(-1);
	Free_Ram_Ptr(cd_file);
	cd_file = NULL;
	return(0);
}

/******************************************************************/

flag load_file_here( text *fname, ubyte *dest, long length )
{
// RESTORE: 3D view: read the file into dest (all of it when length is 0),
// as the PC version's load_file_here did; the 3D build loads the room's
// palette, brightness and pool tables with it (STV3D.C init_3d).
	long	handle, got;

	if((handle = FileOpen(fname, 0, 0)) < 0) return(NO);
	got = FileRead(handle, dest, (length > 0) ? length : 0x7fffffff);
	FileClose(handle);
	return((got > 0) ? YES : NO);
}

/******************************************************************/

void	read_block_from_disk( void *dest, long length, PCDISKFP )
{
	fread( dest, 1, length, fp );
}
/******************************************************************/

long	OpenFile( text *fname)
{
	return(PCopen(fname,0,0));
}

/******************************************************************/

long	CloseFile( PCDISKFP)
{
	PCclose(fp);
}

/******************************************************************/
#ifndef	FINAL

long	OpenFileToSave( text *fname)
{
	return(PCcreat(fname,1,0));
}

/******************************************************************/

void	write_long_to_disk( int fp, long data)
{
	PCwrite( fp, &data, 4);
}
/******************************************************************/
void	write_block_to_disk( int fp, ubyte* data, long len)
{
	PCwrite(fp, data, len);
}
/******************************************************************/
#endif
/******************************************************************/
/******************************************************************/
