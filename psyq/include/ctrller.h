/* PSoXide Psy-Q compatibility layer: the controller buffer InitPAD fills.
   The layout is the bytes a controller sends: a status byte (0xFF when
   nothing answered), the id byte (type in the high nibble, data half-words
   in the low one), then the data, buttons active low. */
#ifndef PSYQ_CTRLLER_H
#define PSYQ_CTRLLER_H
#include <sys/types.h>

#define MAX_CONTROLLER_BYTES 34

#define STD_PAD    4            /* digital pad, id 0x41 */
#define ANALOG_PAD 7            /* DualShock in analog mode, id 0x73 */

typedef struct {
	u_char transStatus;         /* 0x00 good, 0xFF no controller */
	u_char dataFormat;          /* type << 4 | data half-words */
	union {
		u_short pad;            /* buttons, active low, bits as padequ.h */
		u_char  bytes[MAX_CONTROLLER_BYTES - 2];
	} data;
} ControllerPacket;

#define GoodData(p) ((p)->transStatus != 0xFF)
#define GetType(p)  (((p)->dataFormat & 0xF0) >> 4)
#endif
