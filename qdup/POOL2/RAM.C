/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

#include	"globals.h"

// RESTORE: was (512*1024).  E_Load (ESCHER.S) takes 539,300 bytes of this
// memory for the Potters US table and the balls (measured: AllocMem's last
// block, plane_data_mem, ended 15,012 bytes past 512 KB, over balls_xy and
// the 3D build's brightness table).  Sized to what E_Load takes; the
// owner's decision.  RAM.C and STV3D.C both define it.
#define	MODEL_MEM	632196	// RESTORE: was 539300 (the US Potters table); E_Load's mem_used after loading, measured: Potters US 539,300, Potters UK 569,888, Crypt 520,852, Square 632,196 (SQUARE.ESH, 142 KB).  The largest, as the tables' choice is made at the option screen
#define MAX_MEMORY_BLOCK 1048576
/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
ubyte	*RAMallocatedbase_addr;
ubyte	*next_available_ram_addr;
int	Allocated_Memory_Block;
ubyte	*memory_block;
ubyte	*last_ram_ptr;
int	last_ram_len;
/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
ubyte	*temporary_available_ram_addr;
ubyte	*last_ram_addr;
int	max_ram_used;
int	min_ram_free;

int	old_free;
int	old_used;
ubyte	*old_addr;

ext	long		get_ram_available( int, int );
ext	ubyte*	get_ram_start( long size );

static	int	_stacksize = 0x4000;
static	int	_ramsize = 0x00200000;

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

void	save_ram_ptr(void)
{
	old_used = ramused;
	old_free = free_bytes_left;
	old_addr = next_available_ram_addr;
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

void	load_ram_ptr(void)
{
	old_used = ramused;
	old_free = free_bytes_left;
	old_addr = next_available_ram_addr;
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

ubyte*	get_ram( long length )
{
	ubyte	*addr;

	if( length > free_bytes_left ) return NULL;

	length = (( length+3 ) & 0xfffffffc);

	ramused += length;

	free_bytes_left -= length;
	addr = next_available_ram_addr;
	next_available_ram_addr += length;
	last_ram_addr = addr;
	if(ramused > max_ram_used)
		{
		max_ram_used = ramused;
		min_ram_free = free_bytes_left;
		}
	return addr;
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

flag	free_last_ram_block( ubyte* address )
{
	int	temp;
	if(last_ram_addr != address) return FAIL;
	temp = next_available_ram_addr - address;
	next_available_ram_addr = address;
	free_bytes_left += temp;
	ramused -= temp;
	return OK;
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

flag	grab_available_ram_for_us( void )
{
	long	ramavailable;

	ramavailable = get_ram_available(_ramsize, _stacksize);

	TOTALramallocated = 0;
	RAMallocatedbase_addr = (ubyte*) get_ram_start( ramavailable );

	TOTALramallocated = ramavailable;

	memory_block = RAMallocatedbase_addr;

	free_all_ram_again_for_us_to_reuse();
	return OK;
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

void	free_all_ram_again_for_us_to_reuse(void)
{
	memset( RAMallocatedbase_addr, 0, TOTALramallocated );
	next_available_ram_addr = RAMallocatedbase_addr;
	free_bytes_left = TOTALramallocated;
	ramused = 0;
	max_ram_used = 0;
}
/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

void Reset_Memory_Ptr(void)
{
  Allocated_Memory_Block = 0;
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

ubyte*  Get_Ram_Ptr( int length )
{
    ubyte *address;

    if ( Allocated_Memory_Block+length > MAX_MEMORY_BLOCK )
     {
		printf("Out of Memory!!\n");
      printf("Gone Over By %d \n", Allocated_Memory_Block - MAX_MEMORY_BLOCK);
//g      exit_game();
     }
   
    address = memory_block+Allocated_Memory_Block;
	length+=3;
	length&=0xfffffffc;
    Allocated_Memory_Block+=length;
	last_ram_ptr = address;
	last_ram_len = length;

  return address;
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

void	Free_Ram_Ptr( ubyte *addr )
{
	if(addr!=last_ram_ptr) return;
	Allocated_Memory_Block-=last_ram_len;
	last_ram_ptr = last_ram_len = 0;
}
/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

void In_Game_Ram_Init(void)
{
   Reset_Memory_Ptr();

//   textures = Get_Ram_Ptr( 2*1048576 );	  
  	steves_memory=Get_Ram_Ptr(MODEL_MEM);

//   replay_last_shot_buffer =(long*) Get_Ram_Ptr(1248576);
 
//  	backscreen_buffer = Get_Ram_Ptr(640*480);
//  	backscreen_buffer1= Get_Ram_Ptr(640*480);

//  	brightness_table=Get_Ram_Ptr(8192);

//   demo_buffer =(word*) Get_Ram_Ptr(61000);	

//	save_shot_buffer = textures;
	balls_xy= ( long* )( ubyte* )Get_Ram_Ptr(2640);	// RESTORE: 8.24, was double
  	brightness_table=Get_Ram_Ptr(8192);	// RESTORE: 3D view, as the PC version (above); the 3D renderer reads it

//	foul_data=Get_Ram_Ptr(307200);
//	txt=Get_Ram_Ptr(307200);

// 	connect_data=Get_Ram_Ptr(3124);

//   amplify_icon = Get_Ram_Ptr(3397);
//   shots_icon = Get_Ram_Ptr(1604);
//   killer_gfx=Get_Ram_Ptr(1415);
//   ball_data=Get_Ram_Ptr(32980);
}

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */

