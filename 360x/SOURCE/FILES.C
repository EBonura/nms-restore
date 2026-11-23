
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE VARIABLES          *
**************************************/

LONG	mode;					/* THE MODE WORD FOR CDREAD */
UBYTE	status;					/* 0=FAIL, RETURN VALUE FROM READING FROM CD */
UBYTE	result[8];				/* RETURN VALUES FOR ACCESSING CD */

LONG	lastfilelength;			/* LENGTH OF THE LAST FILE */

LONG	startdatsector;			/* STARTING POINT FOR BIG .DAT FILE */
LONG	firstplusdat;				/* SECTOR NUMBER OF .DAT + FIRST SECTOR */

LONG	firstsector;				/* WHERE TO LOAD .DAT INDEX FILE */
LONG	numberofbytes;				/* NUMBER OF BYTES TO READ */

UBYTE	indexbuffer[16384];			/* 512 ENTRY INDEX FOR .DAT FILE */

/**************************************
*   LOADFILE TO AREA - FROM REAL CD   *
**************************************/

UBYTE 
LoadHere(BYTE* name,void *buffer)
{

	mode=CdlModeSpeed;				/* MAKE UP THE CD MODE WORD (JUST DOUBLE SPEED FLAG) */

	status=FileInfo(indexbuffer,name,&firstsector,&numberofbytes);
	if (status==0)
	{
		status=123;				/* CREATE ERROR MESSAGE NUMBER */
		return 0;					/* UNLIKELY TO BE COINCIDENCE IS IT? */
	}

	firstplusdat=firstsector+startdatsector;

	CdIntToPos(firstplusdat,&fp.pos);

	/* RESTORE: BUG FIX. A FAILED SEEK OR READ RETURNED 0, WHICH NO CALLER CHECKED, AND CdReadSync'S ANSWER WAS NOT READ: THE GAME
	   CARRIED ON WITH MISSING DATA. NOW IT STOPS ON A SCREEN NAMING THE FILE (THE LAYER TRIED THE READ FIVE TIMES, RE-SYNCING THE
	   DRIVE BETWEEN TRIES). A FILE NOT IN THE INDEX STILL RETURNS 0 */
	if (!CdControlB(CdlSeekL,(UBYTE*)&fp.pos, 0)) PsyqLoadFailed(name,"SEEK");

    if(!CdRead((numberofbytes>>11)+1,buffer,mode) || (CdReadSync(0,result)<0)) PsyqLoadFailed(name,"READ");

	return TRUE;

}

/**************************************
*       LOADFILE - FROM REAL CD       *
**************************************/

