/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* FLIC structures */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include "globals.h"

char	pullpixel(int pos);
void	writepixel(int pos,char c);
int	copy_word_line(int dest,int i);
int	copy_byte_line(int dest,int i);
int	store_word_line(int dest,int i);
int	store_byte_line(int dest,int i);
char	*case_0007(char *,char *);
#pragma aux case_0007 parm caller [esi] [edi];

char	*back_base;
char	*back_base_old;
char	*buffer_base;
/*
char	back_base[640*480];
char	back_base_old[640*480];
char	buffer_base[640*480];
*/

char	*cur_addr;
int 	pixels_changed;

#define gbyte(fp) (unsigned char)(fgetc(fp))
#define gword(fp) (unsigned int)(gbyte(fp)+(gbyte(fp)<<8))
#define glong(fp) (unsigned int)(gbyte(fp)+(gbyte(fp)<<8)+(gbyte(fp)<<16)+(gbyte(fp)<<24))

#define rgbyte(fp) (unsigned char) (*(cur_addr++))
#define rskip(n) cur_addr+=(n);
#define rfgetc(fp) *(cur_addr++)

short rgint()
{
 	short i;
	i  = rgbyte(fp);
	i += rgbyte(fp)<<8;
	return(i);
}

int rgword()
{
 	int i;
	i  = rgbyte(fp);
	i += rgbyte(fp)<<8;
	return(i);
}

int rglong()
{
 	int i;
	i  = rgbyte(fp);
	i += rgbyte(fp)<<8;
	i += rgbyte(fp)<<16;
	i += rgbyte(fp)<<24;
	return(i);
}

