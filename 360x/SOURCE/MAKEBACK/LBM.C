/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Phillip Trelford's LBM decompress */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include "globals.h"

long	iffread_width;
long	iffread_height;

short		get_word(char *source);
int		get_long(char *source);

char 	line_buffer[4096];
//int 	iff_width,iff_height;
//char 	palette[256*3];
//unsigned char	remap[256];
unsigned char	done[128];

#define type_ILBM 0
#define type_PBM 1

char *decompress_lbm(signed char *source)
{
	unsigned char *buf;
	short width,height;
	short plane_width;
	char  planes,compression;
//	int   dest;
	int   a,x,y,iy;
	char  c;
	int   iff_type=type_PBM;

	if(strncmp(source+8,"ILBM",4)==0)
		iff_type=type_ILBM;

	if(strncmp(source+8,"PBM ",4)==0)
		iff_type=type_PBM;

	source+=8;

	while(strncmp(source,"BMHD",4)!=0)
		source+=2;

	width=get_word(source+8);
	height=get_word(source+10);
	iff_width=(int ) width;
	iff_height=(int ) height;
	planes=source[16];
	plane_width=width/8;
	compression=source[18];

	buf=(char *)malloc(width*height);
	source+=get_long(source+4)+8;

	while(strncmp(source,"CMAP",4)!=0)
		source+=2;

	setlbmpal(source+8);

	source+=get_long(source+4)+8;

	while(strncmp(source,"BODY",4)!=0)
		source+=2;

	source+=8;

	memset(line_buffer,0,width);
	iy=0;
	for(y=0;y<height;y++)
	{
		iy=y*width;
		x=0;
		while(x<width)
		{
			a=(int ) *source++;
			if(a<0)
			{
				a=-a+1;
				c=*source++;
				memset(line_buffer+x,c,a);
				x+=a;
				/* for(i=0;i<a;i++) line_buffer[x++]=c; */
			}
			else
			{
				a++;
				memcpy(line_buffer+x,source,a);
				source+=a; x+=a;
				/* for(i=0;i<a;i++) line_buffer[x++]=*source++; */
			}
		}
		switch (iff_type)
		{
			case type_PBM:
				for(x=0;x<width;x++)
				{
					buf[x+iy]=line_buffer[x];
				}
				break;
			case type_ILBM:
				asm_blit_planes(line_buffer,(buf+iy),plane_width,((int) planes));
				/*
				dest=iy;
				for(x=0;x<plane_width;x++)
				{
					for(i=0;i<planes;i++)
						p[i]=line_buffer[x+i*plane_width];

					for(i=0x80;i>0;i=i>>1)
					{
						c=0;
						for(j=0;j<planes;j++)
							if(p[j]&i) c+=1<<j;
						buf[dest++]=c;
					}
				}
				*/
				break;
		}
	}
	return buf;
}

short get_word(char *source)
{
	short s,t,u;
	t=(short ) source[0];
	u=(short ) source[1];
	s=(t<<8)+u;
	return s;
}

int get_long(char *source)
{
	int i,t,u,v,w;
	t=(int ) source[0];
	u=(int ) source[1];
	v=(int ) source[2];
	w=(int ) source[3];
	i=(t<<24)+(u<<16)+(v<<8)+w;
	return i;
}

