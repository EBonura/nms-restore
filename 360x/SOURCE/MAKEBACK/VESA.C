/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* VESA library*/
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Matrox uses 32-bits in 24-bit mode
// Cirrus uses 24-bits in 24-bit mode

// Missing function flag SVGA_mode_available(long mode)
// VESA Version number support would be nice...
// Do we need to go through DPMI to call 4F05 (probably not)...
// as no variables are passed...
// This could speed things up a tad...

/*
INT 31 P - DPMI 0.9+ - CALL REAL MODE PROCEDURE WITH FAR RETURN FRAME
	AX = 0301h
	BH = flags
	    bit 0: reset the interrupt controller and A20 line (DPMI 0.9)
		   reserved, must be 0 (DPMI 1.0+)
	    others must be 0
	CX = number of words to copy from protected mode to real mode stack
	ES:(E)DI = selector:offset of real mode call structure
		  (see INT 31/AX=0300h)
Return: CF clear if successful
	    real mode call structure modified (all fields except SS:SP, CS:IP
	      filled with return values from real mode interrupt)
	CF set on error
	    AX = error code (DPMI 1.0+) (8012h,8013h,8014h,8021h)(see AX=0000h)
	protected mode stack unchanged
Notes:	16-bit programs use ES:DI as pointer, 32-bit programs use ES:EDI
	the real mode procedure must exit with a FAR return
	DPMI will provide a small (30 words) real mode stack if SS:SP is zero
	the real mode handler must return with the stack in the same state as
	  it was on being called
SeeAlso: AX=0300h,AX=0302h,INT 21/AX=250Eh,INT 21/AH=E1h"OS/286"
SeeAlso: INT 2C/AX=0025h
*/
														  
#include "globals.h"

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* VESA Variables */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

static struct rminfo {
	long	EDI;
	long	ESI;
	long	EBP;
	long	reserved_by_system;
	long	EBX;
	long	EDX;
	long	ECX;
	long	EAX;
	short	flags;
	uword	ES,DS,FS,GS,IP,CS,SP,SS;
} RMI;


flag	VESA_waits_for_VRT;
long	VESA_segment;
long	VESA_selector;
long	VESA_mode_segment;
long	VESA_mode_selector;
uword	*VESA_modelist_ptr;
long	VESA_wingran;							//	VESA GRANULARITY IN KBYTES
uword	WinFuncOff;
uword	WinFuncSeg;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* VESA initialisation */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
/*
INT 10 - VESA SuperVGA BIOS - GET SuperVGA INFORMATION
	AX = 4F00h
	ES:DI -> 256-byte buffer for SuperVGA information (see below)
Return: AL = 4Fh if function supported
	AH = status
	    00h successful
		ES:DI buffer filled
	    01h failed
Desc:	determine whether VESA BIOS extensions are present and the capabilities
	  supported by the display adapter
SeeAlso: AX=4E00h,AX=4F01h,AX=7F00h
Index:	installation check;VESA SuperVGA

Format of SuperVGA information:
Offset	Size	Description
 00h  4 BYTEs	signature ("VESA")
 04h	WORD	VESA version number
 06h	DWORD	pointer to OEM name
		"761295520" for ATI
 0Ah  4 BYTEs	capabilities
 0Eh	DWORD	pointer to list of supported VESA and OEM video modes
		(list of words terminated with FFFFh)
 12h	WORD	total amount of video memory in 64K blocks
 14h 236 BYTEs	reserved
Notes:	the list of supported video modes is stored in the reserved portion of
	  the SuperVGA information record by some implementations, and it may
	  thus be necessary to either copy the mode list or use a different
	  buffer for all subsequent VESA calls
	the 1.1 VESA document specifies 242 reserved bytes at the end, so the
	  buffer should be 262 bytes to ensure that it is not overrun
*/

