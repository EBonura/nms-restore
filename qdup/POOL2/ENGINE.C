#include	"globals.h"

int	E_FinishedEase( void )
{
	return	YES;
}

//OBJECT *E_GetObjectPtr( char *name)
//{
//	return((OBJECT*)get_ram(sizeof(OBJECT)));
//}

int  E_Cos(int angle)
{
	return(1);
}

int  E_Sin(int angle)
{
	return(1);
}

void E_EaseCamera(int on_off)
{
}

void E_PositionCamera( VERTEX *target, VERTEX *rotation, int distance)
{
}

void E_MakeMobile()
{
}

void E_RotateVertex( int xrot, int yrot, int zrot, VERTEX *v)
{
}

int  E_GetScreenPos(OBJECT *obj, VERTEX *pos)
{
	return(1);
}

void E_MoveObject( int x, int y, int z, OBJECT *object)
{
}

void E_MakeMobileCompound()
{
}

void E_SelectObject(char *name)
{
}

void E_MakeDummy()
{
}

void E_ScaleObject(OBJECT *obj_ptr, int scale_factor)
{
}

void E_RotateObject( int xrot, int yrot, int zrot, OBJECT *object)
{
}

void E_EnableObjects(int on_off)
{
}

void E_RenderToScreen(int rotate_on_off, int balls_on_off)
{
}

void E_RenderToBuffer(char *buffer_address, int rotate_on_off, int balls_on_off)
{
}

void E_RenderPos(int y)
{
}

void E_EnableFelt( int on_off)
{
}

//void E_Init(char *memory, int size)
//{
//	mesh_names = (ubyte*)get_ram(1024);
//	mesh_name_ptr = mesh_names;
//	total_objects = 0;
//}

//void E_AddMesh(char *filename)
//{
//	long	fp;
//	long	nobj;
//	sprintf(mesh_name_ptr,"%s",filename);
//	mesh_name_ptr += 64;
//	fp = OpenFile( filename );
//	read_block_from_disk(&nobj,4,fp);
//	CloseFile( fp );
//	total_objects += nobj;
//}

//int  E_Load()
//{
//
//	return(1);
//}

void E_SetVesaShift(int shift_amount)
{
}

void E_LoadMapData()
{
}

void E_SetScreenSize( int width, int height)
{
}

void E_SetScanlineLength( int length)
{
}

void E_SetScreenCentre( int centre_x, int centre_y)
{
}

void E_SetPixelAspect( int pixel_squish)
{
}

void E_SetZoom( int zoom_value)
{
}

void E_UseBrightnessTable(char *brightness_table)
{
}

void E_RemapColours(char *graphics_ptr, int length)
{
}

void E_Reset2DLines()
{
}

void E_Draw2DLine( int x1, int y1, int x2, int y2, int colour, int sort_value)
{
}

void E_ClearZones()
{
}

void E_AddZone( int x1, int y1, int x2, int y2)
{
}

void E_MergeZones()
{
}

void E_BuildChunks()
{
}

void E_CopyRamToRam(char *source_buffer, char *dest_buffer)
{
}

void E_CopyRamToVga(char *ram_buffer)
{
}

void E_CopyVgaToRam(char *ram_buffer)
{
}

void E_SetZoneBank(int bank_num)
{
}

void E_GetCameraPos(VERTEX *cam_pos)
{
}

void E_GetCameraRot(VERTEX *cam_rot)
{
}

void E_WorldRotateObject(OBJECT *obj_ptr)
{
}

void E_UpdateCamera()
{
}

void E_DrawBall( int x, int y, int radius, char *backscreen, int colour, int shape, int hilite_on_off)
{
}

void E_ModifyBall( int x, int y, int radius, int colour, int hilite_on_off, int on_off, int z, int shape_scale, int ball_number)
{
}

void E_SortBalls()
{
}

void E_SetTexture(char *texture_address)
{
}

char *E_PoolTabPtr(void)
{
	return(1);
}

void E_GetObjectExtents( OBJECT *obj_ptr, VERTEX *min, VERTEX *max)
{
}













