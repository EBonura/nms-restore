/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* CD File stuff */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

FILE *CD_file;
long CD_handle;
long CD_cache_remainder;
long CD_cache_remainder_index;
char CD_cache[2048];

long CD_fopen(char *fname)
{
	CD_file=fopen(fname,"rb");
	if(CD_file==NULL) return NULL;
	CD_handle=fileno(CD_file);
	CD_cache_remainder=0;
	CD_cache_remainder_index=0;

	return CD_handle;
}

void CD_fclose(void )
{
	fclose(CD_file);
}

void CD_fread(char *dest,long length)
{
	long temp;
	long i;
	// fread(dest,1,length,CD_file);

	i=CD_cache_remainder;
	if(length<CD_cache_remainder) i=length;

	memcpy(dest,CD_cache+CD_cache_remainder_index,i);
	length-=i;
	dest+=i;

	temp=length/2048;
	fread(dest,1,temp*2048,CD_file);
	dest+=temp*2048;
	length-=temp*2048;
	
	/*
	while(length>2048)
	{
		fread(dest,1,2048,CD_file);
		dest+=2048;
		length-=2048;
	}
	*/

	fread(CD_cache,1,2048,CD_file);
	memcpy(dest,CD_cache,length);
	CD_cache_remainder_index=length;
	CD_cache_remainder=2048-length;
} 


