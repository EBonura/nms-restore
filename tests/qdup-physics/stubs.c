/*
 * Game-side storage and functions FIXBALL.C needs, for the physics test.
 * In the game GLOBALS.H holds the storage and the other modules the
 * functions; here they are zeroed state and empty bodies.
 */
#include "fixball.h"
#include "shot.h"

/* physics data (fixed point, FIXBALL.H) */
ballstruct ball[MAXBALLS];
long LAST_BALL;
struct cush_st cushions_x3[NUM_CUSH_POINTS_X];
struct cush_st cushions_x2[NUM_CUSH_POINTS_X];
struct cush_st cushions_y1[NUM_CUSH_POINTS_Y];
struct cush_st cushions_y4[NUM_CUSH_POINTS_Y];
long max_cush_y2, min_cush_y3, max_cush_x4, min_cush_x1, bumper_scale, ball_mass;
struct pockets_st pockets[NUM_POCKETS];
short ball_col_list_pos;
struct ball_col_st ball_col[BALL_COL_LIST_SIZE];
short ball_hits_pos;
long xpoints = 2000, ypoints = 1000;	// the Potters files' counts (COMMON.C reads them from the table's file)

/* game state */
long General_Rules[50];
playersstruct players[MAXPLAYERS];
struct player_stats player_info[MAX_PLAYERS*2];
OBJECT *ball_ptr[22];
long activated_lines, calculating_computer_shot, potted_flag, break_flag;
int potted_ball[32];
long pot_count, saved_ball_count, nine_ball_count;
char pocket_num;
long show_shot_flag, show_panel;
flag SNOOKER_MODE, SNOOKER_SHOOT_MODE;
long speed_pot, start_time;
int num_frames_camera_still;
long nms_pockets[6];
long potted_good_ball_flag, potted_hi_ball_flag, potted_red_wrong_flag;
long potted_black_wrong_flag, potted_yellow_wrong_flag;
long p1_stored_pot_type, p2_stored_pot_type;
long Player_Active, cushion_hit, white_hit_cushion, Two_Shots;
flag used_free_ball;
long LOWEST_BALL_HIT_FIRST_FLAG, draw_twice;
flag lowest_ball_not_hit_first;
long hit_red_ball_flag, missed_all_balls_flag, ball_hits_count;
flag end_of_frame_flag;
long butt, x_hit_pos, y_hit_pos, new_count, camera_in_pocket;

static OBJECT ball_objects[22];

void stubs_init(void)
{
	int i;
	for (i = 0; i < 22; i++) ball_ptr[i] = &ball_objects[i];
}

int add_sample(int number, int pan) { return 0; }
int add_sample_with_volume(int ballnum, int number, int pan, int vol) { return 0; }
void Add_Score(long score) {}
void Add_Score1(long score) {}
void Add_Score_2_other_player(long score) {}
void check_hits8ball(long ball_num) {}
void check_hits_snooker(long ball_num) {}
void check_hitsuk(long ball_num) {}
void check_hitsus(long ball_num) {}
void DrawScreen(void) {}
void get_new_screen_pos(long loop) {}
void setup_camera_in_pocket(long pocket) {}

/* the table's .csh file is linked in (gen/csh_*.c) */
ubyte *LoadFile(text *name) { return (ubyte *)csh_data; }
void Free_Ram_Ptr(ubyte *addr) {}
