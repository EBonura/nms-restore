/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Background LBM Chopper to Map & Tile Set */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#define ext
#include	"globals.h"
#include	"svga.h"
#include	"uvbelib.h"
#include	"ram.c"

char filename[128];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Some code */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void add_val(char *source,int length,char val)
{
	int i;
	for(i=0;i<length;i++) source[i]+=val;
}

void threshhold(char *source,int length,char val)
{
	int count=0;
	int i;
	for(i=0;i<length;i++)
		if(source[i]<val) { count++; source[i]=0; }
	printf("Pixels changed=%i\n",count);
}


void remap_area(char *source,int length)
{
	int i;
	for(i=0;i<length;i++)
		source[i]=remap[(source[i])];
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* General purpose load file */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

char *load_file(char *fname)
{
	FILE *infile;
	char *temp;
	int handle,fsize;

	if ( (infile=fopen( fname, "rb")) == NULL )
		{
		printf("File Error");
		exit(1);
		}

	handle=fileno(infile);

	fsize = filelength(handle);

	temp=(char *)malloc(fsize);

	fread(temp,1,fsize,infile);

	fclose(infile);

	return(temp);	
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* LBM Chopper */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// One Pass or Two Pass?
// Optimum size appears to be 16

#define block_dim 16
#define block_size (block_dim*block_dim)

long *map;
char *buffer;
char current_blk[block_size];
long map_size;
long buf_size;

void chop_lbm(char *fname)
{
	int x,y,i;
	int blk_x,blk_y,blk_i,blks;
	int map_i;
	char *dest;
	char *source;
	FILE *infile;
	int	handle;

	source=malloc(iff_width*16);
	infile=fopen( fname,"rb");
	handle=fileno(infile);

	buf_size=0;

	printf("Scanning...\n");
	printf("Pixels = %i\n",iff_width*iff_height);

	printf("RAM Left = %i\n",get_ram_available());
	map_size=(iff_width/block_dim) * (iff_height/block_dim) * 4;
	map=(long *) malloc( map_size );
	printf("RAM Left = %i\n",get_ram_available());

	//getch();


	map_i=0;
	blks=0;

	buffer=(char *) malloc(block_size);
	memset(buffer,128,block_size);
	blks++;
	buf_size+=block_size;
	
	for(blk_y=0;blk_y<((iff_height/block_dim)*block_dim);blk_y+=block_dim)
	{
		fread(source,1,iff_width*16,infile);
		for(blk_x=0;blk_x<((iff_width/block_dim)*block_dim);blk_x+=block_dim)
		{
			// Copy Block To Temporary Buffer
			i=0;
			//blk_i=(blk_y*iff_width)+blk_x;
			blk_i=blk_x;
			for(y=0;y<block_dim;y++)
			{
				memcpy(current_blk+i,source+blk_i,block_dim);
				i+=16;
				blk_i+=iff_width;
			}
			// * Add New Blocks *
			i=0;
			if(blks==0)
			{
				buffer=(char *) malloc(block_size);
				memcpy(buffer,current_blk,block_size);
				blks++; 
				buf_size+=block_size;
			}
			else
			{
				dest=buffer;
			 	for(i=0;i<blks;i++)
				{
					if(*current_blk==*dest)
					{
						if(memcmp(current_blk,dest,block_size)==0)
							break;
					}
					dest+=block_size;
				}
				if(i==blks)
				{
				 	if( (buffer=(char *) realloc(buffer,block_size*(blks+1))) ==NULL )
					{
						printf("No more memory\n");
						getch();
						exit(-1);
					}
					else
					{
						memcpy(buffer+(blks*block_size),current_blk,block_size);
						blks++;
						buf_size+=block_size;
					}
				}
			}
			map[map_i++]=i;
		}
		printf("Y = %i Blks = %i \n",blk_y,blks);
	}
	fclose(infile);
}

void screen_dump(int start_x,int start_y,char *source,int width,int height)
{
	int x,y;
	int index;
	for(y=0;y<height;y++)
	{
		index=640*(y+start_y)+start_x;
		screen_dump_line(source,index,width);
		source+=iff_width;
	}
}

void main( char argc, char* argv[] )
{
	FILE	*outfile;
	char 	*fname_lbm;
	char	*lbm_buf;
	char 	*temp_buf;
	char 	*back_buf;
	short bodge_x,bodge_y;
	int count;
	int i;
	char c,lastc;
	
	if ( argc == 2 ) fname_lbm= argv[ 1 ];
	else 
	{
		printf("Please give name of .LBM file\n");
		exit(-1);
	}

	init_heap(12);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Initialize UNIVBE */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

	if( SVGA_init()==FAIL)
	{
		printf("Could not Initialise SVGA\n");
		exit(-1);
	}

	if(SVGA_is_mode_available(0x101)==NO)
	{
		printf("SVGA 640x480 mode 101h is not available\n");
		SVGA_uninit();
		exit(-1);
	}
	else printf("SVGA 640x480 mode available\n");

	SVGA_set_mode(0x101);

	printf("Loading BACKGROUND...\n");
	sprintf(filename,"%s.LBM",fname_lbm);
	printf("Decompressing BACKGROUND...\n");
//	lbm_buf=load_file(filename);
//	back_buf=decompress_lbm(lbm_buf);
	back_buf=(char *) readlbm(filename);

	set_palette(palette,0,256);
	set_pen(0,63,0,0);

	/*
	printf("Remapping BACKGROUND....\n");
	remap_area(back_buf,iff_width*iff_height);
	printf("Theshholding....\n");
//	threshhold(back_buf,iff_width*iff_height,5);
	printf("Adding 128....\n");
	add_val(back_buf,iff_width*iff_height,128);
	*/

	screen_dump(0,0,back_buf,640,480);

	printf("Creating Temp File....\n");
	sprintf(filename,"%s.TMP",fname_lbm);
	outfile=fopen( filename,"wb");
	fwrite(back_buf,iff_width*iff_height,1,outfile);
	fclose(outfile);
	free(back_buf);

	chop_lbm(filename);

	/*	Gives 1.5Mb
	printf("Calculating compressed size....\n");
	count=0;
	lastc=0;
	for(i=0;i<buf_size;i++)
	{
		c=buffer[i];
		if(c==lastc)
		{
			count++;
			lastc--;
		}
		else lastc=c;
	}
	printf("Count %i\n",count);
	*/

	sprintf(filename,"%s.MAP",fname_lbm);
	outfile=fopen( filename,"wb");
	bodge_x=iff_width/block_dim;
	bodge_y=iff_height/block_dim;
	fwrite(&bodge_x,1,2,outfile);
	fwrite(&bodge_y,1,2,outfile);
	fwrite(map,map_size,1,outfile);
	fclose(outfile);

	sprintf(filename,"%s.BIN",fname_lbm);
	outfile=fopen( filename,"wb");
	fwrite(buffer,buf_size,1,outfile);
	fclose(outfile);

	sprintf(filename,"%s.PAL",fname_lbm);
	outfile=fopen( filename,"wb");
	fwrite(psx_palette,512,1,outfile);
	fclose(outfile);

	//printf("Press a key...\n");
	//getch();

	//UV_exit();

	mode3();
}


