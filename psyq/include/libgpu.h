/* PSoXide Psy-Q compatibility layer: the GPU.
   Primitives are the GPU's own command words behind one ordering-table tag
   word (next address in the low 24 bits, payload words in the top 8), so a
   chain of them is what DMA channel 2 walks. The functions are in
   psyq/src/gpu.rs and font.rs. Written from the GPU's documented command set
   and from how NMS's code uses each name. */
#ifndef PSYQ_LIBGPU_H
#define PSYQ_LIBGPU_H
#include <sys/types.h>

typedef struct { short x, y, w, h; } RECT;

typedef struct { u_long tag; } P_TAG;
typedef struct { u_long tag; u_long code[15]; } DR_ENV;

typedef struct {
	RECT    clip;               /* drawing area */
	short   ofs[2];             /* drawing offset */
	RECT    tw;                 /* texture window */
	u_short tpage;              /* texture page and blending */
	u_char  dtd;                /* dither */
	u_char  dfe;                /* draw to the displayed field */
	u_char  isbg;               /* clear the drawing area with r0,g0,b0 */
	u_char  r0, g0, b0;
	DR_ENV  dr_env;
} DRAWENV;

typedef struct {
	RECT    disp;               /* displayed area of VRAM */
	RECT    screen;             /* position and size on the TV */
	u_char  isinter;            /* interlace */
	u_char  isrgb24;            /* 24-bit colour */
	u_char  pad0, pad1;
} DISPENV;

/* primitives */
typedef struct { u_long tag; u_char r0, g0, b0, code; short x0, y0; u_char u0, v0; u_short clut; short w, h; } SPRT;
typedef struct { u_long tag; u_char r0, g0, b0, code; short x0, y0; u_char u0, v0; u_short clut; } SPRT_8, SPRT_16;
typedef struct { u_long tag; u_char r0, g0, b0, code; short x0, y0, w, h; } TILE;
typedef struct { u_long tag; u_char r0, g0, b0, code; short x0, y0; } TILE_1, TILE_8, TILE_16;
typedef struct { u_long tag; u_char r0, g0, b0, code; short x0, y0, w, h; } BLK_FILL;
typedef struct { u_long tag; u_char r0, g0, b0, code; short x0, y0, x1, y1; } LINE_F2;
typedef struct { u_long tag; u_char r0, g0, b0, code; short x0, y0; u_char r1, g1, b1, p1; short x1, y1; } LINE_G2;
typedef struct { u_long tag; u_char r0, g0, b0, code; short x0, y0, x1, y1, x2, y2; } POLY_F3;
typedef struct { u_long tag; u_char r0, g0, b0, code; short x0, y0, x1, y1, x2, y2, x3, y3; } POLY_F4;
typedef struct {
	u_long tag; u_char r0, g0, b0, code;
	short x0, y0; u_char u0, v0; u_short clut;
	short x1, y1; u_char u1, v1; u_short tpage;
	short x2, y2; u_char u2, v2; u_short pad1;
	short x3, y3; u_char u3, v3; u_short pad2;
} POLY_FT4;
typedef struct { u_long tag; u_long code[2]; } DR_MODE;
typedef struct { u_long tag; u_long code[1]; } DR_TPAGE;

/* tag and code */
#define getaddr(p)      (((P_TAG *)(p))->tag & 0x00FFFFFF)
#define setaddr(p, a)   (((P_TAG *)(p))->tag = (((P_TAG *)(p))->tag & 0xFF000000) | ((u_long)(a) & 0x00FFFFFF))
#define getlen(p)       (((P_TAG *)(p))->tag >> 24)
#define setlen(p, n)    (((P_TAG *)(p))->tag = (((P_TAG *)(p))->tag & 0x00FFFFFF) | ((u_long)(n) << 24))
#define getcode(p)      (((u_char *)(p))[7])
#define setcode(p, c)   (((u_char *)(p))[7] = (c))
#define termPrim(p)     setaddr(p, 0xFFFFFFFF)

/* fields */
#define setRECT(r, _x, _y, _w, _h) ((r)->x = (_x), (r)->y = (_y), (r)->w = (_w), (r)->h = (_h))
#define setXY0(p, _x0, _y0)         ((p)->x0 = (_x0), (p)->y0 = (_y0))
#define setXY2(p, _x0, _y0, _x1, _y1) ((p)->x0 = (_x0), (p)->y0 = (_y0), (p)->x1 = (_x1), (p)->y1 = (_y1))
#define setUV0(p, _u0, _v0)         ((p)->u0 = (_u0), (p)->v0 = (_v0))
#define setRGB0(p, _r, _g, _b)      ((p)->r0 = (_r), (p)->g0 = (_g), (p)->b0 = (_b))
#define setWH(p, _w, _h)            ((p)->w = (_w), (p)->h = (_h))

