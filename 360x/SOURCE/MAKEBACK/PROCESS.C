/* ฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿ */
/* PROCESS SPRITES AI */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

void 	get_reference_point(long i,long point,long var_x,long var_y);
void 	adopt_inrange_child(long father,long range);
long	foster_parent;

#include "globals.h"

// This needs to be extended to have fields:- killable and moveable
#define HIGH 1
#define LOW  0

// This attribute table could be compiled from a text file as per tanks
// Lets do it...

/*
attribstruc sprites_attribs[NAME_END]=
{
	NO,NO,LOW,		//NAME_NULL,
	YES,NO,LOW,		//NAME_CSHIP,
	YES,NO,LOW,		//NAME_FIRE01,
	YES,NO,LOW,		//NAME_CURSOR1,
	YES,NO,LOW,		//NAME_pod4,
	YES,NO,LOW,		//NAME_box,
	YES,NO,LOW,		//NAME_BAR_L,
	YES,NO,LOW,		//NAME_BAR_R,

	YES,NO,LOW,		//NAME_AGMINE,
	YES,NO,LOW,		//NAME_CHAIN3,
	YES,NO,LOW,		//NAME_CHAINA,
	YES,NO,LOW,		//NAME_CHAINB,
	YES,NO,LOW,		//NAME_EBALL,
	YES,NO,LOW,		//NAME_SPFIRE,

	YES,NO,LOW,		//NAME_GUIDED,
	YES,NO,LOW,		//NAME_BUCK01,
	YES,NO,LOW,		//NAME_EBALLL,
	YES,NO,LOW,		//NAME_EBALL_M,
	YES,NO,LOW,		//NAME_EBALL_S,
	YES,NO,LOW,		//NAME_SMINE1,
	YES,NO,LOW,		//NAME_SMINE2,
	YES,NO,LOW,		//NAME_SMINE3,

	YES,NO,LOW,		//NAME_R1_HIT,
	YES,NO,LOW,		//NAME_R2_HIT,
	YES,NO,LOW,		//NAME_R3_HIT,	
	YES,NO,LOW,		//NAME_R4_HIT,
	YES,NO,LOW,		//NAME_G1_HIT,
	YES,NO,LOW,		//NAME_G2_HIT,
	YES,NO,LOW,		//NAME_G3_HIT,
	YES,NO,LOW,		//NAME_G4_HIT,
	YES,NO,LOW,		//NAME_W1_HIT,
	YES,NO,LOW,		//NAME_W2_HIT,
	YES,NO,LOW,		//NAME_W3_HIT,	
	YES,NO,LOW,		//NAME_W4_HIT,

	YES,NO,LOW,		//NAME_XB_FLOP1,
	YES,NO,LOW,		//NAME_XB_FLOP2,
	YES,NO,LOW,		//NAME_XB_FLOP3,
	YES,NO,LOW,		//NAME_XB_FLOP4,
	YES,NO,LOW,		//NAME_XB_FLOP5,
	YES,NO,LOW,		//NAME_XB_FLOP6,

	YES,NO,LOW,		//NAME_NXA,
	YES,NO,LOW,		//NAME_NXB,
	YES,NO,LOW,		//NAME_NXC,
	YES,NO,LOW,		//NAME_NXD,
	YES,NO,LOW,		//NAME_NXE,
	YES,NO,LOW,		//NAME_NXF,
	YES,NO,LOW,		//NAME_NXG,
	YES,NO,LOW,		//NAME_NXH,
	YES,NO,LOW,		//NAME_NXI,

	YES,NO,LOW,		//NAME_BORDER1,
	YES,NO,LOW,		//NAME_SMWIRE,
	YES,NO,LOW,		//NAME_GUIDWIRE,
	YES,NO,LOW,		//NAME_EBWIRE,
	YES,NO,LOW,		//NAME_MINIWIRE,
	YES,NO,LOW,		//NAME_LASWIRE,
	YES,NO,LOW,		//NAME_AGMWIRE,
	YES,NO,LOW,		//NAME_BCKWIRE,

	YES,NO,LOW,		//NAME_JELLO,

	NO,YES,LOW,		//NAME_FLOPPY,
	NO,NO,LOW,		//NAME_bouncer3,
	NO,NO,LOW,		//NAME_s,
	NO,NO,LOW,		//NAME_flawa,
	NO,NO,LOW,		//NAME_nuray,

	NO,NO,LOW,		//NAME_chisler,
	NO,NO,LOW,		//NAME_stship2,
	NO,NO,LOW,		//NAME_0pod,
	NO,NO,LOW,		//NAME_flop2,
	NO,NO,LOW, 		//NAME_spiral3,
	NO,NO,LOW, 		//NAME_g2,
	NO,NO,LOW, 		//NAME_fork01,
	NO,NO,LOW, 		//NAME_fork02,
	NO,NO,LOW, 		//NAME_flawi,
	NO,YES,LOW,		//NAME_mrspit,
	NO,NO,LOW, 		//NAME_jshel,
	NO,NO,LOW, 		//NAME_tomi,
	NO,NO,LOW, 		//NAME_float,

	NO,YES,LOW,		//NAME_bug,
	NO,YES,LOW,		//NAME_star,
	NO,YES,LOW,		//NAME_starfsh,
	NO,NO,LOW,		//NAME_turret1,
	NO,YES,LOW,		//NAME_acidspt,
	NO,NO,LOW,		//NAME_chapie,
	NO,NO,LOW,		//NAME_twinspin,
	NO,NO,LOW,		//NAME_l,
	NO,YES,LOW,		//NAME_survrs,
	
	NO,NO,LOW,		//NAME_FUEL,
	YES,NO,HIGH,	//NAME_EXPLOSION,
	NO,YES,HIGH, 	//NAME_SPIKE,
	NO,YES,HIGH,	//NAME_SHELL,
	NO,YES,HIGH,	//NAME_LEAPER,
	NO,NO,LOW,		//NAME_POD,
	NO,NO,LOW,		//NAME_SHIELD,
	NO,NO,LOW,		//NAME_LASER,
	NO,NO,LOW,		//NAME_GRAVITY,
	NO,YES,HIGH,	//NAME_FLOATER,
	NO,YES,HIGH,	//NAME_HAIR1,
	NO,NO,HIGH,		//NAME_BOUNCER1,
	NO,NO,HIGH,		//NAME_BOUNCER2,
	NO,YES,HIGH,	//NAME_FORK01,
	NO,YES,HIGH,	//NAME_FORK02,
	NO,YES,HIGH,	//NAME_BFIRE,
	NO,NO,LOW,		//NAME_POD0,
	NO,YES,HIGH,	//NAME_FLOAT,
	NO,YES,HIGH,	//NAME_SCATTER,
	NO,NO,HIGH,		//NAME_CLAW,
	NO,YES,HIGH,	//NAME_FROG,
	NO,YES,HIGH,	//NAME_FUZZ,
	NO,YES,HIGH,	//NAME_RCK1,
  	NO,YES,HIGH,	//NAME_RCK2,
	NO,YES,HIGH,	//NAME_RCK3,
	NO,YES,HIGH,	//NAME_BUG,
	NO,YES,HIGH,	//NAME_STAR,
	NO,YES,HIGH,	//NAME_TURRET1,
	NO,NO,LOW,		//NAME_SPIRAL,
	NO,YES,HIGH,	//NAME_TENTAC1,
	NO,YES,HIGH,	//NAME_TENTAC2,
	NO,YES,HIGH,	//NAME_MINE,
	NO,YES,HIGH,	//NAME_MUSSEL,
	NO,NO,HIGH,		//NAME_RCKMORPH,
	NO,NO,LOW,		//NAME_CHAPIE,
	NO,YES,HIGH,	//NAME_STARFSH,
	NO,YES,HIGH,	//NAME_ACIDSPT,

	NO,NO,HIGH,		//NAME_BORDSML,
	NO,NO,HIGH,		//NAME_BORDSML2,
	NO,NO,HIGH,		//NAME_BORDSML3,

	NO,NO,HIGH,		//NAME_SHIPWIRE,
	NO,NO,HIGH,		//NAME_SHIPWIRR,
	NO,NO,HIGH,		//NAME_SHIPWIR2,
	NO,NO,HIGH,		//NAME_SHIPWIR3,
	NO,NO,HIGH,		//NAME_SHIPWIR4,

	NO,NO,HIGH,		//NAME_BARS01,
	NO,NO,HIGH,		//NAME_MISSWIRE,

	NO,NO,HIGH,		//NAME_portdu,
	NO,NO,HIGH,		//NAME_portuu,
	NO,NO,HIGH,		//NAME_portlu,
	NO,NO,HIGH,		//NAME_portru,
	NO,NO,HIGH,		//NAME_portul,
	NO,NO,HIGH,		//NAME_portur,
	NO,NO,HIGH,		//NAME_portdl,
	NO,NO,HIGH,		//NAME_portll,
	NO,NO,HIGH,		//NAME_portrl,
	NO,NO,HIGH,		//NAME_windy,

	NO,YES,HIGH,	//NAME_lmspit,
	NO,YES,HIGH,	//NAME_sperm,
	NO,YES,HIGH,	//NAME_sperm2,
	NO,NO,HIGH,		//NAME_windl2,

	NO,YES,HIGH,	//NAME_mine2,
	NO,YES,HIGH,	//NAME_windl4,
	NO,YES,HIGH,	//NAME_lazl4,
	NO,NO,HIGH,		//NAME_l42bt,

	NO,YES,HIGH,	//NAME_tentc1l5,
	NO,YES,HIGH,	//NAME_tentc2l5,
	NO,YES,HIGH,	//NAME_turrl6,
	
	NO,YES,HIGH,	//NAME_fflop,
	NO,YES,HIGH,	//NAME_l2c1,
	NO,YES,HIGH,	//NAME_l2c2,
	NO,YES,HIGH,	//NAME_l2c3,
	NO,YES,HIGH,	//NAME_l2c4,
	NO,YES,HIGH,	//NAME_l2c5,
	NO,YES,HIGH,	//NAME_block1,
	NO,YES,HIGH,	//NAME_block2,
	NO,YES,HIGH,	//NAME_block3,
	NO,YES,HIGH,	//NAME_block4,
	NO,YES,HIGH,	//NAME_windtun,
	NO,YES,HIGH,	//NAME_bloba,
	NO,YES,HIGH,	//NAME_tad,
	NO,YES,HIGH,	//NAME_spfire,
	NO,YES,HIGH,	//NAME_l2rock1,

	NO,YES,HIGH,	//NAME_bordsml,
	NO,YES,HIGH,	//NAME_bordsml3,
	NO,YES,HIGH,	//NAME_overlay,
	NO,YES,HIGH,	//NAME_overly2,
	NO,YES,HIGH,	//NAME_shipwir2,

	NO,YES,HIGH,	//NAME_tubel1,
	NO,YES,HIGH,	//NAME_7chapl1,
	NO,NO,HIGH,		//NAME_vinel11,
	NO,NO,HIGH,		//NAME_vines12,
	NO,NO,HIGH,		//NAME_vine13,
	NO,NO,HIGH,		//NAME_vine14,
	NO,NO,HIGH,		//NAME_vine15,
	NO,NO,HIGH,		//NAME_vine16,
	NO,YES,HIGH,	//NAME_s_l1c1,
	NO,YES,HIGH,	//NAME_s_l1c2,
	NO,YES,HIGH,	//NAME_s_l1c3,
	NO,YES,HIGH,	//NAME_s_l1c4,
	NO,YES,HIGH,	//NAME_s_l1c5,
	NO,YES,HIGH,	//NAME_tonsl1,
	NO,YES,HIGH,	//NAME_l1c1,
	NO,YES,HIGH,	//NAME_l1c2,
	NO,YES,HIGH,	//NAME_l1c3,
	NO,YES,HIGH,	//NAME_l1c4,
	NO,YES,HIGH,	//NAME_l1c5,
	NO,YES,HIGH,	//NAME_lazerl1,
	NO,YES,HIGH,	//NAME_msspit,
	NO,YES,HIGH,	//NAME_lrtad,

	NO,NO,HIGH,		//NAME_vinel21,
	NO,NO,HIGH,		//NAME_vinel22,
	NO,NO,HIGH,		//NAME_vinel23,
	NO,NO,HIGH,		//NAME_vinel24,
	NO,NO,HIGH,		//NAME_vinel26,
	NO,YES,HIGH,	//NAME_s_l2c1,
	NO,YES,HIGH,	//NAME_s_l2c2,
	NO,YES,HIGH,	//NAME_s_l2c3,
	NO,YES,HIGH,	//NAME_s_l2c4,
	NO,YES,HIGH,	//NAME_s_l2c5,
	NO,YES,HIGH,	//NAME_l2tentl,
	NO,YES,HIGH,	//NAME_l2tentr,
	NO,YES,HIGH,	//NAME_plungl2,
	NO,YES,HIGH,	//NAME_chapl2,
	NO,YES,HIGH,	//NAME_lazerl2,
	NO,YES,HIGH,	//NAME_vtestl2,

	NO,NO,HIGH,	//NAME_vtestl4,
	NO,NO,HIGH,		//NAME_vinl41,
	NO,NO,HIGH,		//NAME_vinel42,
	NO,NO,HIGH,		//NAME_vinel43,
	NO,NO,HIGH,		//NAME_vinel44,
	NO,NO,HIGH,		//NAME_vinel46,
	NO,YES,HIGH,	//NAME_s_l4c1,
	NO,YES,HIGH,	//NAME_s_l4c2,
	NO,YES,HIGH,	//NAME_s_l4c3,
	NO,YES,HIGH,	//NAME_s_l4c4,
	NO,YES,HIGH,	//NAME_s_l4c5,
	NO,YES,HIGH,	//NAME_tubes,
	NO,YES,HIGH,	//NAME_l4rock1,
	NO,YES,HIGH,	//NAME_l4c1,
	NO,YES,HIGH,	//NAME_l4c2,
	NO,YES,HIGH,	//NAME_l4c3,
	NO,YES,HIGH,	//NAME_l4c4,
	NO,YES,HIGH,	//NAME_l4c5,
	NO,YES,HIGH,	//NAME_l4bt,
	NO,YES,HIGH,	//NAME_plungl4,
	NO,YES,HIGH,	//NAME_lazerl4,

	NO,NO,HIGH,		//NAME_vinel61,
	NO,NO,HIGH,		//NAME_vinel62,
	NO,NO,HIGH,		//NAME_vinel63,
	NO,NO,HIGH,		//NAME_vinel64,
	NO,NO,HIGH,		//NAME_vinel65,
	NO,NO,HIGH,		//NAME_vinel66,
	NO,YES,HIGH,	//NAME_s_l6c1,
	NO,YES,HIGH,	//NAME_s_l6c2,
	NO,YES,HIGH,	//NAME_s_l6c3,
	NO,YES,HIGH,	//NAME_s_l6c4,
	NO,YES,HIGH,	//NAME_s_l6c5,
	NO,YES,HIGH,	//NAME_l6c1,
	NO,YES,HIGH,	//NAME_l6c2,
	NO,YES,HIGH,	//NAME_l6c3,
	NO,YES,HIGH,	//NAME_l6c4,
	NO,YES,HIGH,	//NAME_l6c5,
	NO,YES,HIGH,	//NAME_l61bt,
	NO,YES,HIGH,	//NAME_l62bt,
	NO,YES,HIGH,	//NAME_l63bt,
	NO,YES,HIGH,	//NAME_l64bt,
	NO,YES,HIGH,	//NAME_l65bt,
	NO,YES,HIGH,	//NAME_l6rock1,
	NO,YES,HIGH,	//NAME_plungl6,
	NO,YES,HIGH,	//NAME_lazerl6,
	NO,YES,HIGH,	//NAME_podtemp,
	NO,YES,HIGH,	//NAME_chapl6,
	NO,YES,HIGH,	//NAME_vtestl6,
	NO,YES,HIGH,	//NAME_tonsl6,
	NO,YES,HIGH,	//NAME_tubel6,

	NO,NO,HIGH,		//NAME_vinel51,
	NO,NO,HIGH,		//NAME_vinel52,
	NO,NO,HIGH,		//NAME_vinel53,
	NO,NO,HIGH,		//NAME_vinel54,
	NO,NO,HIGH,		//NAME_vinel56,
	NO,YES,HIGH,	//NAME_s_l5c1,
	NO,YES,HIGH,	//NAME_s_l5c2,
	NO,YES,HIGH,	//NAME_s_l5c3,
	NO,YES,HIGH,	//NAME_s_l5c4,
	NO,YES,HIGH,	//NAME_s_l5c5,
	NO,YES,HIGH,	//NAME_l5rock1,
	NO,YES,HIGH,	//NAME_vtestl5,
	NO,YES,HIGH,	//NAME_l5c1,
	NO,YES,HIGH,	//NAME_l5c2,
	NO,YES,HIGH,	//NAME_l5c3,
	NO,YES,HIGH,	//NAME_l5c4,
	NO,YES,HIGH,	//NAME_l5c5,
	NO,YES,HIGH,	//NAME_lazerl5,
	NO,YES,HIGH,	//NAME_chapl5,
	NO,YES,HIGH,	//NAME_plungl5,
	NO,YES,HIGH,	//NAME_l51bt,
	NO,YES,HIGH,	//NAME_l52bt,
	NO,YES,HIGH,	//NAME_l53bt,
	NO,YES,HIGH,	//NAME_l54bt,
	NO,YES,HIGH,	//NAME_tonsl5,

	NO,NO,HIGH,		//NAME_vinel31,						  
	NO,NO,HIGH,		//NAME_vinel33,						  
	NO,NO,HIGH,		//NAME_vinel34,						  
	NO,NO,HIGH,		//NAME_vinel36,						  
	NO,YES,HIGH,	//NAME_s_l3c1,						  
	NO,YES,HIGH,	//NAME_s_l3c2,						  
	NO,YES,HIGH,	//NAME_s_l3c3,						  
	NO,YES,HIGH,	//NAME_s_l3c4,						  
	NO,YES,HIGH,	//NAME_s_l3c5,						  
	NO,YES,HIGH,	//NAME_l3rock1,						  
	NO,YES,HIGH,	//NAME_plungl3,						  
	NO,YES,HIGH,	//NAME_mussl3,						  
	NO,YES,HIGH,	//NAME_chapl3,						  
	NO,YES,HIGH,	//NAME_l3c1,						  
	NO,YES,HIGH,	//NAME_l3c2,						  
	NO,YES,HIGH,	//NAME_l3c3,						  
	NO,YES,HIGH,	//NAME_l3c4,						  
	NO,YES,HIGH,	//NAME_l3c5,						  
	NO,YES,HIGH,	//NAME_l31bt,
	NO,YES,HIGH,	//NAME_l32bt,
	NO,YES,HIGH,	//NAME_lazer3,
	NO,YES,HIGH,	//NAME_vtest3l,

	NO,YES,HIGH,	//NAME_l7c1,
	NO,YES,HIGH,	//NAME_l7c2,
	NO,YES,HIGH,	//NAME_l7c3,
	NO,YES,HIGH,	//NAME_l7c4,
	NO,YES,HIGH,	//NAME_l7c5,
	NO,YES,HIGH,	//NAME_l71bt,
	NO,YES,HIGH,	//NAME_l72bt,
	NO,YES,HIGH,	//NAME_l73bt,
	NO,YES,HIGH,	//NAME_l74bt,
	
	NO,YES,HIGH,	//NAME_l8c1,
	NO,YES,HIGH,	//NAME_l81bt,
	NO,YES,HIGH,	//NAME_l82bt,
	NO,YES,HIGH,	//NAME_l83bt,
	NO,YES,HIGH,	//NAME_l84bt,
	
	NO,YES,HIGH,	//NAME_l9c1,
	NO,YES,HIGH,	//NAME_l91bt,
	NO,YES,HIGH,	//NAME_l92bt,
	NO,YES,HIGH,	//NAME_l93bt,
	NO,YES,HIGH,	//NAME_l94bt,
				
	NO,YES,HIGH,	//NAME_bugger,

	NO,YES,HIGH,	//NAME_donkey,
	NO,YES,HIGH,	//NAME_kong,

	NO,YES,HIGH,	//NAME_lmrspit,

	NO,YES,HIGH,	//NAME_l2rock2,
	NO,YES,HIGH,	//NAME_l2rock3,

	NO,YES,HIGH,	//NAME_tubel3,
	NO,YES,HIGH,	//NAME_windl3,
	NO,YES,HIGH,	//NAME_l3rock2,
	NO,YES,HIGH,	//NAME_l3rock3,
	NO,YES,HIGH,	//NAME_l33bt,
	NO,YES,HIGH,	//NAME_turrl3,
	NO,YES,HIGH,	//NAME_tonsl3,
	NO,YES,HIGH,	//NAME_vtestfl3,
	NO,YES,HIGH,	//NAME_vinfl3_3,
	NO,YES,HIGH,	//NAME_vtestl3,

	NO,YES,HIGH,	//NAME_l4rock2,
	NO,YES,HIGH,	//NAME_l4rock3,

	NO,YES,HIGH,	//NAME_l5rock2,
	NO,YES,HIGH,	//NAME_l5rock3,

	NO,YES,HIGH,	//NAME_tentc1l6,
	NO,YES,HIGH,	//NAME_l6rock2,
	NO,YES,HIGH,	//NAME_l6rock3,

	NO,YES,HIGH,	//NAME_l7rock1,
	NO,YES,HIGH,	//NAME_l7rock2,
	NO,YES,HIGH,	//NAME_l7rock3,
	NO,YES,HIGH,	//NAME_l7rock4,

	NO,YES,HIGH,	//NAME_spermneo,
	NO,YES,HIGH,	//NAME_spermne2,
	NO,YES,HIGH,	//NAME_spermne3,

	NO,YES,HIGH,	//NAME_l8blob,
	NO,YES,HIGH,	//NAME_s_l8c3,

	NO,YES,HIGH,	//NAME_flipl8_1,
	NO,YES,HIGH,	//NAME_flipl8_d,
	NO,YES,HIGH,	//NAME_flipl8_u,
	NO,YES,HIGH,	//NAME_flipl8_r,

//	NO,//	NO,LOW	//NAME_END
};
*/

