/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/*									    	 QD-UP      										 */
/*											PC-CD ROM											 */
/*									 For BMG	Interactive										 */
/*									 By NMS Software Ltd.									 */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

//g#include	"types.h"

typedef struct {
  text *file;
} txtstructptr ;

typedef struct {
  char *string;
} stringstructptr ;

typedef	struct	{
			long	key;
			text	chr;
			}	fastscanstruct;

#pragma aux mode13 = 	\
		"mov	ax,13h",\
		"int	10h"	\
modify [ax];
void		mode13(void);

#pragma aux set_debug = 	\
		"int	10h" ,	\
		"int	3h"	\
modify [ax];
void		set_debug(void);


//gtypedef double matrix4x4[4][4];
//gtypedef double matrix4x1[4];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From FLICS.C */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void initialise_flics(void );
void play_flic(char *fname,long res);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From POOL.C */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

//gvoid	main( byte argc, text* argv[] );
void	replace_wrong_slash_from_ini_root_string( text *source, text *dest );
void setvgamode( long m );
word collision_test( word xpos, word ypos, word xpos1, word ypos1, word xsize,
						word ysize,word xsize1,word ysize1);
flag	read_pool_ini( void );

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From STV3D.C */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void CollideCueWithBalls(void);
void GetBallPtrs(void);
void GetBumperPtrs(void);
void Load( char *filename, void *address, int length);
void setup_camera_in_pocket(long pocket);
void turn_off_pocket_cameras(void);
void DoCamera(void);
void PutCamera(VERTEX look_at);
flag zoom_out(int ball_num);
void ReplaceExtension(char *filename, char *extension);
void LoadTextures( text *path);
void LoadBallTextures( text *path);
void PositionCue(void);
void PositionCue1(void);
void init_3d(void);
void setup_resolution(void);
char VerticesSame(VERTEX *v1, VERTEX *v2);
flag test_camera_still(void);
void draw_3d(void);
void RenderBasicScene(int	toscr);
void ResetObjectRotation(OBJECT *obj);
void LocalRotateObject(int x_rotate, int y_rotate, int z_rotate, OBJECT *obj);
void UpdateBallPositions(void);
void GetBumperExtents(void);
void CalcBumperVisibility(void);
void turn_on_diamonds(long loop);
void turn_on_far_bumpers(void);
void turn_on_near_bumpers(void);
void DrawAllBalls(int toscr);
void Fast_Haaaa_DrawAllBalls(int toscr);
void MoveBall_new(void);
void Move_Camera_Position( long camera_status);
void check_all_camera_positions(void);
void Draw_Line_cross( char *backscreen , char col , OBJECT *dummy, OBJECT *dummy1, int xoffset,int yoffset);
void Draw_Line( char *backscreen , char col,OBJECT *dummy,OBJECT *dummy1);
void Draw_Head_String( char *backscreen );
void get_new_screen_pos(long loop);
void draw_tracking_lines( char *buffer);
void build_tracking_line_points(void);
void setup_ai_camera(void);
void setup_detail_level_of_room(void);
void setup_continuous_pocket(void);
void setup_nms_pocket_graphics(void);
void normal_rooms(void);
void moon_room(void);
void beach_room(void);
void toy_room(void);
void coffin_room(void);
void Select_Pocket_And_Ball(void);
void load_3d_textures(void);
void reset_tracking_lines(void);
void setup_default_camera_position( int ease_speed_value, flag point_at_cue_ball);
void setup_draw_3d_cue(void);
void draw_3d_cue(void);
void Move_Camera_View_Point(void);
void Draw_Camera_View_Point( char *buffer );
void check_change_resolution(void);
void LoadTextures2( text *path);
void init_3d_for_tournament(void);
void draw_3d_for_tournament(void);
void load_all_game_graphics_from_disk(void);
int  E_FinishedEase(void);
void setup_vesa(void);
void setup_end_of_frame_camera(void);
void check_camera_limets(void);


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From MY3DS.c */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void Re_Draw_Screen( void );
void setup_felt_palette(void);
void Handle3D(void);
void Render(void);
void Render1(void);
void show_render(void);
void DrawScreen2D(void);		// RESTORE: GFX.C, the 3D view
void display_screen(void);
void Raise_Butt_Of_Cue(void);
void identity4x4(matrix4x4 matx);
void multiply4x4(matrix4x4 matx1, matrix4x4 matx2, matrix4x4 result);
void multiply4x1(matrix4x4 matx1, matrix4x1 matx2, matrix4x1 result);
void rotate(double roll, double pitch, double yaw, matrix4x4 spin);
void SetupPanel(void);
void Low_Res_Panel(void);
void Hi_Res_Panel(void);
void make_shade(ubyte	*tab,long shad);
void check_all_game_events(void);
void setup_table_varibles(void);
void background_samples(void);
void scan_for_cheat_modes(void);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From CD.c */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
void loop_cd_music_track( long whichtrack );
int  Length_CDTrack(long track);
void Stop_CDTrack(void);
long init_CD(void);
int  Play_CDTrack(long track);
void set_CD_volume(long);
#pragma	aux	set_CD_volume parm caller [eax];
long return_1st_CDdrive(void);
void read_VTOC(char *);
flag initialise_the_cd_playback_system( void );
flag decode_if_yes_or_no( text chr );
flag read_pool_ini( void );
flag read_hmi_ini( void );
int  cd_pos( void );
void start_new_track( long track );

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From GAME.c	*/
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void check_all_fast_keys(void);
void Run_Game(void);
void Play_Single_Game(void);
void play_one_frame(void);
void end_of_frame_control(void);
flag set_target_for_white( void );
char control_demo(void);
void Update_Cue_Position( void );
flag check_hit_cue_ball(void);
flag Show_Shot(void);
void Show_Undo_Shot(void);
void Show_Replay(void);
void tracking_lines_loop(void);
void players_shot(void);
double check_ball_hypot(long x, long y);
void Update_English_On_Cue_Ball(void);
void reset_aiming_varibles(void);
void reset_aiming_varibles1(void);
void start_game_track(void);
void set_target_for_white_TRICK( void );
flag Show_Shot_Trick(void);
void players_shot_TRICK(void);
void play_trickshot(void);
void Save_Trick_Shot(void);
void setup_saved_trickshot_shot(void);
void show_andplay_trickshot_loop(void);
void setup_replay_saved_shot(void);
void quit_frame_control(void);
void Re_Rack_Balls_with_other_gametype(void);
void Re_Rack_Balls(void);
void pause_game(void);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From GAMETYPEA.c */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void Setup_New_Game_Type(void);
void setup_balls( void );
void setup_uk_pool(void);
void setup_us_pool(void);
void setup_us_8ball(void);
void setup_snooker(void);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From BALL.c */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