/* texture page and CLUT words */
#define getTPage(tp, abr, x, y) \
	((((tp) & 3) << 7) | (((abr) & 3) << 5) | (((y) & 0x100) >> 4) | (((x) & 0x3FF) >> 6))
#define getClut(x, y)   (((y) << 6) | (((x) >> 4) & 0x3F))

/* primitive set-up */
#define setSprt(p)      (setlen(p, 4), setcode(p, 0x64))
#define setTile(p)      (setlen(p, 3), setcode(p, 0x60))
#define setLineF2(p)    (setlen(p, 3), setcode(p, 0x40))
#define setPolyF4(p)    (setlen(p, 5), setcode(p, 0x28))
#define setPolyFT4(p)   (setlen(p, 9), setcode(p, 0x2C))
#define setSemiTrans(p, abe) ((abe) ? (getcode(p) |= 2) : (getcode(p) &= ~2))
#define setShadeTex(p, tge)  ((tge) ? (getcode(p) |= 1) : (getcode(p) &= ~1))

void SetSprt(SPRT *p);
void SetSprt8(SPRT_8 *p);
void SetSprt16(SPRT_16 *p);
void SetTile(TILE *p);
void SetLineF2(LINE_F2 *p);
void SetPolyF4(POLY_F4 *p);
void SetPolyFT4(POLY_FT4 *p);
void SetSemiTrans(void *p, int abe);
void SetShadeTex(void *p, int tge);
void SetDrawMode(DR_MODE *p, int dfe, int dtd, int tpage, RECT *tw);
void AddPrim(void *ot, void *p);
void AddPrims(void *ot, void *p0, void *p1);
void CatPrim(void *p0, void *p1);
void TermPrim(void *p);

/* GPU */
int  ResetGraph(int mode);
int  SetGraphDebug(int level);
void SetDispMask(int mask);
int  GetODE(void);
int  DrawSync(int mode);
u_long *ClearOTag(u_long *ot, int n);
u_long *ClearOTagR(u_long *ot, int n);
void DrawOTag(u_long *p);
/* not Psy-Q's: DrawOTag walked by the CPU, holding before any primitive
   that would meet the next vertical blank until it is over (gpu.rs) */
void DrawOTagField(u_long *p);
void DrawPrim(void *p);
int  LoadImage(RECT *rect, u_long *p);
int  StoreImage(RECT *rect, u_long *p);
int  MoveImage(RECT *rect, int x, int y);
int  ClearImage(RECT *rect, u_char r, u_char g, u_char b);
DRAWENV *SetDefDrawEnv(DRAWENV *env, int x, int y, int w, int h);
DISPENV *SetDefDispEnv(DISPENV *env, int x, int y, int w, int h);
DRAWENV *PutDrawEnv(DRAWENV *env);
DISPENV *PutDispEnv(DISPENV *env);
/* LoadTPage loads a texture page image (tp 0 4-bit, 1 8-bit, 2 16-bit;
   w in pixels) and returns its texture page word; DrawSyncCallback names a
   function for when drawing ends (stored, see psyq/src/gs.rs) */
u_short LoadTPage(u_long *pix, int tp, int abr, int x, int y, int w, int h);
u_short GetTPage(int tp, int abr, int x, int y);
int  DrawSyncCallback(void (*func)(void));

/* debug font: FntLoad puts an 8x8 font at (tx, ty) in VRAM; FntPrint
   collects text for the stream SetDumpFnt chose; FntFlush draws it */
void FntLoad(int tx, int ty);
int  FntOpen(int x, int y, int w, int h, int isbg, int n);
void SetDumpFnt(int id);
int  FntPrint(const char *fmt, ...);
u_long *FntFlush(int id);

/* libgs calls QD Up's debug paths name (stubs, see psyq/README.md) */
void GsInitGraph(unsigned short x, unsigned short y, unsigned short intmode,
                 unsigned short dith, unsigned short vrammode);
void GsDefDispBuff(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1);
void GsSwapDispBuff(void);
#endif