void setlbmpal(unsigned char *pal)
{
	int i,j,k;
	int depth,min_depth;
	unsigned char r,g,b;
	unsigned char *source=pal;
	unsigned char *dest=palette;
	unsigned short *destw;
	unsigned short bgr;

	destw=(unsigned short *) psx_palette;

//	source[0]=0;
//	source[1]=0;
//	source[2]=0;

	for(i=0;i<256;i++)
	{
		r=*source++;
		g=*source++;
		b=*source++;

		*dest++=r>>2;
		*dest++=g>>2;
		*dest++=b>>2;

		r=(r>>3)&31;
		g=(g>>3)&31;
		b=(b>>3)&31;

		bgr=(b<<10)+(g<<5)+r;
		if(bgr==0) bgr=0x8000;


		*destw++=bgr;
	}

	source=pal;
	
	for(i=0;i<128;i++) done[i]=0;

	for(i=0;i<128;i++)
	{
		source=pal;
		min_depth=(256*3)+(256*4)+(256*2);
		k=0;
		for(j=0;j<128;j++)
		{
			r=*source++;
			g=*source++;
			b=*source++;
			if(done[j]==0)
			{
				depth=(r*3)+(g*4)+(b*2);
				if(depth<min_depth)
				{
					min_depth=depth;
					k=j;
				}
			}
		}
		done[k]=1;
		remap[k]=i;
		r=pal[(k*3)+0]>>2;
		g=pal[(k*3)+1]>>2;
		b=pal[(k*3)+2]>>2;

		iff_palette[(i+128)*3+0]=r;
		iff_palette[(i+128)*3+1]=g;
		iff_palette[(i+128)*3+2]=b;
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Mike's LBM decompress */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// This needs to have a 2k hard-disk/CD-ROM cache


#define	PCDISKFP	FILE*fp
#define	PCERRORFP	NULL


typedef	struct	{

	word	width;								//	width, in pixels
	word	height;								//	height, in pixels
	word	originalx;							//	original x position
	word	originaly;							//	original y position
	byte	planes;								//	depth of the image
	byte	masking;							//	masking type
	byte	compression;						//	compression type
	byte	pad1;								//	unused, should be 0
	word	transpclrl;							//	transparent colour
	byte	xaspect;							//	x aspect ratio
	byte	yaspect;							//	y aspect ratio
	word	pwidth;								//	original page width
	word	pheight;							//	original page height

}bmhdheader;

typedef	enum	{	FORM, ILBM, BMHD, CMAP, TEXT, BODY, PBM, DPPS, CRNG, TINY, CAMG,
					MAXLBMCONTROLS	};

text	*lbmcontroltexts[ MAXLBMCONTROLS ]=	{	"FORM", "ILBM", "BMHD", "CMAP", "TEXT", "BODY", "PBM ", "DPPS", "CRNG",
												"TINY", "CAMG"
											};

/*------------------------------------------------------------------------------------------------------------------------------*/

#define	BMHDHEADERLEN	20
#define	MAXBUFFLEN		100
#define	COLMAPLEN		768

/*------------------------------------------------------------------------------------------------------------------------------*/

bmhdheader*	read_lbm_bmhd_header( FILE *fp );
long	read_rev_long_from_disk( FILE *fp );
long	read_rev_word_from_disk( FILE *fp );
void	lbmerror( text *message, text *con, FILE *fp );

/*------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------*/

long	read_long_from_disk( PCDISKFP )
{
	long	retvalue = 0;
	
	fread( &retvalue, 1, 4, fp );
	return retvalue;
}


/*------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------*/

long	read_word_from_disk( PCDISKFP )
{
	long	retvalue = 0;
	
	fread( &retvalue, 1, 2, fp );
	return retvalue;
}


/*------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------*/

long	read_byte_from_disk( PCDISKFP )
{
	long	retvalue = 0;
	
	fread( &retvalue, 1, 1, fp );
	return retvalue;
}

/*------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------*/

void	read_block_from_disk( void *dest, long length, PCDISKFP )
{
/* 	puts( "read block from disk" );	*/
/* 	printf( "%s\n", dest );	*/
	fread( dest, 1, length, fp );
/* 	puts( "block read OK from disk" );	*/
/* 	printf( "%s\n", dest );	*/
}



/*------------------------------------------------------------------------------------------------------------------------------*/
/*#define	DISPLAYLBMLOADING */
/*#define	DISPLAYLBMLOADINGCOLS	*/

ubyte*	readlbm( text *filename)
{
	PCDISKFP;
	text		control[ 5 ];
	word		i;
	flag		wassame;
	long		length;
	text		textbuffer[ MAXBUFFLEN ];
	word		x, y, len;
	ubyte		gfk ;
	long		iff_type=type_ILBM;
	ubyte		*iffbuffer, *iffwr;
	ubyte		*linebuf, *wr;
	word		width, height;
	ubyte		planes, compression;
	bmhdheader	*header;
	word		blw;

	if (( fp = fopen( filename, "rb" )) == PCERRORFP ) return NULL;

	control[ 0 ] = 'f';
	control[ 1 ] = 'r';
	control[ 2 ] = 'e';
	control[ 3 ] = 'd';
	control[ 4 ] = 0;

	while ( 1 )
		{
		read_block_from_disk( control, 4, fp );
		wassame = NO;

		for ( i=0; i<MAXLBMCONTROLS; i++ )
			{
			if (( strncmp( control, lbmcontroltexts[ i ] ,4 )) == 0 )
				{
				wassame = YES;

				switch ( i )
					{
					case	FORM:
							length = read_rev_long_from_disk( fp );
							break;
					case	CAMG:
					case	TINY:
					case	CRNG:
					case	DPPS:
							length = read_rev_long_from_disk( fp );
							if( (length&1)==1 )	length++;
							for ( x=0; x<length; x++ )
								{
								read_byte_from_disk( fp );
								}
							break;
					case	PBM:
							iff_type=type_PBM;
							break;
					case	ILBM:
							iff_type=type_ILBM;
							break;
					case	BMHD:
							length = read_rev_long_from_disk( fp );
							if ( length == BMHDHEADERLEN )
								{
								header = read_lbm_bmhd_header( fp );
								width = header->width;
								height = header->height;
								planes = header->planes;
								compression = header->compression;
								}
							else
								{
								lbmerror( "HEADER WRONG LENGTH", control, fp );
								return 0;
								}
							break;

					case	CMAP:
							length = read_rev_long_from_disk( fp );
							if ( length <= COLMAPLEN )
								{
								read_block_from_disk( line_buffer, length, fp );
								setlbmpal( line_buffer);
								}
							else
								{
								lbmerror( "TOO MANY COLOURS", control, fp );
								return 0;
								}
							break;

					case	TEXT:
							length = read_rev_long_from_disk( fp );
							if ( length <= MAXBUFFLEN )
								{
								textbuffer[ length ] = 0;
								read_block_from_disk( &textbuffer, length, fp );
								}
							else
								{
								read_block_from_disk( &textbuffer, MAXBUFFLEN-1, fp );
								textbuffer[ MAXBUFFLEN - 1 ] = 0;
								lbmerror("TEXT LENGTH TOO BIG",control,fp);
								return 0;
								}
							break;


					case	BODY:


							length = read_rev_long_from_disk( fp );
							blw = ( width + 7 ) >> 3;

							iffread_width = width;
							iffread_height = height;

							linebuf=line_buffer;

							if (( iffbuffer = ( ubyte* ) heap_malloc( width * height )) == NULL )
								{
								free( linebuf );
								lbmerror( "OUT OF MEMORY #2", control, fp );
								//wait_for_enter_key();
								return 0;
								}


							memset( linebuf, 0, width );
							iffwr = iffbuffer;

							for ( y=0; y<height; y++ )
								{
								if ( compression == 0 )
									{
									read_block_from_disk( linebuf, blw * planes, fp );
									}
								else
									{
									wr = linebuf;
									x = 0;
									while ( x < (blw * planes) )
										{
										gfk = 128;
										while ( gfk == 128 )
											{
											gfk = fgetc( fp );
											}
										if ( gfk >= 128 )
											{
											len = 257 - gfk;
											memset( wr, fgetc( fp ), len );
											wr += len;
											x += len;
											}
										else
											{
											for ( len=0; len <= gfk; len++ )
												{
												*wr++ = fgetc( fp );
												}
											x += gfk + 1;
											}
										}
									}
									
/*	CONVERT SCAN LINES BIT PLANES TO A 256 COLOUR SCANLINE	*/
								if( iff_type==type_ILBM )
					 				asm_blit_planes(linebuf,iffwr,((int) blw),((int) planes));
								else memcpy(iffwr,linebuf,width);

								iffwr += width;
								}
							break;
					}
  				}
			if ( wassame == YES ) break;
			}
	
		if ( wassame == NO )
			{
			lbmerror( "control is unknown", control, fp );
			getch();
			return 0;
			}
		if (( wassame == YES )&&( i == BODY )) break;

		}
	fclose( fp );

	return iffbuffer;
}

/*------------------------------------------------------------------------------------------------------------------------------*/

ubyte		BMHDbuffer[ BMHDHEADERLEN ];

/*------------------------------------------------------------------------------------------------------------------------------*/

bmhdheader*	read_lbm_bmhd_header( FILE *fp )
{
	bmhdheader	*header = ( bmhdheader* )( &BMHDbuffer );

	header->width		=read_rev_word_from_disk( fp );
	header->height		=read_rev_word_from_disk( fp );
	header->originalx	=read_rev_word_from_disk( fp );
	header->originaly	=read_rev_word_from_disk( fp );
	header->planes		=read_byte_from_disk( fp );
	header->masking		=read_byte_from_disk( fp );
	header->compression	=read_byte_from_disk( fp );
	header->pad1		=read_byte_from_disk( fp );
	header->transpclrl	=read_rev_word_from_disk( fp );
	header->xaspect		=read_byte_from_disk( fp );
	header->yaspect		=read_byte_from_disk( fp );
	header->pwidth		=read_rev_word_from_disk( fp );
	header->pheight		=read_rev_word_from_disk( fp );

	iff_width=header->width;
	iff_height=header->height;

#ifdef	DISPLAYLBMLOADING
	printf( "width		%0d\n", header->width );
	printf( "height		%d\n", header->height );
	printf( "original x & y: %d , %d\n", header->originalx, header->originaly );
	printf( "planes		%d\n", header->planes );
	printf( "masking		%d\n", header->masking );
	printf( "compression	%d\n", header->compression );
	printf( "pad1		%d\n", header->pad1 );
	printf( "transpclrl	%d\n", header->transpclrl );
	printf( "x & y aspect ratios: %d , %d\n", header->xaspect, header->yaspect );
	printf( "pwidth %d , pheight %d\n", header->pwidth, header->pheight );
#endif
	return header;
}

/*------------------------------------------------------------------------------------------------------------------------------*/

long	read_rev_long_from_disk( FILE *fp )
{
	long	value1 = 0, value2 = 0;

	value1 = read_rev_word_from_disk( fp );
	value2 = read_rev_word_from_disk( fp );
	
	value2 += ( value1 << 16 );
	return value2;
}
/*------------------------------------------------------------------------------------------------------------------------------*/

long	read_rev_word_from_disk( FILE *fp )
{
	long	value1 = 0, value2 = 0;
	
	value1 = read_byte_from_disk( fp );
	value2 = read_byte_from_disk( fp );

	value2 += ( value1 << 8 );
	return value2;
}

/*------------------------------------------------------------------------------------------------------------------------------*/

void	lbmerror( text *message, text *con, FILE *fp )
{
	puts( "LBM READ ERROR" );
	printf( "LBM READ ERROR with %s , %d %d %d %d\n", con, *(con+0), *(con+1), *(con+2), *(con+3) );
	printf( "%s\n", message );
	fclose( fp );
}

/*------------------------------------------------------------------------------------------------------------------------------*/