UBYTE* 
LoadFile(BYTE* name)
{

UBYTE	*returnfile;				/* RETURN VALUE FOR LOAD FILE */
UBYTE*	filepoint;				/* FILE POINTER FOR HARD DRIVE LOAD */
LONG	filehandle;				/* FILE HANDLE FOR HARD DRIVE LOAD */

	if (finalmaster==NO) printf("Loading File %s...\n",name);

	if (cdmaster==NO)				/* LOAD FROM HARD DRIVE */
	{
		filehandle = PCopen(name,0,0);
		if (filehandle==-1)
		{
			printf("LOAD FAILED....");
			CRASH;
			return NULL;
		}

		lastfilelength = PClseek(filehandle,0,2);
		PClseek(filehandle,0,0);
		filepoint = MyMalloc(lastfilelength);
		if (filepoint==0)
		{
			printf("LOAD FAILED....");
			CRASH;
			return NULL;
		}

		PCread(filehandle,filepoint,lastfilelength);
		PCclose(filehandle);
		return filepoint;
	}

    mode=CdlModeSpeed;				/* MAKE UP THE CD MODE WORD (JUST DOUBLE SPEED FLAG) */

	status=FileInfo(indexbuffer,name,&firstsector,&numberofbytes);
	if (status==0)
	{
		status=123;				/* CREATE ERROR MESSAGE NUMBER */
		return 0;					/* UNLIKELY TO BE COINCIDENCE IS IT? */
	}

	firstplusdat=firstsector+startdatsector;

	CdIntToPos(firstplusdat,&fp.pos);	/* CONVERT THEM TO SONY FORMAT */

	/* RESTORE: BUG FIX, AS LoadHere: A FILE IN THE INDEX THAT CANNOT BE READ, OR HAS NO ROOM, STOPS THE GAME ON A SCREEN NAMING IT */
	if (!CdControlB(CdlSeekL,(UBYTE*)&fp.pos, 0)) PsyqLoadFailed(name,"SEEK");

	if ((numberofbytes>0) && (numberofbytes<2048))
	{
		/* RESTORE: platform (2 MB). A file of less than one sector took a whole sector of the heap (the rounding below), and a table of a few
		   hundred bytes, which stays loaded, is most of the small files: Mission 1 holds many. It is read into a sector of the heap that is
		   freed again, and the file's own length (the hard drive path's) is kept. Same sector, same read, one copy; nothing reads past a
		   file's end (the padding, zero on the disc, was never part of the file) */
		UBYTE	*sector;

		returnfile=(UBYTE*)MyMalloc(numberofbytes);
		sector=(UBYTE*)MyMalloc(2048);
		if ((returnfile==0) || (sector==0)) PsyqLoadFailed(name,"MEMORY");
		if(!CdRead(1,(ULONG*)sector,mode) || (CdReadSync(0,result)<0)) PsyqLoadFailed(name,"READ");
		memcpy(returnfile,sector,numberofbytes);
		MyFree(sector);
		lastfilelength=numberofbytes;
		return returnfile;
	}

	returnfile=(UBYTE*)MyMalloc(((numberofbytes>>11)+1)<<11);
	if (returnfile==0) PsyqLoadFailed(name,"MEMORY");

    if(!CdRead((numberofbytes>>11)+1,(ULONG*)returnfile,mode) || (CdReadSync(0,result)<0)) PsyqLoadFailed(name,"READ");

	lastfilelength=numberofbytes;		/* RESTORE: the length, as the PC link path sets it */
	return returnfile;

}

/**************************************
*  RESTORE: SOUND BANK BODY IN PIECES *
**************************************/

/* RESTORE: platform (2 MB). NMS loaded a sound bank's body whole and sent
   it to sound RAM with SsVabTransBody (SOUND.C). The mission's bank,
   OPTIONS.VB, is 175,472 bytes, and Mission 1 leaves 63 KB of the heap
   free when MAIN.C starts the sound; NMS's dev kit had 8 MB. This reads the
   file 8 KB at a time, the way LoadFile reads a whole one, and sends each
   piece on with SsVabTransBodyPartly. Returns vabid when the body is in
   sound RAM, as SsVabTransBody does, else -1. */

#define	VABPIECE	(4*2048)			/* RESTORE: BYTES PER PIECE, WHOLE SECTORS (8 KB: WAS 16 KB, FOR THE HEAP MARGIN OF MISSION 1, docs/DECISIONS.md) */

SHORT
Stream_Vab_Body(BYTE* name,SHORT vabid)
{

UBYTE	*buffer;					/* ONE PIECE OF THE FILE */
LONG	left;						/* BYTES STILL TO SEND */
LONG	piece;					/* BYTES IN THIS PIECE */
LONG	sector;					/* NEXT SECTOR TO READ */
LONG	filehandle;				/* FILE HANDLE FOR HARD DRIVE LOAD */
SHORT	ret;

	if (finalmaster==NO) printf("Streaming File %s...\n",name);

	buffer=(UBYTE*)MyMalloc(VABPIECE);
	if (buffer==0) return -1;
	ret=-1;

	if (cdmaster==NO)				/* LOAD FROM HARD DRIVE */
	{
		filehandle = PCopen(name,0,0);
		if (filehandle==-1)
		{
			MyFree(buffer);
			return -1;
		}
		left = PClseek(filehandle,0,2);
		PClseek(filehandle,0,0);
		while (left>0)
		{
			piece=(left>VABPIECE) ? VABPIECE : left;
			PCread(filehandle,buffer,piece);
			ret=SsVabTransBodyPartly(buffer,piece,vabid);
			left-=piece;
		}
		PCclose(filehandle);
	}
	else
	{
		mode=CdlModeSpeed;			/* MAKE UP THE CD MODE WORD (JUST DOUBLE SPEED FLAG) */
		if (FileInfo(indexbuffer,name,&firstsector,&numberofbytes)!=0)
		{
			sector=firstsector+startdatsector;
			left=numberofbytes;
			while (left>0)
			{
				piece=(left>VABPIECE) ? VABPIECE : left;
				CdIntToPos(sector,&fp.pos);
				/* RESTORE: BUG FIX: a piece that cannot be read stops the game on a screen naming the file, as LoadFile */
				if (!CdControlB(CdlSeekL,(UBYTE*)&fp.pos, 0)) PsyqLoadFailed(name,"SEEK");
				if (!CdRead((piece+2047)>>11,(ULONG*)buffer,mode) || (CdReadSync(0,result)<0)) PsyqLoadFailed(name,"READ");
				ret=SsVabTransBodyPartly(buffer,piece,vabid);
				sector+=VABPIECE>>11;
				left-=piece;
			}
		}
	}

	MyFree(buffer);
	return ret;

}

