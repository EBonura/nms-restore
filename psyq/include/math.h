/* PSoXide Psy-Q compatibility layer: double precision maths, in software
   (psyq/src/libm.rs; the arithmetic itself is the compiler's soft-float). */
#ifndef PSYQ_MATH_H
#define PSYQ_MATH_H
double sin(double x);
double cos(double x);
double tan(double x);
double atan(double x);
double atan2(double y, double x);
double asin(double x);
double acos(double x);
double sqrt(double x);
double hypot(double x, double y);
double fabs(double x);
double floor(double x);
double ceil(double x);
double fmod(double x, double y);
#endif
