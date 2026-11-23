/*
void setpal(char *pal,int start,int length)
{
	int i;
	char r,g,b;
	char *source=pal;
	for(i=0;i<length;i++)
	{
		r=*source++;
		g=*source++;
		b=*source++;

		outp(0x3c8,i+start);
		outp(0x3c9,r);
		outp(0x3c9,g);
		outp(0x3c9,b);
	}
}
*/

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* LBM Chopper */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Now in MAKEBACK
// One Pass or Two Pass?
// Optimum size appears to be 16

char get_map_pixel(int x,int y);
char current_blk[blk_size];

void chop_lbm(char *source)
{
	int y,i;
	int blk_x,blk_y,blk_i,blks;
	int map_i;
	char *dest;

	printf("Scanning...\n");
	printf("Pixels = %i\n",iff_width*iff_height);

	printf("RAM Left = %i\n",get_ram_available());
	map=(long *) malloc( (iff_width/blk_dim) * (iff_height/blk_dim) * 4);
	printf("RAM Left = %i\n",get_ram_available());

	map_i=0;
	blks=0;
	for(blk_y=0;blk_y<iff_height;blk_y+=blk_dim)
	{
		for(blk_x=0;blk_x<iff_width;blk_x+=blk_dim)
		{
			// Copy Block To Temporary Buffer
			i=0;
			blk_i=(blk_y*iff_width)+blk_x;
			for(y=0;y<blk_dim;y++)
			{
				memcpy(current_blk+i,source+blk_i,blk_dim);
				i+=16;
				blk_i+=iff_width;
			}
			// * Add New Blocks *
			i=0;
			if(blks==0)
			{
				blk_buf=malloc(blk_size);
				memcpy(blk_buf,current_blk,blk_size);
				blks++; 
			}
			else
			{
				dest=blk_buf;
			 	for(i=0;i<blks;i++)
				{
					if(*current_blk==*dest)
					{
						if(memcmp(current_blk,dest,blk_size)==0)
							break;
					}
					dest+=blk_size;
				}
				if(i==blks)
				{
				 	if( (blk_buf=realloc(blk_buf,blk_size*(blks+1))) ==NULL )
					{
						printf("No more memory\n");
						getch();
						exit(-1);
					}
					else
					{
						memcpy(blk_buf+(blks*blk_size),current_blk,blk_size);
						blks++;
					}
				}
			}
			map[map_i++]=i;
		}
		printf("Y = %i Blks = %i \n",blk_y,blks);
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Integer Square Root (Approximation Method) */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

unsigned long sqr_root(unsigned long val)
{
	int	i;
	unsigned long	upper,lower,mid;
	unsigned long	sqr;

	upper=val;
	lower=0;

	if(upper>65536) upper=65536;

	for(i=0;i<16;i++)
	{
		mid=lower+((upper-lower)>>1);
		sqr=mid*mid;
		if(sqr==val) break;
		if(sqr>val) upper=mid;
		else lower=mid;
	}
	return(mid);
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Some code */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Shouldn't be here anymore

void add_val(char *source,int length,char val)
{
	int i;
	for(i=0;i<length;i++)
		source[i]+=val;
}

void remap_area(char *source,int length)
{
	int i;
	for(i=0;i<length;i++)
		source[i]=remap[(source[i])];
}

char *create_1plane(char *source,int length)
{
	int i,j;
	char m;
	char *dest,*temp;
	temp=dest=(char *) malloc(length/8);

	for(i=0;i<(length/8);i++)
	{
		m=0;
		for(j=0x80;j>0;j=j>>1) if((*source++)!=0) m+=j;
		*dest++=m;
	}
	return temp;
}

void generate_cos_sin_tables(void )
{
	long i;

	for(i=0;i<(256*4);i++)
	{
		if(i>0)
		{
			cos_table[i]=cos((i*PI*2)/1024)*32767;
			sin_table[i]=sin((i*PI*2)/1024)*32767;
		}
		else
		{
			 cos_table[i]=32767;
			 sin_table[i]=0;
		}
	}
}
