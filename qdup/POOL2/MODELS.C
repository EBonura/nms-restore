#include	"globals.h"
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	InitCamera( void )
{
	scaler = ONE;

	camera.vx=ONE/10;
	camera.vy=ONE/4;
	camera.vz=0;
	distance=SCR_Z*14;
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
// ang = rotation angle
// vec = camera position
// mat = transformation matrix
void	SetCameraPMD(SVECTOR *ang, VECTOR *vec, MATRIX *mat, int scale)
{
	VECTOR	svec;		// Scale
	VECTOR	tvec;		// Transformation
	SVECTOR	rvec;		// Rotation Angle

	rvec.vx = vec->vx;
	rvec.vy = vec->vy;
	rvec.vz = vec->vz;

	tvec.vx = tvec.vy = 0;
	tvec.vz = distance;

	setVector(&svec, scale, scale, scale);
	RotMatrix(&rvec,mat);		// Rotation
	TransMatrix(mat, &tvec);	// Transformation
	ScaleMatrix(mat, &svec);	// Scale
	SetTransMatrix(mat);
	SetRotMatrix(mat);
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
/* independent vertex type (8 type) */
static void (*PMD_func[])() =
{
	RotPMD_FT3,	RotPMD_FT4,	RotPMD_GT3,	RotPMD_GT4,
	RotPMD_F3,	RotPMD_F4,	RotPMD_G3,	RotPMD_G4,
};
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
/* shared vertex type (8 type) */
static void (*PMD_SV_func[])() =
{
	RotPMD_SV_FT3,	RotPMD_SV_FT4,	RotPMD_SV_GT3,	RotPMD_SV_GT4,
	RotPMD_SV_F3,	RotPMD_SV_F4,	RotPMD_SV_G3,	RotPMD_SV_G4,
};
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	RotPMD(u_long *ot, int id, u_long *ModelPMD)
{
	int	i,j;
	long	*PMDTOP;			// PMD file PRIMITIVE Gp top address
	long	*PMDWC;			// PMD file word counter
	long	POINTER;			// PMD file POINTER
	long	*PTOP;			// PMD file PRIMITIVE Gp Block top address
	long	ID;				// PMD file ID
	long	*PRIMTOP;		// PMD file PRIMITIVE Gp top address
	long	NOBJ;				// PMD file NOBJ
	long	NPTR;				// PMD file NPTR
	long	TYPE_NPACKET;	// PMD file PRIMITIVE Gp header
	short	TYPE;				// PMD file PRIMITIVE Gp type
	short	type;				// PMD file PRIMITIVE Gp local type
	long	*SVTOP;			// PMD file Vertex top address
	long	BACKC;			// PMD file Back clip ON/OFF flag

	PMDWC = PMDTOP = (long *)ModelPMD;

	ID = *PMDWC++;
	PTOP = PMDTOP + ((*PMDWC++)>>2);
	SVTOP = PMDTOP + ((*PMDWC++)>>2);
	NOBJ= *PMDWC++;

	for(i = 0;i < NOBJ; i++)
		{
		NPTR = *PMDWC++;
		for(j = 0;j < NPTR; j++)
			{
			POINTER= *PMDWC++;
			PRIMTOP= PMDTOP+(POINTER>>2);
			TYPE_NPACKET= *(PRIMTOP);
			TYPE_NPACKET|=0x100000;
			*(PRIMTOP)=TYPE_NPACKET;
			TYPE= TYPE_NPACKET>>16;
			type= TYPE&0x000f;
			BACKC= (TYPE&0x0020)>>5;

			if (type < 0x8)
				(*PMD_func[type])(PRIMTOP, o_t, OT_LENGTH-1, id, BACKC);
			else if (type < 0x10)
				(*PMD_SV_func[type-8])(PRIMTOP, SVTOP, o_t, OT_LENGTH-1, id, BACKC);	// RESTORE: was PMD_func[type], past its 8 entries; the 1996 build reached PMD_SV_func[type-8] only because that array follows it in memory
			}
		}
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
