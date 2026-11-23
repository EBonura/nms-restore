/* RESTORE: NEW WORK (owner's decision): a faster shot search for QD Up's
 * computer player, for the PS1.  Not NMS's code.
 *
 * NMS's search (AI.C get_best_shot_for_straight_pot) tries up to 60 cue
 * angles, one degree apart on alternate sides of the ghost-ball angle
 * find_best_pocket gives, and plays each one out with the full physics
 * (take_shot) until a ball is potted, the white is potted or every ball
 * stops.  It takes the first angle that pots an object ball (the ghost-ball
 * angle when none does, or when the first is the 45th or later).  A trial
 * that pots nothing runs until every ball has stopped, and in four turns
 * out of ten none of the 60 pots, so a turn could take NMS's search 19 s.
 *
 * This search keeps NMS's candidates, its trial (take_shot's steps, trial
 * below, with the full physics) and its rules for the result; it changes
 * which candidates are played out, in what order and for how long:
 *  1. Each candidate is judged geometrically: the white's straight path
 *     (its direction read from hit_ball itself), the first ball it meets,
 *     the line that ball leaves along (through the centres at contact), and
 *     whether that line, or the line of the first ball it meets in turn (a
 *     combination), reaches a pocket mouth (pockets[].radius widened by
 *     QDUP_AI_TOLERANCE percent) with no other ball in the way.
 *  2. The judged candidates are played out in full, in NMS's order
 *     (QDUP_AI_ORDER 0; 1 keeps NMS's order for all of them).
 *  3. Then the others, in NMS's order, for at most QDUP_AI_CAP passes of
 *     take_shot's loop each (pots come early: half by pass 32; a trial that
 *     pots nothing runs about 145).
 *  4. The whole search stops once QDUP_AI_WORK units of physics are spent
 *     (balls moving times physics sub-steps; QDUP_AI_PASSES caps passes),
 *     or QDUP_AI_SUBSTEPS physics sub-steps however few balls move, or at
 *     once when one pass takes more than QDUP_AI_STORM sub-steps (a white
 *     placed overlapping another ball makes update_all_balls go back over
 *     its sub-steps again and again, some 690 a pass, where a pass at the
 *     break with the balls bunched takes over 100; such a position took
 *     677 VBlanks; on the corpus no choice changes with this limit).
 * The first candidate that pots is taken; none: NMS's fall-back.  So the
 * shot taken is one the full physics has played to a pot, or NMS's own
 * fall-back.  The work count is the same on every console, so the choice
 * is the same on all of them.  The settings below were picked on a corpus
 * of 619 computer turns (tests/qdup-ai): the same shot as NMS's search in
 * 88% of turns, the same outcome (pot or no pot) in 95%, the search taking
 * at most 110 VBlanks (2.2 s) against NMS's 954.
 *
 * QDUP_EXACT_AI at build time keeps NMS's search.  Test builds
 * (tests/qdup-ai): QDUP_AI_COMPARE runs both on every computer turn and
 * prints both results (QDUP_AI_LABEL also plays out every candidate),
 * QDUP_AI_DUEL gives each side of a computer-against-computer game one of
 * them.
 */
#include	"globals.h"

#ifndef	QDUP_AI_TOLERANCE
#define	QDUP_AI_TOLERANCE	150		// pocket mouth, percent of pockets[].radius
#endif
#ifndef	QDUP_AI_ORDER
#define	QDUP_AI_ORDER		0		// 0: judged candidates first; 1: NMS's order
#endif
#ifndef	QDUP_AI_CAP
#define	QDUP_AI_CAP		30		// passes a candidate not judged to pot is played for
#endif
#ifndef	QDUP_AI_PASSES
#define	QDUP_AI_PASSES		0x7fffffff	// passes for the whole search
#endif
#ifndef	QDUP_AI_WORK
#define	QDUP_AI_WORK		16000		// moving balls times physics sub-steps, for the whole search
#endif
#ifndef	QDUP_AI_SUBSTEPS
#define	QDUP_AI_SUBSTEPS	20000		// physics sub-steps, for the whole search
#endif
#ifndef	QDUP_AI_STORM
#define	QDUP_AI_STORM		400		// sub-steps in one pass that end the search
#endif