#include "ai.dat"

/* ฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿ */
/* Process Sprites */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */
// EVENT based sprite language processor

long process_sprite(long i,long logic_index);

void process_sprites(void )
{
	long i;

	colour_bar(0,0,0,63);

	for(i=0;i<MAX_SPRITES;i++)
	{
		if(sprites[i].on==TRUE)
		{
			if(sprites[i].move_ptr==0)
			{
				printf("Error: %s has NULL AI\n",get_sprite_name(sprites[i].name));
				sprites[i].move_ptr=ai_none;
			}
 			sprites[i].move_ptr=process_sprite(i,sprites[i].move_ptr);
 			sprites[i].thread_ptr=process_sprite(i,sprites[i].thread_ptr);
		}
	}
	colour_bar(0,0,0,0);

}

long process_sprite(long i,long logic_index)
{
	double dtemp;
	flag end_slice;
	long *logic;
	long command;
	long temp1,temp2,temp3,temp4,temp5,temp6;

	end_slice=FALSE;
	logic=ai_data;

	while(end_slice==FALSE)
	{
		command=logic[logic_index++];
		switch(command)
		{
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_WAIT:
				logic_index--;
				end_slice=TRUE;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_STOP:
				end_slice=TRUE;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_PAUSE:
				temp1=logic[logic_index];
				if(sprites[i].obj[temp1]>0)
				{
					sprites[i].obj[temp1]--;
					end_slice=TRUE;
					logic_index--;
				}
				else logic_index++;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_JMP:
				logic_index=logic[logic_index++];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_CALL:
				temp1=logic[logic_index++];
				sprites[i].ret_ptr=logic_index;
				logic_index=temp1;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_RETURN:
				logic_index=sprites[i].ret_ptr;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_THREAD:
				sprites[i].thread_ptr=logic[logic_index++];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_MOVERND:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp2]=(rand()*temp1)>>15;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_MOVEI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp2]=temp1;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_MOVEV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp2]=sprites[i].obj[temp1];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ADDI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp2]+=temp1;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ADDV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp2]+=sprites[i].obj[temp1];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_SUBI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp2]-=temp1;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_SUBV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].obj[temp2]-=sprites[i].obj[temp1];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_INDEX_ARRAY:
				temp1=logic[logic_index++];			// var index
				temp1=sprites[i].obj[temp1];
				temp2=logic[logic_index++];			// array
				if(temp1>logic[temp2+1]) temp1=0;	// validate
				temp1=logic[temp2+temp1+2];
				temp2=logic[logic_index++];			// var dest
				sprites[i].obj[temp2]=temp1;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_SHL:
				temp1=logic[logic_index++];		// shift
				temp2=logic[logic_index++];		// shift var source
				temp1=(sprites[i].obj[temp2])<<temp1;
				temp2=logic[logic_index++];		 		// shift var dest
				sprites[i].obj[temp2]=temp1;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_SHR:
				temp1=logic[logic_index++];		// shift
				temp2=logic[logic_index++];		// shift var source
				temp1=(sprites[i].obj[temp2])>>temp1;
				temp2=logic[logic_index++];			 		// shift var dest
				sprites[i].obj[temp2]=temp1;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_MULI:
				temp1=logic[logic_index++];		// mul1
				temp2=logic[logic_index++];		// mul2
				temp1=sprites[i].obj[temp2]*temp1;
				temp2=logic[logic_index++];		
				sprites[i].obj[temp2]=temp1;		// result
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_MULV:
				temp1=logic[logic_index++];		// mul1
				temp2=logic[logic_index++];		// mul2
				temp1=sprites[i].obj[temp2]*sprites[i].obj[temp1];
				temp2=logic[logic_index++];		
				sprites[i].obj[temp2]=temp1;		// result
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_DIVI:
				temp1=logic[logic_index++];		// divider
				temp2=logic[logic_index++];		// 
				if(temp1!=0) temp1=sprites[i].obj[temp2]/temp1;
				temp2=logic[logic_index++];		// result
				sprites[i].obj[temp2]=temp1;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_DIVV:
				temp1=logic[logic_index++];		// divider
				temp2=logic[logic_index++];		// 
				if(sprites[i].obj[temp1]!=0)
					 temp1=sprites[i].obj[temp2]/sprites[i].obj[temp1];
				temp2=logic[logic_index++];		// result
				sprites[i].obj[temp2]=temp1;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_NEG:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=-sprites[i].obj[temp1];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ADD_CHILD:
				temp1=logic[logic_index++];		// sprites name
				temp2=logic[logic_index++];		// sprites pc
				temp3=add_sprite(temp1,ACTION_CYCLE,
										sprites[i].x,sprites[i].y);
				printf("Child added returned %i\n",temp3);
				if(sprites[i].child!=-1) printf("Old child overwritten\n");
				sprites[i].child=temp3;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ADOPT_INRANGE_CHILD:
				temp1=logic[logic_index++];
				if(sprites[i].child==-1)
				{
					foster_parent=i;
					sprites[i].obj[obj_children]=0;
					adopt_inrange_child(i,temp1);
				}
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_QUEEN_ORDER:
				temp1=logic[logic_index++];
				temp2=i;
				while(sprites[temp2].child!=-1)
				{
					temp2=sprites[temp2].child;
					sprites[temp2].obj[temp1]=sprites[i].obj[temp1];
					//printf("From %i To %i\n",i,temp2);
				}
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case 	COMMAND_GET_QUEENS:
				temp1=logic[logic_index++];
				temp2=sprites[i].queen;
				if(temp2!=-1)
					sprites[i].obj[temp1]=sprites[temp2].obj[temp1];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case	COMMAND_GET_PARENTV:
				temp1=logic[logic_index++];
				temp2=sprites[i].parent;
				if(temp2!=-1)
					sprites[i].obj[temp1]=sprites[temp2].obj[temp1];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case	COMMAND_GET_CHILDV:
				temp1=logic[logic_index++];
				temp2=sprites[i].child;
				if(temp2!=-1)
					sprites[i].obj[temp1]=sprites[temp2].obj[temp1];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_IFEQI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] == temp1)
						logic_index=logic[logic_index++];
				else	logic_index++;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_IFEQV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] == sprites[i].obj[temp1])
						logic_index=logic[logic_index++];
				else	logic_index++;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_IFNEI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] != temp1)
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_IFNEV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] != sprites[i].obj[temp1])
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_IFLEI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] <= temp1)
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_IFLEV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] <= sprites[i].obj[temp1])
						logic_index=logic[logic_index++];
				else	logic_index++;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_IFLTI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] < temp1)
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_IFLTV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] < sprites[i].obj[temp1])
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_IFGTI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] > temp1)
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_IFGTV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] > sprites[i].obj[temp1])
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_IFGEI:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] >= temp1)
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_IFGEV:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				if( sprites[i].obj[temp2] >= sprites[i].obj[temp1])
						logic_index=logic[logic_index];
				else	logic_index++;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_FRAMEINC:
				if ( (sprites[i].framecounter--)<=0 )
				{
					sprites[i].framecounter=sprites[i].anim_delay_rate;
				 	temp1=sprites[i].frame;
					temp1++;
					if( temp1>=sprites[i].endframe ) temp1=sprites[i].startframe;
					sprites[i].frame=temp1;
				}
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_UPDATE_XY:
				sprites[i].x=sprites[i].obj[obj_x];
				sprites[i].y=sprites[i].obj[obj_y];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case	COMMAND_SET_FRAME:
				temp1=logic[logic_index++];
				sprites[i].frame=sprites[i].obj[temp1];
				//printf("Frame Set %i\n",sprites[i].obj[temp1]);
				/*
				if(sprites[i].frame>=seqdata[(sprites[i].name)].frames)
					sprites[i].frame=0;
				if(sprites[i].frame<0) sprites[i].frame=0;
				*/
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case	COMMAND_SETANIMRANGE:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				sprites[i].startframe=sprites[i].obj[temp1];
				sprites[i].endframe=sprites[i].obj[temp2];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case	COMMAND_SETANIMNORMAL:
				sprites[i].startframe=sprites[i].normal_startframe;
				sprites[i].endframe=sprites[i].normal_endframe;
				sprites[i].frame=sprites[i].startframe;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case	COMMAND_SETANIMLEFT:
				sprites[i].startframe=sprites[i].left_startframe;
				sprites[i].endframe=sprites[i].left_endframe;
				sprites[i].frame=sprites[i].startframe;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case	COMMAND_SETANIMRIGHT:
				sprites[i].startframe=sprites[i].right_startframe;
				sprites[i].endframe=sprites[i].right_endframe;
	 			sprites[i].frame=sprites[i].startframe;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case	COMMAND_SETANIMUP:
				sprites[i].startframe=sprites[i].up_startframe;
				sprites[i].endframe=sprites[i].up_endframe;
		 		sprites[i].frame=sprites[i].startframe;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case	COMMAND_SETANIMDOWN:
				sprites[i].startframe=sprites[i].down_startframe;
				sprites[i].endframe=sprites[i].down_endframe;
		 		sprites[i].frame=sprites[i].startframe;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case	COMMAND_SETANIMWOUNDED:
				sprites[i].startframe=sprites[i].wounded_startframe;
				sprites[i].endframe=sprites[i].wounded_endframe;
		 		sprites[i].frame=sprites[i].startframe;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case	COMMAND_SETANIMDEATH:
				sprites[i].startframe=sprites[i].death_startframe;
				sprites[i].endframe=sprites[i].death_endframe;
				sprites[i].frame=sprites[i].startframe;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ANIMPLAYFORWARDONCE:
				if ( (sprites[i].framecounter--)<=0 )
				{
					sprites[i].framecounter=sprites[i].anim_delay_rate;
				 	sprites[i].frame++;
				}

				if(sprites[i].frame<=(sprites[i].endframe-1))
				{ 
					logic_index--;
					end_slice=TRUE;
				}

				if(sprites[i].frame<sprites[i].startframe)
					sprites[i].frame=sprites[i].startframe;
				if(sprites[i].frame>=sprites[i].endframe)
					sprites[i].frame=sprites[i].endframe-1;
				if(sprites[i].frame<0) sprites[i].frame=0;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ANIMPLAYBACKONCE:
				if ( (sprites[i].framecounter--)<=0 )
				{
					sprites[i].framecounter=sprites[i].anim_delay_rate;
				 	sprites[i].frame--;
				}
				if(sprites[i].frame>=sprites[i].startframe)
				{ 
					logic_index--;
					end_slice=TRUE;
				}

				if(sprites[i].frame<sprites[i].startframe)
					sprites[i].frame=sprites[i].startframe;
				if(sprites[i].frame>=sprites[i].endframe)
					sprites[i].frame=sprites[i].endframe-1;

				if(sprites[i].frame<0) sprites[i].frame=0;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ANIMPLAYLOOPONCE:
				if ( (sprites[i].framecounter--)<=0 )
				{
					sprites[i].framecounter=sprites[i].anim_delay_rate;
				 	sprites[i].frame++;
				}

				if(sprites[i].frame<=(sprites[i].endframe-1))
				{ 
					logic_index--;
					end_slice=TRUE;
				}

				if(sprites[i].frame<sprites[i].startframe)
					sprites[i].frame=sprites[i].startframe;
				if(sprites[i].frame>=sprites[i].endframe)
					sprites[i].frame=sprites[i].startframe;
				if(sprites[i].frame<0) sprites[i].frame=0;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ANIMSPEED:
				sprites[i].anim_delay_rate=logic[logic_index++];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ANIMSPEEDV:
				temp1=logic[logic_index++];
				sprites[i].anim_delay_rate=sprites[i].obj[temp1];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_INRANGE:
				temp1=sprites[i].x-craftx;
				temp2=sprites[i].y-crafty;
				if(logic[logic_index++] > ((temp1*temp1)+(temp2*temp2)) )
					logic_index=logic[logic_index++];
				else logic_index++;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_BG_COLLIDE:
				temp1=logic[logic_index++];
				if( sprite_collision(&seqdata[(sprites[i].name)],
											sprites[i].frame,
											sprites[i].x,
											sprites[i].y) )
				{
					logic_index=temp1;
				}
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
// This now works
			case COMMAND_POINT_COLLISION:
				temp1=logic[logic_index++];		//x
				temp2=logic[logic_index++];		//y
				temp1=sprites[i].obj[temp1];
				temp2=sprites[i].obj[temp2];
				temp3=logic[logic_index++];
				temp4=coll_buf[( (temp1+temp2*iff_width)>>3 )];
				if( ( temp4 & (1<<(7-temp1&7)) )!=0) logic_index=temp3;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_FIND_FLOOR:
				temp1=logic[logic_index++];		//x
				temp2=logic[logic_index++];		//y
				temp3=logic[logic_index++]; 		//max scan + return var
				sprites[i].obj[temp3]=find_floor(sprites[i].obj[temp1],
															sprites[i].obj[temp2],
															sprites[i].obj[temp3]);
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
// Scans for walls
			case COMMAND_SCAN:
				temp1=logic[logic_index++];		//x
				temp2=logic[logic_index++];		//y
				temp3=screen_clip_point(sprites[i].obj[temp1],sprites[i].obj[temp2]);
				temp1=logic[logic_index++];		//x1
				temp2=logic[logic_index++];		//x2
				sprites[i].obj[temp1]=temp3&0xffff;
				sprites[i].obj[temp2]=temp3>>16;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_DIE:
				end_slice=TRUE;
				logic_index--;
				if(sprites[i].dying==0)	sprites[i].dying=2;
				else
				{
					sprites[i].dying--;
					if(sprites[i].dying==0)
					{ 
						//printf("Child %i\n",sprites[i].child);
						sprites[i].on=FALSE;

						// Fix linked list

					 	if(sprites[i].parent!=-1)
						{
							//printf("%i Has Parent\n",i);
							temp1=sprites[i].parent;
							sprites[temp1].child=sprites[i].child;
											
							if(sprites[i].child!=-1)
							{
								//printf("%i Has Child\n",i);
								temp1=sprites[i].child;
								sprites[temp1].parent=sprites[i].parent;
							}
						}
						if(sprites[i].queen!=-1)
						{
							temp1=sprites[i].queen;
							sprites[temp1].obj[obj_children]--;
						}
						if(sprites[i].obj[obj_children]>0)
						{
							//printf("Queen %i killed\n",i);
							//printf("Child %i\n",sprites[i].child);
							if(sprites[i].child!=-1)
							{
								//printf("Has a child\n",i);
								for(temp1=0;temp1<MAX_SPRITES;temp1++)
								{
									if(sprites[temp1].queen==i)
										sprites[temp1].queen=sprites[i].child;
								}
								temp1=sprites[i].child;
								sprites[temp1].parent=-1;
								sprites[temp1].obj[obj_children]=sprites[i].obj[obj_children];
								sprites[temp1].path=sprites[i].path;
								sprites[temp1].node=sprites[i].node;
								//printf("New Queen %i Children %i\n",temp1,sprites[i].obj[obj_children]);
							}
						}
					}
				}
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ARRAY:
				temp1=logic[logic_index++];
				logic_index+=temp1;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ADD_PARTICLE:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				temp3=logic[logic_index++];
				temp4=logic[logic_index++];
				temp5=logic[logic_index++];
				temp6=logic[logic_index++];
				add_particle_acceleration(sprites[i].obj[temp1],
												sprites[i].obj[temp2],
												sprites[i].obj[temp3],
												sprites[i].obj[temp4],
												sprites[i].obj[temp5],														
												sprites[i].obj[temp6]);
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ON_HIT:
				sprites[i].on_hit_ptr=logic[logic_index++];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_SHOOTABLE_ON:
				sprites[i].shootable=TRUE;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_SHOOTABLE_OFF:
				sprites[i].shootable=FALSE;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_VISIBLE_ON:
				sprites[i].visible=TRUE;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_VISIBLE_OFF:		 
				sprites[i].visible=FALSE;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_SET_PRIORITY:
				temp1=logic[logic_index++];
				sprites[i].priority=temp1;		 
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_GET_FLOOD_VAL:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=get_flood_val(sprites[i].x,sprites[i].y);
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_GET_VECTOR:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				get_flood_vector(sprites[i].x,sprites[i].y,
									  &sprites[i].obj[temp1],&sprites[i].obj[temp2]);
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_GET_QUEEN_ID:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=sprites[i].queen;
				//printf("Queen ID = %i\n",sprites[i].queen);
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_GET_PARENT_ID:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=sprites[i].parent;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_GET_CHILD_ID:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=sprites[i].child;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_GET_PLAYER_ID:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=craft;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_GET_POD_ID:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=pod_no;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_GET_IDS_VAR:
				temp1=logic[logic_index++];		// ID
				temp2=logic[logic_index++];		// VAR wanted
				temp3=logic[logic_index++];		// VAR dest
				sprites[i].obj[temp3]=sprites[(sprites[i].obj[temp1])].obj[temp2];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_SET_IDS_VAR:
				temp1=logic[logic_index++];		// Var source
				temp2=logic[logic_index++];		// ID
				temp3=logic[logic_index++];		// VAR dest
				sprites[(sprites[i].obj[temp2])].obj[temp3]=sprites[i].obj[temp1];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_GET_ANGLE_TO_ID:
				temp1=logic[logic_index++];
				temp1=sprites[i].obj[temp1];
				if(temp1!=-1)
				{
					sprites[i].angle=atan2(sprites[temp1].x-sprites[i].x,
												  sprites[temp1].y-sprites[i].y);
					dtemp=sprites[i].angle;
					if(dtemp<0) dtemp=(PI*2)+dtemp;
					sprites[i].obj[obj_angle]=(dtemp*1024)/(PI*2);
				}
				
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_SET_VEL:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				temp3=logic[logic_index++];
				sprites[i].obj[temp2]=asm_mul_div(sin_table[(sprites[i].obj[obj_angle])],
															 sprites[i].obj[temp1],32768);
				sprites[i].obj[temp3]=asm_mul_div(cos_table[(sprites[i].obj[obj_angle])],
															 sprites[i].obj[temp1],32768);
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			// Wont work anymore...
			case COMMAND_GET_ROTATION_TO_ID:
				dtemp=sprites[i].angle;
				while(dtemp<0) dtemp+=PI*2;
				while(dtemp>(PI*2)) dtemp-=PI*2;

				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				temp1=sprites[i].obj[temp1];
				dtemp=atan2(sprites[temp1].x-sprites[i].x,
						   	sprites[temp1].y-sprites[i].y);
				//printf("Angle %f New Angle %f\n",sprites[i].angle,dtemp);
				dtemp=sprites[i].angle-dtemp;
				//printf("Difference %f\n",dtemp);
				if(dtemp<0)
				{
					dtemp=-dtemp;
					if(dtemp<PI) temp1=1;
					else temp1=-1;
				}
				else
				{
					if(dtemp<PI) temp1=-1;
					else temp1=1;
				}
				//printf("Difference %f\n",dtemp);
				//printf("Rotation = %i\n",temp1);
				sprites[i].obj[temp2]=temp1;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ADD_TO_ANGLE:
				temp1=logic[logic_index++];
				temp1=sprites[i].obj[temp1];
				/*
					sprites[i].angle+=(temp1*PI*2)/256;
				*/
				sprites[i].obj[obj_angle]+=temp1*4;
				sprites[i].obj[obj_angle]&=1023;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_GET_ANGLE:
				temp1=logic[logic_index++];
				/*
					while(sprites[i].angle<0) sprites[i].angle+=PI*2;
					while(sprites[i].angle>(PI*2)) sprites[i].angle-=PI*2;
					sprites[i].obj[temp1]=(sprites[i].angle*256)/(PI*2);
					sprites[i].obj[temp1]&=255;
				*/
				sprites[i].obj[temp1]=sprites[i].obj[obj_angle];
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_SET_ANGLE:
				temp1=logic[logic_index++];
				sprites[i].angle=(sprites[i].obj[temp1]*PI*2)/256;
				sprites[i].obj[obj_angle]=(temp1&0xff)*4;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_NEXT_NODE:
				temp4=logic[logic_index++];
				temp5=logic[logic_index++];
				temp6=logic[logic_index++];
				if(sprites[i].path==-1)			// No Path available
				{
					sprites[i].obj[temp4]=-1;			
					sprites[i].obj[temp5]=-1;			
					sprites[i].obj[temp6]=-1;
					break;
				}
				get_path_coordinates(&sprites[i].node,&temp1,&temp2);
				temp1=sprites[i].start_x+temp1-sprites[i].x;
				temp2=sprites[i].start_y+temp2-sprites[i].y;
				temp3=sqr_root( (temp1*temp1)+(temp2*temp2) );
				if(temp3>0)
				{
					sprites[i].obj[temp4]=(temp1*65536)/temp3;
					sprites[i].obj[temp5]=(temp2*65536)/temp3;
				}
				else
				{
					printf("Zero length path %s (%i,%i)\n",get_sprite_name(sprites[i].name),temp1,temp2);
					sprites[i].obj[temp4]=0;
					sprites[i].obj[temp5]=0;
				}
				sprites[i].obj[temp6]=temp3;

				// bodge... (for -2 values which mean return
				if(sprites[i].node<0) sprites[i].node=sprites[i].path;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_GET_SPRITES_COORDS:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				temp3=logic[logic_index++];
				temp4=logic[logic_index++];
				get_sprites_coordinates(i,&sprites[i].obj[temp1],
												  &sprites[i].obj[temp2],
												  &sprites[i].obj[temp3],
												  &sprites[i].obj[temp4]);
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ADD_POLO:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				temp3=logic[logic_index++];
				temp4=logic[logic_index++];
				add_polo(sprites[i].obj[temp1],	 
							sprites[i].obj[temp2],
							sprites[i].obj[temp3],
							sprites[i].obj[temp4]);
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_ADD_LINE:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				temp3=logic[logic_index++];
				add_a_glow_line_clipped(sprites[i].obj[temp1],	 
												sprites[i].obj[temp2],
												sprites[i].obj[temp3]);
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_GET_RET:
				temp1=logic[logic_index++];
				sprites[i].obj[temp1]=sprites[i].ret_ptr;
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_GET_REF_POINT:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				temp3=logic[logic_index++];
				get_reference_point(i,temp1,temp2,temp3);
				break;
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
			case COMMAND_OUTPUT_VAR:
				temp1=logic[logic_index++];
				temp2=logic[logic_index++];
				debug_variable[temp1]=sprites[i].obj[temp2];
				break;
	
			}
		}

	/*
	if( sprites[i].buf_rem[buf]==YES && safe_mode==YES)
	{
		if( (sprites[i].buf_x[buf]==sprites[i].x) &&
			 (sprites[i].buf_y[buf]==sprites[i].y) &&
			 (sprites[i].buf_frame[buf]==sprites[i].frame) ) sprites[i].buf_rem[buf]=NO;
			 else sprites[i].buf_rem[buf]=YES;

			//sprites[i].buf_rem[buf]=YES;
	}
	*/

	return logic_index;
}

/* ฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿ */
/* Make a linked list of inrange sprites */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

void adopt_inrange_child(long father,long range)
{
	long i;
	long x,y;

	for(i=0;i<MAX_SPRITES;i++)
	{
		if( (sprites[i].on==TRUE) && (i!=father) )
		{
			if(sprites[father].name==sprites[i].name)
			{
				if( (sprites[i].parent==-1) && (sprites[i].child==-1) )
				{
					x=sprites[father].x-sprites[i].x;
					y=sprites[father].y-sprites[i].y;
					if( ((x*x)+(y*y)) < range )
					{
						sprites[i].parent=father;
						sprites[i].queen=foster_parent;
						sprites[father].child=i;
						sprites[foster_parent].obj[obj_children]++;
						//printf("Father %i Child %i %i\n",father,i,sprites[foster_parent].obj[obj_children]);
						adopt_inrange_child(i,range);
						return;
					}
				}
			}
		}
	}
}
	
/* ฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿ */
/* Clear SPRITE LIST */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

void clear_sprites(void)
{
	int i;
	for(i=0;i<MAX_SPRITES;i++)	sprites[i].on=FALSE;
}

/* ฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿ */
/* Add a sprite to SPRITE LIST */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

typedef struct
{
	long name;
	long action;
} actionstruc;

actionstruc sprites_actions[]={
	NAME_survrs,ai_new_man,
	NAME_lazerl1,ai_lazer,
	NAME_lazerl2,ai_lazer,
	NAME_lazl4,ai_lazer,
	NAME_lazerl5,ai_lazer,
	NAME_lazerl6,ai_lazer,
	NAME_mine2,ai_surface_hug,
	
	NAME_l33bt,ai_soft_rock,
	NAME_l42bt,ai_soft_rock,
	NAME_l51bt,ai_soft_rock,
	NAME_l52bt,ai_soft_rock,
	NAME_l53bt,ai_soft_rock,
	NAME_l54bt,ai_soft_rock,
	NAME_l61bt,ai_soft_rock,
	NAME_l62bt,ai_soft_rock,
	NAME_l63bt,ai_soft_rock,
	NAME_l64bt,ai_soft_rock,

	NAME_bugger,ai_bugger,
	NAME_sperm2,ai_sperm,
	NAME_sperm,ai_flock,
	NAME_tad,ai_flock,
	NAME_l1c5,ai_test_ref,

//	NAME_BOUNCER1,ai_test,
//	NAME_BOUNCER2,ai_test,

	NAME_tonsl1,ai_tonsl1,
	NAME_EBWIRE,ai_dissolve,
	NAME_NXF,ai_explode,
	NAME_FLOPPY,ai_floppy,
	NAME_XB_FLOP1,ai_flop_exp,
	NAME_pod4,ai_pod,
//	NAME_starfsh,ai_starfsh,
//	NAME_lrtad,ai_swarm,
//	NAME_tad,ai_tad,
//	NAME_star,ai_star,
	NAME_l2rock1,ai_asteroid,
	NAME_EXPLOSION,ai_explode,
	NAME_windtun,ai_wind_tunnel,
	NAME_windy,ai_wind_tunnel,
	NAME_l2c1,ai_crystal,
	NAME_l2c2,ai_crystal,
	NAME_l2c3,ai_crystal,
	NAME_l2c4,ai_crystal,
	NAME_l2c5,ai_crystal,
	NAME_l1c1,ai_crystal,
	NAME_l1c2,ai_crystal,
	NAME_l1c3,ai_crystal,
	NAME_l1c4,ai_crystal,
	NAME_l1c5,ai_crystal,
	NAME_s_l4c1,ai_crystal,
	NAME_s_l4c2,ai_crystal,
	NAME_s_l4c3,ai_crystal,
	NAME_s_l4c4,ai_crystal,
	NAME_s_l4c5,ai_crystal,
	NAME_l4c1,ai_crystal,
	NAME_l4c3,ai_crystal,
	NAME_l4c4,ai_crystal,
	NAME_l4c5,ai_crystal,
	NAME_RCK1,ai_asteroid,
	NAME_RCK2,ai_asteroid,
//	NAME_bug,ai_bug,
//	NAME_survrs,ai_survrs,
//	NAME_survrs,ai_lem,
//	NAME_survrs,ai_flea,
//	NAME_starfsh,ai_starfish,
	NAME_TENTAC1,ai_tentacle,
	NAME_TENTAC2,ai_tentacle,
	NAME_l2tentr,ai_tentacle,
	NAME_tentc2l5,ai_tentacle,
	NAME_MUSSEL,ai_mussel,
	NAME_chapl1,ai_mussel,

	NAME_vinel11,ai_none,
	NAME_vines12,ai_none,
	NAME_vine14,ai_none,
	NAME_vine15,ai_none,
	NAME_vine16,ai_none,
	
	NAME_vinel22,ai_none,
	NAME_vinel23,ai_none,
	NAME_vinel24,ai_none,
	NAME_vinel26,ai_none,
	NAME_vtestl2,ai_none,

	NAME_vinel31,ai_none,
	NAME_vinel34,ai_none,
	NAME_vinel36,ai_none,

	NAME_vinel42,ai_none,
	NAME_vinel43,ai_none,
	NAME_vinel44,ai_none,
	NAME_vinel46,ai_none,
	NAME_vtestl4,ai_none,

	NAME_vinel51,ai_none,
	NAME_vinel52,ai_none,
	NAME_vinel53,ai_none,
	NAME_vinel54,ai_none,
	NAME_vinel56,ai_none,
	NAME_vtestl5,ai_none,

	NAME_vinel61,ai_none,
	NAME_vinel62,ai_none,
	NAME_vinel63,ai_none,
	NAME_vinel64,ai_none,
	NAME_vinel66,ai_none,
	NAME_vtestl6,ai_none,

	NAME_END,ai_none
};

