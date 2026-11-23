//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// RESTORE: fixed-point helpers for FIXBALL.C (QD Up restoration, not NMS code)
//
// NMS had mul64 and get_moment in assembler in CODE.S, and slSin, slCos,
// slSquart and slMulFX in SGL.S (Saturn SGL names, table SIN_COS.TAB); the
// game and the physics test link SGL.S for slSin and slCos.  Here: the
// square root, divide and arctangent FIXBALL.C needs, which NMS's files do
// not have, plus get_moment for the physics test (which does not link
// CODE.S).  The arctangent table is made by tools/qdup_gen_tables.py.  Only
// 32-bit integer maths, apart from the 64-bit product of MIPS `mult` in
// mul64 and slMulFX.
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
#ifdef	FIXBALL_STANDALONE
#include	"fixball.h"
#else
#include	"globals.h"		// the game: includes fixball.h
#endif
#include	"fixtab.h"
// mul64 and slMulFX are inline functions in FIXBALL.H.
// RESTORE: for speed only, next to FIXBALL.C's step functions (see there)
long	fix_sqrt(long val) __attribute__((section(".text.hot.qdup")));
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// 2*mass*radius*radius/5, 8.24 (CODE.S get_moment; the game links CODE.S's)
#ifdef	FIXBALL_STANDALONE
long	get_moment(long mass, long radius)
{
	return((mul64(mul64(radius, radius), mass) / 5) << 1);
}
#endif
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// Square root of an 8.24 value, result 8.24: floor(sqrt(val * 2^24)),
// digit by digit over the 32 bits of val and 24 more zero bits, in 32-bit
// registers.  (slSquart alone gives a 4.12 result.)
long	fix_sqrt(long val)
{
	unsigned long	v = val;
	unsigned long	root = 0, rem = 0, test;
	long	i;

	if(val <= 0) return(0);
// a step while the top two bits are 0 and nothing is found yet changes
// nothing but v: skip those steps (val > 0, so at most 15)
	for(i = 0; (v & 0xc0000000) == 0; i++) v <<= 2;
	for(; i < 28; i++)
		{
		rem = (rem << 2) | (v >> 30);
		v <<= 2;
		root <<= 1;
		test = (root << 1) + 1;
		if(rem >= test)
			{
			rem -= test;
			root++;
			}
		}
	return(root);
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// a / b with 24 fraction bits, (a << 24) / b without a 64-bit divide.
// Truncates toward zero like a C double-to-long conversion; saturates to
// +-0x7fffffff when the quotient does not fit (or b is 0).
long	fixdiv(long a, long b)
{
	unsigned long	ua, ub, q, r;
	long	neg, i;

	neg = (a < 0) ^ (b < 0);
	ua = (a < 0) ? -(unsigned long)a : a;
	ub = (b < 0) ? -(unsigned long)b : b;
	if(ub == 0) return(neg ? -0x7fffffff : 0x7fffffff);

	q = ua / ub;
	if(q >= 0x80) return(neg ? -0x7fffffff : 0x7fffffff);
	r = ua - q * ub;
	for(i = 0; i < 24; i++)
		{
		q <<= 1;
		if(r >= 0x80000000UL || (r << 1) >= ub)
			{
			r = (r << 1) - ub;
			q |= 1;
			}
		else
			{
			r <<= 1;
			}
		}
	if(q > 0x7fffffff) q = 0x7fffffff;
	return(neg ? -(long)q : (long)q);
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// Angle of the vector (x,y) from the +x axis, like atan2(y, x), as an SGL
// angle in -32767..32768 (-180deg..180deg; +180 for y = 0, x < 0, as C's
// atan2(+0, x)).  fix_atan2(0, 0) is 0.
long	fix_atan2(long y, long x)
{
	unsigned long	ax, ay, num, den, ratio, i, f;
	long	ang;

	ax = (x < 0) ? -(unsigned long)x : x;
	ay = (y < 0) ? -(unsigned long)y : y;
	if((ax | ay) == 0) return(0);

	// ratio = min/max in 0..65536 (16 fraction bits)
	if(ay <= ax) { num = ay; den = ax; }
	else { num = ax; den = ay; }
	while(den >= 0x8000)
		{
		num >>= 1;
		den >>= 1;
		}
	if(den == 0) den = 1;
	ratio = (num << 16) / den;

	i = ratio >> 6;
	f = ratio & 63;
	ang = fix_atan_tab[i];
	if(f) ang += ((fix_atan_tab[i+1] - ang) * (long)f + 32) >> 6;
	ang = (ang + 8) >> 4;					// 1/16 units to SGL units, 0..8192

	if(ay > ax) ang = 0x4000 - ang;		// 45..90deg
	if(x < 0) ang = 0x8000 - ang;			// 90..180deg
	if(y < 0) ang = -ang;
	return(ang);
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// A double as 8.24 without floating-point code: the IEEE 754 bits decoded
// with integer shifts, truncated toward zero as a C conversion, saturated
// past +-128.  The 3D view: the 3D cue reads the shot code's double
// cue_position through this every frame (STV3D.C PositionCue).
long	dbl_to_fix24(double d)
{
	union { double d; unsigned long w[2]; } u;	// little-endian: w[1] high
	unsigned long	hi, lo, m;
	long	e, shift, v;

	u.d = d;
	lo = u.w[0];
	hi = u.w[1];
	e = (long)((hi >> 20) & 0x7ff) - 1023;
	if(e < -24) return(0);
	if(e >= 7) return((hi & 0x80000000UL) ? -0x7fffffff : 0x7fffffff);
	m = (hi & 0xfffff) | 0x100000;		// the top 21 of the 53 mantissa bits
	shift = 28 - e;						// value * 2^24 = mantissa >> shift
	if(shift >= 32) v = m >> (shift - 32);
	else v = (m << (32 - shift)) | (lo >> shift);
	return((hi & 0x80000000UL) ? -v : v);
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
// hypot of two 8.24 values, 8.24, for the game code that used the C
// library's hypot on ball positions.  Coordinates on a table are a few
// metres at most; at 7.5 m or more the squares would pass the 8.24 range,
// so such a pair returns 0x7fffffff (far apart).
long	fix_hypot(long x, long y)
{
	if((x >= FIX24(7.5)) || (x <= -FIX24(7.5)) || (y >= FIX24(7.5)) || (y <= -FIX24(7.5)))
		return(0x7fffffff);
	return(fix_sqrt(mul64(x,x) + mul64(y,y)));
}
//ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
