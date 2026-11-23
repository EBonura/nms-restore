/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Globe Code */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include "globals.h"

int globe_y_offset[2048];
int globe_width[2048];
int globe_x_offset[2048];
int globe_radius_store;
int globe_x_size_store, globe_y_size_store;

// ----------------------------------------------------------------------------
void InitGlobe(int x_size, int y_size, int globe_radius)
{

	long  i;
	double posx, posy, angle;

	globe_radius_store = globe_radius;
 	globe_x_size_store = x_size;
	globe_y_size_store = y_size;

	// first, whizz through and setup width table and y_offset

	for (i = -globe_radius; i < globe_radius-1; i++)
	{
		posy = 1.0 + (double) i;
		posy /= globe_radius + 0.5;

		posx = sqrt( 1.0 - (posy * posy));

		globe_width[i + globe_radius] = posx * (double) globe_radius;

		angle = asin( posy);

	   globe_y_offset[i + globe_radius] = ((angle * y_size) / 3.141592654);
    	globe_x_offset[i + globe_radius] = ((angle * x_size) / (3.141592654 * 2));
  }

}
// ----------------------------------------------------------------------------
void DrawGlobe( int centre_x, int centre_y, int view_x, char *bitmap, char *backscreen)
{

int y, screen_x, screen_y, line_width, x_delta, bitmap_x, bitmap_y, x_index;
int x_offset, x_count, remaining_length, count;
char *screen_ptr, *bitmap_ptr;

  for (y = -globe_radius_store; y < globe_radius_store-1; y++)
  {
    screen_y = centre_y + y;

    if ((screen_y > 0) && (screen_y < 480))
    {
      bitmap_y = globe_y_offset[y + globe_radius_store] + (globe_y_size_store / 2);

      line_width = globe_width[y + globe_radius_store];

      if (line_width > 0)
      {
        x_delta = (globe_radius_store * 65536) / line_width;

        x_index = 0;
        screen_x = centre_x - line_width;
        bitmap_x = view_x;

        while (bitmap_x >= globe_x_size_store)
        {
          bitmap_x -= globe_x_size_store;
        }

        while (bitmap_x < 0)
        {
          bitmap_x += globe_x_size_store;
        }

        remaining_length = globe_x_size_store - bitmap_x;

        bitmap_ptr = &bitmap[bitmap_y * globe_x_size_store + bitmap_x];

        x_count = (line_width * 2) - 1;

        if (screen_x < 0)
        {
          x_index -= x_delta * screen_x;
          x_count += screen_x;
          screen_x = 0;
        }

        screen_ptr = &backscreen[screen_y*640 + screen_x];

        if (x_count > 640)
        {
          x_count = 640;
        }

        count = x_count;
        if (count > remaining_length)
        {
          count = remaining_length;
        }
        x_count -= count;

		  while(count>0)
        {
          x_offset = globe_x_offset[ (x_index >> 16) ];

          *screen_ptr++ = bitmap_ptr[ x_offset ];

          x_index += x_delta;
			 count--;
        }

        if (x_count > 0)
        {
          bitmap_ptr -= globe_x_size_store;

			 while(x_count>0)
          {
            x_offset = globe_x_offset[ (x_index >> 16) ];

				*screen_ptr++=bitmap_ptr[ x_offset ];
            x_index += x_delta;
				x_count--;
          }
        }
      }
    }
  }

}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Remapper */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

//void create_remaptable



/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Globe Viewer */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

long starts[480];
long ends[480];

void view_globe(void )
{
  	char *source,*dest;
	char *cmap,*cdest;
	long i,plays;
	long x,y;
	long dx,dy;
	long width,height;
	long start,end;
	char *temp;

//	source=readlbm("ULTIM_X3.LBM");
//	source=readlbm("SPHERE/L4TMAP.LBM");
//	source=readlbm("GFX/LEVELS/LEVEL9.LBM");
//	source=readlbm("DAVE.LBM");
	source=readlbm("TEMP2.LBM");
	dest=heap_malloc(640*480);
	cdest=heap_malloc(640*480);
	memset(dest,0,640*480);
	memset(cdest,0,640*480);

	iff_width=640;
	iff_height=400;
	source=malloc(640*400);

	draw_mandy(source);

	temp=malloc(iff_width*iff_height*2);

	memcpy(temp,source,iff_width*iff_height);
	memcpy(temp+iff_width*iff_height,source,iff_width*iff_height);
	source=temp;

//	cmap=temp;

	/*
	for(i=0;i<10000;i++)
	{
		x=(iff_width*rand()/32768);
		y=(iff_height*rand()/32768);
		cmap[x+y*iff_width]++;
	}
	*/

	set_palette(palette,0,256);

	/*
	for(y=0;y<iff_height;y++)
	{
		screen_dump_line(source+y*iff_width,y*SVGA_logical_length,iff_width);
		memcpy(dest+y*640,source+y*iff_width,iff_width);
	}
	*/

	width=280;
	height=280;

	InitGlobe(iff_width,iff_height,width);

	DrawGlobe(320,240,0,source,dest);
		for(i=0;i<480;i++)
		{
		 	screen_dump_line(dest+i*640,i*SVGA_logical_length,640);
		}
//	getch();
	
	for(y=0;y<480;y++)
	{
		start=0;
		while(start<640)
		{ 
			if(dest[y*640+start]!=0) break;
			start++;
		}
		
		end=640;
		while(end>start)
		{ 
			if(dest[y*640+end]!=0) break;
			end--;
		}
		starts[y]=start;
		ends[y]=end;
	}
	x=320;
	y=240;
	dx=1;
	dy=1;
	for(plays=0;plays<iff_width*16;plays+=2)
	{
		scan_for_keysjp();

		if(keys_jp[kESC]==128) exit(-1);

		memset(dest,0,640*480);
		DrawGlobe(x,y,plays,source+(plays%iff_height)*iff_width,dest);

//		DrawGlobe(x,y,plays,cmap,cdest);

//		asm_andor_line(cdest,dest,640*480);

		wait_vbi();
		for(i=0;i<480;i++)
		{
		 	screen_dump_line(dest+i*640,i*SVGA_logical_length,640);

//		 	screen_dump_line(dest+i*640+starts[i],i*SVGA_logical_length+starts[i],ends[y]-starts[y]);
		}
//		x=x+dx*4;
//		y=y+dy*4;
		if( (x>=(640-width)) ) dx=-1;
		if( (x<=width ) ) dx=1;
		if( (y>=(480-height)) ) dy=-1;
		if( (y<=height) ) dy=1;
	}

	exit(-1);
}
