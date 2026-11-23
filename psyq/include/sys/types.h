/* PSoXide Psy-Q compatibility layer: basic unsigned types.
   Written for the NMS restoration from how the games use them. */
#ifndef PSYQ_SYS_TYPES_H
#define PSYQ_SYS_TYPES_H
typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   u_int;
typedef unsigned long  u_long;
/* QD Up uses TRUE and FALSE without defining them (its TYPES.H has them
   commented out): a system header supplied them. */
#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif
#ifndef NULL
#define NULL 0
#endif
#endif