long sprites_still[]={
	NAME_BORDER1,
	
	NAME_BOUNCER1,
	NAME_BOUNCER2,
	NAME_bouncer3,

	NAME_vinel11,
	NAME_vines12,
	NAME_vine14,
	NAME_vine15,
	NAME_vine16,
	NAME_l1c1,
	NAME_l1c2,
	NAME_l1c3,
	NAME_l1c4,
	NAME_l1c5,
	NAME_s_l1c1,
	NAME_s_l1c2,
	NAME_s_l1c3,
	NAME_s_l1c4,
	NAME_s_l1c5,
	
	NAME_vinel22,
	NAME_vinel23,
	NAME_vinel24,
	NAME_vinel26,
	NAME_vtestl2,

	NAME_s_l2c1,
	NAME_s_l2c2,
	NAME_s_l2c3,
	NAME_s_l2c4,
	NAME_s_l2c5,

	NAME_l2c1,
	NAME_l2c2,
	NAME_l2c3,
	NAME_l2c4,
	NAME_l2c5,

	NAME_vinel31,
	NAME_vinel34,
	NAME_vinel36,

	NAME_s_l3c1,
	NAME_s_l3c2,
	NAME_s_l3c3,
	NAME_s_l3c4,
	NAME_s_l3c5,

	NAME_l3c1,
	NAME_l3c2,
	NAME_l3c3,
	NAME_l3c4,
	NAME_l3c5,

	NAME_vinel42,
	NAME_vinel43,
	NAME_vinel44,
	NAME_vinel46,
	NAME_vtestl4,

	NAME_s_l4c1,
	NAME_s_l4c2,
	NAME_s_l4c3,
	NAME_s_l4c4,
	NAME_s_l4c5,

	NAME_l4c1,
	NAME_l4c3,
	NAME_l4c4,
	NAME_l4c5,

	NAME_vinel51,
	NAME_vinel52,
	NAME_vinel53,
	NAME_vinel54,
	NAME_vinel56,
	NAME_vtestl5,

	NAME_s_l5c1,
	NAME_s_l5c2,
	NAME_s_l5c3,
	NAME_s_l5c4,
	NAME_s_l5c5,

	NAME_l5c1,
	NAME_l5c2,
	NAME_l5c3,
	NAME_l5c4,
	NAME_l5c5,

	NAME_vinel61,
	NAME_vinel62,
	NAME_vinel63,
	NAME_vinel64,
	NAME_vinel66,
	NAME_vtestl6,

	NAME_s_l6c1,
	NAME_s_l6c2,
	NAME_s_l6c3,
	NAME_s_l6c4,
	NAME_s_l6c5,

	NAME_l6c1,
	NAME_l6c2,
	NAME_l6c3,
	NAME_l6c4,
	NAME_l6c5,


	NAME_END
};