// RESTORE: the game uses FIXBALL.C (fixed point); its prototypes are in
// FIXBALL.H.  BALL.C's, with doubles, are left out:
//   add_col_to_list ... check_solid_pocket
long get_angle( float xvel, float yvel);
long get_angle1( long xvel, long yvel);
int init_cushions(char filename[]);

// RESTORE: FILE_IO.C's reads for Escher's loader and TYPES.H's file macros
long FileOpen(text *name, long mode, long attr);
long FileRead(long handle, ubyte *dest, long length);
long FileClose(long handle);



/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From MENU.c */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

#define	SOUND_FXS_FLAG	GameOptionsValues[1]
ext		long	GameOptionsValues[20];

void Setup_Default_Option_Values(void);
void remap_gfx_block(long	gfx_num,ubyte	*pal, ubyte	*source);
void remap_1block(long	gfx_num,ubyte	*pal, ubyte	*source);
void remap_raw_block(long	gfx_num,ubyte	*pal, ubyte	*source, long width, long height);
int  draw_string(int	start_x,int	start_y,char	*text);
int  draw_string1(int	start_x,int	start_y,char	*text);
int  draw_string2(int	start_x,int	start_y,char	*text);
void test_line_list(char *source);
void draw_title_screen(void);
void Show_Loading(void);	// RESTORE: PLEASE WAIT while the game loads
void Reset_Memory_Ptr(void);
ubyte*  Get_Ram_Ptr( int length );
void In_Game_Ram_Init(void);
void Wrapper_Ram_Init(void);
void Main_Options_Controls(void);
void exit_game(void);
void Wrapper(void);
void control_all_rooms(void);
void draw_gfx_block1(long	gfx_num,long	dest, ubyte	*source, int masked_flag, int name_index, int direction);
void draw_gfx_block(long	gfx_num,long	dest, ubyte	*source, int masked_flag, int name_index, int direction);
void draw_masked_type(long	width,long height,long	dest, ubyte	*source, long masked_flag,long name_index, long direction);
void draw_masked_type1(long	width,long height,long	dest, ubyte	*source, long masked_flag,long name_index, long direction);
void Projector_Options(void);
void projector_room(void);
void cd_player_control_in_projector_room(void);
void draw_masked_type1(long	width,long height,long	dest, ubyte	*source, long masked_flag,long name_index, long direction);
void Computer_Room(void);
void Computer_Options(void);
void Save_shot_Room(void);
void saveshot_Options(void);
void draw_saved_names(void);
void check_enter(void);
void change_savedname(long	loop);
void Credits_room(void);
void Credits_room1(void);
void Credits_options(void);
void Credits_options1(void);
void Load_Shot_Room(void);
flag loadshot_Options(void);
long check_slot_selected(void);
char * add_key( char string[], char key);
void file_name(void);
void Player_Select_room(void);
void player_select(void);
void draw_balls_icons(void);
void draw_players_in_frames(void);
void control_players(void);
void enter_name_loop(void);
void enter_name(void);
void hot_keys_tourn(void);
void hot_keys(void);
void hot_keys_loadsave(void);
void draw_cue_pointer(void);
void save_mouse_background(void);
void draw_cue_pointer1(void);
void draw_cue_pointer3(void);
void draw_cue_pointer2(void);
void save_cue_pointer(void);
void setup_constant_panel(void);
void draw_connection_in_wrapper(void);
int  get_highest(void );
void check_all_background_collision(void);
void Show_Stats(long	whichplayer);
void setup_stats(long whichplayer);
void stretch_box(void);
void draw_faded_screen(void);
void trick_shot_room(void);
void setup_trick_names(int off);
int  draw_trick_text(int	start_x,int	start_y,char	*text);
void make_trick_tab1(void);
void trick_shot_options(void);
void hi_score_room(void);
void hi_score_options(void);
void check_hi_score_entery(int	player);
void fix_default_hi_score_table_names_with_a_null(void);
void setup_hi_score(void);
int  hi_font(int	start_x,int	start_y,char	*text, int az,int azdot,int azsize,int num,int numdot);
void hi_time_room(void);
void hi_time_options(void);
void check_time_entery(int	player);
void fix_default_fastest_time_table_names_with_a_null(void);
void setup_fastest_time(void);
void book_of_game_rules(void);
void show_book(void);
void book_control(void);
void setup_stick(void);
void update_keycontrols(void);
void setup_contrast(void);
void draw_contrast_scale(void);
void setup_mouse_speed(void);
void draw_speed_scale(void);
void	end_of_frame_screen(void);
int  hi_endfont(int	start_x,int	start_y,char	*text, int az,int azdot,int azsize,int az1,int azdot1);
void end_of_game_control(void);
void end_game_contol(void);
char*	shorten_name(long	name_name_index);
void update_total_scores(void);
void end_of_killer_screen(void);
void fade_to_white(void);
void endkiller_control(void);
void draw_killer_shadow(long xpos, long ypos);
void make_fade_pals(void);
void end_of_tournament(void);
void end_of_tournament_control(void);
void draw_end_tournament(void);
void draw_end_tournament1(void);
void save_hi_score_table_to_disk(void);
void load_hi_score_table_from_disk(void);
void setup_controls(void);
void show_game_controls(void);
void control_options(void);
void setup_volume(void);
void draw_volume_scale(void);
int  Connect_setup(void);
void re_draw_computer_room(void);
flag connect_modem(void);
void control_phone(void);
void control_phone_status(long	status);
void control_phone_receive(void);
int  send_packet( char *data_ptr, ubyte	len,ubyte	type);
void recv_packet( void);
void serial_transfer(void);
void send_master_counter(void);
void send_which_col(void);
void send_cue_position(void);
void send_button_stats(void);
void sync_loop(void);
void quit_send_loop(void);
void quitgame_send_loop(void);
void send_general_options(void);
void send_cue_ball_position(void);
void load_all_old_data_types(void);
void send_external_events(void);
void dissplay_cd_player(void);
void cdplayer_control(void);
void cd_collision_tests(void);
void atract_counter(void);
void reset_atract_counter(void);
void atract_mode(void);
void sync_stage(void);
void build_reorded_line_list(void);
void draw_players_names(void);
void scratch_player_screen(void);
void show_scratch(void);
void show_scratch_loop(void);
void telly_room(void);
void telly_control(void);
int get_highest(void );
void killer_players_screen(void);
flag draw_killer_players(void);
void killer_control(void);
void make_new_killer_order(void);
void save_options_to_disk(void);
void load_options_from_disk(void);
void save_player_stats_to_disk(void);
void load_player_stats_from_disk(void);
void save_player_names_to_disk(void);
void load_player_names_from_disk(void);
void Title_rooom(void);
void show_saved_shot_loop(void);
void scan_for_cheat_modes_for_front_end(void);
void flash_border(void);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From RULES.c */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