flag VESA_init(void )
{
	ubyte			*p, buf[ 512 ];
	char far		*str;
	uword			*sip, seg, off;
	unsigned long	l;

	union REGS regs;
	struct SREGS sregs;

	VESA_waits_for_VRT=FALSE;

	memset( &sregs, 0, sizeof( sregs ));
	regs.w.ax = 0x0100;						// Allocate DOS Memory			
	regs.w.bx = 0x0020;						// 512 bytes
	int386x( DPMI_INT , &regs, &regs, &sregs);
	if( regs.w.bx < 0x0020 )	return FAIL;	// error
	VESA_mode_segment = regs.w.ax;
	VESA_mode_selector = regs.w.dx;

	regs.w.ax = 0x0100;						// Allocate DOS Memory			
	regs.w.bx = 0x0020;						// 512 bytes
	int386x( DPMI_INT , &regs, &regs, &sregs);
	if( regs.w.bx < 0x0020 )	return FAIL;	// error
	VESA_segment = regs.w.ax;
	VESA_selector = regs.w.dx;

	memset( &RMI, 0, sizeof( RMI ));
	RMI.EAX = 0x00004f00;	 			//	GET SuperVGA INFORMATION
	RMI.EBX = 0;
	RMI.ES = VESA_mode_segment; 		 			//	put DOS seg:off into DS:DX
	RMI.EDI = 0;		  	 				//	DOS ignores EDX high word

	regs.w.ax = 0x0300;					// DPMI Issue DOS Int
	regs.h.bl = 0x10;						// VESA SuperVGA BIOS
	regs.h.bh = 0;
	regs.w.cx = 0;
	sregs.es = FP_SEG(&RMI);
	regs.x.edi = FP_OFF(&RMI);
	int386x( DPMI_INT, &regs, &regs, &sregs );
	if( RMI.EAX != 0x004f )	return FAIL;

	str=MK_FP(VESA_mode_selector,0);		// Retrieve Data from DOS memory
	_fmemcpy( buf, str, 256 );

	// Get VESA Version out here perchance

	buf[ 4 ] = 0;
	if ( strcmp( buf, "VESA" ) ) return FAIL;

	p = buf + 14;
	sip = (uword *)p;
	off = *sip;
	sip++;
	seg = *sip;
	l = (unsigned long) seg << 4;
	l += (unsigned long) off;
	VESA_modelist_ptr=(uword *) l;

	return TRUE;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Is VESA mode available? */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
/*
INT 10 - VESA SuperVGA BIOS - GET SuperVGA MODE INFORMATION
	AX = 4F01h
	CX = SuperVGA video mode
	ES:DI -> 256-byte buffer for mode information (see below)
Return: AL = 4Fh function supported
	AH = status
	    00h successful
		ES:DI buffer filled
	    01h failed
Desc:	determine the attributes of the specified video mode
SeeAlso: AX=4F00h,AX=4F02h

Format of mode information:
Offset	Size	Description
 00h	WORD	mode attributes (see below)
 02h	BYTE	window attributes, window A (see below)
 03h	BYTE	window attributes, window B (see below)
 04h	WORD	window granularity in KB
 06h	WORD	window size in KB
 08h	WORD	start segment of window A
 0Ah	WORD	start segment of window B
 0Ch	DWORD	-> FAR window positioning function (equivalent to AX=4F05h)
 10h	WORD	bytes per scan line
---remainder is optional for VESA modes in v1.0/1.1, needed for OEM modes---
 12h	WORD	width in pixels (graphics) or characters (text)
 14h	WORD	height in pixels (graphics) or characters (text)
 16h	BYTE	width of character cell in pixels
 17h	BYTE	height of character cell in pixels
 18h	BYTE	number of memory planes
 19h	BYTE	number of bits per pixel
 1Ah	BYTE	number of banks
 1Bh	BYTE	memory model type (see below)
 1Ch	BYTE	size of bank in KB
 1Dh	BYTE	number of image pages
 1Eh	BYTE	reserved (0)
---VBE v1.2+---
 1Fh	BYTE	red mask size
 20h	BYTE	red field position
 21h	BYTE	green mask size
 22h	BYTE	green field size
 23h	BYTE	blue mask size
 24h	BYTE	blue field size
 25h	BYTE	reserved mask size
 26h	BYTE	reserved mask position
 27h	BYTE	direct color mode info
(28h 216 BYTEs	reserved (0) )
---VBE v2.0---
 28h	unsigned long	PhysBasePtr;			// Physical address for linear buf	
 2Ch	unsigned long	OffScreenMemOffset;	// Ptr to start of offscreen mem
 30h	short   OffScreenMemSize;				// Amount of offscreen mem in 1K's	
 32h	char    res2[206];        				// Pad to 256 byte block size       

Bitfields for window attributes:
Bit(s)	Description
 0	exists
 1	readable
 2	writable
 3-7	reserved

Bitfields for mode attributes:
Bit(s)	Description
 0	mode supported
 1	optional information available
 2	BIOS output supported
 3	set if color, clear if monochrome
 4	set if graphics mode, clear if text mode

Values for memory model type:
 00h	text
 01h	CGA graphics
 02h	HGC graphics
 03h	16-color (EGA) graphics
 04h	packed pixel graphics
 05h	"sequ 256" (non-chain 4) graphics
 06h	direct color (HiColor, 24-bit color)
 07h	YUV (luminance-chrominance, also called YIQ)
 08h-0Fh reserved for VESA
 10h-FFh OEM memory models
*/

flag  VESA_is_mode_available(long mode )
{
	uword			*sip;
	union REGS regs;
	struct SREGS sregs;

	memset( &sregs, 0, sizeof( sregs ));
	sip = VESA_modelist_ptr;
	while ( *sip != 0xffff )
	{
		if ( *sip == mode )
		{
			memset( &RMI, 0, sizeof( RMI ));
			RMI.EAX = 0x00004f01;				//	GET SuperVGA MODE INFORMATION
			RMI.ECX = *sip;						// SuperVGA video mode
			RMI.ES  = VESA_segment;				// ES:DI -> 256-byte buffer for mode information
			RMI.EDI = 0;						

			regs.w.ax = 0x0300;					// DPMI Issue DOS Int
			regs.h.bl = 0x10;
			regs.h.bh = 0;
			regs.w.cx = 0;
			sregs.es = FP_SEG( &RMI );
			regs.x.edi = FP_OFF( &RMI );
			int386x( DPMI_INT, &regs, &regs, &sregs );
			if( (RMI.EAX&0xff00) == 0 ) return YES;
			else 					 			 return NO;
		}
		sip++;
	}
	return NO;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* VESA debug */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void	VESA_scan_modes( void )
{
	ubyte			buf[ 512 ];
	char far		*str;
	uword			*sip,*dip;
	union REGS regs;
	struct SREGS sregs;

	memset( &sregs, 0, sizeof( sregs ));
	sip = VESA_modelist_ptr;
	while ( *sip != 0xffff )
	{
		memset( &RMI, 0, sizeof( RMI ));
		RMI.EAX = 0x00004f01;				//	GET SuperVGA MODE INFORMATION
		RMI.ECX = *sip;						// SuperVGA video mode
		RMI.ES  = VESA_segment;				// ES:DI -> 256-byte buffer for mode information
		RMI.EDI = 0;						

		regs.w.ax = 0x0300;					// DPMI Issue DOS Int
		regs.h.bl = 0x10;
		regs.h.bh = 0;
		regs.w.cx = 0;
		sregs.es = FP_SEG( &RMI );
		regs.x.edi = FP_OFF( &RMI );
		int386x( DPMI_INT, &regs, &regs, &sregs );
		str=MK_FP( VESA_selector ,0 );	// Retrieve Data from DOS memory
		_fmemcpy( buf, str, 29 );

		dip = ( uword* )buf;
		printf("Mode %i,Width %i,Height %i\n",*sip,dip[9],dip[10]);
		printf("Planes Bits Per Pixel %i,Model %i\n",buf[0x19],buf[0x1b]);

		sip++;
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* VESA Init Mode */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag	VESA_init_mode( long mode )
{
	long	i,temp;
	long a,b;

	if(VESA_set_mode(mode)==FAIL) return FAIL;

	a=65536;
	for(i=0;i<3;i++)
	{
		temp=asm_time_VRT();
		//printf("ATime %i\n",temp);
		if(temp<a) a=temp;
	}
	b=0;
	for(i=0;i<3;i++)
	{
		temp=asm_time_VESA_set_display_start(19000);
		//printf("BTime %i\n",temp);
		if(temp>b) b=temp;
	}
	//printf("A=%i\nB=%i\n",a,b);
	
	if(a>b) 	VESA_waits_for_VRT=FALSE;
	else		VESA_waits_for_VRT=TRUE;

	return OK;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* VESA Set Mode */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
/*
--------V-104F02-----------------------------
INT 10 - VESA SuperVGA BIOS - SET SuperVGA VIDEO MODE
	AX = 4F02h
	BX = mode
		bit 15 set means don't clear video memory
Return: AL = 4Fh function supported
	AH = status
	    00h successful
	    01h failed
SeeAlso: AX=4E03h,AX=4F01h,AX=4F03h

Values for VESA video mode:
 00h-FFh OEM video modes (see AH=00h)
 100h	640x400x256
 101h	640x480x256
 102h	800x600x16
 103h	800x600x256
 104h	1024x768x16
 105h	1024x768x256
 106h	1280x1024x16
 107h	1280x1024x256
 108h	80x60 text
 109h	132x25 text
 10Ah	132x43 text
 10Bh	132x50 text
 10Ch	132x60 text
---VBE v1.2---
 10Dh	320x200x32K
 10Eh	320x200x64K
 10Fh	320x200x16M
 110h	640x480x32K
 111h	640x480x64K
 112h	640x480x16M
 113h	800x600x32K
 114h	800x600x64K
 115h	800x600x16M
 116h	1024x768x32K
 117h	1024x768x64K
 118h	1024x768x16M
 119h	1280x1024x32K
 11Ah	1280x1024x64K
 11Bh	1280x1024x16M
Index:	video modes
*/

void asm_set_WinFunc(long seg,long off);

flag	VESA_set_mode( long mode )
{
	ubyte			buf[ 512 ];
	char far		*str;
	uword			*sip;
	union REGS regs;
	struct SREGS sregs;

	memset( &sregs, 0, sizeof( sregs ));
	sip = VESA_modelist_ptr;
	while ( *sip != 0xffff )
	{
		if ( *sip == mode )
		{
			memset( &RMI, 0, sizeof( RMI ));
			RMI.EAX = 0x00004f01;				//	GET SuperVGA MODE INFORMATION
			RMI.ECX = mode;						// SuperVGA video mode
			RMI.ES  = VESA_segment;				// ES:DI -> 256-byte buffer for mode information
			RMI.EDI = 0;						

			regs.w.ax = 0x0300;					// DPMI Issue DOS Int
			regs.h.bl = 0x10;
			regs.h.bh = 0;
			regs.w.cx = 0;
			sregs.es = FP_SEG( &RMI );
			regs.x.edi = FP_OFF( &RMI );
			int386x( DPMI_INT, &regs, &regs, &sregs );
			str=MK_FP( VESA_selector ,0 );	// Retrieve Data from DOS memory
			_fmemcpy( buf, str, 29 );

			sip = ( uword* )buf;
			VESA_wingran = (long ) sip[2];
			asm_set_vesagran(VESA_wingran);

			/*
			printf("Mode   = %04X\n",sip[0]);
			printf("Attr   = %04X\n",sip[1]);
			printf("WinSize = %04X\n",sip[3]);
			printf("WinASeg = %04X\n",sip[4]);
			printf("WinBSeg = %04X\n",sip[5]);
			printf("WinFOff = %04X\n",WinFuncOff);
			printf("WinFSeg = %04X\n",WinFuncSeg);
			printf("bytes per scanline = %i\n",sip[8]);
			*/
			WinFuncOff=sip[6];
			WinFuncSeg=sip[7];
			asm_set_WinFunc(WinFuncSeg,WinFuncOff);
			SVGA_logical_length=sip[8];
			//printf("Logical Length = %i\n",SVGA_logical_length);
	
			/*
			if(SVGA_logical_length==640)
			{
				if(VESA_set_logical_scan_line_length(800)==OK)
				{
					printf("New Logical Length = %i\n",SVGA_logical_length);
					SVGA_logical_length=800;
				}
				else printf("Set Scan Line Length did not work\n");
			}
			*/

			memset( &RMI, 0, sizeof( RMI ));
			RMI.EAX = 0x00004f02;		 		//	SET SuperVGA VIDEO MODE
			RMI.EBX = mode;

			regs.w.ax = 0x0300;
			regs.h.bl = 0x10;						// VESA SuperVGA BIOS
			regs.h.bh = 0;
			regs.w.cx = 0;
			sregs.es = FP_SEG( &RMI );
			regs.x.edi = FP_OFF( &RMI );
			int386x( DPMI_INT, &regs, &regs, &sregs );
			if( (RMI.EAX&0xff00) == 0 ) return OK;
			else 					 			 return FAIL;
		}
		sip++;
	}
	return FAIL;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* VESA Set Logical Scan Line Length */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
/*
INT 10 - VESA SuperVGA BIOS v1.1+ - GET/SET LOGICAL SCAN LINE LENGTH
	AX = 4F06h
	BL = function
	    00h set scan line length
		CX = desired width in pixels
	    01h get scan line length
Return: AL = 4Fh if function supported
	AH = status
	    00h successful
	    01h failed
	BX = bytes per scan line
	CX = number of pixels per scan line
	DX = maximum number of scan lines
Notes:	if the desired width is not achievable, the next larger width will be
	  set
	the scan line may be wider than the visible area of the screen
	this function is valid in text modes, provided that values are
	  multiplied by the character cell width/height
SeeAlso: AX=4F01h,AX=4F05h,AX=4F07h
*/

flag VESA_set_logical_scan_line_length(long length)
{
	union REGS regs;
	struct SREGS sregs;

	memset( &RMI, 0, sizeof( RMI ));
	RMI.EAX = 0x00004f06;
	RMI.EBX = 0x00000000;
	RMI.ECX = length;

	memset( &sregs, 0, sizeof( sregs ));
	regs.w.ax = 0x0300;
	regs.h.bl = 0x10;
	regs.h.bh = 0;
	regs.w.cx = 0;
	sregs.es = FP_SEG( &RMI );
	regs.x.edi = FP_OFF( &RMI );
	int386x( 0x31, &regs, &regs, &sregs );

	if( (RMI.EAX&0x00ff) != 0x4f ) return FAIL;

	if( (RMI.EAX&0xff00) == 0) return OK;

	return FAIL;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* VESA Set Display Start */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
/*
INT 10 - VESA SuperVGA BIOS v1.1+ - GET/SET DISPLAY START
	AX = 4F07h
	BH = 00h (reserved)
	BL = function
	    00h set display start
		CX = leftmost displayed pixel in scan line
		DX = first displayed scan line
	    01h get display start
		Return: BH = 00h		       
			CX = leftmost displayed pixel in scan line
			DX = first displayed scan line
Return: AL = 4Fh if function supported
	AH = status
	    00h successful
	    01h failed
Note:	this function is valid in text modes, provided that values are
	  multiplied by the character cell width/height
*/

flag VESA_set_display_start(long x,long y,flag waitVRT)
{
	union REGS regs;
	struct SREGS sregs;

	memset( &RMI, 0, sizeof( RMI ));
	RMI.EAX = 0x00004f07;
	RMI.EBX = 0x00000000;
	RMI.ECX = x;
	RMI.EDX = y;

	memset( &sregs, 0, sizeof( sregs ));
	regs.w.ax = 0x0300;
	regs.h.bl = 0x10;
	regs.h.bh = 0;
	regs.w.cx = 0;
	sregs.es = FP_SEG( &RMI );
	regs.x.edi = FP_OFF( &RMI );
	int386x( 0x31, &regs, &regs, &sregs );

	if(VESA_waits_for_VRT==FALSE) wait_vbi();

	if( (RMI.EAX&0x00ff) != 0x4f ) return FAIL;

	if( (RMI.EAX&0xff00) == 0) return OK;

	return FAIL;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* VESA Close */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void VESA_close(void)
{
	union REGS regs;
	struct SREGS sregs;

	memset( &sregs, 0, sizeof( sregs ));
	regs.w.ax = 0x0101;							// Free DOS Memory Block
	regs.w.dx = VESA_mode_selector;
	sregs.es = FP_SEG( &RMI );
	regs.x.edi = FP_OFF( &RMI );
	int386x( 0x31, &regs, &regs, &sregs );
	//printf("Memory close returned %i\n",regs.w.ax);

	memset( &sregs, 0, sizeof( sregs ));
	regs.w.ax = 0x0101;							// Free DOS Memory Block
	regs.w.dx = VESA_selector;
	sregs.es = FP_SEG( &RMI );
	regs.x.edi = FP_OFF( &RMI );
	int386x( 0x31, &regs, &regs, &sregs );
	//printf("Memory close returned %i\n",regs.w.ax);

}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* VESA Set Band */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void VESA_set_band(long band)
{
	union REGS regs;
	struct SREGS sregs;

	if(use_DOS4GWPRO==YES)
	{
		memset( &RMI, 0, sizeof( RMI ));
		RMI.CS=WinFuncSeg;
		RMI.IP=WinFuncOff;

		RMI.EAX=0x4f05;
		RMI.EBX=0;
		RMI.EDX=(band*64)/VESA_wingran;

		memset( &sregs, 0, sizeof( sregs ));
		regs.w.ax = 0x0301;							// CALL REAL MODE PROCEDURE WITH FAR RETURN FRAME
		regs.h.bh = 0x0001;							// flags
		regs.w.cx = 0x0000;							// no. of words to copy
		sregs.es = FP_SEG( &RMI );
		regs.x.edi = FP_OFF( &RMI );
		int386x( 0x31, &regs, &regs, &sregs );

	//	if( (RMI.EAX&0x00ff) != 0x4f ) printf("Function not supported\n");
	//	if( (RMI.EAX&0xff00) == 0) printf("Function call executed OK\n");
	}
	else
	{
	 	memset( &RMI, 0, sizeof( RMI ));
		RMI.EAX = 0x00004f05;						//	call service 39h ah=0x39
		RMI.EBX = 0x00000000;
		RMI.EDX = (band * 64) / VESA_wingran;

		memset( &sregs, 0, sizeof( sregs ));
		regs.w.ax = 0x0300;
		regs.h.bl = 0x10;
		regs.h.bh = 0;
		regs.w.cx = 0;
		sregs.es = FP_SEG( &RMI );
		regs.x.edi = FP_OFF( &RMI );
		int386x( 0x31, &regs, &regs, &sregs );
	}

}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* End of File */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */


