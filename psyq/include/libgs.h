/* PSoXide Psy-Q compatibility layer: the extended graphics library (libgs)
   as 360X uses it: double-buffered ordering tables, 2D sprites and lines,
   and the TMD model path of the planet screens (coordinate systems, a view,
   flat lights, fog). psyq/src/gs.rs. Written from how NMS's code uses each
   name and from what NMS's 1996 build of 360X observably does (field
   offsets and arithmetic read from its MAIN.CPE with MAIN.SYM), never from
   Sony's headers or documentation. */
#ifndef PSYQ_LIBGS_H
#define PSYQ_LIBGS_H
#include <sys/types.h>
#include <libgpu.h>
#include <libgte.h>

/* -------------------------------------------------------------- packets */

typedef unsigned char PACKET;           /* GsSetWorkBase's packet area */

/* one ordering-table entry: the next primitive's address (24 bits) and a
   length byte, as the GPU's linked-list DMA reads it */
typedef struct {
	unsigned p : 24;
	unsigned char num : 8;
} GsOT_TAG;

/* an ordering table: 1 << length entries at org, drawn from the last entry
   to the first (GsClearOt links it backwards), so a lower priority number
   is drawn later, in front */
typedef struct {
	unsigned long length;
	GsOT_TAG *org;
	unsigned long offset;   /* priority of entry 0 */
	unsigned long point;
	GsOT_TAG *tag;          /* first entry the GPU walks */
} GsOT;

/* attribute bits of sprites, lines and objects */
#define GsDOFF       (1u << 31) /* not drawn */
#define GsALON       (1u << 30) /* semi-transparent */
#define GsAZERO      (0u << 28) /* blend 50% back + 50% front */
#define GsAONE       (1u << 28) /* 100% back + 100% front */
#define GsATWO       (2u << 28) /* 100% back - 100% front */
#define GsATHREE     (3u << 28) /* 100% back + 25% front */
#define GsROTOFF     (1u << 27) /* GsSortSprite: no rotation or scale */
#define GsTMDIV      (1u << 9)
#define GsLLMOD      (1u << 3)
#define GsLDIM0      0

/* --------------------------------------------------------------- sprites */

typedef struct {
	unsigned long attribute;    /* bits 24-25 colour mode (0 4-bit, 1 8-bit, 2 16-bit),
	                               bit 23 flip x, bit 22 flip y (GsSortSprite),
	                               bit 6 texture not shaded, and the bits above */
	short x, y;                 /* on screen, from the origin GsInit3D puts mid-screen */
	unsigned short w, h;
	unsigned short tpage;       /* texture page number (0-31) */
	unsigned char u, v;
	short cx, cy;               /* CLUT position in VRAM */
	unsigned char r, g, b;      /* 128 = the texture as it is */
	short mx, my;               /* rotation and scaling centre, from (u, v) */
	short scalex, scaley;       /* 4096 = 1.0 */
	long rotate;                /* 4096 = one degree */
} GsSPRITE;

typedef struct {
	unsigned long attribute;
	short x0, y0, x1, y1;
	unsigned char r, g, b;
} GsLINE;

/* a flat-coloured rectangle; attribute bit 30 and bits 28-29 as for lines (semi-transparency and its blend) */
typedef struct {
	unsigned long attribute;
	short x, y;                 /* on screen, from the origin GsInit3D puts mid-screen */
	unsigned short w, h;
	unsigned char r, g, b;
} GsBOXF;

typedef struct {
	unsigned long attribute;
	short x0, y0, x1, y1;
	unsigned char r0, g0, b0;
	unsigned char r1, g1, b1;
} GsGLINE;

/* a TIM image as GsGetTimInfo describes it */
typedef struct {
	unsigned long pmode;        /* TIM flags: bits 0-2 mode, bit 3 CLUT present */
	short px, py;
	unsigned short pw, ph;      /* width in VRAM half-words */
	unsigned long *pixel;
	short cx, cy;
	unsigned short cw, ch;
	unsigned long *clut;
} GsIMAGE;

/* ------------------------------------------------------- 3D: coordinates */