#define SCREEN_AREA 0xa000
#define SCREEN_LIN_ADDR ((SCREEN_AREA) << 4)
char *screen_base= (char *)SCREEN_LIN_ADDR;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Play a FLC */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void play_a_flic(char *fname )
{
	long	i;
	FILE *fp;
	
	if ((fp=fopen( fname, "rb")) != NULL)
 	{
		read_flc_header(fp);	

		while (ftell(fp)<(fliheader.fli_size-128 ) ) 
		{
			decodechunk( fp );

		  	for(i=0;i<480;i++) screen_dump_line(back_base+i*640,i*640,fliheader.width);
		}
		fclose(fp);
	}
	else printf("Could not open file\n");
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Play FLC */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Need an autobox routine with a width height and origin

void read_flc_header(FILE *fp)
{
	fread(cur_addr=buffer_base,128,1,fp);
	fliheader.fli_size=rglong();
	fliheader.fli_type=rgword();		
	fliheader.frames=rgword()+1;
	fliheader.width=rgword();
	fliheader.height=rgword();
	fliheader.depth=rgword();
	fliheader.flags=rgword();
	fliheader.speed=rglong();
	fliheader.res_1=rgword();
	fliheader.created=rglong();
	fliheader.creator=rglong();
	fliheader.updated=rglong();
	fliheader.updater=rglong();
	fliheader.aspectx=rgword();
	fliheader.aspecty=rgword();
	rskip(19<1);
	fliheader.oframe1=rglong();
	fliheader.oframe2=rglong();
	rskip(20<1);
}
	
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Decode FLC chunk */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

int		decodechunk( FILE *fp ) 
{
	struct	CHUNK_HEADER chunk;
	int		k;
	long	n;
	int		displacement=0; 	// (640*96)+(640-544)/2;
	int		i, busy = 0, errchk;

	n = ftell( fp );
	k = gbyte( fp );
	k = k + gbyte( fp ) * 256;
	k = k + gbyte( fp ) * 256 * 256;
	k = k + gbyte( fp ) * 256 * 256 * 256;
	chunk.chunk_size = k;													 /* glong(); */
	k = gbyte( fp );
	k = k + gbyte( fp ) * 256;
	chunk.chunk_type = k;													/* gword(); */

	if ( chunk.chunk_type != 0xF1FA ) fread( cur_addr = buffer_base, 1, chunk.chunk_size - 6, fp );
	//printf("Chunk Type= %08X\n",chunk.chunk_type);
	//getch();
	switch ( chunk.chunk_type )
		{
		case	0xF100:				// prefix chunk - ignore
		 		break;
		
		case	0xF1FA:  
				errchk=0;
				k = gbyte( fp );
				k = k + gbyte( fp ) * 256;
				chunk.nsubchunks = k;			/* gword(fp); */
		 		fread( chunk.res1, 1, 8, fp );
				for ( i=0; i<chunk.nsubchunks; i++ ) busy |= decodechunk( fp );
				busy ++;
				break;

		case	0x0004:
				busy ++;
					{
					char *temp;
					int	i, np, ccolor = 0, ncolor;
					np = rgword();
					for ( i=0; i<np; i++ )
						{
						ccolor += rgbyte( fp );
						ncolor = rgbyte( fp );
						if ( ncolor == 0 ) ncolor = 256;
						temp = palette;
						for(; ncolor>0; ncolor -= 1, ccolor += 1 )
							{
							temp[ ccolor * 3 + 0 ] = rgbyte( fp ) / 4;
							temp[ ccolor * 3 + 1 ] = rgbyte( fp ) / 4;
							temp[ ccolor * 3 + 2 ] = rgbyte( fp ) / 4;
							}
						}
					set_palette( palette,0,256 );
					}
				break;

		case	0x0007:	
				busy ++;
				errchk=1;
				//startframe();
				/*
					{

					short		yc;
					int			dest;
					short		y = 0, np, lx;
					signed char i;
					char		b1, b2;

					yc = rgint();
					lx = fliheader.width - 1;

					while( yc > 0 )
						{
						np = rgint();
						if ( np >= 0 )
							{
							dest = (y*screen_width) + displacement;
							while( np > 0 )
								{
								dest += rgbyte( fp );
								i = rfgetc( fp );
								if ( i > 0 ) dest=copy_word_line(dest,i);
								else dest=store_word_line(dest,-i);
								np--;
								}
							++y;
							--yc;
							}
						else if (((unsigned int)np)&0x4000)
							{
							y -= np;
							}
						else
							{
							// *( dest + lx ) = ( unsigned char ) np;

							if (( np = rgword()) == 0 )
								{
								++y;
								--yc;
								}
							}
						}
					}		

				*/
				cur_addr=case_0007(cur_addr,back_base);
				//endframe();
				break;


		case	0x000B:
				busy ++;
					{	
					char	*temp;
					int		i, np, ccolor = 0, ncolor;

					np = rgword();
					for ( i=0; i<np; i++ )
						{
						ccolor += rgbyte( fp );
						ncolor = rgbyte( fp );
						if ( ncolor == 0 ) ncolor = 256;

						temp = palette;
						for( ; ncolor>0; ncolor -= 1, ccolor += 1 )
							{
							temp[ 0 ] = rgbyte( fp );
							temp[ 1 ] = rgbyte( fp );
							temp[ 2 ] = rgbyte( fp );
							temp += 3;
							}
						}
					set_palette( palette,0,256 );
					}
				break;


		case	0x000C :
				errchk=1;
				busy++;
					{
					int			sy, yc;
					int			dest;
					int			y = 0, b, np;
					signed char	i;

					sy = rgword();
					yc = rgword();

					for ( y=sy; yc>0; yc--, y++ )
						{
						np = rgbyte( fp );
						dest = ( y * screen_width ) + displacement ;
						for (; np>0; np-- )
							{
							dest += rgbyte( fp );
							i = rfgetc( fp );
							if ( i > 0 )
								{
							    for ( ; i>0; i-- ) 
									writepixel(dest++,rgbyte(fp));
								}
							else
								{
							    i = 0 - i;
							    b = rgbyte( fp );
							    for ( ; i>0; i-- )
									writepixel(dest++,b);
								}
							}
						}
					}
				break;

		case	0x000D:
				busy ++;
					{
					//unsigned int	i;
					// char			*dest;
					// for ( i=0, dest = screen_base; i<64000; i++, *( dest ++ ) = 0 );
					}
				break;

		case	0x000F:
				errchk=1;
				busy ++;
				{
					int		dest;
					int		x, y = 0;
					signed  char i;
					unsigned char   skipit;			//	won't compile without this

					while ( y < fliheader.height )
					{
						x = 0;
						skipit = rgbyte( fp );					// throw away old length
						dest = ( y * screen_width )+displacement;
//						while ( x < fliheader.width )
						while (skipit>0 )
						{
							i = rfgetc( fp );
							if ( i < 0 )
							{
								i = 0 - i;
								x += i;
								dest=copy_byte_line(dest,i);
							}
							else
						 	{
								x += i;
								dest=store_byte_line(dest,i);
							}
							skipit--;
						}
					y ++;
					}
				}
				break;

		case	0x0010:
				errchk=1;
				busy ++;
				{
					int y;
					int dest;
					for(y=0;y<(fliheader.height);y++)
					{
						dest=(y*screen_width)+ displacement;
						// for(x=0;x<(fliheader.width);x++) writepixel(dest++,rfgetc(fp));
						dest=copy_word_line(dest,(fliheader.width>>1));
					}
				}
				break;

		case	0x0012:
				break;

		 default:
				/* printf("Unknown Type of Chunk: %4X, %ld\n",chunk.chunk_type,chunk.chunk_size); */
				break;
		}
	
		fseek( fp, n + chunk.chunk_size, SEEK_SET );


/*	 if (getch()=='q')
	  MyExit(0);
*/
	return( errchk );
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* FLC fast screen copies */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

int	copy_word_line(int dest,int i)
{
	char *ptr=back_base;
	memcpy(ptr+dest,cur_addr,i<<1);
	dest+=i<<1;
	cur_addr+=i<<1;

	/*
	char *ptr=(char *) screen_lin_addr;
	long newband;
	newband=((dest+i<<1)>>16);
	if	(newband==currentband)
	{
		memcpy(ptr+(dest&0xffff),cur_addr,i<<1);
		dest+=i<<1;
		cur_addr+=i<<1;
	}
	else
	{
		i=i<<1;
		for (; i>0; i-- )
		{
			if ( (dest>>16)!= currentband )
			{
				currentband=dest>>16;
				setvesaband(currentband);
			}
			ptr[(dest&0xffff)]=rgbyte(fp);
			dest++;
		}
	}
	*/
	return dest;
}

int	copy_byte_line(int dest,int i)
{
	char *ptr=back_base;
	ptr+=dest;
	dest+=i;
	for (; i>0; i-- ) *(ptr++)=rgbyte(fp);

/*
	long newband;
	newband=((dest+i)>>16);
	if	(newband==currentband)
	{
		char *ptr=(char *) screen_lin_addr;
		ptr+=(dest & 0xffff);
		dest+=i;
		for (; i>0; i-- ) *(ptr++)=rgbyte(fp);
	}
	else
	{
		for (; i>0; i-- ) writepixel(dest++,rgbyte(fp));
	}
*/
	return dest;
}


int store_word_line(int dest,int i)
{
	char *ptr=back_base;
	char a=rgbyte(fp);
	char b=rgbyte(fp);
	ptr+=dest;
	dest+=i<<1;
	for (; i>0; i-- )
	{
		*(ptr++)=a;
		*(ptr++)=b;
	}

	/*
	long newband;
	char a=rgbyte(fp);
	char b=rgbyte(fp);
	newband=((dest+i<<1)>>16);
	if	(newband==currentband)
	{
		char *ptr=(char *) screen_lin_addr;
		ptr+=(dest & 0xffff);
		dest+=i<<1;
		for (; i>0; i-- )
		{
			*(ptr++)=a;
			*(ptr++)=b;
		}
	}
	else
	{
		for (; i>0; i-- )
		{
			writepixel(dest++,a);
			writepixel(dest++,b);
		}
	}
*/
	return dest;
}

int store_byte_line(int dest,int i)
{
	char *ptr=back_base;
	char a=rgbyte(fp);
	ptr+=dest;
	dest+=i;
	for (; i>0; i-- ) *(ptr++)=a;

	/*
	long newband;
	char a=rgbyte(fp);
	newband=((dest+i)>>16);
	if	(newband==currentband)
	{
		char *ptr=(char *) screen_lin_addr;
		ptr+=(dest & 0xffff);
		dest+=i;
		for (; i>0; i-- ) *(ptr++)=a;
	}
	else
	{
		for (; i>0; i-- ) writepixel(dest++,a);
	}
	*/
	return dest;
}

void writepixel(int pos,char c)
{
	back_base[pos]=c;

	/*
	char *j=(char *) screen_lin_addr;
	long newband;
	newband=(pos>>16);
	if (newband!=currentband) 
	{
		setvesaband(newband);
		currentband=newband;
	}
	j+=(pos & 0xffff);
	*j=c;
	*/
}

void screenplotpixel(int pos,char c)
{
	char *j=(char *) screen_lin_addr;
	long newband;
	newband=(pos>>16);
	if (newband!=currentband) 
	{
		setvesaband(newband);
		currentband=newband;
	}
	j+=(pos & 0xffff);
	*j=c;
}


char pullpixel(int pos)
{
	return back_base[pos];
/*
	char *j=(char *) screen_lin_addr;
	long newband;
	newband=(pos>>16);
	if (newband!=currentband) 
	{
		setvesaband(newband);
		currentband=newband;
	}
	j+=(pos & 0xffff);
	return *j;
*/
}

