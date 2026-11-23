
/**************************************
*            INCBIN HEADERS           *
**************************************/

#include "..\tempsnd\shock.h"

/**************************************
*          DECLARE VARIABLES          *
**************************************/

USHORT	Ping_Pong(USHORT num,USHORT limit);
LONG	BOUND(LONG,LONG);
BYTE	SIGN(LONG var);
SHORT 	GetSin(SHORT angle);
SHORT 	GetCos(SHORT angle);
LONG	random(ULONG size);
ULONG	fast_sqrt(ULONG num);

/**************************************
*     PING PONG A NUMBER ROUTINE      *
**************************************/

USHORT							/*limit is 1 more than half way.  i.e (frame MOD 8,4+1)=(frame BAND 7,5) */
Ping_Pong(USHORT num,USHORT limit)
{

	if (num<limit) return num;
	return -num+((limit-1)<<1);

}

/**************************************
*      CHECK FOR NUMBER BOUNDRY       *
**************************************/

LONG
BOUND(LONG var,LONG lim)
{

	if (var<0) return 0;
	if (var<lim) return var;
	return lim;

}

/**************************************
*       CHECK SIGN OF A NUMBER        *
**************************************/

BYTE
SIGN(LONG var)
{

	if (var<0) return -1;
	return +1;

}

/**************************************
*        GET SIN OF A CIRCLE          *
**************************************/

SHORT 
GetSin(SHORT angle)				/*0-1023  one quarter==256*/
{								/*return -32768 to 32768 */

SHORT	result;
UBYTE	quad;

	quad=angle>>8; /*Calculate quadrent*/
	switch(quad)
	{
		case 0: return sintable[angle]; 
		break;
		case 1: return sintable[511-angle];
		break;
		case 2: return -(sintable[angle-512]);
		break;
		case 3: return -(sintable[1023-angle]);
		break;
		case 4: return sintable[angle-1024];
		break;
   	}
	return result;

}

/**************************************
*        GET COS OF A CIRCLE          *
**************************************/

SHORT 
GetCos(SHORT angle)				/* 0-1023 */
{

	SHORT result; 
	result=GetSin(angle+256);
	return result;

}

/**************************************
*        GET A RANDOM NUMBER          *
**************************************/

LONG
random(ULONG size)
{

	return (rand() MOD size);

}

/**************************************
*    GET SQUARE ROOT OF A NUMBER      *
**************************************/

ULONG							/*Gives very fast sqrt function. fraction in lower 16 bits.*/
fast_sqrt(ULONG num)
{

ULONG 	root;
BYTE 	j=0;

	while(num>1024){num=num>>2;j++;}
	root=sqrtable[num];
	root=root<<j;
	return root;

}

