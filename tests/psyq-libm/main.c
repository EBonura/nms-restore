/* Check the layer's double maths against the host's: prints the bits of
   each result; check.py runs it in the emulator and compares. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static const double in[] = {
	0.0, 1e-9, 0.1, 0.5, 0.7853981633974483, 1.0, 1.5707963267948966, 2.0, 3.0,
	3.141592653589793, 4.0, 6.283185307179586, 10.0, 100.0, 12345.678, -0.3, -2.5, -7.0,
	1.9198621771937625, 0.0174532925199433, 0.25, 0.75, 1.75, 0.999, 1e5
};

static void out(const char *f, int i, double x)
{
	unsigned long *w = (unsigned long *)&x;
	printf("%s %d %08x%08x\n", f, i, w[1], w[0]);
}

void main(void)
{
	int i, n = sizeof(in) / sizeof(in[0]);
	for (i = 0; i < n; i++) {
		double x = in[i];
		out("sin", i, sin(x));
		out("cos", i, cos(x));
		out("atan", i, atan(x));
		out("atan2", i, atan2(x, 0.7 - x));
		out("atan2n", i, atan2(-x, -0.3 - x));
		out("sqrt", i, sqrt(fabs(x)));
		out("hypot", i, hypot(x, 1.3 * x - 0.2));
		out("floor", i, floor(x));
	}
	out("atof", 0, atof("0.0525"));
	out("atof", 1, atof("-1.2345e-3"));
	out("atof", 2, atof("1002.5"));
	out("atof", 3, atof("3.1415927"));
	printf("LIBM DONE\n");
}
