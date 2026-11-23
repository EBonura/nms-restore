/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Global Flags */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

long 		buf;
long 		rope_length;
long 		ship_mass;
long 		ball_mass;
long 		gravity_factor;
double 	rope_max;
double	rope_min;


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Globals for ship */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

long  	craftx,crafty;
long		craft;
long		pod_no;
long		frac_craft_x,frac_craft_y;

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Globals for FIRE */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#define MAX_FIRE 8

sprite_sequence fire;
long  	fire_on[MAX_FIRE];
long  	fire_frame[MAX_FIRE];
long  	fire_x[MAX_FIRE];
long  	fire_y[MAX_FIRE];
long  	fire_dx[MAX_FIRE];
long  	fire_dy[MAX_FIRE];
long  	fire_bx[MAX_FIRE];
long  	fire_by[MAX_FIRE];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Lines */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */


spritestruct sprites[MAX_SPRITES];

sprite_sequence seqdata[NAME_END];

long 			fsize;
fnames_list *sprite_fnames;

long 			iff_width;
long 			iff_height;

Uint16 sprites_still[]=
{

	NAME_portdu,
	NAME_portuu,
	NAME_portlu,
	NAME_portru,
	NAME_portul,
	NAME_portur,
	NAME_portdl,
	NAME_portll,
	NAME_portrl,

	NAME_v1_1,
	NAME_v1_2,
	NAME_v1_3,
	NAME_v1_4,
	NAME_v1_5,
	NAME_v1_6,
	NAME_v1_7,
	NAME_v1_8,
	NAME_v1_9,
	NAME_v1_10,
	NAME_v1_11,
	NAME_v1_12,
	NAME_v1_13,

	NAME_v2_1,
	NAME_v2_2,
	NAME_v2_3,
	NAME_v2_4,
	NAME_v2_5,
	NAME_v2_6,
	NAME_v2_7,
	NAME_v2_8,
	NAME_v2_9,
	NAME_v2_10,

	NAME_v4_1,
	NAME_v4_2,
	NAME_v4_3,
	NAME_v4_4,
	NAME_v4_5,
	NAME_v4_6,
	NAME_v4_7,
	NAME_v4_8,
	NAME_v4_9,
	NAME_v4_10,
	NAME_v4_11,
	NAME_v4_12,

	NAME_v5_1,
	NAME_v5_2,
	NAME_v5_3,
	NAME_v5_4,
	NAME_v5_5,
	NAME_v5_6,
	NAME_v5_7,
	NAME_v5_8,
	NAME_v5_9,
	NAME_v5_10,
	NAME_v5_11,
	NAME_v5_12,

	NAME_v6_1,
	NAME_v6_2,
	NAME_v6_3,
	NAME_v6_4,
	NAME_v6_5,
	NAME_v6_6,
	NAME_v6_7,
	NAME_v6_8,
	NAME_v6_9,
	NAME_v6_10,
	NAME_v6_11,
	NAME_v6_12,

	
	NAME_vinel11,
	NAME_vines12,
	NAME_vine14,
	NAME_vine15,
	NAME_vine16,
	
	NAME_vinel22,
	NAME_vinel23,
	NAME_vinel24,
	NAME_vinel26,
	NAME_vtestl2,

	NAME_vinel31,
	NAME_vinel34,
	NAME_vinel36,

	NAME_vinel42,
	NAME_vinel43,
	NAME_vinel44,
	NAME_vinel46,
	NAME_vtestl4,

	NAME_vinel51,
	NAME_vinel52,
	NAME_vinel53,
	NAME_vinel54,
	NAME_vinel56,
	NAME_vtestl5,

	NAME_vinel61,
	NAME_vinel62,
	NAME_vinel63,
	NAME_vinel64,
	NAME_vinel66,
	NAME_vtestl6,

	
	NAME_END
};
