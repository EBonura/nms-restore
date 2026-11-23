/* PSoXide Psy-Q compatibility layer: controller button bits in the order the
   controller sends them (low byte first), active high once the game inverts
   the buffer. L = left hand (direction pad), R = right hand (shapes). The
   values match the masks in NMS's 1996 build of get_pad and getkey. */
#ifndef PSYQ_PADEQU_H
#define PSYQ_PADEQU_H
#define PAD_SEL   0x0001
#define PAD_START 0x0008
#define PAD_LU    0x0010
#define PAD_LR    0x0020
#define PAD_LD    0x0040
#define PAD_LL    0x0080
#define PAD_TL2   0x0100
#define PAD_TR2   0x0200
#define PAD_TL1   0x0400
#define PAD_TR1   0x0800
#define PAD_RU    0x1000        /* triangle */
#define PAD_RR    0x2000        /* circle */
#define PAD_RD    0x4000        /* cross */
#define PAD_RL    0x8000        /* square */
#endif