flag update_fouls(void);
void us_8ball_foul(void);
void nms_pool_foul(void);
void uk_pool_foul(void);
void us_pool_foul(void);
void speed_pool_foul(void);
void white_ball_gone(void);
void continuous_foul(void);
void check_hits8ball( long ball_num );
void check_hits_snooker( long ball_num );
void check_hitsuk( long ball_num );
void check_hitsus( long ball_num );
void save_replay(void);
void setup_replay(void);
void killer_pool_foul(void);
void Killer_Pool_(void);
void white_ball_gone_float(void);
void check_all_fouls(void);
void respot_ball(void);
void reset_fouls(void);
void Swap_Player(void);
void check_ball_in_safe_place( long ballnum );
void setup_flash_name(void);


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From SCRDRWS.c */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void draw_640x480_screen(  char *source );
void draw_640x480_screen1(  char *source );
void screen_dump(int start_x,int start_y,char *source,int width,int height);
void draw_panel_bits(void);
void snooker_panel(void);
void continuous_panel(void);
void draw_snooker_balls(void);
void draw_trick_shot_name(void);
void draw_player_turn_arror(void);
void us_pool_panel(void);
void uk_pool_panel(void);
void nms_pool_panel(void);
void killer_pool_panel(void);
void draw_killer_lives(void);
void draw_player_names_killer(void);
int  hi_speed_font(int	start_x,int	start_y,char	*text, int az,int azdot,int azsize,int az1,int azdot1);
void speed_pool_panel(void);
void nine_ball_panel(void);
void update_timer(void);
void Draw_BigTimer(void);
void draw_9_balls(void);
void HI_CLS(void);
void draw_scores_snooker(void);
void draw_score(long player);
void draw_number(long	num,long	xpos,long y);
void Setup_Scores(void);
void Update_Scores(	void);
void Add_Score_2_other_player( long score);
void Add_Score( long score);
void Add_Score1( long score);
void draw_player_names(void);
void draw_quit_frame(void);
void draw_end_of_frame(void);
void draw_player_names_speed(void);
void draw_text(int	start_x,int	start_y,char	*text, long play_pen);
void len_text(int	start_x,int	start_y,char	*text, long play_pen);
void draw_frames_won(long player);
void draw_turbo_icon(void);
void clear_replay_panel(void);
void draw_ball_type_selecter(void);
void choose_loop(void);
void draw_number_of_shots_icons(void);
void draw_connection(void);
void build_low_text(long play_pen);
void draw_text_low(int	start_x,int	start_y,char	*text, long play_pen);
void draw_number_low(long	num,long	xpos,long y);
void draw_text_len(int	start_x,int	start_y,char	*text, long play_pen);
void draw_amplify(void);
void draw_text_foul(int	start_x,int	start_y,char	*text, long play_pen);
void build_low_text_foul(void);
void draw_text_low_foul(int	start_x,int	start_y,char	*text, long play_pen);
void draw_last_foul(void);


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From TOURN.c */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void tournament_Room(void);
void setup_who_plays_who(void);
void setup_player_1_names(void);
void setup_player_2_names(void);
void find_winners(long number_of_winners , long read_position , long start_position );
flag check_who_won(void);
void setup_tour_names(void);
char*	make_inital(long	player);
void tournament_Options(void);
void flash_tour_names(void);
void show_tournament_round(void);
void Show_round_Control(void);
void draw_shield(long shield_frame);
flag loadgame_Options(void);
flag loadgame_Options(void);
void Load_game_Room(void);
void Save_game_Room(void);
void savegame_Options(void);
void make_tournament_table(void);
void setup_frames_won_table(void);
void Won_Tournament_Room(void);
void update_torn(void);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From AI.c */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
void computer_shot(void);
void get_best_shot(void);
void get_best_shot_for_straight_pot(void);
int  find_best_pocket(void);
flag take_shot(int angle);
double Return_angle_to_closest_pocket(long check_ball);
long Check_best_Pocket(long check_ball);
long Check_best_double_Pocket(long check_ball);
long Get_good_angle_to_Object_ball_1_(void);
long Check_Around_object_ball_1_(void);
long Get_Clear_Path_To_Pocket(long pocket);
long Look_line_to_Object_ball_2(void);
void add_angle_for_next_shot(long	angle_to_add);
void add_angle_for_next_shot_checked(long	angle_to_add);
void reset_shot(void);
long get_ball_angle( double xvel, double yvel);
long Select_Ball_To_Pot(void);
long Aim_Cue_To_Current_Ball(void);
void Save_ball_data(void);
void Reset_3dball(void);
void take_shot2(void);
void move_cue_ball_to_good_position(void);
void Aim_At_Next_Ball(void);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From VESA.C */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void 	set_x320y240(void );
void	dump_x320y240(char *source);
#pragma aux dump_x320y240 parm caller [esi];

