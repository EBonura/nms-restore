/* PSoXide Psy-Q compatibility layer: geometry (GTE) types and calls.
   QD Up's PS1 code only sets the GTE up and builds a camera matrix for its
   option-screen models, which the boot path does not draw: the matrix calls
   are stubs that say so on the debug console (psyq/src/stubs.rs). */
#ifndef PSYQ_LIBGTE_H
#define PSYQ_LIBGTE_H
#include <sys/types.h>

#define ONE 4096                /* 1.0 in the GTE's 4.12 fixed point */

typedef struct { short m[3][3]; long t[3]; } MATRIX;
typedef struct { long vx, vy, vz, pad; } VECTOR;
typedef struct { short vx, vy, vz, pad; } SVECTOR;
typedef struct { u_char r, g, b, cd; } CVECTOR;

#define setVector(v, _x, _y, _z) ((v)->vx = (_x), (v)->vy = (_y), (v)->vz = (_z))

void InitGeom(void);
void SetGeomOffset(long ofx, long ofy);
void SetGeomScreen(long h);
MATRIX *RotMatrix(SVECTOR *r, MATRIX *m);
MATRIX *TransMatrix(MATRIX *m, VECTOR *v);
MATRIX *ScaleMatrix(MATRIX *m, VECTOR *v);
void SetRotMatrix(MATRIX *m);
void SetTransMatrix(MATRIX *m);

/* PMD model drawing by primitive type (QD Up's MODELS.C dispatches through
   tables of them; stubs) */
void RotPMD_F3(long *pa, u_long *ot, int otlen, int id, int backc);
void RotPMD_F4(long *pa, u_long *ot, int otlen, int id, int backc);
void RotPMD_G3(long *pa, u_long *ot, int otlen, int id, int backc);
void RotPMD_G4(long *pa, u_long *ot, int otlen, int id, int backc);
void RotPMD_FT3(long *pa, u_long *ot, int otlen, int id, int backc);
void RotPMD_FT4(long *pa, u_long *ot, int otlen, int id, int backc);
void RotPMD_GT3(long *pa, u_long *ot, int otlen, int id, int backc);
void RotPMD_GT4(long *pa, u_long *ot, int otlen, int id, int backc);
void RotPMD_SV_F3(long *pa, long *va, u_long *ot, int otlen, int id, int backc);
void RotPMD_SV_F4(long *pa, long *va, u_long *ot, int otlen, int id, int backc);
void RotPMD_SV_G3(long *pa, long *va, u_long *ot, int otlen, int id, int backc);
void RotPMD_SV_G4(long *pa, long *va, u_long *ot, int otlen, int id, int backc);
void RotPMD_SV_FT3(long *pa, long *va, u_long *ot, int otlen, int id, int backc);
void RotPMD_SV_FT4(long *pa, long *va, u_long *ot, int otlen, int id, int backc);
void RotPMD_SV_GT3(long *pa, long *va, u_long *ot, int otlen, int id, int backc);
void RotPMD_SV_GT4(long *pa, long *va, u_long *ot, int otlen, int id, int backc);
#endif
