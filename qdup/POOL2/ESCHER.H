
// Header for Escher 3D engine.


// First, the data types

typedef struct
        {
          int x, y, z;
        } VERTEX;

typedef struct
        {
          long x, y, z;		// RESTORE: 8.24 ball positions, was double
        } VERTEX1;

typedef struct
        {
          int vertex[2];
          int plane[2];
          int orientation[2];
          int start_u[2];
          int start_v[2];
          int end_u[2];
          int end_v[2];
        } LINE;

#define xplane 0
#define yplane 1
#define zplane 2

#define plane_plain   0
#define plane_texture 1
#define plane_chrome  2
#define plane_tiled   3

typedef struct
        {
          char axis, plane_type, visible_side, current_side;

          int colour;
          int brightness;
          int tile_info;

          int gradient[2];
          int offset;
          int radius;
          int centre_vertex;
          int priority;
          int vertex_start;
          int num_vertices;
        } PLANE;

#define object_static		0
#define object_mobile		1
#define object_dummy		2
#define object_ball		3
#define object_mobile_compound	4

typedef struct
        {
          int num_vertices;
          int first_extra_vertex;
          int num_lines;
          int num_planes;
          int radius;
          int priority;
          int num_plane_vertices;
          int first_sorted_plane;
          int last_sorted_plane;

          VERTEX source_pos;
          VERTEX modified_pos;
          VERTEX world_pos;
          VERTEX rotation;

          VERTEX *source_vertex;
          VERTEX *modified_vertex;
          VERTEX *world_vertex;

          LINE  *lines;
          PLANE *planes;
          short int *plane_vertex;

          char object_name[16];
          char colour;
          char visible;
          char selected;
          char sort_type;
          char enabled;
          char object_type;

        } OBJECT;

// routines to initialise engine

void E_Init(char *memory, int size);
void E_AddMesh(char *filename);
int  E_Load();
void E_SetVesaShift(int shift_amount);
void E_LoadMapData();
void E_SetScreenSize( int width, int height);
void E_SetScanlineLength( int length);
void E_SetScreenCentre( int centre_x, int centre_y);
void E_SetPixelAspect( int pixel_squish);
void E_SetZoom( int zoom_value);

// camera manipulation

void E_MoveCamera(VERTEX *pos);
void E_RotateCamera(VERTEX *rot);
//void E_TargetCamera(VERTEX *target);
// void E_ZoomCamera( int zoom_level);
void E_UpdateCamera();

void E_EaseCamera(int on_off);
int  E_FinishedEase(void);

//void E_PlaceCamera(VERTEX *pos, VERTEX *rot);
void E_PositionCamera( VERTEX *target, VERTEX *rotation, int distance);

void E_GetCameraPos(VERTEX *cam_pos);
void E_GetCameraRot(VERTEX *cam_rot);

// object manipulation

void E_MoveObject( int x, int y, int z, OBJECT *object);
void E_RotateObject( int xrot, int yrot, int zrot, OBJECT *object);
OBJECT *E_GetObjectPtr( char *name);
void E_SelectObject(char *name);
void E_EnableObjects(int on_off);
void E_MakeMobile();
void E_MakeMobileCompound();
void E_MakeDummy();
void E_SetRadius(int new_radius);
void E_ScaleObject(OBJECT *obj_ptr, int scale_factor);
void E_GetObjectExtents( OBJECT *obj_ptr, VERTEX *min, VERTEX *max);
void E_WorldRotateObject(OBJECT *obj_ptr);

// dirty zone routines

void E_ClearZones();
void E_AddZone( int x1, int y1, int x2, int y2);
void E_MergeZones();
void E_BuildChunks();
void E_CopyRamToRam(char *source_buffer, char *dest_buffer);
void E_CopyRamToVga(char *ram_buffer);
void E_CopyVgaToRam(char *ram_buffer);
void E_SetZoneBank(int bank_num);

// misc routines

void E_RotateVertex( int xrot, int yrot, int zrot, VERTEX *v);
void E_ReverseRotateVertex( int xrot, int yrot, int zrot, VERTEX *v);
int  E_GetScreenPos(OBJECT *obj, VERTEX *pos);
void E_DrawBall( int x, int y, int radius, char *backscreen, int colour, int shape, int hilite_on_off);
void E_SetVesaPage(int page);
int  E_Cos(int angle);
int  E_Sin(int angle);
void E_SetTexture(char *texture_address);
int  E_WorldToScreen(VERTEX *world_vertex, VERTEX *reply);
OBJECT *E_GetObjectsPtr();
void E_UseBrightnessTable(char *brightness_table);
int SmoothLine( int x1, int y1, int x2, int y2, char colour, char *screen_address);
char *E_PoolTabPtr(void);
void E_RemapColours(char *graphics_ptr, int length);
void E_Reset2DLines();
void E_Draw2DLine( int x1, int y1, int x2, int y2, int colour, int sort_value);

void E_ModifyBall( int x, int y, int radius, int colour, int hilite_on_off, int on_off, int z, int shape_scale, int ball_number);
void E_SortBalls();

// rendering and render control

void E_RenderToScreen(int rotate_on_off, int balls_on_off);
void E_RenderToBuffer(char *buffer_address, int rotate_on_off, int balls_on_off);
void E_RenderPos(int y);
void E_EnableFelt( int on_off);

// RESTORE: not Escher: the PS1 3D renderer written for the restoration
// (escher/) draws each frame's render calls into this ordering table.
void E_SetOrderingTable(unsigned long *ot, int size);
void E_SetView3D(int on);	// RESTORE: 0 = the 2D view: the renderer draws nothing
extern long view_3d;		// GFX.C: 1 = the 3D view, 0 = NMS's 2D view

