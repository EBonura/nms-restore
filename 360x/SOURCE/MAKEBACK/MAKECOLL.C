#include	"globals.h"
#include	"ram.c"
#include	"ints.c"
#include	"lbm.c"
#include	"svga.c"

char filename[128];

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
	char 	*coll_buf;
	
	if ( argc == 2 ) fname_lbm= argv[ 1 ];
	else 
	{
		printf("Please give name of .LBM file\n");
		exit(-1);
	}

	initsvga(0x101);

	printf("Loading COLLISION...\n");
	sprintf(filename,"%s.LBM",fname_lbm);
//	lbm_buf=load_file(filename);
	printf("Decompressing COLLISION...\n");
//	temp_buf=decompress_lbm(lbm_buf);
	temp_buf=readlbm(filename);
	free(lbm_buf);

	set_pen(0,63,0,0);

	screen_dump(0,0,temp_buf,640,480);

	printf("Compressing COLLISION...\n");
	coll_buf=create_1plane(temp_buf,(iff_width*iff_height));
	free(temp_buf);

	sprintf(filename,"%s.BIN",fname_lbm);
	outfile=fopen( filename,"wb");
	fwrite(coll_buf,(iff_width*iff_height)/8,1,outfile);
	fclose(outfile);

	printf("Press a key...\n");
	getch();

	mode3();
}
