/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Particle code */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// This could be done far more effectively...
// Quadruple buffer of pixel list so old positions are not recalculated...
// i.e. this could be heavily optimised...
// Better priority for pixels would be good so they can go infront of some
// sprites and behind others
// Far too much memory wasted here
// ? line list with max pixels per line (would be more memory efficient)?

#include "globals.h"

#define	MAX_PIXELS (2048)
#define	MAX_PARTICLES (2048)
short		pixel_list_x[2][MAX_PIXELS];
short 	pixel_list_y[2][MAX_PIXELS];
ulong		particle_list_mask[MAX_PARTICLES/32];
long		particle_list_x[MAX_PARTICLES];
long		particle_list_y[MAX_PARTICLES];
long		particle_list_dx[MAX_PARTICLES];
long		particle_list_dy[MAX_PARTICLES];
long		particle_list_ddx[MAX_PARTICLES];
long		particle_list_ddy[MAX_PARTICLES];
long		particle_list_counter[MAX_PARTICLES];
long		particle_list_delay[MAX_PARTICLES];
long		particle_list_halflife[MAX_PARTICLES];
int 		band_lists_count[32];
unsigned short	band_lists_offs[32][MAX_PIXELS*2];
char 		band_lists_col[32][MAX_PIXELS*2];

void init_particle_list(void)
{
	int i;
	for(i=0;i<32;i++) band_lists_count[i]=0;

	for(i=0;i<(MAX_PIXELS/32);i++) particle_list_mask[i]=0;

	for(i=0;i<MAX_PARTICLES;i++)
	{
		particle_list_x[i]=-1;
		particle_list_y[i]=-1;
		particle_list_dx[i]=-1;
		particle_list_dy[i]=-1;
		particle_list_counter[i]=-2;
		particle_list_delay[i]=0;
		particle_list_halflife[i]=0;
	}
	for(i=0;i<MAX_PIXELS;i++)
	{
		pixel_list_x[0][i]=-1;
		pixel_list_y[0][i]=-1;
		pixel_list_x[1][i]=-1;
		pixel_list_y[1][i]=-1;
	}
}

// Again could do with being in code...

