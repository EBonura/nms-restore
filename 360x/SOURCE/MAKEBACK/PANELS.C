/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Panels and map editing code*/
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include "globals.h"

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Variables for map editting */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag 	drag_mode;
long	drag_sprite_id;
long	drag_sprite_original_x;
long	drag_sprite_original_y;
long	clicked_x;
long	clicked_y;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Overlay panels positions */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

panelstruct panels[]={
	NAME_CURSOR1 ,0,0,0,	0,0,
//	NAME_BOX		 ,0,0,0,	640-96,384-96,
	NAME_BAR_L	 ,0,0,0,	640-96,64,
	NAME_BAR_R	 ,0,0,0,	640-96+64,64,

	NAME_SHIPWIRE,0,0,0,	640-56,64+4,
//	NAME_BORDSML ,0,0,0,	640-64,64,
//	NAME_BORDER1 ,0,0,0, 640-64,96,
//	NAME_BORDSML ,0,0,0,	640-64,160+64,
//	NAME_BORDSML2,0,0,0, 640-64,160+64*2,
//	NAME_BORDSML3,0,0,0, 640-64,160+64*3,

//	NAME_BORDER1 ,0,0,0, 640-64,96,
//	NAME_SMWIRE ,0,0,0,	640-64,160+64,
//	NAME_GUIDWIRE,0,0,0, 640-64,160+64*2,
//	NAME_EBWIRE,0,0,0, 640-64,160+64*3,
		
//	NAME_MINIWIRE,0,0,0, 64,64,
//	NAME_LASWIRE,0,0,0, 64,64+64,
//	NAME_AGMWIRE,0,0,0, 64,64+64*2,
//	NAME_BCKWIRE,0,0,0, 64,64+64*3,

 	NAME_END,	  -1,-1,-1, -1,-1
};

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Initialise panels positions in sprite list */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void initialise_panels(void )
{	 
	long i;
	
	drag_mode=FALSE;
	drag_sprite_id=-1;
	clicked_x=-9999;
	clicked_y=-9999;

	if(mouse_connected==YES)
	{
		return_position_and_button_status();
		panels[0].x=mouse_x;
		panels[0].y=mouse_y;
	}

//	create_box(&seqdata[NAME_BOX],80,96);
//	print_string_tobox(vfont4,NAME_BOX,4,4,"MESSAGES");

	i=0;
	while(TRUE)
	{
		if(panels[i].name==NAME_END) break;
		panels[i].id=add_sprite(panels[i].name,ACTION_CYCLE,panels[i].x,panels[i].y);
		if(panels[i].id!=-1)
		{
			sprites[(panels[i].id)].priority=0;
		}
		i++;
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Is it on a sprite... (the mouse) */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

long on_sprite(long the_x,long the_y )
{
	char *gfxdata;
	short *gfxdataw;
	int ptr;
	short width,height;
	int i,x,y;
	int name,frame;
	
	for(i=0;i<MAX_SPRITES;i++)
 	{
		if( (sprites[i].on==TRUE) && (sprites[i].name!=NAME_CURSOR1) )
		{		
			name=sprites[i].name;
			x=sprites[i].x;
			y=sprites[i].y;
			frame=sprites[i].frame;
	
			ptr=seqdata[name].map[frame];
			gfxdata=seqdata[name].data;
			gfxdataw=(short *) (gfxdata+ptr);
			x+=(int ) gfxdataw[0];
			y+=(int ) gfxdataw[1];
			width=gfxdataw[2];
			height=gfxdataw[3];

			if(the_x>=x && the_x<(x+width) 
				&& the_y>=y && the_y<(y+height) )
			{
				// Could do even more precise check as in title screen
				return i;
			}
		}
	}
	return -1;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Edit Map (Query, Drag and Delete Sprites) */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void edit_map(void )
{
	long i;

	if(mouse_connected==YES)
	{
		return_position_and_button_status();
		panels[0].x=mouse_x;
		panels[0].y=mouse_y;

		if(mouse_mode==TRUE && path_mode==FALSE)
		{
			// Sprite Deletion on Right Mouse Button

			return_button_press_data();
			if( (clicked_mouse>0) && (clicked_right_button==YES) )
			{
				if(drag_mode==YES)							// Turn off Drag Mode
				{
					sprites[drag_sprite_id].on=FALSE;	// Kill Sprite
					drag_mode=NO;
				}
				else
				{
					clicked_x=clicked_mouse_x+scroll_x;
					clicked_y=clicked_mouse_y+scroll_y;
					i=on_sprite(clicked_x,clicked_y);	
					if(i!=-1)									// Is Mouse On sprite?
					{
						sprites[i].still=NO;					// Kill Sprite
						sprites[i].on=NO;			
					}
				}
			}

			// Sprite Dragging on Left Mouse Button

			if(clicked_mouse>0 && clicked_left_button==YES)
			{
				if(drag_mode==YES)					
				{
					//set_ai_none(drag_sprite_id);
				 	drag_mode=NO;						 		// Turn drag mode off
				}
				else
				{
					clicked_x=clicked_mouse_x+scroll_x;
					clicked_y=clicked_mouse_y+scroll_y;
					i=on_sprite(clicked_x,clicked_y);  	
					if(i!=-1)									// Is Mouse On Sprite?
					{
						/*
						clear_box(&seqdata[NAME_BOX]);
						print_string_tobox(vfont4,NAME_BOX,2,2,get_sprite_name(sprites[i].name));
						*/
						create_sprite_query_window(i);
						drag_mode=YES;
						drag_sprite_id=i;
						//set_ai_cycle(drag_sprite_id);
						drag_sprite_original_x=sprites[i].x;
						drag_sprite_original_y=sprites[i].y;
						sprites[i].still=NO;
					}
				}
			}

			if(drag_mode==YES)								// Drag Sprite
			{
				sprites[drag_sprite_id].x=drag_sprite_original_x+
												  (mouse_x+scroll_x-clicked_x);
				sprites[drag_sprite_id].y=drag_sprite_original_y+
												  (mouse_y+scroll_y-clicked_y);
			}																	  

			i=viewable_width-mouse_x;						// Update Scroll
			if(i<0)  i=0;
			if(i<32) scroll_x+=(32-i)/2;

			i=32-mouse_x;
			if(i>0) scroll_x-=i/2;

			i=viewable_height-mouse_y;
			if(i<0) i=0;
			if(i<32) scroll_y+=(32-i)/2;

			i=32-mouse_y;
			if(i>0) scroll_y-=i/2;

		 	if(scroll_x>(iff_width-viewable_width)) scroll_x=iff_width-viewable_width;
			if(scroll_x<0) scroll_x=0;
			if(scroll_y<0) scroll_y=0;
			if(scroll_y>(iff_height-viewable_height)) scroll_y=iff_height-viewable_height;

//			bodge_scroll();


 		}
		else
		{
			// This is debug code for find_nearest_sprite procedure
			return_button_press_data();
			if(clicked_mouse>0 && clicked_left_button==YES)
			{
				clicked_x=clicked_mouse_x+scroll_x;
				clicked_y=clicked_mouse_y+scroll_y;
				i=find_nearest_sprite(clicked_x,clicked_y);
				if(i!=-1)
				{
					//clear_box(&seqdata[NAME_BOX]);
					//print_string_tobox(vfont4,NAME_BOX,2,2,get_sprite_name(sprites[i].name));
					add_sprite(NAME_EBWIRE,ACTION_NONE,sprites[i].x,sprites[i].y);
				}
			}
			/*
			// This is aborted code for internal path editing
			if(clicked_mouse>0 && clicked_left_button==YES)
			{
				i=current_line;
				lines[i].line_on=9999;
				lines[i].x1[0]=clicked_x;
				lines[i].x1[1]=clicked_x;
				lines[i].y1[0]=clicked_y;
				lines[i].y1[1]=clicked_y;
				printf("Start = %i,%i\n",clicked_x,clicked_y);
				clicked_x=clicked_mouse_x+scroll_x;
				clicked_y=clicked_mouse_y+scroll_y;
				printf("End = %i,%i\n",clicked_x,clicked_y);
				lines[i].x2[0]=clicked_x;
				lines[i].x2[1]=clicked_x;
				lines[i].y2[0]=clicked_y;
				lines[i].y2[1]=clicked_y;
				current_line++;
				if(current_line>=MAX_LINES) current_line=255;
			}
			*/
		}
		
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Update panels XYs to allow for scrolling */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void update_panels(void)
{
	long i,id;
			 
	edit_map();

	i=0;					  
	while(TRUE)
	{
		if(panels[i].name==NAME_END) break;
		id=panels[i].id;
		if(id!=-1)
		{
			sprites[id].visible=TRUE;
			sprites[id].on=TRUE;
			if(drag_sprite_id==id && drag_mode==YES)
			{
				panels[i].x=sprites[id].x-scroll_x;
				panels[i].y=sprites[id].y-scroll_y;
			}
			else
			{
				sprites[id].x=panels[i].x+scroll_x;
				sprites[id].y=panels[i].y+scroll_y;
			}
		}
		i++;
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* End of File */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