#define	NUM_CANDIDATES	60

extern	int	cue_move_count;		// AI.C

static	int	cand_angle[NUM_CANDIDATES];
static	long	cand_ghost;

// NMS's candidate angles, in its order (AI.C get_best_shot_for_straight_pot)
static void	make_candidates(void)
{
	int	angle_to_pocket_clockwise, angle_to_pocket_anticlockwise, angle_to_ball;
	int	k;

	angle_to_pocket_clockwise = find_best_pocket();
	angle_to_pocket_anticlockwise = angle_to_pocket_clockwise;
	angle_to_ball = angle_to_pocket_clockwise;
	cand_ghost = ((angle_to_ball * 4096) / 360)+1024;

	for(k=0; k<NUM_CANDIDATES; k++)
		{
		if(k%2 == 1)
			{
			cand_angle[k] = ((angle_to_pocket_clockwise * 4096) / 360)+1024;
			angle_to_pocket_clockwise += 1;
			angle_to_pocket_clockwise %= 360;
			}
		else
			{
			cand_angle[k] = ((angle_to_pocket_anticlockwise * 4096) / 360)+1024;
			if((angle_to_pocket_anticlockwise --) < 0) angle_to_pocket_anticlockwise += 360;
			}
		}
}

// (a * b) / c in 64 bits
static long	muldiv(long a, long b, long c)
{
	return((long)(((long long)a * b) / c));
}

// The first ball on the table (not in the exclude mask) whose centre lies
// within rad of the line from (sx, sy) along (dx, dy), of length len, ahead
// of the start: its number (-1 for none), distance along and across.
static int	first_on_line(long sx, long sy, long dx, long dy, long len, long exclude, long rad, long *along, long *across)
{
	int	i, best = -1;
	long	ex, ey, t, p, bt = 0x7fffffff, bp = 0;

	for(i=1; i<LAST_BALL; i++)
		{
		if((ball[i].active != YES) || (exclude & (1 << i))) continue;
		ex = ball[i].x - sx;
		ey = ball[i].y - sy;
		t = (long)((((long long)ex * dx) + ((long long)ey * dy)) / len);
		if(t <= 0) continue;
		p = (long)((((long long)ex * dy) - ((long long)ey * dx)) / len);
		if((p >= rad) || (p <= -rad)) continue;
		if(t < bt) { bt = t; bp = p; best = i; }
		}
	*along = bt;
	*across = bp;
	return(best);
}

// Ball b leaving along (ox, oy), of length len: does it reach a pocket's
// mouth (pockets[].radius widened by QDUP_AI_TOLERANCE percent) with no
// other ball (outside the exclude mask) on the way?
static flag	reaches_pocket(int b, long ox, long oy, long len, long exclude)
{
	int	k, i;
	long	ex, ey, s, q, mouth, fx, fy, a, c, r2;

	for(k=0; k<NUM_POCKETS; k++)
		{
		ex = pockets[k].x - ball[b].x;
		ey = pockets[k].y - ball[b].y;
		s = (long)((((long long)ex * ox) + ((long long)ey * oy)) / len);
		if(s <= 0) continue;
		q = (long)((((long long)ex * oy) - ((long long)ey * ox)) / len);
		mouth = muldiv(pockets[k].radius, QDUP_AI_TOLERANCE, 100);
		if((q >= mouth) || (q <= -mouth)) continue;
		for(i=1; i<LAST_BALL; i++)
			{
			if((i == b) || (ball[i].active != YES) || (exclude & (1 << i))) continue;
			fx = ball[i].x - ball[b].x;
			fy = ball[i].y - ball[b].y;
			a = (long)((((long long)fx * ox) + ((long long)fy * oy)) / len);
			if((a <= 0) || (a >= s)) continue;
			c = (long)((((long long)fx * oy) - ((long long)fy * ox)) / len);
			r2 = ball[b].radius + ball[i].radius;
			if((c < r2) && (c > -r2)) break;
			}
		if(i == LAST_BALL) return(YES);
		}
	return(NO);
}