void move_particles(int buf)
{
	long i,j,k,x,y;
	
	for(j=0;j<(MAX_PARTICLES/32);j++)
	{
		if(particle_list_mask[j]!=0)
		{
		i=j*32;
		for(k=0;k<32;k++)
		{
			if(particle_list_counter[i]>0)
			{
				if(particle_list_delay[i]>0) particle_list_delay[i]--;
				else
				{	
					particle_list_counter[i]--;
					particle_list_delay[i]=particle_list_halflife[i];
				}
			
				x=particle_list_x[i];
				y=particle_list_y[i];
				x+=particle_list_dx[i];
				y+=particle_list_dy[i];
				particle_list_dx[i]+=particle_list_ddx[i];
				particle_list_dy[i]+=particle_list_ddy[i];

		   	particle_list_x[i]=x;
				x=x>>16;
				pixel_list_x[buf][i]=x;
				particle_list_y[i]=y;
				y=y>>16;
				pixel_list_y[buf][i]=y;

				if( ( coll_buf[((x+iff_width*y)>>3)] & (1<<(7-x&7)) ) !=0 )
				{
				 	particle_list_counter[i]=0;
					pixel_list_x[buf][i]=-1;
					pixel_list_y[buf][i]=-1;
					particle_list_x[i]=-1;
					particle_list_y[i]=-1;
				}
			}
			else									// Remove particle from each buffer...
			{
				if(particle_list_counter[i]!=-2)
				{
					particle_list_counter[i]--;
					pixel_list_x[buf][i]=-1;
					pixel_list_y[buf][i]=-1;
				}
				else particle_list_mask[j]&=(0xffffffff-(1<<i&31));
			}
		i++;
		}
		}
	}
	
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Add a particle for back of ship */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void add_particle(long x,long y,long dx,long dy)
{
	int i,j,k;

	for(j=0;j<(MAX_PARTICLES/32);j++)
	{
		if(particle_list_mask[j]!=0xffffffff)
		{
			i=j*32;
			for(k=0;k<32;k++)
			{
				if(particle_list_counter[i]==-2)
				{
					particle_list_mask[j]|=(1<<(i&31));
					particle_list_counter[i]=128;
					particle_list_x[i]=x<<16;
					particle_list_y[i]=y<<16;
					particle_list_dx[i]=dx;
					particle_list_dy[i]=dy;
					particle_list_ddx[i]=0;
					particle_list_ddy[i]=2048;
					pixel_list_x[0][i]=x;
					pixel_list_y[0][i]=y;
					pixel_list_x[1][i]=x;
					pixel_list_y[1][i]=y;
					particle_list_delay[i]=0;
					particle_list_halflife[i]=0;
					return;
				}
				i++;
			}
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Add particle with no gravity */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void add_particle_acceleration(long x,long y,long dx,long dy,
										 long ddx,long ddy)
{
	int i,j,k;

	for(j=0;j<(MAX_PARTICLES/32);j++)
	{
		if(particle_list_mask[j]!=0xffffffff)
		{
			i=j*32;
			for(k=0;k<32;k++)
			{
				if(particle_list_counter[i]==-2)
				{
					particle_list_mask[j]|=(1<<(i&31));
					particle_list_counter[i]=128;
					particle_list_x[i]=x<<16;
					particle_list_y[i]=y<<16;
					particle_list_dx[i]=dx;
					particle_list_dy[i]=dy;
					particle_list_ddx[i]=ddx;
					particle_list_ddy[i]=ddy;
					pixel_list_x[0][i]=x;
					pixel_list_y[0][i]=y;
					pixel_list_x[1][i]=x;
					pixel_list_y[1][i]=y;
					particle_list_delay[i]=0;
					particle_list_halflife[i]=8;
					return;
				}
				i++;
			}
		}
	}
}


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Pixel code */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void dump_pixels(void)
{
	char *vga=(char *) screen_lin_addr;
	int i,x,count;

	for(i=0;i<16;i++)
	{
		if((count=band_lists_count[i])>0)
		{
			if(currentband!=i)
			{
				currentband=i;
				setvesaband(currentband);
			}
			for(x=0;x<count;x++)
				vga[(band_lists_offs[i][x])]=band_lists_col[i][x];
		}
	}
}

void dump_pixel_bank(long bank )
{
	char *vga=(char *) screen_lin_addr;
	int x,count;

	if((count=band_lists_count[bank])>0)
	{
		if(currentband!=bank)
		{
			currentband=bank;
			setvesaband(currentband);
		}
		for(x=0;x<count;x++)
			vga[(band_lists_offs[bank][x])]=band_lists_col[bank][x];
	}
}

void draw_pixels(int buf,int screen,int scroll_x,int scroll_y)
{
	int i,x,y,dest,band,count;

	//for(i=0;i<16;i++) band_lists_count[i]=0;

	for(i=0;i<MAX_PIXELS;i++)
	{
		x=pixel_list_x[buf][i];
		if(x!=-1)
		{
			y=pixel_list_y[buf][i]-scroll_y;
			if((y>=clip_start_y) && (y<viewable_height)
				&& ((x-scroll_x)>=clip_start_x) && ((x-scroll_x)<viewable_width))
			{
				if(safe_mode==YES)
					dest=x-scroll_x+(y+screen)*SVGA_logical_length;
				else
					dest=x+(y_mod[scroll_y]+y+screen)*SVGA_logical_length;

				band=dest>>16;
				count=band_lists_count[band];
				band_lists_count[band]++;
				band_lists_offs[band][count]=dest;
				band_lists_col[band][count]=128+particle_list_counter[i];
			}
		}
	}
	//dump_pixels();
}

// Perhaps this should be retrieved from the circular bg buf instead

char get_map_pixel(int x,int y)
{
//	return dump_blk_buf[(x+(y_mod640[scroll_y+y]))];

	int ix,iy;
	int mx,my;

	ix=x&0x000f;
	iy=(y&0x000f)*blk_dim;
	mx=x>>4;
	my=(y/blk_dim)*(iff_width/blk_dim);

	return *(blk_buf+(map[mx+my]*blk_size)+iy+ix);
}
	
void clear_particle_list(void)
{
	long i;
	
	for(i=0;i<32;i++) band_lists_count[i]=0;
}

void remove_pixels(int buf,int screen,int scroll_x,int scroll_y)
{
	int i,x,y,dest;
	int band,count;

	if(safe_mode==YES && bursting==YES) return;

	if(safe_mode!=YES) scroll_y=old_scroll_y;

	for(i=0;i<MAX_PIXELS;i++)
	{
		x=pixel_list_x[buf][i];
		if(x!=-1)
		{
			y=pixel_list_y[buf][i];	
			y-=scroll_y;
			if((y>=clip_start_y) && (y<viewable_height)
			   && ((x-scroll_x)>=clip_start_x) && ((x-scroll_x)<viewable_width))
			{
				if(safe_mode==YES)
					dest=x-scroll_x+(y+screen)*SVGA_logical_length;
				else
					dest=x+(y_mod[scroll_y]+y+screen)*SVGA_logical_length;

				band=dest>>16;
				count=band_lists_count[band];
				band_lists_count[band]++;
				band_lists_offs[band][count]=dest;

				band_lists_col[band][count]=get_map_pixel(x,y+scroll_y);
			}
		}
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Pixel draw for Lines */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void draw_pixel(int x,int y,char c)
{
	long	dest,band,count;
	y-=bufy[buf];
	if(safe_mode==YES)
		dest=x-bufx[buf]+(y+logical_screen+split_height)*SVGA_logical_length;
	else
		dest=x+(bufy[buf]%screen_height+y+logical_screen+split_height)*SVGA_logical_length;

	band=dest>>16;
	count=band_lists_count[band];
	band_lists_count[band]++;
	band_lists_offs[band][count]=dest;
	band_lists_col[band][count]=c;
}

void remove_pixel(int x,int y)
{
	long	dest,band,count;
	y-=bufy[buf];
	if(safe_mode==YES)
		dest=x-bufx[buf]+(y+logical_screen+split_height)*SVGA_logical_length;
	else
		dest=x+(bufy[buf]%screen_height+y+logical_screen+split_height)*SVGA_logical_length;

	band=dest>>16;
	count=band_lists_count[band];
	band_lists_count[band]++;
	band_lists_offs[band][count]=dest;
	band_lists_col[band][count]=get_map_pixel(x,y+bufy[buf]);
}