/**************************************
*  GET FILE INFO FOR REQUESTED FILE   *
**************************************/

LONG 
FileInfo(UBYTE *index,BYTE *filespec,LONG *firstsec,LONG *size)
{

LONG	noof, min,max, pos, ret;
BYTE	*cp, *ep;

	noof=*(LONG*)index;
	index+=4;

	cp=(BYTE*)strrchr((BYTE*)filespec, '/');
	if (cp==NULL) cp=filespec;
	else cp++;

	min=0;
	max=noof-1;

	for(;;)
	{
		pos=(min+max)>>1;
		ep=index+(pos<<5);
		ret=strcmp(ep, cp);
		if (ret==0) break;
		if (min==max) return 0;
		if (ret<0) min=(min!=pos) ? pos:pos+1;
		else max=pos;
	}

	*firstsec=*(LONG*)(ep+16);
	*size=*(LONG*)(ep+20);

	return 1;
}

/**************************************
*  INITIALISE CD AND SORT DIRECTORY   *
**************************************/

UBYTE 
InitialiseFiles(void)
{

UBYTE	returnvalue;				/* RETURN VALUE FOR INITIALISE */
ULONG	error;					/* ERROR VALUE FOR INITIALISE CD */

	if (cdmaster==NO)				/* CHECK FOR CD MASTER */
	{
		error = PCinit();
		return error;
	}

    CdInit();						/* INITIALISE MAIN CD UNIT */

	if (finalmaster==NO) printf("PASSED CdInit();\n");

	returnvalue=InitCD(indexbuffer,"\\360X.DAT;1");
	if (finalmaster==NO) printf("PASSED InitCD(indexbuffer,360X.DAT;1);\n");

	return returnvalue;

}

/**************************************
*    INITIALISE CD - GET DIRECTORY    *
**************************************/

UBYTE 
InitCD(BYTE *indexbuffer,BYTE *filename)
{

LONG	numSectors;				/* NUMBER OF SECTORS THE FILE TAKES UP */

	mode=CdlModeSpeed;				/* MAKE UP THE CD MODE WORD (JUST DOUBLE SPEED FLAG) */

	/* RESTORE: BUG FIX. THE DISC ALWAYS HOLDS 360X.DAT: NOT FINDING IT OR NOT READING ITS INDEX IS A DRIVE FAILURE, AND EVERY LOAD
	   AFTER IT FAILED IN SILENCE. IT STOPS THE GAME ON A SCREEN THAT SAYS SO (THE LAYER TRIED EACH READ FIVE TIMES FIRST) */
	if (!CdSearchFile(&fp,filename))	/* FIND THE STREAM FILE ON CD */
		PsyqLoadFailed(filename,"LOOKUP");
    lastfilelength=fp.size;			/* GET THE LENGTH OF THE FILE */

	startdatsector=CdPosToInt(&fp.pos);

	if (!CdControlB(CdlSeekL,(UBYTE*)&fp.pos, 0))
		PsyqLoadFailed(filename,"SEEK");	/* CALL CD CONTROL (BLOCKING) TO SEEK TO THE LOGICAL FILE POSITION */

    numSectors=8;					/* READ IN 8 SECTORS (DIRECTORY STRUCTURE) */

    if(!CdRead(numSectors,(ULONG*)indexbuffer,mode) || (CdReadSync(0,result)<0))
      PsyqLoadFailed(filename,"INDEX");	/* CALL CDREAD() TO READ THE SECTORS IN THE BUFFER AT DOUBLE SPEED */

    return lastfilelength;			/* RETURN THE LENGTH OF THE FILE */

}