// would the shot at this cue angle pot an object ball, straight off the
// white or as a combination (the struck ball sending on the first ball in
// its way, through the centres at contact)?
static flag	judge(int angle)
{
	long	vx, vy, v, wx, wy, reach, back, t, p, cx, cy, ox, oy, t2, p2, d2;
	int	h, x;
	ballstruct	*w = &ball[WHITE_BALL];

	// the white's direction, from hit_ball itself
	hit_ball(WHITE_BALL, x_hit_pos, y_hit_pos, angle, butt, DOUBLE_TO_FIX24(speed_meter));
	vx = w->vx;
	vy = w->vy;
	reset_shot();
	v = fix_sqrt(mul64(vx,vx) + mul64(vy,vy));
	if(v <= 0) return(NO);
	wx = w->x;
	wy = w->y;

	h = first_on_line(wx, wy, vx, vy, v, 1, w->radius + ball[1].radius, &t, &p);
	if(h < 0) return(NO);
	reach = w->radius + ball[h].radius;
	back = fix_sqrt(mul64(reach,reach) - mul64(p,p));
	cx = wx + muldiv(vx, t - back, v);	// the white's centre at contact
	cy = wy + muldiv(vy, t - back, v);
	ox = ball[h].x - cx;			// the struck ball's line, length reach
	oy = ball[h].y - cy;
	if(reaches_pocket(h, ox, oy, reach, 0)) return(YES);

	d2 = 2 * ball[h].radius;
	x = first_on_line(ball[h].x, ball[h].y, ox, oy, reach, (1 << h) | 1, d2, &t2, &p2);
	if(x < 0) return(NO);
	back = fix_sqrt(mul64(d2,d2) - mul64(p2,p2));
	cx = ball[h].x + muldiv(ox, t2 - back, reach);
	cy = ball[h].y + muldiv(oy, t2 - back, reach);
	return(reaches_pocket(x, ball[x].x - cx, ball[x].y - cy, d2, 1 << h));
}

// NMS's take_shot (AI.C), step for step, with a limit: after cap passes of
// its loop (two physics calls each, 1/25 s of play) the trial counts as
// not potting.  *passes gets the passes made.  With no limit it gives
// take_shot's answer.
long	ai_work;			// moving balls times physics sub-steps, over the passes played
extern	long	fix_substeps;		// FIXBALL.C
static	long	ai_substeps0;		// fix_substeps when the search began
static	flag	ai_storm;		// a pass took more than QDUP_AI_STORM sub-steps

static flag	trial(int angle, int cap, int *passes)
{
	int	n = 0, i, m;
	long	s0;

	LOWEST_BALL_HIT_FIRST_FLAG=NO;
	first_hit = NO;
	speed_meter=fabs(cue_delta_table[0]);
	hit_ball(WHITE_BALL, x_hit_pos, y_hit_pos, angle, butt, DOUBLE_TO_FIX24(speed_meter));
	while((any_balls_moving()==TRUE))
		{
		n++;
		s0 = fix_substeps;
		update_all_balls(FIX24_ONE/frame_rate);
		update_all_balls(FIX24_ONE/frame_rate);
		if(ball[WHITE_BALL].active==NO) { *passes = n; return FALSE; }
		if(potted_hi_ball_flag != 0) { *passes = n; return TRUE; }
		if(potted_good_ball_flag != 0) { *passes = n; return TRUE; }
		for(i=0, m=0; i<LAST_BALL; i++)
			if(ball[i].moving && ball[i].active) m++;
		ai_work += m * (fix_substeps - s0);
		if(n >= cap) break;
		if(ai_work >= QDUP_AI_WORK) break;
		if(fix_substeps - ai_substeps0 >= QDUP_AI_SUBSTEPS) break;
		if(fix_substeps - s0 > QDUP_AI_STORM) { ai_storm = YES; break; }
		}
	*passes = n;
	return FALSE;
}

int	ai_judged;			// candidates judged to pot (for the test logs)
int	ai_trials;			// trials played out
int	ai_passes;			// their passes

void	get_best_shot_fast(void)
{
	long	l;
	int	k, chosen;
	static	flag	judged[NUM_CANDIDATES];
	int	pass, n, limit, used;

	save_nine_ball_count=nine_ball_count;
	cue_delta_table[0] = (60 * (cue_weight*1.5));
	for(l=0; l<LAST_BALL; l++)
		saveball[l] = ball[l];
	cue_move_count=0;
	speed_meter=fabs(cue_delta_table[0]);

	make_candidates();

	ai_judged = 0;
	for(k=0; k<NUM_CANDIDATES; k++)
		{
		judged[k] = judge(cand_angle[k]);
		ai_judged += judged[k];
		}

	chosen = -1;
	ai_trials = 0;
	used = 0;
	ai_work = 0;
	ai_substeps0 = fix_substeps;
	ai_storm = NO;
	for(pass=0; (pass < 2) && (chosen < 0) && (used < QDUP_AI_PASSES) && (ai_work < QDUP_AI_WORK) && (fix_substeps - ai_substeps0 < QDUP_AI_SUBSTEPS) && !ai_storm; pass++)
		{
		for(k=0; (k<NUM_CANDIDATES) && (chosen < 0) && (used < QDUP_AI_PASSES) && (ai_work < QDUP_AI_WORK) && (fix_substeps - ai_substeps0 < QDUP_AI_SUBSTEPS) && !ai_storm; k++)
			{
			if(QDUP_AI_ORDER)
				{
				if(pass == 1) break;			// one sweep in NMS's order
				}
			else if(judged[k] != (pass == 0)) continue;	// judged ones, then the rest
			limit = judged[k] ? 0x7fffffff : QDUP_AI_CAP;
			if(limit > QDUP_AI_PASSES - used) limit = QDUP_AI_PASSES - used;
			ai_trials++;
			if(trial(cand_angle[k], limit, &n) == TRUE) chosen = k;
			used += n;
			reset_shot();
			}
		}
	ai_passes = used;

// as NMS's search ends: count is the number of the angle taken, 60 when
// none potted; from the 45th on (and when none potted) the ghost-ball angle
	if(chosen >= 0)
		{
		count = chosen;
		cue_angle = cand_angle[chosen];
		}
	else
		{
		count = NUM_CANDIDATES;
		cue_angle = cand_angle[NUM_CANDIDATES-1];
		}
	if(count >= 45) cue_angle = cand_ghost;
	reset_shot();
}

#ifdef	QDUP_AI_COMPARE
// Test builds (tests/qdup-ai): both searches on every computer turn.  The
// game goes on with NMS's choice (QDUP_AI_PLAY_FAST: with this search's),
// so a scripted game reaches the same positions as with NMS's search alone.
// Printed per turn: the position, each search's angle, number, whether
// its shot pots under the full physics (and which balls), its time in
// VBlanks.
static long	play_out(int angle, long *potted)
{
	flag	r;
	int	i;

	for(i=0; i<LAST_BALL; i++) saveball[i] = ball[i];
	save_nine_ball_count = nine_ball_count;
	r = take_shot(angle);
	*potted = 0;
	for(i=1; i<LAST_BALL; i++)
		if((saveball[i].active == YES) && (ball[i].active != YES)) *potted |= 1 << i;
	reset_shot();
	return(r == TRUE);
}