long	asm_time_VESA_set_display_start(long time );
#pragma aux asm_time_VESA_set_display_start parm caller [eax];
long	asm_time_VRT(void );

flag  VESA_is_mode_available(long mode );
flag	VESA_init(void );
void 	VESA_close(void);
flag	VESA_init_mode( long mode );
flag	VESA_set_mode( long mode );
void	VESA_set_band( long mode );
void 	VESA_set_display_start(long x,long y,flag waitVRT);
flag  VESA_set_logical_scan_line_length(long length);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From SVGA.C */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag  SVGA_init(char *path );
void  SVGA_uninit(void );
flag  SVGA_set_mode(long mode);
void	SVGA_set_display_start(long x,long y,flag waitVRT);
void	set_pen(long pen,char r,char g,char b);
void	set_palette(char *source,long index,long length);
void	wait_vbi(void );
void	SVGA_clear_line_list(void);
void	SVGA_blit_line_list(void);

void  asm_init_lines(void);

long	asm_ask_bank(long bank);
#pragma aux asm_ask_bank parm caller [ebx];

void	asm_blit_bank(long bank);
#pragma aux asm_blit_bank_ parm caller [eax];

void  asm_add_bg_line(char *source,long dest,long width);
#pragma aux asm_add_bg_line parm caller [esi] [edi] [ecx] \
		modify exact [];

void  asm_add_dump_line(char *source,long dest,long width);
#pragma aux asm_add_dump_line parm caller [esi] [edi] [ecx] \
		modify exact [];

void  asm_add_andor_line(char *source,long dest,long width);
#pragma aux asm_add_andor_line parm caller [esi] [edi] [ecx] \
		modify exact [];

void  asm_add_set_line(long dest,long width,long c);

#pragma aux asm_add_set_line parm caller [edi] [ecx] [eax] \
		modify exact [];

void screen_dump_line(char *source,long dest,long width);
void screen_dump(int start_x,int start_y,char *source,int width,int height);
void screen_dump1(int start_x,int start_y,char *source,int width,int height);
void screen_dump_line(char *source,long dest,long width);
void draw_gfx_block(long	gfx_num,long	dest, ubyte	*source, int masked_flag, int index, int direction);
void screen_set_line(long dest,long width,ubyte colour);
void screen_copy_line(char *source,long dest,long width);
void screen_copy_line1(ubyte *destbuff,long dest,long width);
void screen_dump_line1(char *source,long dest,long width);
void screen_dump_line2(char *source,long dest,long width);
void screen_andor_line(char *source,int dest,int width);
void screen_andor_line1(char *source,int dest,int width);
void screen_clear_line(long dest,long width);
void screen_set_line(long dest,long width,ubyte colour);
void screen_set_line1(long dest,long width,ubyte colour);
void screen_remap_line2(char *source,int dest,int width);
void screen2screen_remap_line(char *source,int dest,int width);
void screen_remap_line(char *source,char *source1,int dest1,int dest,int width);
void screen_remap_line1(char *source,char *source1,int dest1,int dest,int width);
void check_abort_game(void);
//gvoid wait_for_VBE_vblank(int	y);
//gvoid wait_for_VBE_vblank1(int	y);
//gvoid setup_SVGA_screen(void);
void	_asm_set_vesagran( long i );
#pragma aux _asm_set_vesagran parm caller [eax];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From SOUNDS.c */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

int init_sound(void);
int load_samples(text *string);
void	clear_loaded_samples(void);
void play_test_sample(void);
void clear_sound(void);
void stop_all_samples(void);
void clear_sample_triggers(void);
int init_samples(char filename[]);
int add_sample(int number, int pan);
int add_sample_with_volume(int ballnum,int number, int pan,int vol);
int calculate_vol(int num);
int calculate_vol_pos(int x, int y);
int find_free_sample_handle(int type, int priority);
void process_samples(void);
flag query_sample(long channel);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From COLOURS.c */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

//gvoid fix_palette_to_black(void);
void setpallette ( short *p, long n );
void bgcol ( ubyte red, ubyte green, ubyte blue );
void setcol ( long which_colour, ubyte red, ubyte green, ubyte blue );
void setcol1  ( long which_colour, ubyte red, ubyte green, ubyte blue );
//gvoid wait_for_vblank(void );
void at( long r, long c );
void fadein(void);
void fadeout(void);
void setvgamode(long m);
void conout(void);
void conin(void);
long find_a_close_colour( byte hr, byte hg, byte hb, byte *colpal, long start, long end );

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From READIFF.C */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

ubyte* readlbm( text *filename, flag tempmem );
void lbmerror( text *message, text *con, FILE *fp );
void show2digits( long value );
void show4digits( long value );
long read_reversed_long_from_disk( FILE *fp );
long read_reversed_word_from_disk( FILE *fp );
long and_off_top_8_if_negative	( long value );
long and_off_top_16_if_negative	( long value );

flag initsvga(long mode);
void setvesaband(long band);
void testvesa(void);
void plotpixel(int pos,char c);
char readpixel(int pos);
void test_lbm(char *buf);
char *decompress_lbm(signed char *source);
extern comp_screen_copy(unsigned char * comp_buf);
extern comp_screen_copy1(unsigned char * comp_buf);
extern copy_background(unsigned char * backgroundptr);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/*	From DISK.C */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

long what_length_is_this_file( text *fname );
void load_error( text *message, FILE *fp );
//gvoid close_the_read_file( FILE *fp );
void close_the_saved_file( FILE *fp );
long get_file_length( FILE *fp );
ubyte*	loadafile( text *fname );
flag load_file_here( text *fname, ubyte *dest, long length );
//gFILE*	open_file_to_read( text *fname );
FILE*	open_file_to_save( text *fname );
//gvoid read_block_from_disk( void *dest, long length, FILE *fp );
long read_byte_from_disk( FILE *fp );
long read_long_from_disk( FILE *fp );
long read_word_from_disk( FILE *fp );
flag saveafile ( text *fname, ubyte *source, long len );
//gvoid write_block_to_disk( void *buffer, long length, FILE *fp );
void write_byte_to_disk( long value, FILE *fp );
//gvoid write_long_to_disk( long value, FILE *fp );
void write_word_to_disk( long value, FILE *fp );
void disintercept( void );
void intercept( void );
long read_rev_word_from_disk( FILE *fp );
long read_rev_long_from_disk( FILE *fp );
//gbmhdheader*	read_lbm_bmhd_header( FILE *fp );
ubyte* loadafile1( text *fname );

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From KEYBOARD.C */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

//gtext find_which_key_input( void );
//gtext find_which_key_input1( void );
void wait_for_enter_key( void );
void wait_for_space_key( void );
void wait_for_this_key( long k );
void wait_for_any_key( void );
//gvoid scan_for_keysjp( void );
void intercept( void );
void disintercept( void );
void read_joy_xys(void);
unsigned short read_joy(void);
void return_button_press_data(void);
void select_joystick(void);
void check_joystick(void);
void return_position_and_button_status(void);
void flush_keys(void);
//gchar get_key(void);
void installkey(void);
void clearkey(void);
flag getkey(int key);
flag getkey_jp(int key);

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From MOUSE.C */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void read_mouse_buttons( void );
flag reset_mouse_driver( void);
void update_mouse_xy( void );
void update_mouse_xy1( void );


/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From TIMER.C */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

void pmode_timer(long user);
//gvoid __interrupt __far PHILStimer( void );
//gextern void cdecl set_PIT_period(int microseconds);
void	reset_timer					( void );
void	load_timer					( long );
void	TestZ							( void );
void	TestB							( void );
void	TimeFly						( void );
void	waitretrace					( void );
void	set_screen_start_address	( void );
void installtimer( void );
void cleartimer( void );

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From PAGESWAP.asm */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
#define	JOY_X	1
#define	JOY_Y	2
void check_joystick(void);
word readstick( byte	bmask);
#pragma aux readstick_ parm caller [eax];
word readjbutton( byte	bmask);
#pragma aux readjbutton_ parm caller [eax];
void _JStickInit(void);
void select_joystick(void);
void force_svga_set_address_on_vbl( void );
int save_iff(char *fname,char *source,char *pal,int width,int height);
#pragma aux save_iff parm caller [edx] [esi] [edi] [eax] [ebx];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From RAM.c */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

ubyte*	get_ram( long length );
flag		grab_available_ram_for_us( void );
void		free_the_ram_made_available_ram_to_us( void );
void		free_all_ram_again_for_us_to_reuse(void);
ubyte*	get_temporary_ram( long length );
flag		free_last_temp_ram_block( void* address );
ubyte*	load_file_into_temp_mem( text *fname );

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From TRICK.lst */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

ext stringstructptr trick_names[];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* From LEVELDATA.C */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */

ext txtstructptr mesh_table[];
ext txtstructptr room_palettes[];
ext txtstructptr poolpal_tab[];

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* FLICS..C */
/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
void flic_delay(long del);