long add_sprite(int name,int action,int x,int y)
{
	int i,j;

	// Find a free sprite

	if(seqdata[name].frames<0)
	{
  		printf("%s no Frames found\n",get_sprite_name(name));
	 	return -1;
	}

	for(i=0;i<MAX_SPRITES;i++)
	{
		if(sprites[i].on==FALSE)	break;
	}
	if(i==MAX_SPRITES)
	{
  		printf("%s sprite list fill\n",get_sprite_name(name));
	 	return -1;
	}

	// AI data

	switch (action)
	{
		case ACTION_NONE:
			sprites[i].move_ptr=ai_none;
			break;
		case ACTION_CYCLE:
			sprites[i].move_ptr=ai_cycle;
			break;
		case ACTION_PINGPONG:
			sprites[i].move_ptr=ai_pingpong;
			break;
		case ACTION_PLAYONCE:
			sprites[i].move_ptr=ai_explode;
			break;
		case ACTION_PATH:
			sprites[i].move_ptr=ai_path;
			break;
		default:
			sprites[i].move_ptr=ai_cycle;
			break;
	}

	// Find action associated to sprite

	j=0;
	while(sprites_actions[j].name!=NAME_END)
	{
		if(name==sprites_actions[j].name)
		{
			sprites[i].move_ptr=sprites_actions[j].action;
			break;
		}
		j++;
	}

//	sprites[i].move_ptr=ai_cycle;

	sprites[i].path=-1;
	sprites[i].node=-1;

	sprites[i].thread_ptr=ai_none;

	// Insert sprite data

	sprites[i].still=NO;

	j=0;
	while(sprites_still[j]!=NAME_END)
	{
		if(name==sprites_still[j])
		{
			sprites[i].still=YES;
			break;
		}
		j++;
	}

	sprites[i].queen=-1;
	sprites[i].child=-1;
	sprites[i].parent=-1;
	sprites[i].obj[obj_children]=0;
	sprites[i].hit=FALSE;
	sprites[i].on=TRUE;
	sprites[i].visible=TRUE;
	sprites[i].shootable=sprites_attribs[name].shootable;
	sprites[i].name=name;
	sprites[i].action=action;
	sprites[i].on_hit_ptr=ai_polo;
	sprites[i].x=x;
	sprites[i].start_x=x;
	sprites[i].y=y;
	sprites[i].start_y=y;
	sprites[i].buf_x[0]=x;
	sprites[i].buf_x[1]=x;
	sprites[i].buf_y[0]=y;
	sprites[i].buf_y[1]=y;
	sprites[i].obj[obj_x]=x;
	sprites[i].obj[obj_y]=y;
	sprites[i].anim_delay_rate=4;
	sprites[i].frame=0;
	sprites[i].dying=0;

	sprites[i].startframe=0;
	sprites[i].endframe=seqdata[name].frames-2;

	// Bodge for spikes...

	if(name==NAME_SPIKE) sprites[i].framecounter=8+(rand()*128)>>15;
	else sprites[i].framecounter=0;

	sprites[i].framecounter+=rand()&3;

	for(j=0;j<2;j++)					// Set up Double Buffer Data
	{
		sprites[i].buf_x[j]=x;
		sprites[i].buf_y[j]=y;
		sprites[i].buf_frame[j]=0;
		sprites[i].buf_rem[j]=NO;
	}
	return i;
}

