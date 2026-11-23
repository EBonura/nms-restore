/*-------------------------------------------------------------------------*/
/*      Workarea assignment customize file                                 */
/*          for SGL ver. 2.10 (default)                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#include	"sl_def.h"

/*---- [1.This part must not be modified] ---------------------------------*/
#define		SystemWork		0x060ffc00			/* System Variable         */

/*---- [2.This part must not be modified] ---------------------------------*/

#define		MAX_VERTICES	   0	/* number of vertices that can be used */
#define		MAX_POLYGONS	 500	/* number of vertices that can be used */
#define		MAX_EVENTS		  	1	/* number of events that can be used   */
#define		MAX_WORKS		 	1	/* number of works that can be used    */

#define		WORK_AREA		0x060e0000			/* SGL Work Area           */

//#define		trans_list		SystemWork-0x0100	/* DMA Transfer Table      */
#define		pcmbuf			SoundRAM+0x78000	/* PCM Stream Address      */
#define		PCM_SIZE			0x8000				/* PCM Stream Size         */

#define		master_stack	SystemWork			/* MasterSH2 StackPointer  */
#define		slave_stack		0x06001e00			/* SlaveSH2  StackPointer  */

/*---- [3.Macro] ----------------------------------------------------------*/
#define		_Byte_			sizeof(Uint8)
#define		_Word_			sizeof(Uint16)
#define		_LongWord_		sizeof(Uint32)
#define		_Sprite_			(sizeof(Uint16) * 18)

#define		AdjWork(pt,sz,ct)	(pt + (sz) * (ct))

/*---- [4.Work Area] ------------------------------------------------------*/
    enum workarea
	 {
        sort_list  = WORK_AREA ,
        zbuffer    = (sort_list + 0x2000 + 0xf0),
        spritebuf  = AdjWork(zbuffer   , _LongWord_, 512) ,
        pbuffer    = AdjWork(spritebuf , _Sprite_, (MAX_POLYGONS + 6) * 2) ,
        clofstbuf  = AdjWork(pbuffer   , _LongWord_ * 4, MAX_VERTICES) ,
        commandbuf = 0x060ec000 ,
        NextEntry  = AdjWork(commandbuf, _LongWord_ * 8, MAX_POLYGONS)
    } ;

#define		trans_list		sort_list + 0x2000	/* DMA Transfer Table      */

/*---- [5.Variable area ] -------------------------------------------------*/

    const void*   MasterStack   = (void*)(master_stack) ;
    const void*   SlaveStack    = (void*)(slave_stack) ;
    const Uint16  MaxVertices   = MAX_VERTICES ;
    const Uint16  MaxPolygons   = MAX_POLYGONS ;
    const Uint16  EventSize     = sizeof(EVENT) ;
    const Uint16  WorkSize      = sizeof(WORK) ;
    const Uint32  SortListSize  = 0x2000;
    const Uint32  SpriteBufSize = _Sprite_ * (MAX_POLYGONS + 6) * 2 ;
    const Uint32  PCM_WkSize    = PCM_SIZE ;

    const void*   PCM_Work      = (void*)(pcmbuf) ;
    const Uint16  MaxEvents     = MAX_EVENTS ;
    const Uint16  MaxWorks      = MAX_WORKS ;

    const void*   SortList      = (void*)(sort_list) ;
    const void*   TransList     = (void*)(trans_list) ;
    const void*   Zbuffer       = (void*)(zbuffer) ;
    const void*   SpriteBuf     = (void*)(spritebuf) ;
    const void*   Pbuffer       = (void*)(pbuffer) ;
    const void*   CLOfstBuf     = (void*)(clofstbuf) ;
    const void*   CommandBuf    = (void*)(commandbuf) ;

    EVENT  EventBuf[MAX_EVENTS] ;
    WORK   WorkBuf[MAX_WORKS] ;
    EVENT* RemainEvent[MAX_EVENTS] ;
    WORK*  RemainWork[MAX_WORKS] ;

/*------------------------------------------------------------------------*/
