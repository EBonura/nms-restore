/**********************
** Resource File API **
** Mark Dow 3/1996   **
***********************/
// Rev:1.1
// Needs:
//			p_malloc.obj

/* higher level functions - should probably be in another module */

extern unsigned char *RES_loadafile(char filename[]);
extern unsigned char *RES_loadafile1(char filename[]);
extern unsigned char *RES_loadafile2(char filename[]);

extern long RES_get_file_length(char filename[]);
extern int RES_load_file_here( char filename[], unsigned char *dataptr, long filelen );


/* miscellanus strcutures */

typedef struct
{
	FILE *fp;
	char *header;
	unsigned long header_size;
}RESOURCE;

typedef RESOURCE* HRESOURCE;

typedef struct
{
	HRESOURCE res;	// resource this file is in
	unsigned long offset;
	unsigned long lsize;		// logical size, size when uncompressed
	unsigned long psize;		// physical size, size in resource file
	unsigned long seek_pos;
	char flags;
}RESFILE;

typedef RESFILE* HRESFILE;

/* statically allocated itty bitty buffer */
#define RLE_BUF_SIZ		1024


/* API */


// Description: opens a resource file
// Parameters : filename - name of resource to open
// Returns    : NULL on failure else a resource handle
HRESOURCE RES_open(char *);


// Description: close a resource file (frees mem, releases pysical handles etc.)
// Parameters : handle of resource to close
// Returns    : zip
void RES_close(HRESOURCE);


// Description: sets a resource file as the default one. All functions that
//            : do not take a resource handle operate on the default one.
//            : A newly opened resource file will be defaulted by default!
// Parameters : handle of resource file
// Returns    : zip
void RES_set_as_default(HRESOURCE);


// Description: checks if a logical file exists within the resource file
// Parameters : filename - logical filename
// Returns    : TRUE or FALSE, 1 or 0 etc...
char RES_file_exists(char *);


// Description: opens a logical file
// Parameters : filename - name of the logical file to be opened
// Returns    : handle to the file
HRESFILE RES_fopen(char *filename);


// Description: closes a logical file
// Parameters : handle to the file to close
// Returns    : zip
void RES_fclose(HRESFILE rf);


// Description: seek to a position in a file
// Parameters : as its C library counterpart but with the logical file handle
// Returns    : zip
void RES_fseek(HRESFILE rf, signed long offset, int whence);


// Description: reads a number of bytes from the file into memory
//            : if the file is compressed obviously there is a performance
//            : hit, but no extra memory is used during decompression
// Parameters : as read()
// Returns    : number of bytes read
int RES_fread(void *ptr, unsigned long size, HRESFILE rf);


// Description: Gets the file length
// Parameters : file handle
// Returns    : (unsigned long)file length
//unsigned long RES_file_length(HRESFILE rf);
#define RES_file_length(rf) ((rf)->lsize)


// Description: returns the position of the filepointer
// Parameters : file handle
// Returns    : (unsigned long)file pos
//long RES_ftell(HRESFILE rf);
#define RES_ftell(rf) ((rf)->seek_pos)


// Description: Checks to see if the file specified is stored compressed
// Parameters : file handle
// Returns    : TRUE/FALSE
//char RES_rle(HRESFILE rf);
#define RES_rle(rf) (((rf)->flags)&1)


// Description: Returns one filename from the current resource. on each
//            : call the subsequent filename is returned. This function is
//            : first initialised by passing it NULL
// Parameters : destination for the filename (not filled if set to NULL)
// Returns    : returns TRUE while there are still some more filenames to get
char RES_Dir(char *outstr);


// Description: gets a text-word at the current file position, ignores white space
//            : and zeros
// Parameters : place to store word, file handle
// Returns    : NULL if unable to get word
char *RES_get_word(char *str, HRESFILE rf);



//--------------------------------------------------------------------------
// Functions to allow 'C' library operations on the resource file to carry
// out procedures that are not supported by this module. Obviously these
// do not help with compressed files.


// Description: Gets the file pointer of a file. This may be useful
//            : if an operation on a file is required and not supported by
//            : this module. The seek pos is set to the start of the file.
//            : When seeking use SEEK_CUR or else use RES_GetSeekBase() to
//            : get a base value to add to your seek value. DO NOT use
//            : SEEK_END.
//            : DO NOT mix 'C' library and RES_ operations.
//            : The validity of this file pointer is only guarenteed until
//            : the next RES_ call.
// Parameters : File handle
// Returns    : 'C' library file pointer
FILE *RES_GetFP(HRESFILE rf);


// Description: gets the seek base. adding this to the required seek in the
//            : will give the correct seek pos in the resource file.
// Parameters : file handle
// Returns    : (unsigned long)seek base
#define RES_GetSeekBase(x)		((x)->offset)


// Description: 
// Parameters : 
// Returns    : 
void RES_RLE_offdec(HRESFILE rf, char *dst, unsigned long seek, unsigned long len);
