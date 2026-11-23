struct	{

    unsigned	LargestBlockAvail;
    unsigned	MaxUnlockedPage;
    unsigned	LargestLockablePage;
    unsigned	LinAddrSpace;
    unsigned	NumFreePagesAvail;
    unsigned	NumPhysicalPagesFree;
    unsigned	TotalPhysicalPages;
    unsigned	FreeLinAddrSpace;
    unsigned	SizeOfPageFile;
    unsigned	Reserved[3];

} meminfo;

#define	DPMI_INT	0x31

long	get_ram_available( void )
{
	union REGS regs;
    struct SREGS sregs;

    regs.x.eax = 0x00000500;
    memset( &sregs, 0, sizeof( sregs ) );
    sregs.es = FP_SEG( &meminfo );
    regs.x.edi = FP_OFF( &meminfo );

    int386x( DPMI_INT, &regs, &regs, &sregs );
	return meminfo.LargestBlockAvail;
}


