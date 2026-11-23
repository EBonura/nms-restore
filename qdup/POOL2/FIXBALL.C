//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
//								    	 QD-UP      										 */
//										PC-CD ROM											 */
//								 For BMG	Interactive										 */
//								 By NMS Software Ltd.									 */
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// RESTORE: NMS's file stopped mid-conversion on 19 Sep 1996 (29 of BALL.C's
// functions, typos, an unbalanced parenthesis).  Lines marked RESTORE are
// restoration changes; everything else is NMS's.  The functions BALL.C has
// and this file lacked are added at the end, converted the same way.
// Number formats are described in FIXBALL.H.
#ifdef	FIXBALL_STANDALONE		// RESTORE: the physics test builds this file alone
#include	"fixball.h"
#else
#include	"globals.h"
#endif
#define	abs(_v_)	fix_abs(_v_)	// RESTORE: inline (FIXBALL.H), same results
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
#define		PIx	0x3243f6a		//3.141592654
#define		PI2x	0x6487ed5		//6.283185307
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// RESTORE: BALL.C's constants, in 8.24 (angle_z_friction is applied to avz,
// 16.16, through mul64, so it stays 8.24 too)
#define	Q_FACTOR   (0.2)
#define	R_FACTOR   (0.08)
#define	Z_AXIS_SPIN_FRICTION	0.1
#define	MINIMUM_SPEED_SQUARED	FIX24(0.0004)
#define	NUM_FRIC_VALS			6
#define	FRIC_SPEED_SCALE		10000		// integer, see update_ball_movement
#define	NUM_FRICTION_VALS_Z		8
#define	FRIC_SPEED_SCALE_Z		1

static const long friction[NUM_FRIC_VALS][2]={
								{FIX24(Q_FACTOR*4), FIX24(R_FACTOR*4)},
								{FIX24(Q_FACTOR*3), FIX24(R_FACTOR*3)},
								{FIX24(Q_FACTOR*2.5), FIX24(R_FACTOR*2.5)},
								{FIX24(Q_FACTOR*2), FIX24(R_FACTOR*2)},
								{FIX24(Q_FACTOR*1.5), FIX24(R_FACTOR*1.5)},
								{FIX24(Q_FACTOR), FIX24(R_FACTOR)}};

static const long angle_z_friction[NUM_FRICTION_VALS_Z]={
								FIX24(Z_AXIS_SPIN_FRICTION*9.5),
								FIX24(Z_AXIS_SPIN_FRICTION*9),
								FIX24(Z_AXIS_SPIN_FRICTION*7),
								FIX24(Z_AXIS_SPIN_FRICTION*6),
								FIX24(Z_AXIS_SPIN_FRICTION*5),
								FIX24(Z_AXIS_SPIN_FRICTION*5),
								FIX24(Z_AXIS_SPIN_FRICTION*5),
								FIX24(Z_AXIS_SPIN_FRICTION*5)};