typedef struct GsCOORDINATE2 {
	unsigned long flg;          /* 0 = changed since its world matrix was worked out */
	MATRIX coord;               /* local matrix, relative to super */
	MATRIX workm;               /* local-to-world, as last worked out */
	struct GsCOORDINATE2 *super;
	struct GsCOORDINATE2 *sub;
} GsCOORDINATE2;

#define WORLD ((GsCOORDINATE2 *)0)

/* the viewpoint (vp) looking at the reference point (vr), twisted rz
   (4096 = one degree) about the line of sight, in super's coordinates */
typedef struct {
	long vpx, vpy, vpz;
	long vrx, vry, vrz;
	long rz;
	GsCOORDINATE2 *super;
} GsRVIEW2;
typedef GsRVIEW2 GsRVIEW;

/* a flat (directional) light: the direction it shines, and its colour */
typedef struct {
	long vx, vy, vz;
	unsigned char r, g, b;
} GsF_LIGHT;

/* depth cueing (fog): the GTE's DQA and DQB, and the far colour */
typedef struct {
	short dqa;
	long dqb;
	unsigned char rfc, gfc, bfc;
} GsFOGPARAM;

/* a TMD object linked for drawing */
typedef struct {
	unsigned long attribute;
	GsCOORDINATE2 *coord2;
	unsigned long *tmd;         /* the object's entry in the TMD's object table */
	unsigned long id;
} GsDOBJ2;

/* ------------------------------------------------------------- globals */

extern DRAWENV GsDRAWENV;           /* the drawing environment GsSwapDispBuff sets */
extern DISPENV GsDISPENV;           /* the display GsSwapDispBuff shows */
extern MATRIX GsIDMATRIX;           /* identity */
extern MATRIX GsWSMATRIX;           /* world to screen, from GsSetRefView2 */

/* ------------------------------------------------------------- functions */

void GsInitGraph(unsigned short x, unsigned short y, unsigned short intmode,
                 unsigned short dith, unsigned short vrammode);
void GsDefDispBuff(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1);
void GsSwapDispBuff(void);
int  GsGetActiveBuff(void);
void GsSetWorkBase(PACKET *base);
void GsClearOt(unsigned short offset, unsigned short point, GsOT *ot);
void GsDrawOt(GsOT *ot);
void GsInit3D(void);

void GsSortFastSprite(GsSPRITE *sp, GsOT *ot, unsigned short pri);
void GsSortSprite(GsSPRITE *sp, GsOT *ot, unsigned short pri);
void GsSortLine(GsLINE *lp, GsOT *ot, unsigned short pri);
void GsSortGLine(GsGLINE *lp, GsOT *ot, unsigned short pri);
void GsSortBoxFill(GsBOXF *bp, GsOT *ot, unsigned short pri);
void GsGetTimInfo(unsigned long *tim, GsIMAGE *im);

void GsInitCoordinate2(GsCOORDINATE2 *super, GsCOORDINATE2 *base);
void GsGetLw(GsCOORDINATE2 *coord, MATRIX *m);
void GsGetLs(GsCOORDINATE2 *coord, MATRIX *m);
void GsSetLsMatrix(MATRIX *m);
void GsSetLightMatrix(MATRIX *m);
int  GsSetRefView2(GsRVIEW2 *view);
int  GsSetRefView(GsRVIEW *view);
void GsSetProjection(long h);
void GsSetNearClip(long near);
int  GsSetFlatLight(int id, GsF_LIGHT *light);
void GsSetAmbient(long r, long g, long b);
void GsSetLightMode(int mode);
void GsSetFogParam(GsFOGPARAM *fog);
void GsMapModelingData(unsigned long *p);
void GsLinkObject2(unsigned long tmd, GsDOBJ2 *obj, int n);
void GsLinkObject4(unsigned long tmd, GsDOBJ2 *obj, int n);
void GsSortObject2(GsDOBJ2 *obj, GsOT *ot, int shift);
void GsSortObject4(GsDOBJ2 *obj, GsOT *ot, int shift, unsigned long *scratch);
#endif
