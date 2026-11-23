ext	flag	mouse_connected;
ext	long	mouse_movements;
ext	short last_mouse_x;
ext	short last_mouse_y;
ext	short mouse_x;
ext	short mouse_y;
ext	short mouse_dx;
ext	short mouse_dy;
ext	short	clicked_mouse;
ext	short	clicked_mouse_x;
ext	short	clicked_mouse_y;
ext	short clicked_buttons;
ext	flag 	clicked_left_button;
ext	flag 	clicked_right_button;
ext	flag 	clicked_middle_button;
ext	short mouse_button_status;
ext	flag 	mouse_left_button;
ext	flag 	mouse_right_button;
ext	flag 	mouse_middle_button;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Mouse Library - by Phillip Trelford */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void	reset_mouse_driver( void)
{
	union REGS regs;

	mouse_movements=0;
	last_mouse_x=0;
	last_mouse_y=0;
	mouse_x=0;
	mouse_y=0;
	mouse_dx=0;
	mouse_dy=0;
	clicked_buttons=0;
	clicked_left_button=NO;
	clicked_right_button=NO;
	clicked_middle_button=NO;
	clicked_mouse=0;
	clicked_mouse_x=0;
	clicked_mouse_y=0;
	mouse_button_status=0;
	mouse_left_button=NO;
	mouse_right_button=NO;
	mouse_middle_button=NO;

	regs.w.ax = 0x00;						//	RESET DRIVER AND READ STATUS
	int386( 0x33, &regs, &regs );
	if(regs.w.ax==0) 
	{
		mouse_connected=NO;
		return;
	}

	regs.w.ax = 0x07;						//	DEFINE HORIZONTAL CURSOR RANGE
	regs.w.cx = 0;
	regs.w.dx = 640;
 	int386( 0x33, &regs, &regs );
	
	regs.w.ax = 0x08;						//	DEFINE VERTICAL CURSOR RANGE
	regs.w.cx = 0;
	regs.w.dx = 480;
 	int386( 0x33, &regs, &regs );
	
	mouse_connected=YES;
}

void return_position_and_button_status(void)
{
	long i;
	union REGS regsin,regsout;
	
	last_mouse_x=mouse_x;
	last_mouse_y=mouse_y;

	if(mouse_connected==YES)
	{
		regsin.w.ax = 0x03;						// RETURN POSITION AND BUTTON STATUS
 		int386( 0x33, &regsin, &regsout );

		mouse_x=regsout.w.cx;
		mouse_y=regsout.w.dx;
		mouse_button_status=regsout.w.bx;
		if(mouse_button_status&1) mouse_left_button=YES;
		else mouse_left_button=NO;

		if(mouse_button_status&2) mouse_right_button=YES;
		else mouse_right_button=NO;

		if(mouse_button_status&4) mouse_middle_button=YES;
		else mouse_middle_button=NO;
	}

	i=1;
	if(last_mouse_x!=mouse_x) i=0;
	if(last_mouse_y!=mouse_y) i=0;
	if(i==0) mouse_movements=0;
	else mouse_movements++;

}

void return_button_press_data(void)
{
	union REGS regsin,regsout;
	
	clicked_left_button=NO;
	clicked_right_button=NO;

	if(mouse_connected==YES)
	{
 		regsin.w.ax = 0x05;						// RETURN BUTTON PRESS DATA
	 	regsin.w.bx = 0x00;
 		int386( 0x33, &regsin, &regsout );

		clicked_buttons=regsout.w.ax;
		clicked_mouse=regsout.w.bx;
		clicked_mouse_x=regsout.w.cx;
		clicked_mouse_y=regsout.w.dx;
		if(clicked_buttons&1) clicked_left_button=YES;

		if(clicked_left_button==NO)
		{
		 	regsin.w.ax = 0x05;						// RETURN BUTTON PRESS DATA
 			regsin.w.bx = 0x01;
		 	int386( 0x33, &regsin, &regsout );
	
			clicked_buttons=regsout.w.ax;
			if(clicked_buttons&2) clicked_right_button=YES;
			clicked_mouse=regsout.w.bx;
			clicked_mouse_x=regsout.w.cx;
			clicked_mouse_y=regsout.w.dx;
		}

	}
}

void position_mouse_pointer(int x,int y)
{
	union REGS regsin,regsout;
	
	if(mouse_connected==YES)
	{
	regsin.w.ax = 0x04;						//	POSITION MOUSE CURSOR
	regsin.w.cx = x;
	regsin.w.dx = y;
 	int386( 0x33, &regsin, &regsout );
	}

	mouse_x=x;
	mouse_y=y;
}