static const long	cush_loss = FIX24(0.95);
static const long	cush_loss_perp = FIX24(0.95);
static const long	ang_cush_loss = FIX24(0.1);
#define	CUSH_SPIN_SCALE		FIX24(0.005)
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	add_col_to_list( short ball_num, long x, long y)
{
	if (ball_col_list_pos>=BALL_COL_LIST_SIZE) return;

	ball_col[ball_col_list_pos].ball_num=ball_num;
	ball_col[ball_col_list_pos].x=x;
	ball_col[ball_col_list_pos].y=y;

	ball_col_list_pos++;

	return;
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void reset_col_list(void)
{
	ball_col_list_pos=0;
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	pocketed_ball(long ball_num, long pock, long ang)
{
	long	temp;
	long	miss_respot = NO;
	ballstruct	*thisball = &ball[ball_num];

	if((temp = thisball->active) == NO) return;
	if(temp == 2) return;

	thisball->active = NO;

	add_col_to_list(ball_num, thisball->x, thisball->y);

	if(activated_lines == YES) return;
// RESTORE: AI.C take_shot ends a trial shot as soon as potted_good_ball_flag
// is set, but in_pocket returned here before anything set it, so no trial
// could succeed.  The owner's decision: set it for a potted object ball in
// a trial.  The rules below are not run in a trial (they assign colours and
// keep scores); any object ball counts.
	if(calculating_computer_shot == YES)
		{
		if(ball_num != WHITEBALL) potted_good_ball_flag = YES;
		return;
		}

	potted_flag=YES;

// Save Ball Potted, In Case We Need To Re-Spot It.
	if((GAME_TYPE == UK_POOL) && (break_flag == YES) && (thisball->num != 8))	// RESTORE: parenthesis
		{
		miss_respot = YES;
		}

	if((thisball->numstruct > WHITE_BALL) && (miss_respot == NO))	// RESTORE: parenthesis
		{
		potted_ball[pot_count] = thisball->numstruct;
		if(pot_count == 0) saved_ball_count = nine_ball_count;
		pot_count++;
		}

	if(ball_num == WHITEBALL)
		{
		pocket_num = 100;

// Check to see if ball was pocketed on left or right hand side
// of the screen

		if(ball_ptr[thisball->numstruct]->world_pos.x == 0)
			{
// if in middle of screen, play sample through both speakers
			add_sample(POT_WHITE, 0);		// RESTORE: was add_sampe
			}
		else
			{
			if(ball_ptr[thisball->numstruct]->world_pos.x < 0)
				{
				add_sample(POT_WHITE, LEFT_PAN);
				}
			else
				{
				add_sample(POT_WHITE, RIGHT_PAN);
				}
			}
		return;
		}

// Check to see if ball was pocketed on left or right hand side
// of the screen

	if(ball_ptr[thisball->numstruct]->world_pos.x == 0)
		{
// if in middle of screen, play sample through both speakers
		add_sample(POT_BALL, 0);		// RESTORE: was add_sampe
		}
	else
		{
		if(ball_ptr[thisball->numstruct]->world_pos.x < 0)
			{
			add_sample(POT_BALL, LEFT_PAN);
			}
		else
			{
			add_sample(POT_BALL, RIGHT_PAN);
			}
		}

	if(show_shot_flag == YES) show_panel=PANEL_ON_TIME;

	if(SNOOKER_MODE == YES)
		{
		snooker_pot_check(ball_num);
		}
	else
		{
		switch(GAME_TYPE)		// RESTORE: was game_type; BALL.C switches on GAME_TYPE
			{
			case	UK_POOL:
					uk_pot_check(ball_num);
					break;

			case	US_8BALL:
					us_8ball_check(ball_num);
					break;

			case	US_POOL:
					us_pot_check(ball_num);
					break;

			case	CONTINUOUS:
					continuous_check(ball_num,pock);	// RESTORE: pock argument
					break;

			case	SPEED_POOL:
					potted_good_ball_flag = YES;
					speed_pot++;
					if(speed_pot == LAST_BALL-1) start_time = NO;
					break;

			case	KILLER_POOL:
					break;

			case	NMS_POOL:
//	Set which side all other same colours must go down
					if(players[0].ball_type == NULL)
						{
// Refresh whole screen to show coloured discs
						num_frames_camera_still = 0;
						if(thisball->obj_type == YELLOW)
// setup pocket for balls to go down
							{
							if((pock != 0) && (pock<4))
								{
								nms_pockets[1] = YELLOW;
								nms_pockets[2] = YELLOW;
								nms_pockets[3] = YELLOW;
								nms_pockets[0] = RED;
								nms_pockets[5] = RED;
								nms_pockets[4] = RED;
								}
							else
								{
								nms_pockets[1] = RED;
								nms_pockets[2] = RED;
								nms_pockets[3] = RED;
								nms_pockets[0] = YELLOW;
								nms_pockets[5] = YELLOW;
								nms_pockets[4] = YELLOW;
								}
							}
						else
							{
							if((pock != 0) && (pock<4))
								{
								nms_pockets[1] = RED;
								nms_pockets[2] = RED;
								nms_pockets[3] = RED;
								nms_pockets[0] = YELLOW;
								nms_pockets[5] = YELLOW;
								nms_pockets[4] = YELLOW;
								}
							else
								{
								nms_pockets[1] = YELLOW;
								nms_pockets[2] = YELLOW;
								nms_pockets[3] = YELLOW;
								nms_pockets[0] = RED;
								nms_pockets[5] = RED;
								nms_pockets[4] = RED;
								}
							}
						}
					nms_pot_check(ball_num,pock);
					break;
			}
		}
	if(potted_good_ball_flag == YES)
		{
		if(Player_Active == 0)
			{
			player_info[left_player].potting_ratio++;
			}
		else
			{
			player_info[right_player].potting_ratio++;
			}
		if(white_hit_cushion > 1) add_sample(CLAPPING,0);
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	continuous_check(long ball_num, long pock)
{
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	nms_pot_check(long ball_num, long pock)
{
	ballstruct	*bi = &ball[ball_num];

	potted_good_ball_flag = YES;
	if(players[0].ball_type == NULL)
		{
		switch(bi -> obj_type)
			{
			case	RED:
					players[Player_Active].ball_type = RED;
					players[1-Player_Active].ball_type = YELLOW;
					break;

			case	YELLOW:
					players[1-Player_Active].ball_type = RED;
					players[Player_Active].ball_type = YELLOW;
					break;

			case	BLACK:
					potted_black_wrong_flag = YES;
					break;
			}
		p1_stored_pot_type = players[0].ball_type;
		p2_stored_pot_type = players[1].ball_type;
		return;
		}

	if(players[Player_Active].ball_type == bi-> obj_type)	// RESTORE: was =, an assignment
		{
		if(nms_pockets[pock] != bi -> obj_type)
			{
			if(bi -> obj_type == RED) potted_red_wrong_flag = YES;
			if(bi -> obj_type == YELLOW) potted_yellow_wrong_flag = YES;
			}
		}
	else
		{
		if(bi -> obj_type == BLACK) potted_black_wrong_flag = YES;
		if(bi -> obj_type == RED) potted_red_wrong_flag = YES;
		if(bi -> obj_type == YELLOW) potted_yellow_wrong_flag = YES;
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	uk_pot_check( long ball_num )
{
	ballstruct	*bi = &ball[ball_num];

	if((players[0].ball_type == NULL) && (players[1].ball_type == NULL))
		{
		potted_good_ball_flag = YES;	// RESTORE: set per case as in BALL.C, not for every pot
		switch(bi -> obj_type)
			{
			case	RED:
					players[Player_Active].ball_type = RED;
					players[1-Player_Active].ball_type = YELLOW;
					break;

			case	YELLOW:
					players[1-Player_Active].ball_type = RED;
					players[Player_Active].ball_type = YELLOW;
					break;

			case	BLACK:
					potted_black_wrong_flag = YES;
					break;
			}
		p1_stored_pot_type = players[0].ball_type;
		p2_stored_pot_type = players[1].ball_type;
		return;
		}

	if(players[Player_Active].ball_type != bi->obj_type)
		{
// Test if have a free ball
		if(UK_FREE_BALL == NO)
			{
			if((Two_Shots > 1) && (bi->obj_type != BLACK) && (used_free_ball == NO))	// RESTORE: was old_type
				{
				potted_good_ball_flag = YES;		// RESTORE: as BALL.C
				return;
				}
			}

		if(bi -> obj_type == BLACK) potted_black_wrong_flag = YES;
		if(bi -> obj_type == RED) potted_red_wrong_flag = YES;
		if(bi -> obj_type == YELLOW) potted_yellow_wrong_flag = YES;
		}
	else
		{
		potted_good_ball_flag = YES;		// RESTORE: as BALL.C
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	us_8ball_check( long ball_num)
{
	ballstruct	*bi = &ball[ball_num];

	if(bi->obj_type == BLACK)		// RESTORE: was bi->obj->type
		{
		if(players[Player_Active].ball_type != bi->obj_type)
			{
			potted_red_wrong_flag = YES;
			}
		}

	if(players[0].ball_type == NULL)
		{
		potted_good_ball_flag = YES;	// RESTORE: set per case as in BALL.C, not for every pot
		switch(bi -> obj_type)
			{
			case	RED:
					players[Player_Active].ball_type = RED;
					players[1-Player_Active].ball_type = YELLOW;
					break;

			case	YELLOW:
					players[1-Player_Active].ball_type = RED;
					players[Player_Active].ball_type = YELLOW;
					break;

			case	BLACK:
					potted_black_wrong_flag = YES;
					break;
			}
		p1_stored_pot_type = players[0].ball_type;
		p2_stored_pot_type = players[1].ball_type;
		return;
		}

	if(players[Player_Active].ball_type != bi->obj_type)
		{
		if(bi -> obj_type == BLACK) potted_black_wrong_flag = YES;
		if(bi -> obj_type == RED) potted_red_wrong_flag = YES;
		if(bi -> obj_type == YELLOW) potted_yellow_wrong_flag = YES;
		}
	else
		{
		potted_good_ball_flag = YES;		// RESTORE: as BALL.C
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	us_pot_check(long ball_num)
{
	long	c, amount;
	ballstruct	*bi = &ball[ball_num];

	if((bi->num == nine_ball_count) || (LOWEST_BALL_HIT_FIRST_FLAG == YES))
		{
		potted_good_ball_flag = YES;
		draw_twice = 2;

// score for ball #1
		amount = 50;
		if(bi->num != nine_ball_count)
			{
			potted_hi_ball_flag = YES;

// multiply score by potted ball number
			amount *= bi->num;

// check for 9 ball potted & if so award special bonus.
			if(bi->num == 9) amount = 500;
			if(show_shot_flag == YES)
				{
				Add_Score(amount);
				}
			return;		// RESTORE: semicolon
			}

		if(bi->num == 9) amount = 500;
		if(show_shot_flag == YES)
			{
			Add_Score(amount);
			}
		if(nine_ball_count == LAST_BALL-1) return;

		c=1;
		while(1)
			{
			while(ball[c].num != nine_ball_count)	// RESTORE: was bi->num, which never changes in the loop
				{
				c++;
				}

			if(ball[c].active == YES) break;

			c=1;
			nine_ball_count++;
			}
		}
	else
		{
		lowest_ball_not_hit_first = YES;
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	snooker_pot_check(long ball_num)
{
	long	amount, count, count1;
	ballstruct	*bi = &ball[ball_num];

	if(show_shot_flag == NO) return;

	potted_good_ball_flag=YES;
	count = 1;

	while( ball[count].active == NO)
		{
		count++;
		}

	if(count<16)
		{
		if(SNOOKER_SHOOT_MODE == POT_RED_BALL)	// RESTORE: was SNOOKER_SHOT_MODE
			{
			if(bi->ball_colour != RED)
				{
				potted_yellow_wrong_flag = YES;
				potted_good_ball_flag = NO;

// potted coloured ball so award other player the points.
				switch(bi->ball_colour)
					{
					case	YELLOW:
					case	GREEN:
					case	BROWN:
							amount = 4;
							break;

					case	BLUEBALL:
							amount = 5;
							break;

					case	PURPLE:
							amount = 6;
							break;

					case	BLACK:
							amount = 7;
							break;
					}
				if(hit_red_ball_flag == NO)
					{
					Add_Score_2_other_player( amount );
					}
				return;
				}

// else if potted red correctly award 1 point.
			Add_Score1( 1 );
			}
		else
			{

// check colour potted correctly.
			switch(bi->ball_colour)
				{
				case	RED:
						potted_yellow_wrong_flag = YES;
						Add_Score_2_other_player(4);
						return;

				case	YELLOW:
						amount = 2;
						break;

				case	GREEN:
						amount = 3;
						break;

				case	BROWN:
						amount = 4;
						break;

				case	BLUEBALL:
						amount = 5;
						break;

				case	PURPLE:
						amount = 6;
						break;

				case	BLACK:
						amount = 7;
						break;
				}
			Add_Score1(amount);
			}
		}
	else
		{

// potted all reds, so now pot colours in correct order
		if(bi->num == count-1)
			{
			switch(bi->ball_colour)
				{
				case	YELLOW:
						amount = 2;
						break;

				case	GREEN:
						amount = 3;
						break;

				case	BROWN:
						amount = 4;
						break;

				case	BLUEBALL:
						amount = 5;
						break;

				case	PURPLE:
						amount = 6;
						break;

				case	BLACK:
						amount = 7;
						break;
				}
			Add_Score1(amount);
			}
		else
			{
			potted_yellow_wrong_flag = YES;
			potted_good_ball_flag = NO;
			switch(bi->ball_colour)
				{
				case	YELLOW:			// RESTORE: a foul gives at least 4, as BALL.C
				case	GREEN:			// (this copy of the table above had 2 and 3)
				case	BROWN:
						amount = 4;
						break;

				case	BLUEBALL:
						amount = 5;
						break;

				case	PURPLE:
						amount = 6;
						break;

				case	BLACK:
						amount = 7;
						break;
				}
			if(hit_red_ball_flag == NO)
				{
				Add_Score_2_other_player(amount);
				}
			}
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	update_ball_movement( long time_step, int ball_num)
{
	long	qx, qy;		// force acting at bottom of ball
	long	rx, ry;		// force acting at point .7 above bed height
	long	a_x, a_y;	// linear accelerations
	long	alpha_x, alpha_y;	// angular accelerations (16.16)
	long	total_velocity;
	int	fric_pos;
	long	xvel, yvel;
	static	int	check_angle_count = 0;
	ballstruct	*bi=&ball[ball_num];

	if(bi->active == 2)
		{
		bi->z -= 84;		// 0.000005		RESTORE: comment said 0.00005
		bi->moving = YES;
		if(bi->z < -(bi->diameter))
			{
			bi->active = NO;
			bi->moving = NO;
			}
		return;
		}

// Don't do anything if ball is not moving
	if((bi->moving == FALSE) || (bi->active == FALSE) || (bi->active == 2))
		{
		return;
		}

// Store previous positions
	bi->x_prev = bi->x;
	bi->y_prev = bi->y;
	bi->z_prev = bi->z;

// Calc position in array for friction
// RESTORE: vx*vx+vy*vy passes 128 (the 8.24 limit) above 8 m/s; it is only
// compared with small values, so saturate it instead
	if((abs(bi->vx) >= FIX24(8)) || (abs(bi->vy) >= FIX24(8)))
		total_velocity = 0x7fffffff;
	else
		total_velocity = mul64(bi->vx,bi->vx)+mul64(bi->vy,bi->vy);
	if(total_velocity < MINIMUM_SPEED_SQUARED )
		{
		if(abs(bi->avz)<FIX16(0.03))		// 0.03		RESTORE: avz is 16.16
			{
			bi->moving = FALSE;
			bi->vx = 0;
			bi->vy = 0;
			bi->vz = 0;
			bi->avx = 0;
			bi->avy = 0;
			bi->avz = 0;
			return;
			}
		}

// RESTORE: (int)(total_velocity*10000.0); the 8.24 product overflows, so
// only form it below the last table step
	if(total_velocity >= FIX24(NUM_FRIC_VALS)/FRIC_SPEED_SCALE)
		fric_pos = NUM_FRIC_VALS-1;
	else
		fric_pos = (total_velocity*FRIC_SPEED_SCALE) >> 24;

	if(fric_pos >= NUM_FRIC_VALS) fric_pos = NUM_FRIC_VALS-1;

// Calculate forces r & q
// RESTORE: avx is 16.16 and radius 8.24, so their product is slMulFX
	qx = -mul64(friction[fric_pos][0], (bi->vx - slMulFX(bi->avx, bi->radius)));
	qy = -mul64(friction[fric_pos][0], (bi->vy - slMulFX(bi->avy, bi->radius)));

	rx = -mul64(friction[fric_pos][1], bi->vx);		// RESTORE: was bi-vx
	ry = -mul64(friction[fric_pos][1], bi->vy);

// Calculate linear and angular accelerations
// RESTORE: (qx+rx)/mass and radius*(2*rx-10*qx)/(10*mi), with
// mi = 2*mass*radius*radius/5, as products with per-ball constants (FIXBALL.H)
	a_x = mul64(qx + rx, bi->inv_mass);
	a_y = mul64(qy + ry, bi->inv_mass);

	alpha_x = mul64(rx - 5*qx, bi->spin_scale);
	alpha_y = mul64(ry - 5*qy, bi->spin_scale);

// Now alter velocities with acceleration
	bi->avx += mul64(alpha_x, time_step);
	bi->avy += mul64(alpha_y, time_step);

	bi->vx += mul64(a_x, time_step);
	bi->vy += mul64(a_y, time_step);

// Update positions
	if((xvel = mul64(time_step, bi->vx)) < 0) xvel =- xvel;
	if((yvel = mul64(time_step, bi->vy)) < 0) yvel =- yvel;

	bi->dist_x += xvel;
	bi->dist_y += yvel;

// X
	bi->x += mul64(time_step, bi->vx);
	bi->ax += mul64(time_step, bi->avx);

// Y
	bi->y += mul64(time_step, bi->vy);
	bi->ay += mul64(time_step, bi->avy);

// Z
	fric_pos = (abs(bi->avz) >> 16) * FRIC_SPEED_SCALE_Z;	// RESTORE: avz is 16.16
	if(fric_pos >= (NUM_FRICTION_VALS_Z))
		{
		fric_pos = NUM_FRICTION_VALS_Z-1;
		}
	bi->avz -= mul64(mul64(time_step, angle_z_friction[fric_pos]), bi->avz);
	bi->az += mul64(time_step, bi->avz);

// Only check every 4th time
// RESTORE: angles are 16.16, so wrap at 2 pi in 16.16
	check_angle_count++;
	if(check_angle_count >= 4)
		{
		while(bi->ax > (PI2x>>8)) bi->ax -= (PI2x>>8);
		while(bi->ay > (PI2x>>8)) bi->ay -= (PI2x>>8);
		while(bi->az > (PI2x>>8)) bi->az -= (PI2x>>8);

		while(bi->ax < 0) bi->ax += (PI2x>>8);
		while(bi->ay < 0) bi->ay += (PI2x>>8);
		while(bi->az < 0) bi->az += (PI2x>>8);
		check_angle_count = 0;
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
#define	SNOOKER_DIAMETER	0xd70a3		// 0.0525m
#define	SNOOKER_RADIUS		0x6b851		// 0.02625m
#define	SNOOKER_SQUARED	0xb4a2		// 0.00275625m

#define	USA_DIAMETER		0xe978d		// 0.057m
#define	USA_RADIUS			0x74bc6		// 0.0285m
#define	USA_SQUARED			0xd4ed		// 0.003249m

#define	UK_DIAMETER			0xcfaac		// 0.0507m
#define	UK_RADIUS			0x67d56		// 0.02535m
#define	UK_SQUARED			0xa875		// 0.00257049m

#define	WHITE_MASS			0x1d70a3		// 0.115kg
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	init_ball_position( int ball_num )	// RESTORE: was init_ball_positions; BALL.C and PROTOS.H name
{
	ballstruct	*bi = &ball[ball_num];

	bi->vx = bi->vy = bi->vz = 0;		// RESTORE: were bi=vz etc.
	bi->ax = bi->ay = bi->az = 0;
	bi->old_ax = bi->old_ay = bi->old_az = 0;
	bi->avx = bi->avy = bi->avz = 0;

	bi->moving = FALSE;
	bi->active = TRUE;

	if(ball_num != WHITE_BALL)
		{
		bi->mass = ball_mass;
		}
	else
		{
		bi->mass = WHITE_MASS;
		}

	if( SNOOKER_MODE == YES)
		{
		bi->diameter = SNOOKER_DIAMETER;
		bi->diameter_squared = SNOOKER_SQUARED;
		bi->radius = SNOOKER_RADIUS;
		}
	else
		{
		switch(TABLESIZE_TYPE)
			{
			case	0:
					bi->diameter = USA_DIAMETER;
					bi->diameter_squared = USA_SQUARED;
					bi->radius = USA_RADIUS;
					break;

			case	1:
					bi->diameter = UK_DIAMETER;
					bi->diameter_squared = UK_SQUARED;
					bi->radius = UK_RADIUS;
					break;

			case	2:
					bi->diameter = USA_DIAMETER;
					bi->diameter_squared = USA_SQUARED;
					bi->radius = USA_RADIUS;
					break;
			}
		}

	bi->x = bi->y = 0;
	bi->z = bi->radius;
	bi->x_prev = bi->y_prev = bi->z_prev = 0;

// Moment of inertia
//	bi->mi = 2*bi->mass*(bi->radius*bi->radius)/5;
	bi->mi = get_moment(bi->mass,bi->radius);

// RESTORE: constants update_ball_movement needs: 1/mass, and
// radius/(5*mi) = 1/(2*mass*radius) in 16.16 (mi itself has only ~3 digits in 8.24)
	bi->inv_mass = fixdiv(FIX24_ONE, bi->mass);
	bi->spin_scale = fixdiv(0x10000, 2*mul64(bi->mass, bi->radius));
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// RESTORE: the cushion checks put the ball back on the cushion line and then
// run update_ball_movement for the time since it crossed, |diff/v|.  When the
// ball moves almost along the cushion, or the cushion table says the line is
// far from the ball, that time is seconds long, and one explicit Euler step of
// friction that long reverses and multiplies the velocity (the 1996 build
// went from 1.8 m/s to 370 m/s in one frame).  The ball crossed during the
// step just taken, so the time since is at most that step: clamp it.
// FIXBALL_NO_EXTRA_CLAMP builds the 1996 behaviour, for comparison tests.
static long	extra_time(long diff, long vel, long current_time_step)
{
	long	t;

	t = fixdiv(diff, vel);
	if(t < 0) t = -t;
#ifndef	FIXBALL_NO_EXTRA_CLAMP
	if(t > current_time_step) t = current_time_step;
#endif
	return(t);
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	check_cushions_float( int ball_num, long current_time_step)
{
	ballstruct	*bi = &ball[ball_num];

	if((bi->moving == FALSE) || (bi->active == FALSE) || (bi->active == 2)) return;

// RESTORE: for speed only, same results: each check below does nothing
// unless the ball is past its cushion's line (its own first test, made
// here before the call, in the same order)
#ifndef	DONT_CHECK_X_CUSH
	if((min_cush_x1 - bi->x) > 0) check_cushions_x1(ball_num, current_time_step);
	if((max_cush_x4 - bi->x) < 0) check_cushions_x4(ball_num, current_time_step);
#endif

#ifndef	DONT_CHECK_Y_CUSH
	if((max_cush_y2 - bi->y) < 0) check_cushions_y2(ball_num, current_time_step);
	if((min_cush_y3 - bi->y) > 0) check_cushions_y3(ball_num, current_time_step);
#endif
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// RESTORE: x1 and x4 compared bi->y with cush_x and used max_cush_x1; y3 used
// max_cush_y2 and >.  All four now test as BALL.C does.
void	check_cushions_x1(int ball_num, long current_time_step)
{
	long	cush_x, x_diff, extra_time_step;
	ballstruct	*bi = &ball[ball_num];

	if((min_cush_x1 - bi->x) > 0 )
		{
		cush_x = get_cush_x(1,0,bi->y);
		if((cush_x == FIX24(-10)) || (cush_x == FIX24(10))) check_all_pockets_x1(ball_num, 1);
		if(bi->x < cush_x)
			{
			// calculate extra_time_step
			x_diff = (bi->x - cush_x);
			extra_time_step = extra_time(x_diff, bi->vx, current_time_step);

			bi->x = cush_x;
			add_col_to_list(ball_num, bi->x, bi->y);
			cush_rebound_x(ball_num, get_cush_ang(1,0,bi->y), 1);
			// update time lost by repositioning ball
			update_ball_movement( extra_time_step, ball_num);

			// mark's cushion checks
			bi->hitcush++;
			if(bi->graphic_type == WHITE_BALL) white_hit_cushion = YES;
			else cushion_hit++;
			}
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	check_cushions_y2(int ball_num, long current_time_step)
{
	long	cush_y, y_diff, extra_time_step;
	ballstruct	*bi = &ball[ball_num];

	if((max_cush_y2 - bi->y) <0 )
		{
		cush_y = get_cush_y(3,bi->x,0);
		check_all_pockets_y2(ball_num, 0);
		if(bi->y > cush_y)
			{
			// calculate extra_time_step
			y_diff = (cush_y - bi->y);
			extra_time_step = extra_time(y_diff, bi->vy, current_time_step);

			bi->y = cush_y;
			add_col_to_list(ball_num, bi->x, bi->y);
			cush_rebound_y(ball_num, get_cush_ang(3,bi->x,0), 3);
			// update time lost by repositioning ball
			update_ball_movement( extra_time_step, ball_num);

			// mark's cushion checks
			bi->hitcush++;
			if(bi->graphic_type == WHITE_BALL) white_hit_cushion = YES;
			else cushion_hit++;
			}
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	check_cushions_y3(int ball_num, long current_time_step)
{
	long	cush_y, y_diff, extra_time_step;
	ballstruct	*bi = &ball[ball_num];

	if((min_cush_y3 - bi->y) > 0 )
		{
		cush_y = get_cush_y(2,bi->x,0);
		check_all_pockets_y3(ball_num, 2);
		if(bi->y < cush_y)
			{
			// calculate extra_time_step
			y_diff = (bi->y - cush_y);
			extra_time_step = extra_time(y_diff, bi->vy, current_time_step);

			bi->y = cush_y;
			add_col_to_list(ball_num, bi->x, bi->y);
			cush_rebound_y(ball_num, get_cush_ang(2,bi->x,0), 2);
			// update time lost by repositioning ball
			update_ball_movement( extra_time_step, ball_num);

			// mark's cushion checks
			bi->hitcush++;
			if(bi->graphic_type == WHITE_BALL) white_hit_cushion = YES;
			else cushion_hit++;
			}
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	check_cushions_x4(int ball_num, long current_time_step)
{
	long	cush_x, x_diff, extra_time_step;
	ballstruct	*bi = &ball[ball_num];

	if((max_cush_x4 - bi->x) <0 )
		{
		cush_x = get_cush_x(4,0,bi->y);
		if((cush_x == FIX24(-10)) || (cush_x == FIX24(10))) check_all_pockets_x4(ball_num, 3);
		if(bi->x > cush_x)
			{
			// calculate extra_time_step
			x_diff = (bi->x - cush_x);
			extra_time_step = extra_time(x_diff, bi->vx, current_time_step);

			bi->x = cush_x;
			add_col_to_list(ball_num, bi->x, bi->y);
			cush_rebound_x(ball_num, get_cush_ang(4,0,bi->y), 4);
			// update time lost by repositioning ball
			update_ball_movement( extra_time_step, ball_num);

			// mark's cushion checks
			bi->hitcush++;
			if(bi->graphic_type == WHITE_BALL) white_hit_cushion = YES;
			else cushion_hit++;
			}
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// RESTORE: the cushion tables are indexed from their middle, which the table's own file
// gives (x_points, y_points: 2000 and 1000 for the Potters and Crypt files, 1500 and
// 750 for the Square), not from the array size the code assumed (NUM_CUSH_POINTS_X/Y
// /2, 1000 and 500).  The Square's points sit 250 entries (x) and 125 (y) before
// where the old offset looked, 0.5 m and 0.25 m: its cushions were found in the
// wrong places and balls left the table after a break.  The Potters files are
// unchanged (the same numbers).
#define BALL_DEBUG
long	get_cush_ang(int bump_num, long x, long y)
{
	int	array_pos;
	long	cush_ang;

	switch(bump_num)
		{
		case	1:
				array_pos=y/bumper_scale;
				array_pos+=ypoints/2;
		#ifdef	BALL_DEBUG
				if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_Y))
					{
					array_pos=0;
					}
		#endif
				cush_ang=cushions_y1[array_pos].cush_ang;
				break;
		case	2:
				array_pos=x/bumper_scale;
				array_pos+=xpoints/2;
		#ifdef	BALL_DEBUG
				if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_X))
					{
					array_pos=0;
					}
		#endif
				cush_ang=cushions_x2[array_pos].cush_ang;
				break;
		case	3:
				array_pos=x/bumper_scale;
				array_pos+=xpoints/2;
		#ifdef	BALL_DEBUG
				if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_X))
					{
					array_pos=0;
					}
		#endif
				cush_ang=cushions_x3[array_pos].cush_ang;
				break;

		case	4:
				array_pos=y/bumper_scale;
				array_pos+=ypoints/2;
		#ifdef	BALL_DEBUG
				if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_Y))
					{
					array_pos=0;
					}
		#endif
				cush_ang=cushions_y4[array_pos].cush_ang;
				break;
		}
	return cush_ang;
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
long	get_cush_x(int	bump_num, long x, long y)
{
	int	array_pos;
	long	intp_x;

	switch(bump_num)
		{
		case	1:
				array_pos=y/bumper_scale;
				array_pos+=ypoints/2;
		#ifdef	BALL_DEBUG
				if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_Y))
					{
					array_pos=0;
					}
		#endif
				intp_x=cushions_y1[array_pos].x;
				return intp_x;
				break;

		case 4:
				array_pos=y/bumper_scale;
				array_pos+=ypoints/2;
		#ifdef	BALL_DEBUG
				if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_Y))
					{
					array_pos=0;
					}
		#endif
				intp_x=cushions_y4[array_pos].x;
				return intp_x;
				break;
		}
	return FIX24(10);		// RESTORE: 10.0 in 8.24, the no-cushion value
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
long	get_cush_y(int bump_num, long x, long y)
{
	int 	array_pos;
	long	intp_y;


	switch(bump_num)
		{
		case 2:
				array_pos=x/bumper_scale;
				array_pos+=xpoints/2;
		#ifdef	BALL_DEBUG
				if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_X))
					{
					array_pos=0;
					}
		#endif
				intp_y=cushions_x2[array_pos].y;
				return intp_y;
				break;

		case 3:
				array_pos=x/bumper_scale;
				array_pos+=xpoints/2;
		#ifdef	BALL_DEBUG
				if ((array_pos<0)||(array_pos>=NUM_CUSH_POINTS_X))
					{
					array_pos=0;
					}
		#endif
				intp_y=cushions_x3[array_pos].y;
				return intp_y;
				break;
		}
	return FIX24(10);		// RESTORE: 10.0 in 8.24
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
#define	slAng2Deg(_n_)	(((_n_)*65536)/360)
#define	slPI	3.141592654
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// RESTORE: angle arrives in degrees, 16.16 (cush_st).  BALL.C negates it and
// turns it into radians; here it becomes an SGL angle for slSin/slCos.
void	cush_rebound_x( int ball_num, long angle, int cush_num)
{
	long	temp_vx, temp_vy;
	long	ang_vel, spin_vx, spin_vy;
	ANGLE	spin_ang;
	long	cush_spin_scale = CUSH_SPIN_SCALE;
	ballstruct	*bi = &ball[ball_num];		// RESTORE: was ball[ball_num]
	ANGLE	a = DEG16_TO_ANG(-angle);		// RESTORE: BALL.C's angle=-angle, in SGL units

// Calculate spin transfer to velocity
	if(cush_num == 4) cush_spin_scale = -cush_spin_scale;

	temp_vx = -(mul64(bi->vx,slCos(a)<<8)+mul64(bi->vy,slSin(a)<<8));
	temp_vy = -mul64(bi->vx,slSin(a)<<8)+mul64(bi->vy,slCos(a)<<8);	// RESTORE: was -(vx*sin+vy*cos)

	bi->vx = mul64(temp_vx,slCos(-a)<<8)+mul64(temp_vy,slSin(-a)<<8);
	bi->vy = mul64(-temp_vx,slSin(-a)<<8)+mul64(temp_vy,slCos(-a)<<8);

	ang_vel = slMulFX(bi->avz,cush_spin_scale);	// RESTORE: avz is 16.16
	spin_ang = a+0x4000;							// RESTORE: + 90deg; was slAng2Deg(slPI/2)
	spin_vx = mul64(ang_vel, slCos(spin_ang)<<8);
	spin_vy = mul64(ang_vel, slSin(spin_ang)<<8);

	bi->vx = mul64(bi->vx, cush_loss);
	bi->vy = mul64(bi->vy, cush_loss_perp);
	bi->avx = mul64(bi->avx, ang_cush_loss);
	bi->avy = mul64(bi->avy, ang_cush_loss);
	bi->avz = 0;

	bi->vx += spin_vx;
	bi->vy += spin_vy;

	bi->dist_x += ang_cush_loss*5;
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	cush_rebound_y( int ball_num, long angle, int cush_num)
{
	long	temp_vx, temp_vy;
	long	ang_vel, spin_vx, spin_vy;
	ANGLE	spin_ang;
	long	cush_spin_scale = CUSH_SPIN_SCALE;
	ballstruct	*bi = &ball[ball_num];		// RESTORE: was ball[ball_num]
	ANGLE	a;

	angle -= 0x5a0000;	// 90deg
	a = DEG16_TO_ANG(angle);					// RESTORE: degrees to SGL angle

// Calculate spin transfer to velocity
	if(cush_num == 2) cush_spin_scale = -cush_spin_scale;
	ang_vel = slMulFX(bi->avz,cush_spin_scale);	// RESTORE: avz is 16.16

	spin_ang = a+0x4000;							// RESTORE: + 90deg; was slAng2Deg(slPI/2)
	spin_vx = mul64(ang_vel, slCos(spin_ang)<<8);
	spin_vy = mul64(ang_vel, slSin(spin_ang)<<8);

	temp_vx = (mul64(bi->vx,slCos(a)<<8)+mul64(bi->vy,slSin(a)<<8));
	temp_vy = -(mul64(-bi->vx,slSin(a)<<8)+mul64(bi->vy,slCos(a)<<8));

	bi->vx = mul64(temp_vx,slCos(-a)<<8)+mul64(temp_vy,slSin(-a)<<8);
	bi->vy = mul64(-temp_vx,slSin(-a)<<8)+mul64(temp_vy,slCos(-a)<<8);

	bi->vx = mul64(bi->vx, cush_loss);
	bi->vy = mul64(bi->vy, cush_loss_perp);
	bi->avx = mul64(bi->avx, ang_cush_loss);
	bi->avy = mul64(bi->avy, ang_cush_loss);
	bi->avz = 0;

	bi->vx += spin_vx;
	bi->vy += spin_vy;

	bi->dist_y += ang_cush_loss*5;		// RESTORE: dist_y, as BALL.C
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
long	get_ball_grad(int ball_num)
{
	ballstruct	*bi = &ball[ball_num];

	if((bi->x_prev == 0) || (bi->y_prev == 0)) return(0);	// RESTORE: second test was x_prev again
//	if((bi->y_prev - bi->y)==0) return(0);
// to avoid Divide By Zero shouldn't the above line be :-
	if((bi->x_prev - bi->x)==0) return(0);

	return(fixdiv(bi->y_prev - bi->y, bi->x_prev-bi->x));	// RESTORE: 8.24 divide
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
long	get_ball_intercept(int ball_num, long grad)
{
	ballstruct	*bi = &ball[ball_num];

	return(bi->y - mul64(grad,bi->x));		// RESTORE: parenthesis
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
long	find_nearest_x_dist(int ball_num)
{
	long	nearest_dist;
	ballstruct	*bi = &ball[ball_num];

	nearest_dist = abs(max_cush_x4 - bi->x);
	if(nearest_dist > abs( min_cush_x1 - bi->x))
		{
		nearest_dist = abs(min_cush_x1 - bi->x);	// RESTORE: was max_cush_x1
		}
	return(nearest_dist);
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// RESTORE: used bi->x and max_cush_y3; BALL.C measures y to the y cushions
long	find_nearest_y_dist(int ball_num)
{
	long	nearest_dist;
	ballstruct	*bi = &ball[ball_num];

	nearest_dist = abs(max_cush_y2 - bi->y);
	if(nearest_dist > abs( min_cush_y3 - bi->y))
		{
		nearest_dist = abs(min_cush_y3 - bi->y);
		}
	return(nearest_dist);
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// RESTORE: slSquart of an 8.24 square is 4.12; fix_sqrt returns 8.24 for the
// comparisons with pockets[].radius in the pocket checks below.
void	check_all_pockets_x1(long ball_num, long bumper)
{
	long	x, y, x1, y1;
	long	hypot;
	ballstruct	*bi = &ball[ball_num];

	x = bi->x;
	y = bi->y;

	x1 = abs(pockets[0].x - x);
	y1 = abs(pockets[0].y - y);

	x1 = mul64(x1,x1);
	y1 = mul64(y1,y1);
	hypot = fix_sqrt(x1+y1);		// RESTORE: was slSquart

	if((bi->x < pockets[0].x) || (bi->y > pockets[0].y))
		{
		pocketed_ball(ball_num, 3, 0);
		}

// check actual pocket
	if(hypot <= pockets[0].radius)
		{
		pocketed_ball(ball_num, 0, 0);
		return;
		}

	x1 = abs(pockets[1].x - x);
	y1 = abs(pockets[1].y - y);

	x1 = mul64(x1,x1);
	y1 = mul64(y1,y1);
	hypot = fix_sqrt(x1+y1);		// RESTORE: was slSquart

	if((bi->x < pockets[1].x) || (bi->y < pockets[1].y))	// RESTORE: y test was >
		{
		pocketed_ball(ball_num, 3, 0);
		}

// check actual pocket
	if(hypot <= pockets[1].radius)
		{
		pocketed_ball(ball_num, 1, 0);		// RESTORE: was pocket 0
		return;
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	check_all_pockets_y2(long ball_num, long bumper)
{
	long	x, y, x1, y1;
	long	hypot;
	ballstruct	*bi = &ball[ball_num];

	if(LOCATION < DIFFERENT_SHAPED_TABLES_INDEX)
		{
		if(bi->y > pockets[5].y)
			{
			pocketed_ball(ball_num, 3, 0);
			}

		if((bi->x > pockets[4].x) || (bi->y > pockets[4].y))	// RESTORE: x test was <
			{
			pocketed_ball(ball_num, 3, 0);
			}

		if((bi->x < pockets[0].x) || (bi->y > pockets[0].y))
			{
			pocketed_ball(ball_num, 3, 0);
			}
		}

	x = bi->x;
	y = bi->y;

	x1 = abs(pockets[0].x - x);
	y1 = abs(pockets[0].y - y);

	x1 = mul64(x1,x1);
	y1 = mul64(y1,y1);
	hypot = fix_sqrt(x1+y1);		// RESTORE: was slSquart

// check actual pocket
	if(hypot <= pockets[0].radius/4)
		{
		pocketed_ball(ball_num, 0, 0);
		return;
		}

	x1 = abs(pockets[5].x - x);
	y1 = abs(pockets[5].y - y);

	x1 = mul64(x1,x1);
	y1 = mul64(y1,y1);
	hypot = fix_sqrt(x1+y1);		// RESTORE: was slSquart

// check actual pocket
	if(hypot <= pockets[5].radius)
		{
		pocketed_ball(ball_num, 5, 0);
		return;
		}

	x1 = abs(pockets[4].x - x);
	y1 = abs(pockets[4].y - y);

	x1 = mul64(x1,x1);
	y1 = mul64(y1,y1);
	hypot = fix_sqrt(x1+y1);		// RESTORE: was slSquart

// check actual pocket
	if(hypot <= pockets[4].radius)
		{
		pocketed_ball(ball_num, 4, 0);
		return;
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	check_all_pockets_y3(long ball_num, long bumper)
{
	long	x, y, x1, y1;
	long	hypot,p;
	ballstruct	*bi = &ball[ball_num];

	for(p = 1; p<4; p++)
		{

		if(LOCATION < DIFFERENT_SHAPED_TABLES_INDEX)
			{
			switch( p )
				{
				case	1:
						if((bi->x < pockets[p].x) || (bi->y < pockets[p].y))
							{
							pocketed_ball(ball_num, 3, 0);
							}
						break;

				case	2:
						if(bi->y < pockets[p].y)
							{
							pocketed_ball(ball_num, 3, 0);
							}
						break;

				case	3:
						if((bi->x > pockets[p].x) || (bi->y < pockets[p].y))
							{
							pocketed_ball(ball_num, 3, 0);
							}
						break;
				}
			}

		x = bi->x;
		y = bi->y;

		x1 = abs(pockets[p].x - x);
		y1 = abs(pockets[p].y - y);

		x1 = mul64(x1,x1);
		y1 = mul64(y1,y1);
		hypot = fix_sqrt(x1+y1);		// RESTORE: was slSquart

// check actual pocket
		if(hypot <= pockets[p].radius)
			{
			pocketed_ball(ball_num, p, 0);
			return;
			}
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	check_all_pockets_x4(long ball_num, long bumper)
{
	long	x, y, x1, y1;
	long	hypot;
	ballstruct	*bi = &ball[ball_num];

	if((bi->x > pockets[3].x) || (bi->y < pockets[3].y))
		{
		pocketed_ball(ball_num, 3, 0);
		}

	x = bi->x;
	y = bi->y;

	x1 = abs(pockets[4].x - x);
	y1 = abs(pockets[4].y - y);

	x1 = mul64(x1,x1);
	y1 = mul64(y1,y1);
	hypot = fix_sqrt(x1+y1);		// RESTORE: was slSquart

	if((bi->x > pockets[4].x) or (bi->y > pockets[4].y))
		{
		pocketed_ball(ball_num, 3, 0);
		}

// check actual pocket
	if(hypot <= pockets[4].radius)
		{
		pocketed_ball(ball_num, 4, 0);
		return;
		}

	x1 = abs(pockets[3].x - x);
	y1 = abs(pockets[3].y - y);

	x1 = mul64(x1,x1);
	y1 = mul64(y1,y1);
	hypot = fix_sqrt(x1+y1);		// RESTORE: was slSquart


// check actual pocket
	if(hypot <= pockets[3].radius)
		{
		pocketed_ball(ball_num, 3, 0);
		return;
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
long	check_solid_pocket(long ball_num, long oldx, long oldy)
{
	long	x, y, x1, y1;
	long	p, hypot;
	ballstruct	*bi = &ball[ball_num];

	for(p=0; p<5; p++)
		{
		x = bi->x;
		y = bi->y;

		x1 = abs(pockets[p].x - x);
		y1 = abs(pockets[p].y - y);

		x1 = mul64(x1,x1);			// RESTORE: were x1*=x1, y1*=y1
		y1 = mul64(y1,y1);
		hypot = fix_sqrt(x1+y1);		// RESTORE: was slSquart

		if(hypot <= pockets[p].radius)
			{
			bi->x = oldx;
			bi->y = oldy;
			return(-1);
			}
		}
	return(0);
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// RESTORE: from here on, BALL.C's remaining functions converted to fixed
// point.  BALL.C's init_cushions is inside #if 0 (COMMON.C's is the live one,
// which parses the text table and writes the .csh file); load_cushions
// below reads that .csh, NMS's fixed-point cushion file.
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	make_sin_cos_table( void )
{
// BALL.C's body is commented out; slSin/slCos use FIXTAB.H
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// fixed-point versions of MY3DS.C's identity4x4, multiply4x4 and
// multiply4x1, for rotate2 and hit_ball.  Entries are 8.24; multiply4x1 works
// on a vector of any format, the result keeps that format.
static void	fix_identity4x4(fmatrix4x4 matx)
{
	int	x, y;

	for(x=0;x<4;x++)
		{
		for(y=0;y<4;y++)
			{
			if(y==x) matx[x][y]=FIX24_ONE;
			else matx[x][y]=0;
			}
		}
}

static void	fix_multiply4x4(fmatrix4x4 matx1, fmatrix4x4 matx2, fmatrix4x4 result)
{
	fmatrix4x4	tempresult;
	int	x, y;

	for(y=0;y<4;y++)
		{
		for(x=0;x<4;x++)
			{
			tempresult[x][y]=mul64(matx1[0][y],matx2[x][0])+mul64(matx1[1][y],matx2[x][1])+
							mul64(matx1[2][y],matx2[x][2])+mul64(matx1[3][y],matx2[x][3]);
			}
		}
	for(y=0;y<4;y++)
		{
		for(x=0;x<4;x++)
			{
			result[x][y]=tempresult[x][y];
			}
		}
}

static void	fix_multiply4x1(fmatrix4x4 matx1, fmatrix4x1 matx2, fmatrix4x1 result)
{
	fmatrix4x1	tempresult;
	int	y;

	for(y=0;y<4;y++)
		{
		tempresult[y]=mul64(matx1[0][y],matx2[0])+mul64(matx1[1][y],matx2[1])+
						mul64(matx1[2][y],matx2[2])+mul64(matx1[3][y],matx2[3]);
		}
	for(y=0;y<4;y++)
		{
		result[y]=tempresult[y];
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// This is true martix rotation creator. the 1 in my3ds.c has a -ve
// in yaw for camera system in renderware
// Angles are SGL angles (65536 = 360deg), the matrix 8.24.
void	rotate2(ANGLE roll, ANGLE pitch, ANGLE yaw, fmatrix4x4 spin)
{
	long	sr, sp, sy, cr, cp, cy;
	fmatrix4x4	mr, mp, my;

	sr=slSin(roll)<<8;	cr=slCos(roll)<<8;
	sy=slSin(yaw)<<8;	cy=slCos(yaw)<<8;
	sp=slSin(pitch)<<8;	cp=slCos(pitch)<<8;

	fix_identity4x4(mr);
	fix_identity4x4(my);
	fix_identity4x4(mp);

	mr[0][0] = mr[1][1] = cr;
	mr[1][0] = - (mr[0][1] = sr);

								// prepare pitch matrix
	mp[1][1] = mp[2][2] = -cp;
	mp[1][2] = - (mp[2][1] = -sp);
								// prepare yaw matrix
	my[0][0] = my[2][2] = cy;
	my[0][2] = - (my[2][0] = sy);

	fix_multiply4x4( mr, my, spin );
	fix_multiply4x4( spin, mp, spin );
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// x_hit, y_hit: cue tip offset; ang_z: cue angle (4096 = 360deg, BALL.C
// subtracts 1024); ang_y: cue elevation (butt, 4096 = 360deg); vel: 8.24 m/s.
// BALL.C took ang_z, ang_y and vel as doubles.
void	hit_ball( long ball_num, long x_hit, long y_hit, long ang_z, long ang_y, long vel)
{
	fmatrix4x1	velocity;
	fmatrix4x1	ang_vel;
	fmatrix4x4	ang_transform;
	long	a, count;
	long	tmp_angle;
	long	spin;
	ANGLE	az, ay;
	ballstruct	*bi = &ball[ball_num];

// convert butt to SGL angle (BALL.C: radians)
	ay = (ANGLE)(ang_y << 4);

	if((tmp_angle = ang_z-1024) < 0) tmp_angle += 4096;
	az = (ANGLE)(tmp_angle << 4);

	for(count=0;count<LAST_BALL;count++)
		{
		ball[count].vx=0;
		ball[count].vy=0;
		ball[count].avx=0;
		ball[count].avy=0;
		}

	velocity[0] = vel;
	velocity[1] = 0;
	velocity[2] = 0;
	velocity[3] = 0;

	ang_vel[3] = 0;

	for(a=0;a<4;a++)
		{
		ang_transform[0][a]=0;
		ang_transform[1][a]=0;
		ang_transform[2][a]=0;
		ang_transform[3][a]=0;
		}

	rotate2(az, 0, ay, ang_transform);

	fix_multiply4x1( ang_transform, velocity, velocity);

	bi->vx = velocity[0];
	bi->vy = -velocity[1];

// ((vel/radius)*(-hit/.4))/65536 as 16.16 rad/s: spin is vel/radius in
// 16.16, times 2.5 (y) or 5 (x); slMulFX with the hit offset then gives
// spin*offset/65536 in 16.16
	spin = fixdiv(vel >> 8, bi->radius);
	ang_vel[0] = slMulFX((spin*5)>>1, -y_hit);
	ang_vel[1] = 0;
	ang_vel[2] = slMulFX(spin*5, -x_hit);

	rotate2(az, ay, 0, ang_transform);
	fix_multiply4x1( ang_transform, ang_vel, ang_vel);

	bi->avx = ang_vel[0];
	bi->avy = -ang_vel[1];
	bi->avz = ang_vel[2];

// Ball now moving

	bi->moving = TRUE;
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
#define BODGE_FACTOR	80
#define MAX_HITS	22

// RESTORE: BALL.C's ball_dist table is not needed (get_max_time_step)
// RESTORE: for speed only: the functions every physics step runs are linked
// next to each other (psyq.ld puts .text.hot first), so they share the
// R3000's 4 KB instruction cache instead of evicting each other
#define	HOT_TEXT	__attribute__((section(".text.hot.qdup")))
long	fix_substeps;		// RESTORE: NEW WORK, physics sub-steps done (qdup/new/aifast.c)
void	update_ball_movement( long time_step, int ball_num) HOT_TEXT;
void	check_cushions_float( int ball_num, long current_time_step) HOT_TEXT;
void	update_all_balls( long time_step ) HOT_TEXT;
long	get_max_time_step( long time_step ) HOT_TEXT;
// see get_max_time_step: the least 2^k with 2^(2k-24) >= s (0 for s = 0,
// when no offset can matter)
static inline long	square_reach(long s)
{
	unsigned long	t;
	long	bits = 0;

	if(s <= 0) return(0);
	t = s - 1;
	if(t >> 16) { bits += 16; t >>= 16; }
	if(t >> 8) { bits += 8; t >>= 8; }
	if(t >> 4) { bits += 4; t >>= 4; }
	if(t >> 2) { bits += 2; t >>= 2; }
	if(t >> 1) { bits += 1; t >>= 1; }
	bits += t;			// bits = bit length of s-1: 2^bits >= s
	return(1L << ((bits + 25) >> 1));	// k = ceil((bits+24)/2)
}

// see get_max_time_step
static inline long	closer_than_step(long dist2, long diameter, long long c)
{
	long long	n, k, lo, hi;

	if(c <= 0) return(0);
	k = (c - 1) >> 24;
	n = ((long long)dist2) << 24;
	lo = diameter - k;
	hi = diameter + k + 1;
	if((lo > 0) && (n < lo * lo)) return(0);
	if(n >= hi * hi) return(0);
	return(1);
}
flag	any_balls_moving(void) HOT_TEXT;
// RESTORE: for speed only: get_max_time_step reads the balls' positions and
// flags from a copy at the start of the CPU's scratchpad (1 KB of 1-cycle
// data memory at 1F800000h, used by nothing else in the game or the layer)
typedef	struct	{
	long	x, y;
	unsigned char	active, moving;
} spadball;
#define	SPAD_BALLS	((spadball *)0x1f800000)
struct ball_hits_st {
	short ball1;
	short ball2;
};

static struct ball_hits_st ball_hits[MAX_HITS];
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	update_all_balls( long time_step )
{
	long	xd, yd;
	int	loop, collision_loop;
	long	time_elapsed = 0, current_time_step;
	static	long	prev_time_step;

	reset_col_list();

// make ball bals moving for test
// BALL.C: (time_elapsed+0.00000001)<time_step; 1e-8 is 0 in 8.24 and the
// sub-steps here add up exactly
	while(time_elapsed < time_step)
		{
		current_time_step = get_max_time_step(time_step);

// do ball collisions, if ball is hit, go to previous positions
		if(ball_hits_pos != 0)
			{
			for(loop=0; loop<LAST_BALL; loop++)
				{
				ball[loop].x = ball[loop].x_prev;
				ball[loop].y = ball[loop].y_prev;
				}
			time_elapsed -= prev_time_step;
			}

		for(collision_loop=0; collision_loop<ball_hits_pos; collision_loop++)
			{
			ball_collision2(ball_hits[collision_loop].ball1, ball_hits[collision_loop].ball2);
			}

// update balls and check cushions
// RESTORE: for speed only, same results: both functions return at once for
// a ball that is still (update_ball_movement acts on a ball that is moving
// and on the table, or dropping into a pocket, active 2; check_cushions_float
// on one that is moving and on the table), so the calls are made only for
// the balls they act on, each test made just before its call as there.
		for(loop=0; loop<LAST_BALL; loop++)
			{
			ballstruct	*bl = &ball[loop];

			if((bl->active == 2) || (bl->moving && bl->active))
				update_ball_movement(current_time_step, loop);
			if(bl->moving && bl->active && (bl->active != 2))
				check_cushions_float(loop, current_time_step);
			}
		time_elapsed += current_time_step;
		prev_time_step = current_time_step;
		fix_substeps++;		// RESTORE: NEW WORK, a count for the computer's search (qdup/new/aifast.c)
		}

	if(calculating_computer_shot == NO) DrawScreen();	// RESTORE: not during the computer's trial shots (AI.C take_shot): each 2D draw waits for a VSync, so a trial took several seconds; the physics and the shot chosen are the same.  Display only, the trials are no longer shown in the 2D view (in the 3D build DrawScreen draws nothing)
	if(end_of_frame_flag == YES) return;

// draw line from old position
	if(activated_lines == TRUE)
		{
		for(loop=0;loop<LAST_BALL;loop++)
			{
			if((loop == WHITE_BALL) && (butt > 1) && (x_hit_pos != 0))
				{
				if(new_count == 0)
					{
					}

				xd = abs(ball[loop].x - ball[loop].x_prev);
				yd = abs(ball[loop].y - ball[loop].y_prev);

				if(xd+yd > FIX24(0.005))
					{
					get_new_screen_pos(loop);
					}
				}
			else
				{
				get_new_screen_pos(loop);
				}
			}
		return;
		}

#define	mile_frac	.001

// ball_mileage is a double in GLOBALS.H's player_stats: game statistics,
// left in floating point until the rest of the game is converted
	if(ball[0].moving == YES)
		{
		if(Player_Active == 0)
			{
			player_info[left_player].ball_mileage += mile_frac;
			}
		else
			{
			player_info[right_player].ball_mileage += mile_frac;
			}
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
long	get_max_time_step( long time_step )
{
	long	max_time_step;
	long	min_time_step;
	int	loop1, loop2;
	long	min_ball_dist = 0x7fffffff;		// BALL.C: 1e99
	long	min_dist_ball_vel;
	long	x, y, hypot, xx, b1x, b1y, dsq, skip, reach;
	int	min_dist_ball_num = 0;
	ballstruct	*b1;
	spadball	*s1, *s2;

	max_time_step = time_step/2;
	min_time_step = time_step/4;

	ball_hits_pos = 0;		// reset hit list

// find smallest distance between 2 balls
// RESTORE: BALL.C clears a table of the distances between every two balls
// and fills both halves, so a pair of moving balls is measured once, from
// its lower ball; a distance of 0 reads as not measured and is measured
// again.  The same pairs are visited here, in the same order, without the
// table: (loop1, loop2) was measured before when loop2 < loop1 and ball
// loop2 is moving, unless its distance was 0 (both offsets under 4096,
// whose squares are 0 in 8.24).  Same minimum, ball and hit list.
// RESTORE: for speed only, same results: the positions and flags are read
// from a copy in the scratchpad (1-cycle loads; main RAM stalls each load),
// and a pair whose x or y offset alone squares to at least both the
// smallest distance so far and this ball's diameter squared is passed over,
// as its distance could change neither (it is never below that square).
// Most such pairs are found without multiplying: an offset of 2^k or more
// squares (mul64) to 2^(2k-24) or more, so reach = 2^k, the least such
// power with 2^(2k-24) >= the bound, passes them over at a compare.
	for(loop1=0; loop1<LAST_BALL; loop1++)
		{
		s1 = &SPAD_BALLS[loop1];
		b1 = &ball[loop1];
		s1->x = b1->x;
		s1->y = b1->y;
		s1->active = b1->active;
		s1->moving = b1->moving;
		}

	for(loop1=0; loop1<LAST_BALL; loop1++)
		{
		s1 = &SPAD_BALLS[loop1];

		if((s1->moving) && (s1->active))
			{
			b1x = s1->x;
			b1y = s1->y;
			dsq = ball[loop1].diameter_squared;
			skip = (min_ball_dist > dsq) ? min_ball_dist : dsq;
			reach = square_reach(skip);
			for(loop2=0, s2=SPAD_BALLS; loop2<LAST_BALL; loop2++, s2++)
				{
				if((loop1 == loop2) || !(s2->active)) continue;
				x = s2->x - b1x;
				y = s2->y - b1y;
				if((abs(x) >= reach) || (abs(y) >= reach)) continue;
				if((loop2 < loop1) && (s2->moving) && ((abs(x) >= 4096) || (abs(y) >= 4096))) continue;
// the squares pass the 8.24 range beyond 11 m; such a pair is far apart
// (BALL.C: distance 0x7fffffff, which neither test below passes)
				if((abs(x) >= FIX24(8)) || (abs(y) >= FIX24(8))) continue;
				xx = mul64(x,x);
				if(xx >= skip) continue;
				hypot = xx + mul64(y,y);

// Check to see if any balls have hit each other
				if(hypot < min_ball_dist)
					{
					min_ball_dist = hypot;
					min_dist_ball_num = loop1;
					if(skip > dsq)
						{
						skip = (min_ball_dist > dsq) ? min_ball_dist : dsq;
						reach = square_reach(skip);
						}
					}

				if(hypot < dsq)
					{
// ball has hit another ball, add to list
					ball_hits[ball_hits_pos].ball1 = loop1;
					ball_hits[ball_hits_pos].ball2 = loop2;
					ball_hits_pos++;
// This is to stop the list at end of array when debugging
					if(ball_hits_pos == LAST_BALL) ball_hits_pos--;
					}
				}
			}
		}

// temporary bodge
// calculate new step based on dist/(ball velocity)
	b1 = &ball[min_dist_ball_num];
	if((abs(b1->vx) >= FIX24(8)) || (abs(b1->vy) >= FIX24(8)))
		min_dist_ball_vel = FIX24(11.3);
	else
		{
// RESTORE: for speed only, same results: fix_sqrt(v2) is below
// 2^(ceil(bits(v2)/2) + 12); a wider bound only widens the test below, so
// when the test fails for that bound it fails for the speed itself and the
// step stays whole without the square root
		long	v2 = mul64(b1->vx,b1->vx) + mul64(b1->vy,b1->vy);
		unsigned long	t = v2;
		long	bits = 0, vhi;

		if(t >> 16) { bits += 16; t >>= 16; }
		if(t >> 8) { bits += 8; t >>= 8; }
		if(t >> 4) { bits += 4; t >>= 4; }
		if(t >> 2) { bits += 2; t >>= 2; }
		if(t >> 1) { bits += 1; t >>= 1; }
		bits += t;
		vhi = (bits == 0) ? 0 : (1L << (((bits + 1) >> 1) + 12));
		if((max_time_step <= min_time_step) || !closer_than_step(min_ball_dist, b1->diameter, (long long)max_time_step * ((long long)vhi * 2)))
			return(max_time_step);
		min_dist_ball_vel = fix_sqrt(v2);
		}

// a zero velocity divides to the largest value, which never halves the step
// (BALL.C: an infinity or NaN, with the same effect)
// RESTORE: for speed only, same results.  The step is halved when
// fixdiv(a, 2 v) < max_time_step, a = |fix_sqrt(min_ball_dist) - diameter|.
// fixdiv is floor(a * 2^24 / (2 v)), or its largest value (2 v = 0, or a
// quotient of 128 or more), so the test is a * 2^24 < C, C = max_time_step
// * 2 v, that is a <= K = (C - 1) >> 24 (C > 0).  With S = fix_sqrt(D) =
// floor(sqrt(D * 2^24)) (0 for D = 0), |S - d| <= K holds exactly when
// (d - K)^2 <= D * 2^24 (or d - K <= 0) and D * 2^24 < (d + K + 1)^2: the
// same answer without the square root or the bit-by-bit division.
	while((max_time_step > min_time_step) && closer_than_step(min_ball_dist, b1->diameter, (long long)max_time_step * ((long long)min_dist_ball_vel * 2)))
		{
		max_time_step /= 2;
		}

	return(max_time_step);
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	ball_collision2(short ball1, short ball2)
{
	long	collision_ang;
	long	ball1_ang, z_spin;
	const long	z_spin_mul = 2;
	const long	elasticity = FIX24(0.90);
	long	vel_ball1, vel_ball2;
	long	perp_vel_ball1, perp_vel_ball2;
	long	new_vel_ball1, new_vel_ball2;
	long	momentum_ball1, momentum_ball2;
	long	initial_vel_diff, total_initial_momentum;
	long	sin_ang, cos_ang, dx, dy, dist;
	int	vol;
	long	ball_num;
	ballstruct	*b1 = &ball[ball1];
	ballstruct	*b2 = &ball[ball2];

	if(b1->active == 2) return;
	if(b2->active == 2) return;
	if(b1->active == FALSE) return;
	if(b2->active == FALSE) return;

// calculate angle of collsion between 2 balls
// (measured from +y towards +x: BALL.C's atan2(dx, dy))
	dx = b2->x - b1->x;
	dy = b2->y - b1->y;
	collision_ang = fix_atan2(dx, dy);

// sin and cos of that angle straight from the vector between the centres:
// more precise than slSin/slCos of the 16-bit angle
	dist = fix_sqrt(mul64(dx,dx) + mul64(dy,dy));
	if(dist == 0)
		{
		sin_ang = 0;
		cos_ang = FIX24_ONE;
		}
	else
		{
		sin_ang = fixdiv(dx, dist);
		cos_ang = fixdiv(dy, dist);
		}

// Calculate ang of dirn of ball1
	ball1_ang = fix_atan2(b1->vx, b1->vy);

// get ball velocities along this angle
	vel_ball1 = mul64(b1->vx,sin_ang) + mul64(b1->vy,cos_ang);
	vel_ball2 = mul64(b2->vx,sin_ang) + mul64(b2->vy,cos_ang);
	initial_vel_diff = vel_ball2 - vel_ball1;

// get ball velocities at perpendicular
	perp_vel_ball1 = -mul64(b1->vy,sin_ang) + mul64(b1->vx,cos_ang);
	perp_vel_ball2 = -mul64(b2->vy,sin_ang) + mul64(b2->vx,cos_ang);

// get momentum along angle
	momentum_ball1 = mul64(vel_ball1, b1->mass);
	momentum_ball2 = mul64(vel_ball2, b2->mass);
	total_initial_momentum = momentum_ball1 + momentum_ball2;

// get new velocities
	new_vel_ball2 = fixdiv(total_initial_momentum - mul64(mul64(b2->mass, elasticity), initial_vel_diff),
							b1->mass + b2->mass);

	new_vel_ball1 = mul64(elasticity, initial_vel_diff) + new_vel_ball2;

// calc z spin
// (collision_ang-ball1_ang) in radians, 16.16: one SGL unit is
// 2pi/65536 rad = 6.2832/65536, and 3217/512 = 6.2832; not wrapped, as BALL.C
	z_spin = mul64(((collision_ang - ball1_ang) * 3217) >> 9, initial_vel_diff) * z_spin_mul;

// put these new velocities in ball structure
	b1->vx = mul64(new_vel_ball1,sin_ang) + mul64(perp_vel_ball1,cos_ang);
	b1->vy = mul64(new_vel_ball1,cos_ang) - mul64(perp_vel_ball1,sin_ang);
	b1->avz += z_spin;
	b1->moving = TRUE;

	b2->vx = mul64(new_vel_ball2,sin_ang) + mul64(perp_vel_ball2,cos_ang);
	b2->vy = mul64(new_vel_ball2,cos_ang) - mul64(perp_vel_ball2,sin_ang);
	b2->avz -= z_spin;
	b2->moving = TRUE;

	add_col_to_list(ball2, b2->x, b2->y);
	add_col_to_list(ball1, b1->x, b1->y);

// update foul flags
	if(activated_lines == YES)
		{
		ball_hits_count++;
		return;
		}

	if(b2->graphic_type != WHITEBALL) ball_num = ball2;
	else ball_num = ball1;

	if(SNOOKER_MODE == YES)
		{
		check_hits_snooker( ball_num );
		}
	else
		{
		if(GAME_TYPE == UK_POOL) check_hitsuk( ball_num );
		if(GAME_TYPE == US_8BALL) check_hits8ball( ball_num );
		if(GAME_TYPE == US_POOL) check_hitsus( ball_num );
		if(GAME_TYPE == NMS_POOL) check_hitsuk( ball_num );
		}

	missed_all_balls_flag = NO;

	if(calculating_computer_shot == YES) return;

// trigger samples of balls hitting
// (vx*vy)*1300, with the 8.24 product taken down 12 bits on each side
	vol = ((mul64(b1->vx, b1->vy) >> 12) * 1300) >> 12;
	add_sample_with_volume(ball1, BALL_HIT, 0, vol);

	vol = ((mul64(b2->vx, b2->vy) >> 12) * 1300) >> 12;
	add_sample_with_volume(ball1, BALL_HIT, 0, vol);
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
flag	any_balls_moving(void)
{
	int	loop;

	for(loop=0; loop<LAST_BALL; loop++)
		{
		if((ball[loop].moving == YES) && (ball[loop].active > 0)) return TRUE;
		}
	return FALSE;
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
void	test_camers_in_pocket( long ballnum )
{
	long	x, y, x1, y1, hypot, dist, step;
	long	p;
	ballstruct	*bi = &ball[ballnum];

	if((ballnum == 0) && (camera_in_pocket == NO))
		{
		x = bi->x;
		y = bi->y;

// BALL.C: for(dist=0; dist<(vx+vy); dist+=(vx+vy)*(0.057/2.0))
		step = mul64(bi->vx + bi->vy, FIX24(0.057/2.0));
		if(step <= 0) return;		// the loop does not run, or never ends
		for(dist = 0; dist < (bi->vx + bi->vy); dist += step)
			{
			x += mul64(FIX24(0.057/2.0), bi->vx);
			y += mul64(FIX24(0.057/2.0), bi->vy);

			for(p=0;p<6;p++)
				{
				x1 = pockets[p].x - x;
				y1 = pockets[p].y - y;

				if((abs(x1) >= FIX24(8)) || (abs(y1) >= FIX24(8))) continue;
				x1 = mul64(x1,x1);
				y1 = mul64(y1,y1);
				hypot = fix_sqrt(x1+y1);

				if(hypot <= mul64(pockets[p].radius, FIX24(1.2)))
					{
					setup_camera_in_pocket(p);
					return;
					}
				}
			}
		}
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// Read a table's .csh file (COMMON.C init_cushions writes it in non-FINAL
// builds): xpoints, ypoints, bumper_scale, min_cush_x1, max_cush_y2,
// min_cush_y3, max_cush_x4 as longs (8.24), the six pockets (x, y, radius,
// 8.24), then cushions_y1, x2, x3 and y4 (x, y 8.24; angle degrees 16.16).
int	load_cushions( char *name )
{
	ubyte	*file;
	long	*src;

	if((file = LoadFile(name)) == FAIL) return(-1);
	src = (long *)file;

	xpoints = *src++;
	ypoints = *src++;
	bumper_scale = *src++;
	min_cush_x1 = *src++;
	max_cush_y2 = *src++;
	min_cush_y3 = *src++;
	max_cush_x4 = *src++;

	memcpy(&pockets[0], src, NUM_POCKETS*12);
	src += NUM_POCKETS*3;
	memcpy(&cushions_y1[0], src, ypoints*12);
	src += ypoints*3;
	memcpy(&cushions_x2[0], src, xpoints*12);
	src += xpoints*3;
	memcpy(&cushions_x3[0], src, xpoints*12);
	src += xpoints*3;
	memcpy(&cushions_y4[0], src, ypoints*12);

	Free_Ram_Ptr(file);
	return(1);
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
