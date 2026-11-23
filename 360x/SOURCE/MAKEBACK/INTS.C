/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Interrupts */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include "globals.h"

#define	KEYSREPEATMAX		10
#define C_UP    1
#define C_DOWN  2
#define C_LEFT  4
#define C_RIGHT 8
#define S_BAR   16
#define SHIFT   32
#define ESCAPE  128
#define KEYINT  0x9

#define TIMER_INT	0x8

long		e0flag;
int screen_start_address=0;
long count=0;

void (__interrupt __far *oldhandler)(void);
unsigned char pressed=0,fired=0,changed=0,previous=0,opressed=0;

void	scan_for_keysjp( void )
{
	long	k;

	//recentkeypress = NO;

	for ( k=0; k<256; k++ )
		{
		if ( keys[ k ] == 128 )
			{
			if (( ++keys_repeat[ k ] ) > KEYSREPEATMAX ) keys_repeat[ k ] = KEYSREPEATMAX;

			if (( keys_jp[ k ] == 0 )&&( keys_lastjp[ k ] == 0 ))
				{
				keys_jp[ k ] = 128;
				keys_lastjp[ k ] = 128;
				//recentkeypress = YES;
				}
			else
				{
				keys_jp[ k ] = 0;
				keys_lastjp[ k ] = 128;
				}
			}
		else
			{
			keys_jp[ k ] = 0;
			keys_lastjp[ k ] = 0;
			keys_repeat[ k ] = 0;
			}
		}
}

void (__interrupt __far *prev_int_1c)();

char 	keys_buf[256];
long	keys_held;
long	keys_inptr;
long	keys_outptr;

void __interrupt __far keytest( void )
{
	long	ax,bx;
	ubyte	al;

	ax = inp( 0x60 );

	if ( ax & 255 == 0xe0 )
		{
		e0flag = 128;
		
		al=inp(0x61);
		outp(0x61,(al|0x80));
		outp(0x61,al);

		outp( 0x20, 0x20 );
		//_chain_intr(prev_int_1c);
		}
	else
		{
		bx = ( ax & 127 ) + e0flag;
		e0flag = 0;
		al = ax & 128;
		al ^= 128;
		keys[ bx ] = al;

		if(al==0)
		{
			if(keys_held<256)
			{
				keys_held++;
				keys_buf[keys_inptr++]=(char) bx;
				keys_inptr&=255;
			}
		}

		al=inp(0x61);
		outp(0x61,(al|0x80));
		outp(0x61,al);

		//_chain_intr(prev_int_1c);
		outp( 0x20, 0x20 );
		}
}

#define	KEYS2SCAN	(38+1)			/*	A-Z 0-9 /	*/

typedef	struct
{
	long	key;
	text	chr;

} fastscanstruct;

fastscanstruct	alphabetandnumbers[ KEYS2SCAN ]={

	kA,'A',	kB,'B',	kC,'C',	kD,'D',
	kE,'E',	kF,'F',	kG,'G',	kH,'H',
	kI,'I',	kJ,'J',	kK,'K',	kL,'L',
	kM,'M',	kN,'N',	kO,'O',	kP,'P',
	kQ,'Q',	kR,'R',	kS,'S',	kT,'T',
	kU,'U',	kV,'V',	kW,'W',	kX,'X',
	kY,'Y',	kZ,'Z',	k1,'1',	k2,'2',
	k3,'3',	k4,'4',	k5,'5',	k6,'6',
	k7,'7',	k8,'8',	k9,'9',	k0,'0',
	kPERIOD,'.', kSPACE,' ',
	kBACKSPACE,8
};
 						
void flush_keys(void)
{
	keys_inptr=0;
	keys_outptr=0;
	keys_held=0;
}

char get_key(void)
{
	long k;
	char key;

	key=0;
	if(keys_held>0)
	{
		keys_held--;
		for ( k=0; k<KEYS2SCAN; k++ )
		{
			if ( (alphabetandnumbers[ k ].key)==(keys_buf[keys_outptr]) )
			{
				key=alphabetandnumbers[ k ].chr;
				break;
			}
		}
		keys_outptr++;
		keys_outptr&=255;
	}
	
	return key;
}

void installkey(void)
{
	prev_int_1c = _dos_getvect( 0x09 );
	oldhandler=_dos_getvect(KEYINT);
	_dos_setvect(KEYINT,keytest);
}

void clearkey(void)
{
	_dos_setvect(KEYINT,oldhandler);
}

/*
void (__interrupt __far *oldtimer)(void);

void __interrupt __far timer(void)
{
	static char int_count=0;
	
	int_count++;
	int_count%=4;
	count++;
	fastremappalette(0,63,63,63);
	set_screen_start_address(screen_start_address);
	TestZ();
	load_timer();
	pan4(screen_start_address);
	test_b();
	fastremappalette(0,0,0,0);
		
	if (int_count==0) {
		_chain_intr(oldtimer);
	}
	else {
		outp(0x20,0x20);
	}
}

void installtimer(void)
{
	oldtimer=_dos_getvect(TIMER_INT);
	_dos_setvect(TIMER_INT,timer);
}

void cleartimer(void)
{
	_dos_setvect(TIMER_INT,oldtimer);
	reset_timer();
}
*/



