/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Phillip Trelford's Heap Code */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

#include "globals.h"

#define MAX_HEAP_BLOCKS 64

char  *heap_block_names_buffer;
long  heap_block_names_index;
long  heap_block_longest_name_length;
long	heap_block_size[MAX_HEAP_BLOCKS];
char  *heap_block_name[MAX_HEAP_BLOCKS];
char	text_unknown[]={"Unknown"};

char	*heap_start;
long	heap_size;
long	heap_blocks;
long	heap_index;
long  heap_last_index;
long	heap_over_error;

void view_heap_allocation(void )
{
	long i;
		
	internal_message("---------------\n");
	internal_message("Heap Allocation\n\n");

	if(heap_blocks>0)
	{
		for(i=0;i<heap_blocks;i++)
		{
			sprintf(message,"%i %s %i\n",i,heap_block_name[i],heap_block_size[i]);
			internal_message(message);
		}

		sprintf(message,"Total Allocation %i\n",heap_index);
		internal_message(message);
	}
	else internal_message("Nothing allocated\n");
	internal_message("---------------\n");
}

flag init_heap(long heap_megs )
{

	heap_size=heap_megs*1048576;
	heap_start=malloc(heap_size);

	heap_block_names_buffer=malloc(128*MAX_HEAP_BLOCKS);
	heap_block_names_index=0;

	heap_blocks=0;

	if(heap_start==NULL)
	{
		printf("COULD NOT ALLOCATE MEMORY\n");
		exit(-1);
	}

	clear_heap();
	return OK;
}

char *add_heap_block_name(char *name)
{
	long length;
	long i;
	char c;

	i=heap_block_names_index;	

	do
	{
		c=*name++;
		heap_block_names_buffer[heap_block_names_index++]=c;
	} while(c!=0);

	length=heap_block_names_index-i;
	if(length>heap_block_longest_name_length) heap_block_longest_name_length=length;

	return &heap_block_names_buffer[i];
}

void clear_heap(void )
{
	long i;

	for(i=0;i<MAX_HEAP_BLOCKS;i++)
	{
		heap_block_size[i]=0;
		heap_block_name[i]=add_heap_block_name(text_unknown);;
	}

	heap_block_names_index=0;
	heap_blocks=0;
	heap_index=0;
	heap_last_index=heap_index;
	//memset(heap_start,heap_size,0);
}

long heap_available(void )
{
	return(heap_size-heap_index);
}

char *heap_malloc(long size)
{
	if(heap_size<(heap_index+size) )
	{
		heap_over_error=(heap_index+size)-heap_size;
		printf("Heap Allocate Error\n");
		return NULL;
	}
	heap_block_size[heap_blocks]=size;
	heap_blocks++;

	heap_last_index=heap_index;
	heap_index+=size;

	return (&heap_start[heap_last_index]);
}

void heap_free_last(void )
{
	heap_index=heap_last_index;
}

char *heap_load_file(char *fname)
{
	FILE *infile;
	char *temp;
	int handle;

	if ( (infile=fopen( fname, "rb")) == NULL )
		{
		printf("File Error %s\n",fname);
		exit(1);
		}

	handle=fileno(infile);

	fsize = filelength(handle);

	heap_block_name[heap_blocks]=add_heap_block_name(fname);

	temp=heap_malloc(fsize);

	if(temp==NULL)
	{
		fclose(infile);
		printf("Malloc Error %s need %ibytes\n",fname,heap_over_error);
		exit(-1);
	}

	fread(temp,1,fsize,infile);

	fclose(infile);

	return(temp);	
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* General purpose load file */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

char *load_file(char *fname)
{
	FILE *infile;
	char *temp;
	int handle;

	if ( (infile=fopen( fname, "rb")) == NULL )
		{
		printf("File Error %s\n",fname);
		exit(1);
		}

	handle=fileno(infile);

	fsize = filelength(handle);

	sprintf(message,"Loaded %s,%i\n",fname,fsize);
	internal_message(message);

	temp=(char *)malloc(fsize);

	if(temp==NULL)
	{
		fclose(infile);
		printf("Malloc Error %s\n",fname);
		exit(-1);
	}

	fread(temp,1,fsize,infile);

	fclose(infile);

	return(temp);	
}


