/* RESTORE: NEW WORK (owner's decision, 2026-11-18). Short control hints in play, in the debug font
 * at the top of the screen where NMS's status lines were (GFX.C DrawScreen2D; those lines are for
 * tool builds only, QDUP_DEBUG_TEXT). The owner: "I thought you would have placed that in the
 * debug text on screen when we play".
 *
 * The hints go with NMS's label above them (foul_occured: SHOOT MODE, AMPLIFY SHOOT MODE, AIM MODE,
 * the fouls; SCRDRWS.C foul_messages) and, inside it, with the button held now (event_flag, which
 * GAME.C sets back to ROTATE_CAMERA_VIEW every frame, 283, and from the held button, 358-416).
 * NMS's label stays SHOOT MODE after Cross is let go (GAME.C 416 sets it; only the shot's end, 685,
 * or the PC's A key, STV3D.C 3550-3557, which the pad does not have, set AIM MODE again), while
 * the buttons are back to aiming: under that label with Cross let go the hint says how to strike
 * from there. They come after the label, so they never cover it; none is shown while the balls
 * roll. hint_mode says which set was drawn (the gates read it beside foul_occured and event_flag).
 * Each line is at most HINT_MAX characters: the font cell is 8 x 8 (psyq font.rs), the text
 * starts at x 16 (POOL.C FntOpen), so 32 characters end at x 272, 48 pixels inside the 320-pixel
 * field; nothing wraps or reaches the edge. Three lines at most, rows 2 to 4 of the text (y 24 to 47),
 * above the table in the 3D view.
 *
 * Written from the code that reads the buttons:
 *   Left, Right aim (GAME.C 311-312);  Cross held is shoot mode (413, NMS's S key) and in it Down
 *   draws the cue back and Up drives it through (the push's speed is the power);  Square with Up,
 *   Down the cue's butt (395, B);  Circle held with the direction pad the spin (358, E);  Triangle,
 *   with the ball in hand, moves the white ball (379, M);  L2 undo, R2 replay (PAD.C, the owner's
 *   mapping of NMS's U and R keys);  L2 and R2 together the 2D or 3D view (GFX.C). */
#include	"globals.h"

#define	HINT_MAX	32

long	hint_frames = 0;	/* frames drawn with a hint (the gates read it) */
long	hint_mode = 0;		/* the set last drawn: 1 aim, 2 shoot (Cross held), 3 shoot label with Cross let go,
						   4 cue height, 5 spin, 6 ball in hand */

void	Print_Control_Hints( void )
{
	if((foul_occured == READY2_SHOOT_MODE) || (foul_occured == AMPLIFY_SHOOT_MODE))
		{
		if(event_flag == TAKING_SHOT)
			{
			FntPrint("DOWN THEN UP: STRIKE\n");
			FntPrint("LET GO OF CROSS: AIM AGAIN\n");
			hint_mode = 2;
			}
		else
			{
			FntPrint("HOLD CROSS, DOWN THEN UP: STRIKE\n");
			FntPrint("LEFT/RIGHT AIM L2 UNDO R2 REPLAY\n");
			hint_mode = 3;
			}
		hint_frames++;
		return;
		}
	switch(event_flag)
		{
		case	RAISE_BUTT_OF_CUE:
				FntPrint("SQUARE + UP DOWN: CUE HEIGHT\n");
				hint_mode = 4;
				break;
		case	ADD_MASSE:
				FntPrint("CIRCLE + DPAD: SPIN\n");
				hint_mode = 5;
				break;
		case	MOVING_BALL_IN_HAND:
				FntPrint("TRIANGLE + DPAD: MOVE THE BALL\n");
				hint_mode = 6;
				break;
		default:
				FntPrint("LEFT/RIGHT AIM  HOLD CROSS SHOOT\n");
				if(ball_in_hand_flag == YES) FntPrint("TRIANGLE MOVES THE BALL IN HAND\n");
				else FntPrint("SQUARE CUE HEIGHT  CIRCLE SPIN\n");
				FntPrint("L2 UNDO  R2 REPLAY  L2+R2 2D/3D\n");
				hint_mode = 1;
				break;
		}
	hint_frames++;
}
