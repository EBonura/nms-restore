#include	"globals.h"
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
extern	G_SPRITE	spr_opts[];
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл

Uint8 ballgfx[]=
{
	0,0,0,0,1,1,1,0,0,0,0,0,
	0,0,1,1,1,1,1,1,1,0,0,0,
	0,1,1,1,1,1,1,1,1,1,0,0,
	0,1,1,1,1,1,1,1,1,1,0,0,
	1,1,1,1,1,1,1,1,1,1,1,0,
	1,1,1,1,1,1,1,1,1,1,1,0,
	1,1,1,1,1,1,1,1,1,1,1,0,
	0,1,1,1,1,1,1,1,1,1,0,0,
	0,1,1,1,1,1,1,1,1,1,0,0,
	0,0,1,1,1,1,1,1,1,0,0,0,
	0,0,0,0,1,1,1,0,0,0,0,0,
};

short	ballcolor[]=
{
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x1f, 0x00, 0x00),		// Red
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),

	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x1f, 0x1f, 0x00),		// Yellow
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),

	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x1f, 0x1f, 0x1f),		// White
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),

	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGBT6_COLOR( 0x00, 0x00, 0x00),		// Black
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),

	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGBT6_COLOR( 0x10, 0x10, 0x10),		// Grey for Pockets
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),
	RGB16_COLOR( 0x00, 0x00, 0x00),

};
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
short	opt_dm[] = { 0x85, 0x87,  0x95, 0x97 };
short	opt_sw[] = {  256,   64,  256,   64  };
short	opt_sh[] = {  256,  256,  224,  224  };
short	opt_sx[] = {    0,  256,    0,  256  };
short	opt_sy[] = {    0,    0,  256,  256  };
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
short	FontPos[] =
{
	0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
	0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
	0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
	0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
	0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
	0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
	0x2500,0x2508,0x2510,0x2518,0x2520,0x2528,0x2530,0x2538,
	0x2540,0x2548,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
	0xffff,0x0000,0x0008,0x0010,0x0018,0x0020,0x0028,0x0030,
	0x0038,0x0040,0x0048,0x0050,0x0058,0x0060,0x1100,0x1108,
	0x1110,0x1118,0x1128,0x1130,0x1138,0x1140,0x1148,0x1158,
	0x1160,0x1168,0x1170,0xffff,0xffff,0xffff,0xffff,0xffff,
	0xffff,0x0000,0x0008,0x0010,0x0018,0x0020,0x0028,0x0030,
	0x0038,0x0040,0x0048,0x0050,0x0058,0x0060,0x1100,0x1108,
	0x1110,0x1118,0x1128,0x1130,0x1138,0x1140,0x1148,0x1158,
	0x1160,0x1168,0x1170,0xffff,0xffff,0xffff,0xffff,0xffff
};
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
short	FontSize[] =
{
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x1008,0x1004,0x1007,0x1006,0x1008,0x1007,0x1007,0x1008,
	0x1006,0x1007,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x1106,0x1106,0x1106,0x1107,0x1106,0x1105,0x1106,
	0x1106,0x1105,0x1107,0x1105,0x1105,0x1108,0x1107,0x1107,
	0x1105,0x1109,0x1106,0x1106,0x1107,0x1106,0x1106,0x110a,
	0x1107,0x1106,0x1107,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x1106,0x1106,0x1106,0x1107,0x1106,0x1105,0x1106,
	0x1106,0x1105,0x1107,0x1105,0x1105,0x1108,0x1107,0x1107,
	0x1105,0x1109,0x1106,0x1106,0x1107,0x1106,0x1106,0x110a,
	0x1107,0x1106,0x1107,0x0000,0x0000,0x0000,0x0000,0x0000
};
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	DrawTitle( void )
{
	RECT	rect;

	SetSprt( &tsp );
	SetSemiTrans( &tsp, NO );
	SetShadeTex( &tsp, ON );
	setWH( &tsp, 114, 121 );
	setXY0( &tsp, 24, 24 );
	setUV0( &tsp, 64, 0);
	tsp.clut = CLUTaddr( 0, 481 );
	setRECT(&rect,0,0,0,0);
	SetDrawMode( &tdm, 0,0,0x87,&rect);
	AddPrim( o_t++, &tsp);
	AddPrim( o_t++, &tdm);
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	DrawReflect( void )
{
	RECT	rect;

	SetSprt( &rsp );
	SetSemiTrans( &rsp, YES );
	SetShadeTex( &rsp, ON );
	setWH( &rsp, 38, 68 );
	setXY0( &rsp, 120, 129 );
	setUV0( &rsp, 64, 121);
	rsp.clut = CLUTaddr( 0, 485 );
	setRECT(&rect,0,0,0,0);
	SetDrawMode( &rdm, 0,0,0x87,&rect);
	AddPrim( o_t++, &rsp);
	AddPrim( o_t++, &rdm);
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	DrawNumber8( void )
{
	RECT	rect;

	SetSprt( &nsp );
	SetSemiTrans( &nsp, YES );
	SetShadeTex( &nsp, ON );
	setWH( &nsp, 68, 162 );
	setXY0( &nsp, 101, 318 );
	setUV0( &nsp, 52, 0);
	nsp.clut = CLUTaddr( 0, 482 );
	setRECT(&rect,0,0,0,0);
	SetDrawMode( &ndm, 0,0,0x88,&rect);
	AddPrim( o_t++, &nsp);
	AddPrim( o_t++, &ndm);
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	DrawReflect1( void )
{
	RECT	rect;

	SetSprt( &rsp1 );
	SetSemiTrans( &rsp1, YES );
	SetShadeTex( &rsp1, ON );
	setWH( &rsp1, 38, 68 );
	setXY0( &rsp1, 120, 129 );
	setUV0( &rsp1, 64, 121);
	rsp1.clut = CLUTaddr( 0, 485 );
	setRECT(&rect,0,0,0,0);
	SetDrawMode( &rdm1, 0,0,0x87,&rect);
	AddPrim( o_t++, &rsp1);
	AddPrim( o_t++, &rdm1);
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	DrawNumber81( void )
{
	RECT	rect;

	SetSprt( &nsp1 );
	SetSemiTrans( &nsp1, YES );
	SetShadeTex( &nsp1, ON );
	setWH( &nsp1, 68, 162 );
	setXY0( &nsp1, 101, 318 );
	setUV0( &nsp1, 52, 0);
	nsp1.clut = CLUTaddr( 0, 482 );
	setRECT(&rect,0,0,0,0);
	SetDrawMode( &ndm1, 0,0,0x88,&rect);
	AddPrim( o_t++, &nsp1);
	AddPrim( o_t++, &ndm1);
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	DrawBackdrop( void )
{
	RECT		rect;
	DR_MODE	*dr;
	SPRT		*spt;
	int		i;
	u_long	*ott;

	setRECT(&rect,0,0,0,0);
	for(i=0;i<4;i++)
		{
		dr = &opt_mode[i];
		spt = &opt_sprs[i];
		SetDrawMode(dr, 0,0,opt_dm[i],&rect);
		SetSprt(spt);
		SetShadeTex(spt,ON);
		SetSemiTrans(spt,OFF);
		setRGB0(spt,0x80,0x80,0x80);
		setWH(spt, opt_sw[i], opt_sh[i]);
		setXY0(spt, opt_sx[i], opt_sy[i]);
		setUV0(spt, 0, 0);
		spt->clut = CLUTaddr(0,480);
		AddPrim( o_t++, spt);
		AddPrim( o_t++, dr);
		}
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	ShowBackdrop( int page )
{
	RECT		rect;
	DR_MODE	*dr;
	SPRT		*spt;
	int		i;
	u_long	*ott;

	PutDispEnv( &Display[ page ].disp );
	PutDrawEnv( &Display[ page ].draw );
	setRECT(&rect,0,0,0,0);
	for(i=0;i<4;i++)
		{
		dr = &opt_mode[i];
		spt = &opt_sprs[i];
		SetDrawMode(dr, 0,0,opt_dm[i],&rect);
		SetSprt(spt);
		SetShadeTex(spt,ON);
		SetSemiTrans(spt,OFF);
		setRGB0(spt,0x80,0x80,0x80);
		setWH(spt, opt_sw[i], opt_sh[i]);
		setXY0(spt, opt_sx[i], opt_sy[i]);
		setUV0(spt, 0, 0);
		spt->clut = CLUTaddr(0,480);
		DrawPrim( dr);
		DrawPrim( spt);
		}
	DrawSync(0);
	VSync(0);
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	DrawIcons()
{
	int		i;
	flag	semi;
	SPRT		*spt;
	DR_MODE	*drm;
	RECT		rect;

	for(i=0;i< MAX_ICONS ;i++)
		{
		spt =&spr_sprs[ i ];
		drm =&spr_mode[ i ];
		setRECT(&rect, 0,0,0,0 );
		SetDrawMode(drm,ON,0,tex[ i ],&rect);
		SetSprt(spt);
		SetShadeTex(spt, ON);
		SetSemiTrans(spt,OFF);
		setRGB0(spt,0x20,0x20,0x20);
		setWH(spt, spw[ i ],sph[ i ]);
		setUV0(spt,spu[ i ],spv[ i ]);
		setXY0(spt,spx[ i ],spy[ i ]);
		spt->clut = CLUTaddr( 0, (486+(5*opt_level)+i) );
		AddPrim( o_t++, spt);
		AddPrim( o_t++, drm);
		}
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	Baize_Felt_Show( u_long *model, int choice );	// RESTORE: new/baizeicon.c
void	Baize_Felt_Restore( void );
void	DrawInsideBall( void )
{
	if(cursor != C_BAIZE) Baize_Felt_Restore();
	switch(cursor)
		{
		case	C_BALLS:
				RotPMD( cdl->ot+64, active_page, ModelsPMD[ BallType+TABLEMODELS ]);
				break;

		case	C_TABLE:
				RotPMD( cdl->ot+64, active_page, ModelsPMD[ TableType ]);
				break;

		case	C_PLAYERS:
				RotPMD( cdl->ot+64, active_page, ModelsPMD[ PlayType+(TABLEMODELS+BALLMODELS) ]);
				break;

		case	C_OPTIONS:
				break;

		case	C_START:
				break;

		case	C_SPEAKER:
				RotPMD( cdl->ot+64, active_page, ModelsPMD[ Speaker ]);
				break;

		case	C_BAIZE:
				Baize_Felt_Show( ModelsPMD[ TableType ], BaizeType );	// RESTORE: owner's decision (2026-11-20): the model carries the chosen baize colour
				RotPMD( cdl->ot+64, active_page, ModelsPMD[ TableType ]);
				break;

		case	C_PAD:
				break;

		case	C_CARD:
				break;

		case	C_TROPHY:
				DisplayScores();
				break;
		}
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	PrintText( text *mess, int x, int y)
{
	ubyte	c;
	int	vx,vy,w,h;
	while((c=*mess++) != 0)
		{
		if(c!=' ')
			{
			vx = FontPos[c] & 0xff;
			vy = FontPos[c] >> 8;
			w = FontSize[c] &0xff;
			h = FontSize[c] >> 8;
			SetSprt(tspr);
			SetShadeTex(tspr,YES);
			SetSemiTrans(tspr,NO);
			setWH(tspr, w, h);
			setUV0(tspr, vx, vy);
			setXY0(tspr, x, y);
			tspr->clut = CLUTaddr( 0, 484);
			AddPrim(o_t, tspr++);
			}
		else w=6;
		x += (w+1);
		}
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	DisplayScores( void )
{
	int	i;
	RECT	rect;
	text	mess[100];

	tspr = &txt_sprs[0];

	for(i=0;i<10;i++)
		{
		sprintf(mess,"score%d  %d",i+1,1000000-(i*50000));
		PrintText( mess, 48, textpos+(i*32) );
		}
	setRECT(&rect,0,0,0,0);
	SetDrawMode( &txt_mode, 0,0,0x98,&rect);
	o_t++;
	AddPrim(o_t++, &txt_mode);

	textpos -= 3;
	if(textpos < -240) textpos = 500;

}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	Title_Loading( void );	// RESTORE: new/viewchoice.c
void	LoadAllWrapperGfx( void )
{
	RECT	rect;
	int	i,j;
	text	tempname[256];
//	BackScreen = LoadTGA("gfx/front.tga");
//	setRECT(&rect, 0,0,320,480);
//	SetVideoRes(MED_RES);
//	LoadImage(&rect, BackScreen);
//	DrawSync(0);
//	free_last_ram_block(BackScreen);

	BackScreen = LoadFile("gfx/backgrnd.psx");
//	BackScreen = LoadFile("gfx/ol.psx");
	ConvertPalette( BackScreen, 256, 0, 480, YES);
	setRECT(&rect, 320,0,160,480);
	LoadImage(&rect, (BackScreen+0x300));
	Free_Ram_Ptr(BackScreen);

	BackScreen = LoadFile("gfx/title.psx");
	ConvertPalette(BackScreen, 256, 0, 481, YES);
	setRECT(&rect, 480,0,57,121);
	LoadImage(&rect, (BackScreen+0x300));
	Free_Ram_Ptr(BackScreen);

	BackScreen = LoadFile("gfx/number8.psx");
	ConvertPalette(BackScreen, 256, 0, 482, YES);
	setRECT(&rect, 538,0,34,162);
	LoadImage(&rect, (BackScreen+0x300));
	Free_Ram_Ptr(BackScreen);

	BackScreen = LoadFile("gfx/font.psx");
	ConvertPalette(BackScreen, 256, 0, 484, YES);
	setRECT(&rect, 512,256,64,64);
	LoadImage(&rect, (BackScreen+0x300));
	Free_Ram_Ptr(BackScreen);
	Title_Loading();	// RESTORE: LOADING on the title picture until these loads are done (new/viewchoice.c)

	BackScreen = LoadFile("gfx/reflect1.psx");
	ConvertPalette(BackScreen, 256, 0, 485, YES);
	setRECT(&rect, 480,121,19,68);
	LoadImage(&rect, (BackScreen+0x300));
	Free_Ram_Ptr(BackScreen);

	LoadIconFiles();
	LoadModelFiles();

}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
// RESTORE: owner's decision (2026-11-13: "add a loading text or something to tell the user that we're
// loading something").  The screen stays as it is for seconds while the game loads (the options screen when the game starts).  This writes NMS's own "PLEASE WAIT" (360X's LOADSAVE.C message,
// the only waiting message in NMS's code; QD Up has no language choice).  Drawn in the panel the option
// screen's other messages use (new/viewchoice.c), over the frozen option screen.
void	Choose_View_Loading( void );
void	Show_Loading( void )
{
	Choose_View_Loading();
}
void	draw_title_screen( void )
{
	RECT	rect;
	text	tempname[256];

	SetVideoRes(MED_RES);
	// RESTORE: BUG FIX. The title comes back after a match (POOL.C's loop) with the game's graphics still in the
	// heap; the picture's 300 KB load on top of them ran past the block (LOAD FAILED, RAM BLOCK). The heap is
	// emptied below once the picture is shown, so nothing in it is needed here: empty it first as well.
	Reset_Memory_Ptr();
	BackScreen = LoadTGA("gfx/front.tga");
	setRECT(&rect, 0,0,320,480);
	LoadImage(&rect, BackScreen);
	DrawSync(0);
	Reset_Memory_Ptr();
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	copy_ball(void)
{
	RECT	rect;
	setRECT(&rect,320,0,6,11);
	LoadImage(&rect, ballgfx);
	DrawSync(0);
	setRECT(&rect,320,511,256,1);
	LoadImage(&rect, ballcolor);
	DrawSync(0);
}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
void	setpallette( short *p, long n )
{
	RECT	rect;
	int	i;
	for(i=256;i<n;i++) if(p[i] == 0x8000) p[i]=0;
	setRECT(&rect,0,480,n,1);
	LoadImage(&rect,(u_long*)p);

}
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл


