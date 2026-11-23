/* Start state of one recorded shot (generated into gen/shot_*.c). */
#ifndef QDUP_SHOT_H
#define QDUP_SHOT_H

struct shot_ball {
	long longs[9];		/* graphic_type .. pocket_ball_went_down */
	long x, y, z, x_prev, y_prev, z_prev, vx, vy, vz, diameter, mass;	/* 8.24 */
	long avx, avy, avz;	/* 16.16 */
	unsigned char moving, active;
};

struct shot_start {
	const char *name;
	long last_ball, ball_mass, tablesize_type, location, game_type, snooker_mode;
	long time_step;		/* 8.24 */
	long cue_angle, butt, speed, x_hit, y_hit;	/* hit_ball inputs, speed 8.24 */
	struct shot_ball balls[16];
};

extern const struct shot_start shot;
extern const long csh_data[];
extern const unsigned long csh_data_size;

#endif
