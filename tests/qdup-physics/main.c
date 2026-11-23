/*
 * QD Up fixed-point physics test: load one recorded shot's start state (the
 * balls just after the stroke in the 1996 build), run FIXBALL.C's
 * update_all_balls once per game frame until the balls stop and print every
 * ball's position per call, with the call's cost in CPU cycles.
 *
 * Output lines (tty):
 *   SHOT <name>
 *   INIT <ball> <diameter> <mass> <expected diameter> <expected mass>   (mismatches over 1 unit)
 *   HIT <vx> <vy> <avx> <avy> <avz> <oracle vx> <oracle vy>   hit_ball check, 8.24 / 16.16
 *   C<n> <cycles> <ball>:<x>,<y>,<vx>,<vy>,<moving>,<active> ...   after call n (C0 = start)
 *   END calls=<n> cycles=<total> max=<max>
 *   DONE
 */
#include "fixball.h"
#include "shot.h"
#include "tty.h"

void stubs_init(void);

#define MAX_CALLS 3000

/* Cycle count of one call: root counter 2 at system clock / 8 (fine, wraps
 * every 524288 cycles) and root counter 1 counting hblanks (coarse, about
 * 2170 cycles each) to say how many times counter 2 wrapped. */
#define T1_COUNT (*(volatile unsigned long *)0x1F801110)
#define T1_MODE  (*(volatile unsigned long *)0x1F801114)
#define T2_COUNT (*(volatile unsigned long *)0x1F801120)
#define T2_MODE  (*(volatile unsigned long *)0x1F801124)
#define CYCLES_PER_HBLANK 2170

static void timer_start(void)
{
	T1_MODE = 0x0100;		/* hblank clock, free run; writing resets the count */
	T2_MODE = 0x0200;		/* sysclk/8, free run */
}

static unsigned long timer_read(void)
{
	unsigned long fine = (T2_COUNT & 0xffff) * 8;
	unsigned long coarse = (T1_COUNT & 0xffff) * CYCLES_PER_HBLANK;
	unsigned long wraps = 0;
	while (fine + (wraps + 1) * 524288 <= coarse + 262144) wraps++;
	return fine + wraps * 524288;
}

static void print_state(int call, unsigned long cycles)
{
	int i;
	tty_printf("C%d %u", call, cycles);
	for (i = 0; i < LAST_BALL; i++)
		tty_printf(" %d:%d,%d,%d,%d,%d,%d", i, ball[i].x, ball[i].y, ball[i].vx, ball[i].vy,
			ball[i].moving, ball[i].active);
	tty_puts("\n");
}

int main(void)
{
	int i, call;
	unsigned long t, total = 0, max = 0;
	const struct shot_ball *s;

	stubs_init();
	tty_printf("SHOT %s\n", shot.name);

	LAST_BALL = shot.last_ball;
	ball_mass = shot.ball_mass;
	TABLESIZE_TYPE = shot.tablesize_type;
	LOCATION = shot.location;
	GAME_TYPE = shot.game_type;
	SNOOKER_MODE = shot.snooker_mode;
	butt = shot.butt;
	x_hit_pos = shot.x_hit;
	y_hit_pos = shot.y_hit;

	if (load_cushions("tables\\potters\\ustable.csh") < 0) tty_puts("load_cushions failed\n");

	for (i = 0; i < LAST_BALL; i++) {
		ballstruct *b = &ball[i];
		s = &shot.balls[i];
		init_ball_position(i);
		if (b->diameter != s->diameter || b->mass - s->mass > 1 || s->mass - b->mass > 1)
			tty_printf("INIT %d %d %d %d %d\n", i, b->diameter, b->mass, s->diameter, s->mass);
		b->graphic_type = s->longs[0]; b->obj_type = s->longs[1]; b->ball_colour = s->longs[2];
		b->foul_flag = s->longs[3]; b->hitcush = s->longs[4]; b->numstruct = s->longs[5];
		b->respot_flag = s->longs[6]; b->num = s->longs[7]; b->pocket_ball_went_down = s->longs[8];
		b->x = s->x; b->y = s->y; b->z = s->z;
		b->x_prev = s->x_prev; b->y_prev = s->y_prev; b->z_prev = s->z_prev;
		b->moving = s->moving; b->active = s->active;
	}

	/* hit_ball on the cue ball, from the stroke's inputs */
	hit_ball(WHITE_BALL, shot.x_hit, shot.y_hit, shot.cue_angle, shot.butt, shot.speed);
	tty_printf("HIT %d %d %d %d %d %d %d\n", ball[0].vx, ball[0].vy, ball[0].avx, ball[0].avy,
		ball[0].avz, shot.balls[0].vx, shot.balls[0].vy);

	/* then run from the 1996 build's own velocities */
	for (i = 0; i < LAST_BALL; i++) {
		s = &shot.balls[i];
		ball[i].vx = s->vx; ball[i].vy = s->vy; ball[i].vz = s->vz;
		ball[i].avx = s->avx; ball[i].avy = s->avy; ball[i].avz = s->avz;
	}

	print_state(0, 0);
	for (call = 1; call <= MAX_CALLS && any_balls_moving(); call++) {
		timer_start();
		update_all_balls(shot.time_step);
		t = timer_read();
		total += t;
		if (t > max) max = t;
		print_state(call, t);
	}
	tty_printf("END calls=%d cycles=%u max=%u\n", call - 1, total, max);
	tty_puts("DONE\n");
	return 0;
}
