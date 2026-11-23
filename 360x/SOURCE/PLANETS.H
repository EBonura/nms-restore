
#ifndef	__H_PLANETS
#define	__H_PLANETS

/**************************************
*          DECLARE DEFINES            *
**************************************/

/**************************************
*         DECLARE VARIABLES           *
**************************************/

extern	MATRIX	GsIDMATRIX,GsWSMATRIX;

/**************************************
*          DECLARE ROUTINES           *
**************************************/

void	Show_Planets(void);
void	Init_Model(void);
void	Init_Texture_Bitmap(UBYTE *currenttim,USHORT xpos,USHORT ypos);
void	Init_Texture_Clut(UBYTE *currenttim,USHORT clutx,USHORT cluty);
void	Read_Model_Data(UBYTE *modeladdr,ULONG modelno);
void	Initialise_World(void);
void	Set_Coordinates(SVECTOR *pos,GsCOORDINATE2 *coor);
void	Change_PageTMD(USHORT x,USHORT y,USHORT px,USHORT py,UBYTE *currentmodel);
void	Warp_To_Planet(void);
void	Zoom_To_Planet(TEXT *planetinfo,SHORT nolines);
void	Initialise_World2(void);

#endif