void	get_best_shot_compare(void)
{
	long	f0, ex_t, fa_t, ex_angle, ex_count, fa_angle, fa_count, ex_pot, fa_pot, ex_mask, fa_mask;
	int	i;

	printf("AIPOS %d %d", (int)GAME_TYPE, (int)Player_Active);
	for(i=0; i<LAST_BALL; i++)
		printf(" %d,%d,%d", (int)ball[i].x, (int)ball[i].y, (int)ball[i].active);
	printf("\n");

	f0 = frames;
	get_best_shot_for_straight_pot();
	ex_t = frames - f0;
	ex_angle = cue_angle;
	ex_count = count;

	f0 = frames;
	get_best_shot_fast();
	fa_t = frames - f0;
	fa_angle = cue_angle;
	fa_count = count;

	ex_pot = play_out(ex_angle, &ex_mask);
	fa_pot = play_out(fa_angle, &fa_mask);
#ifdef	QDUP_AI_LABEL
	// every candidate played out, with the white's starting velocity
	{
	static	flag	once = NO;
	long	m;
	int	k;

	if(once == NO)
		{
		once = YES;
		printf("AIPOCK");
		for(k=0; k<NUM_POCKETS; k++)
			printf(" %d,%d,%d", (int)pockets[k].x, (int)pockets[k].y, (int)pockets[k].radius);
		printf(" %d %d\n", (int)ball[0].radius, (int)ball[1].radius);
		}
	for(i=0; i<LAST_BALL; i++) saveball[i] = ball[i];
	save_nine_ball_count = nine_ball_count;
	printf("AILAB %d", (int)cand_ghost);
	for(k=0; k<NUM_CANDIDATES; k++)
		{
		hit_ball(WHITE_BALL, x_hit_pos, y_hit_pos, cand_angle[k], butt, DOUBLE_TO_FIX24(speed_meter));
		printf(" %d,%d,%d", cand_angle[k], (int)ball[0].vx, (int)ball[0].vy);
		reset_shot();
		{
		int	passes, r;
		for(i=0; i<LAST_BALL; i++) saveball[i] = ball[i];
		r = trial(cand_angle[k], 0x7fffffff, &passes);
		m = 0;
		for(i=1; i<LAST_BALL; i++)
			if((saveball[i].active == YES) && (ball[i].active != YES)) m |= 1 << i;
		reset_shot();
		printf(",%d,%x,%d", r, (int)m, passes);
		}
		}
	printf("\n");
	}
#endif
	printf("AICMP ex %d %d %d %x %d fa %d %d %d %x %d judged %d trials %d passes %d work %d\n",
		(int)ex_count, (int)ex_angle, (int)ex_pot, (int)ex_mask, (int)ex_t,
		(int)fa_count, (int)fa_angle, (int)fa_pot, (int)fa_mask, (int)fa_t,
		ai_judged, ai_trials, ai_passes, (int)ai_work);
#ifdef	QDUP_AI_PLAY_FAST
	cue_angle = fa_angle;
	count = fa_count;
#else
	cue_angle = ex_angle;
	count = ex_count;
#endif
}
#endif

#ifdef	QDUP_AI_DUEL
// Test builds (tests/qdup-ai duel): the computer plays both sides
// (QDUP_TEST_CPU_BOTH), one side with NMS's search, the other with this
// one.  Which side, and a small turn of the opening shot (so that games
// differ), come from the VBlank count at the first turn, which the test
// varies through the time Start is pressed at the title.
void	get_best_shot_duel(void)
{
	static	int	fast_side = -1, offset = 0;
	int	i, fast;

	if(fast_side < 0)
		{
		fast_side = (frames / 9) & 1;
		offset = (frames % 9) - 4;
		}
	fast = (Player_Active == fast_side);
	printf("AIPOS %d %d", (int)GAME_TYPE, (int)Player_Active);
	for(i=0; i<LAST_BALL; i++)
		printf(" %d,%d,%d", (int)ball[i].x, (int)ball[i].y, (int)ball[i].active);
	printf("\n");
	if(fast) get_best_shot_fast();
	else get_best_shot_for_straight_pot();
	if(offset != 0)
		{
		cue_angle += offset;		// the opening shot only
		offset = 0;
		}
	printf("AIDUEL %d %d %d %d\n", (int)Player_Active, fast, (int)count, (int)cue_angle);
}
#endif
