//----------------------------------------------------------------------------
#include "globals.h"
#include	"sndglbl.h"
//----------------------------------------------------------------------------

flag	load_file_here( text *fname, ubyte *dest, long length )
{
	FILE	*fp;
	long	filelength;

	if (( fp = open_file_to_read( fname )) == 0 ) return FAIL;
	filelength = get_file_length( fp );

	if ( filelength > length )
		{
		close_the_read_file( fp );
		puts( "ERROR: read file is too big" );
		return FAIL;
		}

	if ( filelength < length )
		{
		puts( "WARNING: read file is less than expected." );
		}

	read_block_from_disk( dest, filelength, fp );
	close_the_read_file( fp );
	return OK;
}

//----------------------------------------------------------------------------
ubyte*	loadafile ( text *fname )
{
	FILE	*fp;
	long	length;
   	ubyte 	*mem;

	if (( fp = open_file_to_read( fname )) == NULL ) return 0;
	length = get_file_length( fp );
	fsize = length;
	
	if (( mem = (ubyte*)malloc( length*4 )) == NULL )
		{
		fclose( fp );
		return 0;
		}

	read_block_from_disk( mem, length*4, fp );
	close_the_read_file( fp );
	return mem;
}
//----------------------------------------------------------------------------

flag	saveafile ( text *fname, ubyte *source, long len )
{
	FILE	*fp;

	if (( fp = open_file_to_save( fname )) == NULL ) return FAIL;

	write_block_to_disk( source, len*4, fp );
	close_the_saved_file( fp );
	return OK;
}


//----------------------------------------------------------------------------

FILE*	open_file_to_save( text *fname )
{
	FILE	*fp;

	if (( fp = fopen( fname, "wb" )) == NULL )	// wb - Write Binary file
		{
		printf( "Failed to open %s to save.\n", fname );
		}

	printf( "Saving file:%s\n", fname );
	return fp;
}

//----------------------------------------------------------------------------

void	close_the_saved_file( FILE *fp )
{
	fclose( fp );
	puts( "Saved ok." );
}

//----------------------------------------------------------------------------

FILE*	open_file_to_read( text *fname )
{
	FILE	*fp;

	if (( fp = fopen( fname, "rb" )) == NULL )		// wb - Read Binary file
		{
		printf( "Failed to open %s to LOAD.\n", fname );
		}

//	printf( "Loading file:%s\n", fname );
	return fp;
}

//----------------------------------------------------------------------------

void	close_the_read_file( FILE *fp )
{
	fclose( fp );
//	puts( "Loaded ok." );
//	wait_for_space_key();
}

//----------------------------------------------------------------------------


long	what_length_is_this_file( text *fname )
{
	FILE	*fp;
	long	length;

	if (( fp = fopen( fname, "rb" )) == NULL ) return NULL;
	length = get_file_length( fp );
	fclose( fp );
	return length;
}

//----------------------------------------------------------------------------

long	get_file_length( FILE *fp )
{
	long	length;
	
	fseek( fp, 0, SEEK_END );
	length = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	return length;
}

//----------------------------------------------------------------------------

void	write_long_to_disk( long value, FILE *fp )
{
	fwrite( &value, 1, 4, fp );
}

//----------------------------------------------------------------------------

void	write_word_to_disk( long value, FILE *fp )
{
	fwrite( &value, 1, 2, fp );
}

//----------------------------------------------------------------------------

void	write_byte_to_disk( long value, FILE *fp )
{
		fwrite( &value, 1, 1, fp );
}

//----------------------------------------------------------------------------

void	write_block_to_disk( void *bufferl, long length, FILE *fp )
{
	fwrite( bufferl, 1, length, fp );
}

//----------------------------------------------------------------------------

long	read_long_from_disk( FILE *fp )
{
	long	retvalue = 0;
	
	fread( &retvalue, 1, 4, fp );
	return retvalue;
}

//----------------------------------------------------------------------------

long	read_word_from_disk( FILE *fp )
{
	long	retvalue = 0;
	
	fread( &retvalue, 1, 2, fp );
	return retvalue;
}

//----------------------------------------------------------------------------

long	read_byte_from_disk( FILE *fp )
{
	long	retvalue = 0;
	
	fread( &retvalue, 1, 1, fp );
	return retvalue;
}

//----------------------------------------------------------------------------

void	read_block_from_disk( void *dest, long length, FILE *fp )
{
	fread( dest, 1, length, fp );
}

//----------------------------------------------------------------------------