void set_ai_none(long i)
{
	sprites[i].move_ptr=ai_none;
}

void set_ai_cycle(long i)
{
	sprites[i].move_ptr=ai_cycle;
}

void get_reference_point(long i,long point,long var_x,long var_y)
{
	char 	*source;
	short	*sourcew;
	long	count;
	long 	name,frame;
	long	x_origin,y_origin;
	long	no_of_collisions;
	long	no_of_references;

	name=sprites[i].name;
	frame=sprites[i].frame;

	source=seqdata[name].org;
	if(source==NULL)
	{
		sprites[i].obj[var_x]=sprites[i].x;
		sprites[i].obj[var_y]=sprites[i].y;
		return;
	}

//	if(frame>10) return;
	
	for(count=0;count<=frame;count++)
	{
		sourcew=(short *) source;
	
		x_origin=(long ) sourcew[1];
		y_origin=(long ) sourcew[2];
	
		no_of_collisions=(long ) sourcew[3];

		source+=8+2;
		source+=no_of_collisions*8;
		sourcew=(short *) source;

		no_of_references=(long ) sourcew[0];
		sourcew++;

		source=(char *) sourcew;
		source+=no_of_references*4;
	}
	
	sprites[i].obj[var_x]=sprites[i].x-x_origin+sourcew[point*2+0];
	sprites[i].obj[var_y]=sprites[i].y-y_origin+sourcew[point*2+1];

	printf("Frame	  %i\n",frame);
	printf("Point	  %i\n",point);
	printf("Origin X %i Y %i\n",x_origin,y_origin);
	printf("Ref:	X %i Y %i\n",sourcew[(point*2)+0],sourcew[(point*2)+1]);
}
