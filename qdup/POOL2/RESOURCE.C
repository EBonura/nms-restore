#include	"globals.h"
#include	"resapi.h"

HRESOURCE RES_open(char *name)
{
	return(1);
}

void RES_close(HRESOURCE res)
{
}

void RES_set_as_default(HRESOURCE res)
{
}

char RES_file_exists(char *name)
{
	return(1);
}

HRESFILE RES_fopen(char *filename)
{
	return(1);
}

void RES_fclose(HRESFILE rf)
{
}

void RES_fseek(HRESFILE rf, signed long offset, int whence)
{
}

int RES_fread(void *ptr, unsigned long size, HRESFILE rf)
{
	return(1);
}

char RES_Dir(char *outstr)
{
	return(1);
}

char *RES_get_word(char *str, HRESFILE rf)
{
	return(1);
}

FILE *RES_GetFP(HRESFILE rf)
{
	return(1);
}

void RES_RLE_offdec(HRESFILE rf, char *dst, unsigned long seek, unsigned long len)
{
}

int RES_load_file_here( char filename[], unsigned char *dataptr, long filelen )
{
	return(1);
}



