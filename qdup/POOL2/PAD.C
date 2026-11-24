#include	"globals.h"

//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
// RESTORE: BUG FIX (GAME.C Show_potted_balls). Every button pressed since the end-of-frame screen began, as the
// PC's keyboard buffer kept a key struck while the replay was busy elsewhere; GAME.C clears it.
u_long	PadPressed;

void get_pad()
{
#ifdef	PSX
	lastpad = PadDataIn;

	joy1_is_left = joy1_is_right = joy1_is_up = joy1_is_down = NO;
	joy2_is_left = joy2_is_right = joy2_is_up = joy2_is_down = NO;

	if(GoodData(&pad1) && GetType(&pad1)==STD_PAD)
		{
		PadDataIn = (pad1.data.pad ^ 0xffff);
		if(PadDataIn & PAD_LL) joy1_is_left = YES;
		if(PadDataIn & PAD_LR) joy1_is_right = YES;
		if(PadDataIn & PAD_LU) joy1_is_up = YES;
		if(PadDataIn & PAD_LD) joy1_is_down = YES;

		if(PadDataIn & PAD_RL) joy2_is_left = YES;
		if(PadDataIn & PAD_RR) joy2_is_right = YES;
		if(PadDataIn & PAD_RU) joy2_is_up = YES;
		if(PadDataIn & PAD_RD) joy2_is_down = YES;

		}

//	if(ACTUAL_player&1)
//		{
//		if(GoodData(&pad2) && GetType(&pad2)==STD_PAD)
//			{
//			PadDataIn = (pad2.data.pad ^ 0xffff);
//			}
//		}
//
	PadBounce = ((PadDataIn&lastpad)^PadDataIn);
	PadPressed |= PadBounce;	// RESTORE: see PadPressed above
#else
	Uint16 data;

	if(!Per_Connect1) return;
	data = Smpc_Peripheral[0].data;

#endif
}

//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл

flag	getkey( int key )
{
	flag	retval = NO;

	switch(key)
		{
		case	kM:
				if(PadDataIn & PAD_RU) retval = YES;
				break;
				
		case	kS:
				if(PadDataIn & PAD_RD) retval = YES;
				break;
				
		case	kB:
				if(PadDataIn & PAD_RL) retval = YES;
				break;
				
		case	kE:
				if(PadDataIn & PAD_RR) retval = YES;
				break;
				
		case	kSPACE:
				if(PadDataIn & PAD_SEL) retval = YES;
				break;
				
		case	kLEFTSHIFT:
				if(PadDataIn & PAD_TL1) retval = YES;
				break;
				
		case	kRIGHTSHIFT:
				if(PadDataIn & PAD_TR1) retval = YES;
				break;

// RESTORE: NMS's keys R, U, Z and P (replay, undo the shot, skip to the end of
// the shot, pause; GAME.C) were read from the PC keyboard and mapped to
// nothing on the pad.  The owner's mapping, on the buttons the PS1 code never
// reads: Start is P; L2 is U; R2 is R (a new press, so a R2 held through a
// skip does not start a replay) and Z (balls rolling).  L2 and R2 together
// do neither: the 3D build's 2D view toggle (GFX.C) uses that.
		case	kP:
				if(PadDataIn & PAD_START) retval = YES;
				break;

		case	kU:
				if((PadDataIn & PAD_TL2) && !(PadDataIn & PAD_TR2)) retval = YES;
				break;

		case	kZ:
				if((PadDataIn & PAD_TR2) && !(PadDataIn & PAD_TL2)) retval = YES;
				break;

		case	kR:
				if((PadBounce & PAD_TR2) && !(PadDataIn & PAD_TL2)) retval = YES;
				break;
				
		default:
				break;
		}
	return(retval);
}

//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл

flag	getkey_jp( int key )
{
	flag	retval = NO;

	switch(key)
		{
		case	kM:
				if(PadBounce & PAD_RU) retval = YES;
				break;
				
		case	kS:
				if(PadBounce & PAD_RD) retval = YES;
				break;
				
		case	kB:
				if(PadBounce & PAD_RL) retval = YES;
				break;
				
		case	kE:
				if(PadBounce & PAD_RR) retval = YES;
				break;
				
		case	kSPACE:
				if(PadBounce & PAD_SEL) retval = YES;
				break;
				
		case	kLEFTSHIFT:
				if(PadBounce & PAD_TL1) retval = YES;
				break;
				
		case	kRIGHTSHIFT:
				if(PadBounce & PAD_TR1) retval = YES;
				break;

		case	kP:		// RESTORE: see getkey
				if(PadBounce & PAD_START) retval = YES;
				break;

		case	kU:
				if((PadBounce & PAD_TL2) && !(PadDataIn & PAD_TR2)) retval = YES;
				break;

		case	kZ:
		case	kR:
				if((PadBounce & PAD_TR2) && !(PadDataIn & PAD_TL2)) retval = YES;
				break;
				
		default:
				break;
		}
	return(retval);
}

//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл

void	update_mouse_xy( void )
{
	mouse_xdiff = mouse_ydiff  = 0;
}
